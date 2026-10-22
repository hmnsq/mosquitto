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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/socket.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/tcp.h>
#ਨਹੀਂਤਾਂ
#ਅਨ੍ਤਰ੍ਭਾਵ <winsock2.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ws2tcpip.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਨਹੀਂਤਾਂ
#ਅਨ੍ਤਰ੍ਭਾਵ <process.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <winsock2.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ws2tcpip.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "net_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "property_common.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "send_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sys_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "tls_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "will_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utlist.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE

ਸਥਿਰ ਖਾਲੀ bridge__update_backoff(ਬਣਤਰ mosquitto__bridge *bridge);
#ਜੇ defined(__GLIBC__) && defined(WITH_ADNS)
ਸਥਿਰ ਪੂਰਨ bridge__connect_step1(ਬਣਤਰ mosquitto *context);
ਸਥਿਰ ਪੂਰਨ bridge__connect_step2(ਬਣਤਰ mosquitto *context);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ bridge__packet_cleanup(ਬਣਤਰ mosquitto *context);

ਸਥਿਰ ਬਣਤਰ mosquitto *bridge__new(ਬਣਤਰ mosquitto__bridge *bridge)
{
	ਬਣਤਰ mosquitto *new_context = ਨਿਰਾ;
	ਬਣਤਰ mosquitto **bridges;
	ਅੱਖਰ *local_id;

	ਪ੍ਰਤਿਜ੍ਞਾ(bridge);

	local_id = mosquitto_strdup(bridge->local_clientid);
	ਜੇ(!local_id){
		ਵਾਪਸ ਨਿਰਾ;
	}

	HASH_ਲੱਭੋ(hh_id, db.contexts_by_id, local_id, ਲੜੀ_ਲੰਬਾਈ(local_id), new_context);
	ਜੇ(new_context){
		/* (possible from persistent db) */
		mosquitto_FREE(local_id);
	}ਨਹੀਂਤਾਂ{
		/* id wasn't found, so generate a new context */
		new_context = context__init();
		ਜੇ(!new_context){
			mosquitto_FREE(local_id);
			ਵਾਪਸ ਨਿਰਾ;
		}
		new_context->id = local_id;
		context_ਜੋੜੋ_ਮੰਜ਼ਿਲ_by_id(new_context);
	}
	new_context->transport = mosq_t_tcp;
	new_context->bridge = bridge;
	new_context->is_bridge = true;

	new_context->username = bridge->remote_username;
	new_context->password = bridge->remote_password;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	new_context->tls_cafile = bridge->tls_cafile;
	new_context->tls_capath = bridge->tls_capath;
	new_context->tls_certfile = bridge->tls_certfile;
	new_context->tls_keyfile = bridge->tls_keyfile;
	new_context->tls_cert_reqs = SSL_VERIFY_PEER;
	new_context->tls_ocsp_required = bridge->tls_ocsp_required;
	new_context->tls_ਸੰਸਕਰਣ = bridge->tls_ਸੰਸਕਰਣ;
	new_context->tls_insecure = bridge->tls_insecure;
	new_context->tls_alpn = bridge->tls_alpn;
	new_context->tls_ciphers = bridge->tls_ciphers;
	new_context->tls_13_ciphers = bridge->tls_13_ciphers;
	new_context->tls_engine = ਨਿਰਾ;
	new_context->tls_keyform = mosq_k_pem;
	new_context->tls_use_os_certs = bridge->tls_use_os_certs;
	new_context->ssl_ctx_defaults = true;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FINAL_WITH_TLS_PSK
	new_context->tls_psk_ਪਛਾਣ_ਤੱਤ = bridge->tls_psk_ਪਛਾਣ_ਤੱਤ;
	new_context->tls_psk = bridge->tls_psk;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

	bridge->try_ਨਿੱਜੀ_accepted = true;
	ਜੇ(bridge->clean_ਸ਼ੁਰੂ_local == -1){
		/* default to "regular" clean start setting */
		bridge->clean_ਸ਼ੁਰੂ_local = bridge->clean_ਸ਼ੁਰੂ;
	}
	new_context->retain_ਉਪਲਬਧ = bridge->outgoing_retain;
	new_context->protocol = bridge->protocol_ਸੰਸਕਰਣ;
	ਜੇ(!bridge->clean_ਸ਼ੁਰੂ_local){
		new_context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
		plugin_persist_handle_client_ਜੋੜੋ(new_context);
		ਜੇ(new_context->expiry_list_ਵਸਤੂ){
			/* We've restored from persistence and been added to the session
			 * expiry list, even though we should never be expired */
			session_expiry__remove(new_context);
		}
	}

	bridges = mosquitto_realloc(db.bridges, (size_t)(db.bridge_ਗਿਣਤੀ+1)*ਆਕਾਰ(ਬਣਤਰ mosquitto *));
	ਜੇ(bridges){
		db.bridges = bridges;
		db.bridge_ਗਿਣਤੀ++;
		db.bridges[db.bridge_ਗਿਣਤੀ-1] = new_context;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ ਨਿਰਾ;
	}

	ਵਾਪਸ new_context;
}


ਸਥਿਰ ਖਾਲੀ bridge__destroy(ਬਣਤਰ mosquitto *context)
{
	send__disconnect(context, MQTT_RC_SUCCESS, ਨਿਰਾ);
	context__cleanup(context, true);
}


