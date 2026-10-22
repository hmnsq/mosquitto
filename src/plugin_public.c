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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"
#ಅನ್ತರ್ಭಾವ "will_mosq.h"
#ಅನ್ತರ್ಭಾವ "utlist.h"
#ಅನ್ತರ್ಭಾವ "will_mosq.h"

#ifdef WITH_TLS
#  include <openssl/ssl.h>
#endif


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_plugin_set_info(mosquitto_plugin_id_t *identifier,
		ಧ್ರುವ ವರ್ಣ *plugin_name,
		ಧ್ರುವ ವರ್ಣ *plugin_version)
{
	ಯದಿ(identifier == ರಿಕ್ತ || plugin_name == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	ಯದಿ(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}ಅನ್ಯಥಾ{
		identifier->plugin_version = ರಿಕ್ತ;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಧ್ರುವ ವರ್ಣ *mosquitto_client_address(ಧ್ರುವ ಸಂರಚನಾ mosquitto *client)
{
	ಯದಿ(client){
		ಪ್ರತ್ಯಾಗಮ client->address;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
}


BROKER_EXPORT ಸಂರಚನಾ mosquitto *mosquitto_client(ಧ್ರುವ ವರ್ಣ *clientid)
{
	size_t len;
	ಸಂರಚನಾ mosquitto *context;

	ಯದಿ(!clientid){
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	len = strlen(clientid);
	ಯದಿ(len == 0){
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	ಪ್ರತ್ಯಾಗಮ context;
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_client_port(ಧ್ರುವ ಸಂರಚನಾ mosquitto *client)
{
	ಯದಿ(client && client->listener){
		ಪ್ರತ್ಯಾಗಮ client->listener->port;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(ಧ್ರುವ ಸಂರಚನಾ mosquitto *client)
{
	ಯದಿ(client){
		ಪ್ರತ್ಯಾಗಮ client->clean_start;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ true;
	}
}


BROKER_EXPORT ಧ್ರುವ ವರ್ಣ *mosquitto_client_id(ಧ್ರುವ ಸಂರಚನಾ mosquitto *client)
{
	ಯದಿ(client){
		ಪ್ರತ್ಯಾಗಮ client->id;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
}


BROKER_EXPORT unsigned mosquitto_client_id_hashv(ಧ್ರುವ ಸಂರಚನಾ mosquitto *client)
{
	ಯದಿ(client){
		ಪ್ರತ್ಯಾಗಮ client->id_hashv;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ 0;
	}
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_client_keepalive(ಧ್ರುವ ಸಂರಚನಾ mosquitto *client)
{
	ಯದಿ(client){
		ಪ್ರತ್ಯಾಗಮ client->keepalive;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ -1;
	}
}


BROKER_EXPORT ಶೂನ್ಯ *mosquitto_client_certificate(ಧ್ರುವ ಸಂರಚನಾ mosquitto *client)
{
#ifdef WITH_TLS
	ಯದಿ(client && client->ssl){
		ಪ್ರತ್ಯಾಗಮ SSL_get_peer_certificate(client->ssl);
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
#ಅನ್ಯಥಾ
	UNUSED(client);

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#endif
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_client_protocol(ಧ್ರುವ ಸಂರಚನಾ mosquitto *client)
{
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ಯದಿ(client && client->wsi){
		ಪ್ರತ್ಯಾಗಮ mp_websockets;
	}ಅನ್ಯಥಾ
#ನೋಚೇದ್ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	ಯದಿ(client && client->transport == mosq_t_ws){
		ಪ್ರತ್ಯಾಗಮ mp_websockets;
	}ಅನ್ಯಥಾ
#ಅನ್ಯಥಾ
	UNUSED(client);
#endif
	{
		ಪ್ರತ್ಯಾಗಮ mp_mqtt;
	}
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_client_protocol_version(ಧ್ರುವ ಸಂರಚನಾ mosquitto *client)
{
	ಯದಿ(client){
		ವಿಕಲ್ಪ(client->protocol){
			ಪಕ್ಷ mosq_p_mqtt31:
				ಪ್ರತ್ಯಾಗಮ 3;
			ಪಕ್ಷ mosq_p_mqtt311:
				ಪ್ರತ್ಯಾಗಮ 4;
			ಪಕ್ಷ mosq_p_mqtt5:
				ಪ್ರತ್ಯಾಗಮ 5;
			ಸ್ವಭಾವ:
				ಪ್ರತ್ಯಾಗಮ 0;
		}
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ 0;
	}
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_client_sub_ಎಣಿಕೆ(ಧ್ರುವ ಸಂರಚನಾ mosquitto *client)
{
	ಯದಿ(client){
		ಪ್ರತ್ಯಾಗಮ client->subs_ಎಣಿಕೆ;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ 0;
	}
}


BROKER_EXPORT ಧ್ರುವ ವರ್ಣ *mosquitto_client_username(ಧ್ರುವ ಸಂರಚನಾ mosquitto *client)
{
	ಯದಿ(client){
#ifdef WITH_BRIDGE
		ಯದಿ(client->bridge){
			ಪ್ರತ್ಯಾಗಮ client->bridge->local_username;
		}ಅನ್ಯಥಾ
#endif
		{
			ಪ್ರತ್ಯಾಗಮ client->username;
		}
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_broker_publish(
		ಧ್ರುವ ವರ್ಣ *clientid,
		ಧ್ರುವ ವರ್ಣ *topic,
		ಪೂರ್ಣಾಙ್ಕ payloadlen,
		ಶೂನ್ಯ *payload,
		ಪೂರ್ಣಾಙ್ಕ qos,
		bool retain,
		mosquitto_property *properties)
{
	ಸಂರಚನಾ mosquitto__message_v5 *ಸಂದೇಶ;

	ಯದಿ(topic == ರಿಕ್ತ
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == ರಿಕ್ತ)
			|| qos < 0 || qos > 2){

		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	ಸಂದೇಶ = mosquitto_malloc(ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__message_v5));
	ಯದಿ(ಸಂದೇಶ == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	ಸಂದೇಶ->next = ರಿಕ್ತ;
	ಸಂದೇಶ->prev = ರಿಕ್ತ;
	ಯದಿ(clientid){
		ಸಂದೇಶ->clientid = mosquitto_strdup(clientid);
		ಯದಿ(ಸಂದೇಶ->clientid == ರಿಕ್ತ){
			mosquitto_FREE(ಸಂದೇಶ);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
	}ಅನ್ಯಥಾ{
		ಸಂದೇಶ->clientid = ರಿಕ್ತ;
	}
	ಸಂದೇಶ->topic = mosquitto_strdup(topic);
	ಯದಿ(ಸಂದೇಶ->topic == ರಿಕ್ತ){
		mosquitto_FREE(ಸಂದೇಶ->clientid);
		mosquitto_FREE(ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	ಸಂದೇಶ->payloadlen = payloadlen;
	ಸಂದೇಶ->payload = payload;
	ಸಂದೇಶ->qos = qos;
	ಸಂದೇಶ->retain = retain;
	ಸಂದೇಶ->properties = properties;

	DL_APPEND(db.plugin_msgs, ಸಂದೇಶ);

	loop__update_next_event(1);
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_broker_publish_copy(
		ಧ್ರುವ ವರ್ಣ *clientid,
		ಧ್ರುವ ವರ್ಣ *topic,
		ಪೂರ್ಣಾಙ್ಕ payloadlen,
		ಧ್ರುವ ಶೂನ್ಯ *payload,
		ಪೂರ್ಣಾಙ್ಕ qos,
		bool retain,
		mosquitto_property *properties)
{
	ಶೂನ್ಯ *payload_out;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(topic == ರಿಕ್ತ
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == ರಿಕ್ತ)
			|| qos < 0 || qos > 2){

		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (size_t)(payloadlen+1));
	ಯದಿ(payload_out == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
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

	ಯದಿ(rc){
		mosquitto_FREE(payload_out);
	}
	ಪ್ರತ್ಯಾಗಮ rc;
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_set_username(ಸಂರಚನಾ mosquitto *client, ಧ್ರುವ ವರ್ಣ *username)
{
	ವರ್ಣ *u_dup;
	ವರ್ಣ *old;

	ಯದಿ(!client){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	ಯದಿ(username){
		ಯದಿ(mosquitto_validate_utf8(username, (ಪೂರ್ಣಾಙ್ಕ)strlen(username))){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		ಯದಿ(!u_dup){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
	}ಅನ್ಯಥಾ{
		u_dup = ರಿಕ್ತ;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_set_clientid(ಸಂರಚನಾ mosquitto *client, ಧ್ರುವ ವರ್ಣ *clientid)
{
	ಸಂರಚನಾ mosquitto *found_client;
	ವರ್ಣ *id_dup;
	bool in_by_id;
	ಪೂರ್ಣಾಙ್ಕ clientid_len;

	ಯದಿ(!client || !clientid){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
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

	ಯದಿ(in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), found_client);
		ಯದಿ(found_client){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (ಪೂರ್ಣಾಙ್ಕ)strlen(clientid);
	ಯದಿ(mosquitto_validate_utf8(clientid, clientid_len)){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	ಯದಿ(!id_dup){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	ಯದಿ(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	ಯದಿ(in_by_id){
		context__add_to_by_id(client);
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
ಸ್ಥಿರ ಶೂನ್ಯ check_subscription_acls(ಸಂರಚನಾ mosquitto *context)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	uint8_t reason;

	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<context->subs_capacity; i++){
		ಯದಿ(context->subs[i] == ರಿಕ್ತ){
			ಅನುವರ್ತ;
		}
		rc = mosquitto_acl_check(context,
				context->subs[i]->topic_filter,
				0,
				ರಿಕ್ತ,
				0, /* FIXME */
				false,
				ರಿಕ್ತ,
				MOSQ_ACL_SUBSCRIBE);

		ಯದಿ(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


ಸ್ಥಿರ ಶೂನ್ಯ disconnect_client(ಸಂರಚನಾ mosquitto *context, bool with_will)
{
	ಯದಿ(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ರಿಕ್ತ);
	}
	ಯದಿ(with_will == false){
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	ಯದಿ(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_kick_client_by_clientid(ಧ್ರುವ ವರ್ಣ *clientid, bool with_will)
{
	ಸಂರಚನಾ mosquitto *ctxt, *ctxt_tmp;

	ಯದಿ(clientid == ರಿಕ್ತ){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}ಅನ್ಯಥಾ{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), ctxt);
		ಯದಿ(ctxt){
			disconnect_client(ctxt, with_will);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
		}ಅನ್ಯಥಾ{
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_kick_client_by_username(ಧ್ರುವ ವರ್ಣ *username, bool with_will)
{
	ಸಂರಚನಾ mosquitto *ctxt, *ctxt_tmp;

	ಯದಿ(username == ರಿಕ್ತ){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			ಯದಿ(ctxt->username == ರಿಕ್ತ){
				disconnect_client(ctxt, with_will);
			}
		}
	}ಅನ್ಯಥಾ{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			ಯದಿ(ctxt->username != ರಿಕ್ತ && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_apply_on_all_clients(ಪೂರ್ಣಾಙ್ಕ (*FUNC_client_functor)(ಧ್ರುವ ಸಂರಚನಾ mosquitto *, ಶೂನ್ಯ *), ಶೂನ್ಯ *functor_context)
{
	ಪೂರ್ಣಾಙ್ಕ rc = MOSQ_ERR_SUCCESS;
	ಸಂರಚನಾ mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		ಯದಿ(rc != MOSQ_ERR_SUCCESS){
			ಭಙ್ಗ;
		}
	}

	ಪ್ರತ್ಯಾಗಮ rc;
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_persist_client_add(ಸಂರಚನಾ mosquitto_client *client)
{
	ಸಂರಚನಾ mosquitto *context;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(client == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಯದಿ(client->clientid == ರಿಕ್ತ){
		rc = MOSQ_ERR_INVAL;
		ಗಚ್ಛ error;
	}

	context = ರಿಕ್ತ;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	ಯದಿ(context){
		rc = MOSQ_ERR_INVAL;
		ಗಚ್ಛ error;
	}

	context = context__init();
	ಯದಿ(!context){
		rc = MOSQ_ERR_NOMEM;
		ಗಚ್ಛ error;
	}

	context->id = client->clientid;
	client->clientid = ರಿಕ್ತ;
	context->username = client->username;
	client->username = ರಿಕ್ತ;
	context->auth_method = client->auth_method;
	client->auth_method = ರಿಕ್ತ;

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
	ಯದಿ(db.config->per_listener_settings && client->listener_port > 0){
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i < db.config->listener_ಎಣಿಕೆ; i++){
			ಯದಿ(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				ಭಙ್ಗ;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	ಪ್ರತ್ಯಾಗಮ rc;
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_persist_client_update(ಸಂರಚನಾ mosquitto_client *client)
{
	ಸಂರಚನಾ mosquitto *context;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(client == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಯದಿ(client->clientid == ರಿಕ್ತ){
		rc = MOSQ_ERR_INVAL;
		ಗಚ್ಛ error;
	}

	context = ರಿಕ್ತ;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	ಯದಿ(context == ರಿಕ್ತ){
		rc = MOSQ_ERR_NOT_FOUND;
		ಗಚ್ಛ error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = ರಿಕ್ತ;

	context->clean_start = false;
	context->will_delay_time = client->will_delay_time;
	context->session_expiry_time = client->session_expiry_time;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->maximum_packet_size = client->max_packet_size;
	context->retain_available = client->retain_available;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	ಯದಿ(db.config->per_listener_settings && client->listener_port > 0){
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i < db.config->listener_ಎಣಿಕೆ; i++){
			ಯದಿ(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				ಭಙ್ಗ;
			}
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	ಪ್ರತ್ಯಾಗಮ rc;
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_persist_client_delete(ಧ್ರುವ ವರ್ಣ *clientid)
{
	ಸಂರಚನಾ mosquitto *context;

	ಯದಿ(clientid == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	context = ರಿಕ್ತ;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	ಯದಿ(context == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *find_store_ಸಂದೇಶ(uint64_t store_id)
{
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ;

	HASH_FIND(hh, db.ಸಂದೇಶ_store, &store_id, ಪರಿಮಾಣ(store_id), base_ಸಂದೇಶ);
	ಪ್ರತ್ಯಾಗಮ base_ಸಂದೇಶ;
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_persist_client_ಸಂದೇಶ_add(ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ_2 *client_ಸಂದೇಶ)
{
	ಸಂರಚನಾ mosquitto *context;
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ;

	ಯದಿ(client_ಸಂದೇಶ == ರಿಕ್ತ || client_ಸಂದೇಶ->clientid == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_ಸಂದೇಶ->clientid, strlen(client_ಸಂದೇಶ->clientid), context);
	ಯದಿ(context == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_FOUND;
	}
	base_ಸಂದೇಶ = find_store_ಸಂದೇಶ(client_ಸಂದೇಶ->store_id);
	ಯದಿ(base_ಸಂದೇಶ == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_FOUND;
	}

	ಯದಿ(client_ಸಂದೇಶ->direction == mosq_md_out){
		ಯದಿ(client_ಸಂದೇಶ->qos > 0){
			context->last_mid = client_ಸಂದೇಶ->mid;
		}
		ಪ್ರತ್ಯಾಗಮ db_message_ಸೇರಿಸು_outgoing(context, client_ಸಂದೇಶ->cmsg_id, client_ಸಂದೇಶ->mid,
				client_ಸಂದೇಶ->qos, client_ಸಂದೇಶ->retain,
				base_ಸಂದೇಶ, client_ಸಂದೇಶ->subscription_identifier, false, false);
	}ಅನ್ಯಥಾ ಯದಿ(client_ಸಂದೇಶ->direction == mosq_md_in){
		ಪ್ರತ್ಯಾಗಮ db_message_ಸೇರಿಸು_incoming(context, client_ಸಂದೇಶ->cmsg_id, base_ಸಂದೇಶ, false);
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_persist_client_ಸಂದೇಶ_delete(ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ_2 *client_ಸಂದೇಶ)
{
	ಸಂರಚನಾ mosquitto *context;

	ಯದಿ(client_ಸಂದೇಶ == ರಿಕ್ತ || client_ಸಂದೇಶ->clientid == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_ಸಂದೇಶ->clientid, strlen(client_ಸಂದೇಶ->clientid), context);
	ಯದಿ(context == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_FOUND;
	}


	ಪೂರ್ಣಾಙ್ಕ rc = MOSQ_ERR_INVAL;
	ಯದಿ(client_ಸಂದೇಶ->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_ಸಂದೇಶ->mid, mosq_ms_any, client_ಸಂದೇಶ->qos);
	}ಅನ್ಯಥಾ ಯದಿ(client_ಸಂದೇಶ->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_ಸಂದೇಶ->mid);
	}
	ಪ್ರತ್ಯಾಗಮ rc;
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_persist_client_ಸಂದೇಶ_update(ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ_2 *client_ಸಂದೇಶ)
{
	ಸಂರಚನಾ mosquitto *context;

	ಯದಿ(client_ಸಂದೇಶ == ರಿಕ್ತ || client_ಸಂದೇಶ->clientid == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_ಸಂದೇಶ->clientid, strlen(client_ಸಂದೇಶ->clientid), context);
	ಯದಿ(context == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_FOUND;
	}

	ಯದಿ(client_ಸಂದೇಶ->direction == mosq_md_out){
		db__message_update_outgoing(context, client_ಸಂದೇಶ->mid, client_ಸಂದೇಶ->state, client_ಸಂದೇಶ->qos, false);
	}ಅನ್ಯಥಾ ಯದಿ(client_ಸಂದೇಶ->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_persist_client_ಸಂದೇಶ_clear(ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ_2 *client_ಸಂದೇಶ)
{
	ಸಂರಚನಾ mosquitto *context;

	ಯದಿ(client_ಸಂದೇಶ == ರಿಕ್ತ || client_ಸಂದೇಶ->clientid == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_ಸಂದೇಶ->clientid, strlen(client_ಸಂದೇಶ->clientid), context);
	ಯದಿ(context == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_FOUND;
	}

	ಯದಿ(client_ಸಂದೇಶ->direction == mosq_bmd_in || client_ಸಂದೇಶ->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	ಯದಿ(client_ಸಂದೇಶ->direction == mosq_bmd_out || client_ಸಂದೇಶ->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_subscription_add(ಧ್ರುವ ಸಂರಚನಾ mosquitto_subscription *sub)
{
	ಸಂರಚನಾ mosquitto *context;

	ಯದಿ(sub == ರಿಕ್ತ || sub->clientid == ರಿಕ್ತ || sub->topic_filter == ರಿಕ್ತ || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, strlen(sub->clientid), context);

	ಯದಿ(context){
		ಪ್ರತ್ಯಾಗಮ sub__add(context, sub);
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_subscription_delete(ಧ್ರುವ ವರ್ಣ *clientid, ಧ್ರುವ ವರ್ಣ *topic)
{
	ಸಂರಚನಾ mosquitto *context;
	uint8_t reason;

	ಯದಿ(clientid == ರಿಕ್ತ || topic == ರಿಕ್ತ || clientid[0] == '\0' || topic[0] == '\0'){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	ಯದಿ(context){
		ಪ್ರತ್ಯಾಗಮ sub__remove(context, topic, &reason);
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_persist_base_ಸಂದೇಶ_add(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ_2 *ಸಂದೇಶ_add)
{
	ಸಂರಚನಾ mosquitto context;
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ;
	ಪೂರ್ಣಾಙ್ಕ rc;

	memset(&context, 0, ಪರಿಮಾಣ(context));

	ಯದಿ(ಸಂದೇಶ_add->payloadlen > MQTT_MAX_PAYLOAD){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (ವರ್ಣ *)ಸಂದೇಶ_add->source_id;
	context.username = (ವರ್ಣ *)ಸಂದೇಶ_add->source_username;

	base_ಸಂದೇಶ = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ));
	ಯದಿ(base_ಸಂದೇಶ == ರಿಕ್ತ){
		ಗಚ್ಛ error;
	}
	base_ಸಂದೇಶ->data.store_id = ಸಂದೇಶ_add->store_id;
	base_ಸಂದೇಶ->data.expiry_time = ಸಂದೇಶ_add->expiry_time;
	base_ಸಂದೇಶ->data.payloadlen = ಸಂದೇಶ_add->payloadlen;
	base_ಸಂದೇಶ->data.source_mid = ಸಂದೇಶ_add->source_mid;
	base_ಸಂದೇಶ->data.qos = ಸಂದೇಶ_add->qos;
	base_ಸಂದೇಶ->data.retain = ಸಂದೇಶ_add->retain;

	base_ಸಂದೇಶ->data.payload = ಸಂದೇಶ_add->payload;
	ಸಂದೇಶ_add->payload = ರಿಕ್ತ;
	base_ಸಂದೇಶ->data.topic = ಸಂದೇಶ_add->topic;
	ಸಂದೇಶ_add->topic = ರಿಕ್ತ;
	base_ಸಂದೇಶ->data.properties = ಸಂದೇಶ_add->properties;
	ಸಂದೇಶ_add->properties = ರಿಕ್ತ;

	ಯದಿ(ಸಂದೇಶ_add->source_port){
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
			ಯದಿ(db.config->listeners[i].port == ಸಂದೇಶ_add->source_port){
				base_ಸಂದೇಶ->source_listener = &db.config->listeners[i];
				ಭಙ್ಗ;
			}
		}
	}

	base_ಸಂದೇಶ->stored = true;
	rc = db__message_store(&context, base_ಸಂದೇಶ, ರಿಕ್ತ, mosq_mo_broker);
	ಪ್ರತ್ಯಾಗಮ rc;

error:
	mosquitto_property_free_all(&ಸಂದೇಶ_add->properties);
	mosquitto_free(ಸಂದೇಶ_add->topic);
	mosquitto_free(ಸಂದೇಶ_add->payload);
	mosquitto_free(base_ಸಂದೇಶ);

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_persist_base_ಸಂದೇಶ_delete(uint64_t store_id)
{
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ;

	base_ಸಂದೇಶ = find_store_ಸಂದೇಶ(store_id);
	ಯದಿ(base_ಸಂದೇಶ && base_ಸಂದೇಶ->ref_ಎಣಿಕೆ == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_ಸಂದೇಶ_store_remove(base_ಸಂದೇಶ, false);
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಶೂನ್ಯ mosquitto_complete_basic_auth(ಧ್ರುವ ವರ್ಣ *clientid, ಪೂರ್ಣಾಙ್ಕ result)
{
	ಸಂರಚನಾ mosquitto *context;

	ಯದಿ(clientid == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, strlen(clientid), context);
	ಯದಿ(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		ಯದಿ(result == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, ರಿಕ್ತ, 0);
		}ಅನ್ಯಥಾ{
			ಯದಿ(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ರಿಕ್ತ);
			}ಅನ್ಯಥಾ{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ರಿಕ್ತ);
			}
			context->clean_start = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_broker_ಗಂಟು_id_set(uint16_t id)
{
	ಯದಿ(id > 1023){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}ಅನ್ಯಥಾ{
		db.ಗಂಟು_id = id;
		db.ಗಂಟು_id_shifted = ((uint64_t)id) << 54;
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT ಧ್ರುವ ವರ್ಣ *mosquitto_persistence_location(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ db.config->persistence_location;
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_client_will_set(ಧ್ರುವ ವರ್ಣ *clientid, ಧ್ರುವ ವರ್ಣ *topic, ಪೂರ್ಣಾಙ್ಕ payloadlen, ಧ್ರುವ ಶೂನ್ಯ *payload, ಪೂರ್ಣಾಙ್ಕ qos, bool retain, mosquitto_property *properties)
{
	ಸಂರಚನಾ mosquitto *mosq = mosquitto_client(clientid);
	ಯದಿ(!mosq){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_FOUND;
	}
	ಯದಿ(properties && mosq->protocol != mosq_p_mqtt5){
		ಯದಿ(net__is_connected(mosq)){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	ಪ್ರತ್ಯಾಗಮ will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
