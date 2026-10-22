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

#अन्तर्भाव "config.h"

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "send_mosq.h"
#अन्तर्भाव "util_mosq.h"
#अन्तर्भाव "will_mosq.h"
#अन्तर्भाव "utlist.h"
#अन्तर्भाव "will_mosq.h"

#यदि_परिभाषितम् WITH_TLS
#  include <openssl/ssl.h>
#यदि_अन्तः


BROKER_EXPORT पूर्णाङ्क mosquitto_plugin_समुच्चयः_info(mosquitto_plugin_id_t *identifier,
		ध्रुव वर्ण *plugin_name,
		ध्रुव वर्ण *plugin_संस्करणम्)
{
	यदि(identifier == रिक्त || plugin_name == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	यदि(plugin_संस्करणम्){
		identifier->plugin_संस्करणम् = mosquitto_strdup(plugin_संस्करणम्);
	}अन्यथा{
		identifier->plugin_संस्करणम् = रिक्त;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ध्रुव वर्ण *mosquitto_client_स्थानाङ्कः(ध्रुव संरचना mosquitto *client)
{
	यदि(client){
		प्रत्यागम client->स्थानाङ्कः;
	}अन्यथा{
		प्रत्यागम रिक्त;
	}
}


BROKER_EXPORT संरचना mosquitto *mosquitto_client(ध्रुव वर्ण *clientid)
{
	size_t len;
	संरचना mosquitto *context;

	यदि(!clientid){
		प्रत्यागम रिक्त;
	}
	len = वाक्यदैर्घ्यम्(clientid);
	यदि(len == 0){
		प्रत्यागम रिक्त;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, वाक्यदैर्घ्यम्(clientid), context);

	प्रत्यागम context;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_पत्तनम्(ध्रुव संरचना mosquitto *client)
{
	यदि(client && client->listener){
		प्रत्यागम client->listener->पत्तनम्;
	}अन्यथा{
		प्रत्यागम 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(ध्रुव संरचना mosquitto *client)
{
	यदि(client){
		प्रत्यागम client->clean_start;
	}अन्यथा{
		प्रत्यागम true;
	}
}


BROKER_EXPORT ध्रुव वर्ण *mosquitto_client_id(ध्रुव संरचना mosquitto *client)
{
	यदि(client){
		प्रत्यागम client->id;
	}अन्यथा{
		प्रत्यागम रिक्त;
	}
}


BROKER_EXPORT अचिह्नित mosquitto_client_id_hashv(ध्रुव संरचना mosquitto *client)
{
	यदि(client){
		प्रत्यागम client->id_hashv;
	}अन्यथा{
		प्रत्यागम 0;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_keepalive(ध्रुव संरचना mosquitto *client)
{
	यदि(client){
		प्रत्यागम client->keepalive;
	}अन्यथा{
		प्रत्यागम -1;
	}
}


BROKER_EXPORT शून्य *mosquitto_client_certificate(ध्रुव संरचना mosquitto *client)
{
#यदि_परिभाषितम् WITH_TLS
	यदि(client && client->ssl){
		प्रत्यागम SSL_get_peer_certificate(client->ssl);
	}अन्यथा{
		प्रत्यागम रिक्त;
	}
#अन्यथा
	अप्रयुक्तम्(client);

	प्रत्यागम रिक्त;
#यदि_अन्तः
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_protocol(ध्रुव संरचना mosquitto *client)
{
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	यदि(client && client->wsi){
		प्रत्यागम mp_websockets;
	}अन्यथा
#नोचेद्यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	यदि(client && client->transport == mosq_t_ws){
		प्रत्यागम mp_websockets;
	}अन्यथा
#अन्यथा
	अप्रयुक्तम्(client);
#यदि_अन्तः
	{
		प्रत्यागम mp_mqtt;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_protocol_संस्करणम्(ध्रुव संरचना mosquitto *client)
{
	यदि(client){
		विकल्प(client->protocol){
			पक्ष mosq_p_mqtt31:
				प्रत्यागम 3;
			पक्ष mosq_p_mqtt311:
				प्रत्यागम 4;
			पक्ष mosq_p_mqtt5:
				प्रत्यागम 5;
			स्वभाव:
				प्रत्यागम 0;
		}
	}अन्यथा{
		प्रत्यागम 0;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_sub_गणनम्(ध्रुव संरचना mosquitto *client)
{
	यदि(client){
		प्रत्यागम client->subs_गणनम्;
	}अन्यथा{
		प्रत्यागम 0;
	}
}


BROKER_EXPORT ध्रुव वर्ण *mosquitto_client_username(ध्रुव संरचना mosquitto *client)
{
	यदि(client){
#यदि_परिभाषितम् WITH_BRIDGE
		यदि(client->bridge){
			प्रत्यागम client->bridge->local_username;
		}अन्यथा
#यदि_अन्तः
		{
			प्रत्यागम client->username;
		}
	}अन्यथा{
		प्रत्यागम रिक्त;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_broker_publish(
		ध्रुव वर्ण *clientid,
		ध्रुव वर्ण *topic,
		पूर्णाङ्क payloadlen,
		शून्य *payload,
		पूर्णाङ्क qos,
		bool retain,
		mosquitto_property *properties)
{
	संरचना mosquitto_सन्देशः_v5 *सन्देशः;

	यदि(topic == रिक्त
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == रिक्त)
			|| qos < 0 || qos > 2){

		प्रत्यागम MOSQ_ERR_INVAL;
	}

	सन्देशः = mosquitto_malloc(परिमाण(संरचना mosquitto_सन्देशः_v5));
	यदि(सन्देशः == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	सन्देशः->अग्र = रिक्त;
	सन्देशः->पूर्व = रिक्त;
	यदि(clientid){
		सन्देशः->clientid = mosquitto_strdup(clientid);
		यदि(सन्देशः->clientid == रिक्त){
			mosquitto_FREE(सन्देशः);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}अन्यथा{
		सन्देशः->clientid = रिक्त;
	}
	सन्देशः->topic = mosquitto_strdup(topic);
	यदि(सन्देशः->topic == रिक्त){
		mosquitto_FREE(सन्देशः->clientid);
		mosquitto_FREE(सन्देशः);
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	सन्देशः->payloadlen = payloadlen;
	सन्देशः->payload = payload;
	सन्देशः->qos = qos;
	सन्देशः->retain = retain;
	सन्देशः->properties = properties;

	DL_APPEND(db.plugin_msgs, सन्देशः);

	loop_update_अग्र_event(1);
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_broker_publish_copy(
		ध्रुव वर्ण *clientid,
		ध्रुव वर्ण *topic,
		पूर्णाङ्क payloadlen,
		ध्रुव शून्य *payload,
		पूर्णाङ्क qos,
		bool retain,
		mosquitto_property *properties)
{
	शून्य *payload_बहिर्गमनम्;
	पूर्णाङ्क rc;

	यदि(topic == रिक्त
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == रिक्त)
			|| qos < 0 || qos > 2){

		प्रत्यागम MOSQ_ERR_INVAL;
	}

	payload_बहिर्गमनम् = mosquitto_calloc(1, (size_t)(payloadlen+1));
	यदि(payload_बहिर्गमनम् == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	स्मृतिं_प्रतिलिख(payload_बहिर्गमनम्, payload, (size_t)payloadlen);

	rc = mosquitto_broker_publish(
			clientid,
			topic,
			payloadlen,
			payload_बहिर्गमनम्,
			qos,
			retain,
			properties);

	यदि(rc){
		mosquitto_FREE(payload_बहिर्गमनम्);
	}
	प्रत्यागम rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_समुच्चयः_username(संरचना mosquitto *client, ध्रुव वर्ण *username)
{
	वर्ण *u_dup;
	वर्ण *old;

	यदि(!client){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	यदि(username){
		यदि(mosquitto_validate_utf8(username, (पूर्णाङ्क)वाक्यदैर्घ्यम्(username))){
			प्रत्यागम MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		यदि(!u_dup){
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}अन्यथा{
		u_dup = रिक्त;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_समुच्चयः_clientid(संरचना mosquitto *client, ध्रुव वर्ण *clientid)
{
	संरचना mosquitto *प्राप्तम्_client;
	वर्ण *id_dup;
	bool अन्तर्_by_id;
	पूर्णाङ्क clientid_len;

	यदि(!client || !clientid){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	अन्तर्_by_id = client->अन्तर्_by_id;
	/* If in_by_id is true, then this client has already authenticated and
	 * completed the connection flow. This means it *cannot* take over an
	 * existing session, and we must remove/add it to the by_id hash table.
	 *
	 * If in_by_id is false, then this client is currently going through
	 * authentication and so it is safe to change the client id to any value
	 * because it will be checked after authentication.
	 */

	यदि(अन्तर्_by_id){
		HASH_FIND(hh_id, db.contexts_by_id, clientid, वाक्यदैर्घ्यम्(clientid), प्राप्तम्_client);
		यदि(प्राप्तम्_client){
			प्रत्यागम MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (पूर्णाङ्क)वाक्यदैर्घ्यम्(clientid);
	यदि(mosquitto_validate_utf8(clientid, clientid_len)){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	यदि(!id_dup){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	यदि(अन्तर्_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	यदि(अन्तर्_by_id){
		context_सङ्कलय_गन्तव्यम्_by_id(client);
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
स्थिर शून्य check_subscription_acls(संरचना mosquitto *context)
{
	पूर्णाङ्क rc;
	uint8_t reason;

	प्रति(पूर्णाङ्क i=0; i<context->subs_क्षमता; i++){
		यदि(context->subs[i] == रिक्त){
			अनुवर्त;
		}
		rc = mosquitto_acl_check(context,
				context->subs[i]->topic_filter,
				0,
				रिक्त,
				0, /* FIXME */
				false,
				रिक्त,
				MOSQ_ACL_SUBSCRIBE);

		यदि(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


स्थिर शून्य disconnect_client(संरचना mosquitto *context, bool with_will)
{
	यदि(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, रिक्त);
	}
	यदि(with_will == false){
		mosquitto_समुच्चयः_state(context, mosq_cs_disconnecting);
	}
	यदि(context->session_expiry_interval != MQTT_SESSION_EXPIRY_तात्कालिकम्){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT पूर्णाङ्क mosquitto_kick_client_by_clientid(ध्रुव वर्ण *clientid, bool with_will)
{
	संरचना mosquitto *ctxt, *ctxt_tmp;

	यदि(clientid == रिक्त){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अन्यथा{
		HASH_FIND(hh_id, db.contexts_by_id, clientid, वाक्यदैर्घ्यम्(clientid), ctxt);
		यदि(ctxt){
			disconnect_client(ctxt, with_will);
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}अन्यथा{
			प्रत्यागम MOSQ_ERR_NOT_प्राप्तम्;
		}
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_kick_client_by_username(ध्रुव वर्ण *username, bool with_will)
{
	संरचना mosquitto *ctxt, *ctxt_tmp;

	यदि(username == रिक्त){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			यदि(ctxt->username == रिक्त){
				disconnect_client(ctxt, with_will);
			}
		}
	}अन्यथा{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			यदि(ctxt->username != रिक्त && !वाक्यं_तुलय(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_apply_on_all_clients(पूर्णाङ्क (*FUNC_client_functor)(ध्रुव संरचना mosquitto *, शून्य *), शून्य *functor_context)
{
	पूर्णाङ्क rc = MOSQ_ERR_SUCCESS;
	संरचना mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		यदि(rc != MOSQ_ERR_SUCCESS){
			भङ्ग;
		}
	}

	प्रत्यागम rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_सङ्कलय(संरचना mosquitto_client *client)
{
	संरचना mosquitto *context;
	पूर्णाङ्क rc;

	यदि(client == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(client->clientid == रिक्त){
		rc = MOSQ_ERR_INVAL;
		गच्छ error;
	}

	context = रिक्त;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, वाक्यदैर्घ्यम्(client->clientid), context);
	यदि(context){
		rc = MOSQ_ERR_INVAL;
		गच्छ error;
	}

	context = context__init();
	यदि(!context){
		rc = MOSQ_ERR_NOMEM;
		गच्छ error;
	}

	context->id = client->clientid;
	client->clientid = रिक्त;
	context->username = client->username;
	client->username = रिक्त;
	context->auth_विधानम् = client->auth_विधानम्;
	client->auth_विधानम् = रिक्त;

	context->clean_start = false;
	context->will_delay_time = client->will_delay_time;
	context->session_expiry_time = client->session_expiry_time;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->अधिकतमम्_packet_आकार = client->max_packet_आकार;
	context->retain_उपलब्धम् = client->retain_उपलब्धम्;
	context->is_persisted = true;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	यदि(db.config->प्रत्येकम्_listener_settings && client->listener_पत्तनम् > 0){
		प्रति(पूर्णाङ्क i=0; i < db.config->listener_गणनम्; i++){
			यदि(db.config->listeners[i].पत्तनम् == client->listener_पत्तनम्){
				context->listener = &db.config->listeners[i];
				भङ्ग;
			}
		}
	}

	context_सङ्कलय_गन्तव्यम्_by_id(context);
	session_expiry_सङ्कलय_from_persistence(context, context->session_expiry_time);

	प्रत्यागम MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->clientid);
	SAFE_FREE(client->username);
	SAFE_FREE(client->auth_विधानम्);
	प्रत्यागम rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_update(संरचना mosquitto_client *client)
{
	संरचना mosquitto *context;
	पूर्णाङ्क rc;

	यदि(client == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(client->clientid == रिक्त){
		rc = MOSQ_ERR_INVAL;
		गच्छ error;
	}

	context = रिक्त;
	HASH_FIND(hh_id, db.contexts_by_id, client->clientid, वाक्यदैर्घ्यम्(client->clientid), context);
	यदि(context == रिक्त){
		rc = MOSQ_ERR_NOT_प्राप्तम्;
		गच्छ error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = रिक्त;

	context->clean_start = false;
	context->will_delay_time = client->will_delay_time;
	context->session_expiry_time = client->session_expiry_time;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->अधिकतमम्_packet_आकार = client->max_packet_आकार;
	context->retain_उपलब्धम् = client->retain_उपलब्धम्;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	यदि(db.config->प्रत्येकम्_listener_settings && client->listener_पत्तनम् > 0){
		प्रति(पूर्णाङ्क i=0; i < db.config->listener_गणनम्; i++){
			यदि(db.config->listeners[i].पत्तनम् == client->listener_पत्तनम्){
				context->listener = &db.config->listeners[i];
				भङ्ग;
			}
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
error:
	SAFE_FREE(client->username);
	प्रत्यागम rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_delete(ध्रुव वर्ण *clientid)
{
	संरचना mosquitto *context;

	यदि(clientid == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	context = रिक्त;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, वाक्यदैर्घ्यम्(clientid), context);
	यदि(context == रिक्त){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_start = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_तात्कालिकम्;
	context->is_persisted = false;
	mosquitto_समुच्चयः_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर संरचना mosquitto_आधारः_सन्देशः *find_store_सन्देशः(uint64_t store_id)
{
	संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः;

	HASH_FIND(hh, db.सन्देशः_store, &store_id, परिमाण(store_id), आधारः_सन्देशः);
	प्रत्यागम आधारः_सन्देशः;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_सन्देशः_सङ्कलय(संरचना mosquitto_client_सन्देशः_2 *client_सन्देशः)
{
	संरचना mosquitto *context;
	संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः;

	यदि(client_सन्देशः == रिक्त || client_सन्देशः->clientid == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_सन्देशः->clientid, वाक्यदैर्घ्यम्(client_सन्देशः->clientid), context);
	यदि(context == रिक्त){
		प्रत्यागम MOSQ_ERR_NOT_प्राप्तम्;
	}
	आधारः_सन्देशः = find_store_सन्देशः(client_सन्देशः->store_id);
	यदि(आधारः_सन्देशः == रिक्त){
		प्रत्यागम MOSQ_ERR_NOT_प्राप्तम्;
	}

	यदि(client_सन्देशः->direction == mosq_md_बहिर्गमनम्){
		यदि(client_सन्देशः->qos > 0){
			context->चरमः_mid = client_सन्देशः->mid;
		}
		प्रत्यागम db_सन्देशः_निवेशय_outgoing(context, client_सन्देशः->cmsg_id, client_सन्देशः->mid,
				client_सन्देशः->qos, client_सन्देशः->retain,
				आधारः_सन्देशः, client_सन्देशः->subscription_identifier, false, false);
	}अन्यथा यदि(client_सन्देशः->direction == mosq_md_अन्तर्){
		प्रत्यागम db_सन्देशः_निवेशय_incoming(context, client_सन्देशः->cmsg_id, आधारः_सन्देशः, false);
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_सन्देशः_delete(संरचना mosquitto_client_सन्देशः_2 *client_सन्देशः)
{
	संरचना mosquitto *context;

	यदि(client_सन्देशः == रिक्त || client_सन्देशः->clientid == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_सन्देशः->clientid, वाक्यदैर्घ्यम्(client_सन्देशः->clientid), context);
	यदि(context == रिक्त){
		प्रत्यागम MOSQ_ERR_NOT_प्राप्तम्;
	}


	पूर्णाङ्क rc = MOSQ_ERR_INVAL;
	यदि(client_सन्देशः->direction == mosq_md_बहिर्गमनम्){
		rc = db_सन्देशः_delete_outgoing(context, client_सन्देशः->mid, mosq_ms_any, client_सन्देशः->qos);
	}अन्यथा यदि(client_सन्देशः->direction == mosq_md_अन्तर्){
		rc = db_सन्देशः_remove_incoming(context, client_सन्देशः->mid);
	}
	प्रत्यागम rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_सन्देशः_update(संरचना mosquitto_client_सन्देशः_2 *client_सन्देशः)
{
	संरचना mosquitto *context;

	यदि(client_सन्देशः == रिक्त || client_सन्देशः->clientid == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_सन्देशः->clientid, वाक्यदैर्घ्यम्(client_सन्देशः->clientid), context);
	यदि(context == रिक्त){
		प्रत्यागम MOSQ_ERR_NOT_प्राप्तम्;
	}

	यदि(client_सन्देशः->direction == mosq_md_बहिर्गमनम्){
		db_सन्देशः_update_outgoing(context, client_सन्देशः->mid, client_सन्देशः->state, client_सन्देशः->qos, false);
	}अन्यथा यदि(client_सन्देशः->direction == mosq_md_अन्तर्){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_client_सन्देशः_clear(संरचना mosquitto_client_सन्देशः_2 *client_सन्देशः)
{
	संरचना mosquitto *context;

	यदि(client_सन्देशः == रिक्त || client_सन्देशः->clientid == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, client_सन्देशः->clientid, वाक्यदैर्घ्यम्(client_सन्देशः->clientid), context);
	यदि(context == रिक्त){
		प्रत्यागम MOSQ_ERR_NOT_प्राप्तम्;
	}

	यदि(client_सन्देशः->direction == mosq_bmd_अन्तर् || client_सन्देशः->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	यदि(client_सन्देशः->direction == mosq_bmd_बहिर्गमनम् || client_सन्देशः->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_subscription_सङ्कलय(ध्रुव संरचना mosquitto_subscription *sub)
{
	संरचना mosquitto *context;

	यदि(sub == रिक्त || sub->clientid == रिक्त || sub->topic_filter == रिक्त || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, sub->clientid, वाक्यदैर्घ्यम्(sub->clientid), context);

	यदि(context){
		प्रत्यागम sub_सङ्कलय(context, sub);
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_NOT_प्राप्तम्;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_subscription_delete(ध्रुव वर्ण *clientid, ध्रुव वर्ण *topic)
{
	संरचना mosquitto *context;
	uint8_t reason;

	यदि(clientid == रिक्त || topic == रिक्त || clientid[0] == '\0' || topic[0] == '\0'){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh_id, db.contexts_by_id, clientid, वाक्यदैर्घ्यम्(clientid), context);

	यदि(context){
		प्रत्यागम sub__remove(context, topic, &reason);
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_NOT_प्राप्तम्;
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_आधारः_सन्देशः_सङ्कलय(संरचना mosquitto_आधारः_सन्देशः_2 *सन्देशः_सङ्कलय)
{
	संरचना mosquitto context;
	संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः;
	पूर्णाङ्क rc;

	स्मृतिं_पूरय(&context, 0, परिमाण(context));

	यदि(सन्देशः_सङ्कलय->payloadlen > MQTT_MAX_PAYLOAD){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (वर्ण *)सन्देशः_सङ्कलय->source_id;
	context.username = (वर्ण *)सन्देशः_सङ्कलय->source_username;

	आधारः_सन्देशः = mosquitto_calloc(1, परिमाण(संरचना mosquitto_आधारः_सन्देशः));
	यदि(आधारः_सन्देशः == रिक्त){
		गच्छ error;
	}
	आधारः_सन्देशः->दत्त.store_id = सन्देशः_सङ्कलय->store_id;
	आधारः_सन्देशः->दत्त.expiry_time = सन्देशः_सङ्कलय->expiry_time;
	आधारः_सन्देशः->दत्त.payloadlen = सन्देशः_सङ्कलय->payloadlen;
	आधारः_सन्देशः->दत्त.source_mid = सन्देशः_सङ्कलय->source_mid;
	आधारः_सन्देशः->दत्त.qos = सन्देशः_सङ्कलय->qos;
	आधारः_सन्देशः->दत्त.retain = सन्देशः_सङ्कलय->retain;

	आधारः_सन्देशः->दत्त.payload = सन्देशः_सङ्कलय->payload;
	सन्देशः_सङ्कलय->payload = रिक्त;
	आधारः_सन्देशः->दत्त.topic = सन्देशः_सङ्कलय->topic;
	सन्देशः_सङ्कलय->topic = रिक्त;
	आधारः_सन्देशः->दत्त.properties = सन्देशः_सङ्कलय->properties;
	सन्देशः_सङ्कलय->properties = रिक्त;

	यदि(सन्देशः_सङ्कलय->source_पत्तनम्){
		प्रति(पूर्णाङ्क i=0; i<db.config->listener_गणनम्; i++){
			यदि(db.config->listeners[i].पत्तनम् == सन्देशः_सङ्कलय->source_पत्तनम्){
				आधारः_सन्देशः->source_listener = &db.config->listeners[i];
				भङ्ग;
			}
		}
	}

	आधारः_सन्देशः->stored = true;
	rc = db_सन्देशः_store(&context, आधारः_सन्देशः, रिक्त, mosq_mo_broker);
	प्रत्यागम rc;

error:
	mosquitto_property_free_all(&सन्देशः_सङ्कलय->properties);
	mosquitto_free(सन्देशः_सङ्कलय->topic);
	mosquitto_free(सन्देशः_सङ्कलय->payload);
	mosquitto_free(आधारः_सन्देशः);

	प्रत्यागम MOSQ_ERR_NOMEM;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_आधारः_सन्देशः_delete(uint64_t store_id)
{
	संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः;

	आधारः_सन्देशः = find_store_सन्देशः(store_id);
	यदि(आधारः_सन्देशः && आधारः_सन्देशः->ref_गणनम् == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_सन्देशः_store_remove(आधारः_सन्देशः, false);
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT शून्य mosquitto_complete_basic_auth(ध्रुव वर्ण *clientid, पूर्णाङ्क परिणाम)
{
	संरचना mosquitto *context;

	यदि(clientid == रिक्त){
		प्रत्यागम;
	}

	HASH_FIND(hh_id, db.contexts_by_id_delayed_auth, clientid, वाक्यदैर्घ्यम्(clientid), context);
	यदि(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		यदि(परिणाम == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, रिक्त, 0);
		}अन्यथा{
			यदि(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, रिक्त);
			}अन्यथा{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, रिक्त);
			}
			context->clean_start = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_तात्कालिकम्;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT पूर्णाङ्क mosquitto_broker_ग्रन्थिः_id_समुच्चयः(uint16_t id)
{
	यदि(id > 1023){
		प्रत्यागम MOSQ_ERR_INVAL;
	}अन्यथा{
		db.ग्रन्थिः_id = id;
		db.ग्रन्थिः_id_shifted = ((uint64_t)id) << 54;
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT ध्रुव वर्ण *mosquitto_persistence_location(शून्य)
{
	प्रत्यागम db.config->persistence_location;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_client_will_समुच्चयः(ध्रुव वर्ण *clientid, ध्रुव वर्ण *topic, पूर्णाङ्क payloadlen, ध्रुव शून्य *payload, पूर्णाङ्क qos, bool retain, mosquitto_property *properties)
{
	संरचना mosquitto *mosq = mosquitto_client(clientid);
	यदि(!mosq){
		प्रत्यागम MOSQ_ERR_NOT_प्राप्तम्;
	}
	यदि(properties && mosq->protocol != mosq_p_mqtt5){
		यदि(net__is_connected(mosq)){
			प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	प्रत्यागम will_समुच्चयः(mosq, topic, payloadlen, payload, qos, retain, properties);
}
