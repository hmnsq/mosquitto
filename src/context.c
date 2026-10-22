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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <time.h>
#જો defined(__APPLE__) || defined(_AIX)
#અન્તર્ભાવ <sys/socket.h>
#endif

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "alias_mosq.h"
#અન્તર્ભાવ "packet_mosq.h"
#અન્તર્ભાવ "property_mosq.h"
#અન્તર્ભાવ "sys_tree.h"
#અન્તર્ભાવ "util_mosq.h"
#અન્તર્ભાવ "will_mosq.h"

#અન્તર્ભાવ "uthash.h"


પૂર્ણાંક context__init_sock(સંરચના mosquitto *context, mosq_sock_t sock, bool get_address)
{
	context->sock = sock;

	જો((પૂર્ણાંક)context->sock >= 0){
		જો(get_address){
			અક્ષર address[1024];

			જો(!net__socket_get_address(context->sock,
					address, કદ(address),
					&context->remote_port)){

				context->address = mosquitto_strdup(address);
			}
			જો(!context->address){
				/* getpeername and inet_ntop failed and not a bridge */
				પાછા MOSQ_ERR_NOMEM;
			}
		}
		HASH_ADD(hh_sock, db.contexts_by_sock, sock, કદ(context->sock), context);
	}
	પાછા MOSQ_ERR_SUCCESS;
}

