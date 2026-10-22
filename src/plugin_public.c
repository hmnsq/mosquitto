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


BROKER_EXPORT पूर्णांक mosquitto_plugin_set_info(mosquitto_plugin_id_t *identifier,
		स्थिरांक अक्षर *plugin_name,
		स्थिरांक अक्षर *plugin_version)
{
	जर(identifier == रिक्त || plugin_name == रिक्त){
		परत MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	जर(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}अन्यथा{
		identifier->plugin_version = रिक्त;
	}

	परत MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT स्थिरांक अक्षर *mosquitto_client_address(स्थिरांक संरचना mosquitto *client)
{
	जर(client){
		परत client->address;
	}अन्यथा{
		परत रिक्त;
	}
}


BROKER_EXPORT संरचना mosquitto *mosquitto_client(स्थिरांक अक्षर *clientid)
{
	माप_प्रकार len;
	संरचना mosquitto *context;

	जर(!clientid){
		परत रिक्त;
	}
	len = म_माप(clientid);
	जर(len == 0){
		परत रिक्त;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, म_माप(clientid), context);

	परत context;
}


BROKER_EXPORT पूर्णांक mosquitto_client_port(स्थिरांक संरचना mosquitto *client)
{
	जर(client && client->listener){
		परत client->listener->port;
	}अन्यथा{
		परत 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(स्थिरांक संरचना mosquitto *client)
{
	जर(client){
		परत client->clean_start;
	}अन्यथा{
		परत true;
	}
}


BROKER_EXPORT स्थिरांक अक्षर *mosquitto_client_id(स्थिरांक संरचना mosquitto *client)
{
	जर(client){
		परत client->id;
	}अन्यथा{
		परत रिक्त;
	}
}


BROKER_EXPORT अचिन्हित mosquitto_client_id_hashv(स्थिरांक संरचना mosquitto *client)
{
	जर(client){
		परत client->id_hashv;
	}अन्यथा{
		परत 0;
	}
}


BROKER_EXPORT पूर्णांक mosquitto_client_keepalive(स्थिरांक संरचना mosquitto *client)
{
	जर(client){
		परत client->keepalive;
	}अन्यथा{
		परत -1;
	}
}


BROKER_EXPORT शून्य *mosquitto_client_certificate(स्थिरांक संरचना mosquitto *client)
{
#ifdef WITH_TLS
	जर(client && client->ssl){
		परत SSL_get_peer_certificate(client->ssl);
	}अन्यथा{
		परत रिक्त;
	}
#अन्यथा
	UNUSED(client);

	परत रिक्त;
#endif
}


BROKER_EXPORT पूर्णांक mosquitto_client_protocol(स्थिरांक संरचना mosquitto *client)
{
#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	जर(client && client->wsi){
		परत mp_websockets;
	}अन्यथा
#किंवाजर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	जर(client && client->transport == mosq_t_ws){
		परत mp_websockets;
	}अन्यथा
#अन्यथा
	UNUSED(client);
#endif
	{
		परत mp_mqtt;
	}
}


BROKER_EXPORT पूर्णांक mosquitto_client_protocol_version(स्थिरांक संरचना mosquitto *client)
{
	जर(client){
		निवड(client->protocol){
			स्थिती mosq_p_mqtt31:
				परत 3;
			स्थिती mosq_p_mqtt311:
				परत 4;
			स्थिती mosq_p_mqtt5:
				परत 5;
			पूर्वनिर्धारित:
				परत 0;
		}
	}अन्यथा{
		परत 0;
	}
}


BROKER_EXPORT पूर्णांक mosquitto_client_sub_मोजणी(स्थिरांक संरचना mosquitto *client)
{
	जर(client){
		परत client->subs_मोजणी;
	}अन्यथा{
		परत 0;
	}
}


BROKER_EXPORT स्थिरांक अक्षर *mosquitto_client_username(स्थिरांक संरचना mosquitto *client)
{
	जर(client){
#ifdef WITH_BRIDGE
		जर(client->bridge){
			परत client->bridge->local_username;
		}अन्यथा
#endif
		{
			परत client->username;
		}
	}अन्यथा{
		परत रिक्त;
	}
}


BROKER_EXPORT पूर्णांक mosquitto_broker_publish(
		स्थिरांक अक्षर *clientid,
		स्थिरांक अक्षर *topic,
		पूर्णांक payloadlen,
		शून्य *payload,
		पूर्णांक qos,
		bool retain,
		mosquitto_property *properties)
{
	संरचना mosquitto__message_v5 *संदेश;

	जर(topic == रिक्त
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == रिक्त)
			|| qos < 0 || qos > 2){

		परत MOSQ_ERR_INVAL;
	}

	संदेश = mosquitto_malloc(आकार(संरचना mosquitto__message_v5));
	जर(संदेश == रिक्त){
		परत MOSQ_ERR_NOMEM;
	}

	संदेश->next = रिक्त;
	संदेश->prev = रिक्त;
	जर(clientid){
		संदेश->clientid = mosquitto_strdup(clientid);
		जर(संदेश->clientid == रिक्त){
			mosquitto_FREE(संदेश);
			परत MOSQ_ERR_NOMEM;
		}
	}अन्यथा{
		संदेश->clientid = रिक्त;
	}
	संदेश->topic = mosquitto_strdup(topic);
	जर(संदेश->topic == रिक्त){
		mosquitto_FREE(संदेश->clientid);
		mosquitto_FREE(संदेश);
		परत MOSQ_ERR_NOMEM;
	}
	संदेश->payloadlen = payloadlen;
	संदेश->payload = payload;
	संदेश->qos = qos;
	संदेश->retain = retain;
	संदेश->properties = properties;

	DL_APPEND(db.plugin_msgs, संदेश);

	loop__update_next_event(1);
	परत MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णांक mosquitto_broker_publish_copy(
		स्थिरांक अक्षर *clientid,
		स्थिरांक अक्षर *topic,
		पूर्णांक payloadlen,
		स्थिरांक शून्य *payload,
		पूर्णांक qos,
		bool retain,
		mosquitto_property *properties)
{
	शून्य *payload_out;
	पूर्णांक rc;

	जर(topic == रिक्त
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == रिक्त)
			|| qos < 0 || qos > 2){

		परत MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (माप_प्रकार)(payloadlen+1));
	जर(payload_out == रिक्त){
		परत MOSQ_ERR_NOMEM;
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

	जर(rc){
		mosquitto_FREE(payload_out);
	}
	परत rc;
}


BROKER_EXPORT पूर्णांक mosquitto_set_username(संरचना mosquitto *client, स्थिरांक अक्षर *username)
{
	अक्षर *u_dup;
	अक्षर *old;

	जर(!client){
		परत MOSQ_ERR_INVAL;
	}

	जर(username){
		जर(mosquitto_validate_utf8(username, (पूर्णांक)म_माप(username))){
			परत MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		जर(!u_dup){
			परत MOSQ_ERR_NOMEM;
		}
	}अन्यथा{
		u_dup = रिक्त;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	परत MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णांक mosquitto_set_clientid(संरचना mosquitto *client, स्थिरांक अक्षर *clientid)
{
	संरचना mosquitto *found_client;
	अक्षर *id_dup;
	bool in_by_id;
	पूर्णांक clientid_len;

	जर(!client || !clientid){
		परत MOSQ_ERR_INVAL;
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

	जर(in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, clientid, म_माप(clientid), found_client);
		जर(found_client){
			परत MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (पूर्णांक)म_माप(clientid);
	जर(mosquitto_validate_utf8(clientid, clientid_len)){
		परत MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	जर(!id_dup){
		परत MOSQ_ERR_NOMEM;
	}

	जर(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	जर(in_by_id){
		context__add_to_by_id(client);
	}

	परत MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
स्थिर शून्य check_subscription_acls(संरचना mosquitto *context)
{
	पूर्णांक rc;
	uint8_t reason;

	साठी(पूर्णांक i=0; i<context->subs_capacity; i++){
		जर(context->subs[i] == रिक्त){
			सुरूठेवा;
		}
		rc = mosquitto_acl_check(context,
				context->subs[i]->topic_filter,
				0,
				रिक्त,
				0, /* FIXME */
				false,
				रिक्त,
				MOSQ_ACL_SUBSCRIBE);

		जर(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


स्थिर शून्य disconnect_client(संरचना mosquitto *context, bool with_will)
{
	जर(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, रिक्त);
	}
	जर(with_will == false){
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	जर(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT पूर्णांक mosquitto_kick_client_by_clientid(स्थिरांक अक्षर *clientid, bool with_will)
{
	संरचना mosquitto *ctxt, *ctxt_tmp;

	जर(clientid == रिक्त){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		परत MOSQ_ERR_SUCCESS;
	}अन्यथा{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, म_माप(clientid), ctxt);
		जर(ctxt){
			disconnect_client(ctxt, with_will);
			परत MOSQ_ERR_SUCCESS;
		}अन्यथा{
			परत MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT पूर्णांक mosquitto_kick_client_by_username(स्थिरांक अक्षर *username, bool with_will)
{
	संरचना mosquitto *ctxt, *ctxt_tmp;

	जर(username == रिक्त){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			जर(ctxt->username == रिक्त){
				disconnect_client(ctxt, with_will);
			}
		}
	}अन्यथा{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			जर(ctxt->username != रिक्त && !म_भेद(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	परत MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णांक mosquitto_apply_on_all_clients(पूर्णांक (*FUNC_client_functor)(स्थिरांक संरचना mosquitto *, शून्य *), शून्य *functor_context)
{
	पूर्णांक rc = MOSQ_ERR_SUCCESS;
	संरचना mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		जर(rc != MOSQ_ERR_SUCCESS){
			थांबा;
		}
	}

	परत rc;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_client_add(संरचना mosquitto_client *client)
{
	संरचना mosquitto *context;
	पूर्णांक rc;

	जर(client == रिक्त){
		परत MOSQ_ERR_INVAL;
	}
	जर(client->clientid == रिक्त){
		rc = MOSQ_ERR_INVAL;
		जा error;
	}

	context = रिक्त;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, म_माप(client->clientid), context);
	जर(context){
		rc = MOSQ_ERR_INVAL;
		जा error;
	}

	context = context__init();
	जर(!context){
		rc = MOSQ_ERR_NOMEM;
		जा error;
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
	जर(db.config->per_listener_settings && client->listener_port > 0){
		साठी(पूर्णांक i=0; i < db.config->listener_मोजणी; i++){
			जर(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				थांबा;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	परत MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	परत rc;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_client_update(संरचना mosquitto_client *client)
{
	संरचना mosquitto *context;
	पूर्णांक rc;

	जर(client == रिक्त){
		परत MOSQ_ERR_INVAL;
	}
	जर(client->clientid == रिक्त){
		rc = MOSQ_ERR_INVAL;
		जा error;
	}

	context = रिक्त;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, म_माप(client->clientid), context);
	जर(context == रिक्त){
		rc = MOSQ_ERR_NOT_FOUND;
		जा error;
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
	जर(db.config->per_listener_settings && client->listener_port > 0){
		साठी(पूर्णांक i=0; i < db.config->listener_मोजणी; i++){
			जर(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				थांबा;
			}
		}
	}

	परत MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	परत rc;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_client_delete(स्थिरांक अक्षर *clientid)
{
	संरचना mosquitto *context;

	जर(clientid == रिक्त){
		परत MOSQ_ERR_INVAL;
	}

	context = रिक्त;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, म_माप(clientid), context);
	जर(context == रिक्त){
		परत MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	परत MOSQ_ERR_SUCCESS;
}


स्थिर संरचना mosquitto_base_संदेश *find_store_संदेश(uint64_t store_id)
{
	संरचना mosquitto_base_संदेश *base_संदेश;

	HASH_FIND(hh, db.संदेश_store, &store_id, आकार(store_id), base_संदेश);
	परत base_संदेश;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_client_संदेश_add(संरचना mosquitto_client_संदेश_2 *client_संदेश)
{
	संरचना mosquitto *context;
	संरचना mosquitto_base_संदेश *base_संदेश;

	जर(client_संदेश == रिक्त || client_संदेश->clientid == रिक्त){
		परत MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_संदेश->clientid, म_माप(client_संदेश->clientid), context);
	जर(context == रिक्त){
		परत MOSQ_ERR_NOT_FOUND;
	}
	base_संदेश = find_store_संदेश(client_संदेश->store_id);
	जर(base_संदेश == रिक्त){
		परत MOSQ_ERR_NOT_FOUND;
	}

	जर(client_संदेश->direction == mosq_md_out){
		जर(client_संदेश->qos > 0){
			context->last_mid = client_संदेश->mid;
		}
		परत db_message_घाला_outgoing(context, client_संदेश->cmsg_id, client_संदेश->mid,
				client_संदेश->qos, client_संदेश->retain,
				base_संदेश, client_संदेश->subscription_identifier, false, false);
	}अन्यथा जर(client_संदेश->direction == mosq_md_in){
		परत db_message_घाला_incoming(context, client_संदेश->cmsg_id, base_संदेश, false);
	}अन्यथा{
		परत MOSQ_ERR_INVAL;
	}
	परत MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_client_संदेश_delete(संरचना mosquitto_client_संदेश_2 *client_संदेश)
{
	संरचना mosquitto *context;

	जर(client_संदेश == रिक्त || client_संदेश->clientid == रिक्त){
		परत MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_संदेश->clientid, म_माप(client_संदेश->clientid), context);
	जर(context == रिक्त){
		परत MOSQ_ERR_NOT_FOUND;
	}


	पूर्णांक rc = MOSQ_ERR_INVAL;
	जर(client_संदेश->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_संदेश->mid, mosq_ms_any, client_संदेश->qos);
	}अन्यथा जर(client_संदेश->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_संदेश->mid);
	}
	परत rc;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_client_संदेश_update(संरचना mosquitto_client_संदेश_2 *client_संदेश)
{
	संरचना mosquitto *context;

	जर(client_संदेश == रिक्त || client_संदेश->clientid == रिक्त){
		परत MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_संदेश->clientid, म_माप(client_संदेश->clientid), context);
	जर(context == रिक्त){
		परत MOSQ_ERR_NOT_FOUND;
	}

	जर(client_संदेश->direction == mosq_md_out){
		db__message_update_outgoing(context, client_संदेश->mid, client_संदेश->state, client_संदेश->qos, false);
	}अन्यथा जर(client_संदेश->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}अन्यथा{
		परत MOSQ_ERR_INVAL;
	}
	परत MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_client_संदेश_clear(संरचना mosquitto_client_संदेश_2 *client_संदेश)
{
	संरचना mosquitto *context;

	जर(client_संदेश == रिक्त || client_संदेश->clientid == रिक्त){
		परत MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_संदेश->clientid, म_माप(client_संदेश->clientid), context);
	जर(context == रिक्त){
		परत MOSQ_ERR_NOT_FOUND;
	}

	जर(client_संदेश->direction == mosq_bmd_in || client_संदेश->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	जर(client_संदेश->direction == mosq_bmd_out || client_संदेश->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	परत MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णांक mosquitto_subscription_add(स्थिरांक संरचना mosquitto_subscription *sub)
{
	संरचना mosquitto *context;

	जर(sub == रिक्त || sub->clientid == रिक्त || sub->topic_filter == रिक्त || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		परत MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, म_माप(sub->clientid), context);

	जर(context){
		परत sub__add(context, sub);
	}अन्यथा{
		परत MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT पूर्णांक mosquitto_subscription_delete(स्थिरांक अक्षर *clientid, स्थिरांक अक्षर *topic)
{
	संरचना mosquitto *context;
	uint8_t reason;

	जर(clientid == रिक्त || topic == रिक्त || clientid[0] == '\0' || topic[0] == '\0'){
		परत MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, म_माप(clientid), context);

	जर(context){
		परत sub__remove(context, topic, &reason);
	}अन्यथा{
		परत MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT पूर्णांक mosquitto_persist_base_संदेश_add(संरचना mosquitto_base_संदेश_2 *संदेश_add)
{
	संरचना mosquitto context;
	संरचना mosquitto_base_संदेश *base_संदेश;
	पूर्णांक rc;

	स_रखो(&context, 0, आकार(context));

	जर(संदेश_add->payloadlen > MQTT_MAX_PAYLOAD){
		परत MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (अक्षर *)संदेश_add->source_id;
	context.username = (अक्षर *)संदेश_add->source_username;

	base_संदेश = mosquitto_calloc(1, आकार(संरचना mosquitto_base_संदेश));
	जर(base_संदेश == रिक्त){
		जा error;
	}
	base_संदेश->data.store_id = संदेश_add->store_id;
	base_संदेश->data.expiry_time = संदेश_add->expiry_time;
	base_संदेश->data.payloadlen = संदेश_add->payloadlen;
	base_संदेश->data.source_mid = संदेश_add->source_mid;
	base_संदेश->data.qos = संदेश_add->qos;
	base_संदेश->data.retain = संदेश_add->retain;

	base_संदेश->data.payload = संदेश_add->payload;
	संदेश_add->payload = रिक्त;
	base_संदेश->data.topic = संदेश_add->topic;
	संदेश_add->topic = रिक्त;
	base_संदेश->data.properties = संदेश_add->properties;
	संदेश_add->properties = रिक्त;

	जर(संदेश_add->source_port){
		साठी(पूर्णांक i=0; i<db.config->listener_मोजणी; i++){
			जर(db.config->listeners[i].port == संदेश_add->source_port){
				base_संदेश->source_listener = &db.config->listeners[i];
				थांबा;
			}
		}
	}

	base_संदेश->stored = true;
	rc = db__message_store(&context, base_संदेश, रिक्त, mosq_mo_broker);
	परत rc;

error:
	mosquitto_property_free_all(&संदेश_add->properties);
	mosquitto_free(संदेश_add->topic);
	mosquitto_free(संदेश_add->payload);
	mosquitto_free(base_संदेश);

	परत MOSQ_ERR_NOMEM;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_base_संदेश_delete(uint64_t store_id)
{
	संरचना mosquitto_base_संदेश *base_संदेश;

	base_संदेश = find_store_संदेश(store_id);
	जर(base_संदेश && base_संदेश->ref_मोजणी == 0){
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

	परत MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT शून्य mosquitto_complete_basic_auth(स्थिरांक अक्षर *clientid, पूर्णांक result)
{
	संरचना mosquitto *context;

	जर(clientid == रिक्त){
		परत;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, म_माप(clientid), context);
	जर(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		जर(result == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, रिक्त, 0);
		}अन्यथा{
			जर(context->protocol == mosq_p_mqtt5){
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


BROKER_EXPORT पूर्णांक mosquitto_broker_गाठ_id_set(uint16_t id)
{
	जर(id > 1023){
		परत MOSQ_ERR_INVAL;
	}अन्यथा{
		db.गाठ_id = id;
		db.गाठ_id_shifted = ((uint64_t)id) << 54;
		परत MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT स्थिरांक अक्षर *mosquitto_persistence_location(शून्य)
{
	परत db.config->persistence_location;
}


BROKER_EXPORT पूर्णांक mosquitto_client_will_set(स्थिरांक अक्षर *clientid, स्थिरांक अक्षर *topic, पूर्णांक payloadlen, स्थिरांक शून्य *payload, पूर्णांक qos, bool retain, mosquitto_property *properties)
{
	संरचना mosquitto *mosq = mosquitto_client(clientid);
	जर(!mosq){
		परत MOSQ_ERR_NOT_FOUND;
	}
	जर(properties && mosq->protocol != mosq_p_mqtt5){
		जर(net__is_connected(mosq)){
			परत MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	परत will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
