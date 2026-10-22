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
#إذا defined(__APPLE__) || defined(_AIX)
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

	إذا((صحيح)context->sock >= 0){
		إذا(get_address){
			حرف address[1024];

			إذا(!net__socket_get_address(context->sock,
					address, حجم(address),
					&context->remote_port)){

				context->address = mosquitto_strdup(address);
			}
			إذا(!context->address){
				/* getpeername and inet_ntop failed and not a bridge */
				أرجع MOSQ_ERR_NOMEM;
			}
		}
		HASH_ADD(hh_sock, db.contexts_by_sock, sock, حجم(context->sock), context);
	}
	أرجع MOSQ_ERR_SUCCESS;
}

بنية mosquitto *context__init(فراغ)
{
	بنية mosquitto *context;

	context = mosquitto_calloc(1, حجم(بنية mosquitto));
	إذا(!context){
		أرجع عدم;
	}

	context->in_packet.packet_buffer_size = db.config->packet_buffer_size;
	context->in_packet.packet_buffer = mosquitto_calloc(1, context->in_packet.packet_buffer_size);
	إذا(!context->in_packet.packet_buffer){
		mosquitto_FREE(context);
		أرجع عدم;
	}

#إذا defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#وإلا
	context->pollfd_index = -1;
#endif
	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_رسالة_in = db.now_s;
	context->next_رسالة_out = db.now_s + 20;
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
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
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
	context->out_packet_عدد = 0;
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

	أرجع context;
}


ساكن فراغ context__cleanup_out_packets(بنية mosquitto *context)
{
	بنية mosquitto__packet *packet;

	إذا(!context){
		أرجع;
	}

	طالما(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_عدد);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_عدد = 0;
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
	إذا(!context){
		أرجع;
	}

	إذا(force_free){
		context->clean_start = true;
	}

#ifdef WITH_BRIDGE
	إذا(context->bridge){
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
	إذا(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->address);

	context__send_will(context);

	إذا(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->in_packet));
	context__cleanup_out_packets(context);
#إذا defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	إذا(context->adns){
		gai_cancel(context->adns);
		بنية addrinfo *ar_request = (بنية addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#endif

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#endif
	mosquitto_FREE(context->proxy.buf);
	إذا(force_free){
		mosquitto_FREE(context);
	}
}


فراغ context__send_will(بنية mosquitto *ctxt)
{
	إذا(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		إذا(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay__add(ctxt);
			أرجع;
		}

		إذا(mosquitto_acl_check(ctxt,
				ctxt->will->رسالة.topic,
				(uint32_t)ctxt->will->رسالة.payloadlen,
				ctxt->will->رسالة.payload,
				(uint8_t)ctxt->will->رسالة.qos,
				ctxt->will->رسالة.retain,
				ctxt->will->properties,
				MOSQ_ACL_WRITE) == MOSQ_ERR_SUCCESS){

			/* Unexpected disconnect, queue the client will. */
			db__messages_easy_queue(ctxt,
					ctxt->will->رسالة.topic,
					(uint8_t)ctxt->will->رسالة.qos,
					(uint32_t)ctxt->will->رسالة.payloadlen,
					ctxt->will->رسالة.payload,
					ctxt->will->رسالة.retain,
					ctxt->will->expiry_interval,
					&ctxt->will->properties);
		}
	}
	will__clear(ctxt);
}


فراغ context__disconnect(بنية mosquitto *context, صحيح reason)
{
	إذا(mosquitto__get_state(context) == mosq_cs_disconnected){
		أرجع;
	}

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	إذا(context->transport == mosq_t_ws){
		uint8_t buf[4] = {0x88, 0x02, 0x03, context->wsd.disconnect_reason};
		/* Send the disconnect reason, but don't care if it fails */
		إذا(send(context->sock, buf, 4, 0)){
		}
	}
#endif
	إذا(context->id){
		بنية mosquitto *context_found;
		HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
		إذا(context_found == context){
			net__socket_close(context);
			context__add_to_disused(context);
			أرجع;
		}
	}

	context__send_will(context);

	إذا(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
		plugin__handle_disconnect(context, reason);
	}وإلا{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#ifdef WITH_BRIDGE
	إذا(context->bridge == عدم)
	/* Outgoing bridge connection never expire */
#endif
	{
		إذا(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			إذا(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context__add_to_disused(context);
			}
		}وإلا{
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
	إذا(context->state == mosq_cs_disused){
		أرجع;
	}

	mosquitto__set_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_next = db.ll_for_free;
	db.ll_for_free = context;
}


فراغ context__free_disused(فراغ)
{
	بنية mosquitto *context, *next;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	بنية mosquitto *last = عدم;
#endif

	context = db.ll_for_free;
	db.ll_for_free = عدم;
	طالما(context){
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		إذا(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			إذا(last){
				last->for_free_next = context;
			}وإلا{
				db.ll_for_free = context;
			}
			next = context->for_free_next;
			context->for_free_next = عدم;
			last = context;
			context = next;
		}وإلا
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
	إذا(context->in_by_id == false){
		context->in_by_id = true;
		HASH_قيمة(context->id, strlen(context->id), context->id_hashv);
		HASH_ADD_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, strlen(context->id), context->id_hashv, context);
	}
}


فراغ context__remove_from_by_id(بنية mosquitto *context)
{
	بنية mosquitto *context_found;

	إذا(!context->id){
		أرجع;
	}

	إذا(context->in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), context_found);
		إذا(context_found == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_found);
		}
		context->id_hashv = 0;
		context->in_by_id = false;
		أرجع;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
	إذا(context_found == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_found);
	}
}
