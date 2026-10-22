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

#شامل "config.h"

#شامل <time.h>
#اگر defined(__APPLE__) || defined(_AIX)
#شامل <sys/socket.h>
#endif

#شامل "mosquitto_broker_internal.h"
#شامل "alias_mosq.h"
#شامل "packet_mosq.h"
#شامل "property_mosq.h"
#شامل "sys_tree.h"
#شامل "util_mosq.h"
#شامل "will_mosq.h"

#شامل "uthash.h"


صحیح context__init_sock(ساختار mosquitto *context, mosq_sock_t sock, bool get_address)
{
	context->sock = sock;

	اگر((صحیح)context->sock >= 0){
		اگر(get_address){
			نویسه address[1024];

			اگر(!net__socket_get_address(context->sock,
					address, اندازه(address),
					&context->remote_port)){

				context->address = mosquitto_strdup(address);
			}
			اگر(!context->address){
				/* getpeername and inet_ntop failed and not a bridge */
				بازگشت MOSQ_ERR_NOMEM;
			}
		}
		HASH_ADD(hh_sock, db.contexts_by_sock, sock, اندازه(context->sock), context);
	}
	بازگشت MOSQ_ERR_SUCCESS;
}

ساختار mosquitto *context__init(تهی)
{
	ساختار mosquitto *context;

	context = mosquitto_calloc(1, اندازه(ساختار mosquitto));
	اگر(!context){
		بازگشت پوچ;
	}

	context->in_packet.packet_buffer_size = db.config->packet_buffer_size;
	context->in_packet.packet_buffer = mosquitto_calloc(1, context->in_packet.packet_buffer_size);
	اگر(!context->in_packet.packet_buffer){
		mosquitto_FREE(context);
		بازگشت پوچ;
	}

#اگر defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#وگرنه
	context->pollfd_index = -1;
#endif
	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_پیام_in = db.now_s;
	context->next_پیام_out = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_start = true;
	context->id = پوچ;
	context->last_mid = 0;
	context->will = پوچ;
	context->username = پوچ;
	context->password = پوچ;
	context->listener = پوچ;
	context->acl_list = پوچ;
	context->retain_available = true;
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	memset(&context->wsd, 0, اندازه(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.mask = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#endif

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->in_packet.payload = پوچ;
	packet__cleanup(&context->in_packet);
	context->out_packet = پوچ;
	context->out_packet_شمار = 0;
	context->out_packet_bytes = 0;

	context->address = پوچ;
	context->bridge = پوچ;
	context->msgs_in.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_in.inflight_quota = db.config->max_inflight_messages;
	context->msgs_out.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_out.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#ifdef WITH_TLS
	context->ssl = پوچ;
#endif

	بازگشت context;
}


ایستا تهی context__cleanup_out_packets(ساختار mosquitto *context)
{
	ساختار mosquitto__packet *packet;

	اگر(!context){
		بازگشت;
	}

	وقتی(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_شمار);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_شمار = 0;
	context->out_packet_bytes = 0;
}


/*
 * This will result in any outgoing packets going unsent. If we're disconnected
 * forcefully then it is usually an error condition and shouldn't be a problem,
 * but it will mean that CONNACK messages will never get sent for bad protocol
 * versions for example.
 */
تهی context__cleanup(ساختار mosquitto *context, bool force_free)
{
	اگر(!context){
		بازگشت;
	}

	اگر(force_free){
		context->clean_start = true;
	}

#ifdef WITH_BRIDGE
	اگر(context->bridge){
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
	اگر(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->address);

	context__send_will(context);

	اگر(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->in_packet));
	context__cleanup_out_packets(context);
#اگر defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	اگر(context->adns){
		gai_cancel(context->adns);
		ساختار addrinfo *ar_request = (ساختار addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#endif

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#endif
	mosquitto_FREE(context->proxy.buf);
	اگر(force_free){
		mosquitto_FREE(context);
	}
}


تهی context__send_will(ساختار mosquitto *ctxt)
{
	اگر(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		اگر(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay__add(ctxt);
			بازگشت;
		}

		اگر(mosquitto_acl_check(ctxt,
				ctxt->will->پیام.topic,
				(uint32_t)ctxt->will->پیام.payloadlen,
				ctxt->will->پیام.payload,
				(uint8_t)ctxt->will->پیام.qos,
				ctxt->will->پیام.retain,
				ctxt->will->properties,
				MOSQ_ACL_WRITE) == MOSQ_ERR_SUCCESS){

			/* Unexpected disconnect, queue the client will. */
			db__messages_easy_queue(ctxt,
					ctxt->will->پیام.topic,
					(uint8_t)ctxt->will->پیام.qos,
					(uint32_t)ctxt->will->پیام.payloadlen,
					ctxt->will->پیام.payload,
					ctxt->will->پیام.retain,
					ctxt->will->expiry_interval,
					&ctxt->will->properties);
		}
	}
	will__clear(ctxt);
}


تهی context__disconnect(ساختار mosquitto *context, صحیح reason)
{
	اگر(mosquitto__get_state(context) == mosq_cs_disconnected){
		بازگشت;
	}

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	اگر(context->transport == mosq_t_ws){
		uint8_t buf[4] = {0x88, 0x02, 0x03, context->wsd.disconnect_reason};
		/* Send the disconnect reason, but don't care if it fails */
		اگر(send(context->sock, buf, 4, 0)){
		}
	}
#endif
	اگر(context->id){
		ساختار mosquitto *context_found;
		HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
		اگر(context_found == context){
			net__socket_close(context);
			context__add_to_disused(context);
			بازگشت;
		}
	}

	context__send_will(context);

	اگر(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
		plugin__handle_disconnect(context, reason);
	}وگرنه{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#ifdef WITH_BRIDGE
	اگر(context->bridge == پوچ)
	/* Outgoing bridge connection never expire */
#endif
	{
		اگر(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			اگر(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context__add_to_disused(context);
			}
		}وگرنه{
			session_expiry__add(context);
		}
	}
	keepalive__remove(context);
	mosquitto__set_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context__cleanup_out_packets(context);
}


تهی context__add_to_disused(ساختار mosquitto *context)
{
	اگر(context->state == mosq_cs_disused){
		بازگشت;
	}

	mosquitto__set_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_next = db.ll_for_free;
	db.ll_for_free = context;
}


تهی context__free_disused(تهی)
{
	ساختار mosquitto *context, *next;
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ساختار mosquitto *last = پوچ;
#endif

	context = db.ll_for_free;
	db.ll_for_free = پوچ;
	وقتی(context){
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		اگر(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			اگر(last){
				last->for_free_next = context;
			}وگرنه{
				db.ll_for_free = context;
			}
			next = context->for_free_next;
			context->for_free_next = پوچ;
			last = context;
			context = next;
		}وگرنه
#endif
		{
			next = context->for_free_next;
			context__cleanup(context, true);
			context = next;
		}
	}
}


تهی context__add_to_by_id(ساختار mosquitto *context)
{
	اگر(context->in_by_id == false){
		context->in_by_id = true;
		HASH_مقدار(context->id, strlen(context->id), context->id_hashv);
		HASH_ADD_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, strlen(context->id), context->id_hashv, context);
	}
}


تهی context__remove_from_by_id(ساختار mosquitto *context)
{
	ساختار mosquitto *context_found;

	اگر(!context->id){
		بازگشت;
	}

	اگر(context->in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), context_found);
		اگر(context_found == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_found);
		}
		context->id_hashv = 0;
		context->in_by_id = false;
		بازگشت;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
	اگر(context_found == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_found);
	}
}
