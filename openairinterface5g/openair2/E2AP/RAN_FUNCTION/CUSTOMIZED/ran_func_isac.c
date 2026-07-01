/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "ran_func_isac.h"

#include "openair1/PHY/NR_ESTIMATION/nr_isac_csi.h"
#include "openair2/E2AP/flexric/src/util/time_now_us.h"

#include <assert.h>
#include <stdlib.h>

// Upper bound of snapshots moved into one indication; the SM polling period
// (>= 1 ms) times the SRS rate stays well below this.
#define ISAC_MAX_DRAIN 16

static
void fill_snap(isac_csi_snap_t* dst, nr_isac_csi_snap_t const* src)
{
  isac_csi_meta_t* m = &dst->meta;
  nr_isac_csi_meta_t const* s = &src->meta;

  m->tstamp_us = s->tstamp_us;
  m->ul_carrier_freq_hz = s->ul_carrier_freq_hz;
  m->seq = s->seq;
  m->nb_sc = s->nb_sc;
  m->first_sc = s->first_sc;
  m->scs_hz = s->scs_hz;
  m->frame = s->frame;
  m->slot = s->slot;
  m->rnti = s->rnti;
  m->wb_snr_db = s->wb_snr_db;
  m->ta_offset = s->ta_offset;
  m->nb_prb_carrier = s->nb_prb_carrier;
  m->nb_prb_srs = s->nb_prb_srs;
  m->nb_symb_srs = s->nb_symb_srs;
  m->nb_rx_ant = s->nb_rx_ant;
  m->nb_ue_ports = s->nb_ue_ports;
  m->comb_size = s->comb_size;
  m->pad = 0;

  // ownership of the malloc'ed buffer moves to the SM, freed by free_isac_ind_msg
  dst->iq = src->iq;
}

bool read_isac_sm(void* data)
{
  assert(data != NULL);

  isac_ind_data_t* isac = (isac_ind_data_t*)data;
  isac_ind_msg_t* msg = &isac->msg;

  nr_isac_csi_snap_t tmp[ISAC_MAX_DRAIN];
  uint32_t dropped = 0;
  size_t const n = nr_isac_csi_drain(tmp, ISAC_MAX_DRAIN, &dropped);

  msg->tstamp = time_now_us();
  msg->dropped_snap = dropped;
  msg->len_snap = n;
  msg->snap = NULL;

  if(n == 0)
    return false; // nothing new: no indication this period

  msg->snap = calloc(n, sizeof(isac_csi_snap_t));
  assert(msg->snap != NULL && "Memory exhausted");

  for(size_t i = 0; i < n; ++i)
    fill_snap(&msg->snap[i], &tmp[i]);

  return true;
}

void read_isac_setup_sm(void* data)
{
  assert(data != NULL);
  assert(0 !=0 && "Not supported");
}

sm_ag_if_ans_t write_ctrl_isac_sm(void const* data)
{
  assert(data != NULL);
  printf("write_ctrl callback for ISAC SM: operation not supported\n");
  sm_ag_if_ans_t ans = {0};
  return ans;
}
