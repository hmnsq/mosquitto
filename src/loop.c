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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#  define GNU_ਸਰੋਤ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਨਹੀਂਤਾਂ
#ਅਨ੍ਤਰ੍ਭਾਵ <process.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <winsock2.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ws2tcpip.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#  include <sys/socket.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <utlist.h>

#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "property_common.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "send_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sys_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"

ਬਾਹਰੀ ਪੂਰਨ g_ਲੜੀਵਾਰ;

#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_ਸੰਸਕਰਣ_ਸੰਖਿਆ == 3002000


ਖਾਲੀ lws__sul_callback(ਬਣਤਰ lws_sorted_usec_list *l)
{
}

ਸਥਿਰ ਬਣਤਰ lws_sorted_usec_list sul;
#ਜੇ_ਅੰਤ


ਸਥਿਰ ਪੂਰਨ single_publish(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_v5 *pub_ਸੁਨੇਹਾ, uint32_t ਸੁਨੇਹਾ_expiry)
{
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ;
	uint16_t mid;

	ਅਧਾਰ_ਸੁਨੇਹਾ = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ));
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic = pub_ਸੁਨੇਹਾ->topic;
	pub_ਸੁਨੇਹਾ->topic = ਨਿਰਾ;
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain = 0;
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen = (uint32_t)pub_ਸੁਨੇਹਾ->payloadlen;
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload = mosquitto_malloc(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen+1);
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload == ਨਿਰਾ){
		db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload)[ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen] = 0;
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload, pub_ਸੁਨੇਹਾ->payload, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen);

	ਜੇ(pub_ਸੁਨੇਹਾ->properties){
		ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.properties = pub_ਸੁਨੇਹਾ->properties;
		pub_ਸੁਨੇਹਾ->properties = ਨਿਰਾ;
	}

	ਜੇ(db_ਸੁਨੇਹਾ_store(context, ਅਧਾਰ_ਸੁਨੇਹਾ, &ਸੁਨੇਹਾ_expiry, mosq_mo_broker)){
		ਵਾਪਸ 1;
	}

	ਜੇ(pub_ਸੁਨੇਹਾ->qos){
		mid = mosquitto__mid_generate(context);
	}ਨਹੀਂਤਾਂ{
		mid = 0;
	}
	ਵਾਪਸ db_ਸੁਨੇਹਾ_ਪਾਓ_outgoing(context, 0, mid, (uint8_t)pub_ਸੁਨੇਹਾ->qos, 0, ਅਧਾਰ_ਸੁਨੇਹਾ, 0, true, true);
}


ਸਥਿਰ ਖਾਲੀ read_ਸੁਨੇਹਾ_expiry_interval(mosquitto_property **proplist, uint32_t *ਸੁਨੇਹਾ_expiry)
{
	mosquitto_property *p, *ਪਿਛਲਾ = ਨਿਰਾ;

	*ਸੁਨੇਹਾ_expiry = ਸੁਨੇਹਾ_EXPIRY_INFINITE;

	ਜੇ(!proplist){
		ਵਾਪਸ;
	}

	p = *proplist;
	ਜਦੋਂ(p){
		ਜੇ(mosquitto_property_identifier(p) == MQTT_PROP_ਸੁਨੇਹਾ_EXPIRY_INTERVAL){
			*ਸੁਨੇਹਾ_expiry = mosquitto_property_int32_ਮੁੱਲ(p);
			ਜੇ(p == *proplist){
				*proplist = mosquitto_property_ਅਗਲਾ(p);
			}ਨਹੀਂਤਾਂ{
				ਪਿਛਲਾ->ਅਗਲਾ = mosquitto_property_ਅਗਲਾ(p);
			}
			mosquitto_property_free(&p);
			ਵਾਪਸ;

		}
		ਪਿਛਲਾ = p;
		p = mosquitto_property_ਅਗਲਾ(p);
	}
}


ਸਥਿਰ ਖਾਲੀ ਕਤਾਰ_plugin_msgs(ਖਾਲੀ)
{
	ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_v5 *ਸੁਨੇਹਾ, *tmp;
	ਬਣਤਰ mosquitto *context;
	uint32_t ਸੁਨੇਹਾ_expiry;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(db.plugin_msgs, ਸੁਨੇਹਾ, tmp){
		DL_DELETE(db.plugin_msgs, ਸੁਨੇਹਾ);

		read_ਸੁਨੇਹਾ_expiry_interval(&ਸੁਨੇਹਾ->properties, &ਸੁਨੇਹਾ_expiry);

		ਜੇ(ਸੁਨੇਹਾ->clientid){
			HASH_ਲੱਭੋ(hh_id, db.contexts_by_id, ਸੁਨੇਹਾ->clientid, ਲੜੀ_ਲੰਬਾਈ(ਸੁਨੇਹਾ->clientid), context);
			ਜੇ(context){
				single_publish(context, ਸੁਨੇਹਾ, ਸੁਨੇਹਾ_expiry);
			}
		}ਨਹੀਂਤਾਂ{
			db_messages_easy_ਕਤਾਰ(ਨਿਰਾ, ਸੁਨੇਹਾ->topic, (uint8_t)ਸੁਨੇਹਾ->qos, (uint32_t)ਸੁਨੇਹਾ->payloadlen, ਸੁਨੇਹਾ->payload, ਸੁਨੇਹਾ->retain, ਸੁਨੇਹਾ_expiry, &ਸੁਨੇਹਾ->properties);
		}
		mosquitto_FREE(ਸੁਨੇਹਾ->topic);
		mosquitto_FREE(ਸੁਨੇਹਾ->payload);
		mosquitto_property_free_all(&ਸੁਨੇਹਾ->properties);
		mosquitto_FREE(ਸੁਨੇਹਾ->clientid);
		mosquitto_FREE(ਸੁਨੇਹਾ);
	}
}


