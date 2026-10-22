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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <errno.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#ifndef WIN32
#অন্তর্ভাব <netdb.h>
#অন্তর্ভাব <sys/socket.h>
#অন্তর্ভাব <netinet/in.h>
#অন্তর্ভাব <netinet/tcp.h>
#অন্যথা
#অন্তর্ভাব <winsock2.h>
#অন্তর্ভাব <ws2tcpip.h>
#endif

#ifndef WIN32
#অন্তর্ভাব <unistd.h>
#অন্যথা
#অন্তর্ভাব <process.h>
#অন্তর্ভাব <winsock2.h>
#অন্তর্ভাব <ws2tcpip.h>
#endif

#অন্তর্ভাব "mosquitto.h"
#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto_internal.h"
#অন্তর্ভাব "net_mosq.h"
#অন্তর্ভাব "packet_mosq.h"
#অন্তর্ভাব "property_common.h"
#অন্তর্ভাব "send_mosq.h"
#অন্তর্ভাব "sys_tree.h"
#অন্তর্ভাব "tls_mosq.h"
#অন্তর্ভাব "util_mosq.h"
#অন্তর্ভাব "will_mosq.h"
#অন্তর্ভাব "utlist.h"

#ifdef WITH_BRIDGE

স্থির শূন্য bridge__update_backoff(সংরচনা mosquitto__bridge *bridge);
#যদি defined(__GLIBC__) && defined(WITH_ADNS)
স্থির পূর্ণাঙ্ক bridge__connect_step1(সংরচনা mosquitto *context);
স্থির পূর্ণাঙ্ক bridge__connect_step2(সংরচনা mosquitto *context);
#endif
স্থির শূন্য bridge__packet_cleanup(সংরচনা mosquitto *context);

