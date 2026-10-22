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

#ضمن "config.h"

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto_internal.h"
#ضمن "mosquitto/broker.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "send_mosq.h"
#ضمن "util_mosq.h"
#ضمن "utlist.h"
#ضمن "lib_load.h"
#ضمن "will_mosq.h"
#ضمن <stdint.h>


فراغ plugin_persist__handle_restore(فراغ)
{
	بنية mosquitto_evt_persist_restore event_داده;
	بنية mosquitto__callback *cb_پایه, *cb_بعدی;
	بنية mosquitto__security_options *opts;

	opts = &db.config->security_options;
	memset(&event_داده, 0, حجم(event_داده));

	DL_FOREACH_امن(opts->plugin_callbacks.persist_restore, cb_پایه, cb_بعدی){
		cb_پایه->cb(MOSQ_EVT_PERSIST_RESTORE, &event_داده, cb_پایه->userdata);
	}
}


فراغ plugin_persist_handle_client_جمع_کن(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_client event_داده;
	بنية mosquitto__callback *cb_پایه, *cb_بعدی;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown || context->is_persisted){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_داده, 0, حجم(event_داده));
	event_داده.داده.clientid = context->id;
	event_داده.داده.username = context->username;
	event_داده.داده.auth_روش = context->auth_روش;
	event_داده.داده.will_delay_زمان = context->will_delay_زمان;
	event_داده.داده.session_expiry_زمان = context->session_expiry_زمان;
	event_داده.داده.will_delay_interval = context->will_delay_interval;
	event_داده.داده.session_expiry_interval = context->session_expiry_interval;
	إذا(context->listener){
		event_داده.داده.listener_پورت = context->listener->پورت;
	}وإلا{
		event_داده.داده.listener_پورت = 0;
	}
	event_داده.داده.max_qos = context->max_qos;
	event_داده.داده.retain_موجود = context->retain_موجود;
	event_داده.داده.max_packet_اندازه = context->بیشینه_packet_اندازه;

	DL_FOREACH_امن(opts->plugin_callbacks.persist_client_جمع_کن, cb_پایه, cb_بعدی){
		cb_پایه->cb(MOSQ_EVT_PERSIST_CLIENT_جمع_کن, &event_داده, cb_پایه->userdata);
	}

	إذا(context->will){
		plugin_persist_handle_will_جمع_کن(context);
	}

	context->is_persisted = true;
}


فراغ plugin_persist__handle_client_update(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_client event_داده;
	بنية mosquitto__callback *cb_پایه, *cb_بعدی;
	بنية mosquitto__security_options *opts;
	بنية mosquitto_پیام_v5_2 will;

	استفادهنشده(will); /* FIXME */

	إذا(db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_داده, 0, حجم(event_داده));
	event_داده.داده.clientid = context->id;
	event_داده.داده.username = context->username;
	event_داده.داده.auth_روش = context->auth_روش;
	event_داده.داده.will_delay_زمان = context->will_delay_زمان;
	event_داده.داده.session_expiry_زمان = context->session_expiry_زمان;
	event_داده.داده.will_delay_interval = context->will_delay_interval;
	event_داده.داده.session_expiry_interval = context->session_expiry_interval;
	إذا(context->listener){
		event_داده.داده.listener_پورت = context->listener->پورت;
	}وإلا{
		event_داده.داده.listener_پورت = 0;
	}
	event_داده.داده.max_qos = context->max_qos;
	event_داده.داده.retain_موجود = context->retain_موجود;
	event_داده.داده.max_packet_اندازه = context->بیشینه_packet_اندازه;

	DL_FOREACH_امن(opts->plugin_callbacks.persist_client_update, cb_پایه, cb_بعدی){
		cb_پایه->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_داده, cb_پایه->userdata);
	}

	إذا(context->will){
		plugin_persist_handle_will_جمع_کن(context);
	}وإلا{
		plugin_persist__handle_will_delete(context);
	}
}


