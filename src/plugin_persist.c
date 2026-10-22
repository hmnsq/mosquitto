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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto_internal.h"
#അന്തര്ഭാവ "mosquitto/broker.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "send_mosq.h"
#അന്തര്ഭാവ "util_mosq.h"
#അന്തര്ഭാവ "utlist.h"
#അന്തര്ഭാവ "lib_load.h"
#അന്തര്ഭാവ "will_mosq.h"
#അന്തര്ഭാവ <stdint.h>


ശൂന്യം plugin_persist__handle_restore(ശൂന്യം)
{
	ഘടന mosquitto_evt_persist_restore event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;
	ഘടന mosquitto__security_options *opts;

	opts = &db.config->security_options;
	memset(&event_data, 0, വലുപ്പം(event_data));

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_restore, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RESTORE, &event_data, cb_base->userdata);
	}
}


ശൂന്യം plugin_persist__handle_client_add(ഘടന mosquitto *context)
{
	ഘടന mosquitto_evt_persist_client event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;
	ഘടന mosquitto__security_options *opts;

	എങ്കിൽ(db.shutdown || context->is_persisted){
		തിരികെ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, വലുപ്പം(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	എങ്കിൽ(context->listener){
		event_data.data.listener_port = context->listener->port;
	}അല്ലെങ്കിൽ{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ADD, &event_data, cb_base->userdata);
	}

	എങ്കിൽ(context->will){
		plugin_persist__handle_will_add(context);
	}

	context->is_persisted = true;
}


ശൂന്യം plugin_persist__handle_client_update(ഘടന mosquitto *context)
{
	ഘടന mosquitto_evt_persist_client event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;
	ഘടന mosquitto__security_options *opts;
	ഘടന mosquitto_message_v5 will;

	UNUSED(will); /* FIXME */

	എങ്കിൽ(db.shutdown){
		തിരികെ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, വലുപ്പം(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	എങ്കിൽ(context->listener){
		event_data.data.listener_port = context->listener->port;
	}അല്ലെങ്കിൽ{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_data, cb_base->userdata);
	}

	എങ്കിൽ(context->will){
		plugin_persist__handle_will_add(context);
	}അല്ലെങ്കിൽ{
		plugin_persist__handle_will_delete(context);
	}
}


ശൂന്യം plugin_persist__handle_client_delete(ഘടന mosquitto *context)
{
	ഘടന mosquitto_evt_persist_client event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;
	ഘടന mosquitto__security_options *opts;

	എങ്കിൽ(context->id == ശൂന്യ
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		തിരികെ;
	}

	plugin_persist__handle_will_delete(context);

	എങ്കിൽ(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		തിരികെ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, വലുപ്പം(event_data));
	event_data.data.clientid = context->id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_data, cb_base->userdata);
	}
	context->is_persisted = false;
}


ശൂന്യം plugin_persist__handle_subscription_add(ഘടന mosquitto *context, സ്ഥിരം ഘടന mosquitto_subscription *sub)
{
	ഘടന mosquitto_evt_persist_subscription event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;
	ഘടന mosquitto__security_options *opts;

	എങ്കിൽ(db.shutdown || context->is_persisted == false){
		തിരികെ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, വലുപ്പം(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub->topic_filter;
	event_data.data.identifier = sub->identifier;
	event_data.data.options = sub->options;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD, &event_data, cb_base->userdata);
	}
}


ശൂന്യം plugin_persist__handle_subscription_delete(ഘടന mosquitto *context, അക്ഷരം *sub)
{
	ഘടന mosquitto_evt_persist_subscription event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;
	ഘടന mosquitto__security_options *opts;

	എങ്കിൽ(db.shutdown || context->is_persisted == false){
		തിരികെ;
	}
	എങ്കിൽ(!sub){
		തിരികെ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, വലുപ്പം(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_data, cb_base->userdata);
	}
}


സ്ഥിതം വരിയിൽ ശൂന്യം set_client_സന്ദേശം_event_data(ഘടന mosquitto_evt_persist_client_സന്ദേശം *event_data, ഘടന mosquitto *context, സ്ഥിരം ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം)
{
	event_data->data.clientid = context->id;
	event_data->data.cmsg_id = client_സന്ദേശം->data.cmsg_id;
	event_data->data.direction = (uint8_t)client_സന്ദേശം->data.direction;
	event_data->data.dup = client_സന്ദേശം->data.dup;
	event_data->data.mid = client_സന്ദേശം->data.mid;
	event_data->data.qos = client_സന്ദേശം->data.qos;
	event_data->data.retain = client_സന്ദേശം->data.retain;
	event_data->data.state = (uint8_t)client_സന്ദേശം->data.state;
	event_data->data.store_id = client_സന്ദേശം->base_സന്ദേശം->data.store_id;
	event_data->data.subscription_identifier = client_സന്ദേശം->data.subscription_identifier;
}


ശൂന്യം plugin_persist_handle_client_സന്ദേശം_add(ഘടന mosquitto *context, സ്ഥിരം ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം)
{
	ഘടന mosquitto_evt_persist_client_സന്ദേശം event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;
	ഘടന mosquitto__security_options *opts;

	എങ്കിൽ(context->is_persisted == false
			|| (client_സന്ദേശം->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		തിരികെ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, വലുപ്പം(event_data));

	set_client_സന്ദേശം_event_data(&event_data, context, client_സന്ദേശം);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_സന്ദേശം_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_സന്ദേശം_ADD, &event_data, cb_base->userdata);
	}
}


