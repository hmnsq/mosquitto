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
#अन्तर्भाव "config.h"

#अन्तर्भाव <inttypes.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <uthash.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto_internal.h"
#अन्तर्भाव "net_mosq.h"


स्थिर शून्य client_cost(खाता *fptr, संरचना mosquitto *context, पूर्णांक fn_index)
{
	दीर्घ pkt_मोजणी, pkt_bytes;
	दीर्घ cmsg_मोजणी;
	दीर्घ cmsg_bytes;
	दीर्घ tBytes;

	pkt_मोजणी = 1;
	pkt_bytes = context->in_packet.packet_length + context->out_packet_bytes;

	cmsg_मोजणी = context->msgs_in.inflight_मोजणी + context->msgs_in.queued_मोजणी;
	cmsg_bytes = context->msgs_in.inflight_bytes + context->msgs_in.queued_bytes;
	cmsg_मोजणी += context->msgs_out.inflight_मोजणी + context->msgs_out.queued_मोजणी;
	cmsg_bytes += context->msgs_out.inflight_bytes + context->msgs_out.queued_bytes;

	tBytes = pkt_bytes + cmsg_bytes;
	जर(context->id){
		tBytes += (दीर्घ)म_माप(context->id);
	}
	ख_लिखो(fptr, "%d %ld %lu %lu %lu %lu %d 0 0 %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", fn_index,
			tBytes,
			pkt_मोजणी, cmsg_मोजणी,
			pkt_bytes, cmsg_bytes,
			context->sock == INVALID_SOCKET?0:context->sock,
			context->stats.messages_sent,
			context->stats.messages_received,
			context->stats.messages_dropped
			);
}


स्थिर शून्य report_subscriptions(खाता *fptr, संरचना mosquitto *context, पूर्णांक *fn_index_max)
{
	साठी(पूर्णांक i=0; i<context->subs_मोजणी; i++){
		जर(context->subs && context->subs[i]){
			संरचना mosquitto__subhier *subhier = context->subs[i]->hier;
			पूर्णांक topic_मोजणी = 0;
			अक्षर *topics[100];

			करा{
				topics[topic_मोजणी] = subhier->topic;
				subhier = subhier->parent;
				topic_मोजणी++;
				जर(topic_मोजणी == 100){
					थांबा;
				}
			}पर्यंत(subhier);
			जर(topic_मोजणी == 100){
				सुरूठेवा;
			}

			ख_लिखो(fptr, "cfn=(%d) ", *fn_index_max);
			जर(topics[topic_मोजणी-2] && topics[topic_मोजणी-2][0] == '\0'){
				topic_मोजणी--;
			}
			साठी(पूर्णांक j=topic_मोजणी-2; j>0; j--){
				जर(topics[j]){
					ख_लिखो(fptr, "%s/", topics[j]);
				}
			}
			जर(topics[0]){
				ख_लिखो(fptr, "%s\n", topics[0]);
			}

			ख_लिखो(fptr, "calls=1 %d\n", *fn_index_max);
			ख_लिखो(fptr, "1 0 0 0 0 0 0 1 0 0 0 0\n");
			(*fn_index_max)++;
		}
	}
}


शून्य xtreport(शून्य)
{
	pid_t pid;
	अक्षर filename[40];
	खाता *fptr;
	संरचना mosquitto *context, *ctxt_tmp;
	पूर्णांक fn_index = 2, fn_index_max;
	स्थिर पूर्णांक iter = 1;

	pid = getpid();
	snprintf(filename, 40, "/tmp/xtmosquitto.kcg.%d.%d", pid, iter);
	iter++;
	fptr = ख_खोलो(filename, "wt");
	जर(fptr == रिक्त){
		परत;
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

	fn_index = 2;
	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		जर(context->id){
			ख_लिखो(fptr, "cfn=(%d) %s\n", fn_index, context->id);
		}अन्यथा{
			ख_लिखो(fptr, "cfn=(%d) unknown\n", fn_index);
		}
		ख_लिखो(fptr, "calls=1 %d\n", fn_index);
		client_cost(fptr, context, fn_index);
		fn_index++;
	}
	fn_index_max = fn_index;

	fn_index = 2;
	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		ख_लिखो(fptr, "\nfn=(%d)\n", fn_index);
		client_cost(fptr, context, fn_index);
		fn_index++;

		report_subscriptions(fptr, context, &fn_index_max);
	}

	ख_लिखो(fptr, "\nfn=(%d) messages\n", fn_index_max);
	ख_लिखो(fptr, "1 0 0 0 0 0 0 0 0 0 0 0\n");

	संरचना mosquitto_base_संदेश *base_संदेश, *base_संदेश_tmp;

	HASH_ITER(hh, db.संदेश_store, base_संदेश, base_संदेश_tmp){
		जर(base_संदेश->ref_मोजणी > 1){
			ख_लिखो(fptr, "cfn=(%d) %" PRIu64 "\n", fn_index_max, base_संदेश->data.store_id);
			ख_लिखो(fptr, "calls=%d %d\n", base_संदेश->ref_मोजणी, fn_index_max);
			ख_लिखो(fptr, "%d 0 0 0 0 0 0 0 %d 0 0 0\n", fn_index_max, base_संदेश->ref_मोजणी);
			fn_index_max++;
		}
	}

	ख_बंद(fptr);
}
#endif
