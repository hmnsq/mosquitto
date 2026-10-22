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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>
#ਜੇ defined(__APPLE__) || defined(_AIX)
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/socket.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "alias_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "property_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sys_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "will_mosq.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "uthash.h"


ਪੂਰਨ context__init_sock(ਬਣਤਰ mosquitto *context, mosq_sock_t sock, bool get_ਪਤਾ)
{
	context->sock = sock;

	ਜੇ((ਪੂਰਨ)context->sock >= 0){
		ਜੇ(get_ਪਤਾ){
			ਅੱਖਰ ਪਤਾ[1024];

			ਜੇ(!net_socket_get_ਪਤਾ(context->sock,
					ਪਤਾ, ਆਕਾਰ(ਪਤਾ),
					&context->remote_ਪੋਰਟ)){

				context->ਪਤਾ = mosquitto_strdup(ਪਤਾ);
			}
			ਜੇ(!context->ਪਤਾ){
				/* getpeername and inet_ntop failed and not a bridge */
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
		}
		HASH_ਜੋੜੋ(hh_sock, db.contexts_by_sock, sock, ਆਕਾਰ(context->sock), context);
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}

ਬਣਤਰ mosquitto *context__init(ਖਾਲੀ)
{
	ਬਣਤਰ mosquitto *context;

	context = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto));
	ਜੇ(!context){
		ਵਾਪਸ ਨਿਰਾ;
	}

	context->ਵਿੱਚ_packet.packet_ਬਫਰ_ਮਾਪ = db.config->packet_ਬਫਰ_ਮਾਪ;
	context->ਵਿੱਚ_packet.packet_ਬਫਰ = mosquitto_calloc(1, context->ਵਿੱਚ_packet.packet_ਬਫਰ_ਮਾਪ);
	ਜੇ(!context->ਵਿੱਚ_packet.packet_ਬਫਰ){
		mosquitto_FREE(context);
		ਵਾਪਸ ਨਿਰਾ;
	}

#ਜੇ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#ਨਹੀਂਤਾਂ
	context->pollfd_ਸੂਚਕਾਂਕ = -1;
#ਜੇ_ਅੰਤ
	mosquitto_ਸੈੱਟ_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->ਆਖਰੀ_ਸੁਨੇਹਾ_ਵਿੱਚ = db.now_s;
	context->ਅਗਲਾ_ਸੁਨੇਹਾ_ਬਾਹਰਲਾ = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_ਸ਼ੁਰੂ = true;
	context->id = ਨਿਰਾ;
	context->ਆਖਰੀ_mid = 0;
	context->will = ਨਿਰਾ;
	context->username = ਨਿਰਾ;
	context->password = ਨਿਰਾ;
	context->listener = ਨਿਰਾ;
	context->acl_list = ਨਿਰਾ;
	context->retain_ਉਪਲਬਧ = true;
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	memset(&context->wsd, 0, ਆਕਾਰ(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.ਨਕਾਬ = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#ਜੇ_ਅੰਤ

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->ਵਿੱਚ_packet.payload = ਨਿਰਾ;
	packet__cleanup(&context->ਵਿੱਚ_packet);
	context->ਬਾਹਰਲਾ_packet = ਨਿਰਾ;
	context->ਬਾਹਰਲਾ_packet_ਗਿਣਤੀ = 0;
	context->ਬਾਹਰਲਾ_packet_ਬਾਈਟਾਂ = 0;

	context->ਪਤਾ = ਨਿਰਾ;
	context->bridge = ਨਿਰਾ;
	context->msgs_ਵਿੱਚ.inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ = db.config->max_inflight_messages;
	context->msgs_ਵਿੱਚ.inflight_quota = db.config->max_inflight_messages;
	context->msgs_ਬਾਹਰਲਾ.inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ = db.config->max_inflight_messages;
	context->msgs_ਬਾਹਰਲਾ.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	context->ssl = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

	ਵਾਪਸ context;
}


ਸਥਿਰ ਖਾਲੀ context_cleanup_ਬਾਹਰਲਾ_packets(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ mosquitto__packet *packet;

	ਜੇ(!context){
		ਵਾਪਸ;
	}

	ਜਦੋਂ(context->ਬਾਹਰਲਾ_packet){
		packet = context->ਬਾਹਰਲਾ_packet;
		context->ਬਾਹਰਲਾ_packet = context->ਬਾਹਰਲਾ_packet->ਅਗਲਾ;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_ਬਾਹਰਲਾ_packets, context->ਬਾਹਰਲਾ_packet_ਗਿਣਤੀ);
	metrics__int_dec(mosq_gauge_ਬਾਹਰਲਾ_packet_ਬਾਈਟਾਂ, context->ਬਾਹਰਲਾ_packet_ਬਾਈਟਾਂ);
	context->ਬਾਹਰਲਾ_packet_ਗਿਣਤੀ = 0;
	context->ਬਾਹਰਲਾ_packet_ਬਾਈਟਾਂ = 0;
}


/*
 * This will result in any outgoing packets going unsent. If we're disconnected
 * forcefully then it is usually an error condition and shouldn't be a problem,
 * but it will mean that CONNACK messages will never get sent for bad protocol
 * versions for example.
 */
ਖਾਲੀ context__cleanup(ਬਣਤਰ mosquitto *context, bool force_free)
{
	ਜੇ(!context){
		ਵਾਪਸ;
	}

	ਜੇ(force_free){
		context->clean_ਸ਼ੁਰੂ = true;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
	ਜੇ(context->bridge){
		bridge__cleanup(context);
	}
#ਜੇ_ਅੰਤ
	mosquitto_FREE(context->ਵਿੱਚ_packet.packet_ਬਫਰ);
	context->ਵਿੱਚ_packet.packet_ਬਫਰ_ਮਾਪ = 0;

	alias__free_all(context);
	keepalive__remove(context);
	context_cleanup_ਬਾਹਰਲਾ_packets(context);

	mosquitto_FREE(context->auth_ਵਿਧੀ);
	mosquitto_FREE(context->username);
	mosquitto_FREE(context->password);

	net__socket_close(context);
	ਜੇ(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->ਪਤਾ);

	context__send_will(context);

	ਜੇ(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->ਵਿੱਚ_packet));
	context_cleanup_ਬਾਹਰਲਾ_packets(context);
#ਜੇ defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	ਜੇ(context->adns){
		gai_cancel(context->adns);
		ਬਣਤਰ addrinfo *ar_request = (ਬਣਤਰ addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#ਜੇ_ਅੰਤ

#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#ਜੇ_ਅੰਤ
	mosquitto_FREE(context->proxy.buf);
	ਜੇ(force_free){
		mosquitto_FREE(context);
	}
}


ਖਾਲੀ context__send_will(ਬਣਤਰ mosquitto *ctxt)
{
	ਜੇ(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		ਜੇ(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay_ਜੋੜੋ(ctxt);
			ਵਾਪਸ;
		}

		ਜੇ(mosquitto_acl_check(ctxt,
				ctxt->will->ਸੁਨੇਹਾ.topic,
				(uint32_t)ctxt->will->ਸੁਨੇਹਾ.payloadlen,
				ctxt->will->ਸੁਨੇਹਾ.payload,
				(uint8_t)ctxt->will->ਸੁਨੇਹਾ.qos,
				ctxt->will->ਸੁਨੇਹਾ.retain,
				ctxt->will->properties,
				MOSQ_ACL_WRITE) == MOSQ_ERR_SUCCESS){

			/* Unexpected disconnect, queue the client will. */
			db_messages_easy_ਕਤਾਰ(ctxt,
					ctxt->will->ਸੁਨੇਹਾ.topic,
					(uint8_t)ctxt->will->ਸੁਨੇਹਾ.qos,
					(uint32_t)ctxt->will->ਸੁਨੇਹਾ.payloadlen,
					ctxt->will->ਸੁਨੇਹਾ.payload,
					ctxt->will->ਸੁਨੇਹਾ.retain,
					ctxt->will->expiry_interval,
					&ctxt->will->properties);
		}
	}
	will__clear(ctxt);
}


ਖਾਲੀ context__disconnect(ਬਣਤਰ mosquitto *context, ਪੂਰਨ reason)
{
	ਜੇ(mosquitto__get_state(context) == mosq_cs_disconnected){
		ਵਾਪਸ;
	}

#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	ਜੇ(context->transport == mosq_t_ws){
		uint8_t buf[4] = {0x88, 0x02, 0x03, context->wsd.disconnect_reason};
		/* Send the disconnect reason, but don't care if it fails */
		ਜੇ(send(context->sock, buf, 4, 0)){
		}
	}
#ਜੇ_ਅੰਤ
	ਜੇ(context->id){
		ਬਣਤਰ mosquitto *context_ਮਿਲਿਆ;
		HASH_ਲੱਭੋ(hh_id, db.contexts_by_id_delayed_auth, context->id, ਲੜੀ_ਲੰਬਾਈ(context->id), context_ਮਿਲਿਆ);
		ਜੇ(context_ਮਿਲਿਆ == context){
			net__socket_close(context);
			context_ਜੋੜੋ_ਮੰਜ਼ਿਲ_disused(context);
			ਵਾਪਸ;
		}
	}

	context__send_will(context);

	ਜੇ(context->session_expiry_interval == MQTT_SESSION_EXPIRY_ਤੁਰੰਤ){
		plugin__handle_disconnect(context, reason);
	}ਨਹੀਂਤਾਂ{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
	ਜੇ(context->bridge == ਨਿਰਾ)
	/* Outgoing bridge connection never expire */
#ਜੇ_ਅੰਤ
	{
		ਜੇ(context->session_expiry_interval == MQTT_SESSION_EXPIRY_ਤੁਰੰਤ){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			ਜੇ(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context_ਜੋੜੋ_ਮੰਜ਼ਿਲ_disused(context);
			}
		}ਨਹੀਂਤਾਂ{
			session_expiry_ਜੋੜੋ(context);
		}
	}
	keepalive__remove(context);
	mosquitto_ਸੈੱਟ_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context_cleanup_ਬਾਹਰਲਾ_packets(context);
}


ਖਾਲੀ context_ਜੋੜੋ_ਮੰਜ਼ਿਲ_disused(ਬਣਤਰ mosquitto *context)
{
	ਜੇ(context->state == mosq_cs_disused){
		ਵਾਪਸ;
	}

	mosquitto_ਸੈੱਟ_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_ਅਗਲਾ = db.ll_for_free;
	db.ll_for_free = context;
}


ਖਾਲੀ context__free_disused(ਖਾਲੀ)
{
	ਬਣਤਰ mosquitto *context, *ਅਗਲਾ;
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ਬਣਤਰ mosquitto *ਆਖਰੀ = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

	context = db.ll_for_free;
	db.ll_for_free = ਨਿਰਾ;
	ਜਦੋਂ(context){
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ਜੇ(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			ਜੇ(ਆਖਰੀ){
				ਆਖਰੀ->for_free_ਅਗਲਾ = context;
			}ਨਹੀਂਤਾਂ{
				db.ll_for_free = context;
			}
			ਅਗਲਾ = context->for_free_ਅਗਲਾ;
			context->for_free_ਅਗਲਾ = ਨਿਰਾ;
			ਆਖਰੀ = context;
			context = ਅਗਲਾ;
		}ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		{
			ਅਗਲਾ = context->for_free_ਅਗਲਾ;
			context__cleanup(context, true);
			context = ਅਗਲਾ;
		}
	}
}


ਖਾਲੀ context_ਜੋੜੋ_ਮੰਜ਼ਿਲ_by_id(ਬਣਤਰ mosquitto *context)
{
	ਜੇ(context->ਵਿੱਚ_by_id == false){
		context->ਵਿੱਚ_by_id = true;
		HASH_ਮੁੱਲ(context->id, ਲੜੀ_ਲੰਬਾਈ(context->id), context->id_hashv);
		HASH_ਜੋੜੋ_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, ਲੜੀ_ਲੰਬਾਈ(context->id), context->id_hashv, context);
	}
}


ਖਾਲੀ context__remove_from_by_id(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ mosquitto *context_ਮਿਲਿਆ;

	ਜੇ(!context->id){
		ਵਾਪਸ;
	}

	ਜੇ(context->ਵਿੱਚ_by_id){
		HASH_ਲੱਭੋ(hh_id, db.contexts_by_id, context->id, ਲੜੀ_ਲੰਬਾਈ(context->id), context_ਮਿਲਿਆ);
		ਜੇ(context_ਮਿਲਿਆ == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_ਮਿਲਿਆ);
		}
		context->id_hashv = 0;
		context->ਵਿੱਚ_by_id = false;
		ਵਾਪਸ;
	}

	HASH_ਲੱਭੋ(hh_id, db.contexts_by_id_delayed_auth, context->id, ਲੜੀ_ਲੰਬਾਈ(context->id), context_ਮਿਲਿਆ);
	ਜੇ(context_ਮਿਲਿਆ == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_ਮਿਲਿਆ);
	}
}
