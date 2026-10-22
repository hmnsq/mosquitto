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

#ifndef WIN32
#شامل <netdb.h>
#شامل <sys/socket.h>
#شامل <netinet/in.h>
#شامل <netinet/tcp.h>
#وگرنه
#شامل <winsock2.h>
#شامل <ws2tcpip.h>
#endif

#ifndef WIN32
#شامل <unistd.h>
#وگرنه
#شامل <process.h>
#شامل <winsock2.h>
#شامل <ws2tcpip.h>
#endif

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

#ifdef WITH_BRIDGE

ایستا تهی bridge__update_backoff(ساختار mosquitto__bridge *bridge);
#اگر defined(__GLIBC__) && defined(WITH_ADNS)
ایستا صحیح bridge__connect_step1(ساختار mosquitto *context);
ایستا صحیح bridge__connect_step2(ساختار mosquitto *context);
#endif
ایستا تهی bridge__packet_cleanup(ساختار mosquitto *context);

ایستا ساختار mosquitto *bridge__new(ساختار mosquitto__bridge *bridge)
{
	ساختار mosquitto *new_context = پوچ;
	ساختار mosquitto **bridges;
	نویسه *local_id;

	أكد(bridge);

	local_id = mosquitto_strdup(bridge->local_clientid);
	اگر(!local_id){
		بازگشت پوچ;
	}

	HASH_FIND(hh_id, db.contexts_by_id, local_id, strlen(local_id), new_context);
	اگر(new_context){
		/* (possible from persistent db) */
		mosquitto_FREE(local_id);
	}وگرنه{
		/* id wasn't found, so generate a new context */
		new_context = context__init();
		اگر(!new_context){
			mosquitto_FREE(local_id);
			بازگشت پوچ;
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
	new_context->tls_engine = پوچ;
	new_context->tls_keyform = mosq_k_pem;
	new_context->tls_use_os_certs = bridge->tls_use_os_certs;
	new_context->ssl_ctx_defaults = true;
#ifdef FINAL_WITH_TLS_PSK
	new_context->tls_psk_identity = bridge->tls_psk_identity;
	new_context->tls_psk = bridge->tls_psk;
#endif
#endif

	bridge->try_private_accepted = true;
	اگر(bridge->clean_start_local == -1){
		/* default to "regular" clean start setting */
		bridge->clean_start_local = bridge->clean_start;
	}
	new_context->retain_available = bridge->outgoing_retain;
	new_context->protocol = bridge->protocol_version;
	اگر(!bridge->clean_start_local){
		new_context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
		plugin_persist__handle_client_add(new_context);
		اگر(new_context->expiry_list_item){
			/* We've restored from persistence and been added to the session
			 * expiry list, even though we should never be expired */
			session_expiry__remove(new_context);
		}
	}

	bridges = mosquitto_realloc(db.bridges, (size_t)(db.bridge_شمار+1)*اندازه(ساختار mosquitto *));
	اگر(bridges){
		db.bridges = bridges;
		db.bridge_شمار++;
		db.bridges[db.bridge_شمار-1] = new_context;
	}وگرنه{
		بازگشت پوچ;
	}

	بازگشت new_context;
}


ایستا تهی bridge__destroy(ساختار mosquitto *context)
{
	send__disconnect(context, MQTT_RC_SUCCESS, پوچ);
	context__cleanup(context, true);
}


تهی bridge__start_all(تهی)
{
	برای(صحیح i=0; i<db.config->bridge_شمار; i++){
		ساختار mosquitto *context;
		صحیح ret;

		context = bridge__new(db.config->bridges[i]);
		اگر(!context){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
			بازگشت;
		}
		أكد(context);

#اگر defined(__GLIBC__) && defined(WITH_ADNS)
		context->bridge->restart_t = 1; /* force quick restart of bridge */
		loop__update_next_event(1000);
		ret = bridge__connect_step1(context);
#وگرنه
		ret = bridge__connect(context);
#endif

		اگر(ret && ret != MOSQ_ERR_CONN_PENDING){
			log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Unable to connect bridge %s.",
					context->bridge->name);
		}

		db.config->bridges[i] = پوچ;
	}
}


ایستا صحیح bridge__set_tcp_keepalive(ساختار mosquitto *context)
{
	unsigned صحیح enabled = 1;
	bool ret;

	اگر(context->bridge->tcp_keepalive_idle == 0
			|| context->bridge->tcp_keepalive_interval == 0
			|| context->bridge->tcp_keepalive_counter == 0){

		بازگشت MOSQ_ERR_SUCCESS;
	}

#ifdef WIN32
#  define SETSOCKOPT_TYPE نویسه *
#وگرنه
#  define SETSOCKOPT_TYPE ثابت تهی *
#endif

	ret = setsockopt(context->sock, SOL_SOCKET, SO_KEEPALIVE, (SETSOCKOPT_TYPE)&enabled, اندازه(enabled));

#ifdef TCP_KEEPIDLE
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPIDLE,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_idle, اندازه(context->bridge->tcp_keepalive_idle));
#endif
#ifdef TCP_KEEPINTVL
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPINTVL,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_interval, اندازه(context->bridge->tcp_keepalive_interval));
#endif
#ifdef TCP_KEEPCNT
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPCNT,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_counter, اندازه(context->bridge->tcp_keepalive_counter));
#endif

