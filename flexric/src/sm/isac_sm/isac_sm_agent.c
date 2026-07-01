/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "isac_sm_agent.h"

#include "../../util/alg_ds/alg/defer.h"
#include "dec/isac_dec_generic.h"
#include "isac_sm_id.h"
#include "enc/isac_enc_generic.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{

  sm_agent_t base;

#ifdef ASN
  isac_enc_asn_t enc;
#elif FLATBUFFERS 
  isac_enc_fb_t enc;
#elif PLAIN
  isac_enc_plain_t enc;
#else
  static_assert(false, "No encryptioin type selected");
#endif

} sm_isac_agent_t;


// Function pointers provided by the RAN for the 
// 5 procedures, 
// subscription, indication, control, 
// E2 Setup and RIC Service Update. 
//
static
sm_ag_if_ans_subs_t on_subscription_isac_sm_ag(sm_agent_t const* sm_agent, const sm_subs_data_t* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);

  sm_isac_agent_t* sm = (sm_isac_agent_t*)sm_agent;
 
  isac_event_trigger_t ev = isac_dec_event_trigger(&sm->enc, data->len_et, data->event_trigger);

  sm_ag_if_ans_subs_t ans = {.type = PERIODIC_SUBSCRIPTION_FLRC}; 
  ans.per.t.ms = ev.ms;
  return ans;
//  const sm_wr_if_t wr = {.type = SUBSCRIBE_TIMER, .sub_timer = timer };
//  sm->base.io.write(&wr);
//  printf("on_subscription called with event trigger = %u \n", ev.ms);
}

static
exp_ind_data_t on_indication_isac_sm_ag(sm_agent_t const* sm_agent, void* act_def)
{
  //printf("on_indication called \n");
  assert(sm_agent != NULL);
  assert(act_def == NULL && "Action definition data not needed for this SM");
  sm_isac_agent_t* sm = (sm_isac_agent_t*)sm_agent;

  exp_ind_data_t ret = {.has_value = true};

  // Fill Indication Header
  isac_ind_hdr_t hdr = {.dummy = 0 };
  byte_array_t ba_hdr = isac_enc_ind_hdr(&sm->enc, &hdr);
  ret.data.ind_hdr = ba_hdr.buf;
  ret.data.len_hdr = ba_hdr.len;

  // Fill Indication Message 
  //sm_ag_if_rd_t rd_if = {.type = INDICATION_MSG_AGENT_IF_ANS_V0};
  //rd_if.ind.type = ISAC_STATS_V0;

  isac_ind_data_t isac = {0};
 // Liberate the memory if previously allocated by the RAN. It sucks. Profoundly
  //  defer({ free_sm_rd_if(&rd_if); }; );
  defer({ free_isac_ind_hdr(&isac.hdr) ;});
  defer({ free_isac_ind_msg(&isac.msg) ;});
  defer({ free_isac_call_proc_id(isac.proc_id);});
 
  // This may allocate memory by the RAN
  if(sm->base.io.read_ind(&isac) == false)
    return (exp_ind_data_t){.has_value = false};

  byte_array_t ba = isac_enc_ind_msg(&sm->enc, &isac.msg);
  ret.data.ind_msg = ba.buf;
  ret.data.len_msg = ba.len;

  // Fill the optional Call Process ID
  ret.data.call_process_id = NULL;
  ret.data.len_cpid = 0;

  return ret;
}

static
sm_ctrl_out_data_t on_control_isac_sm_ag(sm_agent_t const* sm_agent, sm_ctrl_req_data_t const* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);
  sm_isac_agent_t* sm = (sm_isac_agent_t*) sm_agent;

  isac_ctrl_hdr_t hdr = isac_dec_ctrl_hdr(&sm->enc, data->len_hdr, data->ctrl_hdr);
  assert(hdr.dummy == 1 && "Only dummy == 1 supported ");

  isac_ctrl_msg_t msg = isac_dec_ctrl_msg(&sm->enc, data->len_msg, data->ctrl_msg);
  assert(msg.action == 42 && "Only action number 42 supported");

//  sm_ag_if_wr_t wr = {.type = CONTROL_SM_AG_IF_WR };
//  wr.ctrl.type = ISAC_CTRL_REQ_V0; 

  isac_ctrl_req_data_t isac_ctrl = {0};
  isac_ctrl.hdr.dummy = hdr.dummy;
  isac_ctrl.msg.action = msg.action;

  sm->base.io.write_ctrl(&isac_ctrl);
  sm_ctrl_out_data_t ret = {0};
  ret.len_out = 0;
  ret.ctrl_out = NULL;

  //printf("on_control called \n");
  return ret;
}

