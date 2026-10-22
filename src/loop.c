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

#جے_بنا_تعریف WIN32
#  define GNU_سروت
#جے_انت

#جے_بنا_تعریف WIN32
#ضمن <unistd.h>
#وإلا
#ضمن <process.h>
#ضمن <winsock2.h>
#ضمن <ws2tcpip.h>
#جے_انت

#ضمن <errno.h>
#ضمن <signal.h>
#ضمن <stdio.h>
#ضمن <string.h>
#جے_بنا_تعریف WIN32
#  include <sys/socket.h>
#جے_انت
#ضمن <time.h>
#ضمن <utlist.h>

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#جے_انت

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "packet_mosq.h"
#ضمن "property_common.h"
#ضمن "send_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"

خارجي صحيح g_لگاتار;

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_نسخہ_نمبر == 3002000


فراغ lws__sul_callback(بنية lws_sorted_usec_list *l)
{
}

ساكن بنية lws_sorted_usec_list sul;
#جے_انت


ساكن صحيح single_publish(بنية mosquitto *context, بنية mosquitto_سنیہا_v5 *pub_پیغام, uint32_t سنیہا_expiry)
{
	بنية mosquitto_ادھار_پیغام *ادھار_پیغام;
	uint16_t mid;

	ادھار_پیغام = mosquitto_calloc(1, حجم(بنية mosquitto_ادھار_پیغام));
	إذا(ادھار_پیغام == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	ادھار_پیغام->انکڑے.topic = pub_پیغام->topic;
	pub_پیغام->topic = عدم;
	ادھار_پیغام->انکڑے.retain = 0;
	ادھار_پیغام->انکڑے.payloadlen = (uint32_t)pub_پیغام->payloadlen;
	ادھار_پیغام->انکڑے.payload = mosquitto_malloc(ادھار_پیغام->انکڑے.payloadlen+1);
	إذا(ادھار_پیغام->انکڑے.payload == عدم){
		db_پیغام_store_free(ادھار_پیغام);
		أرجع MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)ادھار_پیغام->انکڑے.payload)[ادھار_پیغام->انکڑے.payloadlen] = 0;
	حافظہ_نقل(ادھار_پیغام->انکڑے.payload, pub_پیغام->payload, ادھار_پیغام->انکڑے.payloadlen);

	إذا(pub_پیغام->properties){
		ادھار_پیغام->انکڑے.properties = pub_پیغام->properties;
		pub_پیغام->properties = عدم;
	}

	إذا(db_سنیہا_store(context, ادھار_پیغام, &سنیہا_expiry, mosq_mo_broker)){
		أرجع 1;
	}

	إذا(pub_پیغام->qos){
		mid = mosquitto__mid_generate(context);
	}وإلا{
		mid = 0;
	}
	أرجع db_سنیہا_داخل_کرو_outgoing(context, 0, mid, (uint8_t)pub_پیغام->qos, 0, ادھار_پیغام, 0, true, true);
}


ساكن فراغ read_سنیہا_expiry_interval(mosquitto_property **proplist, uint32_t *سنیہا_expiry)
{
	mosquitto_property *p, *پچھلا = عدم;

	*سنیہا_expiry = پیغام_EXPIRY_INFINITE;

	إذا(!proplist){
		أرجع;
	}

	p = *proplist;
	طالما(p){
		إذا(mosquitto_property_identifier(p) == MQTT_PROP_سنیہا_EXPIRY_INTERVAL){
			*سنیہا_expiry = mosquitto_property_int32_قدر(p);
			إذا(p == *proplist){
				*proplist = mosquitto_property_اگلا(p);
			}وإلا{
				پچھلا->اگلا = mosquitto_property_اگلا(p);
			}
			mosquitto_property_free(&p);
			أرجع;

		}
		پچھلا = p;
		p = mosquitto_property_اگلا(p);
	}
}


ساكن فراغ قطار_plugin_msgs(فراغ)
{
	بنية mosquitto_سنیہا_v5 *پیغام, *tmp;
	بنية mosquitto *context;
	uint32_t سنیہا_expiry;

	DL_FOREACH_محفوظ(db.plugin_msgs, پیغام, tmp){
		DL_DELETE(db.plugin_msgs, پیغام);

		read_سنیہا_expiry_interval(&پیغام->properties, &سنیہا_expiry);

		إذا(پیغام->clientid){
			HASH_لبھو(hh_id, db.contexts_by_id, پیغام->clientid, لڑی_لمبائی(پیغام->clientid), context);
			إذا(context){
				single_publish(context, پیغام, سنیہا_expiry);
			}
		}وإلا{
			db_messages_easy_قطار(عدم, پیغام->topic, (uint8_t)پیغام->qos, (uint32_t)پیغام->payloadlen, پیغام->payload, پیغام->retain, سنیہا_expiry, &پیغام->properties);
		}
		mosquitto_FREE(پیغام->topic);
		mosquitto_FREE(پیغام->payload);
		mosquitto_property_free_all(&پیغام->properties);
		mosquitto_FREE(پیغام->clientid);
		mosquitto_FREE(پیغام);
	}
}


