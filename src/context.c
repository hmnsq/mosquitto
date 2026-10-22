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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <time.h>
#എങ്കിൽ defined(__APPLE__) || defined(_AIX)
#അന്തര്ഭാവ <sys/socket.h>
#endif

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "alias_mosq.h"
#അന്തര്ഭാവ "packet_mosq.h"
#അന്തര്ഭാവ "property_mosq.h"
#അന്തര്ഭാവ "sys_tree.h"
#അന്തര്ഭാവ "util_mosq.h"
#അന്തര്ഭാവ "will_mosq.h"

#അന്തര്ഭാവ "uthash.h"


പൂർണ്ണസംഖ്യ context__init_sock(ഘടന mosquitto *context, mosq_sock_t sock, bool get_address)
{
	context->sock = sock;

	എങ്കിൽ((പൂർണ്ണസംഖ്യ)context->sock >= 0){
		എങ്കിൽ(get_address){
			അക്ഷരം address[1024];

			എങ്കിൽ(!net__socket_get_address(context->sock,
					address, വലുപ്പം(address),
					&context->remote_port)){

				context->address = mosquitto_strdup(address);
			}
			എങ്കിൽ(!context->address){
				/* getpeername and inet_ntop failed and not a bridge */
				തിരികെ MOSQ_ERR_NOMEM;
			}
		}
		HASH_ADD(hh_sock, db.contexts_by_sock, sock, വലുപ്പം(context->sock), context);
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}

ഘടന mosquitto *context__init(ശൂന്യം)
{
	ഘടന mosquitto *context;

	context = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto));
	എങ്കിൽ(!context){
		തിരികെ ശൂന്യ;
	}

	context->in_packet.packet_buffer_size = db.config->packet_buffer_size;
	context->in_packet.packet_buffer = mosquitto_calloc(1, context->in_packet.packet_buffer_size);
	എങ്കിൽ(!context->in_packet.packet_buffer){
		mosquitto_FREE(context);
		തിരികെ ശൂന്യ;
	}

#എങ്കിൽ defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#അല്ലെങ്കിൽ
	context->pollfd_index = -1;
#endif
	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_സന്ദേശം_in = db.now_s;
	context->next_സന്ദേശം_out = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_start = true;
	context->id = ശൂന്യ;
	context->last_mid = 0;
	context->will = ശൂന്യ;
	context->username = ശൂന്യ;
	context->password = ശൂന്യ;
	context->listener = ശൂന്യ;
	context->acl_list = ശൂന്യ;
	context->retain_available = true;
#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	memset(&context->wsd, 0, വലുപ്പം(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.mask = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#endif

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->in_packet.payload = ശൂന്യ;
	packet__cleanup(&context->in_packet);
	context->out_packet = ശൂന്യ;
	context->out_packet_എണ്ണം = 0;
	context->out_packet_bytes = 0;

	context->address = ശൂന്യ;
	context->bridge = ശൂന്യ;
	context->msgs_in.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_in.inflight_quota = db.config->max_inflight_messages;
	context->msgs_out.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_out.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#ifdef WITH_TLS
	context->ssl = ശൂന്യ;
#endif

	തിരികെ context;
}


സ്ഥിതം ശൂന്യം context__cleanup_out_packets(ഘടന mosquitto *context)
{
	ഘടന mosquitto__packet *packet;

	എങ്കിൽ(!context){
		തിരികെ;
	}

	ആകുമ്പോൾ(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_എണ്ണം);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_എണ്ണം = 0;
	context->out_packet_bytes = 0;
}


/*
 * This will result in any outgoing packets going unsent. If we're disconnected
 * forcefully then it is usually an error condition and shouldn't be a problem,
 * but it will mean that CONNACK messages will never get sent for bad protocol
 * versions for example.
 */
