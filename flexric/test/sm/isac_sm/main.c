/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */


#include "../../rnd/fill_rnd_data_isac.h"
#include "../../../src/sm/isac_sm/isac_sm_agent.h"
#include "../../../src/sm/isac_sm/isac_sm_ric.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static
isac_ind_data_t cp;

/////
// AGENT
////

static
bool read_ind_isac(void* read)
{
  assert(read != NULL);
//  assert(read->type == INDICATION_MSG_AGENT_IF_ANS_V0);
//  assert(read->ind.type == ISAC_STATS_V0);

  isac_ind_data_t* ind = (isac_ind_data_t*)read; // &read->ind.isac;

  fill_isac_ind_data(ind);
  cp.hdr = cp_isac_ind_hdr(&ind->hdr);
  cp.msg = cp_isac_ind_msg(&ind->msg);
  return true;
}


static 
sm_ag_if_ans_t write_ctrl(void const* data)
{
  assert(data != NULL);

  isac_ctrl_req_data_t* ctrl = (isac_ctrl_req_data_t*)data; 
  assert(ctrl->hdr.dummy == 1);
  assert(ctrl->msg.action == 42);

  sm_ag_if_ans_t ans = {.type = CTRL_OUTCOME_SM_AG_IF_ANS_V0 };
  return ans;
}


/////////////////////////////
// Check Functions
// //////////////////////////

static
void check_eq_ran_function(sm_agent_t const* ag, sm_ric_t const* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);
  assert(ag->info.id() == ric->ran_func_id);
}

// RIC -> E2
static
void check_subscription(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);

  char sub[] = "2_ms";
  sm_subs_data_t data = ric->proc.on_subscription(ric, &sub);

  sm_ag_if_ans_subs_t const subs = ag->proc.on_subscription(ag, &data); 
  assert(subs.type == PERIODIC_SUBSCRIPTION_FLRC);
  assert(subs.per.t.ms == 2);

  free_sm_subs_data(&data);
}

// E2 -> RIC
static
void check_indication(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);

  exp_ind_data_t exp = ag->proc.on_indication(ag, NULL);
  assert(exp.has_value == true);
  sm_ag_if_rd_ind_t msg = ric->proc.on_indication(ric, &exp.data);

  assert(msg.type == ISAC_STATS_V0);
  isac_ind_data_t* data = &msg.isac;

  assert(eq_isac_ind_hdr(&data->hdr, &cp.hdr) == true);
  assert(eq_isac_ind_msg(&data->msg, &cp.msg) == true);
  assert(eq_isac_call_proc_id(data->proc_id, cp.proc_id) == true);

  free_isac_ind_hdr(&data->hdr);
  free_isac_ind_msg(&data->msg);

  free_exp_ind_data(&exp); 
}


// RIC -> E2
static
void check_ctrl(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);

  isac_ctrl_req_data_t ctrl = {.hdr.dummy = 1, .msg.action = 42}; 
  sm_ctrl_req_data_t msg = ric->proc.on_control_req(ric, &ctrl);
  
  sm_ctrl_out_data_t out = ag->proc.on_control(ag, &msg);
  assert(out.len_out == 0 && out.ctrl_out == NULL );

  free_sm_ctrl_req_data(&msg);
}

int main()
{
  sm_io_ag_ran_t io_ag = {0};
  io_ag.read_ind_tbl[ISAC_STATS_V0] = read_ind_isac; 
  io_ag.write_ctrl_tbl[ISAC_CTRL_REQ_V0] = write_ctrl; 

  sm_agent_t* sm_ag = make_isac_sm_agent(io_ag);
  sm_ric_t* sm_ric = make_isac_sm_ric();

  check_eq_ran_function(sm_ag, sm_ric);
  check_subscription(sm_ag, sm_ric);
  check_indication(sm_ag, sm_ric);
  check_ctrl(sm_ag, sm_ric);

  sm_ag->free_sm(sm_ag);
  sm_ric->free_sm(sm_ric);

  printf("Success\n");
  return EXIT_SUCCESS;
}

