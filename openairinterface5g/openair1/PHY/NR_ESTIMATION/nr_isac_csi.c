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

#include "nr_isac_csi.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Ring depth: at 30 kHz SCS the fastest SRS periodicity OAI schedules is a few
// slots; the E2 agent drains every few ms, so 32 entries give ample slack.
#define NR_ISAC_CSI_RING_SZ 32

int64_t nr_isac_time_now_us(void)
{
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return (int64_t)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

typedef struct {
  nr_isac_csi_meta_t meta;
  int16_t *iq;
  size_t iq_cap; // allocated int16 elements
} csi_entry_t;

static struct {
  pthread_mutex_t mtx;
  csi_entry_t e[NR_ISAC_CSI_RING_SZ];
  uint64_t wr; // total published
  uint64_t rd; // total drained
  uint32_t dropped;
} ring = {.mtx = PTHREAD_MUTEX_INITIALIZER};

void nr_isac_csi_publish(const nr_isac_csi_meta_t *meta,
                         const int16_t *chan_base,
                         size_t stride_ant,
                         size_t stride_port,
                         const uint32_t *start_sc_off)
{
  size_t const iq_len = 2ul * meta->nb_rx_ant * meta->nb_ue_ports * meta->nb_sc;
  if (iq_len == 0)
    return;

  pthread_mutex_lock(&ring.mtx);

  if (ring.wr - ring.rd == NR_ISAC_CSI_RING_SZ) {
    ring.rd++; // overwrite the oldest unread snapshot
    ring.dropped++;
  }

  csi_entry_t *e = &ring.e[ring.wr % NR_ISAC_CSI_RING_SZ];
  if (e->iq_cap < iq_len) {
    free(e->iq);
    e->iq = malloc(iq_len * sizeof(int16_t));
    if (e->iq == NULL) {
      e->iq_cap = 0;
      pthread_mutex_unlock(&ring.mtx);
      return;
    }
    e->iq_cap = iq_len;
  }

  e->meta = *meta;
  e->meta.seq = (uint32_t)ring.wr;

  int16_t *dst = e->iq;
  for (int a = 0; a < meta->nb_rx_ant; a++) {
    for (int p = 0; p < meta->nb_ue_ports; p++) {
      const int16_t *src = chan_base + 2ul * (a * stride_ant + p * stride_port + start_sc_off[p]);
      memcpy(dst, src, 2ul * meta->nb_sc * sizeof(int16_t));
      dst += 2ul * meta->nb_sc;
    }
  }

  ring.wr++;
  pthread_mutex_unlock(&ring.mtx);
}

size_t nr_isac_csi_drain(nr_isac_csi_snap_t *out, size_t max_snap, uint32_t *dropped)
{
  size_t n = 0;

  pthread_mutex_lock(&ring.mtx);

  while (ring.rd < ring.wr && n < max_snap) {
    csi_entry_t const *e = &ring.e[ring.rd % NR_ISAC_CSI_RING_SZ];
    size_t const iq_len = 2ul * e->meta.nb_rx_ant * e->meta.nb_ue_ports * e->meta.nb_sc;

    int16_t *iq = malloc(iq_len * sizeof(int16_t));
    if (iq == NULL)
      break;
    memcpy(iq, e->iq, iq_len * sizeof(int16_t));

    out[n].meta = e->meta;
    out[n].iq = iq;
    n++;
    ring.rd++;
  }

  if (dropped != NULL)
    *dropped = ring.dropped;

  pthread_mutex_unlock(&ring.mtx);
  return n;
}
