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

#अन्तर्भाव "config.h"

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto_internal.h"
#अन्तर्भाव "mosquitto/broker.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "send_mosq.h"
#अन्तर्भाव "util_mosq.h"
#अन्तर्भाव "utlist.h"
#अन्तर्भाव "lib_load.h"
#अन्तर्भाव "will_mosq.h"
#अन्तर्भाव <stdint.h>


शून्य plugin_persist__handle_restore(शून्य)
{
	संरचना mosquitto_evt_persist_restore event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	opts = &db.config->security_options;
	memset(&event_data, 0, परिमाण(event_data));

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_restore, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RESTORE, &event_data, cb_base->userdata);
	}
}


शून्य plugin_persist__handle_client_add(संरचना mosquitto *context)
{
	संरचना mosquitto_evt_persist_client event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जइ(db.shutdown || context->is_persisted){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, परिमाण(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	जइ(context->listener){
		event_data.data.listener_port = context->listener->port;
	}अण्णहा{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ADD, &event_data, cb_base->userdata);
	}

	जइ(context->will){
		plugin_persist__handle_will_add(context);
	}

	context->is_persisted = true;
}


शून्य plugin_persist__handle_client_update(संरचना mosquitto *context)
{
	संरचना mosquitto_evt_persist_client event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;
	संरचना mosquitto_message_v5 will;

	UNUSED(will); /* FIXME */

	जइ(db.shutdown){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, परिमाण(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	जइ(context->listener){
		event_data.data.listener_port = context->listener->port;
	}अण्णहा{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_data, cb_base->userdata);
	}

	जइ(context->will){
		plugin_persist__handle_will_add(context);
	}अण्णहा{
		plugin_persist__handle_will_delete(context);
	}
}


शून्य plugin_persist__handle_client_delete(संरचना mosquitto *context)
{
	संरचना mosquitto_evt_persist_client event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जइ(context->id == रिक्त
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		प्रत्यागम;
	}

	plugin_persist__handle_will_delete(context);

	जइ(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, परिमाण(event_data));
	event_data.data.clientid = context->id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_data, cb_base->userdata);
	}
	context->is_persisted = false;
}


शून्य plugin_persist__handle_subscription_add(संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub)
{
	संरचना mosquitto_evt_persist_subscription event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जइ(db.shutdown || context->is_persisted == false){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, परिमाण(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub->topic_filter;
	event_data.data.identifier = sub->identifier;
	event_data.data.options = sub->options;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD, &event_data, cb_base->userdata);
	}
}


शून्य plugin_persist__handle_subscription_delete(संरचना mosquitto *context, वर्ण *sub)
{
	संरचना mosquitto_evt_persist_subscription event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जइ(db.shutdown || context->is_persisted == false){
		प्रत्यागम;
	}
	जइ(!sub){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, परिमाण(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_data, cb_base->userdata);
	}
}


स्थिर अन्तःस्थ शून्य set_client_सन्देशः_event_data(संरचना mosquitto_evt_persist_client_सन्देशः *event_data, संरचना mosquitto *context, ध्रुव संरचना mosquitto_client_सन्देशः *client_सन्देशः)
{
	event_data->data.clientid = context->id;
	event_data->data.cmsg_id = client_सन्देशः->data.cmsg_id;
	event_data->data.direction = (uint8_t)client_सन्देशः->data.direction;
	event_data->data.dup = client_सन्देशः->data.dup;
	event_data->data.mid = client_सन्देशः->data.mid;
	event_data->data.qos = client_सन्देशः->data.qos;
	event_data->data.retain = client_सन्देशः->data.retain;
	event_data->data.state = (uint8_t)client_सन्देशः->data.state;
	event_data->data.store_id = client_सन्देशः->base_सन्देशः->data.store_id;
	event_data->data.subscription_identifier = client_सन्देशः->data.subscription_identifier;
}


शून्य plugin_persist_handle_client_सन्देशः_add(संरचना mosquitto *context, ध्रुव संरचना mosquitto_client_सन्देशः *client_सन्देशः)
{
	संरचना mosquitto_evt_persist_client_सन्देशः event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जइ(context->is_persisted == false
			|| (client_सन्देशः->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		प्रत्यागम;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, परिमाण(event_data));

	set_client_सन्देशः_event_data(&event_data, context, client_सन्देशः);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_सन्देशः_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_सन्देशः_ADD, &event_data, cb_base->userdata);
	}
}


