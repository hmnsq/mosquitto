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

#ضمن "config.h"

#ضمن <time.h>
#אם defined(__APPLE__) || defined(_AIX)
#ضمن <sys/socket.h>
#endif

#ضمن "mosquitto_broker_internal.h"
#ضمن "alias_mosq.h"
#ضمن "packet_mosq.h"
#ضمن "property_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"
#ضمن "will_mosq.h"

#ضمن "uthash.h"


صحيح context__init_sock(بنية mosquitto *context, mosq_sock_t sock, bool get_address)
{
	context->sock = sock;

	אם((صحيح)context->sock >= 0){
		אם(get_address){
			حرف address[1024];

			אם(!net__socket_get_address(context->sock,
					address, حجم(address),
					&context->remote_port)){

				context->address = mosquitto_strdup(address);
			}
			אם(!context->address){
				/* getpeername and inet_ntop failed and not a bridge */
				החזר MOSQ_ERR_NOMEM;
			}
		}
		HASH_ADD(hh_sock, db.contexts_by_sock, sock, حجم(context->sock), context);
	}
	החזר MOSQ_ERR_SUCCESS;
}

بنية mosquitto *context__init(فراغ)
{
	بنية mosquitto *context;

	context = mosquitto_calloc(1, حجم(بنية mosquitto));
	אם(!context){
		החזר عدم;
	}

	context->in_packet.packet_buffer_size = db.config->packet_buffer_size;
	context->in_packet.packet_buffer = mosquitto_calloc(1, context->in_packet.packet_buffer_size);
	אם(!context->in_packet.packet_buffer){
		mosquitto_FREE(context);
		החזר عدم;
	}

#אם defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#אחרת
	context->pollfd_index = -1;
#endif
	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_msg_in = db.now_s;
	context->next_msg_out = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_start = true;
	context->id = عدم;
	context->last_mid = 0;
	context->will = عدم;
	context->username = عدم;
	context->password = عدم;
	context->listener = عدم;
	context->acl_list = عدم;
	context->retain_available = true;
#אם defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	memset(&context->wsd, 0, حجم(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.mask = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#endif

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->in_packet.payload = عدم;
	packet__cleanup(&context->in_packet);
	context->out_packet = عدم;
	context->out_packet_count = 0;
	context->out_packet_bytes = 0;

	context->address = عدم;
	context->bridge = عدم;
	context->msgs_in.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_in.inflight_quota = db.config->max_inflight_messages;
	context->msgs_out.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_out.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#ifdef WITH_TLS
	context->ssl = عدم;
#endif

	החזר context;
}


ساكن فراغ context__cleanup_out_packets(بنية mosquitto *context)
{
	بنية mosquitto__packet *packet;

	אם(!context){
		החזר;
	}

	כלעוד(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_count);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_count = 0;
	context->out_packet_bytes = 0;
}


/*
 * This will result in any outgoing packets going unsent. If we're disconnected
 * forcefully then it is usually an error condition and shouldn't be a problem,
 * but it will mean that CONNACK messages will never get sent for bad protocol
 * versions for example.
 */
فراغ context__cleanup(بنية mosquitto *context, bool force_free)
{
	אם(!context){
		החזר;
	}

	אם(force_free){
		context->clean_start = true;
	}

#ifdef WITH_BRIDGE
	אם(context->bridge){
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
	אם(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->address);

	context__send_will(context);

	אם(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->in_packet));
	context__cleanup_out_packets(context);
#אם defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	אם(context->adns){
		gai_cancel(context->adns);
		بنية addrinfo *ar_request = (بنية addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#endif

#אם defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#endif
	mosquitto_FREE(context->proxy.buf);
	אם(force_free){
		mosquitto_FREE(context);
	}
}


فراغ context__send_will(بنية mosquitto *ctxt)
{
	אם(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		אם(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay__add(ctxt);
			החזר;
		}

		אם(mosquitto_acl_check(ctxt,
				ctxt->will->msg.topic,
				(uint32_t)ctxt->will->msg.payloadlen,
				ctxt->will->msg.payload,
				(uint8_t)ctxt->will->msg.qos,
				ctxt->will->msg.retain,
				ctxt->will->properties,
				MOSQ_ACL_WRITE) == MOSQ_ERR_SUCCESS){

			/* Unexpected disconnect, queue the client will. */
			db__messages_easy_queue(ctxt,
					ctxt->will->msg.topic,
					(uint8_t)ctxt->will->msg.qos,
					(uint32_t)ctxt->will->msg.payloadlen,
					ctxt->will->msg.payload,
					ctxt->will->msg.retain,
					ctxt->will->expiry_interval,
					&ctxt->will->properties);
		}
	}
	will__clear(ctxt);
}


فراغ context__disconnect(بنية mosquitto *context, صحيح reason)
{
	אם(mosquitto__get_state(context) == mosq_cs_disconnected){
		החזר;
	}

#אם defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	אם(context->transport == mosq_t_ws){
		uint8_t buf[4] = {0x88, 0x02, 0x03, context->wsd.disconnect_reason};
		/* Send the disconnect reason, but don't care if it fails */
		אם(send(context->sock, buf, 4, 0)){
		}
	}
#endif
	אם(context->id){
		بنية mosquitto *context_found;
		HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
		אם(context_found == context){
			net__socket_close(context);
			context__add_to_disused(context);
			החזר;
		}
	}

	context__send_will(context);

	אם(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
		plugin__handle_disconnect(context, reason);
	}אחרת{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#ifdef WITH_BRIDGE
	אם(context->bridge == عدم)
	/* Outgoing bridge connection never expire */
#endif
	{
		אם(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			אם(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context__add_to_disused(context);
			}
		}אחרת{
			session_expiry__add(context);
		}
	}
	keepalive__remove(context);
	mosquitto__set_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context__cleanup_out_packets(context);
}


فراغ context__add_to_disused(بنية mosquitto *context)
{
	אם(context->state == mosq_cs_disused){
		החזר;
	}

	mosquitto__set_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_next = db.ll_for_free;
	db.ll_for_free = context;
}


فراغ context__free_disused(فراغ)
{
	بنية mosquitto *context, *next;
#אם defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	بنية mosquitto *last = عدم;
#endif

	context = db.ll_for_free;
	db.ll_for_free = عدم;
	כלעוד(context){
#אם defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		אם(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			אם(last){
				last->for_free_next = context;
			}אחרת{
				db.ll_for_free = context;
			}
			next = context->for_free_next;
			context->for_free_next = عدم;
			last = context;
			context = next;
		}אחרת
#endif
		{
			next = context->for_free_next;
			context__cleanup(context, true);
			context = next;
		}
	}
}


فراغ context__add_to_by_id(بنية mosquitto *context)
{
	אם(context->in_by_id == false){
		context->in_by_id = true;
		HASH_VALUE(context->id, strlen(context->id), context->id_hashv);
		HASH_ADD_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, strlen(context->id), context->id_hashv, context);
	}
}


فراغ context__remove_from_by_id(بنية mosquitto *context)
{
	بنية mosquitto *context_found;

	אם(!context->id){
		החזר;
	}

	אם(context->in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), context_found);
		אם(context_found == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_found);
		}
		context->id_hashv = 0;
		context->in_by_id = false;
		החזר;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
	אם(context_found == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_found);
	}
}