સંરચના mosquitto *context__init(શૂન્ય)
{
	સંરચના mosquitto *context;

	context = mosquitto_calloc(1, કદ(સંરચના mosquitto));
	જો(!context){
		પાછા ખાલી;
	}

	context->in_packet.packet_buffer_size = db.config->packet_buffer_size;
	context->in_packet.packet_buffer = mosquitto_calloc(1, context->in_packet.packet_buffer_size);
	જો(!context->in_packet.packet_buffer){
		mosquitto_FREE(context);
		પાછા ખાલી;
	}

#જો defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#નહીંતર
	context->pollfd_index = -1;
#endif
	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_સંદેશ_in = db.now_s;
	context->next_સંદેશ_out = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_start = true;
	context->id = ખાલી;
	context->last_mid = 0;
	context->will = ખાલી;
	context->username = ખાલી;
	context->password = ખાલી;
	context->listener = ખાલી;
	context->acl_list = ખાલી;
	context->retain_available = true;
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	memset(&context->wsd, 0, કદ(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.mask = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#endif

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->in_packet.payload = ખાલી;
	packet__cleanup(&context->in_packet);
	context->out_packet = ખાલી;
	context->out_packet_ગણતરી = 0;
	context->out_packet_bytes = 0;

	context->address = ખાલી;
	context->bridge = ખાલી;
	context->msgs_in.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_in.inflight_quota = db.config->max_inflight_messages;
	context->msgs_out.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_out.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#ifdef WITH_TLS
	context->ssl = ખાલી;
#endif

	પાછા context;
}


સ્થિર શૂન્ય context__cleanup_out_packets(સંરચના mosquitto *context)
{
	સંરચના mosquitto__packet *packet;

	જો(!context){
		પાછા;
	}

	જ્યારે(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_ગણતરી);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_ગણતરી = 0;
	context->out_packet_bytes = 0;
}


/*
 * This will result in any outgoing packets going unsent. If we're disconnected
 * forcefully then it is usually an error condition and shouldn't be a problem,
 * but it will mean that CONNACK messages will never get sent for bad protocol
 * versions for example.
 */
શૂન્ય context__cleanup(સંરચના mosquitto *context, bool force_free)
{
	જો(!context){
		પાછા;
	}

	જો(force_free){
		context->clean_start = true;
	}

#ifdef WITH_BRIDGE
	જો(context->bridge){
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
	જો(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->address);

	context__send_will(context);

	જો(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->in_packet));
	context__cleanup_out_packets(context);
#જો defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	જો(context->adns){
		gai_cancel(context->adns);
		સંરચના addrinfo *ar_request = (સંરચના addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#endif

#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#endif
	mosquitto_FREE(context->proxy.buf);
	જો(force_free){
		mosquitto_FREE(context);
	}
}


શૂન્ય context__send_will(સંરચના mosquitto *ctxt)
{
	જો(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		જો(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay__add(ctxt);
			પાછા;
		}

		જો(mosquitto_acl_check(ctxt,
				ctxt->will->સંદેશ.topic,
				(uint32_t)ctxt->will->સંદેશ.payloadlen,
				ctxt->will->સંદેશ.payload,
				(uint8_t)ctxt->will->સંદેશ.qos,
				ctxt->will->સંદેશ.retain,
				ctxt->will->properties,
				MOSQ_ACL_WRITE) == MOSQ_ERR_SUCCESS){

			/* Unexpected disconnect, queue the client will. */
			db__messages_easy_queue(ctxt,
					ctxt->will->સંદેશ.topic,
					(uint8_t)ctxt->will->સંદેશ.qos,
					(uint32_t)ctxt->will->સંદેશ.payloadlen,
					ctxt->will->સંદેશ.payload,
					ctxt->will->સંદેશ.retain,
					ctxt->will->expiry_interval,
					&ctxt->will->properties);
		}
	}
	will__clear(ctxt);
}


શૂન્ય context__disconnect(સંરચના mosquitto *context, પૂર્ણાંક reason)
{
	જો(mosquitto__get_state(context) == mosq_cs_disconnected){
		પાછા;
	}

#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	જો(context->transport == mosq_t_ws){
		uint8_t buf[4] = {0x88, 0x02, 0x03, context->wsd.disconnect_reason};
		/* Send the disconnect reason, but don't care if it fails */
		જો(send(context->sock, buf, 4, 0)){
		}
	}
#endif
	જો(context->id){
		સંરચના mosquitto *context_found;
		HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
		જો(context_found == context){
			net__socket_close(context);
			context__add_to_disused(context);
			પાછા;
		}
	}

	context__send_will(context);

	જો(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
		plugin__handle_disconnect(context, reason);
	}નહીંતર{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#ifdef WITH_BRIDGE
	જો(context->bridge == ખાલી)
	/* Outgoing bridge connection never expire */
#endif
	{
		જો(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			જો(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context__add_to_disused(context);
			}
		}નહીંતર{
			session_expiry__add(context);
		}
	}
	keepalive__remove(context);
	mosquitto__set_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context__cleanup_out_packets(context);
}


શૂન્ય context__add_to_disused(સંરચના mosquitto *context)
{
	જો(context->state == mosq_cs_disused){
		પાછા;
	}

	mosquitto__set_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_next = db.ll_for_free;
	db.ll_for_free = context;
}


શૂન્ય context__free_disused(શૂન્ય)
{
	સંરચના mosquitto *context, *next;
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	સંરચના mosquitto *last = ખાલી;
#endif

	context = db.ll_for_free;
	db.ll_for_free = ખાલી;
	જ્યારે(context){
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		જો(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			જો(last){
				last->for_free_next = context;
			}નહીંતર{
				db.ll_for_free = context;
			}
			next = context->for_free_next;
			context->for_free_next = ખાલી;
			last = context;
			context = next;
		}નહીંતર
#endif
		{
			next = context->for_free_next;
			context__cleanup(context, true);
			context = next;
		}
	}
}


શૂન્ય context__add_to_by_id(સંરચના mosquitto *context)
{
	જો(context->in_by_id == false){
		context->in_by_id = true;
		HASH_કિંમત(context->id, strlen(context->id), context->id_hashv);
		HASH_ADD_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, strlen(context->id), context->id_hashv, context);
	}
}


શૂન્ય context__remove_from_by_id(સંરચના mosquitto *context)
{
	સંરચના mosquitto *context_found;

	જો(!context->id){
		પાછા;
	}

	જો(context->in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), context_found);
		જો(context_found == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_found);
		}
		context->id_hashv = 0;
		context->in_by_id = false;
		પાછા;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
	જો(context_found == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_found);
	}
}