#undef SETSOCKOPT_TYPE

	اگر(ret){
		بازگشت MOSQ_ERR_UNKNOWN;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TCP_USER_TIMEOUT


ایستا صحیح bridge__set_tcp_user_timeout(ساختار mosquitto *context)
{
	صحیح timeout = context->bridge->tcp_user_timeout;
	اگر(timeout >= 0){
		اگر(setsockopt(context->sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (نویسه *)&timeout, اندازه(timeout))){
			بازگشت MOSQ_ERR_UNKNOWN;
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}
#endif

#اگر defined(__GLIBC__) && defined(WITH_ADNS)


ایستا صحیح bridge__connect_step1(ساختار mosquitto *context)
{
	صحیح rc;
	نویسه *notification_topic;
	size_t notification_topic_len;
	uint8_t notification_payload;
	ساختار mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	اگر(!context || !context->bridge){
		بازگشت MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_پیام_in = db.now_s;
	context->next_پیام_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = پوچ;
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
		اگر(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(پوچ, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			اگر(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}وگرنه{
				qos = cur_topic->qos;
			}
			ساختار mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			اگر(sub__add(context, &sub) > 0){
				بازگشت 1;
			}
			retain__queue(context, &sub);
		}
	}

	اگر(context->bridge->notifications){
		اگر(context->max_qos == 0){
			qos = 0;
		}وگرنه{
			qos = 1;
		}
		اگر(context->bridge->notification_topic){
			اگر(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, پیام_EXPIRY_INFINITE, پوچ);
				context->bridge->initial_notification_done = true;
			}
			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, پوچ);
			اگر(rc != MOSQ_ERR_SUCCESS){
				بازگشت rc;
			}
		}وگرنه{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(اندازه(نویسه)*(notification_topic_len+1));
			اگر(!notification_topic){
				بازگشت MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			اگر(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, پیام_EXPIRY_INFINITE, پوچ);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, پوچ);
			mosquitto_FREE(notification_topic);
			اگر(rc != MOSQ_ERR_SUCCESS){
				بازگشت rc;
			}
		}
	}

	log__printf(پوچ, MOSQ_LOG_NOTICE, "Connecting bridge (step 1) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step1(context, context->bridge->addresses[context->bridge->cur_address].address);
	اگر(rc > 0){
		اگر(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			بازگشت rc; /* Error already printed */
		}وگرنه اگر(rc == MOSQ_ERR_ERRNO){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}وگرنه اگر(rc == MOSQ_ERR_EAI){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		بازگشت rc;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح bridge__connect_step2(ساختار mosquitto *context)
{
	صحیح rc;

	اگر(!context || !context->bridge){
		بازگشت MOSQ_ERR_INVAL;
	}

	log__printf(پوچ, MOSQ_LOG_NOTICE, "Connecting bridge (step 2) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step2(context, context->bridge->addresses[context->bridge->cur_address].port, &context->sock);
	اگر(rc > 0){
		اگر(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			بازگشت rc; /* Error already printed */
		}وگرنه اگر(rc == MOSQ_ERR_ERRNO){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}وگرنه اگر(rc == MOSQ_ERR_EAI){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		بازگشت rc;
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, اندازه(context->sock), context);

	اگر(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}
	بازگشت rc;
}


صحیح bridge__connect_step3(ساختار mosquitto *context)
{
	صحیح rc;
	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = پوچ;

	rc = net__socket_connect_step3(context, context->bridge->addresses[context->bridge->cur_address].address);
	اگر(rc > 0){
		اگر(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			بازگشت rc; /* Error already printed */
		}وگرنه اگر(rc == MOSQ_ERR_ERRNO){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}وگرنه اگر(rc == MOSQ_ERR_EAI){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		بازگشت rc;
	}

	اگر(context->bridge->round_robin == false && context->bridge->cur_address != 0){
		context->bridge->primary_retry = db.now_s + 5;
		loop__update_next_event(5000);
	}

	اگر(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		بازگشت MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	اگر(bridge__set_tcp_user_timeout(context)){
		بازگشت MOSQ_ERR_UNKNOWN;
	}
#endif

	اگر(context->bridge->receive_maximum != 0){
		receive_maximum.مقدار.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	اگر(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.مقدار.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	اگر(context->bridge->max_topic_alias != 0){
		topic_alias_max.مقدار.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc = send__connect(context, context->keepalive, context->clean_start, properties);
	اگر(rc == MOSQ_ERR_SUCCESS){
		بازگشت MOSQ_ERR_SUCCESS;
	}وگرنه اگر(rc == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		بازگشت MOSQ_ERR_SUCCESS;
	}وگرنه{
		اگر(rc == MOSQ_ERR_TLS){
			بازگشت rc; /* Error already printed */
		}وگرنه اگر(rc == MOSQ_ERR_ERRNO){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}وگرنه اگر(rc == MOSQ_ERR_EAI){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		بازگشت rc;
	}
}
#وگرنه


صحیح bridge__connect(ساختار mosquitto *context)
{
	صحیح rc, rc2;
	نویسه *notification_topic = پوچ;
	size_t notification_topic_len;
	uint8_t notification_payload;
	ساختار mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = پوچ;

	اگر(!context || !context->bridge){
		بازگشت MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->last_پیام_in = db.now_s;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->next_پیام_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = پوچ;
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
		اگر(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(پوچ, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			اگر(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}وگرنه{
				qos = cur_topic->qos;
			}
			ساختار mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			اگر(sub__add(context, &sub) > 0){
				بازگشت 1;
			}
		}
	}

	اگر(context->bridge->notifications){
		اگر(context->max_qos == 0){
			qos = 0;
		}وگرنه{
			qos = 1;
		}
		اگر(context->bridge->notification_topic){
			اگر(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, پیام_EXPIRY_INFINITE, پوچ);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, پوچ);
			اگر(rc != MOSQ_ERR_SUCCESS){
				بازگشت rc;
			}
		}وگرنه{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(اندازه(نویسه)*(notification_topic_len+1));
			اگر(!notification_topic){
				بازگشت MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			اگر(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, پیام_EXPIRY_INFINITE, پوچ);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, پوچ);
			اگر(rc != MOSQ_ERR_SUCCESS){
				mosquitto_FREE(notification_topic);
				بازگشت rc;
			}
			mosquitto_FREE(notification_topic);
		}
	}

	log__printf(پوچ, MOSQ_LOG_NOTICE, "Connecting bridge %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__socket_connect(context,
			context->bridge->addresses[context->bridge->cur_address].address,
			context->bridge->addresses[context->bridge->cur_address].port,
			context->bridge->bind_address,
			false);

	اگر(rc > 0){
		اگر(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			بازگشت rc; /* Error already printed */
		}وگرنه اگر(rc == MOSQ_ERR_ERRNO){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}وگرنه اگر(rc == MOSQ_ERR_EAI){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		بازگشت rc;
	}وگرنه اگر(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, اندازه(context->sock), context);

	اگر(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		بازگشت MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	اگر(bridge__set_tcp_user_timeout(context)){
		بازگشت MOSQ_ERR_UNKNOWN;
	}
#endif

	اگر(context->bridge->receive_maximum != 0){
		receive_maximum.مقدار.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	اگر(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.مقدار.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	اگر(context->bridge->max_topic_alias != 0){
		topic_alias_max.مقدار.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc2 = send__connect(context, context->keepalive, context->clean_start, properties);
	اگر(rc2 == MOSQ_ERR_SUCCESS){
		بازگشت rc;
	}وگرنه اگر(rc2 == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		بازگشت MOSQ_ERR_SUCCESS;
	}وگرنه{
		اگر(rc2 == MOSQ_ERR_TLS){
			بازگشت rc2; /* Error already printed */
		}وگرنه اگر(rc2 == MOSQ_ERR_ERRNO){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}وگرنه اگر(rc2 == MOSQ_ERR_EAI){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		بازگشت rc2;
	}
}
#endif


صحیح bridge__on_connect(ساختار mosquitto *context)
{
	نویسه *notification_topic;
	size_t notification_topic_len;
	ساختار mosquitto__bridge_topic *cur_topic;
	صحیح sub_opts;
	bool retain = true;
	uint8_t qos;

	اگر(context->bridge->notifications){
		اگر(context->max_qos == 0){
			qos = 0;
		}وگرنه{
			qos = 1;
		}
		اگر(!context->retain_available){
			retain = false;
		}
		نویسه notification_payload = '1';
		اگر(context->bridge->notification_topic){
			اگر(!context->bridge->notifications_local_only){
				اگر(send__real_publish(context, mosquitto__mid_generate(context),
						context->bridge->notification_topic, 1, &notification_payload, qos, retain, 0, 0, پوچ, 0)){

					بازگشت 1;
				}
			}
			db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, پیام_EXPIRY_INFINITE, پوچ);
		}وگرنه{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(اندازه(نویسه)*(notification_topic_len+1));
			اگر(!notification_topic){
				بازگشت MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);
			notification_payload = '1';
			اگر(!context->bridge->notifications_local_only){
				اگر(send__real_publish(context, mosquitto__mid_generate(context),
						notification_topic, 1, &notification_payload, qos, retain, 0, 0, پوچ, 0)){

					mosquitto_FREE(notification_topic);
					بازگشت 1;
				}
			}
			db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, پیام_EXPIRY_INFINITE, پوچ);
			mosquitto_FREE(notification_topic);
		}
	}

	LL_FOREACH(context->bridge->topics, cur_topic){
		اگر(cur_topic->direction == bd_in || cur_topic->direction == bd_both){
			اگر(cur_topic->qos > context->max_qos){
				sub_opts = context->max_qos;
			}وگرنه{
				sub_opts = cur_topic->qos;
			}
			اگر(context->bridge->protocol_version == mosq_p_mqtt5){
				sub_opts = sub_opts
						| MQTT_SUB_OPT_NO_LOCAL
						| MQTT_SUB_OPT_RETAIN_AS_PUBLISHED
						| MQTT_SUB_OPT_SEND_RETAIN_ALWAYS;
			}
			اگر(send__subscribe(context, پوچ, 1, &cur_topic->remote_topic, sub_opts, پوچ)){
				بازگشت 1;
			}
		}وگرنه{
			اگر(context->bridge->attempt_unsubscribe){
				اگر(send__unsubscribe(context, پوچ, 1, &cur_topic->remote_topic, پوچ)){
					/* direction = inwards only. This means we should not be subscribed
					* to the topic. It is possible that we used to be subscribed to
					* this topic so unsubscribe. */
					بازگشت 1;
				}
			}
		}
	}
	ساختار mosquitto_subscription sub;
	memset(&sub, 0, اندازه(sub));
	LL_FOREACH(context->bridge->topics, cur_topic){
		sub.topic_filter = cur_topic->local_topic;
		اگر(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			اگر(cur_topic->qos > context->max_qos){
				sub.options = context->max_qos;
			}وگرنه{
				sub.options = cur_topic->qos;
			}
			retain__queue(context, &sub);
		}
	}

	context->bridge->connected_at = db.now_s;

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح bridge__register_local_connections(تهی)
{
	ساختار mosquitto *context, *ctxt_tmp = پوچ;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		اگر(context->bridge){
			اگر(mux__new(context)){
				log__printf(پوچ, MOSQ_LOG_ERR, "Error in initial bridge registration: %s", strerror(errno));
				بازگشت MOSQ_ERR_UNKNOWN;
			}
			mux__add_out(context);
		}
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


تهی bridge__reload(تهی)
{
	صحیح i;
	صحیح j;

	// destroy old bridges that dissappeared
	برای(i=0; i<db.bridge_شمار; i++){
		برای(j=0; j<db.config->bridge_شمار; j++){
			اگر(!strcmp(db.bridges[i]->bridge->name, db.config->bridges[j]->name)){
				بشکن;
			}
		}

		اگر(j==db.config->bridge_شمار){
			bridge__destroy(db.bridges[i]);
		}
	}

	برای(i=0; i<db.config->bridge_شمار; i++){
		برای(j=0; j<db.bridge_شمار; j++){
			اگر(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
				بشکن;
			}
		}

		اگر(j==db.bridge_شمار){
			// a new bridge was found, create it
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = پوچ;
			ادامه;
		}

		اگر(db.config->bridges[i]->reload_type == brt_immediate){
			// in this case, an existing bridge should match
			برای(j=0; j<db.bridge_شمار; j++){
				اگر(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
					بشکن;
				}
			}

			أكد(j<db.bridge_شمار);
			db.bridges[j]->will_delay_interval = 0;
			bridge__destroy(db.bridges[j]);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = پوچ;
		}
	}
}


تهی bridge__db_cleanup(تهی)
{
	صحیح i;

	برای(i=0; i<db.bridge_شمار; i++){
		اگر(db.bridges[i]){
			context__cleanup(db.bridges[i], true);
		}
	}
	mosquitto_FREE(db.bridges);
}


تهی bridge__cleanup(ساختار mosquitto *context)
{
	صحیح i;

	أكد(db.bridge_شمار > 0);

	برای(i=0; i<db.bridge_شمار; i++){
		اگر(db.bridges[i] == context){
			db.bridges[i] = db.bridges[db.bridge_شمار-1];
			بشکن;
		}
	}

	db.bridge_شمار--;
	اگر(db.bridge_شمار == 0){
		mosquitto_FREE(db.bridges);
	}وگرنه{
		db.bridges = mosquitto_realloc(db.bridges, (unsigned)db.bridge_شمار * اندازه(db.bridges[0]));
	}

	mosquitto_FREE(context->bridge->name);
	mosquitto_FREE(context->bridge->local_clientid);
	mosquitto_FREE(context->bridge->local_username);
	mosquitto_FREE(context->bridge->local_password);
#ifdef WITH_TLS
	mosquitto_FREE(context->bridge->tls_certfile);
	mosquitto_FREE(context->bridge->tls_keyfile);
#endif

	اگر(context->bridge->remote_clientid != context->id){
		mosquitto_FREE(context->bridge->remote_clientid);
	}
	context->bridge->remote_clientid = پوچ;

	اگر(context->bridge->remote_username != context->username){
		mosquitto_FREE(context->bridge->remote_username);
	}
	context->bridge->remote_username = پوچ;

	اگر(context->bridge->remote_password != context->password){
		mosquitto_FREE(context->bridge->remote_password);
	}
	context->bridge->remote_password = پوچ;
#ifdef WITH_TLS
	اگر(context->ssl_ctx){
		SSL_CTX_free(context->ssl_ctx);
		context->ssl_ctx = پوچ;
	}
#endif

	برای(i=0; i<context->bridge->address_شمار; i++){
		mosquitto_FREE(context->bridge->addresses[i].address);
	}

	mosquitto_FREE(context->bridge->addresses);

	config__bridge_cleanup(context->bridge);
	context->bridge = پوچ;
}


ایستا تهی bridge__packet_cleanup(ساختار mosquitto *context)
{
	ساختار mosquitto__packet *packet;
	اگر(!context){
		بازگشت;
	}

	وقتی(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_FREE(packet);
	}
	context->out_packet = پوچ;
	context->out_packet_last = پوچ;
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_شمار);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_شمار = 0;
	context->out_packet_bytes = 0;

	packet__cleanup(&(context->in_packet));
}


ایستا صحیح rand_between(صحیح کم, صحیح بالا)
{
	صحیح r;
	mosquitto_getrandom(&r, اندازه(صحیح));
	بازگشت (abs(r) % (بالا - کم)) + کم;
}


ایستا تهی bridge__backoff_step(ساختار mosquitto__bridge *bridge)
{
	/*
		“Decorrelated Jitter” calculation, according to:
			https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
	*/

	bridge->restart_timeout = rand_between(bridge->backoff_base, bridge->restart_timeout * 3);
	اگر(bridge->restart_timeout > bridge->backoff_cap){
		bridge->restart_timeout = bridge->backoff_cap;
	}
}


ایستا تهی bridge__backoff_reset(ساختار mosquitto__bridge *bridge)
{
	bridge->restart_timeout = bridge->backoff_base;
}


ایستا تهی bridge__update_backoff(ساختار mosquitto__bridge *bridge)
{
	اگر(!bridge){
		بازگشت;
	}
	اگر(!bridge->backoff_cap){
		/* skip if not using jitter */
		بازگشت;

	}
	اگر(bridge->connected_at && db.now_s - bridge->connected_at >= bridge->stable_connection_period){
		log__printf(پوچ, MOSQ_LOG_INFO, "Bridge %s connection was stable enough, resetting backoff", bridge->name);
		bridge__backoff_reset(bridge);
	}وگرنه{
		bridge__backoff_step(bridge);
	}

	bridge->connected_at = 0;

	log__printf(پوچ, MOSQ_LOG_INFO, "Bridge %s next backoff will be %d ms", bridge->name, bridge->restart_timeout);
}


ایستا تهی bridge_check_pending(ساختار mosquitto *context)
{
	صحیح err;
	socklen_t len;

	اگر(context->state == mosq_cs_connect_pending){
		len = اندازه(صحیح);
		اگر(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (نویسه *)&err, &len)){
			اگر(err == 0){
				mosquitto__set_state(context, mosq_cs_new);
#اگر defined(WITH_ADNS) && defined(WITH_BRIDGE)
				اگر(context->bridge){
					bridge__connect_step3(context);
				}
#endif
			}وگرنه اگر(err == ECONNREFUSED){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				بازگشت;
			}
		}وگرنه{
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			بازگشت;
		}
	}
}


ایستا bool reload_if_needed(ساختار mosquitto *context)
{
	صحیح i;

	برای(i=0; i<db.config->bridge_شمار; i++){
		اگر(db.config->bridges[i] && !strcmp(context->bridge->name, db.config->bridges[i]->name)){
			bridge__destroy(context);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = پوچ;
			loop__update_next_event(100);
			بازگشت true;
		}
	}

	بازگشت false;
}


تهی bridge_check(تهی)
{
	ایستا time_t last_check = 0;
	ساختار mosquitto *context = پوچ;
	socklen_t len;
	صحیح i;
	صحیح rc;
	صحیح err;

	اگر(db.now_s <= last_check){
		بازگشت;
	}

	برای(i=0; i<db.bridge_شمار; i++){
		اگر(!db.bridges[i]){
			ادامه;
		}

		context = db.bridges[i];

		اگر(net__is_connected(context)){
			mosquitto__check_keepalive(context);
			bridge_check_pending(context);

			/* Check for bridges that are not round robin and not currently
			 * connected to their primary broker. */
			اگر(context->bridge->round_robin == false
					&& context->bridge->cur_address != 0
					&& context->bridge->primary_retry
					&& db.now_s >= context->bridge->primary_retry){

				اگر(context->bridge->primary_retry_sock == INVALID_SOCKET){
					rc = net__try_connect(context->bridge->addresses[0].address,
							context->bridge->addresses[0].port,
							&context->bridge->primary_retry_sock,
							context->bridge->bind_address, false);

					اگر(rc == 0){
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = 0;
						mux__delete(context);
						net__socket_close(context);
						context->bridge->cur_address = 0;
					}
				}وگرنه{
					len = اندازه(صحیح);
					اگر(!getsockopt(context->bridge->primary_retry_sock, SOL_SOCKET, SO_ERROR, (نویسه *)&err, &len)){
						اگر(err == 0){
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = 0;
							mux__delete(context);
							net__socket_close(context);
							context->bridge->cur_address = context->bridge->address_شمار-1;
						}وگرنه{
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = db.now_s+5;
							loop__update_next_event(5000);
						}
					}وگرنه{
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = db.now_s+5;
						loop__update_next_event(5000);
					}
				}
			}
		}وگرنه{
			اگر(reload_if_needed(context)){
				ادامه;
			}

			/* Want to try to restart the bridge connection */
			اگر(!context->bridge->restart_t){
				bridge__update_backoff(context->bridge);
				context->bridge->restart_t = 1000*db.now_s+context->bridge->restart_timeout;
				context->bridge->cur_address++;
				اگر(context->bridge->cur_address == context->bridge->address_شمار){
					context->bridge->cur_address = 0;
				}
				loop__update_next_event(context->bridge->restart_timeout);
			}وگرنه{
				اگر((context->bridge->start_type == bst_lazy && context->bridge->lazy_reconnect)
						|| (context->bridge->start_type == bst_automatic && 1000*db.now_s >= context->bridge->restart_t)){

#اگر defined(__GLIBC__) && defined(WITH_ADNS)
					اگر(context->adns){
						/* Connection attempted, waiting on DNS lookup */
						rc = gai_error(context->adns);
						اگر(rc == EAI_INPROGRESS){
							/* Just keep on waiting */
						}وگرنه اگر(rc == 0){
							rc = bridge__connect_step2(context);
							اگر(rc == MOSQ_ERR_SUCCESS){
								mux__new(context);
								اگر(context->out_packet){
									mux__add_out(context);
								}
							}وگرنه اگر(rc == MOSQ_ERR_CONN_PENDING){
								mux__new(context);
								mux__add_out(context);
								context->bridge->restart_t = 0;
							}وگرنه{
								context->bridge->cur_address++;
								اگر(context->bridge->cur_address == context->bridge->address_شمار){
									context->bridge->cur_address = 0;
								}
								context->bridge->restart_t = 0;
							}
						}وگرنه{
							/* Need to retry */
							اگر(context->adns->ar_result){
								freeaddrinfo(context->adns->ar_result);
							}
							mosquitto_FREE(context->adns);
							context->bridge->restart_t = 0;
						}
					}وگرنه{
						rc = bridge__connect_step1(context);
						اگر(rc){
							context->bridge->cur_address++;
							اگر(context->bridge->cur_address == context->bridge->address_شمار){
								context->bridge->cur_address = 0;
							}
						}وگرنه{
							/* Short wait for ADNS lookup */
							context->bridge->restart_t = 1;
							loop__update_next_event(1000);
						}
					}
#وگرنه
					{
						rc = bridge__connect(context);
						context->bridge->restart_t = 0;
						اگر(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING){
							اگر(context->bridge->round_robin == false && context->bridge->cur_address != 0){
								context->bridge->primary_retry = db.now_s + 5;
								loop__update_next_event(5000);
							}
							mux__new(context);
							اگر(context->out_packet){
								mux__add_out(context);
							}
						}وگرنه{
							context->bridge->cur_address++;
							اگر(context->bridge->cur_address == context->bridge->address_شمار){
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
