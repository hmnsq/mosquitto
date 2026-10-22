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

#समावेश "config.h"

#समावेश <assert.h>
#समावेश <errno.h>
#समावेश <stdio.h>
#समावेश <string.h>

#अगर_अघोषित WIN32
#समावेश <netdb.h>
#समावेश <sys/socket.h>
#समावेश <netinet/in.h>
#समावेश <netinet/tcp.h>
#अन्यथा
#समावेश <winsock2.h>
#समावेश <ws2tcpip.h>
#पूर्ण_अगर

#अगर_अघोषित WIN32
#समावेश <unistd.h>
#अन्यथा
#समावेश <process.h>
#समावेश <winsock2.h>
#समावेश <ws2tcpip.h>
#पूर्ण_अगर

#समावेश "mosquitto.h"
#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto_internal.h"
#समावेश "net_mosq.h"
#समावेश "packet_mosq.h"
#समावेश "property_common.h"
#समावेश "send_mosq.h"
#समावेश "sys_tree.h"
#समावेश "tls_mosq.h"
#समावेश "util_mosq.h"
#समावेश "will_mosq.h"
#समावेश "utlist.h"

#अगर_घोषित WITH_BRIDGE

अटल व्योम bridge__update_backoff(काष्ठा mosquitto__bridge *bridge);
#अगर defined(__GLIBC__) && defined(WITH_ADNS)
अटल पूर्णांक bridge__connect_step1(काष्ठा mosquitto *context);
अटल पूर्णांक bridge__connect_step2(काष्ठा mosquitto *context);
#पूर्ण_अगर
अटल व्योम bridge__packet_cleanup(काष्ठा mosquitto *context);

अटल काष्ठा mosquitto *bridge__new(काष्ठा mosquitto__bridge *bridge)
{
	काष्ठा mosquitto *new_context = शून्य;
	काष्ठा mosquitto **bridges;
	अक्षर *local_id;

	निश्चित(bridge);

	local_id = mosquitto_strdup(bridge->local_clientid);
	अगर(!local_id){
		वापस शून्य;
	}

	HASH_FIND(hh_id, db.contexts_by_id, local_id, म_माप(local_id), new_context);
	अगर(new_context){
		/* (possible from persistent db) */
		mosquitto_FREE(local_id);
	}अन्यथा{
		/* id wasn't found, so generate a new context */
		new_context = context__init();
		अगर(!new_context){
			mosquitto_FREE(local_id);
			वापस शून्य;
		}
		new_context->id = local_id;
		context__add_to_by_id(new_context);
	}
	new_context->transport = mosq_t_tcp;
	new_context->bridge = bridge;
	new_context->is_bridge = true;

	new_context->username = bridge->remote_username;
	new_context->password = bridge->remote_password;

#अगर_घोषित WITH_TLS
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
	new_context->tls_engine = शून्य;
	new_context->tls_keyform = mosq_k_pem;
	new_context->tls_use_os_certs = bridge->tls_use_os_certs;
	new_context->ssl_ctx_defaults = true;
#अगर_घोषित FINAL_WITH_TLS_PSK
	new_context->tls_psk_identity = bridge->tls_psk_identity;
	new_context->tls_psk = bridge->tls_psk;
#पूर्ण_अगर
#पूर्ण_अगर

	bridge->try_private_accepted = true;
	अगर(bridge->clean_start_local == -1){
		/* default to "regular" clean start setting */
		bridge->clean_start_local = bridge->clean_start;
	}
	new_context->retain_available = bridge->outgoing_retain;
	new_context->protocol = bridge->protocol_version;
	अगर(!bridge->clean_start_local){
		new_context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
		plugin_persist__handle_client_add(new_context);
		अगर(new_context->expiry_list_item){
			/* We've restored from persistence and been added to the session
			 * expiry list, even though we should never be expired */
			session_expiry__remove(new_context);
		}
	}

	bridges = mosquitto_realloc(db.bridges, (माप_प्रकार)(db.bridge_गिनती+1)*माप(काष्ठा mosquitto *));
	अगर(bridges){
		db.bridges = bridges;
		db.bridge_गिनती++;
		db.bridges[db.bridge_गिनती-1] = new_context;
	}अन्यथा{
		वापस शून्य;
	}

	वापस new_context;
}


