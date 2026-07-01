/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*
 * ISAC monitoring xApp: subscribes to the ISAC SM (SM_ISAC_ID) and logs every
 * CSI snapshot H[k, r] to a binary file for offline Doppler-isolation sensing
 * (see isac_csi_reader.py / isac_doppler_pipeline.py).
 *
 * File format (little endian):
 *   file header : magic "ISACCSI1" (8 bytes), u32 version, u32 sizeof(isac_csi_meta_t)
 *   record      : isac_csi_meta_t, i64 rx_tstamp_us (xApp arrival time),
 *                 int16 iq[2 * nb_rx_ant * nb_ue_ports * nb_sc]
 *
 * Reliability: gNB-side sequence numbers are checked for gaps and the gNB
 * ring-overflow counter (dropped_snap) is reported.
 */

#include "../../../../src/xApp/e42_xapp_api.h"
#include "../../../../src/util/alg_ds/alg/defer.h"
#include "../../../../src/util/time_now_us.h"
#include "../../../../src/sm/isac_sm/isac_sm_id.h"

#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static
FILE* fp_csi;

static
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

static
uint64_t cnt_snap;

static
uint32_t last_seq;

static
uint64_t lost_seq;

static
uint32_t last_dropped;

static
bool volatile keep_running = true;

static
void sigint_handler(int sig)
{
  (void)sig;
  keep_running = false;
}

static
void write_file_header(FILE* f)
{
  const char magic[8] = {'I','S','A','C','C','S','I','1'};
  const uint32_t version = 1;
  const uint32_t meta_sz = (uint32_t)sizeof(isac_csi_meta_t);
  fwrite(magic, 1, 8, f);
  fwrite(&version, sizeof(version), 1, f);
  fwrite(&meta_sz, sizeof(meta_sz), 1, f);
}

static
void sm_cb_isac(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == ISAC_STATS_V0);

  int64_t const now = time_now_us();
  isac_ind_msg_t const* msg = &rd->ind.isac.msg;

  pthread_mutex_lock(&mtx);

  for(uint32_t i = 0; i < msg->len_snap; ++i){
    isac_csi_snap_t const* s = &msg->snap[i];
    isac_csi_meta_t const* m = &s->meta;

    if(cnt_snap > 0 && m->seq != last_seq + 1)
      lost_seq += m->seq - last_seq - 1;
    last_seq = m->seq;

    fwrite(m, sizeof(*m), 1, fp_csi);
    fwrite(&now, sizeof(now), 1, fp_csi);
    size_t const len_iq = isac_snap_iq_len(m);
    fwrite(s->iq, sizeof(int16_t), len_iq, fp_csi);

    ++cnt_snap;

    if(cnt_snap % 64 == 1){
      printf("[ISAC xApp] snap %" PRIu64 ": seq %u rnti %04x sfn %u.%u R %u ports %u sc %u snr %d dB "
             "e2e latency %" PRId64 " us | lost(seq) %" PRIu64 " dropped(gNB) %u\n",
             cnt_snap, m->seq, m->rnti, m->frame, m->slot, m->nb_rx_ant, m->nb_ue_ports,
             m->nb_sc, m->wb_snr_db, now - m->tstamp_us, lost_seq, msg->dropped_snap);
    }
  }

  if(msg->dropped_snap != last_dropped){
    printf("[ISAC xApp] WARNING: gNB CSI ring overflow, dropped_snap %u -> %u. "
           "Poll faster (shorter period) or enlarge the PHY ring.\n",
           last_dropped, msg->dropped_snap);
    last_dropped = msg->dropped_snap;
  }

  fflush(fp_csi);
  pthread_mutex_unlock(&mtx);
}

int main(int argc, char *argv[])
{
  fr_args_t args = init_fr_args(argc, argv);

  const char* out_path = getenv("ISAC_CSI_FILE");
  if(out_path == NULL)
    out_path = "/tmp/isac_csi.bin";

  fp_csi = fopen(out_path, "wb");
  assert(fp_csi != NULL && "Could not open CSI output file");
  write_file_header(fp_csi);
  printf("[ISAC xApp] writing CSI snapshots to %s\n", out_path);

  signal(SIGINT, sigint_handler);
  signal(SIGTERM, sigint_handler);

  //Init the xApp
  init_xapp_api(&args);
  sleep(1);

  e2_node_arr_xapp_t nodes = e2_nodes_xapp_api();
  defer({ free_e2_node_arr_xapp(&nodes); });
  assert(nodes.len > 0);
  printf("[ISAC xApp] connected E2 nodes = %d\n", nodes.len);

  // Drain the gNB CSI ring every 5 ms. The indication carries all snapshots
  // gathered since the last poll, so no SRS occasion is missed.
  const char* period = "5_ms";

  sm_ans_xapp_t* handle = calloc(nodes.len, sizeof(sm_ans_xapp_t));
  assert(handle != NULL);
  defer({ free(handle); });

  for(int i = 0; i < nodes.len; ++i){
    e2_node_connected_xapp_t* n = &nodes.n[i];
    handle[i] = report_sm_xapp_api(&n->id, SM_ISAC_ID, (void*)period, sm_cb_isac);
    assert(handle[i].success == true);
  }

  while(keep_running)
    usleep(100000);

  for(int i = 0; i < nodes.len; ++i){
    if(handle[i].success == true)
      rm_report_sm_xapp_api(handle[i].u.handle);
  }
  sleep(1);

  //Stop the xApp. Avoid deadlock.
  while(try_stop_xapp_api() == false)
    usleep(1000);

  pthread_mutex_lock(&mtx);
  fclose(fp_csi);
  pthread_mutex_unlock(&mtx);

  printf("[ISAC xApp] done. %" PRIu64 " snapshots, %" PRIu64 " lost (seq gaps), %u dropped at gNB. File: %s\n",
         cnt_snap, lost_seq, last_dropped, out_path);
  return 0;
}
