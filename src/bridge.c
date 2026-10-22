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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <errno.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

#ifndef WIN32
#அந்தர்பாவ <netdb.h>
#அந்தர்பாவ <sys/socket.h>
#அந்தர்பாவ <netinet/in.h>
#அந்தர்பாவ <netinet/tcp.h>
#இல்லையெனில்
#அந்தர்பாவ <winsock2.h>
#அந்தர்பாவ <ws2tcpip.h>
#endif

#ifndef WIN32
#அந்தர்பாவ <unistd.h>
#இல்லையெனில்
#அந்தர்பாவ <process.h>
#அந்தர்பாவ <winsock2.h>
#அந்தர்பாவ <ws2tcpip.h>
#endif

#அந்தர்பாவ "mosquitto.h"
#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto_internal.h"
#அந்தர்பாவ "net_mosq.h"
#அந்தர்பாவ "packet_mosq.h"
#அந்தர்பாவ "property_common.h"
#அந்தர்பாவ "send_mosq.h"
#அந்தர்பாவ "sys_tree.h"
#அந்தர்பாவ "tls_mosq.h"
#அந்தர்பாவ "util_mosq.h"
#அந்தர்பாவ "will_mosq.h"
#அந்தர்பாவ "utlist.h"

#ifdef WITH_BRIDGE

நிலையான வெறுமை bridge__update_backoff(கட்டமைப்பு mosquitto__bridge *bridge);
#எனில் defined(__GLIBC__) && defined(WITH_ADNS)
நிலையான முழு bridge__connect_step1(கட்டமைப்பு mosquitto *context);
நிலையான முழு bridge__connect_step2(கட்டமைப்பு mosquitto *context);
#endif
நிலையான வெறுமை bridge__packet_cleanup(கட்டமைப்பு mosquitto *context);

நிலையான கட்டமைப்பு mosquitto *bridge__new(கட்டமைப்பு mosquitto__bridge *bridge)
{
	கட்டமைப்பு mosquitto *new_context = வெற்று;
	கட்டமைப்பு mosquitto **bridges;
	எழுத்து *local_id;

	ப்ரதிஜ்ஞா(bridge);

	local_id = mosquitto_strdup(bridge->local_clientid);
	எனில்(!local_id){
		திரும்பு வெற்று;
	}

	HASH_FIND(hh_id, db.contexts_by_id, local_id, strlen(local_id), new_context);
	எனில்(new_context){
		/* (possible from persistent db) */
		mosquitto_FREE(local_id);
	}இல்லையெனில்{
		/* id wasn't found, so generate a new context */
		new_context = context__init();
		எனில்(!new_context){
			mosquitto_FREE(local_id);
			திரும்பு வெற்று;
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
	new_context->tls_engine = வெற்று;
	new_context->tls_keyform = mosq_k_pem;
	new_context->tls_use_os_certs = bridge->tls_use_os_certs;
	new_context->ssl_ctx_defaults = true;
#ifdef FINAL_WITH_TLS_PSK
	new_context->tls_psk_identity = bridge->tls_psk_identity;
	new_context->tls_psk = bridge->tls_psk;
#endif
#endif

	bridge->try_private_accepted = true;
	எனில்(bridge->clean_start_local == -1){
		/* default to "regular" clean start setting */
		bridge->clean_start_local = bridge->clean_start;
	}
	new_context->retain_available = bridge->outgoing_retain;
	new_context->protocol = bridge->protocol_version;
	எனில்(!bridge->clean_start_local){
		new_context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
		plugin_persist__handle_client_add(new_context);
		எனில்(new_context->expiry_list_item){
			/* We've restored from persistence and been added to the session
			 * expiry list, even though we should never be expired */
			session_expiry__remove(new_context);
		}
	}

	bridges = mosquitto_realloc(db.bridges, (size_t)(db.bridge_மொத்தம்+1)*அளவு(கட்டமைப்பு mosquitto *));
	எனில்(bridges){
		db.bridges = bridges;
		db.bridge_மொத்தம்++;
		db.bridges[db.bridge_மொத்தம்-1] = new_context;
	}இல்லையெனில்{
		திரும்பு வெற்று;
	}

	திரும்பு new_context;
}


நிலையான வெறுமை bridge__destroy(கட்டமைப்பு mosquitto *context)
{
	send__disconnect(context, MQTT_RC_SUCCESS, வெற்று);
	context__cleanup(context, true);
}


வெறுமை bridge__start_all(வெறுமை)
{
	க்கு(முழு i=0; i<db.config->bridge_மொத்தம்; i++){
		கட்டமைப்பு mosquitto *context;
		முழு ret;

		context = bridge__new(db.config->bridges[i]);
		எனில்(!context){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
			திரும்பு;
		}
		ப்ரதிஜ்ஞா(context);

#எனில் defined(__GLIBC__) && defined(WITH_ADNS)
		context->bridge->restart_t = 1; /* force quick restart of bridge */
		loop__update_next_event(1000);
		ret = bridge__connect_step1(context);
#இல்லையெனில்
		ret = bridge__connect(context);
#endif

		எனில்(ret && ret != MOSQ_ERR_CONN_PENDING){
			log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Unable to connect bridge %s.",
					context->bridge->name);
		}

		db.config->bridges[i] = வெற்று;
	}
}


நிலையான முழு bridge__set_tcp_keepalive(கட்டமைப்பு mosquitto *context)
{
	unsigned முழு enabled = 1;
	bool ret;

	எனில்(context->bridge->tcp_keepalive_idle == 0
			|| context->bridge->tcp_keepalive_interval == 0
			|| context->bridge->tcp_keepalive_counter == 0){

		திரும்பு MOSQ_ERR_SUCCESS;
	}

#ifdef WIN32
#  define SETSOCKOPT_TYPE எழுத்து *
#இல்லையெனில்
#  define SETSOCKOPT_TYPE மாறிலி வெறுமை *
#endif

	ret = setsockopt(context->sock, SOL_SOCKET, SO_KEEPALIVE, (SETSOCKOPT_TYPE)&enabled, அளவு(enabled));

#ifdef TCP_KEEPIDLE
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPIDLE,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_idle, அளவு(context->bridge->tcp_keepalive_idle));
#endif
#ifdef TCP_KEEPINTVL
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPINTVL,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_interval, அளவு(context->bridge->tcp_keepalive_interval));
#endif
#ifdef TCP_KEEPCNT
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPCNT,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_counter, அளவு(context->bridge->tcp_keepalive_counter));
#endif

