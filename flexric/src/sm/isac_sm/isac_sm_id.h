/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef ISAC_SERVICE_MODEL_ID_H
#define ISAC_SERVICE_MODEL_ID_H 

/*
 * Service Model ID needed for the agent as well as for the ric to ensure that they match. 
 */

#include <stdint.h>

static
const uint16_t SM_ISAC_ID = 150; 

__attribute__((unused)) static
const char* SM_ISAC_STR = "ISAC_STATS_V0"; 

static
const uint16_t SM_ISAC_REV = 1; 

__attribute__((unused)) static
const char SM_ISAC_SHORT_NAME[] = "E2SM-ISAC";

//iso(0) identified-organization(0)
//dod(0) internet(0) private(0)
//enterprise(0) 53148 e2(0)
// version1 (1) e2sm(150) e2sm-RC-
// IEs (0)

__attribute__((unused)) static
const char SM_ISAC_OID[] = "0.0.0.0.0.0.0.0.1.150.0"; 

__attribute__((unused)) static
const char SM_ISAC_DESCRIPTION[] = "ISAC CSI-over-E2 Service Model";

#endif

