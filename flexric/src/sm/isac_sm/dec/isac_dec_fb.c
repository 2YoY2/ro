/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */



#include "isac_dec_fb.h"

//#include "../ie/fb/e2sm_isac_stats_v00_builder.h"
//#include "../ie/fb/e2sm_isac_stats_v00_verifier.h"

#include <assert.h>
#include <stdio.h>


isac_event_trigger_t isac_dec_event_trigger_fb(size_t len, uint8_t const ev_tr[len])
{
  assert(len > 0);

  assert(0!=0 && "Not implemented");

/*
  assert(E2SM_ISACStats_EventTrigger_verify_as_root(ev_tr, len) == 0 && "Invalid event trigger");

//  int rc = E2SM_ISACStats_EventTrigger_verify_as_root(ev_tr, len);
//  printf("EventTrigger is invalid: %s\n", flatcc_verify_error_string(rc));

  E2SM_ISACStats_EventTrigger_table_t ev_tr_fb = E2SM_ISACStats_EventTrigger_as_root(ev_tr);
  assert(ev_tr_fb != NULL);

  const uint8_t val = E2SM_ISACStats_EventTrigger_trig(ev_tr_fb);

  isac_event_trigger_t  ret = {0};
  if(val == E2SM_ISACStats_TriggerNature_oneMs){
    ret.ms = 1;
  } else if(val == E2SM_ISACStats_TriggerNature_twoMs){
    ret.ms = 2;
  } else if(val == E2SM_ISACStats_TriggerNature_fiveMs){
    ret.ms = 5;
  } else {
    assert(0!=0 && "Not foreseen case");
  }
  return ret;
  */

}

isac_action_def_t isac_dec_action_def_fb(size_t len, uint8_t const action_def[len])
{

  assert(0!=0 && "Not implemented");
}

isac_ind_hdr_t isac_dec_ind_hdr_fb(size_t len, uint8_t const ind_hdr[len])
{

  assert(0!=0 && "Not implemented");
}

isac_ind_msg_t isac_dec_ind_msg_fb(size_t len, uint8_t const ind_msg[len])
{
  assert(len > 0);

  assert(0!=0 && "Not implemented");

  /*
  assert(E2SM_ISACStats_IndicationMessage_verify_as_root(ind_msg, len) == 0 && "Invalid event trigger");

//  int rc = E2SM_ISACStats_EventTrigger_verify_as_root(ev_tr, len);
//  printf("EventTrigger is invalid: %s\n", flatcc_verify_error_string(rc));


  E2SM_ISACStats_IndicationMessage_table_t ind_msg_fb = E2SM_ISACStats_IndicationMessage_as_root(ind_msg);
  assert(ind_msg_fb != NULL);

  isac_ind_msg_t ret = {0};
  ret.tx_bytes = E2SM_ISACStats_IndicationMessage_tx_bytes(ind_msg_fb); 
  ret.tx_pkts = E2SM_ISACStats_IndicationMessage_tx_pkts(ind_msg_fb); 
  return ret;
  */
}

isac_call_proc_id_t isac_dec_call_proc_id_fb(size_t len, uint8_t const call_proc_id[len])
{

  assert(0!=0 && "Not implemented");
}

isac_ctrl_hdr_t isac_dec_ctrl_hdr_fb(size_t len, uint8_t const ctrl_hdr[len])
{

  assert(0!=0 && "Not implemented");
}

isac_ctrl_msg_t isac_dec_ctrl_msg_fb(size_t len, uint8_t const ctrl_msg[len])
{

  assert(0!=0 && "Not implemented");
}


isac_ctrl_out_t isac_dec_ctrl_out_fb(size_t len, uint8_t const ctrl_out[len]) 
{

  assert(0!=0 && "Not implemented");
}

isac_func_def_t isac_dec_func_def_fb(size_t len, uint8_t const func_def[len])
{
  assert(0!=0 && "Not implemented");
}


