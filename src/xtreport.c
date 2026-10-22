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

#اگر_تعریف_شدہ WITH_XTREPORT

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


ساکن خالی client_لاگت(FILE *fptr, ساخت mosquitto *context, صحیح fn_اشاریہ)
{
	طویل pkt_گنتی, pkt_بائٹیں;
	طویل cmsg_گنتی;
	طویل cmsg_بائٹیں;
	طویل t_بائٹیں;

	pkt_گنتی = 1;
	pkt_بائٹیں = context->اندر_packet.packet_لمبائی + context->برآمد_packet_بائٹیں;

	cmsg_گنتی = context->msgs_اندر.inflight_گنتی + context->msgs_اندر.queued_گنتی;
	cmsg_بائٹیں = context->msgs_اندر.inflight_بائٹیں + context->msgs_اندر.queued_بائٹیں;
	cmsg_گنتی += context->msgs_برآمد.inflight_گنتی + context->msgs_برآمد.queued_گنتی;
	cmsg_بائٹیں += context->msgs_برآمد.inflight_بائٹیں + context->msgs_برآمد.queued_بائٹیں;

	t_بائٹیں = pkt_بائٹیں + cmsg_بائٹیں;
	اگر(context->id){
		t_بائٹیں += (طویل)لڑی_لمبائی(context->id);
	}
	fprintf(fptr, "%d %ld %lu %lu %lu %lu %d 0 0 %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", fn_اشاریہ,
			t_بائٹیں,
			pkt_گنتی, cmsg_گنتی,
			pkt_بائٹیں, cmsg_بائٹیں,
			context->sock == INVALID_SOCKET?0:context->sock,
			context->stats.messages_بھیجا,
			context->stats.messages_received,
			context->stats.messages_dropped
			);
}


ساکن خالی report_subscriptions(FILE *fptr, ساخت mosquitto *context, صحیح *fn_اشاریہ_max)
{
	برائے(صحیح i=0; i<context->subs_گنتی; i++){
		اگر(context->subs && context->subs[i]){
			ساخت mosquitto__subhier *subhier = context->subs[i]->hier;
			صحیح topic_گنتی = 0;
			حرف *topics[100];

			کرو{
				topics[topic_گنتی] = subhier->topic;
				subhier = subhier->والد;
				topic_گنتی++;
				اگر(topic_گنتی == 100){
					توڑو;
				}
			}جب(subhier);
			اگر(topic_گنتی == 100){
				جاری;
			}

			fprintf(fptr, "cfn=(%d) ", *fn_اشاریہ_max);
			اگر(topics[topic_گنتی-2] && topics[topic_گنتی-2][0] == '\0'){
				topic_گنتی--;
			}
			برائے(صحیح j=topic_گنتی-2; j>0; j--){
				اگر(topics[j]){
					fprintf(fptr, "%s/", topics[j]);
				}
			}
			اگر(topics[0]){
				fprintf(fptr, "%s\n", topics[0]);
			}

			fprintf(fptr, "calls=1 %d\n", *fn_اشاریہ_max);
			fprintf(fptr, "1 0 0 0 0 0 0 1 0 0 0 0\n");
			(*fn_اشاریہ_max)++;
		}
	}
}


خالی xtreport(خالی)
{
	pid_t pid;
	حرف filename[40];
	FILE *fptr;
	ساخت mosquitto *context, *ctxt_tmp;
	صحیح fn_اشاریہ = 2, fn_اشاریہ_max;
	ساکن صحیح iter = 1;

	pid = getpid();
	snprintf(filename, 40, "/tmp/xtmosquitto.kcg.%d.%d", pid, iter);
	iter++;
	fptr = fopen(filename, "wt");
	اگر(fptr == تہی){
		واپس;
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

	fn_اشاریہ = 2;
	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		اگر(context->id){
			fprintf(fptr, "cfn=(%d) %s\n", fn_اشاریہ, context->id);
		}ورنہ{
			fprintf(fptr, "cfn=(%d) unknown\n", fn_اشاریہ);
		}
		fprintf(fptr, "calls=1 %d\n", fn_اشاریہ);
		client_لاگت(fptr, context, fn_اشاریہ);
		fn_اشاریہ++;
	}
	fn_اشاریہ_max = fn_اشاریہ;

	fn_اشاریہ = 2;
	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		fprintf(fptr, "\nfn=(%d)\n", fn_اشاریہ);
		client_لاگت(fptr, context, fn_اشاریہ);
		fn_اشاریہ++;

		report_subscriptions(fptr, context, &fn_اشاریہ_max);
	}

	fprintf(fptr, "\nfn=(%d) messages\n", fn_اشاریہ_max);
	fprintf(fptr, "1 0 0 0 0 0 0 0 0 0 0 0\n");

	ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام, *بنیاد_پیغام_tmp;

	HASH_ITER(hh, db.پیغام_store, بنیاد_پیغام, بنیاد_پیغام_tmp){
		اگر(بنیاد_پیغام->ref_گنتی > 1){
			fprintf(fptr, "cfn=(%d) %" PRIu64 "\n", fn_اشاریہ_max, بنیاد_پیغام->ڈیٹا.store_id);
			fprintf(fptr, "calls=%d %d\n", بنیاد_پیغام->ref_گنتی, fn_اشاریہ_max);
			fprintf(fptr, "%d 0 0 0 0 0 0 0 %d 0 0 0\n", fn_اشاریہ_max, بنیاد_پیغام->ref_گنتی);
			fn_اشاریہ_max++;
		}
	}

	fclose(fptr);
}
#اگر_ختم
