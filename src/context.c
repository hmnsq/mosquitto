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

#include "config.h"

#include <time.h>
#如果 defined(__APPLE__) || defined(_AIX)
#include <sys/socket.h>
#endif

#include "mosquitto_broker_internal.h"
#include "alias_mosq.h"
#include "packet_mosq.h"
#include "property_mosq.h"
#include "sys_tree.h"
#include "util_mosq.h"
#include "will_mosq.h"

#include "uthash.h"


整数 context__init_sock(struct mosquitto *context, mosq_sock_t sock, bool get_address)
{
	context->sock = sock;

	如果((整数)context->sock >= 0){
		如果(get_address){
			char address[1024];

			如果(!net__socket_get_address(context->sock,
					address, sizeof(address),
					&context->remote_port)){

				context->address = mosquitto_strdup(address);
			}
			如果(!context->address){
				/* getpeername and inet_ntop failed and not a bridge */
				返回 MOSQ_ERR_NOMEM;
			}
		}
		HASH_ADD(hh_sock, db.contexts_by_sock, sock, sizeof(context->sock), context);
	}
	返回 MOSQ_ERR_SUCCESS;
}

struct mosquitto *context__init(void)
{
	struct mosquitto *context;

	context = mosquitto_calloc(1, sizeof(struct mosquitto));
	如果(!context){
		返回 NULL;
	}

	context->in_packet.packet_buffer_size = db.config->packet_buffer_size;
	context->in_packet.packet_buffer = mosquitto_calloc(1, context->in_packet.packet_buffer_size);
	如果(!context->in_packet.packet_buffer){
		mosquitto_FREE(context);
		返回 NULL;
	}

#如果 defined(WITH_EPOLL) || defined(WITH_KQUEUE)
	context->ident = id_client;
#否则
	context->pollfd_index = -1;
#endif
	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_msg_in = db.now_s;
	context->next_msg_out = db.now_s + 20;
	context->keepalive = 20; /* Default to 20s */
	context->clean_start = true;
	context->id = NULL;
	context->last_mid = 0;
	context->will = NULL;
	context->username = NULL;
	context->password = NULL;
	context->listener = NULL;
	context->acl_list = NULL;
	context->retain_available = true;
#如果 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	memset(&context->wsd, 0, sizeof(context->wsd));
	context->wsd.opcode = UINT8_MAX;
	context->wsd.mask = UINT8_MAX;
	context->wsd.disconnect_reason = 0xE8;
#endif

	/* is_bridge records whether this client is a bridge or not. This could be
	 * done by looking at context->bridge for bridges that we create ourself,
	 * but incoming bridges need some other way of being recorded. */
	context->is_bridge = false;

	context->in_packet.payload = NULL;
	packet__cleanup(&context->in_packet);
	context->out_packet = NULL;
	context->out_packet_count = 0;
	context->out_packet_bytes = 0;

	context->address = NULL;
	context->bridge = NULL;
	context->msgs_in.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_in.inflight_quota = db.config->max_inflight_messages;
	context->msgs_out.inflight_maximum = db.config->max_inflight_messages;
	context->msgs_out.inflight_quota = db.config->max_inflight_messages;
	context->max_qos = 2;
#ifdef WITH_TLS
	context->ssl = NULL;
#endif

	返回 context;
}


