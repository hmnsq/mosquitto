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

#अन्तर्भाव "config.h"

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "send_mosq.h"
#अन्तर्भाव "util_mosq.h"
#अन्तर्भाव "will_mosq.h"
#अन्तर्भाव "utlist.h"
#अन्तर्भाव "will_mosq.h"

#ifdef WITH_TLS
#  include <openssl/ssl.h>
#endif


BROKER_EXPORT पूर्णाङ्क mosquitto_plugin_set_info(mosquitto_plugin_id_t *identifier,
		अचर वर्ण *plugin_name,
		अचर वर्ण *plugin_version)
{
	यदि(identifier == रिक्त || plugin_name == रिक्त){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	यदि(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}अन्यथा{
		identifier->plugin_version = रिक्त;
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT अचर वर्ण *mosquitto_client_address(अचर संरचना mosquitto *client)
{
	यदि(client){
		फर्काऊ client->address;
	}अन्यथा{
		फर्काऊ रिक्त;
	}
}


BROKER_EXPORT संरचना mosquitto *mosquitto_client(अचर वर्ण *clientid)
{
	माप_प्रकार len;
	संरचना mosquitto *context;

	यदि(!clientid){
		फर्काऊ रिक्त;
	}
	len = म_माप(clientid);
	यदि(len == 0){
		फर्काऊ रिक्त;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, म_माप(clientid), context);

	फर्काऊ context;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_port(अचर संरचना mosquitto *client)
{
	यदि(client && client->listener){
		फर्काऊ client->listener->port;
	}अन्यथा{
		फर्काऊ 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(अचर संरचना mosquitto *client)
{
	यदि(client){
		फर्काऊ client->clean_start;
	}अन्यथा{
		फर्काऊ true;
	}
}


BROKER_EXPORT अचर वर्ण *mosquitto_client_id(अचर संरचना mosquitto *client)
{
	यदि(client){
		फर्काऊ client->id;
	}अन्यथा{
		फर्काऊ रिक्त;
	}
}


BROKER_EXPORT अचिन्हित mosquitto_client_id_hashv(अचर संरचना mosquitto *client)
{
	यदि(client){
		फर्काऊ client->id_hashv;
	}अन्यथा{
		फर्काऊ 0;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_keepalive(अचर संरचना mosquitto *client)
{
	यदि(client){
		फर्काऊ client->keepalive;
	}अन्यथा{
		फर्काऊ -1;
	}
}


BROKER_EXPORT शून्य *mosquitto_client_certificate(अचर संरचना mosquitto *client)
{
#ifdef WITH_TLS
	यदि(client && client->ssl){
		फर्काऊ SSL_get_peer_certificate(client->ssl);
	}अन्यथा{
		फर्काऊ रिक्त;
	}
#अन्यथा
	UNUSED(client);

	फर्काऊ रिक्त;
#endif
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_protocol(अचर संरचना mosquitto *client)
{
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	यदि(client && client->wsi){
		फर्काऊ mp_websockets;
	}अन्यथा
#अथवायदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	यदि(client && client->transport == mosq_t_ws){
		फर्काऊ mp_websockets;
	}अन्यथा
#अन्यथा
	UNUSED(client);
#endif
	{
		फर्काऊ mp_mqtt;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_protocol_version(अचर संरचना mosquitto *client)
{
	यदि(client){
		विकल्प(client->protocol){
			पक्ष mosq_p_mqtt31:
				फर्काऊ 3;
			पक्ष mosq_p_mqtt311:
				फर्काऊ 4;
			पक्ष mosq_p_mqtt5:
				फर्काऊ 5;
			स्वभाव:
				फर्काऊ 0;
		}
	}अन्यथा{
		फर्काऊ 0;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_sub_गन्ती(अचर संरचना mosquitto *client)
{
	यदि(client){
		फर्काऊ client->subs_गन्ती;
	}अन्यथा{
		फर्काऊ 0;
	}
}


BROKER_EXPORT अचर वर्ण *mosquitto_client_username(अचर संरचना mosquitto *client)
{
	यदि(client){
#ifdef WITH_BRIDGE
		यदि(client->bridge){
			फर्काऊ client->bridge->local_username;
		}अन्यथा
#endif
		{
			फर्काऊ client->username;
		}
	}अन्यथा{
		फर्काऊ रिक्त;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_broker_publish(
		अचर वर्ण *clientid,
		अचर वर्ण *topic,
		पूर्णाङ्क payloadlen,
		शून्य *payload,
		पूर्णाङ्क qos,
		bool retain,
		mosquitto_property *properties)
{
	संरचना mosquitto__message_v5 *सन्देश;

	यदि(topic == रिक्त
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == रिक्त)
			|| qos < 0 || qos > 2){

		फर्काऊ MOSQ_ERR_INVAL;
	}

	सन्देश = mosquitto_malloc(परिमाण(संरचना mosquitto__message_v5));
	यदि(सन्देश == रिक्त){
		फर्काऊ MOSQ_ERR_NOMEM;
	}

	सन्देश->next = रिक्त;
	सन्देश->prev = रिक्त;
	यदि(clientid){
		सन्देश->clientid = mosquitto_strdup(clientid);
		यदि(सन्देश->clientid == रिक्त){
			mosquitto_FREE(सन्देश);
			फर्काऊ MOSQ_ERR_NOMEM;
		}
	}अन्यथा{
		सन्देश->clientid = रिक्त;
	}
	सन्देश->topic = mosquitto_strdup(topic);
	यदि(सन्देश->topic == रिक्त){
		mosquitto_FREE(सन्देश->clientid);
		mosquitto_FREE(सन्देश);
		फर्काऊ MOSQ_ERR_NOMEM;
	}
	सन्देश->payloadlen = payloadlen;
	सन्देश->payload = payload;
	सन्देश->qos = qos;
	सन्देश->retain = retain;
	सन्देश->properties = properties;

	DL_APPEND(db.plugin_msgs, सन्देश);

	loop__update_next_event(1);
	फर्काऊ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_broker_publish_copy(
		अचर वर्ण *clientid,
		अचर वर्ण *topic,
		पूर्णाङ्क payloadlen,
		अचर शून्य *payload,
		पूर्णाङ्क qos,
		bool retain,
		mosquitto_property *properties)
{
	शून्य *payload_out;
	पूर्णाङ्क rc;

	यदि(topic == रिक्त
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == रिक्त)
			|| qos < 0 || qos > 2){

		फर्काऊ MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (माप_प्रकार)(payloadlen+1));
	यदि(payload_out == रिक्त){
		फर्काऊ MOSQ_ERR_NOMEM;
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

	यदि(rc){
		mosquitto_FREE(payload_out);
	}
	फर्काऊ rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_set_username(संरचना mosquitto *client, अचर वर्ण *username)
{
	वर्ण *u_dup;
	वर्ण *old;

	यदि(!client){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	यदि(username){
		यदि(mosquitto_validate_utf8(username, (पूर्णाङ्क)म_माप(username))){
			फर्काऊ MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		यदि(!u_dup){
			फर्काऊ MOSQ_ERR_NOMEM;
		}
	}अन्यथा{
		u_dup = रिक्त;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	फर्काऊ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_set_clientid(संरचना mosquitto *client, अचर वर्ण *clientid)
{
	संरचना mosquitto *found_client;
	वर्ण *id_dup;
	bool in_by_id;
	पूर्णाङ्क clientid_len;

	यदि(!client || !clientid){
		फर्काऊ MOSQ_ERR_INVAL;
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

	यदि(in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, clientid, म_माप(clientid), found_client);
		यदि(found_client){
			फर्काऊ MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (पूर्णाङ्क)म_माप(clientid);
	यदि(mosquitto_validate_utf8(clientid, clientid_len)){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	यदि(!id_dup){
		फर्काऊ MOSQ_ERR_NOMEM;
	}

	यदि(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	यदि(in_by_id){
		context__add_to_by_id(client);
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
स्थिर शून्य check_subscription_acls(संरचना mosquitto *context)
{
	पूर्णाङ्क rc;
	uint8_t reason;

	लागि(पूर्णाङ्क i=0; i<context->subs_capacity; i++){
		यदि(context->subs[i] == रिक्त){
			जारी;
		}
		rc = mosquitto_acl_check(context,
				context->subs[i]->topic_filter,
				0,
				रिक्त,
				0, /* FIXME */
				false,
				रिक्त,
				MOSQ_ACL_SUBSCRIBE);

		यदि(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


स्थिर शून्य disconnect_client(संरचना mosquitto *context, bool with_will)
{
	यदि(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, रिक्त);
	}
	यदि(with_will == false){
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	यदि(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT पूर्णाङ्क mosquitto_kick_client_by_clientid(अचर वर्ण *clientid, bool with_will)
{
	संरचना mosquitto *ctxt, *ctxt_tmp;

	यदि(clientid == रिक्त){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		फर्काऊ MOSQ_ERR_SUCCESS;
	}अन्यथा{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, म_माप(clientid), ctxt);
		यदि(ctxt){
			disconnect_client(ctxt, with_will);
			फर्काऊ MOSQ_ERR_SUCCESS;
		}अन्यथा{
			फर्काऊ MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_kick_client_by_username(अचर वर्ण *username, bool with_will)
{
	संरचना mosquitto *ctxt, *ctxt_tmp;

	यदि(username == रिक्त){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			यदि(ctxt->username == रिक्त){
				disconnect_client(ctxt, with_will);
			}
		}
	}अन्यथा{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			यदि(ctxt->username != रिक्त && !म_भेद(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	फर्काऊ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_apply_on_all_clients(पूर्णाङ्क (*FUNC_client_functor)(अचर संरचना mosquitto *, शून्य *), शून्य *functor_context)
{
	पूर्णाङ्क rc = MOSQ_ERR_SUCCESS;
	संरचना mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		यदि(rc != MOSQ_ERR_SUCCESS){
			रोक;
		}
	}

	फर्काऊ rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_add(संरचना mosquitto_client *client)
{
	संरचना mosquitto *context;
	पूर्णाङ्क rc;

	यदि(client == रिक्त){
		फर्काऊ MOSQ_ERR_INVAL;
	}
	यदि(client->clientid == रिक्त){
		rc = MOSQ_ERR_INVAL;
		गच्छ error;
	}

	context = रिक्त;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, म_माप(client->clientid), context);
	यदि(context){
		rc = MOSQ_ERR_INVAL;
		गच्छ error;
	}

	context = context__init();
	यदि(!context){
		rc = MOSQ_ERR_NOMEM;
		गच्छ error;
	}

	context->id = client->clientid;
	client->clientid = रिक्त;
	context->username = client->username;
	client->username = रिक्त;
	context->auth_method = client->auth_method;
	client->auth_method = रिक्त;

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
	यदि(db.config->per_listener_settings && client->listener_port > 0){
		लागि(पूर्णाङ्क i=0; i < db.config->listener_गन्ती; i++){
			यदि(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				रोक;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	फर्काऊ MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	फर्काऊ rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_update(संरचना mosquitto_client *client)
{
	संरचना mosquitto *context;
	पूर्णाङ्क rc;

	यदि(client == रिक्त){
		फर्काऊ MOSQ_ERR_INVAL;
	}
	यदि(client->clientid == रिक्त){
		rc = MOSQ_ERR_INVAL;
		गच्छ error;
	}

	context = रिक्त;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, म_माप(client->clientid), context);
	यदि(context == रिक्त){
		rc = MOSQ_ERR_NOT_FOUND;
		गच्छ error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = रिक्त;

	context->clean_start = false;
	context->will_delay_time = client->will_delay_time;
	context->session_expiry_time = client->session_expiry_time;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->maximum_packet_size = client->max_packet_size;
	context->retain_available = client->retain_available;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	यदि(db.config->per_listener_settings && client->listener_port > 0){
		लागि(पूर्णाङ्क i=0; i < db.config->listener_गन्ती; i++){
			यदि(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				रोक;
			}
		}
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	फर्काऊ rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_delete(अचर वर्ण *clientid)
{
	संरचना mosquitto *context;

	यदि(clientid == रिक्त){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	context = रिक्त;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, म_माप(clientid), context);
	यदि(context == रिक्त){
		फर्काऊ MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर संरचना mosquitto_base_सन्देश *find_store_सन्देश(uint64_t store_id)
{
	संरचना mosquitto_base_सन्देश *base_सन्देश;

	HASH_FIND(hh, db.सन्देश_store, &store_id, परिमाण(store_id), base_सन्देश);
	फर्काऊ base_सन्देश;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_सन्देश_add(संरचना mosquitto_client_सन्देश_2 *client_सन्देश)
{
	संरचना mosquitto *context;
	संरचना mosquitto_base_सन्देश *base_सन्देश;

	यदि(client_सन्देश == रिक्त || client_सन्देश->clientid == रिक्त){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_सन्देश->clientid, म_माप(client_सन्देश->clientid), context);
	यदि(context == रिक्त){
		फर्काऊ MOSQ_ERR_NOT_FOUND;
	}
	base_सन्देश = find_store_सन्देश(client_सन्देश->store_id);
	यदि(base_सन्देश == रिक्त){
		फर्काऊ MOSQ_ERR_NOT_FOUND;
	}

	यदि(client_सन्देश->direction == mosq_md_out){
		यदि(client_सन्देश->qos > 0){
			context->last_mid = client_सन्देश->mid;
		}
		फर्काऊ db_message_घुसाउ_outgoing(context, client_सन्देश->cmsg_id, client_सन्देश->mid,
				client_सन्देश->qos, client_सन्देश->retain,
				base_सन्देश, client_सन्देश->subscription_identifier, false, false);
	}अन्यथा यदि(client_सन्देश->direction == mosq_md_in){
		फर्काऊ db_message_घुसाउ_incoming(context, client_सन्देश->cmsg_id, base_सन्देश, false);
	}अन्यथा{
		फर्काऊ MOSQ_ERR_INVAL;
	}
	फर्काऊ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_सन्देश_delete(संरचना mosquitto_client_सन्देश_2 *client_सन्देश)
{
	संरचना mosquitto *context;

	यदि(client_सन्देश == रिक्त || client_सन्देश->clientid == रिक्त){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_सन्देश->clientid, म_माप(client_सन्देश->clientid), context);
	यदि(context == रिक्त){
		फर्काऊ MOSQ_ERR_NOT_FOUND;
	}


	पूर्णाङ्क rc = MOSQ_ERR_INVAL;
	यदि(client_सन्देश->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_सन्देश->mid, mosq_ms_any, client_सन्देश->qos);
	}अन्यथा यदि(client_सन्देश->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_सन्देश->mid);
	}
	फर्काऊ rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_सन्देश_update(संरचना mosquitto_client_सन्देश_2 *client_सन्देश)
{
	संरचना mosquitto *context;

	यदि(client_सन्देश == रिक्त || client_सन्देश->clientid == रिक्त){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_सन्देश->clientid, म_माप(client_सन्देश->clientid), context);
	यदि(context == रिक्त){
		फर्काऊ MOSQ_ERR_NOT_FOUND;
	}

	यदि(client_सन्देश->direction == mosq_md_out){
		db__message_update_outgoing(context, client_सन्देश->mid, client_सन्देश->state, client_सन्देश->qos, false);
	}अन्यथा यदि(client_सन्देश->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}अन्यथा{
		फर्काऊ MOSQ_ERR_INVAL;
	}
	फर्काऊ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_सन्देश_clear(संरचना mosquitto_client_सन्देश_2 *client_सन्देश)
{
	संरचना mosquitto *context;

	यदि(client_सन्देश == रिक्त || client_सन्देश->clientid == रिक्त){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_सन्देश->clientid, म_माप(client_सन्देश->clientid), context);
	यदि(context == रिक्त){
		फर्काऊ MOSQ_ERR_NOT_FOUND;
	}

	यदि(client_सन्देश->direction == mosq_bmd_in || client_सन्देश->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	यदि(client_सन्देश->direction == mosq_bmd_out || client_सन्देश->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	फर्काऊ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_subscription_add(अचर संरचना mosquitto_subscription *sub)
{
	संरचना mosquitto *context;

	यदि(sub == रिक्त || sub->clientid == रिक्त || sub->topic_filter == रिक्त || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, म_माप(sub->clientid), context);

	यदि(context){
		फर्काऊ sub__add(context, sub);
	}अन्यथा{
		फर्काऊ MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_subscription_delete(अचर वर्ण *clientid, अचर वर्ण *topic)
{
	संरचना mosquitto *context;
	uint8_t reason;

	यदि(clientid == रिक्त || topic == रिक्त || clientid[0] == '\0' || topic[0] == '\0'){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, म_माप(clientid), context);

	यदि(context){
		फर्काऊ sub__remove(context, topic, &reason);
	}अन्यथा{
		फर्काऊ MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_base_सन्देश_add(संरचना mosquitto_base_सन्देश_2 *सन्देश_add)
{
	संरचना mosquitto context;
	संरचना mosquitto_base_सन्देश *base_सन्देश;
	पूर्णाङ्क rc;

	स_रखो(&context, 0, परिमाण(context));

	यदि(सन्देश_add->payloadlen > MQTT_MAX_PAYLOAD){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (वर्ण *)सन्देश_add->source_id;
	context.username = (वर्ण *)सन्देश_add->source_username;

	base_सन्देश = mosquitto_calloc(1, परिमाण(संरचना mosquitto_base_सन्देश));
	यदि(base_सन्देश == रिक्त){
		गच्छ error;
	}
	base_सन्देश->data.store_id = सन्देश_add->store_id;
	base_सन्देश->data.expiry_time = सन्देश_add->expiry_time;
	base_सन्देश->data.payloadlen = सन्देश_add->payloadlen;
	base_सन्देश->data.source_mid = सन्देश_add->source_mid;
	base_सन्देश->data.qos = सन्देश_add->qos;
	base_सन्देश->data.retain = सन्देश_add->retain;

	base_सन्देश->data.payload = सन्देश_add->payload;
	सन्देश_add->payload = रिक्त;
	base_सन्देश->data.topic = सन्देश_add->topic;
	सन्देश_add->topic = रिक्त;
	base_सन्देश->data.properties = सन्देश_add->properties;
	सन्देश_add->properties = रिक्त;

	यदि(सन्देश_add->source_port){
		लागि(पूर्णाङ्क i=0; i<db.config->listener_गन्ती; i++){
			यदि(db.config->listeners[i].port == सन्देश_add->source_port){
				base_सन्देश->source_listener = &db.config->listeners[i];
				रोक;
			}
		}
	}

	base_सन्देश->stored = true;
	rc = db__message_store(&context, base_सन्देश, रिक्त, mosq_mo_broker);
	फर्काऊ rc;

error:
	mosquitto_property_free_all(&सन्देश_add->properties);
	mosquitto_free(सन्देश_add->topic);
	mosquitto_free(सन्देश_add->payload);
	mosquitto_free(base_सन्देश);

	फर्काऊ MOSQ_ERR_NOMEM;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_base_सन्देश_delete(uint64_t store_id)
{
	संरचना mosquitto_base_सन्देश *base_सन्देश;

	base_सन्देश = find_store_सन्देश(store_id);
	यदि(base_सन्देश && base_सन्देश->ref_गन्ती == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_सन्देश_store_remove(base_सन्देश, false);
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT शून्य mosquitto_complete_basic_auth(अचर वर्ण *clientid, पूर्णाङ्क result)
{
	संरचना mosquitto *context;

	यदि(clientid == रिक्त){
		फर्काऊ;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, म_माप(clientid), context);
	यदि(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		यदि(result == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, रिक्त, 0);
		}अन्यथा{
			यदि(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, रिक्त);
			}अन्यथा{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, रिक्त);
			}
			context->clean_start = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_broker_गाँठो_id_set(uint16_t id)
{
	यदि(id > 1023){
		फर्काऊ MOSQ_ERR_INVAL;
	}अन्यथा{
		db.गाँठो_id = id;
		db.गाँठो_id_shifted = ((uint64_t)id) << 54;
		फर्काऊ MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT अचर वर्ण *mosquitto_persistence_location(शून्य)
{
	फर्काऊ db.config->persistence_location;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_will_set(अचर वर्ण *clientid, अचर वर्ण *topic, पूर्णाङ्क payloadlen, अचर शून्य *payload, पूर्णाङ्क qos, bool retain, mosquitto_property *properties)
{
	संरचना mosquitto *mosq = mosquitto_client(clientid);
	यदि(!mosq){
		फर्काऊ MOSQ_ERR_NOT_FOUND;
	}
	यदि(properties && mosq->protocol != mosq_p_mqtt5){
		यदि(net__is_connected(mosq)){
			फर्काऊ MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	फर्काऊ will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
