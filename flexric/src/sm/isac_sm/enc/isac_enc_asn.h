/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */



#ifndef ISAC_ENCRYPTIOIN_ASN_H
#define ISAC_ENCRYPTIOIN_ASN_H

#include "../../../util/byte_array.h"
#include "../ie/isac_data_ie.h"

// Used for static polymorphism. 
// See isac_enc_generic.h file
typedef struct{

} isac_enc_asn_t;

byte_array_t isac_enc_event_trigger_asn(isac_event_trigger_t const* event_trigger);

byte_array_t isac_enc_action_def_asn(isac_action_def_t const*);

byte_array_t isac_enc_ind_hdr_asn(isac_ind_hdr_t const*); 

byte_array_t isac_enc_ind_msg_asn(isac_ind_msg_t const*); 

byte_array_t isac_enc_call_proc_id_asn(isac_call_proc_id_t const*); 

byte_array_t isac_enc_ctrl_hdr_asn(isac_ctrl_hdr_t const*); 

byte_array_t isac_enc_ctrl_msg_asn(isac_ctrl_msg_t const*); 

byte_array_t isac_enc_ctrl_out_asn(isac_ctrl_out_t const*); 

byte_array_t isac_enc_func_def_asn(isac_func_def_t const*);

#endif