#undef SETSOCKOPT_TYPE

	எனில்(ret){
		திரும்பு MOSQ_ERR_UNKNOWN;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TCP_USER_TIMEOUT


நிலையான முழு bridge__set_tcp_user_timeout(கட்டமைப்பு mosquitto *context)
{
	முழு timeout = context->bridge->tcp_user_timeout;
	எனில்(timeout >= 0){
		எனில்(setsockopt(context->sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (எழுத்து *)&timeout, அளவு(timeout))){
			திரும்பு MOSQ_ERR_UNKNOWN;
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}
#endif

#எனில் defined(__GLIBC__) && defined(WITH_ADNS)


நிலையான முழு bridge__connect_step1(கட்டமைப்பு mosquitto *context)
{
	முழு rc;
	எழுத்து *notification_topic;
	size_t notification_topic_len;
	uint8_t notification_payload;
	கட்டமைப்பு mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	எனில்(!context || !context->bridge){
		திரும்பு MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_செய்தி_in = db.now_s;
	context->next_செய்தி_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = வெற்று;
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
		எனில்(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(வெற்று, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			எனில்(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}இல்லையெனில்{
				qos = cur_topic->qos;
			}
			கட்டமைப்பு mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			எனில்(sub__add(context, &sub) > 0){
				திரும்பு 1;
			}
			retain__queue(context, &sub);
		}
	}

	எனில்(context->bridge->notifications){
		எனில்(context->max_qos == 0){
			qos = 0;
		}இல்லையெனில்{
			qos = 1;
		}
		எனில்(context->bridge->notification_topic){
			எனில்(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, செய்தி_EXPIRY_INFINITE, வெற்று);
				context->bridge->initial_notification_done = true;
			}
			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, வெற்று);
			எனில்(rc != MOSQ_ERR_SUCCESS){
				திரும்பு rc;
			}
		}இல்லையெனில்{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(அளவு(எழுத்து)*(notification_topic_len+1));
			எனில்(!notification_topic){
				திரும்பு MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			எனில்(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, செய்தி_EXPIRY_INFINITE, வெற்று);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, வெற்று);
			mosquitto_FREE(notification_topic);
			எனில்(rc != MOSQ_ERR_SUCCESS){
				திரும்பு rc;
			}
		}
	}

	log__printf(வெற்று, MOSQ_LOG_NOTICE, "Connecting bridge (step 1) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step1(context, context->bridge->addresses[context->bridge->cur_address].address);
	எனில்(rc > 0){
		எனில்(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			திரும்பு rc; /* Error already printed */
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_ERRNO){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_EAI){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		திரும்பு rc;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு bridge__connect_step2(கட்டமைப்பு mosquitto *context)
{
	முழு rc;

	எனில்(!context || !context->bridge){
		திரும்பு MOSQ_ERR_INVAL;
	}

	log__printf(வெற்று, MOSQ_LOG_NOTICE, "Connecting bridge (step 2) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step2(context, context->bridge->addresses[context->bridge->cur_address].port, &context->sock);
	எனில்(rc > 0){
		எனில்(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			திரும்பு rc; /* Error already printed */
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_ERRNO){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_EAI){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		திரும்பு rc;
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, அளவு(context->sock), context);

	எனில்(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}
	திரும்பு rc;
}


முழு bridge__connect_step3(கட்டமைப்பு mosquitto *context)
{
	முழு rc;
	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = வெற்று;

	rc = net__socket_connect_step3(context, context->bridge->addresses[context->bridge->cur_address].address);
	எனில்(rc > 0){
		எனில்(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			திரும்பு rc; /* Error already printed */
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_ERRNO){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_EAI){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		திரும்பு rc;
	}

	எனில்(context->bridge->round_robin == false && context->bridge->cur_address != 0){
		context->bridge->primary_retry = db.now_s + 5;
		loop__update_next_event(5000);
	}

	எனில்(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		திரும்பு MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	எனில்(bridge__set_tcp_user_timeout(context)){
		திரும்பு MOSQ_ERR_UNKNOWN;
	}
#endif

	எனில்(context->bridge->receive_maximum != 0){
		receive_maximum.மதிப்பு.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	எனில்(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.மதிப்பு.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	எனில்(context->bridge->max_topic_alias != 0){
		topic_alias_max.மதிப்பு.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc = send__connect(context, context->keepalive, context->clean_start, properties);
	எனில்(rc == MOSQ_ERR_SUCCESS){
		திரும்பு MOSQ_ERR_SUCCESS;
	}இல்லையெனில் எனில்(rc == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		திரும்பு MOSQ_ERR_SUCCESS;
	}இல்லையெனில்{
		எனில்(rc == MOSQ_ERR_TLS){
			திரும்பு rc; /* Error already printed */
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_ERRNO){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_EAI){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		திரும்பு rc;
	}
}
#இல்லையெனில்


முழு bridge__connect(கட்டமைப்பு mosquitto *context)
{
	முழு rc, rc2;
	எழுத்து *notification_topic = வெற்று;
	size_t notification_topic_len;
	uint8_t notification_payload;
	கட்டமைப்பு mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = வெற்று;

	எனில்(!context || !context->bridge){
		திரும்பு MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->last_செய்தி_in = db.now_s;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->next_செய்தி_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = வெற்று;
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
		எனில்(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(வெற்று, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			எனில்(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}இல்லையெனில்{
				qos = cur_topic->qos;
			}
			கட்டமைப்பு mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			எனில்(sub__add(context, &sub) > 0){
				திரும்பு 1;
			}
		}
	}

	எனில்(context->bridge->notifications){
		எனில்(context->max_qos == 0){
			qos = 0;
		}இல்லையெனில்{
			qos = 1;
		}
		எனில்(context->bridge->notification_topic){
			எனில்(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, செய்தி_EXPIRY_INFINITE, வெற்று);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, வெற்று);
			எனில்(rc != MOSQ_ERR_SUCCESS){
				திரும்பு rc;
			}
		}இல்லையெனில்{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(அளவு(எழுத்து)*(notification_topic_len+1));
			எனில்(!notification_topic){
				திரும்பு MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			எனில்(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, செய்தி_EXPIRY_INFINITE, வெற்று);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, வெற்று);
			எனில்(rc != MOSQ_ERR_SUCCESS){
				mosquitto_FREE(notification_topic);
				திரும்பு rc;
			}
			mosquitto_FREE(notification_topic);
		}
	}

	log__printf(வெற்று, MOSQ_LOG_NOTICE, "Connecting bridge %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__socket_connect(context,
			context->bridge->addresses[context->bridge->cur_address].address,
			context->bridge->addresses[context->bridge->cur_address].port,
			context->bridge->bind_address,
			false);

	எனில்(rc > 0){
		எனில்(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			திரும்பு rc; /* Error already printed */
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_ERRNO){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_EAI){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		திரும்பு rc;
	}இல்லையெனில் எனில்(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, அளவு(context->sock), context);

	எனில்(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		திரும்பு MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	எனில்(bridge__set_tcp_user_timeout(context)){
		திரும்பு MOSQ_ERR_UNKNOWN;
	}
#endif

	எனில்(context->bridge->receive_maximum != 0){
		receive_maximum.மதிப்பு.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	எனில்(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.மதிப்பு.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	எனில்(context->bridge->max_topic_alias != 0){
		topic_alias_max.மதிப்பு.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc2 = send__connect(context, context->keepalive, context->clean_start, properties);
	எனில்(rc2 == MOSQ_ERR_SUCCESS){
		திரும்பு rc;
	}இல்லையெனில் எனில்(rc2 == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		திரும்பு MOSQ_ERR_SUCCESS;
	}இல்லையெனில்{
		எனில்(rc2 == MOSQ_ERR_TLS){
			திரும்பு rc2; /* Error already printed */
		}இல்லையெனில் எனில்(rc2 == MOSQ_ERR_ERRNO){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}இல்லையெனில் எனில்(rc2 == MOSQ_ERR_EAI){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		திரும்பு rc2;
	}
}
#endif


முழு bridge__on_connect(கட்டமைப்பு mosquitto *context)
{
	எழுத்து *notification_topic;
	size_t notification_topic_len;
	கட்டமைப்பு mosquitto__bridge_topic *cur_topic;
	முழு sub_opts;
	bool retain = true;
	uint8_t qos;

	எனில்(context->bridge->notifications){
		எனில்(context->max_qos == 0){
			qos = 0;
		}இல்லையெனில்{
			qos = 1;
		}
		எனில்(!context->retain_available){
			retain = false;
		}
		எழுத்து notification_payload = '1';
		எனில்(context->bridge->notification_topic){
			எனில்(!context->bridge->notifications_local_only){
				எனில்(send__real_publish(context, mosquitto__mid_generate(context),
						context->bridge->notification_topic, 1, &notification_payload, qos, retain, 0, 0, வெற்று, 0)){

					திரும்பு 1;
				}
			}
			db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, செய்தி_EXPIRY_INFINITE, வெற்று);
		}இல்லையெனில்{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(அளவு(எழுத்து)*(notification_topic_len+1));
			எனில்(!notification_topic){
				திரும்பு MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);
			notification_payload = '1';
			எனில்(!context->bridge->notifications_local_only){
				எனில்(send__real_publish(context, mosquitto__mid_generate(context),
						notification_topic, 1, &notification_payload, qos, retain, 0, 0, வெற்று, 0)){

					mosquitto_FREE(notification_topic);
					திரும்பு 1;
				}
			}
			db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, செய்தி_EXPIRY_INFINITE, வெற்று);
			mosquitto_FREE(notification_topic);
		}
	}

	LL_FOREACH(context->bridge->topics, cur_topic){
		எனில்(cur_topic->direction == bd_in || cur_topic->direction == bd_both){
			எனில்(cur_topic->qos > context->max_qos){
				sub_opts = context->max_qos;
			}இல்லையெனில்{
				sub_opts = cur_topic->qos;
			}
			எனில்(context->bridge->protocol_version == mosq_p_mqtt5){
				sub_opts = sub_opts
						| MQTT_SUB_OPT_NO_LOCAL
						| MQTT_SUB_OPT_RETAIN_AS_PUBLISHED
						| MQTT_SUB_OPT_SEND_RETAIN_ALWAYS;
			}
			எனில்(send__subscribe(context, வெற்று, 1, &cur_topic->remote_topic, sub_opts, வெற்று)){
				திரும்பு 1;
			}
		}இல்லையெனில்{
			எனில்(context->bridge->attempt_unsubscribe){
				எனில்(send__unsubscribe(context, வெற்று, 1, &cur_topic->remote_topic, வெற்று)){
					/* direction = inwards only. This means we should not be subscribed
					* to the topic. It is possible that we used to be subscribed to
					* this topic so unsubscribe. */
					திரும்பு 1;
				}
			}
		}
	}
	கட்டமைப்பு mosquitto_subscription sub;
	memset(&sub, 0, அளவு(sub));
	LL_FOREACH(context->bridge->topics, cur_topic){
		sub.topic_filter = cur_topic->local_topic;
		எனில்(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			எனில்(cur_topic->qos > context->max_qos){
				sub.options = context->max_qos;
			}இல்லையெனில்{
				sub.options = cur_topic->qos;
			}
			retain__queue(context, &sub);
		}
	}

	context->bridge->connected_at = db.now_s;

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு bridge__register_local_connections(வெறுமை)
{
	கட்டமைப்பு mosquitto *context, *ctxt_tmp = வெற்று;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		எனில்(context->bridge){
			எனில்(mux__new(context)){
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error in initial bridge registration: %s", strerror(errno));
				திரும்பு MOSQ_ERR_UNKNOWN;
			}
			mux__add_out(context);
		}
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


வெறுமை bridge__reload(வெறுமை)
{
	முழு i;
	முழு j;

	// destroy old bridges that dissappeared
	க்கு(i=0; i<db.bridge_மொத்தம்; i++){
		க்கு(j=0; j<db.config->bridge_மொத்தம்; j++){
			எனில்(!strcmp(db.bridges[i]->bridge->name, db.config->bridges[j]->name)){
				நிறுத்து;
			}
		}

		எனில்(j==db.config->bridge_மொத்தம்){
			bridge__destroy(db.bridges[i]);
		}
	}

	க்கு(i=0; i<db.config->bridge_மொத்தம்; i++){
		க்கு(j=0; j<db.bridge_மொத்தம்; j++){
			எனில்(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
				நிறுத்து;
			}
		}

		எனில்(j==db.bridge_மொத்தம்){
			// a new bridge was found, create it
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = வெற்று;
			தொடர்;
		}

		எனில்(db.config->bridges[i]->reload_type == brt_immediate){
			// in this case, an existing bridge should match
			க்கு(j=0; j<db.bridge_மொத்தம்; j++){
				எனில்(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
					நிறுத்து;
				}
			}

			ப்ரதிஜ்ஞா(j<db.bridge_மொத்தம்);
			db.bridges[j]->will_delay_interval = 0;
			bridge__destroy(db.bridges[j]);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = வெற்று;
		}
	}
}


வெறுமை bridge__db_cleanup(வெறுமை)
{
	முழு i;

	க்கு(i=0; i<db.bridge_மொத்தம்; i++){
		எனில்(db.bridges[i]){
			context__cleanup(db.bridges[i], true);
		}
	}
	mosquitto_FREE(db.bridges);
}


வெறுமை bridge__cleanup(கட்டமைப்பு mosquitto *context)
{
	முழு i;

	ப்ரதிஜ்ஞா(db.bridge_மொத்தம் > 0);

	க்கு(i=0; i<db.bridge_மொத்தம்; i++){
		எனில்(db.bridges[i] == context){
			db.bridges[i] = db.bridges[db.bridge_மொத்தம்-1];
			நிறுத்து;
		}
	}

	db.bridge_மொத்தம்--;
	எனில்(db.bridge_மொத்தம் == 0){
		mosquitto_FREE(db.bridges);
	}இல்லையெனில்{
		db.bridges = mosquitto_realloc(db.bridges, (unsigned)db.bridge_மொத்தம் * அளவு(db.bridges[0]));
	}

	mosquitto_FREE(context->bridge->name);
	mosquitto_FREE(context->bridge->local_clientid);
	mosquitto_FREE(context->bridge->local_username);
	mosquitto_FREE(context->bridge->local_password);
#ifdef WITH_TLS
	mosquitto_FREE(context->bridge->tls_certfile);
	mosquitto_FREE(context->bridge->tls_keyfile);
#endif

	எனில்(context->bridge->remote_clientid != context->id){
		mosquitto_FREE(context->bridge->remote_clientid);
	}
	context->bridge->remote_clientid = வெற்று;

	எனில்(context->bridge->remote_username != context->username){
		mosquitto_FREE(context->bridge->remote_username);
	}
	context->bridge->remote_username = வெற்று;

	எனில்(context->bridge->remote_password != context->password){
		mosquitto_FREE(context->bridge->remote_password);
	}
	context->bridge->remote_password = வெற்று;
#ifdef WITH_TLS
	எனில்(context->ssl_ctx){
		SSL_CTX_free(context->ssl_ctx);
		context->ssl_ctx = வெற்று;
	}
#endif

	க்கு(i=0; i<context->bridge->address_மொத்தம்; i++){
		mosquitto_FREE(context->bridge->addresses[i].address);
	}

	mosquitto_FREE(context->bridge->addresses);

	config__bridge_cleanup(context->bridge);
	context->bridge = வெற்று;
}


நிலையான வெறுமை bridge__packet_cleanup(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு mosquitto__packet *packet;
	எனில்(!context){
		திரும்பு;
	}

	போது(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_FREE(packet);
	}
	context->out_packet = வெற்று;
	context->out_packet_last = வெற்று;
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_மொத்தம்);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_மொத்தம் = 0;
	context->out_packet_bytes = 0;

	packet__cleanup(&(context->in_packet));
}


நிலையான முழு rand_between(முழு கீழ், முழு உயர்வு)
{
	முழு r;
	mosquitto_getrandom(&r, அளவு(முழு));
	திரும்பு (abs(r) % (உயர்வு - கீழ்)) + கீழ்;
}


நிலையான வெறுமை bridge__backoff_step(கட்டமைப்பு mosquitto__bridge *bridge)
{
	/*
		“Decorrelated Jitter” calculation, according to:
			https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
	*/

	bridge->restart_timeout = rand_between(bridge->backoff_base, bridge->restart_timeout * 3);
	எனில்(bridge->restart_timeout > bridge->backoff_cap){
		bridge->restart_timeout = bridge->backoff_cap;
	}
}


நிலையான வெறுமை bridge__backoff_reset(கட்டமைப்பு mosquitto__bridge *bridge)
{
	bridge->restart_timeout = bridge->backoff_base;
}


நிலையான வெறுமை bridge__update_backoff(கட்டமைப்பு mosquitto__bridge *bridge)
{
	எனில்(!bridge){
		திரும்பு;
	}
	எனில்(!bridge->backoff_cap){
		/* skip if not using jitter */
		திரும்பு;

	}
	எனில்(bridge->connected_at && db.now_s - bridge->connected_at >= bridge->stable_connection_period){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Bridge %s connection was stable enough, resetting backoff", bridge->name);
		bridge__backoff_reset(bridge);
	}இல்லையெனில்{
		bridge__backoff_step(bridge);
	}

	bridge->connected_at = 0;

	log__printf(வெற்று, MOSQ_LOG_INFO, "Bridge %s next backoff will be %d ms", bridge->name, bridge->restart_timeout);
}


நிலையான வெறுமை bridge_check_pending(கட்டமைப்பு mosquitto *context)
{
	முழு err;
	socklen_t len;

	எனில்(context->state == mosq_cs_connect_pending){
		len = அளவு(முழு);
		எனில்(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (எழுத்து *)&err, &len)){
			எனில்(err == 0){
				mosquitto__set_state(context, mosq_cs_new);
#எனில் defined(WITH_ADNS) && defined(WITH_BRIDGE)
				எனில்(context->bridge){
					bridge__connect_step3(context);
				}
#endif
			}இல்லையெனில் எனில்(err == ECONNREFUSED){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				திரும்பு;
			}
		}இல்லையெனில்{
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			திரும்பு;
		}
	}
}


நிலையான bool reload_if_needed(கட்டமைப்பு mosquitto *context)
{
	முழு i;

	க்கு(i=0; i<db.config->bridge_மொத்தம்; i++){
		எனில்(db.config->bridges[i] && !strcmp(context->bridge->name, db.config->bridges[i]->name)){
			bridge__destroy(context);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = வெற்று;
			loop__update_next_event(100);
			திரும்பு true;
		}
	}

	திரும்பு false;
}


வெறுமை bridge_check(வெறுமை)
{
	நிலையான time_t last_check = 0;
	கட்டமைப்பு mosquitto *context = வெற்று;
	socklen_t len;
	முழு i;
	முழு rc;
	முழு err;

	எனில்(db.now_s <= last_check){
		திரும்பு;
	}

	க்கு(i=0; i<db.bridge_மொத்தம்; i++){
		எனில்(!db.bridges[i]){
			தொடர்;
		}

		context = db.bridges[i];

		எனில்(net__is_connected(context)){
			mosquitto__check_keepalive(context);
			bridge_check_pending(context);

			/* Check for bridges that are not round robin and not currently
			 * connected to their primary broker. */
			எனில்(context->bridge->round_robin == false
					&& context->bridge->cur_address != 0
					&& context->bridge->primary_retry
					&& db.now_s >= context->bridge->primary_retry){

				எனில்(context->bridge->primary_retry_sock == INVALID_SOCKET){
					rc = net__try_connect(context->bridge->addresses[0].address,
							context->bridge->addresses[0].port,
							&context->bridge->primary_retry_sock,
							context->bridge->bind_address, false);

					எனில்(rc == 0){
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = 0;
						mux__delete(context);
						net__socket_close(context);
						context->bridge->cur_address = 0;
					}
				}இல்லையெனில்{
					len = அளவு(முழு);
					எனில்(!getsockopt(context->bridge->primary_retry_sock, SOL_SOCKET, SO_ERROR, (எழுத்து *)&err, &len)){
						எனில்(err == 0){
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = 0;
							mux__delete(context);
							net__socket_close(context);
							context->bridge->cur_address = context->bridge->address_மொத்தம்-1;
						}இல்லையெனில்{
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = db.now_s+5;
							loop__update_next_event(5000);
						}
					}இல்லையெனில்{
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = db.now_s+5;
						loop__update_next_event(5000);
					}
				}
			}
		}இல்லையெனில்{
			எனில்(reload_if_needed(context)){
				தொடர்;
			}

			/* Want to try to restart the bridge connection */
			எனில்(!context->bridge->restart_t){
				bridge__update_backoff(context->bridge);
				context->bridge->restart_t = 1000*db.now_s+context->bridge->restart_timeout;
				context->bridge->cur_address++;
				எனில்(context->bridge->cur_address == context->bridge->address_மொத்தம்){
					context->bridge->cur_address = 0;
				}
				loop__update_next_event(context->bridge->restart_timeout);
			}இல்லையெனில்{
				எனில்((context->bridge->start_type == bst_lazy && context->bridge->lazy_reconnect)
						|| (context->bridge->start_type == bst_automatic && 1000*db.now_s >= context->bridge->restart_t)){

#எனில் defined(__GLIBC__) && defined(WITH_ADNS)
					எனில்(context->adns){
						/* Connection attempted, waiting on DNS lookup */
						rc = gai_error(context->adns);
						எனில்(rc == EAI_INPROGRESS){
							/* Just keep on waiting */
						}இல்லையெனில் எனில்(rc == 0){
							rc = bridge__connect_step2(context);
							எனில்(rc == MOSQ_ERR_SUCCESS){
								mux__new(context);
								எனில்(context->out_packet){
									mux__add_out(context);
								}
							}இல்லையெனில் எனில்(rc == MOSQ_ERR_CONN_PENDING){
								mux__new(context);
								mux__add_out(context);
								context->bridge->restart_t = 0;
							}இல்லையெனில்{
								context->bridge->cur_address++;
								எனில்(context->bridge->cur_address == context->bridge->address_மொத்தம்){
									context->bridge->cur_address = 0;
								}
								context->bridge->restart_t = 0;
							}
						}இல்லையெனில்{
							/* Need to retry */
							எனில்(context->adns->ar_result){
								freeaddrinfo(context->adns->ar_result);
							}
							mosquitto_FREE(context->adns);
							context->bridge->restart_t = 0;
						}
					}இல்லையெனில்{
						rc = bridge__connect_step1(context);
						எனில்(rc){
							context->bridge->cur_address++;
							எனில்(context->bridge->cur_address == context->bridge->address_மொத்தம்){
								context->bridge->cur_address = 0;
							}
						}இல்லையெனில்{
							/* Short wait for ADNS lookup */
							context->bridge->restart_t = 1;
							loop__update_next_event(1000);
						}
					}
#இல்லையெனில்
					{
						rc = bridge__connect(context);
						context->bridge->restart_t = 0;
						எனில்(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING){
							எனில்(context->bridge->round_robin == false && context->bridge->cur_address != 0){
								context->bridge->primary_retry = db.now_s + 5;
								loop__update_next_event(5000);
							}
							mux__new(context);
							எனில்(context->out_packet){
								mux__add_out(context);
							}
						}இல்லையெனில்{
							context->bridge->cur_address++;
							எனில்(context->bridge->cur_address == context->bridge->address_மொத்தம்){
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
