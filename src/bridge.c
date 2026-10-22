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
#जर defined(__GLIBC__) && defined(WITH_ADNS)
स्थिर पूर्णांक bridge__connect_step1(संरचना mosquitto *context);
स्थिर पूर्णांक bridge__connect_step2(संरचना mosquitto *context);
#endif
स्थिर शून्य bridge__packet_cleanup(संरचना mosquitto *context);

स्थिर संरचना mosquitto *bridge__new(संरचना mosquitto__bridge *bridge)
{
	संरचना mosquitto *new_context = रिक्त;
	संरचना mosquitto **bridges;
	अक्षर *local_id;

	प्रतिज्ञा(bridge);

	local_id = mosquitto_strdup(bridge->local_clientid);
	जर(!local_id){
		परत रिक्त;
	}

	HASH_FIND(hh_id, db.contexts_by_id, local_id, म_माप(local_id), new_context);
	जर(new_context){
		/* (possible from persistent db) */
		mosquitto_FREE(local_id);
	}अन्यथा{
		/* id wasn't found, so generate a new context */
		new_context = context__init();
		जर(!new_context){
			mosquitto_FREE(local_id);
			परत रिक्त;
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
	जर(bridge->clean_start_local == -1){
		/* default to "regular" clean start setting */
		bridge->clean_start_local = bridge->clean_start;
	}
	new_context->retain_available = bridge->outgoing_retain;
	new_context->protocol = bridge->protocol_version;
	जर(!bridge->clean_start_local){
		new_context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
		plugin_persist__handle_client_add(new_context);
		जर(new_context->expiry_list_item){
			/* We've restored from persistence and been added to the session
			 * expiry list, even though we should never be expired */
			session_expiry__remove(new_context);
		}
	}

	bridges = mosquitto_realloc(db.bridges, (माप_प्रकार)(db.bridge_मोजणी+1)*आकार(संरचना mosquitto *));
	जर(bridges){
		db.bridges = bridges;
		db.bridge_मोजणी++;
		db.bridges[db.bridge_मोजणी-1] = new_context;
	}अन्यथा{
		परत रिक्त;
	}

	परत new_context;
}


स्थिर शून्य bridge__destroy(संरचना mosquitto *context)
{
	send__disconnect(context, MQTT_RC_SUCCESS, रिक्त);
	context__cleanup(context, true);
}


शून्य bridge__start_all(शून्य)
{
	साठी(पूर्णांक i=0; i<db.config->bridge_मोजणी; i++){
		संरचना mosquitto *context;
		पूर्णांक ret;

		context = bridge__new(db.config->bridges[i]);
		जर(!context){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
			परत;
		}
		प्रतिज्ञा(context);

#जर defined(__GLIBC__) && defined(WITH_ADNS)
		context->bridge->restart_t = 1; /* force quick restart of bridge */
		loop__update_next_event(1000);
		ret = bridge__connect_step1(context);
#अन्यथा
		ret = bridge__connect(context);
#endif

		जर(ret && ret != MOSQ_ERR_CONN_PENDING){
			log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Unable to connect bridge %s.",
					context->bridge->name);
		}

		db.config->bridges[i] = रिक्त;
	}
}


स्थिर पूर्णांक bridge__set_tcp_keepalive(संरचना mosquitto *context)
{
	अचिन्हित पूर्णांक enabled = 1;
	bool ret;

	जर(context->bridge->tcp_keepalive_idle == 0
			|| context->bridge->tcp_keepalive_interval == 0
			|| context->bridge->tcp_keepalive_counter == 0){

		परत MOSQ_ERR_SUCCESS;
	}

#ifdef WIN32
#  define SETSOCKOPT_TYPE अक्षर *
#अन्यथा
#  define SETSOCKOPT_TYPE स्थिरांक शून्य *
#endif

	ret = setsockopt(context->sock, SOL_SOCKET, SO_KEEPALIVE, (SETSOCKOPT_TYPE)&enabled, आकार(enabled));

#ifdef TCP_KEEPIDLE
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPIDLE,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_idle, आकार(context->bridge->tcp_keepalive_idle));
#endif
#ifdef TCP_KEEPINTVL
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPINTVL,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_interval, आकार(context->bridge->tcp_keepalive_interval));
#endif
#ifdef TCP_KEEPCNT
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPCNT,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_counter, आकार(context->bridge->tcp_keepalive_counter));
#endif

