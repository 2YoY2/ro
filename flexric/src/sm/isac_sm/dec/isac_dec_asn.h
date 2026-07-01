/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */


#ifndef ISAC_DECODING_ASN_H
#define ISAC_DECODING_ASN_H

#include <stddef.h>
#include "../ie/isac_data_ie.h"


isac_event_trigger_t isac_dec_event_trigger_asn(size_t len, uint8_t const ev_tr[len]);

isac_action_def_t isac_dec_action_def_asn(size_t len, uint8_t const action_def[len]);

isac_ind_hdr_t isac_dec_ind_hdr_asn(size_t len, uint8_t const ind_hdr[len]); 

isac_ind_msg_t isac_dec_ind_msg_asn(size_t len, uint8_t const ind_msg[len]); 

isac_call_proc_id_t isac_dec_call_proc_id_asn(size_t len, uint8_t const call_proc_id[len]);

isac_ctrl_hdr_t isac_dec_ctrl_hdr_asn(size_t len, uint8_t const ctrl_hdr[len]); 

isac_ctrl_msg_t isac_dec_ctrl_msg_asn(size_t len, uint8_t const ctrl_msg[len]); 

isac_ctrl_out_t isac_dec_ctrl_out_asn(size_t len, uint8_t const ctrl_out[len]); 

isac_func_def_t isac_dec_func_def_asn(size_t len, uint8_t const func_def[len]);

#endif

