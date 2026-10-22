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


تهی plugin_persist__handle_restore(تهی)
{
	ساختار mosquitto_evt_persist_restore event_data;
	ساختار mosquitto__callback *cb_base, *cb_next;
	ساختار mosquitto__security_options *opts;

	opts = &db.config->security_options;
	memset(&event_data, 0, اندازه(event_data));

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_restore, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RESTORE, &event_data, cb_base->userdata);
	}
}


تهی plugin_persist__handle_client_add(ساختار mosquitto *context)
{
	ساختار mosquitto_evt_persist_client event_data;
	ساختار mosquitto__callback *cb_base, *cb_next;
	ساختار mosquitto__security_options *opts;

	اگر(db.shutdown || context->is_persisted){
		بازگشت;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, اندازه(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	اگر(context->listener){
		event_data.data.listener_port = context->listener->port;
	}وگرنه{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ADD, &event_data, cb_base->userdata);
	}

	اگر(context->will){
		plugin_persist__handle_will_add(context);
	}

	context->is_persisted = true;
}


تهی plugin_persist__handle_client_update(ساختار mosquitto *context)
{
	ساختار mosquitto_evt_persist_client event_data;
	ساختار mosquitto__callback *cb_base, *cb_next;
	ساختار mosquitto__security_options *opts;
	ساختار mosquitto_message_v5 will;

	UNUSED(will); /* FIXME */

	اگر(db.shutdown){
		بازگشت;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, اندازه(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	اگر(context->listener){
		event_data.data.listener_port = context->listener->port;
	}وگرنه{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_data, cb_base->userdata);
	}

	اگر(context->will){
		plugin_persist__handle_will_add(context);
	}وگرنه{
		plugin_persist__handle_will_delete(context);
	}
}


تهی plugin_persist__handle_client_delete(ساختار mosquitto *context)
{
	ساختار mosquitto_evt_persist_client event_data;
	ساختار mosquitto__callback *cb_base, *cb_next;
	ساختار mosquitto__security_options *opts;

	اگر(context->id == پوچ
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		بازگشت;
	}

	plugin_persist__handle_will_delete(context);

	اگر(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		بازگشت;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, اندازه(event_data));
	event_data.data.clientid = context->id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_data, cb_base->userdata);
	}
	context->is_persisted = false;
}


تهی plugin_persist__handle_subscription_add(ساختار mosquitto *context, ثابت ساختار mosquitto_subscription *sub)
{
	ساختار mosquitto_evt_persist_subscription event_data;
	ساختار mosquitto__callback *cb_base, *cb_next;
	ساختار mosquitto__security_options *opts;

	اگر(db.shutdown || context->is_persisted == false){
		بازگشت;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, اندازه(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub->topic_filter;
	event_data.data.identifier = sub->identifier;
	event_data.data.options = sub->options;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD, &event_data, cb_base->userdata);
	}
}


تهی plugin_persist__handle_subscription_delete(ساختار mosquitto *context, نویسه *sub)
{
	ساختار mosquitto_evt_persist_subscription event_data;
	ساختار mosquitto__callback *cb_base, *cb_next;
	ساختار mosquitto__security_options *opts;

	اگر(db.shutdown || context->is_persisted == false){
		بازگشت;
	}
	اگر(!sub){
		بازگشت;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, اندازه(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_data, cb_base->userdata);
	}
}


ایستا درونخطی تهی set_client_پیام_event_data(ساختار mosquitto_evt_persist_client_پیام *event_data, ساختار mosquitto *context, ثابت ساختار mosquitto_client_پیام *client_پیام)
{
	event_data->data.clientid = context->id;
	event_data->data.cmsg_id = client_پیام->data.cmsg_id;
	event_data->data.direction = (uint8_t)client_پیام->data.direction;
	event_data->data.dup = client_پیام->data.dup;
	event_data->data.mid = client_پیام->data.mid;
	event_data->data.qos = client_پیام->data.qos;
	event_data->data.retain = client_پیام->data.retain;
	event_data->data.state = (uint8_t)client_پیام->data.state;
	event_data->data.store_id = client_پیام->base_پیام->data.store_id;
	event_data->data.subscription_identifier = client_پیام->data.subscription_identifier;
}


تهی plugin_persist_handle_client_پیام_add(ساختار mosquitto *context, ثابت ساختار mosquitto_client_پیام *client_پیام)
{
	ساختار mosquitto_evt_persist_client_پیام event_data;
	ساختار mosquitto__callback *cb_base, *cb_next;
	ساختار mosquitto__security_options *opts;

	اگر(context->is_persisted == false
			|| (client_پیام->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		بازگشت;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, اندازه(event_data));

	set_client_پیام_event_data(&event_data, context, client_پیام);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_پیام_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_پیام_ADD, &event_data, cb_base->userdata);
	}
}


تهی plugin_persist_handle_client_پیام_delete(ساختار mosquitto *context, ثابت ساختار mosquitto_client_پیام *client_پیام)
{
	ساختار mosquitto_evt_persist_client_پیام event_data;
	ساختار mosquitto__callback *cb_base, *cb_next;
	ساختار mosquitto__security_options *opts;

	اگر(context->is_persisted == false
			|| (client_پیام->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		بازگشت;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, اندازه(event_data));

	set_client_پیام_event_data(&event_data, context, client_پیام);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_پیام_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_پیام_DELETE, &event_data, cb_base->userdata);
	}
}


