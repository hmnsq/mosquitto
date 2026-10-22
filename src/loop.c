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

#شامل "config.h"

#اگر_غیر_تعریف_شدہ WIN32
#  define GNU_ماخذ
#اگر_ختم

#اگر_غیر_تعریف_شدہ WIN32
#شامل <unistd.h>
#ورنہ
#شامل <process.h>
#شامل <winsock2.h>
#شامل <ws2tcpip.h>
#اگر_ختم

#شامل <errno.h>
#شامل <signal.h>
#شامل <stdio.h>
#شامل <string.h>
#اگر_غیر_تعریف_شدہ WIN32
#  include <sys/socket.h>
#اگر_ختم
#شامل <time.h>
#شامل <utlist.h>

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
#  include <libwebsockets.h>
#اگر_ختم

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "packet_mosq.h"
#شامل "property_common.h"
#شامل "send_mosq.h"
#شامل "sys_tree.h"
#شامل "util_mosq.h"

خارجی صحیح g_تسلسل;

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_نسخہ_نمبر == 3002000


خالی lws__sul_callback(ساخت lws_sorted_usec_list *l)
{
}

ساکن ساخت lws_sorted_usec_list sul;
#اگر_ختم


ساکن صحیح single_publish(ساخت mosquitto *context, ساخت mosquitto_پیغام_v5 *pub_پیغام, uint32_t پیغام_expiry)
{
	ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام;
	uint16_t mid;

	بنیاد_پیغام = mosquitto_calloc(1, حجم(ساخت mosquitto_بنیاد_پیغام));
	اگر(بنیاد_پیغام == تہی){
		واپس MOSQ_ERR_NOMEM;
	}

	بنیاد_پیغام->ڈیٹا.topic = pub_پیغام->topic;
	pub_پیغام->topic = تہی;
	بنیاد_پیغام->ڈیٹا.retain = 0;
	بنیاد_پیغام->ڈیٹا.payloadlen = (uint32_t)pub_پیغام->payloadlen;
	بنیاد_پیغام->ڈیٹا.payload = mosquitto_malloc(بنیاد_پیغام->ڈیٹا.payloadlen+1);
	اگر(بنیاد_پیغام->ڈیٹا.payload == تہی){
		db_پیغام_store_free(بنیاد_پیغام);
		واپس MOSQ_ERR_NOMEM;
	}
	/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
	((uint8_t *)بنیاد_پیغام->ڈیٹا.payload)[بنیاد_پیغام->ڈیٹا.payloadlen] = 0;
	حافظہ_نقل(بنیاد_پیغام->ڈیٹا.payload, pub_پیغام->payload, بنیاد_پیغام->ڈیٹا.payloadlen);

	اگر(pub_پیغام->properties){
		بنیاد_پیغام->ڈیٹا.properties = pub_پیغام->properties;
		pub_پیغام->properties = تہی;
	}

	اگر(db_پیغام_store(context, بنیاد_پیغام, &پیغام_expiry, mosq_mo_broker)){
		واپس 1;
	}

	اگر(pub_پیغام->qos){
		mid = mosquitto__mid_generate(context);
	}ورنہ{
		mid = 0;
	}
	واپس db_پیغام_داخل_کرو_outgoing(context, 0, mid, (uint8_t)pub_پیغام->qos, 0, بنیاد_پیغام, 0, true, true);
}


ساکن خالی read_پیغام_expiry_interval(mosquitto_property **proplist, uint32_t *پیغام_expiry)
{
	mosquitto_property *p, *پچھلا_2 = تہی;

	*پیغام_expiry = پیغام_EXPIRY_INFINITE;

	اگر(!proplist){
		واپس;
	}

	p = *proplist;
	جب(p){
		اگر(mosquitto_property_identifier(p) == MQTT_PROP_پیغام_EXPIRY_INTERVAL){
			*پیغام_expiry = mosquitto_property_int32_قدر(p);
			اگر(p == *proplist){
				*proplist = mosquitto_property_اگلا(p);
			}ورنہ{
				پچھلا_2->اگلا = mosquitto_property_اگلا(p);
			}
			mosquitto_property_free(&p);
			واپس;

		}
		پچھلا_2 = p;
		p = mosquitto_property_اگلا(p);
	}
}


