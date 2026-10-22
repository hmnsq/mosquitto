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
*/

#شامل "config.h"

#شامل <assert.h>
#شامل <errno.h>
#شامل <stdio.h>
#شامل <string.h>

#اگر_غیر_تعریف_شدہ WIN32
#شامل <netdb.h>
#شامل <sys/socket.h>
#شامل <netinet/in.h>
#شامل <netinet/tcp.h>
#ورنہ
#شامل <winsock2.h>
#شامل <ws2tcpip.h>
#اگر_ختم

#اگر_غیر_تعریف_شدہ WIN32
#شامل <unistd.h>
#ورنہ
#شامل <process.h>
#شامل <winsock2.h>
#شامل <ws2tcpip.h>
#اگر_ختم

#شامل "mosquitto.h"
#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto_internal.h"
#شامل "net_mosq.h"
#شامل "packet_mosq.h"
#شامل "property_common.h"
#شامل "send_mosq.h"
#شامل "sys_tree.h"
#شامل "tls_mosq.h"
#شامل "util_mosq.h"
#شامل "will_mosq.h"
#شامل "utlist.h"

#اگر_تعریف_شدہ WITH_BRIDGE

ساکن خالی bridge__update_backoff(ساخت mosquitto__bridge *bridge);
#اگر defined(__GLIBC__) && defined(WITH_ADNS)
ساکن صحیح bridge__connect_step1(ساخت mosquitto *context);
ساکن صحیح bridge__connect_step2(ساخت mosquitto *context);
#اگر_ختم
ساکن خالی bridge__packet_cleanup(ساخت mosquitto *context);

ساکن ساخت mosquitto *bridge__new(ساخت mosquitto__bridge *bridge)
{
	ساخت mosquitto *new_context = تہی;
	ساخت mosquitto **bridges;
	حرف *local_id;

	أكد(bridge);

	local_id = mosquitto_strdup(bridge->local_clientid);
	اگر(!local_id){
		واپس تہی;
	}

	HASH_ڈھونڈو(hh_id, db.contexts_by_id, local_id, لڑی_لمبائی(local_id), new_context);
	اگر(new_context){
		/* (possible from persistent db) */
		mosquitto_FREE(local_id);
	}ورنہ{
		/* id wasn't found, so generate a new context */
		new_context = context__init();
		اگر(!new_context){
			mosquitto_FREE(local_id);
			واپس تہی;
		}
		new_context->id = local_id;
		context_جوڑو_مقصد_by_id(new_context);
	}
	new_context->transport = mosq_t_tcp;
	new_context->bridge = bridge;
	new_context->is_bridge = true;

	new_context->username = bridge->remote_username;
	new_context->password = bridge->remote_password;

#اگر_تعریف_شدہ WITH_TLS
	new_context->tls_cafile = bridge->tls_cafile;
	new_context->tls_capath = bridge->tls_capath;
	new_context->tls_certfile = bridge->tls_certfile;
	new_context->tls_keyfile = bridge->tls_keyfile;
	new_context->tls_cert_reqs = SSL_VERIFY_PEER;
	new_context->tls_ocsp_required = bridge->tls_ocsp_required;
	new_context->tls_نسخہ = bridge->tls_نسخہ;
	new_context->tls_insecure = bridge->tls_insecure;
	new_context->tls_alpn = bridge->tls_alpn;
	new_context->tls_ciphers = bridge->tls_ciphers;
	new_context->tls_13_ciphers = bridge->tls_13_ciphers;
	new_context->tls_engine = تہی;
	new_context->tls_keyform = mosq_k_pem;
	new_context->tls_use_os_certs = bridge->tls_use_os_certs;
	new_context->ssl_ctx_defaults = true;
#اگر_تعریف_شدہ FINAL_WITH_TLS_PSK
	new_context->tls_psk_شناختی_عنصر = bridge->tls_psk_شناختی_عنصر;
	new_context->tls_psk = bridge->tls_psk;
#اگر_ختم
#اگر_ختم

	bridge->try_نجی_accepted = true;
	اگر(bridge->clean_آغاز_local == -1){
		/* default to "regular" clean start setting */
		bridge->clean_آغاز_local = bridge->clean_آغاز;
	}
	new_context->retain_دستیاب = bridge->outgoing_retain;
	new_context->protocol = bridge->protocol_نسخہ;
	اگر(!bridge->clean_آغاز_local){
		new_context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
		plugin_persist_handle_client_جوڑو(new_context);
		اگر(new_context->expiry_list_شے){
			/* We've restored from persistence and been added to the session
			 * expiry list, even though we should never be expired */
			session_expiry__remove(new_context);
		}
	}

	bridges = mosquitto_realloc(db.bridges, (size_t)(db.bridge_گنتی+1)*حجم(ساخت mosquitto *));
	اگر(bridges){
		db.bridges = bridges;
		db.bridge_گنتی++;
		db.bridges[db.bridge_گنتی-1] = new_context;
	}ورنہ{
		واپس تہی;
	}

	واپس new_context;
}


