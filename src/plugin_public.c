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
	सचे(identifier == रिक्त || plugin_name == रिक्त){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	सचे(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}अञ्ञथा{
		identifier->plugin_version = रिक्त;
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ध्रुव वर्ण *mosquitto_client_address(ध्रुव संरचना mosquitto *client)
{
	सचे(client){
		पटिनिवत्त client->address;
	}अञ्ञथा{
		पटिनिवत्त रिक्त;
	}
}


BROKER_EXPORT संरचना mosquitto *mosquitto_client(ध्रुव वर्ण *clientid)
{
	size_t len;
	संरचना mosquitto *context;

	सचे(!clientid){
		पटिनिवत्त रिक्त;
	}
	len = strlen(clientid);
	सचे(len == 0){
		पटिनिवत्त रिक्त;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	पटिनिवत्त context;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_port(ध्रुव संरचना mosquitto *client)
{
	सचे(client && client->listener){
		पटिनिवत्त client->listener->port;
	}अञ्ञथा{
		पटिनिवत्त 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(ध्रुव संरचना mosquitto *client)
{
	सचे(client){
		पटिनिवत्त client->clean_start;
	}अञ्ञथा{
		पटिनिवत्त true;
	}
}


BROKER_EXPORT ध्रुव वर्ण *mosquitto_client_id(ध्रुव संरचना mosquitto *client)
{
	सचे(client){
		पटिनिवत्त client->id;
	}अञ्ञथा{
		पटिनिवत्त रिक्त;
	}
}


BROKER_EXPORT unsigned mosquitto_client_id_hashv(ध्रुव संरचना mosquitto *client)
{
	सचे(client){
		पटिनिवत्त client->id_hashv;
	}अञ्ञथा{
		पटिनिवत्त 0;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_keepalive(ध्रुव संरचना mosquitto *client)
{
	सचे(client){
		पटिनिवत्त client->keepalive;
	}अञ्ञथा{
		पटिनिवत्त -1;
	}
}


BROKER_EXPORT शून्य *mosquitto_client_certificate(ध्रुव संरचना mosquitto *client)
{
#ifdef WITH_TLS
	सचे(client && client->ssl){
		पटिनिवत्त SSL_get_peer_certificate(client->ssl);
	}अञ्ञथा{
		पटिनिवत्त रिक्त;
	}
#अञ्ञथा
	UNUSED(client);

	पटिनिवत्त रिक्त;
#endif
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_protocol(ध्रुव संरचना mosquitto *client)
{
#सचे defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	सचे(client && client->wsi){
		पटिनिवत्त mp_websockets;
	}अञ्ञथा
#नोचेद्यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	सचे(client && client->transport == mosq_t_ws){
		पटिनिवत्त mp_websockets;
	}अञ्ञथा
#अञ्ञथा
	UNUSED(client);
#endif
	{
		पटिनिवत्त mp_mqtt;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_protocol_version(ध्रुव संरचना mosquitto *client)
{
	सचे(client){
		विकल्प(client->protocol){
			पक्ष mosq_p_mqtt31:
				पटिनिवत्त 3;
			पक्ष mosq_p_mqtt311:
				पटिनिवत्त 4;
			पक्ष mosq_p_mqtt5:
				पटिनिवत्त 5;
			स्वभाव:
				पटिनिवत्त 0;
		}
	}अञ्ञथा{
		पटिनिवत्त 0;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_sub_गणनम्(ध्रुव संरचना mosquitto *client)
{
	सचे(client){
		पटिनिवत्त client->subs_गणनम्;
	}अञ्ञथा{
		पटिनिवत्त 0;
	}
}


BROKER_EXPORT ध्रुव वर्ण *mosquitto_client_username(ध्रुव संरचना mosquitto *client)
{
	सचे(client){
#ifdef WITH_BRIDGE
		सचे(client->bridge){
			पटिनिवत्त client->bridge->local_username;
		}अञ्ञथा
#endif
		{
			पटिनिवत्त client->username;
		}
	}अञ्ञथा{
		पटिनिवत्त रिक्त;
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
	संरचना mosquitto__message_v5 *सन्देशः;

	सचे(topic == रिक्त
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == रिक्त)
			|| qos < 0 || qos > 2){

		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	सन्देशः = mosquitto_malloc(परिमाण(संरचना mosquitto__message_v5));
	सचे(सन्देशः == रिक्त){
		पटिनिवत्त MOSQ_ERR_NOMEM;
	}

	सन्देशः->next = रिक्त;
	सन्देशः->prev = रिक्त;
	सचे(clientid){
		सन्देशः->clientid = mosquitto_strdup(clientid);
		सचे(सन्देशः->clientid == रिक्त){
			mosquitto_FREE(सन्देशः);
			पटिनिवत्त MOSQ_ERR_NOMEM;
		}
	}अञ्ञथा{
		सन्देशः->clientid = रिक्त;
	}
	सन्देशः->topic = mosquitto_strdup(topic);
	सचे(सन्देशः->topic == रिक्त){
		mosquitto_FREE(सन्देशः->clientid);
		mosquitto_FREE(सन्देशः);
		पटिनिवत्त MOSQ_ERR_NOMEM;
	}
	सन्देशः->payloadlen = payloadlen;
	सन्देशः->payload = payload;
	सन्देशः->qos = qos;
	सन्देशः->retain = retain;
	सन्देशः->properties = properties;

	DL_APPEND(db.plugin_msgs, सन्देशः);

	loop__update_next_event(1);
	पटिनिवत्त MOSQ_ERR_SUCCESS;
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

	सचे(topic == रिक्त
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == रिक्त)
			|| qos < 0 || qos > 2){

		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (size_t)(payloadlen+1));
	सचे(payload_out == रिक्त){
		पटिनिवत्त MOSQ_ERR_NOMEM;
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

	सचे(rc){
		mosquitto_FREE(payload_out);
	}
	पटिनिवत्त rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_set_username(संरचना mosquitto *client, ध्रुव वर्ण *username)
{
	वर्ण *u_dup;
	वर्ण *old;

	सचे(!client){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	सचे(username){
		सचे(mosquitto_validate_utf8(username, (पूर्णाङ्क)strlen(username))){
			पटिनिवत्त MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		सचे(!u_dup){
			पटिनिवत्त MOSQ_ERR_NOMEM;
		}
	}अञ्ञथा{
		u_dup = रिक्त;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_set_clientid(संरचना mosquitto *client, ध्रुव वर्ण *clientid)
{
	संरचना mosquitto *found_client;
	वर्ण *id_dup;
	bool in_by_id;
	पूर्णाङ्क clientid_len;

	सचे(!client || !clientid){
		पटिनिवत्त MOSQ_ERR_INVAL;
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

	सचे(in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), found_client);
		सचे(found_client){
			पटिनिवत्त MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (पूर्णाङ्क)strlen(clientid);
	सचे(mosquitto_validate_utf8(clientid, clientid_len)){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	सचे(!id_dup){
		पटिनिवत्त MOSQ_ERR_NOMEM;
	}

	सचे(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	सचे(in_by_id){
		context__add_to_by_id(client);
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
स्थिर शून्य check_subscription_acls(संरचना mosquitto *context)
{
	पूर्णाङ्क rc;
	uint8_t reason;

	अत्थाय(पूर्णाङ्क i=0; i<context->subs_capacity; i++){
		सचे(context->subs[i] == रिक्त){
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

		सचे(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


स्थिर शून्य disconnect_client(संरचना mosquitto *context, bool with_will)
{
	सचे(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, रिक्त);
	}
	सचे(with_will == false){
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	सचे(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT पूर्णाङ्क mosquitto_kick_client_by_clientid(ध्रुव वर्ण *clientid, bool with_will)
{
	संरचना mosquitto *ctxt, *ctxt_tmp;

	सचे(clientid == रिक्त){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}अञ्ञथा{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), ctxt);
		सचे(ctxt){
			disconnect_client(ctxt, with_will);
			पटिनिवत्त MOSQ_ERR_SUCCESS;
		}अञ्ञथा{
			पटिनिवत्त MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_kick_client_by_username(ध्रुव वर्ण *username, bool with_will)
{
	संरचना mosquitto *ctxt, *ctxt_tmp;

	सचे(username == रिक्त){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			सचे(ctxt->username == रिक्त){
				disconnect_client(ctxt, with_will);
			}
		}
	}अञ्ञथा{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			सचे(ctxt->username != रिक्त && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_apply_on_all_clients(पूर्णाङ्क (*FUNC_client_functor)(ध्रुव संरचना mosquitto *, शून्य *), शून्य *functor_context)
{
	पूर्णाङ्क rc = MOSQ_ERR_SUCCESS;
	संरचना mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		सचे(rc != MOSQ_ERR_SUCCESS){
			भङ्ग;
		}
	}

	पटिनिवत्त rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_add(संरचना mosquitto_client *client)
{
	संरचना mosquitto *context;
	पूर्णाङ्क rc;

	सचे(client == रिक्त){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}
	सचे(client->clientid == रिक्त){
		rc = MOSQ_ERR_INVAL;
		गच्छ error;
	}

	context = रिक्त;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	सचे(context){
		rc = MOSQ_ERR_INVAL;
		गच्छ error;
	}

	context = context__init();
	सचे(!context){
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
	सचे(db.config->per_listener_settings && client->listener_port > 0){
		अत्थाय(पूर्णाङ्क i=0; i < db.config->listener_गणनम्; i++){
			सचे(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				भङ्ग;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	पटिनिवत्त MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	पटिनिवत्त rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_update(संरचना mosquitto_client *client)
{
	संरचना mosquitto *context;
	पूर्णाङ्क rc;

	सचे(client == रिक्त){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}
	सचे(client->clientid == रिक्त){
		rc = MOSQ_ERR_INVAL;
		गच्छ error;
	}

	context = रिक्त;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	सचे(context == रिक्त){
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
	सचे(db.config->per_listener_settings && client->listener_port > 0){
		अत्थाय(पूर्णाङ्क i=0; i < db.config->listener_गणनम्; i++){
			सचे(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				भङ्ग;
			}
		}
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	पटिनिवत्त rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_delete(ध्रुव वर्ण *clientid)
{
	संरचना mosquitto *context;

	सचे(clientid == रिक्त){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	context = रिक्त;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	सचे(context == रिक्त){
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


स्थिर संरचना mosquitto_base_सन्देशः *find_store_सन्देशः(uint64_t store_id)
{
	संरचना mosquitto_base_सन्देशः *base_सन्देशः;

	HASH_FIND(hh, db.सन्देशः_store, &store_id, परिमाण(store_id), base_सन्देशः);
	पटिनिवत्त base_सन्देशः;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_सन्देशः_add(संरचना mosquitto_client_सन्देशः_2 *client_सन्देशः)
{
	संरचना mosquitto *context;
	संरचना mosquitto_base_सन्देशः *base_सन्देशः;

	सचे(client_सन्देशः == रिक्त || client_सन्देशः->clientid == रिक्त){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_सन्देशः->clientid, strlen(client_सन्देशः->clientid), context);
	सचे(context == रिक्त){
		पटिनिवत्त MOSQ_ERR_NOT_FOUND;
	}
	base_सन्देशः = find_store_सन्देशः(client_सन्देशः->store_id);
	सचे(base_सन्देशः == रिक्त){
		पटिनिवत्त MOSQ_ERR_NOT_FOUND;
	}

	सचे(client_सन्देशः->direction == mosq_md_out){
		सचे(client_सन्देशः->qos > 0){
			context->last_mid = client_सन्देशः->mid;
		}
		पटिनिवत्त db_message_निवेशय_outgoing(context, client_सन्देशः->cmsg_id, client_सन्देशः->mid,
				client_सन्देशः->qos, client_सन्देशः->retain,
				base_सन्देशः, client_सन्देशः->subscription_identifier, false, false);
	}अञ्ञथा सचे(client_सन्देशः->direction == mosq_md_in){
		पटिनिवत्त db_message_निवेशय_incoming(context, client_सन्देशः->cmsg_id, base_सन्देशः, false);
	}अञ्ञथा{
		पटिनिवत्त MOSQ_ERR_INVAL;
	}
	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_सन्देशः_delete(संरचना mosquitto_client_सन्देशः_2 *client_सन्देशः)
{
	संरचना mosquitto *context;

	सचे(client_सन्देशः == रिक्त || client_सन्देशः->clientid == रिक्त){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_सन्देशः->clientid, strlen(client_सन्देशः->clientid), context);
	सचे(context == रिक्त){
		पटिनिवत्त MOSQ_ERR_NOT_FOUND;
	}


	पूर्णाङ्क rc = MOSQ_ERR_INVAL;
	सचे(client_सन्देशः->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_सन्देशः->mid, mosq_ms_any, client_सन्देशः->qos);
	}अञ्ञथा सचे(client_सन्देशः->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_सन्देशः->mid);
	}
	पटिनिवत्त rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_सन्देशः_update(संरचना mosquitto_client_सन्देशः_2 *client_सन्देशः)
{
	संरचना mosquitto *context;

	सचे(client_सन्देशः == रिक्त || client_सन्देशः->clientid == रिक्त){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_सन्देशः->clientid, strlen(client_सन्देशः->clientid), context);
	सचे(context == रिक्त){
		पटिनिवत्त MOSQ_ERR_NOT_FOUND;
	}

	सचे(client_सन्देशः->direction == mosq_md_out){
		db__message_update_outgoing(context, client_सन्देशः->mid, client_सन्देशः->state, client_सन्देशः->qos, false);
	}अञ्ञथा सचे(client_सन्देशः->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}अञ्ञथा{
		पटिनिवत्त MOSQ_ERR_INVAL;
	}
	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_सन्देशः_clear(संरचना mosquitto_client_सन्देशः_2 *client_सन्देशः)
{
	संरचना mosquitto *context;

	सचे(client_सन्देशः == रिक्त || client_सन्देशः->clientid == रिक्त){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_सन्देशः->clientid, strlen(client_सन्देशः->clientid), context);
	सचे(context == रिक्त){
		पटिनिवत्त MOSQ_ERR_NOT_FOUND;
	}

	सचे(client_सन्देशः->direction == mosq_bmd_in || client_सन्देशः->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	सचे(client_सन्देशः->direction == mosq_bmd_out || client_सन्देशः->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_subscription_add(ध्रुव संरचना mosquitto_subscription *sub)
{
	संरचना mosquitto *context;

	सचे(sub == रिक्त || sub->clientid == रिक्त || sub->topic_filter == रिक्त || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, strlen(sub->clientid), context);

	सचे(context){
		पटिनिवत्त sub__add(context, sub);
	}अञ्ञथा{
		पटिनिवत्त MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_subscription_delete(ध्रुव वर्ण *clientid, ध्रुव वर्ण *topic)
{
	संरचना mosquitto *context;
	uint8_t reason;

	सचे(clientid == रिक्त || topic == रिक्त || clientid[0] == '\0' || topic[0] == '\0'){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	सचे(context){
		पटिनिवत्त sub__remove(context, topic, &reason);
	}अञ्ञथा{
		पटिनिवत्त MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_base_सन्देशः_add(संरचना mosquitto_base_सन्देशः_2 *सन्देशः_add)
{
	संरचना mosquitto context;
	संरचना mosquitto_base_सन्देशः *base_सन्देशः;
	पूर्णाङ्क rc;

	memset(&context, 0, परिमाण(context));

	सचे(सन्देशः_add->payloadlen > MQTT_MAX_PAYLOAD){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (वर्ण *)सन्देशः_add->source_id;
	context.username = (वर्ण *)सन्देशः_add->source_username;

	base_सन्देशः = mosquitto_calloc(1, परिमाण(संरचना mosquitto_base_सन्देशः));
	सचे(base_सन्देशः == रिक्त){
		गच्छ error;
	}
	base_सन्देशः->data.store_id = सन्देशः_add->store_id;
	base_सन्देशः->data.expiry_time = सन्देशः_add->expiry_time;
	base_सन्देशः->data.payloadlen = सन्देशः_add->payloadlen;
	base_सन्देशः->data.source_mid = सन्देशः_add->source_mid;
	base_सन्देशः->data.qos = सन्देशः_add->qos;
	base_सन्देशः->data.retain = सन्देशः_add->retain;

	base_सन्देशः->data.payload = सन्देशः_add->payload;
	सन्देशः_add->payload = रिक्त;
	base_सन्देशः->data.topic = सन्देशः_add->topic;
	सन्देशः_add->topic = रिक्त;
	base_सन्देशः->data.properties = सन्देशः_add->properties;
	सन्देशः_add->properties = रिक्त;

	सचे(सन्देशः_add->source_port){
		अत्थाय(पूर्णाङ्क i=0; i<db.config->listener_गणनम्; i++){
			सचे(db.config->listeners[i].port == सन्देशः_add->source_port){
				base_सन्देशः->source_listener = &db.config->listeners[i];
				भङ्ग;
			}
		}
	}

	base_सन्देशः->stored = true;
	rc = db__message_store(&context, base_सन्देशः, रिक्त, mosq_mo_broker);
	पटिनिवत्त rc;

error:
	mosquitto_property_free_all(&सन्देशः_add->properties);
	mosquitto_free(सन्देशः_add->topic);
	mosquitto_free(सन्देशः_add->payload);
	mosquitto_free(base_सन्देशः);

	पटिनिवत्त MOSQ_ERR_NOMEM;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_base_सन्देशः_delete(uint64_t store_id)
{
	संरचना mosquitto_base_सन्देशः *base_सन्देशः;

	base_सन्देशः = find_store_सन्देशः(store_id);
	सचे(base_सन्देशः && base_सन्देशः->ref_गणनम् == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_सन्देशः_store_remove(base_सन्देशः, false);
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT शून्य mosquitto_complete_basic_auth(ध्रुव वर्ण *clientid, पूर्णाङ्क result)
{
	संरचना mosquitto *context;

	सचे(clientid == रिक्त){
		पटिनिवत्त;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, strlen(clientid), context);
	सचे(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		सचे(result == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, रिक्त, 0);
		}अञ्ञथा{
			सचे(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, रिक्त);
			}अञ्ञथा{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, रिक्त);
			}
			context->clean_start = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_broker_ग्रन्थिः_id_set(uint16_t id)
{
	सचे(id > 1023){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}अञ्ञथा{
		db.ग्रन्थिः_id = id;
		db.ग्रन्थिः_id_shifted = ((uint64_t)id) << 54;
		पटिनिवत्त MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT ध्रुव वर्ण *mosquitto_persistence_location(शून्य)
{
	पटिनिवत्त db.config->persistence_location;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_will_set(ध्रुव वर्ण *clientid, ध्रुव वर्ण *topic, पूर्णाङ्क payloadlen, ध्रुव शून्य *payload, पूर्णाङ्क qos, bool retain, mosquitto_property *properties)
{
	संरचना mosquitto *mosq = mosquitto_client(clientid);
	सचे(!mosq){
		पटिनिवत्त MOSQ_ERR_NOT_FOUND;
	}
	सचे(properties && mosq->protocol != mosq_p_mqtt5){
		सचे(net__is_connected(mosq)){
			पटिनिवत्त MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	पटिनिवत्त will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
