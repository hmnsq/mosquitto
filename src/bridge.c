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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <errno.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#ifndef WIN32
#અન્તર્ભાવ <netdb.h>
#અન્તર્ભાવ <sys/socket.h>
#અન્તર્ભાવ <netinet/in.h>
#અન્તર્ભાવ <netinet/tcp.h>
#નહીંતર
#અન્તર્ભાવ <winsock2.h>
#અન્તર્ભાવ <ws2tcpip.h>
#endif

#ifndef WIN32
#અન્તર્ભાવ <unistd.h>
#નહીંતર
#અન્તર્ભાવ <process.h>
#અન્તર્ભાવ <winsock2.h>
#અન્તર્ભાવ <ws2tcpip.h>
#endif

#અન્તર્ભાવ "mosquitto.h"
#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto_internal.h"
#અન્તર્ભાવ "net_mosq.h"
#અન્તર્ભાવ "packet_mosq.h"
#અન્તર્ભાવ "property_common.h"
#અન્તર્ભાવ "send_mosq.h"
#અન્તર્ભાવ "sys_tree.h"
#અન્તર્ભાવ "tls_mosq.h"
#અન્તર્ભાવ "util_mosq.h"
#અન્તર્ભાવ "will_mosq.h"
#અન્તર્ભાવ "utlist.h"

#ifdef WITH_BRIDGE

સ્થિર શૂન્ય bridge__update_backoff(સંરચના mosquitto__bridge *bridge);
#જો defined(__GLIBC__) && defined(WITH_ADNS)
સ્થિર પૂર્ણાંક bridge__connect_step1(સંરચના mosquitto *context);
સ્થિર પૂર્ણાંક bridge__connect_step2(સંરચના mosquitto *context);
#endif
સ્થિર શૂન્ય bridge__packet_cleanup(સંરચના mosquitto *context);

સ્થિર સંરચના mosquitto *bridge__new(સંરચના mosquitto__bridge *bridge)
{
	સંરચના mosquitto *new_context = ખાલી;
	સંરચના mosquitto **bridges;
	અક્ષર *local_id;

	પ્રતિજ્ઞા(bridge);

	local_id = mosquitto_strdup(bridge->local_clientid);
	જો(!local_id){
		પાછા ખાલી;
	}

	HASH_FIND(hh_id, db.contexts_by_id, local_id, strlen(local_id), new_context);
	જો(new_context){
		/* (possible from persistent db) */
		mosquitto_FREE(local_id);
	}નહીંતર{
		/* id wasn't found, so generate a new context */
		new_context = context__init();
		જો(!new_context){
			mosquitto_FREE(local_id);
			પાછા ખાલી;
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
	new_context->tls_engine = ખાલી;
	new_context->tls_keyform = mosq_k_pem;
	new_context->tls_use_os_certs = bridge->tls_use_os_certs;
	new_context->ssl_ctx_defaults = true;
#ifdef FINAL_WITH_TLS_PSK
	new_context->tls_psk_identity = bridge->tls_psk_identity;
	new_context->tls_psk = bridge->tls_psk;
#endif
#endif

	bridge->try_private_accepted = true;
	જો(bridge->clean_start_local == -1){
		/* default to "regular" clean start setting */
		bridge->clean_start_local = bridge->clean_start;
	}
	new_context->retain_available = bridge->outgoing_retain;
	new_context->protocol = bridge->protocol_version;
	જો(!bridge->clean_start_local){
		new_context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
		plugin_persist__handle_client_add(new_context);
		જો(new_context->expiry_list_item){
			/* We've restored from persistence and been added to the session
			 * expiry list, even though we should never be expired */
			session_expiry__remove(new_context);
		}
	}

	bridges = mosquitto_realloc(db.bridges, (size_t)(db.bridge_ગણતરી+1)*કદ(સંરચના mosquitto *));
	જો(bridges){
		db.bridges = bridges;
		db.bridge_ગણતરી++;
		db.bridges[db.bridge_ગણતરી-1] = new_context;
	}નહીંતર{
		પાછા ખાલી;
	}

	પાછા new_context;
}


સ્થિર શૂન્ય bridge__destroy(સંરચના mosquitto *context)
{
	send__disconnect(context, MQTT_RC_SUCCESS, ખાલી);
	context__cleanup(context, true);
}


શૂન્ય bridge__start_all(શૂન્ય)
{
	માટે(પૂર્ણાંક i=0; i<db.config->bridge_ગણતરી; i++){
		સંરચના mosquitto *context;
		પૂર્ણાંક ret;

		context = bridge__new(db.config->bridges[i]);
		જો(!context){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
			પાછા;
		}
		પ્રતિજ્ઞા(context);

#જો defined(__GLIBC__) && defined(WITH_ADNS)
		context->bridge->restart_t = 1; /* force quick restart of bridge */
		loop__update_next_event(1000);
		ret = bridge__connect_step1(context);
#નહીંતર
		ret = bridge__connect(context);
#endif

		જો(ret && ret != MOSQ_ERR_CONN_PENDING){
			log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Unable to connect bridge %s.",
					context->bridge->name);
		}

		db.config->bridges[i] = ખાલી;
	}
}


સ્થિર પૂર્ણાંક bridge__set_tcp_keepalive(સંરચના mosquitto *context)
{
	unsigned પૂર્ણાંક enabled = 1;
	bool ret;

	જો(context->bridge->tcp_keepalive_idle == 0
			|| context->bridge->tcp_keepalive_interval == 0
			|| context->bridge->tcp_keepalive_counter == 0){

		પાછા MOSQ_ERR_SUCCESS;
	}

#ifdef WIN32
#  define SETSOCKOPT_TYPE અક્ષર *
#નહીંતર
#  define SETSOCKOPT_TYPE અચલ શૂન્ય *
#endif

	ret = setsockopt(context->sock, SOL_SOCKET, SO_KEEPALIVE, (SETSOCKOPT_TYPE)&enabled, કદ(enabled));

#ifdef TCP_KEEPIDLE
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPIDLE,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_idle, કદ(context->bridge->tcp_keepalive_idle));
#endif
#ifdef TCP_KEEPINTVL
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPINTVL,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_interval, કદ(context->bridge->tcp_keepalive_interval));
#endif
#ifdef TCP_KEEPCNT
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPCNT,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_counter, કદ(context->bridge->tcp_keepalive_counter));
#endif

