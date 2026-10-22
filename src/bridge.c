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

#ifndef WIN32
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಅನ್ತರ್ಭಾವ <sys/socket.h>
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಅನ್ತರ್ಭಾವ <netinet/tcp.h>
#ಅನ್ಯಥಾ
#ಅನ್ತರ್ಭಾವ <winsock2.h>
#ಅನ್ತರ್ಭಾವ <ws2tcpip.h>
#endif

#ifndef WIN32
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ಯಥಾ
#ಅನ್ತರ್ಭಾವ <process.h>
#ಅನ್ತರ್ಭಾವ <winsock2.h>
#ಅನ್ತರ್ಭಾವ <ws2tcpip.h>
#endif

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

#ifdef WITH_BRIDGE

ಸ್ಥಿರ ಶೂನ್ಯ bridge__update_backoff(ಸಂರಚನಾ mosquitto__bridge *bridge);
#ಯದಿ defined(__GLIBC__) && defined(WITH_ADNS)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ bridge__connect_step1(ಸಂರಚನಾ mosquitto *context);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ bridge__connect_step2(ಸಂರಚನಾ mosquitto *context);
#endif
ಸ್ಥಿರ ಶೂನ್ಯ bridge__packet_cleanup(ಸಂರಚನಾ mosquitto *context);

