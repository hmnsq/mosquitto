/*
Copyright (c) 2009-2021 Roger Light <roger@atchoo.org>

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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <time.h>
#ଯଦି defined(__APPLE__) || defined(_AIX)
#ଅନ୍ତର୍ଭାଵ <sys/socket.h>
#endif

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "alias_mosq.h"
#ଅନ୍ତର୍ଭାଵ "packet_mosq.h"
#ଅନ୍ତର୍ଭାଵ "property_mosq.h"
#ଅନ୍ତର୍ଭାଵ "sys_tree.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"
#ଅନ୍ତର୍ଭାଵ "will_mosq.h"

#ଅନ୍ତର୍ଭାଵ "uthash.h"


ପୂର୍ଣ୍ଣସଂଖ୍ୟା context__init_sock(ସଂରଚନା mosquitto *context, mosq_sock_t sock, bool get_address)
{
	context->sock = sock;

	ଯଦି((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)context->sock >= 0){
		ଯଦି(get_address){
			ଅକ୍ଷର address[1024];

			ଯଦି(!net__socket_get_address(context->sock,
					address, ଆକାର(address),
					&context->remote_port)){

				context->address = mosquitto_strdup(address);
			}
			ଯଦି(!context->address){
				/* getpeername and inet_ntop failed and not a bridge */
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
		}
		HASH_ADD(hh_sock, db.contexts_by_sock, sock, ଆକାର(context->sock), context);
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}

ସଂରଚନା mosquitto *context__init(ଶୂନ୍ୟ)
{
	ସଂରଚନା mosquitto *context;

	context = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto));
	ଯଦି(!context){
		ଫେରନ୍ତୁ ଖାଲି;
	}

	context->in_packet.packet_buffer_size = db.config->packet_buffer_size;
	context->in_packet.packet_buffer = mosquitto_calloc(1, context->in_packet.packet_buffer_size);
	ଯଦି(!context->in_packet.packet_buffer){
		mosquitto_FREE(context);
		ଫେରନ୍ତୁ ଖାଲି;
	}

#ଯଦି defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#ଅନ୍ୟଥା
	context->pollfd_index = -1;
