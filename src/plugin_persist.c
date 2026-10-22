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

#include "config.h"

#include "mosquitto_broker_internal.h"
#include "mosquitto_internal.h"
#include "mosquitto/broker.h"
#include "mosquitto/mqtt_protocol.h"
#include "send_mosq.h"
#include "util_mosq.h"
#include "utlist.h"
#include "lib_load.h"
#include "will_mosq.h"
#include <stdint.h>


void plugin_persist__handle_restore(void)
{
	struct mosquitto_evt_persist_restore event_data;
	struct mosquitto__callback *cb_base, *cb_next;
	struct mosquitto__security_options *opts;

	opts = &db.config->security_options;
	memset(&event_data, 0, sizeof(event_data));

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_restore, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RESTORE, &event_data, cb_base->userdata);
	}
}


void plugin_persist__handle_client_add(struct mosquitto *context)
{
	struct mosquitto_evt_persist_client event_data;
	struct mosquitto__callback *cb_base, *cb_next;
	struct mosquitto__security_options *opts;

	if(db.shutdown || context->is_persisted){
		return;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, sizeof(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	if(context->listener){
		event_data.data.listener_port = context->listener->port;
	}else{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ADD, &event_data, cb_base->userdata);
	}

	if(context->will){
		plugin_persist__handle_will_add(context);
	}

	context->is_persisted = true;
}


void plugin_persist__handle_client_update(struct mosquitto *context)
{
	struct mosquitto_evt_persist_client event_data;
	struct mosquitto__callback *cb_base, *cb_next;
	struct mosquitto__security_options *opts;
	struct mosquitto_message_v5 will;

	UNUSED(will); /* FIXME */

	if(db.shutdown){
		return;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, sizeof(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	if(context->listener){
		event_data.data.listener_port = context->listener->port;
	}else{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_data, cb_base->userdata);
	}

	if(context->will){
		plugin_persist__handle_will_add(context);
	}else{
		plugin_persist__handle_will_delete(context);
	}
}


void plugin_persist__handle_client_delete(struct mosquitto *context)
{
	struct mosquitto_evt_persist_client event_data;
	struct mosquitto__callback *cb_base, *cb_next;
	struct mosquitto__security_options *opts;

	if(context->id == NULL
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		return;
	}

	plugin_persist__handle_will_delete(context);

	if(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		return;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, sizeof(event_data));
	event_data.data.clientid = context->id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_data, cb_base->userdata);
	}
	context->is_persisted = false;
}


void plugin_persist__handle_subscription_add(struct mosquitto *context, const struct mosquitto_subscription *sub)
{
	struct mosquitto_evt_persist_subscription event_data;
	struct mosquitto__callback *cb_base, *cb_next;
	struct mosquitto__security_options *opts;

	if(db.shutdown || context->is_persisted == false){
		return;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, sizeof(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub->topic_filter;
	event_data.data.identifier = sub->identifier;
	event_data.data.options = sub->options;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD, &event_data, cb_base->userdata);
	}
}


void plugin_persist__handle_subscription_delete(struct mosquitto *context, char *sub)
{
	struct mosquitto_evt_persist_subscription event_data;
	struct mosquitto__callback *cb_base, *cb_next;
	struct mosquitto__security_options *opts;

	if(db.shutdown || context->is_persisted == false){
		return;
	}
	if(!sub){
		return;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, sizeof(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_data, cb_base->userdata);
	}
}


static inline void set_client_मेनसाहे_event_data(struct mosquitto_evt_persist_client_मेनसाहे *event_data, struct mosquitto *context, const struct mosquitto_client_मेनसाहे *client_मेनसाहे)
{
	event_data->data.clientid = context->id;
	event_data->data.cmsg_id = client_मेनसाहे->data.cmsg_id;
	event_data->data.direction = (uint8_t)client_मेनसाहे->data.direction;
	event_data->data.dup = client_मेनसाहे->data.dup;
	event_data->data.mid = client_मेनसाहे->data.mid;
	event_data->data.qos = client_मेनसाहे->data.qos;
	event_data->data.retain = client_मेनसाहे->data.retain;
	event_data->data.state = (uint8_t)client_मेनसाहे->data.state;
	event_data->data.store_id = client_मेनसाहे->base_मेनसाहे->data.store_id;
	event_data->data.subscription_identifier = client_मेनसाहे->data.subscription_identifier;
}


void plugin_persist_handle_client_मेनसाहे_add(struct mosquitto *context, const struct mosquitto_client_मेनसाहे *client_मेनसाहे)
{
	struct mosquitto_evt_persist_client_मेनसाहे event_data;
	struct mosquitto__callback *cb_base, *cb_next;
	struct mosquitto__security_options *opts;

	if(context->is_persisted == false
			|| (client_मेनसाहे->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		return;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, sizeof(event_data));

	set_client_मेनसाहे_event_data(&event_data, context, client_मेनसाहे);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_मेनसाहे_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_मेनसाहे_ADD, &event_data, cb_base->userdata);
	}
}


void plugin_persist_handle_client_मेनसाहे_delete(struct mosquitto *context, const struct mosquitto_client_मेनसाहे *client_मेनसाहे)
{
	struct mosquitto_evt_persist_client_मेनसाहे event_data;
	struct mosquitto__callback *cb_base, *cb_next;
	struct mosquitto__security_options *opts;

	if(context->is_persisted == false
			|| (client_मेनसाहे->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		return;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, sizeof(event_data));

	set_client_मेनसाहे_event_data(&event_data, context, client_मेनसाहे);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_मेनसाहे_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_मेनसाहे_DELETE, &event_data, cb_base->userdata);
	}
}


