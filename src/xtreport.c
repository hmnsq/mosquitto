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

#నిర్వచితమైతే WITH_XTREPORT

/* This file allows reporting of internal parameters to a kcachegrind
 * compatible output file. It is for debugging purposes only and is most likely
 * of no interest to end users.
 */
#అన్తర్భావ "config.h"

#అన్తర్భావ <inttypes.h>
#అన్తర్భావ <stdint.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <uthash.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto_internal.h"
#అన్తర్భావ "net_mosq.h"


స్థిర శూన్య client_ఖర్చు(FILE *fptr, నిర్మాణం mosquitto *context, పూర్ణాంకం fn_సూచీ)
{
	దీర్ఘ pkt_లెక్క, pkt_బైట్లు;
	దీర్ఘ cmsg_లెక్క;
	దీర్ఘ cmsg_బైట్లు;
	దీర్ఘ t_బైట్లు;

	pkt_లెక్క = 1;
	pkt_బైట్లు = context->లోపల_packet.packet_నిడివి + context->బహిర్గతం_packet_బైట్లు;

	cmsg_లెక్క = context->msgs_లోపల.inflight_లెక్క + context->msgs_లోపల.queued_లెక్క;
	cmsg_బైట్లు = context->msgs_లోపల.inflight_బైట్లు + context->msgs_లోపల.queued_బైట్లు;
	cmsg_లెక్క += context->msgs_బహిర్గతం.inflight_లెక్క + context->msgs_బహిర్గతం.queued_లెక్క;
	cmsg_బైట్లు += context->msgs_బహిర్గతం.inflight_బైట్లు + context->msgs_బహిర్గతం.queued_బైట్లు;

	t_బైట్లు = pkt_బైట్లు + cmsg_బైట్లు;
	ఒకవేళ(context->id){
		t_బైట్లు += (దీర్ఘ)అక్షరశ్రేణి_పొడవు(context->id);
	}
	fprintf(fptr, "%d %ld %lu %lu %lu %lu %d 0 0 %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", fn_సూచీ,
			t_బైట్లు,
			pkt_లెక్క, cmsg_లెక్క,
			pkt_బైట్లు, cmsg_బైట్లు,
			context->sock == INVALID_SOCKET?0:context->sock,
			context->stats.messages_పంపిన,
			context->stats.messages_received,
			context->stats.messages_dropped
			);
}


స్థిర శూన్య report_subscriptions(FILE *fptr, నిర్మాణం mosquitto *context, పూర్ణాంకం *fn_సూచీ_max)
{
	కొరకు(పూర్ణాంకం i=0; i<context->subs_లెక్క; i++){
		ఒకవేళ(context->subs && context->subs[i]){
			నిర్మాణం mosquitto__subhier *subhier = context->subs[i]->hier;
			పూర్ణాంకం topic_లెక్క = 0;
			అక్షరం *topics[100];

			చేయి{
				topics[topic_లెక్క] = subhier->topic;
				subhier = subhier->మాతృ;
				topic_లెక్క++;
				ఒకవేళ(topic_లెక్క == 100){
					ఆపు;
				}
			}అయితే(subhier);
			ఒకవేళ(topic_లెక్క == 100){
				కొనసాగు;
			}

			fprintf(fptr, "cfn=(%d) ", *fn_సూచీ_max);
			ఒకవేళ(topics[topic_లెక్క-2] && topics[topic_లెక్క-2][0] == '\0'){
				topic_లెక్క--;
			}
			కొరకు(పూర్ణాంకం j=topic_లెక్క-2; j>0; j--){
				ఒకవేళ(topics[j]){
					fprintf(fptr, "%s/", topics[j]);
				}
			}
			ఒకవేళ(topics[0]){
				fprintf(fptr, "%s\n", topics[0]);
			}

			fprintf(fptr, "calls=1 %d\n", *fn_సూచీ_max);
			fprintf(fptr, "1 0 0 0 0 0 0 1 0 0 0 0\n");
			(*fn_సూచీ_max)++;
		}
	}
}


శూన్య xtreport(శూన్య)
{
	pid_t pid;
	అక్షరం filename[40];
	FILE *fptr;
	నిర్మాణం mosquitto *context, *ctxt_tmp;
	పూర్ణాంకం fn_సూచీ = 2, fn_సూచీ_max;
	స్థిర పూర్ణాంకం iter = 1;

	pid = getpid();
	snprintf(filename, 40, "/tmp/xtmosquitto.kcg.%d.%d", pid, iter);
	iter++;
	fptr = fopen(filename, "wt");
	ఒకవేళ(fptr == ఖాళీ){
		తిరిగి;
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

	fn_సూచీ = 2;
	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		ఒకవేళ(context->id){
			fprintf(fptr, "cfn=(%d) %s\n", fn_సూచీ, context->id);
		}లేకపోతే{
			fprintf(fptr, "cfn=(%d) unknown\n", fn_సూచీ);
		}
		fprintf(fptr, "calls=1 %d\n", fn_సూచీ);
		client_ఖర్చు(fptr, context, fn_సూచీ);
		fn_సూచీ++;
	}
	fn_సూచీ_max = fn_సూచీ;

	fn_సూచీ = 2;
	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		fprintf(fptr, "\nfn=(%d)\n", fn_సూచీ);
		client_ఖర్చు(fptr, context, fn_సూచీ);
		fn_సూచీ++;

		report_subscriptions(fptr, context, &fn_సూచీ_max);
	}

	fprintf(fptr, "\nfn=(%d) messages\n", fn_సూచీ_max);
	fprintf(fptr, "1 0 0 0 0 0 0 0 0 0 0 0\n");

	నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం, *పునాది_సందేశం_tmp;

	HASH_ITER(hh, db.సందేశం_store, పునాది_సందేశం, పునాది_సందేశం_tmp){
		ఒకవేళ(పునాది_సందేశం->ref_లెక్క > 1){
			fprintf(fptr, "cfn=(%d) %" PRIu64 "\n", fn_సూచీ_max, పునాది_సందేశం->దత్తాంశం.store_id);
			fprintf(fptr, "calls=%d %d\n", పునాది_సందేశం->ref_లెక్క, fn_సూచీ_max);
			fprintf(fptr, "%d 0 0 0 0 0 0 0 %d 0 0 0\n", fn_సూచీ_max, పునాది_సందేశం->ref_లెక్క);
			fn_సూచీ_max++;
		}
	}

	fclose(fptr);
}
#అయితే_ముగింపు
