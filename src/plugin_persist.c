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

#شامل "config.h"

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto_internal.h"
#شامل "mosquitto/broker.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "send_mosq.h"
#شامل "util_mosq.h"
#شامل "utlist.h"
#شامل "lib_load.h"
#شامل "will_mosq.h"
#شامل <stdint.h>


خالی plugin_persist__handle_restore(خالی)
{
	ساخت mosquitto_evt_persist_restore event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	ساخت mosquitto__security_options *opts;

	opts = &db.config->security_options;
	memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_restore, cb_بنیاد, cb_اگلا){
		cb_بنیاد->cb(MOSQ_EVT_PERSIST_RESTORE, &event_ڈیٹا, cb_بنیاد->userdata);
	}
}


خالی plugin_persist_handle_client_جوڑو(ساخت mosquitto *context)
{
	ساخت mosquitto_evt_persist_client event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	ساخت mosquitto__security_options *opts;

	اگر(db.shutdown || context->is_persisted){
		واپس;
	}

	opts = &db.config->security_options;
	memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));
	event_ڈیٹا.ڈیٹا.clientid = context->id;
	event_ڈیٹا.ڈیٹا.username = context->username;
	event_ڈیٹا.ڈیٹا.auth_طریقہ = context->auth_طریقہ;
	event_ڈیٹا.ڈیٹا.will_delay_وقت = context->will_delay_وقت;
	event_ڈیٹا.ڈیٹا.session_expiry_وقت = context->session_expiry_وقت;
	event_ڈیٹا.ڈیٹا.will_delay_interval = context->will_delay_interval;
	event_ڈیٹا.ڈیٹا.session_expiry_interval = context->session_expiry_interval;
	اگر(context->listener){
		event_ڈیٹا.ڈیٹا.listener_پورٹ = context->listener->پورٹ;
	}ورنہ{
		event_ڈیٹا.ڈیٹا.listener_پورٹ = 0;
	}
	event_ڈیٹا.ڈیٹا.max_qos = context->max_qos;
	event_ڈیٹا.ڈیٹا.retain_دستیاب = context->retain_دستیاب;
	event_ڈیٹا.ڈیٹا.max_packet_سائز = context->زیادہ_سے_زیادہ_packet_سائز;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_client_جوڑو, cb_بنیاد, cb_اگلا){
		cb_بنیاد->cb(MOSQ_EVT_PERSIST_CLIENT_جوڑو, &event_ڈیٹا, cb_بنیاد->userdata);
	}

	اگر(context->will){
		plugin_persist_handle_will_جوڑو(context);
	}

	context->is_persisted = true;
}


خالی plugin_persist__handle_client_update(ساخت mosquitto *context)
{
	ساخت mosquitto_evt_persist_client event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	ساخت mosquitto__security_options *opts;
	ساخت mosquitto_پیغام_v5_2 will;

	غیر_مستعمل(will); /* FIXME */

	اگر(db.shutdown){
		واپس;
	}

	opts = &db.config->security_options;
	memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));
	event_ڈیٹا.ڈیٹا.clientid = context->id;
	event_ڈیٹا.ڈیٹا.username = context->username;
	event_ڈیٹا.ڈیٹا.auth_طریقہ = context->auth_طریقہ;
	event_ڈیٹا.ڈیٹا.will_delay_وقت = context->will_delay_وقت;
	event_ڈیٹا.ڈیٹا.session_expiry_وقت = context->session_expiry_وقت;
	event_ڈیٹا.ڈیٹا.will_delay_interval = context->will_delay_interval;
	event_ڈیٹا.ڈیٹا.session_expiry_interval = context->session_expiry_interval;
	اگر(context->listener){
		event_ڈیٹا.ڈیٹا.listener_پورٹ = context->listener->پورٹ;
	}ورنہ{
		event_ڈیٹا.ڈیٹا.listener_پورٹ = 0;
	}
	event_ڈیٹا.ڈیٹا.max_qos = context->max_qos;
	event_ڈیٹا.ڈیٹا.retain_دستیاب = context->retain_دستیاب;
	event_ڈیٹا.ڈیٹا.max_packet_سائز = context->زیادہ_سے_زیادہ_packet_سائز;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_client_update, cb_بنیاد, cb_اگلا){
		cb_بنیاد->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_ڈیٹا, cb_بنیاد->userdata);
	}

	اگر(context->will){
		plugin_persist_handle_will_جوڑو(context);
	}ورنہ{
		plugin_persist__handle_will_delete(context);
	}
}