शून्य plugin_persist_handle_client_सन्देशः_delete(संरचना mosquitto *context, ध्रुव संरचना mosquitto_client_सन्देशः *client_सन्देशः)
{
	संरचना mosquitto_evt_persist_client_सन्देशः event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जइ(context->is_persisted == false
			|| (client_सन्देशः->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		प्रत्यागम;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, परिमाण(event_data));

	set_client_सन्देशः_event_data(&event_data, context, client_सन्देशः);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_सन्देशः_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_सन्देशः_DELETE, &event_data, cb_base->userdata);
	}
}


शून्य plugin_persist_handle_client_सन्देशः_update(संरचना mosquitto *context, ध्रुव संरचना mosquitto_client_सन्देशः *client_सन्देशः)
{
	संरचना mosquitto_evt_persist_client_सन्देशः event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जइ(context->is_persisted == false
			|| (client_सन्देशः->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		प्रत्यागम;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, परिमाण(event_data));

	set_client_सन्देशः_event_data(&event_data, context, client_सन्देशः);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_सन्देशः_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_सन्देशः_UPDATE, &event_data, cb_base->userdata);
	}
}


शून्य plugin_persist_handle_base_सन्देशः_add(संरचना mosquitto_base_सन्देशः *base_सन्देशः)
{
	संरचना mosquitto_evt_persist_base_सन्देशः event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जइ(base_सन्देशः->stored || db.shutdown){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, परिमाण(event_data));

	event_data.data.store_id = base_सन्देशः->data.store_id;
	event_data.data.expiry_time = base_सन्देशः->data.expiry_time;
	event_data.data.topic = base_सन्देशः->data.topic;
	event_data.data.payload = base_सन्देशः->data.payload;
	event_data.data.source_id = base_सन्देशः->data.source_id;
	event_data.data.source_username = base_सन्देशः->data.source_username;
	event_data.data.properties = base_सन्देशः->data.properties;
	event_data.data.payloadlen = base_सन्देशः->data.payloadlen;
	event_data.data.source_mid = base_सन्देशः->data.source_mid;
	जइ(base_सन्देशः->source_listener){
		event_data.data.source_port = base_सन्देशः->source_listener->port;
	}अण्णहा{
		event_data.data.source_port = 0;
	}
	event_data.data.qos = base_सन्देशः->data.qos;
	event_data.data.retain = base_सन्देशः->data.retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_सन्देशः_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_सन्देशः_ADD, &event_data, cb_base->userdata);
	}
	base_सन्देशः->stored = true;
}


शून्य plugin_persist_handle_base_सन्देशः_delete(संरचना mosquitto_base_सन्देशः *base_सन्देशः)
{
	संरचना mosquitto_evt_persist_base_सन्देशः event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जइ(base_सन्देशः->stored == false || db.shutdown){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, परिमाण(event_data));

	event_data.data.store_id = base_सन्देशः->data.store_id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_सन्देशः_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_सन्देशः_DELETE, &event_data, cb_base->userdata);
	}
	base_सन्देशः->stored = false;
}


शून्य plugin_persist_handle_retain_सन्देशः_set(संरचना mosquitto_base_सन्देशः *base_सन्देशः)
{
	संरचना mosquitto_evt_persist_retain_सन्देशः event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जइ(db.shutdown){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, परिमाण(event_data));

	event_data.store_id = base_सन्देशः->data.store_id;
	event_data.topic = base_सन्देशः->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_सन्देशः_set, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_सन्देशः_SET, &event_data, cb_base->userdata);
	}
}


शून्य plugin_persist_handle_retain_सन्देशः_delete(संरचना mosquitto_base_सन्देशः *base_सन्देशः)
{
	संरचना mosquitto_evt_persist_retain_सन्देशः event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जइ(db.shutdown){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, परिमाण(event_data));

	event_data.topic = base_सन्देशः->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_सन्देशः_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_सन्देशः_DELETE, &event_data, cb_base->userdata);
	}
}


शून्य plugin_persist__handle_will_add(संरचना mosquitto *context)
{
	संरचना mosquitto_evt_persist_will_सन्देशः event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;
	संरचना mosquitto_message *will_सन्देशः;

	जइ(db.shutdown || !context->will){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	will_सन्देशः = &context->will->सन्देशः;
	memset(&event_data, 0, परिमाण(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic = will_सन्देशः->topic;
	event_data.data.payload = will_सन्देशः->payload;
	event_data.data.payloadlen = (uint32_t)will_सन्देशः->payloadlen;
	event_data.data.qos = (uint8_t)will_सन्देशः->qos;
	event_data.data.retain = will_सन्देशः->retain;
	event_data.data.properties = context->will->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}
}


शून्य plugin_persist__handle_will_delete(संरचना mosquitto *context)
{
	संरचना mosquitto_evt_persist_will_सन्देशः event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	memset(&event_data, 0, परिमाण(event_data));
	event_data.data.clientid = context->id;

	जइ(db.shutdown){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}

}
