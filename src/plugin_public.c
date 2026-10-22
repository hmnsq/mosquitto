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


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_plugin_set_info(mosquitto_plugin_id_t *identifier,
		ধ্রুবক অক্ষর *plugin_name,
		ধ্রুবক অক্ষর *plugin_version)
{
	যদি(identifier == ফাঁকা || plugin_name == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	যদি(plugin_version){
		identifier->plugin_version = mosquitto_strdup(plugin_version);
	}অন্যথা{
		identifier->plugin_version = ফাঁকা;
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ধ্রুবক অক্ষর *mosquitto_client_address(ধ্রুবক কাঠামো mosquitto *client)
{
	যদি(client){
		ফেরত client->address;
	}অন্যথা{
		ফেরত ফাঁকা;
	}
}


BROKER_EXPORT কাঠামো mosquitto *mosquitto_client(ধ্রুবক অক্ষর *clientid)
{
	size_t len;
	কাঠামো mosquitto *context;

	যদি(!clientid){
		ফেরত ফাঁকা;
	}
	len = strlen(clientid);
	যদি(len == 0){
		ফেরত ফাঁকা;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	ফেরত context;
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_client_port(ধ্রুবক কাঠামো mosquitto *client)
{
	যদি(client && client->listener){
		ফেরত client->listener->port;
	}অন্যথা{
		ফেরত 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(ধ্রুবক কাঠামো mosquitto *client)
{
	যদি(client){
		ফেরত client->clean_start;
	}অন্যথা{
		ফেরত true;
	}
}


BROKER_EXPORT ধ্রুবক অক্ষর *mosquitto_client_id(ধ্রুবক কাঠামো mosquitto *client)
{
	যদি(client){
		ফেরত client->id;
	}অন্যথা{
		ফেরত ফাঁকা;
	}
}


BROKER_EXPORT unsigned mosquitto_client_id_hashv(ধ্রুবক কাঠামো mosquitto *client)
{
	যদি(client){
		ফেরত client->id_hashv;
	}অন্যথা{
		ফেরত 0;
	}
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_client_keepalive(ধ্রুবক কাঠামো mosquitto *client)
{
	যদি(client){
		ফেরত client->keepalive;
	}অন্যথা{
		ফেরত -1;
	}
}


BROKER_EXPORT শূন্য *mosquitto_client_certificate(ধ্রুবক কাঠামো mosquitto *client)
{
#ifdef WITH_TLS
	যদি(client && client->ssl){
		ফেরত SSL_get_peer_certificate(client->ssl);
	}অন্যথা{
		ফেরত ফাঁকা;
	}
#অন্যথা
	UNUSED(client);

	ফেরত ফাঁকা;
#endif
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_client_protocol(ধ্রুবক কাঠামো mosquitto *client)
{
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	যদি(client && client->wsi){
		ফেরত mp_websockets;
	}অন্যথা
#নতুবা_যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	যদি(client && client->transport == mosq_t_ws){
		ফেরত mp_websockets;
	}অন্যথা
#অন্যথা
	UNUSED(client);
#endif
	{
		ফেরত mp_mqtt;
	}
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_client_protocol_version(ধ্রুবক কাঠামো mosquitto *client)
{
	যদি(client){
		নির্বাচন(client->protocol){
			ক্ষেত্র mosq_p_mqtt31:
				ফেরত 3;
			ক্ষেত্র mosq_p_mqtt311:
				ফেরত 4;
			ক্ষেত্র mosq_p_mqtt5:
				ফেরত 5;
			পূর্বনির্ধারিত:
				ফেরত 0;
		}
	}অন্যথা{
		ফেরত 0;
	}
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_client_sub_গুনতি(ধ্রুবক কাঠামো mosquitto *client)
{
	যদি(client){
		ফেরত client->subs_গুনতি;
	}অন্যথা{
		ফেরত 0;
	}
}


BROKER_EXPORT ধ্রুবক অক্ষর *mosquitto_client_username(ধ্রুবক কাঠামো mosquitto *client)
{
	যদি(client){
#ifdef WITH_BRIDGE
		যদি(client->bridge){
			ফেরত client->bridge->local_username;
		}অন্যথা
#endif
		{
			ফেরত client->username;
		}
	}অন্যথা{
		ফেরত ফাঁকা;
	}
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_broker_publish(
		ধ্রুবক অক্ষর *clientid,
		ধ্রুবক অক্ষর *topic,
		পূর্ণসংখ্যা payloadlen,
		শূন্য *payload,
		পূর্ণসংখ্যা qos,
		bool retain,
		mosquitto_property *properties)
{
	কাঠামো mosquitto__message_v5 *বার্তা;

	যদি(topic == ফাঁকা
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == ফাঁকা)
			|| qos < 0 || qos > 2){

		ফেরত MOSQ_ERR_INVAL;
	}

	বার্তা = mosquitto_malloc(আকার(কাঠামো mosquitto__message_v5));
	যদি(বার্তা == ফাঁকা){
		ফেরত MOSQ_ERR_NOMEM;
	}

	বার্তা->পরবর্তী = ফাঁকা;
	বার্তা->পূর্ব = ফাঁকা;
	যদি(clientid){
		বার্তা->clientid = mosquitto_strdup(clientid);
		যদি(বার্তা->clientid == ফাঁকা){
			mosquitto_FREE(বার্তা);
			ফেরত MOSQ_ERR_NOMEM;
		}
	}অন্যথা{
		বার্তা->clientid = ফাঁকা;
	}
	বার্তা->topic = mosquitto_strdup(topic);
	যদি(বার্তা->topic == ফাঁকা){
		mosquitto_FREE(বার্তা->clientid);
		mosquitto_FREE(বার্তা);
		ফেরত MOSQ_ERR_NOMEM;
	}
	বার্তা->payloadlen = payloadlen;
	বার্তা->payload = payload;
	বার্তা->qos = qos;
	বার্তা->retain = retain;
	বার্তা->properties = properties;

	DL_APPEND(db.plugin_msgs, বার্তা);

	loop_update_পরবর্তী_event(1);
	ফেরত MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_broker_publish_copy(
		ধ্রুবক অক্ষর *clientid,
		ধ্রুবক অক্ষর *topic,
		পূর্ণসংখ্যা payloadlen,
		ধ্রুবক শূন্য *payload,
		পূর্ণসংখ্যা qos,
		bool retain,
		mosquitto_property *properties)
{
	শূন্য *payload_out;
	পূর্ণসংখ্যা rc;

	যদি(topic == ফাঁকা
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == ফাঁকা)
			|| qos < 0 || qos > 2){

		ফেরত MOSQ_ERR_INVAL;
	}

	payload_out = mosquitto_calloc(1, (size_t)(payloadlen+1));
	যদি(payload_out == ফাঁকা){
		ফেরত MOSQ_ERR_NOMEM;
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
	ফেরত rc;
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_set_username(কাঠামো mosquitto *client, ধ্রুবক অক্ষর *username)
{
	অক্ষর *u_dup;
	অক্ষর *old;

	যদি(!client){
		ফেরত MOSQ_ERR_INVAL;
	}

	যদি(username){
		যদি(mosquitto_validate_utf8(username, (পূর্ণসংখ্যা)strlen(username))){
			ফেরত MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		যদি(!u_dup){
			ফেরত MOSQ_ERR_NOMEM;
		}
	}অন্যথা{
		u_dup = ফাঁকা;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	ফেরত MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_set_clientid(কাঠামো mosquitto *client, ধ্রুবক অক্ষর *clientid)
{
	কাঠামো mosquitto *found_client;
	অক্ষর *id_dup;
	bool in_by_id;
	পূর্ণসংখ্যা clientid_len;

	যদি(!client || !clientid){
		ফেরত MOSQ_ERR_INVAL;
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
			ফেরত MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (পূর্ণসংখ্যা)strlen(clientid);
	যদি(mosquitto_validate_utf8(clientid, clientid_len)){
		ফেরত MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	যদি(!id_dup){
		ফেরত MOSQ_ERR_NOMEM;
	}

	যদি(in_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	যদি(in_by_id){
		context__add_to_by_id(client);
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
স্থির শূন্য check_subscription_acls(কাঠামো mosquitto *context)
{
	পূর্ণসংখ্যা rc;
	uint8_t reason;

	জন্য(পূর্ণসংখ্যা i=0; i<context->subs_capacity; i++){
		যদি(context->subs[i] == ফাঁকা){
			চালু;
		}
		rc = mosquitto_acl_check(context,
				context->subs[i]->topic_filter,
				0,
				ফাঁকা,
				0, /* FIXME */
				false,
				ফাঁকা,
				MOSQ_ACL_SUBSCRIBE);

		যদি(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


স্থির শূন্য disconnect_client(কাঠামো mosquitto *context, bool with_will)
{
	যদি(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ফাঁকা);
	}
	যদি(with_will == false){
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	যদি(context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_kick_client_by_clientid(ধ্রুবক অক্ষর *clientid, bool with_will)
{
	কাঠামো mosquitto *ctxt, *ctxt_tmp;

	যদি(clientid == ফাঁকা){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		ফেরত MOSQ_ERR_SUCCESS;
	}অন্যথা{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), ctxt);
		যদি(ctxt){
			disconnect_client(ctxt, with_will);
			ফেরত MOSQ_ERR_SUCCESS;
		}অন্যথা{
			ফেরত MOSQ_ERR_NOT_FOUND;
		}
	}
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_kick_client_by_username(ধ্রুবক অক্ষর *username, bool with_will)
{
	কাঠামো mosquitto *ctxt, *ctxt_tmp;

	যদি(username == ফাঁকা){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			যদি(ctxt->username == ফাঁকা){
				disconnect_client(ctxt, with_will);
			}
		}
	}অন্যথা{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			যদি(ctxt->username != ফাঁকা && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_apply_on_all_clients(পূর্ণসংখ্যা (*FUNC_client_functor)(ধ্রুবক কাঠামো mosquitto *, শূন্য *), শূন্য *functor_context)
{
	পূর্ণসংখ্যা rc = MOSQ_ERR_SUCCESS;
	কাঠামো mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		যদি(rc != MOSQ_ERR_SUCCESS){
			বিরতি;
		}
	}

	ফেরত rc;
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_persist_client_add(কাঠামো mosquitto_client *client)
{
	কাঠামো mosquitto *context;
	পূর্ণসংখ্যা rc;

	যদি(client == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}
	যদি(client->clientid == ফাঁকা){
		rc = MOSQ_ERR_INVAL;
		যাও error;
	}

	context = ফাঁকা;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	যদি(context){
		rc = MOSQ_ERR_INVAL;
		যাও error;
	}

	context = context__init();
	যদি(!context){
		rc = MOSQ_ERR_NOMEM;
		যাও error;
	}

	context->id = client->clientid;
	client->clientid = ফাঁকা;
	context->username = client->username;
	client->username = ফাঁকা;
	context->auth_method = client->auth_method;
	client->auth_method = ফাঁকা;

	context->clean_start = false;
	context->will_delay_time = client->will_delay_time;
	context->session_expiry_time = client->session_expiry_time;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->maximum_packet_আকার = client->max_packet_আকার;
	context->retain_available = client->retain_available;
	context->is_persisted = true;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	যদি(db.config->per_listener_settings && client->listener_port > 0){
		জন্য(পূর্ণসংখ্যা i=0; i < db.config->listener_গুনতি; i++){
			যদি(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				বিরতি;
			}
		}
	}

	context__add_to_by_id(context);
	session_expiry__add_from_persistence(context, context->session_expiry_time);

	ফেরত MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_method);
	ফেরত rc;
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_persist_client_update(কাঠামো mosquitto_client *client)
{
	কাঠামো mosquitto *context;
	পূর্ণসংখ্যা rc;

	যদি(client == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}
	যদি(client->clientid == ফাঁকা){
		rc = MOSQ_ERR_INVAL;
		যাও error;
	}

	context = ফাঁকা;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, strlen(client->clientid), context);
	যদি(context == ফাঁকা){
		rc = MOSQ_ERR_NOT_FOUND;
		যাও error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = ফাঁকা;

	context->clean_start = false;
	context->will_delay_time = client->will_delay_time;
	context->session_expiry_time = client->session_expiry_time;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->maximum_packet_আকার = client->max_packet_আকার;
	context->retain_available = client->retain_available;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	যদি(db.config->per_listener_settings && client->listener_port > 0){
		জন্য(পূর্ণসংখ্যা i=0; i < db.config->listener_গুনতি; i++){
			যদি(db.config->listeners[i].port == client->listener_port){
				context->listener = &db.config->listeners[i];
				বিরতি;
			}
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	ফেরত rc;
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_persist_client_delete(ধ্রুবক অক্ষর *clientid)
{
	কাঠামো mosquitto *context;

	যদি(clientid == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}

	context = ফাঁকা;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	যদি(context == ফাঁকা){
		ফেরত MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
	context->is_persisted = false;
	mosquitto__set_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির কাঠামো mosquitto_base_বার্তা *find_store_বার্তা(uint64_t store_id)
{
	কাঠামো mosquitto_base_বার্তা *base_বার্তা;

	HASH_FIND(hh, db.বার্তা_store, &store_id, আকার(store_id), base_বার্তা);
	ফেরত base_বার্তা;
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_persist_client_বার্তা_add(কাঠামো mosquitto_client_বার্তা_2 *client_বার্তা)
{
	কাঠামো mosquitto *context;
	কাঠামো mosquitto_base_বার্তা *base_বার্তা;

	যদি(client_বার্তা == ফাঁকা || client_বার্তা->clientid == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_বার্তা->clientid, strlen(client_বার্তা->clientid), context);
	যদি(context == ফাঁকা){
		ফেরত MOSQ_ERR_NOT_FOUND;
	}
	base_বার্তা = find_store_বার্তা(client_বার্তা->store_id);
	যদি(base_বার্তা == ফাঁকা){
		ফেরত MOSQ_ERR_NOT_FOUND;
	}

	যদি(client_বার্তা->direction == mosq_md_out){
		যদি(client_বার্তা->qos > 0){
			context->last_mid = client_বার্তা->mid;
		}
		ফেরত db_message_ঢোকাও_outgoing(context, client_বার্তা->cmsg_id, client_বার্তা->mid,
				client_বার্তা->qos, client_বার্তা->retain,
				base_বার্তা, client_বার্তা->subscription_identifier, false, false);
	}অন্যথা যদি(client_বার্তা->direction == mosq_md_in){
		ফেরত db_message_ঢোকাও_incoming(context, client_বার্তা->cmsg_id, base_বার্তা, false);
	}অন্যথা{
		ফেরত MOSQ_ERR_INVAL;
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_persist_client_বার্তা_delete(কাঠামো mosquitto_client_বার্তা_2 *client_বার্তা)
{
	কাঠামো mosquitto *context;

	যদি(client_বার্তা == ফাঁকা || client_বার্তা->clientid == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_বার্তা->clientid, strlen(client_বার্তা->clientid), context);
	যদি(context == ফাঁকা){
		ফেরত MOSQ_ERR_NOT_FOUND;
	}


	পূর্ণসংখ্যা rc = MOSQ_ERR_INVAL;
	যদি(client_বার্তা->direction == mosq_md_out){
		rc = db__message_delete_outgoing(context, client_বার্তা->mid, mosq_ms_any, client_বার্তা->qos);
	}অন্যথা যদি(client_বার্তা->direction == mosq_md_in){
		rc = db__message_remove_incoming(context, client_বার্তা->mid);
	}
	ফেরত rc;
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_persist_client_বার্তা_update(কাঠামো mosquitto_client_বার্তা_2 *client_বার্তা)
{
	কাঠামো mosquitto *context;

	যদি(client_বার্তা == ফাঁকা || client_বার্তা->clientid == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_বার্তা->clientid, strlen(client_বার্তা->clientid), context);
	যদি(context == ফাঁকা){
		ফেরত MOSQ_ERR_NOT_FOUND;
	}

	যদি(client_বার্তা->direction == mosq_md_out){
		db__message_update_outgoing(context, client_বার্তা->mid, client_বার্তা->state, client_বার্তা->qos, false);
	}অন্যথা যদি(client_বার্তা->direction == mosq_md_in){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}অন্যথা{
		ফেরত MOSQ_ERR_INVAL;
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_persist_client_বার্তা_clear(কাঠামো mosquitto_client_বার্তা_2 *client_বার্তা)
{
	কাঠামো mosquitto *context;

	যদি(client_বার্তা == ফাঁকা || client_বার্তা->clientid == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_বার্তা->clientid, strlen(client_বার্তা->clientid), context);
	যদি(context == ফাঁকা){
		ফেরত MOSQ_ERR_NOT_FOUND;
	}

	যদি(client_বার্তা->direction == mosq_bmd_in || client_বার্তা->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	যদি(client_বার্তা->direction == mosq_bmd_out || client_বার্তা->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_subscription_add(ধ্রুবক কাঠামো mosquitto_subscription *sub)
{
	কাঠামো mosquitto *context;

	যদি(sub == ফাঁকা || sub->clientid == ফাঁকা || sub->topic_filter == ফাঁকা || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		ফেরত MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, strlen(sub->clientid), context);

	যদি(context){
		ফেরত sub__add(context, sub);
	}অন্যথা{
		ফেরত MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_subscription_delete(ধ্রুবক অক্ষর *clientid, ধ্রুবক অক্ষর *topic)
{
	কাঠামো mosquitto *context;
	uint8_t reason;

	যদি(clientid == ফাঁকা || topic == ফাঁকা || clientid[0] == '\0' || topic[0] == '\0'){
		ফেরত MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);

	যদি(context){
		ফেরত sub__remove(context, topic, &reason);
	}অন্যথা{
		ফেরত MOSQ_ERR_NOT_FOUND;
	}
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_persist_base_বার্তা_add(কাঠামো mosquitto_base_বার্তা_2 *বার্তা_add)
{
	কাঠামো mosquitto context;
	কাঠামো mosquitto_base_বার্তা *base_বার্তা;
	পূর্ণসংখ্যা rc;

	memset(&context, 0, আকার(context));

	যদি(বার্তা_add->payloadlen > MQTT_MAX_PAYLOAD){
		ফেরত MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (অক্ষর *)বার্তা_add->source_id;
	context.username = (অক্ষর *)বার্তা_add->source_username;

	base_বার্তা = mosquitto_calloc(1, আকার(কাঠামো mosquitto_base_বার্তা));
	যদি(base_বার্তা == ফাঁকা){
		যাও error;
	}
	base_বার্তা->তথ্য.store_id = বার্তা_add->store_id;
	base_বার্তা->তথ্য.expiry_time = বার্তা_add->expiry_time;
	base_বার্তা->তথ্য.payloadlen = বার্তা_add->payloadlen;
	base_বার্তা->তথ্য.source_mid = বার্তা_add->source_mid;
	base_বার্তা->তথ্য.qos = বার্তা_add->qos;
	base_বার্তা->তথ্য.retain = বার্তা_add->retain;

	base_বার্তা->তথ্য.payload = বার্তা_add->payload;
	বার্তা_add->payload = ফাঁকা;
	base_বার্তা->তথ্য.topic = বার্তা_add->topic;
	বার্তা_add->topic = ফাঁকা;
	base_বার্তা->তথ্য.properties = বার্তা_add->properties;
	বার্তা_add->properties = ফাঁকা;

	যদি(বার্তা_add->source_port){
		জন্য(পূর্ণসংখ্যা i=0; i<db.config->listener_গুনতি; i++){
			যদি(db.config->listeners[i].port == বার্তা_add->source_port){
				base_বার্তা->source_listener = &db.config->listeners[i];
				বিরতি;
			}
		}
	}

	base_বার্তা->stored = true;
	rc = db__message_store(&context, base_বার্তা, ফাঁকা, mosq_mo_broker);
	ফেরত rc;

error:
	mosquitto_property_free_all(&বার্তা_add->properties);
	mosquitto_free(বার্তা_add->topic);
	mosquitto_free(বার্তা_add->payload);
	mosquitto_free(base_বার্তা);

	ফেরত MOSQ_ERR_NOMEM;
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_persist_base_বার্তা_delete(uint64_t store_id)
{
	কাঠামো mosquitto_base_বার্তা *base_বার্তা;

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

	ফেরত MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT শূন্য mosquitto_complete_basic_auth(ধ্রুবক অক্ষর *clientid, পূর্ণসংখ্যা ফলাফল)
{
	কাঠামো mosquitto *context;

	যদি(clientid == ফাঁকা){
		ফেরত;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, strlen(clientid), context);
	যদি(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		যদি(ফলাফল == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, ফাঁকা, 0);
		}অন্যথা{
			যদি(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ফাঁকা);
			}অন্যথা{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ফাঁকা);
			}
			context->clean_start = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_broker_গ্রন্থি_id_set(uint16_t id)
{
	যদি(id > 1023){
		ফেরত MOSQ_ERR_INVAL;
	}অন্যথা{
		db.গ্রন্থি_id = id;
		db.গ্রন্থি_id_shifted = ((uint64_t)id) << 54;
		ফেরত MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT ধ্রুবক অক্ষর *mosquitto_persistence_location(শূন্য)
{
	ফেরত db.config->persistence_location;
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_client_will_set(ধ্রুবক অক্ষর *clientid, ধ্রুবক অক্ষর *topic, পূর্ণসংখ্যা payloadlen, ধ্রুবক শূন্য *payload, পূর্ণসংখ্যা qos, bool retain, mosquitto_property *properties)
{
	কাঠামো mosquitto *mosq = mosquitto_client(clientid);
	যদি(!mosq){
		ফেরত MOSQ_ERR_NOT_FOUND;
	}
	যদি(properties && mosq->protocol != mosq_p_mqtt5){
		যদি(net__is_connected(mosq)){
			ফেরত MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	ফেরত will__set(mosq, topic, payloadlen, payload, qos, retain, properties);
}
