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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "send_mosq.h"
#অন্তর্ভাব "util_mosq.h"
#অন্তর্ভাব "will_mosq.h"
#অন্তর্ভাব "utlist.h"
#অন্তর্ভাব "will_mosq.h"

#ifdef WITH_TLS
#  include <openssl/ssl.h>
#endif


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_plugin_set_info(mosquitto_plugin_id_t *identifier,
		ধ্রুব বর্ণ *plugin_name,
		ধ্রুব বর্ণ *plugin_version)
{
	যদি(identifier == রিক্ত || plugin_name == রিক্ত){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	যদি(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}অন্যথা{
		identifier->plugin_version = রিক্ত;
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ধ্রুব বর্ণ *mosquitto_client_address(ধ্রুব সংরচনা mosquitto *client)
{
	যদি(client){
		প্রত্যাগম client->address;
	}অন্যথা{
		প্রত্যাগম রিক্ত;
	}
}


BROKER_EXPORT সংরচনা mosquitto *mosquitto_client(ধ্রুব বর্ণ *clientid)
{
	size_t len;
	সংরচনা mosquitto *context;

	যদি(!clientid){
		প্রত্যাগম রিক্ত;
	}
	len = strlen(clientid);
	যদি(len == 0){
		প্রত্যাগম রিক্ত;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	প্রত্যাগম context;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_client_port(ধ্রুব সংরচনা mosquitto *client)
{
	যদি(client && client->listener){
		প্রত্যাগম client->listener->port;
	}অন্যথা{
		প্রত্যাগম 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(ধ্রুব সংরচনা mosquitto *client)
{
	যদি(client){
		প্রত্যাগম client->clean_start;
	}অন্যথা{
		প্রত্যাগম true;
	}
}


BROKER_EXPORT ধ্রুব বর্ণ *mosquitto_client_id(ধ্রুব সংরচনা mosquitto *client)
{
	যদি(client){
		প্রত্যাগম client->id;
	}অন্যথা{
		প্রত্যাগম রিক্ত;
	}
}


BROKER_EXPORT unsigned mosquitto_client_id_hashv(ধ্রুব সংরচনা mosquitto *client)
{
	যদি(client){
		প্রত্যাগম client->id_hashv;
	}অন্যথা{
		প্রত্যাগম 0;
	}
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_client_keepalive(ধ্রুব সংরচনা mosquitto *client)
{
	যদি(client){
		প্রত্যাগম client->keepalive;
	}অন্যথা{
		প্রত্যাগম -1;
	}
}


BROKER_EXPORT শূন্য *mosquitto_client_certificate(ধ্রুব সংরচনা mosquitto *client)
{
#ifdef WITH_TLS
	যদি(client && client->ssl){
		প্রত্যাগম SSL_get_peer_certificate(client->ssl);
	}অন্যথা{
		প্রত্যাগম রিক্ত;
	}
#অন্যথা
	UNUSED(client);

	প্রত্যাগম রিক্ত;
#endif
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_client_protocol(ধ্রুব সংরচনা mosquitto *client)
{
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	যদি(client && client->wsi){
		প্রত্যাগম mp_websockets;
	}অন্যথা
#নোচেদ্যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	যদি(client && client->transport == mosq_t_ws){
		প্রত্যাগম mp_websockets;
	}অন্যথা
#অন্যথা
	UNUSED(client);
#endif
	{
		প্রত্যাগম mp_mqtt;
	}
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_client_protocol_version(ধ্রুব সংরচনা mosquitto *client)
{
	যদি(client){
		বিকল্প(client->protocol){
			পক্ষ mosq_p_mqtt31:
				প্রত্যাগম 3;
			পক্ষ mosq_p_mqtt311:
				প্রত্যাগম 4;
			পক্ষ mosq_p_mqtt5:
				প্রত্যাগম 5;
			স্বভাব:
				প্রত্যাগম 0;
		}
	}অন্যথা{
		প্রত্যাগম 0;
	}
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_client_sub_গুনতি(ধ্রুব সংরচনা mosquitto *client)
{
	যদি(client){
		প্রত্যাগম client->subs_গুনতি;
	}অন্যথা{
		প্রত্যাগম 0;
	}
}


BROKER_EXPORT ধ্রুব বর্ণ *mosquitto_client_username(ধ্রুব সংরচনা mosquitto *client)
{
	যদি(client){
#ifdef WITH_BRIDGE
		যদি(client->bridge){
			প্রত্যাগম client->bridge->local_username;
		}অন্যথা
#endif
		{
			প্রত্যাগম client->username;
		}
	}অন্যথা{
		প্রত্যাগম রিক্ত;
	}
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_broker_publish(
		ধ্রুব বর্ণ *clientid,
		ধ্রুব বর্ণ *topic,
		পূর্ণাঙ্ক payloadlen,
		শূন্য *payload,
		পূর্ণাঙ্ক qos,
		bool retain,
		mosquitto_property *properties)
{
	সংরচনা mosquitto__message_v5 *বার্তা;

	যদি(topic == রিক্ত
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == রিক্ত)
			|| qos < 0 || qos > 2){

		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	বার্তা = mosquitto_malloc(পরিমাণ(সংরচনা mosquitto__message_v5));
	যদি(বার্তা == রিক্ত){
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}

	বার্তা->next = রিক্ত;
	বার্তা->prev = রিক্ত;
	যদি(clientid){
		বার্তা->clientid = mosquitto_strdup(clientid);
		যদি(বার্তা->clientid == রিক্ত){
			mosquitto_FREE(বার্তা);
			প্রত্যাগম MOSQ_ERR_NOMEM;
		}
	}অন্যথা{
		বার্তা->clientid = রিক্ত;
	}
	বার্তা->topic = mosquitto_strdup(topic);
	যদি(বার্তা->topic == রিক্ত){
		mosquitto_FREE(বার্তা->clientid);
		mosquitto_FREE(বার্তা);
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}
	বার্তা->payloadlen = payloadlen;
	বার্তা->payload = payload;
	বার্তা->qos = qos;
	বার্তা->retain = retain;
	বার্তা->properties = properties;

	DL_APPEND(db.plugin_msgs, বার্তা);

	loop__update_next_event(1);
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_broker_publish_copy(
		ধ্রুব বর্ণ *clientid,
		ধ্রুব বর্ণ *topic,
		পূর্ণাঙ্ক payloadlen,
		ধ্রুব শূন্য *payload,
		পূর্ণাঙ্ক qos,
		bool retain,
		mosquitto_property *properties)
{
	শূন্য *payload_out;
	পূর্ণাঙ্ক rc;

	যদি(topic == রিক্ত
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == রিক্ত)
			|| qos < 0 || qos > 2){

		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (size_t)(payloadlen+1));
	যদি(payload_out == রিক্ত){
		প্রত্যাগম MOSQ_ERR_NOMEM;
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

	যদি(rc){
		mosquitto_FREE(payload_out);
	}
	প্রত্যাগম rc;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_set_username(সংরচনা mosquitto *client, ধ্রুব বর্ণ *username)
{
	বর্ণ *u_dup;
	বর্ণ *old;

	যদি(!client){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	যদি(username){
		যদি(mosquitto_validate_utf8(username, (পূর্ণাঙ্ক)strlen(username))){
			প্রত্যাগম MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		যদি(!u_dup){
			প্রত্যাগম MOSQ_ERR_NOMEM;
		}
	}অন্যথা{
		u_dup = রিক্ত;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_set_clientid(সংরচনা mosquitto *client, ধ্রুব বর্ণ *clientid)
{
	সংরচনা mosquitto *found_client;
	বর্ণ *id_dup;
	bool in_by_id;
	পূর্ণাঙ্ক clientid_len;

	যদি(!client || !clientid){
		প্রত্যাগম MOSQ_ERR_INVAL;
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

	যদি(in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), found_client);
		যদি(found_client){
			প্রত্যাগম MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (পূর্ণাঙ্ক)strlen(clientid);
	যদি(mosquitto_validate_utf8(clientid, clientid_len)){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	যদি(!id_dup){
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}

	যদি(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	যদি(in_by_id){
		context__add_to_by_id(client);
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
স্থির শূন্য check_subscription_acls(সংরচনা mosquitto *context)
{
	পূর্ণাঙ্ক rc;
	uint8_t reason;

	প্রতি(পূর্ণাঙ্ক i=0; i<context->subs_capacity; i++){
		যদি(context->subs[i] == রিক্ত){
			অনুবর্ত;
		}
		rc = mosquitto_acl_check(context,
				context->subs[i]->topic_filter,
				0,
				রিক্ত,
				0, /* FIXME */
				false,
				রিক্ত,
				MOSQ_ACL_SUBSCRIBE);

		যদি(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


স্থির শূন্য disconnect_client(সংরচনা mosquitto *context, bool with_will)
{
	যদি(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, রিক্ত);
	}
	যদি(with_will == false){
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	যদি(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_kick_client_by_clientid(ধ্রুব বর্ণ *clientid, bool with_will)
{
	সংরচনা mosquitto *ctxt, *ctxt_tmp;

	যদি(clientid == রিক্ত){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}অন্যথা{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), ctxt);
		যদি(ctxt){
			disconnect_client(ctxt, with_will);
			প্রত্যাগম MOSQ_ERR_SUCCESS;
		}অন্যথা{
			প্রত্যাগম MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_kick_client_by_username(ধ্রুব বর্ণ *username, bool with_will)
{
	সংরচনা mosquitto *ctxt, *ctxt_tmp;

	যদি(username == রিক্ত){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			যদি(ctxt->username == রিক্ত){
				disconnect_client(ctxt, with_will);
			}
		}
	}অন্যথা{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			যদি(ctxt->username != রিক্ত && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_apply_on_all_clients(পূর্ণাঙ্ক (*FUNC_client_functor)(ধ্রুব সংরচনা mosquitto *, শূন্য *), শূন্য *functor_context)
{
	পূর্ণাঙ্ক rc = MOSQ_ERR_SUCCESS;
	সংরচনা mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		যদি(rc != MOSQ_ERR_SUCCESS){
			ভঙ্গ;
		}
	}

	প্রত্যাগম rc;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_persist_client_add(সংরচনা mosquitto_client *client)
{
	সংরচনা mosquitto *context;
	পূর্ণাঙ্ক rc;

	যদি(client == রিক্ত){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}
	যদি(client->clientid == রিক্ত){
		rc = MOSQ_ERR_INVAL;
		গচ্ছ error;
	}

	context = রিক্ত;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	যদি(context){
		rc = MOSQ_ERR_INVAL;
		গচ্ছ error;
	}

	context = context__init();
	যদি(!context){
		rc = MOSQ_ERR_NOMEM;
		গচ্ছ error;
	}

	context->id = client->clientid;
	client->clientid = রিক্ত;
	context->username = client->username;
	client->username = রিক্ত;
	context->auth_method = client->auth_method;
	client->auth_method = রিক্ত;

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
	যদি(db.config->per_listener_settings && client->listener_port > 0){
		প্রতি(পূর্ণাঙ্ক i=0; i < db.config->listener_গুনতি; i++){
			যদি(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				ভঙ্গ;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	প্রত্যাগম MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	প্রত্যাগম rc;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_persist_client_update(সংরচনা mosquitto_client *client)
{
	সংরচনা mosquitto *context;
	পূর্ণাঙ্ক rc;

	যদি(client == রিক্ত){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}
	যদি(client->clientid == রিক্ত){
		rc = MOSQ_ERR_INVAL;
		গচ্ছ error;
	}

	context = রিক্ত;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	যদি(context == রিক্ত){
		rc = MOSQ_ERR_NOT_FOUND;
		গচ্ছ error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = রিক্ত;

	context->clean_start = false;
	context->will_delay_time = client->will_delay_time;
	context->session_expiry_time = client->session_expiry_time;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->maximum_packet_size = client->max_packet_size;
	context->retain_available = client->retain_available;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	যদি(db.config->per_listener_settings && client->listener_port > 0){
		প্রতি(পূর্ণাঙ্ক i=0; i < db.config->listener_গুনতি; i++){
			যদি(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				ভঙ্গ;
			}
		}
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	প্রত্যাগম rc;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_persist_client_delete(ধ্রুব বর্ণ *clientid)
{
	সংরচনা mosquitto *context;

	যদি(clientid == রিক্ত){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	context = রিক্ত;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	যদি(context == রিক্ত){
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


স্থির সংরচনা mosquitto_base_বার্তা *find_store_বার্তা(uint64_t store_id)
{
	সংরচনা mosquitto_base_বার্তা *base_বার্তা;

	HASH_FIND(hh, db.বার্তা_store, &store_id, পরিমাণ(store_id), base_বার্তা);
	প্রত্যাগম base_বার্তা;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_persist_client_বার্তা_add(সংরচনা mosquitto_client_বার্তা_2 *client_বার্তা)
{
	সংরচনা mosquitto *context;
	সংরচনা mosquitto_base_বার্তা *base_বার্তা;

	যদি(client_বার্তা == রিক্ত || client_বার্তা->clientid == রিক্ত){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_বার্তা->clientid, strlen(client_বার্তা->clientid), context);
	যদি(context == রিক্ত){
		প্রত্যাগম MOSQ_ERR_NOT_FOUND;
	}
	base_বার্তা = find_store_বার্তা(client_বার্তা->store_id);
	যদি(base_বার্তা == রিক্ত){
		প্রত্যাগম MOSQ_ERR_NOT_FOUND;
	}

	যদি(client_বার্তা->direction == mosq_md_out){
		যদি(client_বার্তা->qos > 0){
			context->last_mid = client_বার্তা->mid;
		}
		প্রত্যাগম db_message_ঢোকাও_outgoing(context, client_বার্তা->cmsg_id, client_বার্তা->mid,
				client_বার্তা->qos, client_বার্তা->retain,
				base_বার্তা, client_বার্তা->subscription_identifier, false, false);
	}অন্যথা যদি(client_বার্তা->direction == mosq_md_in){
		প্রত্যাগম db_message_ঢোকাও_incoming(context, client_বার্তা->cmsg_id, base_বার্তা, false);
	}অন্যথা{
		প্রত্যাগম MOSQ_ERR_INVAL;
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_persist_client_বার্তা_delete(সংরচনা mosquitto_client_বার্তা_2 *client_বার্তা)
{
	সংরচনা mosquitto *context;

	যদি(client_বার্তা == রিক্ত || client_বার্তা->clientid == রিক্ত){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_বার্তা->clientid, strlen(client_বার্তা->clientid), context);
	যদি(context == রিক্ত){
		প্রত্যাগম MOSQ_ERR_NOT_FOUND;
	}


	পূর্ণাঙ্ক rc = MOSQ_ERR_INVAL;
	যদি(client_বার্তা->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_বার্তা->mid, mosq_ms_any, client_বার্তা->qos);
	}অন্যথা যদি(client_বার্তা->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_বার্তা->mid);
	}
	প্রত্যাগম rc;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_persist_client_বার্তা_update(সংরচনা mosquitto_client_বার্তা_2 *client_বার্তা)
{
	সংরচনা mosquitto *context;

	যদি(client_বার্তা == রিক্ত || client_বার্তা->clientid == রিক্ত){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_বার্তা->clientid, strlen(client_বার্তা->clientid), context);
	যদি(context == রিক্ত){
		প্রত্যাগম MOSQ_ERR_NOT_FOUND;
	}

	যদি(client_বার্তা->direction == mosq_md_out){
		db__message_update_outgoing(context, client_বার্তা->mid, client_বার্তা->state, client_বার্তা->qos, false);
	}অন্যথা যদি(client_বার্তা->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}অন্যথা{
		প্রত্যাগম MOSQ_ERR_INVAL;
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_persist_client_বার্তা_clear(সংরচনা mosquitto_client_বার্তা_2 *client_বার্তা)
{
	সংরচনা mosquitto *context;

	যদি(client_বার্তা == রিক্ত || client_বার্তা->clientid == রিক্ত){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_বার্তা->clientid, strlen(client_বার্তা->clientid), context);
	যদি(context == রিক্ত){
		প্রত্যাগম MOSQ_ERR_NOT_FOUND;
	}

	যদি(client_বার্তা->direction == mosq_bmd_in || client_বার্তা->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	যদি(client_বার্তা->direction == mosq_bmd_out || client_বার্তা->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_subscription_add(ধ্রুব সংরচনা mosquitto_subscription *sub)
{
	সংরচনা mosquitto *context;

	যদি(sub == রিক্ত || sub->clientid == রিক্ত || sub->topic_filter == রিক্ত || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, strlen(sub->clientid), context);

	যদি(context){
		প্রত্যাগম sub__add(context, sub);
	}অন্যথা{
		প্রত্যাগম MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_subscription_delete(ধ্রুব বর্ণ *clientid, ধ্রুব বর্ণ *topic)
{
	সংরচনা mosquitto *context;
	uint8_t reason;

	যদি(clientid == রিক্ত || topic == রিক্ত || clientid[0] == '\0' || topic[0] == '\0'){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	যদি(context){
		প্রত্যাগম sub__remove(context, topic, &reason);
	}অন্যথা{
		প্রত্যাগম MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_persist_base_বার্তা_add(সংরচনা mosquitto_base_বার্তা_2 *বার্তা_add)
{
	সংরচনা mosquitto context;
	সংরচনা mosquitto_base_বার্তা *base_বার্তা;
	পূর্ণাঙ্ক rc;

	memset(&context, 0, পরিমাণ(context));

	যদি(বার্তা_add->payloadlen > MQTT_MAX_PAYLOAD){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (বর্ণ *)বার্তা_add->source_id;
	context.username = (বর্ণ *)বার্তা_add->source_username;

	base_বার্তা = mosquitto_calloc(1, পরিমাণ(সংরচনা mosquitto_base_বার্তা));
	যদি(base_বার্তা == রিক্ত){
		গচ্ছ error;
	}
	base_বার্তা->data.store_id = বার্তা_add->store_id;
	base_বার্তা->data.expiry_time = বার্তা_add->expiry_time;
	base_বার্তা->data.payloadlen = বার্তা_add->payloadlen;
	base_বার্তা->data.source_mid = বার্তা_add->source_mid;
	base_বার্তা->data.qos = বার্তা_add->qos;
	base_বার্তা->data.retain = বার্তা_add->retain;

	base_বার্তা->data.payload = বার্তা_add->payload;
	বার্তা_add->payload = রিক্ত;
	base_বার্তা->data.topic = বার্তা_add->topic;
	বার্তা_add->topic = রিক্ত;
	base_বার্তা->data.properties = বার্তা_add->properties;
	বার্তা_add->properties = রিক্ত;

	যদি(বার্তা_add->source_port){
		প্রতি(পূর্ণাঙ্ক i=0; i<db.config->listener_গুনতি; i++){
			যদি(db.config->listeners[i].port == বার্তা_add->source_port){
				base_বার্তা->source_listener = &db.config->listeners[i];
				ভঙ্গ;
			}
		}
	}

	base_বার্তা->stored = true;
	rc = db__message_store(&context, base_বার্তা, রিক্ত, mosq_mo_broker);
	প্রত্যাগম rc;

error:
	mosquitto_property_free_all(&বার্তা_add->properties);
	mosquitto_free(বার্তা_add->topic);
	mosquitto_free(বার্তা_add->payload);
	mosquitto_free(base_বার্তা);

	প্রত্যাগম MOSQ_ERR_NOMEM;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_persist_base_বার্তা_delete(uint64_t store_id)
{
	সংরচনা mosquitto_base_বার্তা *base_বার্তা;

	base_বার্তা = find_store_বার্তা(store_id);
	যদি(base_বার্তা && base_বার্তা->ref_গুনতি == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_বার্তা_store_remove(base_বার্তা, false);
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT শূন্য mosquitto_complete_basic_auth(ধ্রুব বর্ণ *clientid, পূর্ণাঙ্ক result)
{
	সংরচনা mosquitto *context;

	যদি(clientid == রিক্ত){
		প্রত্যাগম;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, strlen(clientid), context);
	যদি(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		যদি(result == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, রিক্ত, 0);
		}অন্যথা{
			যদি(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, রিক্ত);
			}অন্যথা{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, রিক্ত);
			}
			context->clean_start = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_broker_গ্রন্থি_id_set(uint16_t id)
{
	যদি(id > 1023){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}অন্যথা{
		db.গ্রন্থি_id = id;
		db.গ্রন্থি_id_shifted = ((uint64_t)id) << 54;
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT ধ্রুব বর্ণ *mosquitto_persistence_location(শূন্য)
{
	প্রত্যাগম db.config->persistence_location;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_client_will_set(ধ্রুব বর্ণ *clientid, ধ্রুব বর্ণ *topic, পূর্ণাঙ্ক payloadlen, ধ্রুব শূন্য *payload, পূর্ণাঙ্ক qos, bool retain, mosquitto_property *properties)
{
	সংরচনা mosquitto *mosq = mosquitto_client(clientid);
	যদি(!mosq){
		প্রত্যাগম MOSQ_ERR_NOT_FOUND;
	}
	যদি(properties && mosq->protocol != mosq_p_mqtt5){
		যদি(net__is_connected(mosq)){
			প্রত্যাগম MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	প্রত্যাগম will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
