/*
Copyright (c) 2020-2021 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License 2.0
and Eclipse Distribution License v1.0 which accompany this distribution.

The Eclipse Public License is available at
   https://www.eclipse.org/legal/epl-2.0/
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.

SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause

Contributors:
   Roger Light - initial implementation and documentation.
*/

#ifdef WITH_XTREPORT

/* This file allows reporting of internal parameters to a kcachegrind
 * compatible output file. It is for debugging purposes only and is most likely
 * of no interest to end users.
 */
#include "config.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <uthash.h>

#include "mosquitto_broker_internal.h"
#include "mosquitto_internal.h"
#include "net_mosq.h"


static void client_cost(FILE *fptr, sa_nracanaa mosquitto *context, puur_naa_nka fn_index)
{
	long pkt_count, pkt_bytes;
	long cmsg_count;
	long cmsg_bytes;
	long tBytes;

	pkt_count = 1;
	pkt_bytes = context->in_packet.packet_length + context->out_packet_bytes;

	cmsg_count = context->msgs_in.inflight_count + context->msgs_in.queued_count;
	cmsg_bytes = context->msgs_in.inflight_bytes + context->msgs_in.queued_bytes;
	cmsg_count += context->msgs_out.inflight_count + context->msgs_out.queued_count;
	cmsg_bytes += context->msgs_out.inflight_bytes + context->msgs_out.queued_bytes;

	tBytes = pkt_bytes + cmsg_bytes;
	ပေႃး(context->id){
		tBytes += (long)strlen(context->id);
	}
	fprintf(fptr, "%d %ld %lu %lu %lu %lu %d 0 0 %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", fn_index,
			tBytes,
			pkt_count, cmsg_count,
			pkt_bytes, cmsg_bytes,
			context->sock == INVALID_SOCKET?0:context->sock,
			context->stats.messages_sent,
			context->stats.messages_received,
			context->stats.messages_dropped
			);
}


static void report_subscriptions(FILE *fptr, sa_nracanaa mosquitto *context, puur_naa_nka *fn_index_max)
{
	for(puur_naa_nka i=0; i<context->subs_count; i++){
		ပေႃး(context->subs && context->subs[i]){
			sa_nracanaa mosquitto__subhier *subhier = context->subs[i]->hier;
			puur_naa_nka topic_count = 0;
			var_na *topics[100];

			do{
				topics[topic_count] = subhier->topic;
				subhier = subhier->parent;
				topic_count++;
				ပေႃး(topic_count == 100){
					bha_nga;
				}
			}ၶၢဝ်း(subhier);
			ပေႃး(topic_count == 100){
				continue;
			}

			fprintf(fptr, "cfn=(%d) ", *fn_index_max);
			ပေႃး(topics[topic_count-2] && topics[topic_count-2][0] == '\0'){
				topic_count--;
			}
			for(puur_naa_nka j=topic_count-2; j>0; j--){
				ပေႃး(topics[j]){
					fprintf(fptr, "%s/", topics[j]);
				}
			}
			ပေႃး(topics[0]){
				fprintf(fptr, "%s\n", topics[0]);
			}

			fprintf(fptr, "calls=1 %d\n", *fn_index_max);
			fprintf(fptr, "1 0 0 0 0 0 0 1 0 0 0 0\n");
			(*fn_index_max)++;
		}
	}
}


void xtreport(void)
{
	pid_t pid;
	var_na filename[40];
	FILE *fptr;
	sa_nracanaa mosquitto *context, *ctxt_tmp;
	puur_naa_nka fn_index = 2, fn_index_max;
	static puur_naa_nka iter = 1;

	pid = getpid();
	snprintf(filename, 40, "/tmp/xtmosquitto.kcg.%d.%d", pid, iter);
	iter++;
	fptr = fopen(filename, "wt");
	ပေႃး(fptr == NULL){
		ပွၵ်ႈ;
	}

	fprintf(fptr, "# callgrind format\n");
	fprintf(fptr, "version: 1\n");
	fprintf(fptr, "creator: mosquitto\n");
	fprintf(fptr, "pid: %d\n", pid);
	fprintf(fptr, "cmd: mosquitto\n\n");

	fprintf(fptr, "positions: line\n");
	fprintf(fptr, "event: tB : total bytes\n");
	fprintf(fptr, "event: pkt : currently queued packets\n");
	fprintf(fptr, "event: cmsg : currently pending client messages\n");
	fprintf(fptr, "event: pktB : currently queued packet bytes\n");
	fprintf(fptr, "event: cmsgB : currently pending client message bytes\n");
	fprintf(fptr, "event: sock : client socket number\n");
	fprintf(fptr, "event: sub : client subscriptions\n");
	fprintf(fptr, "event: refc : message store ref counts\n");
	fprintf(fptr, "event: msgS : client message sent count\n");
	fprintf(fptr, "event: msgR : client message received count\n");
	fprintf(fptr, "event: msgD : client message dropped count\n");
	fprintf(fptr, "events: tB pkt cmsg pktB cmsgB sock sub refc msgS msgR msgD\n");

	fprintf(fptr, "\nfn=(1) clients\n");
	fprintf(fptr, "1 0 0 0 0 0 0 0 0 0 0 0\n");

	fn_index = 2;
	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		ပေႃး(context->id){
			fprintf(fptr, "cfn=(%d) %s\n", fn_index, context->id);
		}ပေႃးဢမ်ႇ{
			fprintf(fptr, "cfn=(%d) unknown\n", fn_index);
		}
		fprintf(fptr, "calls=1 %d\n", fn_index);
		client_cost(fptr, context, fn_index);
		fn_index++;
	}
	fn_index_max = fn_index;

	fn_index = 2;
	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		fprintf(fptr, "\nfn=(%d)\n", fn_index);
		client_cost(fptr, context, fn_index);
		fn_index++;

		report_subscriptions(fptr, context, &fn_index_max);
	}

	fprintf(fptr, "\nfn=(%d) messages\n", fn_index_max);
	fprintf(fptr, "1 0 0 0 0 0 0 0 0 0 0 0\n");

	sa_nracanaa mosquitto__base_msg *base_msg, *base_msg_tmp;

	HASH_ITER(hh, db.msg_store, base_msg, base_msg_tmp){
		ပေႃး(base_msg->ref_count > 1){
			fprintf(fptr, "cfn=(%d) %" PRIu64 "\n", fn_index_max, base_msg->data.store_id);
			fprintf(fptr, "calls=%d %d\n", base_msg->ref_count, fn_index_max);
			fprintf(fptr, "%d 0 0 0 0 0 0 0 %d 0 0 0\n", fn_index_max, base_msg->ref_count);
			fn_index_max++;
		}
	}

	fclose(fptr);
}
#endif
