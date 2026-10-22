/*
Copyright (c) 2021 Roger Light <roger@atchoo.org>

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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "send_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utlist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "lib_load.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "will_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>


ਖਾਲੀ plugin_persist__handle_restore(ਖਾਲੀ)
{
	ਬਣਤਰ mosquitto_evt_persist_restore event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਬਣਤਰ mosquitto__security_options *opts;

	opts = &db.config->security_options;
	memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.persist_restore, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		cb_ਅਧਾਰ->cb(MOSQ_EVT_PERSIST_RESTORE, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
	}
}


ਖਾਲੀ plugin_persist_handle_client_ਜੋੜੋ(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ mosquitto_evt_persist_client event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਬਣਤਰ mosquitto__security_options *opts;

	ਜੇ(db.shutdown || context->is_persisted){
		ਵਾਪਸ;
	}

	opts = &db.config->security_options;
	memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.clientid = context->id;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.username = context->username;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.auth_ਵਿਧੀ = context->auth_ਵਿਧੀ;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.will_delay_ਸਮਾਂ = context->will_delay_ਸਮਾਂ;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.session_expiry_ਸਮਾਂ = context->session_expiry_ਸਮਾਂ;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.will_delay_interval = context->will_delay_interval;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.session_expiry_interval = context->session_expiry_interval;
	ਜੇ(context->listener){
		event_ਅੰਕੜੇ.ਅੰਕੜੇ.listener_ਪੋਰਟ = context->listener->ਪੋਰਟ;
	}ਨਹੀਂਤਾਂ{
		event_ਅੰਕੜੇ.ਅੰਕੜੇ.listener_ਪੋਰਟ = 0;
	}
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.max_qos = context->max_qos;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.retain_ਉਪਲਬਧ = context->retain_ਉਪਲਬਧ;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.max_packet_ਮਾਪ = context->ਵੱਧ_ਤੋਂ_ਵੱਧ_packet_ਮਾਪ;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.persist_client_ਜੋੜੋ, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		cb_ਅਧਾਰ->cb(MOSQ_EVT_PERSIST_CLIENT_ਜੋੜੋ, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
	}

	ਜੇ(context->will){
		plugin_persist_handle_will_ਜੋੜੋ(context);
	}

	context->is_persisted = true;
}


ਖਾਲੀ plugin_persist__handle_client_update(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ mosquitto_evt_persist_client event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਬਣਤਰ mosquitto__security_options *opts;
	ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_v5_2 will;

	ਅਣਵਰਤਿਆ(will); /* FIXME */

	ਜੇ(db.shutdown){
		ਵਾਪਸ;
	}

	opts = &db.config->security_options;
	memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.clientid = context->id;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.username = context->username;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.auth_ਵਿਧੀ = context->auth_ਵਿਧੀ;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.will_delay_ਸਮਾਂ = context->will_delay_ਸਮਾਂ;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.session_expiry_ਸਮਾਂ = context->session_expiry_ਸਮਾਂ;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.will_delay_interval = context->will_delay_interval;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.session_expiry_interval = context->session_expiry_interval;
	ਜੇ(context->listener){
		event_ਅੰਕੜੇ.ਅੰਕੜੇ.listener_ਪੋਰਟ = context->listener->ਪੋਰਟ;
	}ਨਹੀਂਤਾਂ{
		event_ਅੰਕੜੇ.ਅੰਕੜੇ.listener_ਪੋਰਟ = 0;
	}
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.max_qos = context->max_qos;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.retain_ਉਪਲਬਧ = context->retain_ਉਪਲਬਧ;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.max_packet_ਮਾਪ = context->ਵੱਧ_ਤੋਂ_ਵੱਧ_packet_ਮਾਪ;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.persist_client_update, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		cb_ਅਧਾਰ->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
	}

	ਜੇ(context->will){
		plugin_persist_handle_will_ਜੋੜੋ(context);
	}ਨਹੀਂਤਾਂ{
		plugin_persist__handle_will_delete(context);
	}
}