فراغ plugin_persist__handle_client_delete(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_client event_داده;
	بنية mosquitto__callback *cb_پایه, *cb_بعدی;
	بنية mosquitto__security_options *opts;

	إذا(context->id == عدم
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		أرجع;
	}

	plugin_persist__handle_will_delete(context);

	إذا(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوری){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_داده, 0, حجم(event_داده));
	event_داده.داده.clientid = context->id;

	DL_FOREACH_امن(opts->plugin_callbacks.persist_client_delete, cb_پایه, cb_بعدی){
		cb_پایه->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_داده, cb_پایه->userdata);
	}
	context->is_persisted = false;
}


فراغ plugin_persist_handle_subscription_جمع_کن(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub)
{
	بنية mosquitto_evt_persist_subscription event_داده;
	بنية mosquitto__callback *cb_پایه, *cb_بعدی;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown || context->is_persisted == false){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_داده, 0, حجم(event_داده));
	event_داده.داده.clientid = context->id;
	event_داده.داده.topic_filter = sub->topic_filter;
	event_داده.داده.identifier = sub->identifier;
	event_داده.داده.options = sub->options;

	DL_FOREACH_امن(opts->plugin_callbacks.persist_subscription_جمع_کن, cb_پایه, cb_بعدی){
		cb_پایه->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_جمع_کن, &event_داده, cb_پایه->userdata);
	}
}


فراغ plugin_persist__handle_subscription_delete(بنية mosquitto *context, حرف *sub)
{
	بنية mosquitto_evt_persist_subscription event_داده;
	بنية mosquitto__callback *cb_پایه, *cb_بعدی;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown || context->is_persisted == false){
		أرجع;
	}
	إذا(!sub){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_داده, 0, حجم(event_داده));
	event_داده.داده.clientid = context->id;
	event_داده.داده.topic_filter = sub;

	DL_FOREACH_امن(opts->plugin_callbacks.persist_subscription_delete, cb_پایه, cb_بعدی){
		cb_پایه->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_داده, cb_پایه->userdata);
	}
}


ساكن مضمن فراغ مجموعه_client_پیام_event_داده(بنية mosquitto_evt_persist_client_پیام *event_داده, بنية mosquitto *context, ثابت بنية mosquitto_client_پیام *client_پیام)
{
	event_داده->داده.clientid = context->id;
	event_داده->داده.cmsg_id = client_پیام->داده.cmsg_id;
	event_داده->داده.direction = (uint8_t)client_پیام->داده.direction;
	event_داده->داده.dup = client_پیام->داده.dup;
	event_داده->داده.mid = client_پیام->داده.mid;
	event_داده->داده.qos = client_پیام->داده.qos;
	event_داده->داده.retain = client_پیام->داده.retain;
	event_داده->داده.state = (uint8_t)client_پیام->داده.state;
	event_داده->داده.store_id = client_پیام->پایه_پیام->داده.store_id;
	event_داده->داده.subscription_identifier = client_پیام->داده.subscription_identifier;
}


فراغ plugin_persist_handle_client_پیام_جمع_کن(بنية mosquitto *context, ثابت بنية mosquitto_client_پیام *client_پیام)
{
	بنية mosquitto_evt_persist_client_پیام event_داده;
	بنية mosquitto__callback *cb_پایه, *cb_بعدی;
	بنية mosquitto__security_options *opts;

	إذا(context->is_persisted == false
			|| (client_پیام->داده.qos == 0 && db.config->صف_qos0_messages == false)
			|| db.shutdown){

		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_داده, 0, حجم(event_داده));

	مجموعه_client_پیام_event_داده(&event_داده, context, client_پیام);

	DL_FOREACH_امن(opts->plugin_callbacks.persist_client_پیام_جمع_کن, cb_پایه, cb_بعدی){
		cb_پایه->cb(MOSQ_EVT_PERSIST_CLIENT_پیام_جمع_کن, &event_داده, cb_پایه->userdata);
	}
}


