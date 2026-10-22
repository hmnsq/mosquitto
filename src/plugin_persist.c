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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/broker.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"
#ಅನ್ತರ್ಭಾವ "utlist.h"
#ಅನ್ತರ್ಭಾವ "lib_load.h"
#ಅನ್ತರ್ಭಾವ "will_mosq.h"
#ಅನ್ತರ್ಭಾವ <stdint.h>


ಶೂನ್ಯ plugin_persist__handle_restore(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ mosquitto_evt_persist_restore event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;
	ಸಂರಚನಾ mosquitto__security_options *opts;

	opts = &db.config->security_options;
	memset(&event_data, 0, ಪರಿಮಾಣ(event_data));

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_restore, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RESTORE, &event_data, cb_base->userdata);
	}
}


ಶೂನ್ಯ plugin_persist__handle_client_add(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ mosquitto_evt_persist_client event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;
	ಸಂರಚನಾ mosquitto__security_options *opts;

	ಯದಿ(db.shutdown || context->is_persisted){
		ಪ್ರತ್ಯಾಗಮ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ಪರಿಮಾಣ(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	ಯದಿ(context->listener){
		event_data.data.listener_port = context->listener->port;
	}ಅನ್ಯಥಾ{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ADD, &event_data, cb_base->userdata);
	}

	ಯದಿ(context->will){
		plugin_persist__handle_will_add(context);
	}

	context->is_persisted = true;
}


ಶೂನ್ಯ plugin_persist__handle_client_update(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ mosquitto_evt_persist_client event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;
	ಸಂರಚನಾ mosquitto__security_options *opts;
	ಸಂರಚನಾ mosquitto_message_v5 will;

	UNUSED(will); /* FIXME */

	ಯದಿ(db.shutdown){
		ಪ್ರತ್ಯಾಗಮ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ಪರಿಮಾಣ(event_data));
	event_data.data.clientid = context->id;
	event_data.data.username = context->username;
	event_data.data.auth_method = context->auth_method;
	event_data.data.will_delay_time = context->will_delay_time;
	event_data.data.session_expiry_time = context->session_expiry_time;
	event_data.data.will_delay_interval = context->will_delay_interval;
	event_data.data.session_expiry_interval = context->session_expiry_interval;
	ಯದಿ(context->listener){
		event_data.data.listener_port = context->listener->port;
	}ಅನ್ಯಥಾ{
		event_data.data.listener_port = 0;
	}
	event_data.data.max_qos = context->max_qos;
	event_data.data.retain_available = context->retain_available;
	event_data.data.max_packet_size = context->maximum_packet_size;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_data, cb_base->userdata);
	}

	ಯದಿ(context->will){
		plugin_persist__handle_will_add(context);
	}ಅನ್ಯಥಾ{
		plugin_persist__handle_will_delete(context);
	}
}


ಶೂನ್ಯ plugin_persist__handle_client_delete(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ mosquitto_evt_persist_client event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;
	ಸಂರಚನಾ mosquitto__security_options *opts;

	ಯದಿ(context->id == ರಿಕ್ತ
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		ಪ್ರತ್ಯಾಗಮ;
	}

	plugin_persist__handle_will_delete(context);

	ಯದಿ(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		ಪ್ರತ್ಯಾಗಮ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ಪರಿಮಾಣ(event_data));
	event_data.data.clientid = context->id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_data, cb_base->userdata);
	}
	context->is_persisted = false;
}


ಶೂನ್ಯ plugin_persist__handle_subscription_add(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಸಂರಚನಾ mosquitto_subscription *sub)
{
	ಸಂರಚನಾ mosquitto_evt_persist_subscription event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;
	ಸಂರಚನಾ mosquitto__security_options *opts;

	ಯದಿ(db.shutdown || context->is_persisted == false){
		ಪ್ರತ್ಯಾಗಮ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ಪರಿಮಾಣ(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub->topic_filter;
	event_data.data.identifier = sub->identifier;
	event_data.data.options = sub->options;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD, &event_data, cb_base->userdata);
	}
}


