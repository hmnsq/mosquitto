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
#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <inttypes.h>
#અન્તર્ભાવ <stdint.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <uthash.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto_internal.h"
#અન્તર્ભાવ "net_mosq.h"


સ્થિર શૂન્ય client_cost(FILE *fptr, સંરચના mosquitto *context, પૂર્ણાંક fn_index)
{
	long pkt_ગણતરી, pkt_bytes;
	long cmsg_ગણતરી;
	long cmsg_bytes;
	long tBytes;

	pkt_ગણતરી = 1;
	pkt_bytes = context->in_packet.packet_length + context->out_packet_bytes;

	cmsg_ગણતરી = context->msgs_in.inflight_ગણતરી + context->msgs_in.queued_ગણતરી;
	cmsg_bytes = context->msgs_in.inflight_bytes + context->msgs_in.queued_bytes;
	cmsg_ગણતરી += context->msgs_out.inflight_ગણતરી + context->msgs_out.queued_ગણતરી;
	cmsg_bytes += context->msgs_out.inflight_bytes + context->msgs_out.queued_bytes;

	tBytes = pkt_bytes + cmsg_bytes;
	જો(context->id){
		tBytes += (long)strlen(context->id);
	}
	fprintf(fptr, "%d %ld %lu %lu %lu %lu %d 0 0 %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", fn_index,
			tBytes,
			pkt_ગણતરી, cmsg_ગણતરી,
			pkt_bytes, cmsg_bytes,
			context->sock == INVALID_SOCKET?0:context->sock,
			context->stats.messages_sent,
			context->stats.messages_received,
			context->stats.messages_dropped
			);
}


સ્થિર શૂન્ય report_subscriptions(FILE *fptr, સંરચના mosquitto *context, પૂર્ણાંક *fn_index_max)
{
	માટે(પૂર્ણાંક i=0; i<context->subs_ગણતરી; i++){
		જો(context->subs && context->subs[i]){
			સંરચના mosquitto__subhier *subhier = context->subs[i]->hier;
			પૂર્ણાંક topic_ગણતરી = 0;
			અક્ષર *topics[100];

			કરો{
				topics[topic_ગણતરી] = subhier->topic;
				subhier = subhier->parent;
				topic_ગણતરી++;
				જો(topic_ગણતરી == 100){
					તોડો;
				}
			}જ્યારે(subhier);
			જો(topic_ગણતરી == 100){
				ચાલુ;
			}

			fprintf(fptr, "cfn=(%d) ", *fn_index_max);
			જો(topics[topic_ગણતરી-2] && topics[topic_ગણતરી-2][0] == '\0'){
				topic_ગણતરી--;
			}
			માટે(પૂર્ણાંક j=topic_ગણતરી-2; j>0; j--){
				જો(topics[j]){
					fprintf(fptr, "%s/", topics[j]);
				}
			}
			જો(topics[0]){
				fprintf(fptr, "%s\n", topics[0]);
			}

			fprintf(fptr, "calls=1 %d\n", *fn_index_max);
			fprintf(fptr, "1 0 0 0 0 0 0 1 0 0 0 0\n");
			(*fn_index_max)++;
		}
	}
}


શૂન્ય xtreport(શૂન્ય)
{
	pid_t pid;
	અક્ષર filename[40];
	FILE *fptr;
	સંરચના mosquitto *context, *ctxt_tmp;
	પૂર્ણાંક fn_index = 2, fn_index_max;
	સ્થિર પૂર્ણાંક iter = 1;

	pid = getpid();
	snprintf(filename, 40, "/tmp/xtmosquitto.kcg.%d.%d", pid, iter);
	iter++;
	fptr = fopen(filename, "wt");
	જો(fptr == ખાલી){
		પાછા;
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
		જો(context->id){
			fprintf(fptr, "cfn=(%d) %s\n", fn_index, context->id);
		}નહીંતર{
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

	સંરચના mosquitto_base_સંદેશ *base_સંદેશ, *base_સંદેશ_tmp;

	HASH_ITER(hh, db.સંદેશ_store, base_સંદેશ, base_સંદેશ_tmp){
		જો(base_સંદેશ->ref_ગણતરી > 1){
			fprintf(fptr, "cfn=(%d) %" PRIu64 "\n", fn_index_max, base_સંદેશ->data.store_id);
			fprintf(fptr, "calls=%d %d\n", base_સંદેશ->ref_ગણતરી, fn_index_max);
			fprintf(fptr, "%d 0 0 0 0 0 0 0 %d 0 0 0\n", fn_index_max, base_સંદેશ->ref_ગણતરી);
			fn_index_max++;
		}
	}

	fclose(fptr);
}
#endif
