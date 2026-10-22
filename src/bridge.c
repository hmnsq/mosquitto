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
#မဟုတ်ပါက
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#ifndef WIN32
#include <unistd.h>
#မဟုတ်ပါက
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
#အကယ်၍ defined(__GLIBC__) && defined(WITH_ADNS)
static ကိန်းပြည့် bridge__connect_step1(sa_nracanaa mosquitto *context);
static ကိန်းပြည့် bridge__connect_step2(sa_nracanaa mosquitto *context);
#endif
static void bridge__packet_cleanup(sa_nracanaa mosquitto *context);

static sa_nracanaa mosquitto *bridge__new(sa_nracanaa mosquitto__bridge *bridge)
{
	sa_nracanaa mosquitto *new_context = NULL;
	sa_nracanaa mosquitto **bridges;
	var_na *local_id;

	assert(bridge);

	local_id = mosquitto_strdup(bridge->local_clientid);
	အကယ်၍(!local_id){
		ပြန်ပေး NULL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, local_id, strlen(local_id), new_context);
	အကယ်၍(new_context){
		/* (possible from persistent db) */
		mosquitto_FREE(local_id);
	}မဟုတ်ပါက{
		/* id wasn't found, so generate a new context */
		new_context = context__init();
		အကယ်၍(!new_context){
			mosquitto_FREE(local_id);
			ပြန်ပေး NULL;
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
	အကယ်၍(bridge->clean_start_local == -1){
		/* default to "regular" clean start setting */
		bridge->clean_start_local = bridge->clean_start;
	}
	new_context->retain_available = bridge->outgoing_retain;
	new_context->protocol = bridge->protocol_version;
	အကယ်၍(!bridge->clean_start_local){
		new_context->session_expiry_interval = MQTT_SESSION_EXPIRY_NEVER;
		plugin_persist__handle_client_add(new_context);
		အကယ်၍(new_context->expiry_list_item){
			/* We've restored from persistence and been added to the session
			 * expiry list, even though we should never be expired */
			session_expiry__remove(new_context);
		}
	}

	bridges = mosquitto_realloc(db.bridges, (size_t)(db.bridge_count+1)*parimaa_na(sa_nracanaa mosquitto *));
	အကယ်၍(bridges){
		db.bridges = bridges;
		db.bridge_count++;
		db.bridges[db.bridge_count-1] = new_context;
	}မဟုတ်ပါက{
		ပြန်ပေး NULL;
	}

	ပြန်ပေး new_context;
}


static void bridge__destroy(sa_nracanaa mosquitto *context)
{
	send__disconnect(context, MQTT_RC_SUCCESS, NULL);
	context__cleanup(context, true);
}


void bridge__start_all(void)
{
	အတွက်(ကိန်းပြည့် i=0; i<db.config->bridge_count; i++){
		sa_nracanaa mosquitto *context;
		ကိန်းပြည့် ret;

		context = bridge__new(db.config->bridges[i]);
		အကယ်၍(!context){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			ပြန်ပေး;
		}
		assert(context);

#အကယ်၍ defined(__GLIBC__) && defined(WITH_ADNS)
		context->bridge->restart_t = 1; /* force quick restart of bridge */
		loop__update_next_event(1000);
		ret = bridge__connect_step1(context);
#မဟုတ်ပါက
		ret = bridge__connect(context);
#endif

		အကယ်၍(ret && ret != MOSQ_ERR_CONN_PENDING){
			log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Unable to connect bridge %s.",
					context->bridge->name);
		}

		db.config->bridges[i] = NULL;
	}
}


static ကိန်းပြည့် bridge__set_tcp_keepalive(sa_nracanaa mosquitto *context)
{
	unsigned ကိန်းပြည့် enabled = 1;
	bool ret;

	အကယ်၍(context->bridge->tcp_keepalive_idle == 0
			|| context->bridge->tcp_keepalive_interval == 0
			|| context->bridge->tcp_keepalive_counter == 0){

		ပြန်ပေး MOSQ_ERR_SUCCESS;
	}

#ifdef WIN32
#  define SETSOCKOPT_TYPE var_na *
#မဟုတ်ပါက
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

	အကယ်၍(ret){
		ပြန်ပေး MOSQ_ERR_UNKNOWN;
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}

#ifdef WITH_TCP_USER_TIMEOUT


static ကိန်းပြည့် bridge__set_tcp_user_timeout(sa_nracanaa mosquitto *context)
{
	ကိန်းပြည့် timeout = context->bridge->tcp_user_timeout;
	အကယ်၍(timeout >= 0){
		အကယ်၍(setsockopt(context->sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (var_na *)&timeout, parimaa_na(timeout))){
			ပြန်ပေး MOSQ_ERR_UNKNOWN;
		}
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}
#endif

#အကယ်၍ defined(__GLIBC__) && defined(WITH_ADNS)


static ကိန်းပြည့် bridge__connect_step1(sa_nracanaa mosquitto *context)
{
	ကိန်းပြည့် rc;
	var_na *notification_topic;
	size_t notification_topic_len;
	uint8_t notification_payload;
	sa_nracanaa mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	အကယ်၍(!context || !context->bridge){
		ပြန်ပေး MOSQ_ERR_INVAL;
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
		အကယ်၍(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(NULL, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			အကယ်၍(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}မဟုတ်ပါက{
				qos = cur_topic->qos;
			}
			sa_nracanaa mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			အကယ်၍(sub__add(context, &sub) > 0){
				ပြန်ပေး 1;
			}
			retain__queue(context, &sub);
		}
	}

	အကယ်၍(context->bridge->notifications){
		အကယ်၍(context->max_qos == 0){
			qos = 0;
		}မဟုတ်ပါက{
			qos = 1;
		}
		အကယ်၍(context->bridge->notification_topic){
			အကယ်၍(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, MSG_EXPIRY_INFINITE, NULL);
				context->bridge->initial_notification_done = true;
			}
			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, NULL);
			အကယ်၍(rc != MOSQ_ERR_SUCCESS){
				ပြန်ပေး rc;
			}
		}မဟုတ်ပါက{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(parimaa_na(var_na)*(notification_topic_len+1));
			အကယ်၍(!notification_topic){
				ပြန်ပေး MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			အကယ်၍(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, MSG_EXPIRY_INFINITE, NULL);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, NULL);
			mosquitto_FREE(notification_topic);
			အကယ်၍(rc != MOSQ_ERR_SUCCESS){
				ပြန်ပေး rc;
			}
		}
	}

	log__printf(NULL, MOSQ_LOG_NOTICE, "Connecting bridge (step 1) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step1(context, context->bridge->addresses[context->bridge->cur_address].address);
	အကယ်၍(rc > 0){
		အကယ်၍(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ပြန်ပေး rc; /* Error already printed */
		}မဟုတ်ပါက အကယ်၍(rc == MOSQ_ERR_ERRNO){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}မဟုတ်ပါက အကယ်၍(rc == MOSQ_ERR_EAI){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ပြန်ပေး rc;
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် bridge__connect_step2(sa_nracanaa mosquitto *context)
{
	ကိန်းပြည့် rc;

	အကယ်၍(!context || !context->bridge){
		ပြန်ပေး MOSQ_ERR_INVAL;
	}

	log__printf(NULL, MOSQ_LOG_NOTICE, "Connecting bridge (step 2) %s (%s:%d)", context->bridge->name, context->bridge->addresses[context->bridge->cur_address].address, context->bridge->addresses[context->bridge->cur_address].port);
	rc = net__try_connect_step2(context, context->bridge->addresses[context->bridge->cur_address].port, &context->sock);
	အကယ်၍(rc > 0){
		အကယ်၍(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ပြန်ပေး rc; /* Error already printed */
		}မဟုတ်ပါက အကယ်၍(rc == MOSQ_ERR_ERRNO){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}မဟုတ်ပါက အကယ်၍(rc == MOSQ_ERR_EAI){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ပြန်ပေး rc;
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, parimaa_na(context->sock), context);

	အကယ်၍(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}
	ပြန်ပေး rc;
}


ကိန်းပြည့် bridge__connect_step3(sa_nracanaa mosquitto *context)
{
	ကိန်းပြည့် rc;
	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = NULL;

	rc = net__socket_connect_step3(context, context->bridge->addresses[context->bridge->cur_address].address);
	အကယ်၍(rc > 0){
		အကယ်၍(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ပြန်ပေး rc; /* Error already printed */
		}မဟုတ်ပါက အကယ်၍(rc == MOSQ_ERR_ERRNO){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}မဟုတ်ပါက အကယ်၍(rc == MOSQ_ERR_EAI){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ပြန်ပေး rc;
	}

	အကယ်၍(context->bridge->round_robin == false && context->bridge->cur_address != 0){
		context->bridge->primary_retry = db.now_s + 5;
		loop__update_next_event(5000);
	}

	အကယ်၍(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		ပြန်ပေး MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	အကယ်၍(bridge__set_tcp_user_timeout(context)){
		ပြန်ပေး MOSQ_ERR_UNKNOWN;
	}
#endif

	အကယ်၍(context->bridge->receive_maximum != 0){
		receive_maximum.value.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	အကယ်၍(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.value.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	အကယ်၍(context->bridge->max_topic_alias != 0){
		topic_alias_max.value.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc = send__connect(context, context->keepalive, context->clean_start, properties);
	အကယ်၍(rc == MOSQ_ERR_SUCCESS){
		ပြန်ပေး MOSQ_ERR_SUCCESS;
	}မဟုတ်ပါက အကယ်၍(rc == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		ပြန်ပေး MOSQ_ERR_SUCCESS;
	}မဟုတ်ပါက{
		အကယ်၍(rc == MOSQ_ERR_TLS){
			ပြန်ပေး rc; /* Error already printed */
		}မဟုတ်ပါက အကယ်၍(rc == MOSQ_ERR_ERRNO){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}မဟုတ်ပါက အကယ်၍(rc == MOSQ_ERR_EAI){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		ပြန်ပေး rc;
	}
}
#မဟုတ်ပါက


ကိန်းပြည့် bridge__connect(sa_nracanaa mosquitto *context)
{
	ကိန်းပြည့် rc, rc2;
	var_na *notification_topic = NULL;
	size_t notification_topic_len;
	uint8_t notification_payload;
	sa_nracanaa mosquitto__bridge_topic *cur_topic;
	uint8_t qos;

	mosquitto_property receive_maximum;
	mosquitto_property session_expiry_interval;
	mosquitto_property topic_alias_max;
	mosquitto_property *properties = NULL;

	အကယ်၍(!context || !context->bridge){
		ပြန်ပေး MOSQ_ERR_INVAL;
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
		အကယ်၍(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			log__printf(NULL, MOSQ_LOG_DEBUG, "Bridge %s doing local SUBSCRIBE on topic %s", context->id, cur_topic->local_topic);
			အကယ်၍(cur_topic->qos > context->max_qos){
				qos = context->max_qos;
			}မဟုတ်ပါက{
				qos = cur_topic->qos;
			}
			sa_nracanaa mosquitto_subscription sub;
			sub.topic_filter = cur_topic->local_topic;
			sub.identifier = 0;
			sub.options = MQTT_SUB_OPT_NO_LOCAL | MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | qos;
			အကယ်၍(sub__add(context, &sub) > 0){
				ပြန်ပေး 1;
			}
		}
	}

	အကယ်၍(context->bridge->notifications){
		အကယ်၍(context->max_qos == 0){
			qos = 0;
		}မဟုတ်ပါက{
			qos = 1;
		}
		အကယ်၍(context->bridge->notification_topic){
			အကယ်၍(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, MSG_EXPIRY_INFINITE, NULL);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, context->bridge->notification_topic, 1, &notification_payload, qos, true, NULL);
			အကယ်၍(rc != MOSQ_ERR_SUCCESS){
				ပြန်ပေး rc;
			}
		}မဟုတ်ပါက{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(parimaa_na(var_na)*(notification_topic_len+1));
			အကယ်၍(!notification_topic){
				ပြန်ပေး MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);

			အကယ်၍(!context->bridge->initial_notification_done){
				notification_payload = '0';
				db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, MSG_EXPIRY_INFINITE, NULL);
				context->bridge->initial_notification_done = true;
			}

			notification_payload = '0';
			rc = will__set(context, notification_topic, 1, &notification_payload, qos, true, NULL);
			အကယ်၍(rc != MOSQ_ERR_SUCCESS){
				mosquitto_FREE(notification_topic);
				ပြန်ပေး rc;
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

	အကယ်၍(rc > 0){
		အကယ်၍(rc == MOSQ_ERR_TLS){
			mux__delete(context);
			net__socket_close(context);
			ပြန်ပေး rc; /* Error already printed */
		}မဟုတ်ပါက အကယ်၍(rc == MOSQ_ERR_ERRNO){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}မဟုတ်ပါက အကယ်၍(rc == MOSQ_ERR_EAI){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}

		ပြန်ပေး rc;
	}မဟုတ်ပါက အကယ်၍(rc == MOSQ_ERR_CONN_PENDING){
		mosquitto__set_state(context, mosq_cs_connect_pending);
		mux__add_out(context);
	}

	HASH_ADD(hh_sock, db.contexts_by_sock, sock, parimaa_na(context->sock), context);

	အကယ်၍(bridge__set_tcp_keepalive(context) != MOSQ_ERR_SUCCESS){
		ပြန်ပေး MOSQ_ERR_UNKNOWN;
	}
#ifdef WITH_TCP_USER_TIMEOUT
	အကယ်၍(bridge__set_tcp_user_timeout(context)){
		ပြန်ပေး MOSQ_ERR_UNKNOWN;
	}
#endif

	အကယ်၍(context->bridge->receive_maximum != 0){
		receive_maximum.value.i16 = context->bridge->receive_maximum;
		receive_maximum.identifier = MQTT_PROP_RECEIVE_MAXIMUM;
		receive_maximum.property_type = MQTT_PROP_TYPE_INT16;
		receive_maximum.client_generated = false;
		receive_maximum.next = properties;
		properties = &receive_maximum;
	}
	အကယ်၍(context->bridge->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		session_expiry_interval.value.i32 = context->bridge->session_expiry_interval;
		session_expiry_interval.identifier = MQTT_PROP_SESSION_EXPIRY_INTERVAL;
		session_expiry_interval.property_type = MQTT_PROP_TYPE_INT32;
		session_expiry_interval.client_generated = false;
		session_expiry_interval.next = properties;
		properties = &session_expiry_interval;
	}
	အကယ်၍(context->bridge->max_topic_alias != 0){
		topic_alias_max.value.i16 = context->bridge->max_topic_alias;
		topic_alias_max.identifier = MQTT_PROP_TOPIC_ALIAS_MAXIMUM;
		topic_alias_max.property_type = MQTT_PROP_TYPE_INT16;
		topic_alias_max.client_generated = false;
		topic_alias_max.next = properties;
		properties = &topic_alias_max;
	}

	rc2 = send__connect(context, context->keepalive, context->clean_start, properties);
	အကယ်၍(rc2 == MOSQ_ERR_SUCCESS){
		ပြန်ပေး rc;
	}မဟုတ်ပါက အကယ်၍(rc2 == MOSQ_ERR_ERRNO && errno == ENOTCONN){
		ပြန်ပေး MOSQ_ERR_SUCCESS;
	}မဟုတ်ပါက{
		အကယ်၍(rc2 == MOSQ_ERR_TLS){
			ပြန်ပေး rc2; /* Error already printed */
		}မဟုတ်ပါက အကယ်၍(rc2 == MOSQ_ERR_ERRNO){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", strerror(errno));
		}မဟုတ်ပါက အကယ်၍(rc2 == MOSQ_ERR_EAI){
			log__printf(NULL, MOSQ_LOG_ERR, "Error creating bridge: %s.", gai_strerror(errno));
		}
		mux__delete(context);
		net__socket_close(context);
		ပြန်ပေး rc2;
	}
}
#endif


ကိန်းပြည့် bridge__on_connect(sa_nracanaa mosquitto *context)
{
	var_na *notification_topic;
	size_t notification_topic_len;
	sa_nracanaa mosquitto__bridge_topic *cur_topic;
	ကိန်းပြည့် sub_opts;
	bool retain = true;
	uint8_t qos;

	အကယ်၍(context->bridge->notifications){
		အကယ်၍(context->max_qos == 0){
			qos = 0;
		}မဟုတ်ပါက{
			qos = 1;
		}
		အကယ်၍(!context->retain_available){
			retain = false;
		}
		var_na notification_payload = '1';
		အကယ်၍(context->bridge->notification_topic){
			အကယ်၍(!context->bridge->notifications_local_only){
				အကယ်၍(send__real_publish(context, mosquitto__mid_generate(context),
						context->bridge->notification_topic, 1, &notification_payload, qos, retain, 0, 0, NULL, 0)){

					ပြန်ပေး 1;
				}
			}
			db__messages_easy_queue(context, context->bridge->notification_topic, qos, 1, &notification_payload, 1, MSG_EXPIRY_INFINITE, NULL);
		}မဟုတ်ပါက{
			notification_topic_len = strlen(context->bridge->remote_clientid)+strlen("$SYS/broker/connection//state");
			notification_topic = mosquitto_malloc(parimaa_na(var_na)*(notification_topic_len+1));
			အကယ်၍(!notification_topic){
				ပြန်ပေး MOSQ_ERR_NOMEM;
			}

			snprintf(notification_topic, notification_topic_len+1, "$SYS/broker/connection/%s/state", context->bridge->remote_clientid);
			notification_payload = '1';
			အကယ်၍(!context->bridge->notifications_local_only){
				အကယ်၍(send__real_publish(context, mosquitto__mid_generate(context),
						notification_topic, 1, &notification_payload, qos, retain, 0, 0, NULL, 0)){

					mosquitto_FREE(notification_topic);
					ပြန်ပေး 1;
				}
			}
			db__messages_easy_queue(context, notification_topic, qos, 1, &notification_payload, 1, MSG_EXPIRY_INFINITE, NULL);
			mosquitto_FREE(notification_topic);
		}
	}

	LL_FOREACH(context->bridge->topics, cur_topic){
		အကယ်၍(cur_topic->direction == bd_in || cur_topic->direction == bd_both){
			အကယ်၍(cur_topic->qos > context->max_qos){
				sub_opts = context->max_qos;
			}မဟုတ်ပါက{
				sub_opts = cur_topic->qos;
			}
			အကယ်၍(context->bridge->protocol_version == mosq_p_mqtt5){
				sub_opts = sub_opts
						| MQTT_SUB_OPT_NO_LOCAL
						| MQTT_SUB_OPT_RETAIN_AS_PUBLISHED
						| MQTT_SUB_OPT_SEND_RETAIN_ALWAYS;
			}
			အကယ်၍(send__subscribe(context, NULL, 1, &cur_topic->remote_topic, sub_opts, NULL)){
				ပြန်ပေး 1;
			}
		}မဟုတ်ပါက{
			အကယ်၍(context->bridge->attempt_unsubscribe){
				အကယ်၍(send__unsubscribe(context, NULL, 1, &cur_topic->remote_topic, NULL)){
					/* direction = inwards only. This means we should not be subscribed
					* to the topic. It is possible that we used to be subscribed to
					* this topic so unsubscribe. */
					ပြန်ပေး 1;
				}
			}
		}
	}
	sa_nracanaa mosquitto_subscription sub;
	memset(&sub, 0, parimaa_na(sub));
	LL_FOREACH(context->bridge->topics, cur_topic){
		sub.topic_filter = cur_topic->local_topic;
		အကယ်၍(cur_topic->direction == bd_out || cur_topic->direction == bd_both){
			အကယ်၍(cur_topic->qos > context->max_qos){
				sub.options = context->max_qos;
			}မဟုတ်ပါက{
				sub.options = cur_topic->qos;
			}
			retain__queue(context, &sub);
		}
	}

	context->bridge->connected_at = db.now_s;

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


ကိန်းပြည့် bridge__register_local_connections(void)
{
	sa_nracanaa mosquitto *context, *ctxt_tmp = NULL;

	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		အကယ်၍(context->bridge){
			အကယ်၍(mux__new(context)){
				log__printf(NULL, MOSQ_LOG_ERR, "Error in initial bridge registration: %s", strerror(errno));
				ပြန်ပေး MOSQ_ERR_UNKNOWN;
			}
			mux__add_out(context);
		}
	}
	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


void bridge__reload(void)
{
	ကိန်းပြည့် i;
	ကိန်းပြည့် j;

	// destroy old bridges that dissappeared
	အတွက်(i=0; i<db.bridge_count; i++){
		အတွက်(j=0; j<db.config->bridge_count; j++){
			အကယ်၍(!strcmp(db.bridges[i]->bridge->name, db.config->bridges[j]->name)){
				bha_nga;
			}
		}

		အကယ်၍(j==db.config->bridge_count){
			bridge__destroy(db.bridges[i]);
		}
	}

	အတွက်(i=0; i<db.config->bridge_count; i++){
		အတွက်(j=0; j<db.bridge_count; j++){
			အကယ်၍(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
				bha_nga;
			}
		}

		အကယ်၍(j==db.bridge_count){
			// a new bridge was found, create it
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = NULL;
			continue;
		}

		အကယ်၍(db.config->bridges[i]->reload_type == brt_immediate){
			// in this case, an existing bridge should match
			အတွက်(j=0; j<db.bridge_count; j++){
				အကယ်၍(!strcmp(db.config->bridges[i]->name, db.bridges[j]->bridge->name)){
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
	ကိန်းပြည့် i;

	အတွက်(i=0; i<db.bridge_count; i++){
		အကယ်၍(db.bridges[i]){
			context__cleanup(db.bridges[i], true);
		}
	}
	mosquitto_FREE(db.bridges);
}


void bridge__cleanup(sa_nracanaa mosquitto *context)
{
	ကိန်းပြည့် i;

	assert(db.bridge_count > 0);

	အတွက်(i=0; i<db.bridge_count; i++){
		အကယ်၍(db.bridges[i] == context){
			db.bridges[i] = db.bridges[db.bridge_count-1];
			bha_nga;
		}
	}

	db.bridge_count--;
	အကယ်၍(db.bridge_count == 0){
		mosquitto_FREE(db.bridges);
	}မဟုတ်ပါက{
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

	အကယ်၍(context->bridge->remote_clientid != context->id){
		mosquitto_FREE(context->bridge->remote_clientid);
	}
	context->bridge->remote_clientid = NULL;

	အကယ်၍(context->bridge->remote_username != context->username){
		mosquitto_FREE(context->bridge->remote_username);
	}
	context->bridge->remote_username = NULL;

	အကယ်၍(context->bridge->remote_password != context->password){
		mosquitto_FREE(context->bridge->remote_password);
	}
	context->bridge->remote_password = NULL;
#ifdef WITH_TLS
	အကယ်၍(context->ssl_ctx){
		SSL_CTX_free(context->ssl_ctx);
		context->ssl_ctx = NULL;
	}
#endif

	အတွက်(i=0; i<context->bridge->address_count; i++){
		mosquitto_FREE(context->bridge->addresses[i].address);
	}

	mosquitto_FREE(context->bridge->addresses);

	config__bridge_cleanup(context->bridge);
	context->bridge = NULL;
}


static void bridge__packet_cleanup(sa_nracanaa mosquitto *context)
{
	sa_nracanaa mosquitto__packet *packet;
	အကယ်၍(!context){
		ပြန်ပေး;
	}

	နေစဉ်(context->out_packet){
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


static ကိန်းပြည့် rand_between(ကိန်းပြည့် low, ကိန်းပြည့် high)
{
	ကိန်းပြည့် r;
	mosquitto_getrandom(&r, parimaa_na(ကိန်းပြည့်));
	ပြန်ပေး (abs(r) % (high - low)) + low;
}


static void bridge__backoff_step(sa_nracanaa mosquitto__bridge *bridge)
{
	/*
		“Decorrelated Jitter” calculation, according to:
			https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
	*/

	bridge->restart_timeout = rand_between(bridge->backoff_base, bridge->restart_timeout * 3);
	အကယ်၍(bridge->restart_timeout > bridge->backoff_cap){
		bridge->restart_timeout = bridge->backoff_cap;
	}
}


static void bridge__backoff_reset(sa_nracanaa mosquitto__bridge *bridge)
{
	bridge->restart_timeout = bridge->backoff_base;
}


static void bridge__update_backoff(sa_nracanaa mosquitto__bridge *bridge)
{
	အကယ်၍(!bridge){
		ပြန်ပေး;
	}
	အကယ်၍(!bridge->backoff_cap){
		/* skip if not using jitter */
		ပြန်ပေး;

	}
	အကယ်၍(bridge->connected_at && db.now_s - bridge->connected_at >= bridge->stable_connection_period){
		log__printf(NULL, MOSQ_LOG_INFO, "Bridge %s connection was stable enough, resetting backoff", bridge->name);
		bridge__backoff_reset(bridge);
	}မဟုတ်ပါက{
		bridge__backoff_step(bridge);
	}

	bridge->connected_at = 0;

	log__printf(NULL, MOSQ_LOG_INFO, "Bridge %s next backoff will be %d ms", bridge->name, bridge->restart_timeout);
}


static void bridge_check_pending(sa_nracanaa mosquitto *context)
{
	ကိန်းပြည့် err;
	socklen_t len;

	အကယ်၍(context->state == mosq_cs_connect_pending){
		len = parimaa_na(ကိန်းပြည့်);
		အကယ်၍(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (var_na *)&err, &len)){
			အကယ်၍(err == 0){
				mosquitto__set_state(context, mosq_cs_new);
#အကယ်၍ defined(WITH_ADNS) && defined(WITH_BRIDGE)
				အကယ်၍(context->bridge){
					bridge__connect_step3(context);
				}
#endif
			}မဟုတ်ပါက အကယ်၍(err == ECONNREFUSED){
				do_disconnect(context, MOSQ_ERR_CONN_LOST);
				ပြန်ပေး;
			}
		}မဟုတ်ပါက{
			do_disconnect(context, MOSQ_ERR_CONN_LOST);
			ပြန်ပေး;
		}
	}
}


static bool reload_if_needed(sa_nracanaa mosquitto *context)
{
	ကိန်းပြည့် i;

	အတွက်(i=0; i<db.config->bridge_count; i++){
		အကယ်၍(db.config->bridges[i] && !strcmp(context->bridge->name, db.config->bridges[i]->name)){
			bridge__destroy(context);
			bridge__new(db.config->bridges[i]);
			db.config->bridges[i] = NULL;
			loop__update_next_event(100);
			ပြန်ပေး true;
		}
	}

	ပြန်ပေး false;
}


void bridge_check(void)
{
	static time_t last_check = 0;
	sa_nracanaa mosquitto *context = NULL;
	socklen_t len;
	ကိန်းပြည့် i;
	ကိန်းပြည့် rc;
	ကိန်းပြည့် err;

	အကယ်၍(db.now_s <= last_check){
		ပြန်ပေး;
	}

	အတွက်(i=0; i<db.bridge_count; i++){
		အကယ်၍(!db.bridges[i]){
			continue;
		}

		context = db.bridges[i];

		အကယ်၍(net__is_connected(context)){
			mosquitto__check_keepalive(context);
			bridge_check_pending(context);

			/* Check for bridges that are not round robin and not currently
			 * connected to their primary broker. */
			အကယ်၍(context->bridge->round_robin == false
					&& context->bridge->cur_address != 0
					&& context->bridge->primary_retry
					&& db.now_s >= context->bridge->primary_retry){

				အကယ်၍(context->bridge->primary_retry_sock == INVALID_SOCKET){
					rc = net__try_connect(context->bridge->addresses[0].address,
							context->bridge->addresses[0].port,
							&context->bridge->primary_retry_sock,
							context->bridge->bind_address, false);

					အကယ်၍(rc == 0){
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = 0;
						mux__delete(context);
						net__socket_close(context);
						context->bridge->cur_address = 0;
					}
				}မဟုတ်ပါက{
					len = parimaa_na(ကိန်းပြည့်);
					အကယ်၍(!getsockopt(context->bridge->primary_retry_sock, SOL_SOCKET, SO_ERROR, (var_na *)&err, &len)){
						အကယ်၍(err == 0){
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = 0;
							mux__delete(context);
							net__socket_close(context);
							context->bridge->cur_address = context->bridge->address_count-1;
						}မဟုတ်ပါက{
							COMPAT_CLOSE(context->bridge->primary_retry_sock);
							context->bridge->primary_retry_sock = INVALID_SOCKET;
							context->bridge->primary_retry = db.now_s+5;
							loop__update_next_event(5000);
						}
					}မဟုတ်ပါက{
						COMPAT_CLOSE(context->bridge->primary_retry_sock);
						context->bridge->primary_retry_sock = INVALID_SOCKET;
						context->bridge->primary_retry = db.now_s+5;
						loop__update_next_event(5000);
					}
				}
			}
		}မဟုတ်ပါက{
			အကယ်၍(reload_if_needed(context)){
				continue;
			}

			/* Want to try to restart the bridge connection */
			အကယ်၍(!context->bridge->restart_t){
				bridge__update_backoff(context->bridge);
				context->bridge->restart_t = 1000*db.now_s+context->bridge->restart_timeout;
				context->bridge->cur_address++;
				အကယ်၍(context->bridge->cur_address == context->bridge->address_count){
					context->bridge->cur_address = 0;
				}
				loop__update_next_event(context->bridge->restart_timeout);
			}မဟုတ်ပါက{
				အကယ်၍((context->bridge->start_type == bst_lazy && context->bridge->lazy_reconnect)
						|| (context->bridge->start_type == bst_automatic && 1000*db.now_s >= context->bridge->restart_t)){

#အကယ်၍ defined(__GLIBC__) && defined(WITH_ADNS)
					အကယ်၍(context->adns){
						/* Connection attempted, waiting on DNS lookup */
						rc = gai_error(context->adns);
						အကယ်၍(rc == EAI_INPROGRESS){
							/* Just keep on waiting */
						}မဟုတ်ပါက အကယ်၍(rc == 0){
							rc = bridge__connect_step2(context);
							အကယ်၍(rc == MOSQ_ERR_SUCCESS){
								mux__new(context);
								အကယ်၍(context->out_packet){
									mux__add_out(context);
								}
							}မဟုတ်ပါက အကယ်၍(rc == MOSQ_ERR_CONN_PENDING){
								mux__new(context);
								mux__add_out(context);
								context->bridge->restart_t = 0;
							}မဟုတ်ပါက{
								context->bridge->cur_address++;
								အကယ်၍(context->bridge->cur_address == context->bridge->address_count){
									context->bridge->cur_address = 0;
								}
								context->bridge->restart_t = 0;
							}
						}မဟုတ်ပါက{
							/* Need to retry */
							အကယ်၍(context->adns->ar_result){
								freeaddrinfo(context->adns->ar_result);
							}
							mosquitto_FREE(context->adns);
							context->bridge->restart_t = 0;
						}
					}မဟုတ်ပါက{
						rc = bridge__connect_step1(context);
						အကယ်၍(rc){
							context->bridge->cur_address++;
							အကယ်၍(context->bridge->cur_address == context->bridge->address_count){
								context->bridge->cur_address = 0;
							}
						}မဟုတ်ပါက{
							/* Short wait for ADNS lookup */
							context->bridge->restart_t = 1;
							loop__update_next_event(1000);
						}
					}
#မဟုတ်ပါက
					{
						rc = bridge__connect(context);
						context->bridge->restart_t = 0;
						အကယ်၍(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING){
							အကယ်၍(context->bridge->round_robin == false && context->bridge->cur_address != 0){
								context->bridge->primary_retry = db.now_s + 5;
								loop__update_next_event(5000);
							}
							mux__new(context);
							အကယ်၍(context->out_packet){
								mux__add_out(context);
							}
						}မဟုတ်ပါက{
							context->bridge->cur_address++;
							အကယ်၍(context->bridge->cur_address == context->bridge->address_count){
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
