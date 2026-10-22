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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <time.h>
#ಆದರೆ defined(__APPLE__) || defined(_AIX)
#ಅನ್ತರ್ಭಾವ <sys/socket.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "alias_mosq.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "property_mosq.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"
#ಅನ್ತರ್ಭಾವ "will_mosq.h"

#ಅನ್ತರ್ಭಾವ "uthash.h"


ಪೂರ್ಣಾಂಕ context__init_sock(ರಚನೆ mosquitto *context, mosq_sock_t sock, bool get_ವಿಳಾಸ)
{
	context->sock = sock;

	ಆದರೆ((ಪೂರ್ಣಾಂಕ)context->sock >= 0){
		ಆದರೆ(get_ವಿಳಾಸ){
			ಅಕ್ಷರ ವಿಳಾಸ[1024];

			ಆದರೆ(!net_socket_get_ವಿಳಾಸ(context->sock,
					ವಿಳಾಸ, ಗಾತ್ರ(ವಿಳಾಸ),
					&context->remote_ಪೋರ್ಟ್)){

				context->ವಿಳಾಸ = mosquitto_strdup(ವಿಳಾಸ);
			}
			ಆದರೆ(!context->ವಿಳಾಸ){
				/* getpeername and inet_ntop failed and not a bridge */
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
		}
		HASH_ಕೂಡಿಸು(hh_sock, db.contexts_by_sock, sock, ಗಾತ್ರ(context->sock), context);
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}

ರಚನೆ mosquitto *context__init(ಶೂನ್ಯ)
{
	ರಚನೆ mosquitto *context;

	context = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto));
	ಆದರೆ(!context){
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	context->ಒಳ_packet.packet_ಬಫರ್_ಗಾತ್ರ = db.config->packet_ಬಫರ್_ಗಾತ್ರ;
	context->ಒಳ_packet.packet_ಬಫರ್ = mosquitto_calloc(1, context->ಒಳ_packet.packet_ಬಫರ್_ಗಾತ್ರ);
	ಆದರೆ(!context->ಒಳ_packet.packet_ಬಫರ್){
		mosquitto_FREE(context);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

#ಆದರೆ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#ಇಲ್ಲದಿದ್ದರೆ
	context->pollfd_ಸೂಚ್ಯಂಕ = -1;
#ಆದರೆ_ಅಂತ್ಯ
	mosquitto_ಗಣ_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->ಕೊನೆಯ_ಸಂದೇಶ_ಒಳ = db.now_s;
	context->ಮುಂದಿನ_ಸಂದೇಶ_ಹೊರಹೋಗು = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_ಆರಂಭ = true;
	context->id = ಖಾಲಿ;
	context->ಕೊನೆಯ_mid = 0;
	context->will = ಖಾಲಿ;
	context->username = ಖಾಲಿ;
	context->password = ಖಾಲಿ;
	context->listener = ಖಾಲಿ;
	context->acl_list = ಖಾಲಿ;
	context->retain_ಲಭ್ಯ = true;
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	memset(&context->wsd, 0, ಗಾತ್ರ(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.ಮುಖವಾಡ = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#ಆದರೆ_ಅಂತ್ಯ

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->ಒಳ_packet.payload = ಖಾಲಿ;
	packet__cleanup(&context->ಒಳ_packet);
	context->ಹೊರಹೋಗು_packet = ಖಾಲಿ;
	context->ಹೊರಹೋಗು_packet_ಎಣಿಕೆ = 0;
	context->ಹೊರಹೋಗು_packet_ಬೈಟುಗಳು = 0;

	context->ವಿಳಾಸ = ಖಾಲಿ;
	context->bridge = ಖಾಲಿ;
	context->msgs_ಒಳ.inflight_ಗರಿಷ್ಠ = db.config->max_inflight_messages;
	context->msgs_ಒಳ.inflight_quota = db.config->max_inflight_messages;
	context->msgs_ಹೊರಹೋಗು.inflight_ಗರಿಷ್ಠ = db.config->max_inflight_messages;
	context->msgs_ಹೊರಹೋಗು.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	context->ssl = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

	ಹಿಂತಿರುಗಿ context;
}


ಸ್ಥಿರ ಶೂನ್ಯ context_cleanup_ಹೊರಹೋಗು_packets(ರಚನೆ mosquitto *context)
{
	ರಚನೆ mosquitto__packet *packet;

	ಆದರೆ(!context){
		ಹಿಂತಿರುಗಿ;
	}

	ಆಗ(context->ಹೊರಹೋಗು_packet){
		packet = context->ಹೊರಹೋಗು_packet;
		context->ಹೊರಹೋಗು_packet = context->ಹೊರಹೋಗು_packet->ಮುಂದಿನ;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_ಹೊರಹೋಗು_packets, context->ಹೊರಹೋಗು_packet_ಎಣಿಕೆ);
	metrics__int_dec(mosq_gauge_ಹೊರಹೋಗು_packet_ಬೈಟುಗಳು, context->ಹೊರಹೋಗು_packet_ಬೈಟುಗಳು);
	context->ಹೊರಹೋಗು_packet_ಎಣಿಕೆ = 0;
	context->ಹೊರಹೋಗು_packet_ಬೈಟುಗಳು = 0;
}


/*
 * This will result in any outgoing packets going unsent. If we're disconnected
 * forcefully then it is usually an error condition and shouldn't be a problem,
 * but it will mean that CONNACK messages will never get sent for bad protocol
 * versions for example.
 */
ಶೂನ್ಯ context__cleanup(ರಚನೆ mosquitto *context, bool force_free)
{
	ಆದರೆ(!context){
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ(force_free){
		context->clean_ಆರಂಭ = true;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
	ಆದರೆ(context->bridge){
		bridge__cleanup(context);
	}
#ಆದರೆ_ಅಂತ್ಯ
	mosquitto_FREE(context->ಒಳ_packet.packet_ಬಫರ್);
	context->ಒಳ_packet.packet_ಬಫರ್_ಗಾತ್ರ = 0;

	alias__free_all(context);
	keepalive__remove(context);
	context_cleanup_ಹೊರಹೋಗು_packets(context);

	mosquitto_FREE(context->auth_ವಿಧಾನ);
	mosquitto_FREE(context->username);
	mosquitto_FREE(context->password);

	net__socket_close(context);
	ಆದರೆ(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->ವಿಳಾಸ);

	context__send_will(context);

	ಆದರೆ(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->ಒಳ_packet));
	context_cleanup_ಹೊರಹೋಗು_packets(context);
#ಆದರೆ defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	ಆದರೆ(context->adns){
		gai_cancel(context->adns);
		ರಚನೆ addrinfo *ar_request = (ರಚನೆ addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#ಆದರೆ_ಅಂತ್ಯ
	mosquitto_FREE(context->proxy.buf);
	ಆದರೆ(force_free){
		mosquitto_FREE(context);
	}
}


ಶೂನ್ಯ context__send_will(ರಚನೆ mosquitto *ctxt)
{
	ಆದರೆ(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		ಆದರೆ(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay_ಕೂಡಿಸು(ctxt);
			ಹಿಂತಿರುಗಿ;
		}

		ಆದರೆ(mosquitto_acl_check(ctxt,
				ctxt->will->ಸಂದೇಶ.topic,
				(uint32_t)ctxt->will->ಸಂದೇಶ.payloadlen,
				ctxt->will->ಸಂದೇಶ.payload,
				(uint8_t)ctxt->will->ಸಂದೇಶ.qos,
				ctxt->will->ಸಂದೇಶ.retain,
				ctxt->will->properties,
				MOSQ_ACL_WRITE) == MOSQ_ERR_SUCCESS){

			/* Unexpected disconnect, queue the client will. */
			db_messages_easy_ಸರತಿ(ctxt,
					ctxt->will->ಸಂದೇಶ.topic,
					(uint8_t)ctxt->will->ಸಂದೇಶ.qos,
					(uint32_t)ctxt->will->ಸಂದೇಶ.payloadlen,
					ctxt->will->ಸಂದೇಶ.payload,
					ctxt->will->ಸಂದೇಶ.retain,
					ctxt->will->expiry_interval,
					&ctxt->will->properties);
		}
	}
	will__clear(ctxt);
}


ಶೂನ್ಯ context__disconnect(ರಚನೆ mosquitto *context, ಪೂರ್ಣಾಂಕ reason)
{
	ಆದರೆ(mosquitto__get_state(context) == mosq_cs_disconnected){
		ಹಿಂತಿರುಗಿ;
	}

#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	ಆದರೆ(context->transport == mosq_t_ws){
		uint8_t buf[4] = {0x88, 0x02, 0x03, context->wsd.disconnect_reason};
		/* Send the disconnect reason, but don't care if it fails */
		ಆದರೆ(send(context->sock, buf, 4, 0)){
		}
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ(context->id){
		ರಚನೆ mosquitto *context_ಸಿಕ್ಕಿತು;
		HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id_delayed_auth, context->id, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->id), context_ಸಿಕ್ಕಿತು);
		ಆದರೆ(context_ಸಿಕ್ಕಿತು == context){
			net__socket_close(context);
			context_ಕೂಡಿಸು_ಗಮ್ಯ_disused(context);
			ಹಿಂತಿರುಗಿ;
		}
	}

	context__send_will(context);

	ಆದರೆ(context->session_expiry_interval == MQTT_SESSION_EXPIRY_ತಕ್ಷಣ){
		plugin__handle_disconnect(context, reason);
	}ಇಲ್ಲದಿದ್ದರೆ{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
	ಆದರೆ(context->bridge == ಖಾಲಿ)
	/* Outgoing bridge connection never expire */
#ಆದರೆ_ಅಂತ್ಯ
	{
		ಆದರೆ(context->session_expiry_interval == MQTT_SESSION_EXPIRY_ತಕ್ಷಣ){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			ಆದರೆ(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context_ಕೂಡಿಸು_ಗಮ್ಯ_disused(context);
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			session_expiry_ಕೂಡಿಸು(context);
		}
	}
	keepalive__remove(context);
	mosquitto_ಗಣ_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context_cleanup_ಹೊರಹೋಗು_packets(context);
}


ಶೂನ್ಯ context_ಕೂಡಿಸು_ಗಮ್ಯ_disused(ರಚನೆ mosquitto *context)
{
	ಆದರೆ(context->state == mosq_cs_disused){
		ಹಿಂತಿರುಗಿ;
	}

	mosquitto_ಗಣ_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_ಮುಂದಿನ = db.ll_for_free;
	db.ll_for_free = context;
}


ಶೂನ್ಯ context__free_disused(ಶೂನ್ಯ)
{
	ರಚನೆ mosquitto *context, *ಮುಂದಿನ;
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ರಚನೆ mosquitto *ಕೊನೆಯ = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

	context = db.ll_for_free;
	db.ll_for_free = ಖಾಲಿ;
	ಆಗ(context){
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ಆದರೆ(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			ಆದರೆ(ಕೊನೆಯ){
				ಕೊನೆಯ->for_free_ಮುಂದಿನ = context;
			}ಇಲ್ಲದಿದ್ದರೆ{
				db.ll_for_free = context;
			}
			ಮುಂದಿನ = context->for_free_ಮುಂದಿನ;
			context->for_free_ಮುಂದಿನ = ಖಾಲಿ;
			ಕೊನೆಯ = context;
			context = ಮುಂದಿನ;
		}ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		{
			ಮುಂದಿನ = context->for_free_ಮುಂದಿನ;
			context__cleanup(context, true);
			context = ಮುಂದಿನ;
		}
	}
}


ಶೂನ್ಯ context_ಕೂಡಿಸು_ಗಮ್ಯ_by_id(ರಚನೆ mosquitto *context)
{
	ಆದರೆ(context->ಒಳ_by_id == false){
		context->ಒಳ_by_id = true;
		HASH_ಮೌಲ್ಯ(context->id, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->id), context->id_hashv);
		HASH_ಕೂಡಿಸು_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->id), context->id_hashv, context);
	}
}


ಶೂನ್ಯ context__remove_from_by_id(ರಚನೆ mosquitto *context)
{
	ರಚನೆ mosquitto *context_ಸಿಕ್ಕಿತು;

	ಆದರೆ(!context->id){
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ(context->ಒಳ_by_id){
		HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id, context->id, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->id), context_ಸಿಕ್ಕಿತು);
		ಆದರೆ(context_ಸಿಕ್ಕಿತು == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_ಸಿಕ್ಕಿತು);
		}
		context->id_hashv = 0;
		context->ಒಳ_by_id = false;
		ಹಿಂತಿರುಗಿ;
	}

	HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id_delayed_auth, context->id, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->id), context_ಸಿಕ್ಕಿತು);
	ಆದರೆ(context_ಸಿಕ್ಕಿತು == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_ಸಿಕ್ಕಿತು);
	}
}
