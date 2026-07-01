/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */


#include "isac_data_ie.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

//////////////////////////////////////
// RIC Event Trigger Definition
/////////////////////////////////////

void free_isac_event_trigger(isac_event_trigger_t* src)
{
  assert(src != NULL);
  (void)src;
}

isac_event_trigger_t cp_isac_event_trigger( isac_event_trigger_t const* src)
{
  assert(src != NULL);
  isac_event_trigger_t et = {.ms = src->ms };
  return et;
}

bool eq_isac_event_trigger(isac_event_trigger_t const* m0, isac_event_trigger_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);
  return m0->ms == m1->ms;
}

//////////////////////////////////////
// RIC Action Definition
/////////////////////////////////////

void free_isac_action_def(isac_action_def_t* src)
{
  assert(src != NULL);
  (void)src;
}

isac_action_def_t cp_isac_action_def(isac_action_def_t* src)
{
  assert(src != NULL);
  isac_action_def_t ad = {.dummy = src->dummy};
  return ad;
}

bool eq_isac_action_def(isac_event_trigger_t* m0,  isac_event_trigger_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);
  return m0->ms == m1->ms;
}

//////////////////////////////////////
// RIC Indication Header
/////////////////////////////////////

void free_isac_ind_hdr(isac_ind_hdr_t* src)
{
  assert(src != NULL);
  (void)src;
}

isac_ind_hdr_t cp_isac_ind_hdr(isac_ind_hdr_t const* src)
{
  assert(src != NULL);
  isac_ind_hdr_t dst = {.dummy = src->dummy};
  return dst;
}

bool eq_isac_ind_hdr(isac_ind_hdr_t* m0, isac_ind_hdr_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);
  return m0->dummy == m1->dummy;
}

//////////////////////////////////////
// RIC Indication Message
/////////////////////////////////////

size_t isac_snap_iq_len(isac_csi_meta_t const* m)
{
  assert(m != NULL);
  return 2u * m->nb_rx_ant * m->nb_ue_ports * m->nb_sc;
}

isac_csi_snap_t cp_isac_csi_snap(isac_csi_snap_t const* src)
{
  assert(src != NULL);

  isac_csi_snap_t dst = {.meta = src->meta, .iq = NULL};

  size_t const len = isac_snap_iq_len(&src->meta);
  if(len > 0){
    assert(src->iq != NULL);
    dst.iq = malloc(len * sizeof(int16_t));
    assert(dst.iq != NULL && "Memory exhausted");
    memcpy(dst.iq, src->iq, len * sizeof(int16_t));
  }

  return dst;
}

bool eq_isac_csi_snap(isac_csi_snap_t const* m0, isac_csi_snap_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(memcmp(&m0->meta, &m1->meta, sizeof(isac_csi_meta_t)) != 0)
    return false;

  size_t const len = isac_snap_iq_len(&m0->meta);
  if(len == 0)
    return true;

  return memcmp(m0->iq, m1->iq, len * sizeof(int16_t)) == 0;
}

void free_isac_ind_msg(isac_ind_msg_t* src)
{
  assert(src != NULL);

  for(uint32_t i = 0; i < src->len_snap; ++i){
    if(src->snap[i].iq != NULL)
      free(src->snap[i].iq);
  }

  if(src->snap != NULL){
    assert(src->len_snap > 0);
    free(src->snap);
  }
}

isac_ind_msg_t cp_isac_ind_msg(isac_ind_msg_t const* src)
{
  assert(src != NULL);

  isac_ind_msg_t cp = {
    .len_snap = src->len_snap,
    .snap = NULL,
    .dropped_snap = src->dropped_snap,
    .tstamp = src->tstamp,
  };

  if(cp.len_snap > 0){
    cp.snap = calloc(cp.len_snap, sizeof(isac_csi_snap_t));
    assert(cp.snap != NULL && "Memory exhausted");
    for(uint32_t i = 0; i < cp.len_snap; ++i){
      cp.snap[i] = cp_isac_csi_snap(&src->snap[i]);
    }
  }

  return cp;
}

bool eq_isac_ind_msg(isac_ind_msg_t* m0, isac_ind_msg_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->len_snap != m1->len_snap
      || m0->dropped_snap != m1->dropped_snap
      || m0->tstamp != m1->tstamp)
    return false;

  for(uint32_t i = 0; i < m0->len_snap; ++i){
    if(eq_isac_csi_snap(&m0->snap[i], &m1->snap[i]) == false)
      return false;
  }

  return true;
}

