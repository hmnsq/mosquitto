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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಅನ್ತರ್ಭಾವ <sys/socket.h>
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಅನ್ತರ್ಭಾವ <netinet/tcp.h>
#ಇಲ್ಲದಿದ್ದರೆ
#ಅನ್ತರ್ಭಾವ <winsock2.h>
#ಅನ್ತರ್ಭಾವ <ws2tcpip.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಇಲ್ಲದಿದ್ದರೆ
#ಅನ್ತರ್ಭಾವ <process.h>
#ಅನ್ತರ್ಭಾವ <winsock2.h>
#ಅನ್ತರ್ಭಾವ <ws2tcpip.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "mosquitto.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_internal.h"
#ಅನ್ತರ್ಭಾವ "net_mosq.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "property_common.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"
#ಅನ್ತರ್ಭಾವ "tls_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"
#ಅನ್ತರ್ಭಾವ "will_mosq.h"
#ಅನ್ತರ್ಭಾವ "utlist.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE

ಸ್ಥಿರ ಶೂನ್ಯ bridge__update_backoff(ರಚನೆ mosquitto__bridge *bridge);
#ಆದರೆ defined(__GLIBC__) && defined(WITH_ADNS)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ bridge__connect_step1(ರಚನೆ mosquitto *context);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ bridge__connect_step2(ರಚನೆ mosquitto *context);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ bridge__packet_cleanup(ರಚನೆ mosquitto *context);

