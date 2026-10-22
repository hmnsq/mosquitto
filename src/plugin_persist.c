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
	بنية mosquitto_evt_persist_restore event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_restore, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RESTORE, &event_data, cb_base->userdata);
	}
}


فراغ plugin_persist__handle_client_add(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_client event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	אם(db.shutdown || context->is_persisted){
		החזר;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	אם(context->listener){
		event_data.data.listener_port = context->listener->port;
	}אחרת{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ADD, &event_data, cb_base->userdata);
	}

	אם(context->will){
		plugin_persist__handle_will_add(context);
	}

	context->is_persisted = true;
}


فراغ plugin_persist__handle_client_update(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_client event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;
	بنية mosquitto_message_v5 will;

	UNUSED(will); /* FIXME */

	אם(db.shutdown){
		החזר;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	אם(context->listener){
		event_data.data.listener_port = context->listener->port;
	}אחרת{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_data, cb_base->userdata);
	}

	אם(context->will){
		plugin_persist__handle_will_add(context);
	}אחרת{
		plugin_persist__handle_will_delete(context);
	}
}


فراغ plugin_persist__handle_client_delete(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_client event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	אם(context->id == عدم
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		החזר;
	}

	plugin_persist__handle_will_delete(context);

	אם(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		החזר;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));
	event_data.data.clientid = context->id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_data, cb_base->userdata);
	}
	context->is_persisted = false;
}


فراغ plugin_persist__handle_subscription_add(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub)
{
	بنية mosquitto_evt_persist_subscription event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	אם(db.shutdown || context->is_persisted == false){
		החזר;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub->topic_filter;
	event_data.data.identifier = sub->identifier;
	event_data.data.options = sub->options;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD, &event_data, cb_base->userdata);
	}
}


فراغ plugin_persist__handle_subscription_delete(بنية mosquitto *context, حرف *sub)
{
	بنية mosquitto_evt_persist_subscription event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	אם(db.shutdown || context->is_persisted == false){
		החזר;
	}
	אם(!sub){
		החזר;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_data, cb_base->userdata);
	}
}


ساكن مضمن فراغ set_client_msg_event_data(بنية mosquitto_evt_persist_client_msg *event_data, بنية mosquitto *context, ثابت بنية mosquitto__client_msg *client_msg)
{
	event_data->data.clientid = context->id;
	event_data->data.cmsg_id = client_msg->data.cmsg_id;
	event_data->data.direction = (uint8_t)client_msg->data.direction;
	event_data->data.dup = client_msg->data.dup;
	event_data->data.mid = client_msg->data.mid;
	event_data->data.qos = client_msg->data.qos;
	event_data->data.retain = client_msg->data.retain;
	event_data->data.state = (uint8_t)client_msg->data.state;
	event_data->data.store_id = client_msg->base_msg->data.store_id;
	event_data->data.subscription_identifier = client_msg->data.subscription_identifier;
}


فراغ plugin_persist__handle_client_msg_add(بنية mosquitto *context, ثابت بنية mosquitto__client_msg *client_msg)
{
	بنية mosquitto_evt_persist_client_msg event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	אם(context->is_persisted == false
			|| (client_msg->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		החזר;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));

	set_client_msg_event_data(&event_data, context, client_msg);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_msg_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_MSG_ADD, &event_data, cb_base->userdata);
	}
}


فراغ plugin_persist__handle_client_msg_delete(بنية mosquitto *context, ثابت بنية mosquitto__client_msg *client_msg)
{
	بنية mosquitto_evt_persist_client_msg event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	אם(context->is_persisted == false
			|| (client_msg->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		החזר;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));

	set_client_msg_event_data(&event_data, context, client_msg);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_msg_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_MSG_DELETE, &event_data, cb_base->userdata);
	}
}


فراغ plugin_persist__handle_client_msg_update(بنية mosquitto *context, ثابت بنية mosquitto__client_msg *client_msg)
{
	بنية mosquitto_evt_persist_client_msg event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	אם(context->is_persisted == false
			|| (client_msg->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		החזר;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));

	set_client_msg_event_data(&event_data, context, client_msg);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_msg_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_MSG_UPDATE, &event_data, cb_base->userdata);
	}
}


فراغ plugin_persist__handle_base_msg_add(بنية mosquitto__base_msg *base_msg)
{
	بنية mosquitto_evt_persist_base_msg event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	אם(base_msg->stored || db.shutdown){
		החזר;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));

	event_data.data.store_id = base_msg->data.store_id;
	event_data.data.expiry_time = base_msg->data.expiry_time;
	event_data.data.topic = base_msg->data.topic;
	event_data.data.payload = base_msg->data.payload;
	event_data.data.source_id = base_msg->data.source_id;
	event_data.data.source_username = base_msg->data.source_username;
	event_data.data.properties = base_msg->data.properties;
	event_data.data.payloadlen = base_msg->data.payloadlen;
	event_data.data.source_mid = base_msg->data.source_mid;
	אם(base_msg->source_listener){
		event_data.data.source_port = base_msg->source_listener->port;
	}אחרת{
		event_data.data.source_port = 0;
	}
	event_data.data.qos = base_msg->data.qos;
	event_data.data.retain = base_msg->data.retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_msg_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_MSG_ADD, &event_data, cb_base->userdata);
	}
	base_msg->stored = true;
}


فراغ plugin_persist__handle_base_msg_delete(بنية mosquitto__base_msg *base_msg)
{
	بنية mosquitto_evt_persist_base_msg event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	אם(base_msg->stored == false || db.shutdown){
		החזר;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));

	event_data.data.store_id = base_msg->data.store_id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_msg_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_MSG_DELETE, &event_data, cb_base->userdata);
	}
	base_msg->stored = false;
}


فراغ plugin_persist__handle_retain_msg_set(بنية mosquitto__base_msg *base_msg)
{
	بنية mosquitto_evt_persist_retain_msg event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	אם(db.shutdown){
		החזר;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));

	event_data.store_id = base_msg->data.store_id;
	event_data.topic = base_msg->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_msg_set, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_MSG_SET, &event_data, cb_base->userdata);
	}
}


فراغ plugin_persist__handle_retain_msg_delete(بنية mosquitto__base_msg *base_msg)
{
	بنية mosquitto_evt_persist_retain_msg event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	אם(db.shutdown){
		החזר;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));

	event_data.topic = base_msg->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_msg_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_MSG_DELETE, &event_data, cb_base->userdata);
	}
}


فراغ plugin_persist__handle_will_add(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_will_msg event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;
	بنية mosquitto_message *will_msg;

	אם(db.shutdown || !context->will){
		החזר;
	}

	opts = &db.config->security_options;
	will_msg = &context->will->msg;
	memset(&event_data, 0, حجم(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic = will_msg->topic;
	event_data.data.payload = will_msg->payload;
	event_data.data.payloadlen = (uint32_t)will_msg->payloadlen;
	event_data.data.qos = (uint8_t)will_msg->qos;
	event_data.data.retain = will_msg->retain;
	event_data.data.properties = context->will->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}
}


فراغ plugin_persist__handle_will_delete(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_will_msg event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	memset(&event_data, 0, حجم(event_data));
	event_data.data.clientid = context->id;

	אם(db.shutdown){
		החזר;
	}

	opts = &db.config->security_options;
	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}

}
