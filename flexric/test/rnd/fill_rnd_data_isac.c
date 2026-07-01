/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "fill_rnd_data_isac.h"
#include "../../src/util/time_now_us.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

void fill_isac_ind_data(isac_ind_data_t* ind)
{
  assert(ind != NULL);
  srand(time(0));

  isac_ind_msg_t* msg = &ind->msg;

  msg->tstamp = time_now_us();
  msg->dropped_snap = abs(rand() % 8);
  msg->len_snap = 1 + abs(rand() % 3);

  msg->snap = calloc(msg->len_snap, sizeof(isac_csi_snap_t));
  assert(msg->snap != NULL && "memory exhausted");

  for(uint32_t i = 0; i < msg->len_snap; ++i){
    isac_csi_meta_t* m = &msg->snap[i].meta;

    m->tstamp_us = time_now_us();
    m->ul_carrier_freq_hz = 3700000000u;
    m->seq = i;
    m->nb_sc = 128 + abs(rand() % 128); // small tensor, enough to exercise the codec
    m->first_sc = abs(rand() % 12);
    m->scs_hz = 30000;
    m->frame = abs(rand() % 1024);
    m->slot = abs(rand() % 20);
    m->rnti = abs(rand() % 0xFFFE) + 1;
    m->wb_snr_db = (rand() % 60) - 10;
    m->ta_offset = abs(rand() % 64);
    m->nb_prb_carrier = 273;
    m->nb_prb_srs = 272;
    m->nb_symb_srs = 1;
    m->nb_rx_ant = 4;
    m->nb_ue_ports = 1;
    m->comb_size = 2;
    m->pad = 0;

    size_t const len = isac_snap_iq_len(m);
    msg->snap[i].iq = malloc(len * sizeof(int16_t));
    assert(msg->snap[i].iq != NULL && "memory exhausted");
    for(size_t k = 0; k < len; ++k){
      msg->snap[i].iq[k] = (int16_t)(rand() % 65536 - 32768);
    }
  }
}
