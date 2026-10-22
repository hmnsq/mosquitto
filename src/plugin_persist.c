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
	কাঠামো mosquitto_evt_persist_restore event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	কাঠামো mosquitto__security_options *opts;

	opts = &db.config->security_options;
	memset(&event_তথ্য, 0, আকার(event_তথ্য));

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_restore, cb_base, cb_পরবর্তী){
		cb_base->cb(MOSQ_EVT_PERSIST_RESTORE, &event_তথ্য, cb_base->userdata);
	}
}


শূন্য plugin_persist__handle_client_add(কাঠামো mosquitto *context)
{
	কাঠামো mosquitto_evt_persist_client event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	কাঠামো mosquitto__security_options *opts;

	যদি(db.shutdown || context->is_persisted){
		ফেরত;
	}

	opts = &db.config->security_options;
	memset(&event_তথ্য, 0, আকার(event_তথ্য));
	event_তথ্য.তথ্য.clientid = context->id;
	event_তথ্য.তথ্য.username = context->username;
	event_তথ্য.তথ্য.auth_method = context->auth_method;
	event_তথ্য.তথ্য.will_delay_time = context->will_delay_time;
	event_তথ্য.তথ্য.session_expiry_time = context->session_expiry_time;
	event_তথ্য.তথ্য.will_delay_interval = context->will_delay_interval;
	event_তথ্য.তথ্য.session_expiry_interval = context->session_expiry_interval;
	যদি(context->listener){
		event_তথ্য.তথ্য.listener_port = context->listener->port;
	}অন্যথা{
		event_তথ্য.তথ্য.listener_port = 0;
	}
	event_তথ্য.তথ্য.max_qos = context->max_qos;
	event_তথ্য.তথ্য.retain_available = context->retain_available;
	event_তথ্য.তথ্য.max_packet_আকার = context->maximum_packet_আকার;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_add, cb_base, cb_পরবর্তী){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_ADD, &event_তথ্য, cb_base->userdata);
	}

	যদি(context->will){
		plugin_persist__handle_will_add(context);
	}

	context->is_persisted = true;
}


শূন্য plugin_persist__handle_client_update(কাঠামো mosquitto *context)
{
	কাঠামো mosquitto_evt_persist_client event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	কাঠামো mosquitto__security_options *opts;
	কাঠামো mosquitto_message_v5 will;

	UNUSED(will); /* FIXME */

	যদি(db.shutdown){
		ফেরত;
	}

	opts = &db.config->security_options;
	memset(&event_তথ্য, 0, আকার(event_তথ্য));
	event_তথ্য.তথ্য.clientid = context->id;
	event_তথ্য.তথ্য.username = context->username;
	event_তথ্য.তথ্য.auth_method = context->auth_method;
	event_তথ্য.তথ্য.will_delay_time = context->will_delay_time;
	event_তথ্য.তথ্য.session_expiry_time = context->session_expiry_time;
	event_তথ্য.তথ্য.will_delay_interval = context->will_delay_interval;
	event_তথ্য.তথ্য.session_expiry_interval = context->session_expiry_interval;
	যদি(context->listener){
		event_তথ্য.তথ্য.listener_port = context->listener->port;
	}অন্যথা{
		event_তথ্য.তথ্য.listener_port = 0;
	}
	event_তথ্য.তথ্য.max_qos = context->max_qos;
	event_তথ্য.তথ্য.retain_available = context->retain_available;
	event_তথ্য.তথ্য.max_packet_আকার = context->maximum_packet_আকার;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_update, cb_base, cb_পরবর্তী){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_তথ্য, cb_base->userdata);
	}

	যদি(context->will){
		plugin_persist__handle_will_add(context);
	}অন্যথা{
		plugin_persist__handle_will_delete(context);
	}
}


