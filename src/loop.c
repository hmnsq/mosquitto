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

#ضمن "config.h"

#جيڪڏهن_وصف_نه_ٿيل WIN32
#  define GNU_ذريعو
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل WIN32
#ضمن <unistd.h>
#وإلا
#ضمن <process.h>
#ضمن <winsock2.h>
#ضمن <ws2tcpip.h>
#جيڪڏهن_پورو

#ضمن <errno.h>
#ضمن <signal.h>
#ضمن <stdio.h>
#ضمن <string.h>
#جيڪڏهن_وصف_نه_ٿيل WIN32
#  include <sys/socket.h>
#جيڪڏهن_پورو
#ضمن <time.h>
#ضمن <utlist.h>

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#جيڪڏهن_پورو

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "packet_mosq.h"
#ضمن "property_common.h"
#ضمن "send_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"

خارجي صحيح g_لاڳيتو;

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_نسخو_عدد == 3002000


فراغ lws__sul_callback(بنية lws_sorted_usec_list *l)
{
}

ساكن بنية lws_sorted_usec_list sul;
#جيڪڏهن_پورو


ساكن صحيح single_publish(بنية mosquitto *context, بنية mosquitto_پيغام_v5 *pub_نياپو, uint32_t پيغام_expiry)
{
	بنية mosquitto_بنياد_نياپو *بنياد_نياپو;
	uint16_t mid;

	بنياد_نياپو = mosquitto_calloc(1, حجم(بنية mosquitto_بنياد_نياپو));
	إذا(بنياد_نياپو == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	بنياد_نياپو->معلومات.topic = pub_نياپو->topic;
	pub_نياپو->topic = عدم;
	بنياد_نياپو->معلومات.retain = 0;
	بنياد_نياپو->معلومات.payloadlen = (uint32_t)pub_نياپو->payloadlen;
	بنياد_نياپو->معلومات.payload = mosquitto_malloc(بنياد_نياپو->معلومات.payloadlen+1);
	إذا(بنياد_نياپو->معلومات.payload == عدم){
		db_نياپو_store_free(بنياد_نياپو);
		أرجع MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)بنياد_نياپو->معلومات.payload)[بنياد_نياپو->معلومات.payloadlen] = 0;
	يادگيري_نقل(بنياد_نياپو->معلومات.payload, pub_نياپو->payload, بنياد_نياپو->معلومات.payloadlen);

	إذا(pub_نياپو->properties){
		بنياد_نياپو->معلومات.properties = pub_نياپو->properties;
		pub_نياپو->properties = عدم;
	}

	إذا(db_پيغام_store(context, بنياد_نياپو, &پيغام_expiry, mosq_mo_broker)){
		أرجع 1;
	}

	إذا(pub_نياپو->qos){
		mid = mosquitto__mid_generate(context);
	}وإلا{
		mid = 0;
	}
	أرجع db_پيغام_وجهو_outgoing(context, 0, mid, (uint8_t)pub_نياپو->qos, 0, بنياد_نياپو, 0, true, true);
}


ساكن فراغ read_پيغام_expiry_interval(mosquitto_property **proplist, uint32_t *پيغام_expiry)
{
	mosquitto_property *p, *اڳيون = عدم;

	*پيغام_expiry = نياپو_EXPIRY_INFINITE;

	إذا(!proplist){
		أرجع;
	}

	p = *proplist;
	طالما(p){
		إذا(mosquitto_property_identifier(p) == MQTT_PROP_پيغام_EXPIRY_INTERVAL){
			*پيغام_expiry = mosquitto_property_int32_قدر(p);
			إذا(p == *proplist){
				*proplist = mosquitto_property_ايندڙ(p);
			}وإلا{
				اڳيون->ايندڙ = mosquitto_property_ايندڙ(p);
			}
			mosquitto_property_free(&p);
			أرجع;

		}
		اڳيون = p;
		p = mosquitto_property_ايندڙ(p);
	}
}


ساكن فراغ قطار_plugin_msgs(فراغ)
{
	بنية mosquitto_پيغام_v5 *نياپو, *tmp;
	بنية mosquitto *context;
	uint32_t پيغام_expiry;

	DL_FOREACH_محفوظ(db.plugin_msgs, نياپو, tmp){
		DL_DELETE(db.plugin_msgs, نياپو);

		read_پيغام_expiry_interval(&نياپو->properties, &پيغام_expiry);

		إذا(نياپو->clientid){
			HASH_ڳوليو(hh_id, db.contexts_by_id, نياپو->clientid, لڙي_ڊيگهه(نياپو->clientid), context);
			إذا(context){
				single_publish(context, نياپو, پيغام_expiry);
			}
		}وإلا{
			db_messages_easy_قطار(عدم, نياپو->topic, (uint8_t)نياپو->qos, (uint32_t)نياپو->payloadlen, نياپو->payload, نياپو->retain, پيغام_expiry, &نياپو->properties);
		}
		mosquitto_FREE(نياپو->topic);
		mosquitto_FREE(نياپو->payload);
		mosquitto_property_free_all(&نياپو->properties);
		mosquitto_FREE(نياپو->clientid);
		mosquitto_FREE(نياپو);
	}
}