अटल व्योम bridge__destroy(काष्ठा mosquitto *context)
{
	send__disconnect(context, MQTT_RC_SUCCESS, शून्य);
	context__cleanup(context, true);
}


व्योम bridge__start_all(व्योम)
{
	क्रम(पूर्णांक i=0; i<db.config->bridge_गिनती; i++){
		काष्ठा mosquitto *context;
		पूर्णांक ret;

		context = bridge__new(db.config->bridges[i]);
		अगर(!context){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
			वापस;
		}
		निश्चित(context);

#अगर defined(__GLIBC__) && defined(WITH_ADNS)
		context->bridge->restart_t = 1; /* force quick restart of bridge */
		loop_update_अग्र_event(1000);
		ret = bridge__connect_step1(context);
#अन्यथा
		ret = bridge__connect(context);
#पूर्ण_अगर

		अगर(ret && ret != MOSQ_ERR_CONN_PENDING){
			log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Unable to connect bridge %s.",
					context->bridge->name);
		}

		db.config->bridges[i] = शून्य;
	}
}


अटल पूर्णांक bridge__set_tcp_keepalive(काष्ठा mosquitto *context)
{
	अचिन्हित पूर्णांक enabled = 1;
	bool ret;

	अगर(context->bridge->tcp_keepalive_idle == 0
			|| context->bridge->tcp_keepalive_interval == 0
			|| context->bridge->tcp_keepalive_गणक == 0){

		वापस MOSQ_ERR_SUCCESS;
	}

#अगर_घोषित WIN32
#  define SETSOCKOPT_TYPE अक्षर *
#अन्यथा
#  define SETSOCKOPT_TYPE स्थिर व्योम *
#पूर्ण_अगर

	ret = setsockopt(context->sock, SOL_SOCKET, SO_KEEPALIVE, (SETSOCKOPT_TYPE)&enabled, माप(enabled));

#अगर_घोषित TCP_KEEPIDLE
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPIDLE,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_idle, माप(context->bridge->tcp_keepalive_idle));
#पूर्ण_अगर
#अगर_घोषित TCP_KEEPINTVL
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPINTVL,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_interval, माप(context->bridge->tcp_keepalive_interval));
#पूर्ण_अगर
#अगर_घोषित TCP_KEEPCNT
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPCNT,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_गणक, माप(context->bridge->tcp_keepalive_गणक));
#पूर्ण_अगर

#अघोषित SETSOCKOPT_TYPE

	अगर(ret){
		वापस MOSQ_ERR_UNKNOWN;
	}

	वापस MOSQ_ERR_SUCCESS;
}

#अगर_घोषित WITH_TCP_USER_TIMEOUT


