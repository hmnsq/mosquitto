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
#अन्यथा
#अन्तर्भाव <winsock2.h>
#अन्तर्भाव <ws2tcpip.h>
#endif

#ifndef WIN32
#अन्तर्भाव <unistd.h>
#अन्यथा
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
#यदि defined(__GLIBC__) && defined(WITH_ADNS)
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
	यदि(!local_id){
		फर्काऊ रिक्त;
	}

	HASH_FIND(hh_id, db.contexts_by_id, local_id, म_माप(local_id), new_context);
	यदि(new_context){
		/* (possible from persistent db) */
		mosquitto_FREE(local_id);
	}अन्यथा{
		/* id wasn't found, so generate a new context */
		new_context = context__init();
		यदि(!new_context){
			mosquitto_FREE(local_id);
			फर्काऊ रिक्त;
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
	यदि(bridge->clean_start_local == -1){
		/* default to "regular" clean start setting */
		bridge->clean_start_local = bridge->clean_start;
	}
	new_context->retain_available = bridge->outgoing_retain;
	new_context->protocol = bridge->protocol_version;
	यदि(!bridge->clean_start_local){
		new_context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
		plugin_persist__handle_client_add(new_context);
		यदि(new_context->expiry_list_item){
			/* We've restored from persistence and been added to the session
			 * expiry list, even though we should never be expired */
			session_expiry__remove(new_context);
		}
	}

	bridges = mosquitto_realloc(db.bridges, (माप_प्रकार)(db.bridge_गन्ती+1)*परिमाण(संरचना mosquitto *));
	यदि(bridges){
		db.bridges = bridges;
		db.bridge_गन्ती++;
		db.bridges[db.bridge_गन्ती-1] = new_context;
	}अन्यथा{
		फर्काऊ रिक्त;
	}

	फर्काऊ new_context;
}


स्थिर शून्य bridge__destroy(संरचना mosquitto *context)
{
	send__disconnect(context, MQTT_RC_SUCCESS, रिक्त);
	context__cleanup(context, true);
}


शून्य bridge__start_all(शून्य)
{
	लागि(पूर्णाङ्क i=0; i<db.config->bridge_गन्ती; i++){
		संरचना mosquitto *context;
		पूर्णाङ्क ret;

		context = bridge__new(db.config->bridges[i]);
		यदि(!context){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
			फर्काऊ;
		}
		प्रतिज्ञा(context);

#यदि defined(__GLIBC__) && defined(WITH_ADNS)
		context->bridge->restart_t = 1; /* force quick restart of bridge */
		loop__update_next_event(1000);
		ret = bridge__connect_step1(context);
#अन्यथा
		ret = bridge__connect(context);
#endif

		यदि(ret && ret != MOSQ_ERR_CONN_PENDING){
			log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Unable to connect bridge %s.",
					context->bridge->name);
		}

		db.config->bridges[i] = रिक्त;
	}
}


