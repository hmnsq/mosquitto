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

#അന്തര്ഭാവ "config.h"

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#അന്തര്ഭാവ <unistd.h>
#അല്ലെങ്കിൽ
#അന്തര്ഭാവ <process.h>
#അന്തര്ഭാവ <winsock2.h>
#അന്തര്ഭാവ <ws2tcpip.h>
#endif

#അന്തര്ഭാവ <errno.h>
#അന്തര്ഭാവ <signal.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#അന്തര്ഭാവ <time.h>
#അന്തര്ഭാവ <utlist.h>

#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "packet_mosq.h"
#അന്തര്ഭാവ "property_common.h"
#അന്തര്ഭാവ "send_mosq.h"
#അന്തര്ഭാവ "sys_tree.h"
#അന്തര്ഭാവ "util_mosq.h"

ബാഹ്യ പൂർണ്ണസംഖ്യ g_run;

#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_സംഖ്യ == 3002000


ശൂന്യം lws__sul_callback(ഘടന lws_sorted_usec_list *l)
{
}

സ്ഥിതം ഘടന lws_sorted_usec_list sul;
#endif


സ്ഥിതം പൂർണ്ണസംഖ്യ single_publish(ഘടന mosquitto *context, ഘടന mosquitto__message_v5 *pub_സന്ദേശം, uint32_t message_expiry)
{
	ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം;
	uint16_t mid;

	base_സന്ദേശം = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto_base_സന്ദേശം));
	എങ്കിൽ(base_സന്ദേശം == ശൂന്യ){
		തിരികെ MOSQ_ERR_NOMEM;
	}

	base_സന്ദേശം->data.topic = pub_സന്ദേശം->topic;
	pub_സന്ദേശം->topic = ശൂന്യ;
	base_സന്ദേശം->data.retain = 0;
	base_സന്ദേശം->data.payloadlen = (uint32_t)pub_സന്ദേശം->payloadlen;
	base_സന്ദേശം->data.payload = mosquitto_malloc(base_സന്ദേശം->data.payloadlen+1);
	എങ്കിൽ(base_സന്ദേശം->data.payload == ശൂന്യ){
		db_സന്ദേശം_store_free(base_സന്ദേശം);
		തിരികെ MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)base_സന്ദേശം->data.payload)[base_സന്ദേശം->data.payloadlen] = 0;
	memcpy(base_സന്ദേശം->data.payload, pub_സന്ദേശം->payload, base_സന്ദേശം->data.payloadlen);

	എങ്കിൽ(pub_സന്ദേശം->properties){
		base_സന്ദേശം->data.properties = pub_സന്ദേശം->properties;
		pub_സന്ദേശം->properties = ശൂന്യ;
	}

	എങ്കിൽ(db__message_store(context, base_സന്ദേശം, &message_expiry, mosq_mo_broker)){
		തിരികെ 1;
	}

	എങ്കിൽ(pub_സന്ദേശം->qos){
		mid = mosquitto__mid_generate(context);
	}അല്ലെങ്കിൽ{
		mid = 0;
	}
	തിരികെ db_message_തിരുകുക_outgoing(context, 0, mid, (uint8_t)pub_സന്ദേശം->qos, 0, base_സന്ദേശം, 0, true, true);
}


സ്ഥിതം ശൂന്യം read_message_expiry_interval(mosquitto_property **proplist, uint32_t *message_expiry)
{
	mosquitto_property *p, *previous = ശൂന്യ;

	*message_expiry = സന്ദേശം_EXPIRY_INFINITE;

	എങ്കിൽ(!proplist){
		തിരികെ;
	}

	p = *proplist;
	ആകുമ്പോൾ(p){
		എങ്കിൽ(mosquitto_property_identifier(p) == MQTT_PROP_MESSAGE_EXPIRY_INTERVAL){
			*message_expiry = mosquitto_property_int32_മൂല്യം(p);
			എങ്കിൽ(p == *proplist){
				*proplist = mosquitto_property_next(p);
			}അല്ലെങ്കിൽ{
				previous->next = mosquitto_property_next(p);
			}
			mosquitto_property_free(&p);
			തിരികെ;

		}
		previous = p;
		p = mosquitto_property_next(p);
	}
}


സ്ഥിതം ശൂന്യം queue_plugin_msgs(ശൂന്യം)
{
	ഘടന mosquitto__message_v5 *സന്ദേശം, *tmp;
	ഘടന mosquitto *context;
	uint32_t message_expiry;

	DL_FOREACH_SAFE(db.plugin_msgs, സന്ദേശം, tmp){
		DL_DELETE(db.plugin_msgs, സന്ദേശം);

		read_message_expiry_interval(&സന്ദേശം->properties, &message_expiry);

		എങ്കിൽ(സന്ദേശം->clientid){
			HASH_FIND(hh_id, db.contexts_by_id, സന്ദേശം->clientid, strlen(സന്ദേശം->clientid), context);
			എങ്കിൽ(context){
				single_publish(context, സന്ദേശം, message_expiry);
			}
		}അല്ലെങ്കിൽ{
			db__messages_easy_queue(ശൂന്യ, സന്ദേശം->topic, (uint8_t)സന്ദേശം->qos, (uint32_t)സന്ദേശം->payloadlen, സന്ദേശം->payload, സന്ദേശം->retain, message_expiry, &സന്ദേശം->properties);
		}
		mosquitto_FREE(സന്ദേശം->topic);
		mosquitto_FREE(സന്ദേശം->payload);
		mosquitto_property_free_all(&സന്ദേശം->properties);
		mosquitto_FREE(സന്ദേശം->clientid);
		mosquitto_FREE(സന്ദേശം);
	}
}


