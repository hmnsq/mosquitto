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
	بنية mosquitto_evt_persist_restore event_معلومات;
	بنية mosquitto__callback *cb_بنسټ, *cb_بل;
	بنية mosquitto__security_options *opts;

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));

	DL_FOREACH_خوندي(opts->plugin_callbacks.persist_restore, cb_بنسټ, cb_بل){
		cb_بنسټ->cb(MOSQ_EVT_PERSIST_RESTORE, &event_معلومات, cb_بنسټ->userdata);
	}
}


فراغ plugin_persist_handle_client_جمع_کړه(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_client event_معلومات;
	بنية mosquitto__callback *cb_بنسټ, *cb_بل;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown || context->is_persisted){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));
	event_معلومات.معلومات.clientid = context->id;
	event_معلومات.معلومات.username = context->username;
	event_معلومات.معلومات.auth_لاره = context->auth_لاره;
	event_معلومات.معلومات.will_delay_وخت = context->will_delay_وخت;
	event_معلومات.معلومات.session_expiry_وخت = context->session_expiry_وخت;
	event_معلومات.معلومات.will_delay_interval = context->will_delay_interval;
	event_معلومات.معلومات.session_expiry_interval = context->session_expiry_interval;
	إذا(context->listener){
		event_معلومات.معلومات.listener_پورټ = context->listener->پورټ;
	}وإلا{
		event_معلومات.معلومات.listener_پورټ = 0;
	}
	event_معلومات.معلومات.max_qos = context->max_qos;
	event_معلومات.معلومات.retain_شته = context->retain_شته;
	event_معلومات.معلومات.max_packet_اندازه = context->اعظمي_packet_اندازه;

	DL_FOREACH_خوندي(opts->plugin_callbacks.persist_client_جمع_کړه, cb_بنسټ, cb_بل){
		cb_بنسټ->cb(MOSQ_EVT_PERSIST_CLIENT_جمع_کړه, &event_معلومات, cb_بنسټ->userdata);
	}

	إذا(context->will){
		plugin_persist_handle_will_جمع_کړه(context);
	}

	context->is_persisted = true;
}


فراغ plugin_persist__handle_client_update(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_client event_معلومات;
	بنية mosquitto__callback *cb_بنسټ, *cb_بل;
	بنية mosquitto__security_options *opts;
	بنية mosquitto_پیغام_v5_2 will;

	ناکارول_شوی(will); /* FIXME */

	إذا(db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));
	event_معلومات.معلومات.clientid = context->id;
	event_معلومات.معلومات.username = context->username;
	event_معلومات.معلومات.auth_لاره = context->auth_لاره;
	event_معلومات.معلومات.will_delay_وخت = context->will_delay_وخت;
	event_معلومات.معلومات.session_expiry_وخت = context->session_expiry_وخت;
	event_معلومات.معلومات.will_delay_interval = context->will_delay_interval;
	event_معلومات.معلومات.session_expiry_interval = context->session_expiry_interval;
	إذا(context->listener){
		event_معلومات.معلومات.listener_پورټ = context->listener->پورټ;
	}وإلا{
		event_معلومات.معلومات.listener_پورټ = 0;
	}
	event_معلومات.معلومات.max_qos = context->max_qos;
	event_معلومات.معلومات.retain_شته = context->retain_شته;
	event_معلومات.معلومات.max_packet_اندازه = context->اعظمي_packet_اندازه;

	DL_FOREACH_خوندي(opts->plugin_callbacks.persist_client_update, cb_بنسټ, cb_بل){
		cb_بنسټ->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_معلومات, cb_بنسټ->userdata);
	}

	إذا(context->will){
		plugin_persist_handle_will_جمع_کړه(context);
	}وإلا{
		plugin_persist__handle_will_delete(context);
	}
}


