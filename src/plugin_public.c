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

#ضمن "config.h"

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "send_mosq.h"
#ضمن "util_mosq.h"
#ضمن "will_mosq.h"
#ضمن "utlist.h"
#ضمن "will_mosq.h"

#ifdef WITH_TLS
#  include <openssl/ssl.h>
#endif


BROKER_EXPORT صحيح mosquitto_plugin_set_info(mosquitto_plugin_id_t *identifier,
		ثابت حرف *plugin_name,
		ثابت حرف *plugin_version)
{
	ܐܢ(identifier == عدم || plugin_name == عدم){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	ܐܢ(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}ܐܚܪܢܐ{
		identifier->plugin_version = عدم;
	}

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ثابت حرف *mosquitto_client_address(ثابت بنية mosquitto *client)
{
	ܐܢ(client){
		ܦܢܐ client->address;
	}ܐܚܪܢܐ{
		ܦܢܐ عدم;
	}
}


BROKER_EXPORT بنية mosquitto *mosquitto_client(ثابت حرف *clientid)
{
	size_t len;
	بنية mosquitto *context;

	ܐܢ(!clientid){
		ܦܢܐ عدم;
	}
	len = strlen(clientid);
	ܐܢ(len == 0){
		ܦܢܐ عدم;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	ܦܢܐ context;
}


BROKER_EXPORT صحيح mosquitto_client_port(ثابت بنية mosquitto *client)
{
	ܐܢ(client && client->listener){
		ܦܢܐ client->listener->port;
	}ܐܚܪܢܐ{
		ܦܢܐ 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(ثابت بنية mosquitto *client)
{
	ܐܢ(client){
		ܦܢܐ client->clean_start;
	}ܐܚܪܢܐ{
		ܦܢܐ true;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_client_id(ثابت بنية mosquitto *client)
{
	ܐܢ(client){
		ܦܢܐ client->id;
	}ܐܚܪܢܐ{
		ܦܢܐ عدم;
	}
}


BROKER_EXPORT unsigned mosquitto_client_id_hashv(ثابت بنية mosquitto *client)
{
	ܐܢ(client){
		ܦܢܐ client->id_hashv;
	}ܐܚܪܢܐ{
		ܦܢܐ 0;
	}
}


BROKER_EXPORT صحيح mosquitto_client_keepalive(ثابت بنية mosquitto *client)
{
	ܐܢ(client){
		ܦܢܐ client->keepalive;
	}ܐܚܪܢܐ{
		ܦܢܐ -1;
	}
}


BROKER_EXPORT فراغ *mosquitto_client_certificate(ثابت بنية mosquitto *client)
{
#ifdef WITH_TLS
	ܐܢ(client && client->ssl){
		ܦܢܐ SSL_get_peer_certificate(client->ssl);
	}ܐܚܪܢܐ{
		ܦܢܐ عدم;
	}
#ܐܚܪܢܐ
	UNUSED(client);

	ܦܢܐ عدم;
#endif
}


BROKER_EXPORT صحيح mosquitto_client_protocol(ثابت بنية mosquitto *client)
{
#ܐܢ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ܐܢ(client && client->wsi){
		ܦܢܐ mp_websockets;
	}ܐܚܪܢܐ
#وإلاإذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	ܐܢ(client && client->transport == mosq_t_ws){
		ܦܢܐ mp_websockets;
	}ܐܚܪܢܐ
#ܐܚܪܢܐ
	UNUSED(client);
#endif
	{
		ܦܢܐ mp_mqtt;
	}
}


BROKER_EXPORT صحيح mosquitto_client_protocol_version(ثابت بنية mosquitto *client)
{
	ܐܢ(client){
		بدل(client->protocol){
			حالة mosq_p_mqtt31:
				ܦܢܐ 3;
			حالة mosq_p_mqtt311:
				ܦܢܐ 4;
			حالة mosq_p_mqtt5:
				ܦܢܐ 5;
			افتراضي:
				ܦܢܐ 0;
		}
	}ܐܚܪܢܐ{
		ܦܢܐ 0;
	}
}


BROKER_EXPORT صحيح mosquitto_client_sub_count(ثابت بنية mosquitto *client)
{
	ܐܢ(client){
		ܦܢܐ client->subs_count;
	}ܐܚܪܢܐ{
		ܦܢܐ 0;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_client_username(ثابت بنية mosquitto *client)
{
	ܐܢ(client){
#ifdef WITH_BRIDGE
		ܐܢ(client->bridge){
			ܦܢܐ client->bridge->local_username;
		}ܐܚܪܢܐ
#endif
		{
			ܦܢܐ client->username;
		}
	}ܐܚܪܢܐ{
		ܦܢܐ عدم;
	}
}


BROKER_EXPORT صحيح mosquitto_broker_publish(
		ثابت حرف *clientid,
		ثابت حرف *topic,
		صحيح payloadlen,
		فراغ *payload,
		صحيح qos,
		bool retain,
		mosquitto_property *properties)
{
	بنية mosquitto__message_v5 *msg;

	ܐܢ(topic == عدم
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == عدم)
			|| qos < 0 || qos > 2){

		ܦܢܐ MOSQ_ERR_INVAL;
	}

	msg = mosquitto_malloc(حجم(بنية mosquitto__message_v5));
	ܐܢ(msg == عدم){
		ܦܢܐ MOSQ_ERR_NOMEM;
	}

	msg->next = عدم;
	msg->prev = عدم;
	ܐܢ(clientid){
		msg->clientid = mosquitto_strdup(clientid);
		ܐܢ(msg->clientid == عدم){
			mosquitto_FREE(msg);
			ܦܢܐ MOSQ_ERR_NOMEM;
		}
	}ܐܚܪܢܐ{
		msg->clientid = عدم;
	}
	msg->topic = mosquitto_strdup(topic);
	ܐܢ(msg->topic == عدم){
		mosquitto_FREE(msg->clientid);
		mosquitto_FREE(msg);
		ܦܢܐ MOSQ_ERR_NOMEM;
	}
	msg->payloadlen = payloadlen;
	msg->payload = payload;
	msg->qos = qos;
	msg->retain = retain;
	msg->properties = properties;

	DL_APPEND(db.plugin_msgs, msg);

	loop__update_next_event(1);
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_broker_publish_copy(
		ثابت حرف *clientid,
		ثابت حرف *topic,
		صحيح payloadlen,
		ثابت فراغ *payload,
		صحيح qos,
		bool retain,
		mosquitto_property *properties)
{
	فراغ *payload_out;
	صحيح rc;

	ܐܢ(topic == عدم
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == عدم)
			|| qos < 0 || qos > 2){

		ܦܢܐ MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (size_t)(payloadlen+1));
	ܐܢ(payload_out == عدم){
		ܦܢܐ MOSQ_ERR_NOMEM;
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

	ܐܢ(rc){
		mosquitto_FREE(payload_out);
	}
	ܦܢܐ rc;
}


BROKER_EXPORT صحيح mosquitto_set_username(بنية mosquitto *client, ثابت حرف *username)
{
	حرف *u_dup;
	حرف *old;

	ܐܢ(!client){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	ܐܢ(username){
		ܐܢ(mosquitto_validate_utf8(username, (صحيح)strlen(username))){
			ܦܢܐ MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		ܐܢ(!u_dup){
			ܦܢܐ MOSQ_ERR_NOMEM;
		}
	}ܐܚܪܢܐ{
		u_dup = عدم;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_set_clientid(بنية mosquitto *client, ثابت حرف *clientid)
{
	بنية mosquitto *found_client;
	حرف *id_dup;
	bool in_by_id;
	صحيح clientid_len;

	ܐܢ(!client || !clientid){
		ܦܢܐ MOSQ_ERR_INVAL;
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

	ܐܢ(in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), found_client);
		ܐܢ(found_client){
			ܦܢܐ MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (صحيح)strlen(clientid);
	ܐܢ(mosquitto_validate_utf8(clientid, clientid_len)){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	ܐܢ(!id_dup){
		ܦܢܐ MOSQ_ERR_NOMEM;
	}

	ܐܢ(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	ܐܢ(in_by_id){
		context__add_to_by_id(client);
	}

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
ساكن فراغ check_subscription_acls(بنية mosquitto *context)
{
	صحيح rc;
	uint8_t reason;

	لكل(صحيح i=0; i<context->subs_capacity; i++){
		ܐܢ(context->subs[i] == عدم){
			استمر;
		}
		rc = mosquitto_acl_check(context,
				context->subs[i]->topic_filter,
				0,
				عدم,
				0, /* FIXME */
				false,
				عدم,
				MOSQ_ACL_SUBSCRIBE);

		ܐܢ(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


ساكن فراغ disconnect_client(بنية mosquitto *context, bool with_will)
{
	ܐܢ(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, عدم);
	}
	ܐܢ(with_will == false){
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	ܐܢ(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT صحيح mosquitto_kick_client_by_clientid(ثابت حرف *clientid, bool with_will)
{
	بنية mosquitto *ctxt, *ctxt_tmp;

	ܐܢ(clientid == عدم){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		ܦܢܐ MOSQ_ERR_SUCCESS;
	}ܐܚܪܢܐ{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), ctxt);
		ܐܢ(ctxt){
			disconnect_client(ctxt, with_will);
			ܦܢܐ MOSQ_ERR_SUCCESS;
		}ܐܚܪܢܐ{
			ܦܢܐ MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT صحيح mosquitto_kick_client_by_username(ثابت حرف *username, bool with_will)
{
	بنية mosquitto *ctxt, *ctxt_tmp;

	ܐܢ(username == عدم){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			ܐܢ(ctxt->username == عدم){
				disconnect_client(ctxt, with_will);
			}
		}
	}ܐܚܪܢܐ{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			ܐܢ(ctxt->username != عدم && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_apply_on_all_clients(صحيح (*FUNC_client_functor)(ثابت بنية mosquitto *, فراغ *), فراغ *functor_context)
{
	صحيح rc = MOSQ_ERR_SUCCESS;
	بنية mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		ܐܢ(rc != MOSQ_ERR_SUCCESS){
			اكسر;
		}
	}

	ܦܢܐ rc;
}


BROKER_EXPORT صحيح mosquitto_persist_client_add(بنية mosquitto_client *client)
{
	بنية mosquitto *context;
	صحيح rc;

	ܐܢ(client == عدم){
		ܦܢܐ MOSQ_ERR_INVAL;
	}
	ܐܢ(client->clientid == عدم){
		rc = MOSQ_ERR_INVAL;
		اذهب error;
	}

	context = عدم;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	ܐܢ(context){
		rc = MOSQ_ERR_INVAL;
		اذهب error;
	}

	context = context__init();
	ܐܢ(!context){
		rc = MOSQ_ERR_NOMEM;
		اذهب error;
	}

	context->id = client->clientid;
	client->clientid = عدم;
	context->username = client->username;
	client->username = عدم;
	context->auth_method = client->auth_method;
	client->auth_method = عدم;

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
	ܐܢ(db.config->per_listener_settings && client->listener_port > 0){
		لكل(صحيح i=0; i < db.config->listener_count; i++){
			ܐܢ(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	ܦܢܐ MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	ܦܢܐ rc;
}


BROKER_EXPORT صحيح mosquitto_persist_client_update(بنية mosquitto_client *client)
{
	بنية mosquitto *context;
	صحيح rc;

	ܐܢ(client == عدم){
		ܦܢܐ MOSQ_ERR_INVAL;
	}
	ܐܢ(client->clientid == عدم){
		rc = MOSQ_ERR_INVAL;
		اذهب error;
	}

	context = عدم;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	ܐܢ(context == عدم){
		rc = MOSQ_ERR_NOT_FOUND;
		اذهب error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = عدم;

	context->clean_start = false;
	context->will_delay_time = client->will_delay_time;
	context->session_expiry_time = client->session_expiry_time;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->maximum_packet_size = client->max_packet_size;
	context->retain_available = client->retain_available;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	ܐܢ(db.config->per_listener_settings && client->listener_port > 0){
		لكل(صحيح i=0; i < db.config->listener_count; i++){
			ܐܢ(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	ܦܢܐ MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	ܦܢܐ rc;
}


BROKER_EXPORT صحيح mosquitto_persist_client_delete(ثابت حرف *clientid)
{
	بنية mosquitto *context;

	ܐܢ(clientid == عدم){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	context = عدم;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	ܐܢ(context == عدم){
		ܦܢܐ MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


ساكن بنية mosquitto__base_msg *find_store_msg(uint64_t store_id)
{
	بنية mosquitto__base_msg *base_msg;

	HASH_FIND(hh, db.msg_store, &store_id, حجم(store_id), base_msg);
	ܦܢܐ base_msg;
}


BROKER_EXPORT صحيح mosquitto_persist_client_msg_add(بنية mosquitto_client_msg *client_msg)
{
	بنية mosquitto *context;
	بنية mosquitto__base_msg *base_msg;

	ܐܢ(client_msg == عدم || client_msg->clientid == عدم){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	ܐܢ(context == عدم){
		ܦܢܐ MOSQ_ERR_NOT_FOUND;
	}
	base_msg = find_store_msg(client_msg->store_id);
	ܐܢ(base_msg == عدم){
		ܦܢܐ MOSQ_ERR_NOT_FOUND;
	}

	ܐܢ(client_msg->direction == mosq_md_out){
		ܐܢ(client_msg->qos > 0){
			context->last_mid = client_msg->mid;
		}
		ܦܢܐ db__message_insert_outgoing(context, client_msg->cmsg_id, client_msg->mid,
				client_msg->qos, client_msg->retain,
				base_msg, client_msg->subscription_identifier, false, false);
	}ܐܚܪܢܐ ܐܢ(client_msg->direction == mosq_md_in){
		ܦܢܐ db__message_insert_incoming(context, client_msg->cmsg_id, base_msg, false);
	}ܐܚܪܢܐ{
		ܦܢܐ MOSQ_ERR_INVAL;
	}
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_persist_client_msg_delete(بنية mosquitto_client_msg *client_msg)
{
	بنية mosquitto *context;

	ܐܢ(client_msg == عدم || client_msg->clientid == عدم){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	ܐܢ(context == عدم){
		ܦܢܐ MOSQ_ERR_NOT_FOUND;
	}


	صحيح rc = MOSQ_ERR_INVAL;
	ܐܢ(client_msg->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_msg->mid, mosq_ms_any, client_msg->qos);
	}ܐܚܪܢܐ ܐܢ(client_msg->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_msg->mid);
	}
	ܦܢܐ rc;
}


BROKER_EXPORT صحيح mosquitto_persist_client_msg_update(بنية mosquitto_client_msg *client_msg)
{
	بنية mosquitto *context;

	ܐܢ(client_msg == عدم || client_msg->clientid == عدم){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	ܐܢ(context == عدم){
		ܦܢܐ MOSQ_ERR_NOT_FOUND;
	}

	ܐܢ(client_msg->direction == mosq_md_out){
		db__message_update_outgoing(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}ܐܚܪܢܐ ܐܢ(client_msg->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}ܐܚܪܢܐ{
		ܦܢܐ MOSQ_ERR_INVAL;
	}
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_persist_client_msg_clear(بنية mosquitto_client_msg *client_msg)
{
	بنية mosquitto *context;

	ܐܢ(client_msg == عدم || client_msg->clientid == عدم){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	ܐܢ(context == عدم){
		ܦܢܐ MOSQ_ERR_NOT_FOUND;
	}

	ܐܢ(client_msg->direction == mosq_bmd_in || client_msg->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	ܐܢ(client_msg->direction == mosq_bmd_out || client_msg->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_subscription_add(ثابت بنية mosquitto_subscription *sub)
{
	بنية mosquitto *context;

	ܐܢ(sub == عدم || sub->clientid == عدم || sub->topic_filter == عدم || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, strlen(sub->clientid), context);

	ܐܢ(context){
		ܦܢܐ sub__add(context, sub);
	}ܐܚܪܢܐ{
		ܦܢܐ MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT صحيح mosquitto_subscription_delete(ثابت حرف *clientid, ثابت حرف *topic)
{
	بنية mosquitto *context;
	uint8_t reason;

	ܐܢ(clientid == عدم || topic == عدم || clientid[0] == '\0' || topic[0] == '\0'){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	ܐܢ(context){
		ܦܢܐ sub__remove(context, topic, &reason);
	}ܐܚܪܢܐ{
		ܦܢܐ MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT صحيح mosquitto_persist_base_msg_add(بنية mosquitto_base_msg *msg_add)
{
	بنية mosquitto context;
	بنية mosquitto__base_msg *base_msg;
	صحيح rc;

	memset(&context, 0, حجم(context));

	ܐܢ(msg_add->payloadlen > MQTT_MAX_PAYLOAD){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (حرف *)msg_add->source_id;
	context.username = (حرف *)msg_add->source_username;

	base_msg = mosquitto_calloc(1, حجم(بنية mosquitto__base_msg));
	ܐܢ(base_msg == عدم){
		اذهب error;
	}
	base_msg->data.store_id = msg_add->store_id;
	base_msg->data.expiry_time = msg_add->expiry_time;
	base_msg->data.payloadlen = msg_add->payloadlen;
	base_msg->data.source_mid = msg_add->source_mid;
	base_msg->data.qos = msg_add->qos;
	base_msg->data.retain = msg_add->retain;

	base_msg->data.payload = msg_add->payload;
	msg_add->payload = عدم;
	base_msg->data.topic = msg_add->topic;
	msg_add->topic = عدم;
	base_msg->data.properties = msg_add->properties;
	msg_add->properties = عدم;

	ܐܢ(msg_add->source_port){
		لكل(صحيح i=0; i<db.config->listener_count; i++){
			ܐܢ(db.config->listeners[i].port == msg_add->source_port){
				base_msg->source_listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	base_msg->stored = true;
	rc = db__message_store(&context, base_msg, عدم, mosq_mo_broker);
	ܦܢܐ rc;

error:
	mosquitto_property_free_all(&msg_add->properties);
	mosquitto_free(msg_add->topic);
	mosquitto_free(msg_add->payload);
	mosquitto_free(base_msg);

	ܦܢܐ MOSQ_ERR_NOMEM;
}


BROKER_EXPORT صحيح mosquitto_persist_base_msg_delete(uint64_t store_id)
{
	بنية mosquitto__base_msg *base_msg;

	base_msg = find_store_msg(store_id);
	ܐܢ(base_msg && base_msg->ref_count == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db__msg_store_remove(base_msg, false);
	}

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT فراغ mosquitto_complete_basic_auth(ثابت حرف *clientid, صحيح result)
{
	بنية mosquitto *context;

	ܐܢ(clientid == عدم){
		ܦܢܐ;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, strlen(clientid), context);
	ܐܢ(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		ܐܢ(result == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, عدم, 0);
		}ܐܚܪܢܐ{
			ܐܢ(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, عدم);
			}ܐܚܪܢܐ{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, عدم);
			}
			context->clean_start = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT صحيح mosquitto_broker_node_id_set(uint16_t id)
{
	ܐܢ(id > 1023){
		ܦܢܐ MOSQ_ERR_INVAL;
	}ܐܚܪܢܐ{
		db.node_id = id;
		db.node_id_shifted = ((uint64_t)id) << 54;
		ܦܢܐ MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_persistence_location(فراغ)
{
	ܦܢܐ db.config->persistence_location;
}


BROKER_EXPORT صحيح mosquitto_client_will_set(ثابت حرف *clientid, ثابت حرف *topic, صحيح payloadlen, ثابت فراغ *payload, صحيح qos, bool retain, mosquitto_property *properties)
{
	بنية mosquitto *mosq = mosquitto_client(clientid);
	ܐܢ(!mosq){
		ܦܢܐ MOSQ_ERR_NOT_FOUND;
	}
	ܐܢ(properties && mosq->protocol != mosq_p_mqtt5){
		ܐܢ(net__is_connected(mosq)){
			ܦܢܐ MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	ܦܢܐ will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
