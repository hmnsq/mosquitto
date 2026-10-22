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

#અન્તર્ભાવ "config.h"

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#અન્તર્ભાવ <unistd.h>
#નહીંતર
#અન્તર્ભાવ <process.h>
#અન્તર્ભાવ <winsock2.h>
#અન્તર્ભાવ <ws2tcpip.h>
#endif

#અન્તર્ભાવ <errno.h>
#અન્તર્ભાવ <signal.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#અન્તર્ભાવ <time.h>
#અન્તર્ભાવ <utlist.h>

#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "packet_mosq.h"
#અન્તર્ભાવ "property_common.h"
#અન્તર્ભાવ "send_mosq.h"
#અન્તર્ભાવ "sys_tree.h"
#અન્તર્ભાવ "util_mosq.h"

બાહ્ય પૂર્ણાંક g_run;

#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_સંખ્યા == 3002000


શૂન્ય lws__sul_callback(સંરચના lws_sorted_usec_list *l)
{
}

સ્થિર સંરચના lws_sorted_usec_list sul;
#endif


સ્થિર પૂર્ણાંક single_publish(સંરચના mosquitto *context, સંરચના mosquitto__message_v5 *pub_સંદેશ, uint32_t message_expiry)
{
	સંરચના mosquitto_base_સંદેશ *base_સંદેશ;
	uint16_t mid;

	base_સંદેશ = mosquitto_calloc(1, કદ(સંરચના mosquitto_base_સંદેશ));
	જો(base_સંદેશ == ખાલી){
		પાછા MOSQ_ERR_NOMEM;
	}

	base_સંદેશ->data.topic = pub_સંદેશ->topic;
	pub_સંદેશ->topic = ખાલી;
	base_સંદેશ->data.retain = 0;
	base_સંદેશ->data.payloadlen = (uint32_t)pub_સંદેશ->payloadlen;
	base_સંદેશ->data.payload = mosquitto_malloc(base_સંદેશ->data.payloadlen+1);
	જો(base_સંદેશ->data.payload == ખાલી){
		db_સંદેશ_store_free(base_સંદેશ);
		પાછા MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)base_સંદેશ->data.payload)[base_સંદેશ->data.payloadlen] = 0;
	memcpy(base_સંદેશ->data.payload, pub_સંદેશ->payload, base_સંદેશ->data.payloadlen);

	જો(pub_સંદેશ->properties){
		base_સંદેશ->data.properties = pub_સંદેશ->properties;
		pub_સંદેશ->properties = ખાલી;
	}

	જો(db__message_store(context, base_સંદેશ, &message_expiry, mosq_mo_broker)){
		પાછા 1;
	}

	જો(pub_સંદેશ->qos){
		mid = mosquitto__mid_generate(context);
	}નહીંતર{
		mid = 0;
	}
	પાછા db_message_ઉમેરો_outgoing(context, 0, mid, (uint8_t)pub_સંદેશ->qos, 0, base_સંદેશ, 0, true, true);
}


સ્થિર શૂન્ય read_message_expiry_interval(mosquitto_property **proplist, uint32_t *message_expiry)
{
	mosquitto_property *p, *previous = ખાલી;

	*message_expiry = સંદેશ_EXPIRY_INFINITE;

	જો(!proplist){
		પાછા;
	}

	p = *proplist;
	જ્યારે(p){
		જો(mosquitto_property_identifier(p) == MQTT_PROP_MESSAGE_EXPIRY_INTERVAL){
			*message_expiry = mosquitto_property_int32_કિંમત(p);
			જો(p == *proplist){
				*proplist = mosquitto_property_next(p);
			}નહીંતર{
				previous->next = mosquitto_property_next(p);
			}
			mosquitto_property_free(&p);
			પાછા;

		}
		previous = p;
		p = mosquitto_property_next(p);
	}
}


સ્થિર શૂન્ય queue_plugin_msgs(શૂન્ય)
{
	સંરચના mosquitto__message_v5 *સંદેશ, *tmp;
	સંરચના mosquitto *context;
	uint32_t message_expiry;

	DL_FOREACH_SAFE(db.plugin_msgs, સંદેશ, tmp){
		DL_DELETE(db.plugin_msgs, સંદેશ);

		read_message_expiry_interval(&સંદેશ->properties, &message_expiry);

		જો(સંદેશ->clientid){
			HASH_FIND(hh_id, db.contexts_by_id, સંદેશ->clientid, strlen(સંદેશ->clientid), context);
			જો(context){
				single_publish(context, સંદેશ, message_expiry);
			}
		}નહીંતર{
			db__messages_easy_queue(ખાલી, સંદેશ->topic, (uint8_t)સંદેશ->qos, (uint32_t)સંદેશ->payloadlen, સંદેશ->payload, સંદેશ->retain, message_expiry, &સંદેશ->properties);
		}
		mosquitto_FREE(સંદેશ->topic);
		mosquitto_FREE(સંદેશ->payload);
		mosquitto_property_free_all(&સંદેશ->properties);
		mosquitto_FREE(સંદેશ->clientid);
		mosquitto_FREE(સંદેશ);
	}
}


