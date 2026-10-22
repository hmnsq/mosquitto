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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଅନ୍ୟଥା
#ଅନ୍ତର୍ଭାଵ <process.h>
#ଅନ୍ତର୍ଭାଵ <winsock2.h>
#ଅନ୍ତର୍ଭାଵ <ws2tcpip.h>
#endif

#ଅନ୍ତର୍ଭାଵ <errno.h>
#ଅନ୍ତର୍ଭାଵ <signal.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#ଅନ୍ତର୍ଭାଵ <time.h>
#ଅନ୍ତର୍ଭାଵ <utlist.h>

#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "packet_mosq.h"
#ଅନ୍ତର୍ଭାଵ "property_common.h"
#ଅନ୍ତର୍ଭାଵ "send_mosq.h"
#ଅନ୍ତର୍ଭାଵ "sys_tree.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"

ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା g_run;

#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_ସଂଖ୍ୟା == 3002000


ଶୂନ୍ୟ lws__sul_callback(ସଂରଚନା lws_sorted_usec_list *l)
{
}

ସ୍ଥିର ସଂରଚନା lws_sorted_usec_list sul;
#endif


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା single_publish(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto__message_v5 *pub_ସନ୍ଦେଶ, uint32_t message_expiry)
{
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ;
	uint16_t mid;

	base_ସନ୍ଦେଶ = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ));
	ଯଦି(base_ସନ୍ଦେଶ == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	base_ସନ୍ଦେଶ->data.topic = pub_ସନ୍ଦେଶ->topic;
	pub_ସନ୍ଦେଶ->topic = ଖାଲି;
	base_ସନ୍ଦେଶ->data.retain = 0;
	base_ସନ୍ଦେଶ->data.payloadlen = (uint32_t)pub_ସନ୍ଦେଶ->payloadlen;
	base_ସନ୍ଦେଶ->data.payload = mosquitto_malloc(base_ସନ୍ଦେଶ->data.payloadlen+1);
	ଯଦି(base_ସନ୍ଦେଶ->data.payload == ଖାଲି){
		db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)base_ସନ୍ଦେଶ->data.payload)[base_ସନ୍ଦେଶ->data.payloadlen] = 0;
	memcpy(base_ସନ୍ଦେଶ->data.payload, pub_ସନ୍ଦେଶ->payload, base_ସନ୍ଦେଶ->data.payloadlen);

	ଯଦି(pub_ସନ୍ଦେଶ->properties){
		base_ସନ୍ଦେଶ->data.properties = pub_ସନ୍ଦେଶ->properties;
		pub_ସନ୍ଦେଶ->properties = ଖାଲି;
	}

	ଯଦି(db__message_store(context, base_ସନ୍ଦେଶ, &message_expiry, mosq_mo_broker)){
		ଫେରନ୍ତୁ 1;
	}

	ଯଦି(pub_ସନ୍ଦେଶ->qos){
		mid = mosquitto__mid_generate(context);
	}ଅନ୍ୟଥା{
		mid = 0;
	}
	ଫେରନ୍ତୁ db_message_ଭର୍ତ୍ତି_outgoing(context, 0, mid, (uint8_t)pub_ସନ୍ଦେଶ->qos, 0, base_ସନ୍ଦେଶ, 0, true, true);
}


ସ୍ଥିର ଶୂନ୍ୟ read_message_expiry_interval(mosquitto_property **proplist, uint32_t *message_expiry)
{
	mosquitto_property *p, *previous = ଖାଲି;

	*message_expiry = ସନ୍ଦେଶ_EXPIRY_INFINITE;

	ଯଦି(!proplist){
		ଫେରନ୍ତୁ;
	}

	p = *proplist;
	ଯେତେବେଳେ(p){
		ଯଦି(mosquitto_property_identifier(p) == MQTT_PROP_MESSAGE_EXPIRY_INTERVAL){
			*message_expiry = mosquitto_property_int32_ମୂଲ୍ୟ(p);
			ଯଦି(p == *proplist){
				*proplist = mosquitto_property_next(p);
			}ଅନ୍ୟଥା{
				previous->next = mosquitto_property_next(p);
			}
			mosquitto_property_free(&p);
			ଫେରନ୍ତୁ;

		}
		previous = p;
		p = mosquitto_property_next(p);
	}
}


ସ୍ଥିର ଶୂନ୍ୟ queue_plugin_msgs(ଶୂନ୍ୟ)
{
	ସଂରଚନା mosquitto__message_v5 *ସନ୍ଦେଶ, *tmp;
	ସଂରଚନା mosquitto *context;
	uint32_t message_expiry;

	DL_FOREACH_SAFE(db.plugin_msgs, ସନ୍ଦେଶ, tmp){
		DL_DELETE(db.plugin_msgs, ସନ୍ଦେଶ);

		read_message_expiry_interval(&ସନ୍ଦେଶ->properties, &message_expiry);

		ଯଦି(ସନ୍ଦେଶ->clientid){
			HASH_FIND(hh_id, db.contexts_by_id, ସନ୍ଦେଶ->clientid, strlen(ସନ୍ଦେଶ->clientid), context);
			ଯଦି(context){
				single_publish(context, ସନ୍ଦେଶ, message_expiry);
			}
		}ଅନ୍ୟଥା{
			db__messages_easy_queue(ଖାଲି, ସନ୍ଦେଶ->topic, (uint8_t)ସନ୍ଦେଶ->qos, (uint32_t)ସନ୍ଦେଶ->payloadlen, ସନ୍ଦେଶ->payload, ସନ୍ଦେଶ->retain, message_expiry, &ସନ୍ଦେଶ->properties);
		}
		mosquitto_FREE(ସନ୍ଦେଶ->topic);
		mosquitto_FREE(ସନ୍ଦେଶ->payload);
		mosquitto_property_free_all(&ସନ୍ଦେଶ->properties);
		mosquitto_FREE(ସନ୍ଦେଶ->clientid);
		mosquitto_FREE(ସନ୍ଦେଶ);
	}
}


