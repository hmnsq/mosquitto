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

#اگر_تعریف_نشده WIN32
#  define GNU_منبع
#پایان_اگر

#اگر_تعریف_نشده WIN32
#ضمن <unistd.h>
#وإلا
#ضمن <process.h>
#ضمن <winsock2.h>
#ضمن <ws2tcpip.h>
#پایان_اگر

#ضمن <errno.h>
#ضمن <signal.h>
#ضمن <stdio.h>
#ضمن <string.h>
#اگر_تعریف_نشده WIN32
#  include <sys/socket.h>
#پایان_اگر
#ضمن <time.h>
#ضمن <utlist.h>

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#پایان_اگر

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "packet_mosq.h"
#ضمن "property_common.h"
#ضمن "send_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"

خارجي صحيح g_پیاپی;

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_نسخه_عدد == 3002000


فراغ lws__sul_callback(بنية lws_sorted_usec_list *l)
{
}

ساكن بنية lws_sorted_usec_list sul;
#پایان_اگر


ساكن صحيح single_publish(بنية mosquitto *context, بنية mosquitto_پیام_v5 *pub_پیام, uint32_t پیام_expiry)
{
	بنية mosquitto_پایه_پیام *پایه_پیام;
	uint16_t mid;

	پایه_پیام = mosquitto_calloc(1, حجم(بنية mosquitto_پایه_پیام));
	إذا(پایه_پیام == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	پایه_پیام->داده.topic = pub_پیام->topic;
	pub_پیام->topic = عدم;
	پایه_پیام->داده.retain = 0;
	پایه_پیام->داده.payloadlen = (uint32_t)pub_پیام->payloadlen;
	پایه_پیام->داده.payload = mosquitto_malloc(پایه_پیام->داده.payloadlen+1);
	إذا(پایه_پیام->داده.payload == عدم){
		db_پیام_store_free(پایه_پیام);
		أرجع MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)پایه_پیام->داده.payload)[پایه_پیام->داده.payloadlen] = 0;
	کپی_حافظه(پایه_پیام->داده.payload, pub_پیام->payload, پایه_پیام->داده.payloadlen);

	إذا(pub_پیام->properties){
		پایه_پیام->داده.properties = pub_پیام->properties;
		pub_پیام->properties = عدم;
	}

	إذا(db_پیام_store(context, پایه_پیام, &پیام_expiry, mosq_mo_broker)){
		أرجع 1;
	}

	إذا(pub_پیام->qos){
		mid = mosquitto__mid_generate(context);
	}وإلا{
		mid = 0;
	}
	أرجع db_پیام_درج_outgoing(context, 0, mid, (uint8_t)pub_پیام->qos, 0, پایه_پیام, 0, true, true);
}


ساكن فراغ read_پیام_expiry_interval(mosquitto_property **proplist, uint32_t *پیام_expiry)
{
	mosquitto_property *p, *قبلی = عدم;

	*پیام_expiry = پیام_EXPIRY_INFINITE;

	إذا(!proplist){
		أرجع;
	}

	p = *proplist;
	طالما(p){
		إذا(mosquitto_property_identifier(p) == MQTT_PROP_پیام_EXPIRY_INTERVAL){
			*پیام_expiry = mosquitto_property_int32_مقدار(p);
			إذا(p == *proplist){
				*proplist = mosquitto_property_بعدی(p);
			}وإلا{
				قبلی->بعدی = mosquitto_property_بعدی(p);
			}
			mosquitto_property_free(&p);
			أرجع;

		}
		قبلی = p;
		p = mosquitto_property_بعدی(p);
	}
}


ساكن فراغ صف_plugin_msgs(فراغ)
{
	بنية mosquitto_پیام_v5 *پیام, *tmp;
	بنية mosquitto *context;
	uint32_t پیام_expiry;

	DL_FOREACH_امن(db.plugin_msgs, پیام, tmp){
		DL_DELETE(db.plugin_msgs, پیام);

		read_پیام_expiry_interval(&پیام->properties, &پیام_expiry);

		إذا(پیام->clientid){
			HASH_پیدا_کن(hh_id, db.contexts_by_id, پیام->clientid, طول_رشته(پیام->clientid), context);
			إذا(context){
				single_publish(context, پیام, پیام_expiry);
			}
		}وإلا{
			db_messages_easy_صف(عدم, پیام->topic, (uint8_t)پیام->qos, (uint32_t)پیام->payloadlen, پیام->payload, پیام->retain, پیام_expiry, &پیام->properties);
		}
		mosquitto_FREE(پیام->topic);
		mosquitto_FREE(پیام->payload);
		mosquitto_property_free_all(&پیام->properties);
		mosquitto_FREE(پیام->clientid);
		mosquitto_FREE(پیام);
	}
}


