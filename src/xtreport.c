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

#اگر_تعریف_شده WITH_XTREPORT

/* This file allows reporting of internal parameters to a kcachegrind
 * compatible output file. It is for debugging purposes only and is most likely
 * of no interest to end users.
 */
#ضمن "config.h"

#ضمن <inttypes.h>
#ضمن <stdint.h>
#ضمن <stdio.h>
#ضمن <uthash.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto_internal.h"
#ضمن "net_mosq.h"


ساكن فراغ client_هزینه(FILE *fptr, بنية mosquitto *context, صحيح fn_شاخص)
{
	دراز pkt_شمار, pkt_بایتها;
	دراز cmsg_شمار;
	دراز cmsg_بایتها;
	دراز t_بایتها;

	pkt_شمار = 1;
	pkt_بایتها = context->درون_packet.packet_درازی + context->بیرون_packet_بایتها;

	cmsg_شمار = context->msgs_درون.inflight_شمار + context->msgs_درون.queued_شمار;
	cmsg_بایتها = context->msgs_درون.inflight_بایتها + context->msgs_درون.queued_بایتها;
	cmsg_شمار += context->msgs_بیرون.inflight_شمار + context->msgs_بیرون.queued_شمار;
	cmsg_بایتها += context->msgs_بیرون.inflight_بایتها + context->msgs_بیرون.queued_بایتها;

	t_بایتها = pkt_بایتها + cmsg_بایتها;
	إذا(context->id){
		t_بایتها += (دراز)طول_رشته(context->id);
	}
	fprintf(fptr, "%d %ld %lu %lu %lu %lu %d 0 0 %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", fn_شاخص,
			t_بایتها,
			pkt_شمار, cmsg_شمار,
			pkt_بایتها, cmsg_بایتها,
			context->sock == INVALID_SOCKET?0:context->sock,
			context->stats.messages_فرستاده,
			context->stats.messages_received,
			context->stats.messages_dropped
			);
}


ساكن فراغ report_subscriptions(FILE *fptr, بنية mosquitto *context, صحيح *fn_شاخص_max)
{
	لكل(صحيح i=0; i<context->subs_شمار; i++){
		إذا(context->subs && context->subs[i]){
			بنية mosquitto__subhier *subhier = context->subs[i]->hier;
			صحيح topic_شمار = 0;
			حرف *topics[100];

			افعل{
				topics[topic_شمار] = subhier->topic;
				subhier = subhier->والد;
				topic_شمار++;
				إذا(topic_شمار == 100){
					اكسر;
				}
			}طالما(subhier);
			إذا(topic_شمار == 100){
				استمر;
			}

			fprintf(fptr, "cfn=(%d) ", *fn_شاخص_max);
			إذا(topics[topic_شمار-2] && topics[topic_شمار-2][0] == '\0'){
				topic_شمار--;
			}
			لكل(صحيح j=topic_شمار-2; j>0; j--){
				إذا(topics[j]){
					fprintf(fptr, "%s/", topics[j]);
				}
			}
			إذا(topics[0]){
				fprintf(fptr, "%s\n", topics[0]);
			}

			fprintf(fptr, "calls=1 %d\n", *fn_شاخص_max);
			fprintf(fptr, "1 0 0 0 0 0 0 1 0 0 0 0\n");
			(*fn_شاخص_max)++;
		}
	}
}


فراغ xtreport(فراغ)
{
	pid_t pid;
	حرف filename[40];
	FILE *fptr;
	بنية mosquitto *context, *ctxt_tmp;
	صحيح fn_شاخص = 2, fn_شاخص_max;
	ساكن صحيح iter = 1;

	pid = getpid();
	snprintf(filename, 40, "/tmp/xtmosquitto.kcg.%d.%d", pid, iter);
	iter++;
	fptr = fopen(filename, "wt");
	إذا(fptr == عدم){
		أرجع;
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

	fn_شاخص = 2;
	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		إذا(context->id){
			fprintf(fptr, "cfn=(%d) %s\n", fn_شاخص, context->id);
		}وإلا{
			fprintf(fptr, "cfn=(%d) unknown\n", fn_شاخص);
		}
		fprintf(fptr, "calls=1 %d\n", fn_شاخص);
		client_هزینه(fptr, context, fn_شاخص);
		fn_شاخص++;
	}
	fn_شاخص_max = fn_شاخص;

	fn_شاخص = 2;
	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		fprintf(fptr, "\nfn=(%d)\n", fn_شاخص);
		client_هزینه(fptr, context, fn_شاخص);
		fn_شاخص++;

		report_subscriptions(fptr, context, &fn_شاخص_max);
	}

	fprintf(fptr, "\nfn=(%d) messages\n", fn_شاخص_max);
	fprintf(fptr, "1 0 0 0 0 0 0 0 0 0 0 0\n");

	بنية mosquitto_پایه_پیام *پایه_پیام, *پایه_پیام_tmp;

	HASH_ITER(hh, db.پیام_store, پایه_پیام, پایه_پیام_tmp){
		إذا(پایه_پیام->ref_شمار > 1){
			fprintf(fptr, "cfn=(%d) %" PRIu64 "\n", fn_شاخص_max, پایه_پیام->داده.store_id);
			fprintf(fptr, "calls=%d %d\n", پایه_پیام->ref_شمار, fn_شاخص_max);
			fprintf(fptr, "%d 0 0 0 0 0 0 0 %d 0 0 0\n", fn_شاخص_max, پایه_پیام->ref_شمار);
			fn_شاخص_max++;
		}
	}

	fclose(fptr);
}
#پایان_اگر
