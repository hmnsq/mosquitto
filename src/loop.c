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

#அந்தர்பாவ "config.h"

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#அந்தர்பாவ <unistd.h>
#இல்லையெனில்
#அந்தர்பாவ <process.h>
#அந்தர்பாவ <winsock2.h>
#அந்தர்பாவ <ws2tcpip.h>
#endif

#அந்தர்பாவ <errno.h>
#அந்தர்பாவ <signal.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#அந்தர்பாவ <time.h>
#அந்தர்பாவ <utlist.h>

#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "packet_mosq.h"
#அந்தர்பாவ "property_common.h"
#அந்தர்பாவ "send_mosq.h"
#அந்தர்பாவ "sys_tree.h"
#அந்தர்பாவ "util_mosq.h"

வெளி முழு g_run;

#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_எண் == 3002000


வெறுமை lws__sul_callback(கட்டமைப்பு lws_sorted_usec_list *l)
{
}

நிலையான கட்டமைப்பு lws_sorted_usec_list sul;
#endif


நிலையான முழு single_publish(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto__message_v5 *pub_செய்தி, uint32_t message_expiry)
{
	கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி;
	uint16_t mid;

	base_செய்தி = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto_base_செய்தி));
	எனில்(base_செய்தி == வெற்று){
		திரும்பு MOSQ_ERR_NOMEM;
	}

	base_செய்தி->data.topic = pub_செய்தி->topic;
	pub_செய்தி->topic = வெற்று;
	base_செய்தி->data.retain = 0;
	base_செய்தி->data.payloadlen = (uint32_t)pub_செய்தி->payloadlen;
	base_செய்தி->data.payload = mosquitto_malloc(base_செய்தி->data.payloadlen+1);
	எனில்(base_செய்தி->data.payload == வெற்று){
		db_செய்தி_store_free(base_செய்தி);
		திரும்பு MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)base_செய்தி->data.payload)[base_செய்தி->data.payloadlen] = 0;
	memcpy(base_செய்தி->data.payload, pub_செய்தி->payload, base_செய்தி->data.payloadlen);

	எனில்(pub_செய்தி->properties){
		base_செய்தி->data.properties = pub_செய்தி->properties;
		pub_செய்தி->properties = வெற்று;
	}

	எனில்(db__message_store(context, base_செய்தி, &message_expiry, mosq_mo_broker)){
		திரும்பு 1;
	}

	எனில்(pub_செய்தி->qos){
		mid = mosquitto__mid_generate(context);
	}இல்லையெனில்{
		mid = 0;
	}
	திரும்பு db_message_செருகு_outgoing(context, 0, mid, (uint8_t)pub_செய்தி->qos, 0, base_செய்தி, 0, true, true);
}


நிலையான வெறுமை read_message_expiry_interval(mosquitto_property **proplist, uint32_t *message_expiry)
{
	mosquitto_property *p, *previous = வெற்று;

	*message_expiry = செய்தி_EXPIRY_INFINITE;

	எனில்(!proplist){
		திரும்பு;
	}

	p = *proplist;
	போது(p){
		எனில்(mosquitto_property_identifier(p) == MQTT_PROP_MESSAGE_EXPIRY_INTERVAL){
			*message_expiry = mosquitto_property_int32_மதிப்பு(p);
			எனில்(p == *proplist){
				*proplist = mosquitto_property_next(p);
			}இல்லையெனில்{
				previous->next = mosquitto_property_next(p);
			}
			mosquitto_property_free(&p);
			திரும்பு;

		}
		previous = p;
		p = mosquitto_property_next(p);
	}
}


நிலையான வெறுமை queue_plugin_msgs(வெறுமை)
{
	கட்டமைப்பு mosquitto__message_v5 *செய்தி, *tmp;
	கட்டமைப்பு mosquitto *context;
	uint32_t message_expiry;

	DL_FOREACH_SAFE(db.plugin_msgs, செய்தி, tmp){
		DL_DELETE(db.plugin_msgs, செய்தி);

		read_message_expiry_interval(&செய்தி->properties, &message_expiry);

		எனில்(செய்தி->clientid){
			HASH_FIND(hh_id, db.contexts_by_id, செய்தி->clientid, strlen(செய்தி->clientid), context);
			எனில்(context){
				single_publish(context, செய்தி, message_expiry);
			}
		}இல்லையெனில்{
			db__messages_easy_queue(வெற்று, செய்தி->topic, (uint8_t)செய்தி->qos, (uint32_t)செய்தி->payloadlen, செய்தி->payload, செய்தி->retain, message_expiry, &செய்தி->properties);
		}
		mosquitto_FREE(செய்தி->topic);
		mosquitto_FREE(செய்தி->payload);
		mosquitto_property_free_all(&செய்தி->properties);
		mosquitto_FREE(செய்தி->clientid);
		mosquitto_FREE(செய்தி);
	}
}


