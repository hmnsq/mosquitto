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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto_internal.h"
#অন্তর্ভাব "mosquitto/broker.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "send_mosq.h"
#অন্তর্ভাব "util_mosq.h"
#অন্তর্ভাব "utlist.h"
#অন্তর্ভাব "lib_load.h"
#অন্তর্ভাব "will_mosq.h"
#অন্তর্ভাব <stdint.h>


শূন্য plugin_persist__handle_restore(শূন্য)
{
	সংরচনা mosquitto_evt_persist_restore event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;
	সংরচনা mosquitto__security_options *opts;

	opts = &db.config->security_options;
	memset(&event_data, 0, পরিমাণ(event_data));

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_restore, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RESTORE, &event_data, cb_base->userdata);
	}
}


শূন্য plugin_persist__handle_client_add(সংরচনা mosquitto *context)
{
	সংরচনা mosquitto_evt_persist_client event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;
	সংরচনা mosquitto__security_options *opts;

	যদি(db.shutdown || context->is_persisted){
		প্রত্যাগম;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, পরিমাণ(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	যদি(context->listener){
		event_data.data.listener_port = context->listener->port;
	}অন্যথা{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ADD, &event_data, cb_base->userdata);
	}

	যদি(context->will){
		plugin_persist__handle_will_add(context);
	}

	context->is_persisted = true;
}


শূন্য plugin_persist__handle_client_update(সংরচনা mosquitto *context)
{
	সংরচনা mosquitto_evt_persist_client event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;
	সংরচনা mosquitto__security_options *opts;
	সংরচনা mosquitto_message_v5 will;

	UNUSED(will); /* FIXME */

	যদি(db.shutdown){
		প্রত্যাগম;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, পরিমাণ(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	যদি(context->listener){
		event_data.data.listener_port = context->listener->port;
	}অন্যথা{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_data, cb_base->userdata);
	}

	যদি(context->will){
		plugin_persist__handle_will_add(context);
	}অন্যথা{
		plugin_persist__handle_will_delete(context);
	}
}


শূন্য plugin_persist__handle_client_delete(সংরচনা mosquitto *context)
{
	সংরচনা mosquitto_evt_persist_client event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;
	সংরচনা mosquitto__security_options *opts;

	যদি(context->id == রিক্ত
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		প্রত্যাগম;
	}

	plugin_persist__handle_will_delete(context);

	যদি(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		প্রত্যাগম;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, পরিমাণ(event_data));
	event_data.data.clientid = context->id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_data, cb_base->userdata);
	}
	context->is_persisted = false;
}


শূন্য plugin_persist__handle_subscription_add(সংরচনা mosquitto *context, ধ্রুব সংরচনা mosquitto_subscription *sub)
{
	সংরচনা mosquitto_evt_persist_subscription event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;
	সংরচনা mosquitto__security_options *opts;

	যদি(db.shutdown || context->is_persisted == false){
		প্রত্যাগম;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, পরিমাণ(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub->topic_filter;
	event_data.data.identifier = sub->identifier;
	event_data.data.options = sub->options;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD, &event_data, cb_base->userdata);
	}
}


শূন্য plugin_persist__handle_subscription_delete(সংরচনা mosquitto *context, বর্ণ *sub)
{
	সংরচনা mosquitto_evt_persist_subscription event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;
	সংরচনা mosquitto__security_options *opts;

	যদি(db.shutdown || context->is_persisted == false){
		প্রত্যাগম;
	}
	যদি(!sub){
		প্রত্যাগম;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, পরিমাণ(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_data, cb_base->userdata);
	}
}


স্থির অন্তঃস্থ শূন্য set_client_বার্তা_event_data(সংরচনা mosquitto_evt_persist_client_বার্তা *event_data, সংরচনা mosquitto *context, ধ্রুব সংরচনা mosquitto_client_বার্তা *client_বার্তা)
{
	event_data->data.clientid = context->id;
	event_data->data.cmsg_id = client_বার্তা->data.cmsg_id;
	event_data->data.direction = (uint8_t)client_বার্তা->data.direction;
	event_data->data.dup = client_বার্তা->data.dup;
	event_data->data.mid = client_বার্তা->data.mid;
	event_data->data.qos = client_বার্তা->data.qos;
	event_data->data.retain = client_বার্তা->data.retain;
	event_data->data.state = (uint8_t)client_বার্তা->data.state;
	event_data->data.store_id = client_বার্তা->base_বার্তা->data.store_id;
	event_data->data.subscription_identifier = client_বার্তা->data.subscription_identifier;
}


শূন্য plugin_persist_handle_client_বার্তা_add(সংরচনা mosquitto *context, ধ্রুব সংরচনা mosquitto_client_বার্তা *client_বার্তা)
{
	সংরচনা mosquitto_evt_persist_client_বার্তা event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;
	সংরচনা mosquitto__security_options *opts;

	যদি(context->is_persisted == false
			|| (client_বার্তা->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		প্রত্যাগম;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, পরিমাণ(event_data));

	set_client_বার্তা_event_data(&event_data, context, client_বার্তা);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_বার্তা_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_বার্তা_ADD, &event_data, cb_base->userdata);
	}
}


