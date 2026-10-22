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
	स_रखो(&event_data, 0, आकार(event_data));

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_restore, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RESTORE, &event_data, cb_base->userdata);
	}
}


शून्य plugin_persist__handle_client_add(संरचना mosquitto *context)
{
	संरचना mosquitto_evt_persist_client event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जर(db.shutdown || context->is_persisted){
		परत;
	}

	opts = &db.config->security_options;
	स_रखो(&event_data, 0, आकार(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	जर(context->listener){
		event_data.data.listener_port = context->listener->port;
	}अन्यथा{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ADD, &event_data, cb_base->userdata);
	}

	जर(context->will){
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

	जर(db.shutdown){
		परत;
	}

	opts = &db.config->security_options;
	स_रखो(&event_data, 0, आकार(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	जर(context->listener){
		event_data.data.listener_port = context->listener->port;
	}अन्यथा{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_data, cb_base->userdata);
	}

	जर(context->will){
		plugin_persist__handle_will_add(context);
	}अन्यथा{
		plugin_persist__handle_will_delete(context);
	}
}


शून्य plugin_persist__handle_client_delete(संरचना mosquitto *context)
{
	संरचना mosquitto_evt_persist_client event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जर(context->id == रिक्त
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		परत;
	}

	plugin_persist__handle_will_delete(context);

	जर(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		परत;
	}

	opts = &db.config->security_options;
	स_रखो(&event_data, 0, आकार(event_data));
	event_data.data.clientid = context->id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_data, cb_base->userdata);
	}
	context->is_persisted = false;
}


शून्य plugin_persist__handle_subscription_add(संरचना mosquitto *context, स्थिरांक संरचना mosquitto_subscription *sub)
{
	संरचना mosquitto_evt_persist_subscription event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जर(db.shutdown || context->is_persisted == false){
		परत;
	}

	opts = &db.config->security_options;
	स_रखो(&event_data, 0, आकार(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub->topic_filter;
	event_data.data.identifier = sub->identifier;
	event_data.data.options = sub->options;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD, &event_data, cb_base->userdata);
	}
}


शून्य plugin_persist__handle_subscription_delete(संरचना mosquitto *context, अक्षर *sub)
{
	संरचना mosquitto_evt_persist_subscription event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जर(db.shutdown || context->is_persisted == false){
		परत;
	}
	जर(!sub){
		परत;
	}

	opts = &db.config->security_options;
	स_रखो(&event_data, 0, आकार(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_data, cb_base->userdata);
	}
}


स्थिर ओळीत शून्य set_client_संदेश_event_data(संरचना mosquitto_evt_persist_client_संदेश *event_data, संरचना mosquitto *context, स्थिरांक संरचना mosquitto_client_संदेश *client_संदेश)
{
	event_data->data.clientid = context->id;
	event_data->data.cmsg_id = client_संदेश->data.cmsg_id;
	event_data->data.direction = (uint8_t)client_संदेश->data.direction;
	event_data->data.dup = client_संदेश->data.dup;
	event_data->data.mid = client_संदेश->data.mid;
	event_data->data.qos = client_संदेश->data.qos;
	event_data->data.retain = client_संदेश->data.retain;
	event_data->data.state = (uint8_t)client_संदेश->data.state;
	event_data->data.store_id = client_संदेश->base_संदेश->data.store_id;
	event_data->data.subscription_identifier = client_संदेश->data.subscription_identifier;
}


शून्य plugin_persist_handle_client_संदेश_add(संरचना mosquitto *context, स्थिरांक संरचना mosquitto_client_संदेश *client_संदेश)
{
	संरचना mosquitto_evt_persist_client_संदेश event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जर(context->is_persisted == false
			|| (client_संदेश->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		परत;
	}

	opts = &db.config->security_options;
	स_रखो(&event_data, 0, आकार(event_data));

	set_client_संदेश_event_data(&event_data, context, client_संदेश);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_संदेश_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_संदेश_ADD, &event_data, cb_base->userdata);
	}
}


शून्य plugin_persist_handle_client_संदेश_delete(संरचना mosquitto *context, स्थिरांक संरचना mosquitto_client_संदेश *client_संदेश)
{
	संरचना mosquitto_evt_persist_client_संदेश event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जर(context->is_persisted == false
			|| (client_संदेश->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		परत;
	}

	opts = &db.config->security_options;
	स_रखो(&event_data, 0, आकार(event_data));

	set_client_संदेश_event_data(&event_data, context, client_संदेश);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_संदेश_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_संदेश_DELETE, &event_data, cb_base->userdata);
	}
}