স্থির সংরচনা mosquitto *bridge__new(সংরচনা mosquitto__bridge *bridge)
{
	সংরচনা mosquitto *new_context = রিক্ত;
	সংরচনা mosquitto **bridges;
	বর্ণ *local_id;

	প্রতিজ্ঞা(bridge);

	local_id = mosquitto_strdup(bridge->local_clientid);
	যদি(!local_id){
		প্রত্যাগম রিক্ত;
	}

	HASH_FIND(hh_id, db.contexts_by_id, local_id, strlen(local_id), new_context);
	যদি(new_context){
		/* (possible from persistent db) */
		mosquitto_FREE(local_id);
	}অন্যথা{
		/* id wasn't found, so generate a new context */
		new_context = context__init();
		যদি(!new_context){
			mosquitto_FREE(local_id);
			প্রত্যাগম রিক্ত;
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
	new_context->tls_engine = রিক্ত;
	new_context->tls_keyform = mosq_k_pem;
	new_context->tls_use_os_certs = bridge->tls_use_os_certs;
	new_context->ssl_ctx_defaults = true;
#ifdef FINAL_WITH_TLS_PSK
	new_context->tls_psk_identity = bridge->tls_psk_identity;
	new_context->tls_psk = bridge->tls_psk;
#endif
#endif

	bridge->try_private_accepted = true;
	যদি(bridge->clean_start_local == -1){
		/* default to "regular" clean start setting */
		bridge->clean_start_local = bridge->clean_start;
	}
	new_context->retain_available = bridge->outgoing_retain;
	new_context->protocol = bridge->protocol_version;
	যদি(!bridge->clean_start_local){
		new_context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
		plugin_persist__handle_client_add(new_context);
		যদি(new_context->expiry_list_item){
			/* We've restored from persistence and been added to the session
			 * expiry list, even though we should never be expired */
			session_expiry__remove(new_context);
		}
	}

	bridges = mosquitto_realloc(db.bridges, (size_t)(db.bridge_গুনতি+1)*পরিমাণ(সংরচনা mosquitto *));
	যদি(bridges){
		db.bridges = bridges;
		db.bridge_গুনতি++;
		db.bridges[db.bridge_গুনতি-1] = new_context;
	}অন্যথা{
		প্রত্যাগম রিক্ত;
	}

	প্রত্যাগম new_context;
}


স্থির শূন্য bridge__destroy(সংরচনা mosquitto *context)
{
	send__disconnect(context, MQTT_RC_SUCCESS, রিক্ত);
	context__cleanup(context, true);
}


শূন্য bridge__start_all(শূন্য)
{
	প্রতি(পূর্ণাঙ্ক i=0; i<db.config->bridge_গুনতি; i++){
		সংরচনা mosquitto *context;
		পূর্ণাঙ্ক ret;

		context = bridge__new(db.config->bridges[i]);
		যদি(!context){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
			প্রত্যাগম;
		}
		প্রতিজ্ঞা(context);

#যদি defined(__GLIBC__) && defined(WITH_ADNS)
		context->bridge->restart_t = 1; /* force quick restart of bridge */
		loop__update_next_event(1000);
		ret = bridge__connect_step1(context);
#অন্যথা
		ret = bridge__connect(context);
#endif

		যদি(ret && ret != MOSQ_ERR_CONN_PENDING){
			log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Unable to connect bridge %s.",
					context->bridge->name);
		}

		db.config->bridges[i] = রিক্ত;
	}
}


স্থির পূর্ণাঙ্ক bridge__set_tcp_keepalive(সংরচনা mosquitto *context)
{
	unsigned পূর্ণাঙ্ক enabled = 1;
	bool ret;

	যদি(context->bridge->tcp_keepalive_idle == 0
			|| context->bridge->tcp_keepalive_interval == 0
			|| context->bridge->tcp_keepalive_counter == 0){

		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}

#ifdef WIN32
#  define SETSOCKOPT_TYPE বর্ণ *
#অন্যথা
#  define SETSOCKOPT_TYPE ধ্রুব শূন্য *
#endif

	ret = setsockopt(context->sock, SOL_SOCKET, SO_KEEPALIVE, (SETSOCKOPT_TYPE)&enabled, পরিমাণ(enabled));

#ifdef TCP_KEEPIDLE
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPIDLE,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_idle, পরিমাণ(context->bridge->tcp_keepalive_idle));
#endif
#ifdef TCP_KEEPINTVL
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPINTVL,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_interval, পরিমাণ(context->bridge->tcp_keepalive_interval));
#endif
#ifdef TCP_KEEPCNT
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPCNT,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_counter, পরিমাণ(context->bridge->tcp_keepalive_counter));
#endif

