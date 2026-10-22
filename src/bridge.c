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

#అన్తర్భావ "config.h"

#అన్తర్భావ <assert.h>
#అన్తర్భావ <errno.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#నిర్వచితం_కాకపోతే WIN32
#అన్తర్భావ <netdb.h>
#అన్తర్భావ <sys/socket.h>
#అన్తర్భావ <netinet/in.h>
#అన్తర్భావ <netinet/tcp.h>
#లేకపోతే
#అన్తర్భావ <winsock2.h>
#అన్తర్భావ <ws2tcpip.h>
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే WIN32
#అన్తర్భావ <unistd.h>
#లేకపోతే
#అన్తర్భావ <process.h>
#అన్తర్భావ <winsock2.h>
#అన్తర్భావ <ws2tcpip.h>
#అయితే_ముగింపు

#అన్తర్భావ "mosquitto.h"
#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto_internal.h"
#అన్తర్భావ "net_mosq.h"
#అన్తర్భావ "packet_mosq.h"
#అన్తర్భావ "property_common.h"
#అన్తర్భావ "send_mosq.h"
#అన్తర్భావ "sys_tree.h"
#అన్తర్భావ "tls_mosq.h"
#అన్తర్భావ "util_mosq.h"
#అన్తర్భావ "will_mosq.h"
#అన్తర్భావ "utlist.h"

#నిర్వచితమైతే WITH_BRIDGE

స్థిర శూన్య bridge__update_backoff(నిర్మాణం mosquitto__bridge *bridge);
#ఒకవేళ defined(__GLIBC__) && defined(WITH_ADNS)
స్థిర పూర్ణాంకం bridge__connect_step1(నిర్మాణం mosquitto *context);
స్థిర పూర్ణాంకం bridge__connect_step2(నిర్మాణం mosquitto *context);
#అయితే_ముగింపు
స్థిర శూన్య bridge__packet_cleanup(నిర్మాణం mosquitto *context);

స్థిర నిర్మాణం mosquitto *bridge__new(నిర్మాణం mosquitto__bridge *bridge)
{
	నిర్మాణం mosquitto *new_context = ఖాళీ;
	నిర్మాణం mosquitto **bridges;
	అక్షరం *local_id;

	ప్రతిజ్ఞా(bridge);

	local_id = mosquitto_strdup(bridge->local_clientid);
	ఒకవేళ(!local_id){
		తిరిగి ఖాళీ;
	}

	HASH_కనుగొను(hh_id, db.contexts_by_id, local_id, అక్షరశ్రేణి_పొడవు(local_id), new_context);
	ఒకవేళ(new_context){
		/* (possible from persistent db) */
		mosquitto_FREE(local_id);
	}లేకపోతే{
		/* id wasn't found, so generate a new context */
		new_context = context__init();
		ఒకవేళ(!new_context){
			mosquitto_FREE(local_id);
			తిరిగి ఖాళీ;
		}
		new_context->id = local_id;
		context_కూడు_గమ్యం_by_id(new_context);
	}
	new_context->transport = mosq_t_tcp;
	new_context->bridge = bridge;
	new_context->is_bridge = true;

	new_context->username = bridge->remote_username;
	new_context->password = bridge->remote_password;

#నిర్వచితమైతే WITH_TLS
	new_context->tls_cafile = bridge->tls_cafile;
	new_context->tls_capath = bridge->tls_capath;
	new_context->tls_certfile = bridge->tls_certfile;
	new_context->tls_keyfile = bridge->tls_keyfile;
	new_context->tls_cert_reqs = SSL_VERIFY_PEER;
	new_context->tls_ocsp_required = bridge->tls_ocsp_required;
	new_context->tls_సంస్కరణ = bridge->tls_సంస్కరణ;
	new_context->tls_insecure = bridge->tls_insecure;
	new_context->tls_alpn = bridge->tls_alpn;
	new_context->tls_ciphers = bridge->tls_ciphers;
	new_context->tls_13_ciphers = bridge->tls_13_ciphers;
	new_context->tls_engine = ఖాళీ;
	new_context->tls_keyform = mosq_k_pem;
	new_context->tls_use_os_certs = bridge->tls_use_os_certs;
	new_context->ssl_ctx_defaults = true;
#నిర్వచితమైతే FINAL_WITH_TLS_PSK
	new_context->tls_psk_తటస్థం = bridge->tls_psk_తటస్థం;
	new_context->tls_psk = bridge->tls_psk;
#అయితే_ముగింపు
#అయితే_ముగింపు

	bridge->try_వ్యక్తిగత_accepted = true;
	ఒకవేళ(bridge->clean_ప్రారంభం_local == -1){
		/* default to "regular" clean start setting */
		bridge->clean_ప్రారంభం_local = bridge->clean_ప్రారంభం;
	}
	new_context->retain_అందుబాటు = bridge->outgoing_retain;
	new_context->protocol = bridge->protocol_సంస్కరణ;
	ఒకవేళ(!bridge->clean_ప్రారంభం_local){
		new_context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
		plugin_persist_handle_client_కూడు(new_context);
		ఒకవేళ(new_context->expiry_list_అంశం){
			/* We've restored from persistence and been added to the session
			 * expiry list, even though we should never be expired */
			session_expiry__remove(new_context);
		}
	}

	bridges = mosquitto_realloc(db.bridges, (size_t)(db.bridge_లెక్క+1)*పరిమాణం(నిర్మాణం mosquitto *));
	ఒకవేళ(bridges){
		db.bridges = bridges;
		db.bridge_లెక్క++;
		db.bridges[db.bridge_లెక్క-1] = new_context;
	}లేకపోతే{
		తిరిగి ఖాళీ;
	}

	తిరిగి new_context;
}


