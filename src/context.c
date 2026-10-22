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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <time.h>
#எனில் defined(__APPLE__) || defined(_AIX)
#அந்தர்பாவ <sys/socket.h>
#endif

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "alias_mosq.h"
#அந்தர்பாவ "packet_mosq.h"
#அந்தர்பாவ "property_mosq.h"
#அந்தர்பாவ "sys_tree.h"
#அந்தர்பாவ "util_mosq.h"
#அந்தர்பாவ "will_mosq.h"

#அந்தர்பாவ "uthash.h"


முழு context__init_sock(கட்டமைப்பு mosquitto *context, mosq_sock_t sock, bool get_address)
{
	context->sock = sock;

	எனில்((முழு)context->sock >= 0){
		எனில்(get_address){
			எழுத்து address[1024];

			எனில்(!net__socket_get_address(context->sock,
					address, அளவு(address),
					&context->remote_port)){

				context->address = mosquitto_strdup(address);
			}
			எனில்(!context->address){
				/* getpeername and inet_ntop failed and not a bridge */
				திரும்பு MOSQ_ERR_NOMEM;
			}
		}
		HASH_ADD(hh_sock, db.contexts_by_sock, sock, அளவு(context->sock), context);
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}

கட்டமைப்பு mosquitto *context__init(வெறுமை)
{
	கட்டமைப்பு mosquitto *context;

	context = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto));
	எனில்(!context){
		திரும்பு வெற்று;
	}

	context->in_packet.packet_buffer_size = db.config->packet_buffer_size;
	context->in_packet.packet_buffer = mosquitto_calloc(1, context->in_packet.packet_buffer_size);
	எனில்(!context->in_packet.packet_buffer){
		mosquitto_FREE(context);
		திரும்பு வெற்று;
	}

#எனில் defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#இல்லையெனில்
	context->pollfd_index = -1;
