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
	אם(identifier == عدم || plugin_name == عدم){
		החזר MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	אם(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}אחרת{
		identifier->plugin_version = عدم;
	}

	החזר MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ثابت حرف *mosquitto_client_address(ثابت بنية mosquitto *client)
{
	אם(client){
		החזר client->address;
	}אחרת{
		החזר عدم;
	}
}


BROKER_EXPORT بنية mosquitto *mosquitto_client(ثابت حرف *clientid)
{
	size_t len;
	بنية mosquitto *context;

	אם(!clientid){
		החזר عدم;
	}
	len = strlen(clientid);
	אם(len == 0){
		החזר عدم;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	החזר context;
}


BROKER_EXPORT صحيح mosquitto_client_port(ثابت بنية mosquitto *client)
{
	אם(client && client->listener){
		החזר client->listener->port;
	}אחרת{
		החזר 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(ثابت بنية mosquitto *client)
{
	אם(client){
		החזר client->clean_start;
	}אחרת{
		החזר true;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_client_id(ثابت بنية mosquitto *client)
{
	אם(client){
		החזר client->id;
	}אחרת{
		החזר عدم;
	}
}


BROKER_EXPORT unsigned mosquitto_client_id_hashv(ثابت بنية mosquitto *client)
{
	אם(client){
		החזר client->id_hashv;
	}אחרת{
		החזר 0;
	}
}


BROKER_EXPORT صحيح mosquitto_client_keepalive(ثابت بنية mosquitto *client)
{
	אם(client){
		החזר client->keepalive;
	}אחרת{
		החזר -1;
	}
}


BROKER_EXPORT فراغ *mosquitto_client_certificate(ثابت بنية mosquitto *client)
{
#ifdef WITH_TLS
	אם(client && client->ssl){
		החזר SSL_get_peer_certificate(client->ssl);
	}אחרת{
		החזר عدم;
	}
#אחרת
	UNUSED(client);

	החזר عدم;
#endif
}


BROKER_EXPORT صحيح mosquitto_client_protocol(ثابت بنية mosquitto *client)
{
#אם defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	אם(client && client->wsi){
		החזר mp_websockets;
	}אחרת
#وإلاإذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	אם(client && client->transport == mosq_t_ws){
		החזר mp_websockets;
	}אחרת
#אחרת
	UNUSED(client);
#endif
	{
		החזר mp_mqtt;
	}
}


BROKER_EXPORT صحيح mosquitto_client_protocol_version(ثابت بنية mosquitto *client)
{
	אם(client){
		بدل(client->protocol){
			حالة mosq_p_mqtt31:
				החזר 3;
			حالة mosq_p_mqtt311:
				החזר 4;
			حالة mosq_p_mqtt5:
				החזר 5;
			افتراضي:
				החזר 0;
		}
	}אחרת{
		החזר 0;
	}
}


BROKER_EXPORT صحيح mosquitto_client_sub_count(ثابت بنية mosquitto *client)
{
	אם(client){
		החזר client->subs_count;
	}אחרת{
		החזר 0;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_client_username(ثابت بنية mosquitto *client)
{
	אם(client){
#ifdef WITH_BRIDGE
		אם(client->bridge){
			החזר client->bridge->local_username;
		}אחרת
#endif
		{
			החזר client->username;
		}
	}אחרת{
		החזר عدم;
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

	אם(topic == عدم
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == عدم)
			|| qos < 0 || qos > 2){

		החזר MOSQ_ERR_INVAL;
	}

	msg = mosquitto_malloc(حجم(بنية mosquitto__message_v5));
	אם(msg == عدم){
		החזר MOSQ_ERR_NOMEM;
	}

	msg->next = عدم;
	msg->prev = عدم;
	אם(clientid){
		msg->clientid = mosquitto_strdup(clientid);
		אם(msg->clientid == عدم){
			mosquitto_FREE(msg);
			החזר MOSQ_ERR_NOMEM;
		}
	}אחרת{
		msg->clientid = عدم;
	}
	msg->topic = mosquitto_strdup(topic);
	אם(msg->topic == عدم){
		mosquitto_FREE(msg->clientid);
		mosquitto_FREE(msg);
		החזר MOSQ_ERR_NOMEM;
	}
	msg->payloadlen = payloadlen;
	msg->payload = payload;
	msg->qos = qos;
	msg->retain = retain;
	msg->properties = properties;

	DL_APPEND(db.plugin_msgs, msg);

	loop__update_next_event(1);
	החזר MOSQ_ERR_SUCCESS;
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

	אם(topic == عدم
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == عدم)
			|| qos < 0 || qos > 2){

		החזר MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (size_t)(payloadlen+1));
	אם(payload_out == عدم){
		החזר MOSQ_ERR_NOMEM;
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

	אם(rc){
		mosquitto_FREE(payload_out);
	}
	החזר rc;
}


BROKER_EXPORT صحيح mosquitto_set_username(بنية mosquitto *client, ثابت حرف *username)
{
	حرف *u_dup;
	حرف *old;

	אם(!client){
		החזר MOSQ_ERR_INVAL;
	}

	אם(username){
		אם(mosquitto_validate_utf8(username, (صحيح)strlen(username))){
			החזר MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		אם(!u_dup){
			החזר MOSQ_ERR_NOMEM;
		}
	}אחרת{
		u_dup = عدم;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	החזר MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_set_clientid(بنية mosquitto *client, ثابت حرف *clientid)
{
	بنية mosquitto *found_client;
	حرف *id_dup;
	bool in_by_id;
	صحيح clientid_len;

	אם(!client || !clientid){
		החזר MOSQ_ERR_INVAL;
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

	אם(in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), found_client);
		אם(found_client){
			החזר MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (صحيح)strlen(clientid);
	אם(mosquitto_validate_utf8(clientid, clientid_len)){
		החזר MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	אם(!id_dup){
		החזר MOSQ_ERR_NOMEM;
	}

	אם(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	אם(in_by_id){
		context__add_to_by_id(client);
	}

	החזר MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
ساكن فراغ check_subscription_acls(بنية mosquitto *context)
{
	صحيح rc;
	uint8_t reason;

	עבור(صحيح i=0; i<context->subs_capacity; i++){
		אם(context->subs[i] == عدم){
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

		אם(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


ساكن فراغ disconnect_client(بنية mosquitto *context, bool with_will)
{
	אם(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, عدم);
	}
	אם(with_will == false){
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	אם(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT صحيح mosquitto_kick_client_by_clientid(ثابت حرف *clientid, bool with_will)
{
	بنية mosquitto *ctxt, *ctxt_tmp;

	אם(clientid == عدم){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		החזר MOSQ_ERR_SUCCESS;
	}אחרת{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), ctxt);
		אם(ctxt){
			disconnect_client(ctxt, with_will);
			החזר MOSQ_ERR_SUCCESS;
		}אחרת{
			החזר MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT صحيح mosquitto_kick_client_by_username(ثابت حرف *username, bool with_will)
{
	بنية mosquitto *ctxt, *ctxt_tmp;

	אם(username == عدم){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			אם(ctxt->username == عدم){
				disconnect_client(ctxt, with_will);
			}
		}
	}אחרת{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			אם(ctxt->username != عدم && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	החזר MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_apply_on_all_clients(صحيح (*FUNC_client_functor)(ثابت بنية mosquitto *, فراغ *), فراغ *functor_context)
{
	صحيح rc = MOSQ_ERR_SUCCESS;
	بنية mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		אם(rc != MOSQ_ERR_SUCCESS){
			اكسر;
		}
	}

	החזר rc;
}


BROKER_EXPORT صحيح mosquitto_persist_client_add(بنية mosquitto_client *client)
{
	بنية mosquitto *context;
	صحيح rc;

	אם(client == عدم){
		החזר MOSQ_ERR_INVAL;
	}
	אם(client->clientid == عدم){
		rc = MOSQ_ERR_INVAL;
		اذهب error;
	}

	context = عدم;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	אם(context){
		rc = MOSQ_ERR_INVAL;
		اذهب error;
	}

	context = context__init();
	אם(!context){
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
	אם(db.config->per_listener_settings && client->listener_port > 0){
		עבור(صحيح i=0; i < db.config->listener_count; i++){
			אם(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	החזר MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	החזר rc;
}


BROKER_EXPORT صحيح mosquitto_persist_client_update(بنية mosquitto_client *client)
{
	بنية mosquitto *context;
	صحيح rc;

	אם(client == عدم){
		החזר MOSQ_ERR_INVAL;
	}
	אם(client->clientid == عدم){
		rc = MOSQ_ERR_INVAL;
		اذهب error;
	}

	context = عدم;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	אם(context == عدم){
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
	אם(db.config->per_listener_settings && client->listener_port > 0){
		עבור(صحيح i=0; i < db.config->listener_count; i++){
			אם(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	החזר MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	החזר rc;
}


BROKER_EXPORT صحيح mosquitto_persist_client_delete(ثابت حرف *clientid)
{
	بنية mosquitto *context;

	אם(clientid == عدم){
		החזר MOSQ_ERR_INVAL;
	}

	context = عدم;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	אם(context == عدم){
		החזר MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	החזר MOSQ_ERR_SUCCESS;
}


ساكن بنية mosquitto__base_msg *find_store_msg(uint64_t store_id)
{
	بنية mosquitto__base_msg *base_msg;

	HASH_FIND(hh, db.msg_store, &store_id, حجم(store_id), base_msg);
	החזר base_msg;
}


BROKER_EXPORT صحيح mosquitto_persist_client_msg_add(بنية mosquitto_client_msg *client_msg)
{
	بنية mosquitto *context;
	بنية mosquitto__base_msg *base_msg;

	אם(client_msg == عدم || client_msg->clientid == عدم){
		החזר MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	אם(context == عدم){
		החזר MOSQ_ERR_NOT_FOUND;
	}
	base_msg = find_store_msg(client_msg->store_id);
	אם(base_msg == عدم){
		החזר MOSQ_ERR_NOT_FOUND;
	}

	אם(client_msg->direction == mosq_md_out){
		אם(client_msg->qos > 0){
			context->last_mid = client_msg->mid;
		}
		החזר db__message_insert_outgoing(context, client_msg->cmsg_id, client_msg->mid,
				client_msg->qos, client_msg->retain,
				base_msg, client_msg->subscription_identifier, false, false);
	}אחרת אם(client_msg->direction == mosq_md_in){
		החזר db__message_insert_incoming(context, client_msg->cmsg_id, base_msg, false);
	}אחרת{
		החזר MOSQ_ERR_INVAL;
	}
	החזר MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_persist_client_msg_delete(بنية mosquitto_client_msg *client_msg)
{
	بنية mosquitto *context;

	אם(client_msg == عدم || client_msg->clientid == عدم){
		החזר MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	אם(context == عدم){
		החזר MOSQ_ERR_NOT_FOUND;
	}


	صحيح rc = MOSQ_ERR_INVAL;
	אם(client_msg->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_msg->mid, mosq_ms_any, client_msg->qos);
	}אחרת אם(client_msg->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_msg->mid);
	}
	החזר rc;
}


BROKER_EXPORT صحيح mosquitto_persist_client_msg_update(بنية mosquitto_client_msg *client_msg)
{
	بنية mosquitto *context;

	אם(client_msg == عدم || client_msg->clientid == عدم){
		החזר MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	אם(context == عدم){
		החזר MOSQ_ERR_NOT_FOUND;
	}

	אם(client_msg->direction == mosq_md_out){
		db__message_update_outgoing(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}אחרת אם(client_msg->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}אחרת{
		החזר MOSQ_ERR_INVAL;
	}
	החזר MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_persist_client_msg_clear(بنية mosquitto_client_msg *client_msg)
{
	بنية mosquitto *context;

	אם(client_msg == عدم || client_msg->clientid == عدم){
		החזר MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	אם(context == عدم){
		החזר MOSQ_ERR_NOT_FOUND;
	}

	אם(client_msg->direction == mosq_bmd_in || client_msg->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	אם(client_msg->direction == mosq_bmd_out || client_msg->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	החזר MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_subscription_add(ثابت بنية mosquitto_subscription *sub)
{
	بنية mosquitto *context;

	אם(sub == عدم || sub->clientid == عدم || sub->topic_filter == عدم || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		החזר MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, strlen(sub->clientid), context);

	אם(context){
		החזר sub__add(context, sub);
	}אחרת{
		החזר MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT صحيح mosquitto_subscription_delete(ثابت حرف *clientid, ثابت حرف *topic)
{
	بنية mosquitto *context;
	uint8_t reason;

	אם(clientid == عدم || topic == عدم || clientid[0] == '\0' || topic[0] == '\0'){
		החזר MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	אם(context){
		החזר sub__remove(context, topic, &reason);
	}אחרת{
		החזר MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT صحيح mosquitto_persist_base_msg_add(بنية mosquitto_base_msg *msg_add)
{
	بنية mosquitto context;
	بنية mosquitto__base_msg *base_msg;
	صحيح rc;

	memset(&context, 0, حجم(context));

	אם(msg_add->payloadlen > MQTT_MAX_PAYLOAD){
		החזר MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (حرف *)msg_add->source_id;
	context.username = (حرف *)msg_add->source_username;

	base_msg = mosquitto_calloc(1, حجم(بنية mosquitto__base_msg));
	אם(base_msg == عدم){
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

	אם(msg_add->source_port){
		עבור(صحيح i=0; i<db.config->listener_count; i++){
			אם(db.config->listeners[i].port == msg_add->source_port){
				base_msg->source_listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	base_msg->stored = true;
	rc = db__message_store(&context, base_msg, عدم, mosq_mo_broker);
	החזר rc;

error:
	mosquitto_property_free_all(&msg_add->properties);
	mosquitto_free(msg_add->topic);
	mosquitto_free(msg_add->payload);
	mosquitto_free(base_msg);

	החזר MOSQ_ERR_NOMEM;
}


BROKER_EXPORT صحيح mosquitto_persist_base_msg_delete(uint64_t store_id)
{
	بنية mosquitto__base_msg *base_msg;

	base_msg = find_store_msg(store_id);
	אם(base_msg && base_msg->ref_count == 0){
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

	החזר MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT فراغ mosquitto_complete_basic_auth(ثابت حرف *clientid, صحيح result)
{
	بنية mosquitto *context;

	אם(clientid == عدم){
		החזר;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, strlen(clientid), context);
	אם(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		אם(result == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, عدم, 0);
		}אחרת{
			אם(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, عدم);
			}אחרת{
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
	אם(id > 1023){
		החזר MOSQ_ERR_INVAL;
	}אחרת{
		db.node_id = id;
		db.node_id_shifted = ((uint64_t)id) << 54;
		החזר MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_persistence_location(فراغ)
{
	החזר db.config->persistence_location;
}


BROKER_EXPORT صحيح mosquitto_client_will_set(ثابت حرف *clientid, ثابت حرف *topic, صحيح payloadlen, ثابت فراغ *payload, صحيح qos, bool retain, mosquitto_property *properties)
{
	بنية mosquitto *mosq = mosquitto_client(clientid);
	אם(!mosq){
		החזר MOSQ_ERR_NOT_FOUND;
	}
	אם(properties && mosq->protocol != mosq_p_mqtt5){
		אם(net__is_connected(mosq)){
			החזר MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	החזר will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
