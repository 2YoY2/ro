/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */



#include "isac_sm_ric.h"
#include "isac_sm_id.h"

#include <assert.h>
#include <stdlib.h>

#include "enc/isac_enc_generic.h"
#include "dec/isac_dec_generic.h"

typedef struct{
  sm_ric_t base;

#ifdef ASN
  isac_enc_asn_t enc;
#elif FLATBUFFERS 
  isac_enc_fb_t enc;
#elif PLAIN
  isac_enc_plain_t enc;
#else
  static_assert(false, "No encryption type selected");
#endif
} sm_isac_ric_t;


static
sm_subs_data_t on_subscription_isac_sm_ric(sm_ric_t const* sm_ric, void* cmd)
{
  assert(sm_ric != NULL); 
  assert(cmd != NULL); 

  sm_isac_ric_t* sm = (sm_isac_ric_t*)sm_ric;  
 
  isac_sub_data_t isac = {0}; 

  const int max_str_sz = 10;
  if(strncmp(cmd, "1_ms", max_str_sz) == 0 ){
    isac.et.ms = 1;
  } else if (strncmp(cmd, "2_ms", max_str_sz) == 0 ) {
    isac.et.ms = 2;
  } else if (strncmp(cmd, "5_ms", max_str_sz) == 0 ) {
    isac.et.ms = 5;
  } else if (strncmp(cmd, "10_ms", max_str_sz) == 0 ) {
    isac.et.ms = 10;
  } else if (strncmp(cmd, "20_ms", max_str_sz) == 0 ) {
    isac.et.ms = 20;
  } else if (strncmp(cmd, "50_ms", max_str_sz) == 0 ) {
    isac.et.ms = 50;
  } else if (strncmp(cmd, "100_ms", max_str_sz) == 0 ) {
    isac.et.ms = 100;
  } else {
    assert(0 != 0 && "Invalid input");
  }
  
  // Event trigger
  const byte_array_t ba = isac_enc_event_trigger(&sm->enc, &isac.et); 

  sm_subs_data_t data = {0}; 
  
  // Event trigger IE
  data.event_trigger = ba.buf;
  data.len_et = ba.len;

  // Action Definition IE
  data.action_def = NULL;
  data.len_ad = 0;

  return data;
}

static
sm_ag_if_rd_ind_t on_indication_isac_sm_ric(sm_ric_t const* sm_ric, sm_ind_data_t const* data)
{
  assert(sm_ric != NULL); 
  assert(data != NULL); 
  sm_isac_ric_t* sm = (sm_isac_ric_t*)sm_ric;  

  sm_ag_if_rd_ind_t rd_if = {.type =  ISAC_STATS_V0};

  // Header
  rd_if.isac.hdr = isac_dec_ind_hdr(&sm->enc, data->len_hdr, data->ind_hdr);

  // Message
  rd_if.isac.msg = isac_dec_ind_msg(&sm->enc, data->len_msg, data->ind_msg);

  //  call_process_id
  assert(data->call_process_id == NULL && "not implemented"); 
  rd_if.isac.proc_id = NULL;

  return rd_if;
}

static
sm_ctrl_req_data_t ric_on_control_req_isac_sm_ric(sm_ric_t const* sm_ric, void* ctrl)
{
  assert(sm_ric != NULL); 
  assert(ctrl != NULL); 
  isac_ctrl_req_data_t const* req = (isac_ctrl_req_data_t const*)ctrl;
  assert(req->hdr.dummy == 1);
  assert(req->msg.action == 42);

  sm_isac_ric_t* sm = (sm_isac_ric_t*)sm_ric;  

  byte_array_t ba = isac_enc_ctrl_hdr(&sm->enc, &req->hdr);
  
  sm_ctrl_req_data_t ret_data = {0};  
  ret_data.ctrl_hdr = ba.buf;
  ret_data.len_hdr = ba.len;

  ba = isac_enc_ctrl_msg(&sm->enc, &req->msg);
  ret_data.ctrl_msg = ba.buf;
  ret_data.len_msg = ba.len;

  return ret_data;
}

static
sm_ag_if_ans_ctrl_t ric_on_control_out_isac_sm_ric(sm_ric_t const* sm_ric,const sm_ctrl_out_data_t * out)
{
  assert(sm_ric != NULL); 
  assert(out != NULL);

  sm_isac_ric_t* sm = (sm_isac_ric_t*)sm_ric;  

  sm_ag_if_ans_ctrl_t ag_if = {.type =  ISAC_AGENT_IF_CTRL_ANS_V0};  
  ag_if.isac = isac_dec_ctrl_out(&sm->enc, out->len_out, out->ctrl_out);
  assert(ag_if.isac.ans ==  ISAC_CTRL_OUT_OK);

  return ag_if;
}

