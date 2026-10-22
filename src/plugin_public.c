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


BROKER_EXPORT 整数 mosquitto_plugin_set_info(mosquitto_plugin_id_t *identifier,
		const char *plugin_name,
		const char *plugin_version)
{
	如果(identifier == NULL || plugin_name == NULL){
		返回 MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	如果(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}否则{
		identifier->plugin_version = NULL;
	}

	返回 MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT const char *mosquitto_client_address(const struct mosquitto *client)
{
	如果(client){
		返回 client->address;
	}否则{
		返回 NULL;
	}
}


BROKER_EXPORT struct mosquitto *mosquitto_client(const char *clientid)
{
	size_t len;
	struct mosquitto *context;

	如果(!clientid){
		返回 NULL;
	}
	len = strlen(clientid);
	如果(len == 0){
		返回 NULL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	返回 context;
}


BROKER_EXPORT 整数 mosquitto_client_port(const struct mosquitto *client)
{
	如果(client && client->listener){
		返回 client->listener->port;
	}否则{
		返回 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(const struct mosquitto *client)
{
	如果(client){
		返回 client->clean_start;
	}否则{
		返回 true;
	}
}


BROKER_EXPORT const char *mosquitto_client_id(const struct mosquitto *client)
{
	如果(client){
		返回 client->id;
	}否则{
		返回 NULL;
	}
}


BROKER_EXPORT unsigned mosquitto_client_id_hashv(const struct mosquitto *client)
{
	如果(client){
		返回 client->id_hashv;
	}否则{
		返回 0;
	}
}


BROKER_EXPORT 整数 mosquitto_client_keepalive(const struct mosquitto *client)
{
	如果(client){
		返回 client->keepalive;
	}否则{
		返回 -1;
	}
}


BROKER_EXPORT void *mosquitto_client_certificate(const struct mosquitto *client)
{
#ifdef WITH_TLS
	如果(client && client->ssl){
		返回 SSL_get_peer_certificate(client->ssl);
	}否则{
		返回 NULL;
	}
#否则
	UNUSED(client);

	返回 NULL;
#endif
}


BROKER_EXPORT 整数 mosquitto_client_protocol(const struct mosquitto *client)
{
#如果 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	如果(client && client->wsi){
		返回 mp_websockets;
	}否则
#elif defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	如果(client && client->transport == mosq_t_ws){
		返回 mp_websockets;
	}否则
#否则
	UNUSED(client);
#endif
	{
		返回 mp_mqtt;
	}
}


BROKER_EXPORT 整数 mosquitto_client_protocol_version(const struct mosquitto *client)
{
	如果(client){
		switch(client->protocol){
			case mosq_p_mqtt31:
				返回 3;
			case mosq_p_mqtt311:
				返回 4;
			case mosq_p_mqtt5:
				返回 5;
			default:
				返回 0;
		}
	}否则{
		返回 0;
	}
}


BROKER_EXPORT 整数 mosquitto_client_sub_count(const struct mosquitto *client)
{
	如果(client){
		返回 client->subs_count;
	}否则{
		返回 0;
	}
}


BROKER_EXPORT const char *mosquitto_client_username(const struct mosquitto *client)
{
	如果(client){
#ifdef WITH_BRIDGE
		如果(client->bridge){
			返回 client->bridge->local_username;
		}否则
#endif
		{
			返回 client->username;
		}
	}否则{
		返回 NULL;
	}
}


BROKER_EXPORT 整数 mosquitto_broker_publish(
		const char *clientid,
		const char *topic,
		整数 payloadlen,
		void *payload,
		整数 qos,
		bool retain,
		mosquitto_property *properties)
{
	struct mosquitto__message_v5 *msg;

	如果(topic == NULL
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == NULL)
			|| qos < 0 || qos > 2){

		返回 MOSQ_ERR_INVAL;
	}

	msg = mosquitto_malloc(sizeof(struct mosquitto__message_v5));
	如果(msg == NULL){
		返回 MOSQ_ERR_NOMEM;
	}

	msg->next = NULL;
	msg->prev = NULL;
	如果(clientid){
		msg->clientid = mosquitto_strdup(clientid);
		如果(msg->clientid == NULL){
			mosquitto_FREE(msg);
			返回 MOSQ_ERR_NOMEM;
		}
	}否则{
		msg->clientid = NULL;
	}
	msg->topic = mosquitto_strdup(topic);
	如果(msg->topic == NULL){
		mosquitto_FREE(msg->clientid);
		mosquitto_FREE(msg);
		返回 MOSQ_ERR_NOMEM;
	}
	msg->payloadlen = payloadlen;
	msg->payload = payload;
	msg->qos = qos;
	msg->retain = retain;
	msg->properties = properties;

	DL_APPEND(db.plugin_msgs, msg);

	loop__update_next_event(1);
	返回 MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT 整数 mosquitto_broker_publish_copy(
		const char *clientid,
		const char *topic,
		整数 payloadlen,
		const void *payload,
		整数 qos,
		bool retain,
		mosquitto_property *properties)
{
	void *payload_out;
	整数 rc;

	如果(topic == NULL
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == NULL)
			|| qos < 0 || qos > 2){

		返回 MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (size_t)(payloadlen+1));
	如果(payload_out == NULL){
		返回 MOSQ_ERR_NOMEM;
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

	如果(rc){
		mosquitto_FREE(payload_out);
	}
	返回 rc;
}


BROKER_EXPORT 整数 mosquitto_set_username(struct mosquitto *client, const char *username)
{
	char *u_dup;
	char *old;

	如果(!client){
		返回 MOSQ_ERR_INVAL;
	}

	如果(username){
		如果(mosquitto_validate_utf8(username, (整数)strlen(username))){
			返回 MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		如果(!u_dup){
			返回 MOSQ_ERR_NOMEM;
		}
	}否则{
		u_dup = NULL;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	返回 MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT 整数 mosquitto_set_clientid(struct mosquitto *client, const char *clientid)
{
	struct mosquitto *found_client;
	char *id_dup;
	bool in_by_id;
	整数 clientid_len;

	如果(!client || !clientid){
		返回 MOSQ_ERR_INVAL;
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

	如果(in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), found_client);
		如果(found_client){
			返回 MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (整数)strlen(clientid);
	如果(mosquitto_validate_utf8(clientid, clientid_len)){
		返回 MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	如果(!id_dup){
		返回 MOSQ_ERR_NOMEM;
	}

	如果(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	如果(in_by_id){
		context__add_to_by_id(client);
	}

	返回 MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
static void check_subscription_acls(struct mosquitto *context)
{
	整数 rc;
	uint8_t reason;

	为(整数 i=0; i<context->subs_capacity; i++){
		如果(context->subs[i] == NULL){
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

		如果(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


static void disconnect_client(struct mosquitto *context, bool with_will)
{
	如果(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, NULL);
	}
	如果(with_will == false){
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	如果(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT 整数 mosquitto_kick_client_by_clientid(const char *clientid, bool with_will)
{
	struct mosquitto *ctxt, *ctxt_tmp;

	如果(clientid == NULL){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		返回 MOSQ_ERR_SUCCESS;
	}否则{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), ctxt);
		如果(ctxt){
			disconnect_client(ctxt, with_will);
			返回 MOSQ_ERR_SUCCESS;
		}否则{
			返回 MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT 整数 mosquitto_kick_client_by_username(const char *username, bool with_will)
{
	struct mosquitto *ctxt, *ctxt_tmp;

	如果(username == NULL){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			如果(ctxt->username == NULL){
				disconnect_client(ctxt, with_will);
			}
		}
	}否则{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			如果(ctxt->username != NULL && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	返回 MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT 整数 mosquitto_apply_on_all_clients(整数 (*FUNC_client_functor)(const struct mosquitto *, void *), void *functor_context)
{
	整数 rc = MOSQ_ERR_SUCCESS;
	struct mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		如果(rc != MOSQ_ERR_SUCCESS){
			break;
		}
	}

	返回 rc;
}


BROKER_EXPORT 整数 mosquitto_persist_client_add(struct mosquitto_client *client)
{
	struct mosquitto *context;
	整数 rc;

	如果(client == NULL){
		返回 MOSQ_ERR_INVAL;
	}
	如果(client->clientid == NULL){
		rc = MOSQ_ERR_INVAL;
		goto error;
	}

	context = NULL;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	如果(context){
		rc = MOSQ_ERR_INVAL;
		goto error;
	}

	context = context__init();
	如果(!context){
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
	如果(db.config->per_listener_settings && client->listener_port > 0){
		为(整数 i=0; i < db.config->listener_count; i++){
			如果(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				break;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	返回 MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	返回 rc;
}


BROKER_EXPORT 整数 mosquitto_persist_client_update(struct mosquitto_client *client)
{
	struct mosquitto *context;
	整数 rc;

	如果(client == NULL){
		返回 MOSQ_ERR_INVAL;
	}
	如果(client->clientid == NULL){
		rc = MOSQ_ERR_INVAL;
		goto error;
	}

	context = NULL;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	如果(context == NULL){
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
	如果(db.config->per_listener_settings && client->listener_port > 0){
		为(整数 i=0; i < db.config->listener_count; i++){
			如果(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				break;
			}
		}
	}

	返回 MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	返回 rc;
}


BROKER_EXPORT 整数 mosquitto_persist_client_delete(const char *clientid)
{
	struct mosquitto *context;

	如果(clientid == NULL){
		返回 MOSQ_ERR_INVAL;
	}

	context = NULL;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	如果(context == NULL){
		返回 MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	返回 MOSQ_ERR_SUCCESS;
}


static struct mosquitto__base_msg *find_store_msg(uint64_t store_id)
{
	struct mosquitto__base_msg *base_msg;

	HASH_FIND(hh, db.msg_store, &store_id, sizeof(store_id), base_msg);
	返回 base_msg;
}


BROKER_EXPORT 整数 mosquitto_persist_client_msg_add(struct mosquitto_client_msg *client_msg)
{
	struct mosquitto *context;
	struct mosquitto__base_msg *base_msg;

	如果(client_msg == NULL || client_msg->clientid == NULL){
		返回 MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	如果(context == NULL){
		返回 MOSQ_ERR_NOT_FOUND;
	}
	base_msg = find_store_msg(client_msg->store_id);
	如果(base_msg == NULL){
		返回 MOSQ_ERR_NOT_FOUND;
	}

	如果(client_msg->direction == mosq_md_out){
		如果(client_msg->qos > 0){
			context->last_mid = client_msg->mid;
		}
		返回 db__message_insert_outgoing(context, client_msg->cmsg_id, client_msg->mid,
				client_msg->qos, client_msg->retain,
				base_msg, client_msg->subscription_identifier, false, false);
	}否则 如果(client_msg->direction == mosq_md_in){
		返回 db__message_insert_incoming(context, client_msg->cmsg_id, base_msg, false);
	}否则{
		返回 MOSQ_ERR_INVAL;
	}
	返回 MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT 整数 mosquitto_persist_client_msg_delete(struct mosquitto_client_msg *client_msg)
{
	struct mosquitto *context;

	如果(client_msg == NULL || client_msg->clientid == NULL){
		返回 MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	如果(context == NULL){
		返回 MOSQ_ERR_NOT_FOUND;
	}


	整数 rc = MOSQ_ERR_INVAL;
	如果(client_msg->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_msg->mid, mosq_ms_any, client_msg->qos);
	}否则 如果(client_msg->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_msg->mid);
	}
	返回 rc;
}


BROKER_EXPORT 整数 mosquitto_persist_client_msg_update(struct mosquitto_client_msg *client_msg)
{
	struct mosquitto *context;

	如果(client_msg == NULL || client_msg->clientid == NULL){
		返回 MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	如果(context == NULL){
		返回 MOSQ_ERR_NOT_FOUND;
	}

	如果(client_msg->direction == mosq_md_out){
		db__message_update_outgoing(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}否则 如果(client_msg->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}否则{
		返回 MOSQ_ERR_INVAL;
	}
	返回 MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT 整数 mosquitto_persist_client_msg_clear(struct mosquitto_client_msg *client_msg)
{
	struct mosquitto *context;

	如果(client_msg == NULL || client_msg->clientid == NULL){
		返回 MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_msg->clientid, strlen(client_msg->clientid), context);
	如果(context == NULL){
		返回 MOSQ_ERR_NOT_FOUND;
	}

	如果(client_msg->direction == mosq_bmd_in || client_msg->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	如果(client_msg->direction == mosq_bmd_out || client_msg->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	返回 MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT 整数 mosquitto_subscription_add(const struct mosquitto_subscription *sub)
{
	struct mosquitto *context;

	如果(sub == NULL || sub->clientid == NULL || sub->topic_filter == NULL || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		返回 MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, strlen(sub->clientid), context);

	如果(context){
		返回 sub__add(context, sub);
	}否则{
		返回 MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT 整数 mosquitto_subscription_delete(const char *clientid, const char *topic)
{
	struct mosquitto *context;
	uint8_t reason;

	如果(clientid == NULL || topic == NULL || clientid[0] == '\0' || topic[0] == '\0'){
		返回 MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	如果(context){
		返回 sub__remove(context, topic, &reason);
	}否则{
		返回 MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT 整数 mosquitto_persist_base_msg_add(struct mosquitto_base_msg *msg_add)
{
	struct mosquitto context;
	struct mosquitto__base_msg *base_msg;
	整数 rc;

	memset(&context, 0, sizeof(context));

	如果(msg_add->payloadlen > MQTT_MAX_PAYLOAD){
		返回 MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (char *)msg_add->source_id;
	context.username = (char *)msg_add->source_username;

	base_msg = mosquitto_calloc(1, sizeof(struct mosquitto__base_msg));
	如果(base_msg == NULL){
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

	如果(msg_add->source_port){
		为(整数 i=0; i<db.config->listener_count; i++){
			如果(db.config->listeners[i].port == msg_add->source_port){
				base_msg->source_listener = &db.config->listeners[i];
				break;
			}
		}
	}

	base_msg->stored = true;
	rc = db__message_store(&context, base_msg, NULL, mosq_mo_broker);
	返回 rc;

error:
	mosquitto_property_free_all(&msg_add->properties);
	mosquitto_free(msg_add->topic);
	mosquitto_free(msg_add->payload);
	mosquitto_free(base_msg);

	返回 MOSQ_ERR_NOMEM;
}


BROKER_EXPORT 整数 mosquitto_persist_base_msg_delete(uint64_t store_id)
{
	struct mosquitto__base_msg *base_msg;

	base_msg = find_store_msg(store_id);
	如果(base_msg && base_msg->ref_count == 0){
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

	返回 MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT void mosquitto_complete_basic_auth(const char *clientid, 整数 result)
{
	struct mosquitto *context;

	如果(clientid == NULL){
		返回;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, strlen(clientid), context);
	如果(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		如果(result == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, NULL, 0);
		}否则{
			如果(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, NULL);
			}否则{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, NULL);
			}
			context->clean_start = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT 整数 mosquitto_broker_node_id_set(uint16_t id)
{
	如果(id > 1023){
		返回 MOSQ_ERR_INVAL;
	}否则{
		db.node_id = id;
		db.node_id_shifted = ((uint64_t)id) << 54;
		返回 MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT const char *mosquitto_persistence_location(void)
{
	返回 db.config->persistence_location;
}


BROKER_EXPORT 整数 mosquitto_client_will_set(const char *clientid, const char *topic, 整数 payloadlen, const void *payload, 整数 qos, bool retain, mosquitto_property *properties)
{
	struct mosquitto *mosq = mosquitto_client(clientid);
	如果(!mosq){
		返回 MOSQ_ERR_NOT_FOUND;
	}
	如果(properties && mosq->protocol != mosq_p_mqtt5){
		如果(net__is_connected(mosq)){
			返回 MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	返回 will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
