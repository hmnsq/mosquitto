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

#यदि_अपरिभाषितम् WIN32
#  define _GNU_SOURCE
#यदि_अन्तः

#यदि_अपरिभाषितम् WIN32
#अन्तर्भाव <unistd.h>
#अन्यथा
#अन्तर्भाव <process.h>
#अन्तर्भाव <winsock2.h>
#अन्तर्भाव <ws2tcpip.h>
#यदि_अन्तः

#अन्तर्भाव <errno.h>
#अन्तर्भाव <signal.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#यदि_अपरिभाषितम् WIN32
#  include <sys/socket.h>
#यदि_अन्तः
#अन्तर्भाव <time.h>
#अन्तर्भाव <utlist.h>

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#यदि_अन्तः

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "property_common.h"
#अन्तर्भाव "send_mosq.h"
#अन्तर्भाव "sys_tree.h"
#अन्तर्भाव "util_mosq.h"

बाह्य पूर्णाङ्क g_सततम्;

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_संस्करणम्_सङ्ख्या == 3002000


शून्य lws__sul_callback(संरचना lws_sorted_usec_list *l)
{
}

स्थिर संरचना lws_sorted_usec_list sul;
#यदि_अन्तः


स्थिर पूर्णाङ्क single_publish(संरचना mosquitto *context, संरचना mosquitto_सन्देशः_v5 *pub_सन्देशः, uint32_t सन्देशः_expiry)
{
	संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः;
	uint16_t mid;

	आधारः_सन्देशः = mosquitto_calloc(1, परिमाण(संरचना mosquitto_आधारः_सन्देशः));
	यदि(आधारः_सन्देशः == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	आधारः_सन्देशः->दत्त.topic = pub_सन्देशः->topic;
	pub_सन्देशः->topic = रिक्त;
	आधारः_सन्देशः->दत्त.retain = 0;
	आधारः_सन्देशः->दत्त.payloadlen = (uint32_t)pub_सन्देशः->payloadlen;
	आधारः_सन्देशः->दत्त.payload = mosquitto_malloc(आधारः_सन्देशः->दत्त.payloadlen+1);
	यदि(आधारः_सन्देशः->दत्त.payload == रिक्त){
		db_सन्देशः_store_free(आधारः_सन्देशः);
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)आधारः_सन्देशः->दत्त.payload)[आधारः_सन्देशः->दत्त.payloadlen] = 0;
	स्मृतिं_प्रतिलिख(आधारः_सन्देशः->दत्त.payload, pub_सन्देशः->payload, आधारः_सन्देशः->दत्त.payloadlen);

	यदि(pub_सन्देशः->properties){
		आधारः_सन्देशः->दत्त.properties = pub_सन्देशः->properties;
		pub_सन्देशः->properties = रिक्त;
	}

	यदि(db_सन्देशः_store(context, आधारः_सन्देशः, &सन्देशः_expiry, mosq_mo_broker)){
		प्रत्यागम 1;
	}

	यदि(pub_सन्देशः->qos){
		mid = mosquitto__mid_generate(context);
	}अन्यथा{
		mid = 0;
	}
	प्रत्यागम db_सन्देशः_निवेशय_outgoing(context, 0, mid, (uint8_t)pub_सन्देशः->qos, 0, आधारः_सन्देशः, 0, true, true);
}


स्थिर शून्य read_सन्देशः_expiry_interval(mosquitto_property **proplist, uint32_t *सन्देशः_expiry)
{
	mosquitto_property *p, *previous = रिक्त;

	*सन्देशः_expiry = सन्देशः_EXPIRY_INFINITE;

	यदि(!proplist){
		प्रत्यागम;
	}

	p = *proplist;
	यावत्(p){
		यदि(mosquitto_property_identifier(p) == MQTT_PROP_सन्देशः_EXPIRY_INTERVAL){
			*सन्देशः_expiry = mosquitto_property_int32_मानम्(p);
			यदि(p == *proplist){
				*proplist = mosquitto_property_अग्र(p);
			}अन्यथा{
				previous->अग्र = mosquitto_property_अग्र(p);
			}
			mosquitto_property_free(&p);
			प्रत्यागम;

		}
		previous = p;
		p = mosquitto_property_अग्र(p);
	}
}


स्थिर शून्य queue_plugin_msgs(शून्य)
{
	संरचना mosquitto_सन्देशः_v5 *सन्देशः, *tmp;
	संरचना mosquitto *context;
	uint32_t सन्देशः_expiry;

	DL_FOREACH_SAFE(db.plugin_msgs, सन्देशः, tmp){
		DL_DELETE(db.plugin_msgs, सन्देशः);

		read_सन्देशः_expiry_interval(&सन्देशः->properties, &सन्देशः_expiry);

		यदि(सन्देशः->clientid){
			HASH_FIND(hh_id, db.contexts_by_id, सन्देशः->clientid, वाक्यदैर्घ्यम्(सन्देशः->clientid), context);
			यदि(context){
				single_publish(context, सन्देशः, सन्देशः_expiry);
			}
		}अन्यथा{
			db__messages_easy_queue(रिक्त, सन्देशः->topic, (uint8_t)सन्देशः->qos, (uint32_t)सन्देशः->payloadlen, सन्देशः->payload, सन्देशः->retain, सन्देशः_expiry, &सन्देशः->properties);
		}
		mosquitto_FREE(सन्देशः->topic);
		mosquitto_FREE(सन्देशः->payload);
		mosquitto_property_free_all(&सन्देशः->properties);
		mosquitto_FREE(सन्देशः->clientid);
		mosquitto_FREE(सन्देशः);
	}
}


