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

#समावेश "config.h"

#अगर_अघोषित WIN32
#  define _GNU_SOURCE
#पूर्ण_अगर

#अगर_अघोषित WIN32
#समावेश <unistd.h>
#अन्यथा
#समावेश <process.h>
#समावेश <winsock2.h>
#समावेश <ws2tcpip.h>
#पूर्ण_अगर

#समावेश <errno.h>
#समावेश <signal.h>
#समावेश <stdio.h>
#समावेश <string.h>
#अगर_अघोषित WIN32
#  include <sys/socket.h>
#पूर्ण_अगर
#समावेश <time.h>
#समावेश <utlist.h>

#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#पूर्ण_अगर

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "packet_mosq.h"
#समावेश "property_common.h"
#समावेश "send_mosq.h"
#समावेश "sys_tree.h"
#समावेश "util_mosq.h"

बाह्य पूर्णांक g_run;

#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_संख्या == 3002000


व्योम lws__sul_callback(काष्ठा lws_sorted_usec_list *l)
{
}

अटल काष्ठा lws_sorted_usec_list sul;
#पूर्ण_अगर


अटल पूर्णांक single_publish(काष्ठा mosquitto *context, काष्ठा mosquitto__message_v5 *pub_संदेश, uint32_t message_expiry)
{
	काष्ठा mosquitto_base_संदेश *base_संदेश;
	uint16_t mid;

	base_संदेश = mosquitto_calloc(1, माप(काष्ठा mosquitto_base_संदेश));
	अगर(base_संदेश == शून्य){
		वापस MOSQ_ERR_NOMEM;
	}

	base_संदेश->तथ्य.topic = pub_संदेश->topic;
	pub_संदेश->topic = शून्य;
	base_संदेश->तथ्य.retain = 0;
	base_संदेश->तथ्य.payloadlen = (uint32_t)pub_संदेश->payloadlen;
	base_संदेश->तथ्य.payload = mosquitto_malloc(base_संदेश->तथ्य.payloadlen+1);
	अगर(base_संदेश->तथ्य.payload == शून्य){
		db_संदेश_store_free(base_संदेश);
		वापस MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)base_संदेश->तथ्य.payload)[base_संदेश->तथ्य.payloadlen] = 0;
	स_नकल(base_संदेश->तथ्य.payload, pub_संदेश->payload, base_संदेश->तथ्य.payloadlen);

	अगर(pub_संदेश->properties){
		base_संदेश->तथ्य.properties = pub_संदेश->properties;
		pub_संदेश->properties = शून्य;
	}

	अगर(db__message_store(context, base_संदेश, &message_expiry, mosq_mo_broker)){
		वापस 1;
	}

	अगर(pub_संदेश->qos){
		mid = mosquitto__mid_generate(context);
	}अन्यथा{
		mid = 0;
	}
	वापस db_message_जोड़ो_outgoing(context, 0, mid, (uint8_t)pub_संदेश->qos, 0, base_संदेश, 0, true, true);
}


अटल व्योम read_message_expiry_interval(mosquitto_property **proplist, uint32_t *message_expiry)
{
	mosquitto_property *p, *previous = शून्य;

	*message_expiry = संदेश_EXPIRY_INFINITE;

	अगर(!proplist){
		वापस;
	}

	p = *proplist;
	जबतक(p){
		अगर(mosquitto_property_identifier(p) == MQTT_PROP_MESSAGE_EXPIRY_INTERVAL){
			*message_expiry = mosquitto_property_int32_मूल्य(p);
			अगर(p == *proplist){
				*proplist = mosquitto_property_अग्र(p);
			}अन्यथा{
				previous->अग्र = mosquitto_property_अग्र(p);
			}
			mosquitto_property_free(&p);
			वापस;

		}
		previous = p;
		p = mosquitto_property_अग्र(p);
	}
}


अटल व्योम queue_plugin_msgs(व्योम)
{
	काष्ठा mosquitto__message_v5 *संदेश, *tmp;
	काष्ठा mosquitto *context;
	uint32_t message_expiry;

	DL_FOREACH_SAFE(db.plugin_msgs, संदेश, tmp){
		DL_DELETE(db.plugin_msgs, संदेश);

		read_message_expiry_interval(&संदेश->properties, &message_expiry);

		अगर(संदेश->clientid){
			HASH_FIND(hh_id, db.contexts_by_id, संदेश->clientid, म_माप(संदेश->clientid), context);
			अगर(context){
				single_publish(context, संदेश, message_expiry);
			}
		}अन्यथा{
			db__messages_easy_queue(शून्य, संदेश->topic, (uint8_t)संदेश->qos, (uint32_t)संदेश->payloadlen, संदेश->payload, संदेश->retain, message_expiry, &संदेश->properties);
		}
		mosquitto_FREE(संदेश->topic);
		mosquitto_FREE(संदेश->payload);
		mosquitto_property_free_all(&संदेश->properties);
		mosquitto_FREE(संदेश->clientid);
		mosquitto_FREE(संदेश);
	}
}


