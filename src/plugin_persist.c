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

	إذا(db.shutdown || context->is_persisted){
		أرجع;
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
	إذا(context->listener){
		event_data.data.listener_port = context->listener->port;
	}وإلا{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ADD, &event_data, cb_base->userdata);
	}

	إذا(context->will){
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

	إذا(db.shutdown){
		أرجع;
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
	إذا(context->listener){
		event_data.data.listener_port = context->listener->port;
	}وإلا{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_data, cb_base->userdata);
	}

	إذا(context->will){
		plugin_persist__handle_will_add(context);
	}وإلا{
		plugin_persist__handle_will_delete(context);
	}
}


فراغ plugin_persist__handle_client_delete(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_client event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	إذا(context->id == عدم
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		أرجع;
	}

	plugin_persist__handle_will_delete(context);

	إذا(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		أرجع;
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

	إذا(db.shutdown || context->is_persisted == false){
		أرجع;
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

	إذا(db.shutdown || context->is_persisted == false){
		أرجع;
	}
	إذا(!sub){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_data, cb_base->userdata);
	}
}


ساكن مضمن فراغ set_client_رسالة_event_data(بنية mosquitto_evt_persist_client_رسالة *event_data, بنية mosquitto *context, ثابت بنية mosquitto_client_رسالة *client_رسالة)
{
	event_data->data.clientid = context->id;
	event_data->data.cmsg_id = client_رسالة->data.cmsg_id;
	event_data->data.direction = (uint8_t)client_رسالة->data.direction;
	event_data->data.dup = client_رسالة->data.dup;
	event_data->data.mid = client_رسالة->data.mid;
	event_data->data.qos = client_رسالة->data.qos;
	event_data->data.retain = client_رسالة->data.retain;
	event_data->data.state = (uint8_t)client_رسالة->data.state;
	event_data->data.store_id = client_رسالة->base_رسالة->data.store_id;
	event_data->data.subscription_identifier = client_رسالة->data.subscription_identifier;
}


فراغ plugin_persist_handle_client_رسالة_add(بنية mosquitto *context, ثابت بنية mosquitto_client_رسالة *client_رسالة)
{
	بنية mosquitto_evt_persist_client_رسالة event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	إذا(context->is_persisted == false
			|| (client_رسالة->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));

	set_client_رسالة_event_data(&event_data, context, client_رسالة);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_رسالة_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_رسالة_ADD, &event_data, cb_base->userdata);
	}
}


فراغ plugin_persist_handle_client_رسالة_delete(بنية mosquitto *context, ثابت بنية mosquitto_client_رسالة *client_رسالة)
{
	بنية mosquitto_evt_persist_client_رسالة event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	إذا(context->is_persisted == false
			|| (client_رسالة->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));

	set_client_رسالة_event_data(&event_data, context, client_رسالة);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_رسالة_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_رسالة_DELETE, &event_data, cb_base->userdata);
	}
}


فراغ plugin_persist_handle_client_رسالة_update(بنية mosquitto *context, ثابت بنية mosquitto_client_رسالة *client_رسالة)
{
	بنية mosquitto_evt_persist_client_رسالة event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	إذا(context->is_persisted == false
			|| (client_رسالة->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));

	set_client_رسالة_event_data(&event_data, context, client_رسالة);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_رسالة_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_رسالة_UPDATE, &event_data, cb_base->userdata);
	}
}


فراغ plugin_persist_handle_base_رسالة_add(بنية mosquitto_base_رسالة *base_رسالة)
{
	بنية mosquitto_evt_persist_base_رسالة event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	إذا(base_رسالة->stored || db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));

	event_data.data.store_id = base_رسالة->data.store_id;
	event_data.data.expiry_time = base_رسالة->data.expiry_time;
	event_data.data.topic = base_رسالة->data.topic;
	event_data.data.payload = base_رسالة->data.payload;
	event_data.data.source_id = base_رسالة->data.source_id;
	event_data.data.source_username = base_رسالة->data.source_username;
	event_data.data.properties = base_رسالة->data.properties;
	event_data.data.payloadlen = base_رسالة->data.payloadlen;
	event_data.data.source_mid = base_رسالة->data.source_mid;
	إذا(base_رسالة->source_listener){
		event_data.data.source_port = base_رسالة->source_listener->port;
	}وإلا{
		event_data.data.source_port = 0;
	}
	event_data.data.qos = base_رسالة->data.qos;
	event_data.data.retain = base_رسالة->data.retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_رسالة_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_رسالة_ADD, &event_data, cb_base->userdata);
	}
	base_رسالة->stored = true;
}


فراغ plugin_persist_handle_base_رسالة_delete(بنية mosquitto_base_رسالة *base_رسالة)
{
	بنية mosquitto_evt_persist_base_رسالة event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	إذا(base_رسالة->stored == false || db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));

	event_data.data.store_id = base_رسالة->data.store_id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_رسالة_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_رسالة_DELETE, &event_data, cb_base->userdata);
	}
	base_رسالة->stored = false;
}


فراغ plugin_persist_handle_retain_رسالة_set(بنية mosquitto_base_رسالة *base_رسالة)
{
	بنية mosquitto_evt_persist_retain_رسالة event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));

	event_data.store_id = base_رسالة->data.store_id;
	event_data.topic = base_رسالة->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_رسالة_set, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_رسالة_SET, &event_data, cb_base->userdata);
	}
}


فراغ plugin_persist_handle_retain_رسالة_delete(بنية mosquitto_base_رسالة *base_رسالة)
{
	بنية mosquitto_evt_persist_retain_رسالة event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, حجم(event_data));

	event_data.topic = base_رسالة->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_رسالة_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_رسالة_DELETE, &event_data, cb_base->userdata);
	}
}


فراغ plugin_persist__handle_will_add(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_will_رسالة event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;
	بنية mosquitto_message *will_رسالة;

	إذا(db.shutdown || !context->will){
		أرجع;
	}

	opts = &db.config->security_options;
	will_رسالة = &context->will->رسالة;
	memset(&event_data, 0, حجم(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic = will_رسالة->topic;
	event_data.data.payload = will_رسالة->payload;
	event_data.data.payloadlen = (uint32_t)will_رسالة->payloadlen;
	event_data.data.qos = (uint8_t)will_رسالة->qos;
	event_data.data.retain = will_رسالة->retain;
	event_data.data.properties = context->will->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}
}


فراغ plugin_persist__handle_will_delete(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_will_رسالة event_data;
	بنية mosquitto__callback *cb_base, *cb_next;
	بنية mosquitto__security_options *opts;

	memset(&event_data, 0, حجم(event_data));
	event_data.data.clientid = context->id;

	إذا(db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}

}