ਖਾਲੀ plugin_persist__handle_client_delete(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ mosquitto_evt_persist_client event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਬਣਤਰ mosquitto__security_options *opts;

	ਜੇ(context->id == ਨਿਰਾ
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		ਵਾਪਸ;
	}

	plugin_persist__handle_will_delete(context);

	ਜੇ(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_ਤੁਰੰਤ){
		ਵਾਪਸ;
	}

	opts = &db.config->security_options;
	memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.clientid = context->id;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.persist_client_delete, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		cb_ਅਧਾਰ->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
	}
	context->is_persisted = false;
}


ਖਾਲੀ plugin_persist_handle_subscription_ਜੋੜੋ(ਬਣਤਰ mosquitto *context, ਅਚਲ ਬਣਤਰ mosquitto_subscription *sub)
{
	ਬਣਤਰ mosquitto_evt_persist_subscription event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਬਣਤਰ mosquitto__security_options *opts;

	ਜੇ(db.shutdown || context->is_persisted == false){
		ਵਾਪਸ;
	}

	opts = &db.config->security_options;
	memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.clientid = context->id;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.topic_filter = sub->topic_filter;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.identifier = sub->identifier;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.options = sub->options;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.persist_subscription_ਜੋੜੋ, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		cb_ਅਧਾਰ->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_ਜੋੜੋ, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
	}
}


ਖਾਲੀ plugin_persist__handle_subscription_delete(ਬਣਤਰ mosquitto *context, ਅੱਖਰ *sub)
{
	ਬਣਤਰ mosquitto_evt_persist_subscription event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਬਣਤਰ mosquitto__security_options *opts;

	ਜੇ(db.shutdown || context->is_persisted == false){
		ਵਾਪਸ;
	}
	ਜੇ(!sub){
		ਵਾਪਸ;
	}

	opts = &db.config->security_options;
	memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.clientid = context->id;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.topic_filter = sub;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.persist_subscription_delete, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		cb_ਅਧਾਰ->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
	}
}


ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ਸੈੱਟ_client_ਸੁਨੇਹਾ_event_ਅੰਕੜੇ(ਬਣਤਰ mosquitto_evt_persist_client_ਸੁਨੇਹਾ *event_ਅੰਕੜੇ, ਬਣਤਰ mosquitto *context, ਅਚਲ ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ)
{
	event_ਅੰਕੜੇ->ਅੰਕੜੇ.clientid = context->id;
	event_ਅੰਕੜੇ->ਅੰਕੜੇ.cmsg_id = client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.cmsg_id;
	event_ਅੰਕੜੇ->ਅੰਕੜੇ.direction = (uint8_t)client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.direction;
	event_ਅੰਕੜੇ->ਅੰਕੜੇ.dup = client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.dup;
	event_ਅੰਕੜੇ->ਅੰਕੜੇ.mid = client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.mid;
	event_ਅੰਕੜੇ->ਅੰਕੜੇ.qos = client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos;
	event_ਅੰਕੜੇ->ਅੰਕੜੇ.retain = client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain;
	event_ਅੰਕੜੇ->ਅੰਕੜੇ.state = (uint8_t)client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state;
	event_ਅੰਕੜੇ->ਅੰਕੜੇ.store_id = client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.store_id;
	event_ਅੰਕੜੇ->ਅੰਕੜੇ.subscription_identifier = client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.subscription_identifier;
}


ਖਾਲੀ plugin_persist_handle_client_ਸੁਨੇਹਾ_ਜੋੜੋ(ਬਣਤਰ mosquitto *context, ਅਚਲ ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ)
{
	ਬਣਤਰ mosquitto_evt_persist_client_ਸੁਨੇਹਾ event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਬਣਤਰ mosquitto__security_options *opts;

	ਜੇ(context->is_persisted == false
			|| (client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos == 0 && db.config->ਕਤਾਰ_qos0_messages == false)
			|| db.shutdown){

		ਵਾਪਸ;
	}

	opts = &db.config->security_options;
	memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));

	ਸੈੱਟ_client_ਸੁਨੇਹਾ_event_ਅੰਕੜੇ(&event_ਅੰਕੜੇ, context, client_ਸੁਨੇਹਾ);

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.persist_client_ਸੁਨੇਹਾ_ਜੋੜੋ, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		cb_ਅਧਾਰ->cb(MOSQ_EVT_PERSIST_CLIENT_ਸੁਨੇਹਾ_ਜੋੜੋ, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
	}
}


