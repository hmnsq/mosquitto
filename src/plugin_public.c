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

#اگر_تعریف_شده WITH_TLS
#  include <openssl/ssl.h>
#پایان_اگر


BROKER_EXPORT صحيح mosquitto_plugin_مجموعه_info(mosquitto_plugin_id_t *identifier,
		ثابت حرف *plugin_name,
		ثابت حرف *plugin_نسخه)
{
	إذا(identifier == عدم || plugin_name == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	إذا(plugin_نسخه){
		identifier->plugin_نسخه = mosquitto_strdup(plugin_نسخه);
	}وإلا{
		identifier->plugin_نسخه = عدم;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ثابت حرف *mosquitto_client_آدرس(ثابت بنية mosquitto *client)
{
	إذا(client){
		أرجع client->آدرس;
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
	len = طول_رشته(clientid);
	إذا(len == 0){
		أرجع عدم;
	}

	HASH_پیدا_کن(hh_id, db.contexts_by_id, clientid, طول_رشته(clientid), context);

	أرجع context;
}


BROKER_EXPORT صحيح mosquitto_client_پورت(ثابت بنية mosquitto *client)
{
	إذا(client && client->listener){
		أرجع client->listener->پورت;
	}وإلا{
		أرجع 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(ثابت بنية mosquitto *client)
{
	إذا(client){
		أرجع client->clean_شروع;
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


BROKER_EXPORT بیعلامت mosquitto_client_id_hashv(ثابت بنية mosquitto *client)
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
#اگر_تعریف_شده WITH_TLS
	إذا(client && client->ssl){
		أرجع SSL_get_peer_certificate(client->ssl);
	}وإلا{
		أرجع عدم;
	}
#وإلا
	استفادهنشده(client);

	أرجع عدم;
#پایان_اگر
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
	استفادهنشده(client);
#پایان_اگر
	{
		أرجع mp_mqtt;
	}
}


BROKER_EXPORT صحيح mosquitto_client_protocol_نسخه(ثابت بنية mosquitto *client)
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


BROKER_EXPORT صحيح mosquitto_client_sub_شمار(ثابت بنية mosquitto *client)
{
	إذا(client){
		أرجع client->subs_شمار;
	}وإلا{
		أرجع 0;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_client_username(ثابت بنية mosquitto *client)
{
	إذا(client){
#اگر_تعریف_شده WITH_BRIDGE
		إذا(client->bridge){
			أرجع client->bridge->local_username;
		}وإلا
#پایان_اگر
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
	بنية mosquitto_پیام_v5 *پیام;

	إذا(topic == عدم
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == عدم)
			|| qos < 0 || qos > 2){

		أرجع MOSQ_ERR_INVAL;
	}

	پیام = mosquitto_malloc(حجم(بنية mosquitto_پیام_v5));
	إذا(پیام == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	پیام->بعدی = عدم;
	پیام->prev = عدم;
	إذا(clientid){
		پیام->clientid = mosquitto_strdup(clientid);
		إذا(پیام->clientid == عدم){
			mosquitto_FREE(پیام);
			أرجع MOSQ_ERR_NOMEM;
		}
	}وإلا{
		پیام->clientid = عدم;
	}
	پیام->topic = mosquitto_strdup(topic);
	إذا(پیام->topic == عدم){
		mosquitto_FREE(پیام->clientid);
		mosquitto_FREE(پیام);
		أرجع MOSQ_ERR_NOMEM;
	}
	پیام->payloadlen = payloadlen;
	پیام->payload = payload;
	پیام->qos = qos;
	پیام->retain = retain;
	پیام->properties = properties;

	DL_APPEND(db.plugin_msgs, پیام);

	loop_update_بعدی_event(1);
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
	فراغ *payload_بیرون;
	صحيح rc;

	إذا(topic == عدم
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == عدم)
			|| qos < 0 || qos > 2){

		أرجع MOSQ_ERR_INVAL;
	}

	payload_بیرون = mosquitto_calloc(1, (size_t)(payloadlen+1));
	إذا(payload_بیرون == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}
	کپی_حافظه(payload_بیرون, payload, (size_t)payloadlen);

	rc = mosquitto_broker_publish(
			clientid,
			topic,
			payloadlen,
			payload_بیرون,
			qos,
			retain,
			properties);

	إذا(rc){
		mosquitto_FREE(payload_بیرون);
	}
	أرجع rc;
}


BROKER_EXPORT صحيح mosquitto_مجموعه_username(بنية mosquitto *client, ثابت حرف *username)
{
	حرف *u_dup;
	حرف *old;

	إذا(!client){
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(username){
		إذا(mosquitto_validate_utf8(username, (صحيح)طول_رشته(username))){
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


BROKER_EXPORT صحيح mosquitto_مجموعه_clientid(بنية mosquitto *client, ثابت حرف *clientid)
{
	بنية mosquitto *یافت_client;
	حرف *id_dup;
	bool درون_by_id;
	صحيح clientid_len;

	إذا(!client || !clientid){
		أرجع MOSQ_ERR_INVAL;
	}

	درون_by_id = client->درون_by_id;
	/* If in_by_id is true, then this client has already authenticated and
	 * completed the connection flow. This means it *cannot* take over an
	 * existing session, and we must remove/add it to the by_id hash table.
	 *
	 * If in_by_id is false, then this client is currently going through
	 * authentication and so it is safe to change the client id to any value
	 * because it will be checked after authentication.
	 */

	إذا(درون_by_id){
		HASH_پیدا_کن(hh_id, db.contexts_by_id, clientid, طول_رشته(clientid), یافت_client);
		إذا(یافت_client){
			أرجع MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (صحيح)طول_رشته(clientid);
	إذا(mosquitto_validate_utf8(clientid, clientid_len)){
		أرجع MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	إذا(!id_dup){
		أرجع MOSQ_ERR_NOMEM;
	}

	إذا(درون_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	إذا(درون_by_id){
		context_جمع_کن_مقصد_by_id(client);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
ساكن فراغ check_subscription_acls(بنية mosquitto *context)
{
	صحيح rc;
	uint8_t reason;

	لكل(صحيح i=0; i<context->subs_ظرفیت; i++){
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
		mosquitto_مجموعه_state(context, mosq_cs_disconnecting);
	}
	إذا(context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوری){
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
		HASH_پیدا_کن(hh_id, db.contexts_by_id, clientid, طول_رشته(clientid), ctxt);
		إذا(ctxt){
			disconnect_client(ctxt, with_will);
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			أرجع MOSQ_ERR_NOT_یافت;
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


BROKER_EXPORT صحيح mosquitto_persist_client_جمع_کن(بنية mosquitto_client *client)
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
	HASH_پیدا_کن(hh_id, db.contexts_by_id, client->clientid, طول_رشته(client->clientid), context);
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
	context->auth_روش = client->auth_روش;
	client->auth_روش = عدم;

	context->clean_شروع = false;
	context->will_delay_زمان = client->will_delay_زمان;
	context->session_expiry_زمان = client->session_expiry_زمان;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->بیشینه_packet_اندازه = client->max_packet_اندازه;
	context->retain_موجود = client->retain_موجود;
	context->is_persisted = true;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	إذا(db.config->در_هر_listener_settings && client->listener_پورت > 0){
		لكل(صحيح i=0; i < db.config->listener_شمار; i++){
			إذا(db.config->listeners[i].پورت == client->listener_پورت){
				context->listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	context_جمع_کن_مقصد_by_id(context);
	session_expiry_جمع_کن_from_persistence(context, context->session_expiry_زمان);

	أرجع MOSQ_ERR_SUCCESS;
error:
	امن_FREE(client->clientid);
	امن_FREE(client->username);
	امن_FREE(client->auth_روش);
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
	HASH_پیدا_کن(hh_id, db.contexts_by_id, client->clientid, طول_رشته(client->clientid), context);
	إذا(context == عدم){
		rc = MOSQ_ERR_NOT_یافت;
		اذهب error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = عدم;

	context->clean_شروع = false;
	context->will_delay_زمان = client->will_delay_زمان;
	context->session_expiry_زمان = client->session_expiry_زمان;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->بیشینه_packet_اندازه = client->max_packet_اندازه;
	context->retain_موجود = client->retain_موجود;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	إذا(db.config->در_هر_listener_settings && client->listener_پورت > 0){
		لكل(صحيح i=0; i < db.config->listener_شمار; i++){
			إذا(db.config->listeners[i].پورت == client->listener_پورت){
				context->listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
error:
	امن_FREE(client->username);
	أرجع rc;
}


BROKER_EXPORT صحيح mosquitto_persist_client_delete(ثابت حرف *clientid)
{
	بنية mosquitto *context;

	إذا(clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	context = عدم;
	HASH_پیدا_کن(hh_id, db.contexts_by_id, clientid, طول_رشته(clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_شروع = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوری;
	context->is_persisted = false;
	mosquitto_مجموعه_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن بنية mosquitto_پایه_پیام *پیدا_کن_store_پیام(uint64_t store_id)
{
	بنية mosquitto_پایه_پیام *پایه_پیام;

	HASH_پیدا_کن(hh, db.پیام_store, &store_id, حجم(store_id), پایه_پیام);
	أرجع پایه_پیام;
}


BROKER_EXPORT صحيح mosquitto_persist_client_پیام_جمع_کن(بنية mosquitto_client_پیام_2 *client_پیام)
{
	بنية mosquitto *context;
	بنية mosquitto_پایه_پیام *پایه_پیام;

	إذا(client_پیام == عدم || client_پیام->clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_پیدا_کن(hh_id, db.contexts_by_id, client_پیام->clientid, طول_رشته(client_پیام->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_یافت;
	}
	پایه_پیام = پیدا_کن_store_پیام(client_پیام->store_id);
	إذا(پایه_پیام == عدم){
		أرجع MOSQ_ERR_NOT_یافت;
	}

	إذا(client_پیام->direction == mosq_md_بیرون){
		إذا(client_پیام->qos > 0){
			context->آخرین_mid = client_پیام->mid;
		}
		أرجع db_پیام_درج_outgoing(context, client_پیام->cmsg_id, client_پیام->mid,
				client_پیام->qos, client_پیام->retain,
				پایه_پیام, client_پیام->subscription_identifier, false, false);
	}وإلا إذا(client_پیام->direction == mosq_md_درون){
		أرجع db_پیام_درج_incoming(context, client_پیام->cmsg_id, پایه_پیام, false);
	}وإلا{
		أرجع MOSQ_ERR_INVAL;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_persist_client_پیام_delete(بنية mosquitto_client_پیام_2 *client_پیام)
{
	بنية mosquitto *context;

	إذا(client_پیام == عدم || client_پیام->clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_پیدا_کن(hh_id, db.contexts_by_id, client_پیام->clientid, طول_رشته(client_پیام->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_یافت;
	}


	صحيح rc = MOSQ_ERR_INVAL;
	إذا(client_پیام->direction == mosq_md_بیرون){
		rc = db_پیام_delete_outgoing(context, client_پیام->mid, mosq_ms_any, client_پیام->qos);
	}وإلا إذا(client_پیام->direction == mosq_md_درون){
		rc = db_پیام_remove_incoming(context, client_پیام->mid);
	}
	أرجع rc;
}


BROKER_EXPORT صحيح mosquitto_persist_client_پیام_update(بنية mosquitto_client_پیام_2 *client_پیام)
{
	بنية mosquitto *context;

	إذا(client_پیام == عدم || client_پیام->clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_پیدا_کن(hh_id, db.contexts_by_id, client_پیام->clientid, طول_رشته(client_پیام->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_یافت;
	}

	إذا(client_پیام->direction == mosq_md_بیرون){
		db_پیام_update_outgoing(context, client_پیام->mid, client_پیام->state, client_پیام->qos, false);
	}وإلا إذا(client_پیام->direction == mosq_md_درون){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}وإلا{
		أرجع MOSQ_ERR_INVAL;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_persist_client_پیام_clear(بنية mosquitto_client_پیام_2 *client_پیام)
{
	بنية mosquitto *context;

	إذا(client_پیام == عدم || client_پیام->clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_پیدا_کن(hh_id, db.contexts_by_id, client_پیام->clientid, طول_رشته(client_پیام->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_یافت;
	}

	إذا(client_پیام->direction == mosq_bmd_درون || client_پیام->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	إذا(client_پیام->direction == mosq_bmd_بیرون || client_پیام->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_subscription_جمع_کن(ثابت بنية mosquitto_subscription *sub)
{
	بنية mosquitto *context;

	إذا(sub == عدم || sub->clientid == عدم || sub->topic_filter == عدم || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_پیدا_کن(hh_id, db.contexts_by_id, sub->clientid, طول_رشته(sub->clientid), context);

	إذا(context){
		أرجع sub_جمع_کن(context, sub);
	}وإلا{
		أرجع MOSQ_ERR_NOT_یافت;
	}
}


BROKER_EXPORT صحيح mosquitto_subscription_delete(ثابت حرف *clientid, ثابت حرف *topic)
{
	بنية mosquitto *context;
	uint8_t reason;

	إذا(clientid == عدم || topic == عدم || clientid[0] == '\0' || topic[0] == '\0'){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_پیدا_کن(hh_id, db.contexts_by_id, clientid, طول_رشته(clientid), context);

	إذا(context){
		أرجع sub__remove(context, topic, &reason);
	}وإلا{
		أرجع MOSQ_ERR_NOT_یافت;
	}
}


BROKER_EXPORT صحيح mosquitto_persist_پایه_پیام_جمع_کن(بنية mosquitto_پایه_پیام_2 *پیام_جمع_کن)
{
	بنية mosquitto context;
	بنية mosquitto_پایه_پیام *پایه_پیام;
	صحيح rc;

	memset(&context, 0, حجم(context));

	إذا(پیام_جمع_کن->payloadlen > MQTT_MAX_PAYLOAD){
		أرجع MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (حرف *)پیام_جمع_کن->منبع_id;
	context.username = (حرف *)پیام_جمع_کن->منبع_username;

	پایه_پیام = mosquitto_calloc(1, حجم(بنية mosquitto_پایه_پیام));
	إذا(پایه_پیام == عدم){
		اذهب error;
	}
	پایه_پیام->داده.store_id = پیام_جمع_کن->store_id;
	پایه_پیام->داده.expiry_زمان = پیام_جمع_کن->expiry_زمان;
	پایه_پیام->داده.payloadlen = پیام_جمع_کن->payloadlen;
	پایه_پیام->داده.منبع_mid = پیام_جمع_کن->منبع_mid;
	پایه_پیام->داده.qos = پیام_جمع_کن->qos;
	پایه_پیام->داده.retain = پیام_جمع_کن->retain;

	پایه_پیام->داده.payload = پیام_جمع_کن->payload;
	پیام_جمع_کن->payload = عدم;
	پایه_پیام->داده.topic = پیام_جمع_کن->topic;
	پیام_جمع_کن->topic = عدم;
	پایه_پیام->داده.properties = پیام_جمع_کن->properties;
	پیام_جمع_کن->properties = عدم;

	إذا(پیام_جمع_کن->منبع_پورت){
		لكل(صحيح i=0; i<db.config->listener_شمار; i++){
			إذا(db.config->listeners[i].پورت == پیام_جمع_کن->منبع_پورت){
				پایه_پیام->منبع_listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	پایه_پیام->stored = true;
	rc = db_پیام_store(&context, پایه_پیام, عدم, mosq_mo_broker);
	أرجع rc;

error:
	mosquitto_property_free_all(&پیام_جمع_کن->properties);
	mosquitto_free(پیام_جمع_کن->topic);
	mosquitto_free(پیام_جمع_کن->payload);
	mosquitto_free(پایه_پیام);

	أرجع MOSQ_ERR_NOMEM;
}


BROKER_EXPORT صحيح mosquitto_persist_پایه_پیام_delete(uint64_t store_id)
{
	بنية mosquitto_پایه_پیام *پایه_پیام;

	پایه_پیام = پیدا_کن_store_پیام(store_id);
	إذا(پایه_پیام && پایه_پیام->ref_شمار == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_پیام_store_remove(پایه_پیام, false);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT فراغ mosquitto_complete_basic_auth(ثابت حرف *clientid, صحيح نتیجه)
{
	بنية mosquitto *context;

	إذا(clientid == عدم){
		أرجع;
	}

	HASH_پیدا_کن(hh_id, db.contexts_by_id_delayed_auth, clientid, طول_رشته(clientid), context);
	إذا(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		إذا(نتیجه == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, عدم, 0);
		}وإلا{
			إذا(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, عدم);
			}وإلا{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, عدم);
			}
			context->clean_شروع = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوری;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT صحيح mosquitto_broker_گره_id_مجموعه(uint16_t id)
{
	إذا(id > 1023){
		أرجع MOSQ_ERR_INVAL;
	}وإلا{
		db.گره_id = id;
		db.گره_id_shifted = ((uint64_t)id) << 54;
		أرجع MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_persistence_location(فراغ)
{
	أرجع db.config->persistence_location;
}


BROKER_EXPORT صحيح mosquitto_client_will_مجموعه(ثابت حرف *clientid, ثابت حرف *topic, صحيح payloadlen, ثابت فراغ *payload, صحيح qos, bool retain, mosquitto_property *properties)
{
	بنية mosquitto *mosq = mosquitto_client(clientid);
	إذا(!mosq){
		أرجع MOSQ_ERR_NOT_یافت;
	}
	إذا(properties && mosq->protocol != mosq_p_mqtt5){
		إذا(net__is_connected(mosq)){
			أرجع MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	أرجع will_مجموعه(mosq, topic, payloadlen, payload, qos, retain, properties);
}
