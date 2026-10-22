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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <errno.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ifndef WIN32
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#ଅନ୍ତର୍ଭାଵ <sys/socket.h>
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#ଅନ୍ତର୍ଭାଵ <netinet/tcp.h>
#ଅନ୍ୟଥା
#ଅନ୍ତର୍ଭାଵ <winsock2.h>
#ଅନ୍ତର୍ଭାଵ <ws2tcpip.h>
#endif

#ifndef WIN32
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଅନ୍ୟଥା
#ଅନ୍ତର୍ଭାଵ <process.h>
#ଅନ୍ତର୍ଭାଵ <winsock2.h>
#ଅନ୍ତର୍ଭାଵ <ws2tcpip.h>
#endif

#ଅନ୍ତର୍ଭାଵ "mosquitto.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto_internal.h"
#ଅନ୍ତର୍ଭାଵ "net_mosq.h"
#ଅନ୍ତର୍ଭାଵ "packet_mosq.h"
#ଅନ୍ତର୍ଭାଵ "property_common.h"
#ଅନ୍ତର୍ଭାଵ "send_mosq.h"
#ଅନ୍ତର୍ଭାଵ "sys_tree.h"
#ଅନ୍ତର୍ଭାଵ "tls_mosq.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"
#ଅନ୍ତର୍ଭାଵ "will_mosq.h"
#ଅନ୍ତର୍ଭାଵ "utlist.h"

#ifdef WITH_BRIDGE

ସ୍ଥିର ଶୂନ୍ୟ bridge__update_backoff(ସଂରଚନା mosquitto__bridge *bridge);
#ଯଦି defined(__GLIBC__) && defined(WITH_ADNS)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__connect_step1(ସଂରଚନା mosquitto *context);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__connect_step2(ସଂରଚନା mosquitto *context);
#endif
ସ୍ଥିର ଶୂନ୍ୟ bridge__packet_cleanup(ସଂରଚନା mosquitto *context);