ಶೂನ್ಯ plugin_persist__handle_subscription_delete(ಸಂರಚನಾ mosquitto *context, ವರ್ಣ *sub)
{
	ಸಂರಚನಾ mosquitto_evt_persist_subscription event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;
	ಸಂರಚನಾ mosquitto__security_options *opts;

	ಯದಿ(db.shutdown || context->is_persisted == false){
		ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ(!sub){
		ಪ್ರತ್ಯಾಗಮ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ಪರಿಮಾಣ(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic_filter = sub;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_data, cb_base->userdata);
	}
}


ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ set_client_ಸಂದೇಶ_event_data(ಸಂರಚನಾ mosquitto_evt_persist_client_ಸಂದೇಶ *event_data, ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ)
{
	event_data->data.clientid = context->id;
	event_data->data.cmsg_id = client_ಸಂದೇಶ->data.cmsg_id;
	event_data->data.direction = (uint8_t)client_ಸಂದೇಶ->data.direction;
	event_data->data.dup = client_ಸಂದೇಶ->data.dup;
	event_data->data.mid = client_ಸಂದೇಶ->data.mid;
	event_data->data.qos = client_ಸಂದೇಶ->data.qos;
	event_data->data.retain = client_ಸಂದೇಶ->data.retain;
	event_data->data.state = (uint8_t)client_ಸಂದೇಶ->data.state;
	event_data->data.store_id = client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.store_id;
	event_data->data.subscription_identifier = client_ಸಂದೇಶ->data.subscription_identifier;
}


ಶೂನ್ಯ plugin_persist_handle_client_ಸಂದೇಶ_add(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ)
{
	ಸಂರಚನಾ mosquitto_evt_persist_client_ಸಂದೇಶ event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;
	ಸಂರಚನಾ mosquitto__security_options *opts;

	ಯದಿ(context->is_persisted == false
			|| (client_ಸಂದೇಶ->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		ಪ್ರತ್ಯಾಗಮ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ಪರಿಮಾಣ(event_data));

	set_client_ಸಂದೇಶ_event_data(&event_data, context, client_ಸಂದೇಶ);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_ಸಂದೇಶ_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_ADD, &event_data, cb_base->userdata);
	}
}


ಶೂನ್ಯ plugin_persist_handle_client_ಸಂದೇಶ_delete(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ)
{
	ಸಂರಚನಾ mosquitto_evt_persist_client_ಸಂದೇಶ event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;
	ಸಂರಚನಾ mosquitto__security_options *opts;

	ಯದಿ(context->is_persisted == false
			|| (client_ಸಂದೇಶ->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		ಪ್ರತ್ಯಾಗಮ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ಪರಿಮಾಣ(event_data));

	set_client_ಸಂದೇಶ_event_data(&event_data, context, client_ಸಂದೇಶ);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_ಸಂದೇಶ_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_DELETE, &event_data, cb_base->userdata);
	}
}


ಶೂನ್ಯ plugin_persist_handle_client_ಸಂದೇಶ_update(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ)
{
	ಸಂರಚನಾ mosquitto_evt_persist_client_ಸಂದೇಶ event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;
	ಸಂರಚನಾ mosquitto__security_options *opts;

	ಯದಿ(context->is_persisted == false
			|| (client_ಸಂದೇಶ->data.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		ಪ್ರತ್ಯಾಗಮ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ಪರಿಮಾಣ(event_data));

	set_client_ಸಂದೇಶ_event_data(&event_data, context, client_ಸಂದೇಶ);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_ಸಂದೇಶ_update, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_UPDATE, &event_data, cb_base->userdata);
	}
}


ಶೂನ್ಯ plugin_persist_handle_base_ಸಂದೇಶ_add(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ)
{
	ಸಂರಚನಾ mosquitto_evt_persist_base_ಸಂದೇಶ event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;
	ಸಂರಚನಾ mosquitto__security_options *opts;

	ಯದಿ(base_ಸಂದೇಶ->stored || db.shutdown){
		ಪ್ರತ್ಯಾಗಮ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ಪರಿಮಾಣ(event_data));

	event_data.data.store_id = base_ಸಂದೇಶ->data.store_id;
	event_data.data.expiry_time = base_ಸಂದೇಶ->data.expiry_time;
	event_data.data.topic = base_ಸಂದೇಶ->data.topic;
	event_data.data.payload = base_ಸಂದೇಶ->data.payload;
	event_data.data.source_id = base_ಸಂದೇಶ->data.source_id;
	event_data.data.source_username = base_ಸಂದೇಶ->data.source_username;
	event_data.data.properties = base_ಸಂದೇಶ->data.properties;
	event_data.data.payloadlen = base_ಸಂದೇಶ->data.payloadlen;
	event_data.data.source_mid = base_ಸಂದೇಶ->data.source_mid;
	ಯದಿ(base_ಸಂದೇಶ->source_listener){
		event_data.data.source_port = base_ಸಂದೇಶ->source_listener->port;
	}ಅನ್ಯಥಾ{
		event_data.data.source_port = 0;
	}
	event_data.data.qos = base_ಸಂದೇಶ->data.qos;
	event_data.data.retain = base_ಸಂದೇಶ->data.retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_ಸಂದೇಶ_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_ಸಂದೇಶ_ADD, &event_data, cb_base->userdata);
	}
	base_ಸಂದೇಶ->stored = true;
}


