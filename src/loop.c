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

#अन्तर्भाव "config.h"

#ifndef WIN32
#  define _GNU_SOURCE
#endif

#ifndef WIN32
#अन्तर्भाव <unistd.h>
#अन्यथा
#अन्तर्भाव <process.h>
#अन्तर्भाव <winsock2.h>
#अन्तर्भाव <ws2tcpip.h>
#endif

#अन्तर्भाव <errno.h>
#अन्तर्भाव <signal.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#अन्तर्भाव <time.h>
#अन्तर्भाव <utlist.h>

#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "property_common.h"
#अन्तर्भाव "send_mosq.h"
#अन्तर्भाव "sys_tree.h"
#अन्तर्भाव "util_mosq.h"

बाह्य पूर्णांक g_run;

#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_संख्या == 3002000


शून्य lws__sul_callback(संरचना lws_sorted_usec_list *l)
{
}

स्थिर संरचना lws_sorted_usec_list sul;
#endif


स्थिर पूर्णांक single_publish(संरचना mosquitto *context, संरचना mosquitto__message_v5 *pub_संदेश, uint32_t message_expiry)
{
	संरचना mosquitto_base_संदेश *base_संदेश;
	uint16_t mid;

	base_संदेश = mosquitto_calloc(1, आकार(संरचना mosquitto_base_संदेश));
	जर(base_संदेश == रिक्त){
		परत MOSQ_ERR_NOMEM;
	}

	base_संदेश->data.topic = pub_संदेश->topic;
	pub_संदेश->topic = रिक्त;
	base_संदेश->data.retain = 0;
	base_संदेश->data.payloadlen = (uint32_t)pub_संदेश->payloadlen;
	base_संदेश->data.payload = mosquitto_malloc(base_संदेश->data.payloadlen+1);
	जर(base_संदेश->data.payload == रिक्त){
		db_संदेश_store_free(base_संदेश);
		परत MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)base_संदेश->data.payload)[base_संदेश->data.payloadlen] = 0;
	स_नकल(base_संदेश->data.payload, pub_संदेश->payload, base_संदेश->data.payloadlen);

	जर(pub_संदेश->properties){
		base_संदेश->data.properties = pub_संदेश->properties;
		pub_संदेश->properties = रिक्त;
	}

	जर(db__message_store(context, base_संदेश, &message_expiry, mosq_mo_broker)){
		परत 1;
	}

	जर(pub_संदेश->qos){
		mid = mosquitto__mid_generate(context);
	}अन्यथा{
		mid = 0;
	}
	परत db_message_घाला_outgoing(context, 0, mid, (uint8_t)pub_संदेश->qos, 0, base_संदेश, 0, true, true);
}


स्थिर शून्य read_message_expiry_interval(mosquitto_property **proplist, uint32_t *message_expiry)
{
	mosquitto_property *p, *previous = रिक्त;

	*message_expiry = संदेश_EXPIRY_INFINITE;

	जर(!proplist){
		परत;
	}

	p = *proplist;
	पर्यंत(p){
		जर(mosquitto_property_identifier(p) == MQTT_PROP_MESSAGE_EXPIRY_INTERVAL){
			*message_expiry = mosquitto_property_int32_मूल्य(p);
			जर(p == *proplist){
				*proplist = mosquitto_property_next(p);
			}अन्यथा{
				previous->next = mosquitto_property_next(p);
			}
			mosquitto_property_free(&p);
			परत;

		}
		previous = p;
		p = mosquitto_property_next(p);
	}
}


स्थिर शून्य queue_plugin_msgs(शून्य)
{
	संरचना mosquitto__message_v5 *संदेश, *tmp;
	संरचना mosquitto *context;
	uint32_t message_expiry;

	DL_FOREACH_SAFE(db.plugin_msgs, संदेश, tmp){
		DL_DELETE(db.plugin_msgs, संदेश);

		read_message_expiry_interval(&संदेश->properties, &message_expiry);

		जर(संदेश->clientid){
			HASH_FIND(hh_id, db.contexts_by_id, संदेश->clientid, म_माप(संदेश->clientid), context);
			जर(context){
				single_publish(context, संदेश, message_expiry);
			}
		}अन्यथा{
			db__messages_easy_queue(रिक्त, संदेश->topic, (uint8_t)संदेश->qos, (uint32_t)संदेश->payloadlen, संदेश->payload, संदेश->retain, message_expiry, &संदेश->properties);
		}
		mosquitto_FREE(संदेश->topic);
		mosquitto_FREE(संदेश->payload);
		mosquitto_property_free_all(&संदेश->properties);
		mosquitto_FREE(संदेश->clientid);
		mosquitto_FREE(संदेश);
	}
}