#undef SETSOCKOPT_TYPE

	যদি(ret){
		প্রত্যাগম MOSQ_ERR_UNKNOWN;
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TCP_USER_TIMEOUT


স্থির পূর্ণাঙ্ক bridge__set_tcp_user_timeout(সংরচনা mosquitto *context)
{
	পূর্ণাঙ্ক timeout = context->bridge->tcp_user_timeout;
	যদি(timeout >= 0){
		যদি(setsockopt(context->sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (বর্ণ *)&timeout, পরিমাণ(timeout))){
			প্রত্যাগম MOSQ_ERR_UNKNOWN;
		}
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}
#endif

#যদি defined(__GLIBC__) && defined(WITH_ADNS)


স্থির পূর্ণাঙ্ক bridge__connect_step1(সংরচনা mosquitto *context)
{
	পূর্ণাঙ্ক rc;
	বর্ণ *notification_topic;
	size_t notification_topic_len;
	uint8_t notification_payload;
	সংরচনা mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	যদি(!context || !context->bridge){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_বার্তা_in = db.now_s;
	context->next_বার্তা_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = রিক্ত;
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
		যদি(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(রিক্ত, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			যদি(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}অন্যথা{
				qos = cur_topic->qos;
			}
			সংরচনা mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			যদি(sub__add(context, &sub) > 0){
				প্রত্যাগম 1;
			}
			retain__queue(context, &sub);
		}
	}

	যদি(context->bridge->notifications){
		যদি(context->max_qos == 0){
			qos = 0;
		}অন্যথা{
			qos = 1;
		}
		যদি(context->bridge->notification_topic){
			যদি(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, বার্তা_EXPIRY_INFINITE, রিক্ত);
				context->bridge->initial_notification_done = true;
			}
			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, রিক্ত);
			যদি(rc != MOSQ_ERR_SUCCESS){
				প্রত্যাগম rc;
			}
		}অন্যথা{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(পরিমাণ(বর্ণ)*(notification_topic_len+1));
			যদি(!notification_topic){
				প্রত্যাগম MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			যদি(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, বার্তা_EXPIRY_INFINITE, রিক্ত);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, রিক্ত);
			mosquitto_FREE(notification_topic);
			যদি(rc != MOSQ_ERR_SUCCESS){
				প্রত্যাগম rc;
			}
		}
	}

	log__printf(রিক্ত, MOSQ_LOG_NOTICE, "Connecting bridge (step 1) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step1(context, context->bridge->addresses[context->bridge->cur_address].address);
	যদি(rc > 0){
		যদি(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			প্রত্যাগম rc; /* Error already printed */
		}অন্যথা যদি(rc == MOSQ_ERR_ERRNO){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}অন্যথা যদি(rc == MOSQ_ERR_EAI){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		প্রত্যাগম rc;
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণাঙ্ক bridge__connect_step2(সংরচনা mosquitto *context)
{
	পূর্ণাঙ্ক rc;

	যদি(!context || !context->bridge){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	log__printf(রিক্ত, MOSQ_LOG_NOTICE, "Connecting bridge (step 2) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step2(context, context->bridge->addresses[context->bridge->cur_address].port, &context->sock);
	যদি(rc > 0){
		যদি(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			প্রত্যাগম rc; /* Error already printed */
		}অন্যথা যদি(rc == MOSQ_ERR_ERRNO){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}অন্যথা যদি(rc == MOSQ_ERR_EAI){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		প্রত্যাগম rc;
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, পরিমাণ(context->sock), context);

	যদি(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}
	প্রত্যাগম rc;
}


পূর্ণাঙ্ক bridge__connect_step3(সংরচনা mosquitto *context)
{
	পূর্ণাঙ্ক rc;
	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = রিক্ত;

	rc = net__socket_connect_step3(context, context->bridge->addresses[context->bridge->cur_address].address);
	যদি(rc > 0){
		যদি(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			প্রত্যাগম rc; /* Error already printed */
		}অন্যথা যদি(rc == MOSQ_ERR_ERRNO){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}অন্যথা যদি(rc == MOSQ_ERR_EAI){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		প্রত্যাগম rc;
	}

	যদি(context->bridge->round_robin == false && context->bridge->cur_address != 0){
		context->bridge->primary_retry = db.now_s + 5;
		loop__update_next_event(5000);
	}

	যদি(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		প্রত্যাগম MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	যদি(bridge__set_tcp_user_timeout(context)){
		প্রত্যাগম MOSQ_ERR_UNKNOWN;
	}
#endif

	যদি(context->bridge->receive_maximum != 0){
		receive_maximum.মান.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	যদি(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.মান.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	যদি(context->bridge->max_topic_alias != 0){
		topic_alias_max.মান.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc = send__connect(context, context->keepalive, context->clean_start, properties);
	যদি(rc == MOSQ_ERR_SUCCESS){
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}অন্যথা যদি(rc == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}অন্যথা{
		যদি(rc == MOSQ_ERR_TLS){
			প্রত্যাগম rc; /* Error already printed */
		}অন্যথা যদি(rc == MOSQ_ERR_ERRNO){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}অন্যথা যদি(rc == MOSQ_ERR_EAI){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		প্রত্যাগম rc;
	}
}
#অন্যথা


পূর্ণাঙ্ক bridge__connect(সংরচনা mosquitto *context)
{
	পূর্ণাঙ্ক rc, rc2;
	বর্ণ *notification_topic = রিক্ত;
	size_t notification_topic_len;
	uint8_t notification_payload;
	সংরচনা mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = রিক্ত;

	যদি(!context || !context->bridge){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->last_বার্তা_in = db.now_s;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->next_বার্তা_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = রিক্ত;
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
		যদি(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(রিক্ত, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			যদি(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}অন্যথা{
				qos = cur_topic->qos;
			}
			সংরচনা mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			যদি(sub__add(context, &sub) > 0){
				প্রত্যাগম 1;
			}
		}
	}

	যদি(context->bridge->notifications){
		যদি(context->max_qos == 0){
			qos = 0;
		}অন্যথা{
			qos = 1;
		}
		যদি(context->bridge->notification_topic){
			যদি(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, বার্তা_EXPIRY_INFINITE, রিক্ত);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, রিক্ত);
			যদি(rc != MOSQ_ERR_SUCCESS){
				প্রত্যাগম rc;
			}
		}অন্যথা{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(পরিমাণ(বর্ণ)*(notification_topic_len+1));
			যদি(!notification_topic){
				প্রত্যাগম MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			যদি(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, বার্তা_EXPIRY_INFINITE, রিক্ত);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, রিক্ত);
			যদি(rc != MOSQ_ERR_SUCCESS){
				mosquitto_FREE(notification_topic);
				প্রত্যাগম rc;
			}
			mosquitto_FREE(notification_topic);
		}
	}

	log__printf(রিক্ত, MOSQ_LOG_NOTICE, "Connecting bridge %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__socket_connect(context,
			context->bridge->addresses[context->bridge->cur_address].address,
			context->bridge->addresses[context->bridge->cur_address].port,
			context->bridge->bind_address,
			false);

	যদি(rc > 0){
		যদি(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			প্রত্যাগম rc; /* Error already printed */
		}অন্যথা যদি(rc == MOSQ_ERR_ERRNO){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}অন্যথা যদি(rc == MOSQ_ERR_EAI){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		প্রত্যাগম rc;
	}অন্যথা যদি(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, পরিমাণ(context->sock), context);

	যদি(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		প্রত্যাগম MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	যদি(bridge__set_tcp_user_timeout(context)){
		প্রত্যাগম MOSQ_ERR_UNKNOWN;
	}
#endif

	যদি(context->bridge->receive_maximum != 0){
		receive_maximum.মান.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	যদি(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.মান.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	যদি(context->bridge->max_topic_alias != 0){
		topic_alias_max.মান.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc2 = send__connect(context, context->keepalive, context->clean_start, properties);
	যদি(rc2 == MOSQ_ERR_SUCCESS){
		প্রত্যাগম rc;
	}অন্যথা যদি(rc2 == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}অন্যথা{
		যদি(rc2 == MOSQ_ERR_TLS){
			প্রত্যাগম rc2; /* Error already printed */
		}অন্যথা যদি(rc2 == MOSQ_ERR_ERRNO){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}অন্যথা যদি(rc2 == MOSQ_ERR_EAI){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		প্রত্যাগম rc2;
	}
}
#endif


পূর্ণাঙ্ক bridge__on_connect(সংরচনা mosquitto *context)
{
	বর্ণ *notification_topic;
	size_t notification_topic_len;
	সংরচনা mosquitto__bridge_topic *cur_topic;
	পূর্ণাঙ্ক sub_opts;
	bool retain = true;
	uint8_t qos;

	যদি(context->bridge->notifications){
		যদি(context->max_qos == 0){
			qos = 0;
		}অন্যথা{
			qos = 1;
		}
		যদি(!context->retain_available){
			retain = false;
		}
		বর্ণ notification_payload = '1';
		যদি(context->bridge->notification_topic){
			যদি(!context->bridge->notifications_local_only){
				যদি(send__real_publish(context, mosquitto__mid_generate(context),
						context->bridge->notification_topic, 1, &notification_payload, qos, retain, 0, 0, রিক্ত, 0)){

					প্রত্যাগম 1;
				}
			}
			db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, বার্তা_EXPIRY_INFINITE, রিক্ত);
		}অন্যথা{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(পরিমাণ(বর্ণ)*(notification_topic_len+1));
			যদি(!notification_topic){
				প্রত্যাগম MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);
			notification_payload = '1';
			যদি(!context->bridge->notifications_local_only){
				যদি(send__real_publish(context, mosquitto__mid_generate(context),
						notification_topic, 1, &notification_payload, qos, retain, 0, 0, রিক্ত, 0)){

					mosquitto_FREE(notification_topic);
					প্রত্যাগম 1;
				}
			}
			db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, বার্তা_EXPIRY_INFINITE, রিক্ত);
			mosquitto_FREE(notification_topic);
		}
	}

	LL_FOREACH(context->bridge->topics, cur_topic){
		যদি(cur_topic->direction == bd_in || cur_topic->direction == bd_both){
			যদি(cur_topic->qos > context->max_qos){
				sub_opts = context->max_qos;
			}অন্যথা{
				sub_opts = cur_topic->qos;
			}
			যদি(context->bridge->protocol_version == mosq_p_mqtt5){
				sub_opts = sub_opts
						| MQTT_SUB_OPT_NO_LOCAL
						| MQTT_SUB_OPT_RETAIN_AS_PUBLISHED
						| MQTT_SUB_OPT_SEND_RETAIN_ALWAYS;
			}
			যদি(send__subscribe(context, রিক্ত, 1, &cur_topic->remote_topic, sub_opts, রিক্ত)){
				প্রত্যাগম 1;
			}
		}অন্যথা{
			যদি(context->bridge->attempt_unsubscribe){
				যদি(send__unsubscribe(context, রিক্ত, 1, &cur_topic->remote_topic, রিক্ত)){
					/* direction = inwards only. This means we should not be subscribed
					* to the topic. It is possible that we used to be subscribed to
					* this topic so unsubscribe. */
					প্রত্যাগম 1;
				}
			}
		}
	}
	সংরচনা mosquitto_subscription sub;
	memset(&sub, 0, পরিমাণ(sub));
	LL_FOREACH(context->bridge->topics, cur_topic){
		sub.topic_filter = cur_topic->local_topic;
		যদি(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			যদি(cur_topic->qos > context->max_qos){
				sub.options = context->max_qos;
			}অন্যথা{
				sub.options = cur_topic->qos;
			}
			retain__queue(context, &sub);
		}
	}

	context->bridge->connected_at = db.now_s;

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক bridge__register_local_connections(শূন্য)
{
	সংরচনা mosquitto *context, *ctxt_tmp = রিক্ত;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		যদি(context->bridge){
			যদি(mux__new(context)){
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error in initial bridge registration: %s", strerror(errno));
				প্রত্যাগম MOSQ_ERR_UNKNOWN;
			}
			mux__add_out(context);
		}
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


শূন্য bridge__reload(শূন্য)
{
	পূর্ণাঙ্ক i;
	পূর্ণাঙ্ক j;

	// destroy old bridges that dissappeared
	প্রতি(i=0; i<db.bridge_গুনতি; i++){
		প্রতি(j=0; j<db.config->bridge_গুনতি; j++){
			যদি(!strcmp(db.bridges[i]->bridge->name, db.config->bridges[j]->name)){
				ভঙ্গ;
			}
		}

		যদি(j==db.config->bridge_গুনতি){
			bridge__destroy(db.bridges[i]);
		}
	}

	প্রতি(i=0; i<db.config->bridge_গুনতি; i++){
		প্রতি(j=0; j<db.bridge_গুনতি; j++){
			যদি(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
				ভঙ্গ;
			}
		}

		যদি(j==db.bridge_গুনতি){
			// a new bridge was found, create it
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = রিক্ত;
			অনুবর্ত;
		}

		যদি(db.config->bridges[i]->reload_type == brt_immediate){
			// in this case, an existing bridge should match
			প্রতি(j=0; j<db.bridge_গুনতি; j++){
				যদি(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
					ভঙ্গ;
				}
			}

			প্রতিজ্ঞা(j<db.bridge_গুনতি);
			db.bridges[j]->will_delay_interval = 0;
			bridge__destroy(db.bridges[j]);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = রিক্ত;
		}
	}
}


শূন্য bridge__db_cleanup(শূন্য)
{
	পূর্ণাঙ্ক i;

	প্রতি(i=0; i<db.bridge_গুনতি; i++){
		যদি(db.bridges[i]){
			context__cleanup(db.bridges[i], true);
		}
	}
	mosquitto_FREE(db.bridges);
}


শূন্য bridge__cleanup(সংরচনা mosquitto *context)
{
	পূর্ণাঙ্ক i;

	প্রতিজ্ঞা(db.bridge_গুনতি > 0);

	প্রতি(i=0; i<db.bridge_গুনতি; i++){
		যদি(db.bridges[i] == context){
			db.bridges[i] = db.bridges[db.bridge_গুনতি-1];
			ভঙ্গ;
		}
	}

	db.bridge_গুনতি--;
	যদি(db.bridge_গুনতি == 0){
		mosquitto_FREE(db.bridges);
	}অন্যথা{
		db.bridges = mosquitto_realloc(db.bridges, (unsigned)db.bridge_গুনতি * পরিমাণ(db.bridges[0]));
	}

	mosquitto_FREE(context->bridge->name);
	mosquitto_FREE(context->bridge->local_clientid);
	mosquitto_FREE(context->bridge->local_username);
	mosquitto_FREE(context->bridge->local_password);
#ifdef WITH_TLS
	mosquitto_FREE(context->bridge->tls_certfile);
	mosquitto_FREE(context->bridge->tls_keyfile);
#endif

	যদি(context->bridge->remote_clientid != context->id){
		mosquitto_FREE(context->bridge->remote_clientid);
	}
	context->bridge->remote_clientid = রিক্ত;

	যদি(context->bridge->remote_username != context->username){
		mosquitto_FREE(context->bridge->remote_username);
	}
	context->bridge->remote_username = রিক্ত;

	যদি(context->bridge->remote_password != context->password){
		mosquitto_FREE(context->bridge->remote_password);
	}
	context->bridge->remote_password = রিক্ত;
#ifdef WITH_TLS
	যদি(context->ssl_ctx){
		SSL_CTX_free(context->ssl_ctx);
		context->ssl_ctx = রিক্ত;
	}
#endif

	প্রতি(i=0; i<context->bridge->address_গুনতি; i++){
		mosquitto_FREE(context->bridge->addresses[i].address);
	}

	mosquitto_FREE(context->bridge->addresses);

	config__bridge_cleanup(context->bridge);
	context->bridge = রিক্ত;
}


স্থির শূন্য bridge__packet_cleanup(সংরচনা mosquitto *context)
{
	সংরচনা mosquitto__packet *packet;
	যদি(!context){
		প্রত্যাগম;
	}

	যাবত্(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_FREE(packet);
	}
	context->out_packet = রিক্ত;
	context->out_packet_last = রিক্ত;
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_গুনতি);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_গুনতি = 0;
	context->out_packet_bytes = 0;

	packet__cleanup(&(context->in_packet));
}


স্থির পূর্ণাঙ্ক rand_between(পূর্ণাঙ্ক নিচের, পূর্ণাঙ্ক উপরের)
{
	পূর্ণাঙ্ক r;
	mosquitto_getrandom(&r, পরিমাণ(পূর্ণাঙ্ক));
	প্রত্যাগম (abs(r) % (উপরের - নিচের)) + নিচের;
}


স্থির শূন্য bridge__backoff_step(সংরচনা mosquitto__bridge *bridge)
{
	/*
		“Decorrelated Jitter” calculation, according to:
			https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
	*/

	bridge->restart_timeout = rand_between(bridge->backoff_base, bridge->restart_timeout * 3);
	যদি(bridge->restart_timeout > bridge->backoff_cap){
		bridge->restart_timeout = bridge->backoff_cap;
	}
}


স্থির শূন্য bridge__backoff_reset(সংরচনা mosquitto__bridge *bridge)
{
	bridge->restart_timeout = bridge->backoff_base;
}


স্থির শূন্য bridge__update_backoff(সংরচনা mosquitto__bridge *bridge)
{
	যদি(!bridge){
		প্রত্যাগম;
	}
	যদি(!bridge->backoff_cap){
		/* skip if not using jitter */
		প্রত্যাগম;

	}
	যদি(bridge->connected_at && db.now_s - bridge->connected_at >= bridge->stable_connection_period){
		log__printf(রিক্ত, MOSQ_LOG_INFO, "Bridge %s connection was stable enough, resetting backoff", bridge->name);
		bridge__backoff_reset(bridge);
	}অন্যথা{
		bridge__backoff_step(bridge);
	}

	bridge->connected_at = 0;

	log__printf(রিক্ত, MOSQ_LOG_INFO, "Bridge %s next backoff will be %d ms", bridge->name, bridge->restart_timeout);
}


স্থির শূন্য bridge_check_pending(সংরচনা mosquitto *context)
{
	পূর্ণাঙ্ক err;
	socklen_t len;

	যদি(context->state == mosq_cs_connect_pending){
		len = পরিমাণ(পূর্ণাঙ্ক);
		যদি(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (বর্ণ *)&err, &len)){
			যদি(err == 0){
				mosquitto__set_state(context, mosq_cs_new);
#যদি defined(WITH_ADNS) && defined(WITH_BRIDGE)
				যদি(context->bridge){
					bridge__connect_step3(context);
				}
#endif
			}অন্যথা যদি(err == ECONNREFUSED){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				প্রত্যাগম;
			}
		}অন্যথা{
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			প্রত্যাগম;
		}
	}
}


স্থির bool reload_if_needed(সংরচনা mosquitto *context)
{
	পূর্ণাঙ্ক i;

	প্রতি(i=0; i<db.config->bridge_গুনতি; i++){
		যদি(db.config->bridges[i] && !strcmp(context->bridge->name, db.config->bridges[i]->name)){
			bridge__destroy(context);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = রিক্ত;
			loop__update_next_event(100);
			প্রত্যাগম true;
		}
	}

	প্রত্যাগম false;
}


শূন্য bridge_check(শূন্য)
{
	স্থির time_t last_check = 0;
	সংরচনা mosquitto *context = রিক্ত;
	socklen_t len;
	পূর্ণাঙ্ক i;
	পূর্ণাঙ্ক rc;
	পূর্ণাঙ্ক err;

	যদি(db.now_s <= last_check){
		প্রত্যাগম;
	}

	প্রতি(i=0; i<db.bridge_গুনতি; i++){
		যদি(!db.bridges[i]){
			অনুবর্ত;
		}

		context = db.bridges[i];

		যদি(net__is_connected(context)){
			mosquitto__check_keepalive(context);
			bridge_check_pending(context);

			/* Check for bridges that are not round robin and not currently
			 * connected to their primary broker. */
			যদি(context->bridge->round_robin == false
					&& context->bridge->cur_address != 0
					&& context->bridge->primary_retry
					&& db.now_s >= context->bridge->primary_retry){

				যদি(context->bridge->primary_retry_sock == INVALID_SOCKET){
					rc = net__try_connect(context->bridge->addresses[0].address,
							context->bridge->addresses[0].port,
							&context->bridge->primary_retry_sock,
							context->bridge->bind_address, false);

					যদি(rc == 0){
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = 0;
						mux__delete(context);
						net__socket_close(context);
						context->bridge->cur_address = 0;
					}
				}অন্যথা{
					len = পরিমাণ(পূর্ণাঙ্ক);
					যদি(!getsockopt(context->bridge->primary_retry_sock, SOL_SOCKET, SO_ERROR, (বর্ণ *)&err, &len)){
						যদি(err == 0){
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = 0;
							mux__delete(context);
							net__socket_close(context);
							context->bridge->cur_address = context->bridge->address_গুনতি-1;
						}অন্যথা{
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = db.now_s+5;
							loop__update_next_event(5000);
						}
					}অন্যথা{
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = db.now_s+5;
						loop__update_next_event(5000);
					}
				}
			}
		}অন্যথা{
			যদি(reload_if_needed(context)){
				অনুবর্ত;
			}

			/* Want to try to restart the bridge connection */
			যদি(!context->bridge->restart_t){
				bridge__update_backoff(context->bridge);
				context->bridge->restart_t = 1000*db.now_s+context->bridge->restart_timeout;
				context->bridge->cur_address++;
				যদি(context->bridge->cur_address == context->bridge->address_গুনতি){
					context->bridge->cur_address = 0;
				}
				loop__update_next_event(context->bridge->restart_timeout);
			}অন্যথা{
				যদি((context->bridge->start_type == bst_lazy && context->bridge->lazy_reconnect)
						|| (context->bridge->start_type == bst_automatic && 1000*db.now_s >= context->bridge->restart_t)){

#যদি defined(__GLIBC__) && defined(WITH_ADNS)
					যদি(context->adns){
						/* Connection attempted, waiting on DNS lookup */
						rc = gai_error(context->adns);
						যদি(rc == EAI_INPROGRESS){
							/* Just keep on waiting */
						}অন্যথা যদি(rc == 0){
							rc = bridge__connect_step2(context);
							যদি(rc == MOSQ_ERR_SUCCESS){
								mux__new(context);
								যদি(context->out_packet){
									mux__add_out(context);
								}
							}অন্যথা যদি(rc == MOSQ_ERR_CONN_PENDING){
								mux__new(context);
								mux__add_out(context);
								context->bridge->restart_t = 0;
							}অন্যথা{
								context->bridge->cur_address++;
								যদি(context->bridge->cur_address == context->bridge->address_গুনতি){
									context->bridge->cur_address = 0;
								}
								context->bridge->restart_t = 0;
							}
						}অন্যথা{
							/* Need to retry */
							যদি(context->adns->ar_result){
								freeaddrinfo(context->adns->ar_result);
							}
							mosquitto_FREE(context->adns);
							context->bridge->restart_t = 0;
						}
					}অন্যথা{
						rc = bridge__connect_step1(context);
						যদি(rc){
							context->bridge->cur_address++;
							যদি(context->bridge->cur_address == context->bridge->address_গুনতি){
								context->bridge->cur_address = 0;
							}
						}অন্যথা{
							/* Short wait for ADNS lookup */
							context->bridge->restart_t = 1;
							loop__update_next_event(1000);
						}
					}
#অন্যথা
					{
						rc = bridge__connect(context);
						context->bridge->restart_t = 0;
						যদি(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING){
							যদি(context->bridge->round_robin == false && context->bridge->cur_address != 0){
								context->bridge->primary_retry = db.now_s + 5;
								loop__update_next_event(5000);
							}
							mux__new(context);
							যদি(context->out_packet){
								mux__add_out(context);
							}
						}অন্যথা{
							context->bridge->cur_address++;
							যদি(context->bridge->cur_address == context->bridge->address_গুনতি){
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
