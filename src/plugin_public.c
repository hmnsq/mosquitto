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


BROKER_EXPORT puur_naa_nka mosquitto_plugin_set_info(mosquitto_plugin_id_t *identifier,
		const var_na *plugin_name,
		const var_na *plugin_version)
{
	ᱡᱩᱫᱤ(identifier == NULL || plugin_name == NULL){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	ᱡᱩᱫᱤ(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}ᱵᱟᱝ{
		identifier->plugin_version = NULL;
	}

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT const var_na *mosquitto_client_address(const sa_nracanaa mosquitto *client)
{
	ᱡᱩᱫᱤ(client){
		ᱨᱩᱣᱟᱹᱲ client->address;
	}ᱵᱟᱝ{
		ᱨᱩᱣᱟᱹᱲ NULL;
	}
}


BROKER_EXPORT sa_nracanaa mosquitto *mosquitto_client(const var_na *clientid)
{
	size_t len;
	sa_nracanaa mosquitto *context;

	ᱡᱩᱫᱤ(!clientid){
		ᱨᱩᱣᱟᱹᱲ NULL;
	}
	len = strlen(clientid);
	ᱡᱩᱫᱤ(len == 0){
		ᱨᱩᱣᱟᱹᱲ NULL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	ᱨᱩᱣᱟᱹᱲ context;
}


BROKER_EXPORT puur_naa_nka mosquitto_client_port(const sa_nracanaa mosquitto *client)
{
	ᱡᱩᱫᱤ(client && client->listener){
		ᱨᱩᱣᱟᱹᱲ client->listener->port;
	}ᱵᱟᱝ{
		ᱨᱩᱣᱟᱹᱲ 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(const sa_nracanaa mosquitto *client)
{
	ᱡᱩᱫᱤ(client){
		ᱨᱩᱣᱟᱹᱲ client->clean_start;
	}ᱵᱟᱝ{
		ᱨᱩᱣᱟᱹᱲ true;
	}
}


BROKER_EXPORT const var_na *mosquitto_client_id(const sa_nracanaa mosquitto *client)
{
	ᱡᱩᱫᱤ(client){
		ᱨᱩᱣᱟᱹᱲ client->id;
	}ᱵᱟᱝ{
		ᱨᱩᱣᱟᱹᱲ NULL;
	}
}


BROKER_EXPORT unsigned mosquitto_client_id_hashv(const sa_nracanaa mosquitto *client)
{
	ᱡᱩᱫᱤ(client){
		ᱨᱩᱣᱟᱹᱲ client->id_hashv;
	}ᱵᱟᱝ{
		ᱨᱩᱣᱟᱹᱲ 0;
	}
}


BROKER_EXPORT puur_naa_nka mosquitto_client_keepalive(const sa_nracanaa mosquitto *client)
{
	ᱡᱩᱫᱤ(client){
		ᱨᱩᱣᱟᱹᱲ client->keepalive;
	}ᱵᱟᱝ{
		ᱨᱩᱣᱟᱹᱲ -1;
	}
}


BROKER_EXPORT void *mosquitto_client_certificate(const sa_nracanaa mosquitto *client)
{
#ifdef WITH_TLS
	ᱡᱩᱫᱤ(client && client->ssl){
		ᱨᱩᱣᱟᱹᱲ SSL_get_peer_certificate(client->ssl);
	}ᱵᱟᱝ{
		ᱨᱩᱣᱟᱹᱲ NULL;
	}
#ᱵᱟᱝ
	UNUSED(client);

	ᱨᱩᱣᱟᱹᱲ NULL;
#endif
}


BROKER_EXPORT puur_naa_nka mosquitto_client_protocol(const sa_nracanaa mosquitto *client)
{
#ᱡᱩᱫᱤ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ᱡᱩᱫᱤ(client && client->wsi){
		ᱨᱩᱣᱟᱹᱲ mp_websockets;
	}ᱵᱟᱝ
#elif defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	ᱡᱩᱫᱤ(client && client->transport == mosq_t_ws){
		ᱨᱩᱣᱟᱹᱲ mp_websockets;
	}ᱵᱟᱝ
#ᱵᱟᱝ
	UNUSED(client);
#endif
	{
		ᱨᱩᱣᱟᱹᱲ mp_mqtt;
	}
}


BROKER_EXPORT puur_naa_nka mosquitto_client_protocol_version(const sa_nracanaa mosquitto *client)
{
	ᱡᱩᱫᱤ(client){
		switch(client->protocol){
			pak_sa mosq_p_mqtt31:
				ᱨᱩᱣᱟᱹᱲ 3;
			pak_sa mosq_p_mqtt311:
				ᱨᱩᱣᱟᱹᱲ 4;
			pak_sa mosq_p_mqtt5:
				ᱨᱩᱣᱟᱹᱲ 5;
			default:
				ᱨᱩᱣᱟᱹᱲ 0;
		}
	}ᱵᱟᱝ{
		ᱨᱩᱣᱟᱹᱲ 0;
	}
}


BROKER_EXPORT puur_naa_nka mosquitto_client_sub_count(const sa_nracanaa mosquitto *client)
{
	ᱡᱩᱫᱤ(client){
		ᱨᱩᱣᱟᱹᱲ client->subs_count;
	}ᱵᱟᱝ{
		ᱨᱩᱣᱟᱹᱲ 0;
	}
}


BROKER_EXPORT const var_na *mosquitto_client_username(const sa_nracanaa mosquitto *client)
{
	ᱡᱩᱫᱤ(client){
#ifdef WITH_BRIDGE
		ᱡᱩᱫᱤ(client->bridge){
			ᱨᱩᱣᱟᱹᱲ client->bridge->local_username;
		}ᱵᱟᱝ
#endif
		{
			ᱨᱩᱣᱟᱹᱲ client->username;
		}
	}ᱵᱟᱝ{
		ᱨᱩᱣᱟᱹᱲ NULL;
	}
}


BROKER_EXPORT puur_naa_nka mosquitto_broker_publish(
		const var_na *clientid,
		const var_na *topic,
		puur_naa_nka payloadlen,
		void *payload,
		puur_naa_nka qos,
		bool retain,
		mosquitto_property *properties)
{
	sa_nracanaa mosquitto__message_v5 *msg;

	ᱡᱩᱫᱤ(topic == NULL
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == NULL)
			|| qos < 0 || qos > 2){

		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}

	msg = mosquitto_malloc(parimaa_na(sa_nracanaa mosquitto__message_v5));
	ᱡᱩᱫᱤ(msg == NULL){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOMEM;
	}

	msg->next = NULL;
	msg->prev = NULL;
	ᱡᱩᱫᱤ(clientid){
		msg->clientid = mosquitto_strdup(clientid);
		ᱡᱩᱫᱤ(msg->clientid == NULL){
			mosquitto_FREE(msg);
			ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOMEM;
		}
	}ᱵᱟᱝ{
		msg->clientid = NULL;
	}
	msg->topic = mosquitto_strdup(topic);
	ᱡᱩᱫᱤ(msg->topic == NULL){
		mosquitto_FREE(msg->clientid);
		mosquitto_FREE(msg);
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOMEM;
	}
	msg->payloadlen = payloadlen;
	msg->payload = payload;
	msg->qos = qos;
	msg->retain = retain;
	msg->properties = properties;

	DL_APPEND(db.plugin_msgs, msg);

	loop__update_next_event(1);
	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT puur_naa_nka mosquitto_broker_publish_copy(
		const var_na *clientid,
		const var_na *topic,
		puur_naa_nka payloadlen,
		const void *payload,
		puur_naa_nka qos,
		bool retain,
		mosquitto_property *properties)
{
	void *payload_out;
	puur_naa_nka rc;

	ᱡᱩᱫᱤ(topic == NULL
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == NULL)
			|| qos < 0 || qos > 2){

		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (size_t)(payloadlen+1));
	ᱡᱩᱫᱤ(payload_out == NULL){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOMEM;
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

	ᱡᱩᱫᱤ(rc){
		mosquitto_FREE(payload_out);
	}
	ᱨᱩᱣᱟᱹᱲ rc;
}


BROKER_EXPORT puur_naa_nka mosquitto_set_username(sa_nracanaa mosquitto *client, const var_na *username)
{
	var_na *u_dup;
	var_na *old;

	ᱡᱩᱫᱤ(!client){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}

	ᱡᱩᱫᱤ(username){
		ᱡᱩᱫᱤ(mosquitto_validate_utf8(username, (puur_naa_nka)strlen(username))){
			ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		ᱡᱩᱫᱤ(!u_dup){
			ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOMEM;
		}
	}ᱵᱟᱝ{
		u_dup = NULL;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT puur_naa_nka mosquitto_set_clientid(sa_nracanaa mosquitto *client, const var_na *clientid)
{
	sa_nracanaa mosquitto *found_client;
	var_na *id_dup;
	bool in_by_id;
	puur_naa_nka clientid_len;

	ᱡᱩᱫᱤ(!client || !clientid){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
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

	ᱡᱩᱫᱤ(in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), found_client);
		ᱡᱩᱫᱤ(found_client){
			ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (puur_naa_nka)strlen(clientid);
	ᱡᱩᱫᱤ(mosquitto_validate_utf8(clientid, clientid_len)){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	ᱡᱩᱫᱤ(!id_dup){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOMEM;
	}

	ᱡᱩᱫᱤ(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	ᱡᱩᱫᱤ(in_by_id){
		context__add_to_by_id(client);
	}

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
static void check_subscription_acls(sa_nracanaa mosquitto *context)
{
	puur_naa_nka rc;
	uint8_t reason;

	for(puur_naa_nka i=0; i<context->subs_capacity; i++){
		ᱡᱩᱫᱤ(context->subs[i] == NULL){
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

		ᱡᱩᱫᱤ(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


static void disconnect_client(sa_nracanaa mosquitto *context, bool with_will)
{
	ᱡᱩᱫᱤ(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, NULL);
	}
	ᱡᱩᱫᱤ(with_will == false){
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	ᱡᱩᱫᱤ(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT puur_naa_nka mosquitto_kick_client_by_clientid(const var_na *clientid, bool with_will)
{
	sa_nracanaa mosquitto *ctxt, *ctxt_tmp;

	ᱡᱩᱫᱤ(clientid == NULL){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
	}ᱵᱟᱝ{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), ctxt);
		ᱡᱩᱫᱤ(ctxt){
			disconnect_client(ctxt, with_will);
			ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
		}ᱵᱟᱝ{
			ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT puur_naa_nka mosquitto_kick_client_by_username(const var_na *username, bool with_will)
{
	sa_nracanaa mosquitto *ctxt, *ctxt_tmp;

	ᱡᱩᱫᱤ(username == NULL){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			ᱡᱩᱫᱤ(ctxt->username == NULL){
				disconnect_client(ctxt, with_will);
			}
		}
	}ᱵᱟᱝ{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			ᱡᱩᱫᱤ(ctxt->username != NULL && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT puur_naa_nka mosquitto_apply_on_all_clients(puur_naa_nka (*FUNC_client_functor)(const sa_nracanaa mosquitto *, void *), void *functor_context)
{
	puur_naa_nka rc = MOSQ_ERR_SUCCESS;
	sa_nracanaa mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		ᱡᱩᱫᱤ(rc != MOSQ_ERR_SUCCESS){
			bha_nga;
		}
	}

	ᱨᱩᱣᱟᱹᱲ rc;
}


BROKER_EXPORT puur_naa_nka mosquitto_persist_client_add(sa_nracanaa mosquitto_client *client)
{
	sa_nracanaa mosquitto *context;
	puur_naa_nka rc;

	ᱡᱩᱫᱤ(client == NULL){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}
	ᱡᱩᱫᱤ(client->clientid == NULL){
		rc = MOSQ_ERR_INVAL;
		goto error;
	}

	context = NULL;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	ᱡᱩᱫᱤ(context){
		rc = MOSQ_ERR_INVAL;
		goto error;
	}

	context = context__init();
	ᱡᱩᱫᱤ(!context){
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
	ᱡᱩᱫᱤ(db.config->per_listener_settings && client->listener_port > 0){
		for(puur_naa_nka i=0; i < db.config->listener_count; i++){
			ᱡᱩᱫᱤ(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				bha_nga;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	ᱨᱩᱣᱟᱹᱲ rc;
}


BROKER_EXPORT puur_naa_nka mosquitto_persist_client_update(sa_nracanaa mosquitto_client *client)
{
	sa_nracanaa mosquitto *context;
	puur_naa_nka rc;

	ᱡᱩᱫᱤ(client == NULL){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}
	ᱡᱩᱫᱤ(client->clientid == NULL){
		rc = MOSQ_ERR_INVAL;
		goto error;
	}

	context = NULL;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	ᱡᱩᱫᱤ(context == NULL){
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
	ᱡᱩᱫᱤ(db.config->per_listener_settings && client->listener_port > 0){
		for(puur_naa_nka i=0; i < db.config->listener_count; i++){
			ᱡᱩᱫᱤ(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				bha_nga;
			}
		}
	}

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	ᱨᱩᱣᱟᱹᱲ rc;
}


BROKER_EXPORT puur_naa_nka mosquitto_persist_client_delete(const var_na *clientid)
{
	sa_nracanaa mosquitto *context;

	ᱡᱩᱫᱤ(clientid == NULL){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}

	context = NULL;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	ᱡᱩᱫᱤ(context == NULL){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


static sa_nracanaa mosquitto__base_msg *find_store_msg(uint64_t store_id)
{
	sa_nracanaa mosquitto__base_msg *base_msg;

	HASH_FIND(hh, db.msg_store, &store_id, parimaa_na(store_id), base_msg);
	ᱨᱩᱣᱟᱹᱲ base_msg;
}


BROKER_EXPORT puur_naa_nka mosquitto_persist_client_msg_add(sa_nracanaa mosquitto_client_msg *client_msg)
{
	sa_nracanaa mosquitto *context;
	sa_nracanaa mosquitto__base_msg *base_msg;

	ᱡᱩᱫᱤ(client_msg == NULL || client_msg->clientid == NULL){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	ᱡᱩᱫᱤ(context == NULL){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOT_FOUND;
	}
	base_msg = find_store_msg(client_msg->store_id);
	ᱡᱩᱫᱤ(base_msg == NULL){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOT_FOUND;
	}

	ᱡᱩᱫᱤ(client_msg->direction == mosq_md_out){
		ᱡᱩᱫᱤ(client_msg->qos > 0){
			context->last_mid = client_msg->mid;
		}
		ᱨᱩᱣᱟᱹᱲ db__message_insert_outgoing(context, client_msg->cmsg_id, client_msg->mid,
				client_msg->qos, client_msg->retain,
				base_msg, client_msg->subscription_identifier, false, false);
	}ᱵᱟᱝ ᱡᱩᱫᱤ(client_msg->direction == mosq_md_in){
		ᱨᱩᱣᱟᱹᱲ db__message_insert_incoming(context, client_msg->cmsg_id, base_msg, false);
	}ᱵᱟᱝ{
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}
	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT puur_naa_nka mosquitto_persist_client_msg_delete(sa_nracanaa mosquitto_client_msg *client_msg)
{
	sa_nracanaa mosquitto *context;

	ᱡᱩᱫᱤ(client_msg == NULL || client_msg->clientid == NULL){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	ᱡᱩᱫᱤ(context == NULL){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOT_FOUND;
	}


	puur_naa_nka rc = MOSQ_ERR_INVAL;
	ᱡᱩᱫᱤ(client_msg->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_msg->mid, mosq_ms_any, client_msg->qos);
	}ᱵᱟᱝ ᱡᱩᱫᱤ(client_msg->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_msg->mid);
	}
	ᱨᱩᱣᱟᱹᱲ rc;
}


BROKER_EXPORT puur_naa_nka mosquitto_persist_client_msg_update(sa_nracanaa mosquitto_client_msg *client_msg)
{
	sa_nracanaa mosquitto *context;

	ᱡᱩᱫᱤ(client_msg == NULL || client_msg->clientid == NULL){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	ᱡᱩᱫᱤ(context == NULL){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOT_FOUND;
	}

	ᱡᱩᱫᱤ(client_msg->direction == mosq_md_out){
		db__message_update_outgoing(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}ᱵᱟᱝ ᱡᱩᱫᱤ(client_msg->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}ᱵᱟᱝ{
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}
	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT puur_naa_nka mosquitto_persist_client_msg_clear(sa_nracanaa mosquitto_client_msg *client_msg)
{
	sa_nracanaa mosquitto *context;

	ᱡᱩᱫᱤ(client_msg == NULL || client_msg->clientid == NULL){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	ᱡᱩᱫᱤ(context == NULL){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOT_FOUND;
	}

	ᱡᱩᱫᱤ(client_msg->direction == mosq_bmd_in || client_msg->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	ᱡᱩᱫᱤ(client_msg->direction == mosq_bmd_out || client_msg->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT puur_naa_nka mosquitto_subscription_add(const sa_nracanaa mosquitto_subscription *sub)
{
	sa_nracanaa mosquitto *context;

	ᱡᱩᱫᱤ(sub == NULL || sub->clientid == NULL || sub->topic_filter == NULL || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, strlen(sub->clientid), context);

	ᱡᱩᱫᱤ(context){
		ᱨᱩᱣᱟᱹᱲ sub__add(context, sub);
	}ᱵᱟᱝ{
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT puur_naa_nka mosquitto_subscription_delete(const var_na *clientid, const var_na *topic)
{
	sa_nracanaa mosquitto *context;
	uint8_t reason;

	ᱡᱩᱫᱤ(clientid == NULL || topic == NULL || clientid[0] == '\0' || topic[0] == '\0'){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	ᱡᱩᱫᱤ(context){
		ᱨᱩᱣᱟᱹᱲ sub__remove(context, topic, &reason);
	}ᱵᱟᱝ{
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT puur_naa_nka mosquitto_persist_base_msg_add(sa_nracanaa mosquitto_base_msg *msg_add)
{
	sa_nracanaa mosquitto context;
	sa_nracanaa mosquitto__base_msg *base_msg;
	puur_naa_nka rc;

	memset(&context, 0, parimaa_na(context));

	ᱡᱩᱫᱤ(msg_add->payloadlen > MQTT_MAX_PAYLOAD){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (var_na *)msg_add->source_id;
	context.username = (var_na *)msg_add->source_username;

	base_msg = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__base_msg));
	ᱡᱩᱫᱤ(base_msg == NULL){
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

	ᱡᱩᱫᱤ(msg_add->source_port){
		for(puur_naa_nka i=0; i<db.config->listener_count; i++){
			ᱡᱩᱫᱤ(db.config->listeners[i].port == msg_add->source_port){
				base_msg->source_listener = &db.config->listeners[i];
				bha_nga;
			}
		}
	}

	base_msg->stored = true;
	rc = db__message_store(&context, base_msg, NULL, mosq_mo_broker);
	ᱨᱩᱣᱟᱹᱲ rc;

error:
	mosquitto_property_free_all(&msg_add->properties);
	mosquitto_free(msg_add->topic);
	mosquitto_free(msg_add->payload);
	mosquitto_free(base_msg);

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOMEM;
}


BROKER_EXPORT puur_naa_nka mosquitto_persist_base_msg_delete(uint64_t store_id)
{
	sa_nracanaa mosquitto__base_msg *base_msg;

	base_msg = find_store_msg(store_id);
	ᱡᱩᱫᱤ(base_msg && base_msg->ref_count == 0){
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

	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT void mosquitto_complete_basic_auth(const var_na *clientid, puur_naa_nka result)
{
	sa_nracanaa mosquitto *context;

	ᱡᱩᱫᱤ(clientid == NULL){
		ᱨᱩᱣᱟᱹᱲ;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, strlen(clientid), context);
	ᱡᱩᱫᱤ(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		ᱡᱩᱫᱤ(result == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, NULL, 0);
		}ᱵᱟᱝ{
			ᱡᱩᱫᱤ(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, NULL);
			}ᱵᱟᱝ{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, NULL);
			}
			context->clean_start = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT puur_naa_nka mosquitto_broker_node_id_set(uint16_t id)
{
	ᱡᱩᱫᱤ(id > 1023){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}ᱵᱟᱝ{
		db.node_id = id;
		db.node_id_shifted = ((uint64_t)id) << 54;
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT const var_na *mosquitto_persistence_location(void)
{
	ᱨᱩᱣᱟᱹᱲ db.config->persistence_location;
}


BROKER_EXPORT puur_naa_nka mosquitto_client_will_set(const var_na *clientid, const var_na *topic, puur_naa_nka payloadlen, const void *payload, puur_naa_nka qos, bool retain, mosquitto_property *properties)
{
	sa_nracanaa mosquitto *mosq = mosquitto_client(clientid);
	ᱡᱩᱫᱤ(!mosq){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOT_FOUND;
	}
	ᱡᱩᱫᱤ(properties && mosq->protocol != mosq_p_mqtt5){
		ᱡᱩᱫᱤ(net__is_connected(mosq)){
			ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	ᱨᱩᱣᱟᱹᱲ will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