فراغ loop_update_ايندڙ_event(time_t new_ms)
{
	إذا(new_ms > 0 && new_ms < db.ايندڙ_event_ms){
		db.ايندڙ_event_ms = new_ms;
	}
}


صحيح mosquitto_main_loop(بنية mosquitto__listener_sock *listensock, صحيح listensock_ڳڻپ)
{
#جيڪڏهن_وصف_ٿيل WITH_PERSISTENCE
	time_t آخري_backup = mosquitto_وقت();
#جيڪڏهن_پورو
	صحيح rc;


	watchdog__init();
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_نسخو_عدد == 3002000
	memset(&sul, 0, حجم(بنية lws_sorted_usec_list));
#جيڪڏهن_پورو

	db.now_s = mosquitto_وقت();
	db.now_real_s = وقت(عدم);

#جيڪڏهن_وصف_ٿيل WITH_BRIDGE
	rc = bridge_رجسٽر_local_connections();
	إذا(rc){
		أرجع rc;
	}
#جيڪڏهن_پورو

	طالما(g_لاڳيتو){
		retain__expiry_check();
		قطار_plugin_msgs();
		context__free_disused();

		db.ايندڙ_event_ms = 86400000;
#جيڪڏهن_وصف_ٿيل WITH_SYS_وڻ
		إذا(db.config->sys_interval > 0){
			sys_وڻ_update(false);
		}
#جيڪڏهن_پورو

		keepalive__check();
		watchdog__check();

#جيڪڏهن_وصف_ٿيل WITH_BRIDGE
		bridge_check();
#جيڪڏهن_پورو
		plugin__handle_tick();
		session_expiry__check();
		will_delay__check();

		rc = mux__handle(listensock, listensock_ڳڻپ);
		إذا(rc){
			أرجع rc;
		}

#جيڪڏهن_وصف_ٿيل WITH_PERSISTENCE
		إذا(db.config->persistence && db.config->autosave_interval){
			إذا(db.config->autosave_on_changes){
				إذا(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}وإلا{
				إذا(آخري_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					آخري_backup = db.now_s;
				}
			}
		}
#جيڪڏهن_پورو

		rc = signal__flag_check();
		إذا(rc){
			أرجع rc;
		}

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		لكل(صحيح i=0; i<db.config->listener_ڳڻپ; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			إذا(db.config->listeners[i].ws_context){
#إذا LWS_LIBRARY_نسخو_عدد > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#وإلاإذا LWS_LIBRARY_نسخو_عدد == 3002000
				lws_sul_وقت_جدول(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#وإلا
				lws_service(db.config->listeners[i].ws_context, 0);
#جيڪڏهن_پورو

			}
		}
#جيڪڏهن_پورو
	}

	أرجع MOSQ_ERR_SUCCESS;
}


فراغ do_disconnect(بنية mosquitto *context, صحيح reason)
{
	ثابت حرف *id;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#جيڪڏهن_پورو

	إذا(context->state == mosq_cs_disconnected){
		أرجع;
	}
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	إذا(context->wsi){
		إذا(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		إذا(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto_سيٽ_state(context, mosq_cs_disconnect_ws);
		}
		إذا(context->wsi){
			lws_callback_on_writable(context->wsi);
		}
		إذا(context->sock != INVALID_SOCKET){
			HASH_DELETE(hh_sock, db.contexts_by_sock, context);
			mux__delete(context);
			context->sock = INVALID_SOCKET;
		}
		إذا(is_duplicate){
			/* This occurs if another client is taking over the same client id.
			 * It is important to remove this from the by_id hash here, so it
			 * doesn't leave us with multiple clients in the hash with the same
			 * id. Websockets doesn't actually close the connection here,
			 * unlike for normal clients, which means there is extra time when
			 * there could be two clients with the same id in the hash. */
			context__remove_from_by_id(context);
		}
	}وإلا
#جيڪڏهن_پورو
	{
		إذا(db.config->connection_messages == true){
			إذا(context->id){
				id = context->id;
			}وإلا{
				id = context->پتو;
			}
			إذا(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				بدل(reason){
					حالة MOSQ_ERR_SUCCESS:
						اكسر;
					حالة MOSQ_ERR_MALFORMED_PACKET:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_PROTOCOL:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_CONN_گم:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_AUTH:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_KEEPALIVE:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_PAYLOAD_سائيز:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_NOMEM:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_NOT_SUPPORTED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_ERRNO:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->پتو, context->remote_پورٽ, strerror(errno));
						اكسر;
					حالة MOSQ_ERR_RECEIVE_وڌ_کان_وڌ_EXCEEDED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_SERVER_BUSY:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_BANNED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_BAD_AUTHENTICATION_طريقو:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_CONNECTION_شرح_EXCEEDED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_SESSION_ورتل_OVER:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->پتو, context->remote_پورٽ);
						اكسر;
					حالة MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						اكسر;
					افتراضي:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->پتو, context->remote_پورٽ, mosquitto_strerror(reason));
						اكسر;
				}
			}وإلا{
				إذا(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->پتو, context->remote_پورٽ);
				}وإلا{
					log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->پتو, context->remote_پورٽ);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
