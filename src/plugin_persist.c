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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto_internal.h"
#அந்தர்பாவ "mosquitto/broker.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "send_mosq.h"
#அந்தர்பாவ "util_mosq.h"
#அந்தர்பாவ "utlist.h"
#அந்தர்பாவ "lib_load.h"
#அந்தர்பாவ "will_mosq.h"
#அந்தர்பாவ <stdint.h>


வெறுமை plugin_persist__handle_restore(வெறுமை)
{
	கட்டமைப்பு mosquitto_evt_persist_restore event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	கட்டமைப்பு mosquitto__security_options *opts;

	opts = &db.config->security_options;
	memset(&event_data, 0, அளவு(event_data));

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_restore, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RESTORE, &event_data, cb_base->userdata);
	}
}


வெறுமை plugin_persist__handle_client_add(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு mosquitto_evt_persist_client event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	கட்டமைப்பு mosquitto__security_options *opts;

	எனில்(db.shutdown || context->is_persisted){
		திரும்பு;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, அளவு(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	எனில்(context->listener){
		event_data.data.listener_port = context->listener->port;
	}இல்லையெனில்{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ADD, &event_data, cb_base->userdata);
	}

	எனில்(context->will){
		plugin_persist__handle_will_add(context);
	}

	context->is_persisted = true;
}


வெறுமை plugin_persist__handle_client_update(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு mosquitto_evt_persist_client event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	கட்டமைப்பு mosquitto__security_options *opts;
	கட்டமைப்பு mosquitto_message_v5 will;

	UNUSED(will); /* FIXME */

	எனில்(db.shutdown){
		திரும்பு;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, அளவு(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	எனில்(context->listener){
		event_data.data.listener_port = context->listener->port;
	}இல்லையெனில்{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_data, cb_base->userdata);
	}

	எனில்(context->will){
		plugin_persist__handle_will_add(context);
	}இல்லையெனில்{
		plugin_persist__handle_will_delete(context);
	}
}


வெறுமை plugin_persist__handle_client_delete(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு mosquitto_evt_persist_client event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	கட்டமைப்பு mosquitto__security_options *opts;

	எனில்(context->id == வெற்று
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		திரும்பு;
	}

	plugin_persist__handle_will_delete(context);

	எனில்(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		திரும்பு;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, அளவு(event_data));
	event_data.data.clientid = context->id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_data, cb_base->userdata);
	}
	context->is_persisted = false;
}


வெறுமை plugin_persist__handle_subscription_add(கட்டமைப்பு mosquitto *context, மாறிலி கட்டமைப்பு mosquitto_subscription *sub)
{
	கட்டமைப்பு mosquitto_evt_persist_subscription event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	கட்டமைப்பு mosquitto__security_options *opts;

	எனில்(db.shutdown || context->is_persisted == false){
		திரும்பு;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, அளவு(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub->topic_filter;
	event_data.data.identifier = sub->identifier;
	event_data.data.options = sub->options;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD, &event_data, cb_base->userdata);
	}
}


வெறுமை plugin_persist__handle_subscription_delete(கட்டமைப்பு mosquitto *context, எழுத்து *sub)
{
	கட்டமைப்பு mosquitto_evt_persist_subscription event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	கட்டமைப்பு mosquitto__security_options *opts;

	எனில்(db.shutdown || context->is_persisted == false){
		திரும்பு;
	}
	எனில்(!sub){
		திரும்பு;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, அளவு(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_data, cb_base->userdata);
	}
}


நிலையான உள்வரி வெறுமை set_client_செய்தி_event_data(கட்டமைப்பு mosquitto_evt_persist_client_செய்தி *event_data, கட்டமைப்பு mosquitto *context, மாறிலி கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி)
{
	event_data->data.clientid = context->id;
	event_data->data.cmsg_id = client_செய்தி->data.cmsg_id;
	event_data->data.direction = (uint8_t)client_செய்தி->data.direction;
	event_data->data.dup = client_செய்தி->data.dup;
	event_data->data.mid = client_செய்தி->data.mid;
	event_data->data.qos = client_செய்தி->data.qos;
	event_data->data.retain = client_செய்தி->data.retain;
	event_data->data.state = (uint8_t)client_செய்தி->data.state;
	event_data->data.store_id = client_செய்தி->base_செய்தி->data.store_id;
	event_data->data.subscription_identifier = client_செய்தி->data.subscription_identifier;
}


வெறுமை plugin_persist_handle_client_செய்தி_add(கட்டமைப்பு mosquitto *context, மாறிலி கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி)
{
	கட்டமைப்பு mosquitto_evt_persist_client_செய்தி event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	கட்டமைப்பு mosquitto__security_options *opts;

	எனில்(context->is_persisted == false
			|| (client_செய்தி->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		திரும்பு;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, அளவு(event_data));

	set_client_செய்தி_event_data(&event_data, context, client_செய்தி);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_செய்தி_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_செய்தி_ADD, &event_data, cb_base->userdata);
	}
}