فراغ plugin_persist__handle_client_delete(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_client event_معلومات;
	بنية mosquitto__callback *cb_بنسټ, *cb_بل;
	بنية mosquitto__security_options *opts;

	إذا(context->id == عدم
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		أرجع;
	}

	plugin_persist__handle_will_delete(context);

	إذا(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_سمدستي){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));
	event_معلومات.معلومات.clientid = context->id;

	DL_FOREACH_خوندي(opts->plugin_callbacks.persist_client_delete, cb_بنسټ, cb_بل){
		cb_بنسټ->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_معلومات, cb_بنسټ->userdata);
	}
	context->is_persisted = false;
}


فراغ plugin_persist_handle_subscription_جمع_کړه(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub)
{
	بنية mosquitto_evt_persist_subscription event_معلومات;
	بنية mosquitto__callback *cb_بنسټ, *cb_بل;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown || context->is_persisted == false){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));
	event_معلومات.معلومات.clientid = context->id;
	event_معلومات.معلومات.topic_filter = sub->topic_filter;
	event_معلومات.معلومات.identifier = sub->identifier;
	event_معلومات.معلومات.options = sub->options;

	DL_FOREACH_خوندي(opts->plugin_callbacks.persist_subscription_جمع_کړه, cb_بنسټ, cb_بل){
		cb_بنسټ->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_جمع_کړه, &event_معلومات, cb_بنسټ->userdata);
	}
}


فراغ plugin_persist__handle_subscription_delete(بنية mosquitto *context, حرف *sub)
{
	بنية mosquitto_evt_persist_subscription event_معلومات;
	بنية mosquitto__callback *cb_بنسټ, *cb_بل;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown || context->is_persisted == false){
		أرجع;
	}
	إذا(!sub){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));
	event_معلومات.معلومات.clientid = context->id;
	event_معلومات.معلومات.topic_filter = sub;

	DL_FOREACH_خوندي(opts->plugin_callbacks.persist_subscription_delete, cb_بنسټ, cb_بل){
		cb_بنسټ->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_معلومات, cb_بنسټ->userdata);
	}
}


ساكن مضمن فراغ سېټ_client_پیغام_event_معلومات(بنية mosquitto_evt_persist_client_پیغام *event_معلومات, بنية mosquitto *context, ثابت بنية mosquitto_client_پیغام *client_پیغام)
{
	event_معلومات->معلومات.clientid = context->id;
	event_معلومات->معلومات.cmsg_id = client_پیغام->معلومات.cmsg_id;
	event_معلومات->معلومات.direction = (uint8_t)client_پیغام->معلومات.direction;
	event_معلومات->معلومات.dup = client_پیغام->معلومات.dup;
	event_معلومات->معلومات.mid = client_پیغام->معلومات.mid;
	event_معلومات->معلومات.qos = client_پیغام->معلومات.qos;
	event_معلومات->معلومات.retain = client_پیغام->معلومات.retain;
	event_معلومات->معلومات.state = (uint8_t)client_پیغام->معلومات.state;
	event_معلومات->معلومات.store_id = client_پیغام->بنسټ_پیغام->معلومات.store_id;
	event_معلومات->معلومات.subscription_identifier = client_پیغام->معلومات.subscription_identifier;
}


فراغ plugin_persist_handle_client_پیغام_جمع_کړه(بنية mosquitto *context, ثابت بنية mosquitto_client_پیغام *client_پیغام)
{
	بنية mosquitto_evt_persist_client_پیغام event_معلومات;
	بنية mosquitto__callback *cb_بنسټ, *cb_بل;
	بنية mosquitto__security_options *opts;

	إذا(context->is_persisted == false
			|| (client_پیغام->معلومات.qos == 0 && db.config->کتار_qos0_messages == false)
			|| db.shutdown){

		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));

	سېټ_client_پیغام_event_معلومات(&event_معلومات, context, client_پیغام);

	DL_FOREACH_خوندي(opts->plugin_callbacks.persist_client_پیغام_جمع_کړه, cb_بنسټ, cb_بل){
		cb_بنسټ->cb(MOSQ_EVT_PERSIST_CLIENT_پیغام_جمع_کړه, &event_معلومات, cb_بنسټ->userdata);
	}
}