ਖਾਲੀ plugin_persist_handle_client_ਸੁਨੇਹਾ_delete(ਬਣਤਰ mosquitto *context, ਅਚਲ ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ)
{
	ਬਣਤਰ mosquitto_evt_persist_client_ਸੁਨੇਹਾ event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਬਣਤਰ mosquitto__security_options *opts;

	ਜੇ(context->is_persisted == false
			|| (client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos == 0 && db.config->ਕਤਾਰ_qos0_messages == false)
			|| db.shutdown){

		ਵਾਪਸ;
	}

	opts = &db.config->security_options;
	memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));

	ਸੈੱਟ_client_ਸੁਨੇਹਾ_event_ਅੰਕੜੇ(&event_ਅੰਕੜੇ, context, client_ਸੁਨੇਹਾ);

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.persist_client_ਸੁਨੇਹਾ_delete, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		cb_ਅਧਾਰ->cb(MOSQ_EVT_PERSIST_CLIENT_ਸੁਨੇਹਾ_DELETE, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
	}
}


ਖਾਲੀ plugin_persist_handle_client_ਸੁਨੇਹਾ_update(ਬਣਤਰ mosquitto *context, ਅਚਲ ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ)
{
	ਬਣਤਰ mosquitto_evt_persist_client_ਸੁਨੇਹਾ event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਬਣਤਰ mosquitto__security_options *opts;

	ਜੇ(context->is_persisted == false
			|| (client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos == 0 && db.config->ਕਤਾਰ_qos0_messages == false)
			|| db.shutdown){

		ਵਾਪਸ;
	}

	opts = &db.config->security_options;
	memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));

	ਸੈੱਟ_client_ਸੁਨੇਹਾ_event_ਅੰਕੜੇ(&event_ਅੰਕੜੇ, context, client_ਸੁਨੇਹਾ);

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.persist_client_ਸੁਨੇਹਾ_update, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		cb_ਅਧਾਰ->cb(MOSQ_EVT_PERSIST_CLIENT_ਸੁਨੇਹਾ_UPDATE, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
	}
}


ਖਾਲੀ plugin_persist_handle_ਅਧਾਰ_ਸੁਨੇਹਾ_ਜੋੜੋ(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ)
{
	ਬਣਤਰ mosquitto_evt_persist_ਅਧਾਰ_ਸੁਨੇਹਾ event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਬਣਤਰ mosquitto__security_options *opts;

	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->stored || db.shutdown){
		ਵਾਪਸ;
	}

	opts = &db.config->security_options;
	memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));

	event_ਅੰਕੜੇ.ਅੰਕੜੇ.store_id = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.store_id;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.expiry_ਸਮਾਂ = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.expiry_ਸਮਾਂ;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.topic = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.payload = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.ਸਰੋਤ_id = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_id;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.ਸਰੋਤ_username = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_username;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.properties = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.properties;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.payloadlen = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.ਸਰੋਤ_mid = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid;
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਸਰੋਤ_listener){
		event_ਅੰਕੜੇ.ਅੰਕੜੇ.ਸਰੋਤ_ਪੋਰਟ = ਅਧਾਰ_ਸੁਨੇਹਾ->ਸਰੋਤ_listener->ਪੋਰਟ;
	}ਨਹੀਂਤਾਂ{
		event_ਅੰਕੜੇ.ਅੰਕੜੇ.ਸਰੋਤ_ਪੋਰਟ = 0;
	}
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.qos = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.retain = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.persist_ਅਧਾਰ_ਸੁਨੇਹਾ_ਜੋੜੋ, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		cb_ਅਧਾਰ->cb(MOSQ_EVT_PERSIST_ਅਧਾਰ_ਸੁਨੇਹਾ_ਜੋੜੋ, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
	}
	ਅਧਾਰ_ਸੁਨੇਹਾ->stored = true;
}