static void context__cleanup_out_packets(struct mosquitto *context)
{
	struct mosquitto__packet *packet;

	如果(!context){
		返回;
	}

	当(context->out_packet){
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
void context__cleanup(struct mosquitto *context, bool force_free)
{
	如果(!context){
		返回;
	}

	如果(force_free){
		context->clean_start = true;
	}

#ifdef WITH_BRIDGE
	如果(context->bridge){
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
	如果(force_free){
		sub__clean_session(context);
	}
	db__messages_delete(context, force_free);

	mosquitto_FREE(context->address);

	context__send_will(context);

	如果(context->id){
		context__remove_from_by_id(context);
		mosquitto_FREE(context->id);
	}
	packet__cleanup(&(context->in_packet));
	context__cleanup_out_packets(context);
#如果 defined(WITH_BROKER) && defined(__GLIBC__) && defined(WITH_ADNS)
	如果(context->adns){
		gai_cancel(context->adns);
		struct addrinfo *ar_request = (struct addrinfo *)context->adns->ar_request;
		mosquitto_FREE(ar_request);
		mosquitto_FREE(context->adns);
	}
#endif

#如果 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	mosquitto_FREE(context->http_request);
#endif
	mosquitto_FREE(context->proxy.buf);
	如果(force_free){
		mosquitto_FREE(context);
	}
}


void context__send_will(struct mosquitto *ctxt)
{
	如果(ctxt->state != mosq_cs_disconnecting && ctxt->will){
		如果(ctxt->session_expiry_interval > 0 && ctxt->will_delay_interval > 0){
			will_delay__add(ctxt);
			返回;
		}

		如果(mosquitto_acl_check(ctxt,
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


void context__disconnect(struct mosquitto *context, 整数 reason)
{
	如果(mosquitto__get_state(context) == mosq_cs_disconnected){
		返回;
	}

#如果 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	如果(context->transport == mosq_t_ws){
		uint8_t buf[4] = {0x88, 0x02, 0x03, context->wsd.disconnect_reason};
		/* Send the disconnect reason, but don't care if it fails */
		如果(send(context->sock, buf, 4, 0)){
		}
	}
#endif
	如果(context->id){
		struct mosquitto *context_found;
		HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
		如果(context_found == context){
			net__socket_close(context);
			context__add_to_disused(context);
			返回;
		}
	}

	context__send_will(context);

	如果(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
		plugin__handle_disconnect(context, reason);
	}否则{
		plugin__handle_client_offline(context, reason);
	}

	net__socket_close(context);
#ifdef WITH_BRIDGE
	如果(context->bridge == NULL)
	/* Outgoing bridge connection never expire */
#endif
	{
		如果(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
			plugin_persist__handle_client_delete(context);
			/* Client session is due to be expired now */
			如果(context->will_delay_interval == 0){
				/* This will be done later, after the will is published for delay>0. */
				context__add_to_disused(context);
			}
		}否则{
			session_expiry__add(context);
		}
	}
	keepalive__remove(context);
	mosquitto__set_state(context, mosq_cs_disconnected);
	alias__free_all(context);
	context__cleanup_out_packets(context);
}


void context__add_to_disused(struct mosquitto *context)
{
	如果(context->state == mosq_cs_disused){
		返回;
	}

	mosquitto__set_state(context, mosq_cs_disused);

	context__remove_from_by_id(context);

	context->for_free_next = db.ll_for_free;
	db.ll_for_free = context;
}


void context__free_disused(void)
{
	struct mosquitto *context, *next;
#如果 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	struct mosquitto *last = NULL;
#endif

	context = db.ll_for_free;
	db.ll_for_free = NULL;
	当(context){
#如果 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		如果(context->wsi){
			/* Don't delete yet, lws hasn't finished with it */
			如果(last){
				last->for_free_next = context;
			}否则{
				db.ll_for_free = context;
			}
			next = context->for_free_next;
			context->for_free_next = NULL;
			last = context;
			context = next;
		}否则
#endif
		{
			next = context->for_free_next;
			context__cleanup(context, true);
			context = next;
		}
	}
}


void context__add_to_by_id(struct mosquitto *context)
{
	如果(context->in_by_id == false){
		context->in_by_id = true;
		HASH_VALUE(context->id, strlen(context->id), context->id_hashv);
		HASH_ADD_KEYPTR_BYHASHVALUE(hh_id, db.contexts_by_id, context->id, strlen(context->id), context->id_hashv, context);
	}
}


void context__remove_from_by_id(struct mosquitto *context)
{
	struct mosquitto *context_found;

	如果(!context->id){
		返回;
	}

	如果(context->in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, context->id, strlen(context->id), context_found);
		如果(context_found == context){
			HASH_DELETE(hh_id, db.contexts_by_id, context_found);
		}
		context->id_hashv = 0;
		context->in_by_id = false;
		返回;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, context->id, strlen(context->id), context_found);
	如果(context_found == context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context_found);
	}
}