static
sm_ag_if_rd_e2setup_t ric_on_e2_setup_isac_sm_ric(sm_ric_t const* sm_ric, sm_e2_setup_data_t const* setup)
{
  assert(sm_ric != NULL); 
  assert(setup != NULL); 

  sm_ag_if_rd_e2setup_t dst = {.type = ISAC_AGENT_IF_E2_SETUP_ANS_V0 }; 
  
  dst.isac.func_def.len = setup->len_rfd;
  if(dst.isac.func_def.len > 0){
    dst.isac.func_def.buf = calloc(dst.isac.func_def.len, sizeof(uint8_t));
    assert(dst.isac.func_def.buf != NULL && "Memory exhausted");
    memcpy(dst.isac.func_def.buf, setup->ran_fun_def, setup->len_rfd);
  }

  return dst;
}


static
sm_ag_if_rd_rsu_t on_ric_service_update_isac_sm_ric(sm_ric_t const* sm_ric, sm_ric_service_update_data_t const* rsu)
{
  assert(sm_ric != NULL); 
  assert(  rsu != NULL); 
  //  sm_isac_ric_t* sm = (sm_isac_ric_t*)sm_ric;  

  assert(0!=0 && "Not implemented");
  sm_ag_if_rd_rsu_t dst = {0};
  return dst;
}

static
void free_isac_sm_ric(sm_ric_t* sm_ric)
{
  assert(sm_ric != NULL);
  sm_isac_ric_t* sm = (sm_isac_ric_t*)sm_ric;
  free(sm);
}


//
// Allocation SM functions. The memory malloc by the SM is also freed by it.
//

static
void free_subs_data_isac_sm_ric(void* msg)
{
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");
}

static
void free_ind_data_isac_sm_ric(void* msg)
{
  assert(msg != NULL);

  sm_ag_if_rd_ind_t* rd_ind  = (sm_ag_if_rd_ind_t*)msg;
  assert(rd_ind->type == ISAC_STATS_V0);

  isac_ind_data_t* ind = &rd_ind->isac;

  free_isac_ind_hdr(&ind->hdr); 
  free_isac_ind_msg(&ind->msg); 
  assert(ind->proc_id == NULL && "Not implemented");
  if(ind->proc_id != NULL){
    free_isac_call_proc_id(ind->proc_id);
  }
}

static
void free_ctrl_req_data_isac_sm_ric(void* msg)
{
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");
}


static
void free_ctrl_out_data_isac_sm_ric(void* msg)
{
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");
}

static
void free_e2_setup_isac_sm_ric(void* msg)
{
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");
}

static
void free_ric_service_update_isac_sm_ric(void* msg)
{
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");
}


sm_ric_t* make_isac_sm_ric(void /* sm_io_ric_t io */)
{
  sm_isac_ric_t* sm = calloc(1, sizeof(sm_isac_ric_t));
  assert(sm != NULL && "Memory exhausted");

  *((uint16_t*)&sm->base.ran_func_id) = SM_ISAC_ID; 

  sm->base.free_sm = free_isac_sm_ric;

  // Memory (De)Allocation
  sm->base.alloc.free_subs_data_msg = free_subs_data_isac_sm_ric; 
  sm->base.alloc.free_ind_data = free_ind_data_isac_sm_ric ; 
  sm->base.alloc.free_ctrl_req_data = free_ctrl_req_data_isac_sm_ric; 
  sm->base.alloc.free_ctrl_out_data = free_ctrl_out_data_isac_sm_ric; 
  sm->base.alloc.free_e2_setup = free_e2_setup_isac_sm_ric; 
  sm->base.alloc.free_ric_service_update = free_ric_service_update_isac_sm_ric; 


  // O-RAN E2SM 5 Procedures
  sm->base.proc.on_subscription = on_subscription_isac_sm_ric; 
  sm->base.proc.on_indication = on_indication_isac_sm_ric;

  // Control needs two calls
  sm->base.proc.on_control_req = ric_on_control_req_isac_sm_ric;
  sm->base.proc.on_control_out = ric_on_control_out_isac_sm_ric;

  sm->base.proc.on_e2_setup = ric_on_e2_setup_isac_sm_ric;
  sm->base.proc.on_ric_service_update = on_ric_service_update_isac_sm_ric; 
  sm->base.handle = NULL;

  assert(strlen(SM_ISAC_STR) < sizeof(sm->base.ran_func_name));
  memcpy(sm->base.ran_func_name, SM_ISAC_STR, strlen(SM_ISAC_STR));

  return &sm->base;
}


uint16_t id_sm_isac_ric(sm_ric_t const* sm_ric)
{
  assert(sm_ric != NULL);
  sm_isac_ric_t* sm = (sm_isac_ric_t*)sm_ric;
  return sm->base.ran_func_id;
}