শূন্য plugin_persist__handle_client_delete(কাঠামো mosquitto *context)
{
	কাঠামো mosquitto_evt_persist_client event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	কাঠামো mosquitto__security_options *opts;

	যদি(context->id == ফাঁকা
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		ফেরত;
	}

	plugin_persist__handle_will_delete(context);

	যদি(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){
		ফেরত;
	}

	opts = &db.config->security_options;
	memset(&event_তথ্য, 0, আকার(event_তথ্য));
	event_তথ্য.তথ্য.clientid = context->id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_delete, cb_base, cb_পরবর্তী){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_তথ্য, cb_base->userdata);
	}
	context->is_persisted = false;
}


শূন্য plugin_persist__handle_subscription_add(কাঠামো mosquitto *context, ধ্রুবক কাঠামো mosquitto_subscription *sub)
{
	কাঠামো mosquitto_evt_persist_subscription event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	কাঠামো mosquitto__security_options *opts;

	যদি(db.shutdown || context->is_persisted == false){
		ফেরত;
	}

	opts = &db.config->security_options;
	memset(&event_তথ্য, 0, আকার(event_তথ্য));
	event_তথ্য.তথ্য.clientid = context->id;
	event_তথ্য.তথ্য.topic_filter = sub->topic_filter;
	event_তথ্য.তথ্য.identifier = sub->identifier;
	event_তথ্য.তথ্য.options = sub->options;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_add, cb_base, cb_পরবর্তী){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD, &event_তথ্য, cb_base->userdata);
	}
}


শূন্য plugin_persist__handle_subscription_delete(কাঠামো mosquitto *context, অক্ষর *sub)
{
	কাঠামো mosquitto_evt_persist_subscription event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	কাঠামো mosquitto__security_options *opts;

	যদি(db.shutdown || context->is_persisted == false){
		ফেরত;
	}
	যদি(!sub){
		ফেরত;
	}

	opts = &db.config->security_options;
	memset(&event_তথ্য, 0, আকার(event_তথ্য));
	event_তথ্য.তথ্য.clientid = context->id;
	event_তথ্য.তথ্য.topic_filter = sub;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_delete, cb_base, cb_পরবর্তী){
		cb_base->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_তথ্য, cb_base->userdata);
	}
}


স্থির অন্তঃরেখ শূন্য set_client_বার্তা_event_তথ্য(কাঠামো mosquitto_evt_persist_client_বার্তা *event_তথ্য, কাঠামো mosquitto *context, ধ্রুবক কাঠামো mosquitto_client_বার্তা *client_বার্তা)
{
	event_তথ্য->তথ্য.clientid = context->id;
	event_তথ্য->তথ্য.cmsg_id = client_বার্তা->তথ্য.cmsg_id;
	event_তথ্য->তথ্য.direction = (uint8_t)client_বার্তা->তথ্য.direction;
	event_তথ্য->তথ্য.dup = client_বার্তা->তথ্য.dup;
	event_তথ্য->তথ্য.mid = client_বার্তা->তথ্য.mid;
	event_তথ্য->তথ্য.qos = client_বার্তা->তথ্য.qos;
	event_তথ্য->তথ্য.retain = client_বার্তা->তথ্য.retain;
	event_তথ্য->তথ্য.state = (uint8_t)client_বার্তা->তথ্য.state;
	event_তথ্য->তথ্য.store_id = client_বার্তা->base_বার্তা->তথ্য.store_id;
	event_তথ্য->তথ্য.subscription_identifier = client_বার্তা->তথ্য.subscription_identifier;
}


শূন্য plugin_persist_handle_client_বার্তা_add(কাঠামো mosquitto *context, ধ্রুবক কাঠামো mosquitto_client_বার্তা *client_বার্তা)
{
	কাঠামো mosquitto_evt_persist_client_বার্তা event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	কাঠামো mosquitto__security_options *opts;

	যদি(context->is_persisted == false
			|| (client_বার্তা->তথ্য.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		ফেরত;
	}

	opts = &db.config->security_options;
	memset(&event_তথ্য, 0, আকার(event_তথ্য));

	set_client_বার্তা_event_তথ্য(&event_তথ্য, context, client_বার্তা);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_বার্তা_add, cb_base, cb_পরবর্তী){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_বার্তা_ADD, &event_তথ্য, cb_base->userdata);
	}
}