व्योम loop_update_अग्र_event(समय_प्रकार new_ms)
{
	अगर(new_ms > 0 && new_ms < db.अग्र_event_ms){
		db.अग्र_event_ms = new_ms;
	}
}


पूर्णांक mosquitto_main_loop(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती)
{
#अगर_घोषित WITH_PERSISTENCE
	समय_प्रकार last_backup = mosquitto_time();
#पूर्ण_अगर
	पूर्णांक rc;


	watchdog__init();
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_संख्या == 3002000
	स_रखो(&sul, 0, माप(काष्ठा lws_sorted_usec_list));
#पूर्ण_अगर

	db.now_s = mosquitto_time();
	db.now_real_s = समय(शून्य);

#अगर_घोषित WITH_BRIDGE
	rc = bridge__register_local_connections();
	अगर(rc){
		वापस rc;
	}
#पूर्ण_अगर

	जबतक(g_run){
		retain__expiry_check();
		queue_plugin_msgs();
		context__free_disused();

		db.अग्र_event_ms = 86400000;
#अगर_घोषित WITH_SYS_वृक्ष
		अगर(db.config->sys_interval > 0){
			sys_वृक्ष_update(false);
		}
#पूर्ण_अगर

		keepalive__check();
		watchdog__check();

#अगर_घोषित WITH_BRIDGE
		bridge_check();
#पूर्ण_अगर
		plugin__handle_tick();
		session_expiry__check();
		will_delay__check();

		rc = mux__handle(listensock, listensock_गिनती);
		अगर(rc){
			वापस rc;
		}

#अगर_घोषित WITH_PERSISTENCE
		अगर(db.config->persistence && db.config->autosave_interval){
			अगर(db.config->autosave_on_changes){
				अगर(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}अन्यथा{
				अगर(last_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					last_backup = db.now_s;
				}
			}
		}
#पूर्ण_अगर

		rc = signal__flag_check();
		अगर(rc){
			वापस rc;
		}

#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		क्रम(पूर्णांक i=0; i<db.config->listener_गिनती; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			अगर(db.config->listeners[i].ws_context){
#अगर LWS_LIBRARY_VERSION_संख्या > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#या_अगर LWS_LIBRARY_VERSION_संख्या == 3002000
				lws_sul_schedule(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#अन्यथा
				lws_service(db.config->listeners[i].ws_context, 0);
#पूर्ण_अगर

			}
		}
#पूर्ण_अगर
	}

	वापस MOSQ_ERR_SUCCESS;
}


व्योम do_disconnect(काष्ठा mosquitto *context, पूर्णांक reason)
{
	स्थिर अक्षर *id;
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#पूर्ण_अगर

	अगर(context->state == mosq_cs_disconnected){
		वापस;
	}
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	अगर(context->wsi){
		अगर(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		अगर(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto__set_state(context, mosq_cs_disconnect_ws);
		}
		अगर(context->wsi){
			lws_callback_on_writable(context->wsi);
		}
		अगर(context->sock != INVALID_SOCKET){
			HASH_DELETE(hh_sock, db.contexts_by_sock, context);
			mux__delete(context);
			context->sock = INVALID_SOCKET;
		}
		अगर(is_duplicate){
			/* This occurs if another client is taking over the same client id.
			 * It is important to remove this from the by_id hash here, so it
			 * doesn't leave us with multiple clients in the hash with the same
			 * id. Websockets doesn't actually close the connection here,
			 * unlike for normal clients, which means there is extra time when
			 * there could be two clients with the same id in the hash. */
			context__remove_from_by_id(context);
		}
	}अन्यथा
#पूर्ण_अगर
	{
		अगर(db.config->connection_messages == true){
			अगर(context->id){
				id = context->id;
			}अन्यथा{
				id = context->address;
			}
			अगर(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				चयन(reason){
					हाल MOSQ_ERR_SUCCESS:
						अवरोध;
					हाल MOSQ_ERR_MALFORMED_PACKET:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_PROTOCOL:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_CONN_LOST:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_AUTH:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_KEEPALIVE:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_PAYLOAD_आकार:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_NOMEM:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_NOT_SUPPORTED:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_ERRNO:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->address, context->remote_port, म_त्रुटि(त्रुटि_सं));
						अवरोध;
					हाल MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_SERVER_BUSY:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_BANNED:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_SESSION_TAKEN_OVER:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->address, context->remote_port);
						अवरोध;
					हाल MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						अवरोध;
					शेष:
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->address, context->remote_port, mosquitto_strerror(reason));
						अवरोध;
				}
			}अन्यथा{
				अगर(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
				}अन्यथा{
					log__printf(शून्य, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->address, context->remote_port);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