ساکن خالی قطار_plugin_msgs(خالی)
{
	ساخت mosquitto_پیغام_v5 *پیغام, *tmp;
	ساخت mosquitto *context;
	uint32_t پیغام_expiry;

	DL_FOREACH_محفوظ(db.plugin_msgs, پیغام, tmp){
		DL_DELETE(db.plugin_msgs, پیغام);

		read_پیغام_expiry_interval(&پیغام->properties, &پیغام_expiry);

		اگر(پیغام->clientid){
			HASH_ڈھونڈو(hh_id, db.contexts_by_id, پیغام->clientid, لڑی_لمبائی(پیغام->clientid), context);
			اگر(context){
				single_publish(context, پیغام, پیغام_expiry);
			}
		}ورنہ{
			db_messages_easy_قطار(تہی, پیغام->topic, (uint8_t)پیغام->qos, (uint32_t)پیغام->payloadlen, پیغام->payload, پیغام->retain, پیغام_expiry, &پیغام->properties);
		}
		mosquitto_FREE(پیغام->topic);
		mosquitto_FREE(پیغام->payload);
		mosquitto_property_free_all(&پیغام->properties);
		mosquitto_FREE(پیغام->clientid);
		mosquitto_FREE(پیغام);
	}
}


خالی loop_update_اگلا_event(time_t new_ms)
{
	اگر(new_ms > 0 && new_ms < db.اگلا_event_ms){
		db.اگلا_event_ms = new_ms;
	}
}


صحیح mosquitto_main_loop(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی)
{
#اگر_تعریف_شدہ WITH_PERSISTENCE
	time_t آخری_backup = mosquitto_وقت();
#اگر_ختم
	صحیح rc;


	watchdog__init();
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS && LWS_LIBRARY_نسخہ_نمبر == 3002000
	memset(&sul, 0, حجم(ساخت lws_sorted_usec_list));
#اگر_ختم

	db.now_s = mosquitto_وقت();
	db.now_real_s = وقت(تہی);

#اگر_تعریف_شدہ WITH_BRIDGE
	rc = bridge_رجسٹر_local_connections();
	اگر(rc){
		واپس rc;
	}
#اگر_ختم

	جب(g_تسلسل){
		retain__expiry_check();
		قطار_plugin_msgs();
		context__free_disused();

		db.اگلا_event_ms = 86400000;
#اگر_تعریف_شدہ WITH_SYS_درخت
		اگر(db.config->sys_interval > 0){
			sys_درخت_update(false);
		}
#اگر_ختم

		keepalive__check();
		watchdog__check();

#اگر_تعریف_شدہ WITH_BRIDGE
		bridge_check();
#اگر_ختم
		plugin__handle_tick();
		session_expiry__check();
		will_delay__check();

		rc = mux__handle(listensock, listensock_گنتی);
		اگر(rc){
			واپس rc;
		}

#اگر_تعریف_شدہ WITH_PERSISTENCE
		اگر(db.config->persistence && db.config->autosave_interval){
			اگر(db.config->autosave_on_changes){
				اگر(db.persistence_changes >= db.config->autosave_interval){
					persist__backup(false);
					db.persistence_changes = 0;
				}
			}ورنہ{
				اگر(آخری_backup + db.config->autosave_interval < db.now_s){
					persist__backup(false);
					آخری_backup = db.now_s;
				}
			}
		}
#اگر_ختم

		rc = signal__flag_check();
		اگر(rc){
			واپس rc;
		}

#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		برائے(صحیح i=0; i<db.config->listener_گنتی; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			اگر(db.config->listeners[i].ws_context){
#اگر LWS_LIBRARY_نسخہ_نمبر > 3002000
				lws_service(db.config->listeners[i].ws_context, -1);
#وگرنہ_اگر LWS_LIBRARY_نسخہ_نمبر == 3002000
				lws_sul_جدول_وقت(db.config->listeners[i].ws_context, 0, &sul, lws__sul_callback, 10);
				lws_service(db.config->listeners[i].ws_context, 0);
#ورنہ
				lws_service(db.config->listeners[i].ws_context, 0);
#اگر_ختم

			}
		}