خالی plugin_persist__handle_client_delete(ساخت mosquitto *context)
{
	ساخت mosquitto_evt_persist_client event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	ساخت mosquitto__security_options *opts;

	اگر(context->id == تہی
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		واپس;
	}

	plugin_persist__handle_will_delete(context);

	اگر(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوری){
		واپس;
	}

	opts = &db.config->security_options;
	memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));
	event_ڈیٹا.ڈیٹا.clientid = context->id;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_client_delete, cb_بنیاد, cb_اگلا){
		cb_بنیاد->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_ڈیٹا, cb_بنیاد->userdata);
	}
	context->is_persisted = false;
}


خالی plugin_persist_handle_subscription_جوڑو(ساخت mosquitto *context, ثابت ساخت mosquitto_subscription *sub)
{
	ساخت mosquitto_evt_persist_subscription event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	ساخت mosquitto__security_options *opts;

	اگر(db.shutdown || context->is_persisted == false){
		واپس;
	}

	opts = &db.config->security_options;
	memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));
	event_ڈیٹا.ڈیٹا.clientid = context->id;
	event_ڈیٹا.ڈیٹا.topic_filter = sub->topic_filter;
	event_ڈیٹا.ڈیٹا.identifier = sub->identifier;
	event_ڈیٹا.ڈیٹا.options = sub->options;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_subscription_جوڑو, cb_بنیاد, cb_اگلا){
		cb_بنیاد->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_جوڑو, &event_ڈیٹا, cb_بنیاد->userdata);
	}
}


خالی plugin_persist__handle_subscription_delete(ساخت mosquitto *context, حرف *sub)
{
	ساخت mosquitto_evt_persist_subscription event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	ساخت mosquitto__security_options *opts;

	اگر(db.shutdown || context->is_persisted == false){
		واپس;
	}
	اگر(!sub){
		واپس;
	}

	opts = &db.config->security_options;
	memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));
	event_ڈیٹا.ڈیٹا.clientid = context->id;
	event_ڈیٹا.ڈیٹا.topic_filter = sub;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_subscription_delete, cb_بنیاد, cb_اگلا){
		cb_بنیاد->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_ڈیٹا, cb_بنیاد->userdata);
	}
}


ساکن اندرونی خالی سیٹ_client_پیغام_event_ڈیٹا(ساخت mosquitto_evt_persist_client_پیغام *event_ڈیٹا, ساخت mosquitto *context, ثابت ساخت mosquitto_client_پیغام *client_پیغام)
{
	event_ڈیٹا->ڈیٹا.clientid = context->id;
	event_ڈیٹا->ڈیٹا.cmsg_id = client_پیغام->ڈیٹا.cmsg_id;
	event_ڈیٹا->ڈیٹا.direction = (uint8_t)client_پیغام->ڈیٹا.direction;
	event_ڈیٹا->ڈیٹا.dup = client_پیغام->ڈیٹا.dup;
	event_ڈیٹا->ڈیٹا.mid = client_پیغام->ڈیٹا.mid;
	event_ڈیٹا->ڈیٹا.qos = client_پیغام->ڈیٹا.qos;
	event_ڈیٹا->ڈیٹا.retain = client_پیغام->ڈیٹا.retain;
	event_ڈیٹا->ڈیٹا.state = (uint8_t)client_پیغام->ڈیٹا.state;
	event_ڈیٹا->ڈیٹا.store_id = client_پیغام->بنیاد_پیغام->ڈیٹا.store_id;
	event_ڈیٹا->ڈیٹا.subscription_identifier = client_پیغام->ڈیٹا.subscription_identifier;
}


خالی plugin_persist_handle_client_پیغام_جوڑو(ساخت mosquitto *context, ثابت ساخت mosquitto_client_پیغام *client_پیغام)
{
	ساخت mosquitto_evt_persist_client_پیغام event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	ساخت mosquitto__security_options *opts;

	اگر(context->is_persisted == false
			|| (client_پیغام->ڈیٹا.qos == 0 && db.config->قطار_qos0_messages == false)
			|| db.shutdown){

		واپس;
	}

	opts = &db.config->security_options;
	memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));

	سیٹ_client_پیغام_event_ڈیٹا(&event_ڈیٹا, context, client_پیغام);

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_client_پیغام_جوڑو, cb_بنیاد, cb_اگلا){
		cb_بنیاد->cb(MOSQ_EVT_PERSIST_CLIENT_پیغام_جوڑو, &event_ڈیٹا, cb_بنیاد->userdata);
	}
}