فراغ plugin_persist_handle_client_پیغام_delete(بنية mosquitto *context, ثابت بنية mosquitto_client_پیغام *client_پیغام)
{
	بنية mosquitto_evt_persist_client_پیغام event_معلومات;
	بنية mosquitto__callback *cb_بنسټ, *cb_بل;
	بنية mosquitto__security_options *opts;

	إذا(context->is_persisted == false
			|| (client_پیغام->معلومات.qos == 0 && db.config->کتار_qos0_messages == false)
			|| db.shutdown){

		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));

	سېټ_client_پیغام_event_معلومات(&event_معلومات, context, client_پیغام);

	DL_FOREACH_خوندي(opts->plugin_callbacks.persist_client_پیغام_delete, cb_بنسټ, cb_بل){
		cb_بنسټ->cb(MOSQ_EVT_PERSIST_CLIENT_پیغام_DELETE, &event_معلومات, cb_بنسټ->userdata);
	}
}


فراغ plugin_persist_handle_client_پیغام_update(بنية mosquitto *context, ثابت بنية mosquitto_client_پیغام *client_پیغام)
{
	بنية mosquitto_evt_persist_client_پیغام event_معلومات;
	بنية mosquitto__callback *cb_بنسټ, *cb_بل;
	بنية mosquitto__security_options *opts;

	إذا(context->is_persisted == false
			|| (client_پیغام->معلومات.qos == 0 && db.config->کتار_qos0_messages == false)
			|| db.shutdown){

		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));

	سېټ_client_پیغام_event_معلومات(&event_معلومات, context, client_پیغام);

	DL_FOREACH_خوندي(opts->plugin_callbacks.persist_client_پیغام_update, cb_بنسټ, cb_بل){
		cb_بنسټ->cb(MOSQ_EVT_PERSIST_CLIENT_پیغام_UPDATE, &event_معلومات, cb_بنسټ->userdata);
	}
}


فراغ plugin_persist_handle_بنسټ_پیغام_جمع_کړه(بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام)
{
	بنية mosquitto_evt_persist_بنسټ_پیغام event_معلومات;
	بنية mosquitto__callback *cb_بنسټ, *cb_بل;
	بنية mosquitto__security_options *opts;

	إذا(بنسټ_پیغام->stored || db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));

	event_معلومات.معلومات.store_id = بنسټ_پیغام->معلومات.store_id;
	event_معلومات.معلومات.expiry_وخت = بنسټ_پیغام->معلومات.expiry_وخت;
	event_معلومات.معلومات.topic = بنسټ_پیغام->معلومات.topic;
	event_معلومات.معلومات.payload = بنسټ_پیغام->معلومات.payload;
	event_معلومات.معلومات.سرچینه_id = بنسټ_پیغام->معلومات.سرچینه_id;
	event_معلومات.معلومات.سرچینه_username = بنسټ_پیغام->معلومات.سرچینه_username;
	event_معلومات.معلومات.properties = بنسټ_پیغام->معلومات.properties;
	event_معلومات.معلومات.payloadlen = بنسټ_پیغام->معلومات.payloadlen;
	event_معلومات.معلومات.سرچینه_mid = بنسټ_پیغام->معلومات.سرچینه_mid;
	إذا(بنسټ_پیغام->سرچینه_listener){
		event_معلومات.معلومات.سرچینه_پورټ = بنسټ_پیغام->سرچینه_listener->پورټ;
	}وإلا{
		event_معلومات.معلومات.سرچینه_پورټ = 0;
	}
	event_معلومات.معلومات.qos = بنسټ_پیغام->معلومات.qos;
	event_معلومات.معلومات.retain = بنسټ_پیغام->معلومات.retain;

	DL_FOREACH_خوندي(opts->plugin_callbacks.persist_بنسټ_پیغام_جمع_کړه, cb_بنسټ, cb_بل){
		cb_بنسټ->cb(MOSQ_EVT_PERSIST_بنسټ_پیغام_جمع_کړه, &event_معلومات, cb_بنسټ->userdata);
	}
	بنسټ_پیغام->stored = true;
}


