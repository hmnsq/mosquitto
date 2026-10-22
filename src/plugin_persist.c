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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto_internal.h"
#અન્તર્ભાવ "mosquitto/broker.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "send_mosq.h"
#અન્તર્ભાવ "util_mosq.h"
#અન્તર્ભાવ "utlist.h"
#અન્તર્ભાવ "lib_load.h"
#અન્તર્ભાવ "will_mosq.h"
#અન્તર્ભાવ <stdint.h>


શૂન્ય plugin_persist__handle_restore(શૂન્ય)
{
	સંરચના mosquitto_evt_persist_restore event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	સંરચના mosquitto__security_options *opts;

	opts = &db.config->security_options;
	memset(&event_data, 0, કદ(event_data));

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_restore, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RESTORE, &event_data, cb_base->userdata);
	}
}


શૂન્ય plugin_persist__handle_client_add(સંરચના mosquitto *context)
{
	સંરચના mosquitto_evt_persist_client event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	સંરચના mosquitto__security_options *opts;

	જો(db.shutdown || context->is_persisted){
		પાછા;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, કદ(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	જો(context->listener){
		event_data.data.listener_port = context->listener->port;
	}નહીંતર{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ADD, &event_data, cb_base->userdata);
	}

	જો(context->will){
		plugin_persist__handle_will_add(context);
	}

	context->is_persisted = true;
}


શૂન્ય plugin_persist__handle_client_update(સંરચના mosquitto *context)
{
	સંરચના mosquitto_evt_persist_client event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	સંરચના mosquitto__security_options *opts;
	સંરચના mosquitto_message_v5 will;

	UNUSED(will); /* FIXME */

	જો(db.shutdown){
		પાછા;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, કદ(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	જો(context->listener){
		event_data.data.listener_port = context->listener->port;
	}નહીંતર{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_data, cb_base->userdata);
	}

	જો(context->will){
		plugin_persist__handle_will_add(context);
	}નહીંતર{
		plugin_persist__handle_will_delete(context);
	}
}


શૂન્ય plugin_persist__handle_client_delete(સંરચના mosquitto *context)
{
	સંરચના mosquitto_evt_persist_client event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	સંરચના mosquitto__security_options *opts;

	જો(context->id == ખાલી
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		પાછા;
	}

	plugin_persist__handle_will_delete(context);

	જો(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		પાછા;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, કદ(event_data));
	event_data.data.clientid = context->id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_data, cb_base->userdata);
	}
	context->is_persisted = false;
}


શૂન્ય plugin_persist__handle_subscription_add(સંરચના mosquitto *context, અચલ સંરચના mosquitto_subscription *sub)
{
	સંરચના mosquitto_evt_persist_subscription event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	સંરચના mosquitto__security_options *opts;

	જો(db.shutdown || context->is_persisted == false){
		પાછા;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, કદ(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub->topic_filter;
	event_data.data.identifier = sub->identifier;
	event_data.data.options = sub->options;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD, &event_data, cb_base->userdata);
	}
}


શૂન્ય plugin_persist__handle_subscription_delete(સંરચના mosquitto *context, અક્ષર *sub)
{
	સંરચના mosquitto_evt_persist_subscription event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	સંરચના mosquitto__security_options *opts;

	જો(db.shutdown || context->is_persisted == false){
		પાછા;
	}
	જો(!sub){
		પાછા;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, કદ(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_data, cb_base->userdata);
	}
}


સ્થિર અંતઃરેખ શૂન્ય set_client_સંદેશ_event_data(સંરચના mosquitto_evt_persist_client_સંદેશ *event_data, સંરચના mosquitto *context, અચલ સંરચના mosquitto_client_સંદેશ *client_સંદેશ)
{
	event_data->data.clientid = context->id;
	event_data->data.cmsg_id = client_સંદેશ->data.cmsg_id;
	event_data->data.direction = (uint8_t)client_સંદેશ->data.direction;
	event_data->data.dup = client_સંદેશ->data.dup;
	event_data->data.mid = client_સંદેશ->data.mid;
	event_data->data.qos = client_સંદેશ->data.qos;
	event_data->data.retain = client_સંદેશ->data.retain;
	event_data->data.state = (uint8_t)client_સંદેશ->data.state;
	event_data->data.store_id = client_સંદેશ->base_સંદેશ->data.store_id;
	event_data->data.subscription_identifier = client_સંદેશ->data.subscription_identifier;
}


શૂન્ય plugin_persist_handle_client_સંદેશ_add(સંરચના mosquitto *context, અચલ સંરચના mosquitto_client_સંદેશ *client_સંદેશ)
{
	સંરચના mosquitto_evt_persist_client_સંદેશ event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	સંરચના mosquitto__security_options *opts;

	જો(context->is_persisted == false
			|| (client_સંદેશ->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		પાછા;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, કદ(event_data));

	set_client_સંદેશ_event_data(&event_data, context, client_સંદેશ);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_સંદેશ_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_સંદેશ_ADD, &event_data, cb_base->userdata);
	}
}