ಸ್ಥಿರ ಸಂರಚನಾ mosquitto *bridge__new(ಸಂರಚನಾ mosquitto__bridge *bridge)
{
	ಸಂರಚನಾ mosquitto *new_context = ರಿಕ್ತ;
	ಸಂರಚನಾ mosquitto **bridges;
	ವರ್ಣ *local_id;

	ಪ್ರತಿಜ್ಞಾ(bridge);

	local_id = mosquitto_strdup(bridge->local_clientid);
	ಯದಿ(!local_id){
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	HASH_FIND(hh_id, db.contexts_by_id, local_id, strlen(local_id), new_context);
	ಯದಿ(new_context){
		/* (possible from persistent db) */
		mosquitto_FREE(local_id);
	}ಅನ್ಯಥಾ{
		/* id wasn't found, so generate a new context */
		new_context = context__init();
		ಯದಿ(!new_context){
			mosquitto_FREE(local_id);
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		new_context->id = local_id;
		context__add_to_by_id(new_context);
	}
	new_context->transport = mosq_t_tcp;
	new_context->bridge = bridge;
	new_context->is_bridge = true;

	new_context->username = bridge->remote_username;
	new_context->password = bridge->remote_password;

#ifdef WITH_TLS
	new_context->tls_cafile = bridge->tls_cafile;
	new_context->tls_capath = bridge->tls_capath;
	new_context->tls_certfile = bridge->tls_certfile;
	new_context->tls_keyfile = bridge->tls_keyfile;
	new_context->tls_cert_reqs = SSL_VERIFY_PEER;
	new_context->tls_ocsp_required = bridge->tls_ocsp_required;
	new_context->tls_version = bridge->tls_version;
	new_context->tls_insecure = bridge->tls_insecure;
	new_context->tls_alpn = bridge->tls_alpn;
	new_context->tls_ciphers = bridge->tls_ciphers;
	new_context->tls_13_ciphers = bridge->tls_13_ciphers;
	new_context->tls_engine = ರಿಕ್ತ;
	new_context->tls_keyform = mosq_k_pem;
	new_context->tls_use_os_certs = bridge->tls_use_os_certs;
	new_context->ssl_ctx_defaults = true;
#ifdef FINAL_WITH_TLS_PSK
	new_context->tls_psk_identity = bridge->tls_psk_identity;
	new_context->tls_psk = bridge->tls_psk;
#endif
#endif

	bridge->try_private_accepted = true;
	ಯದಿ(bridge->clean_start_local == -1){
		/* default to "regular" clean start setting */
		bridge->clean_start_local = bridge->clean_start;
	}
	new_context->retain_available = bridge->outgoing_retain;
	new_context->protocol = bridge->protocol_version;
	ಯದಿ(!bridge->clean_start_local){
		new_context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
		plugin_persist__handle_client_add(new_context);
		ಯದಿ(new_context->expiry_list_item){
			/* We've restored from persistence and been added to the session
			 * expiry list, even though we should never be expired */
			session_expiry__remove(new_context);
		}
	}

	bridges = mosquitto_realloc(db.bridges, (size_t)(db.bridge_ಎಣಿಕೆ+1)*ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto *));
	ಯದಿ(bridges){
		db.bridges = bridges;
		db.bridge_ಎಣಿಕೆ++;
		db.bridges[db.bridge_ಎಣಿಕೆ-1] = new_context;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	ಪ್ರತ್ಯಾಗಮ new_context;
}


ಸ್ಥಿರ ಶೂನ್ಯ bridge__destroy(ಸಂರಚನಾ mosquitto *context)
{
	send__disconnect(context, MQTT_RC_SUCCESS, ರಿಕ್ತ);
	context__cleanup(context, true);
}


ಶೂನ್ಯ bridge__start_all(ಶೂನ್ಯ)
{
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.config->bridge_ಎಣಿಕೆ; i++){
		ಸಂರಚನಾ mosquitto *context;
		ಪೂರ್ಣಾಙ್ಕ ret;

		context = bridge__new(db.config->bridges[i]);
		ಯದಿ(!context){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
			ಪ್ರತ್ಯಾಗಮ;
		}
		ಪ್ರತಿಜ್ಞಾ(context);

#ಯದಿ defined(__GLIBC__) && defined(WITH_ADNS)
		context->bridge->restart_t = 1; /* force quick restart of bridge */
		loop__update_next_event(1000);
		ret = bridge__connect_step1(context);
#ಅನ್ಯಥಾ
		ret = bridge__connect(context);
#endif

		ಯದಿ(ret && ret != MOSQ_ERR_CONN_PENDING){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Unable to connect bridge %s.",
					context->bridge->name);
		}

		db.config->bridges[i] = ರಿಕ್ತ;
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ bridge__set_tcp_keepalive(ಸಂರಚನಾ mosquitto *context)
{
	unsigned ಪೂರ್ಣಾಙ್ಕ enabled = 1;
	bool ret;

	ಯದಿ(context->bridge->tcp_keepalive_idle == 0
			|| context->bridge->tcp_keepalive_interval == 0
			|| context->bridge->tcp_keepalive_counter == 0){

		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

#ifdef WIN32
#  define SETSOCKOPT_TYPE ವರ್ಣ *
#ಅನ್ಯಥಾ
#  define SETSOCKOPT_TYPE ಧ್ರುವ ಶೂನ್ಯ *
#endif

	ret = setsockopt(context->sock, SOL_SOCKET, SO_KEEPALIVE, (SETSOCKOPT_TYPE)&enabled, ಪರಿಮಾಣ(enabled));

#ifdef TCP_KEEPIDLE
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPIDLE,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_idle, ಪರಿಮಾಣ(context->bridge->tcp_keepalive_idle));
#endif
#ifdef TCP_KEEPINTVL
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPINTVL,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_interval, ಪರಿಮಾಣ(context->bridge->tcp_keepalive_interval));
#endif
#ifdef TCP_KEEPCNT
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPCNT,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_counter, ಪರಿಮಾಣ(context->bridge->tcp_keepalive_counter));
#endif

#undef SETSOCKOPT_TYPE

	ಯದಿ(ret){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TCP_USER_TIMEOUT


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ bridge__set_tcp_user_timeout(ಸಂರಚನಾ mosquitto *context)
{
	ಪೂರ್ಣಾಙ್ಕ timeout = context->bridge->tcp_user_timeout;
	ಯದಿ(timeout >= 0){
		ಯದಿ(setsockopt(context->sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (ವರ್ಣ *)&timeout, ಪರಿಮಾಣ(timeout))){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}
#endif

#ಯದಿ defined(__GLIBC__) && defined(WITH_ADNS)


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ bridge__connect_step1(ಸಂರಚನಾ mosquitto *context)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ವರ್ಣ *notification_topic;
	size_t notification_topic_len;
	uint8_t notification_payload;
	ಸಂರಚನಾ mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	ಯದಿ(!context || !context->bridge){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_ಸಂದೇಶ_in = db.now_s;
	context->next_ಸಂದೇಶ_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = ರಿಕ್ತ;
	context->ping_t = 0;
	context->bridge->lazy_reconnect = false;
	context->maximum_packet_size = context->bridge->maximum_packet_size;
	bridge__packet_cleanup(context);
	db__message_reconnect_reset(context);

	db__messages_delete(context, false);

	/* Delete all local subscriptions even for clean_start==false. We don't
	 * remove any messages and the next loop carries out the resubscription
	 * anyway. This means any unwanted subs will be removed.
	 */
	sub__clean_session(context);

	LL_FOREACH(context->bridge->topics, cur_topic){
		ಯದಿ(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			ಯದಿ(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}ಅನ್ಯಥಾ{
				qos = cur_topic->qos;
			}
			ಸಂರಚನಾ mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			ಯದಿ(sub__add(context, &sub) > 0){
				ಪ್ರತ್ಯಾಗಮ 1;
			}
			retain__queue(context, &sub);
		}
	}

	ಯದಿ(context->bridge->notifications){
		ಯದಿ(context->max_qos == 0){
			qos = 0;
		}ಅನ್ಯಥಾ{
			qos = 1;
		}
		ಯದಿ(context->bridge->notification_topic){
			ಯದಿ(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, ಸಂದೇಶ_EXPIRY_INFINITE, ರಿಕ್ತ);
				context->bridge->initial_notification_done = true;
			}
			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, ರಿಕ್ತ);
			ಯದಿ(rc != MOSQ_ERR_SUCCESS){
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}ಅನ್ಯಥಾ{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(ಪರಿಮಾಣ(ವರ್ಣ)*(notification_topic_len+1));
			ಯದಿ(!notification_topic){
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			ಯದಿ(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, ಸಂದೇಶ_EXPIRY_INFINITE, ರಿಕ್ತ);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, ರಿಕ್ತ);
			mosquitto_FREE(notification_topic);
			ಯದಿ(rc != MOSQ_ERR_SUCCESS){
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}
	}

	log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Connecting bridge (step 1) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step1(context, context->bridge->addresses[context->bridge->cur_address].address);
	ಯದಿ(rc > 0){
		ಯದಿ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ಪ್ರತ್ಯಾಗಮ rc; /* Error already printed */
		}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_ERRNO){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_EAI){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ bridge__connect_step2(ಸಂರಚನಾ mosquitto *context)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(!context || !context->bridge){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Connecting bridge (step 2) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step2(context, context->bridge->addresses[context->bridge->cur_address].port, &context->sock);
	ಯದಿ(rc > 0){
		ಯದಿ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ಪ್ರತ್ಯಾಗಮ rc; /* Error already printed */
		}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_ERRNO){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_EAI){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ಪ್ರತ್ಯಾಗಮ rc;
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, ಪರಿಮಾಣ(context->sock), context);

	ಯದಿ(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}
	ಪ್ರತ್ಯಾಗಮ rc;
}


ಪೂರ್ಣಾಙ್ಕ bridge__connect_step3(ಸಂರಚನಾ mosquitto *context)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = ರಿಕ್ತ;

	rc = net__socket_connect_step3(context, context->bridge->addresses[context->bridge->cur_address].address);
	ಯದಿ(rc > 0){
		ಯದಿ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ಪ್ರತ್ಯಾಗಮ rc; /* Error already printed */
		}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_ERRNO){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_EAI){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಯದಿ(context->bridge->round_robin == false && context->bridge->cur_address != 0){
		context->bridge->primary_retry = db.now_s + 5;
		loop__update_next_event(5000);
	}

	ಯದಿ(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	ಯದಿ(bridge__set_tcp_user_timeout(context)){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}
#endif

	ಯದಿ(context->bridge->receive_maximum != 0){
		receive_maximum.ಮೌಲ್ಯ.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	ಯದಿ(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.ಮೌಲ್ಯ.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	ಯದಿ(context->bridge->max_topic_alias != 0){
		topic_alias_max.ಮೌಲ್ಯ.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc = send__connect(context, context->keepalive, context->clean_start, properties);
	ಯದಿ(rc == MOSQ_ERR_SUCCESS){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}ಅನ್ಯಥಾ{
		ಯದಿ(rc == MOSQ_ERR_TLS){
			ಪ್ರತ್ಯಾಗಮ rc; /* Error already printed */
		}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_ERRNO){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_EAI){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		ಪ್ರತ್ಯಾಗಮ rc;
	}
}
#ಅನ್ಯಥಾ


ಪೂರ್ಣಾಙ್ಕ bridge__connect(ಸಂರಚನಾ mosquitto *context)
{
	ಪೂರ್ಣಾಙ್ಕ rc, rc2;
	ವರ್ಣ *notification_topic = ರಿಕ್ತ;
	size_t notification_topic_len;
	uint8_t notification_payload;
	ಸಂರಚನಾ mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = ರಿಕ್ತ;

	ಯದಿ(!context || !context->bridge){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->last_ಸಂದೇಶ_in = db.now_s;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->next_ಸಂದೇಶ_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = ರಿಕ್ತ;
	context->ping_t = 0;
	context->bridge->lazy_reconnect = false;
	context->maximum_packet_size = context->bridge->maximum_packet_size;
	bridge__packet_cleanup(context);
	db__message_reconnect_reset(context);

	db__messages_delete(context, false);

	/* Delete all local subscriptions even for clean_start==false. We don't
	 * remove any messages and the next loop carries out the resubscription
	 * anyway. This means any unwanted subs will be removed.
	 */
	sub__clean_session(context);

	LL_FOREACH(context->bridge->topics, cur_topic){
		ಯದಿ(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			ಯದಿ(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}ಅನ್ಯಥಾ{
				qos = cur_topic->qos;
			}
			ಸಂರಚನಾ mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			ಯದಿ(sub__add(context, &sub) > 0){
				ಪ್ರತ್ಯಾಗಮ 1;
			}
		}
	}

	ಯದಿ(context->bridge->notifications){
		ಯದಿ(context->max_qos == 0){
			qos = 0;
		}ಅನ್ಯಥಾ{
			qos = 1;
		}
		ಯದಿ(context->bridge->notification_topic){
			ಯದಿ(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, ಸಂದೇಶ_EXPIRY_INFINITE, ರಿಕ್ತ);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, ರಿಕ್ತ);
			ಯದಿ(rc != MOSQ_ERR_SUCCESS){
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}ಅನ್ಯಥಾ{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(ಪರಿಮಾಣ(ವರ್ಣ)*(notification_topic_len+1));
			ಯದಿ(!notification_topic){
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			ಯದಿ(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, ಸಂದೇಶ_EXPIRY_INFINITE, ರಿಕ್ತ);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, ರಿಕ್ತ);
			ಯದಿ(rc != MOSQ_ERR_SUCCESS){
				mosquitto_FREE(notification_topic);
				ಪ್ರತ್ಯಾಗಮ rc;
			}
			mosquitto_FREE(notification_topic);
		}
	}

	log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Connecting bridge %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__socket_connect(context,
			context->bridge->addresses[context->bridge->cur_address].address,
			context->bridge->addresses[context->bridge->cur_address].port,
			context->bridge->bind_address,
			false);

	ಯದಿ(rc > 0){
		ಯದಿ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ಪ್ರತ್ಯಾಗಮ rc; /* Error already printed */
		}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_ERRNO){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_EAI){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ಪ್ರತ್ಯಾಗಮ rc;
	}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, ಪರಿಮಾಣ(context->sock), context);

	ಯದಿ(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	ಯದಿ(bridge__set_tcp_user_timeout(context)){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}
#endif

	ಯದಿ(context->bridge->receive_maximum != 0){
		receive_maximum.ಮೌಲ್ಯ.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	ಯದಿ(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.ಮೌಲ್ಯ.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	ಯದಿ(context->bridge->max_topic_alias != 0){
		topic_alias_max.ಮೌಲ್ಯ.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc2 = send__connect(context, context->keepalive, context->clean_start, properties);
	ಯದಿ(rc2 == MOSQ_ERR_SUCCESS){
		ಪ್ರತ್ಯಾಗಮ rc;
	}ಅನ್ಯಥಾ ಯದಿ(rc2 == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}ಅನ್ಯಥಾ{
		ಯದಿ(rc2 == MOSQ_ERR_TLS){
			ಪ್ರತ್ಯಾಗಮ rc2; /* Error already printed */
		}ಅನ್ಯಥಾ ಯದಿ(rc2 == MOSQ_ERR_ERRNO){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ಅನ್ಯಥಾ ಯದಿ(rc2 == MOSQ_ERR_EAI){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		ಪ್ರತ್ಯಾಗಮ rc2;
	}
}
#endif


ಪೂರ್ಣಾಙ್ಕ bridge__on_connect(ಸಂರಚನಾ mosquitto *context)
{
	ವರ್ಣ *notification_topic;
	size_t notification_topic_len;
	ಸಂರಚನಾ mosquitto__bridge_topic *cur_topic;
	ಪೂರ್ಣಾಙ್ಕ sub_opts;
	bool retain = true;
	uint8_t qos;

	ಯದಿ(context->bridge->notifications){
		ಯದಿ(context->max_qos == 0){
			qos = 0;
		}ಅನ್ಯಥಾ{
			qos = 1;
		}
		ಯದಿ(!context->retain_available){
			retain = false;
		}
		ವರ್ಣ notification_payload = '1';
		ಯದಿ(context->bridge->notification_topic){
			ಯದಿ(!context->bridge->notifications_local_only){
				ಯದಿ(send__real_publish(context, mosquitto__mid_generate(context),
						context->bridge->notification_topic, 1, &notification_payload, qos, retain, 0, 0, ರಿಕ್ತ, 0)){

					ಪ್ರತ್ಯಾಗಮ 1;
				}
			}
			db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, ಸಂದೇಶ_EXPIRY_INFINITE, ರಿಕ್ತ);
		}ಅನ್ಯಥಾ{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(ಪರಿಮಾಣ(ವರ್ಣ)*(notification_topic_len+1));
			ಯದಿ(!notification_topic){
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);
			notification_payload = '1';
			ಯದಿ(!context->bridge->notifications_local_only){
				ಯದಿ(send__real_publish(context, mosquitto__mid_generate(context),
						notification_topic, 1, &notification_payload, qos, retain, 0, 0, ರಿಕ್ತ, 0)){

					mosquitto_FREE(notification_topic);
					ಪ್ರತ್ಯಾಗಮ 1;
				}
			}
			db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, ಸಂದೇಶ_EXPIRY_INFINITE, ರಿಕ್ತ);
			mosquitto_FREE(notification_topic);
		}
	}

	LL_FOREACH(context->bridge->topics, cur_topic){
		ಯದಿ(cur_topic->direction == bd_in || cur_topic->direction == bd_both){
			ಯದಿ(cur_topic->qos > context->max_qos){
				sub_opts = context->max_qos;
			}ಅನ್ಯಥಾ{
				sub_opts = cur_topic->qos;
			}
			ಯದಿ(context->bridge->protocol_version == mosq_p_mqtt5){
				sub_opts = sub_opts
						| MQTT_SUB_OPT_NO_LOCAL
						| MQTT_SUB_OPT_RETAIN_AS_PUBLISHED
						| MQTT_SUB_OPT_SEND_RETAIN_ALWAYS;
			}
			ಯದಿ(send__subscribe(context, ರಿಕ್ತ, 1, &cur_topic->remote_topic, sub_opts, ರಿಕ್ತ)){
				ಪ್ರತ್ಯಾಗಮ 1;
			}
		}ಅನ್ಯಥಾ{
			ಯದಿ(context->bridge->attempt_unsubscribe){
				ಯದಿ(send__unsubscribe(context, ರಿಕ್ತ, 1, &cur_topic->remote_topic, ರಿಕ್ತ)){
					/* direction = inwards only. This means we should not be subscribed
					* to the topic. It is possible that we used to be subscribed to
					* this topic so unsubscribe. */
					ಪ್ರತ್ಯಾಗಮ 1;
				}
			}
		}
	}
	ಸಂರಚನಾ mosquitto_subscription sub;
	memset(&sub, 0, ಪರಿಮಾಣ(sub));
	LL_FOREACH(context->bridge->topics, cur_topic){
		sub.topic_filter = cur_topic->local_topic;
		ಯದಿ(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			ಯದಿ(cur_topic->qos > context->max_qos){
				sub.options = context->max_qos;
			}ಅನ್ಯಥಾ{
				sub.options = cur_topic->qos;
			}
			retain__queue(context, &sub);
		}
	}

	context->bridge->connected_at = db.now_s;

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ bridge__register_local_connections(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ mosquitto *context, *ctxt_tmp = ರಿಕ್ತ;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ಯದಿ(context->bridge){
			ಯದಿ(mux__new(context)){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error in initial bridge registration: %s", strerror(errno));
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
			}
			mux__add_out(context);
		}
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಶೂನ್ಯ bridge__reload(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ i;
	ಪೂರ್ಣಾಙ್ಕ j;

	// destroy old bridges that dissappeared
	ಪ್ರತಿ(i=0; i<db.bridge_ಎಣಿಕೆ; i++){
		ಪ್ರತಿ(j=0; j<db.config->bridge_ಎಣಿಕೆ; j++){
			ಯದಿ(!strcmp(db.bridges[i]->bridge->name, db.config->bridges[j]->name)){
				ಭಙ್ಗ;
			}
		}

		ಯದಿ(j==db.config->bridge_ಎಣಿಕೆ){
			bridge__destroy(db.bridges[i]);
		}
	}

	ಪ್ರತಿ(i=0; i<db.config->bridge_ಎಣಿಕೆ; i++){
		ಪ್ರತಿ(j=0; j<db.bridge_ಎಣಿಕೆ; j++){
			ಯದಿ(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
				ಭಙ್ಗ;
			}
		}

		ಯದಿ(j==db.bridge_ಎಣಿಕೆ){
			// a new bridge was found, create it
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ರಿಕ್ತ;
			ಅನುವರ್ತ;
		}

		ಯದಿ(db.config->bridges[i]->reload_type == brt_immediate){
			// in this case, an existing bridge should match
			ಪ್ರತಿ(j=0; j<db.bridge_ಎಣಿಕೆ; j++){
				ಯದಿ(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
					ಭಙ್ಗ;
				}
			}

			ಪ್ರತಿಜ್ಞಾ(j<db.bridge_ಎಣಿಕೆ);
			db.bridges[j]->will_delay_interval = 0;
			bridge__destroy(db.bridges[j]);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ರಿಕ್ತ;
		}
	}
}


ಶೂನ್ಯ bridge__db_cleanup(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ i;

	ಪ್ರತಿ(i=0; i<db.bridge_ಎಣಿಕೆ; i++){
		ಯದಿ(db.bridges[i]){
			context__cleanup(db.bridges[i], true);
		}
	}
	mosquitto_FREE(db.bridges);
}


ಶೂನ್ಯ bridge__cleanup(ಸಂರಚನಾ mosquitto *context)
{
	ಪೂರ್ಣಾಙ್ಕ i;

	ಪ್ರತಿಜ್ಞಾ(db.bridge_ಎಣಿಕೆ > 0);

	ಪ್ರತಿ(i=0; i<db.bridge_ಎಣಿಕೆ; i++){
		ಯದಿ(db.bridges[i] == context){
			db.bridges[i] = db.bridges[db.bridge_ಎಣಿಕೆ-1];
			ಭಙ್ಗ;
		}
	}

	db.bridge_ಎಣಿಕೆ--;
	ಯದಿ(db.bridge_ಎಣಿಕೆ == 0){
		mosquitto_FREE(db.bridges);
	}ಅನ್ಯಥಾ{
		db.bridges = mosquitto_realloc(db.bridges, (unsigned)db.bridge_ಎಣಿಕೆ * ಪರಿಮಾಣ(db.bridges[0]));
	}

	mosquitto_FREE(context->bridge->name);
	mosquitto_FREE(context->bridge->local_clientid);
	mosquitto_FREE(context->bridge->local_username);
	mosquitto_FREE(context->bridge->local_password);
#ifdef WITH_TLS
	mosquitto_FREE(context->bridge->tls_certfile);
	mosquitto_FREE(context->bridge->tls_keyfile);
#endif

	ಯದಿ(context->bridge->remote_clientid != context->id){
		mosquitto_FREE(context->bridge->remote_clientid);
	}
	context->bridge->remote_clientid = ರಿಕ್ತ;

	ಯದಿ(context->bridge->remote_username != context->username){
		mosquitto_FREE(context->bridge->remote_username);
	}
	context->bridge->remote_username = ರಿಕ್ತ;

	ಯದಿ(context->bridge->remote_password != context->password){
		mosquitto_FREE(context->bridge->remote_password);
	}
	context->bridge->remote_password = ರಿಕ್ತ;
#ifdef WITH_TLS
	ಯದಿ(context->ssl_ctx){
		SSL_CTX_free(context->ssl_ctx);
		context->ssl_ctx = ರಿಕ್ತ;
	}
#endif

	ಪ್ರತಿ(i=0; i<context->bridge->address_ಎಣಿಕೆ; i++){
		mosquitto_FREE(context->bridge->addresses[i].address);
	}

	mosquitto_FREE(context->bridge->addresses);

	config__bridge_cleanup(context->bridge);
	context->bridge = ರಿಕ್ತ;
}


ಸ್ಥಿರ ಶೂನ್ಯ bridge__packet_cleanup(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ mosquitto__packet *packet;
	ಯದಿ(!context){
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯಾವತ್(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_FREE(packet);
	}
	context->out_packet = ರಿಕ್ತ;
	context->out_packet_last = ರಿಕ್ತ;
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_ಎಣಿಕೆ);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_ಎಣಿಕೆ = 0;
	context->out_packet_bytes = 0;

	packet__cleanup(&(context->in_packet));
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rand_between(ಪೂರ್ಣಾಙ್ಕ ಕೆಳಗಿನ, ಪೂರ್ಣಾಙ್ಕ ಮೇಲಿನ)
{
	ಪೂರ್ಣಾಙ್ಕ r;
	mosquitto_getrandom(&r, ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));
	ಪ್ರತ್ಯಾಗಮ (abs(r) % (ಮೇಲಿನ - ಕೆಳಗಿನ)) + ಕೆಳಗಿನ;
}


ಸ್ಥಿರ ಶೂನ್ಯ bridge__backoff_step(ಸಂರಚನಾ mosquitto__bridge *bridge)
{
	/*
		“Decorrelated Jitter” calculation, according to:
			https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
	*/

	bridge->restart_timeout = rand_between(bridge->backoff_base, bridge->restart_timeout * 3);
	ಯದಿ(bridge->restart_timeout > bridge->backoff_cap){
		bridge->restart_timeout = bridge->backoff_cap;
	}
}


ಸ್ಥಿರ ಶೂನ್ಯ bridge__backoff_reset(ಸಂರಚನಾ mosquitto__bridge *bridge)
{
	bridge->restart_timeout = bridge->backoff_base;
}


ಸ್ಥಿರ ಶೂನ್ಯ bridge__update_backoff(ಸಂರಚನಾ mosquitto__bridge *bridge)
{
	ಯದಿ(!bridge){
		ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ(!bridge->backoff_cap){
		/* skip if not using jitter */
		ಪ್ರತ್ಯಾಗಮ;

	}
	ಯದಿ(bridge->connected_at && db.now_s - bridge->connected_at >= bridge->stable_connection_period){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Bridge %s connection was stable enough, resetting backoff", bridge->name);
		bridge__backoff_reset(bridge);
	}ಅನ್ಯಥಾ{
		bridge__backoff_step(bridge);
	}

	bridge->connected_at = 0;

	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Bridge %s next backoff will be %d ms", bridge->name, bridge->restart_timeout);
}


ಸ್ಥಿರ ಶೂನ್ಯ bridge_check_pending(ಸಂರಚನಾ mosquitto *context)
{
	ಪೂರ್ಣಾಙ್ಕ err;
	socklen_t len;

	ಯದಿ(context->state == mosq_cs_connect_pending){
		len = ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ);
		ಯದಿ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (ವರ್ಣ *)&err, &len)){
			ಯದಿ(err == 0){
				mosquitto__set_state(context, mosq_cs_new);
#ಯದಿ defined(WITH_ADNS) && defined(WITH_BRIDGE)
				ಯದಿ(context->bridge){
					bridge__connect_step3(context);
				}
#endif
			}ಅನ್ಯಥಾ ಯದಿ(err == ECONNREFUSED){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				ಪ್ರತ್ಯಾಗಮ;
			}
		}ಅನ್ಯಥಾ{
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			ಪ್ರತ್ಯಾಗಮ;
		}
	}
}


ಸ್ಥಿರ bool reload_if_needed(ಸಂರಚನಾ mosquitto *context)
{
	ಪೂರ್ಣಾಙ್ಕ i;

	ಪ್ರತಿ(i=0; i<db.config->bridge_ಎಣಿಕೆ; i++){
		ಯದಿ(db.config->bridges[i] && !strcmp(context->bridge->name, db.config->bridges[i]->name)){
			bridge__destroy(context);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ರಿಕ್ತ;
			loop__update_next_event(100);
			ಪ್ರತ್ಯಾಗಮ true;
		}
	}

	ಪ್ರತ್ಯಾಗಮ false;
}


ಶೂನ್ಯ bridge_check(ಶೂನ್ಯ)
{
	ಸ್ಥಿರ time_t last_check = 0;
	ಸಂರಚನಾ mosquitto *context = ರಿಕ್ತ;
	socklen_t len;
	ಪೂರ್ಣಾಙ್ಕ i;
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಪೂರ್ಣಾಙ್ಕ err;

	ಯದಿ(db.now_s <= last_check){
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಪ್ರತಿ(i=0; i<db.bridge_ಎಣಿಕೆ; i++){
		ಯದಿ(!db.bridges[i]){
			ಅನುವರ್ತ;
		}

		context = db.bridges[i];

		ಯದಿ(net__is_connected(context)){
			mosquitto__check_keepalive(context);
			bridge_check_pending(context);

			/* Check for bridges that are not round robin and not currently
			 * connected to their primary broker. */
			ಯದಿ(context->bridge->round_robin == false
					&& context->bridge->cur_address != 0
					&& context->bridge->primary_retry
					&& db.now_s >= context->bridge->primary_retry){

				ಯದಿ(context->bridge->primary_retry_sock == INVALID_SOCKET){
					rc = net__try_connect(context->bridge->addresses[0].address,
							context->bridge->addresses[0].port,
							&context->bridge->primary_retry_sock,
							context->bridge->bind_address, false);

					ಯದಿ(rc == 0){
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = 0;
						mux__delete(context);
						net__socket_close(context);
						context->bridge->cur_address = 0;
					}
				}ಅನ್ಯಥಾ{
					len = ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ);
					ಯದಿ(!getsockopt(context->bridge->primary_retry_sock, SOL_SOCKET, SO_ERROR, (ವರ್ಣ *)&err, &len)){
						ಯದಿ(err == 0){
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = 0;
							mux__delete(context);
							net__socket_close(context);
							context->bridge->cur_address = context->bridge->address_ಎಣಿಕೆ-1;
						}ಅನ್ಯಥಾ{
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = db.now_s+5;
							loop__update_next_event(5000);
						}
					}ಅನ್ಯಥಾ{
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = db.now_s+5;
						loop__update_next_event(5000);
					}
				}
			}
		}ಅನ್ಯಥಾ{
			ಯದಿ(reload_if_needed(context)){
				ಅನುವರ್ತ;
			}

			/* Want to try to restart the bridge connection */
			ಯದಿ(!context->bridge->restart_t){
				bridge__update_backoff(context->bridge);
				context->bridge->restart_t = 1000*db.now_s+context->bridge->restart_timeout;
				context->bridge->cur_address++;
				ಯದಿ(context->bridge->cur_address == context->bridge->address_ಎಣಿಕೆ){
					context->bridge->cur_address = 0;
				}
				loop__update_next_event(context->bridge->restart_timeout);
			}ಅನ್ಯಥಾ{
				ಯದಿ((context->bridge->start_type == bst_lazy && context->bridge->lazy_reconnect)
						|| (context->bridge->start_type == bst_automatic && 1000*db.now_s >= context->bridge->restart_t)){

#ಯದಿ defined(__GLIBC__) && defined(WITH_ADNS)
					ಯದಿ(context->adns){
						/* Connection attempted, waiting on DNS lookup */
						rc = gai_error(context->adns);
						ಯದಿ(rc == EAI_INPROGRESS){
							/* Just keep on waiting */
						}ಅನ್ಯಥಾ ಯದಿ(rc == 0){
							rc = bridge__connect_step2(context);
							ಯದಿ(rc == MOSQ_ERR_SUCCESS){
								mux__new(context);
								ಯದಿ(context->out_packet){
									mux__add_out(context);
								}
							}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_CONN_PENDING){
								mux__new(context);
								mux__add_out(context);
								context->bridge->restart_t = 0;
							}ಅನ್ಯಥಾ{
								context->bridge->cur_address++;
								ಯದಿ(context->bridge->cur_address == context->bridge->address_ಎಣಿಕೆ){
									context->bridge->cur_address = 0;
								}
								context->bridge->restart_t = 0;
							}
						}ಅನ್ಯಥಾ{
							/* Need to retry */
							ಯದಿ(context->adns->ar_result){
								freeaddrinfo(context->adns->ar_result);
							}
							mosquitto_FREE(context->adns);
							context->bridge->restart_t = 0;
						}
					}ಅನ್ಯಥಾ{
						rc = bridge__connect_step1(context);
						ಯದಿ(rc){
							context->bridge->cur_address++;
							ಯದಿ(context->bridge->cur_address == context->bridge->address_ಎಣಿಕೆ){
								context->bridge->cur_address = 0;
							}
						}ಅನ್ಯಥಾ{
							/* Short wait for ADNS lookup */
							context->bridge->restart_t = 1;
							loop__update_next_event(1000);
						}
					}
#ಅನ್ಯಥಾ
					{
						rc = bridge__connect(context);
						context->bridge->restart_t = 0;
						ಯದಿ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING){
							ಯದಿ(context->bridge->round_robin == false && context->bridge->cur_address != 0){
								context->bridge->primary_retry = db.now_s + 5;
								loop__update_next_event(5000);
							}
							mux__new(context);
							ಯದಿ(context->out_packet){
								mux__add_out(context);
							}
						}ಅನ್ಯಥಾ{
							context->bridge->cur_address++;
							ಯದಿ(context->bridge->cur_address == context->bridge->address_ಎಣಿಕೆ){
								context->bridge->cur_address = 0;
							}
						}
					}
#endif
				}
			}
		}
	}
}

#endif