#اگر_ختم
	}

	واپس MOSQ_ERR_SUCCESS;
}


خالی do_disconnect(ساخت mosquitto *context, صحیح reason)
{
	ثابت حرف *id;
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	bool is_duplicate = false;
#اگر_ختم

	اگر(context->state == mosq_cs_disconnected){
		واپس;
	}
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	اگر(context->wsi){
		اگر(context->state == mosq_cs_duplicate){
			is_duplicate = true;
		}

		اگر(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
			mosquitto_سیٹ_state(context, mosq_cs_disconnect_ws);
		}
		اگر(context->wsi){
			lws_callback_on_writable(context->wsi);
		}
		اگر(context->sock != INVALID_SOCKET){
			HASH_DELETE(hh_sock, db.contexts_by_sock, context);
			mux__delete(context);
			context->sock = INVALID_SOCKET;
		}
		اگر(is_duplicate){
			/* This occurs if another client is taking over the same client id.
			 * It is important to remove this from the by_id hash here, so it
			 * doesn't leave us with multiple clients in the hash with the same
			 * id. Websockets doesn't actually close the connection here,
			 * unlike for normal clients, which means there is extra time when
			 * there could be two clients with the same id in the hash. */
			context__remove_from_by_id(context);
		}
	}ورنہ
#اگر_ختم
	{
		اگر(db.config->connection_messages == true){
			اگر(context->id){
				id = context->id;
			}ورنہ{
				id = context->پتہ;
			}
			اگر(context->state != mosq_cs_disconnecting && context->state != mosq_cs_disconnect_with_will){
				انتخاب(reason){
					حالت MOSQ_ERR_SUCCESS:
						توڑو;
					حالت MOSQ_ERR_MALFORMED_PACKET:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: malformed packet.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_PROTOCOL:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: protocol error.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_CONN_گمشدہ:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection closed by client.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_AUTH:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: not authorised.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_KEEPALIVE:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: exceeded timeout.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_OVERSIZE_PACKET:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize packet.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_PAYLOAD_سائز:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: oversize payload.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_NOMEM:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: out of memory.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_NOT_SUPPORTED:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: used disallowed feature (QoS too high, retain not supported, or bad AUTH method).", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_ADMINISTRATIVE_ACTION:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_ERRNO:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: %s.", id, context->پتہ, context->remote_پورٹ, strerror(errno));
						توڑو;
					حالت MOSQ_ERR_RECEIVE_زیادہ_سے_زیادہ_EXCEEDED:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: receive maximum exceeded.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_IMPLEMENTATION_SPECIFIC:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: implementation specific error.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_CLIENT_IDENTIFIER_NOT_VALID:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client identifier not valid.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_BAD_USERNAME_OR_PASSWORD:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad username or password.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_SERVER_UNAVAILABLE:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server unavailable.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_SERVER_BUSY:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: server busy.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_BANNED:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: client banned.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_BAD_AUTHENTICATION_طریقہ:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad authentication method.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_QUOTA_EXCEEDED:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: quota exceeded.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_CONNECTION_شرح_EXCEEDED:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: connection rate exceeded.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_SESSION_لیا_گیا_OVER:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: session taken over.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_TOPIC_ALIAS_INVALID:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: topic alias invalid.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_HTTP_BAD_ORIGIN:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: non-matching http origin.", id, context->پتہ, context->remote_پورٹ);
						توڑو;
					حالت MOSQ_ERR_PROXY:
						/* This was a proxy v2 health check connection, so don't report */
						توڑو;
					طےشدہ:
						log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: bad socket read/write: %s", id, context->پتہ, context->remote_پورٹ, mosquitto_strerror(reason));
						توڑو;
				}
			}ورنہ{
				اگر(reason == MOSQ_ERR_ADMINISTRATIVE_ACTION){
					log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected: administrative action.", id, context->پتہ, context->remote_پورٹ);
				}ورنہ{
					log__printf(تہی, MOSQ_LOG_NOTICE, "Client %s [%s:%d] disconnected.", id, context->پتہ, context->remote_پورٹ);
				}
			}
		}
		mux__delete(context);
		context__disconnect(context, reason);
	}
}