స్థిర శూన్య bridge__destroy(నిర్మాణం mosquitto *context)
{
	send__disconnect(context, MQTT_RC_SUCCESS, ఖాళీ);
	context__cleanup(context, true);
}


శూన్య bridge_ప్రారంభం_all(శూన్య)
{
	కొరకు(పూర్ణాంకం i=0; i<db.config->bridge_లెక్క; i++){
		నిర్మాణం mosquitto *context;
		పూర్ణాంకం ret;

		context = bridge__new(db.config->bridges[i]);
		ఒకవేళ(!context){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
			తిరిగి;
		}
		ప్రతిజ్ఞా(context);

#ఒకవేళ defined(__GLIBC__) && defined(WITH_ADNS)
		context->bridge->restart_t = 1; /* force quick restart of bridge */
		loop_update_తదుపరి_event(1000);
		ret = bridge__connect_step1(context);
#లేకపోతే
		ret = bridge__connect(context);
#అయితే_ముగింపు

		ఒకవేళ(ret && ret != MOSQ_ERR_CONN_PENDING){
			log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Unable to connect bridge %s.",
					context->bridge->name);
		}

		db.config->bridges[i] = ఖాళీ;
	}
}


స్థిర పూర్ణాంకం bridge_సమితి_tcp_keepalive(నిర్మాణం mosquitto *context)
{
	గుర్తులేని పూర్ణాంకం enabled = 1;
	bool ret;

	ఒకవేళ(context->bridge->tcp_keepalive_idle == 0
			|| context->bridge->tcp_keepalive_interval == 0
			|| context->bridge->tcp_keepalive_counter == 0){

		తిరిగి MOSQ_ERR_SUCCESS;
	}

#నిర్వచితమైతే WIN32
#  define SETSOCKOPT_TYPE అక్షరం *
#లేకపోతే
#  define SETSOCKOPT_TYPE స్థిరరాశి శూన్య *
#అయితే_ముగింపు

	ret = setsockopt(context->sock, SOL_SOCKET, SO_KEEPALIVE, (SETSOCKOPT_TYPE)&enabled, పరిమాణం(enabled));

#నిర్వచితమైతే TCP_KEEPIDLE
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPIDLE,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_idle, పరిమాణం(context->bridge->tcp_keepalive_idle));
#అయితే_ముగింపు
#నిర్వచితమైతే TCP_KEEPINTVL
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPINTVL,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_interval, పరిమాణం(context->bridge->tcp_keepalive_interval));
#అయితే_ముగింపు
#నిర్వచితమైతే TCP_KEEPCNT
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPCNT,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_counter, పరిమాణం(context->bridge->tcp_keepalive_counter));
#అయితే_ముగింపు

#నిర్వచనం_రద్దు SETSOCKOPT_TYPE

	ఒకవేళ(ret){
		తిరిగి MOSQ_ERR_UNKNOWN;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}

#నిర్వచితమైతే WITH_TCP_USER_TIMEOUT


