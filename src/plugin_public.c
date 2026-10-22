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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "send_mosq.h"
#અન્તર્ભાવ "util_mosq.h"
#અન્તર્ભાવ "will_mosq.h"
#અન્તર્ભાવ "utlist.h"
#અન્તર્ભાવ "will_mosq.h"

#ifdef WITH_TLS
#  include <openssl/ssl.h>
#endif


BROKER_EXPORT પૂર્ણાંક mosquitto_plugin_set_info(mosquitto_plugin_id_t *identifier,
		અચલ અક્ષર *plugin_name,
		અચલ અક્ષર *plugin_version)
{
	જો(identifier == ખાલી || plugin_name == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	જો(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}નહીંતર{
		identifier->plugin_version = ખાલી;
	}

	પાછા MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT અચલ અક્ષર *mosquitto_client_address(અચલ સંરચના mosquitto *client)
{
	જો(client){
		પાછા client->address;
	}નહીંતર{
		પાછા ખાલી;
	}
}


BROKER_EXPORT સંરચના mosquitto *mosquitto_client(અચલ અક્ષર *clientid)
{
	size_t len;
	સંરચના mosquitto *context;

	જો(!clientid){
		પાછા ખાલી;
	}
	len = strlen(clientid);
	જો(len == 0){
		પાછા ખાલી;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	પાછા context;
}


BROKER_EXPORT પૂર્ણાંક mosquitto_client_port(અચલ સંરચના mosquitto *client)
{
	જો(client && client->listener){
		પાછા client->listener->port;
	}નહીંતર{
		પાછા 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(અચલ સંરચના mosquitto *client)
{
	જો(client){
		પાછા client->clean_start;
	}નહીંતર{
		પાછા true;
	}
}


BROKER_EXPORT અચલ અક્ષર *mosquitto_client_id(અચલ સંરચના mosquitto *client)
{
	જો(client){
		પાછા client->id;
	}નહીંતર{
		પાછા ખાલી;
	}
}


BROKER_EXPORT unsigned mosquitto_client_id_hashv(અચલ સંરચના mosquitto *client)
{
	જો(client){
		પાછા client->id_hashv;
	}નહીંતર{
		પાછા 0;
	}
}


BROKER_EXPORT પૂર્ણાંક mosquitto_client_keepalive(અચલ સંરચના mosquitto *client)
{
	જો(client){
		પાછા client->keepalive;
	}નહીંતર{
		પાછા -1;
	}
}


BROKER_EXPORT શૂન્ય *mosquitto_client_certificate(અચલ સંરચના mosquitto *client)
{
#ifdef WITH_TLS
	જો(client && client->ssl){
		પાછા SSL_get_peer_certificate(client->ssl);
	}નહીંતર{
		પાછા ખાલી;
	}
#નહીંતર
	UNUSED(client);

	પાછા ખાલી;
#endif
}


BROKER_EXPORT પૂર્ણાંક mosquitto_client_protocol(અચલ સંરચના mosquitto *client)
{
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	જો(client && client->wsi){
		પાછા mp_websockets;
	}નહીંતર
#અથવાજો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	જો(client && client->transport == mosq_t_ws){
		પાછા mp_websockets;
	}નહીંતર
#નહીંતર
	UNUSED(client);
#endif
	{
		પાછા mp_mqtt;
	}
}


BROKER_EXPORT પૂર્ણાંક mosquitto_client_protocol_version(અચલ સંરચના mosquitto *client)
{
	જો(client){
		પસંદગી(client->protocol){
			સ્થિતિ mosq_p_mqtt31:
				પાછા 3;
			સ્થિતિ mosq_p_mqtt311:
				પાછા 4;
			સ્થિતિ mosq_p_mqtt5:
				પાછા 5;
			મૂળભૂત:
				પાછા 0;
		}
	}નહીંતર{
		પાછા 0;
	}
}


BROKER_EXPORT પૂર્ણાંક mosquitto_client_sub_ગણતરી(અચલ સંરચના mosquitto *client)
{
	જો(client){
		પાછા client->subs_ગણતરી;
	}નહીંતર{
		પાછા 0;
	}
}


BROKER_EXPORT અચલ અક્ષર *mosquitto_client_username(અચલ સંરચના mosquitto *client)
{
	જો(client){
#ifdef WITH_BRIDGE
		જો(client->bridge){
			પાછા client->bridge->local_username;
		}નહીંતર
#endif
		{
			પાછા client->username;
		}
	}નહીંતર{
		પાછા ખાલી;
	}
}


BROKER_EXPORT પૂર્ણાંક mosquitto_broker_publish(
		અચલ અક્ષર *clientid,
		અચલ અક્ષર *topic,
		પૂર્ણાંક payloadlen,
		શૂન્ય *payload,
		પૂર્ણાંક qos,
		bool retain,
		mosquitto_property *properties)
{
	સંરચના mosquitto__message_v5 *સંદેશ;

	જો(topic == ખાલી
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == ખાલી)
			|| qos < 0 || qos > 2){

		પાછા MOSQ_ERR_INVAL;
	}

	સંદેશ = mosquitto_malloc(કદ(સંરચના mosquitto__message_v5));
	જો(સંદેશ == ખાલી){
		પાછા MOSQ_ERR_NOMEM;
	}

	સંદેશ->next = ખાલી;
	સંદેશ->prev = ખાલી;
	જો(clientid){
		સંદેશ->clientid = mosquitto_strdup(clientid);
		જો(સંદેશ->clientid == ખાલી){
			mosquitto_FREE(સંદેશ);
			પાછા MOSQ_ERR_NOMEM;
		}
	}નહીંતર{
		સંદેશ->clientid = ખાલી;
	}
	સંદેશ->topic = mosquitto_strdup(topic);
	જો(સંદેશ->topic == ખાલી){
		mosquitto_FREE(સંદેશ->clientid);
		mosquitto_FREE(સંદેશ);
		પાછા MOSQ_ERR_NOMEM;
	}
	સંદેશ->payloadlen = payloadlen;
	સંદેશ->payload = payload;
	સંદેશ->qos = qos;
	સંદેશ->retain = retain;
	સંદેશ->properties = properties;

	DL_APPEND(db.plugin_msgs, સંદેશ);

	loop__update_next_event(1);
	પાછા MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT પૂર્ણાંક mosquitto_broker_publish_copy(
		અચલ અક્ષર *clientid,
		અચલ અક્ષર *topic,
		પૂર્ણાંક payloadlen,
		અચલ શૂન્ય *payload,
		પૂર્ણાંક qos,
		bool retain,
		mosquitto_property *properties)
{
	શૂન્ય *payload_out;
	પૂર્ણાંક rc;

	જો(topic == ખાલી
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == ખાલી)
			|| qos < 0 || qos > 2){

		પાછા MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (size_t)(payloadlen+1));
	જો(payload_out == ખાલી){
		પાછા MOSQ_ERR_NOMEM;
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

	જો(rc){
		mosquitto_FREE(payload_out);
	}
	પાછા rc;
}


BROKER_EXPORT પૂર્ણાંક mosquitto_set_username(સંરચના mosquitto *client, અચલ અક્ષર *username)
{
	અક્ષર *u_dup;
	અક્ષર *old;

	જો(!client){
		પાછા MOSQ_ERR_INVAL;
	}

	જો(username){
		જો(mosquitto_validate_utf8(username, (પૂર્ણાંક)strlen(username))){
			પાછા MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		જો(!u_dup){
			પાછા MOSQ_ERR_NOMEM;
		}
	}નહીંતર{
		u_dup = ખાલી;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	પાછા MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT પૂર્ણાંક mosquitto_set_clientid(સંરચના mosquitto *client, અચલ અક્ષર *clientid)
{
	સંરચના mosquitto *found_client;
	અક્ષર *id_dup;
	bool in_by_id;
	પૂર્ણાંક clientid_len;

	જો(!client || !clientid){
		પાછા MOSQ_ERR_INVAL;
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

	જો(in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), found_client);
		જો(found_client){
			પાછા MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (પૂર્ણાંક)strlen(clientid);
	જો(mosquitto_validate_utf8(clientid, clientid_len)){
		પાછા MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	જો(!id_dup){
		પાછા MOSQ_ERR_NOMEM;
	}

	જો(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	જો(in_by_id){
		context__add_to_by_id(client);
	}

	પાછા MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
સ્થિર શૂન્ય check_subscription_acls(સંરચના mosquitto *context)
{
	પૂર્ણાંક rc;
	uint8_t reason;

	માટે(પૂર્ણાંક i=0; i<context->subs_capacity; i++){
		જો(context->subs[i] == ખાલી){
			ચાલુ;
		}
		rc = mosquitto_acl_check(context,
				context->subs[i]->topic_filter,
				0,
				ખાલી,
				0, /* FIXME */
				false,
				ખાલી,
				MOSQ_ACL_SUBSCRIBE);

		જો(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


સ્થિર શૂન્ય disconnect_client(સંરચના mosquitto *context, bool with_will)
{
	જો(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ખાલી);
	}
	જો(with_will == false){
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	જો(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT પૂર્ણાંક mosquitto_kick_client_by_clientid(અચલ અક્ષર *clientid, bool with_will)
{
	સંરચના mosquitto *ctxt, *ctxt_tmp;

	જો(clientid == ખાલી){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		પાછા MOSQ_ERR_SUCCESS;
	}નહીંતર{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), ctxt);
		જો(ctxt){
			disconnect_client(ctxt, with_will);
			પાછા MOSQ_ERR_SUCCESS;
		}નહીંતર{
			પાછા MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT પૂર્ણાંક mosquitto_kick_client_by_username(અચલ અક્ષર *username, bool with_will)
{
	સંરચના mosquitto *ctxt, *ctxt_tmp;

	જો(username == ખાલી){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			જો(ctxt->username == ખાલી){
				disconnect_client(ctxt, with_will);
			}
		}
	}નહીંતર{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			જો(ctxt->username != ખાલી && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	પાછા MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT પૂર્ણાંક mosquitto_apply_on_all_clients(પૂર્ણાંક (*FUNC_client_functor)(અચલ સંરચના mosquitto *, શૂન્ય *), શૂન્ય *functor_context)
{
	પૂર્ણાંક rc = MOSQ_ERR_SUCCESS;
	સંરચના mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		જો(rc != MOSQ_ERR_SUCCESS){
			તોડો;
		}
	}

	પાછા rc;
}


BROKER_EXPORT પૂર્ણાંક mosquitto_persist_client_add(સંરચના mosquitto_client *client)
{
	સંરચના mosquitto *context;
	પૂર્ણાંક rc;

	જો(client == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}
	જો(client->clientid == ખાલી){
		rc = MOSQ_ERR_INVAL;
		જાઓ error;
	}

	context = ખાલી;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	જો(context){
		rc = MOSQ_ERR_INVAL;
		જાઓ error;
	}

	context = context__init();
	જો(!context){
		rc = MOSQ_ERR_NOMEM;
		જાઓ error;
	}

	context->id = client->clientid;
	client->clientid = ખાલી;
	context->username = client->username;
	client->username = ખાલી;
	context->auth_method = client->auth_method;
	client->auth_method = ખાલી;

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
	જો(db.config->per_listener_settings && client->listener_port > 0){
		માટે(પૂર્ણાંક i=0; i < db.config->listener_ગણતરી; i++){
			જો(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				તોડો;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	પાછા MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	પાછા rc;
}


BROKER_EXPORT પૂર્ણાંક mosquitto_persist_client_update(સંરચના mosquitto_client *client)
{
	સંરચના mosquitto *context;
	પૂર્ણાંક rc;

	જો(client == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}
	જો(client->clientid == ખાલી){
		rc = MOSQ_ERR_INVAL;
		જાઓ error;
	}

	context = ખાલી;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	જો(context == ખાલી){
		rc = MOSQ_ERR_NOT_FOUND;
		જાઓ error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = ખાલી;

	context->clean_start = false;
	context->will_delay_time = client->will_delay_time;
	context->session_expiry_time = client->session_expiry_time;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->maximum_packet_size = client->max_packet_size;
	context->retain_available = client->retain_available;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	જો(db.config->per_listener_settings && client->listener_port > 0){
		માટે(પૂર્ણાંક i=0; i < db.config->listener_ગણતરી; i++){
			જો(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				તોડો;
			}
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	પાછા rc;
}


BROKER_EXPORT પૂર્ણાંક mosquitto_persist_client_delete(અચલ અક્ષર *clientid)
{
	સંરચના mosquitto *context;

	જો(clientid == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}

	context = ખાલી;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	જો(context == ખાલી){
		પાછા MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર સંરચના mosquitto_base_સંદેશ *find_store_સંદેશ(uint64_t store_id)
{
	સંરચના mosquitto_base_સંદેશ *base_સંદેશ;

	HASH_FIND(hh, db.સંદેશ_store, &store_id, કદ(store_id), base_સંદેશ);
	પાછા base_સંદેશ;
}


BROKER_EXPORT પૂર્ણાંક mosquitto_persist_client_સંદેશ_add(સંરચના mosquitto_client_સંદેશ_2 *client_સંદેશ)
{
	સંરચના mosquitto *context;
	સંરચના mosquitto_base_સંદેશ *base_સંદેશ;

	જો(client_સંદેશ == ખાલી || client_સંદેશ->clientid == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_સંદેશ->clientid, strlen(client_સંદેશ->clientid), context);
	જો(context == ખાલી){
		પાછા MOSQ_ERR_NOT_FOUND;
	}
	base_સંદેશ = find_store_સંદેશ(client_સંદેશ->store_id);
	જો(base_સંદેશ == ખાલી){
		પાછા MOSQ_ERR_NOT_FOUND;
	}

	જો(client_સંદેશ->direction == mosq_md_out){
		જો(client_સંદેશ->qos > 0){
			context->last_mid = client_સંદેશ->mid;
		}
		પાછા db_message_ઉમેરો_outgoing(context, client_સંદેશ->cmsg_id, client_સંદેશ->mid,
				client_સંદેશ->qos, client_સંદેશ->retain,
				base_સંદેશ, client_સંદેશ->subscription_identifier, false, false);
	}નહીંતર જો(client_સંદેશ->direction == mosq_md_in){
		પાછા db_message_ઉમેરો_incoming(context, client_સંદેશ->cmsg_id, base_સંદેશ, false);
	}નહીંતર{
		પાછા MOSQ_ERR_INVAL;
	}
	પાછા MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT પૂર્ણાંક mosquitto_persist_client_સંદેશ_delete(સંરચના mosquitto_client_સંદેશ_2 *client_સંદેશ)
{
	સંરચના mosquitto *context;

	જો(client_સંદેશ == ખાલી || client_સંદેશ->clientid == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_સંદેશ->clientid, strlen(client_સંદેશ->clientid), context);
	જો(context == ખાલી){
		પાછા MOSQ_ERR_NOT_FOUND;
	}


	પૂર્ણાંક rc = MOSQ_ERR_INVAL;
	જો(client_સંદેશ->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_સંદેશ->mid, mosq_ms_any, client_સંદેશ->qos);
	}નહીંતર જો(client_સંદેશ->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_સંદેશ->mid);
	}
	પાછા rc;
}


BROKER_EXPORT પૂર્ણાંક mosquitto_persist_client_સંદેશ_update(સંરચના mosquitto_client_સંદેશ_2 *client_સંદેશ)
{
	સંરચના mosquitto *context;

	જો(client_સંદેશ == ખાલી || client_સંદેશ->clientid == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_સંદેશ->clientid, strlen(client_સંદેશ->clientid), context);
	જો(context == ખાલી){
		પાછા MOSQ_ERR_NOT_FOUND;
	}

	જો(client_સંદેશ->direction == mosq_md_out){
		db__message_update_outgoing(context, client_સંદેશ->mid, client_સંદેશ->state, client_સંદેશ->qos, false);
	}નહીંતર જો(client_સંદેશ->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}નહીંતર{
		પાછા MOSQ_ERR_INVAL;
	}
	પાછા MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT પૂર્ણાંક mosquitto_persist_client_સંદેશ_clear(સંરચના mosquitto_client_સંદેશ_2 *client_સંદેશ)
{
	સંરચના mosquitto *context;

	જો(client_સંદેશ == ખાલી || client_સંદેશ->clientid == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_સંદેશ->clientid, strlen(client_સંદેશ->clientid), context);
	જો(context == ખાલી){
		પાછા MOSQ_ERR_NOT_FOUND;
	}

	જો(client_સંદેશ->direction == mosq_bmd_in || client_સંદેશ->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	જો(client_સંદેશ->direction == mosq_bmd_out || client_સંદેશ->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	પાછા MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT પૂર્ણાંક mosquitto_subscription_add(અચલ સંરચના mosquitto_subscription *sub)
{
	સંરચના mosquitto *context;

	જો(sub == ખાલી || sub->clientid == ખાલી || sub->topic_filter == ખાલી || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		પાછા MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, strlen(sub->clientid), context);

	જો(context){
		પાછા sub__add(context, sub);
	}નહીંતર{
		પાછા MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT પૂર્ણાંક mosquitto_subscription_delete(અચલ અક્ષર *clientid, અચલ અક્ષર *topic)
{
	સંરચના mosquitto *context;
	uint8_t reason;

	જો(clientid == ખાલી || topic == ખાલી || clientid[0] == '\0' || topic[0] == '\0'){
		પાછા MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	જો(context){
		પાછા sub__remove(context, topic, &reason);
	}નહીંતર{
		પાછા MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT પૂર્ણાંક mosquitto_persist_base_સંદેશ_add(સંરચના mosquitto_base_સંદેશ_2 *સંદેશ_add)
{
	સંરચના mosquitto context;
	સંરચના mosquitto_base_સંદેશ *base_સંદેશ;
	પૂર્ણાંક rc;

	memset(&context, 0, કદ(context));

	જો(સંદેશ_add->payloadlen > MQTT_MAX_PAYLOAD){
		પાછા MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (અક્ષર *)સંદેશ_add->source_id;
	context.username = (અક્ષર *)સંદેશ_add->source_username;

	base_સંદેશ = mosquitto_calloc(1, કદ(સંરચના mosquitto_base_સંદેશ));
	જો(base_સંદેશ == ખાલી){
		જાઓ error;
	}
	base_સંદેશ->data.store_id = સંદેશ_add->store_id;
	base_સંદેશ->data.expiry_time = સંદેશ_add->expiry_time;
	base_સંદેશ->data.payloadlen = સંદેશ_add->payloadlen;
	base_સંદેશ->data.source_mid = સંદેશ_add->source_mid;
	base_સંદેશ->data.qos = સંદેશ_add->qos;
	base_સંદેશ->data.retain = સંદેશ_add->retain;

	base_સંદેશ->data.payload = સંદેશ_add->payload;
	સંદેશ_add->payload = ખાલી;
	base_સંદેશ->data.topic = સંદેશ_add->topic;
	સંદેશ_add->topic = ખાલી;
	base_સંદેશ->data.properties = સંદેશ_add->properties;
	સંદેશ_add->properties = ખાલી;

	જો(સંદેશ_add->source_port){
		માટે(પૂર્ણાંક i=0; i<db.config->listener_ગણતરી; i++){
			જો(db.config->listeners[i].port == સંદેશ_add->source_port){
				base_સંદેશ->source_listener = &db.config->listeners[i];
				તોડો;
			}
		}
	}

	base_સંદેશ->stored = true;
	rc = db__message_store(&context, base_સંદેશ, ખાલી, mosq_mo_broker);
	પાછા rc;

error:
	mosquitto_property_free_all(&સંદેશ_add->properties);
	mosquitto_free(સંદેશ_add->topic);
	mosquitto_free(સંદેશ_add->payload);
	mosquitto_free(base_સંદેશ);

	પાછા MOSQ_ERR_NOMEM;
}


BROKER_EXPORT પૂર્ણાંક mosquitto_persist_base_સંદેશ_delete(uint64_t store_id)
{
	સંરચના mosquitto_base_સંદેશ *base_સંદેશ;

	base_સંદેશ = find_store_સંદેશ(store_id);
	જો(base_સંદેશ && base_સંદેશ->ref_ગણતરી == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_સંદેશ_store_remove(base_સંદેશ, false);
	}

	પાછા MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT શૂન્ય mosquitto_complete_basic_auth(અચલ અક્ષર *clientid, પૂર્ણાંક result)
{
	સંરચના mosquitto *context;

	જો(clientid == ખાલી){
		પાછા;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, strlen(clientid), context);
	જો(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		જો(result == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, ખાલી, 0);
		}નહીંતર{
			જો(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ખાલી);
			}નહીંતર{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ખાલી);
			}
			context->clean_start = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT પૂર્ણાંક mosquitto_broker_ગાંઠ_id_set(uint16_t id)
{
	જો(id > 1023){
		પાછા MOSQ_ERR_INVAL;
	}નહીંતર{
		db.ગાંઠ_id = id;
		db.ગાંઠ_id_shifted = ((uint64_t)id) << 54;
		પાછા MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT અચલ અક્ષર *mosquitto_persistence_location(શૂન્ય)
{
	પાછા db.config->persistence_location;
}


BROKER_EXPORT પૂર્ણાંક mosquitto_client_will_set(અચલ અક્ષર *clientid, અચલ અક્ષર *topic, પૂર્ણાંક payloadlen, અચલ શૂન્ય *payload, પૂર્ણાંક qos, bool retain, mosquitto_property *properties)
{
	સંરચના mosquitto *mosq = mosquitto_client(clientid);
	જો(!mosq){
		પાછા MOSQ_ERR_NOT_FOUND;
	}
	જો(properties && mosq->protocol != mosq_p_mqtt5){
		જો(net__is_connected(mosq)){
			પાછા MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	પાછા will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
