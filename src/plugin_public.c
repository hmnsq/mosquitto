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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "send_mosq.h"
#അന്തര്ഭാവ "util_mosq.h"
#അന്തര്ഭാവ "will_mosq.h"
#അന്തര്ഭാവ "utlist.h"
#അന്തര്ഭാവ "will_mosq.h"

#ifdef WITH_TLS
#  include <openssl/ssl.h>
#endif


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_plugin_set_info(mosquitto_plugin_id_t *identifier,
		സ്ഥിരം അക്ഷരം *plugin_name,
		സ്ഥിരം അക്ഷരം *plugin_version)
{
	എങ്കിൽ(identifier == ശൂന്യ || plugin_name == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	എങ്കിൽ(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}അല്ലെങ്കിൽ{
		identifier->plugin_version = ശൂന്യ;
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT സ്ഥിരം അക്ഷരം *mosquitto_client_address(സ്ഥിരം ഘടന mosquitto *client)
{
	എങ്കിൽ(client){
		തിരികെ client->address;
	}അല്ലെങ്കിൽ{
		തിരികെ ശൂന്യ;
	}
}


BROKER_EXPORT ഘടന mosquitto *mosquitto_client(സ്ഥിരം അക്ഷരം *clientid)
{
	size_t len;
	ഘടന mosquitto *context;

	എങ്കിൽ(!clientid){
		തിരികെ ശൂന്യ;
	}
	len = strlen(clientid);
	എങ്കിൽ(len == 0){
		തിരികെ ശൂന്യ;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	തിരികെ context;
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_client_port(സ്ഥിരം ഘടന mosquitto *client)
{
	എങ്കിൽ(client && client->listener){
		തിരികെ client->listener->port;
	}അല്ലെങ്കിൽ{
		തിരികെ 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(സ്ഥിരം ഘടന mosquitto *client)
{
	എങ്കിൽ(client){
		തിരികെ client->clean_start;
	}അല്ലെങ്കിൽ{
		തിരികെ true;
	}
}


BROKER_EXPORT സ്ഥിരം അക്ഷരം *mosquitto_client_id(സ്ഥിരം ഘടന mosquitto *client)
{
	എങ്കിൽ(client){
		തിരികെ client->id;
	}അല്ലെങ്കിൽ{
		തിരികെ ശൂന്യ;
	}
}


BROKER_EXPORT unsigned mosquitto_client_id_hashv(സ്ഥിരം ഘടന mosquitto *client)
{
	എങ്കിൽ(client){
		തിരികെ client->id_hashv;
	}അല്ലെങ്കിൽ{
		തിരികെ 0;
	}
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_client_keepalive(സ്ഥിരം ഘടന mosquitto *client)
{
	എങ്കിൽ(client){
		തിരികെ client->keepalive;
	}അല്ലെങ്കിൽ{
		തിരികെ -1;
	}
}


BROKER_EXPORT ശൂന്യം *mosquitto_client_certificate(സ്ഥിരം ഘടന mosquitto *client)
{
#ifdef WITH_TLS
	എങ്കിൽ(client && client->ssl){
		തിരികെ SSL_get_peer_certificate(client->ssl);
	}അല്ലെങ്കിൽ{
		തിരികെ ശൂന്യ;
	}
#അല്ലെങ്കിൽ
	UNUSED(client);

	തിരികെ ശൂന്യ;
#endif
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_client_protocol(സ്ഥിരം ഘടന mosquitto *client)
{
#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	എങ്കിൽ(client && client->wsi){
		തിരികെ mp_websockets;
	}അല്ലെങ്കിൽ
#അല്ലെങ്കിലെങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	എങ്കിൽ(client && client->transport == mosq_t_ws){
		തിരികെ mp_websockets;
	}അല്ലെങ്കിൽ
#അല്ലെങ്കിൽ
	UNUSED(client);
#endif
	{
		തിരികെ mp_mqtt;
	}
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_client_protocol_version(സ്ഥിരം ഘടന mosquitto *client)
{
	എങ്കിൽ(client){
		തിരഞ്ഞെടുപ്പ്(client->protocol){
			അവസ്ഥ mosq_p_mqtt31:
				തിരികെ 3;
			അവസ്ഥ mosq_p_mqtt311:
				തിരികെ 4;
			അവസ്ഥ mosq_p_mqtt5:
				തിരികെ 5;
			സ്ഥിരസ്ഥിതി:
				തിരികെ 0;
		}
	}അല്ലെങ്കിൽ{
		തിരികെ 0;
	}
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_client_sub_എണ്ണം(സ്ഥിരം ഘടന mosquitto *client)
{
	എങ്കിൽ(client){
		തിരികെ client->subs_എണ്ണം;
	}അല്ലെങ്കിൽ{
		തിരികെ 0;
	}
}


BROKER_EXPORT സ്ഥിരം അക്ഷരം *mosquitto_client_username(സ്ഥിരം ഘടന mosquitto *client)
{
	എങ്കിൽ(client){
#ifdef WITH_BRIDGE
		എങ്കിൽ(client->bridge){
			തിരികെ client->bridge->local_username;
		}അല്ലെങ്കിൽ
#endif
		{
			തിരികെ client->username;
		}
	}അല്ലെങ്കിൽ{
		തിരികെ ശൂന്യ;
	}
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_broker_publish(
		സ്ഥിരം അക്ഷരം *clientid,
		സ്ഥിരം അക്ഷരം *topic,
		പൂർണ്ണസംഖ്യ payloadlen,
		ശൂന്യം *payload,
		പൂർണ്ണസംഖ്യ qos,
		bool retain,
		mosquitto_property *properties)
{
	ഘടന mosquitto__message_v5 *സന്ദേശം;

	എങ്കിൽ(topic == ശൂന്യ
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == ശൂന്യ)
			|| qos < 0 || qos > 2){

		തിരികെ MOSQ_ERR_INVAL;
	}

	സന്ദേശം = mosquitto_malloc(വലുപ്പം(ഘടന mosquitto__message_v5));
	എങ്കിൽ(സന്ദേശം == ശൂന്യ){
		തിരികെ MOSQ_ERR_NOMEM;
	}

	സന്ദേശം->next = ശൂന്യ;
	സന്ദേശം->prev = ശൂന്യ;
	എങ്കിൽ(clientid){
		സന്ദേശം->clientid = mosquitto_strdup(clientid);
		എങ്കിൽ(സന്ദേശം->clientid == ശൂന്യ){
			mosquitto_FREE(സന്ദേശം);
			തിരികെ MOSQ_ERR_NOMEM;
		}
	}അല്ലെങ്കിൽ{
		സന്ദേശം->clientid = ശൂന്യ;
	}
	സന്ദേശം->topic = mosquitto_strdup(topic);
	എങ്കിൽ(സന്ദേശം->topic == ശൂന്യ){
		mosquitto_FREE(സന്ദേശം->clientid);
		mosquitto_FREE(സന്ദേശം);
		തിരികെ MOSQ_ERR_NOMEM;
	}
	സന്ദേശം->payloadlen = payloadlen;
	സന്ദേശം->payload = payload;
	സന്ദേശം->qos = qos;
	സന്ദേശം->retain = retain;
	സന്ദേശം->properties = properties;

	DL_APPEND(db.plugin_msgs, സന്ദേശം);

	loop__update_next_event(1);
	തിരികെ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_broker_publish_copy(
		സ്ഥിരം അക്ഷരം *clientid,
		സ്ഥിരം അക്ഷരം *topic,
		പൂർണ്ണസംഖ്യ payloadlen,
		സ്ഥിരം ശൂന്യം *payload,
		പൂർണ്ണസംഖ്യ qos,
		bool retain,
		mosquitto_property *properties)
{
	ശൂന്യം *payload_out;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(topic == ശൂന്യ
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == ശൂന്യ)
			|| qos < 0 || qos > 2){

		തിരികെ MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (size_t)(payloadlen+1));
	എങ്കിൽ(payload_out == ശൂന്യ){
		തിരികെ MOSQ_ERR_NOMEM;
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

	എങ്കിൽ(rc){
		mosquitto_FREE(payload_out);
	}
	തിരികെ rc;
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_set_username(ഘടന mosquitto *client, സ്ഥിരം അക്ഷരം *username)
{
	അക്ഷരം *u_dup;
	അക്ഷരം *old;

	എങ്കിൽ(!client){
		തിരികെ MOSQ_ERR_INVAL;
	}

	എങ്കിൽ(username){
		എങ്കിൽ(mosquitto_validate_utf8(username, (പൂർണ്ണസംഖ്യ)strlen(username))){
			തിരികെ MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		എങ്കിൽ(!u_dup){
			തിരികെ MOSQ_ERR_NOMEM;
		}
	}അല്ലെങ്കിൽ{
		u_dup = ശൂന്യ;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	തിരികെ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_set_clientid(ഘടന mosquitto *client, സ്ഥിരം അക്ഷരം *clientid)
{
	ഘടന mosquitto *found_client;
	അക്ഷരം *id_dup;
	bool in_by_id;
	പൂർണ്ണസംഖ്യ clientid_len;

	എങ്കിൽ(!client || !clientid){
		തിരികെ MOSQ_ERR_INVAL;
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

	എങ്കിൽ(in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), found_client);
		എങ്കിൽ(found_client){
			തിരികെ MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (പൂർണ്ണസംഖ്യ)strlen(clientid);
	എങ്കിൽ(mosquitto_validate_utf8(clientid, clientid_len)){
		തിരികെ MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	എങ്കിൽ(!id_dup){
		തിരികെ MOSQ_ERR_NOMEM;
	}

	എങ്കിൽ(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	എങ്കിൽ(in_by_id){
		context__add_to_by_id(client);
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
സ്ഥിതം ശൂന്യം check_subscription_acls(ഘടന mosquitto *context)
{
	പൂർണ്ണസംഖ്യ rc;
	uint8_t reason;

	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<context->subs_capacity; i++){
		എങ്കിൽ(context->subs[i] == ശൂന്യ){
			തുടരുക;
		}
		rc = mosquitto_acl_check(context,
				context->subs[i]->topic_filter,
				0,
				ശൂന്യ,
				0, /* FIXME */
				false,
				ശൂന്യ,
				MOSQ_ACL_SUBSCRIBE);

		എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


സ്ഥിതം ശൂന്യം disconnect_client(ഘടന mosquitto *context, bool with_will)
{
	എങ്കിൽ(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ശൂന്യ);
	}
	എങ്കിൽ(with_will == false){
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	എങ്കിൽ(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_kick_client_by_clientid(സ്ഥിരം അക്ഷരം *clientid, bool with_will)
{
	ഘടന mosquitto *ctxt, *ctxt_tmp;

	എങ്കിൽ(clientid == ശൂന്യ){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		തിരികെ MOSQ_ERR_SUCCESS;
	}അല്ലെങ്കിൽ{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), ctxt);
		എങ്കിൽ(ctxt){
			disconnect_client(ctxt, with_will);
			തിരികെ MOSQ_ERR_SUCCESS;
		}അല്ലെങ്കിൽ{
			തിരികെ MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_kick_client_by_username(സ്ഥിരം അക്ഷരം *username, bool with_will)
{
	ഘടന mosquitto *ctxt, *ctxt_tmp;

	എങ്കിൽ(username == ശൂന്യ){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			എങ്കിൽ(ctxt->username == ശൂന്യ){
				disconnect_client(ctxt, with_will);
			}
		}
	}അല്ലെങ്കിൽ{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			എങ്കിൽ(ctxt->username != ശൂന്യ && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_apply_on_all_clients(പൂർണ്ണസംഖ്യ (*FUNC_client_functor)(സ്ഥിരം ഘടന mosquitto *, ശൂന്യം *), ശൂന്യം *functor_context)
{
	പൂർണ്ണസംഖ്യ rc = MOSQ_ERR_SUCCESS;
	ഘടന mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
			നിർത്തുക;
		}
	}

	തിരികെ rc;
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_persist_client_add(ഘടന mosquitto_client *client)
{
	ഘടന mosquitto *context;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(client == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}
	എങ്കിൽ(client->clientid == ശൂന്യ){
		rc = MOSQ_ERR_INVAL;
		പോകുക error;
	}

	context = ശൂന്യ;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	എങ്കിൽ(context){
		rc = MOSQ_ERR_INVAL;
		പോകുക error;
	}

	context = context__init();
	എങ്കിൽ(!context){
		rc = MOSQ_ERR_NOMEM;
		പോകുക error;
	}

	context->id = client->clientid;
	client->clientid = ശൂന്യ;
	context->username = client->username;
	client->username = ശൂന്യ;
	context->auth_method = client->auth_method;
	client->auth_method = ശൂന്യ;

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
	എങ്കിൽ(db.config->per_listener_settings && client->listener_port > 0){
		വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i < db.config->listener_എണ്ണം; i++){
			എങ്കിൽ(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				നിർത്തുക;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	തിരികെ MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	തിരികെ rc;
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_persist_client_update(ഘടന mosquitto_client *client)
{
	ഘടന mosquitto *context;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(client == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}
	എങ്കിൽ(client->clientid == ശൂന്യ){
		rc = MOSQ_ERR_INVAL;
		പോകുക error;
	}

	context = ശൂന്യ;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	എങ്കിൽ(context == ശൂന്യ){
		rc = MOSQ_ERR_NOT_FOUND;
		പോകുക error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = ശൂന്യ;

	context->clean_start = false;
	context->will_delay_time = client->will_delay_time;
	context->session_expiry_time = client->session_expiry_time;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->maximum_packet_size = client->max_packet_size;
	context->retain_available = client->retain_available;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	എങ്കിൽ(db.config->per_listener_settings && client->listener_port > 0){
		വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i < db.config->listener_എണ്ണം; i++){
			എങ്കിൽ(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				നിർത്തുക;
			}
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	തിരികെ rc;
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_persist_client_delete(സ്ഥിരം അക്ഷരം *clientid)
{
	ഘടന mosquitto *context;

	എങ്കിൽ(clientid == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}

	context = ശൂന്യ;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	എങ്കിൽ(context == ശൂന്യ){
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം ഘടന mosquitto_base_സന്ദേശം *find_store_സന്ദേശം(uint64_t store_id)
{
	ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം;

	HASH_FIND(hh, db.സന്ദേശം_store, &store_id, വലുപ്പം(store_id), base_സന്ദേശം);
	തിരികെ base_സന്ദേശം;
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_persist_client_സന്ദേശം_add(ഘടന mosquitto_client_സന്ദേശം_2 *client_സന്ദേശം)
{
	ഘടന mosquitto *context;
	ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം;

	എങ്കിൽ(client_സന്ദേശം == ശൂന്യ || client_സന്ദേശം->clientid == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_സന്ദേശം->clientid, strlen(client_സന്ദേശം->clientid), context);
	എങ്കിൽ(context == ശൂന്യ){
		തിരികെ MOSQ_ERR_NOT_FOUND;
	}
	base_സന്ദേശം = find_store_സന്ദേശം(client_സന്ദേശം->store_id);
	എങ്കിൽ(base_സന്ദേശം == ശൂന്യ){
		തിരികെ MOSQ_ERR_NOT_FOUND;
	}

	എങ്കിൽ(client_സന്ദേശം->direction == mosq_md_out){
		എങ്കിൽ(client_സന്ദേശം->qos > 0){
			context->last_mid = client_സന്ദേശം->mid;
		}
		തിരികെ db_message_തിരുകുക_outgoing(context, client_സന്ദേശം->cmsg_id, client_സന്ദേശം->mid,
				client_സന്ദേശം->qos, client_സന്ദേശം->retain,
				base_സന്ദേശം, client_സന്ദേശം->subscription_identifier, false, false);
	}അല്ലെങ്കിൽ എങ്കിൽ(client_സന്ദേശം->direction == mosq_md_in){
		തിരികെ db_message_തിരുകുക_incoming(context, client_സന്ദേശം->cmsg_id, base_സന്ദേശം, false);
	}അല്ലെങ്കിൽ{
		തിരികെ MOSQ_ERR_INVAL;
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_persist_client_സന്ദേശം_delete(ഘടന mosquitto_client_സന്ദേശം_2 *client_സന്ദേശം)
{
	ഘടന mosquitto *context;

	എങ്കിൽ(client_സന്ദേശം == ശൂന്യ || client_സന്ദേശം->clientid == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_സന്ദേശം->clientid, strlen(client_സന്ദേശം->clientid), context);
	എങ്കിൽ(context == ശൂന്യ){
		തിരികെ MOSQ_ERR_NOT_FOUND;
	}


	പൂർണ്ണസംഖ്യ rc = MOSQ_ERR_INVAL;
	എങ്കിൽ(client_സന്ദേശം->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_സന്ദേശം->mid, mosq_ms_any, client_സന്ദേശം->qos);
	}അല്ലെങ്കിൽ എങ്കിൽ(client_സന്ദേശം->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_സന്ദേശം->mid);
	}
	തിരികെ rc;
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_persist_client_സന്ദേശം_update(ഘടന mosquitto_client_സന്ദേശം_2 *client_സന്ദേശം)
{
	ഘടന mosquitto *context;

	എങ്കിൽ(client_സന്ദേശം == ശൂന്യ || client_സന്ദേശം->clientid == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_സന്ദേശം->clientid, strlen(client_സന്ദേശം->clientid), context);
	എങ്കിൽ(context == ശൂന്യ){
		തിരികെ MOSQ_ERR_NOT_FOUND;
	}

	എങ്കിൽ(client_സന്ദേശം->direction == mosq_md_out){
		db__message_update_outgoing(context, client_സന്ദേശം->mid, client_സന്ദേശം->state, client_സന്ദേശം->qos, false);
	}അല്ലെങ്കിൽ എങ്കിൽ(client_സന്ദേശം->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}അല്ലെങ്കിൽ{
		തിരികെ MOSQ_ERR_INVAL;
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_persist_client_സന്ദേശം_clear(ഘടന mosquitto_client_സന്ദേശം_2 *client_സന്ദേശം)
{
	ഘടന mosquitto *context;

	എങ്കിൽ(client_സന്ദേശം == ശൂന്യ || client_സന്ദേശം->clientid == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_സന്ദേശം->clientid, strlen(client_സന്ദേശം->clientid), context);
	എങ്കിൽ(context == ശൂന്യ){
		തിരികെ MOSQ_ERR_NOT_FOUND;
	}

	എങ്കിൽ(client_സന്ദേശം->direction == mosq_bmd_in || client_സന്ദേശം->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	എങ്കിൽ(client_സന്ദേശം->direction == mosq_bmd_out || client_സന്ദേശം->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_subscription_add(സ്ഥിരം ഘടന mosquitto_subscription *sub)
{
	ഘടന mosquitto *context;

	എങ്കിൽ(sub == ശൂന്യ || sub->clientid == ശൂന്യ || sub->topic_filter == ശൂന്യ || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		തിരികെ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, strlen(sub->clientid), context);

	എങ്കിൽ(context){
		തിരികെ sub__add(context, sub);
	}അല്ലെങ്കിൽ{
		തിരികെ MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_subscription_delete(സ്ഥിരം അക്ഷരം *clientid, സ്ഥിരം അക്ഷരം *topic)
{
	ഘടന mosquitto *context;
	uint8_t reason;

	എങ്കിൽ(clientid == ശൂന്യ || topic == ശൂന്യ || clientid[0] == '\0' || topic[0] == '\0'){
		തിരികെ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	എങ്കിൽ(context){
		തിരികെ sub__remove(context, topic, &reason);
	}അല്ലെങ്കിൽ{
		തിരികെ MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_persist_base_സന്ദേശം_add(ഘടന mosquitto_base_സന്ദേശം_2 *സന്ദേശം_add)
{
	ഘടന mosquitto context;
	ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം;
	പൂർണ്ണസംഖ്യ rc;

	memset(&context, 0, വലുപ്പം(context));

	എങ്കിൽ(സന്ദേശം_add->payloadlen > MQTT_MAX_PAYLOAD){
		തിരികെ MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (അക്ഷരം *)സന്ദേശം_add->source_id;
	context.username = (അക്ഷരം *)സന്ദേശം_add->source_username;

	base_സന്ദേശം = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto_base_സന്ദേശം));
	എങ്കിൽ(base_സന്ദേശം == ശൂന്യ){
		പോകുക error;
	}
	base_സന്ദേശം->data.store_id = സന്ദേശം_add->store_id;
	base_സന്ദേശം->data.expiry_time = സന്ദേശം_add->expiry_time;
	base_സന്ദേശം->data.payloadlen = സന്ദേശം_add->payloadlen;
	base_സന്ദേശം->data.source_mid = സന്ദേശം_add->source_mid;
	base_സന്ദേശം->data.qos = സന്ദേശം_add->qos;
	base_സന്ദേശം->data.retain = സന്ദേശം_add->retain;

	base_സന്ദേശം->data.payload = സന്ദേശം_add->payload;
	സന്ദേശം_add->payload = ശൂന്യ;
	base_സന്ദേശം->data.topic = സന്ദേശം_add->topic;
	സന്ദേശം_add->topic = ശൂന്യ;
	base_സന്ദേശം->data.properties = സന്ദേശം_add->properties;
	സന്ദേശം_add->properties = ശൂന്യ;

	എങ്കിൽ(സന്ദേശം_add->source_port){
		വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<db.config->listener_എണ്ണം; i++){
			എങ്കിൽ(db.config->listeners[i].port == സന്ദേശം_add->source_port){
				base_സന്ദേശം->source_listener = &db.config->listeners[i];
				നിർത്തുക;
			}
		}
	}

	base_സന്ദേശം->stored = true;
	rc = db__message_store(&context, base_സന്ദേശം, ശൂന്യ, mosq_mo_broker);
	തിരികെ rc;

error:
	mosquitto_property_free_all(&സന്ദേശം_add->properties);
	mosquitto_free(സന്ദേശം_add->topic);
	mosquitto_free(സന്ദേശം_add->payload);
	mosquitto_free(base_സന്ദേശം);

	തിരികെ MOSQ_ERR_NOMEM;
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_persist_base_സന്ദേശം_delete(uint64_t store_id)
{
	ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം;

	base_സന്ദേശം = find_store_സന്ദേശം(store_id);
	എങ്കിൽ(base_സന്ദേശം && base_സന്ദേശം->ref_എണ്ണം == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_സന്ദേശം_store_remove(base_സന്ദേശം, false);
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ശൂന്യം mosquitto_complete_basic_auth(സ്ഥിരം അക്ഷരം *clientid, പൂർണ്ണസംഖ്യ result)
{
	ഘടന mosquitto *context;

	എങ്കിൽ(clientid == ശൂന്യ){
		തിരികെ;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, strlen(clientid), context);
	എങ്കിൽ(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		എങ്കിൽ(result == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, ശൂന്യ, 0);
		}അല്ലെങ്കിൽ{
			എങ്കിൽ(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ശൂന്യ);
			}അല്ലെങ്കിൽ{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ശൂന്യ);
			}
			context->clean_start = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_broker_കണ്ണി_id_set(uint16_t id)
{
	എങ്കിൽ(id > 1023){
		തിരികെ MOSQ_ERR_INVAL;
	}അല്ലെങ്കിൽ{
		db.കണ്ണി_id = id;
		db.കണ്ണി_id_shifted = ((uint64_t)id) << 54;
		തിരികെ MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT സ്ഥിരം അക്ഷരം *mosquitto_persistence_location(ശൂന്യം)
{
	തിരികെ db.config->persistence_location;
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_client_will_set(സ്ഥിരം അക്ഷരം *clientid, സ്ഥിരം അക്ഷരം *topic, പൂർണ്ണസംഖ്യ payloadlen, സ്ഥിരം ശൂന്യം *payload, പൂർണ്ണസംഖ്യ qos, bool retain, mosquitto_property *properties)
{
	ഘടന mosquitto *mosq = mosquitto_client(clientid);
	എങ്കിൽ(!mosq){
		തിരികെ MOSQ_ERR_NOT_FOUND;
	}
	എങ്കിൽ(properties && mosq->protocol != mosq_p_mqtt5){
		എങ്കിൽ(net__is_connected(mosq)){
			തിരികെ MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	തിരികെ will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