ശൂന്യം plugin_persist_handle_client_സന്ദേശം_delete(ഘടന mosquitto *context, സ്ഥിരം ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം)
{
	ഘടന mosquitto_evt_persist_client_സന്ദേശം event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;
	ഘടന mosquitto__security_options *opts;

	എങ്കിൽ(context->is_persisted == false
			|| (client_സന്ദേശം->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		തിരികെ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, വലുപ്പം(event_data));

	set_client_സന്ദേശം_event_data(&event_data, context, client_സന്ദേശം);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_സന്ദേശം_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_സന്ദേശം_DELETE, &event_data, cb_base->userdata);
	}
}


ശൂന്യം plugin_persist_handle_client_സന്ദേശം_update(ഘടന mosquitto *context, സ്ഥിരം ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം)
{
	ഘടന mosquitto_evt_persist_client_സന്ദേശം event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;
	ഘടന mosquitto__security_options *opts;

	എങ്കിൽ(context->is_persisted == false
			|| (client_സന്ദേശം->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		തിരികെ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, വലുപ്പം(event_data));

	set_client_സന്ദേശം_event_data(&event_data, context, client_സന്ദേശം);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_സന്ദേശം_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_സന്ദേശം_UPDATE, &event_data, cb_base->userdata);
	}
}


ശൂന്യം plugin_persist_handle_base_സന്ദേശം_add(ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം)
{
	ഘടന mosquitto_evt_persist_base_സന്ദേശം event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;
	ഘടന mosquitto__security_options *opts;

	എങ്കിൽ(base_സന്ദേശം->stored || db.shutdown){
		തിരികെ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, വലുപ്പം(event_data));

	event_data.data.store_id = base_സന്ദേശം->data.store_id;
	event_data.data.expiry_time = base_സന്ദേശം->data.expiry_time;
	event_data.data.topic = base_സന്ദേശം->data.topic;
	event_data.data.payload = base_സന്ദേശം->data.payload;
	event_data.data.source_id = base_സന്ദേശം->data.source_id;
	event_data.data.source_username = base_സന്ദേശം->data.source_username;
	event_data.data.properties = base_സന്ദേശം->data.properties;
	event_data.data.payloadlen = base_സന്ദേശം->data.payloadlen;
	event_data.data.source_mid = base_സന്ദേശം->data.source_mid;
	എങ്കിൽ(base_സന്ദേശം->source_listener){
		event_data.data.source_port = base_സന്ദേശം->source_listener->port;
	}അല്ലെങ്കിൽ{
		event_data.data.source_port = 0;
	}
	event_data.data.qos = base_സന്ദേശം->data.qos;
	event_data.data.retain = base_സന്ദേശം->data.retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_സന്ദേശം_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_സന്ദേശം_ADD, &event_data, cb_base->userdata);
	}
	base_സന്ദേശം->stored = true;
}


ശൂന്യം plugin_persist_handle_base_സന്ദേശം_delete(ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം)
{
	ഘടന mosquitto_evt_persist_base_സന്ദേശം event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;
	ഘടന mosquitto__security_options *opts;

	എങ്കിൽ(base_സന്ദേശം->stored == false || db.shutdown){
		തിരികെ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, വലുപ്പം(event_data));

	event_data.data.store_id = base_സന്ദേശം->data.store_id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_സന്ദേശം_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_സന്ദേശം_DELETE, &event_data, cb_base->userdata);
	}
	base_സന്ദേശം->stored = false;
}


ശൂന്യം plugin_persist_handle_retain_സന്ദേശം_set(ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം)
{
	ഘടന mosquitto_evt_persist_retain_സന്ദേശം event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;
	ഘടന mosquitto__security_options *opts;

	എങ്കിൽ(db.shutdown){
		തിരികെ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, വലുപ്പം(event_data));

	event_data.store_id = base_സന്ദേശം->data.store_id;
	event_data.topic = base_സന്ദേശം->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_സന്ദേശം_set, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_സന്ദേശം_SET, &event_data, cb_base->userdata);
	}
}


ശൂന്യം plugin_persist_handle_retain_സന്ദേശം_delete(ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം)
{
	ഘടന mosquitto_evt_persist_retain_സന്ദേശം event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;
	ഘടന mosquitto__security_options *opts;

	എങ്കിൽ(db.shutdown){
		തിരികെ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, വലുപ്പം(event_data));

	event_data.topic = base_സന്ദേശം->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_സന്ദേശം_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_സന്ദേശം_DELETE, &event_data, cb_base->userdata);
	}
}


ശൂന്യം plugin_persist__handle_will_add(ഘടന mosquitto *context)
{
	ഘടന mosquitto_evt_persist_will_സന്ദേശം event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;
	ഘടന mosquitto__security_options *opts;
	ഘടന mosquitto_message *will_സന്ദേശം;

	എങ്കിൽ(db.shutdown || !context->will){
		തിരികെ;
	}

	opts = &db.config->security_options;
	will_സന്ദേശം = &context->will->സന്ദേശം;
	memset(&event_data, 0, വലുപ്പം(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic = will_സന്ദേശം->topic;
	event_data.data.payload = will_സന്ദേശം->payload;
	event_data.data.payloadlen = (uint32_t)will_സന്ദേശം->payloadlen;
	event_data.data.qos = (uint8_t)will_സന്ദേശം->qos;
	event_data.data.retain = will_സന്ദേശം->retain;
	event_data.data.properties = context->will->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}
}


ശൂന്യം plugin_persist__handle_will_delete(ഘടന mosquitto *context)
{
	ഘടന mosquitto_evt_persist_will_സന്ദേശം event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;
	ഘടന mosquitto__security_options *opts;

	memset(&event_data, 0, വലുപ്പം(event_data));
	event_data.data.clientid = context->id;

	എങ്കിൽ(db.shutdown){
		തിരികെ;
	}

	opts = &db.config->security_options;
	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}

}
