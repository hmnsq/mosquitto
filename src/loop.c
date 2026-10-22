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
#မဟုတ်ပါက
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

#အကယ်၍ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "packet_mosq.h"
#include "property_common.h"
#include "send_mosq.h"
#include "sys_tree.h"
#include "util_mosq.h"

extern ကိန်းပြည့် g_run;

#အကယ်၍ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_NUMBER == 3002000


void lws__sul_callback(sa_nracanaa lws_sorted_usec_list *l)
{
}

static sa_nracanaa lws_sorted_usec_list sul;
#endif


static ကိန်းပြည့် single_publish(sa_nracanaa mosquitto *context, sa_nracanaa mosquitto__message_v5 *pub_msg, uint32_t message_expiry)
{
	sa_nracanaa mosquitto__base_msg *base_msg;
	uint16_t mid;

	base_msg = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__base_msg));
	အကယ်၍(base_msg == NULL){
		ပြန်ပေး MOSQ_ERR_NOMEM;
	}

	base_msg->data.topic = pub_msg->topic;
	pub_msg->topic = NULL;
	base_msg->data.retain = 0;
	base_msg->data.payloadlen = (uint32_t)pub_msg->payloadlen;
	base_msg->data.payload = mosquitto_malloc(base_msg->data.payloadlen+1);
	အကယ်၍(base_msg->data.payload == NULL){
		db__msg_store_free(base_msg);
		ပြန်ပေး MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)base_msg->data.payload)[base_msg->data.payloadlen] = 0;
	memcpy(base_msg->data.payload, pub_msg->payload, base_msg->data.payloadlen);

	အကယ်၍(pub_msg->properties){
		base_msg->data.properties = pub_msg->properties;
		pub_msg->properties = NULL;
	}

	အကယ်၍(db__message_store(context, base_msg, &message_expiry, mosq_mo_broker)){
		ပြန်ပေး 1;
	}

	အကယ်၍(pub_msg->qos){
		mid = mosquitto__mid_generate(context);
	}မဟုတ်ပါက{
		mid = 0;
	}
	ပြန်ပေး db__message_insert_outgoing(context, 0, mid, (uint8_t)pub_msg->qos, 0, base_msg, 0, true, true);
}


static void read_message_expiry_interval(mosquitto_property **proplist, uint32_t *message_expiry)
{
	mosquitto_property *p, *previous = NULL;

	*message_expiry = MSG_EXPIRY_INFINITE;

	အကယ်၍(!proplist){
		ပြန်ပေး;
	}

	p = *proplist;
	နေစဉ်(p){
		အကယ်၍(mosquitto_property_identifier(p) == MQTT_PROP_MESSAGE_EXPIRY_INTERVAL){
			*message_expiry = mosquitto_property_int32_value(p);
			အကယ်၍(p == *proplist){
				*proplist = mosquitto_property_next(p);
			}မဟုတ်ပါက{
				previous->next = mosquitto_property_next(p);
			}
			mosquitto_property_free(&p);
			ပြန်ပေး;

		}
		previous = p;
		p = mosquitto_property_next(p);
	}
}


