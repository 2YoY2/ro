/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */



#include "isac_dec_plain.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

isac_event_trigger_t isac_dec_event_trigger_plain(size_t len, uint8_t const ev_tr[len])
{
  isac_event_trigger_t ev = {0};
  memcpy(&ev.ms, ev_tr, sizeof(ev.ms));
  return ev;
}

isac_action_def_t isac_dec_action_def_plain(size_t len, uint8_t const action_def[len])
{
  assert(0!=0 && "Not implemented");
  assert(action_def != NULL);
  isac_action_def_t act_def;// = {0};
  return act_def;
}

isac_ind_hdr_t isac_dec_ind_hdr_plain(size_t len, uint8_t const ind_hdr[len])
{
  assert(len == sizeof(isac_ind_hdr_t)); 
  isac_ind_hdr_t ret;
  memcpy(&ret, ind_hdr, len);
  return ret;
}

isac_ind_msg_t isac_dec_ind_msg_plain(size_t len, uint8_t const ind_msg[len])
{
  isac_ind_msg_t ret = {0};

  uint8_t const* ptr = ind_msg;
  uint8_t const* end = ind_msg + len;

  assert(len >= sizeof(ret.len_snap) + sizeof(ret.dropped_snap) + sizeof(ret.tstamp));

  memcpy(&ret.len_snap, ptr, sizeof(ret.len_snap));
  ptr += sizeof(ret.len_snap);

  memcpy(&ret.dropped_snap, ptr, sizeof(ret.dropped_snap));
  ptr += sizeof(ret.dropped_snap);

  memcpy(&ret.tstamp, ptr, sizeof(ret.tstamp));
  ptr += sizeof(ret.tstamp);

  if(ret.len_snap > 0){
    ret.snap = calloc(ret.len_snap, sizeof(isac_csi_snap_t));
    assert(ret.snap != NULL && "Memory exhausted!");
  }

  for(uint32_t i = 0; i < ret.len_snap; ++i){
    isac_csi_snap_t* s = &ret.snap[i];

    assert(ptr + sizeof(isac_csi_meta_t) <= end && "Truncated ISAC ind msg");
    memcpy(&s->meta, ptr, sizeof(isac_csi_meta_t));
    ptr += sizeof(isac_csi_meta_t);

    size_t const iq_bytes = isac_snap_iq_len(&s->meta) * sizeof(int16_t);
    if(iq_bytes > 0){
      assert(ptr + iq_bytes <= end && "Truncated ISAC ind msg");
      s->iq = malloc(iq_bytes);
      assert(s->iq != NULL && "Memory exhausted!");
      memcpy(s->iq, ptr, iq_bytes);
      ptr += iq_bytes;
    }
  }

  assert(ptr == end && "Data layout mismatch");

  return ret;
}

isac_call_proc_id_t isac_dec_call_proc_id_plain(size_t len, uint8_t const call_proc_id[len])
{
  assert(0!=0 && "Not implemented");
  assert(call_proc_id != NULL);
  isac_call_proc_id_t proc_id = {0};
  return proc_id;
}

isac_ctrl_hdr_t isac_dec_ctrl_hdr_plain(size_t len, uint8_t const ctrl_hdr[len])
{
  assert(len == sizeof(isac_ctrl_hdr_t)); 
  isac_ctrl_hdr_t ret;
  memcpy(&ret, ctrl_hdr, len);
  return ret;
}

isac_ctrl_msg_t isac_dec_ctrl_msg_plain(size_t len, uint8_t const ctrl_msg[len])
{
  assert(len == sizeof(isac_ctrl_msg_t)); 
  isac_ctrl_msg_t ret;
  memcpy(&ret, ctrl_msg, len);
  return ret;
}

isac_ctrl_out_t isac_dec_ctrl_out_plain(size_t len, uint8_t const ctrl_out[len]) 
{
  assert(0!=0 && "Not implemented");
  assert(ctrl_out != NULL);
  isac_ctrl_out_t out = {0};
  return out;
}

isac_func_def_t isac_dec_func_def_plain(size_t len, uint8_t const func_def[len])
{
  assert(0!=0 && "Not implemented");
  assert(func_def != NULL);
  isac_func_def_t def = {0};
  return def;
}


