/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */



#ifndef ISAC_ENCODING_FLATBUFFERS_H
#define ISAC_ENCODING_FLATBUFFERS_H

#include "../../../util/byte_array.h"
#include "../ie/isac_data_ie.h"


// Used for static polymorphism. 
// View isac_enc_generic file
typedef struct{

} isac_enc_fb_t;

byte_array_t isac_enc_event_trigger_fb(isac_event_trigger_t const* event_trigger);

byte_array_t isac_enc_action_def_fb(isac_action_def_t const*);

byte_array_t isac_enc_ind_hdr_fb(isac_ind_hdr_t const*); 

byte_array_t isac_enc_ind_msg_fb(isac_ind_msg_t const*); 

byte_array_t isac_enc_call_proc_id_fb(isac_call_proc_id_t const*); 

byte_array_t isac_enc_ctrl_hdr_fb(isac_ctrl_hdr_t const*); 

byte_array_t isac_enc_ctrl_msg_fb(isac_ctrl_msg_t const*); 

byte_array_t isac_enc_ctrl_out_fb(isac_ctrl_out_t const*); 

byte_array_t isac_enc_func_def_fb(isac_func_def_t const*);


#endif