ശൂന്യം context__cleanup(ഘടന mosquitto *context, bool force_free)
{
	എങ്കിൽ(!context){
		തിരികെ;
	}

	എങ്കിൽ(force_free){
		context->clean_start = true;
	}

#ifdef WITH_BRIDGE
	എങ്കിൽ(context->bridge){
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
	എങ്കിൽ(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->address);

	context__send_will(context);

	എങ്കിൽ(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->in_packet));
	context__cleanup_out_packets(context);
#എങ്കിൽ defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	എങ്കിൽ(context->adns){
		gai_cancel(context->adns);
		ഘടന addrinfo *ar_request = (ഘടന addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#endif

#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#endif
	mosquitto_FREE(context->proxy.buf);
	എങ്കിൽ(force_free){
		mosquitto_FREE(context);
	}
}


ശൂന്യം context__send_will(ഘടന mosquitto *ctxt)
{
	എങ്കിൽ(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		എങ്കിൽ(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay__add(ctxt);
			തിരികെ;
		}

		എങ്കിൽ(mosquitto_acl_check(ctxt,
				ctxt->will->സന്ദേശം.topic,
				(uint32_t)ctxt->will->സന്ദേശം.payloadlen,
				ctxt->will->സന്ദേശം.payload,
				(uint8_t)ctxt->will->സന്ദേശം.qos,
				ctxt->will->സന്ദേശം.retain,
				ctxt->will->properties,
				MOSQ_ACL_WRITE) == MOSQ_ERR_SUCCESS){

			/* Unexpected disconnect, queue the client will. */
			db__messages_easy_queue(ctxt,
					ctxt->will->സന്ദേശം.topic,
					(uint8_t)ctxt->will->സന്ദേശം.qos,
					(uint32_t)ctxt->will->സന്ദേശം.payloadlen,
					ctxt->will->സന്ദേശം.payload,
					ctxt->will->സന്ദേശം.retain,
					ctxt->will->expiry_interval,
					&ctxt->will->properties);
		}
	}
	will__clear(ctxt);
}


ശൂന്യം context__disconnect(ഘടന mosquitto *context, പൂർണ്ണസംഖ്യ reason)
{
	എങ്കിൽ(mosquitto__get_state(context) == mosq_cs_disconnected){
		തിരികെ;
	}

#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	എങ്കിൽ(context->transport == mosq_t_ws){
		uint8_t buf[4] = {0x88, 0x02, 0x03, context->wsd.disconnect_reason};
		/* Send the disconnect reason, but don't care if it fails */
		എങ്കിൽ(send(context->sock, buf, 4, 0)){
		}
	}
#endif
	എങ്കിൽ(context->id){
		ഘടന mosquitto *context_found;
		HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
		എങ്കിൽ(context_found == context){
			net__socket_close(context);
			context__add_to_disused(context);
			തിരികെ;
		}
	}

	context__send_will(context);

	എങ്കിൽ(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
		plugin__handle_disconnect(context, reason);
	}അല്ലെങ്കിൽ{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#ifdef WITH_BRIDGE
	എങ്കിൽ(context->bridge == ശൂന്യ)
	/* Outgoing bridge connection never expire */
#endif
	{
		എങ്കിൽ(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			എങ്കിൽ(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context__add_to_disused(context);
			}
		}അല്ലെങ്കിൽ{
			session_expiry__add(context);
		}
	}
	keepalive__remove(context);
	mosquitto__set_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context__cleanup_out_packets(context);
}


ശൂന്യം context__add_to_disused(ഘടന mosquitto *context)
{
	എങ്കിൽ(context->state == mosq_cs_disused){
		തിരികെ;
	}

	mosquitto__set_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_next = db.ll_for_free;
	db.ll_for_free = context;
}


ശൂന്യം context__free_disused(ശൂന്യം)
{
	ഘടന mosquitto *context, *next;
#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ഘടന mosquitto *last = ശൂന്യ;
#endif

	context = db.ll_for_free;
	db.ll_for_free = ശൂന്യ;
	ആകുമ്പോൾ(context){
#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		എങ്കിൽ(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			എങ്കിൽ(last){
				last->for_free_next = context;
			}അല്ലെങ്കിൽ{
				db.ll_for_free = context;
			}
			next = context->for_free_next;
			context->for_free_next = ശൂന്യ;
			last = context;
			context = next;
		}അല്ലെങ്കിൽ
#endif
		{
			next = context->for_free_next;
			context__cleanup(context, true);
			context = next;
		}
	}
}


ശൂന്യം context__add_to_by_id(ഘടന mosquitto *context)
{
	എങ്കിൽ(context->in_by_id == false){
		context->in_by_id = true;
		HASH_മൂല്യം(context->id, strlen(context->id), context->id_hashv);
		HASH_ADD_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, strlen(context->id), context->id_hashv, context);
	}
}


ശൂന്യം context__remove_from_by_id(ഘടന mosquitto *context)
{
	ഘടന mosquitto *context_found;

	എങ്കിൽ(!context->id){
		തിരികെ;
	}

	എങ്കിൽ(context->in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), context_found);
		എങ്കിൽ(context_found == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_found);
		}
		context->id_hashv = 0;
		context->in_by_id = false;
		തിരികെ;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
	എങ്കിൽ(context_found == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_found);
	}
}