अटल पूर्णांक bridge__set_tcp_user_timeout(काष्ठा mosquitto *context)
{
	पूर्णांक timeout = context->bridge->tcp_user_timeout;
	अगर(timeout >= 0){
		अगर(setsockopt(context->sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (अक्षर *)&timeout, माप(timeout))){
			वापस MOSQ_ERR_UNKNOWN;
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}
#पूर्ण_अगर

#अगर defined(__GLIBC__) && defined(WITH_ADNS)


अटल पूर्णांक bridge__connect_step1(काष्ठा mosquitto *context)
{
	पूर्णांक rc;
	अक्षर *notification_topic;
	माप_प्रकार notification_topic_len;
	uint8_t notification_payload;
	काष्ठा mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	अगर(!context || !context->bridge){
		वापस MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_संदेश_in = db.now_s;
	context->अग्र_संदेश_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = शून्य;
	context->ping_t = 0;
	context->bridge->lazy_reconnect = false;
	context->maximum_packet_आकार = context->bridge->maximum_packet_आकार;
	bridge__packet_cleanup(context);
	db__message_reconnect_reset(context);

	db__messages_delete(context, false);

	/* Delete all local subscriptions even for clean_start==false. We don't
	 * remove any messages and the next loop carries out the resubscription
	 * anyway. This means any unwanted subs will be removed.
	 */
	sub__clean_session(context);

	LL_FOREACH(context->bridge->topics, cur_topic){
		अगर(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(शून्य, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			अगर(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}अन्यथा{
				qos = cur_topic->qos;
			}
			काष्ठा mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			अगर(sub__add(context, &sub) > 0){
				वापस 1;
			}
			retain__queue(context, &sub);
		}
	}

	अगर(context->bridge->notifications){
		अगर(context->max_qos == 0){
			qos = 0;
		}अन्यथा{
			qos = 1;
		}
		अगर(context->bridge->notification_topic){
			अगर(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, संदेश_EXPIRY_INFINITE, शून्य);
				context->bridge->initial_notification_done = true;
			}
			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, शून्य);
			अगर(rc != MOSQ_ERR_SUCCESS){
				वापस rc;
			}
		}अन्यथा{
			notification_topic_len = म_माप(context->bridge->remote_clientid)+म_माप("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(माप(अक्षर)*(notification_topic_len+1));
			अगर(!notification_topic){
				वापस MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			अगर(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, संदेश_EXPIRY_INFINITE, शून्य);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, शून्य);
			mosquitto_FREE(notification_topic);
			अगर(rc != MOSQ_ERR_SUCCESS){
				वापस rc;
			}
		}
	}

	log__printf(शून्य, MOSQ_LOG_NOTICE, "Connecting bridge (step 1) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step1(context, context->bridge->addresses[context->bridge->cur_address].address);
	अगर(rc > 0){
		अगर(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			वापस rc; /* Error already printed */
		}अन्यथा अगर(rc == MOSQ_ERR_ERRNO){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error creating bridge: %s.", म_त्रुटि(त्रुटि_सं));
		}अन्यथा अगर(rc == MOSQ_ERR_EAI){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(त्रुटि_सं));
		}

		वापस rc;
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक bridge__connect_step2(काष्ठा mosquitto *context)
{
	पूर्णांक rc;

	अगर(!context || !context->bridge){
		वापस MOSQ_ERR_INVAL;
	}

	log__printf(शून्य, MOSQ_LOG_NOTICE, "Connecting bridge (step 2) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step2(context, context->bridge->addresses[context->bridge->cur_address].port, &context->sock);
	अगर(rc > 0){
		अगर(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			वापस rc; /* Error already printed */
		}अन्यथा अगर(rc == MOSQ_ERR_ERRNO){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error creating bridge: %s.", म_त्रुटि(त्रुटि_सं));
		}अन्यथा अगर(rc == MOSQ_ERR_EAI){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(त्रुटि_सं));
		}

		वापस rc;
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, माप(context->sock), context);

	अगर(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}
	वापस rc;
}


पूर्णांक bridge__connect_step3(काष्ठा mosquitto *context)
{
	पूर्णांक rc;
	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = शून्य;

	rc = net__socket_connect_step3(context, context->bridge->addresses[context->bridge->cur_address].address);
	अगर(rc > 0){
		अगर(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			वापस rc; /* Error already printed */
		}अन्यथा अगर(rc == MOSQ_ERR_ERRNO){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error creating bridge: %s.", म_त्रुटि(त्रुटि_सं));
		}अन्यथा अगर(rc == MOSQ_ERR_EAI){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(त्रुटि_सं));
		}

		वापस rc;
	}

	अगर(context->bridge->round_robin == false && context->bridge->cur_address != 0){
		context->bridge->primary_retry = db.now_s + 5;
		loop_update_अग्र_event(5000);
	}

	अगर(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		वापस MOSQ_ERR_UNKNOWN;
	}
#अगर_घोषित WITH_TCP_USER_TIMEOUT
	अगर(bridge__set_tcp_user_timeout(context)){
		वापस MOSQ_ERR_UNKNOWN;
	}
#पूर्ण_अगर

	अगर(context->bridge->receive_maximum != 0){
		receive_maximum.मूल्य.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.अग्र = properties;
		properties = &receive_maximum;
	}
	अगर(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.मूल्य.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.अग्र = properties;
		properties = &session_expiry_interval;
	}
	अगर(context->bridge->max_topic_alias != 0){
		topic_alias_max.मूल्य.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.अग्र = properties;
		properties = &topic_alias_max;
	}

	rc = send__connect(context, context->keepalive, context->clean_start, properties);
	अगर(rc == MOSQ_ERR_SUCCESS){
		वापस MOSQ_ERR_SUCCESS;
	}अन्यथा अगर(rc == MOSQ_ERR_ERRNO && त्रुटि_सं == ENOTCONN){
		वापस MOSQ_ERR_SUCCESS;
	}अन्यथा{
		अगर(rc == MOSQ_ERR_TLS){
			वापस rc; /* Error already printed */
		}अन्यथा अगर(rc == MOSQ_ERR_ERRNO){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error creating bridge: %s.", म_त्रुटि(त्रुटि_सं));
		}अन्यथा अगर(rc == MOSQ_ERR_EAI){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(त्रुटि_सं));
		}
		mux__delete(context);
		net__socket_close(context);
		वापस rc;
	}
}
#अन्यथा


पूर्णांक bridge__connect(काष्ठा mosquitto *context)
{
	पूर्णांक rc, rc2;
	अक्षर *notification_topic = शून्य;
	माप_प्रकार notification_topic_len;
	uint8_t notification_payload;
	काष्ठा mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = शून्य;

	अगर(!context || !context->bridge){
		वापस MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->last_संदेश_in = db.now_s;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->अग्र_संदेश_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = शून्य;
	context->ping_t = 0;
	context->bridge->lazy_reconnect = false;
	context->maximum_packet_आकार = context->bridge->maximum_packet_आकार;
	bridge__packet_cleanup(context);
	db__message_reconnect_reset(context);

	db__messages_delete(context, false);

	/* Delete all local subscriptions even for clean_start==false. We don't
	 * remove any messages and the next loop carries out the resubscription
	 * anyway. This means any unwanted subs will be removed.
	 */
	sub__clean_session(context);

	LL_FOREACH(context->bridge->topics, cur_topic){
		अगर(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(शून्य, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			अगर(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}अन्यथा{
				qos = cur_topic->qos;
			}
			काष्ठा mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			अगर(sub__add(context, &sub) > 0){
				वापस 1;
			}
		}
	}

	अगर(context->bridge->notifications){
		अगर(context->max_qos == 0){
			qos = 0;
		}अन्यथा{
			qos = 1;
		}
		अगर(context->bridge->notification_topic){
			अगर(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, संदेश_EXPIRY_INFINITE, शून्य);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, शून्य);
			अगर(rc != MOSQ_ERR_SUCCESS){
				वापस rc;
			}
		}अन्यथा{
			notification_topic_len = म_माप(context->bridge->remote_clientid)+म_माप("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(माप(अक्षर)*(notification_topic_len+1));
			अगर(!notification_topic){
				वापस MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			अगर(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, संदेश_EXPIRY_INFINITE, शून्य);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, शून्य);
			अगर(rc != MOSQ_ERR_SUCCESS){
				mosquitto_FREE(notification_topic);
				वापस rc;
			}
			mosquitto_FREE(notification_topic);
		}
	}

	log__printf(शून्य, MOSQ_LOG_NOTICE, "Connecting bridge %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__socket_connect(context,
			context->bridge->addresses[context->bridge->cur_address].address,
			context->bridge->addresses[context->bridge->cur_address].port,
			context->bridge->bind_address,
			false);

	अगर(rc > 0){
		अगर(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			वापस rc; /* Error already printed */
		}अन्यथा अगर(rc == MOSQ_ERR_ERRNO){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error creating bridge: %s.", म_त्रुटि(त्रुटि_सं));
		}अन्यथा अगर(rc == MOSQ_ERR_EAI){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(त्रुटि_सं));
		}

		वापस rc;
	}अन्यथा अगर(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, माप(context->sock), context);

	अगर(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		वापस MOSQ_ERR_UNKNOWN;
	}
#अगर_घोषित WITH_TCP_USER_TIMEOUT
	अगर(bridge__set_tcp_user_timeout(context)){
		वापस MOSQ_ERR_UNKNOWN;
	}
#पूर्ण_अगर

	अगर(context->bridge->receive_maximum != 0){
		receive_maximum.मूल्य.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.अग्र = properties;
		properties = &receive_maximum;
	}
	अगर(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.मूल्य.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.अग्र = properties;
		properties = &session_expiry_interval;
	}
	अगर(context->bridge->max_topic_alias != 0){
		topic_alias_max.मूल्य.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.अग्र = properties;
		properties = &topic_alias_max;
	}

	rc2 = send__connect(context, context->keepalive, context->clean_start, properties);
	अगर(rc2 == MOSQ_ERR_SUCCESS){
		वापस rc;
	}अन्यथा अगर(rc2 == MOSQ_ERR_ERRNO && त्रुटि_सं == ENOTCONN){
		वापस MOSQ_ERR_SUCCESS;
	}अन्यथा{
		अगर(rc2 == MOSQ_ERR_TLS){
			वापस rc2; /* Error already printed */
		}अन्यथा अगर(rc2 == MOSQ_ERR_ERRNO){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error creating bridge: %s.", म_त्रुटि(त्रुटि_सं));
		}अन्यथा अगर(rc2 == MOSQ_ERR_EAI){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(त्रुटि_सं));
		}
		mux__delete(context);
		net__socket_close(context);
		वापस rc2;
	}
}
#पूर्ण_अगर


पूर्णांक bridge__on_connect(काष्ठा mosquitto *context)
{
	अक्षर *notification_topic;
	माप_प्रकार notification_topic_len;
	काष्ठा mosquitto__bridge_topic *cur_topic;
	पूर्णांक sub_opts;
	bool retain = true;
	uint8_t qos;

	अगर(context->bridge->notifications){
		अगर(context->max_qos == 0){
			qos = 0;
		}अन्यथा{
			qos = 1;
		}
		अगर(!context->retain_available){
			retain = false;
		}
		अक्षर notification_payload = '1';
		अगर(context->bridge->notification_topic){
			अगर(!context->bridge->notifications_local_only){
				अगर(send__real_publish(context, mosquitto__mid_generate(context),
						context->bridge->notification_topic, 1, &notification_payload, qos, retain, 0, 0, शून्य, 0)){

					वापस 1;
				}
			}
			db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, संदेश_EXPIRY_INFINITE, शून्य);
		}अन्यथा{
			notification_topic_len = म_माप(context->bridge->remote_clientid)+म_माप("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(माप(अक्षर)*(notification_topic_len+1));
			अगर(!notification_topic){
				वापस MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);
			notification_payload = '1';
			अगर(!context->bridge->notifications_local_only){
				अगर(send__real_publish(context, mosquitto__mid_generate(context),
						notification_topic, 1, &notification_payload, qos, retain, 0, 0, शून्य, 0)){

					mosquitto_FREE(notification_topic);
					वापस 1;
				}
			}
			db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, संदेश_EXPIRY_INFINITE, शून्य);
			mosquitto_FREE(notification_topic);
		}
	}

	LL_FOREACH(context->bridge->topics, cur_topic){
		अगर(cur_topic->direction == bd_in || cur_topic->direction == bd_both){
			अगर(cur_topic->qos > context->max_qos){
				sub_opts = context->max_qos;
			}अन्यथा{
				sub_opts = cur_topic->qos;
			}
			अगर(context->bridge->protocol_version == mosq_p_mqtt5){
				sub_opts = sub_opts
						| MQTT_SUB_OPT_NO_LOCAL
						| MQTT_SUB_OPT_RETAIN_AS_PUBLISHED
						| MQTT_SUB_OPT_SEND_RETAIN_ALWAYS;
			}
			अगर(send__subscribe(context, शून्य, 1, &cur_topic->remote_topic, sub_opts, शून्य)){
				वापस 1;
			}
		}अन्यथा{
			अगर(context->bridge->attempt_unsubscribe){
				अगर(send__unsubscribe(context, शून्य, 1, &cur_topic->remote_topic, शून्य)){
					/* direction = inwards only. This means we should not be subscribed
					* to the topic. It is possible that we used to be subscribed to
					* this topic so unsubscribe. */
					वापस 1;
				}
			}
		}
	}
	काष्ठा mosquitto_subscription sub;
	स_रखो(&sub, 0, माप(sub));
	LL_FOREACH(context->bridge->topics, cur_topic){
		sub.topic_filter = cur_topic->local_topic;
		अगर(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			अगर(cur_topic->qos > context->max_qos){
				sub.options = context->max_qos;
			}अन्यथा{
				sub.options = cur_topic->qos;
			}
			retain__queue(context, &sub);
		}
	}

	context->bridge->connected_at = db.now_s;

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक bridge__register_local_connections(व्योम)
{
	काष्ठा mosquitto *context, *ctxt_tmp = शून्य;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		अगर(context->bridge){
			अगर(mux__new(context)){
				log__printf(शून्य, MOSQ_LOG_ERR, "Error in initial bridge registration: %s", म_त्रुटि(त्रुटि_सं));
				वापस MOSQ_ERR_UNKNOWN;
			}
			mux__add_out(context);
		}
	}
	वापस MOSQ_ERR_SUCCESS;
}


व्योम bridge__reload(व्योम)
{
	पूर्णांक i;
	पूर्णांक j;

	// destroy old bridges that dissappeared
	क्रम(i=0; i<db.bridge_गिनती; i++){
		क्रम(j=0; j<db.config->bridge_गिनती; j++){
			अगर(!म_भेद(db.bridges[i]->bridge->name, db.config->bridges[j]->name)){
				अवरोध;
			}
		}

		अगर(j==db.config->bridge_गिनती){
			bridge__destroy(db.bridges[i]);
		}
	}

	क्रम(i=0; i<db.config->bridge_गिनती; i++){
		क्रम(j=0; j<db.bridge_गिनती; j++){
			अगर(!म_भेद(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
				अवरोध;
			}
		}

		अगर(j==db.bridge_गिनती){
			// a new bridge was found, create it
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = शून्य;
			जारी;
		}

		अगर(db.config->bridges[i]->reload_type == brt_immediate){
			// in this case, an existing bridge should match
			क्रम(j=0; j<db.bridge_गिनती; j++){
				अगर(!म_भेद(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
					अवरोध;
				}
			}

			निश्चित(j<db.bridge_गिनती);
			db.bridges[j]->will_delay_interval = 0;
			bridge__destroy(db.bridges[j]);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = शून्य;
		}
	}
}


व्योम bridge__db_cleanup(व्योम)
{
	पूर्णांक i;

	क्रम(i=0; i<db.bridge_गिनती; i++){
		अगर(db.bridges[i]){
			context__cleanup(db.bridges[i], true);
		}
	}
	mosquitto_FREE(db.bridges);
}


व्योम bridge__cleanup(काष्ठा mosquitto *context)
{
	पूर्णांक i;

	निश्चित(db.bridge_गिनती > 0);

	क्रम(i=0; i<db.bridge_गिनती; i++){
		अगर(db.bridges[i] == context){
			db.bridges[i] = db.bridges[db.bridge_गिनती-1];
			अवरोध;
		}
	}

	db.bridge_गिनती--;
	अगर(db.bridge_गिनती == 0){
		mosquitto_FREE(db.bridges);
	}अन्यथा{
		db.bridges = mosquitto_realloc(db.bridges, (अचिन्हित)db.bridge_गिनती * माप(db.bridges[0]));
	}

	mosquitto_FREE(context->bridge->name);
	mosquitto_FREE(context->bridge->local_clientid);
	mosquitto_FREE(context->bridge->local_username);
	mosquitto_FREE(context->bridge->local_password);
#अगर_घोषित WITH_TLS
	mosquitto_FREE(context->bridge->tls_certfile);
	mosquitto_FREE(context->bridge->tls_keyfile);
#पूर्ण_अगर

	अगर(context->bridge->remote_clientid != context->id){
		mosquitto_FREE(context->bridge->remote_clientid);
	}
	context->bridge->remote_clientid = शून्य;

	अगर(context->bridge->remote_username != context->username){
		mosquitto_FREE(context->bridge->remote_username);
	}
	context->bridge->remote_username = शून्य;

	अगर(context->bridge->remote_password != context->password){
		mosquitto_FREE(context->bridge->remote_password);
	}
	context->bridge->remote_password = शून्य;
#अगर_घोषित WITH_TLS
	अगर(context->ssl_ctx){
		SSL_CTX_free(context->ssl_ctx);
		context->ssl_ctx = शून्य;
	}
#पूर्ण_अगर

	क्रम(i=0; i<context->bridge->address_गिनती; i++){
		mosquitto_FREE(context->bridge->addresses[i].address);
	}

	mosquitto_FREE(context->bridge->addresses);

	config__bridge_cleanup(context->bridge);
	context->bridge = शून्य;
}


अटल व्योम bridge__packet_cleanup(काष्ठा mosquitto *context)
{
	काष्ठा mosquitto__packet *packet;
	अगर(!context){
		वापस;
	}

	जबतक(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->अग्र;
		mosquitto_FREE(packet);
	}
	context->out_packet = शून्य;
	context->out_packet_last = शून्य;
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_गिनती);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_गिनती = 0;
	context->out_packet_bytes = 0;

	packet__cleanup(&(context->in_packet));
}


अटल पूर्णांक rand_between(पूर्णांक निचला, पूर्णांक ऊपरी)
{
	पूर्णांक r;
	mosquitto_getrandom(&r, माप(पूर्णांक));
	वापस (असल(r) % (ऊपरी - निचला)) + निचला;
}


अटल व्योम bridge__backoff_step(काष्ठा mosquitto__bridge *bridge)
{
	/*
		“Decorrelated Jitter” calculation, according to:
			https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
	*/

	bridge->restart_timeout = rand_between(bridge->backoff_base, bridge->restart_timeout * 3);
	अगर(bridge->restart_timeout > bridge->backoff_cap){
		bridge->restart_timeout = bridge->backoff_cap;
	}
}


अटल व्योम bridge__backoff_reset(काष्ठा mosquitto__bridge *bridge)
{
	bridge->restart_timeout = bridge->backoff_base;
}


अटल व्योम bridge__update_backoff(काष्ठा mosquitto__bridge *bridge)
{
	अगर(!bridge){
		वापस;
	}
	अगर(!bridge->backoff_cap){
		/* skip if not using jitter */
		वापस;

	}
	अगर(bridge->connected_at && db.now_s - bridge->connected_at >= bridge->stable_connection_period){
		log__printf(शून्य, MOSQ_LOG_INFO, "Bridge %s connection was stable enough, resetting backoff", bridge->name);
		bridge__backoff_reset(bridge);
	}अन्यथा{
		bridge__backoff_step(bridge);
	}

	bridge->connected_at = 0;

	log__printf(शून्य, MOSQ_LOG_INFO, "Bridge %s next backoff will be %d ms", bridge->name, bridge->restart_timeout);
}


अटल व्योम bridge_check_pending(काष्ठा mosquitto *context)
{
	पूर्णांक err;
	socklen_t len;

	अगर(context->state == mosq_cs_connect_pending){
		len = माप(पूर्णांक);
		अगर(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (अक्षर *)&err, &len)){
			अगर(err == 0){
				mosquitto__set_state(context, mosq_cs_new);
#अगर defined(WITH_ADNS) && defined(WITH_BRIDGE)
				अगर(context->bridge){
					bridge__connect_step3(context);
				}
#पूर्ण_अगर
			}अन्यथा अगर(err == ECONNREFUSED){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				वापस;
			}
		}अन्यथा{
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			वापस;
		}
	}
}


अटल bool reload_if_needed(काष्ठा mosquitto *context)
{
	पूर्णांक i;

	क्रम(i=0; i<db.config->bridge_गिनती; i++){
		अगर(db.config->bridges[i] && !म_भेद(context->bridge->name, db.config->bridges[i]->name)){
			bridge__destroy(context);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = शून्य;
			loop_update_अग्र_event(100);
			वापस true;
		}
	}

	वापस false;
}


व्योम bridge_check(व्योम)
{
	अटल समय_प्रकार last_check = 0;
	काष्ठा mosquitto *context = शून्य;
	socklen_t len;
	पूर्णांक i;
	पूर्णांक rc;
	पूर्णांक err;

	अगर(db.now_s <= last_check){
		वापस;
	}

	क्रम(i=0; i<db.bridge_गिनती; i++){
		अगर(!db.bridges[i]){
			जारी;
		}

		context = db.bridges[i];

		अगर(net__is_connected(context)){
			mosquitto__check_keepalive(context);
			bridge_check_pending(context);

			/* Check for bridges that are not round robin and not currently
			 * connected to their primary broker. */
			अगर(context->bridge->round_robin == false
					&& context->bridge->cur_address != 0
					&& context->bridge->primary_retry
					&& db.now_s >= context->bridge->primary_retry){

				अगर(context->bridge->primary_retry_sock == INVALID_SOCKET){
					rc = net__try_connect(context->bridge->addresses[0].address,
							context->bridge->addresses[0].port,
							&context->bridge->primary_retry_sock,
							context->bridge->bind_address, false);

					अगर(rc == 0){
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = 0;
						mux__delete(context);
						net__socket_close(context);
						context->bridge->cur_address = 0;
					}
				}अन्यथा{
					len = माप(पूर्णांक);
					अगर(!getsockopt(context->bridge->primary_retry_sock, SOL_SOCKET, SO_ERROR, (अक्षर *)&err, &len)){
						अगर(err == 0){
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = 0;
							mux__delete(context);
							net__socket_close(context);
							context->bridge->cur_address = context->bridge->address_गिनती-1;
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
			अगर(reload_if_needed(context)){
				जारी;
			}

			/* Want to try to restart the bridge connection */
			अगर(!context->bridge->restart_t){
				bridge__update_backoff(context->bridge);
				context->bridge->restart_t = 1000*db.now_s+context->bridge->restart_timeout;
				context->bridge->cur_address++;
				अगर(context->bridge->cur_address == context->bridge->address_गिनती){
					context->bridge->cur_address = 0;
				}
				loop_update_अग्र_event(context->bridge->restart_timeout);
			}अन्यथा{
				अगर((context->bridge->start_type == bst_lazy && context->bridge->lazy_reconnect)
						|| (context->bridge->start_type == bst_automatic && 1000*db.now_s >= context->bridge->restart_t)){

#अगर defined(__GLIBC__) && defined(WITH_ADNS)
					अगर(context->adns){
						/* Connection attempted, waiting on DNS lookup */
						rc = gai_error(context->adns);
						अगर(rc == EAI_INPROGRESS){
							/* Just keep on waiting */
						}अन्यथा अगर(rc == 0){
							rc = bridge__connect_step2(context);
							अगर(rc == MOSQ_ERR_SUCCESS){
								mux__new(context);
								अगर(context->out_packet){
									mux__add_out(context);
								}
							}अन्यथा अगर(rc == MOSQ_ERR_CONN_PENDING){
								mux__new(context);
								mux__add_out(context);
								context->bridge->restart_t = 0;
							}अन्यथा{
								context->bridge->cur_address++;
								अगर(context->bridge->cur_address == context->bridge->address_गिनती){
									context->bridge->cur_address = 0;
								}
								context->bridge->restart_t = 0;
							}
						}अन्यथा{
							/* Need to retry */
							अगर(context->adns->ar_परिणाम){
								freeaddrinfo(context->adns->ar_परिणाम);
							}
							mosquitto_FREE(context->adns);
							context->bridge->restart_t = 0;
						}
					}अन्यथा{
						rc = bridge__connect_step1(context);
						अगर(rc){
							context->bridge->cur_address++;
							अगर(context->bridge->cur_address == context->bridge->address_गिनती){
								context->bridge->cur_address = 0;
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
						अगर(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING){
							अगर(context->bridge->round_robin == false && context->bridge->cur_address != 0){
								context->bridge->primary_retry = db.now_s + 5;
								loop_update_अग्र_event(5000);
							}
							mux__new(context);
							अगर(context->out_packet){
								mux__add_out(context);
							}
						}अन्यथा{
							context->bridge->cur_address++;
							अगर(context->bridge->cur_address == context->bridge->address_गिनती){
								context->bridge->cur_address = 0;
							}
						}
					}
#पूर्ण_अगर
				}
			}
		}
	}
}

#पूर्ण_अगर