शून्य loop_update_अग्र_event(time_t new_ms)
{
	यदि(new_ms > 0 && new_ms < db.अग्र_event_ms){
		db.अग्र_event_ms = new_ms;
	}
}


पूर्णाङ्क mosquitto_main_loop(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्)
{
#यदि_परिभाषितम् WITH_PERSISTENCE
	time_t चरमः_backup = mosquitto_time();
#यदि_अन्तः
	पूर्णाङ्क rc;


	watchdog__init();
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_संस्करणम्_सङ्ख्या == 3002000
	स्मृतिं_पूरय(&sul, 0, परिमाण(संरचना lws_sorted_usec_list));
#यदि_अन्तः

	db.now_s = mosquitto_time();
	db.now_real_s = time(रिक्त);

#यदि_परिभाषितम् WITH_BRIDGE
	rc = bridge_पञ्जिकाकोशः_local_connections();
	यदि(rc){
		प्रत्यागम rc;
	}
#यदि_अन्तः

	यावत्(g_सततम्){
		retain__expiry_check();
		queue_plugin_msgs();
		context__free_disused();

		db.अग्र_event_ms = 86400000;
#यदि_परिभाषितम् WITH_SYS_वृक्षः
		यदि(db.config->sys_interval > 0){
			sys_वृक्षः_update(false);
		}
#यदि_अन्तः

		keepalive__check();
		watchdog__check();

#यदि_परिभाषितम् WITH_BRIDGE
		bridge_check();
#यदि_अन्तः
		plugin__handle_tick();
		session_expiry__check();
		will_delay__check();

		rc = mux__handle(listensock, listensock_गणनम्);
		यदि(rc){
			प्रत्यागम rc;
		}

#यदि_परिभाषितम् WITH_PERSISTENCE
		यदि(db.config->persistence && db.config->autosave_interval){
			यदि(db.config->autosave_on_changes){
				यदि(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}अन्यथा{
				यदि(चरमः_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					चरमः_backup = db.now_s;
				}
			}
		}
#यदि_अन्तः

		rc = signal__flag_check();
		यदि(rc){
			प्रत्यागम rc;
		}

#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		प्रति(पूर्णाङ्क i=0; i<db.config->listener_गणनम्; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			यदि(db.config->listeners[i].ws_context){
#यदि LWS_LIBRARY_संस्करणम्_सङ्ख्या > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#नोचेद्यदि LWS_LIBRARY_संस्करणम्_सङ्ख्या == 3002000
				lws_sul_कालसारणी(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#अन्यथा
				lws_service(db.config->listeners[i].ws_context, 0);
#यदि_अन्तः

			}
		}
#यदि_अन्तः
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


शून्य do_disconnect(संरचना mosquitto *context, पूर्णाङ्क reason)
{
	ध्रुव वर्ण *id;
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#यदि_अन्तः

	यदि(context->state == mosq_cs_disconnected){
		प्रत्यागम;
	}
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	यदि(context->wsi){
		यदि(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		यदि(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto_समुच्चयः_state(context, mosq_cs_disconnect_ws);
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
#यदि_अन्तः
	{
		यदि(db.config->connection_messages == true){
			यदि(context->id){
				id = context->id;
			}अन्यथा{
				id = context->स्थानाङ्कः;
			}
			यदि(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				विकल्प(reason){
					पक्ष MOSQ_ERR_SUCCESS:
						भङ्ग;
					पक्ष MOSQ_ERR_MALFORMED_PACKET:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_PROTOCOL:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_CONN_नष्टम्:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_AUTH:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_KEEPALIVE:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_PAYLOAD_आकार:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_NOMEM:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_NOT_SUPPORTED:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_ERRNO:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->स्थानाङ्कः, context->remote_पत्तनम्, strerror(errno));
						भङ्ग;
					पक्ष MOSQ_ERR_RECEIVE_अधिकतमम्_EXCEEDED:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_SERVER_BUSY:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_BANNED:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_BAD_AUTHENTICATION_विधानम्:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_CONNECTION_दरः_EXCEEDED:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_SESSION_गृहीतम्_OVER:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
						भङ्ग;
					पक्ष MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						भङ्ग;
					स्वभाव:
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->स्थानाङ्कः, context->remote_पत्तनम्, mosquitto_strerror(reason));
						भङ्ग;
				}
			}अन्यथा{
				यदि(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
				}अन्यथा{
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->स्थानाङ्कः, context->remote_पत्तनम्);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