فراغ plugin_persist_handle_client_پیام_delete(بنية mosquitto *context, ثابت بنية mosquitto_client_پیام *client_پیام)
{
	بنية mosquitto_evt_persist_client_پیام event_داده;
	بنية mosquitto__callback *cb_پایه, *cb_بعدی;
	بنية mosquitto__security_options *opts;

	إذا(context->is_persisted == false
			|| (client_پیام->داده.qos == 0 && db.config->صف_qos0_messages == false)
			|| db.shutdown){

		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_داده, 0, حجم(event_داده));

	مجموعه_client_پیام_event_داده(&event_داده, context, client_پیام);

	DL_FOREACH_امن(opts->plugin_callbacks.persist_client_پیام_delete, cb_پایه, cb_بعدی){
		cb_پایه->cb(MOSQ_EVT_PERSIST_CLIENT_پیام_DELETE, &event_داده, cb_پایه->userdata);
	}
}


فراغ plugin_persist_handle_client_پیام_update(بنية mosquitto *context, ثابت بنية mosquitto_client_پیام *client_پیام)
{
	بنية mosquitto_evt_persist_client_پیام event_داده;
	بنية mosquitto__callback *cb_پایه, *cb_بعدی;
	بنية mosquitto__security_options *opts;

	إذا(context->is_persisted == false
			|| (client_پیام->داده.qos == 0 && db.config->صف_qos0_messages == false)
			|| db.shutdown){

		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_داده, 0, حجم(event_داده));

	مجموعه_client_پیام_event_داده(&event_داده, context, client_پیام);

	DL_FOREACH_امن(opts->plugin_callbacks.persist_client_پیام_update, cb_پایه, cb_بعدی){
		cb_پایه->cb(MOSQ_EVT_PERSIST_CLIENT_پیام_UPDATE, &event_داده, cb_پایه->userdata);
	}
}


فراغ plugin_persist_handle_پایه_پیام_جمع_کن(بنية mosquitto_پایه_پیام *پایه_پیام)
{
	بنية mosquitto_evt_persist_پایه_پیام event_داده;
	بنية mosquitto__callback *cb_پایه, *cb_بعدی;
	بنية mosquitto__security_options *opts;

	إذا(پایه_پیام->stored || db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_داده, 0, حجم(event_داده));

	event_داده.داده.store_id = پایه_پیام->داده.store_id;
	event_داده.داده.expiry_زمان = پایه_پیام->داده.expiry_زمان;
	event_داده.داده.topic = پایه_پیام->داده.topic;
	event_داده.داده.payload = پایه_پیام->داده.payload;
	event_داده.داده.منبع_id = پایه_پیام->داده.منبع_id;
	event_داده.داده.منبع_username = پایه_پیام->داده.منبع_username;
	event_داده.داده.properties = پایه_پیام->داده.properties;
	event_داده.داده.payloadlen = پایه_پیام->داده.payloadlen;
	event_داده.داده.منبع_mid = پایه_پیام->داده.منبع_mid;
	إذا(پایه_پیام->منبع_listener){
		event_داده.داده.منبع_پورت = پایه_پیام->منبع_listener->پورت;
	}وإلا{
		event_داده.داده.منبع_پورت = 0;
	}
	event_داده.داده.qos = پایه_پیام->داده.qos;
	event_داده.داده.retain = پایه_پیام->داده.retain;

	DL_FOREACH_امن(opts->plugin_callbacks.persist_پایه_پیام_جمع_کن, cb_پایه, cb_بعدی){
		cb_پایه->cb(MOSQ_EVT_PERSIST_پایه_پیام_جمع_کن, &event_داده, cb_پایه->userdata);
	}
	پایه_پیام->stored = true;
}