ساکن خالی bridge__destroy(ساخت mosquitto *context)
{
	send__disconnect(context, MQTT_RC_SUCCESS, تہی);
	context__cleanup(context, true);
}


خالی bridge_آغاز_all(خالی)
{
	برائے(صحیح i=0; i<db.config->bridge_گنتی; i++){
		ساخت mosquitto *context;
		صحیح ret;

		context = bridge__new(db.config->bridges[i]);
		اگر(!context){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
			واپس;
		}
		أكد(context);

#اگر defined(__GLIBC__) && defined(WITH_ADNS)
		context->bridge->restart_t = 1; /* force quick restart of bridge */
		loop_update_اگلا_event(1000);
		ret = bridge__connect_step1(context);
#ورنہ
		ret = bridge__connect(context);
#اگر_ختم

		اگر(ret && ret != MOSQ_ERR_CONN_PENDING){
			log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Unable to connect bridge %s.",
					context->bridge->name);
		}

		db.config->bridges[i] = تہی;
	}
}


ساکن صحیح bridge_سیٹ_tcp_keepalive(ساخت mosquitto *context)
{
	غیرعلامتی صحیح enabled = 1;
	bool ret;

	اگر(context->bridge->tcp_keepalive_idle == 0
			|| context->bridge->tcp_keepalive_interval == 0
			|| context->bridge->tcp_keepalive_شمار == 0){

		واپس MOSQ_ERR_SUCCESS;
	}

#اگر_تعریف_شدہ WIN32
#  define SETSOCKOPT_TYPE حرف *
#ورنہ
#  define SETSOCKOPT_TYPE ثابت خالی *
#اگر_ختم

	ret = setsockopt(context->sock, SOL_SOCKET, SO_KEEPALIVE, (SETSOCKOPT_TYPE)&enabled, حجم(enabled));

#اگر_تعریف_شدہ TCP_KEEPIDLE
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPIDLE,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_idle, حجم(context->bridge->tcp_keepalive_idle));
#اگر_ختم
#اگر_تعریف_شدہ TCP_KEEPINTVL
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPINTVL,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_interval, حجم(context->bridge->tcp_keepalive_interval));
#اگر_ختم
#اگر_تعریف_شدہ TCP_KEEPCNT
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPCNT,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_شمار, حجم(context->bridge->tcp_keepalive_شمار));
#اگر_ختم

#تعریف_منسوخ SETSOCKOPT_TYPE

	اگر(ret){
		واپس MOSQ_ERR_UNKNOWN;
	}

	واپس MOSQ_ERR_SUCCESS;
}

#اگر_تعریف_شدہ WITH_TCP_USER_TIMEOUT


