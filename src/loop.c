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

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#endif

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "property_common.h"
#अन्तर्भाव "send_mosq.h"
#अन्तर्भाव "sys_tree.h"
#अन्तर्भाव "util_mosq.h"

बाह्य पूर्णाङ्क g_run;

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_सङ्ख्या == 3002000


शून्य lws__sul_callback(संरचना lws_sorted_usec_list *l)
{
}

स्थिर संरचना lws_sorted_usec_list sul;
#endif


स्थिर पूर्णाङ्क single_publish(संरचना mosquitto *context, संरचना mosquitto__message_v5 *pub_सन्देश, uint32_t message_expiry)
{
	संरचना mosquitto_base_सन्देश *base_सन्देश;
	uint16_t mid;

	base_सन्देश = mosquitto_calloc(1, परिमाण(संरचना mosquitto_base_सन्देश));
	यदि(base_सन्देश == रिक्त){
		फर्काऊ MOSQ_ERR_NOMEM;
	}

	base_सन्देश->data.topic = pub_सन्देश->topic;
	pub_सन्देश->topic = रिक्त;
	base_सन्देश->data.retain = 0;
	base_सन्देश->data.payloadlen = (uint32_t)pub_सन्देश->payloadlen;
	base_सन्देश->data.payload = mosquitto_malloc(base_सन्देश->data.payloadlen+1);
	यदि(base_सन्देश->data.payload == रिक्त){
		db_सन्देश_store_free(base_सन्देश);
		फर्काऊ MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)base_सन्देश->data.payload)[base_सन्देश->data.payloadlen] = 0;
	स_नकल(base_सन्देश->data.payload, pub_सन्देश->payload, base_सन्देश->data.payloadlen);

	यदि(pub_सन्देश->properties){
		base_सन्देश->data.properties = pub_सन्देश->properties;
		pub_सन्देश->properties = रिक्त;
	}

	यदि(db__message_store(context, base_सन्देश, &message_expiry, mosq_mo_broker)){
		फर्काऊ 1;
	}

	यदि(pub_सन्देश->qos){
		mid = mosquitto__mid_generate(context);
	}अन्यथा{
		mid = 0;
	}
	फर्काऊ db_message_घुसाउ_outgoing(context, 0, mid, (uint8_t)pub_सन्देश->qos, 0, base_सन्देश, 0, true, true);
}


स्थिर शून्य read_message_expiry_interval(mosquitto_property **proplist, uint32_t *message_expiry)
{
	mosquitto_property *p, *previous = रिक्त;

	*message_expiry = सन्देश_EXPIRY_INFINITE;

	यदि(!proplist){
		फर्काऊ;
	}

	p = *proplist;
	जबसम्म(p){
		यदि(mosquitto_property_identifier(p) == MQTT_PROP_MESSAGE_EXPIRY_INTERVAL){
			*message_expiry = mosquitto_property_int32_मान(p);
			यदि(p == *proplist){
				*proplist = mosquitto_property_next(p);
			}अन्यथा{
				previous->next = mosquitto_property_next(p);
			}
			mosquitto_property_free(&p);
			फर्काऊ;

		}
		previous = p;
		p = mosquitto_property_next(p);
	}
}


स्थिर शून्य queue_plugin_msgs(शून्य)
{
	संरचना mosquitto__message_v5 *सन्देश, *tmp;
	संरचना mosquitto *context;
	uint32_t message_expiry;

	DL_FOREACH_SAFE(db.plugin_msgs, सन्देश, tmp){
		DL_DELETE(db.plugin_msgs, सन्देश);

		read_message_expiry_interval(&सन्देश->properties, &message_expiry);

		यदि(सन्देश->clientid){
			HASH_FIND(hh_id, db.contexts_by_id, सन्देश->clientid, म_माप(सन्देश->clientid), context);
			यदि(context){
				single_publish(context, सन्देश, message_expiry);
			}
		}अन्यथा{
			db__messages_easy_queue(रिक्त, सन्देश->topic, (uint8_t)सन्देश->qos, (uint32_t)सन्देश->payloadlen, सन्देश->payload, सन्देश->retain, message_expiry, &सन्देश->properties);
		}
		mosquitto_FREE(सन्देश->topic);
		mosquitto_FREE(सन्देश->payload);
		mosquitto_property_free_all(&सन्देश->properties);
		mosquitto_FREE(सन्देश->clientid);
		mosquitto_FREE(सन्देश);
	}
}


