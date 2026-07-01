/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */



#ifndef ISAC_DECODING_GENERIC
#define ISAC_DECODING_GENERIC 

#include "isac_dec_asn.h"
#include "isac_dec_fb.h"
#include "isac_dec_plain.h"

/////////////////////////////////////////////////////////////////////
// 9 Information Elements that are interpreted by the SM according
// to ORAN-WG3.E2SM-v01.00.00 Technical Specification
/////////////////////////////////////////////////////////////////////


#define isac_dec_event_trigger(T,U,V) _Generic ((T), \
                           isac_enc_plain_t*: isac_dec_event_trigger_plain, \
                           isac_enc_asn_t*: isac_dec_event_trigger_asn,\
                           isac_enc_fb_t*: isac_dec_event_trigger_fb,\
                           default: isac_dec_event_trigger_plain) (U,V)

#define isac_dec_action_def(T,U,V) _Generic ((T), \
                           isac_enc_plain_t*: isac_dec_action_def_plain, \
                           isac_enc_asn_t*: isac_dec_action_def_asn, \
                           isac_enc_fb_t*: isac_dec_action_def_fb, \
                           default:  isac_dec_action_def_plain) (U,V)

#define isac_dec_ind_hdr(T,U,V) _Generic ((T), \
                           isac_enc_plain_t*: isac_dec_ind_hdr_plain , \
                           isac_enc_asn_t*: isac_dec_ind_hdr_asn, \
                           isac_enc_fb_t*: isac_dec_ind_hdr_fb, \
                           default:  isac_dec_ind_hdr_plain) (U,V)

#define isac_dec_ind_msg(T,U,V) _Generic ((T), \
                           isac_enc_plain_t*: isac_dec_ind_msg_plain , \
                           isac_enc_asn_t*: isac_dec_ind_msg_asn, \
                           isac_enc_fb_t*: isac_dec_ind_msg_fb, \
                           default:  isac_dec_ind_msg_plain) (U,V)

#define isac_dec_call_proc_id(T,U,V) _Generic ((T), \
                           isac_enc_plain_t*: isac_dec_call_proc_id_plain , \
                           isac_enc_asn_t*: isac_dec_call_proc_id_asn, \
                           isac_enc_fb_t*: isac_dec_call_proc_id_fb, \
                           default:  isac_dec_call_proc_id_plain) (U,V)

#define isac_dec_ctrl_hdr(T,U,V) _Generic ((T), \
                           isac_enc_plain_t*: isac_dec_ctrl_hdr_plain , \
                           isac_enc_asn_t*: isac_dec_ctrl_hdr_asn, \
                           isac_enc_fb_t*: isac_dec_ctrl_hdr_fb, \
                           default: isac_dec_ctrl_hdr_plain) (U,V)

#define isac_dec_ctrl_msg(T,U,V) _Generic ((T), \
                           isac_enc_plain_t*: isac_dec_ctrl_msg_plain , \
                           isac_enc_asn_t*: isac_dec_ctrl_msg_asn, \
                           isac_enc_fb_t*: isac_dec_ctrl_msg_fb, \
                           default:  isac_dec_ctrl_msg_plain) (U,V)

#define isac_dec_ctrl_out(T,U,V) _Generic ((T), \
                           isac_enc_plain_t*: isac_dec_ctrl_out_plain , \
                           isac_enc_asn_t*: isac_dec_ctrl_out_asn, \
                           isac_enc_fb_t*: isac_dec_ctrl_out_fb, \
                           default:  isac_dec_ctrl_out_plain) (U,V)

#define isac_dec_func_def(T,U,V) _Generic ((T), \
                           isac_enc_plain_t*: isac_dec_func_def_plain, \
                           isac_enc_asn_t*: isac_dec_func_def_asn, \
                           isac_enc_fb_t*:  isac_dec_func_def_fb, \
                           default:  isac_dec_func_def_plain) (U,V)

#endif

