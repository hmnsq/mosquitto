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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <time.h>
#যদি defined(__APPLE__) || defined(_AIX)
#অন্তর্ভাব <sys/socket.h>
#endif

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "alias_mosq.h"
#অন্তর্ভাব "packet_mosq.h"
#অন্তর্ভাব "property_mosq.h"
#অন্তর্ভাব "sys_tree.h"
#অন্তর্ভাব "util_mosq.h"
#অন্তর্ভাব "will_mosq.h"

#অন্তর্ভাব "uthash.h"


পূর্ণাঙ্ক context__init_sock(সংরচনা mosquitto *context, mosq_sock_t sock, bool get_address)
{
	context->sock = sock;

	যদি((পূর্ণাঙ্ক)context->sock >= 0){
		যদি(get_address){
			বর্ণ address[1024];

			যদি(!net__socket_get_address(context->sock,
					address, পরিমাণ(address),
					&context->remote_port)){

				context->address = mosquitto_strdup(address);
			}
			যদি(!context->address){
				/* getpeername and inet_ntop failed and not a bridge */
				প্রত্যাগম MOSQ_ERR_NOMEM;
			}
		}
		HASH_ADD(hh_sock, db.contexts_by_sock, sock, পরিমাণ(context->sock), context);
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}

সংরচনা mosquitto *context__init(শূন্য)
{
	সংরচনা mosquitto *context;

	context = mosquitto_calloc(1, পরিমাণ(সংরচনা mosquitto));
	যদি(!context){
		প্রত্যাগম রিক্ত;
	}

	context->in_packet.packet_buffer_size = db.config->packet_buffer_size;
	context->in_packet.packet_buffer = mosquitto_calloc(1, context->in_packet.packet_buffer_size);
	যদি(!context->in_packet.packet_buffer){
		mosquitto_FREE(context);
		প্রত্যাগম রিক্ত;
	}

#যদি defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#অন্যথা
	context->pollfd_index = -1;
#endif
	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_বার্তা_in = db.now_s;
	context->next_বার্তা_out = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_start = true;
	context->id = রিক্ত;
	context->last_mid = 0;
	context->will = রিক্ত;
	context->username = রিক্ত;
	context->password = রিক্ত;
	context->listener = রিক্ত;
	context->acl_list = রিক্ত;
	context->retain_available = true;
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	memset(&context->wsd, 0, পরিমাণ(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.mask = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#endif

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->in_packet.payload = রিক্ত;
	packet__cleanup(&context->in_packet);
	context->out_packet = রিক্ত;
	context->out_packet_গুনতি = 0;
	context->out_packet_bytes = 0;

	context->address = রিক্ত;
	context->bridge = রিক্ত;
	context->msgs_in.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_in.inflight_quota = db.config->max_inflight_messages;
	context->msgs_out.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_out.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#ifdef WITH_TLS
	context->ssl = রিক্ত;
#endif

	প্রত্যাগম context;
}


স্থির শূন্য context__cleanup_out_packets(সংরচনা mosquitto *context)
{
	সংরচনা mosquitto__packet *packet;

	যদি(!context){
		প্রত্যাগম;
	}

	যাবত্(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_গুনতি);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_গুনতি = 0;
	context->out_packet_bytes = 0;
}


/*
 * This will result in any outgoing packets going unsent. If we're disconnected
 * forcefully then it is usually an error condition and shouldn't be a problem,
 * but it will mean that CONNACK messages will never get sent for bad protocol
 * versions for example.
 */
শূন্য context__cleanup(সংরচনা mosquitto *context, bool force_free)
{
	যদি(!context){
		প্রত্যাগম;
	}

	যদি(force_free){
		context->clean_start = true;
	}

#ifdef WITH_BRIDGE
	যদি(context->bridge){
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
	যদি(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->address);

	context__send_will(context);

	যদি(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->in_packet));
	context__cleanup_out_packets(context);
#যদি defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	যদি(context->adns){
		gai_cancel(context->adns);
		সংরচনা addrinfo *ar_request = (সংরচনা addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#endif

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#endif
	mosquitto_FREE(context->proxy.buf);
	যদি(force_free){
		mosquitto_FREE(context);
	}
}


শূন্য context__send_will(সংরচনা mosquitto *ctxt)
{
	যদি(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		যদি(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay__add(ctxt);
			প্রত্যাগম;
		}

		যদি(mosquitto_acl_check(ctxt,
				ctxt->will->বার্তা.topic,
				(uint32_t)ctxt->will->বার্তা.payloadlen,
				ctxt->will->বার্তা.payload,
				(uint8_t)ctxt->will->বার্তা.qos,
				ctxt->will->বার্তা.retain,
				ctxt->will->properties,
				MOSQ_ACL_WRITE) == MOSQ_ERR_SUCCESS){

			/* Unexpected disconnect, queue the client will. */
			db__messages_easy_queue(ctxt,
					ctxt->will->বার্তা.topic,
					(uint8_t)ctxt->will->বার্তা.qos,
					(uint32_t)ctxt->will->বার্তা.payloadlen,
					ctxt->will->বার্তা.payload,
					ctxt->will->বার্তা.retain,
					ctxt->will->expiry_interval,
					&ctxt->will->properties);
		}
	}
	will__clear(ctxt);
}


শূন্য context__disconnect(সংরচনা mosquitto *context, পূর্ণাঙ্ক reason)
{
	যদি(mosquitto__get_state(context) == mosq_cs_disconnected){
		প্রত্যাগম;
	}

#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	যদি(context->transport == mosq_t_ws){
		uint8_t buf[4] = {0x88, 0x02, 0x03, context->wsd.disconnect_reason};
		/* Send the disconnect reason, but don't care if it fails */
		যদি(send(context->sock, buf, 4, 0)){
		}
	}
#endif
	যদি(context->id){
		সংরচনা mosquitto *context_found;
		HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
		যদি(context_found == context){
			net__socket_close(context);
			context__add_to_disused(context);
			প্রত্যাগম;
		}
	}

	context__send_will(context);

	যদি(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
		plugin__handle_disconnect(context, reason);
	}অন্যথা{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#ifdef WITH_BRIDGE
	যদি(context->bridge == রিক্ত)
	/* Outgoing bridge connection never expire */
#endif
	{
		যদি(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			যদি(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context__add_to_disused(context);
			}
		}অন্যথা{
			session_expiry__add(context);
		}
	}
	keepalive__remove(context);
	mosquitto__set_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context__cleanup_out_packets(context);
}


শূন্য context__add_to_disused(সংরচনা mosquitto *context)
{
	যদি(context->state == mosq_cs_disused){
		প্রত্যাগম;
	}

	mosquitto__set_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_next = db.ll_for_free;
	db.ll_for_free = context;
}


শূন্য context__free_disused(শূন্য)
{
	সংরচনা mosquitto *context, *next;
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	সংরচনা mosquitto *last = রিক্ত;
#endif

	context = db.ll_for_free;
	db.ll_for_free = রিক্ত;
	যাবত্(context){
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		যদি(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			যদি(last){
				last->for_free_next = context;
			}অন্যথা{
				db.ll_for_free = context;
			}
			next = context->for_free_next;
			context->for_free_next = রিক্ত;
			last = context;
			context = next;
		}অন্যথা
#endif
		{
			next = context->for_free_next;
			context__cleanup(context, true);
			context = next;
		}
	}
}


শূন্য context__add_to_by_id(সংরচনা mosquitto *context)
{
	যদি(context->in_by_id == false){
		context->in_by_id = true;
		HASH_মান(context->id, strlen(context->id), context->id_hashv);
		HASH_ADD_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, strlen(context->id), context->id_hashv, context);
	}
}


শূন্য context__remove_from_by_id(সংরচনা mosquitto *context)
{
	সংরচনা mosquitto *context_found;

	যদি(!context->id){
		প্রত্যাগম;
	}

	যদি(context->in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), context_found);
		যদি(context_found == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_found);
		}
		context->id_hashv = 0;
		context->in_by_id = false;
		প্রত্যাগম;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
	যদি(context_found == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_found);
	}
}