static void queue_plugin_msgs(void)
{
	sa_nracanaa mosquitto__message_v5 *msg, *tmp;
	sa_nracanaa mosquitto *context;
	uint32_t message_expiry;

	DL_FOREACH_SAFE(db.plugin_msgs, msg, tmp){
		DL_DELETE(db.plugin_msgs, msg);

		read_message_expiry_interval(&msg->properties, &message_expiry);

		အကယ်၍(msg->clientid){
			HASH_FIND(hh_id, db.contexts_by_id, msg->clientid, strlen(msg->clientid), context);
			အကယ်၍(context){
				single_publish(context, msg, message_expiry);
			}
		}မဟုတ်ပါက{
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
	အကယ်၍(new_ms > 0 && new_ms < db.next_event_ms){
		db.next_event_ms = new_ms;
	}
}


ကိန်းပြည့် mosquitto_main_loop(sa_nracanaa mosquitto__listener_sock *listensock, ကိန်းပြည့် listensock_count)
{
#ifdef WITH_PERSISTENCE
	time_t last_backup = mosquitto_time();
#endif
	ကိန်းပြည့် rc;


	watchdog__init();
#အကယ်၍ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_NUMBER == 3002000
	memset(&sul, 0, parimaa_na(sa_nracanaa lws_sorted_usec_list));
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(NULL);

#ifdef WITH_BRIDGE
	rc = bridge__register_local_connections();
	အကယ်၍(rc){
		ပြန်ပေး rc;
	}
#endif

	နေစဉ်(g_run){
		retain__expiry_check();
		queue_plugin_msgs();
		context__free_disused();

		db.next_event_ms = 86400000;
#ifdef WITH_SYS_TREE
		အကယ်၍(db.config->sys_interval > 0){
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
		အကယ်၍(rc){
			ပြန်ပေး rc;
		}

#ifdef WITH_PERSISTENCE
		အကယ်၍(db.config->persistence && db.config->autosave_interval){
			အကယ်၍(db.config->autosave_on_changes){
				အကယ်၍(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}မဟုတ်ပါက{
				အကယ်၍(last_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					last_backup = db.now_s;
				}
			}
		}
#endif

		rc = signal__flag_check();
		အကယ်၍(rc){
			ပြန်ပေး rc;
		}

#အကယ်၍ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		အတွက်(ကိန်းပြည့် i=0; i<db.config->listener_count; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			အကယ်၍(db.config->listeners[i].ws_context){
#အကယ်၍ LWS_LIBRARY_VERSION_NUMBER > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#elif LWS_LIBRARY_VERSION_NUMBER == 3002000
				lws_sul_schedule(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#မဟုတ်ပါက
				lws_service(db.config->listeners[i].ws_context, 0);
#endif

			}
		}
#endif
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


void do_disconnect(sa_nracanaa mosquitto *context, ကိန်းပြည့် reason)
{
	const var_na *id;
#အကယ်၍ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#endif

	အကယ်၍(context->state == mosq_cs_disconnected){
		ပြန်ပေး;
	}
#အကယ်၍ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	အကယ်၍(context->wsi){
		အကယ်၍(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		အကယ်၍(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto__set_state(context, mosq_cs_disconnect_ws);
		}
		အကယ်၍(context->wsi){
			lws_callback_on_writable(context->wsi);
		}
		အကယ်၍(context->sock != INVALID_SOCKET){
			HASH_DELETE(hh_sock, db.contexts_by_sock, context);
			mux__delete(context);
			context->sock = INVALID_SOCKET;
		}
		အကယ်၍(is_duplicate){
			/* This occurs if another client is taking over the same client id.
			 * It is important to remove this from the by_id hash here, so it
			 * doesn't leave us with multiple clients in the hash with the same
			 * id. Websockets doesn't actually close the connection here,
			 * unlike for normal clients, which means there is extra time when
			 * there could be two clients with the same id in the hash. */
			context__remove_from_by_id(context);
		}
	}မဟုတ်ပါက
#endif
	{
		အကယ်၍(db.config->connection_messages == true){
			အကယ်၍(context->id){
				id = context->id;
			}မဟုတ်ပါက{
				id = context->address;
			}
			အကယ်၍(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				switch(reason){
					pak_sa MOSQ_ERR_SUCCESS:
						bha_nga;
					pak_sa MOSQ_ERR_MALFORMED_PACKET:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_PROTOCOL:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_CONN_LOST:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_AUTH:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_KEEPALIVE:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_PAYLOAD_SIZE:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_NOMEM:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_NOT_SUPPORTED:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_ERRNO:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->address, context->remote_port, strerror(errno));
						bha_nga;
					pak_sa MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_SERVER_BUSY:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_BANNED:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_SESSION_TAKEN_OVER:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->address, context->remote_port);
						bha_nga;
					pak_sa MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						bha_nga;
					default:
						log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->address, context->remote_port, mosquitto_strerror(reason));
						bha_nga;
				}
			}မဟုတ်ပါက{
				အကယ်၍(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
				}မဟုတ်ပါက{
					log__printf(NULL, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->address, context->remote_port);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