வெறுமை plugin_persist_handle_client_செய்தி_delete(கட்டமைப்பு mosquitto *context, மாறிலி கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி)
{
	கட்டமைப்பு mosquitto_evt_persist_client_செய்தி event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	கட்டமைப்பு mosquitto__security_options *opts;

	எனில்(context->is_persisted == false
			|| (client_செய்தி->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		திரும்பு;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, அளவு(event_data));

	set_client_செய்தி_event_data(&event_data, context, client_செய்தி);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_செய்தி_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_செய்தி_DELETE, &event_data, cb_base->userdata);
	}
}


வெறுமை plugin_persist_handle_client_செய்தி_update(கட்டமைப்பு mosquitto *context, மாறிலி கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி)
{
	கட்டமைப்பு mosquitto_evt_persist_client_செய்தி event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	கட்டமைப்பு mosquitto__security_options *opts;

	எனில்(context->is_persisted == false
			|| (client_செய்தி->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		திரும்பு;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, அளவு(event_data));

	set_client_செய்தி_event_data(&event_data, context, client_செய்தி);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_செய்தி_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_செய்தி_UPDATE, &event_data, cb_base->userdata);
	}
}


வெறுமை plugin_persist_handle_base_செய்தி_add(கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி)
{
	கட்டமைப்பு mosquitto_evt_persist_base_செய்தி event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	கட்டமைப்பு mosquitto__security_options *opts;

	எனில்(base_செய்தி->stored || db.shutdown){
		திரும்பு;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, அளவு(event_data));

	event_data.data.store_id = base_செய்தி->data.store_id;
	event_data.data.expiry_time = base_செய்தி->data.expiry_time;
	event_data.data.topic = base_செய்தி->data.topic;
	event_data.data.payload = base_செய்தி->data.payload;
	event_data.data.source_id = base_செய்தி->data.source_id;
	event_data.data.source_username = base_செய்தி->data.source_username;
	event_data.data.properties = base_செய்தி->data.properties;
	event_data.data.payloadlen = base_செய்தி->data.payloadlen;
	event_data.data.source_mid = base_செய்தி->data.source_mid;
	எனில்(base_செய்தி->source_listener){
		event_data.data.source_port = base_செய்தி->source_listener->port;
	}இல்லையெனில்{
		event_data.data.source_port = 0;
	}
	event_data.data.qos = base_செய்தி->data.qos;
	event_data.data.retain = base_செய்தி->data.retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_செய்தி_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_செய்தி_ADD, &event_data, cb_base->userdata);
	}
	base_செய்தி->stored = true;
}


வெறுமை plugin_persist_handle_base_செய்தி_delete(கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி)
{
	கட்டமைப்பு mosquitto_evt_persist_base_செய்தி event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	கட்டமைப்பு mosquitto__security_options *opts;

	எனில்(base_செய்தி->stored == false || db.shutdown){
		திரும்பு;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, அளவு(event_data));

	event_data.data.store_id = base_செய்தி->data.store_id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_செய்தி_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_செய்தி_DELETE, &event_data, cb_base->userdata);
	}
	base_செய்தி->stored = false;
}


வெறுமை plugin_persist_handle_retain_செய்தி_set(கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி)
{
	கட்டமைப்பு mosquitto_evt_persist_retain_செய்தி event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	கட்டமைப்பு mosquitto__security_options *opts;

	எனில்(db.shutdown){
		திரும்பு;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, அளவு(event_data));

	event_data.store_id = base_செய்தி->data.store_id;
	event_data.topic = base_செய்தி->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_செய்தி_set, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_செய்தி_SET, &event_data, cb_base->userdata);
	}
}


வெறுமை plugin_persist_handle_retain_செய்தி_delete(கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி)
{
	கட்டமைப்பு mosquitto_evt_persist_retain_செய்தி event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	கட்டமைப்பு mosquitto__security_options *opts;

	எனில்(db.shutdown){
		திரும்பு;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, அளவு(event_data));

	event_data.topic = base_செய்தி->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_செய்தி_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_செய்தி_DELETE, &event_data, cb_base->userdata);
	}
}


வெறுமை plugin_persist__handle_will_add(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு mosquitto_evt_persist_will_செய்தி event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	கட்டமைப்பு mosquitto__security_options *opts;
	கட்டமைப்பு mosquitto_message *will_செய்தி;

	எனில்(db.shutdown || !context->will){
		திரும்பு;
	}

	opts = &db.config->security_options;
	will_செய்தி = &context->will->செய்தி;
	memset(&event_data, 0, அளவு(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic = will_செய்தி->topic;
	event_data.data.payload = will_செய்தி->payload;
	event_data.data.payloadlen = (uint32_t)will_செய்தி->payloadlen;
	event_data.data.qos = (uint8_t)will_செய்தி->qos;
	event_data.data.retain = will_செய்தி->retain;
	event_data.data.properties = context->will->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}
}


வெறுமை plugin_persist__handle_will_delete(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு mosquitto_evt_persist_will_செய்தி event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	கட்டமைப்பு mosquitto__security_options *opts;

	memset(&event_data, 0, அளவு(event_data));
	event_data.data.clientid = context->id;

	எனில்(db.shutdown){
		திரும்பு;
	}

	opts = &db.config->security_options;
	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}

}