শূন্য plugin_persist_handle_client_বার্তা_delete(কাঠামো mosquitto *context, ধ্রুবক কাঠামো mosquitto_client_বার্তা *client_বার্তা)
{
	কাঠামো mosquitto_evt_persist_client_বার্তা event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	কাঠামো mosquitto__security_options *opts;

	যদি(context->is_persisted == false
			|| (client_বার্তা->তথ্য.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		ফেরত;
	}

	opts = &db.config->security_options;
	memset(&event_তথ্য, 0, আকার(event_তথ্য));

	set_client_বার্তা_event_তথ্য(&event_তথ্য, context, client_বার্তা);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_বার্তা_delete, cb_base, cb_পরবর্তী){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_বার্তা_DELETE, &event_তথ্য, cb_base->userdata);
	}
}


শূন্য plugin_persist_handle_client_বার্তা_update(কাঠামো mosquitto *context, ধ্রুবক কাঠামো mosquitto_client_বার্তা *client_বার্তা)
{
	কাঠামো mosquitto_evt_persist_client_বার্তা event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	কাঠামো mosquitto__security_options *opts;

	যদি(context->is_persisted == false
			|| (client_বার্তা->তথ্য.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		ফেরত;
	}

	opts = &db.config->security_options;
	memset(&event_তথ্য, 0, আকার(event_তথ্য));

	set_client_বার্তা_event_তথ্য(&event_তথ্য, context, client_বার্তা);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_বার্তা_update, cb_base, cb_পরবর্তী){
		cb_base->cb(MOSQ_EVT_PERSIST_CLIENT_বার্তা_UPDATE, &event_তথ্য, cb_base->userdata);
	}
}


শূন্য plugin_persist_handle_base_বার্তা_add(কাঠামো mosquitto_base_বার্তা *base_বার্তা)
{
	কাঠামো mosquitto_evt_persist_base_বার্তা event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	কাঠামো mosquitto__security_options *opts;

	যদি(base_বার্তা->stored || db.shutdown){
		ফেরত;
	}

	opts = &db.config->security_options;
	memset(&event_তথ্য, 0, আকার(event_তথ্য));

	event_তথ্য.তথ্য.store_id = base_বার্তা->তথ্য.store_id;
	event_তথ্য.তথ্য.expiry_time = base_বার্তা->তথ্য.expiry_time;
	event_তথ্য.তথ্য.topic = base_বার্তা->তথ্য.topic;
	event_তথ্য.তথ্য.payload = base_বার্তা->তথ্য.payload;
	event_তথ্য.তথ্য.source_id = base_বার্তা->তথ্য.source_id;
	event_তথ্য.তথ্য.source_username = base_বার্তা->তথ্য.source_username;
	event_তথ্য.তথ্য.properties = base_বার্তা->তথ্য.properties;
	event_তথ্য.তথ্য.payloadlen = base_বার্তা->তথ্য.payloadlen;
	event_তথ্য.তথ্য.source_mid = base_বার্তা->তথ্য.source_mid;
	যদি(base_বার্তা->source_listener){
		event_তথ্য.তথ্য.source_port = base_বার্তা->source_listener->port;
	}অন্যথা{
		event_তথ্য.তথ্য.source_port = 0;
	}
	event_তথ্য.তথ্য.qos = base_বার্তা->তথ্য.qos;
	event_তথ্য.তথ্য.retain = base_বার্তা->তথ্য.retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_বার্তা_add, cb_base, cb_পরবর্তী){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_বার্তা_ADD, &event_তথ্য, cb_base->userdata);
	}
	base_বার্তা->stored = true;
}


