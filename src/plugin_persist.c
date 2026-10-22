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

#समावेश "config.h"

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto_internal.h"
#समावेश "mosquitto/broker.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "send_mosq.h"
#समावेश "util_mosq.h"
#समावेश "utlist.h"
#समावेश "lib_load.h"
#समावेश "will_mosq.h"
#समावेश <stdint.h>


व्योम plugin_persist__handle_restore(व्योम)
{
	काष्ठा mosquitto_evt_persist_restore event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	काष्ठा mosquitto__security_options *opts;

	opts = &db.config->security_options;
	स_रखो(&event_तथ्य, 0, माप(event_तथ्य));

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_restore, cb_base, cb_अग्र){
		cb_base->cb(MOSQ_EVT_PERSIST_RESTORE, &event_तथ्य, cb_base->userdata);
	}
}


व्योम plugin_persist__handle_client_add(काष्ठा mosquitto *context)
{
	काष्ठा mosquitto_evt_persist_client event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	काष्ठा mosquitto__security_options *opts;

	अगर(db.shutdown || context->is_persisted){
		वापस;
	}

	opts = &db.config->security_options;
	स_रखो(&event_तथ्य, 0, माप(event_तथ्य));
	event_तथ्य.तथ्य.clientid = context->id;
	event_तथ्य.तथ्य.username = context->username;
	event_तथ्य.तथ्य.auth_method = context->auth_method;
	event_तथ्य.तथ्य.will_delay_time = context->will_delay_time;
	event_तथ्य.तथ्य.session_expiry_time = context->session_expiry_time;
	event_तथ्य.तथ्य.will_delay_interval = context->will_delay_interval;
	event_तथ्य.तथ्य.session_expiry_interval = context->session_expiry_interval;
	अगर(context->listener){
		event_तथ्य.तथ्य.listener_port = context->listener->port;
	}अन्यथा{
		event_तथ्य.तथ्य.listener_port = 0;
	}
	event_तथ्य.तथ्य.max_qos = context->max_qos;
	event_तथ्य.तथ्य.retain_available = context->retain_available;
	event_तथ्य.तथ्य.max_packet_आकार = context->maximum_packet_आकार;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_add, cb_base, cb_अग्र){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ADD, &event_तथ्य, cb_base->userdata);
	}

	अगर(context->will){
		plugin_persist__handle_will_add(context);
	}

	context->is_persisted = true;
}


व्योम plugin_persist__handle_client_update(काष्ठा mosquitto *context)
{
	काष्ठा mosquitto_evt_persist_client event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	काष्ठा mosquitto__security_options *opts;
	काष्ठा mosquitto_message_v5 will;

	UNUSED(will); /* FIXME */

	अगर(db.shutdown){
		वापस;
	}

	opts = &db.config->security_options;
	स_रखो(&event_तथ्य, 0, माप(event_तथ्य));
	event_तथ्य.तथ्य.clientid = context->id;
	event_तथ्य.तथ्य.username = context->username;
	event_तथ्य.तथ्य.auth_method = context->auth_method;
	event_तथ्य.तथ्य.will_delay_time = context->will_delay_time;
	event_तथ्य.तथ्य.session_expiry_time = context->session_expiry_time;
	event_तथ्य.तथ्य.will_delay_interval = context->will_delay_interval;
	event_तथ्य.तथ्य.session_expiry_interval = context->session_expiry_interval;
	अगर(context->listener){
		event_तथ्य.तथ्य.listener_port = context->listener->port;
	}अन्यथा{
		event_तथ्य.तथ्य.listener_port = 0;
	}
	event_तथ्य.तथ्य.max_qos = context->max_qos;
	event_तथ्य.तथ्य.retain_available = context->retain_available;
	event_तथ्य.तथ्य.max_packet_आकार = context->maximum_packet_आकार;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_update, cb_base, cb_अग्र){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_तथ्य, cb_base->userdata);
	}

	अगर(context->will){
		plugin_persist__handle_will_add(context);
	}अन्यथा{
		plugin_persist__handle_will_delete(context);
	}
}


व्योम plugin_persist__handle_client_delete(काष्ठा mosquitto *context)
{
	काष्ठा mosquitto_evt_persist_client event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	काष्ठा mosquitto__security_options *opts;

	अगर(context->id == शून्य
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		वापस;
	}

	plugin_persist__handle_will_delete(context);

	अगर(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		वापस;
	}

	opts = &db.config->security_options;
	स_रखो(&event_तथ्य, 0, माप(event_तथ्य));
	event_तथ्य.तथ्य.clientid = context->id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_delete, cb_base, cb_अग्र){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_तथ्य, cb_base->userdata);
	}
	context->is_persisted = false;
}


