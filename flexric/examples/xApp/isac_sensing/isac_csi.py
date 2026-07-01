# SPDX-License-Identifier: MIT
"""Reader for the binary CSI log written by xapp_isac_moni.

File format (little endian):
  header : magic "ISACCSI1" (8 bytes), u32 version, u32 meta_size
  record : isac_csi_meta_t (meta_size bytes), i64 rx_tstamp_us,
           int16 iq[2 * nb_rx_ant * nb_ue_ports * nb_sc]

iq layout: [rx ant][ue port][subcarrier][re, im]
"""

import struct
from dataclasses import dataclass

import numpy as np

MAGIC = b"ISACCSI1"

# isac_csi_meta_t, natural x86-64 alignment (52 payload bytes + tail padding)
_META_FMT = "<qQIIIIHHHhHHHHBBBB"
_META_FIELDS = (
    "tstamp_us", "ul_carrier_freq_hz", "seq", "nb_sc", "first_sc", "scs_hz",
    "frame", "slot", "rnti", "wb_snr_db", "ta_offset", "nb_prb_carrier",
    "nb_prb_srs", "nb_symb_srs", "nb_rx_ant", "nb_ue_ports", "comb_size", "pad",
)


@dataclass
class Snapshot:
    meta: dict
    rx_tstamp_us: int
    h: np.ndarray  # complex64 [rx_ant, ue_port, subcarrier]


def read_csi_file(path):
    """Yield Snapshot objects from a xapp_isac_moni capture."""
    with open(path, "rb") as f:
        hdr = f.read(16)
        if len(hdr) < 16 or hdr[:8] != MAGIC:
            raise ValueError(f"{path}: not an ISACCSI1 file")
        version, meta_size = struct.unpack_from("<II", hdr, 8)
        if version != 1:
            raise ValueError(f"unsupported version {version}")
        base = struct.calcsize(_META_FMT)
        if meta_size < base:
            raise ValueError(f"meta_size {meta_size} < {base}")

        while True:
            raw = f.read(meta_size + 8)
            if len(raw) < meta_size + 8:
                return
            meta = dict(zip(_META_FIELDS, struct.unpack_from(_META_FMT, raw)))
            (rx_ts,) = struct.unpack_from("<q", raw, meta_size)

            n = 2 * meta["nb_rx_ant"] * meta["nb_ue_ports"] * meta["nb_sc"]
            iq = np.fromfile(f, dtype="<i2", count=n)
            if iq.size < n:
                return
            h = iq.astype(np.float32).view(np.complex64)
            h = h.reshape(meta["nb_rx_ant"], meta["nb_ue_ports"], meta["nb_sc"])
            yield Snapshot(meta=meta, rx_tstamp_us=rx_ts, h=h)


def load_tensor(path, rnti=None, ue_port=0):
    """Build the slow-time CSI tensor H[m, k, r] from a capture.

    Returns (H, info) where H is complex64 [M, N_sc, R] and info holds the
    physical constants needed by the sensing pipeline.
    """
    snaps = []
    for s in read_csi_file(path):
        if rnti is not None and s.meta["rnti"] != rnti:
            continue
        snaps.append(s)
    if not snaps:
        raise ValueError("no snapshots (wrong rnti?)")

    m0 = snaps[0].meta
    snaps = [s for s in snaps
             if s.meta["nb_sc"] == m0["nb_sc"] and s.meta["nb_rx_ant"] == m0["nb_rx_ant"]]

    # slow-time step from the 10.24 s-periodic (frame, slot) grid
    slots_per_frame = 10 * m0["scs_hz"] // 15000  # e.g. 20 at 30 kHz
    t_slot = 1e-2 / slots_per_frame               # frame = 10 ms
    idx = np.array([s.meta["frame"] * slots_per_frame + s.meta["slot"] for s in snaps],
                   dtype=np.int64)
    idx = np.unwrap(idx, period=1024 * slots_per_frame)
    d = np.diff(idx)
    ts = np.median(d[d > 0]) * t_slot if len(d) else t_slot

    H = np.stack([s.h[:, ue_port, :].T for s in snaps])  # [M, N_sc, R]

    info = {
        "M": H.shape[0],
        "N_sc": H.shape[1],
        "R": H.shape[2],
        "Ts": float(ts),
        "scs_hz": float(m0["scs_hz"]),
        "fc_hz": float(m0["ul_carrier_freq_hz"]),
        "lambda_m": 3e8 / float(m0["ul_carrier_freq_hz"]),
        "nb_prb_srs": m0["nb_prb_srs"],
        "seq": np.array([s.meta["seq"] for s in snaps]),
        "slot_idx": idx,
        "tstamp_us": np.array([s.meta["tstamp_us"] for s in snaps]),
    }
    gaps = int(np.sum(np.diff(info["seq"]) - 1))
    if gaps:
        print(f"WARNING: {gaps} snapshots missing (seq gaps) - Doppler axis may smear")
    return H, info
