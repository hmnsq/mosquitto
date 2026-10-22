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

#यदि_अपरिभाषितम् WIN32
#अन्तर्भाव <netdb.h>
#अन्तर्भाव <sys/socket.h>
#अन्तर्भाव <netinet/in.h>
#अन्तर्भाव <netinet/tcp.h>
#अन्यथा
#अन्तर्भाव <winsock2.h>
#अन्तर्भाव <ws2tcpip.h>
#यदि_अन्तः

#यदि_अपरिभाषितम् WIN32
#अन्तर्भाव <unistd.h>
#अन्यथा
#अन्तर्भाव <process.h>
#अन्तर्भाव <winsock2.h>
#अन्तर्भाव <ws2tcpip.h>
#यदि_अन्तः

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

#यदि_परिभाषितम् WITH_BRIDGE

स्थिर शून्य bridge__update_backoff(संरचना mosquitto__bridge *bridge);
#यदि defined(__GLIBC__) && defined(WITH_ADNS)
स्थिर पूर्णाङ्क bridge__connect_step1(संरचना mosquitto *context);
स्थिर पूर्णाङ्क bridge__connect_step2(संरचना mosquitto *context);
#यदि_अन्तः
स्थिर शून्य bridge__packet_cleanup(संरचना mosquitto *context);

स्थिर संरचना mosquitto *bridge__new(संरचना mosquitto__bridge *bridge)
{
	संरचना mosquitto *new_context = रिक्त;
	संरचना mosquitto **bridges;
	वर्ण *local_id;

	प्रतिज्ञा(bridge);

	local_id = mosquitto_strdup(bridge->local_clientid);
	यदि(!local_id){
		प्रत्यागम रिक्त;
	}

	HASH_FIND(hh_id, db.contexts_by_id, local_id, वाक्यदैर्घ्यम्(local_id), new_context);
	यदि(new_context){
		/* (possible from persistent db) */
		mosquitto_FREE(local_id);
	}अन्यथा{
		/* id wasn't found, so generate a new context */
		new_context = context__init();
		यदि(!new_context){
			mosquitto_FREE(local_id);
			प्रत्यागम रिक्त;
		}
		new_context->id = local_id;
		context_सङ्कलय_गन्तव्यम्_by_id(new_context);
	}
	new_context->transport = mosq_t_tcp;
	new_context->bridge = bridge;
	new_context->is_bridge = true;

	new_context->username = bridge->remote_username;
	new_context->password = bridge->remote_password;

#यदि_परिभाषितम् WITH_TLS
	new_context->tls_cafile = bridge->tls_cafile;
	new_context->tls_capath = bridge->tls_capath;
	new_context->tls_certfile = bridge->tls_certfile;
	new_context->tls_keyfile = bridge->tls_keyfile;
	new_context->tls_cert_reqs = SSL_VERIFY_PEER;
	new_context->tls_ocsp_required = bridge->tls_ocsp_required;
	new_context->tls_संस्करणम् = bridge->tls_संस्करणम्;
	new_context->tls_insecure = bridge->tls_insecure;
	new_context->tls_alpn = bridge->tls_alpn;
	new_context->tls_ciphers = bridge->tls_ciphers;
	new_context->tls_13_ciphers = bridge->tls_13_ciphers;
	new_context->tls_engine = रिक्त;
	new_context->tls_keyform = mosq_k_pem;
	new_context->tls_use_os_certs = bridge->tls_use_os_certs;
	new_context->ssl_ctx_defaults = true;
#यदि_परिभाषितम् FINAL_WITH_TLS_PSK
	new_context->tls_psk_तटस्थः = bridge->tls_psk_तटस्थः;
	new_context->tls_psk = bridge->tls_psk;
#यदि_अन्तः
#यदि_अन्तः

	bridge->try_वैयक्तिकम्_accepted = true;
	यदि(bridge->clean_start_local == -1){
		/* default to "regular" clean start setting */
		bridge->clean_start_local = bridge->clean_start;
	}
	new_context->retain_उपलब्धम् = bridge->outgoing_retain;
	new_context->protocol = bridge->protocol_संस्करणम्;
	यदि(!bridge->clean_start_local){
		new_context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
		plugin_persist_handle_client_सङ्कलय(new_context);
		यदि(new_context->expiry_list_item){
			/* We've restored from persistence and been added to the session
			 * expiry list, even though we should never be expired */
			session_expiry__remove(new_context);
		}
	}

	bridges = mosquitto_realloc(db.bridges, (size_t)(db.bridge_गणनम्+1)*परिमाण(संरचना mosquitto *));
	यदि(bridges){
		db.bridges = bridges;
		db.bridge_गणनम्++;
		db.bridges[db.bridge_गणनम्-1] = new_context;
	}अन्यथा{
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
		यदि(!context){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
			प्रत्यागम;
		}
		प्रतिज्ञा(context);

#यदि defined(__GLIBC__) && defined(WITH_ADNS)
		context->bridge->restart_t = 1; /* force quick restart of bridge */
		loop_update_अग्र_event(1000);
		ret = bridge__connect_step1(context);
#अन्यथा
		ret = bridge__connect(context);
#यदि_अन्तः

		यदि(ret && ret != MOSQ_ERR_CONN_PENDING){
			log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Unable to connect bridge %s.",
					context->bridge->name);
		}

		db.config->bridges[i] = रिक्त;
	}
}


