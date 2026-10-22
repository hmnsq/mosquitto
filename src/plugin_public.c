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

#समावेश "config.h"

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "send_mosq.h"
#समावेश "util_mosq.h"
#समावेश "will_mosq.h"
#समावेश "utlist.h"
#समावेश "will_mosq.h"

#अगर_घोषित WITH_TLS
#  include <openssl/ssl.h>
#पूर्ण_अगर


BROKER_EXPORT पूर्णांक mosquitto_plugin_set_info(mosquitto_plugin_id_t *identifier,
		स्थिर अक्षर *plugin_name,
		स्थिर अक्षर *plugin_version)
{
	अगर(identifier == शून्य || plugin_name == शून्य){
		वापस MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	अगर(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}अन्यथा{
		identifier->plugin_version = शून्य;
	}

	वापस MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT स्थिर अक्षर *mosquitto_client_address(स्थिर काष्ठा mosquitto *client)
{
	अगर(client){
		वापस client->address;
	}अन्यथा{
		वापस शून्य;
	}
}


BROKER_EXPORT काष्ठा mosquitto *mosquitto_client(स्थिर अक्षर *clientid)
{
	माप_प्रकार len;
	काष्ठा mosquitto *context;

	अगर(!clientid){
		वापस शून्य;
	}
	len = म_माप(clientid);
	अगर(len == 0){
		वापस शून्य;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, म_माप(clientid), context);

	वापस context;
}


BROKER_EXPORT पूर्णांक mosquitto_client_port(स्थिर काष्ठा mosquitto *client)
{
	अगर(client && client->listener){
		वापस client->listener->port;
	}अन्यथा{
		वापस 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(स्थिर काष्ठा mosquitto *client)
{
	अगर(client){
		वापस client->clean_start;
	}अन्यथा{
		वापस true;
	}
}


BROKER_EXPORT स्थिर अक्षर *mosquitto_client_id(स्थिर काष्ठा mosquitto *client)
{
	अगर(client){
		वापस client->id;
	}अन्यथा{
		वापस शून्य;
	}
}


BROKER_EXPORT अचिन्हित mosquitto_client_id_hashv(स्थिर काष्ठा mosquitto *client)
{
	अगर(client){
		वापस client->id_hashv;
	}अन्यथा{
		वापस 0;
	}
}


BROKER_EXPORT पूर्णांक mosquitto_client_keepalive(स्थिर काष्ठा mosquitto *client)
{
	अगर(client){
		वापस client->keepalive;
	}अन्यथा{
		वापस -1;
	}
}


BROKER_EXPORT व्योम *mosquitto_client_certificate(स्थिर काष्ठा mosquitto *client)
{
#अगर_घोषित WITH_TLS
	अगर(client && client->ssl){
		वापस SSL_get_peer_certificate(client->ssl);
	}अन्यथा{
		वापस शून्य;
	}
#अन्यथा
	UNUSED(client);

	वापस शून्य;
#पूर्ण_अगर
}


BROKER_EXPORT पूर्णांक mosquitto_client_protocol(स्थिर काष्ठा mosquitto *client)
{
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	अगर(client && client->wsi){
		वापस mp_websockets;
	}अन्यथा
#या_अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	अगर(client && client->transport == mosq_t_ws){
		वापस mp_websockets;
	}अन्यथा
#अन्यथा
	UNUSED(client);
#पूर्ण_अगर
	{
		वापस mp_mqtt;
	}
}


BROKER_EXPORT पूर्णांक mosquitto_client_protocol_version(स्थिर काष्ठा mosquitto *client)
{
	अगर(client){
		चयन(client->protocol){
			हाल mosq_p_mqtt31:
				वापस 3;
			हाल mosq_p_mqtt311:
				वापस 4;
			हाल mosq_p_mqtt5:
				वापस 5;
			शेष:
				वापस 0;
		}
	}अन्यथा{
		वापस 0;
	}
}


BROKER_EXPORT पूर्णांक mosquitto_client_sub_गिनती(स्थिर काष्ठा mosquitto *client)
{
	अगर(client){
		वापस client->subs_गिनती;
	}अन्यथा{
		वापस 0;
	}
}


BROKER_EXPORT स्थिर अक्षर *mosquitto_client_username(स्थिर काष्ठा mosquitto *client)
{
	अगर(client){
#अगर_घोषित WITH_BRIDGE
		अगर(client->bridge){
			वापस client->bridge->local_username;
		}अन्यथा
#पूर्ण_अगर
		{
			वापस client->username;
		}
	}अन्यथा{
		वापस शून्य;
	}
}


BROKER_EXPORT पूर्णांक mosquitto_broker_publish(
		स्थिर अक्षर *clientid,
		स्थिर अक्षर *topic,
		पूर्णांक payloadlen,
		व्योम *payload,
		पूर्णांक qos,
		bool retain,
		mosquitto_property *properties)
{
	काष्ठा mosquitto__message_v5 *संदेश;

	अगर(topic == शून्य
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == शून्य)
			|| qos < 0 || qos > 2){

		वापस MOSQ_ERR_INVAL;
	}

	संदेश = mosquitto_malloc(माप(काष्ठा mosquitto__message_v5));
	अगर(संदेश == शून्य){
		वापस MOSQ_ERR_NOMEM;
	}

	संदेश->अग्र = शून्य;
	संदेश->पूर्व = शून्य;
	अगर(clientid){
		संदेश->clientid = mosquitto_strdup(clientid);
		अगर(संदेश->clientid == शून्य){
			mosquitto_FREE(संदेश);
			वापस MOSQ_ERR_NOMEM;
		}
	}अन्यथा{
		संदेश->clientid = शून्य;
	}
	संदेश->topic = mosquitto_strdup(topic);
	अगर(संदेश->topic == शून्य){
		mosquitto_FREE(संदेश->clientid);
		mosquitto_FREE(संदेश);
		वापस MOSQ_ERR_NOMEM;
	}
	संदेश->payloadlen = payloadlen;
	संदेश->payload = payload;
	संदेश->qos = qos;
	संदेश->retain = retain;
	संदेश->properties = properties;

	DL_APPEND(db.plugin_msgs, संदेश);

	loop_update_अग्र_event(1);
	वापस MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णांक mosquitto_broker_publish_copy(
		स्थिर अक्षर *clientid,
		स्थिर अक्षर *topic,
		पूर्णांक payloadlen,
		स्थिर व्योम *payload,
		पूर्णांक qos,
		bool retain,
		mosquitto_property *properties)
{
	व्योम *payload_out;
	पूर्णांक rc;

	अगर(topic == शून्य
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == शून्य)
			|| qos < 0 || qos > 2){

		वापस MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (माप_प्रकार)(payloadlen+1));
	अगर(payload_out == शून्य){
		वापस MOSQ_ERR_NOMEM;
	}
	स_नकल(payload_out, payload, (माप_प्रकार)payloadlen);

	rc = mosquitto_broker_publish(
			clientid,
			topic,
			payloadlen,
			payload_out,
			qos,
			retain,
			properties);

	अगर(rc){
		mosquitto_FREE(payload_out);
	}
	वापस rc;
}


BROKER_EXPORT पूर्णांक mosquitto_set_username(काष्ठा mosquitto *client, स्थिर अक्षर *username)
{
	अक्षर *u_dup;
	अक्षर *old;

	अगर(!client){
		वापस MOSQ_ERR_INVAL;
	}

	अगर(username){
		अगर(mosquitto_validate_utf8(username, (पूर्णांक)म_माप(username))){
			वापस MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		अगर(!u_dup){
			वापस MOSQ_ERR_NOMEM;
		}
	}अन्यथा{
		u_dup = शून्य;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	वापस MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णांक mosquitto_set_clientid(काष्ठा mosquitto *client, स्थिर अक्षर *clientid)
{
	काष्ठा mosquitto *found_client;
	अक्षर *id_dup;
	bool in_by_id;
	पूर्णांक clientid_len;

	अगर(!client || !clientid){
		वापस MOSQ_ERR_INVAL;
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

	अगर(in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, clientid, म_माप(clientid), found_client);
		अगर(found_client){
			वापस MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (पूर्णांक)म_माप(clientid);
	अगर(mosquitto_validate_utf8(clientid, clientid_len)){
		वापस MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	अगर(!id_dup){
		वापस MOSQ_ERR_NOMEM;
	}

	अगर(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	अगर(in_by_id){
		context__add_to_by_id(client);
	}

	वापस MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
अटल व्योम check_subscription_acls(काष्ठा mosquitto *context)
{
	पूर्णांक rc;
	uint8_t reason;

	क्रम(पूर्णांक i=0; i<context->subs_capacity; i++){
		अगर(context->subs[i] == शून्य){
			जारी;
		}
		rc = mosquitto_acl_check(context,
				context->subs[i]->topic_filter,
				0,
				शून्य,
				0, /* FIXME */
				false,
				शून्य,
				MOSQ_ACL_SUBSCRIBE);

		अगर(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


अटल व्योम disconnect_client(काष्ठा mosquitto *context, bool with_will)
{
	अगर(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, शून्य);
	}
	अगर(with_will == false){
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	अगर(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT पूर्णांक mosquitto_kick_client_by_clientid(स्थिर अक्षर *clientid, bool with_will)
{
	काष्ठा mosquitto *ctxt, *ctxt_tmp;

	अगर(clientid == शून्य){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		वापस MOSQ_ERR_SUCCESS;
	}अन्यथा{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, म_माप(clientid), ctxt);
		अगर(ctxt){
			disconnect_client(ctxt, with_will);
			वापस MOSQ_ERR_SUCCESS;
		}अन्यथा{
			वापस MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT पूर्णांक mosquitto_kick_client_by_username(स्थिर अक्षर *username, bool with_will)
{
	काष्ठा mosquitto *ctxt, *ctxt_tmp;

	अगर(username == शून्य){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			अगर(ctxt->username == शून्य){
				disconnect_client(ctxt, with_will);
			}
		}
	}अन्यथा{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			अगर(ctxt->username != शून्य && !म_भेद(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	वापस MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णांक mosquitto_apply_on_all_clients(पूर्णांक (*FUNC_client_functor)(स्थिर काष्ठा mosquitto *, व्योम *), व्योम *functor_context)
{
	पूर्णांक rc = MOSQ_ERR_SUCCESS;
	काष्ठा mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		अगर(rc != MOSQ_ERR_SUCCESS){
			अवरोध;
		}
	}

	वापस rc;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_client_add(काष्ठा mosquitto_client *client)
{
	काष्ठा mosquitto *context;
	पूर्णांक rc;

	अगर(client == शून्य){
		वापस MOSQ_ERR_INVAL;
	}
	अगर(client->clientid == शून्य){
		rc = MOSQ_ERR_INVAL;
		जाओ error;
	}

	context = शून्य;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, म_माप(client->clientid), context);
	अगर(context){
		rc = MOSQ_ERR_INVAL;
		जाओ error;
	}

	context = context__init();
	अगर(!context){
		rc = MOSQ_ERR_NOMEM;
		जाओ error;
	}

	context->id = client->clientid;
	client->clientid = शून्य;
	context->username = client->username;
	client->username = शून्य;
	context->auth_method = client->auth_method;
	client->auth_method = शून्य;

	context->clean_start = false;
	context->will_delay_time = client->will_delay_time;
	context->session_expiry_time = client->session_expiry_time;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->maximum_packet_आकार = client->max_packet_आकार;
	context->retain_available = client->retain_available;
	context->is_persisted = true;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	अगर(db.config->per_listener_settings && client->listener_port > 0){
		क्रम(पूर्णांक i=0; i < db.config->listener_गिनती; i++){
			अगर(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				अवरोध;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	वापस MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	वापस rc;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_client_update(काष्ठा mosquitto_client *client)
{
	काष्ठा mosquitto *context;
	पूर्णांक rc;

	अगर(client == शून्य){
		वापस MOSQ_ERR_INVAL;
	}
	अगर(client->clientid == शून्य){
		rc = MOSQ_ERR_INVAL;
		जाओ error;
	}

	context = शून्य;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, म_माप(client->clientid), context);
	अगर(context == शून्य){
		rc = MOSQ_ERR_NOT_FOUND;
		जाओ error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = शून्य;

	context->clean_start = false;
	context->will_delay_time = client->will_delay_time;
	context->session_expiry_time = client->session_expiry_time;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->maximum_packet_आकार = client->max_packet_आकार;
	context->retain_available = client->retain_available;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	अगर(db.config->per_listener_settings && client->listener_port > 0){
		क्रम(पूर्णांक i=0; i < db.config->listener_गिनती; i++){
			अगर(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				अवरोध;
			}
		}
	}

	वापस MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	वापस rc;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_client_delete(स्थिर अक्षर *clientid)
{
	काष्ठा mosquitto *context;

	अगर(clientid == शून्य){
		वापस MOSQ_ERR_INVAL;
	}

	context = शून्य;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, म_माप(clientid), context);
	अगर(context == शून्य){
		वापस MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	वापस MOSQ_ERR_SUCCESS;
}


अटल काष्ठा mosquitto_base_संदेश *find_store_संदेश(uint64_t store_id)
{
	काष्ठा mosquitto_base_संदेश *base_संदेश;

	HASH_FIND(hh, db.संदेश_store, &store_id, माप(store_id), base_संदेश);
	वापस base_संदेश;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_client_संदेश_add(काष्ठा mosquitto_client_संदेश_2 *client_संदेश)
{
	काष्ठा mosquitto *context;
	काष्ठा mosquitto_base_संदेश *base_संदेश;

	अगर(client_संदेश == शून्य || client_संदेश->clientid == शून्य){
		वापस MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_संदेश->clientid, म_माप(client_संदेश->clientid), context);
	अगर(context == शून्य){
		वापस MOSQ_ERR_NOT_FOUND;
	}
	base_संदेश = find_store_संदेश(client_संदेश->store_id);
	अगर(base_संदेश == शून्य){
		वापस MOSQ_ERR_NOT_FOUND;
	}

	अगर(client_संदेश->direction == mosq_md_out){
		अगर(client_संदेश->qos > 0){
			context->last_mid = client_संदेश->mid;
		}
		वापस db_message_जोड़ो_outgoing(context, client_संदेश->cmsg_id, client_संदेश->mid,
				client_संदेश->qos, client_संदेश->retain,
				base_संदेश, client_संदेश->subscription_identifier, false, false);
	}अन्यथा अगर(client_संदेश->direction == mosq_md_in){
		वापस db_message_जोड़ो_incoming(context, client_संदेश->cmsg_id, base_संदेश, false);
	}अन्यथा{
		वापस MOSQ_ERR_INVAL;
	}
	वापस MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_client_संदेश_delete(काष्ठा mosquitto_client_संदेश_2 *client_संदेश)
{
	काष्ठा mosquitto *context;

	अगर(client_संदेश == शून्य || client_संदेश->clientid == शून्य){
		वापस MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_संदेश->clientid, म_माप(client_संदेश->clientid), context);
	अगर(context == शून्य){
		वापस MOSQ_ERR_NOT_FOUND;
	}


	पूर्णांक rc = MOSQ_ERR_INVAL;
	अगर(client_संदेश->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_संदेश->mid, mosq_ms_any, client_संदेश->qos);
	}अन्यथा अगर(client_संदेश->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_संदेश->mid);
	}
	वापस rc;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_client_संदेश_update(काष्ठा mosquitto_client_संदेश_2 *client_संदेश)
{
	काष्ठा mosquitto *context;

	अगर(client_संदेश == शून्य || client_संदेश->clientid == शून्य){
		वापस MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_संदेश->clientid, म_माप(client_संदेश->clientid), context);
	अगर(context == शून्य){
		वापस MOSQ_ERR_NOT_FOUND;
	}

	अगर(client_संदेश->direction == mosq_md_out){
		db__message_update_outgoing(context, client_संदेश->mid, client_संदेश->state, client_संदेश->qos, false);
	}अन्यथा अगर(client_संदेश->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}अन्यथा{
		वापस MOSQ_ERR_INVAL;
	}
	वापस MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_client_संदेश_clear(काष्ठा mosquitto_client_संदेश_2 *client_संदेश)
{
	काष्ठा mosquitto *context;

	अगर(client_संदेश == शून्य || client_संदेश->clientid == शून्य){
		वापस MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_संदेश->clientid, म_माप(client_संदेश->clientid), context);
	अगर(context == शून्य){
		वापस MOSQ_ERR_NOT_FOUND;
	}

	अगर(client_संदेश->direction == mosq_bmd_in || client_संदेश->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	अगर(client_संदेश->direction == mosq_bmd_out || client_संदेश->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	वापस MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णांक mosquitto_subscription_add(स्थिर काष्ठा mosquitto_subscription *sub)
{
	काष्ठा mosquitto *context;

	अगर(sub == शून्य || sub->clientid == शून्य || sub->topic_filter == शून्य || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		वापस MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, म_माप(sub->clientid), context);

	अगर(context){
		वापस sub__add(context, sub);
	}अन्यथा{
		वापस MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT पूर्णांक mosquitto_subscription_delete(स्थिर अक्षर *clientid, स्थिर अक्षर *topic)
{
	काष्ठा mosquitto *context;
	uint8_t reason;

	अगर(clientid == शून्य || topic == शून्य || clientid[0] == '\0' || topic[0] == '\0'){
		वापस MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, म_माप(clientid), context);

	अगर(context){
		वापस sub__remove(context, topic, &reason);
	}अन्यथा{
		वापस MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT पूर्णांक mosquitto_persist_base_संदेश_add(काष्ठा mosquitto_base_संदेश_2 *संदेश_add)
{
	काष्ठा mosquitto context;
	काष्ठा mosquitto_base_संदेश *base_संदेश;
	पूर्णांक rc;

	स_रखो(&context, 0, माप(context));

	अगर(संदेश_add->payloadlen > MQTT_MAX_PAYLOAD){
		वापस MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (अक्षर *)संदेश_add->source_id;
	context.username = (अक्षर *)संदेश_add->source_username;

	base_संदेश = mosquitto_calloc(1, माप(काष्ठा mosquitto_base_संदेश));
	अगर(base_संदेश == शून्य){
		जाओ error;
	}
	base_संदेश->तथ्य.store_id = संदेश_add->store_id;
	base_संदेश->तथ्य.expiry_time = संदेश_add->expiry_time;
	base_संदेश->तथ्य.payloadlen = संदेश_add->payloadlen;
	base_संदेश->तथ्य.source_mid = संदेश_add->source_mid;
	base_संदेश->तथ्य.qos = संदेश_add->qos;
	base_संदेश->तथ्य.retain = संदेश_add->retain;

	base_संदेश->तथ्य.payload = संदेश_add->payload;
	संदेश_add->payload = शून्य;
	base_संदेश->तथ्य.topic = संदेश_add->topic;
	संदेश_add->topic = शून्य;
	base_संदेश->तथ्य.properties = संदेश_add->properties;
	संदेश_add->properties = शून्य;

	अगर(संदेश_add->source_port){
		क्रम(पूर्णांक i=0; i<db.config->listener_गिनती; i++){
			अगर(db.config->listeners[i].port == संदेश_add->source_port){
				base_संदेश->source_listener = &db.config->listeners[i];
				अवरोध;
			}
		}
	}

	base_संदेश->stored = true;
	rc = db__message_store(&context, base_संदेश, शून्य, mosq_mo_broker);
	वापस rc;

error:
	mosquitto_property_free_all(&संदेश_add->properties);
	mosquitto_free(संदेश_add->topic);
	mosquitto_free(संदेश_add->payload);
	mosquitto_free(base_संदेश);

	वापस MOSQ_ERR_NOMEM;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_base_संदेश_delete(uint64_t store_id)
{
	काष्ठा mosquitto_base_संदेश *base_संदेश;

	base_संदेश = find_store_संदेश(store_id);
	अगर(base_संदेश && base_संदेश->ref_गिनती == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_संदेश_store_remove(base_संदेश, false);
	}

	वापस MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT व्योम mosquitto_complete_basic_auth(स्थिर अक्षर *clientid, पूर्णांक परिणाम)
{
	काष्ठा mosquitto *context;

	अगर(clientid == शून्य){
		वापस;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, म_माप(clientid), context);
	अगर(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		अगर(परिणाम == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, शून्य, 0);
		}अन्यथा{
			अगर(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, शून्य);
			}अन्यथा{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, शून्य);
			}
			context->clean_start = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT पूर्णांक mosquitto_broker_गाँठ_id_set(uint16_t id)
{
	अगर(id > 1023){
		वापस MOSQ_ERR_INVAL;
	}अन्यथा{
		db.गाँठ_id = id;
		db.गाँठ_id_shifted = ((uint64_t)id) << 54;
		वापस MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT स्थिर अक्षर *mosquitto_persistence_location(व्योम)
{
	वापस db.config->persistence_location;
}


BROKER_EXPORT पूर्णांक mosquitto_client_will_set(स्थिर अक्षर *clientid, स्थिर अक्षर *topic, पूर्णांक payloadlen, स्थिर व्योम *payload, पूर्णांक qos, bool retain, mosquitto_property *properties)
{
	काष्ठा mosquitto *mosq = mosquitto_client(clientid);
	अगर(!mosq){
		वापस MOSQ_ERR_NOT_FOUND;
	}
	अगर(properties && mosq->protocol != mosq_p_mqtt5){
		अगर(net__is_connected(mosq)){
			वापस MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	वापस will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
