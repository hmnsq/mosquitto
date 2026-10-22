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

#include "config.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifndef WIN32
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#ᰀᰧᰀ
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#ifndef WIN32
#include <unistd.h>
#ᰀᰧᰀ
#include <process.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include "mosquitto.h"
#include "mosquitto_broker_internal.h"
#include "mosquitto_internal.h"
#include "net_mosq.h"
#include "packet_mosq.h"
#include "property_common.h"
#include "send_mosq.h"
#include "sys_tree.h"
#include "tls_mosq.h"
#include "util_mosq.h"
#include "will_mosq.h"
#include "utlist.h"

#ifdef WITH_BRIDGE

static void bridge__update_backoff(sa_nracanaa mosquitto__bridge *bridge);
#ᰀᰧ defined(__GLIBC__) && defined(WITH_ADNS)
static puur_naa_nka bridge__connect_step1(sa_nracanaa mosquitto *context);
static puur_naa_nka bridge__connect_step2(sa_nracanaa mosquitto *context);
#endif
static void bridge__packet_cleanup(sa_nracanaa mosquitto *context);

static sa_nracanaa mosquitto *bridge__new(sa_nracanaa mosquitto__bridge *bridge)
{
	sa_nracanaa mosquitto *new_context = NULL;
	sa_nracanaa mosquitto **bridges;
	var_na *local_id;

	assert(bridge);

	local_id = mosquitto_strdup(bridge->local_clientid);
	ᰀᰧ(!local_id){
		ᰀᰧᰌ NULL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, local_id, strlen(local_id), new_context);
	ᰀᰧ(new_context){
		/* (possible from persistent db) */
		mosquitto_FREE(local_id);
	}ᰀᰧᰀ{
		/* id wasn't found, so generate a new context */
		new_context = context__init();
		ᰀᰧ(!new_context){
			mosquitto_FREE(local_id);
			ᰀᰧᰌ NULL;
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
	new_context->tls_engine = NULL;
	new_context->tls_keyform = mosq_k_pem;
	new_context->tls_use_os_certs = bridge->tls_use_os_certs;
	new_context->ssl_ctx_defaults = true;
#ifdef FINAL_WITH_TLS_PSK
	new_context->tls_psk_identity = bridge->tls_psk_identity;
	new_context->tls_psk = bridge->tls_psk;
#endif
#endif

	bridge->try_private_accepted = true;
	ᰀᰧ(bridge->clean_start_local == -1){
		/* default to "regular" clean start setting */
		bridge->clean_start_local = bridge->clean_start;
	}
	new_context->retain_available = bridge->outgoing_retain;
	new_context->protocol = bridge->protocol_version;
	ᰀᰧ(!bridge->clean_start_local){
		new_context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
		plugin_persist__handle_client_add(new_context);
		ᰀᰧ(new_context->expiry_list_item){
			/* We've restored from persistence and been added to the session
			 * expiry list, even though we should never be expired */
			session_expiry__remove(new_context);
		}
	}

	bridges = mosquitto_realloc(db.bridges, (size_t)(db.bridge_count+1)*parimaa_na(sa_nracanaa mosquitto *));
	ᰀᰧ(bridges){
		db.bridges = bridges;
		db.bridge_count++;
		db.bridges[db.bridge_count-1] = new_context;
	}ᰀᰧᰀ{
		ᰀᰧᰌ NULL;
	}

	ᰀᰧᰌ new_context;
}


static void bridge__destroy(sa_nracanaa mosquitto *context)
{
	send__disconnect(context, MQTT_RC_SUCCESS, NULL);
	context__cleanup(context, true);
}


void bridge__start_all(void)
{
	for(puur_naa_nka i=0; i<db.config->bridge_count; i++){
		sa_nracanaa mosquitto *context;
		puur_naa_nka ret;

		context = bridge__new(db.config->bridges[i]);
		ᰀᰧ(!context){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			ᰀᰧᰌ;
		}
		assert(context);

#ᰀᰧ defined(__GLIBC__) && defined(WITH_ADNS)
		context->bridge->restart_t = 1; /* force quick restart of bridge */
		loop__update_next_event(1000);
		ret = bridge__connect_step1(context);
#ᰀᰧᰀ
		ret = bridge__connect(context);
#endif

		ᰀᰧ(ret && ret != MOSQ_ERR_CONN_PENDING){
			log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Unable to connect bridge %s.",
					context->bridge->name);
		}

		db.config->bridges[i] = NULL;
	}
}


static puur_naa_nka bridge__set_tcp_keepalive(sa_nracanaa mosquitto *context)
{
	unsigned puur_naa_nka enabled = 1;
	bool ret;

	ᰀᰧ(context->bridge->tcp_keepalive_idle == 0
			|| context->bridge->tcp_keepalive_interval == 0
			|| context->bridge->tcp_keepalive_counter == 0){

		ᰀᰧᰌ MOSQ_ERR_SUCCESS;
	}

#ifdef WIN32
#  define SETSOCKOPT_TYPE var_na *
#ᰀᰧᰀ
#  define SETSOCKOPT_TYPE const void *
#endif

	ret = setsockopt(context->sock, SOL_SOCKET, SO_KEEPALIVE, (SETSOCKOPT_TYPE)&enabled, parimaa_na(enabled));

#ifdef TCP_KEEPIDLE
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPIDLE,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_idle, parimaa_na(context->bridge->tcp_keepalive_idle));
#endif
#ifdef TCP_KEEPINTVL
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPINTVL,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_interval, parimaa_na(context->bridge->tcp_keepalive_interval));
#endif
#ifdef TCP_KEEPCNT
	ret = ret || setsockopt(context->sock, IPPROTO_TCP, TCP_KEEPCNT,
			(SETSOCKOPT_TYPE)&context->bridge->tcp_keepalive_counter, parimaa_na(context->bridge->tcp_keepalive_counter));