#undef SETSOCKOPT_TYPE

	જો(ret){
		પાછા MOSQ_ERR_UNKNOWN;
	}

	પાછા MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TCP_USER_TIMEOUT


સ્થિર પૂર્ણાંક bridge__set_tcp_user_timeout(સંરચના mosquitto *context)
{
	પૂર્ણાંક timeout = context->bridge->tcp_user_timeout;
	જો(timeout >= 0){
		જો(setsockopt(context->sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (અક્ષર *)&timeout, કદ(timeout))){
			પાછા MOSQ_ERR_UNKNOWN;
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}
#endif

#જો defined(__GLIBC__) && defined(WITH_ADNS)


સ્થિર પૂર્ણાંક bridge__connect_step1(સંરચના mosquitto *context)
{
	પૂર્ણાંક rc;
	અક્ષર *notification_topic;
	size_t notification_topic_len;
	uint8_t notification_payload;
	સંરચના mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	જો(!context || !context->bridge){
		પાછા MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_સંદેશ_in = db.now_s;
	context->next_સંદેશ_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = ખાલી;
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
		જો(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(ખાલી, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			જો(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}નહીંતર{
				qos = cur_topic->qos;
			}
			સંરચના mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			જો(sub__add(context, &sub) > 0){
				પાછા 1;
			}
			retain__queue(context, &sub);
		}
	}

	જો(context->bridge->notifications){
		જો(context->max_qos == 0){
			qos = 0;
		}નહીંતર{
			qos = 1;
		}
		જો(context->bridge->notification_topic){
			જો(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, સંદેશ_EXPIRY_INFINITE, ખાલી);
				context->bridge->initial_notification_done = true;
			}
			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, ખાલી);
			જો(rc != MOSQ_ERR_SUCCESS){
				પાછા rc;
			}
		}નહીંતર{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(કદ(અક્ષર)*(notification_topic_len+1));
			જો(!notification_topic){
				પાછા MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			જો(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, સંદેશ_EXPIRY_INFINITE, ખાલી);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, ખાલી);
			mosquitto_FREE(notification_topic);
			જો(rc != MOSQ_ERR_SUCCESS){
				પાછા rc;
			}
		}
	}

	log__printf(ખાલી, MOSQ_LOG_NOTICE, "Connecting bridge (step 1) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step1(context, context->bridge->addresses[context->bridge->cur_address].address);
	જો(rc > 0){
		જો(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			પાછા rc; /* Error already printed */
		}નહીંતર જો(rc == MOSQ_ERR_ERRNO){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}નહીંતર જો(rc == MOSQ_ERR_EAI){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		પાછા rc;
	}

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક bridge__connect_step2(સંરચના mosquitto *context)
{
	પૂર્ણાંક rc;

	જો(!context || !context->bridge){
		પાછા MOSQ_ERR_INVAL;
	}

	log__printf(ખાલી, MOSQ_LOG_NOTICE, "Connecting bridge (step 2) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step2(context, context->bridge->addresses[context->bridge->cur_address].port, &context->sock);
	જો(rc > 0){
		જો(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			પાછા rc; /* Error already printed */
		}નહીંતર જો(rc == MOSQ_ERR_ERRNO){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}નહીંતર જો(rc == MOSQ_ERR_EAI){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		પાછા rc;
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, કદ(context->sock), context);

	જો(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}
	પાછા rc;
}


પૂર્ણાંક bridge__connect_step3(સંરચના mosquitto *context)
{
	પૂર્ણાંક rc;
	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = ખાલી;

	rc = net__socket_connect_step3(context, context->bridge->addresses[context->bridge->cur_address].address);
	જો(rc > 0){
		જો(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			પાછા rc; /* Error already printed */
		}નહીંતર જો(rc == MOSQ_ERR_ERRNO){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}નહીંતર જો(rc == MOSQ_ERR_EAI){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		પાછા rc;
	}

	જો(context->bridge->round_robin == false && context->bridge->cur_address != 0){
		context->bridge->primary_retry = db.now_s + 5;
		loop__update_next_event(5000);
	}

	જો(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		પાછા MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	જો(bridge__set_tcp_user_timeout(context)){
		પાછા MOSQ_ERR_UNKNOWN;
	}
#endif

	જો(context->bridge->receive_maximum != 0){
		receive_maximum.કિંમત.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	જો(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.કિંમત.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	જો(context->bridge->max_topic_alias != 0){
		topic_alias_max.કિંમત.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc = send__connect(context, context->keepalive, context->clean_start, properties);
	જો(rc == MOSQ_ERR_SUCCESS){
		પાછા MOSQ_ERR_SUCCESS;
	}નહીંતર જો(rc == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		પાછા MOSQ_ERR_SUCCESS;
	}નહીંતર{
		જો(rc == MOSQ_ERR_TLS){
			પાછા rc; /* Error already printed */
		}નહીંતર જો(rc == MOSQ_ERR_ERRNO){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}નહીંતર જો(rc == MOSQ_ERR_EAI){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		પાછા rc;
	}
}
#નહીંતર


પૂર્ણાંક bridge__connect(સંરચના mosquitto *context)
{
	પૂર્ણાંક rc, rc2;
	અક્ષર *notification_topic = ખાલી;
	size_t notification_topic_len;
	uint8_t notification_payload;
	સંરચના mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = ખાલી;

	જો(!context || !context->bridge){
		પાછા MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->last_સંદેશ_in = db.now_s;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->next_સંદેશ_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = ખાલી;
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
		જો(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(ખાલી, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			જો(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}નહીંતર{
				qos = cur_topic->qos;
			}
			સંરચના mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			જો(sub__add(context, &sub) > 0){
				પાછા 1;
			}
		}
	}

	જો(context->bridge->notifications){
		જો(context->max_qos == 0){
			qos = 0;
		}નહીંતર{
			qos = 1;
		}
		જો(context->bridge->notification_topic){
			જો(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, સંદેશ_EXPIRY_INFINITE, ખાલી);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, ખાલી);
			જો(rc != MOSQ_ERR_SUCCESS){
				પાછા rc;
			}
		}નહીંતર{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(કદ(અક્ષર)*(notification_topic_len+1));
			જો(!notification_topic){
				પાછા MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			જો(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, સંદેશ_EXPIRY_INFINITE, ખાલી);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, ખાલી);
			જો(rc != MOSQ_ERR_SUCCESS){
				mosquitto_FREE(notification_topic);
				પાછા rc;
			}
			mosquitto_FREE(notification_topic);
		}
	}

	log__printf(ખાલી, MOSQ_LOG_NOTICE, "Connecting bridge %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__socket_connect(context,
			context->bridge->addresses[context->bridge->cur_address].address,
			context->bridge->addresses[context->bridge->cur_address].port,
			context->bridge->bind_address,
			false);

	જો(rc > 0){
		જો(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			પાછા rc; /* Error already printed */
		}નહીંતર જો(rc == MOSQ_ERR_ERRNO){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}નહીંતર જો(rc == MOSQ_ERR_EAI){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		પાછા rc;
	}નહીંતર જો(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, કદ(context->sock), context);

	જો(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		પાછા MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	જો(bridge__set_tcp_user_timeout(context)){
		પાછા MOSQ_ERR_UNKNOWN;
	}
#endif

	જો(context->bridge->receive_maximum != 0){
		receive_maximum.કિંમત.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	જો(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.કિંમત.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	જો(context->bridge->max_topic_alias != 0){
		topic_alias_max.કિંમત.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc2 = send__connect(context, context->keepalive, context->clean_start, properties);
	જો(rc2 == MOSQ_ERR_SUCCESS){
		પાછા rc;
	}નહીંતર જો(rc2 == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		પાછા MOSQ_ERR_SUCCESS;
	}નહીંતર{
		જો(rc2 == MOSQ_ERR_TLS){
			પાછા rc2; /* Error already printed */
		}નહીંતર જો(rc2 == MOSQ_ERR_ERRNO){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}નહીંતર જો(rc2 == MOSQ_ERR_EAI){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		પાછા rc2;
	}
}
#endif


પૂર્ણાંક bridge__on_connect(સંરચના mosquitto *context)
{
	અક્ષર *notification_topic;
	size_t notification_topic_len;
	સંરચના mosquitto__bridge_topic *cur_topic;
	પૂર્ણાંક sub_opts;
	bool retain = true;
	uint8_t qos;

	જો(context->bridge->notifications){
		જો(context->max_qos == 0){
			qos = 0;
		}નહીંતર{
			qos = 1;
		}
		જો(!context->retain_available){
			retain = false;
		}
		અક્ષર notification_payload = '1';
		જો(context->bridge->notification_topic){
			જો(!context->bridge->notifications_local_only){
				જો(send__real_publish(context, mosquitto__mid_generate(context),
						context->bridge->notification_topic, 1, &notification_payload, qos, retain, 0, 0, ખાલી, 0)){

					પાછા 1;
				}
			}
			db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, સંદેશ_EXPIRY_INFINITE, ખાલી);
		}નહીંતર{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(કદ(અક્ષર)*(notification_topic_len+1));
			જો(!notification_topic){
				પાછા MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);
			notification_payload = '1';
			જો(!context->bridge->notifications_local_only){
				જો(send__real_publish(context, mosquitto__mid_generate(context),
						notification_topic, 1, &notification_payload, qos, retain, 0, 0, ખાલી, 0)){

					mosquitto_FREE(notification_topic);
					પાછા 1;
				}
			}
			db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, સંદેશ_EXPIRY_INFINITE, ખાલી);
			mosquitto_FREE(notification_topic);
		}
	}

	LL_FOREACH(context->bridge->topics, cur_topic){
		જો(cur_topic->direction == bd_in || cur_topic->direction == bd_both){
			જો(cur_topic->qos > context->max_qos){
				sub_opts = context->max_qos;
			}નહીંતર{
				sub_opts = cur_topic->qos;
			}
			જો(context->bridge->protocol_version == mosq_p_mqtt5){
				sub_opts = sub_opts
						| MQTT_SUB_OPT_NO_LOCAL
						| MQTT_SUB_OPT_RETAIN_AS_PUBLISHED
						| MQTT_SUB_OPT_SEND_RETAIN_ALWAYS;
			}
			જો(send__subscribe(context, ખાલી, 1, &cur_topic->remote_topic, sub_opts, ખાલી)){
				પાછા 1;
			}
		}નહીંતર{
			જો(context->bridge->attempt_unsubscribe){
				જો(send__unsubscribe(context, ખાલી, 1, &cur_topic->remote_topic, ખાલી)){
					/* direction = inwards only. This means we should not be subscribed
					* to the topic. It is possible that we used to be subscribed to
					* this topic so unsubscribe. */
					પાછા 1;
				}
			}
		}
	}
	સંરચના mosquitto_subscription sub;
	memset(&sub, 0, કદ(sub));
	LL_FOREACH(context->bridge->topics, cur_topic){
		sub.topic_filter = cur_topic->local_topic;
		જો(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			જો(cur_topic->qos > context->max_qos){
				sub.options = context->max_qos;
			}નહીંતર{
				sub.options = cur_topic->qos;
			}
			retain__queue(context, &sub);
		}
	}

	context->bridge->connected_at = db.now_s;

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક bridge__register_local_connections(શૂન્ય)
{
	સંરચના mosquitto *context, *ctxt_tmp = ખાલી;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		જો(context->bridge){
			જો(mux__new(context)){
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error in initial bridge registration: %s", strerror(errno));
				પાછા MOSQ_ERR_UNKNOWN;
			}
			mux__add_out(context);
		}
	}
	પાછા MOSQ_ERR_SUCCESS;
}


શૂન્ય bridge__reload(શૂન્ય)
{
	પૂર્ણાંક i;
	પૂર્ણાંક j;

	// destroy old bridges that dissappeared
	માટે(i=0; i<db.bridge_ગણતરી; i++){
		માટે(j=0; j<db.config->bridge_ગણતરી; j++){
			જો(!strcmp(db.bridges[i]->bridge->name, db.config->bridges[j]->name)){
				તોડો;
			}
		}

		જો(j==db.config->bridge_ગણતરી){
			bridge__destroy(db.bridges[i]);
		}
	}

	માટે(i=0; i<db.config->bridge_ગણતરી; i++){
		માટે(j=0; j<db.bridge_ગણતરી; j++){
			જો(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
				તોડો;
			}
		}

		જો(j==db.bridge_ગણતરી){
			// a new bridge was found, create it
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ખાલી;
			ચાલુ;
		}

		જો(db.config->bridges[i]->reload_type == brt_immediate){
			// in this case, an existing bridge should match
			માટે(j=0; j<db.bridge_ગણતરી; j++){
				જો(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
					તોડો;
				}
			}

			પ્રતિજ્ઞા(j<db.bridge_ગણતરી);
			db.bridges[j]->will_delay_interval = 0;
			bridge__destroy(db.bridges[j]);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ખાલી;
		}
	}
}


શૂન્ય bridge__db_cleanup(શૂન્ય)
{
	પૂર્ણાંક i;

	માટે(i=0; i<db.bridge_ગણતરી; i++){
		જો(db.bridges[i]){
			context__cleanup(db.bridges[i], true);
		}
	}
	mosquitto_FREE(db.bridges);
}


શૂન્ય bridge__cleanup(સંરચના mosquitto *context)
{
	પૂર્ણાંક i;

	પ્રતિજ્ઞા(db.bridge_ગણતરી > 0);

	માટે(i=0; i<db.bridge_ગણતરી; i++){
		જો(db.bridges[i] == context){
			db.bridges[i] = db.bridges[db.bridge_ગણતરી-1];
			તોડો;
		}
	}

	db.bridge_ગણતરી--;
	જો(db.bridge_ગણતરી == 0){
		mosquitto_FREE(db.bridges);
	}નહીંતર{
		db.bridges = mosquitto_realloc(db.bridges, (unsigned)db.bridge_ગણતરી * કદ(db.bridges[0]));
	}

	mosquitto_FREE(context->bridge->name);
	mosquitto_FREE(context->bridge->local_clientid);
	mosquitto_FREE(context->bridge->local_username);
	mosquitto_FREE(context->bridge->local_password);
#ifdef WITH_TLS
	mosquitto_FREE(context->bridge->tls_certfile);
	mosquitto_FREE(context->bridge->tls_keyfile);
#endif

	જો(context->bridge->remote_clientid != context->id){
		mosquitto_FREE(context->bridge->remote_clientid);
	}
	context->bridge->remote_clientid = ખાલી;

	જો(context->bridge->remote_username != context->username){
		mosquitto_FREE(context->bridge->remote_username);
	}
	context->bridge->remote_username = ખાલી;

	જો(context->bridge->remote_password != context->password){
		mosquitto_FREE(context->bridge->remote_password);
	}
	context->bridge->remote_password = ખાલી;
#ifdef WITH_TLS
	જો(context->ssl_ctx){
		SSL_CTX_free(context->ssl_ctx);
		context->ssl_ctx = ખાલી;
	}
#endif

	માટે(i=0; i<context->bridge->address_ગણતરી; i++){
		mosquitto_FREE(context->bridge->addresses[i].address);
	}

	mosquitto_FREE(context->bridge->addresses);

	config__bridge_cleanup(context->bridge);
	context->bridge = ખાલી;
}


સ્થિર શૂન્ય bridge__packet_cleanup(સંરચના mosquitto *context)
{
	સંરચના mosquitto__packet *packet;
	જો(!context){
		પાછા;
	}

	જ્યારે(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_FREE(packet);
	}
	context->out_packet = ખાલી;
	context->out_packet_last = ખાલી;
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_ગણતરી);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_ગણતરી = 0;
	context->out_packet_bytes = 0;

	packet__cleanup(&(context->in_packet));
}


સ્થિર પૂર્ણાંક rand_between(પૂર્ણાંક નીચલું, પૂર્ણાંક ઉપલું)
{
	પૂર્ણાંક r;
	mosquitto_getrandom(&r, કદ(પૂર્ણાંક));
	પાછા (abs(r) % (ઉપલું - નીચલું)) + નીચલું;
}


સ્થિર શૂન્ય bridge__backoff_step(સંરચના mosquitto__bridge *bridge)
{
	/*
		“Decorrelated Jitter” calculation, according to:
			https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
	*/

	bridge->restart_timeout = rand_between(bridge->backoff_base, bridge->restart_timeout * 3);
	જો(bridge->restart_timeout > bridge->backoff_cap){
		bridge->restart_timeout = bridge->backoff_cap;
	}
}


સ્થિર શૂન્ય bridge__backoff_reset(સંરચના mosquitto__bridge *bridge)
{
	bridge->restart_timeout = bridge->backoff_base;
}


સ્થિર શૂન્ય bridge__update_backoff(સંરચના mosquitto__bridge *bridge)
{
	જો(!bridge){
		પાછા;
	}
	જો(!bridge->backoff_cap){
		/* skip if not using jitter */
		પાછા;

	}
	જો(bridge->connected_at && db.now_s - bridge->connected_at >= bridge->stable_connection_period){
		log__printf(ખાલી, MOSQ_LOG_INFO, "Bridge %s connection was stable enough, resetting backoff", bridge->name);
		bridge__backoff_reset(bridge);
	}નહીંતર{
		bridge__backoff_step(bridge);
	}

	bridge->connected_at = 0;

	log__printf(ખાલી, MOSQ_LOG_INFO, "Bridge %s next backoff will be %d ms", bridge->name, bridge->restart_timeout);
}


સ્થિર શૂન્ય bridge_check_pending(સંરચના mosquitto *context)
{
	પૂર્ણાંક err;
	socklen_t len;

	જો(context->state == mosq_cs_connect_pending){
		len = કદ(પૂર્ણાંક);
		જો(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (અક્ષર *)&err, &len)){
			જો(err == 0){
				mosquitto__set_state(context, mosq_cs_new);
#જો defined(WITH_ADNS) && defined(WITH_BRIDGE)
				જો(context->bridge){
					bridge__connect_step3(context);
				}
#endif
			}નહીંતર જો(err == ECONNREFUSED){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				પાછા;
			}
		}નહીંતર{
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			પાછા;
		}
	}
}


સ્થિર bool reload_if_needed(સંરચના mosquitto *context)
{
	પૂર્ણાંક i;

	માટે(i=0; i<db.config->bridge_ગણતરી; i++){
		જો(db.config->bridges[i] && !strcmp(context->bridge->name, db.config->bridges[i]->name)){
			bridge__destroy(context);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ખાલી;
			loop__update_next_event(100);
			પાછા true;
		}
	}

	પાછા false;
}


શૂન્ય bridge_check(શૂન્ય)
{
	સ્થિર time_t last_check = 0;
	સંરચના mosquitto *context = ખાલી;
	socklen_t len;
	પૂર્ણાંક i;
	પૂર્ણાંક rc;
	પૂર્ણાંક err;

	જો(db.now_s <= last_check){
		પાછા;
	}

	માટે(i=0; i<db.bridge_ગણતરી; i++){
		જો(!db.bridges[i]){
			ચાલુ;
		}

		context = db.bridges[i];

		જો(net__is_connected(context)){
			mosquitto__check_keepalive(context);
			bridge_check_pending(context);

			/* Check for bridges that are not round robin and not currently
			 * connected to their primary broker. */
			જો(context->bridge->round_robin == false
					&& context->bridge->cur_address != 0
					&& context->bridge->primary_retry
					&& db.now_s >= context->bridge->primary_retry){

				જો(context->bridge->primary_retry_sock == INVALID_SOCKET){
					rc = net__try_connect(context->bridge->addresses[0].address,
							context->bridge->addresses[0].port,
							&context->bridge->primary_retry_sock,
							context->bridge->bind_address, false);

					જો(rc == 0){
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = 0;
						mux__delete(context);
						net__socket_close(context);
						context->bridge->cur_address = 0;
					}
				}નહીંતર{
					len = કદ(પૂર્ણાંક);
					જો(!getsockopt(context->bridge->primary_retry_sock, SOL_SOCKET, SO_ERROR, (અક્ષર *)&err, &len)){
						જો(err == 0){
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = 0;
							mux__delete(context);
							net__socket_close(context);
							context->bridge->cur_address = context->bridge->address_ગણતરી-1;
						}નહીંતર{
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = db.now_s+5;
							loop__update_next_event(5000);
						}
					}નહીંતર{
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = db.now_s+5;
						loop__update_next_event(5000);
					}
				}
			}
		}નહીંતર{
			જો(reload_if_needed(context)){
				ચાલુ;
			}

			/* Want to try to restart the bridge connection */
			જો(!context->bridge->restart_t){
				bridge__update_backoff(context->bridge);
				context->bridge->restart_t = 1000*db.now_s+context->bridge->restart_timeout;
				context->bridge->cur_address++;
				જો(context->bridge->cur_address == context->bridge->address_ગણતરી){
					context->bridge->cur_address = 0;
				}
				loop__update_next_event(context->bridge->restart_timeout);
			}નહીંતર{
				જો((context->bridge->start_type == bst_lazy && context->bridge->lazy_reconnect)
						|| (context->bridge->start_type == bst_automatic && 1000*db.now_s >= context->bridge->restart_t)){

#જો defined(__GLIBC__) && defined(WITH_ADNS)
					જો(context->adns){
						/* Connection attempted, waiting on DNS lookup */
						rc = gai_error(context->adns);
						જો(rc == EAI_INPROGRESS){
							/* Just keep on waiting */
						}નહીંતર જો(rc == 0){
							rc = bridge__connect_step2(context);
							જો(rc == MOSQ_ERR_SUCCESS){
								mux__new(context);
								જો(context->out_packet){
									mux__add_out(context);
								}
							}નહીંતર જો(rc == MOSQ_ERR_CONN_PENDING){
								mux__new(context);
								mux__add_out(context);
								context->bridge->restart_t = 0;
							}નહીંતર{
								context->bridge->cur_address++;
								જો(context->bridge->cur_address == context->bridge->address_ગણતરી){
									context->bridge->cur_address = 0;
								}
								context->bridge->restart_t = 0;
							}
						}નહીંતર{
							/* Need to retry */
							જો(context->adns->ar_result){
								freeaddrinfo(context->adns->ar_result);
							}
							mosquitto_FREE(context->adns);
							context->bridge->restart_t = 0;
						}
					}નહીંતર{
						rc = bridge__connect_step1(context);
						જો(rc){
							context->bridge->cur_address++;
							જો(context->bridge->cur_address == context->bridge->address_ગણતરી){
								context->bridge->cur_address = 0;
							}
						}નહીંતર{
							/* Short wait for ADNS lookup */
							context->bridge->restart_t = 1;
							loop__update_next_event(1000);
						}
					}
#નહીંતર
					{
						rc = bridge__connect(context);
						context->bridge->restart_t = 0;
						જો(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING){
							જો(context->bridge->round_robin == false && context->bridge->cur_address != 0){
								context->bridge->primary_retry = db.now_s + 5;
								loop__update_next_event(5000);
							}
							mux__new(context);
							જો(context->out_packet){
								mux__add_out(context);
							}
						}નહીંતર{
							context->bridge->cur_address++;
							જો(context->bridge->cur_address == context->bridge->address_ગણતરી){
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
