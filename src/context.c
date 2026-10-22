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

#अन्तर्भाव "config.h"

#अन्तर्भाव <time.h>
#यदि defined(__APPLE__) || defined(_AIX)
#अन्तर्भाव <sys/socket.h>
#endif

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "alias_mosq.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "property_mosq.h"
#अन्तर्भाव "sys_tree.h"
#अन्तर्भाव "util_mosq.h"
#अन्तर्भाव "will_mosq.h"

#अन्तर्भाव "uthash.h"


पूर्णाङ्क context__init_sock(संरचना mosquitto *context, mosq_sock_t sock, bool get_address)
{
	context->sock = sock;

	यदि((पूर्णाङ्क)context->sock >= 0){
		यदि(get_address){
			वर्ण address[1024];

			यदि(!net__socket_get_address(context->sock,
					address, परिमाण(address),
					&context->remote_port)){

				context->address = mosquitto_strdup(address);
			}
			यदि(!context->address){
				/* getpeername and inet_ntop failed and not a bridge */
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
		}
		HASH_ADD(hh_sock, db.contexts_by_sock, sock, परिमाण(context->sock), context);
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}

संरचना mosquitto *context__init(शून्य)
{
	संरचना mosquitto *context;

	context = mosquitto_calloc(1, परिमाण(संरचना mosquitto));
	यदि(!context){
		प्रत्यागम रिक्त;
	}

	context->in_packet.packet_buffer_size = db.config->packet_buffer_size;
	context->in_packet.packet_buffer = mosquitto_calloc(1, context->in_packet.packet_buffer_size);
	यदि(!context->in_packet.packet_buffer){
		mosquitto_FREE(context);
		प्रत्यागम रिक्त;
	}

#यदि defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#अन्यथा
	context->pollfd_index = -1;
#endif
	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_संदेश_in = db.now_s;
	context->next_संदेश_out = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_start = true;
	context->id = रिक्त;
	context->last_mid = 0;
	context->will = रिक्त;
	context->username = रिक्त;
	context->password = रिक्त;
	context->listener = रिक्त;
	context->acl_list = रिक्त;
	context->retain_available = true;
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	memset(&context->wsd, 0, परिमाण(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.mask = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#endif

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->in_packet.payload = रिक्त;
	packet__cleanup(&context->in_packet);
	context->out_packet = रिक्त;
	context->out_packet_मोजणी = 0;
	context->out_packet_bytes = 0;

	context->address = रिक्त;
	context->bridge = रिक्त;
	context->msgs_in.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_in.inflight_quota = db.config->max_inflight_messages;
	context->msgs_out.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_out.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#ifdef WITH_TLS
	context->ssl = रिक्त;
#endif

	प्रत्यागम context;
}


स्थिर शून्य context__cleanup_out_packets(संरचना mosquitto *context)
{
	संरचना mosquitto__packet *packet;

	यदि(!context){
		प्रत्यागम;
	}

	यावत्(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_free(packet);
	}
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_मोजणी);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_मोजणी = 0;
	context->out_packet_bytes = 0;
}


/*
 * This will result in any outgoing packets going unsent. If we're disconnected
 * forcefully then it is usually an error condition and shouldn't be a problem,
 * but it will mean that CONNACK messages will never get sent for bad protocol
 * versions for example.
 */
शून्य context__cleanup(संरचना mosquitto *context, bool force_free)
{
	यदि(!context){
		प्रत्यागम;
	}

	यदि(force_free){
		context->clean_start = true;
	}

#ifdef WITH_BRIDGE
	यदि(context->bridge){
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
	यदि(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->address);

	context__send_will(context);

	यदि(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->in_packet));
	context__cleanup_out_packets(context);
#यदि defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	यदि(context->adns){
		gai_cancel(context->adns);
		संरचना addrinfo *ar_request = (संरचना addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#endif

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#endif
	mosquitto_FREE(context->proxy.buf);
	यदि(force_free){
		mosquitto_FREE(context);
	}
}


शून्य context__send_will(संरचना mosquitto *ctxt)
{
	यदि(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		यदि(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay__add(ctxt);
			प्रत्यागम;
		}

		यदि(mosquitto_acl_check(ctxt,
				ctxt->will->संदेश.topic,
				(uint32_t)ctxt->will->संदेश.payloadlen,
				ctxt->will->संदेश.payload,
				(uint8_t)ctxt->will->संदेश.qos,
				ctxt->will->संदेश.retain,
				ctxt->will->properties,
				MOSQ_ACL_WRITE) == MOSQ_ERR_SUCCESS){

			/* Unexpected disconnect, queue the client will. */
			db__messages_easy_queue(ctxt,
					ctxt->will->संदेश.topic,
					(uint8_t)ctxt->will->संदेश.qos,
					(uint32_t)ctxt->will->संदेश.payloadlen,
					ctxt->will->संदेश.payload,
					ctxt->will->संदेश.retain,
					ctxt->will->expiry_interval,
					&ctxt->will->properties);
		}
	}
	will__clear(ctxt);
}


शून्य context__disconnect(संरचना mosquitto *context, पूर्णाङ्क reason)
{
	यदि(mosquitto__get_state(context) == mosq_cs_disconnected){
		प्रत्यागम;
	}

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	यदि(context->transport == mosq_t_ws){
		uint8_t buf[4] = {0x88, 0x02, 0x03, context->wsd.disconnect_reason};
		/* Send the disconnect reason, but don't care if it fails */
		यदि(send(context->sock, buf, 4, 0)){
		}
	}
#endif
	यदि(context->id){
		संरचना mosquitto *context_found;
		HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
		यदि(context_found == context){
			net__socket_close(context);
			context__add_to_disused(context);
			प्रत्यागम;
		}
	}

	context__send_will(context);

	यदि(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
		plugin__handle_disconnect(context, reason);
	}अन्यथा{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#ifdef WITH_BRIDGE
	यदि(context->bridge == रिक्त)
	/* Outgoing bridge connection never expire */
#endif
	{
		यदि(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			यदि(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context__add_to_disused(context);
			}
		}अन्यथा{
			session_expiry__add(context);
		}
	}
	keepalive__remove(context);
	mosquitto__set_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context__cleanup_out_packets(context);
}


शून्य context__add_to_disused(संरचना mosquitto *context)
{
	यदि(context->state == mosq_cs_disused){
		प्रत्यागम;
	}

	mosquitto__set_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_next = db.ll_for_free;
	db.ll_for_free = context;
}


शून्य context__free_disused(शून्य)
{
	संरचना mosquitto *context, *next;
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	संरचना mosquitto *last = रिक्त;
#endif

	context = db.ll_for_free;
	db.ll_for_free = रिक्त;
	यावत्(context){
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		यदि(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			यदि(last){
				last->for_free_next = context;
			}अन्यथा{
				db.ll_for_free = context;
			}
			next = context->for_free_next;
			context->for_free_next = रिक्त;
			last = context;
			context = next;
		}अन्यथा
#endif
		{
			next = context->for_free_next;
			context__cleanup(context, true);
			context = next;
		}
	}
}


शून्य context__add_to_by_id(संरचना mosquitto *context)
{
	यदि(context->in_by_id == false){
		context->in_by_id = true;
		HASH_मूल्य(context->id, strlen(context->id), context->id_hashv);
		HASH_ADD_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, strlen(context->id), context->id_hashv, context);
	}
}


शून्य context__remove_from_by_id(संरचना mosquitto *context)
{
	संरचना mosquitto *context_found;

	यदि(!context->id){
		प्रत्यागम;
	}

	यदि(context->in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), context_found);
		यदि(context_found == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_found);
		}
		context->id_hashv = 0;
		context->in_by_id = false;
		प्रत्यागम;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
	यदि(context_found == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_found);
	}
}