void plugin_persist_handle_client_मेनसाहे_update(struct mosquitto *context, const struct mosquitto_client_मेनसाहे *client_मेनसाहे)
{
	struct mosquitto_evt_persist_client_मेनसाहे event_data;
	struct mosquitto__callback *cb_base, *cb_next;
	struct mosquitto__security_options *opts;

	if(context->is_persisted == false
			|| (client_मेनसाहे->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		return;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, sizeof(event_data));

	set_client_मेनसाहे_event_data(&event_data, context, client_मेनसाहे);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_मेनसाहे_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_मेनसाहे_UPDATE, &event_data, cb_base->userdata);
	}
}


void plugin_persist_handle_base_मेनसाहे_add(struct mosquitto_base_मेनसाहे *base_मेनसाहे)
{
	struct mosquitto_evt_persist_base_मेनसाहे event_data;
	struct mosquitto__callback *cb_base, *cb_next;
	struct mosquitto__security_options *opts;

	if(base_मेनसाहे->stored || db.shutdown){
		return;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, sizeof(event_data));

	event_data.data.store_id = base_मेनसाहे->data.store_id;
	event_data.data.expiry_time = base_मेनसाहे->data.expiry_time;
	event_data.data.topic = base_मेनसाहे->data.topic;
	event_data.data.payload = base_मेनसाहे->data.payload;
	event_data.data.source_id = base_मेनसाहे->data.source_id;
	event_data.data.source_username = base_मेनसाहे->data.source_username;
	event_data.data.properties = base_मेनसाहे->data.properties;
	event_data.data.payloadlen = base_मेनसाहे->data.payloadlen;
	event_data.data.source_mid = base_मेनसाहे->data.source_mid;
	if(base_मेनसाहे->source_listener){
		event_data.data.source_port = base_मेनसाहे->source_listener->port;
	}else{
		event_data.data.source_port = 0;
	}
	event_data.data.qos = base_मेनसाहे->data.qos;
	event_data.data.retain = base_मेनसाहे->data.retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_मेनसाहे_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_मेनसाहे_ADD, &event_data, cb_base->userdata);
	}
	base_मेनसाहे->stored = true;
}


void plugin_persist_handle_base_मेनसाहे_delete(struct mosquitto_base_मेनसाहे *base_मेनसाहे)
{
	struct mosquitto_evt_persist_base_मेनसाहे event_data;
	struct mosquitto__callback *cb_base, *cb_next;
	struct mosquitto__security_options *opts;

	if(base_मेनसाहे->stored == false || db.shutdown){
		return;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, sizeof(event_data));

	event_data.data.store_id = base_मेनसाहे->data.store_id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_मेनसाहे_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_मेनसाहे_DELETE, &event_data, cb_base->userdata);
	}
	base_मेनसाहे->stored = false;
}


void plugin_persist_handle_retain_मेनसाहे_set(struct mosquitto_base_मेनसाहे *base_मेनसाहे)
{
	struct mosquitto_evt_persist_retain_मेनसाहे event_data;
	struct mosquitto__callback *cb_base, *cb_next;
	struct mosquitto__security_options *opts;

	if(db.shutdown){
		return;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, sizeof(event_data));

	event_data.store_id = base_मेनसाहे->data.store_id;
	event_data.topic = base_मेनसाहे->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_मेनसाहे_set, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_मेनसाहे_SET, &event_data, cb_base->userdata);
	}
}


void plugin_persist_handle_retain_मेनसाहे_delete(struct mosquitto_base_मेनसाहे *base_मेनसाहे)
{
	struct mosquitto_evt_persist_retain_मेनसाहे event_data;
	struct mosquitto__callback *cb_base, *cb_next;
	struct mosquitto__security_options *opts;

	if(db.shutdown){
		return;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, sizeof(event_data));

	event_data.topic = base_मेनसाहे->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_मेनसाहे_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_मेनसाहे_DELETE, &event_data, cb_base->userdata);
	}
}


void plugin_persist__handle_will_add(struct mosquitto *context)
{
	struct mosquitto_evt_persist_will_मेनसाहे event_data;
	struct mosquitto__callback *cb_base, *cb_next;
	struct mosquitto__security_options *opts;
	struct mosquitto_message *will_मेनसाहे;

	if(db.shutdown || !context->will){
		return;
	}

	opts = &db.config->security_options;
	will_मेनसाहे = &context->will->मेनसाहे;
	memset(&event_data, 0, sizeof(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic = will_मेनसाहे->topic;
	event_data.data.payload = will_मेनसाहे->payload;
	event_data.data.payloadlen = (uint32_t)will_मेनसाहे->payloadlen;
	event_data.data.qos = (uint8_t)will_मेनसाहे->qos;
	event_data.data.retain = will_मेनसाहे->retain;
	event_data.data.properties = context->will->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}
}


void plugin_persist__handle_will_delete(struct mosquitto *context)
{
	struct mosquitto_evt_persist_will_मेनसाहे event_data;
	struct mosquitto__callback *cb_base, *cb_next;
	struct mosquitto__security_options *opts;

	memset(&event_data, 0, sizeof(event_data));
	event_data.data.clientid = context->id;

	if(db.shutdown){
		return;
	}

	opts = &db.config->security_options;
	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}

}
