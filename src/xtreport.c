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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_XTREPORT

/* This file allows reporting of internal parameters to a kcachegrind
 * compatible output file. It is for debugging purposes only and is most likely
 * of no interest to end users.
 */
#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <inttypes.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <uthash.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_internal.h"
#ಅನ್ತರ್ಭಾವ "net_mosq.h"


ಸ್ಥಿರ ಶೂನ್ಯ client_ವೆಚ್ಚ(FILE *fptr, ರಚನೆ mosquitto *context, ಪೂರ್ಣಾಂಕ fn_ಸೂಚ್ಯಂಕ)
{
	ದೀರ್ಘ pkt_ಎಣಿಕೆ, pkt_ಬೈಟುಗಳು;
	ದೀರ್ಘ cmsg_ಎಣಿಕೆ;
	ದೀರ್ಘ cmsg_ಬೈಟುಗಳು;
	ದೀರ್ಘ t_ಬೈಟುಗಳು;

	pkt_ಎಣಿಕೆ = 1;
	pkt_ಬೈಟುಗಳು = context->ಒಳ_packet.packet_ಉದ್ದಳತೆ + context->ಹೊರಹೋಗು_packet_ಬೈಟುಗಳು;

	cmsg_ಎಣಿಕೆ = context->msgs_ಒಳ.inflight_ಎಣಿಕೆ + context->msgs_ಒಳ.queued_ಎಣಿಕೆ;
	cmsg_ಬೈಟುಗಳು = context->msgs_ಒಳ.inflight_ಬೈಟುಗಳು + context->msgs_ಒಳ.queued_ಬೈಟುಗಳು;
	cmsg_ಎಣಿಕೆ += context->msgs_ಹೊರಹೋಗು.inflight_ಎಣಿಕೆ + context->msgs_ಹೊರಹೋಗು.queued_ಎಣಿಕೆ;
	cmsg_ಬೈಟುಗಳು += context->msgs_ಹೊರಹೋಗು.inflight_ಬೈಟುಗಳು + context->msgs_ಹೊರಹೋಗು.queued_ಬೈಟುಗಳು;

	t_ಬೈಟುಗಳು = pkt_ಬೈಟುಗಳು + cmsg_ಬೈಟುಗಳು;
	ಆದರೆ(context->id){
		t_ಬೈಟುಗಳು += (ದೀರ್ಘ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->id);
	}
	fprintf(fptr, "%d %ld %lu %lu %lu %lu %d 0 0 %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", fn_ಸೂಚ್ಯಂಕ,
			t_ಬೈಟುಗಳು,
			pkt_ಎಣಿಕೆ, cmsg_ಎಣಿಕೆ,
			pkt_ಬೈಟುಗಳು, cmsg_ಬೈಟುಗಳು,
			context->sock == INVALID_SOCKET?0:context->sock,
			context->stats.messages_ಕಳುಹಿಸಿದ,
			context->stats.messages_received,
			context->stats.messages_dropped
			);
}


ಸ್ಥಿರ ಶೂನ್ಯ report_subscriptions(FILE *fptr, ರಚನೆ mosquitto *context, ಪೂರ್ಣಾಂಕ *fn_ಸೂಚ್ಯಂಕ_max)
{
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<context->subs_ಎಣಿಕೆ; i++){
		ಆದರೆ(context->subs && context->subs[i]){
			ರಚನೆ mosquitto__subhier *subhier = context->subs[i]->hier;
			ಪೂರ್ಣಾಂಕ topic_ಎಣಿಕೆ = 0;
			ಅಕ್ಷರ *topics[100];

			ಮಾಡು{
				topics[topic_ಎಣಿಕೆ] = subhier->topic;
				subhier = subhier->ಪೋಷಕ;
				topic_ಎಣಿಕೆ++;
				ಆದರೆ(topic_ಎಣಿಕೆ == 100){
					ನಿಲ್ಲಿಸು;
				}
			}ಆಗ(subhier);
			ಆದರೆ(topic_ಎಣಿಕೆ == 100){
				ಮುಂದುವರಿ;
			}

			fprintf(fptr, "cfn=(%d) ", *fn_ಸೂಚ್ಯಂಕ_max);
			ಆದರೆ(topics[topic_ಎಣಿಕೆ-2] && topics[topic_ಎಣಿಕೆ-2][0] == '\0'){
				topic_ಎಣಿಕೆ--;
			}
			ಗಾಗಿ(ಪೂರ್ಣಾಂಕ j=topic_ಎಣಿಕೆ-2; j>0; j--){
				ಆದರೆ(topics[j]){
					fprintf(fptr, "%s/", topics[j]);
				}
			}
			ಆದರೆ(topics[0]){
				fprintf(fptr, "%s\n", topics[0]);
			}

			fprintf(fptr, "calls=1 %d\n", *fn_ಸೂಚ್ಯಂಕ_max);
			fprintf(fptr, "1 0 0 0 0 0 0 1 0 0 0 0\n");
			(*fn_ಸೂಚ್ಯಂಕ_max)++;
		}
	}
}