//////////////////////////////////////
// RIC Call Process ID
/////////////////////////////////////

void free_isac_call_proc_id( isac_call_proc_id_t* src)
{
  assert(src != NULL);
  (void)src;
}

isac_call_proc_id_t cp_isac_call_proc_id( isac_call_proc_id_t* src)
{
  assert(src != NULL);
  isac_call_proc_id_t dst = {.dummy = src->dummy};
  return dst;
}

bool eq_isac_call_proc_id(isac_call_proc_id_t* m0, isac_call_proc_id_t* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  return m0->dummy == m1->dummy;
}

//////////////////////////////////////
// RIC Control Header
/////////////////////////////////////

void free_isac_ctrl_hdr( isac_ctrl_hdr_t* src)
{
  assert(src != NULL);
  (void)src;
}

isac_ctrl_hdr_t cp_isac_ctrl_hdr(isac_ctrl_hdr_t* src)
{
  assert(src != NULL);
  isac_ctrl_hdr_t dst = {.dummy = src->dummy};
  return dst;
}

bool eq_isac_ctrl_hdr(isac_ctrl_hdr_t* m0, isac_ctrl_hdr_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);
  return m0->dummy == m1->dummy;
}

//////////////////////////////////////
// RIC Control Message
/////////////////////////////////////

void free_isac_ctrl_msg( isac_ctrl_msg_t* src)
{
  assert(src != NULL);
  (void)src;
}

isac_ctrl_msg_t cp_isac_ctrl_msg(isac_ctrl_msg_t* src)
{
  assert(src != NULL);
  isac_ctrl_msg_t dst = {.action = src->action};
  return dst;
}

bool eq_isac_ctrl_msg(isac_ctrl_msg_t* m0, isac_ctrl_msg_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);
  return m0->action == m1->action;
}

//////////////////////////////////////
// RIC Control Outcome
/////////////////////////////////////

void free_isac_ctrl_out(isac_ctrl_out_t* src)
{
  assert(src != NULL);
  (void)src;
}

isac_ctrl_out_t cp_isac_ctrl_out(isac_ctrl_out_t* src)
{
  assert(src != NULL);
  isac_ctrl_out_t dst = {.ans = src->ans};
  return dst;
}

bool eq_isac_ctrl_out(isac_ctrl_out_t* m0, isac_ctrl_out_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);
  return m0->ans == m1->ans;
}

//////////////////////////////////////
// RAN Function Definition
/////////////////////////////////////

void free_isac_func_def(isac_func_def_t* src)
{
  assert(src != NULL);
  if(src->buf != NULL)
    free(src->buf);
}

isac_func_def_t cp_isac_func_def(isac_func_def_t const* src)
{
  assert(src != NULL);

  isac_func_def_t dst = {.len = src->len, .buf = NULL};
  if(dst.len > 0){
    dst.buf = malloc(dst.len);
    assert(dst.buf != NULL && "Memory exhausted");
    memcpy(dst.buf, src->buf, dst.len);
  }

  return dst;
}

bool eq_isac_func_def(isac_func_def_t const* m0, isac_func_def_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->len != m1->len)
    return false;

  return memcmp(m0->buf, m1->buf, m0->len) == 0;
}

///////////////
// RIC Indication
///////////////

isac_ind_data_t cp_isac_ind_data( isac_ind_data_t const* src)
{
  assert(src != NULL);

  isac_ind_data_t dst = {0};
  dst.hdr = cp_isac_ind_hdr(&src->hdr);
  dst.msg = cp_isac_ind_msg(&src->msg);

  if(src->proc_id != NULL){
    dst.proc_id = malloc(sizeof(isac_call_proc_id_t));
    assert(dst.proc_id != NULL && "Memory exhausted");
    *dst.proc_id = cp_isac_call_proc_id(src->proc_id);
  }

  return dst;
}

void free_isac_ind_data(isac_ind_data_t* ind)
{
  assert(ind != NULL);

  free_isac_ind_hdr(&ind->hdr);
  free_isac_ind_msg(&ind->msg);
  if(ind->proc_id != NULL){
    free_isac_call_proc_id(ind->proc_id);
    free(ind->proc_id);
  }
}