शून्य loop__update_next_event(समय_प्रकार new_ms)
{
	यदि(new_ms > 0 && new_ms < db.next_event_ms){
		db.next_event_ms = new_ms;
	}
}


पूर्णाङ्क mosquitto_main_loop(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गन्ती)
{
#ifdef WITH_PERSISTENCE
	समय_प्रकार last_backup = mosquitto_time();
#endif
	पूर्णाङ्क rc;


	watchdog__init();
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_VERSION_सङ्ख्या == 3002000
	स_रखो(&sul, 0, परिमाण(संरचना lws_sorted_usec_list));
#endif

	db.now_s = mosquitto_time();
	db.now_real_s = समय(रिक्त);

#ifdef WITH_BRIDGE
	rc = bridge__register_local_connections();
	यदि(rc){
		फर्काऊ rc;
	}
#endif

	जबसम्म(g_run){
		retain__expiry_check();
		queue_plugin_msgs();
		context__free_disused();

		db.next_event_ms = 86400000;
#ifdef WITH_SYS_रूख
		यदि(db.config->sys_interval > 0){
			sys_रूख_update(false);
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

		rc = mux__handle(listensock, listensock_गन्ती);
		यदि(rc){
			फर्काऊ rc;
		}

#ifdef WITH_PERSISTENCE
		यदि(db.config->persistence && db.config->autosave_interval){
			यदि(db.config->autosave_on_changes){
				यदि(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}अन्यथा{
				यदि(last_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					last_backup = db.now_s;
				}
			}
		}
#endif

		rc = signal__flag_check();
		यदि(rc){
			फर्काऊ rc;
		}

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		लागि(पूर्णाङ्क i=0; i<db.config->listener_गन्ती; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			यदि(db.config->listeners[i].ws_context){
#यदि LWS_LIBRARY_VERSION_सङ्ख्या > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#अथवायदि LWS_LIBRARY_VERSION_सङ्ख्या == 3002000
				lws_sul_schedule(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#अन्यथा
				lws_service(db.config->listeners[i].ws_context, 0);
#endif

			}
		}
#endif
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


शून्य do_disconnect(संरचना mosquitto *context, पूर्णाङ्क reason)
{
	अचर वर्ण *id;
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#endif

	यदि(context->state == mosq_cs_disconnected){
		फर्काऊ;
	}
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	यदि(context->wsi){
		यदि(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		यदि(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto__set_state(context, mosq_cs_disconnect_ws);
		}
		यदि(context->wsi){
			lws_callback_on_writable(context->wsi);
		}
		यदि(context->sock != INVALID_SOCKET){
			HASH_DELETE(hh_sock, db.contexts_by_sock, context);
			mux__delete(context);
			context->sock = INVALID_SOCKET;
		}
		यदि(is_duplicate){
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
		यदि(db.config->connection_messages == true){
			यदि(context->id){
				id = context->id;
			}अन्यथा{
				id = context->address;
			}
			यदि(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				विकल्प(reason){
					पक्ष MOSQ_ERR_SUCCESS:
						रोक;
					पक्ष MOSQ_ERR_MALFORMED_PACKET:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_PROTOCOL:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_CONN_LOST:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_AUTH:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_KEEPALIVE:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_PAYLOAD_SIZE:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_NOMEM:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_NOT_SUPPORTED:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_ERRNO:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->address, context->remote_port, म_त्रुटि(त्रुटि_सं));
						रोक;
					पक्ष MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_SERVER_BUSY:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_BANNED:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_BAD_AUTHENTICATION_METHOD:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_CONNECTION_RATE_EXCEEDED:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_SESSION_TAKEN_OVER:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->address, context->remote_port);
						रोक;
					पक्ष MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						रोक;
					स्वभाव:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->address, context->remote_port, mosquitto_strerror(reason));
						रोक;
				}
			}अन्यथा{
				यदि(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
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
