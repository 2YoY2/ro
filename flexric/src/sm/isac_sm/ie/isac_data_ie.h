/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */


#ifndef ISAC_DATA_INFORMATION_ELEMENTS_H
#define ISAC_DATA_INFORMATION_ELEMENTS_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ISAC (Integrated Sensing and Communication) custom Service Model.
 *
 * The Indication Message carries one or more CSI "snapshots". A snapshot is
 * the SRS-based frequency-domain channel estimate H[k, r] of one SRS occasion:
 * complex int16 I/Q per (rx antenna r, ue tx port p, subcarrier k).
 * A sequence of snapshots over slow time m forms the tensor H[m, k, r] needed
 * for Doppler-isolation sensing (MTI -> Doppler DFT -> range IDFT -> angle DFT).
 */


#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

//////////////////////////////////////
// RIC Event Trigger Definition
/////////////////////////////////////

typedef struct {
  uint32_t ms;
} isac_event_trigger_t;

void free_isac_event_trigger(isac_event_trigger_t* src);

isac_event_trigger_t cp_isac_event_trigger( isac_event_trigger_t const* src);

bool eq_isac_event_trigger(isac_event_trigger_t const* m0, isac_event_trigger_t const* m1);



//////////////////////////////////////
// RIC Action Definition
/////////////////////////////////////


typedef struct {
  uint32_t dummy;
} isac_action_def_t;

void free_isac_action_def(isac_action_def_t* src);

isac_action_def_t cp_isac_action_def(isac_action_def_t* src);

bool eq_isac_action_def(isac_event_trigger_t* m0,  isac_event_trigger_t* m1);



//////////////////////////////////////
// RIC Indication Header
/////////////////////////////////////


typedef struct{
  uint32_t dummy;
} isac_ind_hdr_t;

void free_isac_ind_hdr(isac_ind_hdr_t* src);

isac_ind_hdr_t cp_isac_ind_hdr(isac_ind_hdr_t const* src);

bool eq_isac_ind_hdr(isac_ind_hdr_t* m0, isac_ind_hdr_t* m1);

//////////////////////////////////////
// RIC Indication Message
/////////////////////////////////////

// Fixed-size, memcpy-able metadata of one CSI snapshot.
// Everything needed to interpret the I/Q payload and to map the
// (subcarrier, antenna, slow-time) axes to (range, angle, Doppler):
//   - range axis:   subcarrier spacing scs_hz, nb_sc, first_sc
//   - angle axis:   nb_rx_ant, carrier frequency ul_carrier_freq_hz (lambda)
//   - Doppler axis: frame/slot/tstamp_us of consecutive snapshots (Ts)
typedef struct {
  int64_t  tstamp_us;          // gNB wall-clock [us] when the SRS was processed
  uint64_t ul_carrier_freq_hz; // UL carrier center frequency [Hz] (e.g. 3.7e9)
  uint32_t seq;                // gNB-side monotonic snapshot counter (gap => loss)
  uint32_t nb_sc;              // subcarriers per (rx ant, ue port), e.g. 3264/3276
  uint32_t first_sc;           // index of first estimated subcarrier, relative to
                               // the lowest subcarrier of the carrier grid
  uint32_t scs_hz;             // subcarrier spacing [Hz], e.g. 30000
  uint16_t frame;              // SFN [0..1023] of the SRS occasion
  uint16_t slot;               // slot within frame
  uint16_t rnti;               // UE RNTI sounding the channel
  int16_t  wb_snr_db;          // wideband SRS SNR [dB]; INT16_MIN if invalid
  uint16_t ta_offset;          // timing advance offset (0xFFFF invalid)
  uint16_t nb_prb_carrier;     // carrier bandwidth in PRB (e.g. 273)
  uint16_t nb_prb_srs;         // sounded bandwidth in PRB (m_SRS, e.g. 272)
  uint16_t nb_symb_srs;        // number of SRS symbols (payload carries symbol 0)
  uint8_t  nb_rx_ant;          // R: gNB rx antennas in the payload
  uint8_t  nb_ue_ports;        // N_ap: UE SRS tx antenna ports in the payload
  uint8_t  comb_size;          // K_TC of the SRS (estimate is interpolated to all sc)
  uint8_t  pad;
} isac_csi_meta_t;

// One CSI snapshot: metadata + I/Q payload.
// iq layout: [rx ant][ue port][subcarrier][re, im], int16 each,
// i.e. len = 2 * nb_rx_ant * nb_ue_ports * nb_sc values.
typedef struct {
  isac_csi_meta_t meta;
  int16_t* iq;
} isac_csi_snap_t;