ਖਾਲੀ loop_update_ਅਗਲਾ_event(time_t new_ms)
{
	ਜੇ(new_ms > 0 && new_ms < db.ਅਗਲਾ_event_ms){
		db.ਅਗਲਾ_event_ms = new_ms;
	}
}


ਪੂਰਨ mosquitto_main_loop(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE
	time_t ਆਖਰੀ_backup = mosquitto_ਸਮਾਂ();
#ਜੇ_ਅੰਤ
	ਪੂਰਨ rc;


	watchdog__init();
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_ਸੰਸਕਰਣ_ਸੰਖਿਆ == 3002000
	memset(&sul, 0, ਆਕਾਰ(ਬਣਤਰ lws_sorted_usec_list));
#ਜੇ_ਅੰਤ

	db.now_s = mosquitto_ਸਮਾਂ();
	db.now_real_s = ਸਮਾਂ(ਨਿਰਾ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
	rc = bridge_ਰਜਿਸਟਰ_local_connections();
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}
#ਜੇ_ਅੰਤ

	ਜਦੋਂ(g_ਲੜੀਵਾਰ){
		retain__expiry_check();
		ਕਤਾਰ_plugin_msgs();
		context__free_disused();

		db.ਅਗਲਾ_event_ms = 86400000;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYS_ਰੁੱਖ
		ਜੇ(db.config->sys_interval > 0){
			sys_ਰੁੱਖ_update(false);
		}
#ਜੇ_ਅੰਤ

		keepalive__check();
		watchdog__check();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
		bridge_check();
#ਜੇ_ਅੰਤ
		plugin__handle_tick();
		session_expiry__check();
		will_delay__check();

		rc = mux__handle(listensock, listensock_ਗਿਣਤੀ);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE
		ਜੇ(db.config->persistence && db.config->autosave_interval){
			ਜੇ(db.config->autosave_on_changes){
				ਜੇ(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}ਨਹੀਂਤਾਂ{
				ਜੇ(ਆਖਰੀ_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					ਆਖਰੀ_backup = db.now_s;
				}
			}
		}
#ਜੇ_ਅੰਤ

		rc = signal__flag_check();
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}

#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ਲਈ(ਪੂਰਨ i=0; i<db.config->listener_ਗਿਣਤੀ; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			ਜੇ(db.config->listeners[i].ws_context){
#ਜੇ LWS_LIBRARY_ਸੰਸਕਰਣ_ਸੰਖਿਆ > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#ਜਾਂਜੇ LWS_LIBRARY_ਸੰਸਕਰਣ_ਸੰਖਿਆ == 3002000
				lws_sul_ਸਮਾਂ_ਸਾਰਣੀ(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#ਨਹੀਂਤਾਂ
				lws_service(db.config->listeners[i].ws_context, 0);
#ਜੇ_ਅੰਤ

			}
		}
#ਜੇ_ਅੰਤ
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਖਾਲੀ do_disconnect(ਬਣਤਰ mosquitto *context, ਪੂਰਨ reason)
{
	ਅਚਲ ਅੱਖਰ *id;
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#ਜੇ_ਅੰਤ

	ਜੇ(context->state == mosq_cs_disconnected){
		ਵਾਪਸ;
	}
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ਜੇ(context->wsi){
		ਜੇ(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		ਜੇ(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto_ਸੈੱਟ_state(context, mosq_cs_disconnect_ws);
		}
		ਜੇ(context->wsi){
			lws_callback_on_writable(context->wsi);
		}
		ਜੇ(context->sock != INVALID_SOCKET){
			HASH_DELETE(hh_sock, db.contexts_by_sock, context);
			mux__delete(context);
			context->sock = INVALID_SOCKET;
		}
		ਜੇ(is_duplicate){
			/* This occurs if another client is taking over the same client id.
			 * It is important to remove this from the by_id hash here, so it
			 * doesn't leave us with multiple clients in the hash with the same
			 * id. Websockets doesn't actually close the connection here,
			 * unlike for normal clients, which means there is extra time when
			 * there could be two clients with the same id in the hash. */
			context__remove_from_by_id(context);
		}
	}ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
		ਜੇ(db.config->connection_messages == true){
			ਜੇ(context->id){
				id = context->id;
			}ਨਹੀਂਤਾਂ{
				id = context->ਪਤਾ;
			}
			ਜੇ(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				ਚੋਣ(reason){
					ਹਾਲਤ MOSQ_ERR_SUCCESS:
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_MALFORMED_PACKET:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_PROTOCOL:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_CONN_ਗੁਆਚਾ:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_AUTH:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_KEEPALIVE:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_PAYLOAD_ਮਾਪ:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_NOMEM:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_NOT_SUPPORTED:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_ERRNO:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ, strerror(errno));
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_RECEIVE_ਵੱਧ_ਤੋਂ_ਵੱਧ_EXCEEDED:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_SERVER_BUSY:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_BANNED:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_BAD_AUTHENTICATION_ਵਿਧੀ:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_CONNECTION_ਦਰ_EXCEEDED:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_SESSION_ਲਿਆ_OVER:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
						ਰੋਕੋ;
					ਹਾਲਤ MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						ਰੋਕੋ;
					ਮੂਲ:
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->ਪਤਾ, context->remote_ਪੋਰਟ, mosquitto_strerror(reason));
						ਰੋਕੋ;
				}
			}ਨਹੀਂਤਾਂ{
				ਜੇ(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
				}ਨਹੀਂਤਾਂ{
					log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->ਪਤਾ, context->remote_ਪੋਰਟ);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
