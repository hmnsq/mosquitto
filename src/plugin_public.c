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

#جے_تعریف_شدہ WITH_TLS
#  include <openssl/ssl.h>
#جے_انت


BROKER_EXPORT صحيح mosquitto_plugin_سیٹ_info(mosquitto_plugin_id_t *identifier,
		ثابت حرف *plugin_name,
		ثابت حرف *plugin_نسخہ)
{
	إذا(identifier == عدم || plugin_name == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	إذا(plugin_نسخہ){
		identifier->plugin_نسخہ = mosquitto_strdup(plugin_نسخہ);
	}وإلا{
		identifier->plugin_نسخہ = عدم;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ثابت حرف *mosquitto_client_پتہ(ثابت بنية mosquitto *client)
{
	إذا(client){
		أرجع client->پتہ;
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
	len = لڑی_لمبائی(clientid);
	إذا(len == 0){
		أرجع عدم;
	}

	HASH_لبھو(hh_id, db.contexts_by_id, clientid, لڑی_لمبائی(clientid), context);

	أرجع context;
}


BROKER_EXPORT صحيح mosquitto_client_پورٹ(ثابت بنية mosquitto *client)
{
	إذا(client && client->listener){
		أرجع client->listener->پورٹ;
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


BROKER_EXPORT بنانشانی mosquitto_client_id_hashv(ثابت بنية mosquitto *client)
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
#جے_تعریف_شدہ WITH_TLS
	إذا(client && client->ssl){
		أرجع SSL_get_peer_certificate(client->ssl);
	}وإلا{
		أرجع عدم;
	}
#وإلا
	بن_ورتیا(client);

	أرجع عدم;
#جے_انت
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
	بن_ورتیا(client);
#جے_انت
	{
		أرجع mp_mqtt;
	}
}


BROKER_EXPORT صحيح mosquitto_client_protocol_نسخہ(ثابت بنية mosquitto *client)
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


BROKER_EXPORT صحيح mosquitto_client_sub_گنتی(ثابت بنية mosquitto *client)
{
	إذا(client){
		أرجع client->subs_گنتی;
	}وإلا{
		أرجع 0;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_client_username(ثابت بنية mosquitto *client)
{
	إذا(client){
#جے_تعریف_شدہ WITH_BRIDGE
		إذا(client->bridge){
			أرجع client->bridge->local_username;
		}وإلا
#جے_انت
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
	بنية mosquitto_سنیہا_v5 *پیغام;

	إذا(topic == عدم
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == عدم)
			|| qos < 0 || qos > 2){

		أرجع MOSQ_ERR_INVAL;
	}

	پیغام = mosquitto_malloc(حجم(بنية mosquitto_سنیہا_v5));
	إذا(پیغام == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	پیغام->اگلا = عدم;
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

	loop_update_اگلا_event(1);
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
	فراغ *payload_باہرلا;
	صحيح rc;

	إذا(topic == عدم
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == عدم)
			|| qos < 0 || qos > 2){

		أرجع MOSQ_ERR_INVAL;
	}

	payload_باہرلا = mosquitto_calloc(1, (size_t)(payloadlen+1));
	إذا(payload_باہرلا == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}
	حافظہ_نقل(payload_باہرلا, payload, (size_t)payloadlen);

	rc = mosquitto_broker_publish(
			clientid,
			topic,
			payloadlen,
			payload_باہرلا,
			qos,
			retain,
			properties);

	إذا(rc){
		mosquitto_FREE(payload_باہرلا);
	}
	أرجع rc;
}


BROKER_EXPORT صحيح mosquitto_سیٹ_username(بنية mosquitto *client, ثابت حرف *username)
{
	حرف *u_dup;
	حرف *old;

	إذا(!client){
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(username){
		إذا(mosquitto_validate_utf8(username, (صحيح)لڑی_لمبائی(username))){
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


BROKER_EXPORT صحيح mosquitto_سیٹ_clientid(بنية mosquitto *client, ثابت حرف *clientid)
{
	بنية mosquitto *لبھیا_client;
	حرف *id_dup;
	bool وچ_by_id;
	صحيح clientid_len;

	إذا(!client || !clientid){
		أرجع MOSQ_ERR_INVAL;
	}

	وچ_by_id = client->وچ_by_id;
	/* If in_by_id is true, then this client has already authenticated and
	 * completed the connection flow. This means it *cannot* take over an
	 * existing session, and we must remove/add it to the by_id hash table.
	 *
	 * If in_by_id is false, then this client is currently going through
	 * authentication and so it is safe to change the client id to any value
	 * because it will be checked after authentication.
	 */

	إذا(وچ_by_id){
		HASH_لبھو(hh_id, db.contexts_by_id, clientid, لڑی_لمبائی(clientid), لبھیا_client);
		إذا(لبھیا_client){
			أرجع MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (صحيح)لڑی_لمبائی(clientid);
	إذا(mosquitto_validate_utf8(clientid, clientid_len)){
		أرجع MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	إذا(!id_dup){
		أرجع MOSQ_ERR_NOMEM;
	}

	إذا(وچ_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	إذا(وچ_by_id){
		context_جوڑو_منزل_by_id(client);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
ساكن فراغ check_subscription_acls(بنية mosquitto *context)
{
	صحيح rc;
	uint8_t reason;

	لكل(صحيح i=0; i<context->subs_سمرتھا; i++){
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
		mosquitto_سیٹ_state(context, mosq_cs_disconnecting);
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
		HASH_لبھو(hh_id, db.contexts_by_id, clientid, لڑی_لمبائی(clientid), ctxt);
		إذا(ctxt){
			disconnect_client(ctxt, with_will);
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			أرجع MOSQ_ERR_NOT_لبھیا;
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


BROKER_EXPORT صحيح mosquitto_persist_client_جوڑو(بنية mosquitto_client *client)
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
	HASH_لبھو(hh_id, db.contexts_by_id, client->clientid, لڑی_لمبائی(client->clientid), context);
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
	context->auth_طریقہ = client->auth_طریقہ;
	client->auth_طریقہ = عدم;

	context->clean_شروع = false;
	context->will_delay_ویلا = client->will_delay_ویلا;
	context->session_expiry_ویلا = client->session_expiry_ویلا;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->ودھ_توں_ودھ_packet_ناپ = client->max_packet_ناپ;
	context->retain_دستیاب = client->retain_دستیاب;
	context->is_persisted = true;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	إذا(db.config->فی_listener_settings && client->listener_پورٹ > 0){
		لكل(صحيح i=0; i < db.config->listener_گنتی; i++){
			إذا(db.config->listeners[i].پورٹ == client->listener_پورٹ){
				context->listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	context_جوڑو_منزل_by_id(context);
	session_expiry_جوڑو_from_persistence(context, context->session_expiry_ویلا);

	أرجع MOSQ_ERR_SUCCESS;
error:
	محفوظ_FREE(client->clientid);
	محفوظ_FREE(client->username);
	محفوظ_FREE(client->auth_طریقہ);
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
	HASH_لبھو(hh_id, db.contexts_by_id, client->clientid, لڑی_لمبائی(client->clientid), context);
	إذا(context == عدم){
		rc = MOSQ_ERR_NOT_لبھیا;
		اذهب error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = عدم;

	context->clean_شروع = false;
	context->will_delay_ویلا = client->will_delay_ویلا;
	context->session_expiry_ویلا = client->session_expiry_ویلا;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->ودھ_توں_ودھ_packet_ناپ = client->max_packet_ناپ;
	context->retain_دستیاب = client->retain_دستیاب;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	إذا(db.config->فی_listener_settings && client->listener_پورٹ > 0){
		لكل(صحيح i=0; i < db.config->listener_گنتی; i++){
			إذا(db.config->listeners[i].پورٹ == client->listener_پورٹ){
				context->listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
error:
	محفوظ_FREE(client->username);
	أرجع rc;
}


BROKER_EXPORT صحيح mosquitto_persist_client_delete(ثابت حرف *clientid)
{
	بنية mosquitto *context;

	إذا(clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	context = عدم;
	HASH_لبھو(hh_id, db.contexts_by_id, clientid, لڑی_لمبائی(clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_شروع = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوری;
	context->is_persisted = false;
	mosquitto_سیٹ_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن بنية mosquitto_ادھار_پیغام *لبھو_store_پیغام(uint64_t store_id)
{
	بنية mosquitto_ادھار_پیغام *ادھار_پیغام;

	HASH_لبھو(hh, db.پیغام_store, &store_id, حجم(store_id), ادھار_پیغام);
	أرجع ادھار_پیغام;
}


BROKER_EXPORT صحيح mosquitto_persist_client_پیغام_جوڑو(بنية mosquitto_client_پیغام_2 *client_پیغام)
{
	بنية mosquitto *context;
	بنية mosquitto_ادھار_پیغام *ادھار_پیغام;

	إذا(client_پیغام == عدم || client_پیغام->clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_لبھو(hh_id, db.contexts_by_id, client_پیغام->clientid, لڑی_لمبائی(client_پیغام->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_لبھیا;
	}
	ادھار_پیغام = لبھو_store_پیغام(client_پیغام->store_id);
	إذا(ادھار_پیغام == عدم){
		أرجع MOSQ_ERR_NOT_لبھیا;
	}

	إذا(client_پیغام->direction == mosq_md_باہرلا){
		إذا(client_پیغام->qos > 0){
			context->آخری_mid = client_پیغام->mid;
		}
		أرجع db_سنیہا_داخل_کرو_outgoing(context, client_پیغام->cmsg_id, client_پیغام->mid,
				client_پیغام->qos, client_پیغام->retain,
				ادھار_پیغام, client_پیغام->subscription_identifier, false, false);
	}وإلا إذا(client_پیغام->direction == mosq_md_وچ){
		أرجع db_سنیہا_داخل_کرو_incoming(context, client_پیغام->cmsg_id, ادھار_پیغام, false);
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

	HASH_لبھو(hh_id, db.contexts_by_id, client_پیغام->clientid, لڑی_لمبائی(client_پیغام->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_لبھیا;
	}


	صحيح rc = MOSQ_ERR_INVAL;
	إذا(client_پیغام->direction == mosq_md_باہرلا){
		rc = db_سنیہا_delete_outgoing(context, client_پیغام->mid, mosq_ms_any, client_پیغام->qos);
	}وإلا إذا(client_پیغام->direction == mosq_md_وچ){
		rc = db_سنیہا_remove_incoming(context, client_پیغام->mid);
	}
	أرجع rc;
}


BROKER_EXPORT صحيح mosquitto_persist_client_پیغام_update(بنية mosquitto_client_پیغام_2 *client_پیغام)
{
	بنية mosquitto *context;

	إذا(client_پیغام == عدم || client_پیغام->clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_لبھو(hh_id, db.contexts_by_id, client_پیغام->clientid, لڑی_لمبائی(client_پیغام->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_لبھیا;
	}

	إذا(client_پیغام->direction == mosq_md_باہرلا){
		db_سنیہا_update_outgoing(context, client_پیغام->mid, client_پیغام->state, client_پیغام->qos, false);
	}وإلا إذا(client_پیغام->direction == mosq_md_وچ){
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

	HASH_لبھو(hh_id, db.contexts_by_id, client_پیغام->clientid, لڑی_لمبائی(client_پیغام->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_لبھیا;
	}

	إذا(client_پیغام->direction == mosq_bmd_وچ || client_پیغام->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	إذا(client_پیغام->direction == mosq_bmd_باہرلا || client_پیغام->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_subscription_جوڑو(ثابت بنية mosquitto_subscription *sub)
{
	بنية mosquitto *context;

	إذا(sub == عدم || sub->clientid == عدم || sub->topic_filter == عدم || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_لبھو(hh_id, db.contexts_by_id, sub->clientid, لڑی_لمبائی(sub->clientid), context);

	إذا(context){
		أرجع sub_جوڑو(context, sub);
	}وإلا{
		أرجع MOSQ_ERR_NOT_لبھیا;
	}
}


BROKER_EXPORT صحيح mosquitto_subscription_delete(ثابت حرف *clientid, ثابت حرف *topic)
{
	بنية mosquitto *context;
	uint8_t reason;

	إذا(clientid == عدم || topic == عدم || clientid[0] == '\0' || topic[0] == '\0'){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_لبھو(hh_id, db.contexts_by_id, clientid, لڑی_لمبائی(clientid), context);

	إذا(context){
		أرجع sub__remove(context, topic, &reason);
	}وإلا{
		أرجع MOSQ_ERR_NOT_لبھیا;
	}
}


BROKER_EXPORT صحيح mosquitto_persist_ادھار_پیغام_جوڑو(بنية mosquitto_ادھار_پیغام_2 *پیغام_جوڑو)
{
	بنية mosquitto context;
	بنية mosquitto_ادھار_پیغام *ادھار_پیغام;
	صحيح rc;

	memset(&context, 0, حجم(context));

	إذا(پیغام_جوڑو->payloadlen > MQTT_MAX_PAYLOAD){
		أرجع MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (حرف *)پیغام_جوڑو->سروت_id;
	context.username = (حرف *)پیغام_جوڑو->سروت_username;

	ادھار_پیغام = mosquitto_calloc(1, حجم(بنية mosquitto_ادھار_پیغام));
	إذا(ادھار_پیغام == عدم){
		اذهب error;
	}
	ادھار_پیغام->انکڑے.store_id = پیغام_جوڑو->store_id;
	ادھار_پیغام->انکڑے.expiry_ویلا = پیغام_جوڑو->expiry_ویلا;
	ادھار_پیغام->انکڑے.payloadlen = پیغام_جوڑو->payloadlen;
	ادھار_پیغام->انکڑے.سروت_mid = پیغام_جوڑو->سروت_mid;
	ادھار_پیغام->انکڑے.qos = پیغام_جوڑو->qos;
	ادھار_پیغام->انکڑے.retain = پیغام_جوڑو->retain;

	ادھار_پیغام->انکڑے.payload = پیغام_جوڑو->payload;
	پیغام_جوڑو->payload = عدم;
	ادھار_پیغام->انکڑے.topic = پیغام_جوڑو->topic;
	پیغام_جوڑو->topic = عدم;
	ادھار_پیغام->انکڑے.properties = پیغام_جوڑو->properties;
	پیغام_جوڑو->properties = عدم;

	إذا(پیغام_جوڑو->سروت_پورٹ){
		لكل(صحيح i=0; i<db.config->listener_گنتی; i++){
			إذا(db.config->listeners[i].پورٹ == پیغام_جوڑو->سروت_پورٹ){
				ادھار_پیغام->سروت_listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	ادھار_پیغام->stored = true;
	rc = db_سنیہا_store(&context, ادھار_پیغام, عدم, mosq_mo_broker);
	أرجع rc;

error:
	mosquitto_property_free_all(&پیغام_جوڑو->properties);
	mosquitto_free(پیغام_جوڑو->topic);
	mosquitto_free(پیغام_جوڑو->payload);
	mosquitto_free(ادھار_پیغام);

	أرجع MOSQ_ERR_NOMEM;
}


BROKER_EXPORT صحيح mosquitto_persist_ادھار_پیغام_delete(uint64_t store_id)
{
	بنية mosquitto_ادھار_پیغام *ادھار_پیغام;

	ادھار_پیغام = لبھو_store_پیغام(store_id);
	إذا(ادھار_پیغام && ادھار_پیغام->ref_گنتی == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_پیغام_store_remove(ادھار_پیغام, false);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT فراغ mosquitto_complete_basic_auth(ثابت حرف *clientid, صحيح نتیجہ)
{
	بنية mosquitto *context;

	إذا(clientid == عدم){
		أرجع;
	}

	HASH_لبھو(hh_id, db.contexts_by_id_delayed_auth, clientid, لڑی_لمبائی(clientid), context);
	إذا(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		إذا(نتیجہ == MOSQ_ERR_SUCCESS){
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


BROKER_EXPORT صحيح mosquitto_broker_گرہ_id_سیٹ(uint16_t id)
{
	إذا(id > 1023){
		أرجع MOSQ_ERR_INVAL;
	}وإلا{
		db.گرہ_id = id;
		db.گرہ_id_shifted = ((uint64_t)id) << 54;
		أرجع MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_persistence_location(فراغ)
{
	أرجع db.config->persistence_location;
}


BROKER_EXPORT صحيح mosquitto_client_will_سیٹ(ثابت حرف *clientid, ثابت حرف *topic, صحيح payloadlen, ثابت فراغ *payload, صحيح qos, bool retain, mosquitto_property *properties)
{
	بنية mosquitto *mosq = mosquitto_client(clientid);
	إذا(!mosq){
		أرجع MOSQ_ERR_NOT_لبھیا;
	}
	إذا(properties && mosq->protocol != mosq_p_mqtt5){
		إذا(net__is_connected(mosq)){
			أرجع MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	أرجع will_سیٹ(mosq, topic, payloadlen, payload, qos, retain, properties);
}