فراغ loop_update_اگلا_event(time_t new_ms)
{
	إذا(new_ms > 0 && new_ms < db.اگلا_event_ms){
		db.اگلا_event_ms = new_ms;
	}
}


صحيح mosquitto_main_loop(بنية mosquitto__listener_sock *listensock, صحيح listensock_گنتی)
{
#جے_تعریف_شدہ WITH_PERSISTENCE
	time_t آخری_backup = mosquitto_ویلا();
#جے_انت
	صحيح rc;


	watchdog__init();
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_نسخہ_نمبر == 3002000
	memset(&sul, 0, حجم(بنية lws_sorted_usec_list));
#جے_انت

	db.now_s = mosquitto_ویلا();
	db.now_real_s = ویلا(عدم);

#جے_تعریف_شدہ WITH_BRIDGE
	rc = bridge_رجسٹر_local_connections();
	إذا(rc){
		أرجع rc;
	}
#جے_انت

	طالما(g_لگاتار){
		retain__expiry_check();
		قطار_plugin_msgs();
		context__free_disused();

		db.اگلا_event_ms = 86400000;
#جے_تعریف_شدہ WITH_SYS_درخت
		إذا(db.config->sys_interval > 0){
			sys_درخت_update(false);
		}
#جے_انت

		keepalive__check();
		watchdog__check();

#جے_تعریف_شدہ WITH_BRIDGE
		bridge_check();
#جے_انت
		plugin__handle_tick();
		session_expiry__check();
		will_delay__check();

		rc = mux__handle(listensock, listensock_گنتی);
		إذا(rc){
			أرجع rc;
		}

#جے_تعریف_شدہ WITH_PERSISTENCE
		إذا(db.config->persistence && db.config->autosave_interval){
			إذا(db.config->autosave_on_changes){
				إذا(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}وإلا{
				إذا(آخری_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					آخری_backup = db.now_s;
				}
			}
		}
#جے_انت

		rc = signal__flag_check();
		إذا(rc){
			أرجع rc;
		}

#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		لكل(صحيح i=0; i<db.config->listener_گنتی; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			إذا(db.config->listeners[i].ws_context){
#إذا LWS_LIBRARY_نسخہ_نمبر > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#وإلاإذا LWS_LIBRARY_نسخہ_نمبر == 3002000
				lws_sul_ویلا_سارنی(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#وإلا
				lws_service(db.config->listeners[i].ws_context, 0);
#جے_انت

			}
		}
#جے_انت
	}

	أرجع MOSQ_ERR_SUCCESS;
}


فراغ do_disconnect(بنية mosquitto *context, صحيح reason)
{
	ثابت حرف *id;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#جے_انت

	إذا(context->state == mosq_cs_disconnected){
		أرجع;
	}
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	إذا(context->wsi){
		إذا(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		إذا(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto_سیٹ_state(context, mosq_cs_disconnect_ws);
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
#جے_انت
	{
		إذا(db.config->connection_messages == true){
			إذا(context->id){
				id = context->id;
			}وإلا{
				id = context->پتہ;
			}
			إذا(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				بدل(reason){
					حالة MOSQ_ERR_SUCCESS:
						اكسر;
					حالة MOSQ_ERR_MALFORMED_PACKET:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_PROTOCOL:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_CONN_گواچا:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_AUTH:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_KEEPALIVE:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_PAYLOAD_ناپ:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_NOMEM:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_NOT_SUPPORTED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_ERRNO:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->پتہ, context->remote_پورٹ, strerror(errno));
						اكسر;
					حالة MOSQ_ERR_RECEIVE_ودھ_توں_ودھ_EXCEEDED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_SERVER_BUSY:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_BANNED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_BAD_AUTHENTICATION_طریقہ:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_CONNECTION_شرح_EXCEEDED:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_SESSION_لیا_OVER:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->پتہ, context->remote_پورٹ);
						اكسر;
					حالة MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						اكسر;
					افتراضي:
						log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->پتہ, context->remote_پورٹ, mosquitto_strerror(reason));
						اكسر;
				}
			}وإلا{
				إذا(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->پتہ, context->remote_پورٹ);
				}وإلا{
					log__printf(عدم, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->پتہ, context->remote_پورٹ);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
