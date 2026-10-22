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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/broker.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "send_mosq.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"
#ଅନ୍ତର୍ଭାଵ "utlist.h"
#ଅନ୍ତର୍ଭାଵ "lib_load.h"
#ଅନ୍ତର୍ଭାଵ "will_mosq.h"
#ଅନ୍ତର୍ଭାଵ <stdint.h>


ଶୂନ୍ୟ plugin_persist__handle_restore(ଶୂନ୍ୟ)
{
	ସଂରଚନା mosquitto_evt_persist_restore event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ସଂରଚନା mosquitto__security_options *opts;

	opts = &db.config->security_options;
	memset(&event_data, 0, ଆକାର(event_data));

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_restore, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RESTORE, &event_data, cb_base->userdata);
	}
}


ଶୂନ୍ୟ plugin_persist__handle_client_add(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା mosquitto_evt_persist_client event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ସଂରଚନା mosquitto__security_options *opts;

	ଯଦି(db.shutdown || context->is_persisted){
		ଫେରନ୍ତୁ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ଆକାର(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	ଯଦି(context->listener){
		event_data.data.listener_port = context->listener->port;
	}ଅନ୍ୟଥା{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ADD, &event_data, cb_base->userdata);
	}

	ଯଦି(context->will){
		plugin_persist__handle_will_add(context);
	}

	context->is_persisted = true;
}


ଶୂନ୍ୟ plugin_persist__handle_client_update(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା mosquitto_evt_persist_client event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ସଂରଚନା mosquitto__security_options *opts;
	ସଂରଚନା mosquitto_message_v5 will;

	UNUSED(will); /* FIXME */

	ଯଦି(db.shutdown){
		ଫେରନ୍ତୁ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ଆକାର(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	ଯଦି(context->listener){
		event_data.data.listener_port = context->listener->port;
	}ଅନ୍ୟଥା{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_data, cb_base->userdata);
	}

	ଯଦି(context->will){
		plugin_persist__handle_will_add(context);
	}ଅନ୍ୟଥା{
		plugin_persist__handle_will_delete(context);
	}
}


ଶୂନ୍ୟ plugin_persist__handle_client_delete(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା mosquitto_evt_persist_client event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ସଂରଚନା mosquitto__security_options *opts;

	ଯଦି(context->id == ଖାଲି
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		ଫେରନ୍ତୁ;
	}

	plugin_persist__handle_will_delete(context);

	ଯଦି(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		ଫେରନ୍ତୁ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ଆକାର(event_data));
	event_data.data.clientid = context->id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_data, cb_base->userdata);
	}
	context->is_persisted = false;
}


ଶୂନ୍ୟ plugin_persist__handle_subscription_add(ସଂରଚନା mosquitto *context, ଅଚଳ ସଂରଚନା mosquitto_subscription *sub)
{
	ସଂରଚନା mosquitto_evt_persist_subscription event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ସଂରଚନା mosquitto__security_options *opts;

	ଯଦି(db.shutdown || context->is_persisted == false){
		ଫେରନ୍ତୁ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ଆକାର(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub->topic_filter;
	event_data.data.identifier = sub->identifier;
	event_data.data.options = sub->options;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD, &event_data, cb_base->userdata);
	}
}


ଶୂନ୍ୟ plugin_persist__handle_subscription_delete(ସଂରଚନା mosquitto *context, ଅକ୍ଷର *sub)
{
	ସଂରଚନା mosquitto_evt_persist_subscription event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ସଂରଚନା mosquitto__security_options *opts;

	ଯଦି(db.shutdown || context->is_persisted == false){
		ଫେରନ୍ତୁ;
	}
	ଯଦି(!sub){
		ଫେରନ୍ତୁ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ଆକାର(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_data, cb_base->userdata);
	}
}


ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ set_client_ସନ୍ଦେଶ_event_data(ସଂରଚନା mosquitto_evt_persist_client_ସନ୍ଦେଶ *event_data, ସଂରଚନା mosquitto *context, ଅଚଳ ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ)
{
	event_data->data.clientid = context->id;
	event_data->data.cmsg_id = client_ସନ୍ଦେଶ->data.cmsg_id;
	event_data->data.direction = (uint8_t)client_ସନ୍ଦେଶ->data.direction;
	event_data->data.dup = client_ସନ୍ଦେଶ->data.dup;
	event_data->data.mid = client_ସନ୍ଦେଶ->data.mid;
	event_data->data.qos = client_ସନ୍ଦେଶ->data.qos;
	event_data->data.retain = client_ସନ୍ଦେଶ->data.retain;
	event_data->data.state = (uint8_t)client_ସନ୍ଦେଶ->data.state;
	event_data->data.store_id = client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.store_id;
	event_data->data.subscription_identifier = client_ସନ୍ଦେଶ->data.subscription_identifier;
}


