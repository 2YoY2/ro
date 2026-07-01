# SPDX-License-Identifier: MIT
"""Doppler-isolation sensing on CSI captured by the ISAC xApp.

Implements Algorithm 1 of "Doppler-Isolation Sensing from an OFDM Channel:
Range from 273 PRB, Angle from Four Antennas":

  1. MTI clutter cancellation      H~ <- H - mean_m(H)
  2. Doppler separation            slow-time DFT, peaks of P[nu]
  3. Range from the subcarriers    per-Doppler-bin IDFT over k -> tau -> R
  4. Angle from the rx antennas    DFT over the antenna axis -> theta

Usage:
  python3 isac_doppler_pipeline.py /tmp/isac_csi.bin [--rnti 0x1234] [--plot out.png]
"""

import argparse

import numpy as np

from isac_csi import load_tensor

C = 299_792_458.0


def doppler_spectrum(H, ts):
    """MTI + windowed slow-time DFT. Returns (Hd[nu,k,r], nu_axis_hz, P[nu])."""
    M = H.shape[0]
    Ht = H - H.mean(axis=0, keepdims=True)              # 1. MTI
    w = np.hanning(M)[:, None, None]
    Hd = np.fft.fft(Ht * w, axis=0)                     # 2. Doppler DFT
    Hd = np.fft.fftshift(Hd, axes=0)
    nu = np.fft.fftshift(np.fft.fftfreq(M, d=ts))
    P = np.sum(np.abs(Hd) ** 2, axis=(1, 2))
    return Hd, nu, P


def detect_movers(nu, P, thresh_db=10.0, guard_bins=2, max_movers=8):
    """Peaks of P[nu] above median + thresh_db, excluding the DC region.

    The Doppler axis is circular (movers near +-1/(2Ts) straddle the edge),
    so peak comparison and separation wrap around.
    """
    n = len(P)
    p_db = 10 * np.log10(P + 1e-12)
    noise = np.median(p_db)
    dc = int(np.argmin(np.abs(nu)))

    def circ_dist(i, j):
        return min(abs(i - j), n - abs(i - j))

    cand = []
    for i in range(n):
        if circ_dist(i, dc) <= guard_bins:
            continue
        if (p_db[i] >= p_db[(i - 1) % n] and p_db[i] >= p_db[(i + 1) % n]
                and p_db[i] > noise + thresh_db):
            cand.append((p_db[i], i))
    cand.sort(reverse=True)
    keep, used = [], set()
    for _, i in cand:
        if all(circ_dist(i, j) > guard_bins for j in used):
            keep.append(i)
            used.add(i)
        if len(keep) == max_movers:
            break
    return sorted(keep)


def range_profile(h_kr, scs_hz, oversample=8):
    """3. IDFT over subcarriers -> (profile_db, range_axis_m, R_est)."""
    N = h_kr.shape[0]
    w = np.hanning(N)[:, None]
    nfft = oversample * N
    prof = np.fft.ifft(h_kr * w, n=nfft, axis=0)        # peak at tau * N * scs
    p = np.sum(np.abs(prof) ** 2, axis=1)
    tau = np.arange(nfft) / (nfft * scs_hz)
    rng = C * tau / 2.0
    # keep the unambiguous first half
    half = nfft // 2
    p, rng = p[:half], rng[:half]
    i = int(np.argmax(p))
    return 10 * np.log10(p + 1e-12), rng, rng[i], i


def angle_spectrum(s_r, lam, d=None, grid=721):
    """4. DFT/steering scan over the antenna vector -> (spec_db, theta_deg, theta_est)."""
    R = s_r.shape[0]
    d = lam / 2 if d is None else d
    theta = np.linspace(-90, 90, grid)
    a = np.exp(-2j * np.pi * (d / lam) * np.outer(np.sin(np.deg2rad(theta)), np.arange(R)))
    spec = np.abs(a @ s_r) ** 2
    i = int(np.argmax(spec))
    return 10 * np.log10(spec + 1e-12), theta, theta[i]


def run(path, rnti=None, thresh_db=10.0, plot=None):
    H, info = load_tensor(path, rnti=rnti)
    M, N, R = info["M"], info["N_sc"], info["R"]
    ts, lam, scs = info["Ts"], info["lambda_m"], info["scs_hz"]
    bw = N * scs
    print(f"H[m,k,r] = [{M}, {N}, {R}]  Ts = {ts*1e3:.3f} ms  fc = {info['fc_hz']/1e9:.3f} GHz")
    print(f"range resolution  dR = {C/(2*bw):.2f} m   unambiguous {C/(2*scs)/1e3:.1f} km")
    print(f"velocity resolution dv = {lam/(2*M*ts):.3f} m/s   unambiguous +-{lam/(4*ts):.2f} m/s")

    Hd, nu, P = doppler_spectrum(H, ts)
    peaks = detect_movers(nu, P, thresh_db=thresh_db)
    if not peaks:
        print("no movers detected above threshold")
        return

    # v = nu * lambda / 2 is the closing speed: v > 0 approaching, v < 0 receding.
    # Range is the mid-dwell position (the object moves during the observation).
    print(f"\n{'#':>2} {'Doppler[Hz]':>12} {'v[m/s]':>8} {'range[m]':>9} {'angle[deg]':>10}")
    results = []
    for n, i in enumerate(peaks):
        h_i = Hd[i]                                     # isolate Doppler bin: H_i[k, r]
        prof_db, rng_ax, r_est, r_idx = range_profile(h_i, scs)
        # antenna vector at the detected range bin
        nfft = 8 * N
        w = np.hanning(N)[:, None]
        s = np.fft.ifft(h_i * w, n=nfft, axis=0)[r_idx]  # length-R vector
        spec_db, th_ax, th_est = angle_spectrum(s, lam)
        v = nu[i] * lam / 2.0
        results.append((nu[i], v, r_est, th_est, prof_db, rng_ax, spec_db, th_ax))
        print(f"{n:>2} {nu[i]:>12.1f} {v:>8.2f} {r_est:>9.2f} {th_est:>10.1f}")

    if plot:
        import matplotlib
        matplotlib.use("Agg")
        import matplotlib.pyplot as plt

        fig, ax = plt.subplots(1, 3, figsize=(15, 4))
        ax[0].plot(nu, 10 * np.log10(P + 1e-12))
        for r in results:
            ax[0].axvline(r[0], color="r", ls=":", alpha=0.6)
        ax[0].set(xlabel="Doppler [Hz]", ylabel="PSD [dB]", title="1) slow-time FFT")
        for n, r in enumerate(results):
            ax[1].plot(r[5], r[4] - r[4].max(), label=f"mover @ {r[0]:.0f} Hz")
            ax[2].plot(r[7], r[6] - r[6].max(), label=f"mover @ {r[0]:.0f} Hz")
        ax[1].set(xlabel="range [m]", ylabel="norm. power [dB]", xlim=(0, 60),
                  title="2) per-mover range profile")
        ax[2].set(xlabel="angle [deg]", ylabel="norm. power [dB]",
                  title="3) per-mover angle spectrum")
        ax[1].legend(); ax[2].legend()
        fig.tight_layout()
        fig.savefig(plot, dpi=140)
        print(f"\nplot saved to {plot}")


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("capture", help="binary CSI file from xapp_isac_moni")
    ap.add_argument("--rnti", type=lambda x: int(x, 0), default=None)
    ap.add_argument("--thresh-db", type=float, default=10.0)
    ap.add_argument("--plot", default=None, help="save figure to this path")
    args = ap.parse_args()
    run(args.capture, rnti=args.rnti, thresh_db=args.thresh_db, plot=args.plot)