خالی plugin_persist_handle_client_پیغام_delete(ساخت mosquitto *context, ثابت ساخت mosquitto_client_پیغام *client_پیغام)
{
	ساخت mosquitto_evt_persist_client_پیغام event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	ساخت mosquitto__security_options *opts;

	اگر(context->is_persisted == false
			|| (client_پیغام->ڈیٹا.qos == 0 && db.config->قطار_qos0_messages == false)
			|| db.shutdown){

		واپس;
	}

	opts = &db.config->security_options;
	memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));

	سیٹ_client_پیغام_event_ڈیٹا(&event_ڈیٹا, context, client_پیغام);

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_client_پیغام_delete, cb_بنیاد, cb_اگلا){
		cb_بنیاد->cb(MOSQ_EVT_PERSIST_CLIENT_پیغام_DELETE, &event_ڈیٹا, cb_بنیاد->userdata);
	}
}


خالی plugin_persist_handle_client_پیغام_update(ساخت mosquitto *context, ثابت ساخت mosquitto_client_پیغام *client_پیغام)
{
	ساخت mosquitto_evt_persist_client_پیغام event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	ساخت mosquitto__security_options *opts;

	اگر(context->is_persisted == false
			|| (client_پیغام->ڈیٹا.qos == 0 && db.config->قطار_qos0_messages == false)
			|| db.shutdown){

		واپس;
	}

	opts = &db.config->security_options;
	memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));

	سیٹ_client_پیغام_event_ڈیٹا(&event_ڈیٹا, context, client_پیغام);

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_client_پیغام_update, cb_بنیاد, cb_اگلا){
		cb_بنیاد->cb(MOSQ_EVT_PERSIST_CLIENT_پیغام_UPDATE, &event_ڈیٹا, cb_بنیاد->userdata);
	}
}


خالی plugin_persist_handle_بنیاد_پیغام_جوڑو(ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام)
{
	ساخت mosquitto_evt_persist_بنیاد_پیغام event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	ساخت mosquitto__security_options *opts;

	اگر(بنیاد_پیغام->stored || db.shutdown){
		واپس;
	}

	opts = &db.config->security_options;
	memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));

	event_ڈیٹا.ڈیٹا.store_id = بنیاد_پیغام->ڈیٹا.store_id;
	event_ڈیٹا.ڈیٹا.expiry_وقت = بنیاد_پیغام->ڈیٹا.expiry_وقت;
	event_ڈیٹا.ڈیٹا.topic = بنیاد_پیغام->ڈیٹا.topic;
	event_ڈیٹا.ڈیٹا.payload = بنیاد_پیغام->ڈیٹا.payload;
	event_ڈیٹا.ڈیٹا.ماخذ_id = بنیاد_پیغام->ڈیٹا.ماخذ_id;
	event_ڈیٹا.ڈیٹا.ماخذ_username = بنیاد_پیغام->ڈیٹا.ماخذ_username;
	event_ڈیٹا.ڈیٹا.properties = بنیاد_پیغام->ڈیٹا.properties;
	event_ڈیٹا.ڈیٹا.payloadlen = بنیاد_پیغام->ڈیٹا.payloadlen;
	event_ڈیٹا.ڈیٹا.ماخذ_mid = بنیاد_پیغام->ڈیٹا.ماخذ_mid;
	اگر(بنیاد_پیغام->ماخذ_listener){
		event_ڈیٹا.ڈیٹا.ماخذ_پورٹ = بنیاد_پیغام->ماخذ_listener->پورٹ;
	}ورنہ{
		event_ڈیٹا.ڈیٹا.ماخذ_پورٹ = 0;
	}
	event_ڈیٹا.ڈیٹا.qos = بنیاد_پیغام->ڈیٹا.qos;
	event_ڈیٹا.ڈیٹا.retain = بنیاد_پیغام->ڈیٹا.retain;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_بنیاد_پیغام_جوڑو, cb_بنیاد, cb_اگلا){
		cb_بنیاد->cb(MOSQ_EVT_PERSIST_بنیاد_پیغام_جوڑو, &event_ڈیٹا, cb_بنیاد->userdata);
	}
	بنیاد_پیغام->stored = true;
}


