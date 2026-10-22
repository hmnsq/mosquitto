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

#अगर_घोषित WITH_XTREPORT

/* This file allows reporting of internal parameters to a kcachegrind
 * compatible output file. It is for debugging purposes only and is most likely
 * of no interest to end users.
 */
#समावेश "config.h"

#समावेश <inttypes.h>
#समावेश <stdint.h>
#समावेश <stdio.h>
#समावेश <uthash.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto_internal.h"
#समावेश "net_mosq.h"


अटल व्योम client_cost(खाता *fptr, काष्ठा mosquitto *context, पूर्णांक fn_सूचकांक)
{
	दीर्घ pkt_गिनती, pkt_bytes;
	दीर्घ cmsg_गिनती;
	दीर्घ cmsg_bytes;
	दीर्घ tBytes;

	pkt_गिनती = 1;
	pkt_bytes = context->in_packet.packet_लंबाई + context->out_packet_bytes;

	cmsg_गिनती = context->msgs_in.inflight_गिनती + context->msgs_in.queued_गिनती;
	cmsg_bytes = context->msgs_in.inflight_bytes + context->msgs_in.queued_bytes;
	cmsg_गिनती += context->msgs_out.inflight_गिनती + context->msgs_out.queued_गिनती;
	cmsg_bytes += context->msgs_out.inflight_bytes + context->msgs_out.queued_bytes;

	tBytes = pkt_bytes + cmsg_bytes;
	अगर(context->id){
		tBytes += (दीर्घ)म_माप(context->id);
	}
	ख_लिखो(fptr, "%d %ld %lu %lu %lu %lu %d 0 0 %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", fn_सूचकांक,
			tBytes,
			pkt_गिनती, cmsg_गिनती,
			pkt_bytes, cmsg_bytes,
			context->sock == INVALID_SOCKET?0:context->sock,
			context->stats.messages_sent,
			context->stats.messages_received,
			context->stats.messages_dropped
			);
}


अटल व्योम report_subscriptions(खाता *fptr, काष्ठा mosquitto *context, पूर्णांक *fn_सूचकांक_max)
{
	क्रम(पूर्णांक i=0; i<context->subs_गिनती; i++){
		अगर(context->subs && context->subs[i]){
			काष्ठा mosquitto__subhier *subhier = context->subs[i]->hier;
			पूर्णांक topic_गिनती = 0;
			अक्षर *topics[100];

			करो{
				topics[topic_गिनती] = subhier->topic;
				subhier = subhier->parent;
				topic_गिनती++;
				अगर(topic_गिनती == 100){
					अवरोध;
				}
			}जबतक(subhier);
			अगर(topic_गिनती == 100){
				जारी;
			}

			ख_लिखो(fptr, "cfn=(%d) ", *fn_सूचकांक_max);
			अगर(topics[topic_गिनती-2] && topics[topic_गिनती-2][0] == '\0'){
				topic_गिनती--;
			}
			क्रम(पूर्णांक j=topic_गिनती-2; j>0; j--){
				अगर(topics[j]){
					ख_लिखो(fptr, "%s/", topics[j]);
				}
			}
			अगर(topics[0]){
				ख_लिखो(fptr, "%s\n", topics[0]);
			}

			ख_लिखो(fptr, "calls=1 %d\n", *fn_सूचकांक_max);
			ख_लिखो(fptr, "1 0 0 0 0 0 0 1 0 0 0 0\n");
			(*fn_सूचकांक_max)++;
		}
	}
}


व्योम xtreport(व्योम)
{
	pid_t pid;
	अक्षर filename[40];
	खाता *fptr;
	काष्ठा mosquitto *context, *ctxt_tmp;
	पूर्णांक fn_सूचकांक = 2, fn_सूचकांक_max;
	अटल पूर्णांक iter = 1;

	pid = getpid();
	snprintf(filename, 40, "/tmp/xtmosquitto.kcg.%d.%d", pid, iter);
	iter++;
	fptr = ख_खोलो(filename, "wt");
	अगर(fptr == शून्य){
		वापस;
	}

	ख_लिखो(fptr, "# callgrind format\n");
	ख_लिखो(fptr, "version: 1\n");
	ख_लिखो(fptr, "creator: mosquitto\n");
	ख_लिखो(fptr, "pid: %d\n", pid);
	ख_लिखो(fptr, "cmd: mosquitto\n\n");

	ख_लिखो(fptr, "positions: line\n");
	ख_लिखो(fptr, "event: tB : total bytes\n");
	ख_लिखो(fptr, "event: pkt : currently queued packets\n");
	ख_लिखो(fptr, "event: cmsg : currently pending client messages\n");
	ख_लिखो(fptr, "event: pktB : currently queued packet bytes\n");
	ख_लिखो(fptr, "event: cmsgB : currently pending client message bytes\n");
	ख_लिखो(fptr, "event: sock : client socket number\n");
	ख_लिखो(fptr, "event: sub : client subscriptions\n");
	ख_लिखो(fptr, "event: refc : message store ref counts\n");
	ख_लिखो(fptr, "event: msgS : client message sent count\n");
	ख_लिखो(fptr, "event: msgR : client message received count\n");
	ख_लिखो(fptr, "event: msgD : client message dropped count\n");
	ख_लिखो(fptr, "events: tB pkt cmsg pktB cmsgB sock sub refc msgS msgR msgD\n");

	ख_लिखो(fptr, "\nfn=(1) clients\n");
	ख_लिखो(fptr, "1 0 0 0 0 0 0 0 0 0 0 0\n");

	fn_सूचकांक = 2;
	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		अगर(context->id){
			ख_लिखो(fptr, "cfn=(%d) %s\n", fn_सूचकांक, context->id);
		}अन्यथा{
			ख_लिखो(fptr, "cfn=(%d) unknown\n", fn_सूचकांक);
		}
		ख_लिखो(fptr, "calls=1 %d\n", fn_सूचकांक);
		client_cost(fptr, context, fn_सूचकांक);
		fn_सूचकांक++;
	}
	fn_सूचकांक_max = fn_सूचकांक;

	fn_सूचकांक = 2;
	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		ख_लिखो(fptr, "\nfn=(%d)\n", fn_सूचकांक);
		client_cost(fptr, context, fn_सूचकांक);
		fn_सूचकांक++;

		report_subscriptions(fptr, context, &fn_सूचकांक_max);
	}

	ख_लिखो(fptr, "\nfn=(%d) messages\n", fn_सूचकांक_max);
	ख_लिखो(fptr, "1 0 0 0 0 0 0 0 0 0 0 0\n");

	काष्ठा mosquitto_base_संदेश *base_संदेश, *base_संदेश_tmp;

	HASH_ITER(hh, db.संदेश_store, base_संदेश, base_संदेश_tmp){
		अगर(base_संदेश->ref_गिनती > 1){
			ख_लिखो(fptr, "cfn=(%d) %" PRIu64 "\n", fn_सूचकांक_max, base_संदेश->तथ्य.store_id);
			ख_लिखो(fptr, "calls=%d %d\n", base_संदेश->ref_गिनती, fn_सूचकांक_max);
			ख_लिखो(fptr, "%d 0 0 0 0 0 0 0 %d 0 0 0\n", fn_सूचकांक_max, base_संदेश->ref_गिनती);
			fn_सूचकांक_max++;
		}
	}

	ख_बंद(fptr);
}
#पूर्ण_अगर