શૂન્ય plugin_persist_handle_client_સંદેશ_delete(સંરચના mosquitto *context, અચલ સંરચના mosquitto_client_સંદેશ *client_સંદેશ)
{
	સંરચના mosquitto_evt_persist_client_સંદેશ event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	સંરચના mosquitto__security_options *opts;

	જો(context->is_persisted == false
			|| (client_સંદેશ->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		પાછા;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, કદ(event_data));

	set_client_સંદેશ_event_data(&event_data, context, client_સંદેશ);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_સંદેશ_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_સંદેશ_DELETE, &event_data, cb_base->userdata);
	}
}


શૂન્ય plugin_persist_handle_client_સંદેશ_update(સંરચના mosquitto *context, અચલ સંરચના mosquitto_client_સંદેશ *client_સંદેશ)
{
	સંરચના mosquitto_evt_persist_client_સંદેશ event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	સંરચના mosquitto__security_options *opts;

	જો(context->is_persisted == false
			|| (client_સંદેશ->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		પાછા;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, કદ(event_data));

	set_client_સંદેશ_event_data(&event_data, context, client_સંદેશ);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_સંદેશ_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_સંદેશ_UPDATE, &event_data, cb_base->userdata);
	}
}


શૂન્ય plugin_persist_handle_base_સંદેશ_add(સંરચના mosquitto_base_સંદેશ *base_સંદેશ)
{
	સંરચના mosquitto_evt_persist_base_સંદેશ event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	સંરચના mosquitto__security_options *opts;

	જો(base_સંદેશ->stored || db.shutdown){
		પાછા;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, કદ(event_data));

	event_data.data.store_id = base_સંદેશ->data.store_id;
	event_data.data.expiry_time = base_સંદેશ->data.expiry_time;
	event_data.data.topic = base_સંદેશ->data.topic;
	event_data.data.payload = base_સંદેશ->data.payload;
	event_data.data.source_id = base_સંદેશ->data.source_id;
	event_data.data.source_username = base_સંદેશ->data.source_username;
	event_data.data.properties = base_સંદેશ->data.properties;
	event_data.data.payloadlen = base_સંદેશ->data.payloadlen;
	event_data.data.source_mid = base_સંદેશ->data.source_mid;
	જો(base_સંદેશ->source_listener){
		event_data.data.source_port = base_સંદેશ->source_listener->port;
	}નહીંતર{
		event_data.data.source_port = 0;
	}
	event_data.data.qos = base_સંદેશ->data.qos;
	event_data.data.retain = base_સંદેશ->data.retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_સંદેશ_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_સંદેશ_ADD, &event_data, cb_base->userdata);
	}
	base_સંદેશ->stored = true;
}


શૂન્ય plugin_persist_handle_base_સંદેશ_delete(સંરચના mosquitto_base_સંદેશ *base_સંદેશ)
{
	સંરચના mosquitto_evt_persist_base_સંદેશ event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	સંરચના mosquitto__security_options *opts;

	જો(base_સંદેશ->stored == false || db.shutdown){
		પાછા;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, કદ(event_data));

	event_data.data.store_id = base_સંદેશ->data.store_id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_સંદેશ_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_સંદેશ_DELETE, &event_data, cb_base->userdata);
	}
	base_સંદેશ->stored = false;
}


શૂન્ય plugin_persist_handle_retain_સંદેશ_set(સંરચના mosquitto_base_સંદેશ *base_સંદેશ)
{
	સંરચના mosquitto_evt_persist_retain_સંદેશ event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	સંરચના mosquitto__security_options *opts;

	જો(db.shutdown){
		પાછા;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, કદ(event_data));

	event_data.store_id = base_સંદેશ->data.store_id;
	event_data.topic = base_સંદેશ->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_સંદેશ_set, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_સંદેશ_SET, &event_data, cb_base->userdata);
	}
}


શૂન્ય plugin_persist_handle_retain_સંદેશ_delete(સંરચના mosquitto_base_સંદેશ *base_સંદેશ)
{
	સંરચના mosquitto_evt_persist_retain_સંદેશ event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	સંરચના mosquitto__security_options *opts;

	જો(db.shutdown){
		પાછા;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, કદ(event_data));

	event_data.topic = base_સંદેશ->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_સંદેશ_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_સંદેશ_DELETE, &event_data, cb_base->userdata);
	}
}


શૂન્ય plugin_persist__handle_will_add(સંરચના mosquitto *context)
{
	સંરચના mosquitto_evt_persist_will_સંદેશ event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	સંરચના mosquitto__security_options *opts;
	સંરચના mosquitto_message *will_સંદેશ;

	જો(db.shutdown || !context->will){
		પાછા;
	}

	opts = &db.config->security_options;
	will_સંદેશ = &context->will->સંદેશ;
	memset(&event_data, 0, કદ(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic = will_સંદેશ->topic;
	event_data.data.payload = will_સંદેશ->payload;
	event_data.data.payloadlen = (uint32_t)will_સંદેશ->payloadlen;
	event_data.data.qos = (uint8_t)will_સંદેશ->qos;
	event_data.data.retain = will_સંદેશ->retain;
	event_data.data.properties = context->will->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}
}


શૂન્ય plugin_persist__handle_will_delete(સંરચના mosquitto *context)
{
	સંરચના mosquitto_evt_persist_will_સંદેશ event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	સંરચના mosquitto__security_options *opts;

	memset(&event_data, 0, કદ(event_data));
	event_data.data.clientid = context->id;

	જો(db.shutdown){
		પાછા;
	}

	opts = &db.config->security_options;
	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}

}
