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

#అన్తర్భావ "config.h"

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "send_mosq.h"
#అన్తర్భావ "util_mosq.h"
#అన్తర్భావ "will_mosq.h"
#అన్తర్భావ "utlist.h"
#అన్తర్భావ "will_mosq.h"

#నిర్వచితమైతే WITH_TLS
#  include <openssl/ssl.h>
#అయితే_ముగింపు


BROKER_EXPORT పూర్ణాంకం mosquitto_plugin_సమితి_info(mosquitto_plugin_id_t *identifier,
		స్థిరరాశి అక్షరం *plugin_name,
		స్థిరరాశి అక్షరం *plugin_సంస్కరణ)
{
	ఒకవేళ(identifier == ఖాళీ || plugin_name == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	ఒకవేళ(plugin_సంస్కరణ){
		identifier->plugin_సంస్కరణ = mosquitto_strdup(plugin_సంస్కరణ);
	}లేకపోతే{
		identifier->plugin_సంస్కరణ = ఖాళీ;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT స్థిరరాశి అక్షరం *mosquitto_client_చిరునామా(స్థిరరాశి నిర్మాణం mosquitto *client)
{
	ఒకవేళ(client){
		తిరిగి client->చిరునామా;
	}లేకపోతే{
		తిరిగి ఖాళీ;
	}
}


BROKER_EXPORT నిర్మాణం mosquitto *mosquitto_client(స్థిరరాశి అక్షరం *clientid)
{
	size_t len;
	నిర్మాణం mosquitto *context;

	ఒకవేళ(!clientid){
		తిరిగి ఖాళీ;
	}
	len = అక్షరశ్రేణి_పొడవు(clientid);
	ఒకవేళ(len == 0){
		తిరిగి ఖాళీ;
	}

	HASH_కనుగొను(hh_id, db.contexts_by_id, clientid, అక్షరశ్రేణి_పొడవు(clientid), context);

	తిరిగి context;
}


BROKER_EXPORT పూర్ణాంకం mosquitto_client_పోర్టు(స్థిరరాశి నిర్మాణం mosquitto *client)
{
	ఒకవేళ(client && client->listener){
		తిరిగి client->listener->పోర్టు;
	}లేకపోతే{
		తిరిగి 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(స్థిరరాశి నిర్మాణం mosquitto *client)
{
	ఒకవేళ(client){
		తిరిగి client->clean_ప్రారంభం;
	}లేకపోతే{
		తిరిగి true;
	}
}


BROKER_EXPORT స్థిరరాశి అక్షరం *mosquitto_client_id(స్థిరరాశి నిర్మాణం mosquitto *client)
{
	ఒకవేళ(client){
		తిరిగి client->id;
	}లేకపోతే{
		తిరిగి ఖాళీ;
	}
}


BROKER_EXPORT గుర్తులేని mosquitto_client_id_hashv(స్థిరరాశి నిర్మాణం mosquitto *client)
{
	ఒకవేళ(client){
		తిరిగి client->id_hashv;
	}లేకపోతే{
		తిరిగి 0;
	}
}


BROKER_EXPORT పూర్ణాంకం mosquitto_client_keepalive(స్థిరరాశి నిర్మాణం mosquitto *client)
{
	ఒకవేళ(client){
		తిరిగి client->keepalive;
	}లేకపోతే{
		తిరిగి -1;
	}
}


BROKER_EXPORT శూన్య *mosquitto_client_certificate(స్థిరరాశి నిర్మాణం mosquitto *client)
{
#నిర్వచితమైతే WITH_TLS
	ఒకవేళ(client && client->ssl){
		తిరిగి SSL_get_peer_certificate(client->ssl);
	}లేకపోతే{
		తిరిగి ఖాళీ;
	}
#లేకపోతే
	వాడని(client);

	తిరిగి ఖాళీ;
#అయితే_ముగింపు
}


BROKER_EXPORT పూర్ణాంకం mosquitto_client_protocol(స్థిరరాశి నిర్మాణం mosquitto *client)
{
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	ఒకవేళ(client && client->wsi){
		తిరిగి mp_websockets;
	}లేకపోతే
#లేదావేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	ఒకవేళ(client && client->transport == mosq_t_ws){
		తిరిగి mp_websockets;
	}లేకపోతే
#లేకపోతే
	వాడని(client);
#అయితే_ముగింపు
	{
		తిరిగి mp_mqtt;
	}
}


BROKER_EXPORT పూర్ణాంకం mosquitto_client_protocol_సంస్కరణ(స్థిరరాశి నిర్మాణం mosquitto *client)
{
	ఒకవేళ(client){
		ఎంపిక(client->protocol){
			స్థితి mosq_p_mqtt31:
				తిరిగి 3;
			స్థితి mosq_p_mqtt311:
				తిరిగి 4;
			స్థితి mosq_p_mqtt5:
				తిరిగి 5;
			అప్రమేయ:
				తిరిగి 0;
		}
	}లేకపోతే{
		తిరిగి 0;
	}
}


BROKER_EXPORT పూర్ణాంకం mosquitto_client_sub_లెక్క(స్థిరరాశి నిర్మాణం mosquitto *client)
{
	ఒకవేళ(client){
		తిరిగి client->subs_లెక్క;
	}లేకపోతే{
		తిరిగి 0;
	}
}


BROKER_EXPORT స్థిరరాశి అక్షరం *mosquitto_client_username(స్థిరరాశి నిర్మాణం mosquitto *client)
{
	ఒకవేళ(client){
#నిర్వచితమైతే WITH_BRIDGE
		ఒకవేళ(client->bridge){
			తిరిగి client->bridge->local_username;
		}లేకపోతే
#అయితే_ముగింపు
		{
			తిరిగి client->username;
		}
	}లేకపోతే{
		తిరిగి ఖాళీ;
	}
}


BROKER_EXPORT పూర్ణాంకం mosquitto_broker_publish(
		స్థిరరాశి అక్షరం *clientid,
		స్థిరరాశి అక్షరం *topic,
		పూర్ణాంకం payloadlen,
		శూన్య *payload,
		పూర్ణాంకం qos,
		bool retain,
		mosquitto_property *properties)
{
	నిర్మాణం mosquitto_సందేశం_v5 *సందేశం;

	ఒకవేళ(topic == ఖాళీ
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == ఖాళీ)
			|| qos < 0 || qos > 2){

		తిరిగి MOSQ_ERR_INVAL;
	}

	సందేశం = mosquitto_malloc(పరిమాణం(నిర్మాణం mosquitto_సందేశం_v5));
	ఒకవేళ(సందేశం == ఖాళీ){
		తిరిగి MOSQ_ERR_NOMEM;
	}

	సందేశం->తదుపరి = ఖాళీ;
	సందేశం->prev = ఖాళీ;
	ఒకవేళ(clientid){
		సందేశం->clientid = mosquitto_strdup(clientid);
		ఒకవేళ(సందేశం->clientid == ఖాళీ){
			mosquitto_FREE(సందేశం);
			తిరిగి MOSQ_ERR_NOMEM;
		}
	}లేకపోతే{
		సందేశం->clientid = ఖాళీ;
	}
	సందేశం->topic = mosquitto_strdup(topic);
	ఒకవేళ(సందేశం->topic == ఖాళీ){
		mosquitto_FREE(సందేశం->clientid);
		mosquitto_FREE(సందేశం);
		తిరిగి MOSQ_ERR_NOMEM;
	}
	సందేశం->payloadlen = payloadlen;
	సందేశం->payload = payload;
	సందేశం->qos = qos;
	సందేశం->retain = retain;
	సందేశం->properties = properties;

	DL_APPEND(db.plugin_msgs, సందేశం);

	loop_update_తదుపరి_event(1);
	తిరిగి MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT పూర్ణాంకం mosquitto_broker_publish_copy(
		స్థిరరాశి అక్షరం *clientid,
		స్థిరరాశి అక్షరం *topic,
		పూర్ణాంకం payloadlen,
		స్థిరరాశి శూన్య *payload,
		పూర్ణాంకం qos,
		bool retain,
		mosquitto_property *properties)
{
	శూన్య *payload_బహిర్గతం;
	పూర్ణాంకం rc;

	ఒకవేళ(topic == ఖాళీ
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == ఖాళీ)
			|| qos < 0 || qos > 2){

		తిరిగి MOSQ_ERR_INVAL;
	}

	payload_బహిర్గతం = mosquitto_calloc(1, (size_t)(payloadlen+1));
	ఒకవేళ(payload_బహిర్గతం == ఖాళీ){
		తిరిగి MOSQ_ERR_NOMEM;
	}
	జ్ఞాపకం_నకలు(payload_బహిర్గతం, payload, (size_t)payloadlen);

	rc = mosquitto_broker_publish(
			clientid,
			topic,
			payloadlen,
			payload_బహిర్గతం,
			qos,
			retain,
			properties);

	ఒకవేళ(rc){
		mosquitto_FREE(payload_బహిర్గతం);
	}
	తిరిగి rc;
}


BROKER_EXPORT పూర్ణాంకం mosquitto_సమితి_username(నిర్మాణం mosquitto *client, స్థిరరాశి అక్షరం *username)
{
	అక్షరం *u_dup;
	అక్షరం *old;

	ఒకవేళ(!client){
		తిరిగి MOSQ_ERR_INVAL;
	}

	ఒకవేళ(username){
		ఒకవేళ(mosquitto_validate_utf8(username, (పూర్ణాంకం)అక్షరశ్రేణి_పొడవు(username))){
			తిరిగి MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		ఒకవేళ(!u_dup){
			తిరిగి MOSQ_ERR_NOMEM;
		}
	}లేకపోతే{
		u_dup = ఖాళీ;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	తిరిగి MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT పూర్ణాంకం mosquitto_సమితి_clientid(నిర్మాణం mosquitto *client, స్థిరరాశి అక్షరం *clientid)
{
	నిర్మాణం mosquitto *దొరికింది_client;
	అక్షరం *id_dup;
	bool లోపల_by_id;
	పూర్ణాంకం clientid_len;

	ఒకవేళ(!client || !clientid){
		తిరిగి MOSQ_ERR_INVAL;
	}

	లోపల_by_id = client->లోపల_by_id;
	/* If in_by_id is true, then this client has already authenticated and
	 * completed the connection flow. This means it *cannot* take over an
	 * existing session, and we must remove/add it to the by_id hash table.
	 *
	 * If in_by_id is false, then this client is currently going through
	 * authentication and so it is safe to change the client id to any value
	 * because it will be checked after authentication.
	 */

	ఒకవేళ(లోపల_by_id){
		HASH_కనుగొను(hh_id, db.contexts_by_id, clientid, అక్షరశ్రేణి_పొడవు(clientid), దొరికింది_client);
		ఒకవేళ(దొరికింది_client){
			తిరిగి MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (పూర్ణాంకం)అక్షరశ్రేణి_పొడవు(clientid);
	ఒకవేళ(mosquitto_validate_utf8(clientid, clientid_len)){
		తిరిగి MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	ఒకవేళ(!id_dup){
		తిరిగి MOSQ_ERR_NOMEM;
	}

	ఒకవేళ(లోపల_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	ఒకవేళ(లోపల_by_id){
		context_కూడు_గమ్యం_by_id(client);
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
స్థిర శూన్య check_subscription_acls(నిర్మాణం mosquitto *context)
{
	పూర్ణాంకం rc;
	uint8_t reason;

	కొరకు(పూర్ణాంకం i=0; i<context->subs_సామర్థ్యం; i++){
		ఒకవేళ(context->subs[i] == ఖాళీ){
			కొనసాగు;
		}
		rc = mosquitto_acl_check(context,
				context->subs[i]->topic_filter,
				0,
				ఖాళీ,
				0, /* FIXME */
				false,
				ఖాళీ,
				MOSQ_ACL_SUBSCRIBE);

		ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


స్థిర శూన్య disconnect_client(నిర్మాణం mosquitto *context, bool with_will)
{
	ఒకవేళ(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ఖాళీ);
	}
	ఒకవేళ(with_will == false){
		mosquitto_సమితి_state(context, mosq_cs_disconnecting);
	}
	ఒకవేళ(context->session_expiry_interval != MQTT_SESSION_EXPIRY_తక్షణ){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT పూర్ణాంకం mosquitto_kick_client_by_clientid(స్థిరరాశి అక్షరం *clientid, bool with_will)
{
	నిర్మాణం mosquitto *ctxt, *ctxt_tmp;

	ఒకవేళ(clientid == ఖాళీ){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		తిరిగి MOSQ_ERR_SUCCESS;
	}లేకపోతే{
		HASH_కనుగొను(hh_id, db.contexts_by_id, clientid, అక్షరశ్రేణి_పొడవు(clientid), ctxt);
		ఒకవేళ(ctxt){
			disconnect_client(ctxt, with_will);
			తిరిగి MOSQ_ERR_SUCCESS;
		}లేకపోతే{
			తిరిగి MOSQ_ERR_NOT_దొరికింది;
		}
	}
}


BROKER_EXPORT పూర్ణాంకం mosquitto_kick_client_by_username(స్థిరరాశి అక్షరం *username, bool with_will)
{
	నిర్మాణం mosquitto *ctxt, *ctxt_tmp;

	ఒకవేళ(username == ఖాళీ){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			ఒకవేళ(ctxt->username == ఖాళీ){
				disconnect_client(ctxt, with_will);
			}
		}
	}లేకపోతే{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			ఒకవేళ(ctxt->username != ఖాళీ && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT పూర్ణాంకం mosquitto_apply_on_all_clients(పూర్ణాంకం (*FUNC_client_functor)(స్థిరరాశి నిర్మాణం mosquitto *, శూన్య *), శూన్య *functor_context)
{
	పూర్ణాంకం rc = MOSQ_ERR_SUCCESS;
	నిర్మాణం mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
			ఆపు;
		}
	}

	తిరిగి rc;
}


BROKER_EXPORT పూర్ణాంకం mosquitto_persist_client_కూడు(నిర్మాణం mosquitto_client *client)
{
	నిర్మాణం mosquitto *context;
	పూర్ణాంకం rc;

	ఒకవేళ(client == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}
	ఒకవేళ(client->clientid == ఖాళీ){
		rc = MOSQ_ERR_INVAL;
		వెళ్ళు error;
	}

	context = ఖాళీ;
	HASH_కనుగొను(hh_id, db.contexts_by_id, client->clientid, అక్షరశ్రేణి_పొడవు(client->clientid), context);
	ఒకవేళ(context){
		rc = MOSQ_ERR_INVAL;
		వెళ్ళు error;
	}

	context = context__init();
	ఒకవేళ(!context){
		rc = MOSQ_ERR_NOMEM;
		వెళ్ళు error;
	}

	context->id = client->clientid;
	client->clientid = ఖాళీ;
	context->username = client->username;
	client->username = ఖాళీ;
	context->auth_పద్ధతి = client->auth_పద్ధతి;
	client->auth_పద్ధతి = ఖాళీ;

	context->clean_ప్రారంభం = false;
	context->will_delay_సమయం = client->will_delay_సమయం;
	context->session_expiry_సమయం = client->session_expiry_సమయం;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->గరిష్ఠ_packet_కొలత = client->max_packet_కొలత;
	context->retain_అందుబాటు = client->retain_అందుబాటు;
	context->is_persisted = true;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	ఒకవేళ(db.config->చొప్పున_listener_settings && client->listener_పోర్టు > 0){
		కొరకు(పూర్ణాంకం i=0; i < db.config->listener_లెక్క; i++){
			ఒకవేళ(db.config->listeners[i].పోర్టు == client->listener_పోర్టు){
				context->listener = &db.config->listeners[i];
				ఆపు;
			}
		}
	}

	context_కూడు_గమ్యం_by_id(context);
	session_expiry_కూడు_from_persistence(context, context->session_expiry_సమయం);

	తిరిగి MOSQ_ERR_SUCCESS;
error:
	సురక్షిత_FREE(client->clientid);
	సురక్షిత_FREE(client->username);
	సురక్షిత_FREE(client->auth_పద్ధతి);
	తిరిగి rc;
}


BROKER_EXPORT పూర్ణాంకం mosquitto_persist_client_update(నిర్మాణం mosquitto_client *client)
{
	నిర్మాణం mosquitto *context;
	పూర్ణాంకం rc;

	ఒకవేళ(client == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}
	ఒకవేళ(client->clientid == ఖాళీ){
		rc = MOSQ_ERR_INVAL;
		వెళ్ళు error;
	}

	context = ఖాళీ;
	HASH_కనుగొను(hh_id, db.contexts_by_id, client->clientid, అక్షరశ్రేణి_పొడవు(client->clientid), context);
	ఒకవేళ(context == ఖాళీ){
		rc = MOSQ_ERR_NOT_దొరికింది;
		వెళ్ళు error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = ఖాళీ;

	context->clean_ప్రారంభం = false;
	context->will_delay_సమయం = client->will_delay_సమయం;
	context->session_expiry_సమయం = client->session_expiry_సమయం;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->గరిష్ఠ_packet_కొలత = client->max_packet_కొలత;
	context->retain_అందుబాటు = client->retain_అందుబాటు;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	ఒకవేళ(db.config->చొప్పున_listener_settings && client->listener_పోర్టు > 0){
		కొరకు(పూర్ణాంకం i=0; i < db.config->listener_లెక్క; i++){
			ఒకవేళ(db.config->listeners[i].పోర్టు == client->listener_పోర్టు){
				context->listener = &db.config->listeners[i];
				ఆపు;
			}
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
error:
	సురక్షిత_FREE(client->username);
	తిరిగి rc;
}


BROKER_EXPORT పూర్ణాంకం mosquitto_persist_client_delete(స్థిరరాశి అక్షరం *clientid)
{
	నిర్మాణం mosquitto *context;

	ఒకవేళ(clientid == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}

	context = ఖాళీ;
	HASH_కనుగొను(hh_id, db.contexts_by_id, clientid, అక్షరశ్రేణి_పొడవు(clientid), context);
	ఒకవేళ(context == ఖాళీ){
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_ప్రారంభం = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_తక్షణ;
	context->is_persisted = false;
	mosquitto_సమితి_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర నిర్మాణం mosquitto_పునాది_సందేశం *కనుగొను_store_సందేశం(uint64_t store_id)
{
	నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం;

	HASH_కనుగొను(hh, db.సందేశం_store, &store_id, పరిమాణం(store_id), పునాది_సందేశం);
	తిరిగి పునాది_సందేశం;
}


BROKER_EXPORT పూర్ణాంకం mosquitto_persist_client_సందేశం_కూడు(నిర్మాణం mosquitto_client_సందేశం_2 *client_సందేశం)
{
	నిర్మాణం mosquitto *context;
	నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం;

	ఒకవేళ(client_సందేశం == ఖాళీ || client_సందేశం->clientid == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}

	HASH_కనుగొను(hh_id, db.contexts_by_id, client_సందేశం->clientid, అక్షరశ్రేణి_పొడవు(client_సందేశం->clientid), context);
	ఒకవేళ(context == ఖాళీ){
		తిరిగి MOSQ_ERR_NOT_దొరికింది;
	}
	పునాది_సందేశం = కనుగొను_store_సందేశం(client_సందేశం->store_id);
	ఒకవేళ(పునాది_సందేశం == ఖాళీ){
		తిరిగి MOSQ_ERR_NOT_దొరికింది;
	}

	ఒకవేళ(client_సందేశం->direction == mosq_md_బహిర్గతం){
		ఒకవేళ(client_సందేశం->qos > 0){
			context->చివరి_mid = client_సందేశం->mid;
		}
		తిరిగి db_సందేశం_చొప్పించు_outgoing(context, client_సందేశం->cmsg_id, client_సందేశం->mid,
				client_సందేశం->qos, client_సందేశం->retain,
				పునాది_సందేశం, client_సందేశం->subscription_identifier, false, false);
	}లేకపోతే ఒకవేళ(client_సందేశం->direction == mosq_md_లోపల){
		తిరిగి db_సందేశం_చొప్పించు_incoming(context, client_సందేశం->cmsg_id, పునాది_సందేశం, false);
	}లేకపోతే{
		తిరిగి MOSQ_ERR_INVAL;
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT పూర్ణాంకం mosquitto_persist_client_సందేశం_delete(నిర్మాణం mosquitto_client_సందేశం_2 *client_సందేశం)
{
	నిర్మాణం mosquitto *context;

	ఒకవేళ(client_సందేశం == ఖాళీ || client_సందేశం->clientid == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}

	HASH_కనుగొను(hh_id, db.contexts_by_id, client_సందేశం->clientid, అక్షరశ్రేణి_పొడవు(client_సందేశం->clientid), context);
	ఒకవేళ(context == ఖాళీ){
		తిరిగి MOSQ_ERR_NOT_దొరికింది;
	}


	పూర్ణాంకం rc = MOSQ_ERR_INVAL;
	ఒకవేళ(client_సందేశం->direction == mosq_md_బహిర్గతం){
		rc = db_సందేశం_delete_outgoing(context, client_సందేశం->mid, mosq_ms_any, client_సందేశం->qos);
	}లేకపోతే ఒకవేళ(client_సందేశం->direction == mosq_md_లోపల){
		rc = db_సందేశం_remove_incoming(context, client_సందేశం->mid);
	}
	తిరిగి rc;
}


BROKER_EXPORT పూర్ణాంకం mosquitto_persist_client_సందేశం_update(నిర్మాణం mosquitto_client_సందేశం_2 *client_సందేశం)
{
	నిర్మాణం mosquitto *context;

	ఒకవేళ(client_సందేశం == ఖాళీ || client_సందేశం->clientid == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}

	HASH_కనుగొను(hh_id, db.contexts_by_id, client_సందేశం->clientid, అక్షరశ్రేణి_పొడవు(client_సందేశం->clientid), context);
	ఒకవేళ(context == ఖాళీ){
		తిరిగి MOSQ_ERR_NOT_దొరికింది;
	}

	ఒకవేళ(client_సందేశం->direction == mosq_md_బహిర్గతం){
		db_సందేశం_update_outgoing(context, client_సందేశం->mid, client_సందేశం->state, client_సందేశం->qos, false);
	}లేకపోతే ఒకవేళ(client_సందేశం->direction == mosq_md_లోపల){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}లేకపోతే{
		తిరిగి MOSQ_ERR_INVAL;
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT పూర్ణాంకం mosquitto_persist_client_సందేశం_clear(నిర్మాణం mosquitto_client_సందేశం_2 *client_సందేశం)
{
	నిర్మాణం mosquitto *context;

	ఒకవేళ(client_సందేశం == ఖాళీ || client_సందేశం->clientid == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}

	HASH_కనుగొను(hh_id, db.contexts_by_id, client_సందేశం->clientid, అక్షరశ్రేణి_పొడవు(client_సందేశం->clientid), context);
	ఒకవేళ(context == ఖాళీ){
		తిరిగి MOSQ_ERR_NOT_దొరికింది;
	}

	ఒకవేళ(client_సందేశం->direction == mosq_bmd_లోపల || client_సందేశం->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	ఒకవేళ(client_సందేశం->direction == mosq_bmd_బహిర్గతం || client_సందేశం->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT పూర్ణాంకం mosquitto_subscription_కూడు(స్థిరరాశి నిర్మాణం mosquitto_subscription *sub)
{
	నిర్మాణం mosquitto *context;

	ఒకవేళ(sub == ఖాళీ || sub->clientid == ఖాళీ || sub->topic_filter == ఖాళీ || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		తిరిగి MOSQ_ERR_INVAL;
	}

	HASH_కనుగొను(hh_id, db.contexts_by_id, sub->clientid, అక్షరశ్రేణి_పొడవు(sub->clientid), context);

	ఒకవేళ(context){
		తిరిగి sub_కూడు(context, sub);
	}లేకపోతే{
		తిరిగి MOSQ_ERR_NOT_దొరికింది;
	}
}


BROKER_EXPORT పూర్ణాంకం mosquitto_subscription_delete(స్థిరరాశి అక్షరం *clientid, స్థిరరాశి అక్షరం *topic)
{
	నిర్మాణం mosquitto *context;
	uint8_t reason;

	ఒకవేళ(clientid == ఖాళీ || topic == ఖాళీ || clientid[0] == '\0' || topic[0] == '\0'){
		తిరిగి MOSQ_ERR_INVAL;
	}

	HASH_కనుగొను(hh_id, db.contexts_by_id, clientid, అక్షరశ్రేణి_పొడవు(clientid), context);

	ఒకవేళ(context){
		తిరిగి sub__remove(context, topic, &reason);
	}లేకపోతే{
		తిరిగి MOSQ_ERR_NOT_దొరికింది;
	}
}


BROKER_EXPORT పూర్ణాంకం mosquitto_persist_పునాది_సందేశం_కూడు(నిర్మాణం mosquitto_పునాది_సందేశం_2 *సందేశం_కూడు)
{
	నిర్మాణం mosquitto context;
	నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం;
	పూర్ణాంకం rc;

	memset(&context, 0, పరిమాణం(context));

	ఒకవేళ(సందేశం_కూడు->payloadlen > MQTT_MAX_PAYLOAD){
		తిరిగి MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (అక్షరం *)సందేశం_కూడు->ఆధారం_id;
	context.username = (అక్షరం *)సందేశం_కూడు->ఆధారం_username;

	పునాది_సందేశం = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto_పునాది_సందేశం));
	ఒకవేళ(పునాది_సందేశం == ఖాళీ){
		వెళ్ళు error;
	}
	పునాది_సందేశం->దత్తాంశం.store_id = సందేశం_కూడు->store_id;
	పునాది_సందేశం->దత్తాంశం.expiry_సమయం = సందేశం_కూడు->expiry_సమయం;
	పునాది_సందేశం->దత్తాంశం.payloadlen = సందేశం_కూడు->payloadlen;
	పునాది_సందేశం->దత్తాంశం.ఆధారం_mid = సందేశం_కూడు->ఆధారం_mid;
	పునాది_సందేశం->దత్తాంశం.qos = సందేశం_కూడు->qos;
	పునాది_సందేశం->దత్తాంశం.retain = సందేశం_కూడు->retain;

	పునాది_సందేశం->దత్తాంశం.payload = సందేశం_కూడు->payload;
	సందేశం_కూడు->payload = ఖాళీ;
	పునాది_సందేశం->దత్తాంశం.topic = సందేశం_కూడు->topic;
	సందేశం_కూడు->topic = ఖాళీ;
	పునాది_సందేశం->దత్తాంశం.properties = సందేశం_కూడు->properties;
	సందేశం_కూడు->properties = ఖాళీ;

	ఒకవేళ(సందేశం_కూడు->ఆధారం_పోర్టు){
		కొరకు(పూర్ణాంకం i=0; i<db.config->listener_లెక్క; i++){
			ఒకవేళ(db.config->listeners[i].పోర్టు == సందేశం_కూడు->ఆధారం_పోర్టు){
				పునాది_సందేశం->ఆధారం_listener = &db.config->listeners[i];
				ఆపు;
			}
		}
	}

	పునాది_సందేశం->stored = true;
	rc = db_సందేశం_store(&context, పునాది_సందేశం, ఖాళీ, mosq_mo_broker);
	తిరిగి rc;

error:
	mosquitto_property_free_all(&సందేశం_కూడు->properties);
	mosquitto_free(సందేశం_కూడు->topic);
	mosquitto_free(సందేశం_కూడు->payload);
	mosquitto_free(పునాది_సందేశం);

	తిరిగి MOSQ_ERR_NOMEM;
}


BROKER_EXPORT పూర్ణాంకం mosquitto_persist_పునాది_సందేశం_delete(uint64_t store_id)
{
	నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం;

	పునాది_సందేశం = కనుగొను_store_సందేశం(store_id);
	ఒకవేళ(పునాది_సందేశం && పునాది_సందేశం->ref_లెక్క == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_సందేశం_store_remove(పునాది_సందేశం, false);
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT శూన్య mosquitto_complete_basic_auth(స్థిరరాశి అక్షరం *clientid, పూర్ణాంకం ఫలితం)
{
	నిర్మాణం mosquitto *context;

	ఒకవేళ(clientid == ఖాళీ){
		తిరిగి;
	}

	HASH_కనుగొను(hh_id, db.contexts_by_id_delayed_auth, clientid, అక్షరశ్రేణి_పొడవు(clientid), context);
	ఒకవేళ(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		ఒకవేళ(ఫలితం == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, ఖాళీ, 0);
		}లేకపోతే{
			ఒకవేళ(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ఖాళీ);
			}లేకపోతే{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, ఖాళీ);
			}
			context->clean_ప్రారంభం = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_తక్షణ;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT పూర్ణాంకం mosquitto_broker_కణుపు_id_సమితి(uint16_t id)
{
	ఒకవేళ(id > 1023){
		తిరిగి MOSQ_ERR_INVAL;
	}లేకపోతే{
		db.కణుపు_id = id;
		db.కణుపు_id_shifted = ((uint64_t)id) << 54;
		తిరిగి MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT స్థిరరాశి అక్షరం *mosquitto_persistence_location(శూన్య)
{
	తిరిగి db.config->persistence_location;
}


BROKER_EXPORT పూర్ణాంకం mosquitto_client_will_సమితి(స్థిరరాశి అక్షరం *clientid, స్థిరరాశి అక్షరం *topic, పూర్ణాంకం payloadlen, స్థిరరాశి శూన్య *payload, పూర్ణాంకం qos, bool retain, mosquitto_property *properties)
{
	నిర్మాణం mosquitto *mosq = mosquitto_client(clientid);
	ఒకవేళ(!mosq){
		తిరిగి MOSQ_ERR_NOT_దొరికింది;
	}
	ఒకవేళ(properties && mosq->protocol != mosq_p_mqtt5){
		ఒకవేళ(net__is_connected(mosq)){
			తిరిగి MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	తిరిగి will_సమితి(mosq, topic, payloadlen, payload, qos, retain, properties);
}
