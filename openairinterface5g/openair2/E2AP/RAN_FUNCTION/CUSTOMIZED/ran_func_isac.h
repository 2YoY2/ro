/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef SM_ISAC_READ_WRITE_AGENT_H
#define SM_ISAC_READ_WRITE_AGENT_H

#include "openair2/E2AP/flexric/src/agent/../sm/sm_io.h"

bool read_isac_sm(void*);

void read_isac_setup_sm(void*);

sm_ag_if_ans_t write_ctrl_isac_sm(void const*);

#endif
