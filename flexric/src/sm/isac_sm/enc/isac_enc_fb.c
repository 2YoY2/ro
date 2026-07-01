/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */



#include "isac_enc_fb.h"

#include <assert.h>
#include <stdio.h>


//#include "../ie/fb/e2sm_isac_stats_v00_builder.h"
//#include "../ie/fb/e2sm_isac_stats_v00_verifier.h"



byte_array_t isac_enc_event_trigger_fb(isac_event_trigger_t const* event_trigger)
{
  assert(event_trigger != NULL);

  assert(0!=0 && "Not implemented");
  byte_array_t ba = {0};
  return ba;

/*
  flatcc_builder_t builder;
  flatcc_builder_init(&builder);

  E2SM_ISACStats_EventTrigger_start_as_root(&builder); 

  if(event_trigger->ms == 1){
    E2SM_ISACStats_EventTrigger_trig_add(&builder, E2SM_ISACStats_TriggerNature_oneMs );
  } else if(event_trigger->ms == 2){
    E2SM_ISACStats_EventTrigger_trig_add(&builder, E2SM_ISACStats_TriggerNature_twoMs );
  } else if(event_trigger->ms == 5){
    E2SM_ISACStats_EventTrigger_trig_add(&builder, E2SM_ISACStats_TriggerNature_fiveMs );
  } else {
    assert(0!=0 && "Not foreseen state");
  }


  E2SM_ISACStats_EventTrigger_end_as_root(&builder);


  size_t size = 0;
  uint8_t *buf = flatcc_builder_finalize_buffer(&builder, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if ((ret = E2SM_ISACStats_EventTrigger_verify_as_root(buf, size))) {
    printf("Event trigger is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(&builder);

  return ba;
  */
}


byte_array_t isac_enc_action_def_fb(isac_action_def_t const* action_def)
{
  assert(0!=0 && "Not implemented");

  assert(action_def != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t isac_enc_ind_hdr_fb(isac_ind_hdr_t const* ind_hdr)
{
  assert(0!=0 && "Not implemented");

  assert(ind_hdr != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t isac_enc_ind_msg_fb(isac_ind_msg_t const* ind_msg)
{
  assert(ind_msg != NULL);
  assert(0!=0 && "Not implemented");
  byte_array_t ba = {0};
  return ba;
/*
  flatcc_builder_t builder;
  flatcc_builder_init(&builder);

  E2SM_ISACStats_IndicationMessage_start_as_root(&builder); 
  E2SM_ISACStats_IndicationMessage_tx_bytes_add(&builder, ind_msg->tx_bytes); 
  E2SM_ISACStats_IndicationMessage_tx_pkts_add(&builder, ind_msg->tx_pkts); 

  E2SM_ISACStats_EventTrigger_end_as_root(&builder);

  size_t size = 0;
  uint8_t *buf = flatcc_builder_finalize_buffer(&builder, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if ((ret = E2SM_ISACStats_IndicationMessage_verify_as_root(buf, size))) {
    printf("Event trigger is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(&builder);

  return ba;
  */
}

byte_array_t isac_enc_call_proc_id_fb(isac_call_proc_id_t const* call_proc_id)
{
  assert(0!=0 && "Not implemented");

  assert(call_proc_id != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t isac_enc_ctrl_hdr_fb(isac_ctrl_hdr_t const* ctrl_hdr)
{
  assert(0!=0 && "Not implemented");

  assert(ctrl_hdr != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t isac_enc_ctrl_msg_fb(isac_ctrl_msg_t const* ctrl_msg)
{
  assert(0!=0 && "Not implemented");

  assert(ctrl_msg != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t isac_enc_ctrl_out_fb(isac_ctrl_out_t const* ctrl) 
{
  assert(0!=0 && "Not implemented");

  assert(ctrl != NULL );
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t isac_enc_func_def_fb(isac_func_def_t const* func)
{
  assert(0!=0 && "Not implemented");

  assert(func != NULL);
  byte_array_t  ba = {0};
  return ba;
}

