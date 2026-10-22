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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "send_mosq.h"
#அந்தர்பாவ "util_mosq.h"
#அந்தர்பாவ "will_mosq.h"
#அந்தர்பாவ "utlist.h"
#அந்தர்பாவ "will_mosq.h"

#ifdef WITH_TLS
#  include <openssl/ssl.h>
#endif


BROKER_EXPORT முழு mosquitto_plugin_set_info(mosquitto_plugin_id_t *identifier,
		மாறிலி எழுத்து *plugin_name,
		மாறிலி எழுத்து *plugin_version)
{
	எனில்(identifier == வெற்று || plugin_name == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	எனில்(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}இல்லையெனில்{
		identifier->plugin_version = வெற்று;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT மாறிலி எழுத்து *mosquitto_client_address(மாறிலி கட்டமைப்பு mosquitto *client)
{
	எனில்(client){
		திரும்பு client->address;
	}இல்லையெனில்{
		திரும்பு வெற்று;
	}
}


BROKER_EXPORT கட்டமைப்பு mosquitto *mosquitto_client(மாறிலி எழுத்து *clientid)
{
	size_t len;
	கட்டமைப்பு mosquitto *context;

	எனில்(!clientid){
		திரும்பு வெற்று;
	}
	len = strlen(clientid);
	எனில்(len == 0){
		திரும்பு வெற்று;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	திரும்பு context;
}


BROKER_EXPORT முழு mosquitto_client_port(மாறிலி கட்டமைப்பு mosquitto *client)
{
	எனில்(client && client->listener){
		திரும்பு client->listener->port;
	}இல்லையெனில்{
		திரும்பு 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(மாறிலி கட்டமைப்பு mosquitto *client)
{
	எனில்(client){
		திரும்பு client->clean_start;
	}இல்லையெனில்{
		திரும்பு true;
	}
}


BROKER_EXPORT மாறிலி எழுத்து *mosquitto_client_id(மாறிலி கட்டமைப்பு mosquitto *client)
{
	எனில்(client){
		திரும்பு client->id;
	}இல்லையெனில்{
		திரும்பு வெற்று;
	}
}


BROKER_EXPORT unsigned mosquitto_client_id_hashv(மாறிலி கட்டமைப்பு mosquitto *client)
{
	எனில்(client){
		திரும்பு client->id_hashv;
	}இல்லையெனில்{
		திரும்பு 0;
	}
}


BROKER_EXPORT முழு mosquitto_client_keepalive(மாறிலி கட்டமைப்பு mosquitto *client)
{
	எனில்(client){
		திரும்பு client->keepalive;
	}இல்லையெனில்{
		திரும்பு -1;
	}
}


BROKER_EXPORT வெறுமை *mosquitto_client_certificate(மாறிலி கட்டமைப்பு mosquitto *client)
{
#ifdef WITH_TLS
	எனில்(client && client->ssl){
		திரும்பு SSL_get_peer_certificate(client->ssl);
	}இல்லையெனில்{
		திரும்பு வெற்று;
	}
#இல்லையெனில்
	UNUSED(client);

	திரும்பு வெற்று;
#endif
}


BROKER_EXPORT முழு mosquitto_client_protocol(மாறிலி கட்டமைப்பு mosquitto *client)
{
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	எனில்(client && client->wsi){
		திரும்பு mp_websockets;
	}இல்லையெனில்
#அல்லதெனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	எனில்(client && client->transport == mosq_t_ws){
		திரும்பு mp_websockets;
	}இல்லையெனில்
#இல்லையெனில்
	UNUSED(client);
#endif
	{
		திரும்பு mp_mqtt;
	}
}


BROKER_EXPORT முழு mosquitto_client_protocol_version(மாறிலி கட்டமைப்பு mosquitto *client)
{
	எனில்(client){
		தேர்வு(client->protocol){
			நிலை mosq_p_mqtt31:
				திரும்பு 3;
			நிலை mosq_p_mqtt311:
				திரும்பு 4;
			நிலை mosq_p_mqtt5:
				திரும்பு 5;
			இயல்பு:
				திரும்பு 0;
		}
	}இல்லையெனில்{
		திரும்பு 0;
	}
}


BROKER_EXPORT முழு mosquitto_client_sub_மொத்தம்(மாறிலி கட்டமைப்பு mosquitto *client)
{
	எனில்(client){
		திரும்பு client->subs_மொத்தம்;
	}இல்லையெனில்{
		திரும்பு 0;
	}
}


BROKER_EXPORT மாறிலி எழுத்து *mosquitto_client_username(மாறிலி கட்டமைப்பு mosquitto *client)
{
	எனில்(client){
#ifdef WITH_BRIDGE
		எனில்(client->bridge){
			திரும்பு client->bridge->local_username;
		}இல்லையெனில்
#endif
		{
			திரும்பு client->username;
		}
	}இல்லையெனில்{
		திரும்பு வெற்று;
	}
}


BROKER_EXPORT முழு mosquitto_broker_publish(
		மாறிலி எழுத்து *clientid,
		மாறிலி எழுத்து *topic,
		முழு payloadlen,
		வெறுமை *payload,
		முழு qos,
		bool retain,
		mosquitto_property *properties)
{
	கட்டமைப்பு mosquitto__message_v5 *செய்தி;

	எனில்(topic == வெற்று
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == வெற்று)
			|| qos < 0 || qos > 2){

		திரும்பு MOSQ_ERR_INVAL;
	}

	செய்தி = mosquitto_malloc(அளவு(கட்டமைப்பு mosquitto__message_v5));
	எனில்(செய்தி == வெற்று){
		திரும்பு MOSQ_ERR_NOMEM;
	}

	செய்தி->next = வெற்று;
	செய்தி->prev = வெற்று;
	எனில்(clientid){
		செய்தி->clientid = mosquitto_strdup(clientid);
		எனில்(செய்தி->clientid == வெற்று){
			mosquitto_FREE(செய்தி);
			திரும்பு MOSQ_ERR_NOMEM;
		}
	}இல்லையெனில்{
		செய்தி->clientid = வெற்று;
	}
	செய்தி->topic = mosquitto_strdup(topic);
	எனில்(செய்தி->topic == வெற்று){
		mosquitto_FREE(செய்தி->clientid);
		mosquitto_FREE(செய்தி);
		திரும்பு MOSQ_ERR_NOMEM;
	}
	செய்தி->payloadlen = payloadlen;
	செய்தி->payload = payload;
	செய்தி->qos = qos;
	செய்தி->retain = retain;
	செய்தி->properties = properties;

	DL_APPEND(db.plugin_msgs, செய்தி);

	loop__update_next_event(1);
	திரும்பு MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT முழு mosquitto_broker_publish_copy(
		மாறிலி எழுத்து *clientid,
		மாறிலி எழுத்து *topic,
		முழு payloadlen,
		மாறிலி வெறுமை *payload,
		முழு qos,
		bool retain,
		mosquitto_property *properties)
{
	வெறுமை *payload_out;
	முழு rc;

	எனில்(topic == வெற்று
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == வெற்று)
			|| qos < 0 || qos > 2){

		திரும்பு MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (size_t)(payloadlen+1));
	எனில்(payload_out == வெற்று){
		திரும்பு MOSQ_ERR_NOMEM;
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

	எனில்(rc){
		mosquitto_FREE(payload_out);
	}
	திரும்பு rc;
}


BROKER_EXPORT முழு mosquitto_set_username(கட்டமைப்பு mosquitto *client, மாறிலி எழுத்து *username)
{
	எழுத்து *u_dup;
	எழுத்து *old;

	எனில்(!client){
		திரும்பு MOSQ_ERR_INVAL;
	}

	எனில்(username){
		எனில்(mosquitto_validate_utf8(username, (முழு)strlen(username))){
			திரும்பு MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		எனில்(!u_dup){
			திரும்பு MOSQ_ERR_NOMEM;
		}
	}இல்லையெனில்{
		u_dup = வெற்று;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	திரும்பு MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT முழு mosquitto_set_clientid(கட்டமைப்பு mosquitto *client, மாறிலி எழுத்து *clientid)
{
	கட்டமைப்பு mosquitto *found_client;
	எழுத்து *id_dup;
	bool in_by_id;
	முழு clientid_len;

	எனில்(!client || !clientid){
		திரும்பு MOSQ_ERR_INVAL;
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

	எனில்(in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), found_client);
		எனில்(found_client){
			திரும்பு MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (முழு)strlen(clientid);
	எனில்(mosquitto_validate_utf8(clientid, clientid_len)){
		திரும்பு MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	எனில்(!id_dup){
		திரும்பு MOSQ_ERR_NOMEM;
	}

	எனில்(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	எனில்(in_by_id){
		context__add_to_by_id(client);
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
நிலையான வெறுமை check_subscription_acls(கட்டமைப்பு mosquitto *context)
{
	முழு rc;
	uint8_t reason;

	க்கு(முழு i=0; i<context->subs_capacity; i++){
		எனில்(context->subs[i] == வெற்று){
			தொடர்;
		}
		rc = mosquitto_acl_check(context,
				context->subs[i]->topic_filter,
				0,
				வெற்று,
				0, /* FIXME */
				false,
				வெற்று,
				MOSQ_ACL_SUBSCRIBE);

		எனில்(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


நிலையான வெறுமை disconnect_client(கட்டமைப்பு mosquitto *context, bool with_will)
{
	எனில்(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, வெற்று);
	}
	எனில்(with_will == false){
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	எனில்(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT முழு mosquitto_kick_client_by_clientid(மாறிலி எழுத்து *clientid, bool with_will)
{
	கட்டமைப்பு mosquitto *ctxt, *ctxt_tmp;

	எனில்(clientid == வெற்று){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		திரும்பு MOSQ_ERR_SUCCESS;
	}இல்லையெனில்{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), ctxt);
		எனில்(ctxt){
			disconnect_client(ctxt, with_will);
			திரும்பு MOSQ_ERR_SUCCESS;
		}இல்லையெனில்{
			திரும்பு MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT முழு mosquitto_kick_client_by_username(மாறிலி எழுத்து *username, bool with_will)
{
	கட்டமைப்பு mosquitto *ctxt, *ctxt_tmp;

	எனில்(username == வெற்று){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			எனில்(ctxt->username == வெற்று){
				disconnect_client(ctxt, with_will);
			}
		}
	}இல்லையெனில்{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			எனில்(ctxt->username != வெற்று && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT முழு mosquitto_apply_on_all_clients(முழு (*FUNC_client_functor)(மாறிலி கட்டமைப்பு mosquitto *, வெறுமை *), வெறுமை *functor_context)
{
	முழு rc = MOSQ_ERR_SUCCESS;
	கட்டமைப்பு mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		எனில்(rc != MOSQ_ERR_SUCCESS){
			நிறுத்து;
		}
	}

	திரும்பு rc;
}


BROKER_EXPORT முழு mosquitto_persist_client_add(கட்டமைப்பு mosquitto_client *client)
{
	கட்டமைப்பு mosquitto *context;
	முழு rc;

	எனில்(client == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}
	எனில்(client->clientid == வெற்று){
		rc = MOSQ_ERR_INVAL;
		செல் error;
	}

	context = வெற்று;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	எனில்(context){
		rc = MOSQ_ERR_INVAL;
		செல் error;
	}

	context = context__init();
	எனில்(!context){
		rc = MOSQ_ERR_NOMEM;
		செல் error;
	}

	context->id = client->clientid;
	client->clientid = வெற்று;
	context->username = client->username;
	client->username = வெற்று;
	context->auth_method = client->auth_method;
	client->auth_method = வெற்று;

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
	எனில்(db.config->per_listener_settings && client->listener_port > 0){
		க்கு(முழு i=0; i < db.config->listener_மொத்தம்; i++){
			எனில்(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				நிறுத்து;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	திரும்பு MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	திரும்பு rc;
}


BROKER_EXPORT முழு mosquitto_persist_client_update(கட்டமைப்பு mosquitto_client *client)
{
	கட்டமைப்பு mosquitto *context;
	முழு rc;

	எனில்(client == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}
	எனில்(client->clientid == வெற்று){
		rc = MOSQ_ERR_INVAL;
		செல் error;
	}

	context = வெற்று;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	எனில்(context == வெற்று){
		rc = MOSQ_ERR_NOT_FOUND;
		செல் error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = வெற்று;

	context->clean_start = false;
	context->will_delay_time = client->will_delay_time;
	context->session_expiry_time = client->session_expiry_time;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->maximum_packet_size = client->max_packet_size;
	context->retain_available = client->retain_available;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	எனில்(db.config->per_listener_settings && client->listener_port > 0){
		க்கு(முழு i=0; i < db.config->listener_மொத்தம்; i++){
			எனில்(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				நிறுத்து;
			}
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	திரும்பு rc;
}


BROKER_EXPORT முழு mosquitto_persist_client_delete(மாறிலி எழுத்து *clientid)
{
	கட்டமைப்பு mosquitto *context;

	எனில்(clientid == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}

	context = வெற்று;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	எனில்(context == வெற்று){
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான கட்டமைப்பு mosquitto_base_செய்தி *find_store_செய்தி(uint64_t store_id)
{
	கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி;

	HASH_FIND(hh, db.செய்தி_store, &store_id, அளவு(store_id), base_செய்தி);
	திரும்பு base_செய்தி;
}


BROKER_EXPORT முழு mosquitto_persist_client_செய்தி_add(கட்டமைப்பு mosquitto_client_செய்தி_2 *client_செய்தி)
{
	கட்டமைப்பு mosquitto *context;
	கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி;

	எனில்(client_செய்தி == வெற்று || client_செய்தி->clientid == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_செய்தி->clientid, strlen(client_செய்தி->clientid), context);
	எனில்(context == வெற்று){
		திரும்பு MOSQ_ERR_NOT_FOUND;
	}
	base_செய்தி = find_store_செய்தி(client_செய்தி->store_id);
	எனில்(base_செய்தி == வெற்று){
		திரும்பு MOSQ_ERR_NOT_FOUND;
	}

	எனில்(client_செய்தி->direction == mosq_md_out){
		எனில்(client_செய்தி->qos > 0){
			context->last_mid = client_செய்தி->mid;
		}
		திரும்பு db_message_செருகு_outgoing(context, client_செய்தி->cmsg_id, client_செய்தி->mid,
				client_செய்தி->qos, client_செய்தி->retain,
				base_செய்தி, client_செய்தி->subscription_identifier, false, false);
	}இல்லையெனில் எனில்(client_செய்தி->direction == mosq_md_in){
		திரும்பு db_message_செருகு_incoming(context, client_செய்தி->cmsg_id, base_செய்தி, false);
	}இல்லையெனில்{
		திரும்பு MOSQ_ERR_INVAL;
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT முழு mosquitto_persist_client_செய்தி_delete(கட்டமைப்பு mosquitto_client_செய்தி_2 *client_செய்தி)
{
	கட்டமைப்பு mosquitto *context;

	எனில்(client_செய்தி == வெற்று || client_செய்தி->clientid == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_செய்தி->clientid, strlen(client_செய்தி->clientid), context);
	எனில்(context == வெற்று){
		திரும்பு MOSQ_ERR_NOT_FOUND;
	}


	முழு rc = MOSQ_ERR_INVAL;
	எனில்(client_செய்தி->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_செய்தி->mid, mosq_ms_any, client_செய்தி->qos);
	}இல்லையெனில் எனில்(client_செய்தி->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_செய்தி->mid);
	}
	திரும்பு rc;
}


BROKER_EXPORT முழு mosquitto_persist_client_செய்தி_update(கட்டமைப்பு mosquitto_client_செய்தி_2 *client_செய்தி)
{
	கட்டமைப்பு mosquitto *context;

	எனில்(client_செய்தி == வெற்று || client_செய்தி->clientid == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_செய்தி->clientid, strlen(client_செய்தி->clientid), context);
	எனில்(context == வெற்று){
		திரும்பு MOSQ_ERR_NOT_FOUND;
	}

	எனில்(client_செய்தி->direction == mosq_md_out){
		db__message_update_outgoing(context, client_செய்தி->mid, client_செய்தி->state, client_செய்தி->qos, false);
	}இல்லையெனில் எனில்(client_செய்தி->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}இல்லையெனில்{
		திரும்பு MOSQ_ERR_INVAL;
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT முழு mosquitto_persist_client_செய்தி_clear(கட்டமைப்பு mosquitto_client_செய்தி_2 *client_செய்தி)
{
	கட்டமைப்பு mosquitto *context;

	எனில்(client_செய்தி == வெற்று || client_செய்தி->clientid == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_செய்தி->clientid, strlen(client_செய்தி->clientid), context);
	எனில்(context == வெற்று){
		திரும்பு MOSQ_ERR_NOT_FOUND;
	}

	எனில்(client_செய்தி->direction == mosq_bmd_in || client_செய்தி->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	எனில்(client_செய்தி->direction == mosq_bmd_out || client_செய்தி->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT முழு mosquitto_subscription_add(மாறிலி கட்டமைப்பு mosquitto_subscription *sub)
{
	கட்டமைப்பு mosquitto *context;

	எனில்(sub == வெற்று || sub->clientid == வெற்று || sub->topic_filter == வெற்று || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		திரும்பு MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, strlen(sub->clientid), context);

	எனில்(context){
		திரும்பு sub__add(context, sub);
	}இல்லையெனில்{
		திரும்பு MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT முழு mosquitto_subscription_delete(மாறிலி எழுத்து *clientid, மாறிலி எழுத்து *topic)
{
	கட்டமைப்பு mosquitto *context;
	uint8_t reason;

	எனில்(clientid == வெற்று || topic == வெற்று || clientid[0] == '\0' || topic[0] == '\0'){
		திரும்பு MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	எனில்(context){
		திரும்பு sub__remove(context, topic, &reason);
	}இல்லையெனில்{
		திரும்பு MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT முழு mosquitto_persist_base_செய்தி_add(கட்டமைப்பு mosquitto_base_செய்தி_2 *செய்தி_add)
{
	கட்டமைப்பு mosquitto context;
	கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி;
	முழு rc;

	memset(&context, 0, அளவு(context));

	எனில்(செய்தி_add->payloadlen > MQTT_MAX_PAYLOAD){
		திரும்பு MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (எழுத்து *)செய்தி_add->source_id;
	context.username = (எழுத்து *)செய்தி_add->source_username;

	base_செய்தி = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto_base_செய்தி));
	எனில்(base_செய்தி == வெற்று){
		செல் error;
	}
	base_செய்தி->data.store_id = செய்தி_add->store_id;
	base_செய்தி->data.expiry_time = செய்தி_add->expiry_time;
	base_செய்தி->data.payloadlen = செய்தி_add->payloadlen;
	base_செய்தி->data.source_mid = செய்தி_add->source_mid;
	base_செய்தி->data.qos = செய்தி_add->qos;
	base_செய்தி->data.retain = செய்தி_add->retain;

	base_செய்தி->data.payload = செய்தி_add->payload;
	செய்தி_add->payload = வெற்று;
	base_செய்தி->data.topic = செய்தி_add->topic;
	செய்தி_add->topic = வெற்று;
	base_செய்தி->data.properties = செய்தி_add->properties;
	செய்தி_add->properties = வெற்று;

	எனில்(செய்தி_add->source_port){
		க்கு(முழு i=0; i<db.config->listener_மொத்தம்; i++){
			எனில்(db.config->listeners[i].port == செய்தி_add->source_port){
				base_செய்தி->source_listener = &db.config->listeners[i];
				நிறுத்து;
			}
		}
	}

	base_செய்தி->stored = true;
	rc = db__message_store(&context, base_செய்தி, வெற்று, mosq_mo_broker);
	திரும்பு rc;

error:
	mosquitto_property_free_all(&செய்தி_add->properties);
	mosquitto_free(செய்தி_add->topic);
	mosquitto_free(செய்தி_add->payload);
	mosquitto_free(base_செய்தி);

	திரும்பு MOSQ_ERR_NOMEM;
}


BROKER_EXPORT முழு mosquitto_persist_base_செய்தி_delete(uint64_t store_id)
{
	கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி;

	base_செய்தி = find_store_செய்தி(store_id);
	எனில்(base_செய்தி && base_செய்தி->ref_மொத்தம் == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_செய்தி_store_remove(base_செய்தி, false);
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT வெறுமை mosquitto_complete_basic_auth(மாறிலி எழுத்து *clientid, முழு result)
{
	கட்டமைப்பு mosquitto *context;

	எனில்(clientid == வெற்று){
		திரும்பு;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, strlen(clientid), context);
	எனில்(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		எனில்(result == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, வெற்று, 0);
		}இல்லையெனில்{
			எனில்(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, வெற்று);
			}இல்லையெனில்{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, வெற்று);
			}
			context->clean_start = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT முழு mosquitto_broker_முனை_id_set(uint16_t id)
{
	எனில்(id > 1023){
		திரும்பு MOSQ_ERR_INVAL;
	}இல்லையெனில்{
		db.முனை_id = id;
		db.முனை_id_shifted = ((uint64_t)id) << 54;
		திரும்பு MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT மாறிலி எழுத்து *mosquitto_persistence_location(வெறுமை)
{
	திரும்பு db.config->persistence_location;
}


BROKER_EXPORT முழு mosquitto_client_will_set(மாறிலி எழுத்து *clientid, மாறிலி எழுத்து *topic, முழு payloadlen, மாறிலி வெறுமை *payload, முழு qos, bool retain, mosquitto_property *properties)
{
	கட்டமைப்பு mosquitto *mosq = mosquitto_client(clientid);
	எனில்(!mosq){
		திரும்பு MOSQ_ERR_NOT_FOUND;
	}
	எனில்(properties && mosq->protocol != mosq_p_mqtt5){
		எனில்(net__is_connected(mosq)){
			திரும்பு MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	திரும்பு will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
