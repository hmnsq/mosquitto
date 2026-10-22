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

#अन्तर्भाव "config.h"

#अन्तर्भाव <assert.h>
#अन्तर्भाव <errno.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#ifndef WIN32
#अन्तर्भाव <netdb.h>
#अन्तर्भाव <sys/socket.h>
#अन्तर्भाव <netinet/in.h>
#अन्तर्भाव <netinet/tcp.h>
#अण्णहा
#अन्तर्भाव <winsock2.h>
#अन्तर्भाव <ws2tcpip.h>
#endif

#ifndef WIN32
#अन्तर्भाव <unistd.h>
#अण्णहा
#अन्तर्भाव <process.h>
#अन्तर्भाव <winsock2.h>
#अन्तर्भाव <ws2tcpip.h>
#endif

#अन्तर्भाव "mosquitto.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto_internal.h"
#अन्तर्भाव "net_mosq.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "property_common.h"
#अन्तर्भाव "send_mosq.h"
#अन्तर्भाव "sys_tree.h"
#अन्तर्भाव "tls_mosq.h"
#अन्तर्भाव "util_mosq.h"
#अन्तर्भाव "will_mosq.h"
#अन्तर्भाव "utlist.h"

#ifdef WITH_BRIDGE

स्थिर शून्य bridge__update_backoff(संरचना mosquitto__bridge *bridge);
#जइ defined(__GLIBC__) && defined(WITH_ADNS)
स्थिर पूर्णाङ्क bridge__connect_step1(संरचना mosquitto *context);
स्थिर पूर्णाङ्क bridge__connect_step2(संरचना mosquitto *context);
#endif
स्थिर शून्य bridge__packet_cleanup(संरचना mosquitto *context);

स्थिर संरचना mosquitto *bridge__new(संरचना mosquitto__bridge *bridge)
{
	संरचना mosquitto *new_context = रिक्त;
	संरचना mosquitto **bridges;
	वर्ण *local_id;

	प्रतिज्ञा(bridge);

	local_id = mosquitto_strdup(bridge->local_clientid);
	जइ(!local_id){
		प्रत्यागम रिक्त;
	}

	HASH_FIND(hh_id, db.contexts_by_id, local_id, strlen(local_id), new_context);
	जइ(new_context){
		/* (possible from persistent db) */
		mosquitto_FREE(local_id);
	}अण्णहा{
		/* id wasn't found, so generate a new context */
		new_context = context__init();
		जइ(!new_context){
			mosquitto_FREE(local_id);
			प्रत्यागम रिक्त;
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
	new_context->tls_engine = रिक्त;
	new_context->tls_keyform = mosq_k_pem;
	new_context->tls_use_os_certs = bridge->tls_use_os_certs;
	new_context->ssl_ctx_defaults = true;
#ifdef FINAL_WITH_TLS_PSK
	new_context->tls_psk_identity = bridge->tls_psk_identity;
	new_context->tls_psk = bridge->tls_psk;
#endif
#endif

	bridge->try_private_accepted = true;
	जइ(bridge->clean_start_local == -1){
		/* default to "regular" clean start setting */
		bridge->clean_start_local = bridge->clean_start;
	}
	new_context->retain_available = bridge->outgoing_retain;
	new_context->protocol = bridge->protocol_version;
	जइ(!bridge->clean_start_local){
		new_context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
		plugin_persist__handle_client_add(new_context);
		जइ(new_context->expiry_list_item){
			/* We've restored from persistence and been added to the session
			 * expiry list, even though we should never be expired */
			session_expiry__remove(new_context);
		}
	}

	bridges = mosquitto_realloc(db.bridges, (size_t)(db.bridge_गणनम्+1)*परिमाण(संरचना mosquitto *));
	जइ(bridges){
		db.bridges = bridges;
		db.bridge_गणनम्++;
		db.bridges[db.bridge_गणनम्-1] = new_context;
	}अण्णहा{
		प्रत्यागम रिक्त;
	}

	प्रत्यागम new_context;
}


स्थिर शून्य bridge__destroy(संरचना mosquitto *context)
{
	send__disconnect(context, MQTT_RC_SUCCESS, रिक्त);
	context__cleanup(context, true);
}


शून्य bridge__start_all(शून्य)
{
	प्रति(पूर्णाङ्क i=0; i<db.config->bridge_गणनम्; i++){
		संरचना mosquitto *context;
		पूर्णाङ्क ret;

		context = bridge__new(db.config->bridges[i]);
		जइ(!context){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
			प्रत्यागम;
		}
		प्रतिज्ञा(context);

#जइ defined(__GLIBC__) && defined(WITH_ADNS)
		context->bridge->restart_t = 1; /* force quick restart of bridge */
		loop__update_next_event(1000);
		ret = bridge__connect_step1(context);
#अण्णहा
		ret = bridge__connect(context);
#endif

		जइ(ret && ret != MOSQ_ERR_CONN_PENDING){
			log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Unable to connect bridge %s.",
					context->bridge->name);
		}

		db.config->bridges[i] = रिक्त;
	}
}


स्थिर पूर्णाङ्क bridge__set_tcp_keepalive(संरचना mosquitto *context)
{
	unsigned पूर्णाङ्क enabled = 1;
	bool ret;

	जइ(context->bridge->tcp_keepalive_idle == 0
			|| context->bridge->tcp_keepalive_interval == 0
			|| context->bridge->tcp_keepalive_counter == 0){

		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

#ifdef WIN32
#  define SETSOCKOPT_TYPE वर्ण *
#अण्णहा
#  define SETSOCKOPT_TYPE ध्रुव शून्य *
#endif

	ret = setsockopt(context->sock, SOL_SOCKET, SO_KEEPALIVE, (SETSOCKOPT_TYPE)&enabled, परिमाण(enabled));

#ifdef TCP_KEEPIDLE
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPIDLE,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_idle, परिमाण(context->bridge->tcp_keepalive_idle));
#endif
#ifdef TCP_KEEPINTVL
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPINTVL,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_interval, परिमाण(context->bridge->tcp_keepalive_interval));
#endif
#ifdef TCP_KEEPCNT
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPCNT,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_counter, परिमाण(context->bridge->tcp_keepalive_counter));
#endif