શૂન્ય loop__update_next_event(time_t new_ms)
{
	જો(new_ms > 0 && new_ms < db.next_event_ms){
		db.next_event_ms = new_ms;
	}
}


પૂર્ણાંક mosquitto_main_loop(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી)
{
#ifdef WITH_PERSISTENCE
	time_t last_backup = mosquitto_time();
#endif
	પૂર્ણાંક rc;


	watchdog__init();
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_સંખ્યા == 3002000
	memset(&sul, 0, કદ(સંરચના lws_sorted_usec_list));
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = time(ખાલી);

#ifdef WITH_BRIDGE
	rc = bridge__register_local_connections();
	જો(rc){
		પાછા rc;
	}
#endif

	જ્યારે(g_run){
		retain__expiry_check();
		queue_plugin_msgs();
		context__free_disused();

		db.next_event_ms = 86400000;
#ifdef WITH_SYS_વૃક્ષ
		જો(db.config->sys_interval > 0){
			sys_વૃક્ષ_update(false);
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

		rc = mux__handle(listensock, listensock_ગણતરી);
		જો(rc){
			પાછા rc;
		}

#ifdef WITH_PERSISTENCE
		જો(db.config->persistence && db.config->autosave_interval){
			જો(db.config->autosave_on_changes){
				જો(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}નહીંતર{
				જો(last_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					last_backup = db.now_s;
				}
			}
		}
#endif

		rc = signal__flag_check();
		જો(rc){
			પાછા rc;
		}

#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		માટે(પૂર્ણાંક i=0; i<db.config->listener_ગણતરી; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			જો(db.config->listeners[i].ws_context){
#જો LWS_LIBRARY_VERSION_સંખ્યા > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#અથવાજો LWS_LIBRARY_VERSION_સંખ્યા == 3002000
				lws_sul_schedule(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#નહીંતર
				lws_service(db.config->listeners[i].ws_context, 0);
#endif

			}
		}
#endif
	}

	પાછા MOSQ_ERR_SUCCESS;
}


શૂન્ય do_disconnect(સંરચના mosquitto *context, પૂર્ણાંક reason)
{
	અચલ અક્ષર *id;
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#endif

	જો(context->state == mosq_cs_disconnected){
		પાછા;
	}
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	જો(context->wsi){
		જો(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		જો(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto__set_state(context, mosq_cs_disconnect_ws);
		}
		જો(context->wsi){
			lws_callback_on_writable(context->wsi);
		}
		જો(context->sock != INVALID_SOCKET){
			HASH_DELETE(hh_sock, db.contexts_by_sock, context);
			mux__delete(context);
			context->sock = INVALID_SOCKET;
		}
		જો(is_duplicate){
			/* This occurs if another client is taking over the same client id.
			 * It is important to remove this from the by_id hash here, so it
			 * doesn't leave us with multiple clients in the hash with the same
			 * id. Websockets doesn't actually close the connection here,
			 * unlike for normal clients, which means there is extra time when
			 * there could be two clients with the same id in the hash. */
			context__remove_from_by_id(context);
		}
	}નહીંતર
#endif
	{
		જો(db.config->connection_messages == true){
			જો(context->id){
				id = context->id;
			}નહીંતર{
				id = context->address;
			}
			જો(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				પસંદગી(reason){
					સ્થિતિ MOSQ_ERR_SUCCESS:
						તોડો;
					સ્થિતિ MOSQ_ERR_MALFORMED_PACKET:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_PROTOCOL:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_CONN_LOST:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_AUTH:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_KEEPALIVE:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_PAYLOAD_SIZE:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_NOMEM:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_NOT_SUPPORTED:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_ERRNO:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->address, context->remote_port, strerror(errno));
						તોડો;
					સ્થિતિ MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_SERVER_BUSY:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_BANNED:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_SESSION_TAKEN_OVER:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->address, context->remote_port);
						તોડો;
					સ્થિતિ MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						તોડો;
					મૂળભૂત:
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->address, context->remote_port, mosquitto_strerror(reason));
						તોડો;
				}
			}નહીંતર{
				જો(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
				}નહીંતર{
					log__printf(ખાલી, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->address, context->remote_port);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