व्योम plugin_persist__handle_subscription_add(काष्ठा mosquitto *context, स्थिर काष्ठा mosquitto_subscription *sub)
{
	काष्ठा mosquitto_evt_persist_subscription event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	काष्ठा mosquitto__security_options *opts;

	अगर(db.shutdown || context->is_persisted == false){
		वापस;
	}

	opts = &db.config->security_options;
	स_रखो(&event_तथ्य, 0, माप(event_तथ्य));
	event_तथ्य.तथ्य.clientid = context->id;
	event_तथ्य.तथ्य.topic_filter = sub->topic_filter;
	event_तथ्य.तथ्य.identifier = sub->identifier;
	event_तथ्य.तथ्य.options = sub->options;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_add, cb_base, cb_अग्र){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD, &event_तथ्य, cb_base->userdata);
	}
}


व्योम plugin_persist__handle_subscription_delete(काष्ठा mosquitto *context, अक्षर *sub)
{
	काष्ठा mosquitto_evt_persist_subscription event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	काष्ठा mosquitto__security_options *opts;

	अगर(db.shutdown || context->is_persisted == false){
		वापस;
	}
	अगर(!sub){
		वापस;
	}

	opts = &db.config->security_options;
	स_रखो(&event_तथ्य, 0, माप(event_तथ्य));
	event_तथ्य.तथ्य.clientid = context->id;
	event_तथ्य.तथ्य.topic_filter = sub;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_delete, cb_base, cb_अग्र){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_तथ्य, cb_base->userdata);
	}
}


अटल अंतरभूत व्योम set_client_संदेश_event_तथ्य(काष्ठा mosquitto_evt_persist_client_संदेश *event_तथ्य, काष्ठा mosquitto *context, स्थिर काष्ठा mosquitto_client_संदेश *client_संदेश)
{
	event_तथ्य->तथ्य.clientid = context->id;
	event_तथ्य->तथ्य.cmsg_id = client_संदेश->तथ्य.cmsg_id;
	event_तथ्य->तथ्य.direction = (uint8_t)client_संदेश->तथ्य.direction;
	event_तथ्य->तथ्य.dup = client_संदेश->तथ्य.dup;
	event_तथ्य->तथ्य.mid = client_संदेश->तथ्य.mid;
	event_तथ्य->तथ्य.qos = client_संदेश->तथ्य.qos;
	event_तथ्य->तथ्य.retain = client_संदेश->तथ्य.retain;
	event_तथ्य->तथ्य.state = (uint8_t)client_संदेश->तथ्य.state;
	event_तथ्य->तथ्य.store_id = client_संदेश->base_संदेश->तथ्य.store_id;
	event_तथ्य->तथ्य.subscription_identifier = client_संदेश->तथ्य.subscription_identifier;
}


व्योम plugin_persist_handle_client_संदेश_add(काष्ठा mosquitto *context, स्थिर काष्ठा mosquitto_client_संदेश *client_संदेश)
{
	काष्ठा mosquitto_evt_persist_client_संदेश event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	काष्ठा mosquitto__security_options *opts;

	अगर(context->is_persisted == false
			|| (client_संदेश->तथ्य.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		वापस;
	}

	opts = &db.config->security_options;
	स_रखो(&event_तथ्य, 0, माप(event_तथ्य));

	set_client_संदेश_event_तथ्य(&event_तथ्य, context, client_संदेश);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_संदेश_add, cb_base, cb_अग्र){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_संदेश_ADD, &event_तथ्य, cb_base->userdata);
	}
}


व्योम plugin_persist_handle_client_संदेश_delete(काष्ठा mosquitto *context, स्थिर काष्ठा mosquitto_client_संदेश *client_संदेश)
{
	काष्ठा mosquitto_evt_persist_client_संदेश event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	काष्ठा mosquitto__security_options *opts;

	अगर(context->is_persisted == false
			|| (client_संदेश->तथ्य.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		वापस;
	}

	opts = &db.config->security_options;
	स_रखो(&event_तथ्य, 0, माप(event_तथ्य));

	set_client_संदेश_event_तथ्य(&event_तथ्य, context, client_संदेश);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_संदेश_delete, cb_base, cb_अग्र){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_संदेश_DELETE, &event_तथ्य, cb_base->userdata);
	}
}


व्योम plugin_persist_handle_client_संदेश_update(काष्ठा mosquitto *context, स्थिर काष्ठा mosquitto_client_संदेश *client_संदेश)
{
	काष्ठा mosquitto_evt_persist_client_संदेश event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	काष्ठा mosquitto__security_options *opts;

	अगर(context->is_persisted == false
			|| (client_संदेश->तथ्य.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		वापस;
	}

	opts = &db.config->security_options;
	स_रखो(&event_तथ्य, 0, माप(event_तथ्य));

	set_client_संदेश_event_तथ्य(&event_तथ्य, context, client_संदेश);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_संदेश_update, cb_base, cb_अग्र){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_संदेश_UPDATE, &event_तथ्य, cb_base->userdata);
	}
}