ಶೂನ್ಯ xtreport(ಶೂನ್ಯ)
{
	pid_t pid;
	ಅಕ್ಷರ filename[40];
	FILE *fptr;
	ರಚನೆ mosquitto *context, *ctxt_tmp;
	ಪೂರ್ಣಾಂಕ fn_ಸೂಚ್ಯಂಕ = 2, fn_ಸೂಚ್ಯಂಕ_max;
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ iter = 1;

	pid = getpid();
	snprintf(filename, 40, "/tmp/xtmosquitto.kcg.%d.%d", pid, iter);
	iter++;
	fptr = fopen(filename, "wt");
	ಆದರೆ(fptr == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ;
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

	fn_ಸೂಚ್ಯಂಕ = 2;
	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		ಆದರೆ(context->id){
			fprintf(fptr, "cfn=(%d) %s\n", fn_ಸೂಚ್ಯಂಕ, context->id);
		}ಇಲ್ಲದಿದ್ದರೆ{
			fprintf(fptr, "cfn=(%d) unknown\n", fn_ಸೂಚ್ಯಂಕ);
		}
		fprintf(fptr, "calls=1 %d\n", fn_ಸೂಚ್ಯಂಕ);
		client_ವೆಚ್ಚ(fptr, context, fn_ಸೂಚ್ಯಂಕ);
		fn_ಸೂಚ್ಯಂಕ++;
	}
	fn_ಸೂಚ್ಯಂಕ_max = fn_ಸೂಚ್ಯಂಕ;

	fn_ಸೂಚ್ಯಂಕ = 2;
	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		fprintf(fptr, "\nfn=(%d)\n", fn_ಸೂಚ್ಯಂಕ);
		client_ವೆಚ್ಚ(fptr, context, fn_ಸೂಚ್ಯಂಕ);
		fn_ಸೂಚ್ಯಂಕ++;

		report_subscriptions(fptr, context, &fn_ಸೂಚ್ಯಂಕ_max);
	}

	fprintf(fptr, "\nfn=(%d) messages\n", fn_ಸೂಚ್ಯಂಕ_max);
	fprintf(fptr, "1 0 0 0 0 0 0 0 0 0 0 0\n");

	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ, *ತಳ_ಸಂದೇಶ_tmp;

	HASH_ITER(hh, db.ಸಂದೇಶ_store, ತಳ_ಸಂದೇಶ, ತಳ_ಸಂದೇಶ_tmp){
		ಆದರೆ(ತಳ_ಸಂದೇಶ->ref_ಎಣಿಕೆ > 1){
			fprintf(fptr, "cfn=(%d) %" PRIu64 "\n", fn_ಸೂಚ್ಯಂಕ_max, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.store_id);
			fprintf(fptr, "calls=%d %d\n", ತಳ_ಸಂದೇಶ->ref_ಎಣಿಕೆ, fn_ಸೂಚ್ಯಂಕ_max);
			fprintf(fptr, "%d 0 0 0 0 0 0 0 %d 0 0 0\n", fn_ಸೂಚ್ಯಂಕ_max, ತಳ_ಸಂದೇಶ->ref_ಎಣಿಕೆ);
			fn_ಸೂಚ್ಯಂಕ_max++;
		}
	}

	fclose(fptr);
}
#ಆದರೆ_ಅಂತ್ಯ
