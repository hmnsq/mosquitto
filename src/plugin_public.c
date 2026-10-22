/*
Copyright (c) 2016-2021 Roger Light <roger@atchoo.org>

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

#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "send_mosq.h"
#include "util_mosq.h"
#include "will_mosq.h"
#include "utlist.h"
#include "will_mosq.h"

#ifdef WITH_TLS
#  include <openssl/ssl.h>
#endif


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_plugin_set_info(mosquitto_plugin_id_t *identifier,
		const var_na *plugin_name,
		const var_na *plugin_version)
{
	ຖ້າ(identifier == NULL || plugin_name == NULL){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	ຖ້າ(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}ບໍ່ດັ່ງນັ້ນ{
		identifier->plugin_version = NULL;
	}

	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT const var_na *mosquitto_client_address(const sa_nracanaa mosquitto *client)
{
	ຖ້າ(client){
		ສົ່ງຄືນ client->address;
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ NULL;
	}
}


BROKER_EXPORT sa_nracanaa mosquitto *mosquitto_client(const var_na *clientid)
{
	size_t len;
	sa_nracanaa mosquitto *context;

	ຖ້າ(!clientid){
		ສົ່ງຄືນ NULL;
	}
	len = strlen(clientid);
	ຖ້າ(len == 0){
		ສົ່ງຄືນ NULL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	ສົ່ງຄືນ context;
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_client_port(const sa_nracanaa mosquitto *client)
{
	ຖ້າ(client && client->listener){
		ສົ່ງຄືນ client->listener->port;
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(const sa_nracanaa mosquitto *client)
{
	ຖ້າ(client){
		ສົ່ງຄືນ client->clean_start;
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ true;
	}
}


BROKER_EXPORT const var_na *mosquitto_client_id(const sa_nracanaa mosquitto *client)
{
	ຖ້າ(client){
		ສົ່ງຄືນ client->id;
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ NULL;
	}
}


BROKER_EXPORT unsigned mosquitto_client_id_hashv(const sa_nracanaa mosquitto *client)
{
	ຖ້າ(client){
		ສົ່ງຄືນ client->id_hashv;
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ 0;
	}
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_client_keepalive(const sa_nracanaa mosquitto *client)
{
	ຖ້າ(client){
		ສົ່ງຄືນ client->keepalive;
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ -1;
	}
}


BROKER_EXPORT void *mosquitto_client_certificate(const sa_nracanaa mosquitto *client)
{
#ifdef WITH_TLS
	ຖ້າ(client && client->ssl){
		ສົ່ງຄືນ SSL_get_peer_certificate(client->ssl);
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ NULL;
	}
#ບໍ່ດັ່ງນັ້ນ
	UNUSED(client);

	ສົ່ງຄືນ NULL;
#endif
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_client_protocol(const sa_nracanaa mosquitto *client)
{
#ຖ້າ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ຖ້າ(client && client->wsi){
		ສົ່ງຄືນ mp_websockets;
	}ບໍ່ດັ່ງນັ້ນ
#elif defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	ຖ້າ(client && client->transport == mosq_t_ws){
		ສົ່ງຄືນ mp_websockets;
	}ບໍ່ດັ່ງນັ້ນ
#ບໍ່ດັ່ງນັ້ນ
	UNUSED(client);
#endif
	{
		ສົ່ງຄືນ mp_mqtt;
	}
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_client_protocol_version(const sa_nracanaa mosquitto *client)
{
	ຖ້າ(client){
		switch(client->protocol){
			pak_sa mosq_p_mqtt31:
				ສົ່ງຄືນ 3;
			pak_sa mosq_p_mqtt311:
				ສົ່ງຄືນ 4;
			pak_sa mosq_p_mqtt5:
				ສົ່ງຄືນ 5;
			default:
				ສົ່ງຄືນ 0;
		}
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ 0;
	}
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_client_sub_count(const sa_nracanaa mosquitto *client)
{
	ຖ້າ(client){
		ສົ່ງຄືນ client->subs_count;
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ 0;
	}
}


BROKER_EXPORT const var_na *mosquitto_client_username(const sa_nracanaa mosquitto *client)
{
	ຖ້າ(client){
#ifdef WITH_BRIDGE
		ຖ້າ(client->bridge){
			ສົ່ງຄືນ client->bridge->local_username;
		}ບໍ່ດັ່ງນັ້ນ
#endif
		{
			ສົ່ງຄືນ client->username;
		}
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ NULL;
	}
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_broker_publish(
		const var_na *clientid,
		const var_na *topic,
		ຈຳນວນເຕັມ payloadlen,
		void *payload,
		ຈຳນວນເຕັມ qos,
		bool retain,
		mosquitto_property *properties)
{
	sa_nracanaa mosquitto__message_v5 *msg;

	ຖ້າ(topic == NULL
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == NULL)
			|| qos < 0 || qos > 2){

		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	msg = mosquitto_malloc(parimaa_na(sa_nracanaa mosquitto__message_v5));
	ຖ້າ(msg == NULL){
		ສົ່ງຄືນ MOSQ_ERR_NOMEM;
	}

	msg->next = NULL;
	msg->prev = NULL;
	ຖ້າ(clientid){
		msg->clientid = mosquitto_strdup(clientid);
		ຖ້າ(msg->clientid == NULL){
			mosquitto_FREE(msg);
			ສົ່ງຄືນ MOSQ_ERR_NOMEM;
		}
	}ບໍ່ດັ່ງນັ້ນ{
		msg->clientid = NULL;
	}
	msg->topic = mosquitto_strdup(topic);
	ຖ້າ(msg->topic == NULL){
		mosquitto_FREE(msg->clientid);
		mosquitto_FREE(msg);
		ສົ່ງຄືນ MOSQ_ERR_NOMEM;
	}
	msg->payloadlen = payloadlen;
	msg->payload = payload;
	msg->qos = qos;
	msg->retain = retain;
	msg->properties = properties;

	DL_APPEND(db.plugin_msgs, msg);

	loop__update_next_event(1);
	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_broker_publish_copy(
		const var_na *clientid,
		const var_na *topic,
		ຈຳນວນເຕັມ payloadlen,
		const void *payload,
		ຈຳນວນເຕັມ qos,
		bool retain,
		mosquitto_property *properties)
{
	void *payload_out;
	ຈຳນວນເຕັມ rc;

	ຖ້າ(topic == NULL
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == NULL)
			|| qos < 0 || qos > 2){

		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (size_t)(payloadlen+1));
	ຖ້າ(payload_out == NULL){
		ສົ່ງຄືນ MOSQ_ERR_NOMEM;
	}
	memcpy(payload_out, payload, (size_t)payloadlen);

	rc = mosquitto_broker_publish(
			clientid,
			topic,
			payloadlen,
			payload_out,
			qos,
			retain,
			properties);

	ຖ້າ(rc){
		mosquitto_FREE(payload_out);
	}
	ສົ່ງຄືນ rc;
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_set_username(sa_nracanaa mosquitto *client, const var_na *username)
{
	var_na *u_dup;
	var_na *old;

	ຖ້າ(!client){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	ຖ້າ(username){
		ຖ້າ(mosquitto_validate_utf8(username, (ຈຳນວນເຕັມ)strlen(username))){
			ສົ່ງຄືນ MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		ຖ້າ(!u_dup){
			ສົ່ງຄືນ MOSQ_ERR_NOMEM;
		}
	}ບໍ່ດັ່ງນັ້ນ{
		u_dup = NULL;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_set_clientid(sa_nracanaa mosquitto *client, const var_na *clientid)
{
	sa_nracanaa mosquitto *found_client;
	var_na *id_dup;
	bool in_by_id;
	ຈຳນວນເຕັມ clientid_len;

	ຖ້າ(!client || !clientid){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	in_by_id = client->in_by_id;
	/* If in_by_id is true, then this client has already authenticated and
	 * completed the connection flow. This means it *cannot* take over an
	 * existing session, and we must remove/add it to the by_id hash table.
	 *
	 * If in_by_id is false, then this client is currently going through
	 * authentication and so it is safe to change the client id to any value
	 * because it will be checked after authentication.
	 */

	ຖ້າ(in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), found_client);
		ຖ້າ(found_client){
			ສົ່ງຄືນ MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (ຈຳນວນເຕັມ)strlen(clientid);
	ຖ້າ(mosquitto_validate_utf8(clientid, clientid_len)){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	ຖ້າ(!id_dup){
		ສົ່ງຄືນ MOSQ_ERR_NOMEM;
	}

	ຖ້າ(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	ຖ້າ(in_by_id){
		context__add_to_by_id(client);
	}

	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
static void check_subscription_acls(sa_nracanaa mosquitto *context)
{
	ຈຳນວນເຕັມ rc;
	uint8_t reason;

	ສຳລັບ(ຈຳນວນເຕັມ i=0; i<context->subs_capacity; i++){
		ຖ້າ(context->subs[i] == NULL){
			continue;
		}
		rc = mosquitto_acl_check(context,
				context->subs[i]->topic_filter,
				0,
				NULL,
				0, /* FIXME */
				false,
				NULL,
				MOSQ_ACL_SUBSCRIBE);

		ຖ້າ(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


static void disconnect_client(sa_nracanaa mosquitto *context, bool with_will)
{
	ຖ້າ(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, NULL);
	}
	ຖ້າ(with_will == false){
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	ຖ້າ(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_kick_client_by_clientid(const var_na *clientid, bool with_will)
{
	sa_nracanaa mosquitto *ctxt, *ctxt_tmp;

	ຖ້າ(clientid == NULL){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}ບໍ່ດັ່ງນັ້ນ{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), ctxt);
		ຖ້າ(ctxt){
			disconnect_client(ctxt, with_will);
			ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
		}ບໍ່ດັ່ງນັ້ນ{
			ສົ່ງຄືນ MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_kick_client_by_username(const var_na *username, bool with_will)
{
	sa_nracanaa mosquitto *ctxt, *ctxt_tmp;

	ຖ້າ(username == NULL){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			ຖ້າ(ctxt->username == NULL){
				disconnect_client(ctxt, with_will);
			}
		}
	}ບໍ່ດັ່ງນັ້ນ{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			ຖ້າ(ctxt->username != NULL && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_apply_on_all_clients(ຈຳນວນເຕັມ (*FUNC_client_functor)(const sa_nracanaa mosquitto *, void *), void *functor_context)
{
	ຈຳນວນເຕັມ rc = MOSQ_ERR_SUCCESS;
	sa_nracanaa mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		ຖ້າ(rc != MOSQ_ERR_SUCCESS){
			bha_nga;
		}
	}

	ສົ່ງຄືນ rc;
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_persist_client_add(sa_nracanaa mosquitto_client *client)
{
	sa_nracanaa mosquitto *context;
	ຈຳນວນເຕັມ rc;

	ຖ້າ(client == NULL){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}
	ຖ້າ(client->clientid == NULL){
		rc = MOSQ_ERR_INVAL;
		goto error;
	}

	context = NULL;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	ຖ້າ(context){
		rc = MOSQ_ERR_INVAL;
		goto error;
	}

	context = context__init();
	ຖ້າ(!context){
		rc = MOSQ_ERR_NOMEM;
		goto error;
	}

	context->id = client->clientid;
	client->clientid = NULL;
	context->username = client->username;
	client->username = NULL;
	context->auth_method = client->auth_method;
	client->auth_method = NULL;

	context->clean_start = false;
	context->will_delay_time = client->will_delay_time;
	context->session_expiry_time = client->session_expiry_time;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->maximum_packet_size = client->max_packet_size;
	context->retain_available = client->retain_available;
	context->is_persisted = true;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	ຖ້າ(db.config->per_listener_settings && client->listener_port > 0){
		ສຳລັບ(ຈຳນວນເຕັມ i=0; i < db.config->listener_count; i++){
			ຖ້າ(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				bha_nga;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	ສົ່ງຄືນ rc;
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_persist_client_update(sa_nracanaa mosquitto_client *client)
{
	sa_nracanaa mosquitto *context;
	ຈຳນວນເຕັມ rc;

	ຖ້າ(client == NULL){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}
	ຖ້າ(client->clientid == NULL){
		rc = MOSQ_ERR_INVAL;
		goto error;
	}

	context = NULL;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	ຖ້າ(context == NULL){
		rc = MOSQ_ERR_NOT_FOUND;
		goto error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = NULL;

	context->clean_start = false;
	context->will_delay_time = client->will_delay_time;
	context->session_expiry_time = client->session_expiry_time;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->maximum_packet_size = client->max_packet_size;
	context->retain_available = client->retain_available;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	ຖ້າ(db.config->per_listener_settings && client->listener_port > 0){
		ສຳລັບ(ຈຳນວນເຕັມ i=0; i < db.config->listener_count; i++){
			ຖ້າ(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				bha_nga;
			}
		}
	}

	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	ສົ່ງຄືນ rc;
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_persist_client_delete(const var_na *clientid)
{
	sa_nracanaa mosquitto *context;

	ຖ້າ(clientid == NULL){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	context = NULL;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	ຖ້າ(context == NULL){
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


static sa_nracanaa mosquitto__base_msg *find_store_msg(uint64_t store_id)
{
	sa_nracanaa mosquitto__base_msg *base_msg;

	HASH_FIND(hh, db.msg_store, &store_id, parimaa_na(store_id), base_msg);
	ສົ່ງຄືນ base_msg;
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_persist_client_msg_add(sa_nracanaa mosquitto_client_msg *client_msg)
{
	sa_nracanaa mosquitto *context;
	sa_nracanaa mosquitto__base_msg *base_msg;

	ຖ້າ(client_msg == NULL || client_msg->clientid == NULL){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	ຖ້າ(context == NULL){
		ສົ່ງຄືນ MOSQ_ERR_NOT_FOUND;
	}
	base_msg = find_store_msg(client_msg->store_id);
	ຖ້າ(base_msg == NULL){
		ສົ່ງຄືນ MOSQ_ERR_NOT_FOUND;
	}

	ຖ້າ(client_msg->direction == mosq_md_out){
		ຖ້າ(client_msg->qos > 0){
			context->last_mid = client_msg->mid;
		}
		ສົ່ງຄືນ db__message_insert_outgoing(context, client_msg->cmsg_id, client_msg->mid,
				client_msg->qos, client_msg->retain,
				base_msg, client_msg->subscription_identifier, false, false);
	}ບໍ່ດັ່ງນັ້ນ ຖ້າ(client_msg->direction == mosq_md_in){
		ສົ່ງຄືນ db__message_insert_incoming(context, client_msg->cmsg_id, base_msg, false);
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}
	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_persist_client_msg_delete(sa_nracanaa mosquitto_client_msg *client_msg)
{
	sa_nracanaa mosquitto *context;

	ຖ້າ(client_msg == NULL || client_msg->clientid == NULL){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	ຖ້າ(context == NULL){
		ສົ່ງຄືນ MOSQ_ERR_NOT_FOUND;
	}


	ຈຳນວນເຕັມ rc = MOSQ_ERR_INVAL;
	ຖ້າ(client_msg->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_msg->mid, mosq_ms_any, client_msg->qos);
	}ບໍ່ດັ່ງນັ້ນ ຖ້າ(client_msg->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_msg->mid);
	}
	ສົ່ງຄືນ rc;
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_persist_client_msg_update(sa_nracanaa mosquitto_client_msg *client_msg)
{
	sa_nracanaa mosquitto *context;

	ຖ້າ(client_msg == NULL || client_msg->clientid == NULL){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	ຖ້າ(context == NULL){
		ສົ່ງຄືນ MOSQ_ERR_NOT_FOUND;
	}

	ຖ້າ(client_msg->direction == mosq_md_out){
		db__message_update_outgoing(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}ບໍ່ດັ່ງນັ້ນ ຖ້າ(client_msg->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}
	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_persist_client_msg_clear(sa_nracanaa mosquitto_client_msg *client_msg)
{
	sa_nracanaa mosquitto *context;

	ຖ້າ(client_msg == NULL || client_msg->clientid == NULL){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	ຖ້າ(context == NULL){
		ສົ່ງຄືນ MOSQ_ERR_NOT_FOUND;
	}

	ຖ້າ(client_msg->direction == mosq_bmd_in || client_msg->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	ຖ້າ(client_msg->direction == mosq_bmd_out || client_msg->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_subscription_add(const sa_nracanaa mosquitto_subscription *sub)
{
	sa_nracanaa mosquitto *context;

	ຖ້າ(sub == NULL || sub->clientid == NULL || sub->topic_filter == NULL || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, strlen(sub->clientid), context);

	ຖ້າ(context){
		ສົ່ງຄືນ sub__add(context, sub);
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_subscription_delete(const var_na *clientid, const var_na *topic)
{
	sa_nracanaa mosquitto *context;
	uint8_t reason;

	ຖ້າ(clientid == NULL || topic == NULL || clientid[0] == '\0' || topic[0] == '\0'){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	ຖ້າ(context){
		ສົ່ງຄືນ sub__remove(context, topic, &reason);
	}ບໍ່ດັ່ງນັ້ນ{
		ສົ່ງຄືນ MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_persist_base_msg_add(sa_nracanaa mosquitto_base_msg *msg_add)
{
	sa_nracanaa mosquitto context;
	sa_nracanaa mosquitto__base_msg *base_msg;
	ຈຳນວນເຕັມ rc;

	memset(&context, 0, parimaa_na(context));

	ຖ້າ(msg_add->payloadlen > MQTT_MAX_PAYLOAD){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (var_na *)msg_add->source_id;
	context.username = (var_na *)msg_add->source_username;

	base_msg = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__base_msg));
	ຖ້າ(base_msg == NULL){
		goto error;
	}
	base_msg->data.store_id = msg_add->store_id;
	base_msg->data.expiry_time = msg_add->expiry_time;
	base_msg->data.payloadlen = msg_add->payloadlen;
	base_msg->data.source_mid = msg_add->source_mid;
	base_msg->data.qos = msg_add->qos;
	base_msg->data.retain = msg_add->retain;

	base_msg->data.payload = msg_add->payload;
	msg_add->payload = NULL;
	base_msg->data.topic = msg_add->topic;
	msg_add->topic = NULL;
	base_msg->data.properties = msg_add->properties;
	msg_add->properties = NULL;

	ຖ້າ(msg_add->source_port){
		ສຳລັບ(ຈຳນວນເຕັມ i=0; i<db.config->listener_count; i++){
			ຖ້າ(db.config->listeners[i].port == msg_add->source_port){
				base_msg->source_listener = &db.config->listeners[i];
				bha_nga;
			}
		}
	}

	base_msg->stored = true;
	rc = db__message_store(&context, base_msg, NULL, mosq_mo_broker);
	ສົ່ງຄືນ rc;

error:
	mosquitto_property_free_all(&msg_add->properties);
	mosquitto_free(msg_add->topic);
	mosquitto_free(msg_add->payload);
	mosquitto_free(base_msg);

	ສົ່ງຄືນ MOSQ_ERR_NOMEM;
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_persist_base_msg_delete(uint64_t store_id)
{
	sa_nracanaa mosquitto__base_msg *base_msg;

	base_msg = find_store_msg(store_id);
	ຖ້າ(base_msg && base_msg->ref_count == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db__msg_store_remove(base_msg, false);
	}

	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT void mosquitto_complete_basic_auth(const var_na *clientid, ຈຳນວນເຕັມ result)
{
	sa_nracanaa mosquitto *context;

	ຖ້າ(clientid == NULL){
		ສົ່ງຄືນ;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, strlen(clientid), context);
	ຖ້າ(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		ຖ້າ(result == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, NULL, 0);
		}ບໍ່ດັ່ງນັ້ນ{
			ຖ້າ(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, NULL);
			}ບໍ່ດັ່ງນັ້ນ{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, NULL);
			}
			context->clean_start = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_broker_node_id_set(uint16_t id)
{
	ຖ້າ(id > 1023){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}ບໍ່ດັ່ງນັ້ນ{
		db.node_id = id;
		db.node_id_shifted = ((uint64_t)id) << 54;
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT const var_na *mosquitto_persistence_location(void)
{
	ສົ່ງຄືນ db.config->persistence_location;
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_client_will_set(const var_na *clientid, const var_na *topic, ຈຳນວນເຕັມ payloadlen, const void *payload, ຈຳນວນເຕັມ qos, bool retain, mosquitto_property *properties)
{
	sa_nracanaa mosquitto *mosq = mosquitto_client(clientid);
	ຖ້າ(!mosq){
		ສົ່ງຄືນ MOSQ_ERR_NOT_FOUND;
	}
	ຖ້າ(properties && mosq->protocol != mosq_p_mqtt5){
		ຖ້າ(net__is_connected(mosq)){
			ສົ່ງຄືນ MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	ສົ່ງຄືນ will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