ଶୂନ୍ୟ plugin_persist_handle_client_ସନ୍ଦେଶ_add(ସଂରଚନା mosquitto *context, ଅଚଳ ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ)
{
	ସଂରଚନା mosquitto_evt_persist_client_ସନ୍ଦେଶ event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ସଂରଚନା mosquitto__security_options *opts;

	ଯଦି(context->is_persisted == false
			|| (client_ସନ୍ଦେଶ->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		ଫେରନ୍ତୁ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ଆକାର(event_data));

	set_client_ସନ୍ଦେଶ_event_data(&event_data, context, client_ସନ୍ଦେଶ);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_ସନ୍ଦେଶ_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ସନ୍ଦେଶ_ADD, &event_data, cb_base->userdata);
	}
}


ଶୂନ୍ୟ plugin_persist_handle_client_ସନ୍ଦେଶ_delete(ସଂରଚନା mosquitto *context, ଅଚଳ ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ)
{
	ସଂରଚନା mosquitto_evt_persist_client_ସନ୍ଦେଶ event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ସଂରଚନା mosquitto__security_options *opts;

	ଯଦି(context->is_persisted == false
			|| (client_ସନ୍ଦେଶ->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		ଫେରନ୍ତୁ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ଆକାର(event_data));

	set_client_ସନ୍ଦେଶ_event_data(&event_data, context, client_ସନ୍ଦେଶ);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_ସନ୍ଦେଶ_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ସନ୍ଦେଶ_DELETE, &event_data, cb_base->userdata);
	}
}


ଶୂନ୍ୟ plugin_persist_handle_client_ସନ୍ଦେଶ_update(ସଂରଚନା mosquitto *context, ଅଚଳ ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ)
{
	ସଂରଚନା mosquitto_evt_persist_client_ସନ୍ଦେଶ event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ସଂରଚନା mosquitto__security_options *opts;

	ଯଦି(context->is_persisted == false
			|| (client_ସନ୍ଦେଶ->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		ଫେରନ୍ତୁ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ଆକାର(event_data));

	set_client_ସନ୍ଦେଶ_event_data(&event_data, context, client_ସନ୍ଦେଶ);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_ସନ୍ଦେଶ_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ସନ୍ଦେଶ_UPDATE, &event_data, cb_base->userdata);
	}
}


ଶୂନ୍ୟ plugin_persist_handle_base_ସନ୍ଦେଶ_add(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ)
{
	ସଂରଚନା mosquitto_evt_persist_base_ସନ୍ଦେଶ event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ସଂରଚନା mosquitto__security_options *opts;

	ଯଦି(base_ସନ୍ଦେଶ->stored || db.shutdown){
		ଫେରନ୍ତୁ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ଆକାର(event_data));

	event_data.data.store_id = base_ସନ୍ଦେଶ->data.store_id;
	event_data.data.expiry_time = base_ସନ୍ଦେଶ->data.expiry_time;
	event_data.data.topic = base_ସନ୍ଦେଶ->data.topic;
	event_data.data.payload = base_ସନ୍ଦେଶ->data.payload;
	event_data.data.source_id = base_ସନ୍ଦେଶ->data.source_id;
	event_data.data.source_username = base_ସନ୍ଦେଶ->data.source_username;
	event_data.data.properties = base_ସନ୍ଦେଶ->data.properties;
	event_data.data.payloadlen = base_ସନ୍ଦେଶ->data.payloadlen;
	event_data.data.source_mid = base_ସନ୍ଦେଶ->data.source_mid;
	ଯଦି(base_ସନ୍ଦେଶ->source_listener){
		event_data.data.source_port = base_ସନ୍ଦେଶ->source_listener->port;
	}ଅନ୍ୟଥା{
		event_data.data.source_port = 0;
	}
	event_data.data.qos = base_ସନ୍ଦେଶ->data.qos;
	event_data.data.retain = base_ସନ୍ଦେଶ->data.retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_ସନ୍ଦେଶ_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_ସନ୍ଦେଶ_ADD, &event_data, cb_base->userdata);
	}
	base_ସନ୍ଦେଶ->stored = true;
}