static
sm_e2_setup_data_t on_e2_setup_isac_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);
//  printf("[E2SM ISAC] on_e2_setup called \n");

  // Fill E2 Setup Request 
  // sm_ag_if_rd_t rd_if = {.type = E2_SETUP__AGENT_IF_ANS_V0};
  // rd_if.e2ap.type = ISAC_AGENT_IF_E2_SETUP_ANS_V0;

  // This may allocate memory by the RAN
  // sm->base.io.read(&rd_if);
  // Liberate the memory if previously allocated by the RAN. It sucks. Profoundly
  //isac_e2_setup_data_t* setup = &rd_if.e2ap.isac;

  sm_isac_agent_t* sm = (sm_isac_agent_t*)sm_agent;
  (void)sm;

  sm_e2_setup_data_t setup = {.len_rfd = 0, .ran_fun_def = NULL }; 

  size_t const sz = strnlen(SM_ISAC_STR, 256);
  assert(sz < 256 && "Buffer overeflow?");

  setup.len_rfd = sz;
  setup.ran_fun_def = calloc(1, sz);
  assert(setup.ran_fun_def != NULL);

  memcpy(setup.ran_fun_def, SM_ISAC_STR , sz);
 
  /*
  setup.len_rfd = strlen(sm->base.ran_func_name);
  setup.ran_fun_def = calloc(1, strlen(sm->base.ran_func_name));
  assert(setup.ran_fun_def != NULL);
  memcpy(setup.ran_fun_def, sm->base.ran_func_name, strlen(sm->base.ran_func_name));

  // RAN Function
  setup.rf.def = cp_str_to_ba(SM_ISAC_SHORT_NAME);
  setup.rf.id = SM_ISAC_ID;
  setup.rf.rev = SM_ISAC_REV;

  setup.rf.oid = calloc(1, sizeof(byte_array_t) );
  assert(setup.rf.oid != NULL && "Memory exhausted");

  *setup.rf.oid = cp_str_to_ba(SM_ISAC_OID);
*/

  return setup;
}

static
 sm_ric_service_update_data_t on_ric_service_update_isac_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);
  assert(0!=0 && "Not implemented");

  printf("on_ric_service_update called \n");
  sm_ric_service_update_data_t dst = {0}; 
  return dst;
}

static
void free_isac_sm_ag(sm_agent_t* sm_agent)
{
  assert(sm_agent != NULL);
  sm_isac_agent_t* sm = (sm_isac_agent_t*)sm_agent;
  free(sm);
}


// General SM information

// Definition
static
char const* def_isac_sm_ag(void)
{
  return SM_ISAC_STR;
}

// ID
static
uint16_t id_isac_sm_ag(void)
{
  return SM_ISAC_ID; 
}

  // Revision
static
uint16_t rev_isac_sm_ag (void)
{
  return SM_ISAC_REV;
}

// OID
static
char const* oid_isac_sm_ag (void)
{
  return SM_ISAC_OID;
}




sm_agent_t* make_isac_sm_agent(sm_io_ag_ran_t io)
{
  sm_isac_agent_t* sm = calloc(1, sizeof(sm_isac_agent_t));
  assert(sm != NULL && "Memory exhausted!!!");

  // Read
  sm->base.io.read_ind = io.read_ind_tbl[ISAC_STATS_V0];
  sm->base.io.read_setup = io.read_setup_tbl[ISAC_AGENT_IF_E2_SETUP_ANS_V0];
 
  //Write
  sm->base.io.write_ctrl = io.write_ctrl_tbl[ISAC_CTRL_REQ_V0];
  sm->base.io.write_subs = io.write_subs_tbl[ISAC_SUBS_V0];

  sm->base.free_sm = free_isac_sm_ag;
  sm->base.free_act_def = NULL; //free_act_def_isac_sm_ag;

  sm->base.proc.on_subscription = on_subscription_isac_sm_ag;
  sm->base.proc.on_indication = on_indication_isac_sm_ag;
  sm->base.proc.on_control = on_control_isac_sm_ag;
  sm->base.proc.on_ric_service_update = on_ric_service_update_isac_sm_ag;
  sm->base.proc.on_e2_setup = on_e2_setup_isac_sm_ag;
  sm->base.handle = NULL;

  // General SM information
  sm->base.info.def = def_isac_sm_ag;
  sm->base.info.id =  id_isac_sm_ag;
  sm->base.info.rev = rev_isac_sm_ag;
  sm->base.info.oid = oid_isac_sm_ag;



  //*(uint16_t*)(&sm->base.ran_func_id) = SM_ISAC_ID; 
  //assert(strlen( SM_ISAC_STR ) < sizeof(sm->base.ran_func_name));
  //memcpy(sm->base.ran_func_name, SM_ISAC_STR, strlen(SM_ISAC_STR));

  return &sm->base;
}

/*
uint16_t id_isac_sm_agent(sm_agent_t const* sm_agent )
{
  assert(sm_agent != NULL);
  sm_isac_agent_t* sm = (sm_isac_agent_t*)sm_agent;
  return sm->base.ran_func_id;
}
*/

