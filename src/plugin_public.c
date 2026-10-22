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

#که_تعریف_شوی WITH_TLS
#  include <openssl/ssl.h>
#د_که_پای


BROKER_EXPORT صحيح mosquitto_plugin_سېټ_info(mosquitto_plugin_id_t *identifier,
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


BROKER_EXPORT ثابت حرف *mosquitto_client_پته(ثابت بنية mosquitto *client)
{
	إذا(client){
		أرجع client->پته;
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
	len = مزي_اوږدوالی(clientid);
	إذا(len == 0){
		أرجع عدم;
	}

	HASH_پیدا_کړه(hh_id, db.contexts_by_id, clientid, مزي_اوږدوالی(clientid), context);

	أرجع context;
}


BROKER_EXPORT صحيح mosquitto_client_پورټ(ثابت بنية mosquitto *client)
{
	إذا(client && client->listener){
		أرجع client->listener->پورټ;
	}وإلا{
		أرجع 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(ثابت بنية mosquitto *client)
{
	إذا(client){
		أرجع client->clean_پیل;
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


BROKER_EXPORT بینښه mosquitto_client_id_hashv(ثابت بنية mosquitto *client)
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
#که_تعریف_شوی WITH_TLS
	إذا(client && client->ssl){
		أرجع SSL_get_peer_certificate(client->ssl);
	}وإلا{
		أرجع عدم;
	}
#وإلا
	ناکارول_شوی(client);

	أرجع عدم;
#د_که_پای
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
	ناکارول_شوی(client);
#د_که_پای
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


BROKER_EXPORT صحيح mosquitto_client_sub_شمېر(ثابت بنية mosquitto *client)
{
	إذا(client){
		أرجع client->subs_شمېر;
	}وإلا{
		أرجع 0;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_client_username(ثابت بنية mosquitto *client)
{
	إذا(client){
#که_تعریف_شوی WITH_BRIDGE
		إذا(client->bridge){
			أرجع client->bridge->local_username;
		}وإلا
#د_که_پای
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
	بنية mosquitto_پیغام_v5 *پیغام;

	إذا(topic == عدم
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == عدم)
			|| qos < 0 || qos > 2){

		أرجع MOSQ_ERR_INVAL;
	}

	پیغام = mosquitto_malloc(حجم(بنية mosquitto_پیغام_v5));
	إذا(پیغام == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	پیغام->بل = عدم;
	پیغام->prev = عدم;
	إذا(clientid){
		پیغام->clientid = mosquitto_strdup(clientid);
		إذا(پیغام->clientid == عدم){
			mosquitto_FREE(پیغام);
			أرجع MOSQ_ERR_NOMEM;
		}
	}وإلا{
		پیغام->clientid = عدم;
	}
	پیغام->topic = mosquitto_strdup(topic);
	إذا(پیغام->topic == عدم){
		mosquitto_FREE(پیغام->clientid);
		mosquitto_FREE(پیغام);
		أرجع MOSQ_ERR_NOMEM;
	}
	پیغام->payloadlen = payloadlen;
	پیغام->payload = payload;
	پیغام->qos = qos;
	پیغام->retain = retain;
	پیغام->properties = properties;

	DL_APPEND(db.plugin_msgs, پیغام);

	loop_update_بل_event(1);
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
	فراغ *payload_بهر;
	صحيح rc;

	إذا(topic == عدم
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == عدم)
			|| qos < 0 || qos > 2){

		أرجع MOSQ_ERR_INVAL;
	}

	payload_بهر = mosquitto_calloc(1, (size_t)(payloadlen+1));
	إذا(payload_بهر == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}
	حافظه_نقل(payload_بهر, payload, (size_t)payloadlen);

	rc = mosquitto_broker_publish(
			clientid,
			topic,
			payloadlen,
			payload_بهر,
			qos,
			retain,
			properties);

	إذا(rc){
		mosquitto_FREE(payload_بهر);
	}
	أرجع rc;
}


BROKER_EXPORT صحيح mosquitto_سېټ_username(بنية mosquitto *client, ثابت حرف *username)
{
	حرف *u_dup;
	حرف *old;

	إذا(!client){
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(username){
		إذا(mosquitto_validate_utf8(username, (صحيح)مزي_اوږدوالی(username))){
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


BROKER_EXPORT صحيح mosquitto_سېټ_clientid(بنية mosquitto *client, ثابت حرف *clientid)
{
	بنية mosquitto *وموندل_client;
	حرف *id_dup;
	bool دننه_by_id;
	صحيح clientid_len;

	إذا(!client || !clientid){
		أرجع MOSQ_ERR_INVAL;
	}

	دننه_by_id = client->دننه_by_id;
	/* If in_by_id is true, then this client has already authenticated and
	 * completed the connection flow. This means it *cannot* take over an
	 * existing session, and we must remove/add it to the by_id hash table.
	 *
	 * If in_by_id is false, then this client is currently going through
	 * authentication and so it is safe to change the client id to any value
	 * because it will be checked after authentication.
	 */

	إذا(دننه_by_id){
		HASH_پیدا_کړه(hh_id, db.contexts_by_id, clientid, مزي_اوږدوالی(clientid), وموندل_client);
		إذا(وموندل_client){
			أرجع MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (صحيح)مزي_اوږدوالی(clientid);
	إذا(mosquitto_validate_utf8(clientid, clientid_len)){
		أرجع MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	إذا(!id_dup){
		أرجع MOSQ_ERR_NOMEM;
	}

	إذا(دننه_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	إذا(دننه_by_id){
		context_جمع_کړه_منزل_by_id(client);
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
		mosquitto_سېټ_state(context, mosq_cs_disconnecting);
	}
	إذا(context->session_expiry_interval != MQTT_SESSION_EXPIRY_سمدستي){
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
		HASH_پیدا_کړه(hh_id, db.contexts_by_id, clientid, مزي_اوږدوالی(clientid), ctxt);
		إذا(ctxt){
			disconnect_client(ctxt, with_will);
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			أرجع MOSQ_ERR_NOT_وموندل;
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


BROKER_EXPORT صحيح mosquitto_persist_client_جمع_کړه(بنية mosquitto_client *client)
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
	HASH_پیدا_کړه(hh_id, db.contexts_by_id, client->clientid, مزي_اوږدوالی(client->clientid), context);
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
	context->auth_لاره = client->auth_لاره;
	client->auth_لاره = عدم;

	context->clean_پیل = false;
	context->will_delay_وخت = client->will_delay_وخت;
	context->session_expiry_وخت = client->session_expiry_وخت;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->اعظمي_packet_اندازه = client->max_packet_اندازه;
	context->retain_شته = client->retain_شته;
	context->is_persisted = true;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	إذا(db.config->فی_listener_settings && client->listener_پورټ > 0){
		لكل(صحيح i=0; i < db.config->listener_شمېر; i++){
			إذا(db.config->listeners[i].پورټ == client->listener_پورټ){
				context->listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	context_جمع_کړه_منزل_by_id(context);
	session_expiry_جمع_کړه_from_persistence(context, context->session_expiry_وخت);

	أرجع MOSQ_ERR_SUCCESS;
error:
	خوندي_FREE(client->clientid);
	خوندي_FREE(client->username);
	خوندي_FREE(client->auth_لاره);
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
	HASH_پیدا_کړه(hh_id, db.contexts_by_id, client->clientid, مزي_اوږدوالی(client->clientid), context);
	إذا(context == عدم){
		rc = MOSQ_ERR_NOT_وموندل;
		اذهب error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = عدم;

	context->clean_پیل = false;
	context->will_delay_وخت = client->will_delay_وخت;
	context->session_expiry_وخت = client->session_expiry_وخت;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->اعظمي_packet_اندازه = client->max_packet_اندازه;
	context->retain_شته = client->retain_شته;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	إذا(db.config->فی_listener_settings && client->listener_پورټ > 0){
		لكل(صحيح i=0; i < db.config->listener_شمېر; i++){
			إذا(db.config->listeners[i].پورټ == client->listener_پورټ){
				context->listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
error:
	خوندي_FREE(client->username);
	أرجع rc;
}


BROKER_EXPORT صحيح mosquitto_persist_client_delete(ثابت حرف *clientid)
{
	بنية mosquitto *context;

	إذا(clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	context = عدم;
	HASH_پیدا_کړه(hh_id, db.contexts_by_id, clientid, مزي_اوږدوالی(clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_پیل = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_سمدستي;
	context->is_persisted = false;
	mosquitto_سېټ_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن بنية mosquitto_بنسټ_پیغام *پیدا_کړه_store_پیغام(uint64_t store_id)
{
	بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام;

	HASH_پیدا_کړه(hh, db.پیغام_store, &store_id, حجم(store_id), بنسټ_پیغام);
	أرجع بنسټ_پیغام;
}


BROKER_EXPORT صحيح mosquitto_persist_client_پیغام_جمع_کړه(بنية mosquitto_client_پیغام_2 *client_پیغام)
{
	بنية mosquitto *context;
	بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام;

	إذا(client_پیغام == عدم || client_پیغام->clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_پیدا_کړه(hh_id, db.contexts_by_id, client_پیغام->clientid, مزي_اوږدوالی(client_پیغام->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_وموندل;
	}
	بنسټ_پیغام = پیدا_کړه_store_پیغام(client_پیغام->store_id);
	إذا(بنسټ_پیغام == عدم){
		أرجع MOSQ_ERR_NOT_وموندل;
	}

	إذا(client_پیغام->direction == mosq_md_بهر){
		إذا(client_پیغام->qos > 0){
			context->وروستی_mid = client_پیغام->mid;
		}
		أرجع db_پیغام_ورننباسه_outgoing(context, client_پیغام->cmsg_id, client_پیغام->mid,
				client_پیغام->qos, client_پیغام->retain,
				بنسټ_پیغام, client_پیغام->subscription_identifier, false, false);
	}وإلا إذا(client_پیغام->direction == mosq_md_دننه){
		أرجع db_پیغام_ورننباسه_incoming(context, client_پیغام->cmsg_id, بنسټ_پیغام, false);
	}وإلا{
		أرجع MOSQ_ERR_INVAL;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_persist_client_پیغام_delete(بنية mosquitto_client_پیغام_2 *client_پیغام)
{
	بنية mosquitto *context;

	إذا(client_پیغام == عدم || client_پیغام->clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_پیدا_کړه(hh_id, db.contexts_by_id, client_پیغام->clientid, مزي_اوږدوالی(client_پیغام->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_وموندل;
	}


	صحيح rc = MOSQ_ERR_INVAL;
	إذا(client_پیغام->direction == mosq_md_بهر){
		rc = db_پیغام_delete_outgoing(context, client_پیغام->mid, mosq_ms_any, client_پیغام->qos);
	}وإلا إذا(client_پیغام->direction == mosq_md_دننه){
		rc = db_پیغام_remove_incoming(context, client_پیغام->mid);
	}
	أرجع rc;
}


BROKER_EXPORT صحيح mosquitto_persist_client_پیغام_update(بنية mosquitto_client_پیغام_2 *client_پیغام)
{
	بنية mosquitto *context;

	إذا(client_پیغام == عدم || client_پیغام->clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_پیدا_کړه(hh_id, db.contexts_by_id, client_پیغام->clientid, مزي_اوږدوالی(client_پیغام->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_وموندل;
	}

	إذا(client_پیغام->direction == mosq_md_بهر){
		db_پیغام_update_outgoing(context, client_پیغام->mid, client_پیغام->state, client_پیغام->qos, false);
	}وإلا إذا(client_پیغام->direction == mosq_md_دننه){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}وإلا{
		أرجع MOSQ_ERR_INVAL;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_persist_client_پیغام_clear(بنية mosquitto_client_پیغام_2 *client_پیغام)
{
	بنية mosquitto *context;

	إذا(client_پیغام == عدم || client_پیغام->clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_پیدا_کړه(hh_id, db.contexts_by_id, client_پیغام->clientid, مزي_اوږدوالی(client_پیغام->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_وموندل;
	}

	إذا(client_پیغام->direction == mosq_bmd_دننه || client_پیغام->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	إذا(client_پیغام->direction == mosq_bmd_بهر || client_پیغام->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_subscription_جمع_کړه(ثابت بنية mosquitto_subscription *sub)
{
	بنية mosquitto *context;

	إذا(sub == عدم || sub->clientid == عدم || sub->topic_filter == عدم || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_پیدا_کړه(hh_id, db.contexts_by_id, sub->clientid, مزي_اوږدوالی(sub->clientid), context);

	إذا(context){
		أرجع sub_جمع_کړه(context, sub);
	}وإلا{
		أرجع MOSQ_ERR_NOT_وموندل;
	}
}


BROKER_EXPORT صحيح mosquitto_subscription_delete(ثابت حرف *clientid, ثابت حرف *topic)
{
	بنية mosquitto *context;
	uint8_t reason;

	إذا(clientid == عدم || topic == عدم || clientid[0] == '\0' || topic[0] == '\0'){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_پیدا_کړه(hh_id, db.contexts_by_id, clientid, مزي_اوږدوالی(clientid), context);

	إذا(context){
		أرجع sub__remove(context, topic, &reason);
	}وإلا{
		أرجع MOSQ_ERR_NOT_وموندل;
	}
}


BROKER_EXPORT صحيح mosquitto_persist_بنسټ_پیغام_جمع_کړه(بنية mosquitto_بنسټ_پیغام_2 *پیغام_جمع_کړه)
{
	بنية mosquitto context;
	بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام;
	صحيح rc;

	memset(&context, 0, حجم(context));

	إذا(پیغام_جمع_کړه->payloadlen > MQTT_MAX_PAYLOAD){
		أرجع MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (حرف *)پیغام_جمع_کړه->سرچینه_id;
	context.username = (حرف *)پیغام_جمع_کړه->سرچینه_username;

	بنسټ_پیغام = mosquitto_calloc(1, حجم(بنية mosquitto_بنسټ_پیغام));
	إذا(بنسټ_پیغام == عدم){
		اذهب error;
	}
	بنسټ_پیغام->معلومات.store_id = پیغام_جمع_کړه->store_id;
	بنسټ_پیغام->معلومات.expiry_وخت = پیغام_جمع_کړه->expiry_وخت;
	بنسټ_پیغام->معلومات.payloadlen = پیغام_جمع_کړه->payloadlen;
	بنسټ_پیغام->معلومات.سرچینه_mid = پیغام_جمع_کړه->سرچینه_mid;
	بنسټ_پیغام->معلومات.qos = پیغام_جمع_کړه->qos;
	بنسټ_پیغام->معلومات.retain = پیغام_جمع_کړه->retain;

	بنسټ_پیغام->معلومات.payload = پیغام_جمع_کړه->payload;
	پیغام_جمع_کړه->payload = عدم;
	بنسټ_پیغام->معلومات.topic = پیغام_جمع_کړه->topic;
	پیغام_جمع_کړه->topic = عدم;
	بنسټ_پیغام->معلومات.properties = پیغام_جمع_کړه->properties;
	پیغام_جمع_کړه->properties = عدم;

	إذا(پیغام_جمع_کړه->سرچینه_پورټ){
		لكل(صحيح i=0; i<db.config->listener_شمېر; i++){
			إذا(db.config->listeners[i].پورټ == پیغام_جمع_کړه->سرچینه_پورټ){
				بنسټ_پیغام->سرچینه_listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	بنسټ_پیغام->stored = true;
	rc = db_پیغام_store(&context, بنسټ_پیغام, عدم, mosq_mo_broker);
	أرجع rc;

error:
	mosquitto_property_free_all(&پیغام_جمع_کړه->properties);
	mosquitto_free(پیغام_جمع_کړه->topic);
	mosquitto_free(پیغام_جمع_کړه->payload);
	mosquitto_free(بنسټ_پیغام);

	أرجع MOSQ_ERR_NOMEM;
}


BROKER_EXPORT صحيح mosquitto_persist_بنسټ_پیغام_delete(uint64_t store_id)
{
	بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام;

	بنسټ_پیغام = پیدا_کړه_store_پیغام(store_id);
	إذا(بنسټ_پیغام && بنسټ_پیغام->ref_شمېر == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_پیغام_store_remove(بنسټ_پیغام, false);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT فراغ mosquitto_complete_basic_auth(ثابت حرف *clientid, صحيح پایله)
{
	بنية mosquitto *context;

	إذا(clientid == عدم){
		أرجع;
	}

	HASH_پیدا_کړه(hh_id, db.contexts_by_id_delayed_auth, clientid, مزي_اوږدوالی(clientid), context);
	إذا(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		إذا(پایله == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, عدم, 0);
		}وإلا{
			إذا(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, عدم);
			}وإلا{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, عدم);
			}
			context->clean_پیل = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_سمدستي;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT صحيح mosquitto_broker_غوټه_id_سېټ(uint16_t id)
{
	إذا(id > 1023){
		أرجع MOSQ_ERR_INVAL;
	}وإلا{
		db.غوټه_id = id;
		db.غوټه_id_shifted = ((uint64_t)id) << 54;
		أرجع MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_persistence_location(فراغ)
{
	أرجع db.config->persistence_location;
}


BROKER_EXPORT صحيح mosquitto_client_will_سېټ(ثابت حرف *clientid, ثابت حرف *topic, صحيح payloadlen, ثابت فراغ *payload, صحيح qos, bool retain, mosquitto_property *properties)
{
	بنية mosquitto *mosq = mosquitto_client(clientid);
	إذا(!mosq){
		أرجع MOSQ_ERR_NOT_وموندل;
	}
	إذا(properties && mosq->protocol != mosq_p_mqtt5){
		إذا(net__is_connected(mosq)){
			أرجع MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	أرجع will_سېټ(mosq, topic, payloadlen, payload, qos, retain, properties);
}
