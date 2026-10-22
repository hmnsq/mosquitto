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
#ಯದಿ defined(__APPLE__) || defined(_AIX)
#ಅನ್ತರ್ಭಾವ <sys/socket.h>
#endif

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "alias_mosq.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "property_mosq.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"
#ಅನ್ತರ್ಭಾವ "will_mosq.h"

#ಅನ್ತರ್ಭಾವ "uthash.h"


ಪೂರ್ಣಾಙ್ಕ context__init_sock(ಸಂರಚನಾ mosquitto *context, mosq_sock_t sock, bool get_address)
{
	context->sock = sock;

	ಯದಿ((ಪೂರ್ಣಾಙ್ಕ)context->sock >= 0){
		ಯದಿ(get_address){
			ವರ್ಣ address[1024];

			ಯದಿ(!net__socket_get_address(context->sock,
					address, ಪರಿಮಾಣ(address),
					&context->remote_port)){

				context->address = mosquitto_strdup(address);
			}
			ಯದಿ(!context->address){
				/* getpeername and inet_ntop failed and not a bridge */
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
		}
		HASH_ADD(hh_sock, db.contexts_by_sock, sock, ಪರಿಮಾಣ(context->sock), context);
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}

ಸಂರಚನಾ mosquitto *context__init(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ mosquitto *context;

	context = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto));
	ಯದಿ(!context){
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	context->in_packet.packet_buffer_size = db.config->packet_buffer_size;
	context->in_packet.packet_buffer = mosquitto_calloc(1, context->in_packet.packet_buffer_size);
	ಯದಿ(!context->in_packet.packet_buffer){
		mosquitto_FREE(context);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

#ಯದಿ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#ಅನ್ಯಥಾ
	context->pollfd_index = -1;
#endif
	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_ಸಂದೇಶ_in = db.now_s;
	context->next_ಸಂದೇಶ_out = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_start = true;
	context->id = ರಿಕ್ತ;
	context->last_mid = 0;
	context->will = ರಿಕ್ತ;
	context->username = ರಿಕ್ತ;
	context->password = ರಿಕ್ತ;
	context->listener = ರಿಕ್ತ;
	context->acl_list = ರಿಕ್ತ;
	context->retain_available = true;
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	memset(&context->wsd, 0, ಪರಿಮಾಣ(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.mask = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#endif

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->in_packet.payload = ರಿಕ್ತ;
	packet__cleanup(&context->in_packet);
	context->out_packet = ರಿಕ್ತ;
	context->out_packet_ಎಣಿಕೆ = 0;
	context->out_packet_bytes = 0;

	context->address = ರಿಕ್ತ;
	context->bridge = ರಿಕ್ತ;
	context->msgs_in.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_in.inflight_quota = db.config->max_inflight_messages;
	context->msgs_out.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_out.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#ifdef WITH_TLS
	context->ssl = ರಿಕ್ತ;
#endif

	ಪ್ರತ್ಯಾಗಮ context;
}


ಸ್ಥಿರ ಶೂನ್ಯ context__cleanup_out_packets(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ mosquitto__packet *packet;

	ಯದಿ(!context){
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯಾವತ್(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_ಎಣಿಕೆ);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_ಎಣಿಕೆ = 0;
	context->out_packet_bytes = 0;
}


/*
 * This will result in any outgoing packets going unsent. If we're disconnected
 * forcefully then it is usually an error condition and shouldn't be a problem,
 * but it will mean that CONNACK messages will never get sent for bad protocol
 * versions for example.
 */
ಶೂನ್ಯ context__cleanup(ಸಂರಚನಾ mosquitto *context, bool force_free)
{
	ಯದಿ(!context){
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ(force_free){
		context->clean_start = true;
	}

#ifdef WITH_BRIDGE
	ಯದಿ(context->bridge){
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
	ಯದಿ(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->address);

	context__send_will(context);

	ಯದಿ(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->in_packet));
	context__cleanup_out_packets(context);
#ಯದಿ defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	ಯದಿ(context->adns){
		gai_cancel(context->adns);
		ಸಂರಚನಾ addrinfo *ar_request = (ಸಂರಚನಾ addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#endif

#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#endif
	mosquitto_FREE(context->proxy.buf);
	ಯದಿ(force_free){
		mosquitto_FREE(context);
	}
}


ಶೂನ್ಯ context__send_will(ಸಂರಚನಾ mosquitto *ctxt)
{
	ಯದಿ(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		ಯದಿ(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay__add(ctxt);
			ಪ್ರತ್ಯಾಗಮ;
		}

		ಯದಿ(mosquitto_acl_check(ctxt,
				ctxt->will->ಸಂದೇಶ.topic,
				(uint32_t)ctxt->will->ಸಂದೇಶ.payloadlen,
				ctxt->will->ಸಂದೇಶ.payload,
				(uint8_t)ctxt->will->ಸಂದೇಶ.qos,
				ctxt->will->ಸಂದೇಶ.retain,
				ctxt->will->properties,
				MOSQ_ACL_WRITE) == MOSQ_ERR_SUCCESS){

			/* Unexpected disconnect, queue the client will. */
			db__messages_easy_queue(ctxt,
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


ಶೂನ್ಯ context__disconnect(ಸಂರಚನಾ mosquitto *context, ಪೂರ್ಣಾಙ್ಕ reason)
{
	ಯದಿ(mosquitto__get_state(context) == mosq_cs_disconnected){
		ಪ್ರತ್ಯಾಗಮ;
	}

#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	ಯದಿ(context->transport == mosq_t_ws){
		uint8_t buf[4] = {0x88, 0x02, 0x03, context->wsd.disconnect_reason};
		/* Send the disconnect reason, but don't care if it fails */
		ಯದಿ(send(context->sock, buf, 4, 0)){
		}
	}
#endif
	ಯದಿ(context->id){
		ಸಂರಚನಾ mosquitto *context_found;
		HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
		ಯದಿ(context_found == context){
			net__socket_close(context);
			context__add_to_disused(context);
			ಪ್ರತ್ಯಾಗಮ;
		}
	}

	context__send_will(context);

	ಯದಿ(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
		plugin__handle_disconnect(context, reason);
	}ಅನ್ಯಥಾ{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#ifdef WITH_BRIDGE
	ಯದಿ(context->bridge == ರಿಕ್ತ)
	/* Outgoing bridge connection never expire */
#endif
	{
		ಯದಿ(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			ಯದಿ(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context__add_to_disused(context);
			}
		}ಅನ್ಯಥಾ{
			session_expiry__add(context);
		}
	}
	keepalive__remove(context);
	mosquitto__set_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context__cleanup_out_packets(context);
}


ಶೂನ್ಯ context__add_to_disused(ಸಂರಚನಾ mosquitto *context)
{
	ಯದಿ(context->state == mosq_cs_disused){
		ಪ್ರತ್ಯಾಗಮ;
	}

	mosquitto__set_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_next = db.ll_for_free;
	db.ll_for_free = context;
}


ಶೂನ್ಯ context__free_disused(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ mosquitto *context, *next;
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ಸಂರಚನಾ mosquitto *last = ರಿಕ್ತ;
#endif

	context = db.ll_for_free;
	db.ll_for_free = ರಿಕ್ತ;
	ಯಾವತ್(context){
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ಯದಿ(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			ಯದಿ(last){
				last->for_free_next = context;
			}ಅನ್ಯಥಾ{
				db.ll_for_free = context;
			}
			next = context->for_free_next;
			context->for_free_next = ರಿಕ್ತ;
			last = context;
			context = next;
		}ಅನ್ಯಥಾ
#endif
		{
			next = context->for_free_next;
			context__cleanup(context, true);
			context = next;
		}
	}
}


ಶೂನ್ಯ context__add_to_by_id(ಸಂರಚನಾ mosquitto *context)
{
	ಯದಿ(context->in_by_id == false){
		context->in_by_id = true;
		HASH_ಮೌಲ್ಯ(context->id, strlen(context->id), context->id_hashv);
		HASH_ADD_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, strlen(context->id), context->id_hashv, context);
	}
}


ಶೂನ್ಯ context__remove_from_by_id(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ mosquitto *context_found;

	ಯದಿ(!context->id){
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ(context->in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), context_found);
		ಯದಿ(context_found == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_found);
		}
		context->id_hashv = 0;
		context->in_by_id = false;
		ಪ್ರತ್ಯಾಗಮ;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
	ಯದಿ(context_found == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_found);
	}
}