size_t isac_snap_iq_len(isac_csi_meta_t const* m);

isac_csi_snap_t cp_isac_csi_snap(isac_csi_snap_t const* src);

bool eq_isac_csi_snap(isac_csi_snap_t const* m0, isac_csi_snap_t const* m1);

typedef struct {
  uint32_t len_snap;        // number of snapshots drained in this indication
  isac_csi_snap_t* snap;
  uint32_t dropped_snap;    // cumulative snapshots lost to ring overflow at the gNB
  int64_t tstamp;           // [us] when the indication message was built
} isac_ind_msg_t;

void free_isac_ind_msg(isac_ind_msg_t* src);

isac_ind_msg_t cp_isac_ind_msg(isac_ind_msg_t const* src);

bool eq_isac_ind_msg(isac_ind_msg_t* m0, isac_ind_msg_t* m1);


//////////////////////////////////////
// RIC Call Process ID
/////////////////////////////////////

typedef struct {
  uint32_t dummy;
} isac_call_proc_id_t;

void free_isac_call_proc_id( isac_call_proc_id_t* src);

isac_call_proc_id_t cp_isac_call_proc_id( isac_call_proc_id_t* src);

bool eq_isac_call_proc_id(isac_call_proc_id_t* m0, isac_call_proc_id_t* m1);

//////////////////////////////////////
// RIC Control Header
/////////////////////////////////////

typedef struct {
  uint32_t dummy;
} isac_ctrl_hdr_t;

void free_isac_ctrl_hdr( isac_ctrl_hdr_t* src);

isac_ctrl_hdr_t cp_isac_ctrl_hdr(isac_ctrl_hdr_t* src);

bool eq_isac_ctrl_hdr(isac_ctrl_hdr_t* m0, isac_ctrl_hdr_t* m1);

//////////////////////////////////////
// RIC Control Message
/////////////////////////////////////

typedef struct {
  uint32_t action;
} isac_ctrl_msg_t;

void free_isac_ctrl_msg( isac_ctrl_msg_t* src);

isac_ctrl_msg_t cp_isac_ctrl_msg(isac_ctrl_msg_t* src);

bool eq_isac_ctrl_msg(isac_ctrl_msg_t* m0, isac_ctrl_msg_t* m1);


//////////////////////////////////////
// RIC Control Outcome
/////////////////////////////////////

typedef enum{
  ISAC_CTRL_OUT_OK,


  ISAC_CTRL_OUT_END
} isac_ctrl_out_e;

typedef struct {
  isac_ctrl_out_e ans;
} isac_ctrl_out_t;

void free_isac_ctrl_out(isac_ctrl_out_t* src);

isac_ctrl_out_t cp_isac_ctrl_out(isac_ctrl_out_t* src);

bool eq_isac_ctrl_out(isac_ctrl_out_t* m0, isac_ctrl_out_t* m1);


//////////////////////////////////////
// RAN Function Definition
/////////////////////////////////////

typedef struct {
  size_t len;
  uint8_t* buf;
} isac_func_def_t;

void free_isac_func_def(isac_func_def_t* src);

isac_func_def_t cp_isac_func_def(isac_func_def_t const* src);

bool eq_isac_func_def(isac_func_def_t const* m0, isac_func_def_t const* m1);


/////////////////////////////////////////////////
//////////////////////////////////////////////////
/////////////////////////////////////////////////


/*
 * O-RAN defined 5 Procedures: RIC Subscription, RIC Indication, RIC Control, E2 Setup and RIC Service Update
 * */


///////////////
/// RIC Subscription
///////////////

typedef struct{
  isac_event_trigger_t et;
  isac_action_def_t* ad;
} isac_sub_data_t;

///////////////
// RIC Indication
///////////////

typedef struct{
  isac_ind_hdr_t hdr;
  isac_ind_msg_t msg;
  isac_call_proc_id_t* proc_id;
} isac_ind_data_t;

isac_ind_data_t cp_isac_ind_data( isac_ind_data_t const* src);

void free_isac_ind_data(isac_ind_data_t* ind);

///////////////
// RIC Control
///////////////

typedef struct{
  isac_ctrl_hdr_t hdr;
  isac_ctrl_msg_t msg;
} isac_ctrl_req_data_t;

typedef struct{
  isac_ctrl_out_t* out;
} isac_ctrl_out_data_t;

///////////////
// E2 Setup
///////////////

typedef struct{
  isac_func_def_t func_def;
} isac_e2_setup_data_t;

///////////////
// RIC Service Update
///////////////

typedef struct{
  isac_func_def_t func_def;
} isac_ric_service_update_t;

#ifdef __cplusplus
}
#endif




#endif
