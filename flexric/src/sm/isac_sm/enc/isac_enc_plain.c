/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */



#include "isac_enc_plain.h"

#include <assert.h>
#include <stdlib.h>


byte_array_t isac_enc_event_trigger_plain(isac_event_trigger_t const* event_trigger)
{
  assert(event_trigger != NULL);
  byte_array_t  ba = {0};
 
  ba.len = sizeof(event_trigger->ms);
  ba.buf = malloc(ba.len);
  assert(ba.buf != NULL && "Memory exhausted");

  memcpy(ba.buf, &event_trigger->ms, ba.len);

  return ba;
}

byte_array_t isac_enc_action_def_plain(isac_action_def_t const* action_def)
{
  assert(0!=0 && "Not implemented");

  assert(action_def != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t isac_enc_ind_hdr_plain(isac_ind_hdr_t const* ind_hdr)
{
  assert(ind_hdr != NULL);

  byte_array_t ba = {0};

  ba.len = sizeof(isac_ind_hdr_t);
  ba.buf = calloc(ba.len,  sizeof(uint8_t));
  assert(ba.buf != NULL && "memory exhausted");
  memcpy(ba.buf, ind_hdr, ba.len);

  return ba;
}

// Wire layout: [len_snap][dropped_snap][tstamp] then per snapshot
// [isac_csi_meta_t][iq int16 payload of isac_snap_iq_len(meta)]
byte_array_t isac_enc_ind_msg_plain(isac_ind_msg_t const* ind_msg)
{
  assert(ind_msg != NULL);

  byte_array_t ba = {0};
  size_t len = sizeof(ind_msg->len_snap)
              + sizeof(ind_msg->dropped_snap)
              + sizeof(ind_msg->tstamp);
  for(uint32_t i = 0; i < ind_msg->len_snap; ++i){
    len += sizeof(isac_csi_meta_t);
    len += isac_snap_iq_len(&ind_msg->snap[i].meta) * sizeof(int16_t);
  }

  ba.buf = calloc(1, len);
  assert(ba.buf != NULL);

  void* ptr = ba.buf;
  memcpy(ptr, &ind_msg->len_snap, sizeof(ind_msg->len_snap));
  ptr += sizeof(ind_msg->len_snap);

  memcpy(ptr, &ind_msg->dropped_snap, sizeof(ind_msg->dropped_snap));
  ptr += sizeof(ind_msg->dropped_snap);

  memcpy(ptr, &ind_msg->tstamp, sizeof(ind_msg->tstamp));
  ptr += sizeof(ind_msg->tstamp);

  for(uint32_t i = 0; i < ind_msg->len_snap; ++i){
    isac_csi_snap_t const* s = &ind_msg->snap[i];
    memcpy(ptr, &s->meta, sizeof(isac_csi_meta_t));
    ptr += sizeof(isac_csi_meta_t);

    size_t const iq_bytes = isac_snap_iq_len(&s->meta) * sizeof(int16_t);
    if(iq_bytes > 0){
      assert(s->iq != NULL);
      memcpy(ptr, s->iq, iq_bytes);
      ptr += iq_bytes;
    }
  }

  assert(ptr == ba.buf + len && "Data layout mismatch");

  ba.len = len;
  return ba;
}


byte_array_t isac_enc_call_proc_id_plain(isac_call_proc_id_t const* call_proc_id)
{
  assert(0!=0 && "Not implemented");

  assert(call_proc_id != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t isac_enc_ctrl_hdr_plain(isac_ctrl_hdr_t const* ctrl_hdr)
{
  assert(ctrl_hdr != NULL);
  byte_array_t  ba = {0};
  ba.len = sizeof(isac_ctrl_hdr_t);
  ba.buf = calloc(ba.len ,sizeof(uint8_t)); 
  assert(ba.buf != NULL);

  memcpy(ba.buf, ctrl_hdr, ba.len);

  return ba;
}

byte_array_t isac_enc_ctrl_msg_plain(isac_ctrl_msg_t const* ctrl_msg)
{
  assert(ctrl_msg != NULL);

  byte_array_t  ba = {0};
  ba.len = sizeof(isac_ctrl_msg_t);
  ba.buf = calloc(ba.len, sizeof(uint8_t)); 
  assert(ba.buf != NULL);

  memcpy(ba.buf, ctrl_msg, ba.len);

  return ba;
}

byte_array_t isac_enc_ctrl_out_plain(isac_ctrl_out_t const* ctrl) 
{
  assert(0!=0 && "Not implemented");

  assert(ctrl != NULL );
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t isac_enc_func_def_plain(isac_func_def_t const* func)
{
  assert(0!=0 && "Not implemented");

  assert(func != NULL);
  byte_array_t  ba = {0};
  return ba;
}