ಶೂನ್ಯ plugin_persist_handle_base_ಸಂದೇಶ_delete(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ)
{
	ಸಂರಚನಾ mosquitto_evt_persist_base_ಸಂದೇಶ event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;
	ಸಂರಚನಾ mosquitto__security_options *opts;

	ಯದಿ(base_ಸಂದೇಶ->stored == false || db.shutdown){
		ಪ್ರತ್ಯಾಗಮ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ಪರಿಮಾಣ(event_data));

	event_data.data.store_id = base_ಸಂದೇಶ->data.store_id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_ಸಂದೇಶ_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_ಸಂದೇಶ_DELETE, &event_data, cb_base->userdata);
	}
	base_ಸಂದೇಶ->stored = false;
}


ಶೂನ್ಯ plugin_persist_handle_retain_ಸಂದೇಶ_set(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ)
{
	ಸಂರಚನಾ mosquitto_evt_persist_retain_ಸಂದೇಶ event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;
	ಸಂರಚನಾ mosquitto__security_options *opts;

	ಯದಿ(db.shutdown){
		ಪ್ರತ್ಯಾಗಮ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ಪರಿಮಾಣ(event_data));

	event_data.store_id = base_ಸಂದೇಶ->data.store_id;
	event_data.topic = base_ಸಂದೇಶ->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_ಸಂದೇಶ_set, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_ಸಂದೇಶ_SET, &event_data, cb_base->userdata);
	}
}


ಶೂನ್ಯ plugin_persist_handle_retain_ಸಂದೇಶ_delete(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ)
{
	ಸಂರಚನಾ mosquitto_evt_persist_retain_ಸಂದೇಶ event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;
	ಸಂರಚನಾ mosquitto__security_options *opts;

	ಯದಿ(db.shutdown){
		ಪ್ರತ್ಯಾಗಮ;
	}

	opts = &db.config->security_options;
	memset(&event_data, 0, ಪರಿಮಾಣ(event_data));

	event_data.topic = base_ಸಂದೇಶ->data.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_ಸಂದೇಶ_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_ಸಂದೇಶ_DELETE, &event_data, cb_base->userdata);
	}
}


ಶೂನ್ಯ plugin_persist__handle_will_add(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ mosquitto_evt_persist_will_ಸಂದೇಶ event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;
	ಸಂರಚನಾ mosquitto__security_options *opts;
	ಸಂರಚನಾ mosquitto_message *will_ಸಂದೇಶ;

	ಯದಿ(db.shutdown || !context->will){
		ಪ್ರತ್ಯಾಗಮ;
	}

	opts = &db.config->security_options;
	will_ಸಂದೇಶ = &context->will->ಸಂದೇಶ;
	memset(&event_data, 0, ಪರಿಮಾಣ(event_data));
	event_data.data.clientid = context->id;
	event_data.data.topic = will_ಸಂದೇಶ->topic;
	event_data.data.payload = will_ಸಂದೇಶ->payload;
	event_data.data.payloadlen = (uint32_t)will_ಸಂದೇಶ->payloadlen;
	event_data.data.qos = (uint8_t)will_ಸಂದೇಶ->qos;
	event_data.data.retain = will_ಸಂದೇಶ->retain;
	event_data.data.properties = context->will->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_add, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}
}


ಶೂನ್ಯ plugin_persist__handle_will_delete(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ mosquitto_evt_persist_will_ಸಂದೇಶ event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;
	ಸಂರಚನಾ mosquitto__security_options *opts;

	memset(&event_data, 0, ಪರಿಮಾಣ(event_data));
	event_data.data.clientid = context->id;

	ಯದಿ(db.shutdown){
		ಪ್ರತ್ಯಾಗಮ;
	}

	opts = &db.config->security_options;
	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_delete, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_data, cb_base->userdata);
	}

}