فراغ loop_update_بعدی_event(time_t new_ms)
{
	إذا(new_ms > 0 && new_ms < db.بعدی_event_ms){
		db.بعدی_event_ms = new_ms;
	}
}


صحيح mosquitto_main_loop(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمار)
{
#اگر_تعریف_شده WITH_PERSISTENCE
	time_t آخرین_backup = mosquitto_زمان();
#پایان_اگر
	صحيح rc;


	watchdog__init();
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_نسخه_عدد == 3002000
	memset(&sul, 0, حجم(بنية lws_sorted_usec_list));
#پایان_اگر

	db.now_s = mosquitto_زمان();
	db.now_real_s = زمان(عدم);

#اگر_تعریف_شده WITH_BRIDGE
	rc = bridge_ثبات_local_connections();
	إذا(rc){
		أرجع rc;
	}
#پایان_اگر

	طالما(g_پیاپی){
		retain__expiry_check();
		صف_plugin_msgs();
		context__free_disused();

		db.بعدی_event_ms = 86400000;
#اگر_تعریف_شده WITH_SYS_درخت
		إذا(db.config->sys_interval > 0){
			sys_درخت_update(false);
		}
#پایان_اگر

		keepalive__check();
		watchdog__check();

#اگر_تعریف_شده WITH_BRIDGE
		bridge_check();
#پایان_اگر
		plugin__handle_tick();
		session_expiry__check();
		will_delay__check();

		rc = mux__handle(listensock, listensock_شمار);
		إذا(rc){
			أرجع rc;
		}

#اگر_تعریف_شده WITH_PERSISTENCE
		إذا(db.config->persistence && db.config->autosave_interval){
			إذا(db.config->autosave_on_changes){
				إذا(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}وإلا{
				إذا(آخرین_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					آخرین_backup = db.now_s;
				}
			}
		}
#پایان_اگر

		rc = signal__flag_check();
		إذا(rc){
			أرجع rc;
		}

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		لكل(صحيح i=0; i<db.config->listener_شمار; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			إذا(db.config->listeners[i].ws_context){
#إذا LWS_LIBRARY_نسخه_عدد > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#وإلاإذا LWS_LIBRARY_نسخه_عدد == 3002000
				lws_sul_زمانبندی(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#وإلا
				lws_service(db.config->listeners[i].ws_context, 0);
#پایان_اگر

			}
		}
#پایان_اگر
	}

	أرجع MOSQ_ERR_SUCCESS;
}


فراغ do_disconnect(بنية mosquitto *context, صحيح reason)
{
	ثابت حرف *id;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#پایان_اگر

	إذا(context->state == mosq_cs_disconnected){
		أرجع;
	}
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	إذا(context->wsi){
		إذا(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		إذا(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto_مجموعه_state(context, mosq_cs_disconnect_ws);
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
#پایان_اگر
	{
		إذا(db.config->connection_messages == true){
			إذا(context->id){
				id = context->id;
			}وإلا{
				id = context->آدرس;
			}
			إذا(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				بدل(reason){
					حالة MOSQ_ERR_SUCCESS:
						اكسر;
					حالة MOSQ_ERR_MALFORMED_PACKET:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_PROTOCOL:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_CONN_گمشده:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_AUTH:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_KEEPALIVE:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_PAYLOAD_اندازه:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_NOMEM:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_NOT_SUPPORTED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_ERRNO:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->آدرس, context->remote_پورت, strerror(errno));
						اكسر;
					حالة MOSQ_ERR_RECEIVE_بیشینه_EXCEEDED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_SERVER_BUSY:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_BANNED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_BAD_AUTHENTICATION_روش:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_CONNECTION_نرخ_EXCEEDED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_SESSION_گرفته_OVER:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->آدرس, context->remote_پورت);
						اكسر;
					حالة MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						اكسر;
					افتراضي:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->آدرس, context->remote_پورت, mosquitto_strerror(reason));
						اكسر;
				}
			}وإلا{
				إذا(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->آدرس, context->remote_پورت);
				}وإلا{
					log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->آدرس, context->remote_پورت);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