শূন্য plugin_persist_handle_client_বার্তা_delete(সংরচনা mosquitto *context, ধ্রুব সংরচনা mosquitto_client_বার্তা *client_বার্তা)
{
	সংরচনা mosquitto_evt_persist_client_বার্তা event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;
	সংরচনা mosquitto__security_options *opts;

	যদি(context->is_persisted == false
			|| (client_বার্তা->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		প্রত্যাগম;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, পরিমাণ(event_data));

	set_client_বার্তা_event_data(&event_data, context, client_বার্তা);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_বার্তা_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_বার্তা_DELETE, &event_data, cb_base->userdata);
	}
}


শূন্য plugin_persist_handle_client_বার্তা_update(সংরচনা mosquitto *context, ধ্রুব সংরচনা mosquitto_client_বার্তা *client_বার্তা)
{
	সংরচনা mosquitto_evt_persist_client_বার্তা event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;
	সংরচনা mosquitto__security_options *opts;

	যদি(context->is_persisted == false
			|| (client_বার্তা->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		প্রত্যাগম;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, পরিমাণ(event_data));

	set_client_বার্তা_event_data(&event_data, context, client_বার্তা);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_বার্তা_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_বার্তা_UPDATE, &event_data, cb_base->userdata);
	}
}


শূন্য plugin_persist_handle_base_বার্তা_add(সংরচনা mosquitto_base_বার্তা *base_বার্তা)
{
	সংরচনা mosquitto_evt_persist_base_বার্তা event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;
	সংরচনা mosquitto__security_options *opts;

	যদি(base_বার্তা->stored || db.shutdown){
		প্রত্যাগম;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, পরিমাণ(event_data));

	event_data.data.store_id = base_বার্তা->data.store_id;
	event_data.data.expiry_time = base_বার্তা->data.expiry_time;
	event_data.data.topic = base_বার্তা->data.topic;
	event_data.data.payload = base_বার্তা->data.payload;
	event_data.data.source_id = base_বার্তা->data.source_id;
	event_data.data.source_username = base_বার্তা->data.source_username;
	event_data.data.properties = base_বার্তা->data.properties;
	event_data.data.payloadlen = base_বার্তা->data.payloadlen;
	event_data.data.source_mid = base_বার্তা->data.source_mid;
	যদি(base_বার্তা->source_listener){
		event_data.data.source_port = base_বার্তা->source_listener->port;
	}অন্যথা{
		event_data.data.source_port = 0;
	}
	event_data.data.qos = base_বার্তা->data.qos;
	event_data.data.retain = base_বার্তা->data.retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_বার্তা_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_বার্তা_ADD, &event_data, cb_base->userdata);
	}
	base_বার্তা->stored = true;
}


শূন্য plugin_persist_handle_base_বার্তা_delete(সংরচনা mosquitto_base_বার্তা *base_বার্তা)
{
	সংরচনা mosquitto_evt_persist_base_বার্তা event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;
	সংরচনা mosquitto__security_options *opts;

	যদি(base_বার্তা->stored == false || db.shutdown){
		প্রত্যাগম;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, পরিমাণ(event_data));

	event_data.data.store_id = base_বার্তা->data.store_id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_বার্তা_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_বার্তা_DELETE, &event_data, cb_base->userdata);
	}
	base_বার্তা->stored = false;
}


শূন্য plugin_persist_handle_retain_বার্তা_set(সংরচনা mosquitto_base_বার্তা *base_বার্তা)
{
	সংরচনা mosquitto_evt_persist_retain_বার্তা event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;
	সংরচনা mosquitto__security_options *opts;

	যদি(db.shutdown){
		প্রত্যাগম;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, পরিমাণ(event_data));

	event_data.store_id = base_বার্তা->data.store_id;
	event_data.topic = base_বার্তা->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_বার্তা_set, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_বার্তা_SET, &event_data, cb_base->userdata);
	}
}


শূন্য plugin_persist_handle_retain_বার্তা_delete(সংরচনা mosquitto_base_বার্তা *base_বার্তা)
{
	সংরচনা mosquitto_evt_persist_retain_বার্তা event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;
	সংরচনা mosquitto__security_options *opts;

	যদি(db.shutdown){
		প্রত্যাগম;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, পরিমাণ(event_data));

	event_data.topic = base_বার্তা->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_বার্তা_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_বার্তা_DELETE, &event_data, cb_base->userdata);
	}
}


শূন্য plugin_persist__handle_will_add(সংরচনা mosquitto *context)
{
	সংরচনা mosquitto_evt_persist_will_বার্তা event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;
	সংরচনা mosquitto__security_options *opts;
	সংরচনা mosquitto_message *will_বার্তা;

	যদি(db.shutdown || !context->will){
		প্রত্যাগম;
	}

	opts = &db.config->security_options;
	will_বার্তা = &context->will->বার্তা;
	memset(&event_data, 0, পরিমাণ(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic = will_বার্তা->topic;
	event_data.data.payload = will_বার্তা->payload;
	event_data.data.payloadlen = (uint32_t)will_বার্তা->payloadlen;
	event_data.data.qos = (uint8_t)will_বার্তা->qos;
	event_data.data.retain = will_বার্তা->retain;
	event_data.data.properties = context->will->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}
}


শূন্য plugin_persist__handle_will_delete(সংরচনা mosquitto *context)
{
	সংরচনা mosquitto_evt_persist_will_বার্তা event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;
	সংরচনা mosquitto__security_options *opts;

	memset(&event_data, 0, পরিমাণ(event_data));
	event_data.data.clientid = context->id;

	যদি(db.shutdown){
		প্রত্যাগম;
	}

	opts = &db.config->security_options;
	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}

}
