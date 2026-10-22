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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <errno.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#ifndef WIN32
#അന്തര്ഭാവ <netdb.h>
#അന്തര്ഭാവ <sys/socket.h>
#അന്തര്ഭാവ <netinet/in.h>
#അന്തര്ഭാവ <netinet/tcp.h>
#അല്ലെങ്കിൽ
#അന്തര്ഭാവ <winsock2.h>
#അന്തര്ഭാവ <ws2tcpip.h>
#endif

#ifndef WIN32
#അന്തര്ഭാവ <unistd.h>
#അല്ലെങ്കിൽ
#അന്തര്ഭാവ <process.h>
#അന്തര്ഭാവ <winsock2.h>
#അന്തര്ഭാവ <ws2tcpip.h>
#endif

#അന്തര്ഭാവ "mosquitto.h"
#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto_internal.h"
#അന്തര്ഭാവ "net_mosq.h"
#അന്തര്ഭാവ "packet_mosq.h"
#അന്തര്ഭാവ "property_common.h"
#അന്തര്ഭാവ "send_mosq.h"
#അന്തര്ഭാവ "sys_tree.h"
#അന്തര്ഭാവ "tls_mosq.h"
#അന്തര്ഭാവ "util_mosq.h"
#അന്തര്ഭാവ "will_mosq.h"
#അന്തര്ഭാവ "utlist.h"

#ifdef WITH_BRIDGE

സ്ഥിതം ശൂന്യം bridge__update_backoff(ഘടന mosquitto__bridge *bridge);
#എങ്കിൽ defined(__GLIBC__) && defined(WITH_ADNS)
സ്ഥിതം പൂർണ്ണസംഖ്യ bridge__connect_step1(ഘടന mosquitto *context);
സ്ഥിതം പൂർണ്ണസംഖ്യ bridge__connect_step2(ഘടന mosquitto *context);
#endif
സ്ഥിതം ശൂന്യം bridge__packet_cleanup(ഘടന mosquitto *context);

സ്ഥിതം ഘടന mosquitto *bridge__new(ഘടന mosquitto__bridge *bridge)
{
	ഘടന mosquitto *new_context = ശൂന്യ;
	ഘടന mosquitto **bridges;
	അക്ഷരം *local_id;

	പ്രതിജ്ഞാ(bridge);

	local_id = mosquitto_strdup(bridge->local_clientid);
	എങ്കിൽ(!local_id){
		തിരികെ ശൂന്യ;
	}

	HASH_FIND(hh_id, db.contexts_by_id, local_id, strlen(local_id), new_context);
	എങ്കിൽ(new_context){
		/* (possible from persistent db) */
		mosquitto_FREE(local_id);
	}അല്ലെങ്കിൽ{
		/* id wasn't found, so generate a new context */
		new_context = context__init();
		എങ്കിൽ(!new_context){
			mosquitto_FREE(local_id);
			തിരികെ ശൂന്യ;
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
	new_context->tls_engine = ശൂന്യ;
	new_context->tls_keyform = mosq_k_pem;
	new_context->tls_use_os_certs = bridge->tls_use_os_certs;
	new_context->ssl_ctx_defaults = true;
#ifdef FINAL_WITH_TLS_PSK
	new_context->tls_psk_identity = bridge->tls_psk_identity;
	new_context->tls_psk = bridge->tls_psk;
#endif
#endif

	bridge->try_private_accepted = true;
	എങ്കിൽ(bridge->clean_start_local == -1){
		/* default to "regular" clean start setting */
		bridge->clean_start_local = bridge->clean_start;
	}
	new_context->retain_available = bridge->outgoing_retain;
	new_context->protocol = bridge->protocol_version;
	എങ്കിൽ(!bridge->clean_start_local){
		new_context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
		plugin_persist__handle_client_add(new_context);
		എങ്കിൽ(new_context->expiry_list_item){
			/* We've restored from persistence and been added to the session
			 * expiry list, even though we should never be expired */
			session_expiry__remove(new_context);
		}
	}

	bridges = mosquitto_realloc(db.bridges, (size_t)(db.bridge_എണ്ണം+1)*വലുപ്പം(ഘടന mosquitto *));
	എങ്കിൽ(bridges){
		db.bridges = bridges;
		db.bridge_എണ്ണം++;
		db.bridges[db.bridge_എണ്ണം-1] = new_context;
	}അല്ലെങ്കിൽ{
		തിരികെ ശൂന്യ;
	}

	തിരികെ new_context;
}


സ്ഥിതം ശൂന്യം bridge__destroy(ഘടന mosquitto *context)
{
	send__disconnect(context, MQTT_RC_SUCCESS, ശൂന്യ);
	context__cleanup(context, true);
}


ശൂന്യം bridge__start_all(ശൂന്യം)
{
	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<db.config->bridge_എണ്ണം; i++){
		ഘടന mosquitto *context;
		പൂർണ്ണസംഖ്യ ret;

		context = bridge__new(db.config->bridges[i]);
		എങ്കിൽ(!context){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
			തിരികെ;
		}
		പ്രതിജ്ഞാ(context);

#എങ്കിൽ defined(__GLIBC__) && defined(WITH_ADNS)
		context->bridge->restart_t = 1; /* force quick restart of bridge */
		loop__update_next_event(1000);
		ret = bridge__connect_step1(context);
#അല്ലെങ്കിൽ
		ret = bridge__connect(context);
#endif

		എങ്കിൽ(ret && ret != MOSQ_ERR_CONN_PENDING){
			log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "Warning: Unable to connect bridge %s.",
					context->bridge->name);
		}

		db.config->bridges[i] = ശൂന്യ;
	}
}


