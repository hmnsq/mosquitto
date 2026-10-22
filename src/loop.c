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

#ಅನ್ತರ್ಭಾವ "config.h"

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ಯಥಾ
#ಅನ್ತರ್ಭಾವ <process.h>
#ಅನ್ತರ್ಭಾವ <winsock2.h>
#ಅನ್ತರ್ಭಾವ <ws2tcpip.h>
#endif

#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#ಅನ್ತರ್ಭಾವ <time.h>
#ಅನ್ತರ್ಭಾವ <utlist.h>

#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "property_common.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ g_run;

#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_ಸಂಖ್ಯೆ == 3002000


ಶೂನ್ಯ lws__sul_callback(ಸಂರಚನಾ lws_sorted_usec_list *l)
{
}

ಸ್ಥಿರ ಸಂರಚನಾ lws_sorted_usec_list sul;
#endif


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ single_publish(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto__message_v5 *pub_ಸಂದೇಶ, uint32_t message_expiry)
{
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ;
	uint16_t mid;

	base_ಸಂದೇಶ = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ));
	ಯದಿ(base_ಸಂದೇಶ == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	base_ಸಂದೇಶ->data.topic = pub_ಸಂದೇಶ->topic;
	pub_ಸಂದೇಶ->topic = ರಿಕ್ತ;
	base_ಸಂದೇಶ->data.retain = 0;
	base_ಸಂದೇಶ->data.payloadlen = (uint32_t)pub_ಸಂದೇಶ->payloadlen;
	base_ಸಂದೇಶ->data.payload = mosquitto_malloc(base_ಸಂದೇಶ->data.payloadlen+1);
	ಯದಿ(base_ಸಂದೇಶ->data.payload == ರಿಕ್ತ){
		db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)base_ಸಂದೇಶ->data.payload)[base_ಸಂದೇಶ->data.payloadlen] = 0;
	memcpy(base_ಸಂದೇಶ->data.payload, pub_ಸಂದೇಶ->payload, base_ಸಂದೇಶ->data.payloadlen);

	ಯದಿ(pub_ಸಂದೇಶ->properties){
		base_ಸಂದೇಶ->data.properties = pub_ಸಂದೇಶ->properties;
		pub_ಸಂದೇಶ->properties = ರಿಕ್ತ;
	}

	ಯದಿ(db__message_store(context, base_ಸಂದೇಶ, &message_expiry, mosq_mo_broker)){
		ಪ್ರತ್ಯಾಗಮ 1;
	}

	ಯದಿ(pub_ಸಂದೇಶ->qos){
		mid = mosquitto__mid_generate(context);
	}ಅನ್ಯಥಾ{
		mid = 0;
	}
	ಪ್ರತ್ಯಾಗಮ db_message_ಸೇರಿಸು_outgoing(context, 0, mid, (uint8_t)pub_ಸಂದೇಶ->qos, 0, base_ಸಂದೇಶ, 0, true, true);
}


ಸ್ಥಿರ ಶೂನ್ಯ read_message_expiry_interval(mosquitto_property **proplist, uint32_t *message_expiry)
{
	mosquitto_property *p, *previous = ರಿಕ್ತ;

	*message_expiry = ಸಂದೇಶ_EXPIRY_INFINITE;

	ಯದಿ(!proplist){
		ಪ್ರತ್ಯಾಗಮ;
	}

	p = *proplist;
	ಯಾವತ್(p){
		ಯದಿ(mosquitto_property_identifier(p) == MQTT_PROP_MESSAGE_EXPIRY_INTERVAL){
			*message_expiry = mosquitto_property_int32_ಮೌಲ್ಯ(p);
			ಯದಿ(p == *proplist){
				*proplist = mosquitto_property_next(p);
			}ಅನ್ಯಥಾ{
				previous->next = mosquitto_property_next(p);
			}
			mosquitto_property_free(&p);
			ಪ್ರತ್ಯಾಗಮ;

		}
		previous = p;
		p = mosquitto_property_next(p);
	}
}


ಸ್ಥಿರ ಶೂನ್ಯ queue_plugin_msgs(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ mosquitto__message_v5 *ಸಂದೇಶ, *tmp;
	ಸಂರಚನಾ mosquitto *context;
	uint32_t message_expiry;

	DL_FOREACH_SAFE(db.plugin_msgs, ಸಂದೇಶ, tmp){
		DL_DELETE(db.plugin_msgs, ಸಂದೇಶ);

		read_message_expiry_interval(&ಸಂದೇಶ->properties, &message_expiry);

		ಯದಿ(ಸಂದೇಶ->clientid){
			HASH_FIND(hh_id, db.contexts_by_id, ಸಂದೇಶ->clientid, strlen(ಸಂದೇಶ->clientid), context);
			ಯದಿ(context){
				single_publish(context, ಸಂದೇಶ, message_expiry);
			}
		}ಅನ್ಯಥಾ{
			db__messages_easy_queue(ರಿಕ್ತ, ಸಂದೇಶ->topic, (uint8_t)ಸಂದೇಶ->qos, (uint32_t)ಸಂದೇಶ->payloadlen, ಸಂದೇಶ->payload, ಸಂದೇಶ->retain, message_expiry, &ಸಂದೇಶ->properties);
		}
		mosquitto_FREE(ಸಂದೇಶ->topic);
		mosquitto_FREE(ಸಂದೇಶ->payload);
		mosquitto_property_free_all(&ಸಂದೇಶ->properties);
		mosquitto_FREE(ಸಂದೇಶ->clientid);
		mosquitto_FREE(ಸಂದೇಶ);
	}
}


