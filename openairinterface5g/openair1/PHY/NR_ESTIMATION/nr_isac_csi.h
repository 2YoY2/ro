/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

/* ISAC CSI tap: hands the SRS-based frequency-domain channel estimate
 * H[k, r] of every SRS occasion from the gNB PHY to the E2 agent (ISAC
 * service model), through a lock-protected ring buffer.
 *
 * Producer: handle_srs() in the UL PHY processing thread.
 * Consumer: read_isac_sm() in the E2 agent thread (RAN_FUNCTION/CUSTOMIZED).
 *
 * This header is deliberately free of PHY types so the E2 side can include it.
 */

#ifndef NR_ISAC_CSI_H
#define NR_ISAC_CSI_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
  int64_t  tstamp_us;          // wall clock when the SRS was processed
  uint64_t ul_carrier_freq_hz; // UL carrier center frequency
  uint32_t seq;                // filled by the ring, monotonic per gNB
  uint32_t nb_sc;              // subcarriers per (rx ant, ue port)
  uint32_t first_sc;           // first estimated subcarrier, relative to carrier edge
  uint32_t scs_hz;             // subcarrier spacing
  uint16_t frame;
  uint16_t slot;
  uint16_t rnti;
  int16_t  wb_snr_db;
  uint16_t ta_offset;
  uint16_t nb_prb_carrier;
  uint16_t nb_prb_srs;
  uint16_t nb_symb_srs;
  uint8_t  nb_rx_ant;
  uint8_t  nb_ue_ports;
  uint8_t  comb_size;
} nr_isac_csi_meta_t;

typedef struct {
  nr_isac_csi_meta_t meta;
  int16_t *iq; // malloc'ed, ownership goes to the caller of drain();
               // layout [rx ant][ue port][subcarrier][re, im]
} nr_isac_csi_snap_t;

/* Wall clock in microseconds (CLOCK_REALTIME), same base as the RIC/xApp. */
int64_t nr_isac_time_now_us(void);

/* Copy one CSI snapshot into the ring. meta->seq is set internally.
 * chan_base points to the (re, im) int16 pairs of channel estimates;
 * antenna a, port p, subcarrier k lives at complex index
 * a * stride_ant + p * stride_port + start_sc_off[p] + k. */
void nr_isac_csi_publish(const nr_isac_csi_meta_t *meta,
                         const int16_t *chan_base,
                         size_t stride_ant,
                         size_t stride_port,
                         const uint32_t *start_sc_off);

/* Move up to max_snap pending snapshots out of the ring, oldest first.
 * out[i].iq is malloc'ed and owned by the caller. Returns the number of
 * snapshots written; *dropped returns the cumulative overflow counter. */
size_t nr_isac_csi_drain(nr_isac_csi_snap_t *out, size_t max_snap, uint32_t *dropped);

#endif /* NR_ISAC_CSI_H */