வெறுமை loop__update_next_event(time_t new_ms)
{
	எனில்(new_ms > 0 && new_ms < db.next_event_ms){
		db.next_event_ms = new_ms;
	}
}


முழு mosquitto_main_loop(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்)
{
#ifdef WITH_PERSISTENCE
	time_t last_backup = mosquitto_time();
#endif
	முழு rc;


	watchdog__init();
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_எண் == 3002000
	memset(&sul, 0, அளவு(கட்டமைப்பு lws_sorted_usec_list));
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(வெற்று);

#ifdef WITH_BRIDGE
	rc = bridge__register_local_connections();
	எனில்(rc){
		திரும்பு rc;
	}
#endif

	போது(g_run){
		retain__expiry_check();
		queue_plugin_msgs();
		context__free_disused();

		db.next_event_ms = 86400000;
#ifdef WITH_SYS_மரம்
		எனில்(db.config->sys_interval > 0){
			sys_மரம்_update(false);
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

		rc = mux__handle(listensock, listensock_மொத்தம்);
		எனில்(rc){
			திரும்பு rc;
		}

#ifdef WITH_PERSISTENCE
		எனில்(db.config->persistence && db.config->autosave_interval){
			எனில்(db.config->autosave_on_changes){
				எனில்(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}இல்லையெனில்{
				எனில்(last_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					last_backup = db.now_s;
				}
			}
		}
#endif

		rc = signal__flag_check();
		எனில்(rc){
			திரும்பு rc;
		}

#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		க்கு(முழு i=0; i<db.config->listener_மொத்தம்; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			எனில்(db.config->listeners[i].ws_context){
#எனில் LWS_LIBRARY_VERSION_எண் > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#அல்லதெனில் LWS_LIBRARY_VERSION_எண் == 3002000
				lws_sul_schedule(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#இல்லையெனில்
				lws_service(db.config->listeners[i].ws_context, 0);
#endif

			}
		}
#endif
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


வெறுமை do_disconnect(கட்டமைப்பு mosquitto *context, முழு reason)
{
	மாறிலி எழுத்து *id;
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#endif

	எனில்(context->state == mosq_cs_disconnected){
		திரும்பு;
	}
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	எனில்(context->wsi){
		எனில்(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		எனில்(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto__set_state(context, mosq_cs_disconnect_ws);
		}
		எனில்(context->wsi){
			lws_callback_on_writable(context->wsi);
		}
		எனில்(context->sock != INVALID_SOCKET){
			HASH_DELETE(hh_sock, db.contexts_by_sock, context);
			mux__delete(context);
			context->sock = INVALID_SOCKET;
		}
		எனில்(is_duplicate){
			/* This occurs if another client is taking over the same client id.
			 * It is important to remove this from the by_id hash here, so it
			 * doesn't leave us with multiple clients in the hash with the same
			 * id. Websockets doesn't actually close the connection here,
			 * unlike for normal clients, which means there is extra time when
			 * there could be two clients with the same id in the hash. */
			context__remove_from_by_id(context);
		}
	}இல்லையெனில்
#endif
	{
		எனில்(db.config->connection_messages == true){
			எனில்(context->id){
				id = context->id;
			}இல்லையெனில்{
				id = context->address;
			}
			எனில்(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				தேர்வு(reason){
					நிலை MOSQ_ERR_SUCCESS:
						நிறுத்து;
					நிலை MOSQ_ERR_MALFORMED_PACKET:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_PROTOCOL:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_CONN_LOST:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_AUTH:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_KEEPALIVE:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_PAYLOAD_SIZE:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_NOMEM:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_NOT_SUPPORTED:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_ERRNO:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->address, context->remote_port, strerror(errno));
						நிறுத்து;
					நிலை MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_SERVER_BUSY:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_BANNED:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_SESSION_TAKEN_OVER:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->address, context->remote_port);
						நிறுத்து;
					நிலை MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						நிறுத்து;
					இயல்பு:
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->address, context->remote_port, mosquitto_strerror(reason));
						நிறுத்து;
				}
			}இல்லையெனில்{
				எனில்(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
				}இல்லையெனில்{
					log__printf(வெற்று, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->address, context->remote_port);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
