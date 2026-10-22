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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
#  include <openssl/ssl.h>
#ಆದರೆ_ಅಂತ್ಯ


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_plugin_ಗಣ_info(mosquitto_plugin_id_t *identifier,
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *plugin_name,
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *plugin_ಆವೃತ್ತಿ)
{
	ಆದರೆ(identifier == ಖಾಲಿ || plugin_name == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	ಆದರೆ(plugin_ಆವೃತ್ತಿ){
		identifier->plugin_ಆವೃತ್ತಿ = mosquitto_strdup(plugin_ಆವೃತ್ತಿ);
	}ಇಲ್ಲದಿದ್ದರೆ{
		identifier->plugin_ಆವೃತ್ತಿ = ಖಾಲಿ;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mosquitto_client_ವಿಳಾಸ(ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto *client)
{
	ಆದರೆ(client){
		ಹಿಂತಿರುಗಿ client->ವಿಳಾಸ;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
}


BROKER_EXPORT ರಚನೆ mosquitto *mosquitto_client(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clientid)
{
	size_t len;
	ರಚನೆ mosquitto *context;

	ಆದರೆ(!clientid){
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(clientid);
	ಆದರೆ(len == 0){
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id, clientid, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(clientid), context);

	ಹಿಂತಿರುಗಿ context;
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_client_ಪೋರ್ಟ್(ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto *client)
{
	ಆದರೆ(client && client->listener){
		ಹಿಂತಿರುಗಿ client->listener->ಪೋರ್ಟ್;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto *client)
{
	ಆದರೆ(client){
		ಹಿಂತಿರುಗಿ client->clean_ಆರಂಭ;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ true;
	}
}


BROKER_EXPORT ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mosquitto_client_id(ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto *client)
{
	ಆದರೆ(client){
		ಹಿಂತಿರುಗಿ client->id;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
}


BROKER_EXPORT ಚಿಹ್ನೆರಹಿತ mosquitto_client_id_hashv(ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto *client)
{
	ಆದರೆ(client){
		ಹಿಂತಿರುಗಿ client->id_hashv;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ 0;
	}
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_client_keepalive(ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto *client)
{
	ಆದರೆ(client){
		ಹಿಂತಿರುಗಿ client->keepalive;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ -1;
	}
}


BROKER_EXPORT ಶೂನ್ಯ *mosquitto_client_certificate(ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto *client)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	ಆದರೆ(client && client->ssl){
		ಹಿಂತಿರುಗಿ SSL_get_peer_certificate(client->ssl);
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
#ಇಲ್ಲದಿದ್ದರೆ
	ಬಳಸದ(client);

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_client_protocol(ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto *client)
{
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ಆದರೆ(client && client->wsi){
		ಹಿಂತಿರುಗಿ mp_websockets;
	}ಇಲ್ಲದಿದ್ದರೆ
#ಅಥವಾಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	ಆದರೆ(client && client->transport == mosq_t_ws){
		ಹಿಂತಿರುಗಿ mp_websockets;
	}ಇಲ್ಲದಿದ್ದರೆ
#ಇಲ್ಲದಿದ್ದರೆ
	ಬಳಸದ(client);
#ಆದರೆ_ಅಂತ್ಯ
	{
		ಹಿಂತಿರುಗಿ mp_mqtt;
	}
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_client_protocol_ಆವೃತ್ತಿ(ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto *client)
{
	ಆದರೆ(client){
		ಆಯ್ಕೆ(client->protocol){
			ಸ್ಥಿತಿ mosq_p_mqtt31:
				ಹಿಂತಿರುಗಿ 3;
			ಸ್ಥಿತಿ mosq_p_mqtt311:
				ಹಿಂತಿರುಗಿ 4;
			ಸ್ಥಿತಿ mosq_p_mqtt5:
				ಹಿಂತಿರುಗಿ 5;
			ಪೂರ್ವನಿಯೋಜಿತ:
				ಹಿಂತಿರುಗಿ 0;
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ 0;
	}
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_client_sub_ಎಣಿಕೆ(ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto *client)
{
	ಆದರೆ(client){
		ಹಿಂತಿರುಗಿ client->subs_ಎಣಿಕೆ;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ 0;
	}
}


BROKER_EXPORT ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mosquitto_client_username(ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto *client)
{
	ಆದರೆ(client){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
		ಆದರೆ(client->bridge){
			ಹಿಂತಿರುಗಿ client->bridge->local_username;
		}ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		{
			ಹಿಂತಿರುಗಿ client->username;
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_broker_publish(
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clientid,
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic,
		ಪೂರ್ಣಾಂಕ payloadlen,
		ಶೂನ್ಯ *payload,
		ಪೂರ್ಣಾಂಕ qos,
		bool retain,
		mosquitto_property *properties)
{
	ರಚನೆ mosquitto_ಸಂದೇಶ_v5 *ಸಂದೇಶ;

	ಆದರೆ(topic == ಖಾಲಿ
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == ಖಾಲಿ)
			|| qos < 0 || qos > 2){

		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	ಸಂದೇಶ = mosquitto_malloc(ಗಾತ್ರ(ರಚನೆ mosquitto_ಸಂದೇಶ_v5));
	ಆದರೆ(ಸಂದೇಶ == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	ಸಂದೇಶ->ಮುಂದಿನ = ಖಾಲಿ;
	ಸಂದೇಶ->prev = ಖಾಲಿ;
	ಆದರೆ(clientid){
		ಸಂದೇಶ->clientid = mosquitto_strdup(clientid);
		ಆದರೆ(ಸಂದೇಶ->clientid == ಖಾಲಿ){
			mosquitto_FREE(ಸಂದೇಶ);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಸಂದೇಶ->clientid = ಖಾಲಿ;
	}
	ಸಂದೇಶ->topic = mosquitto_strdup(topic);
	ಆದರೆ(ಸಂದೇಶ->topic == ಖಾಲಿ){
		mosquitto_FREE(ಸಂದೇಶ->clientid);
		mosquitto_FREE(ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	ಸಂದೇಶ->payloadlen = payloadlen;
	ಸಂದೇಶ->payload = payload;
	ಸಂದೇಶ->qos = qos;
	ಸಂದೇಶ->retain = retain;
	ಸಂದೇಶ->properties = properties;

	DL_APPEND(db.plugin_msgs, ಸಂದೇಶ);

	loop_update_ಮುಂದಿನ_event(1);
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_broker_publish_copy(
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clientid,
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic,
		ಪೂರ್ಣಾಂಕ payloadlen,
		ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *payload,
		ಪೂರ್ಣಾಂಕ qos,
		bool retain,
		mosquitto_property *properties)
{
	ಶೂನ್ಯ *payload_ಹೊರಹೋಗು;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(topic == ಖಾಲಿ
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == ಖಾಲಿ)
			|| qos < 0 || qos > 2){

		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	payload_ಹೊರಹೋಗು = mosquitto_calloc(1, (size_t)(payloadlen+1));
	ಆದರೆ(payload_ಹೊರಹೋಗು == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	ಸ್ಮರಣೆ_ನಕಲು(payload_ಹೊರಹೋಗು, payload, (size_t)payloadlen);

	rc = mosquitto_broker_publish(
			clientid,
			topic,
			payloadlen,
			payload_ಹೊರಹೋಗು,
			qos,
			retain,
			properties);

	ಆದರೆ(rc){
		mosquitto_FREE(payload_ಹೊರಹೋಗು);
	}
	ಹಿಂತಿರುಗಿ rc;
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_ಗಣ_username(ರಚನೆ mosquitto *client, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *username)
{
	ಅಕ್ಷರ *u_dup;
	ಅಕ್ಷರ *old;

	ಆದರೆ(!client){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	ಆದರೆ(username){
		ಆದರೆ(mosquitto_validate_utf8(username, (ಪೂರ್ಣಾಂಕ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(username))){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		ಆದರೆ(!u_dup){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		u_dup = ಖಾಲಿ;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_ಗಣ_clientid(ರಚನೆ mosquitto *client, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clientid)
{
	ರಚನೆ mosquitto *ಸಿಕ್ಕಿತು_client;
	ಅಕ್ಷರ *id_dup;
	bool ಒಳ_by_id;
	ಪೂರ್ಣಾಂಕ clientid_len;

	ಆದರೆ(!client || !clientid){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	ಒಳ_by_id = client->ಒಳ_by_id;
	/* If in_by_id is true, then this client has already authenticated and
	 * completed the connection flow. This means it *cannot* take over an
	 * existing session, and we must remove/add it to the by_id hash table.
	 *
	 * If in_by_id is false, then this client is currently going through
	 * authentication and so it is safe to change the client id to any value
	 * because it will be checked after authentication.
	 */

	ಆದರೆ(ಒಳ_by_id){
		HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id, clientid, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(clientid), ಸಿಕ್ಕಿತು_client);
		ಆದರೆ(ಸಿಕ್ಕಿತು_client){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (ಪೂರ್ಣಾಂಕ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(clientid);
	ಆದರೆ(mosquitto_validate_utf8(clientid, clientid_len)){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	ಆದರೆ(!id_dup){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	ಆದರೆ(ಒಳ_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	ಆದರೆ(ಒಳ_by_id){
		context_ಕೂಡಿಸು_ಗಮ್ಯ_by_id(client);
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
ಸ್ಥಿರ ಶೂನ್ಯ check_subscription_acls(ರಚನೆ mosquitto *context)
{
	ಪೂರ್ಣಾಂಕ rc;
	uint8_t reason;

	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<context->subs_ಸಾಮರ್ಥ್ಯ; i++){
		ಆದರೆ(context->subs[i] == ಖಾಲಿ){
			ಮುಂದುವರಿ;
		}
		rc = mosquitto_acl_check(context,
				context->subs[i]->topic_filter,
				0,
				ಖಾಲಿ,
				0, /* FIXME */
				false,
				ಖಾಲಿ,
				MOSQ_ACL_SUBSCRIBE);

		ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


ಸ್ಥಿರ ಶೂನ್ಯ disconnect_client(ರಚನೆ mosquitto *context, bool with_will)
{
	ಆದರೆ(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ಖಾಲಿ);
	}
	ಆದರೆ(with_will == false){
		mosquitto_ಗಣ_state(context, mosq_cs_disconnecting);
	}
	ಆದರೆ(context->session_expiry_interval != MQTT_SESSION_EXPIRY_ತಕ್ಷಣ){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_kick_client_by_clientid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clientid, bool with_will)
{
	ರಚನೆ mosquitto *ctxt, *ctxt_tmp;

	ಆದರೆ(clientid == ಖಾಲಿ){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}ಇಲ್ಲದಿದ್ದರೆ{
		HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id, clientid, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(clientid), ctxt);
		ಆದರೆ(ctxt){
			disconnect_client(ctxt, with_will);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_ಸಿಕ್ಕಿತು;
		}
	}
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_kick_client_by_username(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *username, bool with_will)
{
	ರಚನೆ mosquitto *ctxt, *ctxt_tmp;

	ಆದರೆ(username == ಖಾಲಿ){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			ಆದರೆ(ctxt->username == ಖಾಲಿ){
				disconnect_client(ctxt, with_will);
			}
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			ಆದರೆ(ctxt->username != ಖಾಲಿ && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_apply_on_all_clients(ಪೂರ್ಣಾಂಕ (*FUNC_client_functor)(ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto *, ಶೂನ್ಯ *), ಶೂನ್ಯ *functor_context)
{
	ಪೂರ್ಣಾಂಕ rc = MOSQ_ERR_SUCCESS;
	ರಚನೆ mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
			ನಿಲ್ಲಿಸು;
		}
	}

	ಹಿಂತಿರುಗಿ rc;
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_persist_client_ಕೂಡಿಸು(ರಚನೆ mosquitto_client *client)
{
	ರಚನೆ mosquitto *context;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(client == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	ಆದರೆ(client->clientid == ಖಾಲಿ){
		rc = MOSQ_ERR_INVAL;
		ಹೋಗು error;
	}

	context = ಖಾಲಿ;
	HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id, client->clientid, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(client->clientid), context);
	ಆದರೆ(context){
		rc = MOSQ_ERR_INVAL;
		ಹೋಗು error;
	}

	context = context__init();
	ಆದರೆ(!context){
		rc = MOSQ_ERR_NOMEM;
		ಹೋಗು error;
	}

	context->id = client->clientid;
	client->clientid = ಖಾಲಿ;
	context->username = client->username;
	client->username = ಖಾಲಿ;
	context->auth_ವಿಧಾನ = client->auth_ವಿಧಾನ;
	client->auth_ವಿಧಾನ = ಖಾಲಿ;

	context->clean_ಆರಂಭ = false;
	context->will_delay_ಸಮಯ = client->will_delay_ಸಮಯ;
	context->session_expiry_ಸಮಯ = client->session_expiry_ಸಮಯ;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->ಗರಿಷ್ಠ_packet_ಗಾತ್ರ = client->max_packet_ಗಾತ್ರ;
	context->retain_ಲಭ್ಯ = client->retain_ಲಭ್ಯ;
	context->is_persisted = true;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	ಆದರೆ(db.config->ಪ್ರತಿ_listener_settings && client->listener_ಪೋರ್ಟ್ > 0){
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i < db.config->listener_ಎಣಿಕೆ; i++){
			ಆದರೆ(db.config->listeners[i].ಪೋರ್ಟ್ == client->listener_ಪೋರ್ಟ್){
				context->listener = &db.config->listeners[i];
				ನಿಲ್ಲಿಸು;
			}
		}
	}

	context_ಕೂಡಿಸು_ಗಮ್ಯ_by_id(context);
	session_expiry_ಕೂಡಿಸು_from_persistence(context, context->session_expiry_ಸಮಯ);

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
error:
	ಸುರಕ್ಷಿತ_FREE(client->clientid);
	ಸುರಕ್ಷಿತ_FREE(client->username);
	ಸುರಕ್ಷಿತ_FREE(client->auth_ವಿಧಾನ);
	ಹಿಂತಿರುಗಿ rc;
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_persist_client_update(ರಚನೆ mosquitto_client *client)
{
	ರಚನೆ mosquitto *context;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(client == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	ಆದರೆ(client->clientid == ಖಾಲಿ){
		rc = MOSQ_ERR_INVAL;
		ಹೋಗು error;
	}

	context = ಖಾಲಿ;
	HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id, client->clientid, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(client->clientid), context);
	ಆದರೆ(context == ಖಾಲಿ){
		rc = MOSQ_ERR_NOT_ಸಿಕ್ಕಿತು;
		ಹೋಗು error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = ಖಾಲಿ;

	context->clean_ಆರಂಭ = false;
	context->will_delay_ಸಮಯ = client->will_delay_ಸಮಯ;
	context->session_expiry_ಸಮಯ = client->session_expiry_ಸಮಯ;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->ಗರಿಷ್ಠ_packet_ಗಾತ್ರ = client->max_packet_ಗಾತ್ರ;
	context->retain_ಲಭ್ಯ = client->retain_ಲಭ್ಯ;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	ಆದರೆ(db.config->ಪ್ರತಿ_listener_settings && client->listener_ಪೋರ್ಟ್ > 0){
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i < db.config->listener_ಎಣಿಕೆ; i++){
			ಆದರೆ(db.config->listeners[i].ಪೋರ್ಟ್ == client->listener_ಪೋರ್ಟ್){
				context->listener = &db.config->listeners[i];
				ನಿಲ್ಲಿಸು;
			}
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
error:
	ಸುರಕ್ಷಿತ_FREE(client->username);
	ಹಿಂತಿರುಗಿ rc;
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_persist_client_delete(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clientid)
{
	ರಚನೆ mosquitto *context;

	ಆದರೆ(clientid == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	context = ಖಾಲಿ;
	HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id, clientid, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(clientid), context);
	ಆದರೆ(context == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_ಆರಂಭ = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_ತಕ್ಷಣ;
	context->is_persisted = false;
	mosquitto_ಗಣ_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ಕಂಡುಹಿಡಿ_store_ಸಂದೇಶ(uint64_t store_id)
{
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ;

	HASH_ಕಂಡುಹಿಡಿ(hh, db.ಸಂದೇಶ_store, &store_id, ಗಾತ್ರ(store_id), ತಳ_ಸಂದೇಶ);
	ಹಿಂತಿರುಗಿ ತಳ_ಸಂದೇಶ;
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_persist_client_ಸಂದೇಶ_ಕೂಡಿಸು(ರಚನೆ mosquitto_client_ಸಂದೇಶ_2 *client_ಸಂದೇಶ)
{
	ರಚನೆ mosquitto *context;
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ;

	ಆದರೆ(client_ಸಂದೇಶ == ಖಾಲಿ || client_ಸಂದೇಶ->clientid == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id, client_ಸಂದೇಶ->clientid, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(client_ಸಂದೇಶ->clientid), context);
	ಆದರೆ(context == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_ಸಿಕ್ಕಿತು;
	}
	ತಳ_ಸಂದೇಶ = ಕಂಡುಹಿಡಿ_store_ಸಂದೇಶ(client_ಸಂದೇಶ->store_id);
	ಆದರೆ(ತಳ_ಸಂದೇಶ == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_ಸಿಕ್ಕಿತು;
	}

	ಆದರೆ(client_ಸಂದೇಶ->direction == mosq_md_ಹೊರಹೋಗು){
		ಆದರೆ(client_ಸಂದೇಶ->qos > 0){
			context->ಕೊನೆಯ_mid = client_ಸಂದೇಶ->mid;
		}
		ಹಿಂತಿರುಗಿ db_ಸಂದೇಶ_ಸೇರಿಸು_outgoing(context, client_ಸಂದೇಶ->cmsg_id, client_ಸಂದೇಶ->mid,
				client_ಸಂದೇಶ->qos, client_ಸಂದೇಶ->retain,
				ತಳ_ಸಂದೇಶ, client_ಸಂದೇಶ->subscription_identifier, false, false);
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(client_ಸಂದೇಶ->direction == mosq_md_ಒಳ){
		ಹಿಂತಿರುಗಿ db_ಸಂದೇಶ_ಸೇರಿಸು_incoming(context, client_ಸಂದೇಶ->cmsg_id, ತಳ_ಸಂದೇಶ, false);
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_persist_client_ಸಂದೇಶ_delete(ರಚನೆ mosquitto_client_ಸಂದೇಶ_2 *client_ಸಂದೇಶ)
{
	ರಚನೆ mosquitto *context;

	ಆದರೆ(client_ಸಂದೇಶ == ಖಾಲಿ || client_ಸಂದೇಶ->clientid == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id, client_ಸಂದೇಶ->clientid, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(client_ಸಂದೇಶ->clientid), context);
	ಆದರೆ(context == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_ಸಿಕ್ಕಿತು;
	}


	ಪೂರ್ಣಾಂಕ rc = MOSQ_ERR_INVAL;
	ಆದರೆ(client_ಸಂದೇಶ->direction == mosq_md_ಹೊರಹೋಗು){
		rc = db_ಸಂದೇಶ_delete_outgoing(context, client_ಸಂದೇಶ->mid, mosq_ms_any, client_ಸಂದೇಶ->qos);
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(client_ಸಂದೇಶ->direction == mosq_md_ಒಳ){
		rc = db_ಸಂದೇಶ_remove_incoming(context, client_ಸಂದೇಶ->mid);
	}
	ಹಿಂತಿರುಗಿ rc;
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_persist_client_ಸಂದೇಶ_update(ರಚನೆ mosquitto_client_ಸಂದೇಶ_2 *client_ಸಂದೇಶ)
{
	ರಚನೆ mosquitto *context;

	ಆದರೆ(client_ಸಂದೇಶ == ಖಾಲಿ || client_ಸಂದೇಶ->clientid == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id, client_ಸಂದೇಶ->clientid, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(client_ಸಂದೇಶ->clientid), context);
	ಆದರೆ(context == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_ಸಿಕ್ಕಿತು;
	}

	ಆದರೆ(client_ಸಂದೇಶ->direction == mosq_md_ಹೊರಹೋಗು){
		db_ಸಂದೇಶ_update_outgoing(context, client_ಸಂದೇಶ->mid, client_ಸಂದೇಶ->state, client_ಸಂದೇಶ->qos, false);
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(client_ಸಂದೇಶ->direction == mosq_md_ಒಳ){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_persist_client_ಸಂದೇಶ_clear(ರಚನೆ mosquitto_client_ಸಂದೇಶ_2 *client_ಸಂದೇಶ)
{
	ರಚನೆ mosquitto *context;

	ಆದರೆ(client_ಸಂದೇಶ == ಖಾಲಿ || client_ಸಂದೇಶ->clientid == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id, client_ಸಂದೇಶ->clientid, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(client_ಸಂದೇಶ->clientid), context);
	ಆದರೆ(context == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_ಸಿಕ್ಕಿತು;
	}

	ಆದರೆ(client_ಸಂದೇಶ->direction == mosq_bmd_ಒಳ || client_ಸಂದೇಶ->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	ಆದರೆ(client_ಸಂದೇಶ->direction == mosq_bmd_ಹೊರಹೋಗು || client_ಸಂದೇಶ->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_subscription_ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_subscription *sub)
{
	ರಚನೆ mosquitto *context;

	ಆದರೆ(sub == ಖಾಲಿ || sub->clientid == ಖಾಲಿ || sub->topic_filter == ಖಾಲಿ || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id, sub->clientid, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sub->clientid), context);

	ಆದರೆ(context){
		ಹಿಂತಿರುಗಿ sub_ಕೂಡಿಸು(context, sub);
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_ಸಿಕ್ಕಿತು;
	}
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_subscription_delete(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clientid, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic)
{
	ರಚನೆ mosquitto *context;
	uint8_t reason;

	ಆದರೆ(clientid == ಖಾಲಿ || topic == ಖಾಲಿ || clientid[0] == '\0' || topic[0] == '\0'){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id, clientid, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(clientid), context);

	ಆದರೆ(context){
		ಹಿಂತಿರುಗಿ sub__remove(context, topic, &reason);
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_ಸಿಕ್ಕಿತು;
	}
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_persist_ತಳ_ಸಂದೇಶ_ಕೂಡಿಸು(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ_2 *ಸಂದೇಶ_ಕೂಡಿಸು)
{
	ರಚನೆ mosquitto context;
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ;
	ಪೂರ್ಣಾಂಕ rc;

	memset(&context, 0, ಗಾತ್ರ(context));

	ಆದರೆ(ಸಂದೇಶ_ಕೂಡಿಸು->payloadlen > MQTT_MAX_PAYLOAD){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (ಅಕ್ಷರ *)ಸಂದೇಶ_ಕೂಡಿಸು->ಮೂಲ_id;
	context.username = (ಅಕ್ಷರ *)ಸಂದೇಶ_ಕೂಡಿಸು->ಮೂಲ_username;

	ತಳ_ಸಂದೇಶ = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ));
	ಆದರೆ(ತಳ_ಸಂದೇಶ == ಖಾಲಿ){
		ಹೋಗು error;
	}
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.store_id = ಸಂದೇಶ_ಕೂಡಿಸು->store_id;
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.expiry_ಸಮಯ = ಸಂದೇಶ_ಕೂಡಿಸು->expiry_ಸಮಯ;
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen = ಸಂದೇಶ_ಕೂಡಿಸು->payloadlen;
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid = ಸಂದೇಶ_ಕೂಡಿಸು->ಮೂಲ_mid;
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos = ಸಂದೇಶ_ಕೂಡಿಸು->qos;
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain = ಸಂದೇಶ_ಕೂಡಿಸು->retain;

	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload = ಸಂದೇಶ_ಕೂಡಿಸು->payload;
	ಸಂದೇಶ_ಕೂಡಿಸು->payload = ಖಾಲಿ;
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic = ಸಂದೇಶ_ಕೂಡಿಸು->topic;
	ಸಂದೇಶ_ಕೂಡಿಸು->topic = ಖಾಲಿ;
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.properties = ಸಂದೇಶ_ಕೂಡಿಸು->properties;
	ಸಂದೇಶ_ಕೂಡಿಸು->properties = ಖಾಲಿ;

	ಆದರೆ(ಸಂದೇಶ_ಕೂಡಿಸು->ಮೂಲ_ಪೋರ್ಟ್){
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
			ಆದರೆ(db.config->listeners[i].ಪೋರ್ಟ್ == ಸಂದೇಶ_ಕೂಡಿಸು->ಮೂಲ_ಪೋರ್ಟ್){
				ತಳ_ಸಂದೇಶ->ಮೂಲ_listener = &db.config->listeners[i];
				ನಿಲ್ಲಿಸು;
			}
		}
	}

	ತಳ_ಸಂದೇಶ->stored = true;
	rc = db_ಸಂದೇಶ_store(&context, ತಳ_ಸಂದೇಶ, ಖಾಲಿ, mosq_mo_broker);
	ಹಿಂತಿರುಗಿ rc;

error:
	mosquitto_property_free_all(&ಸಂದೇಶ_ಕೂಡಿಸು->properties);
	mosquitto_free(ಸಂದೇಶ_ಕೂಡಿಸು->topic);
	mosquitto_free(ಸಂದೇಶ_ಕೂಡಿಸು->payload);
	mosquitto_free(ತಳ_ಸಂದೇಶ);

	ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_persist_ತಳ_ಸಂದೇಶ_delete(uint64_t store_id)
{
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ;

	ತಳ_ಸಂದೇಶ = ಕಂಡುಹಿಡಿ_store_ಸಂದೇಶ(store_id);
	ಆದರೆ(ತಳ_ಸಂದೇಶ && ತಳ_ಸಂದೇಶ->ref_ಎಣಿಕೆ == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_ಸಂದೇಶ_store_remove(ತಳ_ಸಂದೇಶ, false);
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಶೂನ್ಯ mosquitto_complete_basic_auth(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clientid, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ)
{
	ರಚನೆ mosquitto *context;

	ಆದರೆ(clientid == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ;
	}

	HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id_delayed_auth, clientid, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(clientid), context);
	ಆದರೆ(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		ಆದರೆ(ಫಲಿತಾಂಶ == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, ಖಾಲಿ, 0);
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಆದರೆ(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ಖಾಲಿ);
			}ಇಲ್ಲದಿದ್ದರೆ{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ಖಾಲಿ);
			}
			context->clean_ಆರಂಭ = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_ತಕ್ಷಣ;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_broker_ಗಂಟು_id_ಗಣ(uint16_t id)
{
	ಆದರೆ(id > 1023){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}ಇಲ್ಲದಿದ್ದರೆ{
		db.ಗಂಟು_id = id;
		db.ಗಂಟು_id_shifted = ((uint64_t)id) << 54;
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mosquitto_persistence_location(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ db.config->persistence_location;
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_client_will_ಗಣ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clientid, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, ಪೂರ್ಣಾಂಕ payloadlen, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *payload, ಪೂರ್ಣಾಂಕ qos, bool retain, mosquitto_property *properties)
{
	ರಚನೆ mosquitto *mosq = mosquitto_client(clientid);
	ಆದರೆ(!mosq){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_ಸಿಕ್ಕಿತು;
	}
	ಆದರೆ(properties && mosq->protocol != mosq_p_mqtt5){
		ಆದರೆ(net__is_connected(mosq)){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	ಹಿಂತಿರುಗಿ will_ಗಣ(mosq, topic, payloadlen, payload, qos, retain, properties);
}