#undef SETSOCKOPT_TYPE

	जर(ret){
		परत MOSQ_ERR_UNKNOWN;
	}

	परत MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TCP_USER_TIMEOUT


स्थिर पूर्णांक bridge__set_tcp_user_timeout(संरचना mosquitto *context)
{
	पूर्णांक timeout = context->bridge->tcp_user_timeout;
	जर(timeout >= 0){
		जर(setsockopt(context->sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (अक्षर *)&timeout, आकार(timeout))){
			परत MOSQ_ERR_UNKNOWN;
		}
	}

	परत MOSQ_ERR_SUCCESS;
}
#endif

#जर defined(__GLIBC__) && defined(WITH_ADNS)


स्थिर पूर्णांक bridge__connect_step1(संरचना mosquitto *context)
{
	पूर्णांक rc;
	अक्षर *notification_topic;
	माप_प्रकार notification_topic_len;
	uint8_t notification_payload;
	संरचना mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	जर(!context || !context->bridge){
		परत MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_संदेश_in = db.now_s;
	context->next_संदेश_out = db.now_s + context->bridge->keepalive;
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
		जर(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(रिक्त, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			जर(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}अन्यथा{
				qos = cur_topic->qos;
			}
			संरचना mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			जर(sub__add(context, &sub) > 0){
				परत 1;
			}
			retain__queue(context, &sub);
		}
	}

	जर(context->bridge->notifications){
		जर(context->max_qos == 0){
			qos = 0;
		}अन्यथा{
			qos = 1;
		}
		जर(context->bridge->notification_topic){
			जर(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, संदेश_EXPIRY_INFINITE, रिक्त);
				context->bridge->initial_notification_done = true;
			}
			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, रिक्त);
			जर(rc != MOSQ_ERR_SUCCESS){
				परत rc;
			}
		}अन्यथा{
			notification_topic_len = म_माप(context->bridge->remote_clientid)+म_माप("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(आकार(अक्षर)*(notification_topic_len+1));
			जर(!notification_topic){
				परत MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			जर(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, संदेश_EXPIRY_INFINITE, रिक्त);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, रिक्त);
			mosquitto_FREE(notification_topic);
			जर(rc != MOSQ_ERR_SUCCESS){
				परत rc;
			}
		}
	}

	log__printf(रिक्त, MOSQ_LOG_NOTICE, "Connecting bridge (step 1) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step1(context, context->bridge->addresses[context->bridge->cur_address].address);
	जर(rc > 0){
		जर(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			परत rc; /* Error already printed */
		}अन्यथा जर(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", म_त्रुटि(त्रुटि_सं));
		}अन्यथा जर(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(त्रुटि_सं));
		}

		परत rc;
	}

	परत MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णांक bridge__connect_step2(संरचना mosquitto *context)
{
	पूर्णांक rc;

	जर(!context || !context->bridge){
		परत MOSQ_ERR_INVAL;
	}

	log__printf(रिक्त, MOSQ_LOG_NOTICE, "Connecting bridge (step 2) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step2(context, context->bridge->addresses[context->bridge->cur_address].port, &context->sock);
	जर(rc > 0){
		जर(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			परत rc; /* Error already printed */
		}अन्यथा जर(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", म_त्रुटि(त्रुटि_सं));
		}अन्यथा जर(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(त्रुटि_सं));
		}

		परत rc;
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, आकार(context->sock), context);

	जर(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}
	परत rc;
}


पूर्णांक bridge__connect_step3(संरचना mosquitto *context)
{
	पूर्णांक rc;
	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = रिक्त;

	rc = net__socket_connect_step3(context, context->bridge->addresses[context->bridge->cur_address].address);
	जर(rc > 0){
		जर(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			परत rc; /* Error already printed */
		}अन्यथा जर(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", म_त्रुटि(त्रुटि_सं));
		}अन्यथा जर(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(त्रुटि_सं));
		}

		परत rc;
	}

	जर(context->bridge->round_robin == false && context->bridge->cur_address != 0){
		context->bridge->primary_retry = db.now_s + 5;
		loop__update_next_event(5000);
	}

	जर(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		परत MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	जर(bridge__set_tcp_user_timeout(context)){
		परत MOSQ_ERR_UNKNOWN;
	}
#endif

	जर(context->bridge->receive_maximum != 0){
		receive_maximum.मूल्य.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	जर(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.मूल्य.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	जर(context->bridge->max_topic_alias != 0){
		topic_alias_max.मूल्य.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc = send__connect(context, context->keepalive, context->clean_start, properties);
	जर(rc == MOSQ_ERR_SUCCESS){
		परत MOSQ_ERR_SUCCESS;
	}अन्यथा जर(rc == MOSQ_ERR_ERRNO && त्रुटि_सं == ENOTCONN){
		परत MOSQ_ERR_SUCCESS;
	}अन्यथा{
		जर(rc == MOSQ_ERR_TLS){
			परत rc; /* Error already printed */
		}अन्यथा जर(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", म_त्रुटि(त्रुटि_सं));
		}अन्यथा जर(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(त्रुटि_सं));
		}
		mux__delete(context);
		net__socket_close(context);
		परत rc;
	}
}
#अन्यथा


पूर्णांक bridge__connect(संरचना mosquitto *context)
{
	पूर्णांक rc, rc2;
	अक्षर *notification_topic = रिक्त;
	माप_प्रकार notification_topic_len;
	uint8_t notification_payload;
	संरचना mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = रिक्त;

	जर(!context || !context->bridge){
		परत MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->last_संदेश_in = db.now_s;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->next_संदेश_out = db.now_s + context->bridge->keepalive;
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
		जर(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(रिक्त, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			जर(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}अन्यथा{
				qos = cur_topic->qos;
			}
			संरचना mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			जर(sub__add(context, &sub) > 0){
				परत 1;
			}
		}
	}

	जर(context->bridge->notifications){
		जर(context->max_qos == 0){
			qos = 0;
		}अन्यथा{
			qos = 1;
		}
		जर(context->bridge->notification_topic){
			जर(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, संदेश_EXPIRY_INFINITE, रिक्त);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, रिक्त);
			जर(rc != MOSQ_ERR_SUCCESS){
				परत rc;
			}
		}अन्यथा{
			notification_topic_len = म_माप(context->bridge->remote_clientid)+म_माप("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(आकार(अक्षर)*(notification_topic_len+1));
			जर(!notification_topic){
				परत MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			जर(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, संदेश_EXPIRY_INFINITE, रिक्त);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, रिक्त);
			जर(rc != MOSQ_ERR_SUCCESS){
				mosquitto_FREE(notification_topic);
				परत rc;
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

	जर(rc > 0){
		जर(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			परत rc; /* Error already printed */
		}अन्यथा जर(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", म_त्रुटि(त्रुटि_सं));
		}अन्यथा जर(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(त्रुटि_सं));
		}

		परत rc;
	}अन्यथा जर(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, आकार(context->sock), context);

	जर(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		परत MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	जर(bridge__set_tcp_user_timeout(context)){
		परत MOSQ_ERR_UNKNOWN;
	}
#endif

	जर(context->bridge->receive_maximum != 0){
		receive_maximum.मूल्य.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	जर(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.मूल्य.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	जर(context->bridge->max_topic_alias != 0){
		topic_alias_max.मूल्य.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc2 = send__connect(context, context->keepalive, context->clean_start, properties);
	जर(rc2 == MOSQ_ERR_SUCCESS){
		परत rc;
	}अन्यथा जर(rc2 == MOSQ_ERR_ERRNO && त्रुटि_सं == ENOTCONN){
		परत MOSQ_ERR_SUCCESS;
	}अन्यथा{
		जर(rc2 == MOSQ_ERR_TLS){
			परत rc2; /* Error already printed */
		}अन्यथा जर(rc2 == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", म_त्रुटि(त्रुटि_सं));
		}अन्यथा जर(rc2 == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(त्रुटि_सं));
		}
		mux__delete(context);
		net__socket_close(context);
		परत rc2;
	}
}
#endif


पूर्णांक bridge__on_connect(संरचना mosquitto *context)
{
	अक्षर *notification_topic;
	माप_प्रकार notification_topic_len;
	संरचना mosquitto__bridge_topic *cur_topic;
	पूर्णांक sub_opts;
	bool retain = true;
	uint8_t qos;

	जर(context->bridge->notifications){
		जर(context->max_qos == 0){
			qos = 0;
		}अन्यथा{
			qos = 1;
		}
		जर(!context->retain_available){
			retain = false;
		}
		अक्षर notification_payload = '1';
		जर(context->bridge->notification_topic){
			जर(!context->bridge->notifications_local_only){
				जर(send__real_publish(context, mosquitto__mid_generate(context),
						context->bridge->notification_topic, 1, &notification_payload, qos, retain, 0, 0, रिक्त, 0)){

					परत 1;
				}
			}
			db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, संदेश_EXPIRY_INFINITE, रिक्त);
		}अन्यथा{
			notification_topic_len = म_माप(context->bridge->remote_clientid)+म_माप("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(आकार(अक्षर)*(notification_topic_len+1));
			जर(!notification_topic){
				परत MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);
			notification_payload = '1';
			जर(!context->bridge->notifications_local_only){
				जर(send__real_publish(context, mosquitto__mid_generate(context),
						notification_topic, 1, &notification_payload, qos, retain, 0, 0, रिक्त, 0)){

					mosquitto_FREE(notification_topic);
					परत 1;
				}
			}
			db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, संदेश_EXPIRY_INFINITE, रिक्त);
			mosquitto_FREE(notification_topic);
		}
	}

	LL_FOREACH(context->bridge->topics, cur_topic){
		जर(cur_topic->direction == bd_in || cur_topic->direction == bd_both){
			जर(cur_topic->qos > context->max_qos){
				sub_opts = context->max_qos;
			}अन्यथा{
				sub_opts = cur_topic->qos;
			}
			जर(context->bridge->protocol_version == mosq_p_mqtt5){
				sub_opts = sub_opts
						| MQTT_SUB_OPT_NO_LOCAL
						| MQTT_SUB_OPT_RETAIN_AS_PUBLISHED
						| MQTT_SUB_OPT_SEND_RETAIN_ALWAYS;
			}
			जर(send__subscribe(context, रिक्त, 1, &cur_topic->remote_topic, sub_opts, रिक्त)){
				परत 1;
			}
		}अन्यथा{
			जर(context->bridge->attempt_unsubscribe){
				जर(send__unsubscribe(context, रिक्त, 1, &cur_topic->remote_topic, रिक्त)){
					/* direction = inwards only. This means we should not be subscribed
					* to the topic. It is possible that we used to be subscribed to
					* this topic so unsubscribe. */
					परत 1;
				}
			}
		}
	}
	संरचना mosquitto_subscription sub;
	स_रखो(&sub, 0, आकार(sub));
	LL_FOREACH(context->bridge->topics, cur_topic){
		sub.topic_filter = cur_topic->local_topic;
		जर(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			जर(cur_topic->qos > context->max_qos){
				sub.options = context->max_qos;
			}अन्यथा{
				sub.options = cur_topic->qos;
			}
			retain__queue(context, &sub);
		}
	}

	context->bridge->connected_at = db.now_s;

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक bridge__register_local_connections(शून्य)
{
	संरचना mosquitto *context, *ctxt_tmp = रिक्त;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		जर(context->bridge){
			जर(mux__new(context)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error in initial bridge registration: %s", म_त्रुटि(त्रुटि_सं));
				परत MOSQ_ERR_UNKNOWN;
			}
			mux__add_out(context);
		}
	}
	परत MOSQ_ERR_SUCCESS;
}


शून्य bridge__reload(शून्य)
{
	पूर्णांक i;
	पूर्णांक j;

	// destroy old bridges that dissappeared
	साठी(i=0; i<db.bridge_मोजणी; i++){
		साठी(j=0; j<db.config->bridge_मोजणी; j++){
			जर(!म_भेद(db.bridges[i]->bridge->name, db.config->bridges[j]->name)){
				थांबा;
			}
		}

		जर(j==db.config->bridge_मोजणी){
			bridge__destroy(db.bridges[i]);
		}
	}

	साठी(i=0; i<db.config->bridge_मोजणी; i++){
		साठी(j=0; j<db.bridge_मोजणी; j++){
			जर(!म_भेद(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
				थांबा;
			}
		}

		जर(j==db.bridge_मोजणी){
			// a new bridge was found, create it
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = रिक्त;
			सुरूठेवा;
		}

		जर(db.config->bridges[i]->reload_type == brt_immediate){
			// in this case, an existing bridge should match
			साठी(j=0; j<db.bridge_मोजणी; j++){
				जर(!म_भेद(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
					थांबा;
				}
			}

			प्रतिज्ञा(j<db.bridge_मोजणी);
			db.bridges[j]->will_delay_interval = 0;
			bridge__destroy(db.bridges[j]);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = रिक्त;
		}
	}
}


शून्य bridge__db_cleanup(शून्य)
{
	पूर्णांक i;

	साठी(i=0; i<db.bridge_मोजणी; i++){
		जर(db.bridges[i]){
			context__cleanup(db.bridges[i], true);
		}
	}
	mosquitto_FREE(db.bridges);
}


शून्य bridge__cleanup(संरचना mosquitto *context)
{
	पूर्णांक i;

	प्रतिज्ञा(db.bridge_मोजणी > 0);

	साठी(i=0; i<db.bridge_मोजणी; i++){
		जर(db.bridges[i] == context){
			db.bridges[i] = db.bridges[db.bridge_मोजणी-1];
			थांबा;
		}
	}

	db.bridge_मोजणी--;
	जर(db.bridge_मोजणी == 0){
		mosquitto_FREE(db.bridges);
	}अन्यथा{
		db.bridges = mosquitto_realloc(db.bridges, (अचिन्हित)db.bridge_मोजणी * आकार(db.bridges[0]));
	}

	mosquitto_FREE(context->bridge->name);
	mosquitto_FREE(context->bridge->local_clientid);
	mosquitto_FREE(context->bridge->local_username);
	mosquitto_FREE(context->bridge->local_password);
#ifdef WITH_TLS
	mosquitto_FREE(context->bridge->tls_certfile);
	mosquitto_FREE(context->bridge->tls_keyfile);
#endif

	जर(context->bridge->remote_clientid != context->id){
		mosquitto_FREE(context->bridge->remote_clientid);
	}
	context->bridge->remote_clientid = रिक्त;

	जर(context->bridge->remote_username != context->username){
		mosquitto_FREE(context->bridge->remote_username);
	}
	context->bridge->remote_username = रिक्त;

	जर(context->bridge->remote_password != context->password){
		mosquitto_FREE(context->bridge->remote_password);
	}
	context->bridge->remote_password = रिक्त;
#ifdef WITH_TLS
	जर(context->ssl_ctx){
		SSL_CTX_free(context->ssl_ctx);
		context->ssl_ctx = रिक्त;
	}
#endif

	साठी(i=0; i<context->bridge->address_मोजणी; i++){
		mosquitto_FREE(context->bridge->addresses[i].address);
	}

	mosquitto_FREE(context->bridge->addresses);

	config__bridge_cleanup(context->bridge);
	context->bridge = रिक्त;
}


स्थिर शून्य bridge__packet_cleanup(संरचना mosquitto *context)
{
	संरचना mosquitto__packet *packet;
	जर(!context){
		परत;
	}

	पर्यंत(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_FREE(packet);
	}
	context->out_packet = रिक्त;
	context->out_packet_last = रिक्त;
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_मोजणी);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_मोजणी = 0;
	context->out_packet_bytes = 0;

	packet__cleanup(&(context->in_packet));
}


स्थिर पूर्णांक rand_between(पूर्णांक खालचा, पूर्णांक वरचा)
{
	पूर्णांक r;
	mosquitto_getrandom(&r, आकार(पूर्णांक));
	परत (असल(r) % (वरचा - खालचा)) + खालचा;
}


स्थिर शून्य bridge__backoff_step(संरचना mosquitto__bridge *bridge)
{
	/*
		“Decorrelated Jitter” calculation, according to:
			https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
	*/

	bridge->restart_timeout = rand_between(bridge->backoff_base, bridge->restart_timeout * 3);
	जर(bridge->restart_timeout > bridge->backoff_cap){
		bridge->restart_timeout = bridge->backoff_cap;
	}
}


स्थिर शून्य bridge__backoff_reset(संरचना mosquitto__bridge *bridge)
{
	bridge->restart_timeout = bridge->backoff_base;
}


स्थिर शून्य bridge__update_backoff(संरचना mosquitto__bridge *bridge)
{
	जर(!bridge){
		परत;
	}
	जर(!bridge->backoff_cap){
		/* skip if not using jitter */
		परत;

	}
	जर(bridge->connected_at && db.now_s - bridge->connected_at >= bridge->stable_connection_period){
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
	पूर्णांक err;
	socklen_t len;

	जर(context->state == mosq_cs_connect_pending){
		len = आकार(पूर्णांक);
		जर(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (अक्षर *)&err, &len)){
			जर(err == 0){
				mosquitto__set_state(context, mosq_cs_new);
#जर defined(WITH_ADNS) && defined(WITH_BRIDGE)
				जर(context->bridge){
					bridge__connect_step3(context);
				}
#endif
			}अन्यथा जर(err == ECONNREFUSED){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				परत;
			}
		}अन्यथा{
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			परत;
		}
	}
}


स्थिर bool reload_if_needed(संरचना mosquitto *context)
{
	पूर्णांक i;

	साठी(i=0; i<db.config->bridge_मोजणी; i++){
		जर(db.config->bridges[i] && !म_भेद(context->bridge->name, db.config->bridges[i]->name)){
			bridge__destroy(context);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = रिक्त;
			loop__update_next_event(100);
			परत true;
		}
	}

	परत false;
}


शून्य bridge_check(शून्य)
{
	स्थिर समय_प्रकार last_check = 0;
	संरचना mosquitto *context = रिक्त;
	socklen_t len;
	पूर्णांक i;
	पूर्णांक rc;
	पूर्णांक err;

	जर(db.now_s <= last_check){
		परत;
	}

	साठी(i=0; i<db.bridge_मोजणी; i++){
		जर(!db.bridges[i]){
			सुरूठेवा;
		}

		context = db.bridges[i];

		जर(net__is_connected(context)){
			mosquitto__check_keepalive(context);
			bridge_check_pending(context);

			/* Check for bridges that are not round robin and not currently
			 * connected to their primary broker. */
			जर(context->bridge->round_robin == false
					&& context->bridge->cur_address != 0
					&& context->bridge->primary_retry
					&& db.now_s >= context->bridge->primary_retry){

				जर(context->bridge->primary_retry_sock == INVALID_SOCKET){
					rc = net__try_connect(context->bridge->addresses[0].address,
							context->bridge->addresses[0].port,
							&context->bridge->primary_retry_sock,
							context->bridge->bind_address, false);

					जर(rc == 0){
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = 0;
						mux__delete(context);
						net__socket_close(context);
						context->bridge->cur_address = 0;
					}
				}अन्यथा{
					len = आकार(पूर्णांक);
					जर(!getsockopt(context->bridge->primary_retry_sock, SOL_SOCKET, SO_ERROR, (अक्षर *)&err, &len)){
						जर(err == 0){
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = 0;
							mux__delete(context);
							net__socket_close(context);
							context->bridge->cur_address = context->bridge->address_मोजणी-1;
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
			जर(reload_if_needed(context)){
				सुरूठेवा;
			}

			/* Want to try to restart the bridge connection */
			जर(!context->bridge->restart_t){
				bridge__update_backoff(context->bridge);
				context->bridge->restart_t = 1000*db.now_s+context->bridge->restart_timeout;
				context->bridge->cur_address++;
				जर(context->bridge->cur_address == context->bridge->address_मोजणी){
					context->bridge->cur_address = 0;
				}
				loop__update_next_event(context->bridge->restart_timeout);
			}अन्यथा{
				जर((context->bridge->start_type == bst_lazy && context->bridge->lazy_reconnect)
						|| (context->bridge->start_type == bst_automatic && 1000*db.now_s >= context->bridge->restart_t)){

#जर defined(__GLIBC__) && defined(WITH_ADNS)
					जर(context->adns){
						/* Connection attempted, waiting on DNS lookup */
						rc = gai_error(context->adns);
						जर(rc == EAI_INPROGRESS){
							/* Just keep on waiting */
						}अन्यथा जर(rc == 0){
							rc = bridge__connect_step2(context);
							जर(rc == MOSQ_ERR_SUCCESS){
								mux__new(context);
								जर(context->out_packet){
									mux__add_out(context);
								}
							}अन्यथा जर(rc == MOSQ_ERR_CONN_PENDING){
								mux__new(context);
								mux__add_out(context);
								context->bridge->restart_t = 0;
							}अन्यथा{
								context->bridge->cur_address++;
								जर(context->bridge->cur_address == context->bridge->address_मोजणी){
									context->bridge->cur_address = 0;
								}
								context->bridge->restart_t = 0;
							}
						}अन्यथा{
							/* Need to retry */
							जर(context->adns->ar_result){
								freeaddrinfo(context->adns->ar_result);
							}
							mosquitto_FREE(context->adns);
							context->bridge->restart_t = 0;
						}
					}अन्यथा{
						rc = bridge__connect_step1(context);
						जर(rc){
							context->bridge->cur_address++;
							जर(context->bridge->cur_address == context->bridge->address_मोजणी){
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
						जर(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING){
							जर(context->bridge->round_robin == false && context->bridge->cur_address != 0){
								context->bridge->primary_retry = db.now_s + 5;
								loop__update_next_event(5000);
							}
							mux__new(context);
							जर(context->out_packet){
								mux__add_out(context);
							}
						}अन्यथा{
							context->bridge->cur_address++;
							जर(context->bridge->cur_address == context->bridge->address_मोजणी){
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