ശൂന്യം loop__update_next_event(time_t new_ms)
{
	എങ്കിൽ(new_ms > 0 && new_ms < db.next_event_ms){
		db.next_event_ms = new_ms;
	}
}


പൂർണ്ണസംഖ്യ mosquitto_main_loop(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം)
{
#ifdef WITH_PERSISTENCE
	time_t last_backup = mosquitto_time();
#endif
	പൂർണ്ണസംഖ്യ rc;


	watchdog__init();
#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_സംഖ്യ == 3002000
	memset(&sul, 0, വലുപ്പം(ഘടന lws_sorted_usec_list));
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(ശൂന്യ);

#ifdef WITH_BRIDGE
	rc = bridge__register_local_connections();
	എങ്കിൽ(rc){
		തിരികെ rc;
	}
#endif

	ആകുമ്പോൾ(g_run){
		retain__expiry_check();
		queue_plugin_msgs();
		context__free_disused();

		db.next_event_ms = 86400000;
#ifdef WITH_SYS_വൃക്ഷം
		എങ്കിൽ(db.config->sys_interval > 0){
			sys_വൃക്ഷം_update(false);
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

		rc = mux__handle(listensock, listensock_എണ്ണം);
		എങ്കിൽ(rc){
			തിരികെ rc;
		}

#ifdef WITH_PERSISTENCE
		എങ്കിൽ(db.config->persistence && db.config->autosave_interval){
			എങ്കിൽ(db.config->autosave_on_changes){
				എങ്കിൽ(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}അല്ലെങ്കിൽ{
				എങ്കിൽ(last_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					last_backup = db.now_s;
				}
			}
		}
#endif

		rc = signal__flag_check();
		എങ്കിൽ(rc){
			തിരികെ rc;
		}

#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<db.config->listener_എണ്ണം; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			എങ്കിൽ(db.config->listeners[i].ws_context){
#എങ്കിൽ LWS_LIBRARY_VERSION_സംഖ്യ > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#അല്ലെങ്കിലെങ്കിൽ LWS_LIBRARY_VERSION_സംഖ്യ == 3002000
				lws_sul_schedule(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#അല്ലെങ്കിൽ
				lws_service(db.config->listeners[i].ws_context, 0);
#endif

			}
		}
#endif
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


ശൂന്യം do_disconnect(ഘടന mosquitto *context, പൂർണ്ണസംഖ്യ reason)
{
	സ്ഥിരം അക്ഷരം *id;
#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#endif

	എങ്കിൽ(context->state == mosq_cs_disconnected){
		തിരികെ;
	}
#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	എങ്കിൽ(context->wsi){
		എങ്കിൽ(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		എങ്കിൽ(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto__set_state(context, mosq_cs_disconnect_ws);
		}
		എങ്കിൽ(context->wsi){
			lws_callback_on_writable(context->wsi);
		}
		എങ്കിൽ(context->sock != INVALID_SOCKET){
			HASH_DELETE(hh_sock, db.contexts_by_sock, context);
			mux__delete(context);
			context->sock = INVALID_SOCKET;
		}
		എങ്കിൽ(is_duplicate){
			/* This occurs if another client is taking over the same client id.
			 * It is important to remove this from the by_id hash here, so it
			 * doesn't leave us with multiple clients in the hash with the same
			 * id. Websockets doesn't actually close the connection here,
			 * unlike for normal clients, which means there is extra time when
			 * there could be two clients with the same id in the hash. */
			context__remove_from_by_id(context);
		}
	}അല്ലെങ്കിൽ
#endif
	{
		എങ്കിൽ(db.config->connection_messages == true){
			എങ്കിൽ(context->id){
				id = context->id;
			}അല്ലെങ്കിൽ{
				id = context->address;
			}
			എങ്കിൽ(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				തിരഞ്ഞെടുപ്പ്(reason){
					അവസ്ഥ MOSQ_ERR_SUCCESS:
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_MALFORMED_PACKET:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_PROTOCOL:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_CONN_LOST:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_AUTH:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_KEEPALIVE:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_PAYLOAD_SIZE:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_NOMEM:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_NOT_SUPPORTED:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_ERRNO:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->address, context->remote_port, strerror(errno));
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_SERVER_BUSY:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_BANNED:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_SESSION_TAKEN_OVER:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->address, context->remote_port);
						നിർത്തുക;
					അവസ്ഥ MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						നിർത്തുക;
					സ്ഥിരസ്ഥിതി:
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->address, context->remote_port, mosquitto_strerror(reason));
						നിർത്തുക;
				}
			}അല്ലെങ്കിൽ{
				എങ്കിൽ(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
				}അല്ലെങ്കിൽ{
					log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->address, context->remote_port);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
