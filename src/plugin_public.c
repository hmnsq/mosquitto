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
		ध्रुव वर्ण *plugin_name,
		ध्रुव वर्ण *plugin_version)
{
	यदि(identifier == रिक्त || plugin_name == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	यदि(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}अन्यथा{
		identifier->plugin_version = रिक्त;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ध्रुव वर्ण *mosquitto_client_address(ध्रुव संरचना mosquitto *client)
{
	यदि(client){
		प्रत्यागम client->address;
	}अन्यथा{
		प्रत्यागम रिक्त;
	}
}


BROKER_EXPORT संरचना mosquitto *mosquitto_client(ध्रुव वर्ण *clientid)
{
	size_t len;
	संरचना mosquitto *context;

	यदि(!clientid){
		प्रत्यागम रिक्त;
	}
	len = strlen(clientid);
	यदि(len == 0){
		प्रत्यागम रिक्त;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	प्रत्यागम context;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_port(ध्रुव संरचना mosquitto *client)
{
	यदि(client && client->listener){
		प्रत्यागम client->listener->port;
	}अन्यथा{
		प्रत्यागम 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(ध्रुव संरचना mosquitto *client)
{
	यदि(client){
		प्रत्यागम client->clean_start;
	}अन्यथा{
		प्रत्यागम true;
	}
}


BROKER_EXPORT ध्रुव वर्ण *mosquitto_client_id(ध्रुव संरचना mosquitto *client)
{
	यदि(client){
		प्रत्यागम client->id;
	}अन्यथा{
		प्रत्यागम रिक्त;
	}
}


BROKER_EXPORT unsigned mosquitto_client_id_hashv(ध्रुव संरचना mosquitto *client)
{
	यदि(client){
		प्रत्यागम client->id_hashv;
	}अन्यथा{
		प्रत्यागम 0;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_keepalive(ध्रुव संरचना mosquitto *client)
{
	यदि(client){
		प्रत्यागम client->keepalive;
	}अन्यथा{
		प्रत्यागम -1;
	}
}


BROKER_EXPORT शून्य *mosquitto_client_certificate(ध्रुव संरचना mosquitto *client)
{
#ifdef WITH_TLS
	यदि(client && client->ssl){
		प्रत्यागम SSL_get_peer_certificate(client->ssl);
	}अन्यथा{
		प्रत्यागम रिक्त;
	}
#अन्यथा
	UNUSED(client);

	प्रत्यागम रिक्त;
#endif
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_protocol(ध्रुव संरचना mosquitto *client)
{
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	यदि(client && client->wsi){
		प्रत्यागम mp_websockets;
	}अन्यथा
#नोचेद्यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	यदि(client && client->transport == mosq_t_ws){
		प्रत्यागम mp_websockets;
	}अन्यथा
#अन्यथा
	UNUSED(client);
#endif
	{
		प्रत्यागम mp_mqtt;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_protocol_version(ध्रुव संरचना mosquitto *client)
{
	यदि(client){
		विकल्प(client->protocol){
			पक्ष mosq_p_mqtt31:
				प्रत्यागम 3;
			पक्ष mosq_p_mqtt311:
				प्रत्यागम 4;
			पक्ष mosq_p_mqtt5:
				प्रत्यागम 5;
			स्वभाव:
				प्रत्यागम 0;
		}
	}अन्यथा{
		प्रत्यागम 0;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_sub_गिनती(ध्रुव संरचना mosquitto *client)
{
	यदि(client){
		प्रत्यागम client->subs_गिनती;
	}अन्यथा{
		प्रत्यागम 0;
	}
}


BROKER_EXPORT ध्रुव वर्ण *mosquitto_client_username(ध्रुव संरचना mosquitto *client)
{
	यदि(client){
#ifdef WITH_BRIDGE
		यदि(client->bridge){
			प्रत्यागम client->bridge->local_username;
		}अन्यथा
#endif
		{
			प्रत्यागम client->username;
		}
	}अन्यथा{
		प्रत्यागम रिक्त;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_broker_publish(
		ध्रुव वर्ण *clientid,
		ध्रुव वर्ण *topic,
		पूर्णाङ्क payloadlen,
		शून्य *payload,
		पूर्णाङ्क qos,
		bool retain,
		mosquitto_property *properties)
{
	संरचना mosquitto__message_v5 *संदेश;

	यदि(topic == रिक्त
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == रिक्त)
			|| qos < 0 || qos > 2){

		प्रत्यागम MOSQ_ERR_INVAL;
	}

	संदेश = mosquitto_malloc(परिमाण(संरचना mosquitto__message_v5));
	यदि(संदेश == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	संदेश->next = रिक्त;
	संदेश->prev = रिक्त;
	यदि(clientid){
		संदेश->clientid = mosquitto_strdup(clientid);
		यदि(संदेश->clientid == रिक्त){
			mosquitto_FREE(संदेश);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}अन्यथा{
		संदेश->clientid = रिक्त;
	}
	संदेश->topic = mosquitto_strdup(topic);
	यदि(संदेश->topic == रिक्त){
		mosquitto_FREE(संदेश->clientid);
		mosquitto_FREE(संदेश);
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	संदेश->payloadlen = payloadlen;
	संदेश->payload = payload;
	संदेश->qos = qos;
	संदेश->retain = retain;
	संदेश->properties = properties;

	DL_APPEND(db.plugin_msgs, संदेश);

	loop__update_next_event(1);
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_broker_publish_copy(
		ध्रुव वर्ण *clientid,
		ध्रुव वर्ण *topic,
		पूर्णाङ्क payloadlen,
		ध्रुव शून्य *payload,
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

		प्रत्यागम MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (size_t)(payloadlen+1));
	यदि(payload_out == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
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

	यदि(rc){
		mosquitto_FREE(payload_out);
	}
	प्रत्यागम rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_set_username(संरचना mosquitto *client, ध्रुव वर्ण *username)
{
	वर्ण *u_dup;
	वर्ण *old;

	यदि(!client){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	यदि(username){
		यदि(mosquitto_validate_utf8(username, (पूर्णाङ्क)strlen(username))){
			प्रत्यागम MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		यदि(!u_dup){
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}अन्यथा{
		u_dup = रिक्त;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_set_clientid(संरचना mosquitto *client, ध्रुव वर्ण *clientid)
{
	संरचना mosquitto *found_client;
	वर्ण *id_dup;
	bool in_by_id;
	पूर्णाङ्क clientid_len;

	यदि(!client || !clientid){
		प्रत्यागम MOSQ_ERR_INVAL;
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
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), found_client);
		यदि(found_client){
			प्रत्यागम MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (पूर्णाङ्क)strlen(clientid);
	यदि(mosquitto_validate_utf8(clientid, clientid_len)){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	यदि(!id_dup){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	यदि(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	यदि(in_by_id){
		context__add_to_by_id(client);
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
स्थिर शून्य check_subscription_acls(संरचना mosquitto *context)
{
	पूर्णाङ्क rc;
	uint8_t reason;

	प्रति(पूर्णाङ्क i=0; i<context->subs_capacity; i++){
		यदि(context->subs[i] == रिक्त){
			अनुवर्त;
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


BROKER_EXPORT पूर्णाङ्क mosquitto_kick_client_by_clientid(ध्रुव वर्ण *clientid, bool with_will)
{
	संरचना mosquitto *ctxt, *ctxt_tmp;

	यदि(clientid == रिक्त){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अन्यथा{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), ctxt);
		यदि(ctxt){
			disconnect_client(ctxt, with_will);
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}अन्यथा{
			प्रत्यागम MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_kick_client_by_username(ध्रुव वर्ण *username, bool with_will)
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
			यदि(ctxt->username != रिक्त && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_apply_on_all_clients(पूर्णाङ्क (*FUNC_client_functor)(ध्रुव संरचना mosquitto *, शून्य *), शून्य *functor_context)
{
	पूर्णाङ्क rc = MOSQ_ERR_SUCCESS;
	संरचना mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		यदि(rc != MOSQ_ERR_SUCCESS){
			भङ्ग;
		}
	}

	प्रत्यागम rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_add(संरचना mosquitto_client *client)
{
	संरचना mosquitto *context;
	पूर्णाङ्क rc;

	यदि(client == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(client->clientid == रिक्त){
		rc = MOSQ_ERR_INVAL;
		गच्छ error;
	}

	context = रिक्त;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
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
		प्रति(पूर्णाङ्क i=0; i < db.config->listener_गिनती; i++){
			यदि(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				भङ्ग;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	प्रत्यागम MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	प्रत्यागम rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_update(संरचना mosquitto_client *client)
{
	संरचना mosquitto *context;
	पूर्णाङ्क rc;

	यदि(client == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(client->clientid == रिक्त){
		rc = MOSQ_ERR_INVAL;
		गच्छ error;
	}

	context = रिक्त;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
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
		प्रति(पूर्णाङ्क i=0; i < db.config->listener_गिनती; i++){
			यदि(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				भङ्ग;
			}
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	प्रत्यागम rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_delete(ध्रुव वर्ण *clientid)
{
	संरचना mosquitto *context;

	यदि(clientid == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	context = रिक्त;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	यदि(context == रिक्त){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर संरचना mosquitto_base_संदेश *find_store_संदेश(uint64_t store_id)
{
	संरचना mosquitto_base_संदेश *base_संदेश;

	HASH_FIND(hh, db.संदेश_store, &store_id, परिमाण(store_id), base_संदेश);
	प्रत्यागम base_संदेश;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_संदेश_add(संरचना mosquitto_client_संदेश_2 *client_संदेश)
{
	संरचना mosquitto *context;
	संरचना mosquitto_base_संदेश *base_संदेश;

	यदि(client_संदेश == रिक्त || client_संदेश->clientid == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_संदेश->clientid, strlen(client_संदेश->clientid), context);
	यदि(context == रिक्त){
		प्रत्यागम MOSQ_ERR_NOT_FOUND;
	}
	base_संदेश = find_store_संदेश(client_संदेश->store_id);
	यदि(base_संदेश == रिक्त){
		प्रत्यागम MOSQ_ERR_NOT_FOUND;
	}

	यदि(client_संदेश->direction == mosq_md_out){
		यदि(client_संदेश->qos > 0){
			context->last_mid = client_संदेश->mid;
		}
		प्रत्यागम db_message_जोड़ो_outgoing(context, client_संदेश->cmsg_id, client_संदेश->mid,
				client_संदेश->qos, client_संदेश->retain,
				base_संदेश, client_संदेश->subscription_identifier, false, false);
	}अन्यथा यदि(client_संदेश->direction == mosq_md_in){
		प्रत्यागम db_message_जोड़ो_incoming(context, client_संदेश->cmsg_id, base_संदेश, false);
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_संदेश_delete(संरचना mosquitto_client_संदेश_2 *client_संदेश)
{
	संरचना mosquitto *context;

	यदि(client_संदेश == रिक्त || client_संदेश->clientid == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_संदेश->clientid, strlen(client_संदेश->clientid), context);
	यदि(context == रिक्त){
		प्रत्यागम MOSQ_ERR_NOT_FOUND;
	}


	पूर्णाङ्क rc = MOSQ_ERR_INVAL;
	यदि(client_संदेश->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_संदेश->mid, mosq_ms_any, client_संदेश->qos);
	}अन्यथा यदि(client_संदेश->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_संदेश->mid);
	}
	प्रत्यागम rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_संदेश_update(संरचना mosquitto_client_संदेश_2 *client_संदेश)
{
	संरचना mosquitto *context;

	यदि(client_संदेश == रिक्त || client_संदेश->clientid == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_संदेश->clientid, strlen(client_संदेश->clientid), context);
	यदि(context == रिक्त){
		प्रत्यागम MOSQ_ERR_NOT_FOUND;
	}

	यदि(client_संदेश->direction == mosq_md_out){
		db__message_update_outgoing(context, client_संदेश->mid, client_संदेश->state, client_संदेश->qos, false);
	}अन्यथा यदि(client_संदेश->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_संदेश_clear(संरचना mosquitto_client_संदेश_2 *client_संदेश)
{
	संरचना mosquitto *context;

	यदि(client_संदेश == रिक्त || client_संदेश->clientid == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_संदेश->clientid, strlen(client_संदेश->clientid), context);
	यदि(context == रिक्त){
		प्रत्यागम MOSQ_ERR_NOT_FOUND;
	}

	यदि(client_संदेश->direction == mosq_bmd_in || client_संदेश->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	यदि(client_संदेश->direction == mosq_bmd_out || client_संदेश->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_subscription_add(ध्रुव संरचना mosquitto_subscription *sub)
{
	संरचना mosquitto *context;

	यदि(sub == रिक्त || sub->clientid == रिक्त || sub->topic_filter == रिक्त || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, strlen(sub->clientid), context);

	यदि(context){
		प्रत्यागम sub__add(context, sub);
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_subscription_delete(ध्रुव वर्ण *clientid, ध्रुव वर्ण *topic)
{
	संरचना mosquitto *context;
	uint8_t reason;

	यदि(clientid == रिक्त || topic == रिक्त || clientid[0] == '\0' || topic[0] == '\0'){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	यदि(context){
		प्रत्यागम sub__remove(context, topic, &reason);
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_base_संदेश_add(संरचना mosquitto_base_संदेश_2 *संदेश_add)
{
	संरचना mosquitto context;
	संरचना mosquitto_base_संदेश *base_संदेश;
	पूर्णाङ्क rc;

	memset(&context, 0, परिमाण(context));

	यदि(संदेश_add->payloadlen > MQTT_MAX_PAYLOAD){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (वर्ण *)संदेश_add->source_id;
	context.username = (वर्ण *)संदेश_add->source_username;

	base_संदेश = mosquitto_calloc(1, परिमाण(संरचना mosquitto_base_संदेश));
	यदि(base_संदेश == रिक्त){
		गच्छ error;
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

	यदि(संदेश_add->source_port){
		प्रति(पूर्णाङ्क i=0; i<db.config->listener_गिनती; i++){
			यदि(db.config->listeners[i].port == संदेश_add->source_port){
				base_संदेश->source_listener = &db.config->listeners[i];
				भङ्ग;
			}
		}
	}

	base_संदेश->stored = true;
	rc = db__message_store(&context, base_संदेश, रिक्त, mosq_mo_broker);
	प्रत्यागम rc;

error:
	mosquitto_property_free_all(&संदेश_add->properties);
	mosquitto_free(संदेश_add->topic);
	mosquitto_free(संदेश_add->payload);
	mosquitto_free(base_संदेश);

	प्रत्यागम MOSQ_ERR_NOMEM;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_base_संदेश_delete(uint64_t store_id)
{
	संरचना mosquitto_base_संदेश *base_संदेश;

	base_संदेश = find_store_संदेश(store_id);
	यदि(base_संदेश && base_संदेश->ref_गिनती == 0){
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

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT शून्य mosquitto_complete_basic_auth(ध्रुव वर्ण *clientid, पूर्णाङ्क result)
{
	संरचना mosquitto *context;

	यदि(clientid == रिक्त){
		प्रत्यागम;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, strlen(clientid), context);
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


BROKER_EXPORT पूर्णाङ्क mosquitto_broker_गाँठ_id_set(uint16_t id)
{
	यदि(id > 1023){
		प्रत्यागम MOSQ_ERR_INVAL;
	}अन्यथा{
		db.गाँठ_id = id;
		db.गाँठ_id_shifted = ((uint64_t)id) << 54;
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT ध्रुव वर्ण *mosquitto_persistence_location(शून्य)
{
	प्रत्यागम db.config->persistence_location;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_will_set(ध्रुव वर्ण *clientid, ध्रुव वर्ण *topic, पूर्णाङ्क payloadlen, ध्रुव शून्य *payload, पूर्णाङ्क qos, bool retain, mosquitto_property *properties)
{
	संरचना mosquitto *mosq = mosquitto_client(clientid);
	यदि(!mosq){
		प्रत्यागम MOSQ_ERR_NOT_FOUND;
	}
	यदि(properties && mosq->protocol != mosq_p_mqtt5){
		यदि(net__is_connected(mosq)){
			प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	प्रत्यागम will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
