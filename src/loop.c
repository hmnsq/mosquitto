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
#否则
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

#如果 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "packet_mosq.h"
#include "property_common.h"
#include "send_mosq.h"
#include "sys_tree.h"
#include "util_mosq.h"

extern 整数 g_run;

#如果 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_NUMBER == 3002000


void lws__sul_callback(struct lws_sorted_usec_list *l)
{
}

static struct lws_sorted_usec_list sul;
#endif


static 整数 single_publish(struct mosquitto *context, struct mosquitto__message_v5 *pub_msg, uint32_t message_expiry)
{
	struct mosquitto__base_msg *base_msg;
	uint16_t mid;

	base_msg = mosquitto_calloc(1, sizeof(struct mosquitto__base_msg));
	如果(base_msg == NULL){
		返回 MOSQ_ERR_NOMEM;
	}

	base_msg->data.topic = pub_msg->topic;
	pub_msg->topic = NULL;
	base_msg->data.retain = 0;
	base_msg->data.payloadlen = (uint32_t)pub_msg->payloadlen;
	base_msg->data.payload = mosquitto_malloc(base_msg->data.payloadlen+1);
	如果(base_msg->data.payload == NULL){
		db__msg_store_free(base_msg);
		返回 MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)base_msg->data.payload)[base_msg->data.payloadlen] = 0;
	memcpy(base_msg->data.payload, pub_msg->payload, base_msg->data.payloadlen);

	如果(pub_msg->properties){
		base_msg->data.properties = pub_msg->properties;
		pub_msg->properties = NULL;
	}

	如果(db__message_store(context, base_msg, &message_expiry, mosq_mo_broker)){
		返回 1;
	}

	如果(pub_msg->qos){
		mid = mosquitto__mid_generate(context);
	}否则{
		mid = 0;
	}
	返回 db__message_insert_outgoing(context, 0, mid, (uint8_t)pub_msg->qos, 0, base_msg, 0, true, true);
}


static void read_message_expiry_interval(mosquitto_property **proplist, uint32_t *message_expiry)
{
	mosquitto_property *p, *previous = NULL;

	*message_expiry = MSG_EXPIRY_INFINITE;

	如果(!proplist){
		返回;
	}

	p = *proplist;
	当(p){
		如果(mosquitto_property_identifier(p) == MQTT_PROP_MESSAGE_EXPIRY_INTERVAL){
			*message_expiry = mosquitto_property_int32_value(p);
			如果(p == *proplist){
				*proplist = mosquitto_property_next(p);
			}否则{
				previous->next = mosquitto_property_next(p);
			}
			mosquitto_property_free(&p);
			返回;

		}
		previous = p;
		p = mosquitto_property_next(p);
	}
}


static void queue_plugin_msgs(void)
{
	struct mosquitto__message_v5 *msg, *tmp;
	struct mosquitto *context;
	uint32_t message_expiry;

	DL_FOREACH_SAFE(db.plugin_msgs, msg, tmp){
		DL_DELETE(db.plugin_msgs, msg);

		read_message_expiry_interval(&msg->properties, &message_expiry);

		如果(msg->clientid){
			HASH_FIND(hh_id, db.contexts_by_id, msg->clientid, strlen(msg->clientid), context);
			如果(context){
				single_publish(context, msg, message_expiry);
			}
		}否则{
			db__messages_easy_queue(NULL, msg->topic, (uint8_t)msg->qos, (uint32_t)msg->payloadlen, msg->payload, msg->retain, message_expiry, &msg->properties);
		}
		mosquitto_FREE(msg->topic);
		mosquitto_FREE(msg->payload);
		mosquitto_property_free_all(&msg->properties);
		mosquitto_FREE(msg->clientid);
		mosquitto_FREE(msg);
	}
}


void loop__update_next_event(time_t new_ms)
{
	如果(new_ms > 0 && new_ms < db.next_event_ms){
		db.next_event_ms = new_ms;
	}
}


整数 mosquitto_main_loop(struct mosquitto__listener_sock *listensock, 整数 listensock_count)
{
#ifdef WITH_PERSISTENCE
	time_t last_backup = mosquitto_time();
#endif
	整数 rc;


	watchdog__init();
#如果 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_NUMBER == 3002000
	memset(&sul, 0, sizeof(struct lws_sorted_usec_list));
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(NULL);

#ifdef WITH_BRIDGE
	rc = bridge__register_local_connections();
	如果(rc){
		返回 rc;
	}
#endif

	当(g_run){
		retain__expiry_check();
		queue_plugin_msgs();
		context__free_disused();

		db.next_event_ms = 86400000;
#ifdef WITH_SYS_TREE
		如果(db.config->sys_interval > 0){
			sys_tree__update(false);
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

		rc = mux__handle(listensock, listensock_count);
		如果(rc){
			返回 rc;
		}

#ifdef WITH_PERSISTENCE
		如果(db.config->persistence && db.config->autosave_interval){
			如果(db.config->autosave_on_changes){
				如果(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}否则{
				如果(last_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					last_backup = db.now_s;
				}
			}
		}
#endif

		rc = signal__flag_check();
		如果(rc){
			返回 rc;
		}

#如果 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		为(整数 i=0; i<db.config->listener_count; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			如果(db.config->listeners[i].ws_context){
#如果 LWS_LIBRARY_VERSION_NUMBER > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#elif LWS_LIBRARY_VERSION_NUMBER == 3002000
				lws_sul_schedule(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#否则
				lws_service(db.config->listeners[i].ws_context, 0);
#endif

			}
		}
#endif
	}

	返回 MOSQ_ERR_SUCCESS;
}


void do_disconnect(struct mosquitto *context, 整数 reason)
{
	const char *id;
#如果 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#endif

	如果(context->state == mosq_cs_disconnected){
		返回;
	}
#如果 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	如果(context->wsi){
		如果(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		如果(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto__set_state(context, mosq_cs_disconnect_ws);
		}
		如果(context->wsi){
			lws_callback_on_writable(context->wsi);
		}
		如果(context->sock != INVALID_SOCKET){
			HASH_DELETE(hh_sock, db.contexts_by_sock, context);
			mux__delete(context);
			context->sock = INVALID_SOCKET;
		}
		如果(is_duplicate){
			/* This occurs if another client is taking over the same client id.
			 * It is important to remove this from the by_id hash here, so it
			 * doesn't leave us with multiple clients in the hash with the same
			 * id. Websockets doesn't actually close the connection here,
			 * unlike for normal clients, which means there is extra time when
			 * there could be two clients with the same id in the hash. */
			context__remove_from_by_id(context);
		}
	}否则
#endif
	{
		如果(db.config->connection_messages == true){
			如果(context->id){
				id = context->id;
			}否则{
				id = context->address;
			}
			如果(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
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
			}否则{
				如果(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
				}否则{
					log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->address, context->remote_port);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
