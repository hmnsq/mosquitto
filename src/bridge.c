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

#ضمن "config.h"

#ضمن <assert.h>
#ضمن <errno.h>
#ضمن <stdio.h>
#ضمن <string.h>

#ifndef WIN32
#ضمن <netdb.h>
#ضمن <sys/socket.h>
#ضمن <netinet/in.h>
#ضمن <netinet/tcp.h>
#وإلا
#ضمن <winsock2.h>
#ضمن <ws2tcpip.h>
#endif

#ifndef WIN32
#ضمن <unistd.h>
#وإلا
#ضمن <process.h>
#ضمن <winsock2.h>
#ضمن <ws2tcpip.h>
#endif

#ضمن "mosquitto.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto_internal.h"
#ضمن "net_mosq.h"
#ضمن "packet_mosq.h"
#ضمن "property_common.h"
#ضمن "send_mosq.h"
#ضمن "sys_tree.h"
#ضمن "tls_mosq.h"
#ضمن "util_mosq.h"
#ضمن "will_mosq.h"
#ضمن "utlist.h"

#ifdef WITH_BRIDGE

ساكن فراغ bridge__update_backoff(بنية mosquitto__bridge *bridge);
#إذا defined(__GLIBC__) && defined(WITH_ADNS)
ساكن صحيح bridge__connect_step1(بنية mosquitto *context);
ساكن صحيح bridge__connect_step2(بنية mosquitto *context);
#endif
ساكن فراغ bridge__packet_cleanup(بنية mosquitto *context);

ساكن بنية mosquitto *bridge__new(بنية mosquitto__bridge *bridge)
{
	بنية mosquitto *new_context = عدم;
	بنية mosquitto **bridges;
	حرف *local_id;

	أكد(bridge);

	local_id = mosquitto_strdup(bridge->local_clientid);
	إذا(!local_id){
		إرجاع عدم;
	}

	HASH_FIND(hh_id, db.contexts_by_id, local_id, strlen(local_id), new_context);
	إذا(new_context){
		/* (possible from persistent db) */
		mosquitto_FREE(local_id);
	}وإلا{
		/* id wasn't found, so generate a new context */
		new_context = context__init();
		إذا(!new_context){
			mosquitto_FREE(local_id);
			إرجاع عدم;
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
	new_context->tls_engine = عدم;
	new_context->tls_keyform = mosq_k_pem;
	new_context->tls_use_os_certs = bridge->tls_use_os_certs;
	new_context->ssl_ctx_defaults = true;
#ifdef FINAL_WITH_TLS_PSK
	new_context->tls_psk_identity = bridge->tls_psk_identity;
	new_context->tls_psk = bridge->tls_psk;
#endif
#endif

	bridge->try_private_accepted = true;
	إذا(bridge->clean_start_local == -1){
		/* default to "regular" clean start setting */
		bridge->clean_start_local = bridge->clean_start;
	}
	new_context->retain_available = bridge->outgoing_retain;
	new_context->protocol = bridge->protocol_version;
	إذا(!bridge->clean_start_local){
		new_context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
		plugin_persist__handle_client_add(new_context);
		إذا(new_context->expiry_list_item){
			/* We've restored from persistence and been added to the session
			 * expiry list, even though we should never be expired */
			session_expiry__remove(new_context);
		}
	}

	bridges = mosquitto_realloc(db.bridges, (size_t)(db.bridge_تعداد+1)*حجم(بنية mosquitto *));
	إذا(bridges){
		db.bridges = bridges;
		db.bridge_تعداد++;
		db.bridges[db.bridge_تعداد-1] = new_context;
	}وإلا{
		إرجاع عدم;
	}

	إرجاع new_context;
}


ساكن فراغ bridge__destroy(بنية mosquitto *context)
{
	send__disconnect(context, MQTT_RC_SUCCESS, عدم);
	context__cleanup(context, true);
}


فراغ bridge__start_all(فراغ)
{
	لكل(صحيح i=0; i<db.config->bridge_تعداد; i++){
		بنية mosquitto *context;
		صحيح ret;

		context = bridge__new(db.config->bridges[i]);
		إذا(!context){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
			إرجاع;
		}
		أكد(context);

#إذا defined(__GLIBC__) && defined(WITH_ADNS)
		context->bridge->restart_t = 1; /* force quick restart of bridge */
		loop__update_next_event(1000);
		ret = bridge__connect_step1(context);
#وإلا
		ret = bridge__connect(context);
#endif

		إذا(ret && ret != MOSQ_ERR_CONN_PENDING){
			log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Unable to connect bridge %s.",
					context->bridge->name);
		}

		db.config->bridges[i] = عدم;
	}
}


ساكن صحيح bridge__set_tcp_keepalive(بنية mosquitto *context)
{
	unsigned صحيح enabled = 1;
	bool ret;

	إذا(context->bridge->tcp_keepalive_idle == 0
			|| context->bridge->tcp_keepalive_interval == 0
			|| context->bridge->tcp_keepalive_counter == 0){

		إرجاع MOSQ_ERR_SUCCESS;
	}

#ifdef WIN32
#  define SETSOCKOPT_TYPE حرف *
#وإلا
#  define SETSOCKOPT_TYPE ثابت فراغ *
#endif

	ret = setsockopt(context->sock, SOL_SOCKET, SO_KEEPALIVE, (SETSOCKOPT_TYPE)&enabled, حجم(enabled));

#ifdef TCP_KEEPIDLE
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPIDLE,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_idle, حجم(context->bridge->tcp_keepalive_idle));
#endif
#ifdef TCP_KEEPINTVL
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPINTVL,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_interval, حجم(context->bridge->tcp_keepalive_interval));
#endif
#ifdef TCP_KEEPCNT
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPCNT,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_counter, حجم(context->bridge->tcp_keepalive_counter));
#endif