ସ୍ଥିର ସଂରଚନା mosquitto *bridge__new(ସଂରଚନା mosquitto__bridge *bridge)
{
	ସଂରଚନା mosquitto *new_context = ଖାଲି;
	ସଂରଚନା mosquitto **bridges;
	ଅକ୍ଷର *local_id;

	ପ୍ରତିଜ୍ଞା(bridge);

	local_id = mosquitto_strdup(bridge->local_clientid);
	ଯଦି(!local_id){
		ଫେରନ୍ତୁ ଖାଲି;
	}

	HASH_FIND(hh_id, db.contexts_by_id, local_id, strlen(local_id), new_context);
	ଯଦି(new_context){
		/* (possible from persistent db) */
		mosquitto_FREE(local_id);
	}ଅନ୍ୟଥା{
		/* id wasn't found, so generate a new context */
		new_context = context__init();
		ଯଦି(!new_context){
			mosquitto_FREE(local_id);
			ଫେରନ୍ତୁ ଖାଲି;
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
	new_context->tls_engine = ଖାଲି;
	new_context->tls_keyform = mosq_k_pem;
	new_context->tls_use_os_certs = bridge->tls_use_os_certs;
	new_context->ssl_ctx_defaults = true;
#ifdef FINAL_WITH_TLS_PSK
	new_context->tls_psk_identity = bridge->tls_psk_identity;
	new_context->tls_psk = bridge->tls_psk;
#endif
#endif

	bridge->try_private_accepted = true;
	ଯଦି(bridge->clean_start_local == -1){
		/* default to "regular" clean start setting */
		bridge->clean_start_local = bridge->clean_start;
	}
	new_context->retain_available = bridge->outgoing_retain;
	new_context->protocol = bridge->protocol_version;
	ଯଦି(!bridge->clean_start_local){
		new_context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
		plugin_persist__handle_client_add(new_context);
		ଯଦି(new_context->expiry_list_item){
			/* We've restored from persistence and been added to the session
			 * expiry list, even though we should never be expired */
			session_expiry__remove(new_context);
		}
	}

	bridges = mosquitto_realloc(db.bridges, (size_t)(db.bridge_ଗଣନା+1)*ଆକାର(ସଂରଚନା mosquitto *));
	ଯଦି(bridges){
		db.bridges = bridges;
		db.bridge_ଗଣନା++;
		db.bridges[db.bridge_ଗଣନା-1] = new_context;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ ଖାଲି;
	}

	ଫେରନ୍ତୁ new_context;
}


ସ୍ଥିର ଶୂନ୍ୟ bridge__destroy(ସଂରଚନା mosquitto *context)
{
	send__disconnect(context, MQTT_RC_SUCCESS, ଖାଲି);
	context__cleanup(context, true);
}


ଶୂନ୍ୟ bridge__start_all(ଶୂନ୍ୟ)
{
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.config->bridge_ଗଣନା; i++){
		ସଂରଚନା mosquitto *context;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

		context = bridge__new(db.config->bridges[i]);
		ଯଦି(!context){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
			ଫେରନ୍ତୁ;
		}
		ପ୍ରତିଜ୍ଞା(context);

#ଯଦି defined(__GLIBC__) && defined(WITH_ADNS)
		context->bridge->restart_t = 1; /* force quick restart of bridge */
		loop__update_next_event(1000);
		ret = bridge__connect_step1(context);
#ଅନ୍ୟଥା
		ret = bridge__connect(context);
#endif

		ଯଦି(ret && ret != MOSQ_ERR_CONN_PENDING){
			log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Unable to connect bridge %s.",
					context->bridge->name);
		}

		db.config->bridges[i] = ଖାଲି;
	}
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__set_tcp_keepalive(ସଂରଚନା mosquitto *context)
{
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା enabled = 1;
	bool ret;

	ଯଦି(context->bridge->tcp_keepalive_idle == 0
			|| context->bridge->tcp_keepalive_interval == 0
			|| context->bridge->tcp_keepalive_counter == 0){

		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

#ifdef WIN32
#  define SETSOCKOPT_TYPE ଅକ୍ଷର *
#ଅନ୍ୟଥା
#  define SETSOCKOPT_TYPE ଅଚଳ ଶୂନ୍ୟ *
#endif

	ret = setsockopt(context->sock, SOL_SOCKET, SO_KEEPALIVE, (SETSOCKOPT_TYPE)&enabled, ଆକାର(enabled));

#ifdef TCP_KEEPIDLE
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPIDLE,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_idle, ଆକାର(context->bridge->tcp_keepalive_idle));
#endif
#ifdef TCP_KEEPINTVL
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPINTVL,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_interval, ଆକାର(context->bridge->tcp_keepalive_interval));
#endif
#ifdef TCP_KEEPCNT
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPCNT,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_counter, ଆକାର(context->bridge->tcp_keepalive_counter));
#endif