शून्य plugin_persist_handle_client_संदेश_update(संरचना mosquitto *context, स्थिरांक संरचना mosquitto_client_संदेश *client_संदेश)
{
	संरचना mosquitto_evt_persist_client_संदेश event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जर(context->is_persisted == false
			|| (client_संदेश->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		परत;
	}

	opts = &db.config->security_options;
	स_रखो(&event_data, 0, आकार(event_data));

	set_client_संदेश_event_data(&event_data, context, client_संदेश);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_संदेश_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_संदेश_UPDATE, &event_data, cb_base->userdata);
	}
}


शून्य plugin_persist_handle_base_संदेश_add(संरचना mosquitto_base_संदेश *base_संदेश)
{
	संरचना mosquitto_evt_persist_base_संदेश event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जर(base_संदेश->stored || db.shutdown){
		परत;
	}

	opts = &db.config->security_options;
	स_रखो(&event_data, 0, आकार(event_data));

	event_data.data.store_id = base_संदेश->data.store_id;
	event_data.data.expiry_time = base_संदेश->data.expiry_time;
	event_data.data.topic = base_संदेश->data.topic;
	event_data.data.payload = base_संदेश->data.payload;
	event_data.data.source_id = base_संदेश->data.source_id;
	event_data.data.source_username = base_संदेश->data.source_username;
	event_data.data.properties = base_संदेश->data.properties;
	event_data.data.payloadlen = base_संदेश->data.payloadlen;
	event_data.data.source_mid = base_संदेश->data.source_mid;
	जर(base_संदेश->source_listener){
		event_data.data.source_port = base_संदेश->source_listener->port;
	}अन्यथा{
		event_data.data.source_port = 0;
	}
	event_data.data.qos = base_संदेश->data.qos;
	event_data.data.retain = base_संदेश->data.retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_संदेश_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_संदेश_ADD, &event_data, cb_base->userdata);
	}
	base_संदेश->stored = true;
}


शून्य plugin_persist_handle_base_संदेश_delete(संरचना mosquitto_base_संदेश *base_संदेश)
{
	संरचना mosquitto_evt_persist_base_संदेश event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जर(base_संदेश->stored == false || db.shutdown){
		परत;
	}

	opts = &db.config->security_options;
	स_रखो(&event_data, 0, आकार(event_data));

	event_data.data.store_id = base_संदेश->data.store_id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_संदेश_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_संदेश_DELETE, &event_data, cb_base->userdata);
	}
	base_संदेश->stored = false;
}


शून्य plugin_persist_handle_retain_संदेश_set(संरचना mosquitto_base_संदेश *base_संदेश)
{
	संरचना mosquitto_evt_persist_retain_संदेश event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जर(db.shutdown){
		परत;
	}

	opts = &db.config->security_options;
	स_रखो(&event_data, 0, आकार(event_data));

	event_data.store_id = base_संदेश->data.store_id;
	event_data.topic = base_संदेश->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_संदेश_set, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_संदेश_SET, &event_data, cb_base->userdata);
	}
}


शून्य plugin_persist_handle_retain_संदेश_delete(संरचना mosquitto_base_संदेश *base_संदेश)
{
	संरचना mosquitto_evt_persist_retain_संदेश event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	जर(db.shutdown){
		परत;
	}

	opts = &db.config->security_options;
	स_रखो(&event_data, 0, आकार(event_data));

	event_data.topic = base_संदेश->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_संदेश_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_संदेश_DELETE, &event_data, cb_base->userdata);
	}
}


शून्य plugin_persist__handle_will_add(संरचना mosquitto *context)
{
	संरचना mosquitto_evt_persist_will_संदेश event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;
	संरचना mosquitto_message *will_संदेश;

	जर(db.shutdown || !context->will){
		परत;
	}

	opts = &db.config->security_options;
	will_संदेश = &context->will->संदेश;
	स_रखो(&event_data, 0, आकार(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic = will_संदेश->topic;
	event_data.data.payload = will_संदेश->payload;
	event_data.data.payloadlen = (uint32_t)will_संदेश->payloadlen;
	event_data.data.qos = (uint8_t)will_संदेश->qos;
	event_data.data.retain = will_संदेश->retain;
	event_data.data.properties = context->will->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}
}


शून्य plugin_persist__handle_will_delete(संरचना mosquitto *context)
{
	संरचना mosquitto_evt_persist_will_संदेश event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	संरचना mosquitto__security_options *opts;

	स_रखो(&event_data, 0, आकार(event_data));
	event_data.data.clientid = context->id;

	जर(db.shutdown){
		परत;
	}

	opts = &db.config->security_options;
	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}

}