ಸ್ಥಿರ ರಚನೆ mosquitto *bridge__new(ರಚನೆ mosquitto__bridge *bridge)
{
	ರಚನೆ mosquitto *new_context = ಖಾಲಿ;
	ರಚನೆ mosquitto **bridges;
	ಅಕ್ಷರ *local_id;

	ಪ್ರತಿಜ್ಞಾ(bridge);

	local_id = mosquitto_strdup(bridge->local_clientid);
	ಆದರೆ(!local_id){
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id, local_id, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(local_id), new_context);
	ಆದರೆ(new_context){
		/* (possible from persistent db) */
		mosquitto_FREE(local_id);
	}ಇಲ್ಲದಿದ್ದರೆ{
		/* id wasn't found, so generate a new context */
		new_context = context__init();
		ಆದರೆ(!new_context){
			mosquitto_FREE(local_id);
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		new_context->id = local_id;
		context_ಕೂಡಿಸು_ಗಮ್ಯ_by_id(new_context);
	}
	new_context->transport = mosq_t_tcp;
	new_context->bridge = bridge;
	new_context->is_bridge = true;

	new_context->username = bridge->remote_username;
	new_context->password = bridge->remote_password;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	new_context->tls_cafile = bridge->tls_cafile;
	new_context->tls_capath = bridge->tls_capath;
	new_context->tls_certfile = bridge->tls_certfile;
	new_context->tls_keyfile = bridge->tls_keyfile;
	new_context->tls_cert_reqs = SSL_VERIFY_PEER;
	new_context->tls_ocsp_required = bridge->tls_ocsp_required;
	new_context->tls_ಆವೃತ್ತಿ = bridge->tls_ಆವೃತ್ತಿ;
	new_context->tls_insecure = bridge->tls_insecure;
	new_context->tls_alpn = bridge->tls_alpn;
	new_context->tls_ciphers = bridge->tls_ciphers;
	new_context->tls_13_ciphers = bridge->tls_13_ciphers;
	new_context->tls_engine = ಖಾಲಿ;
	new_context->tls_keyform = mosq_k_pem;
	new_context->tls_use_os_certs = bridge->tls_use_os_certs;
	new_context->ssl_ctx_defaults = true;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FINAL_WITH_TLS_PSK
	new_context->tls_psk_ತಟಸ್ಥ = bridge->tls_psk_ತಟಸ್ಥ;
	new_context->tls_psk = bridge->tls_psk;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

	bridge->try_ಖಾಸಗಿ_accepted = true;
	ಆದರೆ(bridge->clean_ಆರಂಭ_local == -1){
		/* default to "regular" clean start setting */
		bridge->clean_ಆರಂಭ_local = bridge->clean_ಆರಂಭ;
	}
	new_context->retain_ಲಭ್ಯ = bridge->outgoing_retain;
	new_context->protocol = bridge->protocol_ಆವೃತ್ತಿ;
	ಆದರೆ(!bridge->clean_ಆರಂಭ_local){
		new_context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
		plugin_persist_handle_client_ಕೂಡಿಸು(new_context);
		ಆದರೆ(new_context->expiry_list_ಅಂಶ){
			/* We've restored from persistence and been added to the session
			 * expiry list, even though we should never be expired */
			session_expiry__remove(new_context);
		}
	}

	bridges = mosquitto_realloc(db.bridges, (size_t)(db.bridge_ಎಣಿಕೆ+1)*ಗಾತ್ರ(ರಚನೆ mosquitto *));
	ಆದರೆ(bridges){
		db.bridges = bridges;
		db.bridge_ಎಣಿಕೆ++;
		db.bridges[db.bridge_ಎಣಿಕೆ-1] = new_context;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ಹಿಂತಿರುಗಿ new_context;
}


ಸ್ಥಿರ ಶೂನ್ಯ bridge__destroy(ರಚನೆ mosquitto *context)
{
	send__disconnect(context, MQTT_RC_SUCCESS, ಖಾಲಿ);
	context__cleanup(context, true);
}


ಶೂನ್ಯ bridge_ಆರಂಭ_all(ಶೂನ್ಯ)
{
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.config->bridge_ಎಣಿಕೆ; i++){
		ರಚನೆ mosquitto *context;
		ಪೂರ್ಣಾಂಕ ret;

		context = bridge__new(db.config->bridges[i]);
		ಆದರೆ(!context){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
			ಹಿಂತಿರುಗಿ;
		}
		ಪ್ರತಿಜ್ಞಾ(context);

#ಆದರೆ defined(__GLIBC__) && defined(WITH_ADNS)
		context->bridge->restart_t = 1; /* force quick restart of bridge */
		loop_update_ಮುಂದಿನ_event(1000);
		ret = bridge__connect_step1(context);
#ಇಲ್ಲದಿದ್ದರೆ
		ret = bridge__connect(context);
#ಆದರೆ_ಅಂತ್ಯ

		ಆದರೆ(ret && ret != MOSQ_ERR_CONN_PENDING){
			log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Unable to connect bridge %s.",
					context->bridge->name);
		}

		db.config->bridges[i] = ಖಾಲಿ;
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ bridge_ಗಣ_tcp_keepalive(ರಚನೆ mosquitto *context)
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ enabled = 1;
	bool ret;

	ಆದರೆ(context->bridge->tcp_keepalive_idle == 0
			|| context->bridge->tcp_keepalive_interval == 0
			|| context->bridge->tcp_keepalive_counter == 0){

		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
#  define SETSOCKOPT_TYPE ಅಕ್ಷರ *
#ಇಲ್ಲದಿದ್ದರೆ
#  define SETSOCKOPT_TYPE ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *
#ಆದರೆ_ಅಂತ್ಯ

	ret = setsockopt(context->sock, SOL_SOCKET, SO_KEEPALIVE, (SETSOCKOPT_TYPE)&enabled, ಗಾತ್ರ(enabled));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TCP_KEEPIDLE
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPIDLE,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_idle, ಗಾತ್ರ(context->bridge->tcp_keepalive_idle));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TCP_KEEPINTVL
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPINTVL,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_interval, ಗಾತ್ರ(context->bridge->tcp_keepalive_interval));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TCP_KEEPCNT
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPCNT,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_counter, ಗಾತ್ರ(context->bridge->tcp_keepalive_counter));
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು SETSOCKOPT_TYPE

	ಆದರೆ(ret){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TCP_USER_TIMEOUT


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ bridge_ಗಣ_tcp_user_timeout(ರಚನೆ mosquitto *context)
{
	ಪೂರ್ಣಾಂಕ timeout = context->bridge->tcp_user_timeout;
	ಆದರೆ(timeout >= 0){
		ಆದರೆ(setsockopt(context->sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (ಅಕ್ಷರ *)&timeout, ಗಾತ್ರ(timeout))){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(__GLIBC__) && defined(WITH_ADNS)


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ bridge__connect_step1(ರಚನೆ mosquitto *context)
{
	ಪೂರ್ಣಾಂಕ rc;
	ಅಕ್ಷರ *notification_topic;
	size_t notification_topic_len;
	uint8_t notification_payload;
	ರಚನೆ mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	ಆದರೆ(!context || !context->bridge){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	mosquitto_ಗಣ_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->ಕೊನೆಯ_ಸಂದೇಶ_ಒಳ = db.now_s;
	context->ಮುಂದಿನ_ಸಂದೇಶ_ಹೊರಹೋಗು = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_ಆರಂಭ = context->bridge->clean_ಆರಂಭ;
	context->ಒಳ_packet.payload = ಖಾಲಿ;
	context->ping_t = 0;
	context->bridge->lazy_reconnect = false;
	context->ಗರಿಷ್ಠ_packet_ಗಾತ್ರ = context->bridge->ಗರಿಷ್ಠ_packet_ಗಾತ್ರ;
	bridge__packet_cleanup(context);
	db_ಸಂದೇಶ_reconnect_reset(context);

	db__messages_delete(context, false);

	/* Delete all local subscriptions even for clean_start==false. We don't
	 * remove any messages and the next loop carries out the resubscription
	 * anyway. This means any unwanted subs will be removed.
	 */
	sub__clean_session(context);

	LL_FOREACH(context->bridge->topics, cur_topic){
		ಆದರೆ(cur_topic->direction == bd_ಹೊರಹೋಗು || cur_topic->direction == bd_both){
			log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			ಆದರೆ(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}ಇಲ್ಲದಿದ್ದರೆ{
				qos = cur_topic->qos;
			}
			ರಚನೆ mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			ಆದರೆ(sub_ಕೂಡಿಸು(context, &sub) > 0){
				ಹಿಂತಿರುಗಿ 1;
			}
			retain_ಸರತಿ(context, &sub);
		}
	}

	ಆದರೆ(context->bridge->notifications){
		ಆದರೆ(context->max_qos == 0){
			qos = 0;
		}ಇಲ್ಲದಿದ್ದರೆ{
			qos = 1;
		}
		ಆದರೆ(context->bridge->notification_topic){
			ಆದರೆ(!context->bridge->initial_notification_ಮುಗಿದ){
				notification_payload = '0';
				db_messages_easy_ಸರತಿ(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, ಸಂದೇಶ_EXPIRY_INFINITE, ಖಾಲಿ);
				context->bridge->initial_notification_ಮುಗಿದ = true;
			}
			notification_payload = '0';
			rc = will_ಗಣ(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, ಖಾಲಿ);
			ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
				ಹಿಂತಿರುಗಿ rc;
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			notification_topic_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->bridge->remote_clientid)+ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(ಗಾತ್ರ(ಅಕ್ಷರ)*(notification_topic_len+1));
			ಆದರೆ(!notification_topic){
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			ಆದರೆ(!context->bridge->initial_notification_ಮುಗಿದ){
				notification_payload = '0';
				db_messages_easy_ಸರತಿ(context, notification_topic, qos, 1, &notification_payload, 1, ಸಂದೇಶ_EXPIRY_INFINITE, ಖಾಲಿ);
				context->bridge->initial_notification_ಮುಗಿದ = true;
			}

			notification_payload = '0';
			rc = will_ಗಣ(context, notification_topic, 1, &notification_payload, qos, true, ಖಾಲಿ);
			mosquitto_FREE(notification_topic);
			ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
				ಹಿಂತಿರುಗಿ rc;
			}
		}
	}

	log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Connecting bridge (step 1) %s (%s:%d)", context->bridge->name, context->bridge->ವಿಳಾಸಗಳು[context->bridge->cur_ವಿಳಾಸ].ವಿಳಾಸ, context->bridge->ವಿಳಾಸಗಳು[context->bridge->cur_ವಿಳಾಸ].ಪೋರ್ಟ್);
	rc = net__try_connect_step1(context, context->bridge->ವಿಳಾಸಗಳು[context->bridge->cur_ವಿಳಾಸ].ವಿಳಾಸ);
	ಆದರೆ(rc > 0){
		ಆದರೆ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ಹಿಂತಿರುಗಿ rc; /* Error already printed */
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_ERRNO){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_EAI){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ bridge__connect_step2(ರಚನೆ mosquitto *context)
{
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(!context || !context->bridge){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Connecting bridge (step 2) %s (%s:%d)", context->bridge->name, context->bridge->ವಿಳಾಸಗಳು[context->bridge->cur_ವಿಳಾಸ].ವಿಳಾಸ, context->bridge->ವಿಳಾಸಗಳು[context->bridge->cur_ವಿಳಾಸ].ಪೋರ್ಟ್);
	rc = net__try_connect_step2(context, context->bridge->ವಿಳಾಸಗಳು[context->bridge->cur_ವಿಳಾಸ].ಪೋರ್ಟ್, &context->sock);
	ಆದರೆ(rc > 0){
		ಆದರೆ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ಹಿಂತಿರುಗಿ rc; /* Error already printed */
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_ERRNO){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_EAI){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ಹಿಂತಿರುಗಿ rc;
	}

	HASH_ಕೂಡಿಸು(hh_sock, db.contexts_by_sock, sock, ಗಾತ್ರ(context->sock), context);

	ಆದರೆ(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto_ಗಣ_state(context, mosq_cs_connect_pending);
		mux_ಕೂಡಿಸು_ಹೊರಹೋಗು(context);
	}
	ಹಿಂತಿರುಗಿ rc;
}


ಪೂರ್ಣಾಂಕ bridge__connect_step3(ರಚನೆ mosquitto *context)
{
	ಪೂರ್ಣಾಂಕ rc;
	mosquitto_property receive_ಗರಿಷ್ಠ;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = ಖಾಲಿ;

	rc = net__socket_connect_step3(context, context->bridge->ವಿಳಾಸಗಳು[context->bridge->cur_ವಿಳಾಸ].ವಿಳಾಸ);
	ಆದರೆ(rc > 0){
		ಆದರೆ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ಹಿಂತಿರುಗಿ rc; /* Error already printed */
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_ERRNO){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_EAI){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ಹಿಂತಿರುಗಿ rc;
	}

	ಆದರೆ(context->bridge->ಸುತ್ತು_robin == false && context->bridge->cur_ವಿಳಾಸ != 0){
		context->bridge->primary_retry = db.now_s + 5;
		loop_update_ಮುಂದಿನ_event(5000);
	}

	ಆದರೆ(bridge_ಗಣ_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TCP_USER_TIMEOUT
	ಆದರೆ(bridge_ಗಣ_tcp_user_timeout(context)){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ(context->bridge->receive_ಗರಿಷ್ಠ != 0){
		receive_ಗರಿಷ್ಠ.ಮೌಲ್ಯ.i16 = context->bridge->receive_ಗರಿಷ್ಠ;
		receive_ಗರಿಷ್ಠ.identifier = MQTT_PROP_RECEIVE_ಗರಿಷ್ಠ;
		receive_ಗರಿಷ್ಠ.property_type = MQTT_PROP_TYPE_INT16;
		receive_ಗರಿಷ್ಠ.client_generated = false;
		receive_ಗರಿಷ್ಠ.ಮುಂದಿನ = properties;
		properties = &receive_ಗರಿಷ್ಠ;
	}
	ಆದರೆ(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_ತಕ್ಷಣ){
		session_expiry_interval.ಮೌಲ್ಯ.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.ಮುಂದಿನ = properties;
		properties = &session_expiry_interval;
	}
	ಆದರೆ(context->bridge->max_topic_alias != 0){
		topic_alias_max.ಮೌಲ್ಯ.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_ಗರಿಷ್ಠ;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.ಮುಂದಿನ = properties;
		properties = &topic_alias_max;
	}

	rc = send__connect(context, context->keepalive, context->clean_ಆರಂಭ, properties);
	ಆದರೆ(rc == MOSQ_ERR_SUCCESS){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಆದರೆ(rc == MOSQ_ERR_TLS){
			ಹಿಂತಿರುಗಿ rc; /* Error already printed */
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_ERRNO){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_EAI){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		ಹಿಂತಿರುಗಿ rc;
	}
}
#ಇಲ್ಲದಿದ್ದರೆ


ಪೂರ್ಣಾಂಕ bridge__connect(ರಚನೆ mosquitto *context)
{
	ಪೂರ್ಣಾಂಕ rc, rc2;
	ಅಕ್ಷರ *notification_topic = ಖಾಲಿ;
	size_t notification_topic_len;
	uint8_t notification_payload;
	ರಚನೆ mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	mosquitto_property receive_ಗರಿಷ್ಠ;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = ಖಾಲಿ;

	ಆದರೆ(!context || !context->bridge){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	mosquitto_ಗಣ_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->ಕೊನೆಯ_ಸಂದೇಶ_ಒಳ = db.now_s;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->ಮುಂದಿನ_ಸಂದೇಶ_ಹೊರಹೋಗು = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_ಆರಂಭ = context->bridge->clean_ಆರಂಭ;
	context->ಒಳ_packet.payload = ಖಾಲಿ;
	context->ping_t = 0;
	context->bridge->lazy_reconnect = false;
	context->ಗರಿಷ್ಠ_packet_ಗಾತ್ರ = context->bridge->ಗರಿಷ್ಠ_packet_ಗಾತ್ರ;
	bridge__packet_cleanup(context);
	db_ಸಂದೇಶ_reconnect_reset(context);

	db__messages_delete(context, false);

	/* Delete all local subscriptions even for clean_start==false. We don't
	 * remove any messages and the next loop carries out the resubscription
	 * anyway. This means any unwanted subs will be removed.
	 */
	sub__clean_session(context);

	LL_FOREACH(context->bridge->topics, cur_topic){
		ಆದರೆ(cur_topic->direction == bd_ಹೊರಹೋಗು || cur_topic->direction == bd_both){
			log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			ಆದರೆ(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}ಇಲ್ಲದಿದ್ದರೆ{
				qos = cur_topic->qos;
			}
			ರಚನೆ mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			ಆದರೆ(sub_ಕೂಡಿಸು(context, &sub) > 0){
				ಹಿಂತಿರುಗಿ 1;
			}
		}
	}

	ಆದರೆ(context->bridge->notifications){
		ಆದರೆ(context->max_qos == 0){
			qos = 0;
		}ಇಲ್ಲದಿದ್ದರೆ{
			qos = 1;
		}
		ಆದರೆ(context->bridge->notification_topic){
			ಆದರೆ(!context->bridge->initial_notification_ಮುಗಿದ){
				notification_payload = '0';
				db_messages_easy_ಸರತಿ(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, ಸಂದೇಶ_EXPIRY_INFINITE, ಖಾಲಿ);
				context->bridge->initial_notification_ಮುಗಿದ = true;
			}

			notification_payload = '0';
			rc = will_ಗಣ(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, ಖಾಲಿ);
			ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
				ಹಿಂತಿರುಗಿ rc;
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			notification_topic_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->bridge->remote_clientid)+ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(ಗಾತ್ರ(ಅಕ್ಷರ)*(notification_topic_len+1));
			ಆದರೆ(!notification_topic){
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			ಆದರೆ(!context->bridge->initial_notification_ಮುಗಿದ){
				notification_payload = '0';
				db_messages_easy_ಸರತಿ(context, notification_topic, qos, 1, &notification_payload, 1, ಸಂದೇಶ_EXPIRY_INFINITE, ಖಾಲಿ);
				context->bridge->initial_notification_ಮುಗಿದ = true;
			}

			notification_payload = '0';
			rc = will_ಗಣ(context, notification_topic, 1, &notification_payload, qos, true, ಖಾಲಿ);
			ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
				mosquitto_FREE(notification_topic);
				ಹಿಂತಿರುಗಿ rc;
			}
			mosquitto_FREE(notification_topic);
		}
	}

	log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Connecting bridge %s (%s:%d)", context->bridge->name, context->bridge->ವಿಳಾಸಗಳು[context->bridge->cur_ವಿಳಾಸ].ವಿಳಾಸ, context->bridge->ವಿಳಾಸಗಳು[context->bridge->cur_ವಿಳಾಸ].ಪೋರ್ಟ್);
	rc = net__socket_connect(context,
			context->bridge->ವಿಳಾಸಗಳು[context->bridge->cur_ವಿಳಾಸ].ವಿಳಾಸ,
			context->bridge->ವಿಳಾಸಗಳು[context->bridge->cur_ವಿಳಾಸ].ಪೋರ್ಟ್,
			context->bridge->bind_ವಿಳಾಸ,
			false);

	ಆದರೆ(rc > 0){
		ಆದರೆ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ಹಿಂತಿರುಗಿ rc; /* Error already printed */
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_ERRNO){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_EAI){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ಹಿಂತಿರುಗಿ rc;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto_ಗಣ_state(context, mosq_cs_connect_pending);
		mux_ಕೂಡಿಸು_ಹೊರಹೋಗು(context);
	}

	HASH_ಕೂಡಿಸು(hh_sock, db.contexts_by_sock, sock, ಗಾತ್ರ(context->sock), context);

	ಆದರೆ(bridge_ಗಣ_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TCP_USER_TIMEOUT
	ಆದರೆ(bridge_ಗಣ_tcp_user_timeout(context)){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ(context->bridge->receive_ಗರಿಷ್ಠ != 0){
		receive_ಗರಿಷ್ಠ.ಮೌಲ್ಯ.i16 = context->bridge->receive_ಗರಿಷ್ಠ;
		receive_ಗರಿಷ್ಠ.identifier = MQTT_PROP_RECEIVE_ಗರಿಷ್ಠ;
		receive_ಗರಿಷ್ಠ.property_type = MQTT_PROP_TYPE_INT16;
		receive_ಗರಿಷ್ಠ.client_generated = false;
		receive_ಗರಿಷ್ಠ.ಮುಂದಿನ = properties;
		properties = &receive_ಗರಿಷ್ಠ;
	}
	ಆದರೆ(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_ತಕ್ಷಣ){
		session_expiry_interval.ಮೌಲ್ಯ.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.ಮುಂದಿನ = properties;
		properties = &session_expiry_interval;
	}
	ಆದರೆ(context->bridge->max_topic_alias != 0){
		topic_alias_max.ಮೌಲ್ಯ.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_ಗರಿಷ್ಠ;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.ಮುಂದಿನ = properties;
		properties = &topic_alias_max;
	}

	rc2 = send__connect(context, context->keepalive, context->clean_ಆರಂಭ, properties);
	ಆದರೆ(rc2 == MOSQ_ERR_SUCCESS){
		ಹಿಂತಿರುಗಿ rc;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc2 == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಆದರೆ(rc2 == MOSQ_ERR_TLS){
			ಹಿಂತಿರುಗಿ rc2; /* Error already printed */
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc2 == MOSQ_ERR_ERRNO){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc2 == MOSQ_ERR_EAI){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		ಹಿಂತಿರುಗಿ rc2;
	}
}
#ಆದರೆ_ಅಂತ್ಯ


ಪೂರ್ಣಾಂಕ bridge__on_connect(ರಚನೆ mosquitto *context)
{
	ಅಕ್ಷರ *notification_topic;
	size_t notification_topic_len;
	ರಚನೆ mosquitto__bridge_topic *cur_topic;
	ಪೂರ್ಣಾಂಕ sub_opts;
	bool retain = true;
	uint8_t qos;

	ಆದರೆ(context->bridge->notifications){
		ಆದರೆ(context->max_qos == 0){
			qos = 0;
		}ಇಲ್ಲದಿದ್ದರೆ{
			qos = 1;
		}
		ಆದರೆ(!context->retain_ಲಭ್ಯ){
			retain = false;
		}
		ಅಕ್ಷರ notification_payload = '1';
		ಆದರೆ(context->bridge->notification_topic){
			ಆದರೆ(!context->bridge->notifications_local_only){
				ಆದರೆ(send__real_publish(context, mosquitto__mid_generate(context),
						context->bridge->notification_topic, 1, &notification_payload, qos, retain, 0, 0, ಖಾಲಿ, 0)){

					ಹಿಂತಿರುಗಿ 1;
				}
			}
			db_messages_easy_ಸರತಿ(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, ಸಂದೇಶ_EXPIRY_INFINITE, ಖಾಲಿ);
		}ಇಲ್ಲದಿದ್ದರೆ{
			notification_topic_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->bridge->remote_clientid)+ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(ಗಾತ್ರ(ಅಕ್ಷರ)*(notification_topic_len+1));
			ಆದರೆ(!notification_topic){
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);
			notification_payload = '1';
			ಆದರೆ(!context->bridge->notifications_local_only){
				ಆದರೆ(send__real_publish(context, mosquitto__mid_generate(context),
						notification_topic, 1, &notification_payload, qos, retain, 0, 0, ಖಾಲಿ, 0)){

					mosquitto_FREE(notification_topic);
					ಹಿಂತಿರುಗಿ 1;
				}
			}
			db_messages_easy_ಸರತಿ(context, notification_topic, qos, 1, &notification_payload, 1, ಸಂದೇಶ_EXPIRY_INFINITE, ಖಾಲಿ);
			mosquitto_FREE(notification_topic);
		}
	}

	LL_FOREACH(context->bridge->topics, cur_topic){
		ಆದರೆ(cur_topic->direction == bd_ಒಳ || cur_topic->direction == bd_both){
			ಆದರೆ(cur_topic->qos > context->max_qos){
				sub_opts = context->max_qos;
			}ಇಲ್ಲದಿದ್ದರೆ{
				sub_opts = cur_topic->qos;
			}
			ಆದರೆ(context->bridge->protocol_ಆವೃತ್ತಿ == mosq_p_mqtt5){
				sub_opts = sub_opts
						| MQTT_SUB_OPT_NO_LOCAL
						| MQTT_SUB_OPT_RETAIN_AS_PUBLISHED
						| MQTT_SUB_OPT_SEND_RETAIN_ALWAYS;
			}
			ಆದರೆ(send__subscribe(context, ಖಾಲಿ, 1, &cur_topic->remote_topic, sub_opts, ಖಾಲಿ)){
				ಹಿಂತಿರುಗಿ 1;
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಆದರೆ(context->bridge->attempt_unsubscribe){
				ಆದರೆ(send__unsubscribe(context, ಖಾಲಿ, 1, &cur_topic->remote_topic, ಖಾಲಿ)){
					/* direction = inwards only. This means we should not be subscribed
					* to the topic. It is possible that we used to be subscribed to
					* this topic so unsubscribe. */
					ಹಿಂತಿರುಗಿ 1;
				}
			}
		}
	}
	ರಚನೆ mosquitto_subscription sub;
	memset(&sub, 0, ಗಾತ್ರ(sub));
	LL_FOREACH(context->bridge->topics, cur_topic){
		sub.topic_filter = cur_topic->local_topic;
		ಆದರೆ(cur_topic->direction == bd_ಹೊರಹೋಗು || cur_topic->direction == bd_both){
			ಆದರೆ(cur_topic->qos > context->max_qos){
				sub.options = context->max_qos;
			}ಇಲ್ಲದಿದ್ದರೆ{
				sub.options = cur_topic->qos;
			}
			retain_ಸರತಿ(context, &sub);
		}
	}

	context->bridge->connected_at = db.now_s;

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ bridge_ರಿಜಿಸ್ಟರ್_local_connections(ಶೂನ್ಯ)
{
	ರಚನೆ mosquitto *context, *ctxt_tmp = ಖಾಲಿ;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ಆದರೆ(context->bridge){
			ಆದರೆ(mux__new(context)){
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error in initial bridge registration: %s", strerror(errno));
				ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
			}
			mux_ಕೂಡಿಸು_ಹೊರಹೋಗು(context);
		}
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಶೂನ್ಯ bridge__reload(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ i;
	ಪೂರ್ಣಾಂಕ j;

	// destroy old bridges that dissappeared
	ಗಾಗಿ(i=0; i<db.bridge_ಎಣಿಕೆ; i++){
		ಗಾಗಿ(j=0; j<db.config->bridge_ಎಣಿಕೆ; j++){
			ಆದರೆ(!strcmp(db.bridges[i]->bridge->name, db.config->bridges[j]->name)){
				ನಿಲ್ಲಿಸು;
			}
		}

		ಆದರೆ(j==db.config->bridge_ಎಣಿಕೆ){
			bridge__destroy(db.bridges[i]);
		}
	}

	ಗಾಗಿ(i=0; i<db.config->bridge_ಎಣಿಕೆ; i++){
		ಗಾಗಿ(j=0; j<db.bridge_ಎಣಿಕೆ; j++){
			ಆದರೆ(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
				ನಿಲ್ಲಿಸು;
			}
		}

		ಆದರೆ(j==db.bridge_ಎಣಿಕೆ){
			// a new bridge was found, create it
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ಖಾಲಿ;
			ಮುಂದುವರಿ;
		}

		ಆದರೆ(db.config->bridges[i]->reload_type == brt_ತಕ್ಷಣ){
			// in this case, an existing bridge should match
			ಗಾಗಿ(j=0; j<db.bridge_ಎಣಿಕೆ; j++){
				ಆದರೆ(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
					ನಿಲ್ಲಿಸು;
				}
			}

			ಪ್ರತಿಜ್ಞಾ(j<db.bridge_ಎಣಿಕೆ);
			db.bridges[j]->will_delay_interval = 0;
			bridge__destroy(db.bridges[j]);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ಖಾಲಿ;
		}
	}
}


ಶೂನ್ಯ bridge__db_cleanup(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ i;

	ಗಾಗಿ(i=0; i<db.bridge_ಎಣಿಕೆ; i++){
		ಆದರೆ(db.bridges[i]){
			context__cleanup(db.bridges[i], true);
		}
	}
	mosquitto_FREE(db.bridges);
}


ಶೂನ್ಯ bridge__cleanup(ರಚನೆ mosquitto *context)
{
	ಪೂರ್ಣಾಂಕ i;

	ಪ್ರತಿಜ್ಞಾ(db.bridge_ಎಣಿಕೆ > 0);

	ಗಾಗಿ(i=0; i<db.bridge_ಎಣಿಕೆ; i++){
		ಆದರೆ(db.bridges[i] == context){
			db.bridges[i] = db.bridges[db.bridge_ಎಣಿಕೆ-1];
			ನಿಲ್ಲಿಸು;
		}
	}

	db.bridge_ಎಣಿಕೆ--;
	ಆದರೆ(db.bridge_ಎಣಿಕೆ == 0){
		mosquitto_FREE(db.bridges);
	}ಇಲ್ಲದಿದ್ದರೆ{
		db.bridges = mosquitto_realloc(db.bridges, (ಚಿಹ್ನೆರಹಿತ)db.bridge_ಎಣಿಕೆ * ಗಾತ್ರ(db.bridges[0]));
	}

	mosquitto_FREE(context->bridge->name);
	mosquitto_FREE(context->bridge->local_clientid);
	mosquitto_FREE(context->bridge->local_username);
	mosquitto_FREE(context->bridge->local_password);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	mosquitto_FREE(context->bridge->tls_certfile);
	mosquitto_FREE(context->bridge->tls_keyfile);
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ(context->bridge->remote_clientid != context->id){
		mosquitto_FREE(context->bridge->remote_clientid);
	}
	context->bridge->remote_clientid = ಖಾಲಿ;

	ಆದರೆ(context->bridge->remote_username != context->username){
		mosquitto_FREE(context->bridge->remote_username);
	}
	context->bridge->remote_username = ಖಾಲಿ;

	ಆದರೆ(context->bridge->remote_password != context->password){
		mosquitto_FREE(context->bridge->remote_password);
	}
	context->bridge->remote_password = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	ಆದರೆ(context->ssl_ctx){
		SSL_CTX_free(context->ssl_ctx);
		context->ssl_ctx = ಖಾಲಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಗಾಗಿ(i=0; i<context->bridge->ವಿಳಾಸ_ಎಣಿಕೆ; i++){
		mosquitto_FREE(context->bridge->ವಿಳಾಸಗಳು[i].ವಿಳಾಸ);
	}

	mosquitto_FREE(context->bridge->ವಿಳಾಸಗಳು);

	config__bridge_cleanup(context->bridge);
	context->bridge = ಖಾಲಿ;
}


ಸ್ಥಿರ ಶೂನ್ಯ bridge__packet_cleanup(ರಚನೆ mosquitto *context)
{
	ರಚನೆ mosquitto__packet *packet;
	ಆದರೆ(!context){
		ಹಿಂತಿರುಗಿ;
	}

	ಆಗ(context->ಹೊರಹೋಗು_packet){
		packet = context->ಹೊರಹೋಗು_packet;
		context->ಹೊರಹೋಗು_packet = context->ಹೊರಹೋಗು_packet->ಮುಂದಿನ;
		mosquitto_FREE(packet);
	}
	context->ಹೊರಹೋಗು_packet = ಖಾಲಿ;
	context->ಹೊರಹೋಗು_packet_ಕೊನೆಯ = ಖಾಲಿ;
	metrics__int_dec(mosq_gauge_ಹೊರಹೋಗು_packets, context->ಹೊರಹೋಗು_packet_ಎಣಿಕೆ);
	metrics__int_dec(mosq_gauge_ಹೊರಹೋಗು_packet_ಬೈಟುಗಳು, context->ಹೊರಹೋಗು_packet_ಬೈಟುಗಳು);
	context->ಹೊರಹೋಗು_packet_ಎಣಿಕೆ = 0;
	context->ಹೊರಹೋಗು_packet_ಬೈಟುಗಳು = 0;

	packet__cleanup(&(context->ಒಳ_packet));
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rand_between(ಪೂರ್ಣಾಂಕ ಕೆಳಗಿನ, ಪೂರ್ಣಾಂಕ ಮೇಲಿನ)
{
	ಪೂರ್ಣಾಂಕ r;
	mosquitto_getrandom(&r, ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));
	ಹಿಂತಿರುಗಿ (ನಿರಪೇಕ್ಷ(r) % (ಮೇಲಿನ - ಕೆಳಗಿನ)) + ಕೆಳಗಿನ;
}


ಸ್ಥಿರ ಶೂನ್ಯ bridge_backoff_ಹೆಜ್ಜೆ(ರಚನೆ mosquitto__bridge *bridge)
{
	/*
		“Decorrelated Jitter” calculation, according to:
			https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
	*/

	bridge->restart_timeout = rand_between(bridge->backoff_ತಳ, bridge->restart_timeout * 3);
	ಆದರೆ(bridge->restart_timeout > bridge->backoff_cap){
		bridge->restart_timeout = bridge->backoff_cap;
	}
}


ಸ್ಥಿರ ಶೂನ್ಯ bridge__backoff_reset(ರಚನೆ mosquitto__bridge *bridge)
{
	bridge->restart_timeout = bridge->backoff_ತಳ;
}


ಸ್ಥಿರ ಶೂನ್ಯ bridge__update_backoff(ರಚನೆ mosquitto__bridge *bridge)
{
	ಆದರೆ(!bridge){
		ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ(!bridge->backoff_cap){
		/* skip if not using jitter */
		ಹಿಂತಿರುಗಿ;

	}
	ಆದರೆ(bridge->connected_at && db.now_s - bridge->connected_at >= bridge->stable_connection_period){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Bridge %s connection was stable enough, resetting backoff", bridge->name);
		bridge__backoff_reset(bridge);
	}ಇಲ್ಲದಿದ್ದರೆ{
		bridge_backoff_ಹೆಜ್ಜೆ(bridge);
	}

	bridge->connected_at = 0;

	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Bridge %s next backoff will be %d ms", bridge->name, bridge->restart_timeout);
}


ಸ್ಥಿರ ಶೂನ್ಯ bridge_check_pending(ರಚನೆ mosquitto *context)
{
	ಪೂರ್ಣಾಂಕ err;
	socklen_t len;

	ಆದರೆ(context->state == mosq_cs_connect_pending){
		len = ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ);
		ಆದರೆ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (ಅಕ್ಷರ *)&err, &len)){
			ಆದರೆ(err == 0){
				mosquitto_ಗಣ_state(context, mosq_cs_new);
#ಆದರೆ defined(WITH_ADNS) && defined(WITH_BRIDGE)
				ಆದರೆ(context->bridge){
					bridge__connect_step3(context);
				}
#ಆದರೆ_ಅಂತ್ಯ
			}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(err == ECONNREFUSED){
				do_disconnect(context, MOSQ_ERR_CONN_ಕಳೆದುಹೋದ);
				ಹಿಂತಿರುಗಿ;
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			do_disconnect(context, MOSQ_ERR_CONN_ಕಳೆದುಹೋದ);
			ಹಿಂತಿರುಗಿ;
		}
	}
}


ಸ್ಥಿರ bool reload_if_needed(ರಚನೆ mosquitto *context)
{
	ಪೂರ್ಣಾಂಕ i;

	ಗಾಗಿ(i=0; i<db.config->bridge_ಎಣಿಕೆ; i++){
		ಆದರೆ(db.config->bridges[i] && !strcmp(context->bridge->name, db.config->bridges[i]->name)){
			bridge__destroy(context);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ಖಾಲಿ;
			loop_update_ಮುಂದಿನ_event(100);
			ಹಿಂತಿರುಗಿ true;
		}
	}

	ಹಿಂತಿರುಗಿ false;
}


ಶೂನ್ಯ bridge_check(ಶೂನ್ಯ)
{
	ಸ್ಥಿರ time_t ಕೊನೆಯ_check = 0;
	ರಚನೆ mosquitto *context = ಖಾಲಿ;
	socklen_t len;
	ಪೂರ್ಣಾಂಕ i;
	ಪೂರ್ಣಾಂಕ rc;
	ಪೂರ್ಣಾಂಕ err;

	ಆದರೆ(db.now_s <= ಕೊನೆಯ_check){
		ಹಿಂತಿರುಗಿ;
	}

	ಗಾಗಿ(i=0; i<db.bridge_ಎಣಿಕೆ; i++){
		ಆದರೆ(!db.bridges[i]){
			ಮುಂದುವರಿ;
		}

		context = db.bridges[i];

		ಆದರೆ(net__is_connected(context)){
			mosquitto__check_keepalive(context);
			bridge_check_pending(context);

			/* Check for bridges that are not round robin and not currently
			 * connected to their primary broker. */
			ಆದರೆ(context->bridge->ಸುತ್ತು_robin == false
					&& context->bridge->cur_ವಿಳಾಸ != 0
					&& context->bridge->primary_retry
					&& db.now_s >= context->bridge->primary_retry){

				ಆದರೆ(context->bridge->primary_retry_sock == INVALID_SOCKET){
					rc = net__try_connect(context->bridge->ವಿಳಾಸಗಳು[0].ವಿಳಾಸ,
							context->bridge->ವಿಳಾಸಗಳು[0].ಪೋರ್ಟ್,
							&context->bridge->primary_retry_sock,
							context->bridge->bind_ವಿಳಾಸ, false);

					ಆದರೆ(rc == 0){
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = 0;
						mux__delete(context);
						net__socket_close(context);
						context->bridge->cur_ವಿಳಾಸ = 0;
					}
				}ಇಲ್ಲದಿದ್ದರೆ{
					len = ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ);
					ಆದರೆ(!getsockopt(context->bridge->primary_retry_sock, SOL_SOCKET, SO_ERROR, (ಅಕ್ಷರ *)&err, &len)){
						ಆದರೆ(err == 0){
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = 0;
							mux__delete(context);
							net__socket_close(context);
							context->bridge->cur_ವಿಳಾಸ = context->bridge->ವಿಳಾಸ_ಎಣಿಕೆ-1;
						}ಇಲ್ಲದಿದ್ದರೆ{
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = db.now_s+5;
							loop_update_ಮುಂದಿನ_event(5000);
						}
					}ಇಲ್ಲದಿದ್ದರೆ{
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = db.now_s+5;
						loop_update_ಮುಂದಿನ_event(5000);
					}
				}
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಆದರೆ(reload_if_needed(context)){
				ಮುಂದುವರಿ;
			}

			/* Want to try to restart the bridge connection */
			ಆದರೆ(!context->bridge->restart_t){
				bridge__update_backoff(context->bridge);
				context->bridge->restart_t = 1000*db.now_s+context->bridge->restart_timeout;
				context->bridge->cur_ವಿಳಾಸ++;
				ಆದರೆ(context->bridge->cur_ವಿಳಾಸ == context->bridge->ವಿಳಾಸ_ಎಣಿಕೆ){
					context->bridge->cur_ವಿಳಾಸ = 0;
				}
				loop_update_ಮುಂದಿನ_event(context->bridge->restart_timeout);
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಆದರೆ((context->bridge->ಆರಂಭ_type == bst_lazy && context->bridge->lazy_reconnect)
						|| (context->bridge->ಆರಂಭ_type == bst_automatic && 1000*db.now_s >= context->bridge->restart_t)){

#ಆದರೆ defined(__GLIBC__) && defined(WITH_ADNS)
					ಆದರೆ(context->adns){
						/* Connection attempted, waiting on DNS lookup */
						rc = gai_error(context->adns);
						ಆದರೆ(rc == EAI_INPROGRESS){
							/* Just keep on waiting */
						}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == 0){
							rc = bridge__connect_step2(context);
							ಆದರೆ(rc == MOSQ_ERR_SUCCESS){
								mux__new(context);
								ಆದರೆ(context->ಹೊರಹೋಗು_packet){
									mux_ಕೂಡಿಸು_ಹೊರಹೋಗು(context);
								}
							}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_CONN_PENDING){
								mux__new(context);
								mux_ಕೂಡಿಸು_ಹೊರಹೋಗು(context);
								context->bridge->restart_t = 0;
							}ಇಲ್ಲದಿದ್ದರೆ{
								context->bridge->cur_ವಿಳಾಸ++;
								ಆದರೆ(context->bridge->cur_ವಿಳಾಸ == context->bridge->ವಿಳಾಸ_ಎಣಿಕೆ){
									context->bridge->cur_ವಿಳಾಸ = 0;
								}
								context->bridge->restart_t = 0;
							}
						}ಇಲ್ಲದಿದ್ದರೆ{
							/* Need to retry */
							ಆದರೆ(context->adns->ar_ಫಲಿತಾಂಶ){
								freeaddrinfo(context->adns->ar_ಫಲಿತಾಂಶ);
							}
							mosquitto_FREE(context->adns);
							context->bridge->restart_t = 0;
						}
					}ಇಲ್ಲದಿದ್ದರೆ{
						rc = bridge__connect_step1(context);
						ಆದರೆ(rc){
							context->bridge->cur_ವಿಳಾಸ++;
							ಆದರೆ(context->bridge->cur_ವಿಳಾಸ == context->bridge->ವಿಳಾಸ_ಎಣಿಕೆ){
								context->bridge->cur_ವಿಳಾಸ = 0;
							}
						}ಇಲ್ಲದಿದ್ದರೆ{
							/* Short wait for ADNS lookup */
							context->bridge->restart_t = 1;
							loop_update_ಮುಂದಿನ_event(1000);
						}
					}
#ಇಲ್ಲದಿದ್ದರೆ
					{
						rc = bridge__connect(context);
						context->bridge->restart_t = 0;
						ಆದರೆ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING){
							ಆದರೆ(context->bridge->ಸುತ್ತು_robin == false && context->bridge->cur_ವಿಳಾಸ != 0){
								context->bridge->primary_retry = db.now_s + 5;
								loop_update_ಮುಂದಿನ_event(5000);
							}
							mux__new(context);
							ಆದರೆ(context->ಹೊರಹೋಗು_packet){
								mux_ಕೂಡಿಸು_ಹೊರಹೋಗು(context);
							}
						}ಇಲ್ಲದಿದ್ದರೆ{
							context->bridge->cur_ವಿಳಾಸ++;
							ಆದರೆ(context->bridge->cur_ವಿಳಾಸ == context->bridge->ವಿಳಾಸ_ಎಣಿಕೆ){
								context->bridge->cur_ವಿಳಾಸ = 0;
							}
						}
					}
#ಆದರೆ_ಅಂತ್ಯ
				}
			}
		}
	}
}

#ಆದರೆ_ಅಂತ್ಯ
