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
#شامل "config.h"

#شامل <inttypes.h>
#شامل <stdint.h>
#شامل <stdio.h>
#شامل <uthash.h>

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto_internal.h"
#شامل "net_mosq.h"


ایستا تهی client_cost(FILE *fptr, ساختار mosquitto *context, صحیح fn_index)
{
	long pkt_شمار, pkt_bytes;
	long cmsg_شمار;
	long cmsg_bytes;
	long tBytes;

	pkt_شمار = 1;
	pkt_bytes = context->in_packet.packet_length + context->out_packet_bytes;

	cmsg_شمار = context->msgs_in.inflight_شمار + context->msgs_in.queued_شمار;
	cmsg_bytes = context->msgs_in.inflight_bytes + context->msgs_in.queued_bytes;
	cmsg_شمار += context->msgs_out.inflight_شمار + context->msgs_out.queued_شمار;
	cmsg_bytes += context->msgs_out.inflight_bytes + context->msgs_out.queued_bytes;

	tBytes = pkt_bytes + cmsg_bytes;
	اگر(context->id){
		tBytes += (long)strlen(context->id);
	}
	fprintf(fptr, "%d %ld %lu %lu %lu %lu %d 0 0 %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", fn_index,
			tBytes,
			pkt_شمار, cmsg_شمار,
			pkt_bytes, cmsg_bytes,
			context->sock == INVALID_SOCKET?0:context->sock,
			context->stats.messages_sent,
			context->stats.messages_received,
			context->stats.messages_dropped
			);
}


ایستا تهی report_subscriptions(FILE *fptr, ساختار mosquitto *context, صحیح *fn_index_max)
{
	برای(صحیح i=0; i<context->subs_شمار; i++){
		اگر(context->subs && context->subs[i]){
			ساختار mosquitto__subhier *subhier = context->subs[i]->hier;
			صحیح topic_شمار = 0;
			نویسه *topics[100];

			انجامبده{
				topics[topic_شمار] = subhier->topic;
				subhier = subhier->parent;
				topic_شمار++;
				اگر(topic_شمار == 100){
					بشکن;
				}
			}وقتی(subhier);
			اگر(topic_شمار == 100){
				ادامه;
			}

			fprintf(fptr, "cfn=(%d) ", *fn_index_max);
			اگر(topics[topic_شمار-2] && topics[topic_شمار-2][0] == '\0'){
				topic_شمار--;
			}
			برای(صحیح j=topic_شمار-2; j>0; j--){
				اگر(topics[j]){
					fprintf(fptr, "%s/", topics[j]);
				}
			}
			اگر(topics[0]){
				fprintf(fptr, "%s\n", topics[0]);
			}

			fprintf(fptr, "calls=1 %d\n", *fn_index_max);
			fprintf(fptr, "1 0 0 0 0 0 0 1 0 0 0 0\n");
			(*fn_index_max)++;
		}
	}
}


تهی xtreport(تهی)
{
	pid_t pid;
	نویسه filename[40];
	FILE *fptr;
	ساختار mosquitto *context, *ctxt_tmp;
	صحیح fn_index = 2, fn_index_max;
	ایستا صحیح iter = 1;

	pid = getpid();
	snprintf(filename, 40, "/tmp/xtmosquitto.kcg.%d.%d", pid, iter);
	iter++;
	fptr = fopen(filename, "wt");
	اگر(fptr == پوچ){
		بازگشت;
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
		اگر(context->id){
			fprintf(fptr, "cfn=(%d) %s\n", fn_index, context->id);
		}وگرنه{
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

	ساختار mosquitto_base_پیام *base_پیام, *base_پیام_tmp;

	HASH_ITER(hh, db.پیام_store, base_پیام, base_پیام_tmp){
		اگر(base_پیام->ref_شمار > 1){
			fprintf(fptr, "cfn=(%d) %" PRIu64 "\n", fn_index_max, base_پیام->data.store_id);
			fprintf(fptr, "calls=%d %d\n", base_پیام->ref_شمار, fn_index_max);
			fprintf(fptr, "%d 0 0 0 0 0 0 0 %d 0 0 0\n", fn_index_max, base_پیام->ref_شمار);
			fn_index_max++;
		}
	}

	fclose(fptr);
}
#endif