#undef SETSOCKOPT_TYPE

	ଯଦି(ret){
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TCP_USER_TIMEOUT


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__set_tcp_user_timeout(ସଂରଚନା mosquitto *context)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeout = context->bridge->tcp_user_timeout;
	ଯଦି(timeout >= 0){
		ଯଦି(setsockopt(context->sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (ଅକ୍ଷର *)&timeout, ଆକାର(timeout))){
			ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}
#endif

#ଯଦି defined(__GLIBC__) && defined(WITH_ADNS)


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__connect_step1(ସଂରଚନା mosquitto *context)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ଅକ୍ଷର *notification_topic;
	size_t notification_topic_len;
	uint8_t notification_payload;
	ସଂରଚନା mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	ଯଦି(!context || !context->bridge){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_ସନ୍ଦେଶ_in = db.now_s;
	context->next_ସନ୍ଦେଶ_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = ଖାଲି;
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
		ଯଦି(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			ଯଦି(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}ଅନ୍ୟଥା{
				qos = cur_topic->qos;
			}
			ସଂରଚନା mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			ଯଦି(sub__add(context, &sub) > 0){
				ଫେରନ୍ତୁ 1;
			}
			retain__queue(context, &sub);
		}
	}

	ଯଦି(context->bridge->notifications){
		ଯଦି(context->max_qos == 0){
			qos = 0;
		}ଅନ୍ୟଥା{
			qos = 1;
		}
		ଯଦି(context->bridge->notification_topic){
			ଯଦି(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, ସନ୍ଦେଶ_EXPIRY_INFINITE, ଖାଲି);
				context->bridge->initial_notification_done = true;
			}
			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, ଖାଲି);
			ଯଦି(rc != MOSQ_ERR_SUCCESS){
				ଫେରନ୍ତୁ rc;
			}
		}ଅନ୍ୟଥା{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(ଆକାର(ଅକ୍ଷର)*(notification_topic_len+1));
			ଯଦି(!notification_topic){
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			ଯଦି(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, ସନ୍ଦେଶ_EXPIRY_INFINITE, ଖାଲି);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, ଖାଲି);
			mosquitto_FREE(notification_topic);
			ଯଦି(rc != MOSQ_ERR_SUCCESS){
				ଫେରନ୍ତୁ rc;
			}
		}
	}

	log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Connecting bridge (step 1) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step1(context, context->bridge->addresses[context->bridge->cur_address].address);
	ଯଦି(rc > 0){
		ଯଦି(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ଫେରନ୍ତୁ rc; /* Error already printed */
		}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_ERRNO){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_EAI){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__connect_step2(ସଂରଚନା mosquitto *context)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(!context || !context->bridge){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Connecting bridge (step 2) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step2(context, context->bridge->addresses[context->bridge->cur_address].port, &context->sock);
	ଯଦି(rc > 0){
		ଯଦି(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ଫେରନ୍ତୁ rc; /* Error already printed */
		}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_ERRNO){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_EAI){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ଫେରନ୍ତୁ rc;
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, ଆକାର(context->sock), context);

	ଯଦି(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}
	ଫେରନ୍ତୁ rc;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__connect_step3(ସଂରଚନା mosquitto *context)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = ଖାଲି;

	rc = net__socket_connect_step3(context, context->bridge->addresses[context->bridge->cur_address].address);
	ଯଦି(rc > 0){
		ଯଦି(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ଫେରନ୍ତୁ rc; /* Error already printed */
		}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_ERRNO){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_EAI){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ଫେରନ୍ତୁ rc;
	}

	ଯଦି(context->bridge->round_robin == false && context->bridge->cur_address != 0){
		context->bridge->primary_retry = db.now_s + 5;
		loop__update_next_event(5000);
	}

	ଯଦି(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	ଯଦି(bridge__set_tcp_user_timeout(context)){
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}
#endif

	ଯଦି(context->bridge->receive_maximum != 0){
		receive_maximum.ମୂଲ୍ୟ.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	ଯଦି(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.ମୂଲ୍ୟ.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	ଯଦି(context->bridge->max_topic_alias != 0){
		topic_alias_max.ମୂଲ୍ୟ.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc = send__connect(context, context->keepalive, context->clean_start, properties);
	ଯଦି(rc == MOSQ_ERR_SUCCESS){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}ଅନ୍ୟଥା{
		ଯଦି(rc == MOSQ_ERR_TLS){
			ଫେରନ୍ତୁ rc; /* Error already printed */
		}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_ERRNO){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_EAI){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		ଫେରନ୍ତୁ rc;
	}
}
#ଅନ୍ୟଥା


ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__connect(ସଂରଚନା mosquitto *context)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, rc2;
	ଅକ୍ଷର *notification_topic = ଖାଲି;
	size_t notification_topic_len;
	uint8_t notification_payload;
	ସଂରଚନା mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = ଖାଲି;

	ଯଦି(!context || !context->bridge){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->last_ସନ୍ଦେଶ_in = db.now_s;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->next_ସନ୍ଦେଶ_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = ଖାଲି;
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
		ଯଦି(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			ଯଦି(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}ଅନ୍ୟଥା{
				qos = cur_topic->qos;
			}
			ସଂରଚନା mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			ଯଦି(sub__add(context, &sub) > 0){
				ଫେରନ୍ତୁ 1;
			}
		}
	}

	ଯଦି(context->bridge->notifications){
		ଯଦି(context->max_qos == 0){
			qos = 0;
		}ଅନ୍ୟଥା{
			qos = 1;
		}
		ଯଦି(context->bridge->notification_topic){
			ଯଦି(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, ସନ୍ଦେଶ_EXPIRY_INFINITE, ଖାଲି);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, ଖାଲି);
			ଯଦି(rc != MOSQ_ERR_SUCCESS){
				ଫେରନ୍ତୁ rc;
			}
		}ଅନ୍ୟଥା{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(ଆକାର(ଅକ୍ଷର)*(notification_topic_len+1));
			ଯଦି(!notification_topic){
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			ଯଦି(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, ସନ୍ଦେଶ_EXPIRY_INFINITE, ଖାଲି);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, ଖାଲି);
			ଯଦି(rc != MOSQ_ERR_SUCCESS){
				mosquitto_FREE(notification_topic);
				ଫେରନ୍ତୁ rc;
			}
			mosquitto_FREE(notification_topic);
		}
	}

	log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Connecting bridge %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__socket_connect(context,
			context->bridge->addresses[context->bridge->cur_address].address,
			context->bridge->addresses[context->bridge->cur_address].port,
			context->bridge->bind_address,
			false);

	ଯଦି(rc > 0){
		ଯଦି(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ଫେରନ୍ତୁ rc; /* Error already printed */
		}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_ERRNO){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_EAI){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ଫେରନ୍ତୁ rc;
	}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, ଆକାର(context->sock), context);

	ଯଦି(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	ଯଦି(bridge__set_tcp_user_timeout(context)){
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}
#endif

	ଯଦି(context->bridge->receive_maximum != 0){
		receive_maximum.ମୂଲ୍ୟ.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	ଯଦି(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.ମୂଲ୍ୟ.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	ଯଦି(context->bridge->max_topic_alias != 0){
		topic_alias_max.ମୂଲ୍ୟ.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc2 = send__connect(context, context->keepalive, context->clean_start, properties);
	ଯଦି(rc2 == MOSQ_ERR_SUCCESS){
		ଫେରନ୍ତୁ rc;
	}ଅନ୍ୟଥା ଯଦି(rc2 == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}ଅନ୍ୟଥା{
		ଯଦି(rc2 == MOSQ_ERR_TLS){
			ଫେରନ୍ତୁ rc2; /* Error already printed */
		}ଅନ୍ୟଥା ଯଦି(rc2 == MOSQ_ERR_ERRNO){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ଅନ୍ୟଥା ଯଦି(rc2 == MOSQ_ERR_EAI){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		ଫେରନ୍ତୁ rc2;
	}
}
#endif


ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__on_connect(ସଂରଚନା mosquitto *context)
{
	ଅକ୍ଷର *notification_topic;
	size_t notification_topic_len;
	ସଂରଚନା mosquitto__bridge_topic *cur_topic;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub_opts;
	bool retain = true;
	uint8_t qos;

	ଯଦି(context->bridge->notifications){
		ଯଦି(context->max_qos == 0){
			qos = 0;
		}ଅନ୍ୟଥା{
			qos = 1;
		}
		ଯଦି(!context->retain_available){
			retain = false;
		}
		ଅକ୍ଷର notification_payload = '1';
		ଯଦି(context->bridge->notification_topic){
			ଯଦି(!context->bridge->notifications_local_only){
				ଯଦି(send__real_publish(context, mosquitto__mid_generate(context),
						context->bridge->notification_topic, 1, &notification_payload, qos, retain, 0, 0, ଖାଲି, 0)){

					ଫେରନ୍ତୁ 1;
				}
			}
			db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, ସନ୍ଦେଶ_EXPIRY_INFINITE, ଖାଲି);
		}ଅନ୍ୟଥା{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(ଆକାର(ଅକ୍ଷର)*(notification_topic_len+1));
			ଯଦି(!notification_topic){
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);
			notification_payload = '1';
			ଯଦି(!context->bridge->notifications_local_only){
				ଯଦି(send__real_publish(context, mosquitto__mid_generate(context),
						notification_topic, 1, &notification_payload, qos, retain, 0, 0, ଖାଲି, 0)){

					mosquitto_FREE(notification_topic);
					ଫେରନ୍ତୁ 1;
				}
			}
			db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, ସନ୍ଦେଶ_EXPIRY_INFINITE, ଖାଲି);
			mosquitto_FREE(notification_topic);
		}
	}

	LL_FOREACH(context->bridge->topics, cur_topic){
		ଯଦି(cur_topic->direction == bd_in || cur_topic->direction == bd_both){
			ଯଦି(cur_topic->qos > context->max_qos){
				sub_opts = context->max_qos;
			}ଅନ୍ୟଥା{
				sub_opts = cur_topic->qos;
			}
			ଯଦି(context->bridge->protocol_version == mosq_p_mqtt5){
				sub_opts = sub_opts
						| MQTT_SUB_OPT_NO_LOCAL
						| MQTT_SUB_OPT_RETAIN_AS_PUBLISHED
						| MQTT_SUB_OPT_SEND_RETAIN_ALWAYS;
			}
			ଯଦି(send__subscribe(context, ଖାଲି, 1, &cur_topic->remote_topic, sub_opts, ଖାଲି)){
				ଫେରନ୍ତୁ 1;
			}
		}ଅନ୍ୟଥା{
			ଯଦି(context->bridge->attempt_unsubscribe){
				ଯଦି(send__unsubscribe(context, ଖାଲି, 1, &cur_topic->remote_topic, ଖାଲି)){
					/* direction = inwards only. This means we should not be subscribed
					* to the topic. It is possible that we used to be subscribed to
					* this topic so unsubscribe. */
					ଫେରନ୍ତୁ 1;
				}
			}
		}
	}
	ସଂରଚନା mosquitto_subscription sub;
	memset(&sub, 0, ଆକାର(sub));
	LL_FOREACH(context->bridge->topics, cur_topic){
		sub.topic_filter = cur_topic->local_topic;
		ଯଦି(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			ଯଦି(cur_topic->qos > context->max_qos){
				sub.options = context->max_qos;
			}ଅନ୍ୟଥା{
				sub.options = cur_topic->qos;
			}
			retain__queue(context, &sub);
		}
	}

	context->bridge->connected_at = db.now_s;

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା bridge__register_local_connections(ଶୂନ୍ୟ)
{
	ସଂରଚନା mosquitto *context, *ctxt_tmp = ଖାଲି;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ଯଦି(context->bridge){
			ଯଦି(mux__new(context)){
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error in initial bridge registration: %s", strerror(errno));
				ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
			}
			mux__add_out(context);
		}
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ଶୂନ୍ୟ bridge__reload(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା j;

	// destroy old bridges that dissappeared
	ପାଇଁ(i=0; i<db.bridge_ଗଣନା; i++){
		ପାଇଁ(j=0; j<db.config->bridge_ଗଣନା; j++){
			ଯଦି(!strcmp(db.bridges[i]->bridge->name, db.config->bridges[j]->name)){
				ଭାଙ୍ଗ;
			}
		}

		ଯଦି(j==db.config->bridge_ଗଣନା){
			bridge__destroy(db.bridges[i]);
		}
	}

	ପାଇଁ(i=0; i<db.config->bridge_ଗଣନା; i++){
		ପାଇଁ(j=0; j<db.bridge_ଗଣନା; j++){
			ଯଦି(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
				ଭାଙ୍ଗ;
			}
		}

		ଯଦି(j==db.bridge_ଗଣନା){
			// a new bridge was found, create it
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ଖାଲି;
			ଜାରି;
		}

		ଯଦି(db.config->bridges[i]->reload_type == brt_immediate){
			// in this case, an existing bridge should match
			ପାଇଁ(j=0; j<db.bridge_ଗଣନା; j++){
				ଯଦି(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
					ଭାଙ୍ଗ;
				}
			}

			ପ୍ରତିଜ୍ଞା(j<db.bridge_ଗଣନା);
			db.bridges[j]->will_delay_interval = 0;
			bridge__destroy(db.bridges[j]);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ଖାଲି;
		}
	}
}


ଶୂନ୍ୟ bridge__db_cleanup(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ପାଇଁ(i=0; i<db.bridge_ଗଣନା; i++){
		ଯଦି(db.bridges[i]){
			context__cleanup(db.bridges[i], true);
		}
	}
	mosquitto_FREE(db.bridges);
}


ଶୂନ୍ୟ bridge__cleanup(ସଂରଚନା mosquitto *context)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ପ୍ରତିଜ୍ଞା(db.bridge_ଗଣନା > 0);

	ପାଇଁ(i=0; i<db.bridge_ଗଣନା; i++){
		ଯଦି(db.bridges[i] == context){
			db.bridges[i] = db.bridges[db.bridge_ଗଣନା-1];
			ଭାଙ୍ଗ;
		}
	}

	db.bridge_ଗଣନା--;
	ଯଦି(db.bridge_ଗଣନା == 0){
		mosquitto_FREE(db.bridges);
	}ଅନ୍ୟଥା{
		db.bridges = mosquitto_realloc(db.bridges, (unsigned)db.bridge_ଗଣନା * ଆକାର(db.bridges[0]));
	}

	mosquitto_FREE(context->bridge->name);
	mosquitto_FREE(context->bridge->local_clientid);
	mosquitto_FREE(context->bridge->local_username);
	mosquitto_FREE(context->bridge->local_password);
#ifdef WITH_TLS
	mosquitto_FREE(context->bridge->tls_certfile);
	mosquitto_FREE(context->bridge->tls_keyfile);
#endif

	ଯଦି(context->bridge->remote_clientid != context->id){
		mosquitto_FREE(context->bridge->remote_clientid);
	}
	context->bridge->remote_clientid = ଖାଲି;

	ଯଦି(context->bridge->remote_username != context->username){
		mosquitto_FREE(context->bridge->remote_username);
	}
	context->bridge->remote_username = ଖାଲି;

	ଯଦି(context->bridge->remote_password != context->password){
		mosquitto_FREE(context->bridge->remote_password);
	}
	context->bridge->remote_password = ଖାଲି;
#ifdef WITH_TLS
	ଯଦି(context->ssl_ctx){
		SSL_CTX_free(context->ssl_ctx);
		context->ssl_ctx = ଖାଲି;
	}
#endif

	ପାଇଁ(i=0; i<context->bridge->address_ଗଣନା; i++){
		mosquitto_FREE(context->bridge->addresses[i].address);
	}

	mosquitto_FREE(context->bridge->addresses);

	config__bridge_cleanup(context->bridge);
	context->bridge = ଖାଲି;
}


ସ୍ଥିର ଶୂନ୍ୟ bridge__packet_cleanup(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା mosquitto__packet *packet;
	ଯଦି(!context){
		ଫେରନ୍ତୁ;
	}

	ଯେତେବେଳେ(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_FREE(packet);
	}
	context->out_packet = ଖାଲି;
	context->out_packet_last = ଖାଲି;
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_ଗଣନା);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_ଗଣନା = 0;
	context->out_packet_bytes = 0;

	packet__cleanup(&(context->in_packet));
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rand_between(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ତଳ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଉପର)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;
	mosquitto_getrandom(&r, ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
	ଫେରନ୍ତୁ (abs(r) % (ଉପର - ତଳ)) + ତଳ;
}


ସ୍ଥିର ଶୂନ୍ୟ bridge__backoff_step(ସଂରଚନା mosquitto__bridge *bridge)
{
	/*
		“Decorrelated Jitter” calculation, according to:
			https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
	*/

	bridge->restart_timeout = rand_between(bridge->backoff_base, bridge->restart_timeout * 3);
	ଯଦି(bridge->restart_timeout > bridge->backoff_cap){
		bridge->restart_timeout = bridge->backoff_cap;
	}
}


ସ୍ଥିର ଶୂନ୍ୟ bridge__backoff_reset(ସଂରଚନା mosquitto__bridge *bridge)
{
	bridge->restart_timeout = bridge->backoff_base;
}


ସ୍ଥିର ଶୂନ୍ୟ bridge__update_backoff(ସଂରଚନା mosquitto__bridge *bridge)
{
	ଯଦି(!bridge){
		ଫେରନ୍ତୁ;
	}
	ଯଦି(!bridge->backoff_cap){
		/* skip if not using jitter */
		ଫେରନ୍ତୁ;

	}
	ଯଦି(bridge->connected_at && db.now_s - bridge->connected_at >= bridge->stable_connection_period){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Bridge %s connection was stable enough, resetting backoff", bridge->name);
		bridge__backoff_reset(bridge);
	}ଅନ୍ୟଥା{
		bridge__backoff_step(bridge);
	}

	bridge->connected_at = 0;

	log__printf(ଖାଲି, MOSQ_LOG_INFO, "Bridge %s next backoff will be %d ms", bridge->name, bridge->restart_timeout);
}


ସ୍ଥିର ଶୂନ୍ୟ bridge_check_pending(ସଂରଚନା mosquitto *context)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;
	socklen_t len;

	ଯଦି(context->state == mosq_cs_connect_pending){
		len = ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
		ଯଦି(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (ଅକ୍ଷର *)&err, &len)){
			ଯଦି(err == 0){
				mosquitto__set_state(context, mosq_cs_new);
#ଯଦି defined(WITH_ADNS) && defined(WITH_BRIDGE)
				ଯଦି(context->bridge){
					bridge__connect_step3(context);
				}
#endif
			}ଅନ୍ୟଥା ଯଦି(err == ECONNREFUSED){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				ଫେରନ୍ତୁ;
			}
		}ଅନ୍ୟଥା{
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			ଫେରନ୍ତୁ;
		}
	}
}


ସ୍ଥିର bool reload_if_needed(ସଂରଚନା mosquitto *context)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ପାଇଁ(i=0; i<db.config->bridge_ଗଣନା; i++){
		ଯଦି(db.config->bridges[i] && !strcmp(context->bridge->name, db.config->bridges[i]->name)){
			bridge__destroy(context);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ଖାଲି;
			loop__update_next_event(100);
			ଫେରନ୍ତୁ true;
		}
	}

	ଫେରନ୍ତୁ false;
}


ଶୂନ୍ୟ bridge_check(ଶୂନ୍ୟ)
{
	ସ୍ଥିର time_t last_check = 0;
	ସଂରଚନା mosquitto *context = ଖାଲି;
	socklen_t len;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

	ଯଦି(db.now_s <= last_check){
		ଫେରନ୍ତୁ;
	}

	ପାଇଁ(i=0; i<db.bridge_ଗଣନା; i++){
		ଯଦି(!db.bridges[i]){
			ଜାରି;
		}

		context = db.bridges[i];

		ଯଦି(net__is_connected(context)){
			mosquitto__check_keepalive(context);
			bridge_check_pending(context);

			/* Check for bridges that are not round robin and not currently
			 * connected to their primary broker. */
			ଯଦି(context->bridge->round_robin == false
					&& context->bridge->cur_address != 0
					&& context->bridge->primary_retry
					&& db.now_s >= context->bridge->primary_retry){

				ଯଦି(context->bridge->primary_retry_sock == INVALID_SOCKET){
					rc = net__try_connect(context->bridge->addresses[0].address,
							context->bridge->addresses[0].port,
							&context->bridge->primary_retry_sock,
							context->bridge->bind_address, false);

					ଯଦି(rc == 0){
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = 0;
						mux__delete(context);
						net__socket_close(context);
						context->bridge->cur_address = 0;
					}
				}ଅନ୍ୟଥା{
					len = ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
					ଯଦି(!getsockopt(context->bridge->primary_retry_sock, SOL_SOCKET, SO_ERROR, (ଅକ୍ଷର *)&err, &len)){
						ଯଦି(err == 0){
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = 0;
							mux__delete(context);
							net__socket_close(context);
							context->bridge->cur_address = context->bridge->address_ଗଣନା-1;
						}ଅନ୍ୟଥା{
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = db.now_s+5;
							loop__update_next_event(5000);
						}
					}ଅନ୍ୟଥା{
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = db.now_s+5;
						loop__update_next_event(5000);
					}
				}
			}
		}ଅନ୍ୟଥା{
			ଯଦି(reload_if_needed(context)){
				ଜାରି;
			}

			/* Want to try to restart the bridge connection */
			ଯଦି(!context->bridge->restart_t){
				bridge__update_backoff(context->bridge);
				context->bridge->restart_t = 1000*db.now_s+context->bridge->restart_timeout;
				context->bridge->cur_address++;
				ଯଦି(context->bridge->cur_address == context->bridge->address_ଗଣନା){
					context->bridge->cur_address = 0;
				}
				loop__update_next_event(context->bridge->restart_timeout);
			}ଅନ୍ୟଥା{
				ଯଦି((context->bridge->start_type == bst_lazy && context->bridge->lazy_reconnect)
						|| (context->bridge->start_type == bst_automatic && 1000*db.now_s >= context->bridge->restart_t)){

#ଯଦି defined(__GLIBC__) && defined(WITH_ADNS)
					ଯଦି(context->adns){
						/* Connection attempted, waiting on DNS lookup */
						rc = gai_error(context->adns);
						ଯଦି(rc == EAI_INPROGRESS){
							/* Just keep on waiting */
						}ଅନ୍ୟଥା ଯଦି(rc == 0){
							rc = bridge__connect_step2(context);
							ଯଦି(rc == MOSQ_ERR_SUCCESS){
								mux__new(context);
								ଯଦି(context->out_packet){
									mux__add_out(context);
								}
							}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_CONN_PENDING){
								mux__new(context);
								mux__add_out(context);
								context->bridge->restart_t = 0;
							}ଅନ୍ୟଥା{
								context->bridge->cur_address++;
								ଯଦି(context->bridge->cur_address == context->bridge->address_ଗଣନା){
									context->bridge->cur_address = 0;
								}
								context->bridge->restart_t = 0;
							}
						}ଅନ୍ୟଥା{
							/* Need to retry */
							ଯଦି(context->adns->ar_result){
								freeaddrinfo(context->adns->ar_result);
							}
							mosquitto_FREE(context->adns);
							context->bridge->restart_t = 0;
						}
					}ଅନ୍ୟଥା{
						rc = bridge__connect_step1(context);
						ଯଦି(rc){
							context->bridge->cur_address++;
							ଯଦି(context->bridge->cur_address == context->bridge->address_ଗଣନା){
								context->bridge->cur_address = 0;
							}
						}ଅନ୍ୟଥା{
							/* Short wait for ADNS lookup */
							context->bridge->restart_t = 1;
							loop__update_next_event(1000);
						}
					}
#ଅନ୍ୟଥା
					{
						rc = bridge__connect(context);
						context->bridge->restart_t = 0;
						ଯଦି(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING){
							ଯଦି(context->bridge->round_robin == false && context->bridge->cur_address != 0){
								context->bridge->primary_retry = db.now_s + 5;
								loop__update_next_event(5000);
							}
							mux__new(context);
							ଯଦି(context->out_packet){
								mux__add_out(context);
							}
						}ଅନ୍ୟଥା{
							context->bridge->cur_address++;
							ଯଦି(context->bridge->cur_address == context->bridge->address_ଗଣନା){
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