#undef SETSOCKOPT_TYPE

	إذا(ret){
		إرجاع MOSQ_ERR_UNKNOWN;
	}

	إرجاع MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TCP_USER_TIMEOUT


ساكن صحيح bridge__set_tcp_user_timeout(بنية mosquitto *context)
{
	صحيح timeout = context->bridge->tcp_user_timeout;
	إذا(timeout >= 0){
		إذا(setsockopt(context->sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (حرف *)&timeout, حجم(timeout))){
			إرجاع MOSQ_ERR_UNKNOWN;
		}
	}

	إرجاع MOSQ_ERR_SUCCESS;
}
#endif

#إذا defined(__GLIBC__) && defined(WITH_ADNS)


ساكن صحيح bridge__connect_step1(بنية mosquitto *context)
{
	صحيح rc;
	حرف *notification_topic;
	size_t notification_topic_len;
	uint8_t notification_payload;
	بنية mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	إذا(!context || !context->bridge){
		إرجاع MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_رسالة_in = db.now_s;
	context->next_رسالة_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = عدم;
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
		إذا(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(عدم, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			إذا(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}وإلا{
				qos = cur_topic->qos;
			}
			بنية mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			إذا(sub__add(context, &sub) > 0){
				إرجاع 1;
			}
			retain__queue(context, &sub);
		}
	}

	إذا(context->bridge->notifications){
		إذا(context->max_qos == 0){
			qos = 0;
		}وإلا{
			qos = 1;
		}
		إذا(context->bridge->notification_topic){
			إذا(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, رسالة_EXPIRY_INFINITE, عدم);
				context->bridge->initial_notification_done = true;
			}
			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, عدم);
			إذا(rc != MOSQ_ERR_SUCCESS){
				إرجاع rc;
			}
		}وإلا{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(حجم(حرف)*(notification_topic_len+1));
			إذا(!notification_topic){
				إرجاع MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			إذا(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, رسالة_EXPIRY_INFINITE, عدم);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, عدم);
			mosquitto_FREE(notification_topic);
			إذا(rc != MOSQ_ERR_SUCCESS){
				إرجاع rc;
			}
		}
	}

	log__printf(عدم, MOSQ_LOG_NOTICE, "Connecting bridge (step 1) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step1(context, context->bridge->addresses[context->bridge->cur_address].address);
	إذا(rc > 0){
		إذا(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			إرجاع rc; /* Error already printed */
		}وإلا إذا(rc == MOSQ_ERR_ERRNO){
			log__printf(عدم, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}وإلا إذا(rc == MOSQ_ERR_EAI){
			log__printf(عدم, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		إرجاع rc;
	}

	إرجاع MOSQ_ERR_SUCCESS;
}


ساكن صحيح bridge__connect_step2(بنية mosquitto *context)
{
	صحيح rc;

	إذا(!context || !context->bridge){
		إرجاع MOSQ_ERR_INVAL;
	}

	log__printf(عدم, MOSQ_LOG_NOTICE, "Connecting bridge (step 2) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step2(context, context->bridge->addresses[context->bridge->cur_address].port, &context->sock);
	إذا(rc > 0){
		إذا(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			إرجاع rc; /* Error already printed */
		}وإلا إذا(rc == MOSQ_ERR_ERRNO){
			log__printf(عدم, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}وإلا إذا(rc == MOSQ_ERR_EAI){
			log__printf(عدم, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		إرجاع rc;
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, حجم(context->sock), context);

	إذا(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}
	إرجاع rc;
}


صحيح bridge__connect_step3(بنية mosquitto *context)
{
	صحيح rc;
	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = عدم;

	rc = net__socket_connect_step3(context, context->bridge->addresses[context->bridge->cur_address].address);
	إذا(rc > 0){
		إذا(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			إرجاع rc; /* Error already printed */
		}وإلا إذا(rc == MOSQ_ERR_ERRNO){
			log__printf(عدم, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}وإلا إذا(rc == MOSQ_ERR_EAI){
			log__printf(عدم, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		إرجاع rc;
	}

	إذا(context->bridge->round_robin == false && context->bridge->cur_address != 0){
		context->bridge->primary_retry = db.now_s + 5;
		loop__update_next_event(5000);
	}

	إذا(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		إرجاع MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	إذا(bridge__set_tcp_user_timeout(context)){
		إرجاع MOSQ_ERR_UNKNOWN;
	}
#endif

	إذا(context->bridge->receive_maximum != 0){
		receive_maximum.قيمة.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	إذا(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.قيمة.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	إذا(context->bridge->max_topic_alias != 0){
		topic_alias_max.قيمة.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc = send__connect(context, context->keepalive, context->clean_start, properties);
	إذا(rc == MOSQ_ERR_SUCCESS){
		إرجاع MOSQ_ERR_SUCCESS;
	}وإلا إذا(rc == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		إرجاع MOSQ_ERR_SUCCESS;
	}وإلا{
		إذا(rc == MOSQ_ERR_TLS){
			إرجاع rc; /* Error already printed */
		}وإلا إذا(rc == MOSQ_ERR_ERRNO){
			log__printf(عدم, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}وإلا إذا(rc == MOSQ_ERR_EAI){
			log__printf(عدم, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		إرجاع rc;
	}
}
#وإلا


صحيح bridge__connect(بنية mosquitto *context)
{
	صحيح rc, rc2;
	حرف *notification_topic = عدم;
	size_t notification_topic_len;
	uint8_t notification_payload;
	بنية mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = عدم;

	إذا(!context || !context->bridge){
		إرجاع MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->last_رسالة_in = db.now_s;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->next_رسالة_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = عدم;
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
		إذا(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(عدم, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			إذا(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}وإلا{
				qos = cur_topic->qos;
			}
			بنية mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			إذا(sub__add(context, &sub) > 0){
				إرجاع 1;
			}
		}
	}

	إذا(context->bridge->notifications){
		إذا(context->max_qos == 0){
			qos = 0;
		}وإلا{
			qos = 1;
		}
		إذا(context->bridge->notification_topic){
			إذا(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, رسالة_EXPIRY_INFINITE, عدم);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, عدم);
			إذا(rc != MOSQ_ERR_SUCCESS){
				إرجاع rc;
			}
		}وإلا{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(حجم(حرف)*(notification_topic_len+1));
			إذا(!notification_topic){
				إرجاع MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			إذا(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, رسالة_EXPIRY_INFINITE, عدم);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, عدم);
			إذا(rc != MOSQ_ERR_SUCCESS){
				mosquitto_FREE(notification_topic);
				إرجاع rc;
			}
			mosquitto_FREE(notification_topic);
		}
	}

	log__printf(عدم, MOSQ_LOG_NOTICE, "Connecting bridge %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__socket_connect(context,
			context->bridge->addresses[context->bridge->cur_address].address,
			context->bridge->addresses[context->bridge->cur_address].port,
			context->bridge->bind_address,
			false);

	إذا(rc > 0){
		إذا(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			إرجاع rc; /* Error already printed */
		}وإلا إذا(rc == MOSQ_ERR_ERRNO){
			log__printf(عدم, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}وإلا إذا(rc == MOSQ_ERR_EAI){
			log__printf(عدم, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		إرجاع rc;
	}وإلا إذا(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, حجم(context->sock), context);

	إذا(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		إرجاع MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	إذا(bridge__set_tcp_user_timeout(context)){
		إرجاع MOSQ_ERR_UNKNOWN;
	}
#endif

	إذا(context->bridge->receive_maximum != 0){
		receive_maximum.قيمة.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	إذا(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.قيمة.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	إذا(context->bridge->max_topic_alias != 0){
		topic_alias_max.قيمة.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc2 = send__connect(context, context->keepalive, context->clean_start, properties);
	إذا(rc2 == MOSQ_ERR_SUCCESS){
		إرجاع rc;
	}وإلا إذا(rc2 == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		إرجاع MOSQ_ERR_SUCCESS;
	}وإلا{
		إذا(rc2 == MOSQ_ERR_TLS){
			إرجاع rc2; /* Error already printed */
		}وإلا إذا(rc2 == MOSQ_ERR_ERRNO){
			log__printf(عدم, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}وإلا إذا(rc2 == MOSQ_ERR_EAI){
			log__printf(عدم, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		إرجاع rc2;
	}
}
#endif


صحيح bridge__on_connect(بنية mosquitto *context)
{
	حرف *notification_topic;
	size_t notification_topic_len;
	بنية mosquitto__bridge_topic *cur_topic;
	صحيح sub_opts;
	bool retain = true;
	uint8_t qos;

	إذا(context->bridge->notifications){
		إذا(context->max_qos == 0){
			qos = 0;
		}وإلا{
			qos = 1;
		}
		إذا(!context->retain_available){
			retain = false;
		}
		حرف notification_payload = '1';
		إذا(context->bridge->notification_topic){
			إذا(!context->bridge->notifications_local_only){
				إذا(send__real_publish(context, mosquitto__mid_generate(context),
						context->bridge->notification_topic, 1, &notification_payload, qos, retain, 0, 0, عدم, 0)){

					إرجاع 1;
				}
			}
			db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, رسالة_EXPIRY_INFINITE, عدم);
		}وإلا{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(حجم(حرف)*(notification_topic_len+1));
			إذا(!notification_topic){
				إرجاع MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);
			notification_payload = '1';
			إذا(!context->bridge->notifications_local_only){
				إذا(send__real_publish(context, mosquitto__mid_generate(context),
						notification_topic, 1, &notification_payload, qos, retain, 0, 0, عدم, 0)){

					mosquitto_FREE(notification_topic);
					إرجاع 1;
				}
			}
			db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, رسالة_EXPIRY_INFINITE, عدم);
			mosquitto_FREE(notification_topic);
		}
	}

	LL_FOREACH(context->bridge->topics, cur_topic){
		إذا(cur_topic->direction == bd_in || cur_topic->direction == bd_both){
			إذا(cur_topic->qos > context->max_qos){
				sub_opts = context->max_qos;
			}وإلا{
				sub_opts = cur_topic->qos;
			}
			إذا(context->bridge->protocol_version == mosq_p_mqtt5){
				sub_opts = sub_opts
						| MQTT_SUB_OPT_NO_LOCAL
						| MQTT_SUB_OPT_RETAIN_AS_PUBLISHED
						| MQTT_SUB_OPT_SEND_RETAIN_ALWAYS;
			}
			إذا(send__subscribe(context, عدم, 1, &cur_topic->remote_topic, sub_opts, عدم)){
				إرجاع 1;
			}
		}وإلا{
			إذا(context->bridge->attempt_unsubscribe){
				إذا(send__unsubscribe(context, عدم, 1, &cur_topic->remote_topic, عدم)){
					/* direction = inwards only. This means we should not be subscribed
					* to the topic. It is possible that we used to be subscribed to
					* this topic so unsubscribe. */
					إرجاع 1;
				}
			}
		}
	}
	بنية mosquitto_subscription sub;
	memset(&sub, 0, حجم(sub));
	LL_FOREACH(context->bridge->topics, cur_topic){
		sub.topic_filter = cur_topic->local_topic;
		إذا(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			إذا(cur_topic->qos > context->max_qos){
				sub.options = context->max_qos;
			}وإلا{
				sub.options = cur_topic->qos;
			}
			retain__queue(context, &sub);
		}
	}

	context->bridge->connected_at = db.now_s;

	إرجاع MOSQ_ERR_SUCCESS;
}


صحيح bridge__register_local_connections(فراغ)
{
	بنية mosquitto *context, *ctxt_tmp = عدم;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		إذا(context->bridge){
			إذا(mux__new(context)){
				log__printf(عدم, MOSQ_LOG_ERR, "Error in initial bridge registration: %s", strerror(errno));
				إرجاع MOSQ_ERR_UNKNOWN;
			}
			mux__add_out(context);
		}
	}
	إرجاع MOSQ_ERR_SUCCESS;
}


فراغ bridge__reload(فراغ)
{
	صحيح i;
	صحيح j;

	// destroy old bridges that dissappeared
	لكل(i=0; i<db.bridge_تعداد; i++){
		لكل(j=0; j<db.config->bridge_تعداد; j++){
			إذا(!strcmp(db.bridges[i]->bridge->name, db.config->bridges[j]->name)){
				اكسر;
			}
		}

		إذا(j==db.config->bridge_تعداد){
			bridge__destroy(db.bridges[i]);
		}
	}

	لكل(i=0; i<db.config->bridge_تعداد; i++){
		لكل(j=0; j<db.bridge_تعداد; j++){
			إذا(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
				اكسر;
			}
		}

		إذا(j==db.bridge_تعداد){
			// a new bridge was found, create it
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = عدم;
			استمر;
		}

		إذا(db.config->bridges[i]->reload_type == brt_immediate){
			// in this case, an existing bridge should match
			لكل(j=0; j<db.bridge_تعداد; j++){
				إذا(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
					اكسر;
				}
			}

			أكد(j<db.bridge_تعداد);
			db.bridges[j]->will_delay_interval = 0;
			bridge__destroy(db.bridges[j]);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = عدم;
		}
	}
}


فراغ bridge__db_cleanup(فراغ)
{
	صحيح i;

	لكل(i=0; i<db.bridge_تعداد; i++){
		إذا(db.bridges[i]){
			context__cleanup(db.bridges[i], true);
		}
	}
	mosquitto_FREE(db.bridges);
}


فراغ bridge__cleanup(بنية mosquitto *context)
{
	صحيح i;

	أكد(db.bridge_تعداد > 0);

	لكل(i=0; i<db.bridge_تعداد; i++){
		إذا(db.bridges[i] == context){
			db.bridges[i] = db.bridges[db.bridge_تعداد-1];
			اكسر;
		}
	}

	db.bridge_تعداد--;
	إذا(db.bridge_تعداد == 0){
		mosquitto_FREE(db.bridges);
	}وإلا{
		db.bridges = mosquitto_realloc(db.bridges, (unsigned)db.bridge_تعداد * حجم(db.bridges[0]));
	}

	mosquitto_FREE(context->bridge->name);
	mosquitto_FREE(context->bridge->local_clientid);
	mosquitto_FREE(context->bridge->local_username);
	mosquitto_FREE(context->bridge->local_password);
#ifdef WITH_TLS
	mosquitto_FREE(context->bridge->tls_certfile);
	mosquitto_FREE(context->bridge->tls_keyfile);
#endif

	إذا(context->bridge->remote_clientid != context->id){
		mosquitto_FREE(context->bridge->remote_clientid);
	}
	context->bridge->remote_clientid = عدم;

	إذا(context->bridge->remote_username != context->username){
		mosquitto_FREE(context->bridge->remote_username);
	}
	context->bridge->remote_username = عدم;

	إذا(context->bridge->remote_password != context->password){
		mosquitto_FREE(context->bridge->remote_password);
	}
	context->bridge->remote_password = عدم;
#ifdef WITH_TLS
	إذا(context->ssl_ctx){
		SSL_CTX_free(context->ssl_ctx);
		context->ssl_ctx = عدم;
	}
#endif

	لكل(i=0; i<context->bridge->address_تعداد; i++){
		mosquitto_FREE(context->bridge->addresses[i].address);
	}

	mosquitto_FREE(context->bridge->addresses);

	config__bridge_cleanup(context->bridge);
	context->bridge = عدم;
}


ساكن فراغ bridge__packet_cleanup(بنية mosquitto *context)
{
	بنية mosquitto__packet *packet;
	إذا(!context){
		إرجاع;
	}

	بينما(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_FREE(packet);
	}
	context->out_packet = عدم;
	context->out_packet_last = عدم;
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_تعداد);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_تعداد = 0;
	context->out_packet_bytes = 0;

	packet__cleanup(&(context->in_packet));
}


ساكن صحيح rand_between(صحيح أدنى, صحيح أعلى)
{
	صحيح r;
	mosquitto_getrandom(&r, حجم(صحيح));
	إرجاع (abs(r) % (أعلى - أدنى)) + أدنى;
}


ساكن فراغ bridge__backoff_step(بنية mosquitto__bridge *bridge)
{
	/*
		“Decorrelated Jitter” calculation, according to:
			https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
	*/

	bridge->restart_timeout = rand_between(bridge->backoff_base, bridge->restart_timeout * 3);
	إذا(bridge->restart_timeout > bridge->backoff_cap){
		bridge->restart_timeout = bridge->backoff_cap;
	}
}


ساكن فراغ bridge__backoff_reset(بنية mosquitto__bridge *bridge)
{
	bridge->restart_timeout = bridge->backoff_base;
}


ساكن فراغ bridge__update_backoff(بنية mosquitto__bridge *bridge)
{
	إذا(!bridge){
		إرجاع;
	}
	إذا(!bridge->backoff_cap){
		/* skip if not using jitter */
		إرجاع;

	}
	إذا(bridge->connected_at && db.now_s - bridge->connected_at >= bridge->stable_connection_period){
		log__printf(عدم, MOSQ_LOG_INFO, "Bridge %s connection was stable enough, resetting backoff", bridge->name);
		bridge__backoff_reset(bridge);
	}وإلا{
		bridge__backoff_step(bridge);
	}

	bridge->connected_at = 0;

	log__printf(عدم, MOSQ_LOG_INFO, "Bridge %s next backoff will be %d ms", bridge->name, bridge->restart_timeout);
}


ساكن فراغ bridge_check_pending(بنية mosquitto *context)
{
	صحيح err;
	socklen_t len;

	إذا(context->state == mosq_cs_connect_pending){
		len = حجم(صحيح);
		إذا(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (حرف *)&err, &len)){
			إذا(err == 0){
				mosquitto__set_state(context, mosq_cs_new);
#إذا defined(WITH_ADNS) && defined(WITH_BRIDGE)
				إذا(context->bridge){
					bridge__connect_step3(context);
				}
#endif
			}وإلا إذا(err == ECONNREFUSED){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				إرجاع;
			}
		}وإلا{
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			إرجاع;
		}
	}
}


ساكن bool reload_if_needed(بنية mosquitto *context)
{
	صحيح i;

	لكل(i=0; i<db.config->bridge_تعداد; i++){
		إذا(db.config->bridges[i] && !strcmp(context->bridge->name, db.config->bridges[i]->name)){
			bridge__destroy(context);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = عدم;
			loop__update_next_event(100);
			إرجاع true;
		}
	}

	إرجاع false;
}


فراغ bridge_check(فراغ)
{
	ساكن time_t last_check = 0;
	بنية mosquitto *context = عدم;
	socklen_t len;
	صحيح i;
	صحيح rc;
	صحيح err;

	إذا(db.now_s <= last_check){
		إرجاع;
	}

	لكل(i=0; i<db.bridge_تعداد; i++){
		إذا(!db.bridges[i]){
			استمر;
		}

		context = db.bridges[i];

		إذا(net__is_connected(context)){
			mosquitto__check_keepalive(context);
			bridge_check_pending(context);

			/* Check for bridges that are not round robin and not currently
			 * connected to their primary broker. */
			إذا(context->bridge->round_robin == false
					&& context->bridge->cur_address != 0
					&& context->bridge->primary_retry
					&& db.now_s >= context->bridge->primary_retry){

				إذا(context->bridge->primary_retry_sock == INVALID_SOCKET){
					rc = net__try_connect(context->bridge->addresses[0].address,
							context->bridge->addresses[0].port,
							&context->bridge->primary_retry_sock,
							context->bridge->bind_address, false);

					إذا(rc == 0){
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = 0;
						mux__delete(context);
						net__socket_close(context);
						context->bridge->cur_address = 0;
					}
				}وإلا{
					len = حجم(صحيح);
					إذا(!getsockopt(context->bridge->primary_retry_sock, SOL_SOCKET, SO_ERROR, (حرف *)&err, &len)){
						إذا(err == 0){
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = 0;
							mux__delete(context);
							net__socket_close(context);
							context->bridge->cur_address = context->bridge->address_تعداد-1;
						}وإلا{
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = db.now_s+5;
							loop__update_next_event(5000);
						}
					}وإلا{
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = db.now_s+5;
						loop__update_next_event(5000);
					}
				}
			}
		}وإلا{
			إذا(reload_if_needed(context)){
				استمر;
			}

			/* Want to try to restart the bridge connection */
			إذا(!context->bridge->restart_t){
				bridge__update_backoff(context->bridge);
				context->bridge->restart_t = 1000*db.now_s+context->bridge->restart_timeout;
				context->bridge->cur_address++;
				إذا(context->bridge->cur_address == context->bridge->address_تعداد){
					context->bridge->cur_address = 0;
				}
				loop__update_next_event(context->bridge->restart_timeout);
			}وإلا{
				إذا((context->bridge->start_type == bst_lazy && context->bridge->lazy_reconnect)
						|| (context->bridge->start_type == bst_automatic && 1000*db.now_s >= context->bridge->restart_t)){

#إذا defined(__GLIBC__) && defined(WITH_ADNS)
					إذا(context->adns){
						/* Connection attempted, waiting on DNS lookup */
						rc = gai_error(context->adns);
						إذا(rc == EAI_INPROGRESS){
							/* Just keep on waiting */
						}وإلا إذا(rc == 0){
							rc = bridge__connect_step2(context);
							إذا(rc == MOSQ_ERR_SUCCESS){
								mux__new(context);
								إذا(context->out_packet){
									mux__add_out(context);
								}
							}وإلا إذا(rc == MOSQ_ERR_CONN_PENDING){
								mux__new(context);
								mux__add_out(context);
								context->bridge->restart_t = 0;
							}وإلا{
								context->bridge->cur_address++;
								إذا(context->bridge->cur_address == context->bridge->address_تعداد){
									context->bridge->cur_address = 0;
								}
								context->bridge->restart_t = 0;
							}
						}وإلا{
							/* Need to retry */
							إذا(context->adns->ar_result){
								freeaddrinfo(context->adns->ar_result);
							}
							mosquitto_FREE(context->adns);
							context->bridge->restart_t = 0;
						}
					}وإلا{
						rc = bridge__connect_step1(context);
						إذا(rc){
							context->bridge->cur_address++;
							إذا(context->bridge->cur_address == context->bridge->address_تعداد){
								context->bridge->cur_address = 0;
							}
						}وإلا{
							/* Short wait for ADNS lookup */
							context->bridge->restart_t = 1;
							loop__update_next_event(1000);
						}
					}
#وإلا
					{
						rc = bridge__connect(context);
						context->bridge->restart_t = 0;
						إذا(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING){
							إذا(context->bridge->round_robin == false && context->bridge->cur_address != 0){
								context->bridge->primary_retry = db.now_s + 5;
								loop__update_next_event(5000);
							}
							mux__new(context);
							إذا(context->out_packet){
								mux__add_out(context);
							}
						}وإلا{
							context->bridge->cur_address++;
							إذا(context->bridge->cur_address == context->bridge->address_تعداد){
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
