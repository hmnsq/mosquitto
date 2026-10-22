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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_XTREPORT

/* This file allows reporting of internal parameters to a kcachegrind
 * compatible output file. It is for debugging purposes only and is most likely
 * of no interest to end users.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <inttypes.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uthash.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "net_mosq.h"


ਸਥਿਰ ਖਾਲੀ client_ਲਾਗਤ(FILE *fptr, ਬਣਤਰ mosquitto *context, ਪੂਰਨ fn_ਸੂਚਕਾਂਕ)
{
	ਲੰਮਾ pkt_ਗਿਣਤੀ, pkt_ਬਾਈਟਾਂ;
	ਲੰਮਾ cmsg_ਗਿਣਤੀ;
	ਲੰਮਾ cmsg_ਬਾਈਟਾਂ;
	ਲੰਮਾ t_ਬਾਈਟਾਂ;

	pkt_ਗਿਣਤੀ = 1;
	pkt_ਬਾਈਟਾਂ = context->ਵਿੱਚ_packet.packet_ਲੰਮਾਈ + context->ਬਾਹਰਲਾ_packet_ਬਾਈਟਾਂ;

	cmsg_ਗਿਣਤੀ = context->msgs_ਵਿੱਚ.inflight_ਗਿਣਤੀ + context->msgs_ਵਿੱਚ.queued_ਗਿਣਤੀ;
	cmsg_ਬਾਈਟਾਂ = context->msgs_ਵਿੱਚ.inflight_ਬਾਈਟਾਂ + context->msgs_ਵਿੱਚ.queued_ਬਾਈਟਾਂ;
	cmsg_ਗਿਣਤੀ += context->msgs_ਬਾਹਰਲਾ.inflight_ਗਿਣਤੀ + context->msgs_ਬਾਹਰਲਾ.queued_ਗਿਣਤੀ;
	cmsg_ਬਾਈਟਾਂ += context->msgs_ਬਾਹਰਲਾ.inflight_ਬਾਈਟਾਂ + context->msgs_ਬਾਹਰਲਾ.queued_ਬਾਈਟਾਂ;

	t_ਬਾਈਟਾਂ = pkt_ਬਾਈਟਾਂ + cmsg_ਬਾਈਟਾਂ;
	ਜੇ(context->id){
		t_ਬਾਈਟਾਂ += (ਲੰਮਾ)ਲੜੀ_ਲੰਬਾਈ(context->id);
	}
	fprintf(fptr, "%d %ld %lu %lu %lu %lu %d 0 0 %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", fn_ਸੂਚਕਾਂਕ,
			t_ਬਾਈਟਾਂ,
			pkt_ਗਿਣਤੀ, cmsg_ਗਿਣਤੀ,
			pkt_ਬਾਈਟਾਂ, cmsg_ਬਾਈਟਾਂ,
			context->sock == INVALID_SOCKET?0:context->sock,
			context->stats.messages_ਭੇਜਿਆ,
			context->stats.messages_received,
			context->stats.messages_dropped
			);
}


ਸਥਿਰ ਖਾਲੀ report_subscriptions(FILE *fptr, ਬਣਤਰ mosquitto *context, ਪੂਰਨ *fn_ਸੂਚਕਾਂਕ_max)
{
	ਲਈ(ਪੂਰਨ i=0; i<context->subs_ਗਿਣਤੀ; i++){
		ਜੇ(context->subs && context->subs[i]){
			ਬਣਤਰ mosquitto__subhier *subhier = context->subs[i]->hier;
			ਪੂਰਨ topic_ਗਿਣਤੀ = 0;
			ਅੱਖਰ *topics[100];

			ਕਰੋ{
				topics[topic_ਗਿਣਤੀ] = subhier->topic;
				subhier = subhier->ਮਾਪਾ;
				topic_ਗਿਣਤੀ++;
				ਜੇ(topic_ਗਿਣਤੀ == 100){
					ਰੋਕੋ;
				}
			}ਜਦੋਂ(subhier);
			ਜੇ(topic_ਗਿਣਤੀ == 100){
				ਜਾਰੀ;
			}

			fprintf(fptr, "cfn=(%d) ", *fn_ਸੂਚਕਾਂਕ_max);
			ਜੇ(topics[topic_ਗਿਣਤੀ-2] && topics[topic_ਗਿਣਤੀ-2][0] == '\0'){
				topic_ਗਿਣਤੀ--;
			}
			ਲਈ(ਪੂਰਨ j=topic_ਗਿਣਤੀ-2; j>0; j--){
				ਜੇ(topics[j]){
					fprintf(fptr, "%s/", topics[j]);
				}
			}
			ਜੇ(topics[0]){
				fprintf(fptr, "%s\n", topics[0]);
			}

			fprintf(fptr, "calls=1 %d\n", *fn_ਸੂਚਕਾਂਕ_max);
			fprintf(fptr, "1 0 0 0 0 0 0 1 0 0 0 0\n");
			(*fn_ਸੂਚਕਾਂਕ_max)++;
		}
	}
}


ਖਾਲੀ xtreport(ਖਾਲੀ)
{
	pid_t pid;
	ਅੱਖਰ filename[40];
	FILE *fptr;
	ਬਣਤਰ mosquitto *context, *ctxt_tmp;
	ਪੂਰਨ fn_ਸੂਚਕਾਂਕ = 2, fn_ਸੂਚਕਾਂਕ_max;
	ਸਥਿਰ ਪੂਰਨ iter = 1;

	pid = getpid();
	snprintf(filename, 40, "/tmp/xtmosquitto.kcg.%d.%d", pid, iter);
	iter++;
	fptr = fopen(filename, "wt");
	ਜੇ(fptr == ਨਿਰਾ){
		ਵਾਪਸ;
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

	fn_ਸੂਚਕਾਂਕ = 2;
	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		ਜੇ(context->id){
			fprintf(fptr, "cfn=(%d) %s\n", fn_ਸੂਚਕਾਂਕ, context->id);
		}ਨਹੀਂਤਾਂ{
			fprintf(fptr, "cfn=(%d) unknown\n", fn_ਸੂਚਕਾਂਕ);
		}
		fprintf(fptr, "calls=1 %d\n", fn_ਸੂਚਕਾਂਕ);
		client_ਲਾਗਤ(fptr, context, fn_ਸੂਚਕਾਂਕ);
		fn_ਸੂਚਕਾਂਕ++;
	}
	fn_ਸੂਚਕਾਂਕ_max = fn_ਸੂਚਕਾਂਕ;

	fn_ਸੂਚਕਾਂਕ = 2;
	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		fprintf(fptr, "\nfn=(%d)\n", fn_ਸੂਚਕਾਂਕ);
		client_ਲਾਗਤ(fptr, context, fn_ਸੂਚਕਾਂਕ);
		fn_ਸੂਚਕਾਂਕ++;

		report_subscriptions(fptr, context, &fn_ਸੂਚਕਾਂਕ_max);
	}

	fprintf(fptr, "\nfn=(%d) messages\n", fn_ਸੂਚਕਾਂਕ_max);
	fprintf(fptr, "1 0 0 0 0 0 0 0 0 0 0 0\n");

	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ, *ਅਧਾਰ_ਸੁਨੇਹਾ_tmp;

	HASH_ITER(hh, db.ਸੁਨੇਹਾ_store, ਅਧਾਰ_ਸੁਨੇਹਾ, ਅਧਾਰ_ਸੁਨੇਹਾ_tmp){
		ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ref_ਗਿਣਤੀ > 1){
			fprintf(fptr, "cfn=(%d) %" PRIu64 "\n", fn_ਸੂਚਕਾਂਕ_max, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.store_id);
			fprintf(fptr, "calls=%d %d\n", ਅਧਾਰ_ਸੁਨੇਹਾ->ref_ਗਿਣਤੀ, fn_ਸੂਚਕਾਂਕ_max);
			fprintf(fptr, "%d 0 0 0 0 0 0 0 %d 0 0 0\n", fn_ਸੂਚਕਾਂਕ_max, ਅਧਾਰ_ਸੁਨੇਹਾ->ref_ਗਿਣਤੀ);
			fn_ਸੂਚਕਾਂਕ_max++;
		}
	}

	fclose(fptr);
}
#ਜੇ_ਅੰਤ