فراغ plugin_persist_handle_بنسټ_پیغام_delete(بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام)
{
	بنية mosquitto_evt_persist_بنسټ_پیغام event_معلومات;
	بنية mosquitto__callback *cb_بنسټ, *cb_بل;
	بنية mosquitto__security_options *opts;

	إذا(بنسټ_پیغام->stored == false || db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));

	event_معلومات.معلومات.store_id = بنسټ_پیغام->معلومات.store_id;

	DL_FOREACH_خوندي(opts->plugin_callbacks.persist_بنسټ_پیغام_delete, cb_بنسټ, cb_بل){
		cb_بنسټ->cb(MOSQ_EVT_PERSIST_بنسټ_پیغام_DELETE, &event_معلومات, cb_بنسټ->userdata);
	}
	بنسټ_پیغام->stored = false;
}


فراغ plugin_persist_handle_retain_پیغام_سېټ(بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام)
{
	بنية mosquitto_evt_persist_retain_پیغام event_معلومات;
	بنية mosquitto__callback *cb_بنسټ, *cb_بل;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));

	event_معلومات.store_id = بنسټ_پیغام->معلومات.store_id;
	event_معلومات.topic = بنسټ_پیغام->معلومات.topic;

	DL_FOREACH_خوندي(opts->plugin_callbacks.persist_retain_پیغام_سېټ, cb_بنسټ, cb_بل){
		cb_بنسټ->cb(MOSQ_EVT_PERSIST_RETAIN_پیغام_سېټ, &event_معلومات, cb_بنسټ->userdata);
	}
}


فراغ plugin_persist_handle_retain_پیغام_delete(بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام)
{
	بنية mosquitto_evt_persist_retain_پیغام event_معلومات;
	بنية mosquitto__callback *cb_بنسټ, *cb_بل;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));

	event_معلومات.topic = بنسټ_پیغام->معلومات.topic;

	DL_FOREACH_خوندي(opts->plugin_callbacks.persist_retain_پیغام_delete, cb_بنسټ, cb_بل){
		cb_بنسټ->cb(MOSQ_EVT_PERSIST_RETAIN_پیغام_DELETE, &event_معلومات, cb_بنسټ->userdata);
	}
}


فراغ plugin_persist_handle_will_جمع_کړه(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_will_پیغام event_معلومات;
	بنية mosquitto__callback *cb_بنسټ, *cb_بل;
	بنية mosquitto__security_options *opts;
	بنية mosquitto_پیغام *will_پیغام;

	إذا(db.shutdown || !context->will){
		أرجع;
	}

	opts = &db.config->security_options;
	will_پیغام = &context->will->پیغام;
	memset(&event_معلومات, 0, حجم(event_معلومات));
	event_معلومات.معلومات.clientid = context->id;
	event_معلومات.معلومات.topic = will_پیغام->topic;
	event_معلومات.معلومات.payload = will_پیغام->payload;
	event_معلومات.معلومات.payloadlen = (uint32_t)will_پیغام->payloadlen;
	event_معلومات.معلومات.qos = (uint8_t)will_پیغام->qos;
	event_معلومات.معلومات.retain = will_پیغام->retain;
	event_معلومات.معلومات.properties = context->will->properties;

	DL_FOREACH_خوندي(opts->plugin_callbacks.persist_will_جمع_کړه, cb_بنسټ, cb_بل){
		cb_بنسټ->cb(MOSQ_EVT_PERSIST_WILL_جمع_کړه, &event_معلومات, cb_بنسټ->userdata);
	}
}


فراغ plugin_persist__handle_will_delete(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_will_پیغام event_معلومات;
	بنية mosquitto__callback *cb_بنسټ, *cb_بل;
	بنية mosquitto__security_options *opts;

	memset(&event_معلومات, 0, حجم(event_معلومات));
	event_معلومات.معلومات.clientid = context->id;

	إذا(db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	DL_FOREACH_خوندي(opts->plugin_callbacks.persist_will_delete, cb_بنسټ, cb_بل){
		cb_بنسټ->cb(MOSQ_EVT_PERSIST_WILL_جمع_کړه, &event_معلومات, cb_بنسټ->userdata);
	}

}