ଶୂନ୍ୟ loop__update_next_event(time_t new_ms)
{
	ଯଦି(new_ms > 0 && new_ms < db.next_event_ms){
		db.next_event_ms = new_ms;
	}
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_main_loop(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା)
{
#ifdef WITH_PERSISTENCE
	time_t last_backup = mosquitto_time();
#endif
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;


	watchdog__init();
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_ସଂଖ୍ୟା == 3002000
	memset(&sul, 0, ଆକାର(ସଂରଚନା lws_sorted_usec_list));
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(ଖାଲି);

#ifdef WITH_BRIDGE
	rc = bridge__register_local_connections();
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}
#endif

	ଯେତେବେଳେ(g_run){
		retain__expiry_check();
		queue_plugin_msgs();
		context__free_disused();

		db.next_event_ms = 86400000;
#ifdef WITH_SYS_ବୃକ୍ଷ
		ଯଦି(db.config->sys_interval > 0){
			sys_ବୃକ୍ଷ_update(false);
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

		rc = mux__handle(listensock, listensock_ଗଣନା);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}

#ifdef WITH_PERSISTENCE
		ଯଦି(db.config->persistence && db.config->autosave_interval){
			ଯଦି(db.config->autosave_on_changes){
				ଯଦି(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}ଅନ୍ୟଥା{
				ଯଦି(last_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					last_backup = db.now_s;
				}
			}
		}
#endif

		rc = signal__flag_check();
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}

#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.config->listener_ଗଣନା; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			ଯଦି(db.config->listeners[i].ws_context){
#ଯଦି LWS_LIBRARY_VERSION_ସଂଖ୍ୟା > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#ନଚେତଯଦି LWS_LIBRARY_VERSION_ସଂଖ୍ୟା == 3002000
				lws_sul_schedule(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#ଅନ୍ୟଥା
				lws_service(db.config->listeners[i].ws_context, 0);
#endif

			}
		}
#endif
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ଶୂନ୍ୟ do_disconnect(ସଂରଚନା mosquitto *context, ପୂର୍ଣ୍ଣସଂଖ୍ୟା reason)
{
	ଅଚଳ ଅକ୍ଷର *id;
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#endif

	ଯଦି(context->state == mosq_cs_disconnected){
		ଫେରନ୍ତୁ;
	}
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ଯଦି(context->wsi){
		ଯଦି(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		ଯଦି(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto__set_state(context, mosq_cs_disconnect_ws);
		}
		ଯଦି(context->wsi){
			lws_callback_on_writable(context->wsi);
		}
		ଯଦି(context->sock != INVALID_SOCKET){
			HASH_DELETE(hh_sock, db.contexts_by_sock, context);
			mux__delete(context);
			context->sock = INVALID_SOCKET;
		}
		ଯଦି(is_duplicate){
			/* This occurs if another client is taking over the same client id.
			 * It is important to remove this from the by_id hash here, so it
			 * doesn't leave us with multiple clients in the hash with the same
			 * id. Websockets doesn't actually close the connection here,
			 * unlike for normal clients, which means there is extra time when
			 * there could be two clients with the same id in the hash. */
			context__remove_from_by_id(context);
		}
	}ଅନ୍ୟଥା
#endif
	{
		ଯଦି(db.config->connection_messages == true){
			ଯଦି(context->id){
				id = context->id;
			}ଅନ୍ୟଥା{
				id = context->address;
			}
			ଯଦି(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				ବାଛ(reason){
					ସ୍ଥିତି MOSQ_ERR_SUCCESS:
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_MALFORMED_PACKET:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_PROTOCOL:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_CONN_LOST:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_AUTH:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_KEEPALIVE:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_PAYLOAD_SIZE:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_NOMEM:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_NOT_SUPPORTED:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_ERRNO:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->address, context->remote_port, strerror(errno));
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_SERVER_BUSY:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_BANNED:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_SESSION_TAKEN_OVER:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->address, context->remote_port);
						ଭାଙ୍ଗ;
					ସ୍ଥିତି MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						ଭାଙ୍ଗ;
					ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->address, context->remote_port, mosquitto_strerror(reason));
						ଭାଙ୍ଗ;
				}
			}ଅନ୍ୟଥା{
				ଯଦି(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
				}ଅନ୍ୟଥା{
					log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->address, context->remote_port);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