ਖਾਲੀ plugin_persist_handle_ਅਧਾਰ_ਸੁਨੇਹਾ_delete(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ)
{
	ਬਣਤਰ mosquitto_evt_persist_ਅਧਾਰ_ਸੁਨੇਹਾ event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਬਣਤਰ mosquitto__security_options *opts;

	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->stored == false || db.shutdown){
		ਵਾਪਸ;
	}

	opts = &db.config->security_options;
	memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));

	event_ਅੰਕੜੇ.ਅੰਕੜੇ.store_id = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.store_id;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.persist_ਅਧਾਰ_ਸੁਨੇਹਾ_delete, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		cb_ਅਧਾਰ->cb(MOSQ_EVT_PERSIST_ਅਧਾਰ_ਸੁਨੇਹਾ_DELETE, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
	}
	ਅਧਾਰ_ਸੁਨੇਹਾ->stored = false;
}


ਖਾਲੀ plugin_persist_handle_retain_ਸੁਨੇਹਾ_ਸੈੱਟ(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ)
{
	ਬਣਤਰ mosquitto_evt_persist_retain_ਸੁਨੇਹਾ event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਬਣਤਰ mosquitto__security_options *opts;

	ਜੇ(db.shutdown){
		ਵਾਪਸ;
	}

	opts = &db.config->security_options;
	memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));

	event_ਅੰਕੜੇ.store_id = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.store_id;
	event_ਅੰਕੜੇ.topic = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.persist_retain_ਸੁਨੇਹਾ_ਸੈੱਟ, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		cb_ਅਧਾਰ->cb(MOSQ_EVT_PERSIST_RETAIN_ਸੁਨੇਹਾ_ਸੈੱਟ, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
	}
}


ਖਾਲੀ plugin_persist_handle_retain_ਸੁਨੇਹਾ_delete(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ)
{
	ਬਣਤਰ mosquitto_evt_persist_retain_ਸੁਨੇਹਾ event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਬਣਤਰ mosquitto__security_options *opts;

	ਜੇ(db.shutdown){
		ਵਾਪਸ;
	}

	opts = &db.config->security_options;
	memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));

	event_ਅੰਕੜੇ.topic = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.persist_retain_ਸੁਨੇਹਾ_delete, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		cb_ਅਧਾਰ->cb(MOSQ_EVT_PERSIST_RETAIN_ਸੁਨੇਹਾ_DELETE, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
	}
}


ਖਾਲੀ plugin_persist_handle_will_ਜੋੜੋ(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ mosquitto_evt_persist_will_ਸੁਨੇਹਾ event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਬਣਤਰ mosquitto__security_options *opts;
	ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ *will_ਸੁਨੇਹਾ;

	ਜੇ(db.shutdown || !context->will){
		ਵਾਪਸ;
	}

	opts = &db.config->security_options;
	will_ਸੁਨੇਹਾ = &context->will->ਸੁਨੇਹਾ;
	memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.clientid = context->id;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.topic = will_ਸੁਨੇਹਾ->topic;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.payload = will_ਸੁਨੇਹਾ->payload;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.payloadlen = (uint32_t)will_ਸੁਨੇਹਾ->payloadlen;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.qos = (uint8_t)will_ਸੁਨੇਹਾ->qos;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.retain = will_ਸੁਨੇਹਾ->retain;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.properties = context->will->properties;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.persist_will_ਜੋੜੋ, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		cb_ਅਧਾਰ->cb(MOSQ_EVT_PERSIST_WILL_ਜੋੜੋ, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
	}
}


ਖਾਲੀ plugin_persist__handle_will_delete(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ mosquitto_evt_persist_will_ਸੁਨੇਹਾ event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਬਣਤਰ mosquitto__security_options *opts;

	memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.clientid = context->id;

	ਜੇ(db.shutdown){
		ਵਾਪਸ;
	}

	opts = &db.config->security_options;
	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.persist_will_delete, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		cb_ਅਧਾਰ->cb(MOSQ_EVT_PERSIST_WILL_ਜੋੜੋ, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
	}

}