स्थिर पूर्णाङ्क bridge_समुच्चयः_tcp_keepalive(संरचना mosquitto *context)
{
	अचिह्नित पूर्णाङ्क enabled = 1;
	bool ret;

	यदि(context->bridge->tcp_keepalive_idle == 0
			|| context->bridge->tcp_keepalive_interval == 0
			|| context->bridge->tcp_keepalive_गणक == 0){

		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

#यदि_परिभाषितम् WIN32
#  define SETSOCKOPT_TYPE वर्ण *
#अन्यथा
#  define SETSOCKOPT_TYPE ध्रुव शून्य *
#यदि_अन्तः

	ret = setsockopt(context->sock, SOL_SOCKET, SO_KEEPALIVE, (SETSOCKOPT_TYPE)&enabled, परिमाण(enabled));

#यदि_परिभाषितम् TCP_KEEPIDLE
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPIDLE,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_idle, परिमाण(context->bridge->tcp_keepalive_idle));
#यदि_अन्तः
#यदि_परिभाषितम् TCP_KEEPINTVL
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPINTVL,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_interval, परिमाण(context->bridge->tcp_keepalive_interval));
#यदि_अन्तः
#यदि_परिभाषितम् TCP_KEEPCNT
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPCNT,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_गणक, परिमाण(context->bridge->tcp_keepalive_गणक));
#यदि_अन्तः

#अपरिभाषय SETSOCKOPT_TYPE

	यदि(ret){
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}

#यदि_परिभाषितम् WITH_TCP_USER_TIMEOUT