تهی plugin_persist_handle_client_پیام_update(ساختار mosquitto *context, ثابت ساختار mosquitto_client_پیام *client_پیام)
{
	ساختار mosquitto_evt_persist_client_پیام event_data;
	ساختار mosquitto__callback *cb_base, *cb_next;
	ساختار mosquitto__security_options *opts;

	اگر(context->is_persisted == false
			|| (client_پیام->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		بازگشت;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, اندازه(event_data));

	set_client_پیام_event_data(&event_data, context, client_پیام);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_پیام_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_پیام_UPDATE, &event_data, cb_base->userdata);
	}
}


تهی plugin_persist_handle_base_پیام_add(ساختار mosquitto_base_پیام *base_پیام)
{
	ساختار mosquitto_evt_persist_base_پیام event_data;
	ساختار mosquitto__callback *cb_base, *cb_next;
	ساختار mosquitto__security_options *opts;

	اگر(base_پیام->stored || db.shutdown){
		بازگشت;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, اندازه(event_data));

	event_data.data.store_id = base_پیام->data.store_id;
	event_data.data.expiry_time = base_پیام->data.expiry_time;
	event_data.data.topic = base_پیام->data.topic;
	event_data.data.payload = base_پیام->data.payload;
	event_data.data.source_id = base_پیام->data.source_id;
	event_data.data.source_username = base_پیام->data.source_username;
	event_data.data.properties = base_پیام->data.properties;
	event_data.data.payloadlen = base_پیام->data.payloadlen;
	event_data.data.source_mid = base_پیام->data.source_mid;
	اگر(base_پیام->source_listener){
		event_data.data.source_port = base_پیام->source_listener->port;
	}وگرنه{
		event_data.data.source_port = 0;
	}
	event_data.data.qos = base_پیام->data.qos;
	event_data.data.retain = base_پیام->data.retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_پیام_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_پیام_ADD, &event_data, cb_base->userdata);
	}
	base_پیام->stored = true;
}


تهی plugin_persist_handle_base_پیام_delete(ساختار mosquitto_base_پیام *base_پیام)
{
	ساختار mosquitto_evt_persist_base_پیام event_data;
	ساختار mosquitto__callback *cb_base, *cb_next;
	ساختار mosquitto__security_options *opts;

	اگر(base_پیام->stored == false || db.shutdown){
		بازگشت;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, اندازه(event_data));

	event_data.data.store_id = base_پیام->data.store_id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_پیام_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_پیام_DELETE, &event_data, cb_base->userdata);
	}
	base_پیام->stored = false;
}


تهی plugin_persist_handle_retain_پیام_set(ساختار mosquitto_base_پیام *base_پیام)
{
	ساختار mosquitto_evt_persist_retain_پیام event_data;
	ساختار mosquitto__callback *cb_base, *cb_next;
	ساختار mosquitto__security_options *opts;

	اگر(db.shutdown){
		بازگشت;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, اندازه(event_data));

	event_data.store_id = base_پیام->data.store_id;
	event_data.topic = base_پیام->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_پیام_set, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_پیام_SET, &event_data, cb_base->userdata);
	}
}


تهی plugin_persist_handle_retain_پیام_delete(ساختار mosquitto_base_پیام *base_پیام)
{
	ساختار mosquitto_evt_persist_retain_پیام event_data;
	ساختار mosquitto__callback *cb_base, *cb_next;
	ساختار mosquitto__security_options *opts;

	اگر(db.shutdown){
		بازگشت;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, اندازه(event_data));

	event_data.topic = base_پیام->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_پیام_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_پیام_DELETE, &event_data, cb_base->userdata);
	}
}


تهی plugin_persist__handle_will_add(ساختار mosquitto *context)
{
	ساختار mosquitto_evt_persist_will_پیام event_data;
	ساختار mosquitto__callback *cb_base, *cb_next;
	ساختار mosquitto__security_options *opts;
	ساختار mosquitto_message *will_پیام;

	اگر(db.shutdown || !context->will){
		بازگشت;
	}

	opts = &db.config->security_options;
	will_پیام = &context->will->پیام;
	memset(&event_data, 0, اندازه(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic = will_پیام->topic;
	event_data.data.payload = will_پیام->payload;
	event_data.data.payloadlen = (uint32_t)will_پیام->payloadlen;
	event_data.data.qos = (uint8_t)will_پیام->qos;
	event_data.data.retain = will_پیام->retain;
	event_data.data.properties = context->will->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}
}


تهی plugin_persist__handle_will_delete(ساختار mosquitto *context)
{
	ساختار mosquitto_evt_persist_will_پیام event_data;
	ساختار mosquitto__callback *cb_base, *cb_next;
	ساختار mosquitto__security_options *opts;

	memset(&event_data, 0, اندازه(event_data));
	event_data.data.clientid = context->id;

	اگر(db.shutdown){
		بازگشت;
	}

	opts = &db.config->security_options;
	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}

}