ಶೂನ್ಯ loop__update_next_event(time_t new_ms)
{
	ಯದಿ(new_ms > 0 && new_ms < db.next_event_ms){
		db.next_event_ms = new_ms;
	}
}


ಪೂರ್ಣಾಙ್ಕ mosquitto_main_loop(ಸಂರಚನಾ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಙ್ಕ listensock_ಎಣಿಕೆ)
{
#ifdef WITH_PERSISTENCE
	time_t last_backup = mosquitto_time();
#endif
	ಪೂರ್ಣಾಙ್ಕ rc;


	watchdog__init();
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_ಸಂಖ್ಯೆ == 3002000
	memset(&sul, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ lws_sorted_usec_list));
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(ರಿಕ್ತ);

#ifdef WITH_BRIDGE
	rc = bridge__register_local_connections();
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}
#endif

	ಯಾವತ್(g_run){
		retain__expiry_check();
		queue_plugin_msgs();
		context__free_disused();

		db.next_event_ms = 86400000;
#ifdef WITH_SYS_ವೃಕ್ಷ
		ಯದಿ(db.config->sys_interval > 0){
			sys_ವೃಕ್ಷ_update(false);
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

		rc = mux__handle(listensock, listensock_ಎಣಿಕೆ);
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}

#ifdef WITH_PERSISTENCE
		ಯದಿ(db.config->persistence && db.config->autosave_interval){
			ಯದಿ(db.config->autosave_on_changes){
				ಯದಿ(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}ಅನ್ಯಥಾ{
				ಯದಿ(last_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					last_backup = db.now_s;
				}
			}
		}
#endif

		rc = signal__flag_check();
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}

#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			ಯದಿ(db.config->listeners[i].ws_context){
#ಯದಿ LWS_LIBRARY_VERSION_ಸಂಖ್ಯೆ > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#ನೋಚೇದ್ಯದಿ LWS_LIBRARY_VERSION_ಸಂಖ್ಯೆ == 3002000
				lws_sul_schedule(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#ಅನ್ಯಥಾ
				lws_service(db.config->listeners[i].ws_context, 0);
#endif

			}
		}
#endif
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಶೂನ್ಯ do_disconnect(ಸಂರಚನಾ mosquitto *context, ಪೂರ್ಣಾಙ್ಕ reason)
{
	ಧ್ರುವ ವರ್ಣ *id;
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#endif

	ಯದಿ(context->state == mosq_cs_disconnected){
		ಪ್ರತ್ಯಾಗಮ;
	}
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ಯದಿ(context->wsi){
		ಯದಿ(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		ಯದಿ(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto__set_state(context, mosq_cs_disconnect_ws);
		}
		ಯದಿ(context->wsi){
			lws_callback_on_writable(context->wsi);
		}
		ಯದಿ(context->sock != INVALID_SOCKET){
			HASH_DELETE(hh_sock, db.contexts_by_sock, context);
			mux__delete(context);
			context->sock = INVALID_SOCKET;
		}
		ಯದಿ(is_duplicate){
			/* This occurs if another client is taking over the same client id.
			 * It is important to remove this from the by_id hash here, so it
			 * doesn't leave us with multiple clients in the hash with the same
			 * id. Websockets doesn't actually close the connection here,
			 * unlike for normal clients, which means there is extra time when
			 * there could be two clients with the same id in the hash. */
			context__remove_from_by_id(context);
		}
	}ಅನ್ಯಥಾ
#endif
	{
		ಯದಿ(db.config->connection_messages == true){
			ಯದಿ(context->id){
				id = context->id;
			}ಅನ್ಯಥಾ{
				id = context->address;
			}
			ಯದಿ(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				ವಿಕಲ್ಪ(reason){
					ಪಕ್ಷ MOSQ_ERR_SUCCESS:
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_MALFORMED_PACKET:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_PROTOCOL:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_CONN_LOST:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_AUTH:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_KEEPALIVE:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_PAYLOAD_SIZE:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_NOMEM:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_NOT_SUPPORTED:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_ERRNO:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->address, context->remote_port, strerror(errno));
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_SERVER_BUSY:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_BANNED:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_SESSION_TAKEN_OVER:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->address, context->remote_port);
						ಭಙ್ಗ;
					ಪಕ್ಷ MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						ಭಙ್ಗ;
					ಸ್ವಭಾವ:
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->address, context->remote_port, mosquitto_strerror(reason));
						ಭಙ್ಗ;
				}
			}ಅನ್ಯಥಾ{
				ಯದಿ(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
				}ಅನ್ಯಥಾ{
					log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->address, context->remote_port);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