শূন্য plugin_persist_handle_base_বার্তা_delete(কাঠামো mosquitto_base_বার্তা *base_বার্তা)
{
	কাঠামো mosquitto_evt_persist_base_বার্তা event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	কাঠামো mosquitto__security_options *opts;

	যদি(base_বার্তা->stored == false || db.shutdown){
		ফেরত;
	}

	opts = &db.config->security_options;
	memset(&event_তথ্য, 0, আকার(event_তথ্য));

	event_তথ্য.তথ্য.store_id = base_বার্তা->তথ্য.store_id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_base_বার্তা_delete, cb_base, cb_পরবর্তী){
		cb_base->cb(MOSQ_EVT_PERSIST_BASE_বার্তা_DELETE, &event_তথ্য, cb_base->userdata);
	}
	base_বার্তা->stored = false;
}


শূন্য plugin_persist_handle_retain_বার্তা_set(কাঠামো mosquitto_base_বার্তা *base_বার্তা)
{
	কাঠামো mosquitto_evt_persist_retain_বার্তা event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	কাঠামো mosquitto__security_options *opts;

	যদি(db.shutdown){
		ফেরত;
	}

	opts = &db.config->security_options;
	memset(&event_তথ্য, 0, আকার(event_তথ্য));

	event_তথ্য.store_id = base_বার্তা->তথ্য.store_id;
	event_তথ্য.topic = base_বার্তা->তথ্য.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_বার্তা_set, cb_base, cb_পরবর্তী){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_বার্তা_SET, &event_তথ্য, cb_base->userdata);
	}
}


শূন্য plugin_persist_handle_retain_বার্তা_delete(কাঠামো mosquitto_base_বার্তা *base_বার্তা)
{
	কাঠামো mosquitto_evt_persist_retain_বার্তা event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	কাঠামো mosquitto__security_options *opts;

	যদি(db.shutdown){
		ফেরত;
	}

	opts = &db.config->security_options;
	memset(&event_তথ্য, 0, আকার(event_তথ্য));

	event_তথ্য.topic = base_বার্তা->তথ্য.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_বার্তা_delete, cb_base, cb_পরবর্তী){
		cb_base->cb(MOSQ_EVT_PERSIST_RETAIN_বার্তা_DELETE, &event_তথ্য, cb_base->userdata);
	}
}


শূন্য plugin_persist__handle_will_add(কাঠামো mosquitto *context)
{
	কাঠামো mosquitto_evt_persist_will_বার্তা event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	কাঠামো mosquitto__security_options *opts;
	কাঠামো mosquitto_message *will_বার্তা;

	যদি(db.shutdown || !context->will){
		ফেরত;
	}

	opts = &db.config->security_options;
	will_বার্তা = &context->will->বার্তা;
	memset(&event_তথ্য, 0, আকার(event_তথ্য));
	event_তথ্য.তথ্য.clientid = context->id;
	event_তথ্য.তথ্য.topic = will_বার্তা->topic;
	event_তথ্য.তথ্য.payload = will_বার্তা->payload;
	event_তথ্য.তথ্য.payloadlen = (uint32_t)will_বার্তা->payloadlen;
	event_তথ্য.তথ্য.qos = (uint8_t)will_বার্তা->qos;
	event_তথ্য.তথ্য.retain = will_বার্তা->retain;
	event_তথ্য.তথ্য.properties = context->will->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_add, cb_base, cb_পরবর্তী){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_তথ্য, cb_base->userdata);
	}
}


শূন্য plugin_persist__handle_will_delete(কাঠামো mosquitto *context)
{
	কাঠামো mosquitto_evt_persist_will_বার্তা event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	কাঠামো mosquitto__security_options *opts;

	memset(&event_তথ্য, 0, আকার(event_তথ্য));
	event_তথ্য.তথ্য.clientid = context->id;

	যদি(db.shutdown){
		ফেরত;
	}

	opts = &db.config->security_options;
	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_delete, cb_base, cb_পরবর্তী){
		cb_base->cb(MOSQ_EVT_PERSIST_WILL_ADD, &event_তথ্য, cb_base->userdata);
	}

}
