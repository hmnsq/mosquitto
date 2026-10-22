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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#  define GNU_ಮೂಲ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಇಲ್ಲದಿದ್ದರೆ
#ಅನ್ತರ್ಭಾವ <process.h>
#ಅನ್ತರ್ಭಾವ <winsock2.h>
#ಅನ್ತರ್ಭಾವ <ws2tcpip.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#  include <sys/socket.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <time.h>
#ಅನ್ತರ್ಭಾವ <utlist.h>

#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "property_common.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ g_ಸತತ;

#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ == 3002000


ಶೂನ್ಯ lws__sul_callback(ರಚನೆ lws_sorted_usec_list *l)
{
}

ಸ್ಥಿರ ರಚನೆ lws_sorted_usec_list sul;
#ಆದರೆ_ಅಂತ್ಯ


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ single_publish(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto_ಸಂದೇಶ_v5 *pub_ಸಂದೇಶ, uint32_t ಸಂದೇಶ_expiry)
{
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ;
	uint16_t mid;

	ತಳ_ಸಂದೇಶ = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ));
	ಆದರೆ(ತಳ_ಸಂದೇಶ == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic = pub_ಸಂದೇಶ->topic;
	pub_ಸಂದೇಶ->topic = ಖಾಲಿ;
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain = 0;
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen = (uint32_t)pub_ಸಂದೇಶ->payloadlen;
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload = mosquitto_malloc(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen+1);
	ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload == ಖಾಲಿ){
		db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload)[ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen] = 0;
	ಸ್ಮರಣೆ_ನಕಲು(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload, pub_ಸಂದೇಶ->payload, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen);

	ಆದರೆ(pub_ಸಂದೇಶ->properties){
		ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.properties = pub_ಸಂದೇಶ->properties;
		pub_ಸಂದೇಶ->properties = ಖಾಲಿ;
	}

	ಆದರೆ(db_ಸಂದೇಶ_store(context, ತಳ_ಸಂದೇಶ, &ಸಂದೇಶ_expiry, mosq_mo_broker)){
		ಹಿಂತಿರುಗಿ 1;
	}

	ಆದರೆ(pub_ಸಂದೇಶ->qos){
		mid = mosquitto__mid_generate(context);
	}ಇಲ್ಲದಿದ್ದರೆ{
		mid = 0;
	}
	ಹಿಂತಿರುಗಿ db_ಸಂದೇಶ_ಸೇರಿಸು_outgoing(context, 0, mid, (uint8_t)pub_ಸಂದೇಶ->qos, 0, ತಳ_ಸಂದೇಶ, 0, true, true);
}


ಸ್ಥಿರ ಶೂನ್ಯ read_ಸಂದೇಶ_expiry_interval(mosquitto_property **proplist, uint32_t *ಸಂದೇಶ_expiry)
{
	mosquitto_property *p, *ಹಿಂದಿನ = ಖಾಲಿ;

	*ಸಂದೇಶ_expiry = ಸಂದೇಶ_EXPIRY_INFINITE;

	ಆದರೆ(!proplist){
		ಹಿಂತಿರುಗಿ;
	}

	p = *proplist;
	ಆಗ(p){
		ಆದರೆ(mosquitto_property_identifier(p) == MQTT_PROP_ಸಂದೇಶ_EXPIRY_INTERVAL){
			*ಸಂದೇಶ_expiry = mosquitto_property_int32_ಮೌಲ್ಯ(p);
			ಆದರೆ(p == *proplist){
				*proplist = mosquitto_property_ಮುಂದಿನ(p);
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಹಿಂದಿನ->ಮುಂದಿನ = mosquitto_property_ಮುಂದಿನ(p);
			}
			mosquitto_property_free(&p);
			ಹಿಂತಿರುಗಿ;

		}
		ಹಿಂದಿನ = p;
		p = mosquitto_property_ಮುಂದಿನ(p);
	}
}


ಸ್ಥಿರ ಶೂನ್ಯ ಸರತಿ_plugin_msgs(ಶೂನ್ಯ)
{
	ರಚನೆ mosquitto_ಸಂದೇಶ_v5 *ಸಂದೇಶ, *tmp;
	ರಚನೆ mosquitto *context;
	uint32_t ಸಂದೇಶ_expiry;

	DL_FOREACH_ಸುರಕ್ಷಿತ(db.plugin_msgs, ಸಂದೇಶ, tmp){
		DL_DELETE(db.plugin_msgs, ಸಂದೇಶ);

		read_ಸಂದೇಶ_expiry_interval(&ಸಂದೇಶ->properties, &ಸಂದೇಶ_expiry);

		ಆದರೆ(ಸಂದೇಶ->clientid){
			HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id, ಸಂದೇಶ->clientid, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಸಂದೇಶ->clientid), context);
			ಆದರೆ(context){
				single_publish(context, ಸಂದೇಶ, ಸಂದೇಶ_expiry);
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			db_messages_easy_ಸರತಿ(ಖಾಲಿ, ಸಂದೇಶ->topic, (uint8_t)ಸಂದೇಶ->qos, (uint32_t)ಸಂದೇಶ->payloadlen, ಸಂದೇಶ->payload, ಸಂದೇಶ->retain, ಸಂದೇಶ_expiry, &ಸಂದೇಶ->properties);
		}
		mosquitto_FREE(ಸಂದೇಶ->topic);
		mosquitto_FREE(ಸಂದೇಶ->payload);
		mosquitto_property_free_all(&ಸಂದೇಶ->properties);
		mosquitto_FREE(ಸಂದೇಶ->clientid);
		mosquitto_FREE(ಸಂದೇಶ);
	}
}


