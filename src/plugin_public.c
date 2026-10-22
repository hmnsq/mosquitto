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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "send_mosq.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"
#ଅନ୍ତର୍ଭାଵ "will_mosq.h"
#ଅନ୍ତର୍ଭାଵ "utlist.h"
#ଅନ୍ତର୍ଭାଵ "will_mosq.h"

#ifdef WITH_TLS
#  include <openssl/ssl.h>
#endif


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_plugin_set_info(mosquitto_plugin_id_t *identifier,
		ଅଚଳ ଅକ୍ଷର *plugin_name,
		ଅଚଳ ଅକ୍ଷର *plugin_version)
{
	ଯଦି(identifier == ଖାଲି || plugin_name == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	ଯଦି(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}ଅନ୍ୟଥା{
		identifier->plugin_version = ଖାଲି;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ଅଚଳ ଅକ୍ଷର *mosquitto_client_address(ଅଚଳ ସଂରଚନା mosquitto *client)
{
	ଯଦି(client){
		ଫେରନ୍ତୁ client->address;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ ଖାଲି;
	}
}


BROKER_EXPORT ସଂରଚନା mosquitto *mosquitto_client(ଅଚଳ ଅକ୍ଷର *clientid)
{
	size_t len;
	ସଂରଚନା mosquitto *context;

	ଯଦି(!clientid){
		ଫେରନ୍ତୁ ଖାଲି;
	}
	len = strlen(clientid);
	ଯଦି(len == 0){
		ଫେରନ୍ତୁ ଖାଲି;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	ଫେରନ୍ତୁ context;
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_client_port(ଅଚଳ ସଂରଚନା mosquitto *client)
{
	ଯଦି(client && client->listener){
		ଫେରନ୍ତୁ client->listener->port;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(ଅଚଳ ସଂରଚନା mosquitto *client)
{
	ଯଦି(client){
		ଫେରନ୍ତୁ client->clean_start;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ true;
	}
}


BROKER_EXPORT ଅଚଳ ଅକ୍ଷର *mosquitto_client_id(ଅଚଳ ସଂରଚନା mosquitto *client)
{
	ଯଦି(client){
		ଫେରନ୍ତୁ client->id;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ ଖାଲି;
	}
}


BROKER_EXPORT unsigned mosquitto_client_id_hashv(ଅଚଳ ସଂରଚନା mosquitto *client)
{
	ଯଦି(client){
		ଫେରନ୍ତୁ client->id_hashv;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ 0;
	}
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_client_keepalive(ଅଚଳ ସଂରଚନା mosquitto *client)
{
	ଯଦି(client){
		ଫେରନ୍ତୁ client->keepalive;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ -1;
	}
}


BROKER_EXPORT ଶୂନ୍ୟ *mosquitto_client_certificate(ଅଚଳ ସଂରଚନା mosquitto *client)
{
#ifdef WITH_TLS
	ଯଦି(client && client->ssl){
		ଫେରନ୍ତୁ SSL_get_peer_certificate(client->ssl);
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ ଖାଲି;
	}
#ଅନ୍ୟଥା
	UNUSED(client);

	ଫେରନ୍ତୁ ଖାଲି;
#endif
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_client_protocol(ଅଚଳ ସଂରଚନା mosquitto *client)
{
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ଯଦି(client && client->wsi){
		ଫେରନ୍ତୁ mp_websockets;
	}ଅନ୍ୟଥା
#ନଚେତଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	ଯଦି(client && client->transport == mosq_t_ws){
		ଫେରନ୍ତୁ mp_websockets;
	}ଅନ୍ୟଥା
#ଅନ୍ୟଥା
	UNUSED(client);
#endif
	{
		ଫେରନ୍ତୁ mp_mqtt;
	}
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_client_protocol_version(ଅଚଳ ସଂରଚନା mosquitto *client)
{
	ଯଦି(client){
		ବାଛ(client->protocol){
			ସ୍ଥିତି mosq_p_mqtt31:
				ଫେରନ୍ତୁ 3;
			ସ୍ଥିତି mosq_p_mqtt311:
				ଫେରନ୍ତୁ 4;
			ସ୍ଥିତି mosq_p_mqtt5:
				ଫେରନ୍ତୁ 5;
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
				ଫେରନ୍ତୁ 0;
		}
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ 0;
	}
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_client_sub_ଗଣନା(ଅଚଳ ସଂରଚନା mosquitto *client)
{
	ଯଦି(client){
		ଫେରନ୍ତୁ client->subs_ଗଣନା;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ 0;
	}
}


BROKER_EXPORT ଅଚଳ ଅକ୍ଷର *mosquitto_client_username(ଅଚଳ ସଂରଚନା mosquitto *client)
{
	ଯଦି(client){
#ifdef WITH_BRIDGE
		ଯଦି(client->bridge){
			ଫେରନ୍ତୁ client->bridge->local_username;
		}ଅନ୍ୟଥା
#endif
		{
			ଫେରନ୍ତୁ client->username;
		}
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ ଖାଲି;
	}
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_broker_publish(
		ଅଚଳ ଅକ୍ଷର *clientid,
		ଅଚଳ ଅକ୍ଷର *topic,
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା payloadlen,
		ଶୂନ୍ୟ *payload,
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା qos,
		bool retain,
		mosquitto_property *properties)
{
	ସଂରଚନା mosquitto__message_v5 *ସନ୍ଦେଶ;

	ଯଦି(topic == ଖାଲି
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == ଖାଲି)
			|| qos < 0 || qos > 2){

		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	ସନ୍ଦେଶ = mosquitto_malloc(ଆକାର(ସଂରଚନା mosquitto__message_v5));
	ଯଦି(ସନ୍ଦେଶ == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	ସନ୍ଦେଶ->next = ଖାଲି;
	ସନ୍ଦେଶ->prev = ଖାଲି;
	ଯଦି(clientid){
		ସନ୍ଦେଶ->clientid = mosquitto_strdup(clientid);
		ଯଦି(ସନ୍ଦେଶ->clientid == ଖାଲି){
			mosquitto_FREE(ସନ୍ଦେଶ);
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
	}ଅନ୍ୟଥା{
		ସନ୍ଦେଶ->clientid = ଖାଲି;
	}
	ସନ୍ଦେଶ->topic = mosquitto_strdup(topic);
	ଯଦି(ସନ୍ଦେଶ->topic == ଖାଲି){
		mosquitto_FREE(ସନ୍ଦେଶ->clientid);
		mosquitto_FREE(ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	ସନ୍ଦେଶ->payloadlen = payloadlen;
	ସନ୍ଦେଶ->payload = payload;
	ସନ୍ଦେଶ->qos = qos;
	ସନ୍ଦେଶ->retain = retain;
	ସନ୍ଦେଶ->properties = properties;

	DL_APPEND(db.plugin_msgs, ସନ୍ଦେଶ);

	loop__update_next_event(1);
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_broker_publish_copy(
		ଅଚଳ ଅକ୍ଷର *clientid,
		ଅଚଳ ଅକ୍ଷର *topic,
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା payloadlen,
		ଅଚଳ ଶୂନ୍ୟ *payload,
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା qos,
		bool retain,
		mosquitto_property *properties)
{
	ଶୂନ୍ୟ *payload_out;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(topic == ଖାଲି
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == ଖାଲି)
			|| qos < 0 || qos > 2){

		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (size_t)(payloadlen+1));
	ଯଦି(payload_out == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
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

	ଯଦି(rc){
		mosquitto_FREE(payload_out);
	}
	ଫେରନ୍ତୁ rc;
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_set_username(ସଂରଚନା mosquitto *client, ଅଚଳ ଅକ୍ଷର *username)
{
	ଅକ୍ଷର *u_dup;
	ଅକ୍ଷର *old;

	ଯଦି(!client){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	ଯଦି(username){
		ଯଦି(mosquitto_validate_utf8(username, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strlen(username))){
			ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		ଯଦି(!u_dup){
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
	}ଅନ୍ୟଥା{
		u_dup = ଖାଲି;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_set_clientid(ସଂରଚନା mosquitto *client, ଅଚଳ ଅକ୍ଷର *clientid)
{
	ସଂରଚନା mosquitto *found_client;
	ଅକ୍ଷର *id_dup;
	bool in_by_id;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା clientid_len;

	ଯଦି(!client || !clientid){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
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

	ଯଦି(in_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), found_client);
		ଯଦି(found_client){
			ଫେରନ୍ତୁ MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strlen(clientid);
	ଯଦି(mosquitto_validate_utf8(clientid, clientid_len)){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	ଯଦି(!id_dup){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	ଯଦି(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	ଯଦି(in_by_id){
		context__add_to_by_id(client);
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
ସ୍ଥିର ଶୂନ୍ୟ check_subscription_acls(ସଂରଚନା mosquitto *context)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	uint8_t reason;

	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<context->subs_capacity; i++){
		ଯଦି(context->subs[i] == ଖାଲି){
			ଜାରି;
		}
		rc = mosquitto_acl_check(context,
				context->subs[i]->topic_filter,
				0,
				ଖାଲି,
				0, /* FIXME */
				false,
				ଖାଲି,
				MOSQ_ACL_SUBSCRIBE);

		ଯଦି(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


ସ୍ଥିର ଶୂନ୍ୟ disconnect_client(ସଂରଚନା mosquitto *context, bool with_will)
{
	ଯଦି(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ଖାଲି);
	}
	ଯଦି(with_will == false){
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	ଯଦି(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_kick_client_by_clientid(ଅଚଳ ଅକ୍ଷର *clientid, bool with_will)
{
	ସଂରଚନା mosquitto *ctxt, *ctxt_tmp;

	ଯଦି(clientid == ଖାଲି){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}ଅନ୍ୟଥା{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), ctxt);
		ଯଦି(ctxt){
			disconnect_client(ctxt, with_will);
			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}ଅନ୍ୟଥା{
			ଫେରନ୍ତୁ MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_kick_client_by_username(ଅଚଳ ଅକ୍ଷର *username, bool with_will)
{
	ସଂରଚନା mosquitto *ctxt, *ctxt_tmp;

	ଯଦି(username == ଖାଲି){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			ଯଦି(ctxt->username == ଖାଲି){
				disconnect_client(ctxt, with_will);
			}
		}
	}ଅନ୍ୟଥା{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			ଯଦି(ctxt->username != ଖାଲି && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_apply_on_all_clients(ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_client_functor)(ଅଚଳ ସଂରଚନା mosquitto *, ଶୂନ୍ୟ *), ଶୂନ୍ୟ *functor_context)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_SUCCESS;
	ସଂରଚନା mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		ଯଦି(rc != MOSQ_ERR_SUCCESS){
			ଭାଙ୍ଗ;
		}
	}

	ଫେରନ୍ତୁ rc;
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_persist_client_add(ସଂରଚନା mosquitto_client *client)
{
	ସଂରଚନା mosquitto *context;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(client == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଯଦି(client->clientid == ଖାଲି){
		rc = MOSQ_ERR_INVAL;
		ଯାଅ error;
	}

	context = ଖାଲି;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	ଯଦି(context){
		rc = MOSQ_ERR_INVAL;
		ଯାଅ error;
	}

	context = context__init();
	ଯଦି(!context){
		rc = MOSQ_ERR_NOMEM;
		ଯାଅ error;
	}

	context->id = client->clientid;
	client->clientid = ଖାଲି;
	context->username = client->username;
	client->username = ଖାଲି;
	context->auth_method = client->auth_method;
	client->auth_method = ଖାଲି;

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
	ଯଦି(db.config->per_listener_settings && client->listener_port > 0){
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i < db.config->listener_ଗଣନା; i++){
			ଯଦି(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				ଭାଙ୍ଗ;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	ଫେରନ୍ତୁ rc;
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_persist_client_update(ସଂରଚନା mosquitto_client *client)
{
	ସଂରଚନା mosquitto *context;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(client == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଯଦି(client->clientid == ଖାଲି){
		rc = MOSQ_ERR_INVAL;
		ଯାଅ error;
	}

	context = ଖାଲି;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	ଯଦି(context == ଖାଲି){
		rc = MOSQ_ERR_NOT_FOUND;
		ଯାଅ error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = ଖାଲି;

	context->clean_start = false;
	context->will_delay_time = client->will_delay_time;
	context->session_expiry_time = client->session_expiry_time;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->maximum_packet_size = client->max_packet_size;
	context->retain_available = client->retain_available;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	ଯଦି(db.config->per_listener_settings && client->listener_port > 0){
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i < db.config->listener_ଗଣନା; i++){
			ଯଦି(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				ଭାଙ୍ଗ;
			}
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	ଫେରନ୍ତୁ rc;
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_persist_client_delete(ଅଚଳ ଅକ୍ଷର *clientid)
{
	ସଂରଚନା mosquitto *context;

	ଯଦି(clientid == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	context = ଖାଲି;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	ଯଦି(context == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *find_store_ସନ୍ଦେଶ(uint64_t store_id)
{
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ;

	HASH_FIND(hh, db.ସନ୍ଦେଶ_store, &store_id, ଆକାର(store_id), base_ସନ୍ଦେଶ);
	ଫେରନ୍ତୁ base_ସନ୍ଦେଶ;
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_persist_client_ସନ୍ଦେଶ_add(ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ_2 *client_ସନ୍ଦେଶ)
{
	ସଂରଚନା mosquitto *context;
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ;

	ଯଦି(client_ସନ୍ଦେଶ == ଖାଲି || client_ସନ୍ଦେଶ->clientid == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_ସନ୍ଦେଶ->clientid, strlen(client_ସନ୍ଦେଶ->clientid), context);
	ଯଦି(context == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOT_FOUND;
	}
	base_ସନ୍ଦେଶ = find_store_ସନ୍ଦେଶ(client_ସନ୍ଦେଶ->store_id);
	ଯଦି(base_ସନ୍ଦେଶ == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOT_FOUND;
	}

	ଯଦି(client_ସନ୍ଦେଶ->direction == mosq_md_out){
		ଯଦି(client_ସନ୍ଦେଶ->qos > 0){
			context->last_mid = client_ସନ୍ଦେଶ->mid;
		}
		ଫେରନ୍ତୁ db_message_ଭର୍ତ୍ତି_outgoing(context, client_ସନ୍ଦେଶ->cmsg_id, client_ସନ୍ଦେଶ->mid,
				client_ସନ୍ଦେଶ->qos, client_ସନ୍ଦେଶ->retain,
				base_ସନ୍ଦେଶ, client_ସନ୍ଦେଶ->subscription_identifier, false, false);
	}ଅନ୍ୟଥା ଯଦି(client_ସନ୍ଦେଶ->direction == mosq_md_in){
		ଫେରନ୍ତୁ db_message_ଭର୍ତ୍ତି_incoming(context, client_ସନ୍ଦେଶ->cmsg_id, base_ସନ୍ଦେଶ, false);
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_persist_client_ସନ୍ଦେଶ_delete(ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ_2 *client_ସନ୍ଦେଶ)
{
	ସଂରଚନା mosquitto *context;

	ଯଦି(client_ସନ୍ଦେଶ == ଖାଲି || client_ସନ୍ଦେଶ->clientid == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_ସନ୍ଦେଶ->clientid, strlen(client_ସନ୍ଦେଶ->clientid), context);
	ଯଦି(context == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOT_FOUND;
	}


	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_INVAL;
	ଯଦି(client_ସନ୍ଦେଶ->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_ସନ୍ଦେଶ->mid, mosq_ms_any, client_ସନ୍ଦେଶ->qos);
	}ଅନ୍ୟଥା ଯଦି(client_ସନ୍ଦେଶ->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_ସନ୍ଦେଶ->mid);
	}
	ଫେରନ୍ତୁ rc;
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_persist_client_ସନ୍ଦେଶ_update(ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ_2 *client_ସନ୍ଦେଶ)
{
	ସଂରଚନା mosquitto *context;

	ଯଦି(client_ସନ୍ଦେଶ == ଖାଲି || client_ସନ୍ଦେଶ->clientid == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_ସନ୍ଦେଶ->clientid, strlen(client_ସନ୍ଦେଶ->clientid), context);
	ଯଦି(context == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOT_FOUND;
	}

	ଯଦି(client_ସନ୍ଦେଶ->direction == mosq_md_out){
		db__message_update_outgoing(context, client_ସନ୍ଦେଶ->mid, client_ସନ୍ଦେଶ->state, client_ସନ୍ଦେଶ->qos, false);
	}ଅନ୍ୟଥା ଯଦି(client_ସନ୍ଦେଶ->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_persist_client_ସନ୍ଦେଶ_clear(ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ_2 *client_ସନ୍ଦେଶ)
{
	ସଂରଚନା mosquitto *context;

	ଯଦି(client_ସନ୍ଦେଶ == ଖାଲି || client_ସନ୍ଦେଶ->clientid == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_ସନ୍ଦେଶ->clientid, strlen(client_ସନ୍ଦେଶ->clientid), context);
	ଯଦି(context == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOT_FOUND;
	}

	ଯଦି(client_ସନ୍ଦେଶ->direction == mosq_bmd_in || client_ସନ୍ଦେଶ->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	ଯଦି(client_ସନ୍ଦେଶ->direction == mosq_bmd_out || client_ସନ୍ଦେଶ->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_subscription_add(ଅଚଳ ସଂରଚନା mosquitto_subscription *sub)
{
	ସଂରଚନା mosquitto *context;

	ଯଦି(sub == ଖାଲି || sub->clientid == ଖାଲି || sub->topic_filter == ଖାଲି || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, strlen(sub->clientid), context);

	ଯଦି(context){
		ଫେରନ୍ତୁ sub__add(context, sub);
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_subscription_delete(ଅଚଳ ଅକ୍ଷର *clientid, ଅଚଳ ଅକ୍ଷର *topic)
{
	ସଂରଚନା mosquitto *context;
	uint8_t reason;

	ଯଦି(clientid == ଖାଲି || topic == ଖାଲି || clientid[0] == '\0' || topic[0] == '\0'){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	ଯଦି(context){
		ଫେରନ୍ତୁ sub__remove(context, topic, &reason);
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_persist_base_ସନ୍ଦେଶ_add(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ_2 *ସନ୍ଦେଶ_add)
{
	ସଂରଚନା mosquitto context;
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	memset(&context, 0, ଆକାର(context));

	ଯଦି(ସନ୍ଦେଶ_add->payloadlen > MQTT_MAX_PAYLOAD){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (ଅକ୍ଷର *)ସନ୍ଦେଶ_add->source_id;
	context.username = (ଅକ୍ଷର *)ସନ୍ଦେଶ_add->source_username;

	base_ସନ୍ଦେଶ = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ));
	ଯଦି(base_ସନ୍ଦେଶ == ଖାଲି){
		ଯାଅ error;
	}
	base_ସନ୍ଦେଶ->data.store_id = ସନ୍ଦେଶ_add->store_id;
	base_ସନ୍ଦେଶ->data.expiry_time = ସନ୍ଦେଶ_add->expiry_time;
	base_ସନ୍ଦେଶ->data.payloadlen = ସନ୍ଦେଶ_add->payloadlen;
	base_ସନ୍ଦେଶ->data.source_mid = ସନ୍ଦେଶ_add->source_mid;
	base_ସନ୍ଦେଶ->data.qos = ସନ୍ଦେଶ_add->qos;
	base_ସନ୍ଦେଶ->data.retain = ସନ୍ଦେଶ_add->retain;

	base_ସନ୍ଦେଶ->data.payload = ସନ୍ଦେଶ_add->payload;
	ସନ୍ଦେଶ_add->payload = ଖାଲି;
	base_ସନ୍ଦେଶ->data.topic = ସନ୍ଦେଶ_add->topic;
	ସନ୍ଦେଶ_add->topic = ଖାଲି;
	base_ସନ୍ଦେଶ->data.properties = ସନ୍ଦେଶ_add->properties;
	ସନ୍ଦେଶ_add->properties = ଖାଲି;

	ଯଦି(ସନ୍ଦେଶ_add->source_port){
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.config->listener_ଗଣନା; i++){
			ଯଦି(db.config->listeners[i].port == ସନ୍ଦେଶ_add->source_port){
				base_ସନ୍ଦେଶ->source_listener = &db.config->listeners[i];
				ଭାଙ୍ଗ;
			}
		}
	}

	base_ସନ୍ଦେଶ->stored = true;
	rc = db__message_store(&context, base_ସନ୍ଦେଶ, ଖାଲି, mosq_mo_broker);
	ଫେରନ୍ତୁ rc;

error:
	mosquitto_property_free_all(&ସନ୍ଦେଶ_add->properties);
	mosquitto_free(ସନ୍ଦେଶ_add->topic);
	mosquitto_free(ସନ୍ଦେଶ_add->payload);
	mosquitto_free(base_ସନ୍ଦେଶ);

	ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_persist_base_ସନ୍ଦେଶ_delete(uint64_t store_id)
{
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ;

	base_ସନ୍ଦେଶ = find_store_ସନ୍ଦେଶ(store_id);
	ଯଦି(base_ସନ୍ଦେଶ && base_ସନ୍ଦେଶ->ref_ଗଣନା == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_ସନ୍ଦେଶ_store_remove(base_ସନ୍ଦେଶ, false);
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ଶୂନ୍ୟ mosquitto_complete_basic_auth(ଅଚଳ ଅକ୍ଷର *clientid, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result)
{
	ସଂରଚନା mosquitto *context;

	ଯଦି(clientid == ଖାଲି){
		ଫେରନ୍ତୁ;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, strlen(clientid), context);
	ଯଦି(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		ଯଦି(result == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, ଖାଲି, 0);
		}ଅନ୍ୟଥା{
			ଯଦି(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ଖାଲି);
			}ଅନ୍ୟଥା{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ଖାଲି);
			}
			context->clean_start = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_broker_ଗଣ୍ଠି_id_set(uint16_t id)
{
	ଯଦି(id > 1023){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}ଅନ୍ୟଥା{
		db.ଗଣ୍ଠି_id = id;
		db.ଗଣ୍ଠି_id_shifted = ((uint64_t)id) << 54;
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT ଅଚଳ ଅକ୍ଷର *mosquitto_persistence_location(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ db.config->persistence_location;
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_client_will_set(ଅଚଳ ଅକ୍ଷର *clientid, ଅଚଳ ଅକ୍ଷର *topic, ପୂର୍ଣ୍ଣସଂଖ୍ୟା payloadlen, ଅଚଳ ଶୂନ୍ୟ *payload, ପୂର୍ଣ୍ଣସଂଖ୍ୟା qos, bool retain, mosquitto_property *properties)
{
	ସଂରଚନା mosquitto *mosq = mosquitto_client(clientid);
	ଯଦି(!mosq){
		ଫେରନ୍ତୁ MOSQ_ERR_NOT_FOUND;
	}
	ଯଦି(properties && mosq->protocol != mosq_p_mqtt5){
		ଯଦି(net__is_connected(mosq)){
			ଫେରନ୍ତୁ MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	ଫେରନ୍ତୁ will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
