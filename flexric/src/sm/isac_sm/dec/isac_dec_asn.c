/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */



#include "isac_dec_asn.h"

#include <assert.h>

//#include "../ie/asn/E2SM-ISACStats-EventTriggerDefinition.h"
//#include "../ie/asn/E2SM-ISACStats-EventTriggerDefinition-Format1.h"
//#include "../ie/asn/E2SM-ISACStats-IndicationMessage.h"



isac_event_trigger_t isac_dec_event_trigger_asn(size_t len, uint8_t const buf[len])
{

  assert(0!=0 && "Not implemented");

/*
  E2SM_ISACStats_EventTriggerDefinition_t ev_tr = {0};
  E2SM_ISACStats_EventTriggerDefinition_t* ev_tr_ptr = &ev_tr;

  const enum asn_transfer_syntax syntax = ATS_ALIGNED_BASIC_PER;
  const asn_dec_rval_t rval = asn_decode(NULL, syntax, &asn_DEF_E2SM_ISACStats_EventTriggerDefinition, (void **) &ev_tr_ptr, buf, len);
  assert(rval.code == RC_OK);

  ISACStats_TriggerNature_t val = ev_tr.choice.eventDefinition_Format1->triggerNature;
  isac_event_trigger_t ret = {0};
  if(val == ISACStats_TriggerNature_oneMs	){
    ret.ms = 1; 
  }else if( val == ISACStats_TriggerNature_twoMs	){
    ret.ms = 2; 
  } else if (val == ISACStats_TriggerNature_fiveMs	) {
    ret.ms = 5; 
  } else {
    assert(0!=0 && "Not implemented case");
  }

  // Despite its name, it frees the elements allocated by asn_decode
  ASN_STRUCT_RESET(asn_DEF_E2SM_ISACStats_EventTriggerDefinition, &ev_tr );

  return ret;
  */

}

isac_action_def_t isac_dec_action_def_asn(size_t len, uint8_t const action_def[len])
{

  assert(0!=0 && "Not implemented");
}

isac_ind_hdr_t isac_dec_ind_hdr_asn(size_t len, uint8_t const ind_hdr[len])
{
  assert(0!=0 && "Not implemented");

}

isac_ind_msg_t isac_dec_ind_msg_asn(size_t len, uint8_t const ind_msg[len])
{

  assert(0!=0 && "Not implemented");
  /*
  E2SM_ISACStats_IndicationMessage_t ind_msg_asn = {0};
  E2SM_ISACStats_IndicationMessage_t* ind_msg_ptr = &ind_msg_asn; 

  const enum asn_transfer_syntax syntax = ATS_ALIGNED_BASIC_PER;
  const asn_dec_rval_t rval = asn_decode(NULL, syntax, &asn_DEF_E2SM_ISACStats_IndicationMessage, (void **) &ind_msg_ptr, ind_msg, len);
  assert(rval.code == RC_OK);


 isac_ind_msg_t ret = {.tx_bytes = ind_msg_asn.txBytes, .tx_pkts = ind_msg_asn.txPkts};
  ASN_STRUCT_RESET(asn_DEF_E2SM_ISACStats_IndicationMessage, &ind_msg_asn);
  return ret;
  */
}

isac_call_proc_id_t isac_dec_call_proc_id_asn(size_t len, uint8_t const call_proc_id[len])
{
  assert(0!=0 && "Not implemented");

}

isac_ctrl_hdr_t isac_dec_ctrl_hdr_asn(size_t len, uint8_t const ctrl_hdr[len])
{
  assert(0!=0 && "Not implemented");

}

isac_ctrl_msg_t isac_dec_ctrl_msg_asn(size_t len, uint8_t const ctrl_msg[len])
{
  assert(0!=0 && "Not implemented");

}


isac_ctrl_out_t isac_dec_ctrl_out_asn(size_t len, uint8_t const ctrl_out[len]) 
{
  assert(0!=0 && "Not implemented");

}

isac_func_def_t isac_dec_func_def_asn(size_t len, uint8_t const func_def[len])
{
  assert(0!=0 && "Not implemented");

}