#endif
	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_ସନ୍ଦେଶ_in = db.now_s;
	context->next_ସନ୍ଦେଶ_out = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_start = true;
	context->id = ଖାଲି;
	context->last_mid = 0;
	context->will = ଖାଲି;
	context->username = ଖାଲି;
	context->password = ଖାଲି;
	context->listener = ଖାଲି;
	context->acl_list = ଖାଲି;
	context->retain_available = true;
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	memset(&context->wsd, 0, ଆକାର(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.mask = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#endif

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->in_packet.payload = ଖାଲି;
	packet__cleanup(&context->in_packet);
	context->out_packet = ଖାଲି;
	context->out_packet_ଗଣନା = 0;
	context->out_packet_bytes = 0;

	context->address = ଖାଲି;
	context->bridge = ଖାଲି;
	context->msgs_in.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_in.inflight_quota = db.config->max_inflight_messages;
	context->msgs_out.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_out.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#ifdef WITH_TLS
	context->ssl = ଖାଲି;
#endif

	ଫେରନ୍ତୁ context;
}


ସ୍ଥିର ଶୂନ୍ୟ context__cleanup_out_packets(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା mosquitto__packet *packet;

	ଯଦି(!context){
		ଫେରନ୍ତୁ;
	}

	ଯେତେବେଳେ(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_ଗଣନା);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_ଗଣନା = 0;
	context->out_packet_bytes = 0;
}


/*
 * This will result in any outgoing packets going unsent. If we're disconnected
 * forcefully then it is usually an error condition and shouldn't be a problem,
 * but it will mean that CONNACK messages will never get sent for bad protocol
 * versions for example.
 */
ଶୂନ୍ୟ context__cleanup(ସଂରଚନା mosquitto *context, bool force_free)
{
	ଯଦି(!context){
		ଫେରନ୍ତୁ;
	}

	ଯଦି(force_free){
		context->clean_start = true;
	}

#ifdef WITH_BRIDGE
	ଯଦି(context->bridge){
		bridge__cleanup(context);
	}
#endif
	mosquitto_FREE(context->in_packet.packet_buffer);
	context->in_packet.packet_buffer_size = 0;

	alias__free_all(context);
	keepalive__remove(context);
	context__cleanup_out_packets(context);

	mosquitto_FREE(context->auth_method);
	mosquitto_FREE(context->username);
	mosquitto_FREE(context->password);

	net__socket_close(context);
	ଯଦି(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->address);

	context__send_will(context);

	ଯଦି(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->in_packet));
	context__cleanup_out_packets(context);
#ଯଦି defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	ଯଦି(context->adns){
		gai_cancel(context->adns);
		ସଂରଚନା addrinfo *ar_request = (ସଂରଚନା addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#endif

#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#endif
	mosquitto_FREE(context->proxy.buf);
	ଯଦି(force_free){
		mosquitto_FREE(context);
	}
}


ଶୂନ୍ୟ context__send_will(ସଂରଚନା mosquitto *ctxt)
{
	ଯଦି(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		ଯଦି(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay__add(ctxt);
			ଫେରନ୍ତୁ;
		}

		ଯଦି(mosquitto_acl_check(ctxt,
				ctxt->will->ସନ୍ଦେଶ.topic,
				(uint32_t)ctxt->will->ସନ୍ଦେଶ.payloadlen,
				ctxt->will->ସନ୍ଦେଶ.payload,
				(uint8_t)ctxt->will->ସନ୍ଦେଶ.qos,
				ctxt->will->ସନ୍ଦେଶ.retain,
				ctxt->will->properties,
				MOSQ_ACL_WRITE) == MOSQ_ERR_SUCCESS){

			/* Unexpected disconnect, queue the client will. */
			db__messages_easy_queue(ctxt,
					ctxt->will->ସନ୍ଦେଶ.topic,
					(uint8_t)ctxt->will->ସନ୍ଦେଶ.qos,
					(uint32_t)ctxt->will->ସନ୍ଦେଶ.payloadlen,
					ctxt->will->ସନ୍ଦେଶ.payload,
					ctxt->will->ସନ୍ଦେଶ.retain,
					ctxt->will->expiry_interval,
					&ctxt->will->properties);
		}
	}
	will__clear(ctxt);
}


ଶୂନ୍ୟ context__disconnect(ସଂରଚନା mosquitto *context, ପୂର୍ଣ୍ଣସଂଖ୍ୟା reason)
{
	ଯଦି(mosquitto__get_state(context) == mosq_cs_disconnected){
		ଫେରନ୍ତୁ;
	}

#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	ଯଦି(context->transport == mosq_t_ws){
		uint8_t buf[4] = {0x88, 0x02, 0x03, context->wsd.disconnect_reason};
		/* Send the disconnect reason, but don't care if it fails */
		ଯଦି(send(context->sock, buf, 4, 0)){
		}
	}
#endif
	ଯଦି(context->id){
		ସଂରଚନା mosquitto *context_found;
		HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
		ଯଦି(context_found == context){
			net__socket_close(context);
			context__add_to_disused(context);
			ଫେରନ୍ତୁ;
		}
	}

	context__send_will(context);

	ଯଦି(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
		plugin__handle_disconnect(context, reason);
	}ଅନ୍ୟଥା{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#ifdef WITH_BRIDGE
	ଯଦି(context->bridge == ଖାଲି)
	/* Outgoing bridge connection never expire */
#endif
	{
		ଯଦି(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			ଯଦି(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context__add_to_disused(context);
			}
		}ଅନ୍ୟଥା{
			session_expiry__add(context);
		}
	}
	keepalive__remove(context);
	mosquitto__set_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context__cleanup_out_packets(context);
}


ଶୂନ୍ୟ context__add_to_disused(ସଂରଚନା mosquitto *context)
{
	ଯଦି(context->state == mosq_cs_disused){
		ଫେରନ୍ତୁ;
	}

	mosquitto__set_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_next = db.ll_for_free;
	db.ll_for_free = context;
}


ଶୂନ୍ୟ context__free_disused(ଶୂନ୍ୟ)
{
	ସଂରଚନା mosquitto *context, *next;
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ସଂରଚନା mosquitto *last = ଖାଲି;
#endif

	context = db.ll_for_free;
	db.ll_for_free = ଖାଲି;
	ଯେତେବେଳେ(context){
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ଯଦି(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			ଯଦି(last){
				last->for_free_next = context;
			}ଅନ୍ୟଥା{
				db.ll_for_free = context;
			}
			next = context->for_free_next;
			context->for_free_next = ଖାଲି;
			last = context;
			context = next;
		}ଅନ୍ୟଥା
#endif
		{
			next = context->for_free_next;
			context__cleanup(context, true);
			context = next;
		}
	}
}


ଶୂନ୍ୟ context__add_to_by_id(ସଂରଚନା mosquitto *context)
{
	ଯଦି(context->in_by_id == false){
		context->in_by_id = true;
		HASH_ମୂଲ୍ୟ(context->id, strlen(context->id), context->id_hashv);
		HASH_ADD_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, strlen(context->id), context->id_hashv, context);
	}
}


ଶୂନ୍ୟ context__remove_from_by_id(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା mosquitto *context_found;

	ଯଦି(!context->id){
		ଫେରନ୍ତୁ;
	}

	ଯଦି(context->in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), context_found);
		ଯଦି(context_found == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_found);
		}
		context->id_hashv = 0;
		context->in_by_id = false;
		ଫେରନ୍ତୁ;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
	ଯଦି(context_found == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_found);
	}
}