#undef SETSOCKOPT_TYPE

	जइ(ret){
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TCP_USER_TIMEOUT


स्थिर पूर्णाङ्क bridge__set_tcp_user_timeout(संरचना mosquitto *context)
{
	पूर्णाङ्क timeout = context->bridge->tcp_user_timeout;
	जइ(timeout >= 0){
		जइ(setsockopt(context->sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (वर्ण *)&timeout, परिमाण(timeout))){
			प्रत्यागम MOSQ_ERR_UNKNOWN;
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}
#endif

#जइ defined(__GLIBC__) && defined(WITH_ADNS)


स्थिर पूर्णाङ्क bridge__connect_step1(संरचना mosquitto *context)
{
	पूर्णाङ्क rc;
	वर्ण *notification_topic;
	size_t notification_topic_len;
	uint8_t notification_payload;
	संरचना mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	जइ(!context || !context->bridge){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_सन्देशः_in = db.now_s;
	context->next_सन्देशः_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = रिक्त;
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
		जइ(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(रिक्त, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			जइ(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}अण्णहा{
				qos = cur_topic->qos;
			}
			संरचना mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			जइ(sub__add(context, &sub) > 0){
				प्रत्यागम 1;
			}
			retain__queue(context, &sub);
		}
	}

	जइ(context->bridge->notifications){
		जइ(context->max_qos == 0){
			qos = 0;
		}अण्णहा{
			qos = 1;
		}
		जइ(context->bridge->notification_topic){
			जइ(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, सन्देशः_EXPIRY_INFINITE, रिक्त);
				context->bridge->initial_notification_done = true;
			}
			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, रिक्त);
			जइ(rc != MOSQ_ERR_SUCCESS){
				प्रत्यागम rc;
			}
		}अण्णहा{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(परिमाण(वर्ण)*(notification_topic_len+1));
			जइ(!notification_topic){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			जइ(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, सन्देशः_EXPIRY_INFINITE, रिक्त);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, रिक्त);
			mosquitto_FREE(notification_topic);
			जइ(rc != MOSQ_ERR_SUCCESS){
				प्रत्यागम rc;
			}
		}
	}

	log__printf(रिक्त, MOSQ_LOG_NOTICE, "Connecting bridge (step 1) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step1(context, context->bridge->addresses[context->bridge->cur_address].address);
	जइ(rc > 0){
		जइ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			प्रत्यागम rc; /* Error already printed */
		}अण्णहा जइ(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}अण्णहा जइ(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		प्रत्यागम rc;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क bridge__connect_step2(संरचना mosquitto *context)
{
	पूर्णाङ्क rc;

	जइ(!context || !context->bridge){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	log__printf(रिक्त, MOSQ_LOG_NOTICE, "Connecting bridge (step 2) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step2(context, context->bridge->addresses[context->bridge->cur_address].port, &context->sock);
	जइ(rc > 0){
		जइ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			प्रत्यागम rc; /* Error already printed */
		}अण्णहा जइ(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}अण्णहा जइ(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		प्रत्यागम rc;
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, परिमाण(context->sock), context);

	जइ(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}
	प्रत्यागम rc;
}


पूर्णाङ्क bridge__connect_step3(संरचना mosquitto *context)
{
	पूर्णाङ्क rc;
	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = रिक्त;

	rc = net__socket_connect_step3(context, context->bridge->addresses[context->bridge->cur_address].address);
	जइ(rc > 0){
		जइ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			प्रत्यागम rc; /* Error already printed */
		}अण्णहा जइ(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}अण्णहा जइ(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		प्रत्यागम rc;
	}

	जइ(context->bridge->round_robin == false && context->bridge->cur_address != 0){
		context->bridge->primary_retry = db.now_s + 5;
		loop__update_next_event(5000);
	}

	जइ(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	जइ(bridge__set_tcp_user_timeout(context)){
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}
#endif

	जइ(context->bridge->receive_maximum != 0){
		receive_maximum.मानम्.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	जइ(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.मानम्.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	जइ(context->bridge->max_topic_alias != 0){
		topic_alias_max.मानम्.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc = send__connect(context, context->keepalive, context->clean_start, properties);
	जइ(rc == MOSQ_ERR_SUCCESS){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अण्णहा जइ(rc == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अण्णहा{
		जइ(rc == MOSQ_ERR_TLS){
			प्रत्यागम rc; /* Error already printed */
		}अण्णहा जइ(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}अण्णहा जइ(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		प्रत्यागम rc;
	}
}
#अण्णहा


पूर्णाङ्क bridge__connect(संरचना mosquitto *context)
{
	पूर्णाङ्क rc, rc2;
	वर्ण *notification_topic = रिक्त;
	size_t notification_topic_len;
	uint8_t notification_payload;
	संरचना mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = रिक्त;

	जइ(!context || !context->bridge){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->last_सन्देशः_in = db.now_s;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->next_सन्देशः_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = रिक्त;
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
		जइ(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(रिक्त, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			जइ(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}अण्णहा{
				qos = cur_topic->qos;
			}
			संरचना mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			जइ(sub__add(context, &sub) > 0){
				प्रत्यागम 1;
			}
		}
	}

	जइ(context->bridge->notifications){
		जइ(context->max_qos == 0){
			qos = 0;
		}अण्णहा{
			qos = 1;
		}
		जइ(context->bridge->notification_topic){
			जइ(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, सन्देशः_EXPIRY_INFINITE, रिक्त);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, रिक्त);
			जइ(rc != MOSQ_ERR_SUCCESS){
				प्रत्यागम rc;
			}
		}अण्णहा{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(परिमाण(वर्ण)*(notification_topic_len+1));
			जइ(!notification_topic){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			जइ(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, सन्देशः_EXPIRY_INFINITE, रिक्त);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, रिक्त);
			जइ(rc != MOSQ_ERR_SUCCESS){
				mosquitto_FREE(notification_topic);
				प्रत्यागम rc;
			}
			mosquitto_FREE(notification_topic);
		}
	}

	log__printf(रिक्त, MOSQ_LOG_NOTICE, "Connecting bridge %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__socket_connect(context,
			context->bridge->addresses[context->bridge->cur_address].address,
			context->bridge->addresses[context->bridge->cur_address].port,
			context->bridge->bind_address,
			false);

	जइ(rc > 0){
		जइ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			प्रत्यागम rc; /* Error already printed */
		}अण्णहा जइ(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}अण्णहा जइ(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		प्रत्यागम rc;
	}अण्णहा जइ(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, परिमाण(context->sock), context);

	जइ(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	जइ(bridge__set_tcp_user_timeout(context)){
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}
#endif

	जइ(context->bridge->receive_maximum != 0){
		receive_maximum.मानम्.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	जइ(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.मानम्.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	जइ(context->bridge->max_topic_alias != 0){
		topic_alias_max.मानम्.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc2 = send__connect(context, context->keepalive, context->clean_start, properties);
	जइ(rc2 == MOSQ_ERR_SUCCESS){
		प्रत्यागम rc;
	}अण्णहा जइ(rc2 == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अण्णहा{
		जइ(rc2 == MOSQ_ERR_TLS){
			प्रत्यागम rc2; /* Error already printed */
		}अण्णहा जइ(rc2 == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}अण्णहा जइ(rc2 == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		प्रत्यागम rc2;
	}
}
#endif


पूर्णाङ्क bridge__on_connect(संरचना mosquitto *context)
{
	वर्ण *notification_topic;
	size_t notification_topic_len;
	संरचना mosquitto__bridge_topic *cur_topic;
	पूर्णाङ्क sub_opts;
	bool retain = true;
	uint8_t qos;

	जइ(context->bridge->notifications){
		जइ(context->max_qos == 0){
			qos = 0;
		}अण्णहा{
			qos = 1;
		}
		जइ(!context->retain_available){
			retain = false;
		}
		वर्ण notification_payload = '1';
		जइ(context->bridge->notification_topic){
			जइ(!context->bridge->notifications_local_only){
				जइ(send__real_publish(context, mosquitto__mid_generate(context),
						context->bridge->notification_topic, 1, &notification_payload, qos, retain, 0, 0, रिक्त, 0)){

					प्रत्यागम 1;
				}
			}
			db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, सन्देशः_EXPIRY_INFINITE, रिक्त);
		}अण्णहा{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(परिमाण(वर्ण)*(notification_topic_len+1));
			जइ(!notification_topic){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);
			notification_payload = '1';
			जइ(!context->bridge->notifications_local_only){
				जइ(send__real_publish(context, mosquitto__mid_generate(context),
						notification_topic, 1, &notification_payload, qos, retain, 0, 0, रिक्त, 0)){

					mosquitto_FREE(notification_topic);
					प्रत्यागम 1;
				}
			}
			db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, सन्देशः_EXPIRY_INFINITE, रिक्त);
			mosquitto_FREE(notification_topic);
		}
	}

	LL_FOREACH(context->bridge->topics, cur_topic){
		जइ(cur_topic->direction == bd_in || cur_topic->direction == bd_both){
			जइ(cur_topic->qos > context->max_qos){
				sub_opts = context->max_qos;
			}अण्णहा{
				sub_opts = cur_topic->qos;
			}
			जइ(context->bridge->protocol_version == mosq_p_mqtt5){
				sub_opts = sub_opts
						| MQTT_SUB_OPT_NO_LOCAL
						| MQTT_SUB_OPT_RETAIN_AS_PUBLISHED
						| MQTT_SUB_OPT_SEND_RETAIN_ALWAYS;
			}
			जइ(send__subscribe(context, रिक्त, 1, &cur_topic->remote_topic, sub_opts, रिक्त)){
				प्रत्यागम 1;
			}
		}अण्णहा{
			जइ(context->bridge->attempt_unsubscribe){
				जइ(send__unsubscribe(context, रिक्त, 1, &cur_topic->remote_topic, रिक्त)){
					/* direction = inwards only. This means we should not be subscribed
					* to the topic. It is possible that we used to be subscribed to
					* this topic so unsubscribe. */
					प्रत्यागम 1;
				}
			}
		}
	}
	संरचना mosquitto_subscription sub;
	memset(&sub, 0, परिमाण(sub));
	LL_FOREACH(context->bridge->topics, cur_topic){
		sub.topic_filter = cur_topic->local_topic;
		जइ(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			जइ(cur_topic->qos > context->max_qos){
				sub.options = context->max_qos;
			}अण्णहा{
				sub.options = cur_topic->qos;
			}
			retain__queue(context, &sub);
		}
	}

	context->bridge->connected_at = db.now_s;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क bridge__register_local_connections(शून्य)
{
	संरचना mosquitto *context, *ctxt_tmp = रिक्त;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		जइ(context->bridge){
			जइ(mux__new(context)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error in initial bridge registration: %s", strerror(errno));
				प्रत्यागम MOSQ_ERR_UNKNOWN;
			}
			mux__add_out(context);
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


शून्य bridge__reload(शून्य)
{
	पूर्णाङ्क i;
	पूर्णाङ्क j;

	// destroy old bridges that dissappeared
	प्रति(i=0; i<db.bridge_गणनम्; i++){
		प्रति(j=0; j<db.config->bridge_गणनम्; j++){
			जइ(!strcmp(db.bridges[i]->bridge->name, db.config->bridges[j]->name)){
				भङ्ग;
			}
		}

		जइ(j==db.config->bridge_गणनम्){
			bridge__destroy(db.bridges[i]);
		}
	}

	प्रति(i=0; i<db.config->bridge_गणनम्; i++){
		प्रति(j=0; j<db.bridge_गणनम्; j++){
			जइ(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
				भङ्ग;
			}
		}

		जइ(j==db.bridge_गणनम्){
			// a new bridge was found, create it
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = रिक्त;
			अनुवर्त;
		}

		जइ(db.config->bridges[i]->reload_type == brt_immediate){
			// in this case, an existing bridge should match
			प्रति(j=0; j<db.bridge_गणनम्; j++){
				जइ(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
					भङ्ग;
				}
			}

			प्रतिज्ञा(j<db.bridge_गणनम्);
			db.bridges[j]->will_delay_interval = 0;
			bridge__destroy(db.bridges[j]);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = रिक्त;
		}
	}
}


शून्य bridge__db_cleanup(शून्य)
{
	पूर्णाङ्क i;

	प्रति(i=0; i<db.bridge_गणनम्; i++){
		जइ(db.bridges[i]){
			context__cleanup(db.bridges[i], true);
		}
	}
	mosquitto_FREE(db.bridges);
}


शून्य bridge__cleanup(संरचना mosquitto *context)
{
	पूर्णाङ्क i;

	प्रतिज्ञा(db.bridge_गणनम् > 0);

	प्रति(i=0; i<db.bridge_गणनम्; i++){
		जइ(db.bridges[i] == context){
			db.bridges[i] = db.bridges[db.bridge_गणनम्-1];
			भङ्ग;
		}
	}

	db.bridge_गणनम्--;
	जइ(db.bridge_गणनम् == 0){
		mosquitto_FREE(db.bridges);
	}अण्णहा{
		db.bridges = mosquitto_realloc(db.bridges, (unsigned)db.bridge_गणनम् * परिमाण(db.bridges[0]));
	}

	mosquitto_FREE(context->bridge->name);
	mosquitto_FREE(context->bridge->local_clientid);
	mosquitto_FREE(context->bridge->local_username);
	mosquitto_FREE(context->bridge->local_password);
#ifdef WITH_TLS
	mosquitto_FREE(context->bridge->tls_certfile);
	mosquitto_FREE(context->bridge->tls_keyfile);
#endif

	जइ(context->bridge->remote_clientid != context->id){
		mosquitto_FREE(context->bridge->remote_clientid);
	}
	context->bridge->remote_clientid = रिक्त;

	जइ(context->bridge->remote_username != context->username){
		mosquitto_FREE(context->bridge->remote_username);
	}
	context->bridge->remote_username = रिक्त;

	जइ(context->bridge->remote_password != context->password){
		mosquitto_FREE(context->bridge->remote_password);
	}
	context->bridge->remote_password = रिक्त;
#ifdef WITH_TLS
	जइ(context->ssl_ctx){
		SSL_CTX_free(context->ssl_ctx);
		context->ssl_ctx = रिक्त;
	}
#endif

	प्रति(i=0; i<context->bridge->address_गणनम्; i++){
		mosquitto_FREE(context->bridge->addresses[i].address);
	}

	mosquitto_FREE(context->bridge->addresses);

	config__bridge_cleanup(context->bridge);
	context->bridge = रिक्त;
}


स्थिर शून्य bridge__packet_cleanup(संरचना mosquitto *context)
{
	संरचना mosquitto__packet *packet;
	जइ(!context){
		प्रत्यागम;
	}

	जाव(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_FREE(packet);
	}
	context->out_packet = रिक्त;
	context->out_packet_last = रिक्त;
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_गणनम्);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_गणनम् = 0;
	context->out_packet_bytes = 0;

	packet__cleanup(&(context->in_packet));
}


स्थिर पूर्णाङ्क rand_between(पूर्णाङ्क नीचः, पूर्णाङ्क उच्चः)
{
	पूर्णाङ्क r;
	mosquitto_getrandom(&r, परिमाण(पूर्णाङ्क));
	प्रत्यागम (abs(r) % (उच्चः - नीचः)) + नीचः;
}


स्थिर शून्य bridge__backoff_step(संरचना mosquitto__bridge *bridge)
{
	/*
		“Decorrelated Jitter” calculation, according to:
			https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
	*/

	bridge->restart_timeout = rand_between(bridge->backoff_base, bridge->restart_timeout * 3);
	जइ(bridge->restart_timeout > bridge->backoff_cap){
		bridge->restart_timeout = bridge->backoff_cap;
	}
}


स्थिर शून्य bridge__backoff_reset(संरचना mosquitto__bridge *bridge)
{
	bridge->restart_timeout = bridge->backoff_base;
}


स्थिर शून्य bridge__update_backoff(संरचना mosquitto__bridge *bridge)
{
	जइ(!bridge){
		प्रत्यागम;
	}
	जइ(!bridge->backoff_cap){
		/* skip if not using jitter */
		प्रत्यागम;

	}
	जइ(bridge->connected_at && db.now_s - bridge->connected_at >= bridge->stable_connection_period){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Bridge %s connection was stable enough, resetting backoff", bridge->name);
		bridge__backoff_reset(bridge);
	}अण्णहा{
		bridge__backoff_step(bridge);
	}

	bridge->connected_at = 0;

	log__printf(रिक्त, MOSQ_LOG_INFO, "Bridge %s next backoff will be %d ms", bridge->name, bridge->restart_timeout);
}


स्थिर शून्य bridge_check_pending(संरचना mosquitto *context)
{
	पूर्णाङ्क err;
	socklen_t len;

	जइ(context->state == mosq_cs_connect_pending){
		len = परिमाण(पूर्णाङ्क);
		जइ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (वर्ण *)&err, &len)){
			जइ(err == 0){
				mosquitto__set_state(context, mosq_cs_new);
#जइ defined(WITH_ADNS) && defined(WITH_BRIDGE)
				जइ(context->bridge){
					bridge__connect_step3(context);
				}
#endif
			}अण्णहा जइ(err == ECONNREFUSED){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				प्रत्यागम;
			}
		}अण्णहा{
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			प्रत्यागम;
		}
	}
}


स्थिर bool reload_if_needed(संरचना mosquitto *context)
{
	पूर्णाङ्क i;

	प्रति(i=0; i<db.config->bridge_गणनम्; i++){
		जइ(db.config->bridges[i] && !strcmp(context->bridge->name, db.config->bridges[i]->name)){
			bridge__destroy(context);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = रिक्त;
			loop__update_next_event(100);
			प्रत्यागम true;
		}
	}

	प्रत्यागम false;
}


शून्य bridge_check(शून्य)
{
	स्थिर time_t last_check = 0;
	संरचना mosquitto *context = रिक्त;
	socklen_t len;
	पूर्णाङ्क i;
	पूर्णाङ्क rc;
	पूर्णाङ्क err;

	जइ(db.now_s <= last_check){
		प्रत्यागम;
	}

	प्रति(i=0; i<db.bridge_गणनम्; i++){
		जइ(!db.bridges[i]){
			अनुवर्त;
		}

		context = db.bridges[i];

		जइ(net__is_connected(context)){
			mosquitto__check_keepalive(context);
			bridge_check_pending(context);

			/* Check for bridges that are not round robin and not currently
			 * connected to their primary broker. */
			जइ(context->bridge->round_robin == false
					&& context->bridge->cur_address != 0
					&& context->bridge->primary_retry
					&& db.now_s >= context->bridge->primary_retry){

				जइ(context->bridge->primary_retry_sock == INVALID_SOCKET){
					rc = net__try_connect(context->bridge->addresses[0].address,
							context->bridge->addresses[0].port,
							&context->bridge->primary_retry_sock,
							context->bridge->bind_address, false);

					जइ(rc == 0){
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = 0;
						mux__delete(context);
						net__socket_close(context);
						context->bridge->cur_address = 0;
					}
				}अण्णहा{
					len = परिमाण(पूर्णाङ्क);
					जइ(!getsockopt(context->bridge->primary_retry_sock, SOL_SOCKET, SO_ERROR, (वर्ण *)&err, &len)){
						जइ(err == 0){
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = 0;
							mux__delete(context);
							net__socket_close(context);
							context->bridge->cur_address = context->bridge->address_गणनम्-1;
						}अण्णहा{
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = db.now_s+5;
							loop__update_next_event(5000);
						}
					}अण्णहा{
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = db.now_s+5;
						loop__update_next_event(5000);
					}
				}
			}
		}अण्णहा{
			जइ(reload_if_needed(context)){
				अनुवर्त;
			}

			/* Want to try to restart the bridge connection */
			जइ(!context->bridge->restart_t){
				bridge__update_backoff(context->bridge);
				context->bridge->restart_t = 1000*db.now_s+context->bridge->restart_timeout;
				context->bridge->cur_address++;
				जइ(context->bridge->cur_address == context->bridge->address_गणनम्){
					context->bridge->cur_address = 0;
				}
				loop__update_next_event(context->bridge->restart_timeout);
			}अण्णहा{
				जइ((context->bridge->start_type == bst_lazy && context->bridge->lazy_reconnect)
						|| (context->bridge->start_type == bst_automatic && 1000*db.now_s >= context->bridge->restart_t)){

#जइ defined(__GLIBC__) && defined(WITH_ADNS)
					जइ(context->adns){
						/* Connection attempted, waiting on DNS lookup */
						rc = gai_error(context->adns);
						जइ(rc == EAI_INPROGRESS){
							/* Just keep on waiting */
						}अण्णहा जइ(rc == 0){
							rc = bridge__connect_step2(context);
							जइ(rc == MOSQ_ERR_SUCCESS){
								mux__new(context);
								जइ(context->out_packet){
									mux__add_out(context);
								}
							}अण्णहा जइ(rc == MOSQ_ERR_CONN_PENDING){
								mux__new(context);
								mux__add_out(context);
								context->bridge->restart_t = 0;
							}अण्णहा{
								context->bridge->cur_address++;
								जइ(context->bridge->cur_address == context->bridge->address_गणनम्){
									context->bridge->cur_address = 0;
								}
								context->bridge->restart_t = 0;
							}
						}अण्णहा{
							/* Need to retry */
							जइ(context->adns->ar_result){
								freeaddrinfo(context->adns->ar_result);
							}
							mosquitto_FREE(context->adns);
							context->bridge->restart_t = 0;
						}
					}अण्णहा{
						rc = bridge__connect_step1(context);
						जइ(rc){
							context->bridge->cur_address++;
							जइ(context->bridge->cur_address == context->bridge->address_गणनम्){
								context->bridge->cur_address = 0;
							}
						}अण्णहा{
							/* Short wait for ADNS lookup */
							context->bridge->restart_t = 1;
							loop__update_next_event(1000);
						}
					}
#अण्णहा
					{
						rc = bridge__connect(context);
						context->bridge->restart_t = 0;
						जइ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING){
							जइ(context->bridge->round_robin == false && context->bridge->cur_address != 0){
								context->bridge->primary_retry = db.now_s + 5;
								loop__update_next_event(5000);
							}
							mux__new(context);
							जइ(context->out_packet){
								mux__add_out(context);
							}
						}अण्णहा{
							context->bridge->cur_address++;
							जइ(context->bridge->cur_address == context->bridge->address_गणनम्){
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
