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

#यदि_परिभाषितम् WITH_XTREPORT

/* This file allows reporting of internal parameters to a kcachegrind
 * compatible output file. It is for debugging purposes only and is most likely
 * of no interest to end users.
 */
#अन्तर्भाव "config.h"

#अन्तर्भाव <inttypes.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <uthash.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto_internal.h"
#अन्तर्भाव "net_mosq.h"


स्थिर शून्य client_cost(FILE *fptr, संरचना mosquitto *context, पूर्णाङ्क fn_सूचकांक)
{
	दीर्घ pkt_गणनम्, pkt_अष्टकानि;
	दीर्घ cmsg_गणनम्;
	दीर्घ cmsg_अष्टकानि;
	दीर्घ t_अष्टकानि;

	pkt_गणनम् = 1;
	pkt_अष्टकानि = context->अन्तर्_packet.packet_दीर्घता + context->बहिर्गमनम्_packet_अष्टकानि;

	cmsg_गणनम् = context->msgs_अन्तर्.inflight_गणनम् + context->msgs_अन्तर्.queued_गणनम्;
	cmsg_अष्टकानि = context->msgs_अन्तर्.inflight_अष्टकानि + context->msgs_अन्तर्.queued_अष्टकानि;
	cmsg_गणनम् += context->msgs_बहिर्गमनम्.inflight_गणनम् + context->msgs_बहिर्गमनम्.queued_गणनम्;
	cmsg_अष्टकानि += context->msgs_बहिर्गमनम्.inflight_अष्टकानि + context->msgs_बहिर्गमनम्.queued_अष्टकानि;

	t_अष्टकानि = pkt_अष्टकानि + cmsg_अष्टकानि;
	यदि(context->id){
		t_अष्टकानि += (दीर्घ)वाक्यदैर्घ्यम्(context->id);
	}
	fprintf(fptr, "%d %ld %lu %lu %lu %lu %d 0 0 %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", fn_सूचकांक,
			t_अष्टकानि,
			pkt_गणनम्, cmsg_गणनम्,
			pkt_अष्टकानि, cmsg_अष्टकानि,
			context->sock == INVALID_SOCKET?0:context->sock,
			context->stats.messages_प्रेषितम्,
			context->stats.messages_received,
			context->stats.messages_dropped
			);
}


स्थिर शून्य report_subscriptions(FILE *fptr, संरचना mosquitto *context, पूर्णाङ्क *fn_सूचकांक_max)
{
	प्रति(पूर्णाङ्क i=0; i<context->subs_गणनम्; i++){
		यदि(context->subs && context->subs[i]){
			संरचना mosquitto__subhier *subhier = context->subs[i]->hier;
			पूर्णाङ्क topic_गणनम् = 0;
			वर्ण *topics[100];

			कुरु{
				topics[topic_गणनम्] = subhier->topic;
				subhier = subhier->parent;
				topic_गणनम्++;
				यदि(topic_गणनम् == 100){
					भङ्ग;
				}
			}यावत्(subhier);
			यदि(topic_गणनम् == 100){
				अनुवर्त;
			}

			fprintf(fptr, "cfn=(%d) ", *fn_सूचकांक_max);
			यदि(topics[topic_गणनम्-2] && topics[topic_गणनम्-2][0] == '\0'){
				topic_गणनम्--;
			}
			प्रति(पूर्णाङ्क j=topic_गणनम्-2; j>0; j--){
				यदि(topics[j]){
					fprintf(fptr, "%s/", topics[j]);
				}
			}
			यदि(topics[0]){
				fprintf(fptr, "%s\n", topics[0]);
			}

			fprintf(fptr, "calls=1 %d\n", *fn_सूचकांक_max);
			fprintf(fptr, "1 0 0 0 0 0 0 1 0 0 0 0\n");
			(*fn_सूचकांक_max)++;
		}
	}
}


शून्य xtreport(शून्य)
{
	pid_t pid;
	वर्ण filename[40];
	FILE *fptr;
	संरचना mosquitto *context, *ctxt_tmp;
	पूर्णाङ्क fn_सूचकांक = 2, fn_सूचकांक_max;
	स्थिर पूर्णाङ्क iter = 1;

	pid = getpid();
	snprintf(filename, 40, "/tmp/xtmosquitto.kcg.%d.%d", pid, iter);
	iter++;
	fptr = fopen(filename, "wt");
	यदि(fptr == रिक्त){
		प्रत्यागम;
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

	fn_सूचकांक = 2;
	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		यदि(context->id){
			fprintf(fptr, "cfn=(%d) %s\n", fn_सूचकांक, context->id);
		}अन्यथा{
			fprintf(fptr, "cfn=(%d) unknown\n", fn_सूचकांक);
		}
		fprintf(fptr, "calls=1 %d\n", fn_सूचकांक);
		client_cost(fptr, context, fn_सूचकांक);
		fn_सूचकांक++;
	}
	fn_सूचकांक_max = fn_सूचकांक;

	fn_सूचकांक = 2;
	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		fprintf(fptr, "\nfn=(%d)\n", fn_सूचकांक);
		client_cost(fptr, context, fn_सूचकांक);
		fn_सूचकांक++;

		report_subscriptions(fptr, context, &fn_सूचकांक_max);
	}

	fprintf(fptr, "\nfn=(%d) messages\n", fn_सूचकांक_max);
	fprintf(fptr, "1 0 0 0 0 0 0 0 0 0 0 0\n");

	संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः, *आधारः_सन्देशः_tmp;

	HASH_ITER(hh, db.सन्देशः_store, आधारः_सन्देशः, आधारः_सन्देशः_tmp){
		यदि(आधारः_सन्देशः->ref_गणनम् > 1){
			fprintf(fptr, "cfn=(%d) %" PRIu64 "\n", fn_सूचकांक_max, आधारः_सन्देशः->दत्त.store_id);
			fprintf(fptr, "calls=%d %d\n", आधारः_सन्देशः->ref_गणनम्, fn_सूचकांक_max);
			fprintf(fptr, "%d 0 0 0 0 0 0 0 %d 0 0 0\n", fn_सूचकांक_max, आधारः_सन्देशः->ref_गणनम्);
			fn_सूचकांक_max++;
		}
	}

	fclose(fptr);
}
#यदि_अन्तः