സ്ഥിതം പൂർണ്ണസംഖ്യ bridge__set_tcp_keepalive(ഘടന mosquitto *context)
{
	unsigned പൂർണ്ണസംഖ്യ enabled = 1;
	bool ret;

	എങ്കിൽ(context->bridge->tcp_keepalive_idle == 0
			|| context->bridge->tcp_keepalive_interval == 0
			|| context->bridge->tcp_keepalive_counter == 0){

		തിരികെ MOSQ_ERR_SUCCESS;
	}

#ifdef WIN32
#  define SETSOCKOPT_TYPE അക്ഷരം *
#അല്ലെങ്കിൽ
#  define SETSOCKOPT_TYPE സ്ഥിരം ശൂന്യം *
#endif

	ret = setsockopt(context->sock, SOL_SOCKET, SO_KEEPALIVE, (SETSOCKOPT_TYPE)&enabled, വലുപ്പം(enabled));

#ifdef TCP_KEEPIDLE
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPIDLE,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_idle, വലുപ്പം(context->bridge->tcp_keepalive_idle));
#endif
#ifdef TCP_KEEPINTVL
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPINTVL,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_interval, വലുപ്പം(context->bridge->tcp_keepalive_interval));
#endif
#ifdef TCP_KEEPCNT
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPCNT,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_counter, വലുപ്പം(context->bridge->tcp_keepalive_counter));
#endif

