/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */



#include "../../../../test/common/fill_ind_data.h"
#include "../../isac_sm/isac_sm_agent.h"
#include "../../isac_sm/isac_sm_ric.h"

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

void read_RAN(sm_ag_if_rd_t* read)
{
  assert(read != NULL);
  assert(read->type == ISAC_STATS_V0);

  fill_isac_ind_data(&read->isac_stats);
  cp.hdr = cp_isac_ind_hdr(&read->isac_stats.hdr);
  cp.msg = cp_isac_ind_msg(&read->isac_stats.msg);
}

static 
sm_ag_if_ans_t write_RAN(const sm_ag_if_wr_t* data)
{
  assert(data != NULL);
  assert(0!=0 && "Not implemented");
  sm_ag_if_ans_t ans = {0};
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
  assert(ag->ran_func_id == ric->ran_func_id);
}

// RIC -> E2
static
void check_subscription(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);
 
  sm_subs_data_t data = ric->proc.on_subscription(ric, "2_ms");
  ag->proc.on_subscription(ag, &data); 

  free_sm_subs_data(&data);
}

// E2 -> RIC
static
void check_indication(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);

  sm_ind_data_t sm_data = ag->proc.on_indication(ag);
  sm_ag_if_rd_t msg = ric->proc.on_indication(ric, &sm_data);

  assert(msg.type == ISAC_STATS_V0);
  isac_ind_data_t* data = &msg.isac_stats;

  assert(eq_isac_ind_hdr(&data->hdr, &cp.hdr) == true);
  assert(eq_isac_ind_msg(&data->msg, &cp.msg) == true);
  assert(eq_isac_call_proc_id(data->proc_id, cp.proc_id) == true);

  free_isac_ind_hdr(&data->hdr);
  free_isac_ind_msg(&data->msg);

  free_sm_ind_data(&sm_data); 
}

int main()
{
  sm_io_ag_t io_ag = {.read = read_RAN, .write = write_RAN};  
  sm_agent_t* sm_ag = make_isac_sm_agent(io_ag);
  sm_ric_t* sm_ric = make_isac_sm_ric();

  check_eq_ran_function(sm_ag, sm_ric);
  check_subscription(sm_ag, sm_ric);
  check_indication(sm_ag, sm_ric);

  sm_ag->free_sm(sm_ag);
  sm_ric->free_sm(sm_ric);

  printf("Success\n");
  return EXIT_SUCCESS;
}