ଶୂନ୍ୟ plugin_persist_handle_base_ସନ୍ଦେଶ_delete(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ)
{
	ସଂରଚନା mosquitto_evt_persist_base_ସନ୍ଦେଶ event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ସଂରଚନା mosquitto__security_options *opts;

	ଯଦି(base_ସନ୍ଦେଶ->stored == false || db.shutdown){
		ଫେରନ୍ତୁ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ଆକାର(event_data));

	event_data.data.store_id = base_ସନ୍ଦେଶ->data.store_id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_ସନ୍ଦେଶ_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_ସନ୍ଦେଶ_DELETE, &event_data, cb_base->userdata);
	}
	base_ସନ୍ଦେଶ->stored = false;
}


ଶୂନ୍ୟ plugin_persist_handle_retain_ସନ୍ଦେଶ_set(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ)
{
	ସଂରଚନା mosquitto_evt_persist_retain_ସନ୍ଦେଶ event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ସଂରଚନା mosquitto__security_options *opts;

	ଯଦି(db.shutdown){
		ଫେରନ୍ତୁ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ଆକାର(event_data));

	event_data.store_id = base_ସନ୍ଦେଶ->data.store_id;
	event_data.topic = base_ସନ୍ଦେଶ->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_ସନ୍ଦେଶ_set, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_ସନ୍ଦେଶ_SET, &event_data, cb_base->userdata);
	}
}


ଶୂନ୍ୟ plugin_persist_handle_retain_ସନ୍ଦେଶ_delete(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ)
{
	ସଂରଚନା mosquitto_evt_persist_retain_ସନ୍ଦେଶ event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ସଂରଚନା mosquitto__security_options *opts;

	ଯଦି(db.shutdown){
		ଫେରନ୍ତୁ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ଆକାର(event_data));

	event_data.topic = base_ସନ୍ଦେଶ->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_ସନ୍ଦେଶ_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_ସନ୍ଦେଶ_DELETE, &event_data, cb_base->userdata);
	}
}


ଶୂନ୍ୟ plugin_persist__handle_will_add(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା mosquitto_evt_persist_will_ସନ୍ଦେଶ event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ସଂରଚନା mosquitto__security_options *opts;
	ସଂରଚନା mosquitto_message *will_ସନ୍ଦେଶ;

	ଯଦି(db.shutdown || !context->will){
		ଫେରନ୍ତୁ;
	}

	opts = &db.config->security_options;
	will_ସନ୍ଦେଶ = &context->will->ସନ୍ଦେଶ;
	memset(&event_data, 0, ଆକାର(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic = will_ସନ୍ଦେଶ->topic;
	event_data.data.payload = will_ସନ୍ଦେଶ->payload;
	event_data.data.payloadlen = (uint32_t)will_ସନ୍ଦେଶ->payloadlen;
	event_data.data.qos = (uint8_t)will_ସନ୍ଦେଶ->qos;
	event_data.data.retain = will_ସନ୍ଦେଶ->retain;
	event_data.data.properties = context->will->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}
}


ଶୂନ୍ୟ plugin_persist__handle_will_delete(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା mosquitto_evt_persist_will_ସନ୍ଦେଶ event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ସଂରଚନା mosquitto__security_options *opts;

	memset(&event_data, 0, ଆକାର(event_data));
	event_data.data.clientid = context->id;

	ଯଦି(db.shutdown){
		ଫେରନ୍ତୁ;
	}

	opts = &db.config->security_options;
	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}

}
