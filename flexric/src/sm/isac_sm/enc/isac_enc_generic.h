/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */



#ifndef ISAC_ENCODING_GENERIC
#define ISAC_ENCODING_GENERIC 

#include "isac_enc_asn.h"
#include "isac_enc_fb.h"
#include "isac_enc_plain.h"

/////////////////////////////////////////////////////////////////////
// 9 Information Elements that are interpreted by the SM according
// to ORAN-WG3.E2SM-v01.00.00 Technical Specification
/////////////////////////////////////////////////////////////////////


#define isac_enc_event_trigger(T,U) _Generic ((T), \
                           isac_enc_plain_t*: isac_enc_event_trigger_plain, \
                           isac_enc_asn_t*: isac_enc_event_trigger_asn,\
                           isac_enc_fb_t*: isac_enc_event_trigger_fb,\
                           default: isac_enc_event_trigger_plain) (U)

#define isac_enc_action_def(T,U) _Generic ((T), \
                           isac_enc_plain_t*: isac_enc_action_def_plain, \
                           isac_enc_asn_t*: isac_enc_action_def_asn, \
                           isac_enc_fb_t*: isac_enc_action_def_fb, \
                           default:  isac_enc_action_def_plain) (U)

#define isac_enc_ind_hdr(T,U) _Generic ((T), \
                           isac_enc_plain_t*: isac_enc_ind_hdr_plain , \
                           isac_enc_asn_t*: isac_enc_ind_hdr_asn, \
                           isac_enc_fb_t*: isac_enc_ind_hdr_fb, \
                           default:  isac_enc_ind_hdr_plain) (U)

#define isac_enc_ind_msg(T,U) _Generic ((T), \
                           isac_enc_plain_t*: isac_enc_ind_msg_plain , \
                           isac_enc_asn_t*: isac_enc_ind_msg_asn, \
                           isac_enc_fb_t*: isac_enc_ind_msg_fb, \
                           default:  isac_enc_ind_msg_plain) (U)

#define isac_enc_call_proc_id(T,U) _Generic ((T), \
                           isac_enc_plain_t*: isac_enc_call_proc_id_plain , \
                           isac_enc_asn_t*: isac_enc_call_proc_id_asn, \
                           isac_enc_fb_t*: isac_enc_call_proc_id_fb, \
                           default:  isac_enc_call_proc_id_plain) (U)

#define isac_enc_ctrl_hdr(T,U) _Generic ((T), \
                           isac_enc_plain_t*: isac_enc_ctrl_hdr_plain , \
                           isac_enc_asn_t*: isac_enc_ctrl_hdr_asn, \
                           isac_enc_fb_t*: isac_enc_ctrl_hdr_fb, \
                           default:  isac_enc_ctrl_hdr_plain) (U)

#define isac_enc_ctrl_msg(T,U) _Generic ((T), \
                           isac_enc_plain_t*: isac_enc_ctrl_msg_plain , \
                           isac_enc_asn_t*: isac_enc_ctrl_msg_asn, \
                           isac_enc_fb_t*: isac_enc_ctrl_msg_fb, \
                           default:  isac_enc_ctrl_msg_plain) (U)

#define isac_enc_ctrl_out(T,U) _Generic ((T), \
                           isac_enc_plain_t*: isac_enc_ctrl_out_plain , \
                           isac_enc_asn_t*: isac_enc_ctrl_out_asn, \
                           isac_enc_fb_t*: isac_enc_ctrl_out_fb, \
                           default:  isac_enc_ctrl_out_plain) (U)

#define isac_enc_func_def(T,U) _Generic ((T), \
                           isac_enc_plain_t*: isac_enc_func_def_plain, \
                           isac_enc_asn_t*: isac_enc_func_def_asn, \
                           isac_enc_fb_t*:  isac_enc_func_def_fb, \
                           default:  isac_enc_func_def_plain) (U)

#endif