ساکن صحیح bridge_سیٹ_tcp_user_timeout(ساخت mosquitto *context)
{
	صحیح timeout = context->bridge->tcp_user_timeout;
	اگر(timeout >= 0){
		اگر(setsockopt(context->sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (حرف *)&timeout, حجم(timeout))){
			واپس MOSQ_ERR_UNKNOWN;
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}
#اگر_ختم

#اگر defined(__GLIBC__) && defined(WITH_ADNS)


ساکن صحیح bridge__connect_step1(ساخت mosquitto *context)
{
	صحیح rc;
	حرف *notification_topic;
	size_t notification_topic_len;
	uint8_t notification_payload;
	ساخت mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	اگر(!context || !context->bridge){
		واپس MOSQ_ERR_INVAL;
	}

	mosquitto_سیٹ_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->آخری_پیغام_اندر = db.now_s;
	context->اگلا_پیغام_برآمد = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_آغاز = context->bridge->clean_آغاز;
	context->اندر_packet.payload = تہی;
	context->ping_t = 0;
	context->bridge->lazy_reconnect = false;
	context->زیادہ_سے_زیادہ_packet_سائز = context->bridge->زیادہ_سے_زیادہ_packet_سائز;
	bridge__packet_cleanup(context);
	db_پیغام_reconnect_reset(context);

	db__messages_delete(context, false);

	/* Delete all local subscriptions even for clean_start==false. We don't
	 * remove any messages and the next loop carries out the resubscription
	 * anyway. This means any unwanted subs will be removed.
	 */
	sub__clean_session(context);

	LL_FOREACH(context->bridge->topics, cur_topic){
		اگر(cur_topic->direction == bd_برآمد || cur_topic->direction == bd_both){
			log__printf(تہی, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			اگر(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}ورنہ{
				qos = cur_topic->qos;
			}
			ساخت mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			اگر(sub_جوڑو(context, &sub) > 0){
				واپس 1;
			}
			retain_قطار(context, &sub);
		}
	}

	اگر(context->bridge->notifications){
		اگر(context->max_qos == 0){
			qos = 0;
		}ورنہ{
			qos = 1;
		}
		اگر(context->bridge->notification_topic){
			اگر(!context->bridge->initial_notification_مکمل){
				notification_payload = '0';
				db_messages_easy_قطار(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, پیغام_EXPIRY_INFINITE, تہی);
				context->bridge->initial_notification_مکمل = true;
			}
			notification_payload = '0';
			rc = will_سیٹ(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, تہی);
			اگر(rc != MOSQ_ERR_SUCCESS){
				واپس rc;
			}
		}ورنہ{
			notification_topic_len = لڑی_لمبائی(context->bridge->remote_clientid)+لڑی_لمبائی("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(حجم(حرف)*(notification_topic_len+1));
			اگر(!notification_topic){
				واپس MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			اگر(!context->bridge->initial_notification_مکمل){
				notification_payload = '0';
				db_messages_easy_قطار(context, notification_topic, qos, 1, &notification_payload, 1, پیغام_EXPIRY_INFINITE, تہی);
				context->bridge->initial_notification_مکمل = true;
			}

			notification_payload = '0';
			rc = will_سیٹ(context, notification_topic, 1, &notification_payload, qos, true, تہی);
			mosquitto_FREE(notification_topic);
			اگر(rc != MOSQ_ERR_SUCCESS){
				واپس rc;
			}
		}
	}

	log__printf(تہی, MOSQ_LOG_NOTICE, "Connecting bridge (step 1) %s (%s:%d)", context->bridge->name, context->bridge->پتے[context->bridge->cur_پتہ].پتہ, context->bridge->پتے[context->bridge->cur_پتہ].پورٹ);
	rc = net__try_connect_step1(context, context->bridge->پتے[context->bridge->cur_پتہ].پتہ);
	اگر(rc > 0){
		اگر(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			واپس rc; /* Error already printed */
		}ورنہ اگر(rc == MOSQ_ERR_ERRNO){
			log__printf(تہی, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ورنہ اگر(rc == MOSQ_ERR_EAI){
			log__printf(تہی, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		واپس rc;
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح bridge__connect_step2(ساخت mosquitto *context)
{
	صحیح rc;

	اگر(!context || !context->bridge){
		واپس MOSQ_ERR_INVAL;
	}

	log__printf(تہی, MOSQ_LOG_NOTICE, "Connecting bridge (step 2) %s (%s:%d)", context->bridge->name, context->bridge->پتے[context->bridge->cur_پتہ].پتہ, context->bridge->پتے[context->bridge->cur_پتہ].پورٹ);
	rc = net__try_connect_step2(context, context->bridge->پتے[context->bridge->cur_پتہ].پورٹ, &context->sock);
	اگر(rc > 0){
		اگر(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			واپس rc; /* Error already printed */
		}ورنہ اگر(rc == MOSQ_ERR_ERRNO){
			log__printf(تہی, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ورنہ اگر(rc == MOSQ_ERR_EAI){
			log__printf(تہی, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		واپس rc;
	}

	HASH_جوڑو(hh_sock, db.contexts_by_sock, sock, حجم(context->sock), context);

	اگر(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto_سیٹ_state(context, mosq_cs_connect_pending);
		mux_جوڑو_برآمد(context);
	}
	واپس rc;
}


صحیح bridge__connect_step3(ساخت mosquitto *context)
{
	صحیح rc;
	mosquitto_property receive_زیادہ_سے_زیادہ;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = تہی;

	rc = net__socket_connect_step3(context, context->bridge->پتے[context->bridge->cur_پتہ].پتہ);
	اگر(rc > 0){
		اگر(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			واپس rc; /* Error already printed */
		}ورنہ اگر(rc == MOSQ_ERR_ERRNO){
			log__printf(تہی, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ورنہ اگر(rc == MOSQ_ERR_EAI){
			log__printf(تہی, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		واپس rc;
	}

	اگر(context->bridge->باری_robin == false && context->bridge->cur_پتہ != 0){
		context->bridge->primary_retry = db.now_s + 5;
		loop_update_اگلا_event(5000);
	}

	اگر(bridge_سیٹ_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		واپس MOSQ_ERR_UNKNOWN;
	}
#اگر_تعریف_شدہ WITH_TCP_USER_TIMEOUT
	اگر(bridge_سیٹ_tcp_user_timeout(context)){
		واپس MOSQ_ERR_UNKNOWN;
	}
#اگر_ختم

	اگر(context->bridge->receive_زیادہ_سے_زیادہ != 0){
		receive_زیادہ_سے_زیادہ.قدر.i16 = context->bridge->receive_زیادہ_سے_زیادہ;
		receive_زیادہ_سے_زیادہ.identifier = MQTT_PROP_RECEIVE_زیادہ_سے_زیادہ;
		receive_زیادہ_سے_زیادہ.property_type = MQTT_PROP_TYPE_INT16;
		receive_زیادہ_سے_زیادہ.client_generated = false;
		receive_زیادہ_سے_زیادہ.اگلا = properties;
		properties = &receive_زیادہ_سے_زیادہ;
	}
	اگر(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_فوری){
		session_expiry_interval.قدر.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.اگلا = properties;
		properties = &session_expiry_interval;
	}
	اگر(context->bridge->max_topic_alias != 0){
		topic_alias_max.قدر.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_زیادہ_سے_زیادہ;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.اگلا = properties;
		properties = &topic_alias_max;
	}

	rc = send__connect(context, context->keepalive, context->clean_آغاز, properties);
	اگر(rc == MOSQ_ERR_SUCCESS){
		واپس MOSQ_ERR_SUCCESS;
	}ورنہ اگر(rc == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		واپس MOSQ_ERR_SUCCESS;
	}ورنہ{
		اگر(rc == MOSQ_ERR_TLS){
			واپس rc; /* Error already printed */
		}ورنہ اگر(rc == MOSQ_ERR_ERRNO){
			log__printf(تہی, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ورنہ اگر(rc == MOSQ_ERR_EAI){
			log__printf(تہی, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		واپس rc;
	}
}
#ورنہ


صحیح bridge__connect(ساخت mosquitto *context)
{
	صحیح rc, rc2;
	حرف *notification_topic = تہی;
	size_t notification_topic_len;
	uint8_t notification_payload;
	ساخت mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	mosquitto_property receive_زیادہ_سے_زیادہ;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = تہی;

	اگر(!context || !context->bridge){
		واپس MOSQ_ERR_INVAL;
	}

	mosquitto_سیٹ_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->آخری_پیغام_اندر = db.now_s;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->اگلا_پیغام_برآمد = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_آغاز = context->bridge->clean_آغاز;
	context->اندر_packet.payload = تہی;
	context->ping_t = 0;
	context->bridge->lazy_reconnect = false;
	context->زیادہ_سے_زیادہ_packet_سائز = context->bridge->زیادہ_سے_زیادہ_packet_سائز;
	bridge__packet_cleanup(context);
	db_پیغام_reconnect_reset(context);

	db__messages_delete(context, false);

	/* Delete all local subscriptions even for clean_start==false. We don't
	 * remove any messages and the next loop carries out the resubscription
	 * anyway. This means any unwanted subs will be removed.
	 */
	sub__clean_session(context);

	LL_FOREACH(context->bridge->topics, cur_topic){
		اگر(cur_topic->direction == bd_برآمد || cur_topic->direction == bd_both){
			log__printf(تہی, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			اگر(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}ورنہ{
				qos = cur_topic->qos;
			}
			ساخت mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			اگر(sub_جوڑو(context, &sub) > 0){
				واپس 1;
			}
		}
	}

	اگر(context->bridge->notifications){
		اگر(context->max_qos == 0){
			qos = 0;
		}ورنہ{
			qos = 1;
		}
		اگر(context->bridge->notification_topic){
			اگر(!context->bridge->initial_notification_مکمل){
				notification_payload = '0';
				db_messages_easy_قطار(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, پیغام_EXPIRY_INFINITE, تہی);
				context->bridge->initial_notification_مکمل = true;
			}

			notification_payload = '0';
			rc = will_سیٹ(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, تہی);
			اگر(rc != MOSQ_ERR_SUCCESS){
				واپس rc;
			}
		}ورنہ{
			notification_topic_len = لڑی_لمبائی(context->bridge->remote_clientid)+لڑی_لمبائی("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(حجم(حرف)*(notification_topic_len+1));
			اگر(!notification_topic){
				واپس MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			اگر(!context->bridge->initial_notification_مکمل){
				notification_payload = '0';
				db_messages_easy_قطار(context, notification_topic, qos, 1, &notification_payload, 1, پیغام_EXPIRY_INFINITE, تہی);
				context->bridge->initial_notification_مکمل = true;
			}

			notification_payload = '0';
			rc = will_سیٹ(context, notification_topic, 1, &notification_payload, qos, true, تہی);
			اگر(rc != MOSQ_ERR_SUCCESS){
				mosquitto_FREE(notification_topic);
				واپس rc;
			}
			mosquitto_FREE(notification_topic);
		}
	}

	log__printf(تہی, MOSQ_LOG_NOTICE, "Connecting bridge %s (%s:%d)", context->bridge->name, context->bridge->پتے[context->bridge->cur_پتہ].پتہ, context->bridge->پتے[context->bridge->cur_پتہ].پورٹ);
	rc = net__socket_connect(context,
			context->bridge->پتے[context->bridge->cur_پتہ].پتہ,
			context->bridge->پتے[context->bridge->cur_پتہ].پورٹ,
			context->bridge->bind_پتہ,
			false);

	اگر(rc > 0){
		اگر(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			واپس rc; /* Error already printed */
		}ورنہ اگر(rc == MOSQ_ERR_ERRNO){
			log__printf(تہی, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ورنہ اگر(rc == MOSQ_ERR_EAI){
			log__printf(تہی, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		واپس rc;
	}ورنہ اگر(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto_سیٹ_state(context, mosq_cs_connect_pending);
		mux_جوڑو_برآمد(context);
	}

	HASH_جوڑو(hh_sock, db.contexts_by_sock, sock, حجم(context->sock), context);

	اگر(bridge_سیٹ_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		واپس MOSQ_ERR_UNKNOWN;
	}
#اگر_تعریف_شدہ WITH_TCP_USER_TIMEOUT
	اگر(bridge_سیٹ_tcp_user_timeout(context)){
		واپس MOSQ_ERR_UNKNOWN;
	}
#اگر_ختم

	اگر(context->bridge->receive_زیادہ_سے_زیادہ != 0){
		receive_زیادہ_سے_زیادہ.قدر.i16 = context->bridge->receive_زیادہ_سے_زیادہ;
		receive_زیادہ_سے_زیادہ.identifier = MQTT_PROP_RECEIVE_زیادہ_سے_زیادہ;
		receive_زیادہ_سے_زیادہ.property_type = MQTT_PROP_TYPE_INT16;
		receive_زیادہ_سے_زیادہ.client_generated = false;
		receive_زیادہ_سے_زیادہ.اگلا = properties;
		properties = &receive_زیادہ_سے_زیادہ;
	}
	اگر(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_فوری){
		session_expiry_interval.قدر.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.اگلا = properties;
		properties = &session_expiry_interval;
	}
	اگر(context->bridge->max_topic_alias != 0){
		topic_alias_max.قدر.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_زیادہ_سے_زیادہ;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.اگلا = properties;
		properties = &topic_alias_max;
	}

	rc2 = send__connect(context, context->keepalive, context->clean_آغاز, properties);
	اگر(rc2 == MOSQ_ERR_SUCCESS){
		واپس rc;
	}ورنہ اگر(rc2 == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		واپس MOSQ_ERR_SUCCESS;
	}ورنہ{
		اگر(rc2 == MOSQ_ERR_TLS){
			واپس rc2; /* Error already printed */
		}ورنہ اگر(rc2 == MOSQ_ERR_ERRNO){
			log__printf(تہی, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ورنہ اگر(rc2 == MOSQ_ERR_EAI){
			log__printf(تہی, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		واپس rc2;
	}
}
#اگر_ختم


صحیح bridge__on_connect(ساخت mosquitto *context)
{
	حرف *notification_topic;
	size_t notification_topic_len;
	ساخت mosquitto__bridge_topic *cur_topic;
	صحیح sub_opts;
	bool retain = true;
	uint8_t qos;

	اگر(context->bridge->notifications){
		اگر(context->max_qos == 0){
			qos = 0;
		}ورنہ{
			qos = 1;
		}
		اگر(!context->retain_دستیاب){
			retain = false;
		}
		حرف notification_payload = '1';
		اگر(context->bridge->notification_topic){
			اگر(!context->bridge->notifications_local_only){
				اگر(send__real_publish(context, mosquitto__mid_generate(context),
						context->bridge->notification_topic, 1, &notification_payload, qos, retain, 0, 0, تہی, 0)){

					واپس 1;
				}
			}
			db_messages_easy_قطار(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, پیغام_EXPIRY_INFINITE, تہی);
		}ورنہ{
			notification_topic_len = لڑی_لمبائی(context->bridge->remote_clientid)+لڑی_لمبائی("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(حجم(حرف)*(notification_topic_len+1));
			اگر(!notification_topic){
				واپس MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);
			notification_payload = '1';
			اگر(!context->bridge->notifications_local_only){
				اگر(send__real_publish(context, mosquitto__mid_generate(context),
						notification_topic, 1, &notification_payload, qos, retain, 0, 0, تہی, 0)){

					mosquitto_FREE(notification_topic);
					واپس 1;
				}
			}
			db_messages_easy_قطار(context, notification_topic, qos, 1, &notification_payload, 1, پیغام_EXPIRY_INFINITE, تہی);
			mosquitto_FREE(notification_topic);
		}
	}

	LL_FOREACH(context->bridge->topics, cur_topic){
		اگر(cur_topic->direction == bd_اندر || cur_topic->direction == bd_both){
			اگر(cur_topic->qos > context->max_qos){
				sub_opts = context->max_qos;
			}ورنہ{
				sub_opts = cur_topic->qos;
			}
			اگر(context->bridge->protocol_نسخہ == mosq_p_mqtt5){
				sub_opts = sub_opts
						| MQTT_SUB_OPT_NO_LOCAL
						| MQTT_SUB_OPT_RETAIN_AS_PUBLISHED
						| MQTT_SUB_OPT_SEND_RETAIN_ALWAYS;
			}
			اگر(send__subscribe(context, تہی, 1, &cur_topic->remote_topic, sub_opts, تہی)){
				واپس 1;
			}
		}ورنہ{
			اگر(context->bridge->attempt_unsubscribe){
				اگر(send__unsubscribe(context, تہی, 1, &cur_topic->remote_topic, تہی)){
					/* direction = inwards only. This means we should not be subscribed
					* to the topic. It is possible that we used to be subscribed to
					* this topic so unsubscribe. */
					واپس 1;
				}
			}
		}
	}
	ساخت mosquitto_subscription sub;
	memset(&sub, 0, حجم(sub));
	LL_FOREACH(context->bridge->topics, cur_topic){
		sub.topic_filter = cur_topic->local_topic;
		اگر(cur_topic->direction == bd_برآمد || cur_topic->direction == bd_both){
			اگر(cur_topic->qos > context->max_qos){
				sub.options = context->max_qos;
			}ورنہ{
				sub.options = cur_topic->qos;
			}
			retain_قطار(context, &sub);
		}
	}

	context->bridge->connected_at = db.now_s;

	واپس MOSQ_ERR_SUCCESS;
}


صحیح bridge_رجسٹر_local_connections(خالی)
{
	ساخت mosquitto *context, *ctxt_tmp = تہی;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		اگر(context->bridge){
			اگر(mux__new(context)){
				log__printf(تہی, MOSQ_LOG_ERR, "Error in initial bridge registration: %s", strerror(errno));
				واپس MOSQ_ERR_UNKNOWN;
			}
			mux_جوڑو_برآمد(context);
		}
	}
	واپس MOSQ_ERR_SUCCESS;
}


خالی bridge__reload(خالی)
{
	صحیح i;
	صحیح j;

	// destroy old bridges that dissappeared
	برائے(i=0; i<db.bridge_گنتی; i++){
		برائے(j=0; j<db.config->bridge_گنتی; j++){
			اگر(!strcmp(db.bridges[i]->bridge->name, db.config->bridges[j]->name)){
				توڑو;
			}
		}

		اگر(j==db.config->bridge_گنتی){
			bridge__destroy(db.bridges[i]);
		}
	}

	برائے(i=0; i<db.config->bridge_گنتی; i++){
		برائے(j=0; j<db.bridge_گنتی; j++){
			اگر(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
				توڑو;
			}
		}

		اگر(j==db.bridge_گنتی){
			// a new bridge was found, create it
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = تہی;
			جاری;
		}

		اگر(db.config->bridges[i]->reload_type == brt_فوری){
			// in this case, an existing bridge should match
			برائے(j=0; j<db.bridge_گنتی; j++){
				اگر(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
					توڑو;
				}
			}

			أكد(j<db.bridge_گنتی);
			db.bridges[j]->will_delay_interval = 0;
			bridge__destroy(db.bridges[j]);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = تہی;
		}
	}
}


خالی bridge__db_cleanup(خالی)
{
	صحیح i;

	برائے(i=0; i<db.bridge_گنتی; i++){
		اگر(db.bridges[i]){
			context__cleanup(db.bridges[i], true);
		}
	}
	mosquitto_FREE(db.bridges);
}


خالی bridge__cleanup(ساخت mosquitto *context)
{
	صحیح i;

	أكد(db.bridge_گنتی > 0);

	برائے(i=0; i<db.bridge_گنتی; i++){
		اگر(db.bridges[i] == context){
			db.bridges[i] = db.bridges[db.bridge_گنتی-1];
			توڑو;
		}
	}

	db.bridge_گنتی--;
	اگر(db.bridge_گنتی == 0){
		mosquitto_FREE(db.bridges);
	}ورنہ{
		db.bridges = mosquitto_realloc(db.bridges, (غیرعلامتی)db.bridge_گنتی * حجم(db.bridges[0]));
	}

	mosquitto_FREE(context->bridge->name);
	mosquitto_FREE(context->bridge->local_clientid);
	mosquitto_FREE(context->bridge->local_username);
	mosquitto_FREE(context->bridge->local_password);
#اگر_تعریف_شدہ WITH_TLS
	mosquitto_FREE(context->bridge->tls_certfile);
	mosquitto_FREE(context->bridge->tls_keyfile);
#اگر_ختم

	اگر(context->bridge->remote_clientid != context->id){
		mosquitto_FREE(context->bridge->remote_clientid);
	}
	context->bridge->remote_clientid = تہی;

	اگر(context->bridge->remote_username != context->username){
		mosquitto_FREE(context->bridge->remote_username);
	}
	context->bridge->remote_username = تہی;

	اگر(context->bridge->remote_password != context->password){
		mosquitto_FREE(context->bridge->remote_password);
	}
	context->bridge->remote_password = تہی;
#اگر_تعریف_شدہ WITH_TLS
	اگر(context->ssl_ctx){
		SSL_CTX_free(context->ssl_ctx);
		context->ssl_ctx = تہی;
	}
#اگر_ختم

	برائے(i=0; i<context->bridge->پتہ_گنتی; i++){
		mosquitto_FREE(context->bridge->پتے[i].پتہ);
	}

	mosquitto_FREE(context->bridge->پتے);

	config__bridge_cleanup(context->bridge);
	context->bridge = تہی;
}


ساکن خالی bridge__packet_cleanup(ساخت mosquitto *context)
{
	ساخت mosquitto__packet *packet;
	اگر(!context){
		واپس;
	}

	جب(context->برآمد_packet){
		packet = context->برآمد_packet;
		context->برآمد_packet = context->برآمد_packet->اگلا;
		mosquitto_FREE(packet);
	}
	context->برآمد_packet = تہی;
	context->برآمد_packet_آخری = تہی;
	metrics__int_dec(mosq_gauge_برآمد_packets, context->برآمد_packet_گنتی);
	metrics__int_dec(mosq_gauge_برآمد_packet_بائٹیں, context->برآمد_packet_بائٹیں);
	context->برآمد_packet_گنتی = 0;
	context->برآمد_packet_بائٹیں = 0;

	packet__cleanup(&(context->اندر_packet));
}


ساکن صحیح rand_between(صحیح کم, صحیح بلند)
{
	صحیح r;
	mosquitto_getrandom(&r, حجم(صحیح));
	واپس (مطلق_قدر(r) % (بلند - کم)) + کم;
}


ساکن خالی bridge_backoff_قدم(ساخت mosquitto__bridge *bridge)
{
	/*
		“Decorrelated Jitter” calculation, according to:
			https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
	*/

	bridge->restart_timeout = rand_between(bridge->backoff_بنیاد, bridge->restart_timeout * 3);
	اگر(bridge->restart_timeout > bridge->backoff_cap){
		bridge->restart_timeout = bridge->backoff_cap;
	}
}


ساکن خالی bridge__backoff_reset(ساخت mosquitto__bridge *bridge)
{
	bridge->restart_timeout = bridge->backoff_بنیاد;
}


ساکن خالی bridge__update_backoff(ساخت mosquitto__bridge *bridge)
{
	اگر(!bridge){
		واپس;
	}
	اگر(!bridge->backoff_cap){
		/* skip if not using jitter */
		واپس;

	}
	اگر(bridge->connected_at && db.now_s - bridge->connected_at >= bridge->stable_connection_period){
		log__printf(تہی, MOSQ_LOG_INFO, "Bridge %s connection was stable enough, resetting backoff", bridge->name);
		bridge__backoff_reset(bridge);
	}ورنہ{
		bridge_backoff_قدم(bridge);
	}

	bridge->connected_at = 0;

	log__printf(تہی, MOSQ_LOG_INFO, "Bridge %s next backoff will be %d ms", bridge->name, bridge->restart_timeout);
}


ساکن خالی bridge_check_pending(ساخت mosquitto *context)
{
	صحیح err;
	socklen_t len;

	اگر(context->state == mosq_cs_connect_pending){
		len = حجم(صحیح);
		اگر(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (حرف *)&err, &len)){
			اگر(err == 0){
				mosquitto_سیٹ_state(context, mosq_cs_new);
#اگر defined(WITH_ADNS) && defined(WITH_BRIDGE)
				اگر(context->bridge){
					bridge__connect_step3(context);
				}
#اگر_ختم
			}ورنہ اگر(err == ECONNREFUSED){
				do_disconnect(context, MOSQ_ERR_CONN_گمشدہ);
				واپس;
			}
		}ورنہ{
			do_disconnect(context, MOSQ_ERR_CONN_گمشدہ);
			واپس;
		}
	}
}


ساکن bool reload_if_needed(ساخت mosquitto *context)
{
	صحیح i;

	برائے(i=0; i<db.config->bridge_گنتی; i++){
		اگر(db.config->bridges[i] && !strcmp(context->bridge->name, db.config->bridges[i]->name)){
			bridge__destroy(context);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = تہی;
			loop_update_اگلا_event(100);
			واپس true;
		}
	}

	واپس false;
}


خالی bridge_check(خالی)
{
	ساکن time_t آخری_check = 0;
	ساخت mosquitto *context = تہی;
	socklen_t len;
	صحیح i;
	صحیح rc;
	صحیح err;

	اگر(db.now_s <= آخری_check){
		واپس;
	}

	برائے(i=0; i<db.bridge_گنتی; i++){
		اگر(!db.bridges[i]){
			جاری;
		}

		context = db.bridges[i];

		اگر(net__is_connected(context)){
			mosquitto__check_keepalive(context);
			bridge_check_pending(context);

			/* Check for bridges that are not round robin and not currently
			 * connected to their primary broker. */
			اگر(context->bridge->باری_robin == false
					&& context->bridge->cur_پتہ != 0
					&& context->bridge->primary_retry
					&& db.now_s >= context->bridge->primary_retry){

				اگر(context->bridge->primary_retry_sock == INVALID_SOCKET){
					rc = net__try_connect(context->bridge->پتے[0].پتہ,
							context->bridge->پتے[0].پورٹ,
							&context->bridge->primary_retry_sock,
							context->bridge->bind_پتہ, false);

					اگر(rc == 0){
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = 0;
						mux__delete(context);
						net__socket_close(context);
						context->bridge->cur_پتہ = 0;
					}
				}ورنہ{
					len = حجم(صحیح);
					اگر(!getsockopt(context->bridge->primary_retry_sock, SOL_SOCKET, SO_ERROR, (حرف *)&err, &len)){
						اگر(err == 0){
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = 0;
							mux__delete(context);
							net__socket_close(context);
							context->bridge->cur_پتہ = context->bridge->پتہ_گنتی-1;
						}ورنہ{
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = db.now_s+5;
							loop_update_اگلا_event(5000);
						}
					}ورنہ{
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = db.now_s+5;
						loop_update_اگلا_event(5000);
					}
				}
			}
		}ورنہ{
			اگر(reload_if_needed(context)){
				جاری;
			}

			/* Want to try to restart the bridge connection */
			اگر(!context->bridge->restart_t){
				bridge__update_backoff(context->bridge);
				context->bridge->restart_t = 1000*db.now_s+context->bridge->restart_timeout;
				context->bridge->cur_پتہ++;
				اگر(context->bridge->cur_پتہ == context->bridge->پتہ_گنتی){
					context->bridge->cur_پتہ = 0;
				}
				loop_update_اگلا_event(context->bridge->restart_timeout);
			}ورنہ{
				اگر((context->bridge->آغاز_type == bst_lazy && context->bridge->lazy_reconnect)
						|| (context->bridge->آغاز_type == bst_automatic && 1000*db.now_s >= context->bridge->restart_t)){

#اگر defined(__GLIBC__) && defined(WITH_ADNS)
					اگر(context->adns){
						/* Connection attempted, waiting on DNS lookup */
						rc = gai_error(context->adns);
						اگر(rc == EAI_INPROGRESS){
							/* Just keep on waiting */
						}ورنہ اگر(rc == 0){
							rc = bridge__connect_step2(context);
							اگر(rc == MOSQ_ERR_SUCCESS){
								mux__new(context);
								اگر(context->برآمد_packet){
									mux_جوڑو_برآمد(context);
								}
							}ورنہ اگر(rc == MOSQ_ERR_CONN_PENDING){
								mux__new(context);
								mux_جوڑو_برآمد(context);
								context->bridge->restart_t = 0;
							}ورنہ{
								context->bridge->cur_پتہ++;
								اگر(context->bridge->cur_پتہ == context->bridge->پتہ_گنتی){
									context->bridge->cur_پتہ = 0;
								}
								context->bridge->restart_t = 0;
							}
						}ورنہ{
							/* Need to retry */
							اگر(context->adns->ar_نتیجہ){
								freeaddrinfo(context->adns->ar_نتیجہ);
							}
							mosquitto_FREE(context->adns);
							context->bridge->restart_t = 0;
						}
					}ورنہ{
						rc = bridge__connect_step1(context);
						اگر(rc){
							context->bridge->cur_پتہ++;
							اگر(context->bridge->cur_پتہ == context->bridge->پتہ_گنتی){
								context->bridge->cur_پتہ = 0;
							}
						}ورنہ{
							/* Short wait for ADNS lookup */
							context->bridge->restart_t = 1;
							loop_update_اگلا_event(1000);
						}
					}
#ورنہ
					{
						rc = bridge__connect(context);
						context->bridge->restart_t = 0;
						اگر(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING){
							اگر(context->bridge->باری_robin == false && context->bridge->cur_پتہ != 0){
								context->bridge->primary_retry = db.now_s + 5;
								loop_update_اگلا_event(5000);
							}
							mux__new(context);
							اگر(context->برآمد_packet){
								mux_جوڑو_برآمد(context);
							}
						}ورنہ{
							context->bridge->cur_پتہ++;
							اگر(context->bridge->cur_پتہ == context->bridge->پتہ_گنتی){
								context->bridge->cur_پتہ = 0;
							}
						}
					}
#اگر_ختم
				}
			}
		}
	}
}

#اگر_ختم
