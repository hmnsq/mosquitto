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

#جيڪڏهن_وصف_ٿيل WITH_TLS
#  include <openssl/ssl.h>
#جيڪڏهن_پورو


BROKER_EXPORT صحيح mosquitto_plugin_سيٽ_info(mosquitto_plugin_id_t *identifier,
		ثابت حرف *plugin_name,
		ثابت حرف *plugin_نسخو)
{
	إذا(identifier == عدم || plugin_name == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	identifier->plugin_name = mosquitto_strdup(plugin_name);
	إذا(plugin_نسخو){
		identifier->plugin_نسخو = mosquitto_strdup(plugin_نسخو);
	}وإلا{
		identifier->plugin_نسخو = عدم;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ثابت حرف *mosquitto_client_پتو(ثابت بنية mosquitto *client)
{
	إذا(client){
		أرجع client->پتو;
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
	len = لڙي_ڊيگهه(clientid);
	إذا(len == 0){
		أرجع عدم;
	}

	HASH_ڳوليو(hh_id, db.contexts_by_id, clientid, لڙي_ڊيگهه(clientid), context);

	أرجع context;
}


BROKER_EXPORT صحيح mosquitto_client_پورٽ(ثابت بنية mosquitto *client)
{
	إذا(client && client->listener){
		أرجع client->listener->پورٽ;
	}وإلا{
		أرجع 0;
	}
}


BROKER_EXPORT bool mosquitto_client_clean_session(ثابت بنية mosquitto *client)
{
	إذا(client){
		أرجع client->clean_شروعات;
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


BROKER_EXPORT بنانشان mosquitto_client_id_hashv(ثابت بنية mosquitto *client)
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
#جيڪڏهن_وصف_ٿيل WITH_TLS
	إذا(client && client->ssl){
		أرجع SSL_get_peer_certificate(client->ssl);
	}وإلا{
		أرجع عدم;
	}
#وإلا
	اڻ_استعمال_ٿيل(client);

	أرجع عدم;
#جيڪڏهن_پورو
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
	اڻ_استعمال_ٿيل(client);
#جيڪڏهن_پورو
	{
		أرجع mp_mqtt;
	}
}


BROKER_EXPORT صحيح mosquitto_client_protocol_نسخو(ثابت بنية mosquitto *client)
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


BROKER_EXPORT صحيح mosquitto_client_sub_ڳڻپ(ثابت بنية mosquitto *client)
{
	إذا(client){
		أرجع client->subs_ڳڻپ;
	}وإلا{
		أرجع 0;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_client_username(ثابت بنية mosquitto *client)
{
	إذا(client){
#جيڪڏهن_وصف_ٿيل WITH_BRIDGE
		إذا(client->bridge){
			أرجع client->bridge->local_username;
		}وإلا
#جيڪڏهن_پورو
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
	بنية mosquitto_پيغام_v5 *نياپو;

	إذا(topic == عدم
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == عدم)
			|| qos < 0 || qos > 2){

		أرجع MOSQ_ERR_INVAL;
	}

	نياپو = mosquitto_malloc(حجم(بنية mosquitto_پيغام_v5));
	إذا(نياپو == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	نياپو->ايندڙ = عدم;
	نياپو->prev = عدم;
	إذا(clientid){
		نياپو->clientid = mosquitto_strdup(clientid);
		إذا(نياپو->clientid == عدم){
			mosquitto_FREE(نياپو);
			أرجع MOSQ_ERR_NOMEM;
		}
	}وإلا{
		نياپو->clientid = عدم;
	}
	نياپو->topic = mosquitto_strdup(topic);
	إذا(نياپو->topic == عدم){
		mosquitto_FREE(نياپو->clientid);
		mosquitto_FREE(نياپو);
		أرجع MOSQ_ERR_NOMEM;
	}
	نياپو->payloadlen = payloadlen;
	نياپو->payload = payload;
	نياپو->qos = qos;
	نياپو->retain = retain;
	نياپو->properties = properties;

	DL_APPEND(db.plugin_msgs, نياپو);

	loop_update_ايندڙ_event(1);
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
	فراغ *payload_ٻاهريون;
	صحيح rc;

	إذا(topic == عدم
			|| payloadlen < 0
			|| (payloadlen > 0 && payload == عدم)
			|| qos < 0 || qos > 2){

		أرجع MOSQ_ERR_INVAL;
	}

	payload_ٻاهريون = mosquitto_calloc(1, (size_t)(payloadlen+1));
	إذا(payload_ٻاهريون == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}
	يادگيري_نقل(payload_ٻاهريون, payload, (size_t)payloadlen);

	rc = mosquitto_broker_publish(
			clientid,
			topic,
			payloadlen,
			payload_ٻاهريون,
			qos,
			retain,
			properties);

	إذا(rc){
		mosquitto_FREE(payload_ٻاهريون);
	}
	أرجع rc;
}


BROKER_EXPORT صحيح mosquitto_سيٽ_username(بنية mosquitto *client, ثابت حرف *username)
{
	حرف *u_dup;
	حرف *old;

	إذا(!client){
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(username){
		إذا(mosquitto_validate_utf8(username, (صحيح)لڙي_ڊيگهه(username))){
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


BROKER_EXPORT صحيح mosquitto_سيٽ_clientid(بنية mosquitto *client, ثابت حرف *clientid)
{
	بنية mosquitto *مليو_client;
	حرف *id_dup;
	bool اندريون_by_id;
	صحيح clientid_len;

	إذا(!client || !clientid){
		أرجع MOSQ_ERR_INVAL;
	}

	اندريون_by_id = client->اندريون_by_id;
	/* If in_by_id is true, then this client has already authenticated and
	 * completed the connection flow. This means it *cannot* take over an
	 * existing session, and we must remove/add it to the by_id hash table.
	 *
	 * If in_by_id is false, then this client is currently going through
	 * authentication and so it is safe to change the client id to any value
	 * because it will be checked after authentication.
	 */

	إذا(اندريون_by_id){
		HASH_ڳوليو(hh_id, db.contexts_by_id, clientid, لڙي_ڊيگهه(clientid), مليو_client);
		إذا(مليو_client){
			أرجع MOSQ_ERR_ALREADY_EXISTS;
		}
	}

	clientid_len = (صحيح)لڙي_ڊيگهه(clientid);
	إذا(mosquitto_validate_utf8(clientid, clientid_len)){
		أرجع MOSQ_ERR_INVAL;
	}

	id_dup = mosquitto_strdup(clientid);
	إذا(!id_dup){
		أرجع MOSQ_ERR_NOMEM;
	}

	إذا(اندريون_by_id){
		context__remove_from_by_id(client);
	}
	mosquitto_free(client->id);
	client->id = id_dup;
	إذا(اندريون_by_id){
		context_جوڙيو_منزل_by_id(client);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


/* Check to see whether durable clients still have rights to their subscriptions. */
ساكن فراغ check_subscription_acls(بنية mosquitto *context)
{
	صحيح rc;
	uint8_t reason;

	لكل(صحيح i=0; i<context->subs_صلاحيت; i++){
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
		mosquitto_سيٽ_state(context, mosq_cs_disconnecting);
	}
	إذا(context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوري){
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
		HASH_ڳوليو(hh_id, db.contexts_by_id, clientid, لڙي_ڊيگهه(clientid), ctxt);
		إذا(ctxt){
			disconnect_client(ctxt, with_will);
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			أرجع MOSQ_ERR_NOT_مليو;
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


BROKER_EXPORT صحيح mosquitto_persist_client_جوڙيو(بنية mosquitto_client *client)
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
	HASH_ڳوليو(hh_id, db.contexts_by_id, client->clientid, لڙي_ڊيگهه(client->clientid), context);
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
	context->auth_طريقو = client->auth_طريقو;
	client->auth_طريقو = عدم;

	context->clean_شروعات = false;
	context->will_delay_وقت = client->will_delay_وقت;
	context->session_expiry_وقت = client->session_expiry_وقت;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->وڌ_کان_وڌ_packet_سائيز = client->max_packet_سائيز;
	context->retain_موجود = client->retain_موجود;
	context->is_persisted = true;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	إذا(db.config->في_listener_settings && client->listener_پورٽ > 0){
		لكل(صحيح i=0; i < db.config->listener_ڳڻپ; i++){
			إذا(db.config->listeners[i].پورٽ == client->listener_پورٽ){
				context->listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	context_جوڙيو_منزل_by_id(context);
	session_expiry_جوڙيو_from_persistence(context, context->session_expiry_وقت);

	أرجع MOSQ_ERR_SUCCESS;
error:
	محفوظ_FREE(client->clientid);
	محفوظ_FREE(client->username);
	محفوظ_FREE(client->auth_طريقو);
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
	HASH_ڳوليو(hh_id, db.contexts_by_id, client->clientid, لڙي_ڊيگهه(client->clientid), context);
	إذا(context == عدم){
		rc = MOSQ_ERR_NOT_مليو;
		اذهب error;
	}

	mosquitto_free(context->username);
	context->username = client->username;
	client->username = عدم;

	context->clean_شروعات = false;
	context->will_delay_وقت = client->will_delay_وقت;
	context->session_expiry_وقت = client->session_expiry_وقت;
	context->will_delay_interval = client->will_delay_interval;
	context->session_expiry_interval = client->session_expiry_interval;
	context->max_qos = client->max_qos;
	context->وڌ_کان_وڌ_packet_سائيز = client->max_packet_سائيز;
	context->retain_موجود = client->retain_موجود;

	/* in per_listener_settings mode, try to find the listener by persisted port */
	إذا(db.config->في_listener_settings && client->listener_پورٽ > 0){
		لكل(صحيح i=0; i < db.config->listener_ڳڻپ; i++){
			إذا(db.config->listeners[i].پورٽ == client->listener_پورٽ){
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
	HASH_ڳوليو(hh_id, db.contexts_by_id, clientid, لڙي_ڊيگهه(clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_SUCCESS;
	}

	session_expiry__remove(context);
	will_delay__remove(context);
	will__clear(context);

	context->clean_شروعات = true;
	context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوري;
	context->is_persisted = false;
	mosquitto_سيٽ_state(context, mosq_cs_duplicate);
	do_disconnect(context, MOSQ_ERR_SUCCESS);

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن بنية mosquitto_بنياد_نياپو *ڳوليو_store_نياپو(uint64_t store_id)
{
	بنية mosquitto_بنياد_نياپو *بنياد_نياپو;

	HASH_ڳوليو(hh, db.نياپو_store, &store_id, حجم(store_id), بنياد_نياپو);
	أرجع بنياد_نياپو;
}


BROKER_EXPORT صحيح mosquitto_persist_client_نياپو_جوڙيو(بنية mosquitto_client_نياپو_2 *client_نياپو)
{
	بنية mosquitto *context;
	بنية mosquitto_بنياد_نياپو *بنياد_نياپو;

	إذا(client_نياپو == عدم || client_نياپو->clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_ڳوليو(hh_id, db.contexts_by_id, client_نياپو->clientid, لڙي_ڊيگهه(client_نياپو->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_مليو;
	}
	بنياد_نياپو = ڳوليو_store_نياپو(client_نياپو->store_id);
	إذا(بنياد_نياپو == عدم){
		أرجع MOSQ_ERR_NOT_مليو;
	}

	إذا(client_نياپو->direction == mosq_md_ٻاهريون){
		إذا(client_نياپو->qos > 0){
			context->آخري_mid = client_نياپو->mid;
		}
		أرجع db_پيغام_وجهو_outgoing(context, client_نياپو->cmsg_id, client_نياپو->mid,
				client_نياپو->qos, client_نياپو->retain,
				بنياد_نياپو, client_نياپو->subscription_identifier, false, false);
	}وإلا إذا(client_نياپو->direction == mosq_md_اندريون){
		أرجع db_پيغام_وجهو_incoming(context, client_نياپو->cmsg_id, بنياد_نياپو, false);
	}وإلا{
		أرجع MOSQ_ERR_INVAL;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_persist_client_نياپو_delete(بنية mosquitto_client_نياپو_2 *client_نياپو)
{
	بنية mosquitto *context;

	إذا(client_نياپو == عدم || client_نياپو->clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_ڳوليو(hh_id, db.contexts_by_id, client_نياپو->clientid, لڙي_ڊيگهه(client_نياپو->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_مليو;
	}


	صحيح rc = MOSQ_ERR_INVAL;
	إذا(client_نياپو->direction == mosq_md_ٻاهريون){
		rc = db_پيغام_delete_outgoing(context, client_نياپو->mid, mosq_ms_any, client_نياپو->qos);
	}وإلا إذا(client_نياپو->direction == mosq_md_اندريون){
		rc = db_پيغام_remove_incoming(context, client_نياپو->mid);
	}
	أرجع rc;
}


BROKER_EXPORT صحيح mosquitto_persist_client_نياپو_update(بنية mosquitto_client_نياپو_2 *client_نياپو)
{
	بنية mosquitto *context;

	إذا(client_نياپو == عدم || client_نياپو->clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_ڳوليو(hh_id, db.contexts_by_id, client_نياپو->clientid, لڙي_ڊيگهه(client_نياپو->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_مليو;
	}

	إذا(client_نياپو->direction == mosq_md_ٻاهريون){
		db_پيغام_update_outgoing(context, client_نياپو->mid, client_نياپو->state, client_نياپو->qos, false);
	}وإلا إذا(client_نياپو->direction == mosq_md_اندريون){
		// FIXME db__message_update_incoming(context, client_msg->mid, client_msg->state, client_msg->qos, false);
	}وإلا{
		أرجع MOSQ_ERR_INVAL;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_persist_client_نياپو_clear(بنية mosquitto_client_نياپو_2 *client_نياپو)
{
	بنية mosquitto *context;

	إذا(client_نياپو == عدم || client_نياپو->clientid == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_ڳوليو(hh_id, db.contexts_by_id, client_نياپو->clientid, لڙي_ڊيگهه(client_نياپو->clientid), context);
	إذا(context == عدم){
		أرجع MOSQ_ERR_NOT_مليو;
	}

	إذا(client_نياپو->direction == mosq_bmd_اندريون || client_نياپو->direction == mosq_bmd_all){
		db__messages_delete_incoming(context);
	}
	إذا(client_نياپو->direction == mosq_bmd_ٻاهريون || client_نياپو->direction == mosq_bmd_all){
		db__messages_delete_outgoing(context);
	}
	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_subscription_جوڙيو(ثابت بنية mosquitto_subscription *sub)
{
	بنية mosquitto *context;

	إذا(sub == عدم || sub->clientid == عدم || sub->topic_filter == عدم || sub->clientid[0] == '\0' || sub->topic_filter[0] == '\0'){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_ڳوليو(hh_id, db.contexts_by_id, sub->clientid, لڙي_ڊيگهه(sub->clientid), context);

	إذا(context){
		أرجع sub_جوڙيو(context, sub);
	}وإلا{
		أرجع MOSQ_ERR_NOT_مليو;
	}
}


BROKER_EXPORT صحيح mosquitto_subscription_delete(ثابت حرف *clientid, ثابت حرف *topic)
{
	بنية mosquitto *context;
	uint8_t reason;

	إذا(clientid == عدم || topic == عدم || clientid[0] == '\0' || topic[0] == '\0'){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_ڳوليو(hh_id, db.contexts_by_id, clientid, لڙي_ڊيگهه(clientid), context);

	إذا(context){
		أرجع sub__remove(context, topic, &reason);
	}وإلا{
		أرجع MOSQ_ERR_NOT_مليو;
	}
}


BROKER_EXPORT صحيح mosquitto_persist_بنياد_نياپو_جوڙيو(بنية mosquitto_بنياد_نياپو_2 *نياپو_جوڙيو)
{
	بنية mosquitto context;
	بنية mosquitto_بنياد_نياپو *بنياد_نياپو;
	صحيح rc;

	memset(&context, 0, حجم(context));

	إذا(نياپو_جوڙيو->payloadlen > MQTT_MAX_PAYLOAD){
		أرجع MOSQ_ERR_INVAL;
	}

	/* db__message_store only takes a copy of .id and .username, so it is reasonably safe
	 * to cast the const char * to char * */
	context.id = (حرف *)نياپو_جوڙيو->ذريعو_id;
	context.username = (حرف *)نياپو_جوڙيو->ذريعو_username;

	بنياد_نياپو = mosquitto_calloc(1, حجم(بنية mosquitto_بنياد_نياپو));
	إذا(بنياد_نياپو == عدم){
		اذهب error;
	}
	بنياد_نياپو->معلومات.store_id = نياپو_جوڙيو->store_id;
	بنياد_نياپو->معلومات.expiry_وقت = نياپو_جوڙيو->expiry_وقت;
	بنياد_نياپو->معلومات.payloadlen = نياپو_جوڙيو->payloadlen;
	بنياد_نياپو->معلومات.ذريعو_mid = نياپو_جوڙيو->ذريعو_mid;
	بنياد_نياپو->معلومات.qos = نياپو_جوڙيو->qos;
	بنياد_نياپو->معلومات.retain = نياپو_جوڙيو->retain;

	بنياد_نياپو->معلومات.payload = نياپو_جوڙيو->payload;
	نياپو_جوڙيو->payload = عدم;
	بنياد_نياپو->معلومات.topic = نياپو_جوڙيو->topic;
	نياپو_جوڙيو->topic = عدم;
	بنياد_نياپو->معلومات.properties = نياپو_جوڙيو->properties;
	نياپو_جوڙيو->properties = عدم;

	إذا(نياپو_جوڙيو->ذريعو_پورٽ){
		لكل(صحيح i=0; i<db.config->listener_ڳڻپ; i++){
			إذا(db.config->listeners[i].پورٽ == نياپو_جوڙيو->ذريعو_پورٽ){
				بنياد_نياپو->ذريعو_listener = &db.config->listeners[i];
				اكسر;
			}
		}
	}

	بنياد_نياپو->stored = true;
	rc = db_پيغام_store(&context, بنياد_نياپو, عدم, mosq_mo_broker);
	أرجع rc;

error:
	mosquitto_property_free_all(&نياپو_جوڙيو->properties);
	mosquitto_free(نياپو_جوڙيو->topic);
	mosquitto_free(نياپو_جوڙيو->payload);
	mosquitto_free(بنياد_نياپو);

	أرجع MOSQ_ERR_NOMEM;
}


BROKER_EXPORT صحيح mosquitto_persist_بنياد_نياپو_delete(uint64_t store_id)
{
	بنية mosquitto_بنياد_نياپو *بنياد_نياپو;

	بنياد_نياپو = ڳوليو_store_نياپو(store_id);
	إذا(بنياد_نياپو && بنياد_نياپو->ref_ڳڻپ == 0){
		/* If ref count is zero, then we should delete this. It might seem
		 * surprising that the ref count is zero already, but it can be. If ref
		 * count is greater than zero then there may be e.g. a retained message
		 * still referring to this and the retained message persist update is
		 * coming later. If we delete the message now in that case, then when
		 * the retain changes there will be use after free errors. All messages
		 * will eventually hit ref count 0 and be removed in some way or other.
		 */
		db_نياپو_store_remove(بنياد_نياپو, false);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT فراغ mosquitto_complete_basic_auth(ثابت حرف *clientid, صحيح نتيجو)
{
	بنية mosquitto *context;

	إذا(clientid == عدم){
		أرجع;
	}

	HASH_ڳوليو(hh_id, db.contexts_by_id_delayed_auth, clientid, لڙي_ڊيگهه(clientid), context);
	إذا(context){
		HASH_DELETE(hh_id, db.contexts_by_id_delayed_auth, context);
		إذا(نتيجو == MOSQ_ERR_SUCCESS){
			connect__on_authorised(context, عدم, 0);
		}وإلا{
			إذا(context->protocol == mosq_p_mqtt5){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, عدم);
			}وإلا{
				send__connack(context, 0, CONNACK_REFUSED_NOT_AUTHORIZED, عدم);
			}
			context->clean_شروعات = true;
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوري;
			will__clear(context);
			do_disconnect(context, MOSQ_ERR_AUTH);
		}
	}
}


BROKER_EXPORT صحيح mosquitto_broker_ڳنڍ_id_سيٽ(uint16_t id)
{
	إذا(id > 1023){
		أرجع MOSQ_ERR_INVAL;
	}وإلا{
		db.ڳنڍ_id = id;
		db.ڳنڍ_id_shifted = ((uint64_t)id) << 54;
		أرجع MOSQ_ERR_SUCCESS;
	}
}


BROKER_EXPORT ثابت حرف *mosquitto_persistence_location(فراغ)
{
	أرجع db.config->persistence_location;
}


BROKER_EXPORT صحيح mosquitto_client_will_سيٽ(ثابت حرف *clientid, ثابت حرف *topic, صحيح payloadlen, ثابت فراغ *payload, صحيح qos, bool retain, mosquitto_property *properties)
{
	بنية mosquitto *mosq = mosquitto_client(clientid);
	إذا(!mosq){
		أرجع MOSQ_ERR_NOT_مليو;
	}
	إذا(properties && mosq->protocol != mosq_p_mqtt5){
		إذا(net__is_connected(mosq)){
			أرجع MOSQ_ERR_NOT_SUPPORTED;
		}
		mosq->protocol = mosq_p_mqtt5;
	}
	أرجع will_سيٽ(mosq, topic, payloadlen, payload, qos, retain, properties);
}