స్థిర పూర్ణాంకం bridge_సమితి_tcp_user_timeout(నిర్మాణం mosquitto *context)
{
	పూర్ణాంకం timeout = context->bridge->tcp_user_timeout;
	ఒకవేళ(timeout >= 0){
		ఒకవేళ(setsockopt(context->sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (అక్షరం *)&timeout, పరిమాణం(timeout))){
			తిరిగి MOSQ_ERR_UNKNOWN;
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}
#అయితే_ముగింపు

#ఒకవేళ defined(__GLIBC__) && defined(WITH_ADNS)


స్థిర పూర్ణాంకం bridge__connect_step1(నిర్మాణం mosquitto *context)
{
	పూర్ణాంకం rc;
	అక్షరం *notification_topic;
	size_t notification_topic_len;
	uint8_t notification_payload;
	నిర్మాణం mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	ఒకవేళ(!context || !context->bridge){
		తిరిగి MOSQ_ERR_INVAL;
	}

	mosquitto_సమితి_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->చివరి_సందేశం_లోపల = db.now_s;
	context->తదుపరి_సందేశం_బహిర్గతం = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_ప్రారంభం = context->bridge->clean_ప్రారంభం;
	context->లోపల_packet.payload = ఖాళీ;
	context->ping_t = 0;
	context->bridge->lazy_reconnect = false;
	context->గరిష్ఠ_packet_కొలత = context->bridge->గరిష్ఠ_packet_కొలత;
	bridge__packet_cleanup(context);
	db_సందేశం_reconnect_reset(context);

	db__messages_delete(context, false);

	/* Delete all local subscriptions even for clean_start==false. We don't
	 * remove any messages and the next loop carries out the resubscription
	 * anyway. This means any unwanted subs will be removed.
	 */
	sub__clean_session(context);

	LL_FOREACH(context->bridge->topics, cur_topic){
		ఒకవేళ(cur_topic->direction == bd_బహిర్గతం || cur_topic->direction == bd_both){
			log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			ఒకవేళ(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}లేకపోతే{
				qos = cur_topic->qos;
			}
			నిర్మాణం mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			ఒకవేళ(sub_కూడు(context, &sub) > 0){
				తిరిగి 1;
			}
			retain_వరుస(context, &sub);
		}
	}

	ఒకవేళ(context->bridge->notifications){
		ఒకవేళ(context->max_qos == 0){
			qos = 0;
		}లేకపోతే{
			qos = 1;
		}
		ఒకవేళ(context->bridge->notification_topic){
			ఒకవేళ(!context->bridge->initial_notification_పూర్తి){
				notification_payload = '0';
				db_messages_easy_వరుస(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, సందేశం_EXPIRY_INFINITE, ఖాళీ);
				context->bridge->initial_notification_పూర్తి = true;
			}
			notification_payload = '0';
			rc = will_సమితి(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, ఖాళీ);
			ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
				తిరిగి rc;
			}
		}లేకపోతే{
			notification_topic_len = అక్షరశ్రేణి_పొడవు(context->bridge->remote_clientid)+అక్షరశ్రేణి_పొడవు("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(పరిమాణం(అక్షరం)*(notification_topic_len+1));
			ఒకవేళ(!notification_topic){
				తిరిగి MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			ఒకవేళ(!context->bridge->initial_notification_పూర్తి){
				notification_payload = '0';
				db_messages_easy_వరుస(context, notification_topic, qos, 1, &notification_payload, 1, సందేశం_EXPIRY_INFINITE, ఖాళీ);
				context->bridge->initial_notification_పూర్తి = true;
			}

			notification_payload = '0';
			rc = will_సమితి(context, notification_topic, 1, &notification_payload, qos, true, ఖాళీ);
			mosquitto_FREE(notification_topic);
			ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
				తిరిగి rc;
			}
		}
	}

	log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Connecting bridge (step 1) %s (%s:%d)", context->bridge->name, context->bridge->చిరునామాలు[context->bridge->cur_చిరునామా].చిరునామా, context->bridge->చిరునామాలు[context->bridge->cur_చిరునామా].పోర్టు);
	rc = net__try_connect_step1(context, context->bridge->చిరునామాలు[context->bridge->cur_చిరునామా].చిరునామా);
	ఒకవేళ(rc > 0){
		ఒకవేళ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			తిరిగి rc; /* Error already printed */
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_ERRNO){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_EAI){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		తిరిగి rc;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం bridge__connect_step2(నిర్మాణం mosquitto *context)
{
	పూర్ణాంకం rc;

	ఒకవేళ(!context || !context->bridge){
		తిరిగి MOSQ_ERR_INVAL;
	}

	log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Connecting bridge (step 2) %s (%s:%d)", context->bridge->name, context->bridge->చిరునామాలు[context->bridge->cur_చిరునామా].చిరునామా, context->bridge->చిరునామాలు[context->bridge->cur_చిరునామా].పోర్టు);
	rc = net__try_connect_step2(context, context->bridge->చిరునామాలు[context->bridge->cur_చిరునామా].పోర్టు, &context->sock);
	ఒకవేళ(rc > 0){
		ఒకవేళ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			తిరిగి rc; /* Error already printed */
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_ERRNO){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_EAI){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		తిరిగి rc;
	}

	HASH_కూడు(hh_sock, db.contexts_by_sock, sock, పరిమాణం(context->sock), context);

	ఒకవేళ(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto_సమితి_state(context, mosq_cs_connect_pending);
		mux_కూడు_బహిర్గతం(context);
	}
	తిరిగి rc;
}


పూర్ణాంకం bridge__connect_step3(నిర్మాణం mosquitto *context)
{
	పూర్ణాంకం rc;
	mosquitto_property receive_గరిష్ఠ;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = ఖాళీ;

	rc = net__socket_connect_step3(context, context->bridge->చిరునామాలు[context->bridge->cur_చిరునామా].చిరునామా);
	ఒకవేళ(rc > 0){
		ఒకవేళ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			తిరిగి rc; /* Error already printed */
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_ERRNO){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_EAI){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		తిరిగి rc;
	}

	ఒకవేళ(context->bridge->విడత_robin == false && context->bridge->cur_చిరునామా != 0){
		context->bridge->primary_retry = db.now_s + 5;
		loop_update_తదుపరి_event(5000);
	}

	ఒకవేళ(bridge_సమితి_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		తిరిగి MOSQ_ERR_UNKNOWN;
	}
#నిర్వచితమైతే WITH_TCP_USER_TIMEOUT
	ఒకవేళ(bridge_సమితి_tcp_user_timeout(context)){
		తిరిగి MOSQ_ERR_UNKNOWN;
	}
#అయితే_ముగింపు

	ఒకవేళ(context->bridge->receive_గరిష్ఠ != 0){
		receive_గరిష్ఠ.విలువ.i16 = context->bridge->receive_గరిష్ఠ;
		receive_గరిష్ఠ.identifier = MQTT_PROP_RECEIVE_గరిష్ఠ;
		receive_గరిష్ఠ.property_type = MQTT_PROP_TYPE_INT16;
		receive_గరిష్ఠ.client_generated = false;
		receive_గరిష్ఠ.తదుపరి = properties;
		properties = &receive_గరిష్ఠ;
	}
	ఒకవేళ(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_తక్షణ){
		session_expiry_interval.విలువ.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.తదుపరి = properties;
		properties = &session_expiry_interval;
	}
	ఒకవేళ(context->bridge->max_topic_alias != 0){
		topic_alias_max.విలువ.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_గరిష్ఠ;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.తదుపరి = properties;
		properties = &topic_alias_max;
	}

	rc = send__connect(context, context->keepalive, context->clean_ప్రారంభం, properties);
	ఒకవేళ(rc == MOSQ_ERR_SUCCESS){
		తిరిగి MOSQ_ERR_SUCCESS;
	}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		తిరిగి MOSQ_ERR_SUCCESS;
	}లేకపోతే{
		ఒకవేళ(rc == MOSQ_ERR_TLS){
			తిరిగి rc; /* Error already printed */
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_ERRNO){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_EAI){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		తిరిగి rc;
	}
}
#లేకపోతే


పూర్ణాంకం bridge__connect(నిర్మాణం mosquitto *context)
{
	పూర్ణాంకం rc, rc2;
	అక్షరం *notification_topic = ఖాళీ;
	size_t notification_topic_len;
	uint8_t notification_payload;
	నిర్మాణం mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	mosquitto_property receive_గరిష్ఠ;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = ఖాళీ;

	ఒకవేళ(!context || !context->bridge){
		తిరిగి MOSQ_ERR_INVAL;
	}

	mosquitto_సమితి_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->చివరి_సందేశం_లోపల = db.now_s;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->తదుపరి_సందేశం_బహిర్గతం = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_ప్రారంభం = context->bridge->clean_ప్రారంభం;
	context->లోపల_packet.payload = ఖాళీ;
	context->ping_t = 0;
	context->bridge->lazy_reconnect = false;
	context->గరిష్ఠ_packet_కొలత = context->bridge->గరిష్ఠ_packet_కొలత;
	bridge__packet_cleanup(context);
	db_సందేశం_reconnect_reset(context);

	db__messages_delete(context, false);

	/* Delete all local subscriptions even for clean_start==false. We don't
	 * remove any messages and the next loop carries out the resubscription
	 * anyway. This means any unwanted subs will be removed.
	 */
	sub__clean_session(context);

	LL_FOREACH(context->bridge->topics, cur_topic){
		ఒకవేళ(cur_topic->direction == bd_బహిర్గతం || cur_topic->direction == bd_both){
			log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			ఒకవేళ(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}లేకపోతే{
				qos = cur_topic->qos;
			}
			నిర్మాణం mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			ఒకవేళ(sub_కూడు(context, &sub) > 0){
				తిరిగి 1;
			}
		}
	}

	ఒకవేళ(context->bridge->notifications){
		ఒకవేళ(context->max_qos == 0){
			qos = 0;
		}లేకపోతే{
			qos = 1;
		}
		ఒకవేళ(context->bridge->notification_topic){
			ఒకవేళ(!context->bridge->initial_notification_పూర్తి){
				notification_payload = '0';
				db_messages_easy_వరుస(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, సందేశం_EXPIRY_INFINITE, ఖాళీ);
				context->bridge->initial_notification_పూర్తి = true;
			}

			notification_payload = '0';
			rc = will_సమితి(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, ఖాళీ);
			ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
				తిరిగి rc;
			}
		}లేకపోతే{
			notification_topic_len = అక్షరశ్రేణి_పొడవు(context->bridge->remote_clientid)+అక్షరశ్రేణి_పొడవు("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(పరిమాణం(అక్షరం)*(notification_topic_len+1));
			ఒకవేళ(!notification_topic){
				తిరిగి MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			ఒకవేళ(!context->bridge->initial_notification_పూర్తి){
				notification_payload = '0';
				db_messages_easy_వరుస(context, notification_topic, qos, 1, &notification_payload, 1, సందేశం_EXPIRY_INFINITE, ఖాళీ);
				context->bridge->initial_notification_పూర్తి = true;
			}

			notification_payload = '0';
			rc = will_సమితి(context, notification_topic, 1, &notification_payload, qos, true, ఖాళీ);
			ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
				mosquitto_FREE(notification_topic);
				తిరిగి rc;
			}
			mosquitto_FREE(notification_topic);
		}
	}

	log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Connecting bridge %s (%s:%d)", context->bridge->name, context->bridge->చిరునామాలు[context->bridge->cur_చిరునామా].చిరునామా, context->bridge->చిరునామాలు[context->bridge->cur_చిరునామా].పోర్టు);
	rc = net__socket_connect(context,
			context->bridge->చిరునామాలు[context->bridge->cur_చిరునామా].చిరునామా,
			context->bridge->చిరునామాలు[context->bridge->cur_చిరునామా].పోర్టు,
			context->bridge->bind_చిరునామా,
			false);

	ఒకవేళ(rc > 0){
		ఒకవేళ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			తిరిగి rc; /* Error already printed */
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_ERRNO){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_EAI){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		తిరిగి rc;
	}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto_సమితి_state(context, mosq_cs_connect_pending);
		mux_కూడు_బహిర్గతం(context);
	}

	HASH_కూడు(hh_sock, db.contexts_by_sock, sock, పరిమాణం(context->sock), context);

	ఒకవేళ(bridge_సమితి_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		తిరిగి MOSQ_ERR_UNKNOWN;
	}
#నిర్వచితమైతే WITH_TCP_USER_TIMEOUT
	ఒకవేళ(bridge_సమితి_tcp_user_timeout(context)){
		తిరిగి MOSQ_ERR_UNKNOWN;
	}
#అయితే_ముగింపు

	ఒకవేళ(context->bridge->receive_గరిష్ఠ != 0){
		receive_గరిష్ఠ.విలువ.i16 = context->bridge->receive_గరిష్ఠ;
		receive_గరిష్ఠ.identifier = MQTT_PROP_RECEIVE_గరిష్ఠ;
		receive_గరిష్ఠ.property_type = MQTT_PROP_TYPE_INT16;
		receive_గరిష్ఠ.client_generated = false;
		receive_గరిష్ఠ.తదుపరి = properties;
		properties = &receive_గరిష్ఠ;
	}
	ఒకవేళ(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_తక్షణ){
		session_expiry_interval.విలువ.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.తదుపరి = properties;
		properties = &session_expiry_interval;
	}
	ఒకవేళ(context->bridge->max_topic_alias != 0){
		topic_alias_max.విలువ.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_గరిష్ఠ;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.తదుపరి = properties;
		properties = &topic_alias_max;
	}

	rc2 = send__connect(context, context->keepalive, context->clean_ప్రారంభం, properties);
	ఒకవేళ(rc2 == MOSQ_ERR_SUCCESS){
		తిరిగి rc;
	}లేకపోతే ఒకవేళ(rc2 == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		తిరిగి MOSQ_ERR_SUCCESS;
	}లేకపోతే{
		ఒకవేళ(rc2 == MOSQ_ERR_TLS){
			తిరిగి rc2; /* Error already printed */
		}లేకపోతే ఒకవేళ(rc2 == MOSQ_ERR_ERRNO){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}లేకపోతే ఒకవేళ(rc2 == MOSQ_ERR_EAI){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		తిరిగి rc2;
	}
}
#అయితే_ముగింపు


పూర్ణాంకం bridge__on_connect(నిర్మాణం mosquitto *context)
{
	అక్షరం *notification_topic;
	size_t notification_topic_len;
	నిర్మాణం mosquitto__bridge_topic *cur_topic;
	పూర్ణాంకం sub_opts;
	bool retain = true;
	uint8_t qos;

	ఒకవేళ(context->bridge->notifications){
		ఒకవేళ(context->max_qos == 0){
			qos = 0;
		}లేకపోతే{
			qos = 1;
		}
		ఒకవేళ(!context->retain_అందుబాటు){
			retain = false;
		}
		అక్షరం notification_payload = '1';
		ఒకవేళ(context->bridge->notification_topic){
			ఒకవేళ(!context->bridge->notifications_local_only){
				ఒకవేళ(send__real_publish(context, mosquitto__mid_generate(context),
						context->bridge->notification_topic, 1, &notification_payload, qos, retain, 0, 0, ఖాళీ, 0)){

					తిరిగి 1;
				}
			}
			db_messages_easy_వరుస(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, సందేశం_EXPIRY_INFINITE, ఖాళీ);
		}లేకపోతే{
			notification_topic_len = అక్షరశ్రేణి_పొడవు(context->bridge->remote_clientid)+అక్షరశ్రేణి_పొడవు("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(పరిమాణం(అక్షరం)*(notification_topic_len+1));
			ఒకవేళ(!notification_topic){
				తిరిగి MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);
			notification_payload = '1';
			ఒకవేళ(!context->bridge->notifications_local_only){
				ఒకవేళ(send__real_publish(context, mosquitto__mid_generate(context),
						notification_topic, 1, &notification_payload, qos, retain, 0, 0, ఖాళీ, 0)){

					mosquitto_FREE(notification_topic);
					తిరిగి 1;
				}
			}
			db_messages_easy_వరుస(context, notification_topic, qos, 1, &notification_payload, 1, సందేశం_EXPIRY_INFINITE, ఖాళీ);
			mosquitto_FREE(notification_topic);
		}
	}

	LL_FOREACH(context->bridge->topics, cur_topic){
		ఒకవేళ(cur_topic->direction == bd_లోపల || cur_topic->direction == bd_both){
			ఒకవేళ(cur_topic->qos > context->max_qos){
				sub_opts = context->max_qos;
			}లేకపోతే{
				sub_opts = cur_topic->qos;
			}
			ఒకవేళ(context->bridge->protocol_సంస్కరణ == mosq_p_mqtt5){
				sub_opts = sub_opts
						| MQTT_SUB_OPT_NO_LOCAL
						| MQTT_SUB_OPT_RETAIN_AS_PUBLISHED
						| MQTT_SUB_OPT_SEND_RETAIN_ALWAYS;
			}
			ఒకవేళ(send__subscribe(context, ఖాళీ, 1, &cur_topic->remote_topic, sub_opts, ఖాళీ)){
				తిరిగి 1;
			}
		}లేకపోతే{
			ఒకవేళ(context->bridge->attempt_unsubscribe){
				ఒకవేళ(send__unsubscribe(context, ఖాళీ, 1, &cur_topic->remote_topic, ఖాళీ)){
					/* direction = inwards only. This means we should not be subscribed
					* to the topic. It is possible that we used to be subscribed to
					* this topic so unsubscribe. */
					తిరిగి 1;
				}
			}
		}
	}
	నిర్మాణం mosquitto_subscription sub;
	memset(&sub, 0, పరిమాణం(sub));
	LL_FOREACH(context->bridge->topics, cur_topic){
		sub.topic_filter = cur_topic->local_topic;
		ఒకవేళ(cur_topic->direction == bd_బహిర్గతం || cur_topic->direction == bd_both){
			ఒకవేళ(cur_topic->qos > context->max_qos){
				sub.options = context->max_qos;
			}లేకపోతే{
				sub.options = cur_topic->qos;
			}
			retain_వరుస(context, &sub);
		}
	}

	context->bridge->connected_at = db.now_s;

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం bridge_రిజిస్టరు_local_connections(శూన్య)
{
	నిర్మాణం mosquitto *context, *ctxt_tmp = ఖాళీ;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ఒకవేళ(context->bridge){
			ఒకవేళ(mux__new(context)){
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error in initial bridge registration: %s", strerror(errno));
				తిరిగి MOSQ_ERR_UNKNOWN;
			}
			mux_కూడు_బహిర్గతం(context);
		}
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


శూన్య bridge__reload(శూన్య)
{
	పూర్ణాంకం i;
	పూర్ణాంకం j;

	// destroy old bridges that dissappeared
	కొరకు(i=0; i<db.bridge_లెక్క; i++){
		కొరకు(j=0; j<db.config->bridge_లెక్క; j++){
			ఒకవేళ(!strcmp(db.bridges[i]->bridge->name, db.config->bridges[j]->name)){
				ఆపు;
			}
		}

		ఒకవేళ(j==db.config->bridge_లెక్క){
			bridge__destroy(db.bridges[i]);
		}
	}

	కొరకు(i=0; i<db.config->bridge_లెక్క; i++){
		కొరకు(j=0; j<db.bridge_లెక్క; j++){
			ఒకవేళ(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
				ఆపు;
			}
		}

		ఒకవేళ(j==db.bridge_లెక్క){
			// a new bridge was found, create it
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ఖాళీ;
			కొనసాగు;
		}

		ఒకవేళ(db.config->bridges[i]->reload_type == brt_తక్షణ){
			// in this case, an existing bridge should match
			కొరకు(j=0; j<db.bridge_లెక్క; j++){
				ఒకవేళ(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
					ఆపు;
				}
			}

			ప్రతిజ్ఞా(j<db.bridge_లెక్క);
			db.bridges[j]->will_delay_interval = 0;
			bridge__destroy(db.bridges[j]);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ఖాళీ;
		}
	}
}


శూన్య bridge__db_cleanup(శూన్య)
{
	పూర్ణాంకం i;

	కొరకు(i=0; i<db.bridge_లెక్క; i++){
		ఒకవేళ(db.bridges[i]){
			context__cleanup(db.bridges[i], true);
		}
	}
	mosquitto_FREE(db.bridges);
}


శూన్య bridge__cleanup(నిర్మాణం mosquitto *context)
{
	పూర్ణాంకం i;

	ప్రతిజ్ఞా(db.bridge_లెక్క > 0);

	కొరకు(i=0; i<db.bridge_లెక్క; i++){
		ఒకవేళ(db.bridges[i] == context){
			db.bridges[i] = db.bridges[db.bridge_లెక్క-1];
			ఆపు;
		}
	}

	db.bridge_లెక్క--;
	ఒకవేళ(db.bridge_లెక్క == 0){
		mosquitto_FREE(db.bridges);
	}లేకపోతే{
		db.bridges = mosquitto_realloc(db.bridges, (గుర్తులేని)db.bridge_లెక్క * పరిమాణం(db.bridges[0]));
	}

	mosquitto_FREE(context->bridge->name);
	mosquitto_FREE(context->bridge->local_clientid);
	mosquitto_FREE(context->bridge->local_username);
	mosquitto_FREE(context->bridge->local_password);
#నిర్వచితమైతే WITH_TLS
	mosquitto_FREE(context->bridge->tls_certfile);
	mosquitto_FREE(context->bridge->tls_keyfile);
#అయితే_ముగింపు

	ఒకవేళ(context->bridge->remote_clientid != context->id){
		mosquitto_FREE(context->bridge->remote_clientid);
	}
	context->bridge->remote_clientid = ఖాళీ;

	ఒకవేళ(context->bridge->remote_username != context->username){
		mosquitto_FREE(context->bridge->remote_username);
	}
	context->bridge->remote_username = ఖాళీ;

	ఒకవేళ(context->bridge->remote_password != context->password){
		mosquitto_FREE(context->bridge->remote_password);
	}
	context->bridge->remote_password = ఖాళీ;
#నిర్వచితమైతే WITH_TLS
	ఒకవేళ(context->ssl_ctx){
		SSL_CTX_free(context->ssl_ctx);
		context->ssl_ctx = ఖాళీ;
	}
#అయితే_ముగింపు

	కొరకు(i=0; i<context->bridge->చిరునామా_లెక్క; i++){
		mosquitto_FREE(context->bridge->చిరునామాలు[i].చిరునామా);
	}

	mosquitto_FREE(context->bridge->చిరునామాలు);

	config__bridge_cleanup(context->bridge);
	context->bridge = ఖాళీ;
}


స్థిర శూన్య bridge__packet_cleanup(నిర్మాణం mosquitto *context)
{
	నిర్మాణం mosquitto__packet *packet;
	ఒకవేళ(!context){
		తిరిగి;
	}

	అయితే(context->బహిర్గతం_packet){
		packet = context->బహిర్గతం_packet;
		context->బహిర్గతం_packet = context->బహిర్గతం_packet->తదుపరి;
		mosquitto_FREE(packet);
	}
	context->బహిర్గతం_packet = ఖాళీ;
	context->బహిర్గతం_packet_చివరి = ఖాళీ;
	metrics__int_dec(mosq_gauge_బహిర్గతం_packets, context->బహిర్గతం_packet_లెక్క);
	metrics__int_dec(mosq_gauge_బహిర్గతం_packet_బైట్లు, context->బహిర్గతం_packet_బైట్లు);
	context->బహిర్గతం_packet_లెక్క = 0;
	context->బహిర్గతం_packet_బైట్లు = 0;

	packet__cleanup(&(context->లోపల_packet));
}


స్థిర పూర్ణాంకం rand_between(పూర్ణాంకం దిగువ, పూర్ణాంకం ఎగువ)
{
	పూర్ణాంకం r;
	mosquitto_getrandom(&r, పరిమాణం(పూర్ణాంకం));
	తిరిగి (నిరపేక్ష(r) % (ఎగువ - దిగువ)) + దిగువ;
}


స్థిర శూన్య bridge_backoff_అడుగు(నిర్మాణం mosquitto__bridge *bridge)
{
	/*
		“Decorrelated Jitter” calculation, according to:
			https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
	*/

	bridge->restart_timeout = rand_between(bridge->backoff_పునాది, bridge->restart_timeout * 3);
	ఒకవేళ(bridge->restart_timeout > bridge->backoff_cap){
		bridge->restart_timeout = bridge->backoff_cap;
	}
}


స్థిర శూన్య bridge__backoff_reset(నిర్మాణం mosquitto__bridge *bridge)
{
	bridge->restart_timeout = bridge->backoff_పునాది;
}


స్థిర శూన్య bridge__update_backoff(నిర్మాణం mosquitto__bridge *bridge)
{
	ఒకవేళ(!bridge){
		తిరిగి;
	}
	ఒకవేళ(!bridge->backoff_cap){
		/* skip if not using jitter */
		తిరిగి;

	}
	ఒకవేళ(bridge->connected_at && db.now_s - bridge->connected_at >= bridge->stable_connection_period){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Bridge %s connection was stable enough, resetting backoff", bridge->name);
		bridge__backoff_reset(bridge);
	}లేకపోతే{
		bridge_backoff_అడుగు(bridge);
	}

	bridge->connected_at = 0;

	log__printf(ఖాళీ, MOSQ_LOG_INFO, "Bridge %s next backoff will be %d ms", bridge->name, bridge->restart_timeout);
}


స్థిర శూన్య bridge_check_pending(నిర్మాణం mosquitto *context)
{
	పూర్ణాంకం err;
	socklen_t len;

	ఒకవేళ(context->state == mosq_cs_connect_pending){
		len = పరిమాణం(పూర్ణాంకం);
		ఒకవేళ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (అక్షరం *)&err, &len)){
			ఒకవేళ(err == 0){
				mosquitto_సమితి_state(context, mosq_cs_new);
#ఒకవేళ defined(WITH_ADNS) && defined(WITH_BRIDGE)
				ఒకవేళ(context->bridge){
					bridge__connect_step3(context);
				}
#అయితే_ముగింపు
			}లేకపోతే ఒకవేళ(err == ECONNREFUSED){
				do_disconnect(context, MOSQ_ERR_CONN_కోల్పోయిన);
				తిరిగి;
			}
		}లేకపోతే{
			do_disconnect(context, MOSQ_ERR_CONN_కోల్పోయిన);
			తిరిగి;
		}
	}
}


స్థిర bool reload_if_needed(నిర్మాణం mosquitto *context)
{
	పూర్ణాంకం i;

	కొరకు(i=0; i<db.config->bridge_లెక్క; i++){
		ఒకవేళ(db.config->bridges[i] && !strcmp(context->bridge->name, db.config->bridges[i]->name)){
			bridge__destroy(context);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = ఖాళీ;
			loop_update_తదుపరి_event(100);
			తిరిగి true;
		}
	}

	తిరిగి false;
}


శూన్య bridge_check(శూన్య)
{
	స్థిర time_t చివరి_check = 0;
	నిర్మాణం mosquitto *context = ఖాళీ;
	socklen_t len;
	పూర్ణాంకం i;
	పూర్ణాంకం rc;
	పూర్ణాంకం err;

	ఒకవేళ(db.now_s <= చివరి_check){
		తిరిగి;
	}

	కొరకు(i=0; i<db.bridge_లెక్క; i++){
		ఒకవేళ(!db.bridges[i]){
			కొనసాగు;
		}

		context = db.bridges[i];

		ఒకవేళ(net__is_connected(context)){
			mosquitto__check_keepalive(context);
			bridge_check_pending(context);

			/* Check for bridges that are not round robin and not currently
			 * connected to their primary broker. */
			ఒకవేళ(context->bridge->విడత_robin == false
					&& context->bridge->cur_చిరునామా != 0
					&& context->bridge->primary_retry
					&& db.now_s >= context->bridge->primary_retry){

				ఒకవేళ(context->bridge->primary_retry_sock == INVALID_SOCKET){
					rc = net__try_connect(context->bridge->చిరునామాలు[0].చిరునామా,
							context->bridge->చిరునామాలు[0].పోర్టు,
							&context->bridge->primary_retry_sock,
							context->bridge->bind_చిరునామా, false);

					ఒకవేళ(rc == 0){
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = 0;
						mux__delete(context);
						net__socket_close(context);
						context->bridge->cur_చిరునామా = 0;
					}
				}లేకపోతే{
					len = పరిమాణం(పూర్ణాంకం);
					ఒకవేళ(!getsockopt(context->bridge->primary_retry_sock, SOL_SOCKET, SO_ERROR, (అక్షరం *)&err, &len)){
						ఒకవేళ(err == 0){
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = 0;
							mux__delete(context);
							net__socket_close(context);
							context->bridge->cur_చిరునామా = context->bridge->చిరునామా_లెక్క-1;
						}లేకపోతే{
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = db.now_s+5;
							loop_update_తదుపరి_event(5000);
						}
					}లేకపోతే{
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = db.now_s+5;
						loop_update_తదుపరి_event(5000);
					}
				}
			}
		}లేకపోతే{
			ఒకవేళ(reload_if_needed(context)){
				కొనసాగు;
			}

			/* Want to try to restart the bridge connection */
			ఒకవేళ(!context->bridge->restart_t){
				bridge__update_backoff(context->bridge);
				context->bridge->restart_t = 1000*db.now_s+context->bridge->restart_timeout;
				context->bridge->cur_చిరునామా++;
				ఒకవేళ(context->bridge->cur_చిరునామా == context->bridge->చిరునామా_లెక్క){
					context->bridge->cur_చిరునామా = 0;
				}
				loop_update_తదుపరి_event(context->bridge->restart_timeout);
			}లేకపోతే{
				ఒకవేళ((context->bridge->ప్రారంభం_type == bst_lazy && context->bridge->lazy_reconnect)
						|| (context->bridge->ప్రారంభం_type == bst_automatic && 1000*db.now_s >= context->bridge->restart_t)){

#ఒకవేళ defined(__GLIBC__) && defined(WITH_ADNS)
					ఒకవేళ(context->adns){
						/* Connection attempted, waiting on DNS lookup */
						rc = gai_error(context->adns);
						ఒకవేళ(rc == EAI_INPROGRESS){
							/* Just keep on waiting */
						}లేకపోతే ఒకవేళ(rc == 0){
							rc = bridge__connect_step2(context);
							ఒకవేళ(rc == MOSQ_ERR_SUCCESS){
								mux__new(context);
								ఒకవేళ(context->బహిర్గతం_packet){
									mux_కూడు_బహిర్గతం(context);
								}
							}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_CONN_PENDING){
								mux__new(context);
								mux_కూడు_బహిర్గతం(context);
								context->bridge->restart_t = 0;
							}లేకపోతే{
								context->bridge->cur_చిరునామా++;
								ఒకవేళ(context->bridge->cur_చిరునామా == context->bridge->చిరునామా_లెక్క){
									context->bridge->cur_చిరునామా = 0;
								}
								context->bridge->restart_t = 0;
							}
						}లేకపోతే{
							/* Need to retry */
							ఒకవేళ(context->adns->ar_ఫలితం){
								freeaddrinfo(context->adns->ar_ఫలితం);
							}
							mosquitto_FREE(context->adns);
							context->bridge->restart_t = 0;
						}
					}లేకపోతే{
						rc = bridge__connect_step1(context);
						ఒకవేళ(rc){
							context->bridge->cur_చిరునామా++;
							ఒకవేళ(context->bridge->cur_చిరునామా == context->bridge->చిరునామా_లెక్క){
								context->bridge->cur_చిరునామా = 0;
							}
						}లేకపోతే{
							/* Short wait for ADNS lookup */
							context->bridge->restart_t = 1;
							loop_update_తదుపరి_event(1000);
						}
					}
#లేకపోతే
					{
						rc = bridge__connect(context);
						context->bridge->restart_t = 0;
						ఒకవేళ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING){
							ఒకవేళ(context->bridge->విడత_robin == false && context->bridge->cur_చిరునామా != 0){
								context->bridge->primary_retry = db.now_s + 5;
								loop_update_తదుపరి_event(5000);
							}
							mux__new(context);
							ఒకవేళ(context->బహిర్గతం_packet){
								mux_కూడు_బహిర్గతం(context);
							}
						}లేకపోతే{
							context->bridge->cur_చిరునామా++;
							ఒకవేళ(context->bridge->cur_చిరునామా == context->bridge->చిరునామా_లెక్క){
								context->bridge->cur_చిరునామా = 0;
							}
						}
					}
#అయితే_ముగింపు
				}
			}
		}
	}
}

#అయితే_ముగింపు
