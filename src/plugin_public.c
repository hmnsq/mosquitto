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

#اگر_تعریف_شدہ WITH_TLS
#  include <openssl/ssl.h>
#اگر_ختم


BROKER_EXPORT صحیح mosquitto_plugin_سیٹ_info(mosquitto_plugin_id_t *identifier,
		ثابت حرف *plugin_name,
		ثابت حرف *plugin_نسخہ)
{
	اگر(identifier == تہی || plugin_name == تہی){
		واپس MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	اگر(plugin_نسخہ){
		identifier->plugin_نسخہ = mosquitto_strdup(plugin_نسخہ);
	}ورنہ{
		identifier->plugin_نسخہ = تہی;
	}

	واپس MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ثابت حرف *mosquitto_client_پتہ(ثابت ساخت mosquitto *client)
{
	اگر(client){
		واپس client->پتہ;
	}ورنہ{
		واپس تہی;
	}
}


BROKER_EXPORT ساخت mosquitto *mosquitto_client(ثابت حرف *clientid)
{
	size_t len;
	ساخت mosquitto *context;

	اگر(!clientid){
		واپس تہی;
	}
	len = لڑی_لمبائی(clientid);
	اگر(len == 0){
		واپس تہی;
	}

	HASH_ڈھونڈو(hh_id, db.contexts_by_id, clientid, لڑی_لمبائی(clientid), context);

	واپس context;
}


BROKER_EXPORT صحیح mosquitto_client_پورٹ(ثابت ساخت mosquitto *client)
{
	اگر(client && client->listener){
		واپس client->listener->پورٹ;
	}ورنہ{
		واپس 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(ثابت ساخت mosquitto *client)
{
	اگر(client){
		واپس client->clean_آغاز;
	}ورنہ{
		واپس true;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_client_id(ثابت ساخت mosquitto *client)
{
	اگر(client){
		واپس client->id;
	}ورنہ{
		واپس تہی;
	}
}


BROKER_EXPORT غیرعلامتی mosquitto_client_id_hashv(ثابت ساخت mosquitto *client)
{
	اگر(client){
		واپس client->id_hashv;
	}ورنہ{
		واپس 0;
	}
}


BROKER_EXPORT صحیح mosquitto_client_keepalive(ثابت ساخت mosquitto *client)
{
	اگر(client){
		واپس client->keepalive;
	}ورنہ{
		واپس -1;
	}
}


BROKER_EXPORT خالی *mosquitto_client_certificate(ثابت ساخت mosquitto *client)
{
#اگر_تعریف_شدہ WITH_TLS
	اگر(client && client->ssl){
		واپس SSL_get_peer_certificate(client->ssl);
	}ورنہ{
		واپس تہی;
	}
#ورنہ
	غیر_مستعمل(client);

	واپس تہی;
#اگر_ختم
}


BROKER_EXPORT صحیح mosquitto_client_protocol(ثابت ساخت mosquitto *client)
{
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
	اگر(client && client->wsi){
		واپس mp_websockets;
	}ورنہ
#وگرنہ_اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_BUILTIN
	اگر(client && client->transport == mosq_t_ws){
		واپس mp_websockets;
	}ورنہ
#ورنہ
	غیر_مستعمل(client);
#اگر_ختم
	{
		واپس mp_mqtt;
	}
}


BROKER_EXPORT صحیح mosquitto_client_protocol_نسخہ(ثابت ساخت mosquitto *client)
{
	اگر(client){
		انتخاب(client->protocol){
			حالت mosq_p_mqtt31:
				واپس 3;
			حالت mosq_p_mqtt311:
				واپس 4;
			حالت mosq_p_mqtt5:
				واپس 5;
			طےشدہ:
				واپس 0;
		}
	}ورنہ{
		واپس 0;
	}
}


BROKER_EXPORT صحیح mosquitto_client_sub_گنتی(ثابت ساخت mosquitto *client)
{
	اگر(client){
		واپس client->subs_گنتی;
	}ورنہ{
		واپس 0;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_client_username(ثابت ساخت mosquitto *client)
{
	اگر(client){
#اگر_تعریف_شدہ WITH_BRIDGE
		اگر(client->bridge){
			واپس client->bridge->local_username;
		}ورنہ
#اگر_ختم
		{
			واپس client->username;
		}
	}ورنہ{
		واپس تہی;
	}
}


BROKER_EXPORT صحیح mosquitto_broker_publish(
		ثابت حرف *clientid,
		ثابت حرف *topic,
		صحیح payloadlen,
		خالی *payload,
		صحیح qos,
		bool retain,
		mosquitto_property *properties)
{
	ساخت mosquitto_پیغام_v5 *پیغام;

	اگر(topic == تہی
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == تہی)
			|| qos < 0 || qos > 2){

		واپس MOSQ_ERR_INVAL;
	}

	پیغام = mosquitto_malloc(حجم(ساخت mosquitto_پیغام_v5));
	اگر(پیغام == تہی){
		واپس MOSQ_ERR_NOMEM;
	}

	پیغام->اگلا = تہی;
	پیغام->پچھلا = تہی;
	اگر(clientid){
		پیغام->clientid = mosquitto_strdup(clientid);
		اگر(پیغام->clientid == تہی){
			mosquitto_FREE(پیغام);
			واپس MOSQ_ERR_NOMEM;
		}
	}ورنہ{
		پیغام->clientid = تہی;
	}
	پیغام->topic = mosquitto_strdup(topic);
	اگر(پیغام->topic == تہی){
		mosquitto_FREE(پیغام->clientid);
		mosquitto_FREE(پیغام);
		واپس MOSQ_ERR_NOMEM;
	}
	پیغام->payloadlen = payloadlen;
	پیغام->payload = payload;
	پیغام->qos = qos;
	پیغام->retain = retain;
	پیغام->properties = properties;

	DL_APPEND(db.plugin_msgs, پیغام);

	loop_update_اگلا_event(1);
	واپس MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحیح mosquitto_broker_publish_copy(
		ثابت حرف *clientid,
		ثابت حرف *topic,
		صحیح payloadlen,
		ثابت خالی *payload,
		صحیح qos,
		bool retain,
		mosquitto_property *properties)
{
	خالی *payload_برآمد;
	صحیح rc;

	اگر(topic == تہی
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == تہی)
			|| qos < 0 || qos > 2){

		واپس MOSQ_ERR_INVAL;
	}

	payload_برآمد = mosquitto_calloc(1, (size_t)(payloadlen+1));
	اگر(payload_برآمد == تہی){
		واپس MOSQ_ERR_NOMEM;
	}
	حافظہ_نقل(payload_برآمد, payload, (size_t)payloadlen);

	rc = mosquitto_broker_publish(
			clientid,
			topic,
			payloadlen,
			payload_برآمد,
			qos,
			retain,
			properties);

	اگر(rc){
		mosquitto_FREE(payload_برآمد);
	}
	واپس rc;
}


BROKER_EXPORT صحیح mosquitto_سیٹ_username(ساخت mosquitto *client, ثابت حرف *username)
{
	حرف *u_dup;
	حرف *old;

	اگر(!client){
		واپس MOSQ_ERR_INVAL;
	}

	اگر(username){
		اگر(mosquitto_validate_utf8(username, (صحیح)لڑی_لمبائی(username))){
			واپس MOSQ_ERR_MALFORMED_UTF8;
		}
		u_dup = mosquitto_strdup(username);
		اگر(!u_dup){
			واپس MOSQ_ERR_NOMEM;
		}
	}ورنہ{
		u_dup = تہی;
	}

	old = client->username;
	client->username = u_dup;

	mosquitto_FREE(old);
	واپس MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحیح mosquitto_سیٹ_clientid(ساخت mosquitto *client, ثابت حرف *clientid)
{
	ساخت mosquitto *ملا_client;
	حرف *id_dup;
	bool اندر_by_id;
	صحیح clientid_len;

	اگر(!client || !clientid){
		واپس MOSQ_ERR_INVAL;
	}

	اندر_by_id = client->اندر_by_id;
	/* If in_by_id is true, then this client has already authenticated and
	 * completed the connection flow. This means it *cannot* take over an
	 * existing session, and we must remove/add it to the by_id hash table.
	 *
	 * If in_by_id is false, then this client is currently going through
	 * authentication and so it is safe to change the client id to any value
	 * because it will be checked after authentication.
	 */

	اگر(اندر_by_id){
		HASH_ڈھونڈو(hh_id, db.contexts_by_id, clientid, لڑی_لمبائی(clientid), ملا_client);
		اگر(ملا_client){
			واپس MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (صحیح)لڑی_لمبائی(clientid);
	اگر(mosquitto_validate_utf8(clientid, clientid_len)){
		واپس MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	اگر(!id_dup){
		واپس MOSQ_ERR_NOMEM;
	}

	اگر(اندر_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	اگر(اندر_by_id){
		context_جوڑو_مقصد_by_id(client);
	}

	واپس MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
ساکن خالی check_subscription_acls(ساخت mosquitto *context)
{
	صحیح rc;
	uint8_t reason;

	برائے(صحیح i=0; i<context->subs_استعداد; i++){
		اگر(context->subs[i] == تہی){
			جاری;
		}
		rc = mosquitto_acl_check(context,
				context->subs[i]->topic_filter,
				0,
				تہی,
				0, /* FIXME */
				false,
				تہی,
				MOSQ_ACL_SUBSCRIBE);

		اگر(rc != MOSQ_ERR_SUCCESS){
			sub__remove(context, context->subs[i]->topic_filter, &reason);
		}
	}
}


ساکن خالی disconnect_client(ساخت mosquitto *context, bool with_will)
{
	اگر(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, تہی);
	}
	اگر(with_will == false){
		mosquitto_سیٹ_state(context, mosq_cs_disconnecting);
	}
	اگر(context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوری){
		check_subscription_acls(context);
	}
	do_disconnect(context, MOSQ_ERR_ADMINISTRATIVE_ACTION);
}


BROKER_EXPORT صحیح mosquitto_kick_client_by_clientid(ثابت حرف *clientid, bool with_will)
{
	ساخت mosquitto *ctxt, *ctxt_tmp;

	اگر(clientid == تہی){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			disconnect_client(ctxt, with_will);
		}
		واپس MOSQ_ERR_SUCCESS;
	}ورنہ{
		HASH_ڈھونڈو(hh_id, db.contexts_by_id, clientid, لڑی_لمبائی(clientid), ctxt);
		اگر(ctxt){
			disconnect_client(ctxt, with_will);
			واپس MOSQ_ERR_SUCCESS;
		}ورنہ{
			واپس MOSQ_ERR_NOT_ملا;
		}
	}
}


BROKER_EXPORT صحیح mosquitto_kick_client_by_username(ثابت حرف *username, bool with_will)
{
	ساخت mosquitto *ctxt, *ctxt_tmp;

	اگر(username == تہی){
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			اگر(ctxt->username == تہی){
				disconnect_client(ctxt, with_will);
			}
		}
	}ورنہ{
		HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
			اگر(ctxt->username != تہی && !strcmp(ctxt->username, username)){
				disconnect_client(ctxt, with_will);
			}
		}
	}
	واپس MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحیح mosquitto_apply_on_all_clients(صحیح (*FUNC_client_functor)(ثابت ساخت mosquitto *, خالی *), خالی *functor_context)
{
	صحیح rc = MOSQ_ERR_SUCCESS;
	ساخت mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		rc = (*FUNC_client_functor)(ctxt, functor_context);
		اگر(rc != MOSQ_ERR_SUCCESS){
			توڑو;
		}
	}

	واپس rc;
}


BROKER_EXPORT صحیح mosquitto_persist_client_جوڑو(ساخت mosquitto_client *client)
{
	ساخت mosquitto *context;
	صحیح rc;

	اگر(client == تہی){
		واپس MOSQ_ERR_INVAL;
	}
	اگر(client->clientid == تہی){
		rc = MOSQ_ERR_INVAL;
		جاؤ error;
	}

	context = تہی;
	HASH_ڈھونڈو(hh_id, db.contexts_by_id, client->clientid, لڑی_لمبائی(client->clientid), context);
	اگر(context){
		rc = MOSQ_ERR_INVAL;
		جاؤ error;
	}

	context = context__init();
	اگر(!context){
		rc = MOSQ_ERR_NOMEM;
		جاؤ error;
	}

	context->id = client->clientid;
	client->clientid = تہی;
	context->username = client->username;
	client->username = تہی;
	context->auth_طریقہ = client->auth_طریقہ;
	client->auth_طریقہ = تہی;

	context->clean_آغاز = false;
	context->will_delay_وقت = client->will_delay_وقت;
	context->session_expiry_وقت = client->session_expiry_وقت;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->زیادہ_سے_زیادہ_packet_سائز = client->max_packet_سائز;
	context->retain_دستیاب = client->retain_دستیاب;
	context->is_persisted = true;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	اگر(db.config->فی_listener_settings && client->listener_پورٹ > 0){
		برائے(صحیح i=0; i < db.config->listener_گنتی; i++){
			اگر(db.config->listeners[i].پورٹ == client->listener_پورٹ){
				context->listener = &db.config->listeners[i];
				توڑو;
			}
		}
	}

	context_جوڑو_مقصد_by_id(context);
	session_expiry_جوڑو_from_persistence(context, context->session_expiry_وقت);

	واپس MOSQ_ERR_SUCCESS;
error:
	محفوظ_FREE(client->clientid);
	محفوظ_FREE(client->username);
	محفوظ_FREE(client->auth_طریقہ);
	واپس rc;
}


BROKER_EXPORT صحیح mosquitto_persist_client_update(ساخت mosquitto_client *client)
{
	ساخت mosquitto *context;
	صحیح rc;

	اگر(client == تہی){
		واپس MOSQ_ERR_INVAL;
	}
	اگر(client->clientid == تہی){
		rc = MOSQ_ERR_INVAL;
		جاؤ error;
	}

	context = تہی;
	HASH_ڈھونڈو(hh_id, db.contexts_by_id, client->clientid, لڑی_لمبائی(client->clientid), context);
	اگر(context == تہی){
		rc = MOSQ_ERR_NOT_ملا;
		جاؤ error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = تہی;

	context->clean_آغاز = false;
	context->will_delay_وقت = client->will_delay_وقت;
	context->session_expiry_وقت = client->session_expiry_وقت;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->زیادہ_سے_زیادہ_packet_سائز = client->max_packet_سائز;
	context->retain_دستیاب = client->retain_دستیاب;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	اگر(db.config->فی_listener_settings && client->listener_پورٹ > 0){
		برائے(صحیح i=0; i < db.config->listener_گنتی; i++){
			اگر(db.config->listeners[i].پورٹ == client->listener_پورٹ){
				context->listener = &db.config->listeners[i];
				توڑو;
			}
		}
	}

	واپس MOSQ_ERR_SUCCESS;
error:
	محفوظ_FREE(client->username);
	واپس rc;
}


BROKER_EXPORT صحیح mosquitto_persist_client_delete(ثابت حرف *clientid)
{
	ساخت mosquitto *context;

	اگر(clientid == تہی){
		واپس MOSQ_ERR_INVAL;
	}

	context = تہی;
	HASH_ڈھونڈو(hh_id, db.contexts_by_id, clientid, لڑی_لمبائی(clientid), context);
	اگر(context == تہی){
		واپس MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_آغاز = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوری;
	context->is_persisted = false;
	mosquitto_سیٹ_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	واپس MOSQ_ERR_SUCCESS;
}


ساکن ساخت mosquitto_بنیاد_پیغام *ڈھونڈو_store_پیغام(uint64_t store_id)
{
	ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام;

	HASH_ڈھونڈو(hh, db.پیغام_store, &store_id, حجم(store_id), بنیاد_پیغام);
	واپس بنیاد_پیغام;
}


BROKER_EXPORT صحیح mosquitto_persist_client_پیغام_جوڑو(ساخت mosquitto_client_پیغام_2 *client_پیغام)
{
	ساخت mosquitto *context;
	ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام;

	اگر(client_پیغام == تہی || client_پیغام->clientid == تہی){
		واپس MOSQ_ERR_INVAL;
	}

	HASH_ڈھونڈو(hh_id, db.contexts_by_id, client_پیغام->clientid, لڑی_لمبائی(client_پیغام->clientid), context);
	اگر(context == تہی){
		واپس MOSQ_ERR_NOT_ملا;
	}
	بنیاد_پیغام = ڈھونڈو_store_پیغام(client_پیغام->store_id);
	اگر(بنیاد_پیغام == تہی){
		واپس MOSQ_ERR_NOT_ملا;
	}

	اگر(client_پیغام->direction == mosq_md_برآمد){
		اگر(client_پیغام->qos > 0){
			context->آخری_mid = client_پیغام->mid;
		}
		واپس db_پیغام_داخل_کرو_outgoing(context, client_پیغام->cmsg_id, client_پیغام->mid,
				client_پیغام->qos, client_پیغام->retain,
				بنیاد_پیغام, client_پیغام->subscription_identifier, false, false);
	}ورنہ اگر(client_پیغام->direction == mosq_md_اندر){
		واپس db_پیغام_داخل_کرو_incoming(context, client_پیغام->cmsg_id, بنیاد_پیغام, false);
	}ورنہ{
		واپس MOSQ_ERR_INVAL;
	}
	واپس MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحیح mosquitto_persist_client_پیغام_delete(ساخت mosquitto_client_پیغام_2 *client_پیغام)
{
	ساخت mosquitto *context;

	اگر(client_پیغام == تہی || client_پیغام->clientid == تہی){
		واپس MOSQ_ERR_INVAL;
	}

	HASH_ڈھونڈو(hh_id, db.contexts_by_id, client_پیغام->clientid, لڑی_لمبائی(client_پیغام->clientid), context);
	اگر(context == تہی){
		واپس MOSQ_ERR_NOT_ملا;
	}


	صحیح rc = MOSQ_ERR_INVAL;
	اگر(client_پیغام->direction == mosq_md_برآمد){
		rc = db_پیغام_delete_outgoing(context, client_پیغام->mid, mosq_ms_any, client_پیغام->qos);
	}ورنہ اگر(client_پیغام->direction == mosq_md_اندر){
		rc = db_پیغام_remove_incoming(context, client_پیغام->mid);
	}
	واپس rc;
}


BROKER_EXPORT صحیح mosquitto_persist_client_پیغام_update(ساخت mosquitto_client_پیغام_2 *client_پیغام)
{
	ساخت mosquitto *context;

	اگر(client_پیغام == تہی || client_پیغام->clientid == تہی){
		واپس MOSQ_ERR_INVAL;
	}

	HASH_ڈھونڈو(hh_id, db.contexts_by_id, client_پیغام->clientid, لڑی_لمبائی(client_پیغام->clientid), context);
	اگر(context == تہی){
		واپس MOSQ_ERR_NOT_ملا;
	}

	اگر(client_پیغام->direction == mosq_md_برآمد){
		db_پیغام_update_outgoing(context, client_پیغام->mid, client_پیغام->state, client_پیغام->qos, false);
	}ورنہ اگر(client_پیغام->direction == mosq_md_اندر){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}ورنہ{
		واپس MOSQ_ERR_INVAL;
	}
	واپس MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحیح mosquitto_persist_client_پیغام_clear(ساخت mosquitto_client_پیغام_2 *client_پیغام)
{
	ساخت mosquitto *context;

	اگر(client_پیغام == تہی || client_پیغام->clientid == تہی){
		واپس MOSQ_ERR_INVAL;
	}

	HASH_ڈھونڈو(hh_id, db.contexts_by_id, client_پیغام->clientid, لڑی_لمبائی(client_پیغام->clientid), context);
	اگر(context == تہی){
		واپس MOSQ_ERR_NOT_ملا;
	}

	اگر(client_پیغام->direction == mosq_bmd_اندر || client_پیغام->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	اگر(client_پیغام->direction == mosq_bmd_برآمد || client_پیغام->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	واپس MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحیح mosquitto_subscription_جوڑو(ثابت ساخت mosquitto_subscription *sub)
{
	ساخت mosquitto *context;

	اگر(sub == تہی || sub->clientid == تہی || sub->topic_filter == تہی || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		واپس MOSQ_ERR_INVAL;
	}

	HASH_ڈھونڈو(hh_id, db.contexts_by_id, sub->clientid, لڑی_لمبائی(sub->clientid), context);

	اگر(context){
		واپس sub_جوڑو(context, sub);
	}ورنہ{
		واپس MOSQ_ERR_NOT_ملا;
	}
}


BROKER_EXPORT صحیح mosquitto_subscription_delete(ثابت حرف *clientid, ثابت حرف *topic)
{
	ساخت mosquitto *context;
	uint8_t reason;

	اگر(clientid == تہی || topic == تہی || clientid[0] == '\0' || topic[0] == '\0'){
		واپس MOSQ_ERR_INVAL;
	}

	HASH_ڈھونڈو(hh_id, db.contexts_by_id, clientid, لڑی_لمبائی(clientid), context);

	اگر(context){
		واپس sub__remove(context, topic, &reason);
	}ورنہ{
		واپس MOSQ_ERR_NOT_ملا;
	}
}


BROKER_EXPORT صحیح mosquitto_persist_بنیاد_پیغام_جوڑو(ساخت mosquitto_بنیاد_پیغام_2 *پیغام_جوڑو)
{
	ساخت mosquitto context;
	ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام;
	صحیح rc;

	memset(&context, 0, حجم(context));

	اگر(پیغام_جوڑو->payloadlen > MQTT_MAX_PAYLOAD){
		واپس MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (حرف *)پیغام_جوڑو->ماخذ_id;
	context.username = (حرف *)پیغام_جوڑو->ماخذ_username;

	بنیاد_پیغام = mosquitto_calloc(1, حجم(ساخت mosquitto_بنیاد_پیغام));
	اگر(بنیاد_پیغام == تہی){
		جاؤ error;
	}
	بنیاد_پیغام->ڈیٹا.store_id = پیغام_جوڑو->store_id;
	بنیاد_پیغام->ڈیٹا.expiry_وقت = پیغام_جوڑو->expiry_وقت;
	بنیاد_پیغام->ڈیٹا.payloadlen = پیغام_جوڑو->payloadlen;
	بنیاد_پیغام->ڈیٹا.ماخذ_mid = پیغام_جوڑو->ماخذ_mid;
	بنیاد_پیغام->ڈیٹا.qos = پیغام_جوڑو->qos;
	بنیاد_پیغام->ڈیٹا.retain = پیغام_جوڑو->retain;

	بنیاد_پیغام->ڈیٹا.payload = پیغام_جوڑو->payload;
	پیغام_جوڑو->payload = تہی;
	بنیاد_پیغام->ڈیٹا.topic = پیغام_جوڑو->topic;
	پیغام_جوڑو->topic = تہی;
	بنیاد_پیغام->ڈیٹا.properties = پیغام_جوڑو->properties;
	پیغام_جوڑو->properties = تہی;

	اگر(پیغام_جوڑو->ماخذ_پورٹ){
		برائے(صحیح i=0; i<db.config->listener_گنتی; i++){
			اگر(db.config->listeners[i].پورٹ == پیغام_جوڑو->ماخذ_پورٹ){
				بنیاد_پیغام->ماخذ_listener = &db.config->listeners[i];
				توڑو;
			}
		}
	}

	بنیاد_پیغام->stored = true;
	rc = db_پیغام_store(&context, بنیاد_پیغام, تہی, mosq_mo_broker);
	واپس rc;

error:
	mosquitto_property_free_all(&پیغام_جوڑو->properties);
	mosquitto_free(پیغام_جوڑو->topic);
	mosquitto_free(پیغام_جوڑو->payload);
	mosquitto_free(بنیاد_پیغام);

	واپس MOSQ_ERR_NOMEM;
}


BROKER_EXPORT صحیح mosquitto_persist_بنیاد_پیغام_delete(uint64_t store_id)
{
	ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام;

	بنیاد_پیغام = ڈھونڈو_store_پیغام(store_id);
	اگر(بنیاد_پیغام && بنیاد_پیغام->ref_گنتی == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_پیغام_store_remove(بنیاد_پیغام, false);
	}

	واپس MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT خالی mosquitto_complete_basic_auth(ثابت حرف *clientid, صحیح نتیجہ)
{
	ساخت mosquitto *context;

	اگر(clientid == تہی){
		واپس;
	}

	HASH_ڈھونڈو(hh_id, db.contexts_by_id_delayed_auth, clientid, لڑی_لمبائی(clientid), context);
	اگر(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		اگر(نتیجہ == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, تہی, 0);
		}ورنہ{
			اگر(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, تہی);
			}ورنہ{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, تہی);
			}
			context->clean_آغاز = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوری;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT صحیح mosquitto_broker_گرہ_id_سیٹ(uint16_t id)
{
	اگر(id > 1023){
		واپس MOSQ_ERR_INVAL;
	}ورنہ{
		db.گرہ_id = id;
		db.گرہ_id_shifted = ((uint64_t)id) << 54;
		واپس MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_persistence_location(خالی)
{
	واپس db.config->persistence_location;
}


BROKER_EXPORT صحیح mosquitto_client_will_سیٹ(ثابت حرف *clientid, ثابت حرف *topic, صحیح payloadlen, ثابت خالی *payload, صحیح qos, bool retain, mosquitto_property *properties)
{
	ساخت mosquitto *mosq = mosquitto_client(clientid);
	اگر(!mosq){
		واپس MOSQ_ERR_NOT_ملا;
	}
	اگر(properties && mosq->protocol != mosq_p_mqtt5){
		اگر(net__is_connected(mosq)){
			واپس MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	واپس will_سیٹ(mosq, topic, payloadlen, payload, qos, retain, properties);
}
