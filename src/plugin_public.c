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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "send_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "will_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utlist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "will_mosq.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
#  include <openssl/ssl.h>
#ਜੇ_ਅੰਤ


BROKER_EXPORT ਪੂਰਨ mosquitto_plugin_ਸੈੱਟ_info(mosquitto_plugin_id_t *identifier,
		ਅਚਲ ਅੱਖਰ *plugin_name,
		ਅਚਲ ਅੱਖਰ *plugin_ਸੰਸਕਰਣ)
{
	ਜੇ(identifier == ਨਿਰਾ || plugin_name == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	ਜੇ(plugin_ਸੰਸਕਰਣ){
		identifier->plugin_ਸੰਸਕਰਣ = mosquitto_strdup(plugin_ਸੰਸਕਰਣ);
	}ਨਹੀਂਤਾਂ{
		identifier->plugin_ਸੰਸਕਰਣ = ਨਿਰਾ;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ਅਚਲ ਅੱਖਰ *mosquitto_client_ਪਤਾ(ਅਚਲ ਬਣਤਰ mosquitto *client)
{
	ਜੇ(client){
		ਵਾਪਸ client->ਪਤਾ;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ ਨਿਰਾ;
	}
}


BROKER_EXPORT ਬਣਤਰ mosquitto *mosquitto_client(ਅਚਲ ਅੱਖਰ *clientid)
{
	size_t len;
	ਬਣਤਰ mosquitto *context;

	ਜੇ(!clientid){
		ਵਾਪਸ ਨਿਰਾ;
	}
	len = ਲੜੀ_ਲੰਬਾਈ(clientid);
	ਜੇ(len == 0){
		ਵਾਪਸ ਨਿਰਾ;
	}

	HASH_ਲੱਭੋ(hh_id, db.contexts_by_id, clientid, ਲੜੀ_ਲੰਬਾਈ(clientid), context);

	ਵਾਪਸ context;
}


BROKER_EXPORT ਪੂਰਨ mosquitto_client_ਪੋਰਟ(ਅਚਲ ਬਣਤਰ mosquitto *client)
{
	ਜੇ(client && client->listener){
		ਵਾਪਸ client->listener->ਪੋਰਟ;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(ਅਚਲ ਬਣਤਰ mosquitto *client)
{
	ਜੇ(client){
		ਵਾਪਸ client->clean_ਸ਼ੁਰੂ;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ true;
	}
}


BROKER_EXPORT ਅਚਲ ਅੱਖਰ *mosquitto_client_id(ਅਚਲ ਬਣਤਰ mosquitto *client)
{
	ਜੇ(client){
		ਵਾਪਸ client->id;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ ਨਿਰਾ;
	}
}


BROKER_EXPORT ਬਿਨਾਨਿਸ਼ਾਨੀ mosquitto_client_id_hashv(ਅਚਲ ਬਣਤਰ mosquitto *client)
{
	ਜੇ(client){
		ਵਾਪਸ client->id_hashv;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ 0;
	}
}


BROKER_EXPORT ਪੂਰਨ mosquitto_client_keepalive(ਅਚਲ ਬਣਤਰ mosquitto *client)
{
	ਜੇ(client){
		ਵਾਪਸ client->keepalive;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ -1;
	}
}


BROKER_EXPORT ਖਾਲੀ *mosquitto_client_certificate(ਅਚਲ ਬਣਤਰ mosquitto *client)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	ਜੇ(client && client->ssl){
		ਵਾਪਸ SSL_get_peer_certificate(client->ssl);
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ ਨਿਰਾ;
	}
#ਨਹੀਂਤਾਂ
	ਅਣਵਰਤਿਆ(client);

	ਵਾਪਸ ਨਿਰਾ;
#ਜੇ_ਅੰਤ
}


BROKER_EXPORT ਪੂਰਨ mosquitto_client_protocol(ਅਚਲ ਬਣਤਰ mosquitto *client)
{
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ਜੇ(client && client->wsi){
		ਵਾਪਸ mp_websockets;
	}ਨਹੀਂਤਾਂ
#ਜਾਂਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	ਜੇ(client && client->transport == mosq_t_ws){
		ਵਾਪਸ mp_websockets;
	}ਨਹੀਂਤਾਂ
#ਨਹੀਂਤਾਂ
	ਅਣਵਰਤਿਆ(client);
#ਜੇ_ਅੰਤ
	{
		ਵਾਪਸ mp_mqtt;
	}
}


BROKER_EXPORT ਪੂਰਨ mosquitto_client_protocol_ਸੰਸਕਰਣ(ਅਚਲ ਬਣਤਰ mosquitto *client)
{
	ਜੇ(client){
		ਚੋਣ(client->protocol){
			ਹਾਲਤ mosq_p_mqtt31:
				ਵਾਪਸ 3;
			ਹਾਲਤ mosq_p_mqtt311:
				ਵਾਪਸ 4;
			ਹਾਲਤ mosq_p_mqtt5:
				ਵਾਪਸ 5;
			ਮੂਲ:
				ਵਾਪਸ 0;
		}
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ 0;
	}
}


BROKER_EXPORT ਪੂਰਨ mosquitto_client_sub_ਗਿਣਤੀ(ਅਚਲ ਬਣਤਰ mosquitto *client)
{
	ਜੇ(client){
		ਵਾਪਸ client->subs_ਗਿਣਤੀ;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ 0;
	}
}


BROKER_EXPORT ਅਚਲ ਅੱਖਰ *mosquitto_client_username(ਅਚਲ ਬਣਤਰ mosquitto *client)
{
	ਜੇ(client){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
		ਜੇ(client->bridge){
			ਵਾਪਸ client->bridge->local_username;
		}ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		{
			ਵਾਪਸ client->username;
		}
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ ਨਿਰਾ;
	}
}


BROKER_EXPORT ਪੂਰਨ mosquitto_broker_publish(
		ਅਚਲ ਅੱਖਰ *clientid,
		ਅਚਲ ਅੱਖਰ *topic,
		ਪੂਰਨ payloadlen,
		ਖਾਲੀ *payload,
		ਪੂਰਨ qos,
		bool retain,
		mosquitto_property *properties)
{
	ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_v5 *ਸੁਨੇਹਾ;

	ਜੇ(topic == ਨਿਰਾ
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == ਨਿਰਾ)
			|| qos < 0 || qos > 2){

		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਸੁਨੇਹਾ = mosquitto_malloc(ਆਕਾਰ(ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_v5));
	ਜੇ(ਸੁਨੇਹਾ == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	ਸੁਨੇਹਾ->ਅਗਲਾ = ਨਿਰਾ;
	ਸੁਨੇਹਾ->prev = ਨਿਰਾ;
	ਜੇ(clientid){
		ਸੁਨੇਹਾ->clientid = mosquitto_strdup(clientid);
		ਜੇ(ਸੁਨੇਹਾ->clientid == ਨਿਰਾ){
			mosquitto_FREE(ਸੁਨੇਹਾ);
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
	}ਨਹੀਂਤਾਂ{
		ਸੁਨੇਹਾ->clientid = ਨਿਰਾ;
	}
	ਸੁਨੇਹਾ->topic = mosquitto_strdup(topic);
	ਜੇ(ਸੁਨੇਹਾ->topic == ਨਿਰਾ){
		mosquitto_FREE(ਸੁਨੇਹਾ->clientid);
		mosquitto_FREE(ਸੁਨੇਹਾ);
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	ਸੁਨੇਹਾ->payloadlen = payloadlen;
	ਸੁਨੇਹਾ->payload = payload;
	ਸੁਨੇਹਾ->qos = qos;
	ਸੁਨੇਹਾ->retain = retain;
	ਸੁਨੇਹਾ->properties = properties;

	DL_APPEND(db.plugin_msgs, ਸੁਨੇਹਾ);

	loop_update_ਅਗਲਾ_event(1);
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ਪੂਰਨ mosquitto_broker_publish_copy(
		ਅਚਲ ਅੱਖਰ *clientid,
		ਅਚਲ ਅੱਖਰ *topic,
		ਪੂਰਨ payloadlen,
		ਅਚਲ ਖਾਲੀ *payload,
		ਪੂਰਨ qos,
		bool retain,
		mosquitto_property *properties)
{
	ਖਾਲੀ *payload_ਬਾਹਰਲਾ;
	ਪੂਰਨ rc;

	ਜੇ(topic == ਨਿਰਾ
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == ਨਿਰਾ)
			|| qos < 0 || qos > 2){

		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	payload_ਬਾਹਰਲਾ = mosquitto_calloc(1, (size_t)(payloadlen+1));
	ਜੇ(payload_ਬਾਹਰਲਾ == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(payload_ਬਾਹਰਲਾ, payload, (size_t)payloadlen);

	rc = mosquitto_broker_publish(
			clientid,
			topic,
			payloadlen,
			payload_ਬਾਹਰਲਾ,
			qos,
			retain,
			properties);

	ਜੇ(rc){
		mosquitto_FREE(payload_ਬਾਹਰਲਾ);
	}
	ਵਾਪਸ rc;
}


BROKER_EXPORT ਪੂਰਨ mosquitto_ਸੈੱਟ_username(ਬਣਤਰ mosquitto *client, ਅਚਲ ਅੱਖਰ *username)
{
	ਅੱਖਰ *u_dup;
	ਅੱਖਰ *old;

	ਜੇ(!client){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਜੇ(username){
		ਜੇ(mosquitto_validate_utf8(username, (ਪੂਰਨ)ਲੜੀ_ਲੰਬਾਈ(username))){
			ਵਾਪਸ MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		ਜੇ(!u_dup){
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
	}ਨਹੀਂਤਾਂ{
		u_dup = ਨਿਰਾ;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ਪੂਰਨ mosquitto_ਸੈੱਟ_clientid(ਬਣਤਰ mosquitto *client, ਅਚਲ ਅੱਖਰ *clientid)
{
	ਬਣਤਰ mosquitto *ਮਿਲਿਆ_client;
	ਅੱਖਰ *id_dup;
	bool ਵਿੱਚ_by_id;
	ਪੂਰਨ clientid_len;

	ਜੇ(!client || !clientid){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਵਿੱਚ_by_id = client->ਵਿੱਚ_by_id;
	/* If in_by_id is true, then this client has already authenticated and
	 * completed the connection flow. This means it *cannot* take over an
	 * existing session, and we must remove/add it to the by_id hash table.
	 *
	 * If in_by_id is false, then this client is currently going through
	 * authentication and so it is safe to change the client id to any value
	 * because it will be checked after authentication.
	 */

	ਜੇ(ਵਿੱਚ_by_id){
		HASH_ਲੱਭੋ(hh_id, db.contexts_by_id, clientid, ਲੜੀ_ਲੰਬਾਈ(clientid), ਮਿਲਿਆ_client);
		ਜੇ(ਮਿਲਿਆ_client){
			ਵਾਪਸ MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (ਪੂਰਨ)ਲੜੀ_ਲੰਬਾਈ(clientid);
	ਜੇ(mosquitto_validate_utf8(clientid, clientid_len)){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	ਜੇ(!id_dup){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	ਜੇ(ਵਿੱਚ_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	ਜੇ(ਵਿੱਚ_by_id){
		context_ਜੋੜੋ_ਮੰਜ਼ਿਲ_by_id(client);
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
ਸਥਿਰ ਖਾਲੀ check_subscription_acls(ਬਣਤਰ mosquitto *context)
{
	ਪੂਰਨ rc;
	uint8_t reason;

	ਲਈ(ਪੂਰਨ i=0; i<context->subs_ਸਮਰੱਥਾ; i++){
		ਜੇ(context->subs[i] == ਨਿਰਾ){
			ਜਾਰੀ;
		}
		rc = mosquitto_acl_check(context,
				context->subs[i]->topic_filter,
				0,
				ਨਿਰਾ,
				0, /* FIXME */
				false,
				ਨਿਰਾ,
				MOSQ_ACL_SUBSCRIBE);

		ਜੇ(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


ਸਥਿਰ ਖਾਲੀ disconnect_client(ਬਣਤਰ mosquitto *context, bool with_will)
{
	ਜੇ(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ਨਿਰਾ);
	}
	ਜੇ(with_will == false){
		mosquitto_ਸੈੱਟ_state(context, mosq_cs_disconnecting);
	}
	ਜੇ(context->session_expiry_interval != MQTT_SESSION_EXPIRY_ਤੁਰੰਤ){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT ਪੂਰਨ mosquitto_kick_client_by_clientid(ਅਚਲ ਅੱਖਰ *clientid, bool with_will)
{
	ਬਣਤਰ mosquitto *ctxt, *ctxt_tmp;

	ਜੇ(clientid == ਨਿਰਾ){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}ਨਹੀਂਤਾਂ{
		HASH_ਲੱਭੋ(hh_id, db.contexts_by_id, clientid, ਲੜੀ_ਲੰਬਾਈ(clientid), ctxt);
		ਜੇ(ctxt){
			disconnect_client(ctxt, with_will);
			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}ਨਹੀਂਤਾਂ{
			ਵਾਪਸ MOSQ_ERR_NOT_ਮਿਲਿਆ;
		}
	}
}


BROKER_EXPORT ਪੂਰਨ mosquitto_kick_client_by_username(ਅਚਲ ਅੱਖਰ *username, bool with_will)
{
	ਬਣਤਰ mosquitto *ctxt, *ctxt_tmp;

	ਜੇ(username == ਨਿਰਾ){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			ਜੇ(ctxt->username == ਨਿਰਾ){
				disconnect_client(ctxt, with_will);
			}
		}
	}ਨਹੀਂਤਾਂ{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			ਜੇ(ctxt->username != ਨਿਰਾ && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ਪੂਰਨ mosquitto_apply_on_all_clients(ਪੂਰਨ (*FUNC_client_functor)(ਅਚਲ ਬਣਤਰ mosquitto *, ਖਾਲੀ *), ਖਾਲੀ *functor_context)
{
	ਪੂਰਨ rc = MOSQ_ERR_SUCCESS;
	ਬਣਤਰ mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		ਜੇ(rc != MOSQ_ERR_SUCCESS){
			ਰੋਕੋ;
		}
	}

	ਵਾਪਸ rc;
}


BROKER_EXPORT ਪੂਰਨ mosquitto_persist_client_ਜੋੜੋ(ਬਣਤਰ mosquitto_client *client)
{
	ਬਣਤਰ mosquitto *context;
	ਪੂਰਨ rc;

	ਜੇ(client == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਜੇ(client->clientid == ਨਿਰਾ){
		rc = MOSQ_ERR_INVAL;
		ਜਾਓ error;
	}

	context = ਨਿਰਾ;
	HASH_ਲੱਭੋ(hh_id, db.contexts_by_id, client->clientid, ਲੜੀ_ਲੰਬਾਈ(client->clientid), context);
	ਜੇ(context){
		rc = MOSQ_ERR_INVAL;
		ਜਾਓ error;
	}

	context = context__init();
	ਜੇ(!context){
		rc = MOSQ_ERR_NOMEM;
		ਜਾਓ error;
	}

	context->id = client->clientid;
	client->clientid = ਨਿਰਾ;
	context->username = client->username;
	client->username = ਨਿਰਾ;
	context->auth_ਵਿਧੀ = client->auth_ਵਿਧੀ;
	client->auth_ਵਿਧੀ = ਨਿਰਾ;

	context->clean_ਸ਼ੁਰੂ = false;
	context->will_delay_ਸਮਾਂ = client->will_delay_ਸਮਾਂ;
	context->session_expiry_ਸਮਾਂ = client->session_expiry_ਸਮਾਂ;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->ਵੱਧ_ਤੋਂ_ਵੱਧ_packet_ਮਾਪ = client->max_packet_ਮਾਪ;
	context->retain_ਉਪਲਬਧ = client->retain_ਉਪਲਬਧ;
	context->is_persisted = true;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	ਜੇ(db.config->ਪ੍ਰਤੀ_listener_settings && client->listener_ਪੋਰਟ > 0){
		ਲਈ(ਪੂਰਨ i=0; i < db.config->listener_ਗਿਣਤੀ; i++){
			ਜੇ(db.config->listeners[i].ਪੋਰਟ == client->listener_ਪੋਰਟ){
				context->listener = &db.config->listeners[i];
				ਰੋਕੋ;
			}
		}
	}

	context_ਜੋੜੋ_ਮੰਜ਼ਿਲ_by_id(context);
	session_expiry_ਜੋੜੋ_from_persistence(context, context->session_expiry_ਸਮਾਂ);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
error:
	ਸੁਰੱਖਿਅਤ_FREE(client->clientid);
	ਸੁਰੱਖਿਅਤ_FREE(client->username);
	ਸੁਰੱਖਿਅਤ_FREE(client->auth_ਵਿਧੀ);
	ਵਾਪਸ rc;
}


BROKER_EXPORT ਪੂਰਨ mosquitto_persist_client_update(ਬਣਤਰ mosquitto_client *client)
{
	ਬਣਤਰ mosquitto *context;
	ਪੂਰਨ rc;

	ਜੇ(client == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਜੇ(client->clientid == ਨਿਰਾ){
		rc = MOSQ_ERR_INVAL;
		ਜਾਓ error;
	}

	context = ਨਿਰਾ;
	HASH_ਲੱਭੋ(hh_id, db.contexts_by_id, client->clientid, ਲੜੀ_ਲੰਬਾਈ(client->clientid), context);
	ਜੇ(context == ਨਿਰਾ){
		rc = MOSQ_ERR_NOT_ਮਿਲਿਆ;
		ਜਾਓ error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = ਨਿਰਾ;

	context->clean_ਸ਼ੁਰੂ = false;
	context->will_delay_ਸਮਾਂ = client->will_delay_ਸਮਾਂ;
	context->session_expiry_ਸਮਾਂ = client->session_expiry_ਸਮਾਂ;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->ਵੱਧ_ਤੋਂ_ਵੱਧ_packet_ਮਾਪ = client->max_packet_ਮਾਪ;
	context->retain_ਉਪਲਬਧ = client->retain_ਉਪਲਬਧ;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	ਜੇ(db.config->ਪ੍ਰਤੀ_listener_settings && client->listener_ਪੋਰਟ > 0){
		ਲਈ(ਪੂਰਨ i=0; i < db.config->listener_ਗਿਣਤੀ; i++){
			ਜੇ(db.config->listeners[i].ਪੋਰਟ == client->listener_ਪੋਰਟ){
				context->listener = &db.config->listeners[i];
				ਰੋਕੋ;
			}
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
error:
	ਸੁਰੱਖਿਅਤ_FREE(client->username);
	ਵਾਪਸ rc;
}


BROKER_EXPORT ਪੂਰਨ mosquitto_persist_client_delete(ਅਚਲ ਅੱਖਰ *clientid)
{
	ਬਣਤਰ mosquitto *context;

	ਜੇ(clientid == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	context = ਨਿਰਾ;
	HASH_ਲੱਭੋ(hh_id, db.contexts_by_id, clientid, ਲੜੀ_ਲੰਬਾਈ(clientid), context);
	ਜੇ(context == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_ਸ਼ੁਰੂ = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_ਤੁਰੰਤ;
	context->is_persisted = false;
	mosquitto_ਸੈੱਟ_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਲੱਭੋ_store_ਸੁਨੇਹਾ(uint64_t store_id)
{
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ;

	HASH_ਲੱਭੋ(hh, db.ਸੁਨੇਹਾ_store, &store_id, ਆਕਾਰ(store_id), ਅਧਾਰ_ਸੁਨੇਹਾ);
	ਵਾਪਸ ਅਧਾਰ_ਸੁਨੇਹਾ;
}


BROKER_EXPORT ਪੂਰਨ mosquitto_persist_client_ਸੁਨੇਹਾ_ਜੋੜੋ(ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ_2 *client_ਸੁਨੇਹਾ)
{
	ਬਣਤਰ mosquitto *context;
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ;

	ਜੇ(client_ਸੁਨੇਹਾ == ਨਿਰਾ || client_ਸੁਨੇਹਾ->clientid == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	HASH_ਲੱਭੋ(hh_id, db.contexts_by_id, client_ਸੁਨੇਹਾ->clientid, ਲੜੀ_ਲੰਬਾਈ(client_ਸੁਨੇਹਾ->clientid), context);
	ਜੇ(context == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOT_ਮਿਲਿਆ;
	}
	ਅਧਾਰ_ਸੁਨੇਹਾ = ਲੱਭੋ_store_ਸੁਨੇਹਾ(client_ਸੁਨੇਹਾ->store_id);
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOT_ਮਿਲਿਆ;
	}

	ਜੇ(client_ਸੁਨੇਹਾ->direction == mosq_md_ਬਾਹਰਲਾ){
		ਜੇ(client_ਸੁਨੇਹਾ->qos > 0){
			context->ਆਖਰੀ_mid = client_ਸੁਨੇਹਾ->mid;
		}
		ਵਾਪਸ db_ਸੁਨੇਹਾ_ਪਾਓ_outgoing(context, client_ਸੁਨੇਹਾ->cmsg_id, client_ਸੁਨੇਹਾ->mid,
				client_ਸੁਨੇਹਾ->qos, client_ਸੁਨੇਹਾ->retain,
				ਅਧਾਰ_ਸੁਨੇਹਾ, client_ਸੁਨੇਹਾ->subscription_identifier, false, false);
	}ਨਹੀਂਤਾਂ ਜੇ(client_ਸੁਨੇਹਾ->direction == mosq_md_ਵਿੱਚ){
		ਵਾਪਸ db_ਸੁਨੇਹਾ_ਪਾਓ_incoming(context, client_ਸੁਨੇਹਾ->cmsg_id, ਅਧਾਰ_ਸੁਨੇਹਾ, false);
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ਪੂਰਨ mosquitto_persist_client_ਸੁਨੇਹਾ_delete(ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ_2 *client_ਸੁਨੇਹਾ)
{
	ਬਣਤਰ mosquitto *context;

	ਜੇ(client_ਸੁਨੇਹਾ == ਨਿਰਾ || client_ਸੁਨੇਹਾ->clientid == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	HASH_ਲੱਭੋ(hh_id, db.contexts_by_id, client_ਸੁਨੇਹਾ->clientid, ਲੜੀ_ਲੰਬਾਈ(client_ਸੁਨੇਹਾ->clientid), context);
	ਜੇ(context == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOT_ਮਿਲਿਆ;
	}


	ਪੂਰਨ rc = MOSQ_ERR_INVAL;
	ਜੇ(client_ਸੁਨੇਹਾ->direction == mosq_md_ਬਾਹਰਲਾ){
		rc = db_ਸੁਨੇਹਾ_delete_outgoing(context, client_ਸੁਨੇਹਾ->mid, mosq_ms_any, client_ਸੁਨੇਹਾ->qos);
	}ਨਹੀਂਤਾਂ ਜੇ(client_ਸੁਨੇਹਾ->direction == mosq_md_ਵਿੱਚ){
		rc = db_ਸੁਨੇਹਾ_remove_incoming(context, client_ਸੁਨੇਹਾ->mid);
	}
	ਵਾਪਸ rc;
}


BROKER_EXPORT ਪੂਰਨ mosquitto_persist_client_ਸੁਨੇਹਾ_update(ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ_2 *client_ਸੁਨੇਹਾ)
{
	ਬਣਤਰ mosquitto *context;

	ਜੇ(client_ਸੁਨੇਹਾ == ਨਿਰਾ || client_ਸੁਨੇਹਾ->clientid == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	HASH_ਲੱਭੋ(hh_id, db.contexts_by_id, client_ਸੁਨੇਹਾ->clientid, ਲੜੀ_ਲੰਬਾਈ(client_ਸੁਨੇਹਾ->clientid), context);
	ਜੇ(context == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOT_ਮਿਲਿਆ;
	}

	ਜੇ(client_ਸੁਨੇਹਾ->direction == mosq_md_ਬਾਹਰਲਾ){
		db_ਸੁਨੇਹਾ_update_outgoing(context, client_ਸੁਨੇਹਾ->mid, client_ਸੁਨੇਹਾ->state, client_ਸੁਨੇਹਾ->qos, false);
	}ਨਹੀਂਤਾਂ ਜੇ(client_ਸੁਨੇਹਾ->direction == mosq_md_ਵਿੱਚ){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ਪੂਰਨ mosquitto_persist_client_ਸੁਨੇਹਾ_clear(ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ_2 *client_ਸੁਨੇਹਾ)
{
	ਬਣਤਰ mosquitto *context;

	ਜੇ(client_ਸੁਨੇਹਾ == ਨਿਰਾ || client_ਸੁਨੇਹਾ->clientid == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	HASH_ਲੱਭੋ(hh_id, db.contexts_by_id, client_ਸੁਨੇਹਾ->clientid, ਲੜੀ_ਲੰਬਾਈ(client_ਸੁਨੇਹਾ->clientid), context);
	ਜੇ(context == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOT_ਮਿਲਿਆ;
	}

	ਜੇ(client_ਸੁਨੇਹਾ->direction == mosq_bmd_ਵਿੱਚ || client_ਸੁਨੇਹਾ->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	ਜੇ(client_ਸੁਨੇਹਾ->direction == mosq_bmd_ਬਾਹਰਲਾ || client_ਸੁਨੇਹਾ->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ਪੂਰਨ mosquitto_subscription_ਜੋੜੋ(ਅਚਲ ਬਣਤਰ mosquitto_subscription *sub)
{
	ਬਣਤਰ mosquitto *context;

	ਜੇ(sub == ਨਿਰਾ || sub->clientid == ਨਿਰਾ || sub->topic_filter == ਨਿਰਾ || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	HASH_ਲੱਭੋ(hh_id, db.contexts_by_id, sub->clientid, ਲੜੀ_ਲੰਬਾਈ(sub->clientid), context);

	ਜੇ(context){
		ਵਾਪਸ sub_ਜੋੜੋ(context, sub);
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ MOSQ_ERR_NOT_ਮਿਲਿਆ;
	}
}


BROKER_EXPORT ਪੂਰਨ mosquitto_subscription_delete(ਅਚਲ ਅੱਖਰ *clientid, ਅਚਲ ਅੱਖਰ *topic)
{
	ਬਣਤਰ mosquitto *context;
	uint8_t reason;

	ਜੇ(clientid == ਨਿਰਾ || topic == ਨਿਰਾ || clientid[0] == '\0' || topic[0] == '\0'){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	HASH_ਲੱਭੋ(hh_id, db.contexts_by_id, clientid, ਲੜੀ_ਲੰਬਾਈ(clientid), context);

	ਜੇ(context){
		ਵਾਪਸ sub__remove(context, topic, &reason);
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ MOSQ_ERR_NOT_ਮਿਲਿਆ;
	}
}


BROKER_EXPORT ਪੂਰਨ mosquitto_persist_ਅਧਾਰ_ਸੁਨੇਹਾ_ਜੋੜੋ(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ_2 *ਸੁਨੇਹਾ_ਜੋੜੋ)
{
	ਬਣਤਰ mosquitto context;
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ;
	ਪੂਰਨ rc;

	memset(&context, 0, ਆਕਾਰ(context));

	ਜੇ(ਸੁਨੇਹਾ_ਜੋੜੋ->payloadlen > MQTT_MAX_PAYLOAD){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (ਅੱਖਰ *)ਸੁਨੇਹਾ_ਜੋੜੋ->ਸਰੋਤ_id;
	context.username = (ਅੱਖਰ *)ਸੁਨੇਹਾ_ਜੋੜੋ->ਸਰੋਤ_username;

	ਅਧਾਰ_ਸੁਨੇਹਾ = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ));
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ == ਨਿਰਾ){
		ਜਾਓ error;
	}
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.store_id = ਸੁਨੇਹਾ_ਜੋੜੋ->store_id;
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.expiry_ਸਮਾਂ = ਸੁਨੇਹਾ_ਜੋੜੋ->expiry_ਸਮਾਂ;
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen = ਸੁਨੇਹਾ_ਜੋੜੋ->payloadlen;
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid = ਸੁਨੇਹਾ_ਜੋੜੋ->ਸਰੋਤ_mid;
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos = ਸੁਨੇਹਾ_ਜੋੜੋ->qos;
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain = ਸੁਨੇਹਾ_ਜੋੜੋ->retain;

	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload = ਸੁਨੇਹਾ_ਜੋੜੋ->payload;
	ਸੁਨੇਹਾ_ਜੋੜੋ->payload = ਨਿਰਾ;
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic = ਸੁਨੇਹਾ_ਜੋੜੋ->topic;
	ਸੁਨੇਹਾ_ਜੋੜੋ->topic = ਨਿਰਾ;
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.properties = ਸੁਨੇਹਾ_ਜੋੜੋ->properties;
	ਸੁਨੇਹਾ_ਜੋੜੋ->properties = ਨਿਰਾ;

	ਜੇ(ਸੁਨੇਹਾ_ਜੋੜੋ->ਸਰੋਤ_ਪੋਰਟ){
		ਲਈ(ਪੂਰਨ i=0; i<db.config->listener_ਗਿਣਤੀ; i++){
			ਜੇ(db.config->listeners[i].ਪੋਰਟ == ਸੁਨੇਹਾ_ਜੋੜੋ->ਸਰੋਤ_ਪੋਰਟ){
				ਅਧਾਰ_ਸੁਨੇਹਾ->ਸਰੋਤ_listener = &db.config->listeners[i];
				ਰੋਕੋ;
			}
		}
	}

	ਅਧਾਰ_ਸੁਨੇਹਾ->stored = true;
	rc = db_ਸੁਨੇਹਾ_store(&context, ਅਧਾਰ_ਸੁਨੇਹਾ, ਨਿਰਾ, mosq_mo_broker);
	ਵਾਪਸ rc;

error:
	mosquitto_property_free_all(&ਸੁਨੇਹਾ_ਜੋੜੋ->properties);
	mosquitto_free(ਸੁਨੇਹਾ_ਜੋੜੋ->topic);
	mosquitto_free(ਸੁਨੇਹਾ_ਜੋੜੋ->payload);
	mosquitto_free(ਅਧਾਰ_ਸੁਨੇਹਾ);

	ਵਾਪਸ MOSQ_ERR_NOMEM;
}


BROKER_EXPORT ਪੂਰਨ mosquitto_persist_ਅਧਾਰ_ਸੁਨੇਹਾ_delete(uint64_t store_id)
{
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ;

	ਅਧਾਰ_ਸੁਨੇਹਾ = ਲੱਭੋ_store_ਸੁਨੇਹਾ(store_id);
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ && ਅਧਾਰ_ਸੁਨੇਹਾ->ref_ਗਿਣਤੀ == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_ਸੁਨੇਹਾ_store_remove(ਅਧਾਰ_ਸੁਨੇਹਾ, false);
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ਖਾਲੀ mosquitto_complete_basic_auth(ਅਚਲ ਅੱਖਰ *clientid, ਪੂਰਨ ਨਤੀਜਾ)
{
	ਬਣਤਰ mosquitto *context;

	ਜੇ(clientid == ਨਿਰਾ){
		ਵਾਪਸ;
	}

	HASH_ਲੱਭੋ(hh_id, db.contexts_by_id_delayed_auth, clientid, ਲੜੀ_ਲੰਬਾਈ(clientid), context);
	ਜੇ(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		ਜੇ(ਨਤੀਜਾ == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, ਨਿਰਾ, 0);
		}ਨਹੀਂਤਾਂ{
			ਜੇ(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ਨਿਰਾ);
			}ਨਹੀਂਤਾਂ{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ਨਿਰਾ);
			}
			context->clean_ਸ਼ੁਰੂ = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_ਤੁਰੰਤ;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT ਪੂਰਨ mosquitto_broker_ਗੰਢ_id_ਸੈੱਟ(uint16_t id)
{
	ਜੇ(id > 1023){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}ਨਹੀਂਤਾਂ{
		db.ਗੰਢ_id = id;
		db.ਗੰਢ_id_shifted = ((uint64_t)id) << 54;
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT ਅਚਲ ਅੱਖਰ *mosquitto_persistence_location(ਖਾਲੀ)
{
	ਵਾਪਸ db.config->persistence_location;
}


BROKER_EXPORT ਪੂਰਨ mosquitto_client_will_ਸੈੱਟ(ਅਚਲ ਅੱਖਰ *clientid, ਅਚਲ ਅੱਖਰ *topic, ਪੂਰਨ payloadlen, ਅਚਲ ਖਾਲੀ *payload, ਪੂਰਨ qos, bool retain, mosquitto_property *properties)
{
	ਬਣਤਰ mosquitto *mosq = mosquitto_client(clientid);
	ਜੇ(!mosq){
		ਵਾਪਸ MOSQ_ERR_NOT_ਮਿਲਿਆ;
	}
	ਜੇ(properties && mosq->protocol != mosq_p_mqtt5){
		ਜੇ(net__is_connected(mosq)){
			ਵਾਪਸ MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	ਵਾਪਸ will_ਸੈੱਟ(mosq, topic, payloadlen, payload, qos, retain, properties);
}