ಶೂನ್ಯ loop_update_ಮುಂದಿನ_event(time_t new_ms)
{
	ಆದರೆ(new_ms > 0 && new_ms < db.ಮುಂದಿನ_event_ms){
		db.ಮುಂದಿನ_event_ms = new_ms;
	}
}


ಪೂರ್ಣಾಂಕ mosquitto_main_loop(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_PERSISTENCE
	time_t ಕೊನೆಯ_backup = mosquitto_ಸಮಯ();
#ಆದರೆ_ಅಂತ್ಯ
	ಪೂರ್ಣಾಂಕ rc;


	watchdog__init();
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ == 3002000
	memset(&sul, 0, ಗಾತ್ರ(ರಚನೆ lws_sorted_usec_list));
#ಆದರೆ_ಅಂತ್ಯ

	db.now_s = mosquitto_ಸಮಯ();
	db.now_real_s = ಸಮಯ(ಖಾಲಿ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
	rc = bridge_ರಿಜಿಸ್ಟರ್_local_connections();
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಆಗ(g_ಸತತ){
		retain__expiry_check();
		ಸರತಿ_plugin_msgs();
		context__free_disused();

		db.ಮುಂದಿನ_event_ms = 86400000;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYS_ವೃಕ್ಷ
		ಆದರೆ(db.config->sys_interval > 0){
			sys_ವೃಕ್ಷ_update(false);
		}
#ಆದರೆ_ಅಂತ್ಯ

		keepalive__check();
		watchdog__check();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
		bridge_check();
#ಆದರೆ_ಅಂತ್ಯ
		plugin__handle_tick();
		session_expiry__check();
		will_delay__check();

		rc = mux__handle(listensock, listensock_ಎಣಿಕೆ);
		ಆದರೆ(rc){
			ಹಿಂತಿರುಗಿ rc;
		}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_PERSISTENCE
		ಆದರೆ(db.config->persistence && db.config->autosave_interval){
			ಆದರೆ(db.config->autosave_on_changes){
				ಆದರೆ(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಆದರೆ(ಕೊನೆಯ_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					ಕೊನೆಯ_backup = db.now_s;
				}
			}
		}
#ಆದರೆ_ಅಂತ್ಯ

		rc = signal__flag_check();
		ಆದರೆ(rc){
			ಹಿಂತಿರುಗಿ rc;
		}

#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			ಆದರೆ(db.config->listeners[i].ws_context){
#ಆದರೆ LWS_LIBRARY_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#ಅಥವಾಆದರೆ LWS_LIBRARY_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ == 3002000
				lws_sul_ವೇಳಾಪಟ್ಟಿ(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#ಇಲ್ಲದಿದ್ದರೆ
				lws_service(db.config->listeners[i].ws_context, 0);
#ಆದರೆ_ಅಂತ್ಯ

			}
		}
#ಆದರೆ_ಅಂತ್ಯ
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಶೂನ್ಯ do_disconnect(ರಚನೆ mosquitto *context, ಪೂರ್ಣಾಂಕ reason)
{
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *id;
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ(context->state == mosq_cs_disconnected){
		ಹಿಂತಿರುಗಿ;
	}
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ಆದರೆ(context->wsi){
		ಆದರೆ(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		ಆದರೆ(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto_ಗಣ_state(context, mosq_cs_disconnect_ws);
		}
		ಆದರೆ(context->wsi){
			lws_callback_on_writable(context->wsi);
		}
		ಆದರೆ(context->sock != INVALID_SOCKET){
			HASH_DELETE(hh_sock, db.contexts_by_sock, context);
			mux__delete(context);
			context->sock = INVALID_SOCKET;
		}
		ಆದರೆ(is_duplicate){
			/* This occurs if another client is taking over the same client id.
			 * It is important to remove this from the by_id hash here, so it
			 * doesn't leave us with multiple clients in the hash with the same
			 * id. Websockets doesn't actually close the connection here,
			 * unlike for normal clients, which means there is extra time when
			 * there could be two clients with the same id in the hash. */
			context__remove_from_by_id(context);
		}
	}ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
		ಆದರೆ(db.config->connection_messages == true){
			ಆದರೆ(context->id){
				id = context->id;
			}ಇಲ್ಲದಿದ್ದರೆ{
				id = context->ವಿಳಾಸ;
			}
			ಆದರೆ(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				ಆಯ್ಕೆ(reason){
					ಸ್ಥಿತಿ MOSQ_ERR_SUCCESS:
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_MALFORMED_PACKET:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_PROTOCOL:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_CONN_ಕಳೆದುಹೋದ:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_AUTH:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_KEEPALIVE:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_PAYLOAD_ಗಾತ್ರ:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_NOMEM:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_NOT_SUPPORTED:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_ERRNO:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್, strerror(errno));
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_RECEIVE_ಗರಿಷ್ಠ_EXCEEDED:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_SERVER_BUSY:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_BANNED:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_BAD_AUTHENTICATION_ವಿಧಾನ:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_CONNECTION_ದರ_EXCEEDED:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_SESSION_ತೆಗೆದುಕೊಂಡ_OVER:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
						ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						ನಿಲ್ಲಿಸು;
					ಪೂರ್ವನಿಯೋಜಿತ:
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್, mosquitto_strerror(reason));
						ನಿಲ್ಲಿಸು;
				}
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಆದರೆ(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
				}ಇಲ್ಲದಿದ್ದರೆ{
					log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->ವಿಳಾಸ, context->remote_ಪೋರ್ಟ್);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
