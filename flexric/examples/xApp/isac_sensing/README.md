# ISAC over E2: SRS-based CSI delivery for Doppler-isolation sensing

End-to-end pipeline that carries the gNB's SRS channel estimate `H[k, r]`
(273 PRB x 12 = up to 3276 subcarriers, 4 RX antennas, 3.7 GHz / 100 MHz)
over a custom **ISAC E2 service model** to an xApp, and localizes moving
objects with the paper's Doppler-isolation algorithm
(MTI -> slow-time DFT -> per-Doppler-bin range IDFT -> 4-antenna angle DFT).

Same architecture as BubbleRAN's ISAC pipeline: the SRS-triggered channel
estimate is captured in the gNB PHY, handed to the E2 agent, and shipped to
the xApp as raw int16 I/Q with full metadata.

## What was added

FlexRIC (this repo, also used by OAI through the `openair2/E2AP/flexric`
symlink so agent and RIC share one codec):

| piece | path |
|---|---|
| ISAC SM (ID **150**, name `ISAC_STATS_V0`) | `src/sm/isac_sm/` |
| IEs: CSI snapshot = metadata + variable I/Q | `src/sm/isac_sm/ie/isac_data_ie.h` |
| agent-interface registration | `src/sm/agent_if/{read,write,ans}/…` |
| CSI-logging xApp | `examples/xApp/c/monitor/xapp_isac_moni.c` |
| capture reader + sensing pipeline | `examples/xApp/isac_sensing/` |
| SM unit test | `test/sm/isac_sm/` (`ctest -R ISAC`) |

OAI:

| piece | path |
|---|---|
| PHY->E2 CSI ring buffer (32 deep, seq numbers, drop counter) | `openair1/PHY/NR_ESTIMATION/nr_isac_csi.{c,h}` |
| publish hook after SRS channel estimation | `handle_srs()` in `openair1/SCHED_NR/phy_procedures_nr_gNB.c` |
| E2 agent read callback | `openair2/E2AP/RAN_FUNCTION/CUSTOMIZED/ran_func_isac.{c,h}` |
| registration | `openair2/E2AP/RAN_FUNCTION/init_ran_func.c` |

## Data model

One **snapshot** per SRS occasion (slow-time index m):
`isac_csi_meta_t` (frame/slot/rnti/seq, nb_sc, first_sc, scs_hz,
ul_carrier_freq_hz, nb_rx_ant, SNR, TA, timestamps) + int16 I/Q laid out
`[rx ant][ue port][subcarrier][re, im]`. With 4 RX and 272 PRB sounded this
is ~52 kB per snapshot.

Reliability: the PHY never blocks (ring overwrite + `dropped_snap` counter);
the xApp polls every 5 ms and each indication *drains all pending snapshots*,
so nothing is lost as long as the ring (32) is not outrun; gNB-side `seq`
lets the xApp detect any gap end-to-end.

## Build

```bash
# FlexRIC (RIC + xApps + SM plugins)
cd flexric && mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && make -j$(nproc)
sudo make install        # REQUIRED: refreshes /usr/local/lib/flexric/*.so
                         # (a stale libisac_sm.so there will otherwise be loaded)

# OAI gNB with E2 agent (openair2/E2AP/flexric is a symlink to ../flexric)
cd openairinterface5g/cmake_targets
./build_oai --gNB --build-e2 --ninja
```

## gNB configuration (273 PRB / 4 RX / 3.7 GHz, band n78)

In the `gNBs` section of your conf:

```
do_SRS = "periodic";          # gNB schedules periodic SRS per UE
```

and make sure the RU/antenna settings expose 4 RX (`nb_rx = 4`,
`maxMIMO_layers` etc. per your usual 4-antenna conf). The SRS periodicity is
picked automatically from the TDD pattern (smallest of sl4/sl5/sl8/sl10/...
with a full UL slot); the Python reader measures the actual Ts from the
frame/slot stamps, so nothing else must be configured.

At the end of the conf:

```
e2_agent = {
  near_ric_ip_addr = "127.0.0.1";
  sm_dir = "/usr/local/lib/flexric/";
};
```

## Run order

```bash
# 1. RIC
./flexric/build/examples/ric/nearRT-RIC

# 2. gNB (plus your core; RFsim works for functional tests)
sudo ./openairinterface5g/cmake_targets/ran_build/build/nr-softmodem -O <conf> --sa

# 3. UE attaches (SRS only starts once a UE is connected)

# 4. ISAC xApp - writes /tmp/isac_csi.bin (override with ISAC_CSI_FILE=...)
./flexric/build/examples/xApp/c/monitor/xapp_isac_moni
# prints per-snapshot seq/rnti/SFN/SNR/latency and warns on any loss

# 5. Sensing (after capturing a dwell, e.g. a few seconds)
python3 flexric/examples/xApp/isac_sensing/isac_doppler_pipeline.py \
        /tmp/isac_csi.bin --plot /tmp/isac.png
```

Example output (validated on a synthetic 3-mover capture, M=400, Ts=5 ms):

```
range resolution  dR = 1.53 m    velocity resolution dv = 0.020 m/s (+-4.05 m/s)
 #  Doppler[Hz]   v[m/s]  range[m] angle[deg]
 0       -100.0    -4.05     16.07       31.0
 1        -62.5    -2.53     14.54       10.0
 2        -25.0    -1.01      8.99      -27.0
```

## Conventions and caveats

- `v = nu * lambda / 2` is the closing speed (positive = approaching);
  range is the mid-dwell position since the object moves during the dwell.
- Movers at exactly +-1/(2Ts) straddle the Doppler Nyquist edge (mover 0
  above); shorten Ts (denser SRS) to widen the unambiguous span.
- Real captures include UE clock drift / residual CFO: it shifts the whole
  Doppler axis and smears very long dwells. MTI removes the static part;
  keep dwells to a few seconds or estimate the common phase from the LOS bin.
- SRS sounds `m_SRS <= 272` PRB (C_SRS table), i.e. 3264 subcarriers of the
  273-PRB carrier - range resolution 1.53 m as in the paper.
- The estimate is interpolated over the comb (K_TC), so all `nb_sc`
  subcarriers are filled; `comb_size` is in the metadata if you prefer to
  decimate back.