#endif
	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_செய்தி_in = db.now_s;
	context->next_செய்தி_out = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_start = true;
	context->id = வெற்று;
	context->last_mid = 0;
	context->will = வெற்று;
	context->username = வெற்று;
	context->password = வெற்று;
	context->listener = வெற்று;
	context->acl_list = வெற்று;
	context->retain_available = true;
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	memset(&context->wsd, 0, அளவு(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.mask = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#endif

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->in_packet.payload = வெற்று;
	packet__cleanup(&context->in_packet);
	context->out_packet = வெற்று;
	context->out_packet_மொத்தம் = 0;
	context->out_packet_bytes = 0;

	context->address = வெற்று;
	context->bridge = வெற்று;
	context->msgs_in.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_in.inflight_quota = db.config->max_inflight_messages;
	context->msgs_out.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_out.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#ifdef WITH_TLS
	context->ssl = வெற்று;
#endif

	திரும்பு context;
}


நிலையான வெறுமை context__cleanup_out_packets(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு mosquitto__packet *packet;

	எனில்(!context){
		திரும்பு;
	}

	போது(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_மொத்தம்);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_மொத்தம் = 0;
	context->out_packet_bytes = 0;
}


/*
 * This will result in any outgoing packets going unsent. If we're disconnected
 * forcefully then it is usually an error condition and shouldn't be a problem,
 * but it will mean that CONNACK messages will never get sent for bad protocol
 * versions for example.
 */
வெறுமை context__cleanup(கட்டமைப்பு mosquitto *context, bool force_free)
{
	எனில்(!context){
		திரும்பு;
	}

	எனில்(force_free){
		context->clean_start = true;
	}

#ifdef WITH_BRIDGE
	எனில்(context->bridge){
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
	எனில்(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->address);

	context__send_will(context);

	எனில்(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->in_packet));
	context__cleanup_out_packets(context);
#எனில் defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	எனில்(context->adns){
		gai_cancel(context->adns);
		கட்டமைப்பு addrinfo *ar_request = (கட்டமைப்பு addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#endif

#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#endif
	mosquitto_FREE(context->proxy.buf);
	எனில்(force_free){
		mosquitto_FREE(context);
	}
}


வெறுமை context__send_will(கட்டமைப்பு mosquitto *ctxt)
{
	எனில்(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		எனில்(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay__add(ctxt);
			திரும்பு;
		}

		எனில்(mosquitto_acl_check(ctxt,
				ctxt->will->செய்தி.topic,
				(uint32_t)ctxt->will->செய்தி.payloadlen,
				ctxt->will->செய்தி.payload,
				(uint8_t)ctxt->will->செய்தி.qos,
				ctxt->will->செய்தி.retain,
				ctxt->will->properties,
				MOSQ_ACL_WRITE) == MOSQ_ERR_SUCCESS){

			/* Unexpected disconnect, queue the client will. */
			db__messages_easy_queue(ctxt,
					ctxt->will->செய்தி.topic,
					(uint8_t)ctxt->will->செய்தி.qos,
					(uint32_t)ctxt->will->செய்தி.payloadlen,
					ctxt->will->செய்தி.payload,
					ctxt->will->செய்தி.retain,
					ctxt->will->expiry_interval,
					&ctxt->will->properties);
		}
	}
	will__clear(ctxt);
}


வெறுமை context__disconnect(கட்டமைப்பு mosquitto *context, முழு reason)
{
	எனில்(mosquitto__get_state(context) == mosq_cs_disconnected){
		திரும்பு;
	}

#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	எனில்(context->transport == mosq_t_ws){
		uint8_t buf[4] = {0x88, 0x02, 0x03, context->wsd.disconnect_reason};
		/* Send the disconnect reason, but don't care if it fails */
		எனில்(send(context->sock, buf, 4, 0)){
		}
	}
#endif
	எனில்(context->id){
		கட்டமைப்பு mosquitto *context_found;
		HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
		எனில்(context_found == context){
			net__socket_close(context);
			context__add_to_disused(context);
			திரும்பு;
		}
	}

	context__send_will(context);

	எனில்(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
		plugin__handle_disconnect(context, reason);
	}இல்லையெனில்{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#ifdef WITH_BRIDGE
	எனில்(context->bridge == வெற்று)
	/* Outgoing bridge connection never expire */
#endif
	{
		எனில்(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			எனில்(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context__add_to_disused(context);
			}
		}இல்லையெனில்{
			session_expiry__add(context);
		}
	}
	keepalive__remove(context);
	mosquitto__set_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context__cleanup_out_packets(context);
}


வெறுமை context__add_to_disused(கட்டமைப்பு mosquitto *context)
{
	எனில்(context->state == mosq_cs_disused){
		திரும்பு;
	}

	mosquitto__set_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_next = db.ll_for_free;
	db.ll_for_free = context;
}


வெறுமை context__free_disused(வெறுமை)
{
	கட்டமைப்பு mosquitto *context, *next;
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	கட்டமைப்பு mosquitto *last = வெற்று;
#endif

	context = db.ll_for_free;
	db.ll_for_free = வெற்று;
	போது(context){
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		எனில்(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			எனில்(last){
				last->for_free_next = context;
			}இல்லையெனில்{
				db.ll_for_free = context;
			}
			next = context->for_free_next;
			context->for_free_next = வெற்று;
			last = context;
			context = next;
		}இல்லையெனில்
#endif
		{
			next = context->for_free_next;
			context__cleanup(context, true);
			context = next;
		}
	}
}


வெறுமை context__add_to_by_id(கட்டமைப்பு mosquitto *context)
{
	எனில்(context->in_by_id == false){
		context->in_by_id = true;
		HASH_மதிப்பு(context->id, strlen(context->id), context->id_hashv);
		HASH_ADD_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, strlen(context->id), context->id_hashv, context);
	}
}


வெறுமை context__remove_from_by_id(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு mosquitto *context_found;

	எனில்(!context->id){
		திரும்பு;
	}

	எனில்(context->in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), context_found);
		எனில்(context_found == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_found);
		}
		context->id_hashv = 0;
		context->in_by_id = false;
		திரும்பு;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
	எனில்(context_found == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_found);
	}
}