#endif

#undef SETSOCKOPT_TYPE

	ᰀᰧ(ret){
		ᰀᰧᰌ MOSQ_ERR_UNKNOWN;
	}

	ᰀᰧᰌ MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TCP_USER_TIMEOUT


static puur_naa_nka bridge__set_tcp_user_timeout(sa_nracanaa mosquitto *context)
{
	puur_naa_nka timeout = context->bridge->tcp_user_timeout;
	ᰀᰧ(timeout >= 0){
		ᰀᰧ(setsockopt(context->sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (var_na *)&timeout, parimaa_na(timeout))){
			ᰀᰧᰌ MOSQ_ERR_UNKNOWN;
		}
	}

	ᰀᰧᰌ MOSQ_ERR_SUCCESS;
}
#endif

#ᰀᰧ defined(__GLIBC__) && defined(WITH_ADNS)


static puur_naa_nka bridge__connect_step1(sa_nracanaa mosquitto *context)
{
	puur_naa_nka rc;
	var_na *notification_topic;
	size_t notification_topic_len;
	uint8_t notification_payload;
	sa_nracanaa mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	ᰀᰧ(!context || !context->bridge){
		ᰀᰧᰌ MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	context->last_msg_in = db.now_s;
	context->next_msg_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = NULL;
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
		ᰀᰧ(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(NULL, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			ᰀᰧ(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}ᰀᰧᰀ{
				qos = cur_topic->qos;
			}
			sa_nracanaa mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			ᰀᰧ(sub__add(context, &sub) > 0){
				ᰀᰧᰌ 1;
			}
			retain__queue(context, &sub);
		}
	}

	ᰀᰧ(context->bridge->notifications){
		ᰀᰧ(context->max_qos == 0){
			qos = 0;
		}ᰀᰧᰀ{
			qos = 1;
		}
		ᰀᰧ(context->bridge->notification_topic){
			ᰀᰧ(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, MSG_EXPIRY_INFINITE, NULL);
				context->bridge->initial_notification_done = true;
			}
			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, NULL);
			ᰀᰧ(rc != MOSQ_ERR_SUCCESS){
				ᰀᰧᰌ rc;
			}
		}ᰀᰧᰀ{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(parimaa_na(var_na)*(notification_topic_len+1));
			ᰀᰧ(!notification_topic){
				ᰀᰧᰌ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			ᰀᰧ(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, MSG_EXPIRY_INFINITE, NULL);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, NULL);
			mosquitto_FREE(notification_topic);
			ᰀᰧ(rc != MOSQ_ERR_SUCCESS){
				ᰀᰧᰌ rc;
			}
		}
	}

	log__printf(NULL, MOSQ_LOG_NOTICE, "Connecting bridge (step 1) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step1(context, context->bridge->addresses[context->bridge->cur_address].address);
	ᰀᰧ(rc > 0){
		ᰀᰧ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ᰀᰧᰌ rc; /* Error already printed */
		}ᰀᰧᰀ ᰀᰧ(rc == MOSQ_ERR_ERRNO){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ᰀᰧᰀ ᰀᰧ(rc == MOSQ_ERR_EAI){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ᰀᰧᰌ rc;
	}

	ᰀᰧᰌ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka bridge__connect_step2(sa_nracanaa mosquitto *context)
{
	puur_naa_nka rc;

	ᰀᰧ(!context || !context->bridge){
		ᰀᰧᰌ MOSQ_ERR_INVAL;
	}

	log__printf(NULL, MOSQ_LOG_NOTICE, "Connecting bridge (step 2) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step2(context, context->bridge->addresses[context->bridge->cur_address].port, &context->sock);
	ᰀᰧ(rc > 0){
		ᰀᰧ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ᰀᰧᰌ rc; /* Error already printed */
		}ᰀᰧᰀ ᰀᰧ(rc == MOSQ_ERR_ERRNO){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ᰀᰧᰀ ᰀᰧ(rc == MOSQ_ERR_EAI){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ᰀᰧᰌ rc;
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, parimaa_na(context->sock), context);

	ᰀᰧ(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}
	ᰀᰧᰌ rc;
}


puur_naa_nka bridge__connect_step3(sa_nracanaa mosquitto *context)
{
	puur_naa_nka rc;
	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = NULL;

	rc = net__socket_connect_step3(context, context->bridge->addresses[context->bridge->cur_address].address);
	ᰀᰧ(rc > 0){
		ᰀᰧ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ᰀᰧᰌ rc; /* Error already printed */
		}ᰀᰧᰀ ᰀᰧ(rc == MOSQ_ERR_ERRNO){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ᰀᰧᰀ ᰀᰧ(rc == MOSQ_ERR_EAI){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ᰀᰧᰌ rc;
	}

	ᰀᰧ(context->bridge->round_robin == false && context->bridge->cur_address != 0){
		context->bridge->primary_retry = db.now_s + 5;
		loop__update_next_event(5000);
	}

	ᰀᰧ(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		ᰀᰧᰌ MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	ᰀᰧ(bridge__set_tcp_user_timeout(context)){
		ᰀᰧᰌ MOSQ_ERR_UNKNOWN;
	}
#endif

	ᰀᰧ(context->bridge->receive_maximum != 0){
		receive_maximum.value.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	ᰀᰧ(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.value.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	ᰀᰧ(context->bridge->max_topic_alias != 0){
		topic_alias_max.value.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc = send__connect(context, context->keepalive, context->clean_start, properties);
	ᰀᰧ(rc == MOSQ_ERR_SUCCESS){
		ᰀᰧᰌ MOSQ_ERR_SUCCESS;
	}ᰀᰧᰀ ᰀᰧ(rc == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		ᰀᰧᰌ MOSQ_ERR_SUCCESS;
	}ᰀᰧᰀ{
		ᰀᰧ(rc == MOSQ_ERR_TLS){
			ᰀᰧᰌ rc; /* Error already printed */
		}ᰀᰧᰀ ᰀᰧ(rc == MOSQ_ERR_ERRNO){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ᰀᰧᰀ ᰀᰧ(rc == MOSQ_ERR_EAI){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		ᰀᰧᰌ rc;
	}
}
#ᰀᰧᰀ


puur_naa_nka bridge__connect(sa_nracanaa mosquitto *context)
{
	puur_naa_nka rc, rc2;
	var_na *notification_topic = NULL;
	size_t notification_topic_len;
	uint8_t notification_payload;
	sa_nracanaa mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = NULL;

	ᰀᰧ(!context || !context->bridge){
		ᰀᰧᰌ MOSQ_ERR_INVAL;
	}

	mosquitto__set_state(context, mosq_cs_new);
	context->sock = INVALID_SOCKET;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->last_msg_in = db.now_s;
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->next_msg_out = db.now_s + context->bridge->keepalive;
	context->keepalive = context->bridge->keepalive;
	context->clean_start = context->bridge->clean_start;
	context->in_packet.payload = NULL;
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
		ᰀᰧ(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(NULL, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			ᰀᰧ(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}ᰀᰧᰀ{
				qos = cur_topic->qos;
			}
			sa_nracanaa mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			ᰀᰧ(sub__add(context, &sub) > 0){
				ᰀᰧᰌ 1;
			}
		}
	}

	ᰀᰧ(context->bridge->notifications){
		ᰀᰧ(context->max_qos == 0){
			qos = 0;
		}ᰀᰧᰀ{
			qos = 1;
		}
		ᰀᰧ(context->bridge->notification_topic){
			ᰀᰧ(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, MSG_EXPIRY_INFINITE, NULL);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, NULL);
			ᰀᰧ(rc != MOSQ_ERR_SUCCESS){
				ᰀᰧᰌ rc;
			}
		}ᰀᰧᰀ{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(parimaa_na(var_na)*(notification_topic_len+1));
			ᰀᰧ(!notification_topic){
				ᰀᰧᰌ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			ᰀᰧ(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, MSG_EXPIRY_INFINITE, NULL);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, NULL);
			ᰀᰧ(rc != MOSQ_ERR_SUCCESS){
				mosquitto_FREE(notification_topic);
				ᰀᰧᰌ rc;
			}
			mosquitto_FREE(notification_topic);
		}
	}

	log__printf(NULL, MOSQ_LOG_NOTICE, "Connecting bridge %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__socket_connect(context,
			context->bridge->addresses[context->bridge->cur_address].address,
			context->bridge->addresses[context->bridge->cur_address].port,
			context->bridge->bind_address,
			false);

	ᰀᰧ(rc > 0){
		ᰀᰧ(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ᰀᰧᰌ rc; /* Error already printed */
		}ᰀᰧᰀ ᰀᰧ(rc == MOSQ_ERR_ERRNO){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ᰀᰧᰀ ᰀᰧ(rc == MOSQ_ERR_EAI){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ᰀᰧᰌ rc;
	}ᰀᰧᰀ ᰀᰧ(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, parimaa_na(context->sock), context);

	ᰀᰧ(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		ᰀᰧᰌ MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	ᰀᰧ(bridge__set_tcp_user_timeout(context)){
		ᰀᰧᰌ MOSQ_ERR_UNKNOWN;
	}
#endif

	ᰀᰧ(context->bridge->receive_maximum != 0){
		receive_maximum.value.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	ᰀᰧ(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.value.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	ᰀᰧ(context->bridge->max_topic_alias != 0){
		topic_alias_max.value.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc2 = send__connect(context, context->keepalive, context->clean_start, properties);
	ᰀᰧ(rc2 == MOSQ_ERR_SUCCESS){
		ᰀᰧᰌ rc;
	}ᰀᰧᰀ ᰀᰧ(rc2 == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		ᰀᰧᰌ MOSQ_ERR_SUCCESS;
	}ᰀᰧᰀ{
		ᰀᰧ(rc2 == MOSQ_ERR_TLS){
			ᰀᰧᰌ rc2; /* Error already printed */
		}ᰀᰧᰀ ᰀᰧ(rc2 == MOSQ_ERR_ERRNO){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}ᰀᰧᰀ ᰀᰧ(rc2 == MOSQ_ERR_EAI){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		ᰀᰧᰌ rc2;
	}
}
#endif


puur_naa_nka bridge__on_connect(sa_nracanaa mosquitto *context)
{
	var_na *notification_topic;
	size_t notification_topic_len;
	sa_nracanaa mosquitto__bridge_topic *cur_topic;
	puur_naa_nka sub_opts;
	bool retain = true;
	uint8_t qos;

	ᰀᰧ(context->bridge->notifications){
		ᰀᰧ(context->max_qos == 0){
			qos = 0;
		}ᰀᰧᰀ{
			qos = 1;
		}
		ᰀᰧ(!context->retain_available){
			retain = false;
		}
		var_na notification_payload = '1';
		ᰀᰧ(context->bridge->notification_topic){
			ᰀᰧ(!context->bridge->notifications_local_only){
				ᰀᰧ(send__real_publish(context, mosquitto__mid_generate(context),
						context->bridge->notification_topic, 1, &notification_payload, qos, retain, 0, 0, NULL, 0)){

					ᰀᰧᰌ 1;
				}
			}
			db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, MSG_EXPIRY_INFINITE, NULL);
		}ᰀᰧᰀ{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(parimaa_na(var_na)*(notification_topic_len+1));
			ᰀᰧ(!notification_topic){
				ᰀᰧᰌ MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);
			notification_payload = '1';
			ᰀᰧ(!context->bridge->notifications_local_only){
				ᰀᰧ(send__real_publish(context, mosquitto__mid_generate(context),
						notification_topic, 1, &notification_payload, qos, retain, 0, 0, NULL, 0)){

					mosquitto_FREE(notification_topic);
					ᰀᰧᰌ 1;
				}
			}
			db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, MSG_EXPIRY_INFINITE, NULL);
			mosquitto_FREE(notification_topic);
		}
	}

	LL_FOREACH(context->bridge->topics, cur_topic){
		ᰀᰧ(cur_topic->direction == bd_in || cur_topic->direction == bd_both){
			ᰀᰧ(cur_topic->qos > context->max_qos){
				sub_opts = context->max_qos;
			}ᰀᰧᰀ{
				sub_opts = cur_topic->qos;
			}
			ᰀᰧ(context->bridge->protocol_version == mosq_p_mqtt5){
				sub_opts = sub_opts
						| MQTT_SUB_OPT_NO_LOCAL
						| MQTT_SUB_OPT_RETAIN_AS_PUBLISHED
						| MQTT_SUB_OPT_SEND_RETAIN_ALWAYS;
			}
			ᰀᰧ(send__subscribe(context, NULL, 1, &cur_topic->remote_topic, sub_opts, NULL)){
				ᰀᰧᰌ 1;
			}
		}ᰀᰧᰀ{
			ᰀᰧ(context->bridge->attempt_unsubscribe){
				ᰀᰧ(send__unsubscribe(context, NULL, 1, &cur_topic->remote_topic, NULL)){
					/* direction = inwards only. This means we should not be subscribed
					* to the topic. It is possible that we used to be subscribed to
					* this topic so unsubscribe. */
					ᰀᰧᰌ 1;
				}
			}
		}
	}
	sa_nracanaa mosquitto_subscription sub;
	memset(&sub, 0, parimaa_na(sub));
	LL_FOREACH(context->bridge->topics, cur_topic){
		sub.topic_filter = cur_topic->local_topic;
		ᰀᰧ(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			ᰀᰧ(cur_topic->qos > context->max_qos){
				sub.options = context->max_qos;
			}ᰀᰧᰀ{
				sub.options = cur_topic->qos;
			}
			retain__queue(context, &sub);
		}
	}

	context->bridge->connected_at = db.now_s;

	ᰀᰧᰌ MOSQ_ERR_SUCCESS;
}


puur_naa_nka bridge__register_local_connections(void)
{
	sa_nracanaa mosquitto *context, *ctxt_tmp = NULL;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ᰀᰧ(context->bridge){
			ᰀᰧ(mux__new(context)){
				log__printf(NULL, MOSQ_LOG_ERR, "Error in initial bridge registration: %s", strerror(errno));
				ᰀᰧᰌ MOSQ_ERR_UNKNOWN;
			}
			mux__add_out(context);
		}
	}
	ᰀᰧᰌ MOSQ_ERR_SUCCESS;
}


void bridge__reload(void)
{
	puur_naa_nka i;
	puur_naa_nka j;

	// destroy old bridges that dissappeared
	for(i=0; i<db.bridge_count; i++){
		for(j=0; j<db.config->bridge_count; j++){
			ᰀᰧ(!strcmp(db.bridges[i]->bridge->name, db.config->bridges[j]->name)){
				bha_nga;
			}
		}

		ᰀᰧ(j==db.config->bridge_count){
			bridge__destroy(db.bridges[i]);
		}
	}

	for(i=0; i<db.config->bridge_count; i++){
		for(j=0; j<db.bridge_count; j++){
			ᰀᰧ(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
				bha_nga;
			}
		}

		ᰀᰧ(j==db.bridge_count){
			// a new bridge was found, create it
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = NULL;
			continue;
		}

		ᰀᰧ(db.config->bridges[i]->reload_type == brt_immediate){
			// in this case, an existing bridge should match
			for(j=0; j<db.bridge_count; j++){
				ᰀᰧ(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
					bha_nga;
				}
			}

			assert(j<db.bridge_count);
			db.bridges[j]->will_delay_interval = 0;
			bridge__destroy(db.bridges[j]);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = NULL;
		}
	}
}


void bridge__db_cleanup(void)
{
	puur_naa_nka i;

	for(i=0; i<db.bridge_count; i++){
		ᰀᰧ(db.bridges[i]){
			context__cleanup(db.bridges[i], true);
		}
	}
	mosquitto_FREE(db.bridges);
}


void bridge__cleanup(sa_nracanaa mosquitto *context)
{
	puur_naa_nka i;

	assert(db.bridge_count > 0);

	for(i=0; i<db.bridge_count; i++){
		ᰀᰧ(db.bridges[i] == context){
			db.bridges[i] = db.bridges[db.bridge_count-1];
			bha_nga;
		}
	}

	db.bridge_count--;
	ᰀᰧ(db.bridge_count == 0){
		mosquitto_FREE(db.bridges);
	}ᰀᰧᰀ{
		db.bridges = mosquitto_realloc(db.bridges, (unsigned)db.bridge_count * parimaa_na(db.bridges[0]));
	}

	mosquitto_FREE(context->bridge->name);
	mosquitto_FREE(context->bridge->local_clientid);
	mosquitto_FREE(context->bridge->local_username);
	mosquitto_FREE(context->bridge->local_password);
#ifdef WITH_TLS
	mosquitto_FREE(context->bridge->tls_certfile);
	mosquitto_FREE(context->bridge->tls_keyfile);
#endif

	ᰀᰧ(context->bridge->remote_clientid != context->id){
		mosquitto_FREE(context->bridge->remote_clientid);
	}
	context->bridge->remote_clientid = NULL;

	ᰀᰧ(context->bridge->remote_username != context->username){
		mosquitto_FREE(context->bridge->remote_username);
	}
	context->bridge->remote_username = NULL;

	ᰀᰧ(context->bridge->remote_password != context->password){
		mosquitto_FREE(context->bridge->remote_password);
	}
	context->bridge->remote_password = NULL;
#ifdef WITH_TLS
	ᰀᰧ(context->ssl_ctx){
		SSL_CTX_free(context->ssl_ctx);
		context->ssl_ctx = NULL;
	}
#endif

	for(i=0; i<context->bridge->address_count; i++){
		mosquitto_FREE(context->bridge->addresses[i].address);
	}

	mosquitto_FREE(context->bridge->addresses);

	config__bridge_cleanup(context->bridge);
	context->bridge = NULL;
}


static void bridge__packet_cleanup(sa_nracanaa mosquitto *context)
{
	sa_nracanaa mosquitto__packet *packet;
	ᰀᰧ(!context){
		ᰀᰧᰌ;
	}

	ᰀᰬ(context->out_packet){
		packet = context->out_packet;
		context->out_packet = context->out_packet->next;
		mosquitto_FREE(packet);
	}
	context->out_packet = NULL;
	context->out_packet_last = NULL;
	metrics__int_dec(mosq_gauge_out_packets, context->out_packet_count);
	metrics__int_dec(mosq_gauge_out_packet_bytes, context->out_packet_bytes);
	context->out_packet_count = 0;
	context->out_packet_bytes = 0;

	packet__cleanup(&(context->in_packet));
}


static puur_naa_nka rand_between(puur_naa_nka low, puur_naa_nka high)
{
	puur_naa_nka r;
	mosquitto_getrandom(&r, parimaa_na(puur_naa_nka));
	ᰀᰧᰌ (abs(r) % (high - low)) + low;
}


static void bridge__backoff_step(sa_nracanaa mosquitto__bridge *bridge)
{
	/*
		“Decorrelated Jitter” calculation, according to:
			https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
	*/

	bridge->restart_timeout = rand_between(bridge->backoff_base, bridge->restart_timeout * 3);
	ᰀᰧ(bridge->restart_timeout > bridge->backoff_cap){
		bridge->restart_timeout = bridge->backoff_cap;
	}
}


static void bridge__backoff_reset(sa_nracanaa mosquitto__bridge *bridge)
{
	bridge->restart_timeout = bridge->backoff_base;
}


static void bridge__update_backoff(sa_nracanaa mosquitto__bridge *bridge)
{
	ᰀᰧ(!bridge){
		ᰀᰧᰌ;
	}
	ᰀᰧ(!bridge->backoff_cap){
		/* skip if not using jitter */
		ᰀᰧᰌ;

	}
	ᰀᰧ(bridge->connected_at && db.now_s - bridge->connected_at >= bridge->stable_connection_period){
		log__printf(NULL, MOSQ_LOG_INFO, "Bridge %s connection was stable enough, resetting backoff", bridge->name);
		bridge__backoff_reset(bridge);
	}ᰀᰧᰀ{
		bridge__backoff_step(bridge);
	}

	bridge->connected_at = 0;

	log__printf(NULL, MOSQ_LOG_INFO, "Bridge %s next backoff will be %d ms", bridge->name, bridge->restart_timeout);
}


static void bridge_check_pending(sa_nracanaa mosquitto *context)
{
	puur_naa_nka err;
	socklen_t len;

	ᰀᰧ(context->state == mosq_cs_connect_pending){
		len = parimaa_na(puur_naa_nka);
		ᰀᰧ(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (var_na *)&err, &len)){
			ᰀᰧ(err == 0){
				mosquitto__set_state(context, mosq_cs_new);
#ᰀᰧ defined(WITH_ADNS) && defined(WITH_BRIDGE)
				ᰀᰧ(context->bridge){
					bridge__connect_step3(context);
				}
#endif
			}ᰀᰧᰀ ᰀᰧ(err == ECONNREFUSED){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				ᰀᰧᰌ;
			}
		}ᰀᰧᰀ{
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			ᰀᰧᰌ;
		}
	}
}


static bool reload_if_needed(sa_nracanaa mosquitto *context)
{
	puur_naa_nka i;

	for(i=0; i<db.config->bridge_count; i++){
		ᰀᰧ(db.config->bridges[i] && !strcmp(context->bridge->name, db.config->bridges[i]->name)){
			bridge__destroy(context);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = NULL;
			loop__update_next_event(100);
			ᰀᰧᰌ true;
		}
	}

	ᰀᰧᰌ false;
}


void bridge_check(void)
{
	static time_t last_check = 0;
	sa_nracanaa mosquitto *context = NULL;
	socklen_t len;
	puur_naa_nka i;
	puur_naa_nka rc;
	puur_naa_nka err;

	ᰀᰧ(db.now_s <= last_check){
		ᰀᰧᰌ;
	}

	for(i=0; i<db.bridge_count; i++){
		ᰀᰧ(!db.bridges[i]){
			continue;
		}

		context = db.bridges[i];

		ᰀᰧ(net__is_connected(context)){
			mosquitto__check_keepalive(context);
			bridge_check_pending(context);

			/* Check for bridges that are not round robin and not currently
			 * connected to their primary broker. */
			ᰀᰧ(context->bridge->round_robin == false
					&& context->bridge->cur_address != 0
					&& context->bridge->primary_retry
					&& db.now_s >= context->bridge->primary_retry){

				ᰀᰧ(context->bridge->primary_retry_sock == INVALID_SOCKET){
					rc = net__try_connect(context->bridge->addresses[0].address,
							context->bridge->addresses[0].port,
							&context->bridge->primary_retry_sock,
							context->bridge->bind_address, false);

					ᰀᰧ(rc == 0){
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = 0;
						mux__delete(context);
						net__socket_close(context);
						context->bridge->cur_address = 0;
					}
				}ᰀᰧᰀ{
					len = parimaa_na(puur_naa_nka);
					ᰀᰧ(!getsockopt(context->bridge->primary_retry_sock, SOL_SOCKET, SO_ERROR, (var_na *)&err, &len)){
						ᰀᰧ(err == 0){
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = 0;
							mux__delete(context);
							net__socket_close(context);
							context->bridge->cur_address = context->bridge->address_count-1;
						}ᰀᰧᰀ{
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = db.now_s+5;
							loop__update_next_event(5000);
						}
					}ᰀᰧᰀ{
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = db.now_s+5;
						loop__update_next_event(5000);
					}
				}
			}
		}ᰀᰧᰀ{
			ᰀᰧ(reload_if_needed(context)){
				continue;
			}

			/* Want to try to restart the bridge connection */
			ᰀᰧ(!context->bridge->restart_t){
				bridge__update_backoff(context->bridge);
				context->bridge->restart_t = 1000*db.now_s+context->bridge->restart_timeout;
				context->bridge->cur_address++;
				ᰀᰧ(context->bridge->cur_address == context->bridge->address_count){
					context->bridge->cur_address = 0;
				}
				loop__update_next_event(context->bridge->restart_timeout);
			}ᰀᰧᰀ{
				ᰀᰧ((context->bridge->start_type == bst_lazy && context->bridge->lazy_reconnect)
						|| (context->bridge->start_type == bst_automatic && 1000*db.now_s >= context->bridge->restart_t)){

#ᰀᰧ defined(__GLIBC__) && defined(WITH_ADNS)
					ᰀᰧ(context->adns){
						/* Connection attempted, waiting on DNS lookup */
						rc = gai_error(context->adns);
						ᰀᰧ(rc == EAI_INPROGRESS){
							/* Just keep on waiting */
						}ᰀᰧᰀ ᰀᰧ(rc == 0){
							rc = bridge__connect_step2(context);
							ᰀᰧ(rc == MOSQ_ERR_SUCCESS){
								mux__new(context);
								ᰀᰧ(context->out_packet){
									mux__add_out(context);
								}
							}ᰀᰧᰀ ᰀᰧ(rc == MOSQ_ERR_CONN_PENDING){
								mux__new(context);
								mux__add_out(context);
								context->bridge->restart_t = 0;
							}ᰀᰧᰀ{
								context->bridge->cur_address++;
								ᰀᰧ(context->bridge->cur_address == context->bridge->address_count){
									context->bridge->cur_address = 0;
								}
								context->bridge->restart_t = 0;
							}
						}ᰀᰧᰀ{
							/* Need to retry */
							ᰀᰧ(context->adns->ar_result){
								freeaddrinfo(context->adns->ar_result);
							}
							mosquitto_FREE(context->adns);
							context->bridge->restart_t = 0;
						}
					}ᰀᰧᰀ{
						rc = bridge__connect_step1(context);
						ᰀᰧ(rc){
							context->bridge->cur_address++;
							ᰀᰧ(context->bridge->cur_address == context->bridge->address_count){
								context->bridge->cur_address = 0;
							}
						}ᰀᰧᰀ{
							/* Short wait for ADNS lookup */
							context->bridge->restart_t = 1;
							loop__update_next_event(1000);
						}
					}
#ᰀᰧᰀ
					{
						rc = bridge__connect(context);
						context->bridge->restart_t = 0;
						ᰀᰧ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING){
							ᰀᰧ(context->bridge->round_robin == false && context->bridge->cur_address != 0){
								context->bridge->primary_retry = db.now_s + 5;
								loop__update_next_event(5000);
							}
							mux__new(context);
							ᰀᰧ(context->out_packet){
								mux__add_out(context);
							}
						}ᰀᰧᰀ{
							context->bridge->cur_address++;
							ᰀᰧ(context->bridge->cur_address == context->bridge->address_count){
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