خالی plugin_persist_handle_بنیاد_پیغام_delete(ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام)
{
	ساخت mosquitto_evt_persist_بنیاد_پیغام event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	ساخت mosquitto__security_options *opts;

	اگر(بنیاد_پیغام->stored == false || db.shutdown){
		واپس;
	}

	opts = &db.config->security_options;
	memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));

	event_ڈیٹا.ڈیٹا.store_id = بنیاد_پیغام->ڈیٹا.store_id;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_بنیاد_پیغام_delete, cb_بنیاد, cb_اگلا){
		cb_بنیاد->cb(MOSQ_EVT_PERSIST_بنیاد_پیغام_DELETE, &event_ڈیٹا, cb_بنیاد->userdata);
	}
	بنیاد_پیغام->stored = false;
}


خالی plugin_persist_handle_retain_پیغام_سیٹ(ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام)
{
	ساخت mosquitto_evt_persist_retain_پیغام event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	ساخت mosquitto__security_options *opts;

	اگر(db.shutdown){
		واپس;
	}

	opts = &db.config->security_options;
	memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));

	event_ڈیٹا.store_id = بنیاد_پیغام->ڈیٹا.store_id;
	event_ڈیٹا.topic = بنیاد_پیغام->ڈیٹا.topic;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_retain_پیغام_سیٹ, cb_بنیاد, cb_اگلا){
		cb_بنیاد->cb(MOSQ_EVT_PERSIST_RETAIN_پیغام_سیٹ, &event_ڈیٹا, cb_بنیاد->userdata);
	}
}


خالی plugin_persist_handle_retain_پیغام_delete(ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام)
{
	ساخت mosquitto_evt_persist_retain_پیغام event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	ساخت mosquitto__security_options *opts;

	اگر(db.shutdown){
		واپس;
	}

	opts = &db.config->security_options;
	memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));

	event_ڈیٹا.topic = بنیاد_پیغام->ڈیٹا.topic;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_retain_پیغام_delete, cb_بنیاد, cb_اگلا){
		cb_بنیاد->cb(MOSQ_EVT_PERSIST_RETAIN_پیغام_DELETE, &event_ڈیٹا, cb_بنیاد->userdata);
	}
}


خالی plugin_persist_handle_will_جوڑو(ساخت mosquitto *context)
{
	ساخت mosquitto_evt_persist_will_پیغام event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	ساخت mosquitto__security_options *opts;
	ساخت mosquitto_پیغام *will_پیغام;

	اگر(db.shutdown || !context->will){
		واپس;
	}

	opts = &db.config->security_options;
	will_پیغام = &context->will->پیغام;
	memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));
	event_ڈیٹا.ڈیٹا.clientid = context->id;
	event_ڈیٹا.ڈیٹا.topic = will_پیغام->topic;
	event_ڈیٹا.ڈیٹا.payload = will_پیغام->payload;
	event_ڈیٹا.ڈیٹا.payloadlen = (uint32_t)will_پیغام->payloadlen;
	event_ڈیٹا.ڈیٹا.qos = (uint8_t)will_پیغام->qos;
	event_ڈیٹا.ڈیٹا.retain = will_پیغام->retain;
	event_ڈیٹا.ڈیٹا.properties = context->will->properties;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_will_جوڑو, cb_بنیاد, cb_اگلا){
		cb_بنیاد->cb(MOSQ_EVT_PERSIST_WILL_جوڑو, &event_ڈیٹا, cb_بنیاد->userdata);
	}
}


خالی plugin_persist__handle_will_delete(ساخت mosquitto *context)
{
	ساخت mosquitto_evt_persist_will_پیغام event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	ساخت mosquitto__security_options *opts;

	memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));
	event_ڈیٹا.ڈیٹا.clientid = context->id;

	اگر(db.shutdown){
		واپس;
	}

	opts = &db.config->security_options;
	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_will_delete, cb_بنیاد, cb_اگلا){
		cb_بنیاد->cb(MOSQ_EVT_PERSIST_WILL_جوڑو, &event_ڈیٹا, cb_بنیاد->userdata);
	}

}