स्थिर पूर्णाङ्क bridge__set_tcp_keepalive(संरचना mosquitto *context)
{
	अचिन्हित पूर्णाङ्क enabled = 1;
	bool ret;

	यदि(context->bridge->tcp_keepalive_idle == 0
			|| context->bridge->tcp_keepalive_interval == 0
			|| context->bridge->tcp_keepalive_counter == 0){

		फर्काऊ MOSQ_ERR_SUCCESS;
	}

#ifdef WIN32
#  define SETSOCKOPT_TYPE वर्ण *
#अन्यथा
#  define SETSOCKOPT_TYPE अचर शून्य *
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

	यदि(ret){
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TCP_USER_TIMEOUT


स्थिर पूर्णाङ्क bridge__set_tcp_user_timeout(संरचना mosquitto *context)
{
	पूर्णाङ्क timeout = context->bridge->tcp_user_timeout;
	यदि(timeout >= 0){
		यदि(setsockopt(context->sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (वर्ण *)&timeout, परिमाण(timeout))){
			फर्काऊ MOSQ_ERR_UNKNOWN;
		}
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}
#endif

#यदि defined(__GLIBC__) && defined(WITH_ADNS)


स्थिर पूर्णाङ्क bridge__connect_step1(संरचना mosquitto *context)
{
	पूर्णाङ्क rc;
	वर्ण *notification_topic;
	माप_प्रकार notification_topic_len;
	uint8_t notification_payload;
	संरचना mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	यदि(!context || !context->bridge){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_सन्देश_in = db.now_s;
	context->next_सन्देश_out = db.now_s + context->bridge->keepalive;
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
		यदि(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(रिक्त, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			यदि(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}अन्यथा{
				qos = cur_topic->qos;
			}
			संरचना mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			यदि(sub__add(context, &sub) > 0){
				फर्काऊ 1;
			}
			retain__queue(context, &sub);
		}
	}

	यदि(context->bridge->notifications){
		यदि(context->max_qos == 0){
			qos = 0;
		}अन्यथा{
			qos = 1;
		}
		यदि(context->bridge->notification_topic){
			यदि(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, सन्देश_EXPIRY_INFINITE, रिक्त);
				context->bridge->initial_notification_done = true;
			}
			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, रिक्त);
			यदि(rc != MOSQ_ERR_SUCCESS){
				फर्काऊ rc;
			}
		}अन्यथा{
			notification_topic_len = म_माप(context->bridge->remote_clientid)+म_माप("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(परिमाण(वर्ण)*(notification_topic_len+1));
			यदि(!notification_topic){
				फर्काऊ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			यदि(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, सन्देश_EXPIRY_INFINITE, रिक्त);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, रिक्त);
			mosquitto_FREE(notification_topic);
			यदि(rc != MOSQ_ERR_SUCCESS){
				फर्काऊ rc;
			}
		}
	}

	log__printf(रिक्त, MOSQ_LOG_NOTICE, "Connecting bridge (step 1) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step1(context, context->bridge->addresses[context->bridge->cur_address].address);
	यदि(rc > 0){
		यदि(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			फर्काऊ rc; /* Error already printed */
		}अन्यथा यदि(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", म_त्रुटि(त्रुटि_सं));
		}अन्यथा यदि(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(त्रुटि_सं));
		}

		फर्काऊ rc;
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क bridge__connect_step2(संरचना mosquitto *context)
{
	पूर्णाङ्क rc;

	यदि(!context || !context->bridge){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	log__printf(रिक्त, MOSQ_LOG_NOTICE, "Connecting bridge (step 2) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step2(context, context->bridge->addresses[context->bridge->cur_address].port, &context->sock);
	यदि(rc > 0){
		यदि(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			फर्काऊ rc; /* Error already printed */
		}अन्यथा यदि(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", म_त्रुटि(त्रुटि_सं));
		}अन्यथा यदि(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(त्रुटि_सं));
		}

		फर्काऊ rc;
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, परिमाण(context->sock), context);

	यदि(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}
	फर्काऊ rc;
}


पूर्णाङ्क bridge__connect_step3(संरचना mosquitto *context)
{
	पूर्णाङ्क rc;
	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = रिक्त;

	rc = net__socket_connect_step3(context, context->bridge->addresses[context->bridge->cur_address].address);
	यदि(rc > 0){
		यदि(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			फर्काऊ rc; /* Error already printed */
		}अन्यथा यदि(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", म_त्रुटि(त्रुटि_सं));
		}अन्यथा यदि(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(त्रुटि_सं));
		}

		फर्काऊ rc;
	}

	यदि(context->bridge->round_robin == false && context->bridge->cur_address != 0){
		context->bridge->primary_retry = db.now_s + 5;
		loop__update_next_event(5000);
	}

	यदि(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	यदि(bridge__set_tcp_user_timeout(context)){
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}
#endif

	यदि(context->bridge->receive_maximum != 0){
		receive_maximum.मान.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	यदि(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.मान.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	यदि(context->bridge->max_topic_alias != 0){
		topic_alias_max.मान.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc = send__connect(context, context->keepalive, context->clean_start, properties);
	यदि(rc == MOSQ_ERR_SUCCESS){
		फर्काऊ MOSQ_ERR_SUCCESS;
	}अन्यथा यदि(rc == MOSQ_ERR_ERRNO && त्रुटि_सं == ENOTCONN){
		फर्काऊ MOSQ_ERR_SUCCESS;
	}अन्यथा{
		यदि(rc == MOSQ_ERR_TLS){
			फर्काऊ rc; /* Error already printed */
		}अन्यथा यदि(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", म_त्रुटि(त्रुटि_सं));
		}अन्यथा यदि(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(त्रुटि_सं));
		}
		mux__delete(context);
		net__socket_close(context);
		फर्काऊ rc;
	}
}
#अन्यथा


पूर्णाङ्क bridge__connect(संरचना mosquitto *context)
{
	पूर्णाङ्क rc, rc2;
	वर्ण *notification_topic = रिक्त;
	माप_प्रकार notification_topic_len;
	uint8_t notification_payload;
	संरचना mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = रिक्त;

	यदि(!context || !context->bridge){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->last_सन्देश_in = db.now_s;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->next_सन्देश_out = db.now_s + context->bridge->keepalive;
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
		यदि(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(रिक्त, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			यदि(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}अन्यथा{
				qos = cur_topic->qos;
			}
			संरचना mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			यदि(sub__add(context, &sub) > 0){
				फर्काऊ 1;
			}
		}
	}

	यदि(context->bridge->notifications){
		यदि(context->max_qos == 0){
			qos = 0;
		}अन्यथा{
			qos = 1;
		}
		यदि(context->bridge->notification_topic){
			यदि(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, सन्देश_EXPIRY_INFINITE, रिक्त);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, रिक्त);
			यदि(rc != MOSQ_ERR_SUCCESS){
				फर्काऊ rc;
			}
		}अन्यथा{
			notification_topic_len = म_माप(context->bridge->remote_clientid)+म_माप("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(परिमाण(वर्ण)*(notification_topic_len+1));
			यदि(!notification_topic){
				फर्काऊ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			यदि(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, सन्देश_EXPIRY_INFINITE, रिक्त);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, रिक्त);
			यदि(rc != MOSQ_ERR_SUCCESS){
				mosquitto_FREE(notification_topic);
				फर्काऊ rc;
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

	यदि(rc > 0){
		यदि(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			फर्काऊ rc; /* Error already printed */
		}अन्यथा यदि(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", म_त्रुटि(त्रुटि_सं));
		}अन्यथा यदि(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(त्रुटि_सं));
		}

		फर्काऊ rc;
	}अन्यथा यदि(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, परिमाण(context->sock), context);

	यदि(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	यदि(bridge__set_tcp_user_timeout(context)){
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}
#endif

	यदि(context->bridge->receive_maximum != 0){
		receive_maximum.मान.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	यदि(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.मान.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	यदि(context->bridge->max_topic_alias != 0){
		topic_alias_max.मान.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc2 = send__connect(context, context->keepalive, context->clean_start, properties);
	यदि(rc2 == MOSQ_ERR_SUCCESS){
		फर्काऊ rc;
	}अन्यथा यदि(rc2 == MOSQ_ERR_ERRNO && त्रुटि_सं == ENOTCONN){
		फर्काऊ MOSQ_ERR_SUCCESS;
	}अन्यथा{
		यदि(rc2 == MOSQ_ERR_TLS){
			फर्काऊ rc2; /* Error already printed */
		}अन्यथा यदि(rc2 == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", म_त्रुटि(त्रुटि_सं));
		}अन्यथा यदि(rc2 == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(त्रुटि_सं));
		}
		mux__delete(context);
		net__socket_close(context);
		फर्काऊ rc2;
	}
}
#endif


पूर्णाङ्क bridge__on_connect(संरचना mosquitto *context)
{
	वर्ण *notification_topic;
	माप_प्रकार notification_topic_len;
	संरचना mosquitto__bridge_topic *cur_topic;
	पूर्णाङ्क sub_opts;
	bool retain = true;
	uint8_t qos;

	यदि(context->bridge->notifications){
		यदि(context->max_qos == 0){
			qos = 0;
		}अन्यथा{
			qos = 1;
		}
		यदि(!context->retain_available){
			retain = false;
		}
		वर्ण notification_payload = '1';
		यदि(context->bridge->notification_topic){
			यदि(!context->bridge->notifications_local_only){
				यदि(send__real_publish(context, mosquitto__mid_generate(context),
						context->bridge->notification_topic, 1, &notification_payload, qos, retain, 0, 0, रिक्त, 0)){

					फर्काऊ 1;
				}
			}
			db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, सन्देश_EXPIRY_INFINITE, रिक्त);
		}अन्यथा{
			notification_topic_len = म_माप(context->bridge->remote_clientid)+म_माप("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(परिमाण(वर्ण)*(notification_topic_len+1));
			यदि(!notification_topic){
				फर्काऊ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);
			notification_payload = '1';
			यदि(!context->bridge->notifications_local_only){
				यदि(send__real_publish(context, mosquitto__mid_generate(context),
						notification_topic, 1, &notification_payload, qos, retain, 0, 0, रिक्त, 0)){

					mosquitto_FREE(notification_topic);
					फर्काऊ 1;
				}
			}
			db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, सन्देश_EXPIRY_INFINITE, रिक्त);
			mosquitto_FREE(notification_topic);
		}
	}

	LL_FOREACH(context->bridge->topics, cur_topic){
		यदि(cur_topic->direction == bd_in || cur_topic->direction == bd_both){
			यदि(cur_topic->qos > context->max_qos){
				sub_opts = context->max_qos;
			}अन्यथा{
				sub_opts = cur_topic->qos;
			}
			यदि(context->bridge->protocol_version == mosq_p_mqtt5){
				sub_opts = sub_opts
						| MQTT_SUB_OPT_NO_LOCAL
						| MQTT_SUB_OPT_RETAIN_AS_PUBLISHED
						| MQTT_SUB_OPT_SEND_RETAIN_ALWAYS;
			}
			यदि(send__subscribe(context, रिक्त, 1, &cur_topic->remote_topic, sub_opts, रिक्त)){
				फर्काऊ 1;
			}
		}अन्यथा{
			यदि(context->bridge->attempt_unsubscribe){
				यदि(send__unsubscribe(context, रिक्त, 1, &cur_topic->remote_topic, रिक्त)){
					/* direction = inwards only. This means we should not be subscribed
					* to the topic. It is possible that we used to be subscribed to
					* this topic so unsubscribe. */
					फर्काऊ 1;
				}
			}
		}
	}
	संरचना mosquitto_subscription sub;
	स_रखो(&sub, 0, परिमाण(sub));
	LL_FOREACH(context->bridge->topics, cur_topic){
		sub.topic_filter = cur_topic->local_topic;
		यदि(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			यदि(cur_topic->qos > context->max_qos){
				sub.options = context->max_qos;
			}अन्यथा{
				sub.options = cur_topic->qos;
			}
			retain__queue(context, &sub);
		}
	}

	context->bridge->connected_at = db.now_s;

	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क bridge__register_local_connections(शून्य)
{
	संरचना mosquitto *context, *ctxt_tmp = रिक्त;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		यदि(context->bridge){
			यदि(mux__new(context)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error in initial bridge registration: %s", म_त्रुटि(त्रुटि_सं));
				फर्काऊ MOSQ_ERR_UNKNOWN;
			}
			mux__add_out(context);
		}
	}
	फर्काऊ MOSQ_ERR_SUCCESS;
}


शून्य bridge__reload(शून्य)
{
	पूर्णाङ्क i;
	पूर्णाङ्क j;

	// destroy old bridges that dissappeared
	लागि(i=0; i<db.bridge_गन्ती; i++){
		लागि(j=0; j<db.config->bridge_गन्ती; j++){
			यदि(!म_भेद(db.bridges[i]->bridge->name, db.config->bridges[j]->name)){
				रोक;
			}
		}

		यदि(j==db.config->bridge_गन्ती){
			bridge__destroy(db.bridges[i]);
		}
	}

	लागि(i=0; i<db.config->bridge_गन्ती; i++){
		लागि(j=0; j<db.bridge_गन्ती; j++){
			यदि(!म_भेद(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
				रोक;
			}
		}

		यदि(j==db.bridge_गन्ती){
			// a new bridge was found, create it
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = रिक्त;
			जारी;
		}

		यदि(db.config->bridges[i]->reload_type == brt_immediate){
			// in this case, an existing bridge should match
			लागि(j=0; j<db.bridge_गन्ती; j++){
				यदि(!म_भेद(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
					रोक;
				}
			}

			प्रतिज्ञा(j<db.bridge_गन्ती);
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

	लागि(i=0; i<db.bridge_गन्ती; i++){
		यदि(db.bridges[i]){
			context__cleanup(db.bridges[i], true);
		}
	}
	mosquitto_FREE(db.bridges);
}


शून्य bridge__cleanup(संरचना mosquitto *context)
{
	पूर्णाङ्क i;

	प्रतिज्ञा(db.bridge_गन्ती > 0);

	लागि(i=0; i<db.bridge_गन्ती; i++){
		यदि(db.bridges[i] == context){
			db.bridges[i] = db.bridges[db.bridge_गन्ती-1];
			रोक;
		}
	}

	db.bridge_गन्ती--;
	यदि(db.bridge_गन्ती == 0){
		mosquitto_FREE(db.bridges);
	}अन्यथा{
		db.bridges = mosquitto_realloc(db.bridges, (अचिन्हित)db.bridge_गन्ती * परिमाण(db.bridges[0]));
	}

	mosquitto_FREE(context->bridge->name);
	mosquitto_FREE(context->bridge->local_clientid);
	mosquitto_FREE(context->bridge->local_username);
	mosquitto_FREE(context->bridge->local_password);
#ifdef WITH_TLS
	mosquitto_FREE(context->bridge->tls_certfile);
	mosquitto_FREE(context->bridge->tls_keyfile);
#endif

	यदि(context->bridge->remote_clientid != context->id){
		mosquitto_FREE(context->bridge->remote_clientid);
	}
	context->bridge->remote_clientid = रिक्त;

	यदि(context->bridge->remote_username != context->username){
		mosquitto_FREE(context->bridge->remote_username);
	}
	context->bridge->remote_username = रिक्त;

	यदि(context->bridge->remote_password != context->password){
		mosquitto_FREE(context->bridge->remote_password);
	}
	context->bridge->remote_password = रिक्त;
#ifdef WITH_TLS
	यदि(context->ssl_ctx){
		SSL_CTX_free(context->ssl_ctx);
		context->ssl_ctx = रिक्त;
	}
#endif

	लागि(i=0; i<context->bridge->address_गन्ती; i++){
		mosquitto_FREE(context->bridge->addresses[i].address);
	}

	mosquitto_FREE(context->bridge->addresses);

	config__bridge_cleanup(context->bridge);
	context->bridge = रिक्त;
}


स्थिर शून्य bridge__packet_cleanup(संरचना mosquitto *context)
{
	संरचना mosquitto__packet *packet;
	यदि(!context){
		फर्काऊ;
	}

	जबसम्म(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_FREE(packet);
	}
	context->out_packet = रिक्त;
	context->out_packet_last = रिक्त;
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_गन्ती);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_गन्ती = 0;
	context->out_packet_bytes = 0;

	packet__cleanup(&(context->in_packet));
}


स्थिर पूर्णाङ्क rand_between(पूर्णाङ्क तल्लो, पूर्णाङ्क माथिल्लो)
{
	पूर्णाङ्क r;
	mosquitto_getrandom(&r, परिमाण(पूर्णाङ्क));
	फर्काऊ (असल(r) % (माथिल्लो - तल्लो)) + तल्लो;
}


स्थिर शून्य bridge__backoff_step(संरचना mosquitto__bridge *bridge)
{
	/*
		“Decorrelated Jitter” calculation, according to:
			https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
	*/

	bridge->restart_timeout = rand_between(bridge->backoff_base, bridge->restart_timeout * 3);
	यदि(bridge->restart_timeout > bridge->backoff_cap){
		bridge->restart_timeout = bridge->backoff_cap;
	}
}


स्थिर शून्य bridge__backoff_reset(संरचना mosquitto__bridge *bridge)
{
	bridge->restart_timeout = bridge->backoff_base;
}


स्थिर शून्य bridge__update_backoff(संरचना mosquitto__bridge *bridge)
{
	यदि(!bridge){
		फर्काऊ;
	}
	यदि(!bridge->backoff_cap){
		/* skip if not using jitter */
		फर्काऊ;

	}
	यदि(bridge->connected_at && db.now_s - bridge->connected_at >= bridge->stable_connection_period){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Bridge %s connection was stable enough, resetting backoff", bridge->name);
		bridge__backoff_reset(bridge);
	}अन्यथा{
		bridge__backoff_step(bridge);
	}

	bridge->connected_at = 0;

	log__printf(रिक्त, MOSQ_LOG_INFO, "Bridge %s next backoff will be %d ms", bridge->name, bridge->restart_timeout);
}


स्थिर शून्य bridge_check_pending(संरचना mosquitto *context)
{
	पूर्णाङ्क err;
	socklen_t len;

	यदि(context->state == mosq_cs_connect_pending){
		len = परिमाण(पूर्णाङ्क);
		यदि(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (वर्ण *)&err, &len)){
			यदि(err == 0){
				mosquitto__set_state(context, mosq_cs_new);
#यदि defined(WITH_ADNS) && defined(WITH_BRIDGE)
				यदि(context->bridge){
					bridge__connect_step3(context);
				}
#endif
			}अन्यथा यदि(err == ECONNREFUSED){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				फर्काऊ;
			}
		}अन्यथा{
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			फर्काऊ;
		}
	}
}


स्थिर bool reload_if_needed(संरचना mosquitto *context)
{
	पूर्णाङ्क i;

	लागि(i=0; i<db.config->bridge_गन्ती; i++){
		यदि(db.config->bridges[i] && !म_भेद(context->bridge->name, db.config->bridges[i]->name)){
			bridge__destroy(context);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = रिक्त;
			loop__update_next_event(100);
			फर्काऊ true;
		}
	}

	फर्काऊ false;
}


शून्य bridge_check(शून्य)
{
	स्थिर समय_प्रकार last_check = 0;
	संरचना mosquitto *context = रिक्त;
	socklen_t len;
	पूर्णाङ्क i;
	पूर्णाङ्क rc;
	पूर्णाङ्क err;

	यदि(db.now_s <= last_check){
		फर्काऊ;
	}

	लागि(i=0; i<db.bridge_गन्ती; i++){
		यदि(!db.bridges[i]){
			जारी;
		}

		context = db.bridges[i];

		यदि(net__is_connected(context)){
			mosquitto__check_keepalive(context);
			bridge_check_pending(context);

			/* Check for bridges that are not round robin and not currently
			 * connected to their primary broker. */
			यदि(context->bridge->round_robin == false
					&& context->bridge->cur_address != 0
					&& context->bridge->primary_retry
					&& db.now_s >= context->bridge->primary_retry){

				यदि(context->bridge->primary_retry_sock == INVALID_SOCKET){
					rc = net__try_connect(context->bridge->addresses[0].address,
							context->bridge->addresses[0].port,
							&context->bridge->primary_retry_sock,
							context->bridge->bind_address, false);

					यदि(rc == 0){
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = 0;
						mux__delete(context);
						net__socket_close(context);
						context->bridge->cur_address = 0;
					}
				}अन्यथा{
					len = परिमाण(पूर्णाङ्क);
					यदि(!getsockopt(context->bridge->primary_retry_sock, SOL_SOCKET, SO_ERROR, (वर्ण *)&err, &len)){
						यदि(err == 0){
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = 0;
							mux__delete(context);
							net__socket_close(context);
							context->bridge->cur_address = context->bridge->address_गन्ती-1;
						}अन्यथा{
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = db.now_s+5;
							loop__update_next_event(5000);
						}
					}अन्यथा{
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = db.now_s+5;
						loop__update_next_event(5000);
					}
				}
			}
		}अन्यथा{
			यदि(reload_if_needed(context)){
				जारी;
			}

			/* Want to try to restart the bridge connection */
			यदि(!context->bridge->restart_t){
				bridge__update_backoff(context->bridge);
				context->bridge->restart_t = 1000*db.now_s+context->bridge->restart_timeout;
				context->bridge->cur_address++;
				यदि(context->bridge->cur_address == context->bridge->address_गन्ती){
					context->bridge->cur_address = 0;
				}
				loop__update_next_event(context->bridge->restart_timeout);
			}अन्यथा{
				यदि((context->bridge->start_type == bst_lazy && context->bridge->lazy_reconnect)
						|| (context->bridge->start_type == bst_automatic && 1000*db.now_s >= context->bridge->restart_t)){

#यदि defined(__GLIBC__) && defined(WITH_ADNS)
					यदि(context->adns){
						/* Connection attempted, waiting on DNS lookup */
						rc = gai_error(context->adns);
						यदि(rc == EAI_INPROGRESS){
							/* Just keep on waiting */
						}अन्यथा यदि(rc == 0){
							rc = bridge__connect_step2(context);
							यदि(rc == MOSQ_ERR_SUCCESS){
								mux__new(context);
								यदि(context->out_packet){
									mux__add_out(context);
								}
							}अन्यथा यदि(rc == MOSQ_ERR_CONN_PENDING){
								mux__new(context);
								mux__add_out(context);
								context->bridge->restart_t = 0;
							}अन्यथा{
								context->bridge->cur_address++;
								यदि(context->bridge->cur_address == context->bridge->address_गन्ती){
									context->bridge->cur_address = 0;
								}
								context->bridge->restart_t = 0;
							}
						}अन्यथा{
							/* Need to retry */
							यदि(context->adns->ar_result){
								freeaddrinfo(context->adns->ar_result);
							}
							mosquitto_FREE(context->adns);
							context->bridge->restart_t = 0;
						}
					}अन्यथा{
						rc = bridge__connect_step1(context);
						यदि(rc){
							context->bridge->cur_address++;
							यदि(context->bridge->cur_address == context->bridge->address_गन्ती){
								context->bridge->cur_address = 0;
							}
						}अन्यथा{
							/* Short wait for ADNS lookup */
							context->bridge->restart_t = 1;
							loop__update_next_event(1000);
						}
					}
#अन्यथा
					{
						rc = bridge__connect(context);
						context->bridge->restart_t = 0;
						यदि(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING){
							यदि(context->bridge->round_robin == false && context->bridge->cur_address != 0){
								context->bridge->primary_retry = db.now_s + 5;
								loop__update_next_event(5000);
							}
							mux__new(context);
							यदि(context->out_packet){
								mux__add_out(context);
							}
						}अन्यथा{
							context->bridge->cur_address++;
							यदि(context->bridge->cur_address == context->bridge->address_गन्ती){
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