व्योम plugin_persist_handle_base_संदेश_add(काष्ठा mosquitto_base_संदेश *base_संदेश)
{
	काष्ठा mosquitto_evt_persist_base_संदेश event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	काष्ठा mosquitto__security_options *opts;

	अगर(base_संदेश->stored || db.shutdown){
		वापस;
	}

	opts = &db.config->security_options;
	स_रखो(&event_तथ्य, 0, माप(event_तथ्य));

	event_तथ्य.तथ्य.store_id = base_संदेश->तथ्य.store_id;
	event_तथ्य.तथ्य.expiry_time = base_संदेश->तथ्य.expiry_time;
	event_तथ्य.तथ्य.topic = base_संदेश->तथ्य.topic;
	event_तथ्य.तथ्य.payload = base_संदेश->तथ्य.payload;
	event_तथ्य.तथ्य.source_id = base_संदेश->तथ्य.source_id;
	event_तथ्य.तथ्य.source_username = base_संदेश->तथ्य.source_username;
	event_तथ्य.तथ्य.properties = base_संदेश->तथ्य.properties;
	event_तथ्य.तथ्य.payloadlen = base_संदेश->तथ्य.payloadlen;
	event_तथ्य.तथ्य.source_mid = base_संदेश->तथ्य.source_mid;
	अगर(base_संदेश->source_listener){
		event_तथ्य.तथ्य.source_port = base_संदेश->source_listener->port;
	}अन्यथा{
		event_तथ्य.तथ्य.source_port = 0;
	}
	event_तथ्य.तथ्य.qos = base_संदेश->तथ्य.qos;
	event_तथ्य.तथ्य.retain = base_संदेश->तथ्य.retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_संदेश_add, cb_base, cb_अग्र){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_संदेश_ADD, &event_तथ्य, cb_base->userdata);
	}
	base_संदेश->stored = true;
}


व्योम plugin_persist_handle_base_संदेश_delete(काष्ठा mosquitto_base_संदेश *base_संदेश)
{
	काष्ठा mosquitto_evt_persist_base_संदेश event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	काष्ठा mosquitto__security_options *opts;

	अगर(base_संदेश->stored == false || db.shutdown){
		वापस;
	}

	opts = &db.config->security_options;
	स_रखो(&event_तथ्य, 0, माप(event_तथ्य));

	event_तथ्य.तथ्य.store_id = base_संदेश->तथ्य.store_id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_संदेश_delete, cb_base, cb_अग्र){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_संदेश_DELETE, &event_तथ्य, cb_base->userdata);
	}
	base_संदेश->stored = false;
}


व्योम plugin_persist_handle_retain_संदेश_set(काष्ठा mosquitto_base_संदेश *base_संदेश)
{
	काष्ठा mosquitto_evt_persist_retain_संदेश event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	काष्ठा mosquitto__security_options *opts;

	अगर(db.shutdown){
		वापस;
	}

	opts = &db.config->security_options;
	स_रखो(&event_तथ्य, 0, माप(event_तथ्य));

	event_तथ्य.store_id = base_संदेश->तथ्य.store_id;
	event_तथ्य.topic = base_संदेश->तथ्य.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_संदेश_set, cb_base, cb_अग्र){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_संदेश_SET, &event_तथ्य, cb_base->userdata);
	}
}


व्योम plugin_persist_handle_retain_संदेश_delete(काष्ठा mosquitto_base_संदेश *base_संदेश)
{
	काष्ठा mosquitto_evt_persist_retain_संदेश event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	काष्ठा mosquitto__security_options *opts;

	अगर(db.shutdown){
		वापस;
	}

	opts = &db.config->security_options;
	स_रखो(&event_तथ्य, 0, माप(event_तथ्य));

	event_तथ्य.topic = base_संदेश->तथ्य.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_संदेश_delete, cb_base, cb_अग्र){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_संदेश_DELETE, &event_तथ्य, cb_base->userdata);
	}
}


व्योम plugin_persist__handle_will_add(काष्ठा mosquitto *context)
{
	काष्ठा mosquitto_evt_persist_will_संदेश event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	काष्ठा mosquitto__security_options *opts;
	काष्ठा mosquitto_message *will_संदेश;

	अगर(db.shutdown || !context->will){
		वापस;
	}

	opts = &db.config->security_options;
	will_संदेश = &context->will->संदेश;
	स_रखो(&event_तथ्य, 0, माप(event_तथ्य));
	event_तथ्य.तथ्य.clientid = context->id;
	event_तथ्य.तथ्य.topic = will_संदेश->topic;
	event_तथ्य.तथ्य.payload = will_संदेश->payload;
	event_तथ्य.तथ्य.payloadlen = (uint32_t)will_संदेश->payloadlen;
	event_तथ्य.तथ्य.qos = (uint8_t)will_संदेश->qos;
	event_तथ्य.तथ्य.retain = will_संदेश->retain;
	event_तथ्य.तथ्य.properties = context->will->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_add, cb_base, cb_अग्र){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_तथ्य, cb_base->userdata);
	}
}


व्योम plugin_persist__handle_will_delete(काष्ठा mosquitto *context)
{
	काष्ठा mosquitto_evt_persist_will_संदेश event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	काष्ठा mosquitto__security_options *opts;

	स_रखो(&event_तथ्य, 0, माप(event_तथ्य));
	event_तथ्य.तथ्य.clientid = context->id;

	अगर(db.shutdown){
		वापस;
	}

	opts = &db.config->security_options;
	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_delete, cb_base, cb_अग्र){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_तथ्य, cb_base->userdata);
	}

}