ਖਾਲੀ bridge_ਸ਼ੁਰੂ_all(ਖਾਲੀ)
{
	ਲਈ(ਪੂਰਨ i=0; i<db.config->bridge_ਗਿਣਤੀ; i++){
		ਬਣਤਰ mosquitto *context;
		ਪੂਰਨ ret;

		context = bridge__new(db.config->bridges[i]);
		ਜੇ(!context){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
			ਵਾਪਸ;
		}
		ਪ੍ਰਤਿਜ੍ਞਾ(context);

#ਜੇ defined(__GLIBC__) && defined(WITH_ADNS)
		context->bridge->restart_t = 1; /* force quick restart of bridge */
		loop_update_ਅਗਲਾ_event(1000);
		ret = bridge__connect_step1(context);
#ਨਹੀਂਤਾਂ
		ret = bridge__connect(context);
#ਜੇ_ਅੰਤ

		ਜੇ(ret && ret != MOSQ_ERR_CONN_PENDING){
			log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Unable to connect bridge %s.",
					context->bridge->name);
		}

		db.config->bridges[i] = ਨਿਰਾ;
	}
}


ਸਥਿਰ ਪੂਰਨ bridge_ਸੈੱਟ_tcp_keepalive(ਬਣਤਰ mosquitto *context)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ enabled = 1;
	bool ret;

	ਜੇ(context->bridge->tcp_keepalive_idle == 0
			|| context->bridge->tcp_keepalive_interval == 0
			|| context->bridge->tcp_keepalive_counter == 0){

		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
#  define SETSOCKOPT_TYPE ਅੱਖਰ *
#ਨਹੀਂਤਾਂ
#  define SETSOCKOPT_TYPE ਅਚਲ ਖਾਲੀ *
#ਜੇ_ਅੰਤ

	ret = setsockopt(context->sock, SOL_SOCKET, SO_KEEPALIVE, (SETSOCKOPT_TYPE)&enabled, ਆਕਾਰ(enabled));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TCP_KEEPIDLE
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPIDLE,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_idle, ਆਕਾਰ(context->bridge->tcp_keepalive_idle));
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TCP_KEEPINTVL
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPINTVL,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_interval, ਆਕਾਰ(context->bridge->tcp_keepalive_interval));
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TCP_KEEPCNT
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPCNT,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_counter, ਆਕਾਰ(context->bridge->tcp_keepalive_counter));
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ SETSOCKOPT_TYPE

	ਜੇ(ret){
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TCP_USER_TIMEOUT


ਸਥਿਰ ਪੂਰਨ bridge_ਸੈੱਟ_tcp_user_timeout(ਬਣਤਰ mosquitto *context)
{
	ਪੂਰਨ timeout = context->bridge->tcp_user_timeout;
	ਜੇ(timeout >= 0){
		ਜੇ(setsockopt(context->sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (ਅੱਖਰ *)&timeout, ਆਕਾਰ(timeout))){
			ਵਾਪਸ MOSQ_ERR_UNKNOWN;
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(__GLIBC__) && defined(WITH_ADNS)


ਸਥਿਰ ਪੂਰਨ bridge__connect_step1(ਬਣਤਰ mosquitto *context)
{
	ਪੂਰਨ rc;
	ਅੱਖਰ *notification_topic;
	size_t notification_topic_len;
	uint8_t notification_payload;
	ਬਣਤਰ mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	ਜੇ(!context || !context->bridge){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	mosquitto_ਸੈੱਟ_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->ਆਖਰੀ_ਸੁਨੇਹਾ_ਵਿੱਚ = db.now_s;
	context->ਅਗਲਾ_ਸੁਨੇਹਾ_ਬਾਹਰਲਾ = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_ਸ਼ੁਰੂ = context->bridge->clean_ਸ਼ੁਰੂ;
	context->ਵਿੱਚ_packet.payload = ਨਿਰਾ;
	context->ping_t = 0;
	context->bridge->lazy_reconnect = false;
	context->ਵੱਧ_ਤੋਂ_ਵੱਧ_packet_ਮਾਪ = context->bridge->ਵੱਧ_ਤੋਂ_ਵੱਧ_packet_ਮਾਪ;
	bridge__packet_cleanup(context);
	db_ਸੁਨੇਹਾ_reconnect_reset(context);

	db__messages_delete(context, false);

	/* Delete all local subscriptions even for clean_start==false. We don't
	 * remove any messages and the next loop carries out the resubscription
	 * anyway. This means any unwanted subs will be removed.
	 */
	sub__clean_session(context);

	LL_FOREACH(context->bridge->topics, cur_topic){
		ਜੇ(cur_topic->direction == bd_ਬਾਹਰਲਾ || cur_topic->direction == bd_both){
			log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			ਜੇ(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}ਨਹੀਂਤਾਂ{
				qos = cur_topic->qos;
			}
			ਬਣਤਰ mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			ਜੇ(sub_ਜੋੜੋ(context, &sub) > 0){
				ਵਾਪਸ 1;
			}
			retain_ਕਤਾਰ(context, &sub);
		}
	}

	ਜੇ(context->bridge->notifications){
		ਜੇ(context->max_qos == 0){
			qos = 0;
		}ਨਹੀਂਤਾਂ{
			qos = 1;
		}
		ਜੇ(context->bridge->notification_topic){
			ਜੇ(!context->bridge->initial_notification_ਪੂਰਾ){
				notification_payload = '0';
				db_messages_easy_ਕਤਾਰ(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, ਸੁਨੇਹਾ_EXPIRY_INFINITE, ਨਿਰਾ);
				context->bridge->initial_notification_ਪੂਰਾ = true;
			}
			notification_payload = '0';
			rc = will_ਸੈੱਟ(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, ਨਿਰਾ);
			ਜੇ(rc != MOSQ_ERR_SUCCESS){
				ਵਾਪਸ rc;
			}
		}ਨਹੀਂਤਾਂ{
			notification_topic_len = ਲੜੀ_ਲੰਬਾਈ(context->bridge->remote_clientid)+ਲੜੀ_ਲੰਬਾਈ("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(ਆਕਾਰ(ਅੱਖਰ)*(notification_topic_len+1));
			ਜੇ(!notification_topic){
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			ਜੇ(!context->bridge->initial_notification_ਪੂਰਾ){
				notification_payload = '0';
				db_messages_easy_ਕਤਾਰ(context, notification_topic, qos, 1, &notification_payload, 1, ਸੁਨੇਹਾ_EXPIRY_INFINITE, ਨਿਰਾ);
				context->bridge->initial_notification_ਪੂਰਾ = true;
			}

			notification_payload = '0';
			rc = will_ਸੈੱਟ(context, notification_topic, 1, &notification_payload, qos, true, ਨਿਰਾ);
			mosquitto_FREE(notification_topic);
			ਜੇ(rc != MOSQ_ERR_SUCCESS){
				ਵਾਪਸ rc;
			}
		}
	}

	log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Connecting bridge (step 1) %s (%s:%d)", context->bridge->name, context->bridge->ਪਤੇ[context->bridge->cur_ਪਤਾ].ਪਤਾ, context->bridge->ਪਤੇ[context->bridge->cur_ਪਤਾ].ਪੋਰਟ);
	rc = net__try_connect_step1(context, context->bridge->ਪਤੇ[context->bridge->cur_ਪਤਾ].ਪਤਾ);
	ਜੇ(rc > 0){
		ਜੇ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ਵਾਪਸ rc; /* Error already printed */
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_ERRNO){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_EAI){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ਵਾਪਸ rc;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ bridge__connect_step2(ਬਣਤਰ mosquitto *context)
{
	ਪੂਰਨ rc;

	ਜੇ(!context || !context->bridge){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Connecting bridge (step 2) %s (%s:%d)", context->bridge->name, context->bridge->ਪਤੇ[context->bridge->cur_ਪਤਾ].ਪਤਾ, context->bridge->ਪਤੇ[context->bridge->cur_ਪਤਾ].ਪੋਰਟ);
	rc = net__try_connect_step2(context, context->bridge->ਪਤੇ[context->bridge->cur_ਪਤਾ].ਪੋਰਟ, &context->sock);
	ਜੇ(rc > 0){
		ਜੇ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ਵਾਪਸ rc; /* Error already printed */
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_ERRNO){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_EAI){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ਵਾਪਸ rc;
	}

	HASH_ਜੋੜੋ(hh_sock, db.contexts_by_sock, sock, ਆਕਾਰ(context->sock), context);

	ਜੇ(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto_ਸੈੱਟ_state(context, mosq_cs_connect_pending);
		mux_ਜੋੜੋ_ਬਾਹਰਲਾ(context);
	}
	ਵਾਪਸ rc;
}


ਪੂਰਨ bridge__connect_step3(ਬਣਤਰ mosquitto *context)
{
	ਪੂਰਨ rc;
	mosquitto_property receive_ਵੱਧ_ਤੋਂ_ਵੱਧ;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = ਨਿਰਾ;

	rc = net__socket_connect_step3(context, context->bridge->ਪਤੇ[context->bridge->cur_ਪਤਾ].ਪਤਾ);
	ਜੇ(rc > 0){
		ਜੇ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ਵਾਪਸ rc; /* Error already printed */
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_ERRNO){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_EAI){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ਵਾਪਸ rc;
	}

	ਜੇ(context->bridge->ਗੇੜਾ_robin == false && context->bridge->cur_ਪਤਾ != 0){
		context->bridge->primary_retry = db.now_s + 5;
		loop_update_ਅਗਲਾ_event(5000);
	}

	ਜੇ(bridge_ਸੈੱਟ_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TCP_USER_TIMEOUT
	ਜੇ(bridge_ਸੈੱਟ_tcp_user_timeout(context)){
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}
#ਜੇ_ਅੰਤ

	ਜੇ(context->bridge->receive_ਵੱਧ_ਤੋਂ_ਵੱਧ != 0){
		receive_ਵੱਧ_ਤੋਂ_ਵੱਧ.ਮੁੱਲ.i16 = context->bridge->receive_ਵੱਧ_ਤੋਂ_ਵੱਧ;
		receive_ਵੱਧ_ਤੋਂ_ਵੱਧ.identifier = MQTT_PROP_RECEIVE_ਵੱਧ_ਤੋਂ_ਵੱਧ;
		receive_ਵੱਧ_ਤੋਂ_ਵੱਧ.property_type = MQTT_PROP_TYPE_INT16;
		receive_ਵੱਧ_ਤੋਂ_ਵੱਧ.client_generated = false;
		receive_ਵੱਧ_ਤੋਂ_ਵੱਧ.ਅਗਲਾ = properties;
		properties = &receive_ਵੱਧ_ਤੋਂ_ਵੱਧ;
	}
	ਜੇ(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_ਤੁਰੰਤ){
		session_expiry_interval.ਮੁੱਲ.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.ਅਗਲਾ = properties;
		properties = &session_expiry_interval;
	}
	ਜੇ(context->bridge->max_topic_alias != 0){
		topic_alias_max.ਮੁੱਲ.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_ਵੱਧ_ਤੋਂ_ਵੱਧ;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.ਅਗਲਾ = properties;
		properties = &topic_alias_max;
	}

	rc = send__connect(context, context->keepalive, context->clean_ਸ਼ੁਰੂ, properties);
	ਜੇ(rc == MOSQ_ERR_SUCCESS){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}ਨਹੀਂਤਾਂ{
		ਜੇ(rc == MOSQ_ERR_TLS){
			ਵਾਪਸ rc; /* Error already printed */
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_ERRNO){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_EAI){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		ਵਾਪਸ rc;
	}
}
#ਨਹੀਂਤਾਂ


ਪੂਰਨ bridge__connect(ਬਣਤਰ mosquitto *context)
{
	ਪੂਰਨ rc, rc2;
	ਅੱਖਰ *notification_topic = ਨਿਰਾ;
	size_t notification_topic_len;
	uint8_t notification_payload;
	ਬਣਤਰ mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	mosquitto_property receive_ਵੱਧ_ਤੋਂ_ਵੱਧ;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = ਨਿਰਾ;

	ਜੇ(!context || !context->bridge){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	mosquitto_ਸੈੱਟ_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->ਆਖਰੀ_ਸੁਨੇਹਾ_ਵਿੱਚ = db.now_s;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->ਅਗਲਾ_ਸੁਨੇਹਾ_ਬਾਹਰਲਾ = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_ਸ਼ੁਰੂ = context->bridge->clean_ਸ਼ੁਰੂ;
	context->ਵਿੱਚ_packet.payload = ਨਿਰਾ;
	context->ping_t = 0;
	context->bridge->lazy_reconnect = false;
	context->ਵੱਧ_ਤੋਂ_ਵੱਧ_packet_ਮਾਪ = context->bridge->ਵੱਧ_ਤੋਂ_ਵੱਧ_packet_ਮਾਪ;
	bridge__packet_cleanup(context);
	db_ਸੁਨੇਹਾ_reconnect_reset(context);

	db__messages_delete(context, false);

	/* Delete all local subscriptions even for clean_start==false. We don't
	 * remove any messages and the next loop carries out the resubscription
	 * anyway. This means any unwanted subs will be removed.
	 */
	sub__clean_session(context);

	LL_FOREACH(context->bridge->topics, cur_topic){
		ਜੇ(cur_topic->direction == bd_ਬਾਹਰਲਾ || cur_topic->direction == bd_both){
			log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			ਜੇ(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}ਨਹੀਂਤਾਂ{
				qos = cur_topic->qos;
			}
			ਬਣਤਰ mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			ਜੇ(sub_ਜੋੜੋ(context, &sub) > 0){
				ਵਾਪਸ 1;
			}
		}
	}

	ਜੇ(context->bridge->notifications){
		ਜੇ(context->max_qos == 0){
			qos = 0;
		}ਨਹੀਂਤਾਂ{
			qos = 1;
		}
		ਜੇ(context->bridge->notification_topic){
			ਜੇ(!context->bridge->initial_notification_ਪੂਰਾ){
				notification_payload = '0';
				db_messages_easy_ਕਤਾਰ(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, ਸੁਨੇਹਾ_EXPIRY_INFINITE, ਨਿਰਾ);
				context->bridge->initial_notification_ਪੂਰਾ = true;
			}

			notification_payload = '0';
			rc = will_ਸੈੱਟ(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, ਨਿਰਾ);
			ਜੇ(rc != MOSQ_ERR_SUCCESS){
				ਵਾਪਸ rc;
			}
		}ਨਹੀਂਤਾਂ{
			notification_topic_len = ਲੜੀ_ਲੰਬਾਈ(context->bridge->remote_clientid)+ਲੜੀ_ਲੰਬਾਈ("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(ਆਕਾਰ(ਅੱਖਰ)*(notification_topic_len+1));
			ਜੇ(!notification_topic){
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			ਜੇ(!context->bridge->initial_notification_ਪੂਰਾ){
				notification_payload = '0';
				db_messages_easy_ਕਤਾਰ(context, notification_topic, qos, 1, &notification_payload, 1, ਸੁਨੇਹਾ_EXPIRY_INFINITE, ਨਿਰਾ);
				context->bridge->initial_notification_ਪੂਰਾ = true;
			}

			notification_payload = '0';
			rc = will_ਸੈੱਟ(context, notification_topic, 1, &notification_payload, qos, true, ਨਿਰਾ);
			ਜੇ(rc != MOSQ_ERR_SUCCESS){
				mosquitto_FREE(notification_topic);
				ਵਾਪਸ rc;
			}
			mosquitto_FREE(notification_topic);
		}
	}

	log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Connecting bridge %s (%s:%d)", context->bridge->name, context->bridge->ਪਤੇ[context->bridge->cur_ਪਤਾ].ਪਤਾ, context->bridge->ਪਤੇ[context->bridge->cur_ਪਤਾ].ਪੋਰਟ);
	rc = net__socket_connect(context,
			context->bridge->ਪਤੇ[context->bridge->cur_ਪਤਾ].ਪਤਾ,
			context->bridge->ਪਤੇ[context->bridge->cur_ਪਤਾ].ਪੋਰਟ,
			context->bridge->bind_ਪਤਾ,
			false);

	ਜੇ(rc > 0){
		ਜੇ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ਵਾਪਸ rc; /* Error already printed */
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_ERRNO){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_EAI){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ਵਾਪਸ rc;
	}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto_ਸੈੱਟ_state(context, mosq_cs_connect_pending);
		mux_ਜੋੜੋ_ਬਾਹਰਲਾ(context);
	}

	HASH_ਜੋੜੋ(hh_sock, db.contexts_by_sock, sock, ਆਕਾਰ(context->sock), context);

	ਜੇ(bridge_ਸੈੱਟ_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TCP_USER_TIMEOUT
	ਜੇ(bridge_ਸੈੱਟ_tcp_user_timeout(context)){
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}
#ਜੇ_ਅੰਤ

	ਜੇ(context->bridge->receive_ਵੱਧ_ਤੋਂ_ਵੱਧ != 0){
		receive_ਵੱਧ_ਤੋਂ_ਵੱਧ.ਮੁੱਲ.i16 = context->bridge->receive_ਵੱਧ_ਤੋਂ_ਵੱਧ;
		receive_ਵੱਧ_ਤੋਂ_ਵੱਧ.identifier = MQTT_PROP_RECEIVE_ਵੱਧ_ਤੋਂ_ਵੱਧ;
		receive_ਵੱਧ_ਤੋਂ_ਵੱਧ.property_type = MQTT_PROP_TYPE_INT16;
		receive_ਵੱਧ_ਤੋਂ_ਵੱਧ.client_generated = false;
		receive_ਵੱਧ_ਤੋਂ_ਵੱਧ.ਅਗਲਾ = properties;
		properties = &receive_ਵੱਧ_ਤੋਂ_ਵੱਧ;
	}
	ਜੇ(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_ਤੁਰੰਤ){
		session_expiry_interval.ਮੁੱਲ.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.ਅਗਲਾ = properties;
		properties = &session_expiry_interval;
	}
	ਜੇ(context->bridge->max_topic_alias != 0){
		topic_alias_max.ਮੁੱਲ.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_ਵੱਧ_ਤੋਂ_ਵੱਧ;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.ਅਗਲਾ = properties;
		properties = &topic_alias_max;
	}

	rc2 = send__connect(context, context->keepalive, context->clean_ਸ਼ੁਰੂ, properties);
	ਜੇ(rc2 == MOSQ_ERR_SUCCESS){
		ਵਾਪਸ rc;
	}ਨਹੀਂਤਾਂ ਜੇ(rc2 == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}ਨਹੀਂਤਾਂ{
		ਜੇ(rc2 == MOSQ_ERR_TLS){
			ਵਾਪਸ rc2; /* Error already printed */
		}ਨਹੀਂਤਾਂ ਜੇ(rc2 == MOSQ_ERR_ERRNO){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ਨਹੀਂਤਾਂ ਜੇ(rc2 == MOSQ_ERR_EAI){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		ਵਾਪਸ rc2;
	}
}
#ਜੇ_ਅੰਤ


ਪੂਰਨ bridge__on_connect(ਬਣਤਰ mosquitto *context)
{
	ਅੱਖਰ *notification_topic;
	size_t notification_topic_len;
	ਬਣਤਰ mosquitto__bridge_topic *cur_topic;
	ਪੂਰਨ sub_opts;
	bool retain = true;
	uint8_t qos;

	ਜੇ(context->bridge->notifications){
		ਜੇ(context->max_qos == 0){
			qos = 0;
		}ਨਹੀਂਤਾਂ{
			qos = 1;
		}
		ਜੇ(!context->retain_ਉਪਲਬਧ){
			retain = false;
		}
		ਅੱਖਰ notification_payload = '1';
		ਜੇ(context->bridge->notification_topic){
			ਜੇ(!context->bridge->notifications_local_only){
				ਜੇ(send__real_publish(context, mosquitto__mid_generate(context),
						context->bridge->notification_topic, 1, &notification_payload, qos, retain, 0, 0, ਨਿਰਾ, 0)){

					ਵਾਪਸ 1;
				}
			}
			db_messages_easy_ਕਤਾਰ(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, ਸੁਨੇਹਾ_EXPIRY_INFINITE, ਨਿਰਾ);
		}ਨਹੀਂਤਾਂ{
			notification_topic_len = ਲੜੀ_ਲੰਬਾਈ(context->bridge->remote_clientid)+ਲੜੀ_ਲੰਬਾਈ("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(ਆਕਾਰ(ਅੱਖਰ)*(notification_topic_len+1));
			ਜੇ(!notification_topic){
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);
			notification_payload = '1';
			ਜੇ(!context->bridge->notifications_local_only){
				ਜੇ(send__real_publish(context, mosquitto__mid_generate(context),
						notification_topic, 1, &notification_payload, qos, retain, 0, 0, ਨਿਰਾ, 0)){

					mosquitto_FREE(notification_topic);
					ਵਾਪਸ 1;
				}
			}
			db_messages_easy_ਕਤਾਰ(context, notification_topic, qos, 1, &notification_payload, 1, ਸੁਨੇਹਾ_EXPIRY_INFINITE, ਨਿਰਾ);
			mosquitto_FREE(notification_topic);
		}
	}

	LL_FOREACH(context->bridge->topics, cur_topic){
		ਜੇ(cur_topic->direction == bd_ਵਿੱਚ || cur_topic->direction == bd_both){
			ਜੇ(cur_topic->qos > context->max_qos){
				sub_opts = context->max_qos;
			}ਨਹੀਂਤਾਂ{
				sub_opts = cur_topic->qos;
			}
			ਜੇ(context->bridge->protocol_ਸੰਸਕਰਣ == mosq_p_mqtt5){
				sub_opts = sub_opts
						| MQTT_SUB_OPT_NO_LOCAL
						| MQTT_SUB_OPT_RETAIN_AS_PUBLISHED
						| MQTT_SUB_OPT_SEND_RETAIN_ALWAYS;
			}
			ਜੇ(send__subscribe(context, ਨਿਰਾ, 1, &cur_topic->remote_topic, sub_opts, ਨਿਰਾ)){
				ਵਾਪਸ 1;
			}
		}ਨਹੀਂਤਾਂ{
			ਜੇ(context->bridge->attempt_unsubscribe){
				ਜੇ(send__unsubscribe(context, ਨਿਰਾ, 1, &cur_topic->remote_topic, ਨਿਰਾ)){
					/* direction = inwards only. This means we should not be subscribed
					* to the topic. It is possible that we used to be subscribed to
					* this topic so unsubscribe. */
					ਵਾਪਸ 1;
				}
			}
		}
	}
	ਬਣਤਰ mosquitto_subscription sub;
	memset(&sub, 0, ਆਕਾਰ(sub));
	LL_FOREACH(context->bridge->topics, cur_topic){
		sub.topic_filter = cur_topic->local_topic;
		ਜੇ(cur_topic->direction == bd_ਬਾਹਰਲਾ || cur_topic->direction == bd_both){
			ਜੇ(cur_topic->qos > context->max_qos){
				sub.options = context->max_qos;
			}ਨਹੀਂਤਾਂ{
				sub.options = cur_topic->qos;
			}
			retain_ਕਤਾਰ(context, &sub);
		}
	}

	context->bridge->connected_at = db.now_s;

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ bridge_ਰਜਿਸਟਰ_local_connections(ਖਾਲੀ)
{
	ਬਣਤਰ mosquitto *context, *ctxt_tmp = ਨਿਰਾ;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ਜੇ(context->bridge){
			ਜੇ(mux__new(context)){
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error in initial bridge registration: %s", strerror(errno));
				ਵਾਪਸ MOSQ_ERR_UNKNOWN;
			}
			mux_ਜੋੜੋ_ਬਾਹਰਲਾ(context);
		}
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਖਾਲੀ bridge__reload(ਖਾਲੀ)
{
	ਪੂਰਨ i;
	ਪੂਰਨ j;

	// destroy old bridges that dissappeared
	ਲਈ(i=0; i<db.bridge_ਗਿਣਤੀ; i++){
		ਲਈ(j=0; j<db.config->bridge_ਗਿਣਤੀ; j++){
			ਜੇ(!strcmp(db.bridges[i]->bridge->name, db.config->bridges[j]->name)){
				ਰੋਕੋ;
			}
		}

		ਜੇ(j==db.config->bridge_ਗਿਣਤੀ){
			bridge__destroy(db.bridges[i]);
		}
	}

	ਲਈ(i=0; i<db.config->bridge_ਗਿਣਤੀ; i++){
		ਲਈ(j=0; j<db.bridge_ਗਿਣਤੀ; j++){
			ਜੇ(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
				ਰੋਕੋ;
			}
		}

		ਜੇ(j==db.bridge_ਗਿਣਤੀ){
			// a new bridge was found, create it
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ਨਿਰਾ;
			ਜਾਰੀ;
		}

		ਜੇ(db.config->bridges[i]->reload_type == brt_ਤੁਰੰਤ){
			// in this case, an existing bridge should match
			ਲਈ(j=0; j<db.bridge_ਗਿਣਤੀ; j++){
				ਜੇ(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
					ਰੋਕੋ;
				}
			}

			ਪ੍ਰਤਿਜ੍ਞਾ(j<db.bridge_ਗਿਣਤੀ);
			db.bridges[j]->will_delay_interval = 0;
			bridge__destroy(db.bridges[j]);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ਨਿਰਾ;
		}
	}
}


ਖਾਲੀ bridge__db_cleanup(ਖਾਲੀ)
{
	ਪੂਰਨ i;

	ਲਈ(i=0; i<db.bridge_ਗਿਣਤੀ; i++){
		ਜੇ(db.bridges[i]){
			context__cleanup(db.bridges[i], true);
		}
	}
	mosquitto_FREE(db.bridges);
}


ਖਾਲੀ bridge__cleanup(ਬਣਤਰ mosquitto *context)
{
	ਪੂਰਨ i;

	ਪ੍ਰਤਿਜ੍ਞਾ(db.bridge_ਗਿਣਤੀ > 0);

	ਲਈ(i=0; i<db.bridge_ਗਿਣਤੀ; i++){
		ਜੇ(db.bridges[i] == context){
			db.bridges[i] = db.bridges[db.bridge_ਗਿਣਤੀ-1];
			ਰੋਕੋ;
		}
	}

	db.bridge_ਗਿਣਤੀ--;
	ਜੇ(db.bridge_ਗਿਣਤੀ == 0){
		mosquitto_FREE(db.bridges);
	}ਨਹੀਂਤਾਂ{
		db.bridges = mosquitto_realloc(db.bridges, (ਬਿਨਾਨਿਸ਼ਾਨੀ)db.bridge_ਗਿਣਤੀ * ਆਕਾਰ(db.bridges[0]));
	}

	mosquitto_FREE(context->bridge->name);
	mosquitto_FREE(context->bridge->local_clientid);
	mosquitto_FREE(context->bridge->local_username);
	mosquitto_FREE(context->bridge->local_password);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	mosquitto_FREE(context->bridge->tls_certfile);
	mosquitto_FREE(context->bridge->tls_keyfile);
#ਜੇ_ਅੰਤ

	ਜੇ(context->bridge->remote_clientid != context->id){
		mosquitto_FREE(context->bridge->remote_clientid);
	}
	context->bridge->remote_clientid = ਨਿਰਾ;

	ਜੇ(context->bridge->remote_username != context->username){
		mosquitto_FREE(context->bridge->remote_username);
	}
	context->bridge->remote_username = ਨਿਰਾ;

	ਜੇ(context->bridge->remote_password != context->password){
		mosquitto_FREE(context->bridge->remote_password);
	}
	context->bridge->remote_password = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	ਜੇ(context->ssl_ctx){
		SSL_CTX_free(context->ssl_ctx);
		context->ssl_ctx = ਨਿਰਾ;
	}
#ਜੇ_ਅੰਤ

	ਲਈ(i=0; i<context->bridge->ਪਤਾ_ਗਿਣਤੀ; i++){
		mosquitto_FREE(context->bridge->ਪਤੇ[i].ਪਤਾ);
	}

	mosquitto_FREE(context->bridge->ਪਤੇ);

	config__bridge_cleanup(context->bridge);
	context->bridge = ਨਿਰਾ;
}


ਸਥਿਰ ਖਾਲੀ bridge__packet_cleanup(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ mosquitto__packet *packet;
	ਜੇ(!context){
		ਵਾਪਸ;
	}

	ਜਦੋਂ(context->ਬਾਹਰਲਾ_packet){
		packet = context->ਬਾਹਰਲਾ_packet;
		context->ਬਾਹਰਲਾ_packet = context->ਬਾਹਰਲਾ_packet->ਅਗਲਾ;
		mosquitto_FREE(packet);
	}
	context->ਬਾਹਰਲਾ_packet = ਨਿਰਾ;
	context->ਬਾਹਰਲਾ_packet_ਆਖਰੀ = ਨਿਰਾ;
	metrics__int_dec(mosq_gauge_ਬਾਹਰਲਾ_packets, context->ਬਾਹਰਲਾ_packet_ਗਿਣਤੀ);
	metrics__int_dec(mosq_gauge_ਬਾਹਰਲਾ_packet_ਬਾਈਟਾਂ, context->ਬਾਹਰਲਾ_packet_ਬਾਈਟਾਂ);
	context->ਬਾਹਰਲਾ_packet_ਗਿਣਤੀ = 0;
	context->ਬਾਹਰਲਾ_packet_ਬਾਈਟਾਂ = 0;

	packet__cleanup(&(context->ਵਿੱਚ_packet));
}


ਸਥਿਰ ਪੂਰਨ rand_between(ਪੂਰਨ ਨੀਵਾਂ, ਪੂਰਨ ਉੱਚਾ)
{
	ਪੂਰਨ r;
	mosquitto_getrandom(&r, ਆਕਾਰ(ਪੂਰਨ));
	ਵਾਪਸ (ਨਿਰਪੇਖ(r) % (ਉੱਚਾ - ਨੀਵਾਂ)) + ਨੀਵਾਂ;
}


ਸਥਿਰ ਖਾਲੀ bridge_backoff_ਕਦਮ(ਬਣਤਰ mosquitto__bridge *bridge)
{
	/*
		“Decorrelated Jitter” calculation, according to:
			https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
	*/

	bridge->restart_timeout = rand_between(bridge->backoff_ਅਧਾਰ, bridge->restart_timeout * 3);
	ਜੇ(bridge->restart_timeout > bridge->backoff_cap){
		bridge->restart_timeout = bridge->backoff_cap;
	}
}


ਸਥਿਰ ਖਾਲੀ bridge__backoff_reset(ਬਣਤਰ mosquitto__bridge *bridge)
{
	bridge->restart_timeout = bridge->backoff_ਅਧਾਰ;
}


ਸਥਿਰ ਖਾਲੀ bridge__update_backoff(ਬਣਤਰ mosquitto__bridge *bridge)
{
	ਜੇ(!bridge){
		ਵਾਪਸ;
	}
	ਜੇ(!bridge->backoff_cap){
		/* skip if not using jitter */
		ਵਾਪਸ;

	}
	ਜੇ(bridge->connected_at && db.now_s - bridge->connected_at >= bridge->stable_connection_period){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Bridge %s connection was stable enough, resetting backoff", bridge->name);
		bridge__backoff_reset(bridge);
	}ਨਹੀਂਤਾਂ{
		bridge_backoff_ਕਦਮ(bridge);
	}

	bridge->connected_at = 0;

	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Bridge %s next backoff will be %d ms", bridge->name, bridge->restart_timeout);
}


ਸਥਿਰ ਖਾਲੀ bridge_check_pending(ਬਣਤਰ mosquitto *context)
{
	ਪੂਰਨ err;
	socklen_t len;

	ਜੇ(context->state == mosq_cs_connect_pending){
		len = ਆਕਾਰ(ਪੂਰਨ);
		ਜੇ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (ਅੱਖਰ *)&err, &len)){
			ਜੇ(err == 0){
				mosquitto_ਸੈੱਟ_state(context, mosq_cs_new);
#ਜੇ defined(WITH_ADNS) && defined(WITH_BRIDGE)
				ਜੇ(context->bridge){
					bridge__connect_step3(context);
				}
#ਜੇ_ਅੰਤ
			}ਨਹੀਂਤਾਂ ਜੇ(err == ECONNREFUSED){
				do_disconnect(context, MOSQ_ERR_CONN_ਗੁਆਚਾ);
				ਵਾਪਸ;
			}
		}ਨਹੀਂਤਾਂ{
			do_disconnect(context, MOSQ_ERR_CONN_ਗੁਆਚਾ);
			ਵਾਪਸ;
		}
	}
}


ਸਥਿਰ bool reload_if_needed(ਬਣਤਰ mosquitto *context)
{
	ਪੂਰਨ i;

	ਲਈ(i=0; i<db.config->bridge_ਗਿਣਤੀ; i++){
		ਜੇ(db.config->bridges[i] && !strcmp(context->bridge->name, db.config->bridges[i]->name)){
			bridge__destroy(context);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ਨਿਰਾ;
			loop_update_ਅਗਲਾ_event(100);
			ਵਾਪਸ true;
		}
	}

	ਵਾਪਸ false;
}


ਖਾਲੀ bridge_check(ਖਾਲੀ)
{
	ਸਥਿਰ time_t ਆਖਰੀ_check = 0;
	ਬਣਤਰ mosquitto *context = ਨਿਰਾ;
	socklen_t len;
	ਪੂਰਨ i;
	ਪੂਰਨ rc;
	ਪੂਰਨ err;

	ਜੇ(db.now_s <= ਆਖਰੀ_check){
		ਵਾਪਸ;
	}

	ਲਈ(i=0; i<db.bridge_ਗਿਣਤੀ; i++){
		ਜੇ(!db.bridges[i]){
			ਜਾਰੀ;
		}

		context = db.bridges[i];

		ਜੇ(net__is_connected(context)){
			mosquitto__check_keepalive(context);
			bridge_check_pending(context);

			/* Check for bridges that are not round robin and not currently
			 * connected to their primary broker. */
			ਜੇ(context->bridge->ਗੇੜਾ_robin == false
					&& context->bridge->cur_ਪਤਾ != 0
					&& context->bridge->primary_retry
					&& db.now_s >= context->bridge->primary_retry){

				ਜੇ(context->bridge->primary_retry_sock == INVALID_SOCKET){
					rc = net__try_connect(context->bridge->ਪਤੇ[0].ਪਤਾ,
							context->bridge->ਪਤੇ[0].ਪੋਰਟ,
							&context->bridge->primary_retry_sock,
							context->bridge->bind_ਪਤਾ, false);

					ਜੇ(rc == 0){
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = 0;
						mux__delete(context);
						net__socket_close(context);
						context->bridge->cur_ਪਤਾ = 0;
					}
				}ਨਹੀਂਤਾਂ{
					len = ਆਕਾਰ(ਪੂਰਨ);
					ਜੇ(!getsockopt(context->bridge->primary_retry_sock, SOL_SOCKET, SO_ERROR, (ਅੱਖਰ *)&err, &len)){
						ਜੇ(err == 0){
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = 0;
							mux__delete(context);
							net__socket_close(context);
							context->bridge->cur_ਪਤਾ = context->bridge->ਪਤਾ_ਗਿਣਤੀ-1;
						}ਨਹੀਂਤਾਂ{
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = db.now_s+5;
							loop_update_ਅਗਲਾ_event(5000);
						}
					}ਨਹੀਂਤਾਂ{
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = db.now_s+5;
						loop_update_ਅਗਲਾ_event(5000);
					}
				}
			}
		}ਨਹੀਂਤਾਂ{
			ਜੇ(reload_if_needed(context)){
				ਜਾਰੀ;
			}

			/* Want to try to restart the bridge connection */
			ਜੇ(!context->bridge->restart_t){
				bridge__update_backoff(context->bridge);
				context->bridge->restart_t = 1000*db.now_s+context->bridge->restart_timeout;
				context->bridge->cur_ਪਤਾ++;
				ਜੇ(context->bridge->cur_ਪਤਾ == context->bridge->ਪਤਾ_ਗਿਣਤੀ){
					context->bridge->cur_ਪਤਾ = 0;
				}
				loop_update_ਅਗਲਾ_event(context->bridge->restart_timeout);
			}ਨਹੀਂਤਾਂ{
				ਜੇ((context->bridge->ਸ਼ੁਰੂ_type == bst_lazy && context->bridge->lazy_reconnect)
						|| (context->bridge->ਸ਼ੁਰੂ_type == bst_automatic && 1000*db.now_s >= context->bridge->restart_t)){

#ਜੇ defined(__GLIBC__) && defined(WITH_ADNS)
					ਜੇ(context->adns){
						/* Connection attempted, waiting on DNS lookup */
						rc = gai_error(context->adns);
						ਜੇ(rc == EAI_INPROGRESS){
							/* Just keep on waiting */
						}ਨਹੀਂਤਾਂ ਜੇ(rc == 0){
							rc = bridge__connect_step2(context);
							ਜੇ(rc == MOSQ_ERR_SUCCESS){
								mux__new(context);
								ਜੇ(context->ਬਾਹਰਲਾ_packet){
									mux_ਜੋੜੋ_ਬਾਹਰਲਾ(context);
								}
							}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_CONN_PENDING){
								mux__new(context);
								mux_ਜੋੜੋ_ਬਾਹਰਲਾ(context);
								context->bridge->restart_t = 0;
							}ਨਹੀਂਤਾਂ{
								context->bridge->cur_ਪਤਾ++;
								ਜੇ(context->bridge->cur_ਪਤਾ == context->bridge->ਪਤਾ_ਗਿਣਤੀ){
									context->bridge->cur_ਪਤਾ = 0;
								}
								context->bridge->restart_t = 0;
							}
						}ਨਹੀਂਤਾਂ{
							/* Need to retry */
							ਜੇ(context->adns->ar_ਨਤੀਜਾ){
								freeaddrinfo(context->adns->ar_ਨਤੀਜਾ);
							}
							mosquitto_FREE(context->adns);
							context->bridge->restart_t = 0;
						}
					}ਨਹੀਂਤਾਂ{
						rc = bridge__connect_step1(context);
						ਜੇ(rc){
							context->bridge->cur_ਪਤਾ++;
							ਜੇ(context->bridge->cur_ਪਤਾ == context->bridge->ਪਤਾ_ਗਿਣਤੀ){
								context->bridge->cur_ਪਤਾ = 0;
							}
						}ਨਹੀਂਤਾਂ{
							/* Short wait for ADNS lookup */
							context->bridge->restart_t = 1;
							loop_update_ਅਗਲਾ_event(1000);
						}
					}
#ਨਹੀਂਤਾਂ
					{
						rc = bridge__connect(context);
						context->bridge->restart_t = 0;
						ਜੇ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING){
							ਜੇ(context->bridge->ਗੇੜਾ_robin == false && context->bridge->cur_ਪਤਾ != 0){
								context->bridge->primary_retry = db.now_s + 5;
								loop_update_ਅਗਲਾ_event(5000);
							}
							mux__new(context);
							ਜੇ(context->ਬਾਹਰਲਾ_packet){
								mux_ਜੋੜੋ_ਬਾਹਰਲਾ(context);
							}
						}ਨਹੀਂਤਾਂ{
							context->bridge->cur_ਪਤਾ++;
							ਜੇ(context->bridge->cur_ਪਤਾ == context->bridge->ਪਤਾ_ਗਿਣਤੀ){
								context->bridge->cur_ਪਤਾ = 0;
							}
						}
					}
#ਜੇ_ਅੰਤ
				}
			}
		}
	}
}

#ਜੇ_ਅੰਤ
