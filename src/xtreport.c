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
#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <inttypes.h>
#ଅନ୍ତର୍ଭାଵ <stdint.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <uthash.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto_internal.h"
#ଅନ୍ତର୍ଭାଵ "net_mosq.h"


ସ୍ଥିର ଶୂନ୍ୟ client_cost(FILE *fptr, ସଂରଚନା mosquitto *context, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fn_index)
{
	long pkt_ଗଣନା, pkt_bytes;
	long cmsg_ଗଣନା;
	long cmsg_bytes;
	long tBytes;

	pkt_ଗଣନା = 1;
	pkt_bytes = context->in_packet.packet_length + context->out_packet_bytes;

	cmsg_ଗଣନା = context->msgs_in.inflight_ଗଣନା + context->msgs_in.queued_ଗଣନା;
	cmsg_bytes = context->msgs_in.inflight_bytes + context->msgs_in.queued_bytes;
	cmsg_ଗଣନା += context->msgs_out.inflight_ଗଣନା + context->msgs_out.queued_ଗଣନା;
	cmsg_bytes += context->msgs_out.inflight_bytes + context->msgs_out.queued_bytes;

	tBytes = pkt_bytes + cmsg_bytes;
	ଯଦି(context->id){
		tBytes += (long)strlen(context->id);
	}
	fprintf(fptr, "%d %ld %lu %lu %lu %lu %d 0 0 %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", fn_index,
			tBytes,
			pkt_ଗଣନା, cmsg_ଗଣନା,
			pkt_bytes, cmsg_bytes,
			context->sock == INVALID_SOCKET?0:context->sock,
			context->stats.messages_sent,
			context->stats.messages_received,
			context->stats.messages_dropped
			);
}


ସ୍ଥିର ଶୂନ୍ୟ report_subscriptions(FILE *fptr, ସଂରଚନା mosquitto *context, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *fn_index_max)
{
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<context->subs_ଗଣନା; i++){
		ଯଦି(context->subs && context->subs[i]){
			ସଂରଚନା mosquitto__subhier *subhier = context->subs[i]->hier;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା topic_ଗଣନା = 0;
			ଅକ୍ଷର *topics[100];

			କର{
				topics[topic_ଗଣନା] = subhier->topic;
				subhier = subhier->parent;
				topic_ଗଣନା++;
				ଯଦି(topic_ଗଣନା == 100){
					ଭାଙ୍ଗ;
				}
			}ଯେତେବେଳେ(subhier);
			ଯଦି(topic_ଗଣନା == 100){
				ଜାରି;
			}

			fprintf(fptr, "cfn=(%d) ", *fn_index_max);
			ଯଦି(topics[topic_ଗଣନା-2] && topics[topic_ଗଣନା-2][0] == '\0'){
				topic_ଗଣନା--;
			}
			ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା j=topic_ଗଣନା-2; j>0; j--){
				ଯଦି(topics[j]){
					fprintf(fptr, "%s/", topics[j]);
				}
			}
			ଯଦି(topics[0]){
				fprintf(fptr, "%s\n", topics[0]);
			}

			fprintf(fptr, "calls=1 %d\n", *fn_index_max);
			fprintf(fptr, "1 0 0 0 0 0 0 1 0 0 0 0\n");
			(*fn_index_max)++;
		}
	}
}


ଶୂନ୍ୟ xtreport(ଶୂନ୍ୟ)
{
	pid_t pid;
	ଅକ୍ଷର filename[40];
	FILE *fptr;
	ସଂରଚନା mosquitto *context, *ctxt_tmp;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fn_index = 2, fn_index_max;
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା iter = 1;

	pid = getpid();
	snprintf(filename, 40, "/tmp/xtmosquitto.kcg.%d.%d", pid, iter);
	iter++;
	fptr = fopen(filename, "wt");
	ଯଦି(fptr == ଖାଲି){
		ଫେରନ୍ତୁ;
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
		ଯଦି(context->id){
			fprintf(fptr, "cfn=(%d) %s\n", fn_index, context->id);
		}ଅନ୍ୟଥା{
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

	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ, *base_ସନ୍ଦେଶ_tmp;

	HASH_ITER(hh, db.ସନ୍ଦେଶ_store, base_ସନ୍ଦେଶ, base_ସନ୍ଦେଶ_tmp){
		ଯଦି(base_ସନ୍ଦେଶ->ref_ଗଣନା > 1){
			fprintf(fptr, "cfn=(%d) %" PRIu64 "\n", fn_index_max, base_ସନ୍ଦେଶ->data.store_id);
			fprintf(fptr, "calls=%d %d\n", base_ସନ୍ଦେଶ->ref_ଗଣନା, fn_index_max);
			fprintf(fptr, "%d 0 0 0 0 0 0 0 %d 0 0 0\n", fn_index_max, base_ସନ୍ଦେଶ->ref_ଗଣନା);
			fn_index_max++;
		}
	}

	fclose(fptr);
}
#endif
