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
   Tatsuzo Osawa - Add epoll.
*/

#include "config.h"

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#include <unistd.h>
#else
#include <process.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#include <time.h>
#include <utlist.h>

#if defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "packet_mosq.h"
#include "property_common.h"
#include "send_mosq.h"
#include "sys_tree.h"
#include "util_mosq.h"

extern int g_run;

#if defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_नुमेरो == 3002000


void lws__sul_callback(struct lws_sorted_usec_list *l)
{
}

static struct lws_sorted_usec_list sul;
#endif


static int single_publish(struct mosquitto *context, struct mosquitto__message_v5 *pub_मेनसाहे, uint32_t message_expiry)
{
	struct mosquitto_base_मेनसाहे *base_मेनसाहे;
	uint16_t mid;

	base_मेनसाहे = mosquitto_calloc(1, sizeof(struct mosquitto_base_मेनसाहे));
	if(base_मेनसाहे == NULL){
		return MOSQ_ERR_NOMEM;
	}

	base_मेनसाहे->data.topic = pub_मेनसाहे->topic;
	pub_मेनसाहे->topic = NULL;
	base_मेनसाहे->data.retain = 0;
	base_मेनसाहे->data.payloadlen = (uint32_t)pub_मेनसाहे->payloadlen;
	base_मेनसाहे->data.payload = mosquitto_malloc(base_मेनसाहे->data.payloadlen+1);
	if(base_मेनसाहे->data.payload == NULL){
		db_मेनसाहे_store_free(base_मेनसाहे);
		return MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)base_मेनसाहे->data.payload)[base_मेनसाहे->data.payloadlen] = 0;
	memcpy(base_मेनसाहे->data.payload, pub_मेनसाहे->payload, base_मेनसाहे->data.payloadlen);

	if(pub_मेनसाहे->properties){
		base_मेनसाहे->data.properties = pub_मेनसाहे->properties;
		pub_मेनसाहे->properties = NULL;
	}

	if(db__message_store(context, base_मेनसाहे, &message_expiry, mosq_mo_broker)){
		return 1;
	}

	if(pub_मेनसाहे->qos){
		mid = mosquitto__mid_generate(context);
	}else{
		mid = 0;
	}
	return db_message_इसिंगित_outgoing(context, 0, mid, (uint8_t)pub_मेनसाहे->qos, 0, base_मेनसाहे, 0, true, true);
}


static void read_message_expiry_interval(mosquitto_property **proplist, uint32_t *message_expiry)
{
	mosquitto_property *p, *previous = NULL;

	*message_expiry = मेनसाहे_EXPIRY_INFINITE;

	if(!proplist){
		return;
	}

	p = *proplist;
	while(p){
		if(mosquitto_property_identifier(p) == MQTT_PROP_MESSAGE_EXPIRY_INTERVAL){
			*message_expiry = mosquitto_property_int32_हालागा(p);
			if(p == *proplist){
				*proplist = mosquitto_property_next(p);
			}else{
				previous->next = mosquitto_property_next(p);
			}
			mosquitto_property_free(&p);
			return;

		}
		previous = p;
		p = mosquitto_property_next(p);
	}
}


static void queue_plugin_msgs(void)
{
	struct mosquitto__message_v5 *मेनसाहे, *tmp;
	struct mosquitto *context;
	uint32_t message_expiry;

	DL_FOREACH_SAFE(db.plugin_msgs, मेनसाहे, tmp){
		DL_DELETE(db.plugin_msgs, मेनसाहे);

		read_message_expiry_interval(&मेनसाहे->properties, &message_expiry);

		if(मेनसाहे->clientid){
			HASH_FIND(hh_id, db.contexts_by_id, मेनसाहे->clientid, strlen(मेनसाहे->clientid), context);
			if(context){
				single_publish(context, मेनसाहे, message_expiry);
			}
		}else{
			db__messages_easy_queue(NULL, मेनसाहे->topic, (uint8_t)मेनसाहे->qos, (uint32_t)मेनसाहे->payloadlen, मेनसाहे->payload, मेनसाहे->retain, message_expiry, &मेनसाहे->properties);
		}
		mosquitto_FREE(मेनसाहे->topic);
		mosquitto_FREE(मेनसाहे->payload);
		mosquitto_property_free_all(&मेनसाहे->properties);
		mosquitto_FREE(मेनसाहे->clientid);
		mosquitto_FREE(मेनसाहे);
	}
}