शून्य loop__update_next_event(समय_प्रकार new_ms)
{
	जर(new_ms > 0 && new_ms < db.next_event_ms){
		db.next_event_ms = new_ms;
	}
}


पूर्णांक mosquitto_main_loop(संरचना mosquitto__listener_sock *listensock, पूर्णांक listensock_मोजणी)
{
#ifdef WITH_PERSISTENCE
	समय_प्रकार last_backup = mosquitto_time();
#endif
	पूर्णांक rc;


	watchdog__init();
#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_संख्या == 3002000
	स_रखो(&sul, 0, आकार(संरचना lws_sorted_usec_list));
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = समय(रिक्त);

#ifdef WITH_BRIDGE
	rc = bridge__register_local_connections();
	जर(rc){
		परत rc;
	}
#endif

	पर्यंत(g_run){
		retain__expiry_check();
		queue_plugin_msgs();
		context__free_disused();

		db.next_event_ms = 86400000;
#ifdef WITH_SYS_वृक्ष
		जर(db.config->sys_interval > 0){
			sys_वृक्ष_update(false);
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

		rc = mux__handle(listensock, listensock_मोजणी);
		जर(rc){
			परत rc;
		}

#ifdef WITH_PERSISTENCE
		जर(db.config->persistence && db.config->autosave_interval){
			जर(db.config->autosave_on_changes){
				जर(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}अन्यथा{
				जर(last_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					last_backup = db.now_s;
				}
			}
		}
#endif

		rc = signal__flag_check();
		जर(rc){
			परत rc;
		}

#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		साठी(पूर्णांक i=0; i<db.config->listener_मोजणी; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			जर(db.config->listeners[i].ws_context){
#जर LWS_LIBRARY_VERSION_संख्या > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#किंवाजर LWS_LIBRARY_VERSION_संख्या == 3002000
				lws_sul_schedule(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#अन्यथा
				lws_service(db.config->listeners[i].ws_context, 0);
#endif

			}
		}
#endif
	}

	परत MOSQ_ERR_SUCCESS;
}


शून्य do_disconnect(संरचना mosquitto *context, पूर्णांक reason)
{
	स्थिरांक अक्षर *id;
#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#endif

	जर(context->state == mosq_cs_disconnected){
		परत;
	}
#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	जर(context->wsi){
		जर(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		जर(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto__set_state(context, mosq_cs_disconnect_ws);
		}
		जर(context->wsi){
			lws_callback_on_writable(context->wsi);
		}
		जर(context->sock != INVALID_SOCKET){
			HASH_DELETE(hh_sock, db.contexts_by_sock, context);
			mux__delete(context);
			context->sock = INVALID_SOCKET;
		}
		जर(is_duplicate){
			/* This occurs if another client is taking over the same client id.
			 * It is important to remove this from the by_id hash here, so it
			 * doesn't leave us with multiple clients in the hash with the same
			 * id. Websockets doesn't actually close the connection here,
			 * unlike for normal clients, which means there is extra time when
			 * there could be two clients with the same id in the hash. */
			context__remove_from_by_id(context);
		}
	}अन्यथा
#endif
	{
		जर(db.config->connection_messages == true){
			जर(context->id){
				id = context->id;
			}अन्यथा{
				id = context->address;
			}
			जर(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				निवड(reason){
					स्थिती MOSQ_ERR_SUCCESS:
						थांबा;
					स्थिती MOSQ_ERR_MALFORMED_PACKET:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_PROTOCOL:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_CONN_LOST:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_AUTH:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_KEEPALIVE:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_PAYLOAD_SIZE:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_NOMEM:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_NOT_SUPPORTED:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_ERRNO:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->address, context->remote_port, म_त्रुटि(त्रुटि_सं));
						थांबा;
					स्थिती MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_SERVER_BUSY:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_BANNED:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_SESSION_TAKEN_OVER:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->address, context->remote_port);
						थांबा;
					स्थिती MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						थांबा;
					पूर्वनिर्धारित:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->address, context->remote_port, mosquitto_strerror(reason));
						थांबा;
				}
			}अन्यथा{
				जर(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
				}अन्यथा{
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->address, context->remote_port);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