स्थिर पूर्णाङ्क bridge_समुच्चयः_tcp_user_timeout(संरचना mosquitto *context)
{
	पूर्णाङ्क timeout = context->bridge->tcp_user_timeout;
	यदि(timeout >= 0){
		यदि(setsockopt(context->sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (वर्ण *)&timeout, परिमाण(timeout))){
			प्रत्यागम MOSQ_ERR_UNKNOWN;
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}
#यदि_अन्तः

#यदि defined(__GLIBC__) && defined(WITH_ADNS)


स्थिर पूर्णाङ्क bridge__connect_step1(संरचना mosquitto *context)
{
	पूर्णाङ्क rc;
	वर्ण *notification_topic;
	size_t notification_topic_len;
	uint8_t notification_payload;
	संरचना mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	यदि(!context || !context->bridge){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	mosquitto_समुच्चयः_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->चरमः_सन्देशः_अन्तर् = db.now_s;
	context->अग्र_सन्देशः_बहिर्गमनम् = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->अन्तर्_packet.payload = रिक्त;
	context->ping_t = 0;
	context->bridge->lazy_reconnect = false;
	context->अधिकतमम्_packet_आकार = context->bridge->अधिकतमम्_packet_आकार;
	bridge__packet_cleanup(context);
	db_सन्देशः_reconnect_reset(context);

	db__messages_delete(context, false);

	/* Delete all local subscriptions even for clean_start==false. We don't
	 * remove any messages and the next loop carries out the resubscription
	 * anyway. This means any unwanted subs will be removed.
	 */
	sub__clean_session(context);

	LL_FOREACH(context->bridge->topics, cur_topic){
		यदि(cur_topic->direction == bd_बहिर्गमनम् || cur_topic->direction == bd_both){
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
			यदि(sub_सङ्कलय(context, &sub) > 0){
				प्रत्यागम 1;
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
			यदि(!context->bridge->initial_notification_समाप्तम्){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, सन्देशः_EXPIRY_INFINITE, रिक्त);
				context->bridge->initial_notification_समाप्तम् = true;
			}
			notification_payload = '0';
			rc = will_समुच्चयः(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, रिक्त);
			यदि(rc != MOSQ_ERR_SUCCESS){
				प्रत्यागम rc;
			}
		}अन्यथा{
			notification_topic_len = वाक्यदैर्घ्यम्(context->bridge->remote_clientid)+वाक्यदैर्घ्यम्("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(परिमाण(वर्ण)*(notification_topic_len+1));
			यदि(!notification_topic){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			यदि(!context->bridge->initial_notification_समाप्तम्){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, सन्देशः_EXPIRY_INFINITE, रिक्त);
				context->bridge->initial_notification_समाप्तम् = true;
			}

			notification_payload = '0';
			rc = will_समुच्चयः(context, notification_topic, 1, &notification_payload, qos, true, रिक्त);
			mosquitto_FREE(notification_topic);
			यदि(rc != MOSQ_ERR_SUCCESS){
				प्रत्यागम rc;
			}
		}
	}

	log__printf(रिक्त, MOSQ_LOG_NOTICE, "Connecting bridge (step 1) %s (%s:%d)", context->bridge->name, context->bridge->स्थानाङ्काः[context->bridge->cur_स्थानाङ्कः].स्थानाङ्कः, context->bridge->स्थानाङ्काः[context->bridge->cur_स्थानाङ्कः].पत्तनम्);
	rc = net__try_connect_step1(context, context->bridge->स्थानाङ्काः[context->bridge->cur_स्थानाङ्कः].स्थानाङ्कः);
	यदि(rc > 0){
		यदि(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			प्रत्यागम rc; /* Error already printed */
		}अन्यथा यदि(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}अन्यथा यदि(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		प्रत्यागम rc;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क bridge__connect_step2(संरचना mosquitto *context)
{
	पूर्णाङ्क rc;

	यदि(!context || !context->bridge){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	log__printf(रिक्त, MOSQ_LOG_NOTICE, "Connecting bridge (step 2) %s (%s:%d)", context->bridge->name, context->bridge->स्थानाङ्काः[context->bridge->cur_स्थानाङ्कः].स्थानाङ्कः, context->bridge->स्थानाङ्काः[context->bridge->cur_स्थानाङ्कः].पत्तनम्);
	rc = net__try_connect_step2(context, context->bridge->स्थानाङ्काः[context->bridge->cur_स्थानाङ्कः].पत्तनम्, &context->sock);
	यदि(rc > 0){
		यदि(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			प्रत्यागम rc; /* Error already printed */
		}अन्यथा यदि(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}अन्यथा यदि(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		प्रत्यागम rc;
	}

	HASH_सङ्कलय(hh_sock, db.contexts_by_sock, sock, परिमाण(context->sock), context);

	यदि(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto_समुच्चयः_state(context, mosq_cs_connect_pending);
		mux_सङ्कलय_बहिर्गमनम्(context);
	}
	प्रत्यागम rc;
}


पूर्णाङ्क bridge__connect_step3(संरचना mosquitto *context)
{
	पूर्णाङ्क rc;
	mosquitto_property receive_अधिकतमम्;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = रिक्त;

	rc = net__socket_connect_step3(context, context->bridge->स्थानाङ्काः[context->bridge->cur_स्थानाङ्कः].स्थानाङ्कः);
	यदि(rc > 0){
		यदि(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			प्रत्यागम rc; /* Error already printed */
		}अन्यथा यदि(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}अन्यथा यदि(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		प्रत्यागम rc;
	}

	यदि(context->bridge->आवर्तनम्_robin == false && context->bridge->cur_स्थानाङ्कः != 0){
		context->bridge->primary_retry = db.now_s + 5;
		loop_update_अग्र_event(5000);
	}

	यदि(bridge_समुच्चयः_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}
#यदि_परिभाषितम् WITH_TCP_USER_TIMEOUT
	यदि(bridge_समुच्चयः_tcp_user_timeout(context)){
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}
#यदि_अन्तः

	यदि(context->bridge->receive_अधिकतमम् != 0){
		receive_अधिकतमम्.मानम्.i16 = context->bridge->receive_अधिकतमम्;
		receive_अधिकतमम्.identifier = MQTT_PROP_RECEIVE_अधिकतमम्;
		receive_अधिकतमम्.property_type = MQTT_PROP_TYPE_INT16;
		receive_अधिकतमम्.client_generated = false;
		receive_अधिकतमम्.अग्र = properties;
		properties = &receive_अधिकतमम्;
	}
	यदि(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_तात्कालिकम्){
		session_expiry_interval.मानम्.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.अग्र = properties;
		properties = &session_expiry_interval;
	}
	यदि(context->bridge->max_topic_alias != 0){
		topic_alias_max.मानम्.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_अधिकतमम्;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.अग्र = properties;
		properties = &topic_alias_max;
	}

	rc = send__connect(context, context->keepalive, context->clean_start, properties);
	यदि(rc == MOSQ_ERR_SUCCESS){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अन्यथा यदि(rc == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अन्यथा{
		यदि(rc == MOSQ_ERR_TLS){
			प्रत्यागम rc; /* Error already printed */
		}अन्यथा यदि(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}अन्यथा यदि(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		प्रत्यागम rc;
	}
}
#अन्यथा


पूर्णाङ्क bridge__connect(संरचना mosquitto *context)
{
	पूर्णाङ्क rc, rc2;
	वर्ण *notification_topic = रिक्त;
	size_t notification_topic_len;
	uint8_t notification_payload;
	संरचना mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	mosquitto_property receive_अधिकतमम्;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = रिक्त;

	यदि(!context || !context->bridge){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	mosquitto_समुच्चयः_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->चरमः_सन्देशः_अन्तर् = db.now_s;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->अग्र_सन्देशः_बहिर्गमनम् = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->अन्तर्_packet.payload = रिक्त;
	context->ping_t = 0;
	context->bridge->lazy_reconnect = false;
	context->अधिकतमम्_packet_आकार = context->bridge->अधिकतमम्_packet_आकार;
	bridge__packet_cleanup(context);
	db_सन्देशः_reconnect_reset(context);

	db__messages_delete(context, false);

	/* Delete all local subscriptions even for clean_start==false. We don't
	 * remove any messages and the next loop carries out the resubscription
	 * anyway. This means any unwanted subs will be removed.
	 */
	sub__clean_session(context);

	LL_FOREACH(context->bridge->topics, cur_topic){
		यदि(cur_topic->direction == bd_बहिर्गमनम् || cur_topic->direction == bd_both){
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
			यदि(sub_सङ्कलय(context, &sub) > 0){
				प्रत्यागम 1;
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
			यदि(!context->bridge->initial_notification_समाप्तम्){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, सन्देशः_EXPIRY_INFINITE, रिक्त);
				context->bridge->initial_notification_समाप्तम् = true;
			}

			notification_payload = '0';
			rc = will_समुच्चयः(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, रिक्त);
			यदि(rc != MOSQ_ERR_SUCCESS){
				प्रत्यागम rc;
			}
		}अन्यथा{
			notification_topic_len = वाक्यदैर्घ्यम्(context->bridge->remote_clientid)+वाक्यदैर्घ्यम्("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(परिमाण(वर्ण)*(notification_topic_len+1));
			यदि(!notification_topic){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			यदि(!context->bridge->initial_notification_समाप्तम्){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, सन्देशः_EXPIRY_INFINITE, रिक्त);
				context->bridge->initial_notification_समाप्तम् = true;
			}

			notification_payload = '0';
			rc = will_समुच्चयः(context, notification_topic, 1, &notification_payload, qos, true, रिक्त);
			यदि(rc != MOSQ_ERR_SUCCESS){
				mosquitto_FREE(notification_topic);
				प्रत्यागम rc;
			}
			mosquitto_FREE(notification_topic);
		}
	}

	log__printf(रिक्त, MOSQ_LOG_NOTICE, "Connecting bridge %s (%s:%d)", context->bridge->name, context->bridge->स्थानाङ्काः[context->bridge->cur_स्थानाङ्कः].स्थानाङ्कः, context->bridge->स्थानाङ्काः[context->bridge->cur_स्थानाङ्कः].पत्तनम्);
	rc = net__socket_connect(context,
			context->bridge->स्थानाङ्काः[context->bridge->cur_स्थानाङ्कः].स्थानाङ्कः,
			context->bridge->स्थानाङ्काः[context->bridge->cur_स्थानाङ्कः].पत्तनम्,
			context->bridge->bind_स्थानाङ्कः,
			false);

	यदि(rc > 0){
		यदि(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			प्रत्यागम rc; /* Error already printed */
		}अन्यथा यदि(rc == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}अन्यथा यदि(rc == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		प्रत्यागम rc;
	}अन्यथा यदि(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto_समुच्चयः_state(context, mosq_cs_connect_pending);
		mux_सङ्कलय_बहिर्गमनम्(context);
	}

	HASH_सङ्कलय(hh_sock, db.contexts_by_sock, sock, परिमाण(context->sock), context);

	यदि(bridge_समुच्चयः_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}
#यदि_परिभाषितम् WITH_TCP_USER_TIMEOUT
	यदि(bridge_समुच्चयः_tcp_user_timeout(context)){
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}
#यदि_अन्तः

	यदि(context->bridge->receive_अधिकतमम् != 0){
		receive_अधिकतमम्.मानम्.i16 = context->bridge->receive_अधिकतमम्;
		receive_अधिकतमम्.identifier = MQTT_PROP_RECEIVE_अधिकतमम्;
		receive_अधिकतमम्.property_type = MQTT_PROP_TYPE_INT16;
		receive_अधिकतमम्.client_generated = false;
		receive_अधिकतमम्.अग्र = properties;
		properties = &receive_अधिकतमम्;
	}
	यदि(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_तात्कालिकम्){
		session_expiry_interval.मानम्.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.अग्र = properties;
		properties = &session_expiry_interval;
	}
	यदि(context->bridge->max_topic_alias != 0){
		topic_alias_max.मानम्.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_अधिकतमम्;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.अग्र = properties;
		properties = &topic_alias_max;
	}

	rc2 = send__connect(context, context->keepalive, context->clean_start, properties);
	यदि(rc2 == MOSQ_ERR_SUCCESS){
		प्रत्यागम rc;
	}अन्यथा यदि(rc2 == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अन्यथा{
		यदि(rc2 == MOSQ_ERR_TLS){
			प्रत्यागम rc2; /* Error already printed */
		}अन्यथा यदि(rc2 == MOSQ_ERR_ERRNO){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}अन्यथा यदि(rc2 == MOSQ_ERR_EAI){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		प्रत्यागम rc2;
	}
}
#यदि_अन्तः


पूर्णाङ्क bridge__on_connect(संरचना mosquitto *context)
{
	वर्ण *notification_topic;
	size_t notification_topic_len;
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
		यदि(!context->retain_उपलब्धम्){
			retain = false;
		}
		वर्ण notification_payload = '1';
		यदि(context->bridge->notification_topic){
			यदि(!context->bridge->notifications_local_only){
				यदि(send__real_publish(context, mosquitto__mid_generate(context),
						context->bridge->notification_topic, 1, &notification_payload, qos, retain, 0, 0, रिक्त, 0)){

					प्रत्यागम 1;
				}
			}
			db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, सन्देशः_EXPIRY_INFINITE, रिक्त);
		}अन्यथा{
			notification_topic_len = वाक्यदैर्घ्यम्(context->bridge->remote_clientid)+वाक्यदैर्घ्यम्("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(परिमाण(वर्ण)*(notification_topic_len+1));
			यदि(!notification_topic){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);
			notification_payload = '1';
			यदि(!context->bridge->notifications_local_only){
				यदि(send__real_publish(context, mosquitto__mid_generate(context),
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
		यदि(cur_topic->direction == bd_अन्तर् || cur_topic->direction == bd_both){
			यदि(cur_topic->qos > context->max_qos){
				sub_opts = context->max_qos;
			}अन्यथा{
				sub_opts = cur_topic->qos;
			}
			यदि(context->bridge->protocol_संस्करणम् == mosq_p_mqtt5){
				sub_opts = sub_opts
						| MQTT_SUB_OPT_NO_LOCAL
						| MQTT_SUB_OPT_RETAIN_AS_PUBLISHED
						| MQTT_SUB_OPT_SEND_RETAIN_ALWAYS;
			}
			यदि(send__subscribe(context, रिक्त, 1, &cur_topic->remote_topic, sub_opts, रिक्त)){
				प्रत्यागम 1;
			}
		}अन्यथा{
			यदि(context->bridge->attempt_unsubscribe){
				यदि(send__unsubscribe(context, रिक्त, 1, &cur_topic->remote_topic, रिक्त)){
					/* direction = inwards only. This means we should not be subscribed
					* to the topic. It is possible that we used to be subscribed to
					* this topic so unsubscribe. */
					प्रत्यागम 1;
				}
			}
		}
	}
	संरचना mosquitto_subscription sub;
	स्मृतिं_पूरय(&sub, 0, परिमाण(sub));
	LL_FOREACH(context->bridge->topics, cur_topic){
		sub.topic_filter = cur_topic->local_topic;
		यदि(cur_topic->direction == bd_बहिर्गमनम् || cur_topic->direction == bd_both){
			यदि(cur_topic->qos > context->max_qos){
				sub.options = context->max_qos;
			}अन्यथा{
				sub.options = cur_topic->qos;
			}
			retain__queue(context, &sub);
		}
	}

	context->bridge->connected_at = db.now_s;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क bridge_पञ्जिकाकोशः_local_connections(शून्य)
{
	संरचना mosquitto *context, *ctxt_tmp = रिक्त;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		यदि(context->bridge){
			यदि(mux__new(context)){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error in initial bridge registration: %s", strerror(errno));
				प्रत्यागम MOSQ_ERR_UNKNOWN;
			}
			mux_सङ्कलय_बहिर्गमनम्(context);
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
			यदि(!वाक्यं_तुलय(db.bridges[i]->bridge->name, db.config->bridges[j]->name)){
				भङ्ग;
			}
		}

		यदि(j==db.config->bridge_गणनम्){
			bridge__destroy(db.bridges[i]);
		}
	}

	प्रति(i=0; i<db.config->bridge_गणनम्; i++){
		प्रति(j=0; j<db.bridge_गणनम्; j++){
			यदि(!वाक्यं_तुलय(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
				भङ्ग;
			}
		}

		यदि(j==db.bridge_गणनम्){
			// a new bridge was found, create it
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = रिक्त;
			अनुवर्त;
		}

		यदि(db.config->bridges[i]->reload_type == brt_तात्कालिकम्){
			// in this case, an existing bridge should match
			प्रति(j=0; j<db.bridge_गणनम्; j++){
				यदि(!वाक्यं_तुलय(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
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
		यदि(db.bridges[i]){
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
		यदि(db.bridges[i] == context){
			db.bridges[i] = db.bridges[db.bridge_गणनम्-1];
			भङ्ग;
		}
	}

	db.bridge_गणनम्--;
	यदि(db.bridge_गणनम् == 0){
		mosquitto_FREE(db.bridges);
	}अन्यथा{
		db.bridges = mosquitto_realloc(db.bridges, (अचिह्नित)db.bridge_गणनम् * परिमाण(db.bridges[0]));
	}

	mosquitto_FREE(context->bridge->name);
	mosquitto_FREE(context->bridge->local_clientid);
	mosquitto_FREE(context->bridge->local_username);
	mosquitto_FREE(context->bridge->local_password);
#यदि_परिभाषितम् WITH_TLS
	mosquitto_FREE(context->bridge->tls_certfile);
	mosquitto_FREE(context->bridge->tls_keyfile);
#यदि_अन्तः

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
#यदि_परिभाषितम् WITH_TLS
	यदि(context->ssl_ctx){
		SSL_CTX_free(context->ssl_ctx);
		context->ssl_ctx = रिक्त;
	}
#यदि_अन्तः

	प्रति(i=0; i<context->bridge->स्थानाङ्कः_गणनम्; i++){
		mosquitto_FREE(context->bridge->स्थानाङ्काः[i].स्थानाङ्कः);
	}

	mosquitto_FREE(context->bridge->स्थानाङ्काः);

	config__bridge_cleanup(context->bridge);
	context->bridge = रिक्त;
}


स्थिर शून्य bridge__packet_cleanup(संरचना mosquitto *context)
{
	संरचना mosquitto__packet *packet;
	यदि(!context){
		प्रत्यागम;
	}

	यावत्(context->बहिर्गमनम्_packet){
		packet = context->बहिर्गमनम्_packet;
		context->बहिर्गमनम्_packet = context->बहिर्गमनम्_packet->अग्र;
		mosquitto_FREE(packet);
	}
	context->बहिर्गमनम्_packet = रिक्त;
	context->बहिर्गमनम्_packet_चरमः = रिक्त;
	metrics__int_dec(mosq_gauge_बहिर्गमनम्_packets, context->बहिर्गमनम्_packet_गणनम्);
	metrics__int_dec(mosq_gauge_बहिर्गमनम्_packet_अष्टकानि, context->बहिर्गमनम्_packet_अष्टकानि);
	context->बहिर्गमनम्_packet_गणनम् = 0;
	context->बहिर्गमनम्_packet_अष्टकानि = 0;

	packet__cleanup(&(context->अन्तर्_packet));
}


स्थिर पूर्णाङ्क rand_between(पूर्णाङ्क नीचः, पूर्णाङ्क उच्चः)
{
	पूर्णाङ्क r;
	mosquitto_getrandom(&r, परिमाण(पूर्णाङ्क));
	प्रत्यागम (निरपेक्षम्(r) % (उच्चः - नीचः)) + नीचः;
}


स्थिर शून्य bridge_backoff_चरणः(संरचना mosquitto__bridge *bridge)
{
	/*
		“Decorrelated Jitter” calculation, according to:
			https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
	*/

	bridge->restart_timeout = rand_between(bridge->backoff_आधारः, bridge->restart_timeout * 3);
	यदि(bridge->restart_timeout > bridge->backoff_cap){
		bridge->restart_timeout = bridge->backoff_cap;
	}
}


स्थिर शून्य bridge__backoff_reset(संरचना mosquitto__bridge *bridge)
{
	bridge->restart_timeout = bridge->backoff_आधारः;
}


स्थिर शून्य bridge__update_backoff(संरचना mosquitto__bridge *bridge)
{
	यदि(!bridge){
		प्रत्यागम;
	}
	यदि(!bridge->backoff_cap){
		/* skip if not using jitter */
		प्रत्यागम;

	}
	यदि(bridge->connected_at && db.now_s - bridge->connected_at >= bridge->stable_connection_period){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Bridge %s connection was stable enough, resetting backoff", bridge->name);
		bridge__backoff_reset(bridge);
	}अन्यथा{
		bridge_backoff_चरणः(bridge);
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
				mosquitto_समुच्चयः_state(context, mosq_cs_new);
#यदि defined(WITH_ADNS) && defined(WITH_BRIDGE)
				यदि(context->bridge){
					bridge__connect_step3(context);
				}
#यदि_अन्तः
			}अन्यथा यदि(err == ECONNREFUSED){
				do_disconnect(context, MOSQ_ERR_CONN_नष्टम्);
				प्रत्यागम;
			}
		}अन्यथा{
			do_disconnect(context, MOSQ_ERR_CONN_नष्टम्);
			प्रत्यागम;
		}
	}
}


स्थिर bool reload_if_needed(संरचना mosquitto *context)
{
	पूर्णाङ्क i;

	प्रति(i=0; i<db.config->bridge_गणनम्; i++){
		यदि(db.config->bridges[i] && !वाक्यं_तुलय(context->bridge->name, db.config->bridges[i]->name)){
			bridge__destroy(context);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = रिक्त;
			loop_update_अग्र_event(100);
			प्रत्यागम true;
		}
	}

	प्रत्यागम false;
}


शून्य bridge_check(शून्य)
{
	स्थिर time_t चरमः_check = 0;
	संरचना mosquitto *context = रिक्त;
	socklen_t len;
	पूर्णाङ्क i;
	पूर्णाङ्क rc;
	पूर्णाङ्क err;

	यदि(db.now_s <= चरमः_check){
		प्रत्यागम;
	}

	प्रति(i=0; i<db.bridge_गणनम्; i++){
		यदि(!db.bridges[i]){
			अनुवर्त;
		}

		context = db.bridges[i];

		यदि(net__is_connected(context)){
			mosquitto__check_keepalive(context);
			bridge_check_pending(context);

			/* Check for bridges that are not round robin and not currently
			 * connected to their primary broker. */
			यदि(context->bridge->आवर्तनम्_robin == false
					&& context->bridge->cur_स्थानाङ्कः != 0
					&& context->bridge->primary_retry
					&& db.now_s >= context->bridge->primary_retry){

				यदि(context->bridge->primary_retry_sock == INVALID_SOCKET){
					rc = net__try_connect(context->bridge->स्थानाङ्काः[0].स्थानाङ्कः,
							context->bridge->स्थानाङ्काः[0].पत्तनम्,
							&context->bridge->primary_retry_sock,
							context->bridge->bind_स्थानाङ्कः, false);

					यदि(rc == 0){
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = 0;
						mux__delete(context);
						net__socket_close(context);
						context->bridge->cur_स्थानाङ्कः = 0;
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
							context->bridge->cur_स्थानाङ्कः = context->bridge->स्थानाङ्कः_गणनम्-1;
						}अन्यथा{
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = db.now_s+5;
							loop_update_अग्र_event(5000);
						}
					}अन्यथा{
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = db.now_s+5;
						loop_update_अग्र_event(5000);
					}
				}
			}
		}अन्यथा{
			यदि(reload_if_needed(context)){
				अनुवर्त;
			}

			/* Want to try to restart the bridge connection */
			यदि(!context->bridge->restart_t){
				bridge__update_backoff(context->bridge);
				context->bridge->restart_t = 1000*db.now_s+context->bridge->restart_timeout;
				context->bridge->cur_स्थानाङ्कः++;
				यदि(context->bridge->cur_स्थानाङ्कः == context->bridge->स्थानाङ्कः_गणनम्){
					context->bridge->cur_स्थानाङ्कः = 0;
				}
				loop_update_अग्र_event(context->bridge->restart_timeout);
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
								यदि(context->बहिर्गमनम्_packet){
									mux_सङ्कलय_बहिर्गमनम्(context);
								}
							}अन्यथा यदि(rc == MOSQ_ERR_CONN_PENDING){
								mux__new(context);
								mux_सङ्कलय_बहिर्गमनम्(context);
								context->bridge->restart_t = 0;
							}अन्यथा{
								context->bridge->cur_स्थानाङ्कः++;
								यदि(context->bridge->cur_स्थानाङ्कः == context->bridge->स्थानाङ्कः_गणनम्){
									context->bridge->cur_स्थानाङ्कः = 0;
								}
								context->bridge->restart_t = 0;
							}
						}अन्यथा{
							/* Need to retry */
							यदि(context->adns->ar_परिणाम){
								freeaddrinfo(context->adns->ar_परिणाम);
							}
							mosquitto_FREE(context->adns);
							context->bridge->restart_t = 0;
						}
					}अन्यथा{
						rc = bridge__connect_step1(context);
						यदि(rc){
							context->bridge->cur_स्थानाङ्कः++;
							यदि(context->bridge->cur_स्थानाङ्कः == context->bridge->स्थानाङ्कः_गणनम्){
								context->bridge->cur_स्थानाङ्कः = 0;
							}
						}अन्यथा{
							/* Short wait for ADNS lookup */
							context->bridge->restart_t = 1;
							loop_update_अग्र_event(1000);
						}
					}
#अन्यथा
					{
						rc = bridge__connect(context);
						context->bridge->restart_t = 0;
						यदि(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING){
							यदि(context->bridge->आवर्तनम्_robin == false && context->bridge->cur_स्थानाङ्कः != 0){
								context->bridge->primary_retry = db.now_s + 5;
								loop_update_अग्र_event(5000);
							}
							mux__new(context);
							यदि(context->बहिर्गमनम्_packet){
								mux_सङ्कलय_बहिर्गमनम्(context);
							}
						}अन्यथा{
							context->bridge->cur_स्थानाङ्कः++;
							यदि(context->bridge->cur_स्थानाङ्कः == context->bridge->स्थानाङ्कः_गणनम्){
								context->bridge->cur_स्थानाङ्कः = 0;
							}
						}
					}
#यदि_अन्तः
				}
			}
		}
	}
}

#यदि_अन्तः