void loop__update_next_event(time_t new_ms)
{
	if(new_ms > 0 && new_ms < db.next_event_ms){
		db.next_event_ms = new_ms;
	}
}


int mosquitto_main_loop(struct mosquitto__listener_sock *listensock, int listensock_बिलांग)
{
#ifdef WITH_PERSISTENCE
	time_t last_backup = mosquitto_time();
#endif
	int rc;


	watchdog__init();
#if defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_नुमेरो == 3002000
	memset(&sul, 0, sizeof(struct lws_sorted_usec_list));
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(NULL);

#ifdef WITH_BRIDGE
	rc = bridge__register_local_connections();
	if(rc){
		return rc;
	}
#endif

	while(g_run){
		retain__expiry_check();
		queue_plugin_msgs();
		context__free_disused();

		db.next_event_ms = 86400000;
#ifdef WITH_SYS_पुनो
		if(db.config->sys_interval > 0){
			sys_पुनो_update(false);
		}
#endif

		keepalive__check();
		watchdog__check();

#ifdef WITH_BRIDGE
		bridge_check();
#endif
		plugin__handle_tick();
		session_expiry__check();
		will_delay__check();

		rc = mux__handle(listensock, listensock_बिलांग);
		if(rc){
			return rc;
		}

#ifdef WITH_PERSISTENCE
		if(db.config->persistence && db.config->autosave_interval){
			if(db.config->autosave_on_changes){
				if(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}else{
				if(last_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					last_backup = db.now_s;
				}
			}
		}
#endif

		rc = signal__flag_check();
		if(rc){
			return rc;
		}

#if defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		for(int i=0; i<db.config->listener_बिलांग; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			if(db.config->listeners[i].ws_context){
#if LWS_LIBRARY_VERSION_नुमेरो > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#elif LWS_LIBRARY_VERSION_नुमेरो == 3002000
				lws_sul_schedule(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#else
				lws_service(db.config->listeners[i].ws_context, 0);
#endif

			}
		}
#endif
	}

	return MOSQ_ERR_SUCCESS;
}


void do_disconnect(struct mosquitto *context, int reason)
{
	const char *id;
#if defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#endif

	if(context->state == mosq_cs_disconnected){
		return;
	}
#if defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	if(context->wsi){
		if(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		if(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto__set_state(context, mosq_cs_disconnect_ws);
		}
		if(context->wsi){
			lws_callback_on_writable(context->wsi);
		}
		if(context->sock != INVALID_SOCKET){
			HASH_DELETE(hh_sock, db.contexts_by_sock, context);
			mux__delete(context);
			context->sock = INVALID_SOCKET;
		}
		if(is_duplicate){
			/* This occurs if another client is taking over the same client id.
			 * It is important to remove this from the by_id hash here, so it
			 * doesn't leave us with multiple clients in the hash with the same
			 * id. Websockets doesn't actually close the connection here,
			 * unlike for normal clients, which means there is extra time when
			 * there could be two clients with the same id in the hash. */
			context__remove_from_by_id(context);
		}
	}else
#endif
	{
		if(db.config->connection_messages == true){
			if(context->id){
				id = context->id;
			}else{
				id = context->address;
			}
			if(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				switch(reason){
					case MOSQ_ERR_SUCCESS:
						break;
					case MOSQ_ERR_MALFORMED_PACKET:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_PROTOCOL:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_CONN_LOST:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_AUTH:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_KEEPALIVE:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_PAYLOAD_SIZE:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_NOMEM:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_NOT_SUPPORTED:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_ERRNO:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->address, context->remote_port, strerror(errno));
						break;
					case MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_SERVER_BUSY:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_BANNED:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_SESSION_TAKEN_OVER:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->address, context->remote_port);
						break;
					case MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						break;
					default:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->address, context->remote_port, mosquitto_strerror(reason));
						break;
				}
			}else{
				if(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
				}else{
					log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->address, context->remote_port);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
