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

#که_تعریف_شوی WITH_XTREPORT

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


ساكن فراغ client_لګښت(FILE *fptr, بنية mosquitto *context, صحيح fn_شاخص)
{
	اوږد pkt_شمېر, pkt_بایټونه;
	اوږد cmsg_شمېر;
	اوږد cmsg_بایټونه;
	اوږد t_بایټونه;

	pkt_شمېر = 1;
	pkt_بایټونه = context->دننه_packet.packet_اوږدتیا + context->بهر_packet_بایټونه;

	cmsg_شمېر = context->msgs_دننه.inflight_شمېر + context->msgs_دننه.queued_شمېر;
	cmsg_بایټونه = context->msgs_دننه.inflight_بایټونه + context->msgs_دننه.queued_بایټونه;
	cmsg_شمېر += context->msgs_بهر.inflight_شمېر + context->msgs_بهر.queued_شمېر;
	cmsg_بایټونه += context->msgs_بهر.inflight_بایټونه + context->msgs_بهر.queued_بایټونه;

	t_بایټونه = pkt_بایټونه + cmsg_بایټونه;
	إذا(context->id){
		t_بایټونه += (اوږد)مزي_اوږدوالی(context->id);
	}
	fprintf(fptr, "%d %ld %lu %lu %lu %lu %d 0 0 %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", fn_شاخص,
			t_بایټونه,
			pkt_شمېر, cmsg_شمېر,
			pkt_بایټونه, cmsg_بایټونه,
			context->sock == INVALID_SOCKET?0:context->sock,
			context->stats.messages_لېږل_شوی,
			context->stats.messages_received,
			context->stats.messages_dropped
			);
}


ساكن فراغ report_subscriptions(FILE *fptr, بنية mosquitto *context, صحيح *fn_شاخص_max)
{
	لكل(صحيح i=0; i<context->subs_شمېر; i++){
		إذا(context->subs && context->subs[i]){
			بنية mosquitto__subhier *subhier = context->subs[i]->hier;
			صحيح topic_شمېر = 0;
			حرف *topics[100];

			افعل{
				topics[topic_شمېر] = subhier->topic;
				subhier = subhier->پلار;
				topic_شمېر++;
				إذا(topic_شمېر == 100){
					اكسر;
				}
			}طالما(subhier);
			إذا(topic_شمېر == 100){
				استمر;
			}

			fprintf(fptr, "cfn=(%d) ", *fn_شاخص_max);
			إذا(topics[topic_شمېر-2] && topics[topic_شمېر-2][0] == '\0'){
				topic_شمېر--;
			}
			لكل(صحيح j=topic_شمېر-2; j>0; j--){
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
		client_لګښت(fptr, context, fn_شاخص);
		fn_شاخص++;
	}
	fn_شاخص_max = fn_شاخص;

	fn_شاخص = 2;
	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		fprintf(fptr, "\nfn=(%d)\n", fn_شاخص);
		client_لګښت(fptr, context, fn_شاخص);
		fn_شاخص++;

		report_subscriptions(fptr, context, &fn_شاخص_max);
	}

	fprintf(fptr, "\nfn=(%d) messages\n", fn_شاخص_max);
	fprintf(fptr, "1 0 0 0 0 0 0 0 0 0 0 0\n");

	بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام, *بنسټ_پیغام_tmp;

	HASH_ITER(hh, db.پیغام_store, بنسټ_پیغام, بنسټ_پیغام_tmp){
		إذا(بنسټ_پیغام->ref_شمېر > 1){
			fprintf(fptr, "cfn=(%d) %" PRIu64 "\n", fn_شاخص_max, بنسټ_پیغام->معلومات.store_id);
			fprintf(fptr, "calls=%d %d\n", بنسټ_پیغام->ref_شمېر, fn_شاخص_max);
			fprintf(fptr, "%d 0 0 0 0 0 0 0 %d 0 0 0\n", fn_شاخص_max, بنسټ_پیغام->ref_شمېر);
			fn_شاخص_max++;
		}
	}

	fclose(fptr);
}
#د_که_پای
