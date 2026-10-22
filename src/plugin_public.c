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

#include "config.h"

#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "send_mosq.h"
#include "util_mosq.h"
#include "will_mosq.h"
#include "utlist.h"
#include "will_mosq.h"

#ifdef WITH_TLS
#  include <openssl/ssl.h>
#endif


BROKER_EXPORT цяло mosquitto_plugin_set_info(mosquitto_plugin_id_t *identifier,
		const char *plugin_name,
		const char *plugin_version)
{
	ако(identifier == NULL || plugin_name == NULL){
		върни MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	ако(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}иначе{
		identifier->plugin_version = NULL;
	}

	върни MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT const char *mosquitto_client_address(const struct mosquitto *client)
{
	ако(client){
		върни client->address;
	}иначе{
		върни NULL;
	}
}


BROKER_EXPORT struct mosquitto *mosquitto_client(const char *clientid)
{
	size_t len;
	struct mosquitto *context;

	ако(!clientid){
		върни NULL;
	}
	len = strlen(clientid);
	ако(len == 0){
		върни NULL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	върни context;
}


BROKER_EXPORT цяло mosquitto_client_port(const struct mosquitto *client)
{
	ако(client && client->listener){
		върни client->listener->port;
	}иначе{
		върни 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(const struct mosquitto *client)
{
	ако(client){
		върни client->clean_start;
	}иначе{
		върни true;
	}
}


BROKER_EXPORT const char *mosquitto_client_id(const struct mosquitto *client)
{
	ако(client){
		върни client->id;
	}иначе{
		върни NULL;
	}
}


BROKER_EXPORT unsigned mosquitto_client_id_hashv(const struct mosquitto *client)
{
	ако(client){
		върни client->id_hashv;
	}иначе{
		върни 0;
	}
}


BROKER_EXPORT цяло mosquitto_client_keepalive(const struct mosquitto *client)
{
	ако(client){
		върни client->keepalive;
	}иначе{
		върни -1;
	}
}


BROKER_EXPORT void *mosquitto_client_certificate(const struct mosquitto *client)
{
#ifdef WITH_TLS
	ако(client && client->ssl){
		върни SSL_get_peer_certificate(client->ssl);
	}иначе{
		върни NULL;
	}
#иначе
	UNUSED(client);

	върни NULL;
#endif
}


BROKER_EXPORT цяло mosquitto_client_protocol(const struct mosquitto *client)
{
#ако defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ако(client && client->wsi){
		върни mp_websockets;
	}иначе
#elif defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	ако(client && client->transport == mosq_t_ws){
		върни mp_websockets;
	}иначе
#иначе
	UNUSED(client);
#endif
	{
		върни mp_mqtt;
	}
}


BROKER_EXPORT цяло mosquitto_client_protocol_version(const struct mosquitto *client)
{
	ако(client){
		switch(client->protocol){
			case mosq_p_mqtt31:
				върни 3;
			case mosq_p_mqtt311:
				върни 4;
			case mosq_p_mqtt5:
				върни 5;
			default:
				върни 0;
		}
	}иначе{
		върни 0;
	}
}


BROKER_EXPORT цяло mosquitto_client_sub_count(const struct mosquitto *client)
{
	ако(client){
		върни client->subs_count;
	}иначе{
		върни 0;
	}
}


BROKER_EXPORT const char *mosquitto_client_username(const struct mosquitto *client)
{
	ако(client){
#ifdef WITH_BRIDGE
		ако(client->bridge){
			върни client->bridge->local_username;
		}иначе
#endif
		{
			върни client->username;
		}
	}иначе{
		върни NULL;
	}
}


BROKER_EXPORT цяло mosquitto_broker_publish(
		const char *clientid,
		const char *topic,
		цяло payloadlen,
		void *payload,
		цяло qos,
		bool retain,
		mosquitto_property *properties)
{
	struct mosquitto__message_v5 *msg;

	ако(topic == NULL
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == NULL)
			|| qos < 0 || qos > 2){

		върни MOSQ_ERR_INVAL;
	}

	msg = mosquitto_malloc(sizeof(struct mosquitto__message_v5));
	ако(msg == NULL){
		върни MOSQ_ERR_NOMEM;
	}

	msg->next = NULL;
	msg->prev = NULL;
	ако(clientid){
		msg->clientid = mosquitto_strdup(clientid);
		ако(msg->clientid == NULL){
			mosquitto_FREE(msg);
			върни MOSQ_ERR_NOMEM;
		}
	}иначе{
		msg->clientid = NULL;
	}
	msg->topic = mosquitto_strdup(topic);
	ако(msg->topic == NULL){
		mosquitto_FREE(msg->clientid);
		mosquitto_FREE(msg);
		върни MOSQ_ERR_NOMEM;
	}
	msg->payloadlen = payloadlen;
	msg->payload = payload;
	msg->qos = qos;
	msg->retain = retain;
	msg->properties = properties;

	DL_APPEND(db.plugin_msgs, msg);

	loop__update_next_event(1);
	върни MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT цяло mosquitto_broker_publish_copy(
		const char *clientid,
		const char *topic,
		цяло payloadlen,
		const void *payload,
		цяло qos,
		bool retain,
		mosquitto_property *properties)
{
	void *payload_out;
	цяло rc;

	ако(topic == NULL
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == NULL)
			|| qos < 0 || qos > 2){

		върни MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (size_t)(payloadlen+1));
	ако(payload_out == NULL){
		върни MOSQ_ERR_NOMEM;
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

	ако(rc){
		mosquitto_FREE(payload_out);
	}
	върни rc;
}


BROKER_EXPORT цяло mosquitto_set_username(struct mosquitto *client, const char *username)
{
	char *u_dup;
	char *old;

	ако(!client){
		върни MOSQ_ERR_INVAL;
	}

	ако(username){
		ако(mosquitto_validate_utf8(username, (цяло)strlen(username))){
			върни MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		ако(!u_dup){
			върни MOSQ_ERR_NOMEM;
		}
	}иначе{
		u_dup = NULL;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	върни MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT цяло mosquitto_set_clientid(struct mosquitto *client, const char *clientid)
{
	struct mosquitto *found_client;
	char *id_dup;
	bool in_by_id;
	цяло clientid_len;

	ако(!client || !clientid){
		върни MOSQ_ERR_INVAL;
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

	ако(in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), found_client);
		ако(found_client){
			върни MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (цяло)strlen(clientid);
	ако(mosquitto_validate_utf8(clientid, clientid_len)){
		върни MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	ако(!id_dup){
		върни MOSQ_ERR_NOMEM;
	}

	ако(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	ако(in_by_id){
		context__add_to_by_id(client);
	}

	върни MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
static void check_subscription_acls(struct mosquitto *context)
{
	цяло rc;
	uint8_t reason;

	за(цяло i=0; i<context->subs_capacity; i++){
		ако(context->subs[i] == NULL){
			continue;
		}
		rc = mosquitto_acl_check(context,
				context->subs[i]->topic_filter,
				0,
				NULL,
				0, /* FIXME */
				false,
				NULL,
				MOSQ_ACL_SUBSCRIBE);

		ако(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


static void disconnect_client(struct mosquitto *context, bool with_will)
{
	ако(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, NULL);
	}
	ако(with_will == false){
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	ако(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT цяло mosquitto_kick_client_by_clientid(const char *clientid, bool with_will)
{
	struct mosquitto *ctxt, *ctxt_tmp;

	ако(clientid == NULL){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		върни MOSQ_ERR_SUCCESS;
	}иначе{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), ctxt);
		ако(ctxt){
			disconnect_client(ctxt, with_will);
			върни MOSQ_ERR_SUCCESS;
		}иначе{
			върни MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT цяло mosquitto_kick_client_by_username(const char *username, bool with_will)
{
	struct mosquitto *ctxt, *ctxt_tmp;

	ако(username == NULL){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			ако(ctxt->username == NULL){
				disconnect_client(ctxt, with_will);
			}
		}
	}иначе{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			ако(ctxt->username != NULL && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	върни MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT цяло mosquitto_apply_on_all_clients(цяло (*FUNC_client_functor)(const struct mosquitto *, void *), void *functor_context)
{
	цяло rc = MOSQ_ERR_SUCCESS;
	struct mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		ако(rc != MOSQ_ERR_SUCCESS){
			break;
		}
	}

	върни rc;
}


BROKER_EXPORT цяло mosquitto_persist_client_add(struct mosquitto_client *client)
{
	struct mosquitto *context;
	цяло rc;

	ако(client == NULL){
		върни MOSQ_ERR_INVAL;
	}
	ако(client->clientid == NULL){
		rc = MOSQ_ERR_INVAL;
		goto error;
	}

	context = NULL;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	ако(context){
		rc = MOSQ_ERR_INVAL;
		goto error;
	}

	context = context__init();
	ако(!context){
		rc = MOSQ_ERR_NOMEM;
		goto error;
	}

	context->id = client->clientid;
	client->clientid = NULL;
	context->username = client->username;
	client->username = NULL;
	context->auth_method = client->auth_method;
	client->auth_method = NULL;

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
	ако(db.config->per_listener_settings && client->listener_port > 0){
		за(цяло i=0; i < db.config->listener_count; i++){
			ако(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				break;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	върни MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	върни rc;
}


BROKER_EXPORT цяло mosquitto_persist_client_update(struct mosquitto_client *client)
{
	struct mosquitto *context;
	цяло rc;

	ако(client == NULL){
		върни MOSQ_ERR_INVAL;
	}
	ако(client->clientid == NULL){
		rc = MOSQ_ERR_INVAL;
		goto error;
	}

	context = NULL;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	ако(context == NULL){
		rc = MOSQ_ERR_NOT_FOUND;
		goto error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = NULL;

	context->clean_start = false;
	context->will_delay_time = client->will_delay_time;
	context->session_expiry_time = client->session_expiry_time;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->maximum_packet_size = client->max_packet_size;
	context->retain_available = client->retain_available;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	ако(db.config->per_listener_settings && client->listener_port > 0){
		за(цяло i=0; i < db.config->listener_count; i++){
			ако(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				break;
			}
		}
	}

	върни MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	върни rc;
}


BROKER_EXPORT цяло mosquitto_persist_client_delete(const char *clientid)
{
	struct mosquitto *context;

	ако(clientid == NULL){
		върни MOSQ_ERR_INVAL;
	}

	context = NULL;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	ако(context == NULL){
		върни MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	върни MOSQ_ERR_SUCCESS;
}


static struct mosquitto__base_msg *find_store_msg(uint64_t store_id)
{
	struct mosquitto__base_msg *base_msg;

	HASH_FIND(hh, db.msg_store, &store_id, sizeof(store_id), base_msg);
	върни base_msg;
}


BROKER_EXPORT цяло mosquitto_persist_client_msg_add(struct mosquitto_client_msg *client_msg)
{
	struct mosquitto *context;
	struct mosquitto__base_msg *base_msg;

	ако(client_msg == NULL || client_msg->clientid == NULL){
		върни MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	ако(context == NULL){
		върни MOSQ_ERR_NOT_FOUND;
	}
	base_msg = find_store_msg(client_msg->store_id);
	ако(base_msg == NULL){
		върни MOSQ_ERR_NOT_FOUND;
	}

	ако(client_msg->direction == mosq_md_out){
		ако(client_msg->qos > 0){
			context->last_mid = client_msg->mid;
		}
		върни db__message_insert_outgoing(context, client_msg->cmsg_id, client_msg->mid,
				client_msg->qos, client_msg->retain,
				base_msg, client_msg->subscription_identifier, false, false);
	}иначе ако(client_msg->direction == mosq_md_in){
		върни db__message_insert_incoming(context, client_msg->cmsg_id, base_msg, false);
	}иначе{
		върни MOSQ_ERR_INVAL;
	}
	върни MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT цяло mosquitto_persist_client_msg_delete(struct mosquitto_client_msg *client_msg)
{
	struct mosquitto *context;

	ако(client_msg == NULL || client_msg->clientid == NULL){
		върни MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	ако(context == NULL){
		върни MOSQ_ERR_NOT_FOUND;
	}


	цяло rc = MOSQ_ERR_INVAL;
	ако(client_msg->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_msg->mid, mosq_ms_any, client_msg->qos);
	}иначе ако(client_msg->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_msg->mid);
	}
	върни rc;
}


BROKER_EXPORT цяло mosquitto_persist_client_msg_update(struct mosquitto_client_msg *client_msg)
{
	struct mosquitto *context;

	ако(client_msg == NULL || client_msg->clientid == NULL){
		върни MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	ако(context == NULL){
		върни MOSQ_ERR_NOT_FOUND;
	}

	ако(client_msg->direction == mosq_md_out){
		db__message_update_outgoing(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}иначе ако(client_msg->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}иначе{
		върни MOSQ_ERR_INVAL;
	}
	върни MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT цяло mosquitto_persist_client_msg_clear(struct mosquitto_client_msg *client_msg)
{
	struct mosquitto *context;

	ако(client_msg == NULL || client_msg->clientid == NULL){
		върни MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	ако(context == NULL){
		върни MOSQ_ERR_NOT_FOUND;
	}

	ако(client_msg->direction == mosq_bmd_in || client_msg->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	ако(client_msg->direction == mosq_bmd_out || client_msg->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	върни MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT цяло mosquitto_subscription_add(const struct mosquitto_subscription *sub)
{
	struct mosquitto *context;

	ако(sub == NULL || sub->clientid == NULL || sub->topic_filter == NULL || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		върни MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, strlen(sub->clientid), context);

	ако(context){
		върни sub__add(context, sub);
	}иначе{
		върни MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT цяло mosquitto_subscription_delete(const char *clientid, const char *topic)
{
	struct mosquitto *context;
	uint8_t reason;

	ако(clientid == NULL || topic == NULL || clientid[0] == '\0' || topic[0] == '\0'){
		върни MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	ако(context){
		върни sub__remove(context, topic, &reason);
	}иначе{
		върни MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT цяло mosquitto_persist_base_msg_add(struct mosquitto_base_msg *msg_add)
{
	struct mosquitto context;
	struct mosquitto__base_msg *base_msg;
	цяло rc;

	memset(&context, 0, sizeof(context));

	ако(msg_add->payloadlen > MQTT_MAX_PAYLOAD){
		върни MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (char *)msg_add->source_id;
	context.username = (char *)msg_add->source_username;

	base_msg = mosquitto_calloc(1, sizeof(struct mosquitto__base_msg));
	ако(base_msg == NULL){
		goto error;
	}
	base_msg->data.store_id = msg_add->store_id;
	base_msg->data.expiry_time = msg_add->expiry_time;
	base_msg->data.payloadlen = msg_add->payloadlen;
	base_msg->data.source_mid = msg_add->source_mid;
	base_msg->data.qos = msg_add->qos;
	base_msg->data.retain = msg_add->retain;

	base_msg->data.payload = msg_add->payload;
	msg_add->payload = NULL;
	base_msg->data.topic = msg_add->topic;
	msg_add->topic = NULL;
	base_msg->data.properties = msg_add->properties;
	msg_add->properties = NULL;

	ако(msg_add->source_port){
		за(цяло i=0; i<db.config->listener_count; i++){
			ако(db.config->listeners[i].port == msg_add->source_port){
				base_msg->source_listener = &db.config->listeners[i];
				break;
			}
		}
	}

	base_msg->stored = true;
	rc = db__message_store(&context, base_msg, NULL, mosq_mo_broker);
	върни rc;

error:
	mosquitto_property_free_all(&msg_add->properties);
	mosquitto_free(msg_add->topic);
	mosquitto_free(msg_add->payload);
	mosquitto_free(base_msg);

	върни MOSQ_ERR_NOMEM;
}


BROKER_EXPORT цяло mosquitto_persist_base_msg_delete(uint64_t store_id)
{
	struct mosquitto__base_msg *base_msg;

	base_msg = find_store_msg(store_id);
	ако(base_msg && base_msg->ref_count == 0){
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

	върни MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT void mosquitto_complete_basic_auth(const char *clientid, цяло result)
{
	struct mosquitto *context;

	ако(clientid == NULL){
		върни;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, strlen(clientid), context);
	ако(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		ако(result == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, NULL, 0);
		}иначе{
			ако(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, NULL);
			}иначе{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, NULL);
			}
			context->clean_start = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT цяло mosquitto_broker_node_id_set(uint16_t id)
{
	ако(id > 1023){
		върни MOSQ_ERR_INVAL;
	}иначе{
		db.node_id = id;
		db.node_id_shifted = ((uint64_t)id) << 54;
		върни MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT const char *mosquitto_persistence_location(void)
{
	върни db.config->persistence_location;
}


BROKER_EXPORT цяло mosquitto_client_will_set(const char *clientid, const char *topic, цяло payloadlen, const void *payload, цяло qos, bool retain, mosquitto_property *properties)
{
	struct mosquitto *mosq = mosquitto_client(clientid);
	ако(!mosq){
		върни MOSQ_ERR_NOT_FOUND;
	}
	ако(properties && mosq->protocol != mosq_p_mqtt5){
		ако(net__is_connected(mosq)){
			върни MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	върни will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