#undef SETSOCKOPT_TYPE

	എങ്കിൽ(ret){
		തിരികെ MOSQ_ERR_UNKNOWN;
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TCP_USER_TIMEOUT


സ്ഥിതം പൂർണ്ണസംഖ്യ bridge__set_tcp_user_timeout(ഘടന mosquitto *context)
{
	പൂർണ്ണസംഖ്യ timeout = context->bridge->tcp_user_timeout;
	എങ്കിൽ(timeout >= 0){
		എങ്കിൽ(setsockopt(context->sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (അക്ഷരം *)&timeout, വലുപ്പം(timeout))){
			തിരികെ MOSQ_ERR_UNKNOWN;
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}
#endif

#എങ്കിൽ defined(__GLIBC__) && defined(WITH_ADNS)


സ്ഥിതം പൂർണ്ണസംഖ്യ bridge__connect_step1(ഘടന mosquitto *context)
{
	പൂർണ്ണസംഖ്യ rc;
	അക്ഷരം *notification_topic;
	size_t notification_topic_len;
	uint8_t notification_payload;
	ഘടന mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	എങ്കിൽ(!context || !context->bridge){
		തിരികെ MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_സന്ദേശം_in = db.now_s;
	context->next_സന്ദേശം_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = ശൂന്യ;
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
		എങ്കിൽ(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			എങ്കിൽ(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}അല്ലെങ്കിൽ{
				qos = cur_topic->qos;
			}
			ഘടന mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			എങ്കിൽ(sub__add(context, &sub) > 0){
				തിരികെ 1;
			}
			retain__queue(context, &sub);
		}
	}

	എങ്കിൽ(context->bridge->notifications){
		എങ്കിൽ(context->max_qos == 0){
			qos = 0;
		}അല്ലെങ്കിൽ{
			qos = 1;
		}
		എങ്കിൽ(context->bridge->notification_topic){
			എങ്കിൽ(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, സന്ദേശം_EXPIRY_INFINITE, ശൂന്യ);
				context->bridge->initial_notification_done = true;
			}
			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, ശൂന്യ);
			എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
				തിരികെ rc;
			}
		}അല്ലെങ്കിൽ{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(വലുപ്പം(അക്ഷരം)*(notification_topic_len+1));
			എങ്കിൽ(!notification_topic){
				തിരികെ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			എങ്കിൽ(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, സന്ദേശം_EXPIRY_INFINITE, ശൂന്യ);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, ശൂന്യ);
			mosquitto_FREE(notification_topic);
			എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
				തിരികെ rc;
			}
		}
	}

	log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Connecting bridge (step 1) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step1(context, context->bridge->addresses[context->bridge->cur_address].address);
	എങ്കിൽ(rc > 0){
		എങ്കിൽ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			തിരികെ rc; /* Error already printed */
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_ERRNO){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_EAI){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		തിരികെ rc;
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ bridge__connect_step2(ഘടന mosquitto *context)
{
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(!context || !context->bridge){
		തിരികെ MOSQ_ERR_INVAL;
	}

	log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Connecting bridge (step 2) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step2(context, context->bridge->addresses[context->bridge->cur_address].port, &context->sock);
	എങ്കിൽ(rc > 0){
		എങ്കിൽ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			തിരികെ rc; /* Error already printed */
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_ERRNO){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_EAI){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		തിരികെ rc;
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, വലുപ്പം(context->sock), context);

	എങ്കിൽ(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}
	തിരികെ rc;
}


പൂർണ്ണസംഖ്യ bridge__connect_step3(ഘടന mosquitto *context)
{
	പൂർണ്ണസംഖ്യ rc;
	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = ശൂന്യ;

	rc = net__socket_connect_step3(context, context->bridge->addresses[context->bridge->cur_address].address);
	എങ്കിൽ(rc > 0){
		എങ്കിൽ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			തിരികെ rc; /* Error already printed */
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_ERRNO){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_EAI){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		തിരികെ rc;
	}

	എങ്കിൽ(context->bridge->round_robin == false && context->bridge->cur_address != 0){
		context->bridge->primary_retry = db.now_s + 5;
		loop__update_next_event(5000);
	}

	എങ്കിൽ(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		തിരികെ MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	എങ്കിൽ(bridge__set_tcp_user_timeout(context)){
		തിരികെ MOSQ_ERR_UNKNOWN;
	}
#endif

	എങ്കിൽ(context->bridge->receive_maximum != 0){
		receive_maximum.മൂല്യം.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	എങ്കിൽ(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.മൂല്യം.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	എങ്കിൽ(context->bridge->max_topic_alias != 0){
		topic_alias_max.മൂല്യം.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc = send__connect(context, context->keepalive, context->clean_start, properties);
	എങ്കിൽ(rc == MOSQ_ERR_SUCCESS){
		തിരികെ MOSQ_ERR_SUCCESS;
	}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		തിരികെ MOSQ_ERR_SUCCESS;
	}അല്ലെങ്കിൽ{
		എങ്കിൽ(rc == MOSQ_ERR_TLS){
			തിരികെ rc; /* Error already printed */
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_ERRNO){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_EAI){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		തിരികെ rc;
	}
}
#അല്ലെങ്കിൽ


പൂർണ്ണസംഖ്യ bridge__connect(ഘടന mosquitto *context)
{
	പൂർണ്ണസംഖ്യ rc, rc2;
	അക്ഷരം *notification_topic = ശൂന്യ;
	size_t notification_topic_len;
	uint8_t notification_payload;
	ഘടന mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = ശൂന്യ;

	എങ്കിൽ(!context || !context->bridge){
		തിരികെ MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->last_സന്ദേശം_in = db.now_s;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->next_സന്ദേശം_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = ശൂന്യ;
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
		എങ്കിൽ(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			എങ്കിൽ(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}അല്ലെങ്കിൽ{
				qos = cur_topic->qos;
			}
			ഘടന mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			എങ്കിൽ(sub__add(context, &sub) > 0){
				തിരികെ 1;
			}
		}
	}

	എങ്കിൽ(context->bridge->notifications){
		എങ്കിൽ(context->max_qos == 0){
			qos = 0;
		}അല്ലെങ്കിൽ{
			qos = 1;
		}
		എങ്കിൽ(context->bridge->notification_topic){
			എങ്കിൽ(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, സന്ദേശം_EXPIRY_INFINITE, ശൂന്യ);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, ശൂന്യ);
			എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
				തിരികെ rc;
			}
		}അല്ലെങ്കിൽ{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(വലുപ്പം(അക്ഷരം)*(notification_topic_len+1));
			എങ്കിൽ(!notification_topic){
				തിരികെ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			എങ്കിൽ(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, സന്ദേശം_EXPIRY_INFINITE, ശൂന്യ);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, ശൂന്യ);
			എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
				mosquitto_FREE(notification_topic);
				തിരികെ rc;
			}
			mosquitto_FREE(notification_topic);
		}
	}

	log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Connecting bridge %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__socket_connect(context,
			context->bridge->addresses[context->bridge->cur_address].address,
			context->bridge->addresses[context->bridge->cur_address].port,
			context->bridge->bind_address,
			false);

	എങ്കിൽ(rc > 0){
		എങ്കിൽ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			തിരികെ rc; /* Error already printed */
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_ERRNO){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_EAI){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		തിരികെ rc;
	}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, വലുപ്പം(context->sock), context);

	എങ്കിൽ(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		തിരികെ MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	എങ്കിൽ(bridge__set_tcp_user_timeout(context)){
		തിരികെ MOSQ_ERR_UNKNOWN;
	}
#endif

	എങ്കിൽ(context->bridge->receive_maximum != 0){
		receive_maximum.മൂല്യം.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	എങ്കിൽ(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.മൂല്യം.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	എങ്കിൽ(context->bridge->max_topic_alias != 0){
		topic_alias_max.മൂല്യം.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc2 = send__connect(context, context->keepalive, context->clean_start, properties);
	എങ്കിൽ(rc2 == MOSQ_ERR_SUCCESS){
		തിരികെ rc;
	}അല്ലെങ്കിൽ എങ്കിൽ(rc2 == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		തിരികെ MOSQ_ERR_SUCCESS;
	}അല്ലെങ്കിൽ{
		എങ്കിൽ(rc2 == MOSQ_ERR_TLS){
			തിരികെ rc2; /* Error already printed */
		}അല്ലെങ്കിൽ എങ്കിൽ(rc2 == MOSQ_ERR_ERRNO){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}അല്ലെങ്കിൽ എങ്കിൽ(rc2 == MOSQ_ERR_EAI){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		തിരികെ rc2;
	}
}
#endif


പൂർണ്ണസംഖ്യ bridge__on_connect(ഘടന mosquitto *context)
{
	അക്ഷരം *notification_topic;
	size_t notification_topic_len;
	ഘടന mosquitto__bridge_topic *cur_topic;
	പൂർണ്ണസംഖ്യ sub_opts;
	bool retain = true;
	uint8_t qos;

	എങ്കിൽ(context->bridge->notifications){
		എങ്കിൽ(context->max_qos == 0){
			qos = 0;
		}അല്ലെങ്കിൽ{
			qos = 1;
		}
		എങ്കിൽ(!context->retain_available){
			retain = false;
		}
		അക്ഷരം notification_payload = '1';
		എങ്കിൽ(context->bridge->notification_topic){
			എങ്കിൽ(!context->bridge->notifications_local_only){
				എങ്കിൽ(send__real_publish(context, mosquitto__mid_generate(context),
						context->bridge->notification_topic, 1, &notification_payload, qos, retain, 0, 0, ശൂന്യ, 0)){

					തിരികെ 1;
				}
			}
			db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, സന്ദേശം_EXPIRY_INFINITE, ശൂന്യ);
		}അല്ലെങ്കിൽ{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(വലുപ്പം(അക്ഷരം)*(notification_topic_len+1));
			എങ്കിൽ(!notification_topic){
				തിരികെ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);
			notification_payload = '1';
			എങ്കിൽ(!context->bridge->notifications_local_only){
				എങ്കിൽ(send__real_publish(context, mosquitto__mid_generate(context),
						notification_topic, 1, &notification_payload, qos, retain, 0, 0, ശൂന്യ, 0)){

					mosquitto_FREE(notification_topic);
					തിരികെ 1;
				}
			}
			db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, സന്ദേശം_EXPIRY_INFINITE, ശൂന്യ);
			mosquitto_FREE(notification_topic);
		}
	}

	LL_FOREACH(context->bridge->topics, cur_topic){
		എങ്കിൽ(cur_topic->direction == bd_in || cur_topic->direction == bd_both){
			എങ്കിൽ(cur_topic->qos > context->max_qos){
				sub_opts = context->max_qos;
			}അല്ലെങ്കിൽ{
				sub_opts = cur_topic->qos;
			}
			എങ്കിൽ(context->bridge->protocol_version == mosq_p_mqtt5){
				sub_opts = sub_opts
						| MQTT_SUB_OPT_NO_LOCAL
						| MQTT_SUB_OPT_RETAIN_AS_PUBLISHED
						| MQTT_SUB_OPT_SEND_RETAIN_ALWAYS;
			}
			എങ്കിൽ(send__subscribe(context, ശൂന്യ, 1, &cur_topic->remote_topic, sub_opts, ശൂന്യ)){
				തിരികെ 1;
			}
		}അല്ലെങ്കിൽ{
			എങ്കിൽ(context->bridge->attempt_unsubscribe){
				എങ്കിൽ(send__unsubscribe(context, ശൂന്യ, 1, &cur_topic->remote_topic, ശൂന്യ)){
					/* direction = inwards only. This means we should not be subscribed
					* to the topic. It is possible that we used to be subscribed to
					* this topic so unsubscribe. */
					തിരികെ 1;
				}
			}
		}
	}
	ഘടന mosquitto_subscription sub;
	memset(&sub, 0, വലുപ്പം(sub));
	LL_FOREACH(context->bridge->topics, cur_topic){
		sub.topic_filter = cur_topic->local_topic;
		എങ്കിൽ(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			എങ്കിൽ(cur_topic->qos > context->max_qos){
				sub.options = context->max_qos;
			}അല്ലെങ്കിൽ{
				sub.options = cur_topic->qos;
			}
			retain__queue(context, &sub);
		}
	}

	context->bridge->connected_at = db.now_s;

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ bridge__register_local_connections(ശൂന്യം)
{
	ഘടന mosquitto *context, *ctxt_tmp = ശൂന്യ;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		എങ്കിൽ(context->bridge){
			എങ്കിൽ(mux__new(context)){
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error in initial bridge registration: %s", strerror(errno));
				തിരികെ MOSQ_ERR_UNKNOWN;
			}
			mux__add_out(context);
		}
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


ശൂന്യം bridge__reload(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ i;
	പൂർണ്ണസംഖ്യ j;

	// destroy old bridges that dissappeared
	വേണ്ടി(i=0; i<db.bridge_എണ്ണം; i++){
		വേണ്ടി(j=0; j<db.config->bridge_എണ്ണം; j++){
			എങ്കിൽ(!strcmp(db.bridges[i]->bridge->name, db.config->bridges[j]->name)){
				നിർത്തുക;
			}
		}

		എങ്കിൽ(j==db.config->bridge_എണ്ണം){
			bridge__destroy(db.bridges[i]);
		}
	}

	വേണ്ടി(i=0; i<db.config->bridge_എണ്ണം; i++){
		വേണ്ടി(j=0; j<db.bridge_എണ്ണം; j++){
			എങ്കിൽ(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
				നിർത്തുക;
			}
		}

		എങ്കിൽ(j==db.bridge_എണ്ണം){
			// a new bridge was found, create it
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ശൂന്യ;
			തുടരുക;
		}

		എങ്കിൽ(db.config->bridges[i]->reload_type == brt_immediate){
			// in this case, an existing bridge should match
			വേണ്ടി(j=0; j<db.bridge_എണ്ണം; j++){
				എങ്കിൽ(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
					നിർത്തുക;
				}
			}

			പ്രതിജ്ഞാ(j<db.bridge_എണ്ണം);
			db.bridges[j]->will_delay_interval = 0;
			bridge__destroy(db.bridges[j]);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ശൂന്യ;
		}
	}
}


ശൂന്യം bridge__db_cleanup(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ i;

	വേണ്ടി(i=0; i<db.bridge_എണ്ണം; i++){
		എങ്കിൽ(db.bridges[i]){
			context__cleanup(db.bridges[i], true);
		}
	}
	mosquitto_FREE(db.bridges);
}


ശൂന്യം bridge__cleanup(ഘടന mosquitto *context)
{
	പൂർണ്ണസംഖ്യ i;

	പ്രതിജ്ഞാ(db.bridge_എണ്ണം > 0);

	വേണ്ടി(i=0; i<db.bridge_എണ്ണം; i++){
		എങ്കിൽ(db.bridges[i] == context){
			db.bridges[i] = db.bridges[db.bridge_എണ്ണം-1];
			നിർത്തുക;
		}
	}

	db.bridge_എണ്ണം--;
	എങ്കിൽ(db.bridge_എണ്ണം == 0){
		mosquitto_FREE(db.bridges);
	}അല്ലെങ്കിൽ{
		db.bridges = mosquitto_realloc(db.bridges, (unsigned)db.bridge_എണ്ണം * വലുപ്പം(db.bridges[0]));
	}

	mosquitto_FREE(context->bridge->name);
	mosquitto_FREE(context->bridge->local_clientid);
	mosquitto_FREE(context->bridge->local_username);
	mosquitto_FREE(context->bridge->local_password);
#ifdef WITH_TLS
	mosquitto_FREE(context->bridge->tls_certfile);
	mosquitto_FREE(context->bridge->tls_keyfile);
#endif

	എങ്കിൽ(context->bridge->remote_clientid != context->id){
		mosquitto_FREE(context->bridge->remote_clientid);
	}
	context->bridge->remote_clientid = ശൂന്യ;

	എങ്കിൽ(context->bridge->remote_username != context->username){
		mosquitto_FREE(context->bridge->remote_username);
	}
	context->bridge->remote_username = ശൂന്യ;

	എങ്കിൽ(context->bridge->remote_password != context->password){
		mosquitto_FREE(context->bridge->remote_password);
	}
	context->bridge->remote_password = ശൂന്യ;
#ifdef WITH_TLS
	എങ്കിൽ(context->ssl_ctx){
		SSL_CTX_free(context->ssl_ctx);
		context->ssl_ctx = ശൂന്യ;
	}
#endif

	വേണ്ടി(i=0; i<context->bridge->address_എണ്ണം; i++){
		mosquitto_FREE(context->bridge->addresses[i].address);
	}

	mosquitto_FREE(context->bridge->addresses);

	config__bridge_cleanup(context->bridge);
	context->bridge = ശൂന്യ;
}


സ്ഥിതം ശൂന്യം bridge__packet_cleanup(ഘടന mosquitto *context)
{
	ഘടന mosquitto__packet *packet;
	എങ്കിൽ(!context){
		തിരികെ;
	}

	ആകുമ്പോൾ(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_FREE(packet);
	}
	context->out_packet = ശൂന്യ;
	context->out_packet_last = ശൂന്യ;
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_എണ്ണം);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_എണ്ണം = 0;
	context->out_packet_bytes = 0;

	packet__cleanup(&(context->in_packet));
}


സ്ഥിതം പൂർണ്ണസംഖ്യ rand_between(പൂർണ്ണസംഖ്യ താഴെയുള്ള, പൂർണ്ണസംഖ്യ മുകളിലെ)
{
	പൂർണ്ണസംഖ്യ r;
	mosquitto_getrandom(&r, വലുപ്പം(പൂർണ്ണസംഖ്യ));
	തിരികെ (abs(r) % (മുകളിലെ - താഴെയുള്ള)) + താഴെയുള്ള;
}


സ്ഥിതം ശൂന്യം bridge__backoff_step(ഘടന mosquitto__bridge *bridge)
{
	/*
		“Decorrelated Jitter” calculation, according to:
			https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
	*/

	bridge->restart_timeout = rand_between(bridge->backoff_base, bridge->restart_timeout * 3);
	എങ്കിൽ(bridge->restart_timeout > bridge->backoff_cap){
		bridge->restart_timeout = bridge->backoff_cap;
	}
}


സ്ഥിതം ശൂന്യം bridge__backoff_reset(ഘടന mosquitto__bridge *bridge)
{
	bridge->restart_timeout = bridge->backoff_base;
}


സ്ഥിതം ശൂന്യം bridge__update_backoff(ഘടന mosquitto__bridge *bridge)
{
	എങ്കിൽ(!bridge){
		തിരികെ;
	}
	എങ്കിൽ(!bridge->backoff_cap){
		/* skip if not using jitter */
		തിരികെ;

	}
	എങ്കിൽ(bridge->connected_at && db.now_s - bridge->connected_at >= bridge->stable_connection_period){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Bridge %s connection was stable enough, resetting backoff", bridge->name);
		bridge__backoff_reset(bridge);
	}അല്ലെങ്കിൽ{
		bridge__backoff_step(bridge);
	}

	bridge->connected_at = 0;

	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Bridge %s next backoff will be %d ms", bridge->name, bridge->restart_timeout);
}


സ്ഥിതം ശൂന്യം bridge_check_pending(ഘടന mosquitto *context)
{
	പൂർണ്ണസംഖ്യ err;
	socklen_t len;

	എങ്കിൽ(context->state == mosq_cs_connect_pending){
		len = വലുപ്പം(പൂർണ്ണസംഖ്യ);
		എങ്കിൽ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (അക്ഷരം *)&err, &len)){
			എങ്കിൽ(err == 0){
				mosquitto__set_state(context, mosq_cs_new);
#എങ്കിൽ defined(WITH_ADNS) && defined(WITH_BRIDGE)
				എങ്കിൽ(context->bridge){
					bridge__connect_step3(context);
				}
#endif
			}അല്ലെങ്കിൽ എങ്കിൽ(err == ECONNREFUSED){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				തിരികെ;
			}
		}അല്ലെങ്കിൽ{
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			തിരികെ;
		}
	}
}


സ്ഥിതം bool reload_if_needed(ഘടന mosquitto *context)
{
	പൂർണ്ണസംഖ്യ i;

	വേണ്ടി(i=0; i<db.config->bridge_എണ്ണം; i++){
		എങ്കിൽ(db.config->bridges[i] && !strcmp(context->bridge->name, db.config->bridges[i]->name)){
			bridge__destroy(context);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ശൂന്യ;
			loop__update_next_event(100);
			തിരികെ true;
		}
	}

	തിരികെ false;
}


ശൂന്യം bridge_check(ശൂന്യം)
{
	സ്ഥിതം time_t last_check = 0;
	ഘടന mosquitto *context = ശൂന്യ;
	socklen_t len;
	പൂർണ്ണസംഖ്യ i;
	പൂർണ്ണസംഖ്യ rc;
	പൂർണ്ണസംഖ്യ err;

	എങ്കിൽ(db.now_s <= last_check){
		തിരികെ;
	}

	വേണ്ടി(i=0; i<db.bridge_എണ്ണം; i++){
		എങ്കിൽ(!db.bridges[i]){
			തുടരുക;
		}

		context = db.bridges[i];

		എങ്കിൽ(net__is_connected(context)){
			mosquitto__check_keepalive(context);
			bridge_check_pending(context);

			/* Check for bridges that are not round robin and not currently
			 * connected to their primary broker. */
			എങ്കിൽ(context->bridge->round_robin == false
					&& context->bridge->cur_address != 0
					&& context->bridge->primary_retry
					&& db.now_s >= context->bridge->primary_retry){

				എങ്കിൽ(context->bridge->primary_retry_sock == INVALID_SOCKET){
					rc = net__try_connect(context->bridge->addresses[0].address,
							context->bridge->addresses[0].port,
							&context->bridge->primary_retry_sock,
							context->bridge->bind_address, false);

					എങ്കിൽ(rc == 0){
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = 0;
						mux__delete(context);
						net__socket_close(context);
						context->bridge->cur_address = 0;
					}
				}അല്ലെങ്കിൽ{
					len = വലുപ്പം(പൂർണ്ണസംഖ്യ);
					എങ്കിൽ(!getsockopt(context->bridge->primary_retry_sock, SOL_SOCKET, SO_ERROR, (അക്ഷരം *)&err, &len)){
						എങ്കിൽ(err == 0){
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = 0;
							mux__delete(context);
							net__socket_close(context);
							context->bridge->cur_address = context->bridge->address_എണ്ണം-1;
						}അല്ലെങ്കിൽ{
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = db.now_s+5;
							loop__update_next_event(5000);
						}
					}അല്ലെങ്കിൽ{
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = db.now_s+5;
						loop__update_next_event(5000);
					}
				}
			}
		}അല്ലെങ്കിൽ{
			എങ്കിൽ(reload_if_needed(context)){
				തുടരുക;
			}

			/* Want to try to restart the bridge connection */
			എങ്കിൽ(!context->bridge->restart_t){
				bridge__update_backoff(context->bridge);
				context->bridge->restart_t = 1000*db.now_s+context->bridge->restart_timeout;
				context->bridge->cur_address++;
				എങ്കിൽ(context->bridge->cur_address == context->bridge->address_എണ്ണം){
					context->bridge->cur_address = 0;
				}
				loop__update_next_event(context->bridge->restart_timeout);
			}അല്ലെങ്കിൽ{
				എങ്കിൽ((context->bridge->start_type == bst_lazy && context->bridge->lazy_reconnect)
						|| (context->bridge->start_type == bst_automatic && 1000*db.now_s >= context->bridge->restart_t)){

#എങ്കിൽ defined(__GLIBC__) && defined(WITH_ADNS)
					എങ്കിൽ(context->adns){
						/* Connection attempted, waiting on DNS lookup */
						rc = gai_error(context->adns);
						എങ്കിൽ(rc == EAI_INPROGRESS){
							/* Just keep on waiting */
						}അല്ലെങ്കിൽ എങ്കിൽ(rc == 0){
							rc = bridge__connect_step2(context);
							എങ്കിൽ(rc == MOSQ_ERR_SUCCESS){
								mux__new(context);
								എങ്കിൽ(context->out_packet){
									mux__add_out(context);
								}
							}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_CONN_PENDING){
								mux__new(context);
								mux__add_out(context);
								context->bridge->restart_t = 0;
							}അല്ലെങ്കിൽ{
								context->bridge->cur_address++;
								എങ്കിൽ(context->bridge->cur_address == context->bridge->address_എണ്ണം){
									context->bridge->cur_address = 0;
								}
								context->bridge->restart_t = 0;
							}
						}അല്ലെങ്കിൽ{
							/* Need to retry */
							എങ്കിൽ(context->adns->ar_result){
								freeaddrinfo(context->adns->ar_result);
							}
							mosquitto_FREE(context->adns);
							context->bridge->restart_t = 0;
						}
					}അല്ലെങ്കിൽ{
						rc = bridge__connect_step1(context);
						എങ്കിൽ(rc){
							context->bridge->cur_address++;
							എങ്കിൽ(context->bridge->cur_address == context->bridge->address_എണ്ണം){
								context->bridge->cur_address = 0;
							}
						}അല്ലെങ്കിൽ{
							/* Short wait for ADNS lookup */
							context->bridge->restart_t = 1;
							loop__update_next_event(1000);
						}
					}
#അല്ലെങ്കിൽ
					{
						rc = bridge__connect(context);
						context->bridge->restart_t = 0;
						എങ്കിൽ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING){
							എങ്കിൽ(context->bridge->round_robin == false && context->bridge->cur_address != 0){
								context->bridge->primary_retry = db.now_s + 5;
								loop__update_next_event(5000);
							}
							mux__new(context);
							എങ്കിൽ(context->out_packet){
								mux__add_out(context);
							}
						}അല്ലെങ്കിൽ{
							context->bridge->cur_address++;
							എങ്കിൽ(context->bridge->cur_address == context->bridge->address_എണ്ണം){
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