فراغ plugin_persist_handle_پایه_پیام_delete(بنية mosquitto_پایه_پیام *پایه_پیام)
{
	بنية mosquitto_evt_persist_پایه_پیام event_داده;
	بنية mosquitto__callback *cb_پایه, *cb_بعدی;
	بنية mosquitto__security_options *opts;

	إذا(پایه_پیام->stored == false || db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_داده, 0, حجم(event_داده));

	event_داده.داده.store_id = پایه_پیام->داده.store_id;

	DL_FOREACH_امن(opts->plugin_callbacks.persist_پایه_پیام_delete, cb_پایه, cb_بعدی){
		cb_پایه->cb(MOSQ_EVT_PERSIST_پایه_پیام_DELETE, &event_داده, cb_پایه->userdata);
	}
	پایه_پیام->stored = false;
}


فراغ plugin_persist_handle_retain_پیام_مجموعه(بنية mosquitto_پایه_پیام *پایه_پیام)
{
	بنية mosquitto_evt_persist_retain_پیام event_داده;
	بنية mosquitto__callback *cb_پایه, *cb_بعدی;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_داده, 0, حجم(event_داده));

	event_داده.store_id = پایه_پیام->داده.store_id;
	event_داده.topic = پایه_پیام->داده.topic;

	DL_FOREACH_امن(opts->plugin_callbacks.persist_retain_پیام_مجموعه, cb_پایه, cb_بعدی){
		cb_پایه->cb(MOSQ_EVT_PERSIST_RETAIN_پیام_مجموعه, &event_داده, cb_پایه->userdata);
	}
}


فراغ plugin_persist_handle_retain_پیام_delete(بنية mosquitto_پایه_پیام *پایه_پیام)
{
	بنية mosquitto_evt_persist_retain_پیام event_داده;
	بنية mosquitto__callback *cb_پایه, *cb_بعدی;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_داده, 0, حجم(event_داده));

	event_داده.topic = پایه_پیام->داده.topic;

	DL_FOREACH_امن(opts->plugin_callbacks.persist_retain_پیام_delete, cb_پایه, cb_بعدی){
		cb_پایه->cb(MOSQ_EVT_PERSIST_RETAIN_پیام_DELETE, &event_داده, cb_پایه->userdata);
	}
}


فراغ plugin_persist_handle_will_جمع_کن(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_will_پیام event_داده;
	بنية mosquitto__callback *cb_پایه, *cb_بعدی;
	بنية mosquitto__security_options *opts;
	بنية mosquitto_پیام *will_پیام;

	إذا(db.shutdown || !context->will){
		أرجع;
	}

	opts = &db.config->security_options;
	will_پیام = &context->will->پیام;
	memset(&event_داده, 0, حجم(event_داده));
	event_داده.داده.clientid = context->id;
	event_داده.داده.topic = will_پیام->topic;
	event_داده.داده.payload = will_پیام->payload;
	event_داده.داده.payloadlen = (uint32_t)will_پیام->payloadlen;
	event_داده.داده.qos = (uint8_t)will_پیام->qos;
	event_داده.داده.retain = will_پیام->retain;
	event_داده.داده.properties = context->will->properties;

	DL_FOREACH_امن(opts->plugin_callbacks.persist_will_جمع_کن, cb_پایه, cb_بعدی){
		cb_پایه->cb(MOSQ_EVT_PERSIST_WILL_جمع_کن, &event_داده, cb_پایه->userdata);
	}
}


فراغ plugin_persist__handle_will_delete(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_will_پیام event_داده;
	بنية mosquitto__callback *cb_پایه, *cb_بعدی;
	بنية mosquitto__security_options *opts;

	memset(&event_داده, 0, حجم(event_داده));
	event_داده.داده.clientid = context->id;

	إذا(db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	DL_FOREACH_امن(opts->plugin_callbacks.persist_will_delete, cb_پایه, cb_بعدی){
		cb_پایه->cb(MOSQ_EVT_PERSIST_WILL_جمع_کن, &event_داده, cb_پایه->userdata);
	}

}
