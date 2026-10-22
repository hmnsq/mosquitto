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

#شامل "config.h"

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "send_mosq.h"
#شامل "util_mosq.h"
#شامل "will_mosq.h"
#شامل "utlist.h"
#شامل "will_mosq.h"

#ifdef WITH_TLS
#  include <openssl/ssl.h>
#endif


BROKER_EXPORT صحیح mosquitto_plugin_set_info(mosquitto_plugin_id_t *identifier,
		ثابت نویسه *plugin_name,
		ثابت نویسه *plugin_version)
{
	اگر(identifier == پوچ || plugin_name == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	اگر(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}وگرنه{
		identifier->plugin_version = پوچ;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ثابت نویسه *mosquitto_client_address(ثابت ساختار mosquitto *client)
{
	اگر(client){
		بازگشت client->address;
	}وگرنه{
		بازگشت پوچ;
	}
}


BROKER_EXPORT ساختار mosquitto *mosquitto_client(ثابت نویسه *clientid)
{
	size_t len;
	ساختار mosquitto *context;

	اگر(!clientid){
		بازگشت پوچ;
	}
	len = strlen(clientid);
	اگر(len == 0){
		بازگشت پوچ;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	بازگشت context;
}


BROKER_EXPORT صحیح mosquitto_client_port(ثابت ساختار mosquitto *client)
{
	اگر(client && client->listener){
		بازگشت client->listener->port;
	}وگرنه{
		بازگشت 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(ثابت ساختار mosquitto *client)
{
	اگر(client){
		بازگشت client->clean_start;
	}وگرنه{
		بازگشت true;
	}
}


BROKER_EXPORT ثابت نویسه *mosquitto_client_id(ثابت ساختار mosquitto *client)
{
	اگر(client){
		بازگشت client->id;
	}وگرنه{
		بازگشت پوچ;
	}
}


BROKER_EXPORT unsigned mosquitto_client_id_hashv(ثابت ساختار mosquitto *client)
{
	اگر(client){
		بازگشت client->id_hashv;
	}وگرنه{
		بازگشت 0;
	}
}


BROKER_EXPORT صحیح mosquitto_client_keepalive(ثابت ساختار mosquitto *client)
{
	اگر(client){
		بازگشت client->keepalive;
	}وگرنه{
		بازگشت -1;
	}
}


BROKER_EXPORT تهی *mosquitto_client_certificate(ثابت ساختار mosquitto *client)
{
#ifdef WITH_TLS
	اگر(client && client->ssl){
		بازگشت SSL_get_peer_certificate(client->ssl);
	}وگرنه{
		بازگشت پوچ;
	}
#وگرنه
	UNUSED(client);

	بازگشت پوچ;
#endif
}


BROKER_EXPORT صحیح mosquitto_client_protocol(ثابت ساختار mosquitto *client)
{
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	اگر(client && client->wsi){
		بازگشت mp_websockets;
	}وگرنه
#وگرنهاگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	اگر(client && client->transport == mosq_t_ws){
		بازگشت mp_websockets;
	}وگرنه
#وگرنه
	UNUSED(client);
#endif
	{
		بازگشت mp_mqtt;
	}
}


BROKER_EXPORT صحیح mosquitto_client_protocol_version(ثابت ساختار mosquitto *client)
{
	اگر(client){
		انتخاب(client->protocol){
			حالت mosq_p_mqtt31:
				بازگشت 3;
			حالت mosq_p_mqtt311:
				بازگشت 4;
			حالت mosq_p_mqtt5:
				بازگشت 5;
			پیشفرض:
				بازگشت 0;
		}
	}وگرنه{
		بازگشت 0;
	}
}


BROKER_EXPORT صحیح mosquitto_client_sub_شمار(ثابت ساختار mosquitto *client)
{
	اگر(client){
		بازگشت client->subs_شمار;
	}وگرنه{
		بازگشت 0;
	}
}


BROKER_EXPORT ثابت نویسه *mosquitto_client_username(ثابت ساختار mosquitto *client)
{
	اگر(client){
#ifdef WITH_BRIDGE
		اگر(client->bridge){
			بازگشت client->bridge->local_username;
		}وگرنه
#endif
		{
			بازگشت client->username;
		}
	}وگرنه{
		بازگشت پوچ;
	}
}


BROKER_EXPORT صحیح mosquitto_broker_publish(
		ثابت نویسه *clientid,
		ثابت نویسه *topic,
		صحیح payloadlen,
		تهی *payload,
		صحیح qos,
		bool retain,
		mosquitto_property *properties)
{
	ساختار mosquitto__message_v5 *پیام;

	اگر(topic == پوچ
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == پوچ)
			|| qos < 0 || qos > 2){

		بازگشت MOSQ_ERR_INVAL;
	}

	پیام = mosquitto_malloc(اندازه(ساختار mosquitto__message_v5));
	اگر(پیام == پوچ){
		بازگشت MOSQ_ERR_NOMEM;
	}

	پیام->next = پوچ;
	پیام->prev = پوچ;
	اگر(clientid){
		پیام->clientid = mosquitto_strdup(clientid);
		اگر(پیام->clientid == پوچ){
			mosquitto_FREE(پیام);
			بازگشت MOSQ_ERR_NOMEM;
		}
	}وگرنه{
		پیام->clientid = پوچ;
	}
	پیام->topic = mosquitto_strdup(topic);
	اگر(پیام->topic == پوچ){
		mosquitto_FREE(پیام->clientid);
		mosquitto_FREE(پیام);
		بازگشت MOSQ_ERR_NOMEM;
	}
	پیام->payloadlen = payloadlen;
	پیام->payload = payload;
	پیام->qos = qos;
	پیام->retain = retain;
	پیام->properties = properties;

	DL_APPEND(db.plugin_msgs, پیام);

	loop__update_next_event(1);
	بازگشت MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحیح mosquitto_broker_publish_copy(
		ثابت نویسه *clientid,
		ثابت نویسه *topic,
		صحیح payloadlen,
		ثابت تهی *payload,
		صحیح qos,
		bool retain,
		mosquitto_property *properties)
{
	تهی *payload_out;
	صحیح rc;

	اگر(topic == پوچ
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == پوچ)
			|| qos < 0 || qos > 2){

		بازگشت MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (size_t)(payloadlen+1));
	اگر(payload_out == پوچ){
		بازگشت MOSQ_ERR_NOMEM;
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

	اگر(rc){
		mosquitto_FREE(payload_out);
	}
	بازگشت rc;
}


BROKER_EXPORT صحیح mosquitto_set_username(ساختار mosquitto *client, ثابت نویسه *username)
{
	نویسه *u_dup;
	نویسه *old;

	اگر(!client){
		بازگشت MOSQ_ERR_INVAL;
	}

	اگر(username){
		اگر(mosquitto_validate_utf8(username, (صحیح)strlen(username))){
			بازگشت MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		اگر(!u_dup){
			بازگشت MOSQ_ERR_NOMEM;
		}
	}وگرنه{
		u_dup = پوچ;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	بازگشت MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحیح mosquitto_set_clientid(ساختار mosquitto *client, ثابت نویسه *clientid)
{
	ساختار mosquitto *found_client;
	نویسه *id_dup;
	bool in_by_id;
	صحیح clientid_len;

	اگر(!client || !clientid){
		بازگشت MOSQ_ERR_INVAL;
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

	اگر(in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), found_client);
		اگر(found_client){
			بازگشت MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (صحیح)strlen(clientid);
	اگر(mosquitto_validate_utf8(clientid, clientid_len)){
		بازگشت MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	اگر(!id_dup){
		بازگشت MOSQ_ERR_NOMEM;
	}

	اگر(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	اگر(in_by_id){
		context__add_to_by_id(client);
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
ایستا تهی check_subscription_acls(ساختار mosquitto *context)
{
	صحیح rc;
	uint8_t reason;

	برای(صحیح i=0; i<context->subs_capacity; i++){
		اگر(context->subs[i] == پوچ){
			ادامه;
		}
		rc = mosquitto_acl_check(context,
				context->subs[i]->topic_filter,
				0,
				پوچ,
				0, /* FIXME */
				false,
				پوچ,
				MOSQ_ACL_SUBSCRIBE);

		اگر(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


ایستا تهی disconnect_client(ساختار mosquitto *context, bool with_will)
{
	اگر(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, پوچ);
	}
	اگر(with_will == false){
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	اگر(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT صحیح mosquitto_kick_client_by_clientid(ثابت نویسه *clientid, bool with_will)
{
	ساختار mosquitto *ctxt, *ctxt_tmp;

	اگر(clientid == پوچ){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		بازگشت MOSQ_ERR_SUCCESS;
	}وگرنه{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), ctxt);
		اگر(ctxt){
			disconnect_client(ctxt, with_will);
			بازگشت MOSQ_ERR_SUCCESS;
		}وگرنه{
			بازگشت MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT صحیح mosquitto_kick_client_by_username(ثابت نویسه *username, bool with_will)
{
	ساختار mosquitto *ctxt, *ctxt_tmp;

	اگر(username == پوچ){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			اگر(ctxt->username == پوچ){
				disconnect_client(ctxt, with_will);
			}
		}
	}وگرنه{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			اگر(ctxt->username != پوچ && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحیح mosquitto_apply_on_all_clients(صحیح (*FUNC_client_functor)(ثابت ساختار mosquitto *, تهی *), تهی *functor_context)
{
	صحیح rc = MOSQ_ERR_SUCCESS;
	ساختار mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		اگر(rc != MOSQ_ERR_SUCCESS){
			بشکن;
		}
	}

	بازگشت rc;
}


BROKER_EXPORT صحیح mosquitto_persist_client_add(ساختار mosquitto_client *client)
{
	ساختار mosquitto *context;
	صحیح rc;

	اگر(client == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}
	اگر(client->clientid == پوچ){
		rc = MOSQ_ERR_INVAL;
		برو error;
	}

	context = پوچ;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	اگر(context){
		rc = MOSQ_ERR_INVAL;
		برو error;
	}

	context = context__init();
	اگر(!context){
		rc = MOSQ_ERR_NOMEM;
		برو error;
	}

	context->id = client->clientid;
	client->clientid = پوچ;
	context->username = client->username;
	client->username = پوچ;
	context->auth_method = client->auth_method;
	client->auth_method = پوچ;

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
	اگر(db.config->per_listener_settings && client->listener_port > 0){
		برای(صحیح i=0; i < db.config->listener_شمار; i++){
			اگر(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				بشکن;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	بازگشت MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	بازگشت rc;
}


BROKER_EXPORT صحیح mosquitto_persist_client_update(ساختار mosquitto_client *client)
{
	ساختار mosquitto *context;
	صحیح rc;

	اگر(client == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}
	اگر(client->clientid == پوچ){
		rc = MOSQ_ERR_INVAL;
		برو error;
	}

	context = پوچ;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	اگر(context == پوچ){
		rc = MOSQ_ERR_NOT_FOUND;
		برو error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = پوچ;

	context->clean_start = false;
	context->will_delay_time = client->will_delay_time;
	context->session_expiry_time = client->session_expiry_time;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->maximum_packet_size = client->max_packet_size;
	context->retain_available = client->retain_available;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	اگر(db.config->per_listener_settings && client->listener_port > 0){
		برای(صحیح i=0; i < db.config->listener_شمار; i++){
			اگر(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				بشکن;
			}
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	بازگشت rc;
}


BROKER_EXPORT صحیح mosquitto_persist_client_delete(ثابت نویسه *clientid)
{
	ساختار mosquitto *context;

	اگر(clientid == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}

	context = پوچ;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	اگر(context == پوچ){
		بازگشت MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا ساختار mosquitto_base_پیام *find_store_پیام(uint64_t store_id)
{
	ساختار mosquitto_base_پیام *base_پیام;

	HASH_FIND(hh, db.پیام_store, &store_id, اندازه(store_id), base_پیام);
	بازگشت base_پیام;
}


BROKER_EXPORT صحیح mosquitto_persist_client_پیام_add(ساختار mosquitto_client_پیام_2 *client_پیام)
{
	ساختار mosquitto *context;
	ساختار mosquitto_base_پیام *base_پیام;

	اگر(client_پیام == پوچ || client_پیام->clientid == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_پیام->clientid, strlen(client_پیام->clientid), context);
	اگر(context == پوچ){
		بازگشت MOSQ_ERR_NOT_FOUND;
	}
	base_پیام = find_store_پیام(client_پیام->store_id);
	اگر(base_پیام == پوچ){
		بازگشت MOSQ_ERR_NOT_FOUND;
	}

	اگر(client_پیام->direction == mosq_md_out){
		اگر(client_پیام->qos > 0){
			context->last_mid = client_پیام->mid;
		}
		بازگشت db_message_درج_outgoing(context, client_پیام->cmsg_id, client_پیام->mid,
				client_پیام->qos, client_پیام->retain,
				base_پیام, client_پیام->subscription_identifier, false, false);
	}وگرنه اگر(client_پیام->direction == mosq_md_in){
		بازگشت db_message_درج_incoming(context, client_پیام->cmsg_id, base_پیام, false);
	}وگرنه{
		بازگشت MOSQ_ERR_INVAL;
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحیح mosquitto_persist_client_پیام_delete(ساختار mosquitto_client_پیام_2 *client_پیام)
{
	ساختار mosquitto *context;

	اگر(client_پیام == پوچ || client_پیام->clientid == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_پیام->clientid, strlen(client_پیام->clientid), context);
	اگر(context == پوچ){
		بازگشت MOSQ_ERR_NOT_FOUND;
	}


	صحیح rc = MOSQ_ERR_INVAL;
	اگر(client_پیام->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_پیام->mid, mosq_ms_any, client_پیام->qos);
	}وگرنه اگر(client_پیام->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_پیام->mid);
	}
	بازگشت rc;
}


BROKER_EXPORT صحیح mosquitto_persist_client_پیام_update(ساختار mosquitto_client_پیام_2 *client_پیام)
{
	ساختار mosquitto *context;

	اگر(client_پیام == پوچ || client_پیام->clientid == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_پیام->clientid, strlen(client_پیام->clientid), context);
	اگر(context == پوچ){
		بازگشت MOSQ_ERR_NOT_FOUND;
	}

	اگر(client_پیام->direction == mosq_md_out){
		db__message_update_outgoing(context, client_پیام->mid, client_پیام->state, client_پیام->qos, false);
	}وگرنه اگر(client_پیام->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}وگرنه{
		بازگشت MOSQ_ERR_INVAL;
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحیح mosquitto_persist_client_پیام_clear(ساختار mosquitto_client_پیام_2 *client_پیام)
{
	ساختار mosquitto *context;

	اگر(client_پیام == پوچ || client_پیام->clientid == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_پیام->clientid, strlen(client_پیام->clientid), context);
	اگر(context == پوچ){
		بازگشت MOSQ_ERR_NOT_FOUND;
	}

	اگر(client_پیام->direction == mosq_bmd_in || client_پیام->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	اگر(client_پیام->direction == mosq_bmd_out || client_پیام->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحیح mosquitto_subscription_add(ثابت ساختار mosquitto_subscription *sub)
{
	ساختار mosquitto *context;

	اگر(sub == پوچ || sub->clientid == پوچ || sub->topic_filter == پوچ || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		بازگشت MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, strlen(sub->clientid), context);

	اگر(context){
		بازگشت sub__add(context, sub);
	}وگرنه{
		بازگشت MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT صحیح mosquitto_subscription_delete(ثابت نویسه *clientid, ثابت نویسه *topic)
{
	ساختار mosquitto *context;
	uint8_t reason;

	اگر(clientid == پوچ || topic == پوچ || clientid[0] == '\0' || topic[0] == '\0'){
		بازگشت MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	اگر(context){
		بازگشت sub__remove(context, topic, &reason);
	}وگرنه{
		بازگشت MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT صحیح mosquitto_persist_base_پیام_add(ساختار mosquitto_base_پیام_2 *پیام_add)
{
	ساختار mosquitto context;
	ساختار mosquitto_base_پیام *base_پیام;
	صحیح rc;

	memset(&context, 0, اندازه(context));

	اگر(پیام_add->payloadlen > MQTT_MAX_PAYLOAD){
		بازگشت MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (نویسه *)پیام_add->source_id;
	context.username = (نویسه *)پیام_add->source_username;

	base_پیام = mosquitto_calloc(1, اندازه(ساختار mosquitto_base_پیام));
	اگر(base_پیام == پوچ){
		برو error;
	}
	base_پیام->data.store_id = پیام_add->store_id;
	base_پیام->data.expiry_time = پیام_add->expiry_time;
	base_پیام->data.payloadlen = پیام_add->payloadlen;
	base_پیام->data.source_mid = پیام_add->source_mid;
	base_پیام->data.qos = پیام_add->qos;
	base_پیام->data.retain = پیام_add->retain;

	base_پیام->data.payload = پیام_add->payload;
	پیام_add->payload = پوچ;
	base_پیام->data.topic = پیام_add->topic;
	پیام_add->topic = پوچ;
	base_پیام->data.properties = پیام_add->properties;
	پیام_add->properties = پوچ;

	اگر(پیام_add->source_port){
		برای(صحیح i=0; i<db.config->listener_شمار; i++){
			اگر(db.config->listeners[i].port == پیام_add->source_port){
				base_پیام->source_listener = &db.config->listeners[i];
				بشکن;
			}
		}
	}

	base_پیام->stored = true;
	rc = db__message_store(&context, base_پیام, پوچ, mosq_mo_broker);
	بازگشت rc;

error:
	mosquitto_property_free_all(&پیام_add->properties);
	mosquitto_free(پیام_add->topic);
	mosquitto_free(پیام_add->payload);
	mosquitto_free(base_پیام);

	بازگشت MOSQ_ERR_NOMEM;
}


BROKER_EXPORT صحیح mosquitto_persist_base_پیام_delete(uint64_t store_id)
{
	ساختار mosquitto_base_پیام *base_پیام;

	base_پیام = find_store_پیام(store_id);
	اگر(base_پیام && base_پیام->ref_شمار == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_پیام_store_remove(base_پیام, false);
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT تهی mosquitto_complete_basic_auth(ثابت نویسه *clientid, صحیح result)
{
	ساختار mosquitto *context;

	اگر(clientid == پوچ){
		بازگشت;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, strlen(clientid), context);
	اگر(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		اگر(result == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, پوچ, 0);
		}وگرنه{
			اگر(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, پوچ);
			}وگرنه{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, پوچ);
			}
			context->clean_start = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT صحیح mosquitto_broker_گره_id_set(uint16_t id)
{
	اگر(id > 1023){
		بازگشت MOSQ_ERR_INVAL;
	}وگرنه{
		db.گره_id = id;
		db.گره_id_shifted = ((uint64_t)id) << 54;
		بازگشت MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT ثابت نویسه *mosquitto_persistence_location(تهی)
{
	بازگشت db.config->persistence_location;
}


BROKER_EXPORT صحیح mosquitto_client_will_set(ثابت نویسه *clientid, ثابت نویسه *topic, صحیح payloadlen, ثابت تهی *payload, صحیح qos, bool retain, mosquitto_property *properties)
{
	ساختار mosquitto *mosq = mosquitto_client(clientid);
	اگر(!mosq){
		بازگشت MOSQ_ERR_NOT_FOUND;
	}
	اگر(properties && mosq->protocol != mosq_p_mqtt5){
		اگر(net__is_connected(mosq)){
			بازگشت MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	بازگشت will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
