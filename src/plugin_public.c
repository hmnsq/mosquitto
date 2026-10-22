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
	إذا(identifier == عدم || plugin_name == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	إذا(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}وإلا{
		identifier->plugin_version = عدم;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ثابت حرف *mosquitto_client_address(ثابت بنية mosquitto *client)
{
	إذا(client){
		أرجع client->address;
	}وإلا{
		أرجع عدم;
	}
}


BROKER_EXPORT بنية mosquitto *mosquitto_client(ثابت حرف *clientid)
{
	size_t len;
	بنية mosquitto *context;

	إذا(!clientid){
		أرجع عدم;
	}
	len = strlen(clientid);
	إذا(len == 0){
		أرجع عدم;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	أرجع context;
}


BROKER_EXPORT صحيح mosquitto_client_port(ثابت بنية mosquitto *client)
{
	إذا(client && client->listener){
		أرجع client->listener->port;
	}وإلا{
		أرجع 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(ثابت بنية mosquitto *client)
{
	إذا(client){
		أرجع client->clean_start;
	}وإلا{
		أرجع true;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_client_id(ثابت بنية mosquitto *client)
{
	إذا(client){
		أرجع client->id;
	}وإلا{
		أرجع عدم;
	}
}


BROKER_EXPORT unsigned mosquitto_client_id_hashv(ثابت بنية mosquitto *client)
{
	إذا(client){
		أرجع client->id_hashv;
	}وإلا{
		أرجع 0;
	}
}


BROKER_EXPORT صحيح mosquitto_client_keepalive(ثابت بنية mosquitto *client)
{
	إذا(client){
		أرجع client->keepalive;
	}وإلا{
		أرجع -1;
	}
}


BROKER_EXPORT فراغ *mosquitto_client_certificate(ثابت بنية mosquitto *client)
{
#ifdef WITH_TLS
	إذا(client && client->ssl){
		أرجع SSL_get_peer_certificate(client->ssl);
	}وإلا{
		أرجع عدم;
	}
#وإلا
	UNUSED(client);

	أرجع عدم;
#endif
}


BROKER_EXPORT صحيح mosquitto_client_protocol(ثابت بنية mosquitto *client)
{
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	إذا(client && client->wsi){
		أرجع mp_websockets;
	}وإلا
#وإلاإذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	إذا(client && client->transport == mosq_t_ws){
		أرجع mp_websockets;
	}وإلا
#وإلا
	UNUSED(client);
#endif
	{
		أرجع mp_mqtt;
	}
}


BROKER_EXPORT صحيح mosquitto_client_protocol_version(ثابت بنية mosquitto *client)
{
	إذا(client){
		بدل(client->protocol){
			حالة mosq_p_mqtt31:
				أرجع 3;
			حالة mosq_p_mqtt311:
				أرجع 4;
			حالة mosq_p_mqtt5:
				أرجع 5;
			افتراضي:
				أرجع 0;
		}
	}وإلا{
		أرجع 0;
	}
}


BROKER_EXPORT صحيح mosquitto_client_sub_کيراءن(ثابت بنية mosquitto *client)
{
	إذا(client){
		أرجع client->subs_کيراءن;
	}وإلا{
		أرجع 0;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_client_username(ثابت بنية mosquitto *client)
{
	إذا(client){
#ifdef WITH_BRIDGE
		إذا(client->bridge){
			أرجع client->bridge->local_username;
		}وإلا
#endif
		{
			أرجع client->username;
		}
	}وإلا{
		أرجع عدم;
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
	بنية mosquitto__message_v5 *ڤسنن;

	إذا(topic == عدم
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == عدم)
			|| qos < 0 || qos > 2){

		أرجع MOSQ_ERR_INVAL;
	}

	ڤسنن = mosquitto_malloc(حجم(بنية mosquitto__message_v5));
	إذا(ڤسنن == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	ڤسنن->next = عدم;
	ڤسنن->prev = عدم;
	إذا(clientid){
		ڤسنن->clientid = mosquitto_strdup(clientid);
		إذا(ڤسنن->clientid == عدم){
			mosquitto_FREE(ڤسنن);
			أرجع MOSQ_ERR_NOMEM;
		}
	}وإلا{
		ڤسنن->clientid = عدم;
	}
	ڤسنن->topic = mosquitto_strdup(topic);
	إذا(ڤسنن->topic == عدم){
		mosquitto_FREE(ڤسنن->clientid);
		mosquitto_FREE(ڤسنن);
		أرجع MOSQ_ERR_NOMEM;
	}
	ڤسنن->payloadlen = payloadlen;
	ڤسنن->payload = payload;
	ڤسنن->qos = qos;
	ڤسنن->retain = retain;
	ڤسنن->properties = properties;

	DL_APPEND(db.plugin_msgs, ڤسنن);

	loop__update_next_event(1);
	أرجع MOSQ_ERR_SUCCESS;
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

	إذا(topic == عدم
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == عدم)
			|| qos < 0 || qos > 2){

		أرجع MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (size_t)(payloadlen+1));
	إذا(payload_out == عدم){
		أرجع MOSQ_ERR_NOMEM;
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

	إذا(rc){
		mosquitto_FREE(payload_out);
	}
	أرجع rc;
}


BROKER_EXPORT صحيح mosquitto_set_username(بنية mosquitto *client, ثابت حرف *username)
{
	حرف *u_dup;
	حرف *old;

	إذا(!client){
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(username){
		إذا(mosquitto_validate_utf8(username, (صحيح)strlen(username))){
			أرجع MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		إذا(!u_dup){
			أرجع MOSQ_ERR_NOMEM;
		}
	}وإلا{
		u_dup = عدم;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_set_clientid(بنية mosquitto *client, ثابت حرف *clientid)
{
	بنية mosquitto *found_client;
	حرف *id_dup;
	bool in_by_id;
	صحيح clientid_len;

	إذا(!client || !clientid){
		أرجع MOSQ_ERR_INVAL;
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

	إذا(in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), found_client);
		إذا(found_client){
			أرجع MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (صحيح)strlen(clientid);
	إذا(mosquitto_validate_utf8(clientid, clientid_len)){
		أرجع MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	إذا(!id_dup){
		أرجع MOSQ_ERR_NOMEM;
	}

	إذا(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	إذا(in_by_id){
		context__add_to_by_id(client);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
ساكن فراغ check_subscription_acls(بنية mosquitto *context)
{
	صحيح rc;
	uint8_t reason;

	لكل(صحيح i=0; i<context->subs_capacity; i++){
		إذا(context->subs[i] == عدم){
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

		إذا(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


ساكن فراغ disconnect_client(بنية mosquitto *context, bool with_will)
{
	إذا(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, عدم);
	}
	إذا(with_will == false){
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	إذا(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT صحيح mosquitto_kick_client_by_clientid(ثابت حرف *clientid, bool with_will)
{
	بنية mosquitto *ctxt, *ctxt_tmp;

	إذا(clientid == عدم){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), ctxt);
		إذا(ctxt){
			disconnect_client(ctxt, with_will);
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			أرجع MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT صحيح mosquitto_kick_client_by_username(ثابت حرف *username, bool with_will)
{
	بنية mosquitto *ctxt, *ctxt_tmp;

	إذا(username == عدم){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			إذا(ctxt->username == عدم){
				disconnect_client(ctxt, with_will);
			}
		}
	}وإلا{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			إذا(ctxt->username != عدم && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_apply_on_all_clients(صحيح (*FUNC_client_functor)(ثابت بنية mosquitto *, فراغ *), فراغ *functor_context)
{
	صحيح rc = MOSQ_ERR_SUCCESS;
	بنية mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		إذا(rc != MOSQ_ERR_SUCCESS){
			اكسر;
		}
	}

	أرجع rc;
}


BROKER_EXPORT صحيح mosquitto_persist_client_add(بنية mosquitto_client *client)
{
	بنية mosquitto *context;
	صحيح rc;

	إذا(client == عدم){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(client->clientid == عدم){
		rc = MOSQ_ERR_INVAL;
		اذهب error;
	}

	context = عدم;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	إذا(context){
		rc = MOSQ_ERR_INVAL;
		اذهب error;
	}

	context = context__init();
	إذا(!context){
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
	إذا(db.config->per_listener_settings && client->listener_port > 0){
		لكل(صحيح i=0; i < db.config->listener_کيراءن; i++){
			إذا(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	أرجع MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	أرجع rc;
}


BROKER_EXPORT صحيح mosquitto_persist_client_update(بنية mosquitto_client *client)
{
	بنية mosquitto *context;
	صحيح rc;

	إذا(client == عدم){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(client->clientid == عدم){
		rc = MOSQ_ERR_INVAL;
		اذهب error;
	}

	context = عدم;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	إذا(context == عدم){
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
	إذا(db.config->per_listener_settings && client->listener_port > 0){
		لكل(صحيح i=0; i < db.config->listener_کيراءن; i++){
			إذا(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	أرجع rc;
}


BROKER_EXPORT صحيح mosquitto_persist_client_delete(ثابت حرف *clientid)
{
	بنية mosquitto *context;

	إذا(clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	context = عدم;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن بنية mosquitto_base_ڤسنن *find_store_ڤسنن(uint64_t store_id)
{
	بنية mosquitto_base_ڤسنن *base_ڤسنن;

	HASH_FIND(hh, db.ڤسنن_store, &store_id, حجم(store_id), base_ڤسنن);
	أرجع base_ڤسنن;
}


BROKER_EXPORT صحيح mosquitto_persist_client_ڤسنن_add(بنية mosquitto_client_ڤسنن_2 *client_ڤسنن)
{
	بنية mosquitto *context;
	بنية mosquitto_base_ڤسنن *base_ڤسنن;

	إذا(client_ڤسنن == عدم || client_ڤسنن->clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_ڤسنن->clientid, strlen(client_ڤسنن->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_FOUND;
	}
	base_ڤسنن = find_store_ڤسنن(client_ڤسنن->store_id);
	إذا(base_ڤسنن == عدم){
		أرجع MOSQ_ERR_NOT_FOUND;
	}

	إذا(client_ڤسنن->direction == mosq_md_out){
		إذا(client_ڤسنن->qos > 0){
			context->last_mid = client_ڤسنن->mid;
		}
		أرجع db_message_سيسيڤ_outgoing(context, client_ڤسنن->cmsg_id, client_ڤسنن->mid,
				client_ڤسنن->qos, client_ڤسنن->retain,
				base_ڤسنن, client_ڤسنن->subscription_identifier, false, false);
	}وإلا إذا(client_ڤسنن->direction == mosq_md_in){
		أرجع db_message_سيسيڤ_incoming(context, client_ڤسنن->cmsg_id, base_ڤسنن, false);
	}وإلا{
		أرجع MOSQ_ERR_INVAL;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_persist_client_ڤسنن_delete(بنية mosquitto_client_ڤسنن_2 *client_ڤسنن)
{
	بنية mosquitto *context;

	إذا(client_ڤسنن == عدم || client_ڤسنن->clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_ڤسنن->clientid, strlen(client_ڤسنن->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_FOUND;
	}


	صحيح rc = MOSQ_ERR_INVAL;
	إذا(client_ڤسنن->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_ڤسنن->mid, mosq_ms_any, client_ڤسنن->qos);
	}وإلا إذا(client_ڤسنن->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_ڤسنن->mid);
	}
	أرجع rc;
}


BROKER_EXPORT صحيح mosquitto_persist_client_ڤسنن_update(بنية mosquitto_client_ڤسنن_2 *client_ڤسنن)
{
	بنية mosquitto *context;

	إذا(client_ڤسنن == عدم || client_ڤسنن->clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_ڤسنن->clientid, strlen(client_ڤسنن->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_FOUND;
	}

	إذا(client_ڤسنن->direction == mosq_md_out){
		db__message_update_outgoing(context, client_ڤسنن->mid, client_ڤسنن->state, client_ڤسنن->qos, false);
	}وإلا إذا(client_ڤسنن->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}وإلا{
		أرجع MOSQ_ERR_INVAL;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_persist_client_ڤسنن_clear(بنية mosquitto_client_ڤسنن_2 *client_ڤسنن)
{
	بنية mosquitto *context;

	إذا(client_ڤسنن == عدم || client_ڤسنن->clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_ڤسنن->clientid, strlen(client_ڤسنن->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_FOUND;
	}

	إذا(client_ڤسنن->direction == mosq_bmd_in || client_ڤسنن->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	إذا(client_ڤسنن->direction == mosq_bmd_out || client_ڤسنن->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_subscription_add(ثابت بنية mosquitto_subscription *sub)
{
	بنية mosquitto *context;

	إذا(sub == عدم || sub->clientid == عدم || sub->topic_filter == عدم || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, strlen(sub->clientid), context);

	إذا(context){
		أرجع sub__add(context, sub);
	}وإلا{
		أرجع MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT صحيح mosquitto_subscription_delete(ثابت حرف *clientid, ثابت حرف *topic)
{
	بنية mosquitto *context;
	uint8_t reason;

	إذا(clientid == عدم || topic == عدم || clientid[0] == '\0' || topic[0] == '\0'){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	إذا(context){
		أرجع sub__remove(context, topic, &reason);
	}وإلا{
		أرجع MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT صحيح mosquitto_persist_base_ڤسنن_add(بنية mosquitto_base_ڤسنن_2 *ڤسنن_add)
{
	بنية mosquitto context;
	بنية mosquitto_base_ڤسنن *base_ڤسنن;
	صحيح rc;

	memset(&context, 0, حجم(context));

	إذا(ڤسنن_add->payloadlen > MQTT_MAX_PAYLOAD){
		أرجع MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (حرف *)ڤسنن_add->source_id;
	context.username = (حرف *)ڤسنن_add->source_username;

	base_ڤسنن = mosquitto_calloc(1, حجم(بنية mosquitto_base_ڤسنن));
	إذا(base_ڤسنن == عدم){
		اذهب error;
	}
	base_ڤسنن->data.store_id = ڤسنن_add->store_id;
	base_ڤسنن->data.expiry_time = ڤسنن_add->expiry_time;
	base_ڤسنن->data.payloadlen = ڤسنن_add->payloadlen;
	base_ڤسنن->data.source_mid = ڤسنن_add->source_mid;
	base_ڤسنن->data.qos = ڤسنن_add->qos;
	base_ڤسنن->data.retain = ڤسنن_add->retain;

	base_ڤسنن->data.payload = ڤسنن_add->payload;
	ڤسنن_add->payload = عدم;
	base_ڤسنن->data.topic = ڤسنن_add->topic;
	ڤسنن_add->topic = عدم;
	base_ڤسنن->data.properties = ڤسنن_add->properties;
	ڤسنن_add->properties = عدم;

	إذا(ڤسنن_add->source_port){
		لكل(صحيح i=0; i<db.config->listener_کيراءن; i++){
			إذا(db.config->listeners[i].port == ڤسنن_add->source_port){
				base_ڤسنن->source_listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	base_ڤسنن->stored = true;
	rc = db__message_store(&context, base_ڤسنن, عدم, mosq_mo_broker);
	أرجع rc;

error:
	mosquitto_property_free_all(&ڤسنن_add->properties);
	mosquitto_free(ڤسنن_add->topic);
	mosquitto_free(ڤسنن_add->payload);
	mosquitto_free(base_ڤسنن);

	أرجع MOSQ_ERR_NOMEM;
}


BROKER_EXPORT صحيح mosquitto_persist_base_ڤسنن_delete(uint64_t store_id)
{
	بنية mosquitto_base_ڤسنن *base_ڤسنن;

	base_ڤسنن = find_store_ڤسنن(store_id);
	إذا(base_ڤسنن && base_ڤسنن->ref_کيراءن == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_ڤسنن_store_remove(base_ڤسنن, false);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT فراغ mosquitto_complete_basic_auth(ثابت حرف *clientid, صحيح result)
{
	بنية mosquitto *context;

	إذا(clientid == عدم){
		أرجع;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, strlen(clientid), context);
	إذا(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		إذا(result == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, عدم, 0);
		}وإلا{
			إذا(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, عدم);
			}وإلا{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, عدم);
			}
			context->clean_start = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT صحيح mosquitto_broker_نود_id_set(uint16_t id)
{
	إذا(id > 1023){
		أرجع MOSQ_ERR_INVAL;
	}وإلا{
		db.نود_id = id;
		db.نود_id_shifted = ((uint64_t)id) << 54;
		أرجع MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_persistence_location(فراغ)
{
	أرجع db.config->persistence_location;
}


BROKER_EXPORT صحيح mosquitto_client_will_set(ثابت حرف *clientid, ثابت حرف *topic, صحيح payloadlen, ثابت فراغ *payload, صحيح qos, bool retain, mosquitto_property *properties)
{
	بنية mosquitto *mosq = mosquitto_client(clientid);
	إذا(!mosq){
		أرجع MOSQ_ERR_NOT_FOUND;
	}
	إذا(properties && mosq->protocol != mosq_p_mqtt5){
		إذا(net__is_connected(mosq)){
			أرجع MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	أرجع will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
