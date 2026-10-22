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
	संरचना mosquitto_evt_persist_restore event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;
	संरचना mosquitto__security_options *opts;

	opts = &db.config->security_options;
	स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_restore, cb_आधारः, cb_अग्र){
		cb_आधारः->cb(MOSQ_EVT_PERSIST_RESTORE, &event_दत्त, cb_आधारः->userdata);
	}
}


शून्य plugin_persist_handle_client_सङ्कलय(संरचना mosquitto *context)
{
	संरचना mosquitto_evt_persist_client event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;
	संरचना mosquitto__security_options *opts;

	यदि(db.shutdown || context->is_persisted){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));
	event_दत्त.दत्त.clientid = context->id;
	event_दत्त.दत्त.username = context->username;
	event_दत्त.दत्त.auth_विधानम् = context->auth_विधानम्;
	event_दत्त.दत्त.will_delay_time = context->will_delay_time;
	event_दत्त.दत्त.session_expiry_time = context->session_expiry_time;
	event_दत्त.दत्त.will_delay_interval = context->will_delay_interval;
	event_दत्त.दत्त.session_expiry_interval = context->session_expiry_interval;
	यदि(context->listener){
		event_दत्त.दत्त.listener_पत्तनम् = context->listener->पत्तनम्;
	}अन्यथा{
		event_दत्त.दत्त.listener_पत्तनम् = 0;
	}
	event_दत्त.दत्त.max_qos = context->max_qos;
	event_दत्त.दत्त.retain_उपलब्धम् = context->retain_उपलब्धम्;
	event_दत्त.दत्त.max_packet_आकार = context->अधिकतमम्_packet_आकार;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_सङ्कलय, cb_आधारः, cb_अग्र){
		cb_आधारः->cb(MOSQ_EVT_PERSIST_CLIENT_सङ्कलय, &event_दत्त, cb_आधारः->userdata);
	}

	यदि(context->will){
		plugin_persist_handle_will_सङ्कलय(context);
	}

	context->is_persisted = true;
}


शून्य plugin_persist__handle_client_update(संरचना mosquitto *context)
{
	संरचना mosquitto_evt_persist_client event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;
	संरचना mosquitto__security_options *opts;
	संरचना mosquitto_सन्देशः_v5_2 will;

	अप्रयुक्तम्(will); /* FIXME */

	यदि(db.shutdown){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));
	event_दत्त.दत्त.clientid = context->id;
	event_दत्त.दत्त.username = context->username;
	event_दत्त.दत्त.auth_विधानम् = context->auth_विधानम्;
	event_दत्त.दत्त.will_delay_time = context->will_delay_time;
	event_दत्त.दत्त.session_expiry_time = context->session_expiry_time;
	event_दत्त.दत्त.will_delay_interval = context->will_delay_interval;
	event_दत्त.दत्त.session_expiry_interval = context->session_expiry_interval;
	यदि(context->listener){
		event_दत्त.दत्त.listener_पत्तनम् = context->listener->पत्तनम्;
	}अन्यथा{
		event_दत्त.दत्त.listener_पत्तनम् = 0;
	}
	event_दत्त.दत्त.max_qos = context->max_qos;
	event_दत्त.दत्त.retain_उपलब्धम् = context->retain_उपलब्धम्;
	event_दत्त.दत्त.max_packet_आकार = context->अधिकतमम्_packet_आकार;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_update, cb_आधारः, cb_अग्र){
		cb_आधारः->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_दत्त, cb_आधारः->userdata);
	}

	यदि(context->will){
		plugin_persist_handle_will_सङ्कलय(context);
	}अन्यथा{
		plugin_persist__handle_will_delete(context);
	}
}


शून्य plugin_persist__handle_client_delete(संरचना mosquitto *context)
{
	संरचना mosquitto_evt_persist_client event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;
	संरचना mosquitto__security_options *opts;

	यदि(context->id == रिक्त
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		प्रत्यागम;
	}

	plugin_persist__handle_will_delete(context);

	यदि(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_तात्कालिकम्){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));
	event_दत्त.दत्त.clientid = context->id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_delete, cb_आधारः, cb_अग्र){
		cb_आधारः->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_दत्त, cb_आधारः->userdata);
	}
	context->is_persisted = false;
}


शून्य plugin_persist_handle_subscription_सङ्कलय(संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub)
{
	संरचना mosquitto_evt_persist_subscription event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;
	संरचना mosquitto__security_options *opts;

	यदि(db.shutdown || context->is_persisted == false){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));
	event_दत्त.दत्त.clientid = context->id;
	event_दत्त.दत्त.topic_filter = sub->topic_filter;
	event_दत्त.दत्त.identifier = sub->identifier;
	event_दत्त.दत्त.options = sub->options;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_सङ्कलय, cb_आधारः, cb_अग्र){
		cb_आधारः->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_सङ्कलय, &event_दत्त, cb_आधारः->userdata);
	}
}


शून्य plugin_persist__handle_subscription_delete(संरचना mosquitto *context, वर्ण *sub)
{
	संरचना mosquitto_evt_persist_subscription event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;
	संरचना mosquitto__security_options *opts;

	यदि(db.shutdown || context->is_persisted == false){
		प्रत्यागम;
	}
	यदि(!sub){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));
	event_दत्त.दत्त.clientid = context->id;
	event_दत्त.दत्त.topic_filter = sub;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_subscription_delete, cb_आधारः, cb_अग्र){
		cb_आधारः->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_दत्त, cb_आधारः->userdata);
	}
}


स्थिर अन्तःस्थ शून्य समुच्चयः_client_सन्देशः_event_दत्त(संरचना mosquitto_evt_persist_client_सन्देशः *event_दत्त, संरचना mosquitto *context, ध्रुव संरचना mosquitto_client_सन्देशः *client_सन्देशः)
{
	event_दत्त->दत्त.clientid = context->id;
	event_दत्त->दत्त.cmsg_id = client_सन्देशः->दत्त.cmsg_id;
	event_दत्त->दत्त.direction = (uint8_t)client_सन्देशः->दत्त.direction;
	event_दत्त->दत्त.dup = client_सन्देशः->दत्त.dup;
	event_दत्त->दत्त.mid = client_सन्देशः->दत्त.mid;
	event_दत्त->दत्त.qos = client_सन्देशः->दत्त.qos;
	event_दत्त->दत्त.retain = client_सन्देशः->दत्त.retain;
	event_दत्त->दत्त.state = (uint8_t)client_सन्देशः->दत्त.state;
	event_दत्त->दत्त.store_id = client_सन्देशः->आधारः_सन्देशः->दत्त.store_id;
	event_दत्त->दत्त.subscription_identifier = client_सन्देशः->दत्त.subscription_identifier;
}


शून्य plugin_persist_handle_client_सन्देशः_सङ्कलय(संरचना mosquitto *context, ध्रुव संरचना mosquitto_client_सन्देशः *client_सन्देशः)
{
	संरचना mosquitto_evt_persist_client_सन्देशः event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;
	संरचना mosquitto__security_options *opts;

	यदि(context->is_persisted == false
			|| (client_सन्देशः->दत्त.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		प्रत्यागम;
	}

	opts = &db.config->security_options;
	स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));

	समुच्चयः_client_सन्देशः_event_दत्त(&event_दत्त, context, client_सन्देशः);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_सन्देशः_सङ्कलय, cb_आधारः, cb_अग्र){
		cb_आधारः->cb(MOSQ_EVT_PERSIST_CLIENT_सन्देशः_सङ्कलय, &event_दत्त, cb_आधारः->userdata);
	}
}


शून्य plugin_persist_handle_client_सन्देशः_delete(संरचना mosquitto *context, ध्रुव संरचना mosquitto_client_सन्देशः *client_सन्देशः)
{
	संरचना mosquitto_evt_persist_client_सन्देशः event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;
	संरचना mosquitto__security_options *opts;

	यदि(context->is_persisted == false
			|| (client_सन्देशः->दत्त.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		प्रत्यागम;
	}

	opts = &db.config->security_options;
	स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));

	समुच्चयः_client_सन्देशः_event_दत्त(&event_दत्त, context, client_सन्देशः);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_सन्देशः_delete, cb_आधारः, cb_अग्र){
		cb_आधारः->cb(MOSQ_EVT_PERSIST_CLIENT_सन्देशः_DELETE, &event_दत्त, cb_आधारः->userdata);
	}
}


शून्य plugin_persist_handle_client_सन्देशः_update(संरचना mosquitto *context, ध्रुव संरचना mosquitto_client_सन्देशः *client_सन्देशः)
{
	संरचना mosquitto_evt_persist_client_सन्देशः event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;
	संरचना mosquitto__security_options *opts;

	यदि(context->is_persisted == false
			|| (client_सन्देशः->दत्त.qos == 0 && db.config->queue_qos0_messages == false)
			|| db.shutdown){

		प्रत्यागम;
	}

	opts = &db.config->security_options;
	स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));

	समुच्चयः_client_सन्देशः_event_दत्त(&event_दत्त, context, client_सन्देशः);

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_client_सन्देशः_update, cb_आधारः, cb_अग्र){
		cb_आधारः->cb(MOSQ_EVT_PERSIST_CLIENT_सन्देशः_UPDATE, &event_दत्त, cb_आधारः->userdata);
	}
}


शून्य plugin_persist_handle_आधारः_सन्देशः_सङ्कलय(संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः)
{
	संरचना mosquitto_evt_persist_आधारः_सन्देशः event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;
	संरचना mosquitto__security_options *opts;

	यदि(आधारः_सन्देशः->stored || db.shutdown){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));

	event_दत्त.दत्त.store_id = आधारः_सन्देशः->दत्त.store_id;
	event_दत्त.दत्त.expiry_time = आधारः_सन्देशः->दत्त.expiry_time;
	event_दत्त.दत्त.topic = आधारः_सन्देशः->दत्त.topic;
	event_दत्त.दत्त.payload = आधारः_सन्देशः->दत्त.payload;
	event_दत्त.दत्त.source_id = आधारः_सन्देशः->दत्त.source_id;
	event_दत्त.दत्त.source_username = आधारः_सन्देशः->दत्त.source_username;
	event_दत्त.दत्त.properties = आधारः_सन्देशः->दत्त.properties;
	event_दत्त.दत्त.payloadlen = आधारः_सन्देशः->दत्त.payloadlen;
	event_दत्त.दत्त.source_mid = आधारः_सन्देशः->दत्त.source_mid;
	यदि(आधारः_सन्देशः->source_listener){
		event_दत्त.दत्त.source_पत्तनम् = आधारः_सन्देशः->source_listener->पत्तनम्;
	}अन्यथा{
		event_दत्त.दत्त.source_पत्तनम् = 0;
	}
	event_दत्त.दत्त.qos = आधारः_सन्देशः->दत्त.qos;
	event_दत्त.दत्त.retain = आधारः_सन्देशः->दत्त.retain;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_आधारः_सन्देशः_सङ्कलय, cb_आधारः, cb_अग्र){
		cb_आधारः->cb(MOSQ_EVT_PERSIST_आधारः_सन्देशः_सङ्कलय, &event_दत्त, cb_आधारः->userdata);
	}
	आधारः_सन्देशः->stored = true;
}


शून्य plugin_persist_handle_आधारः_सन्देशः_delete(संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः)
{
	संरचना mosquitto_evt_persist_आधारः_सन्देशः event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;
	संरचना mosquitto__security_options *opts;

	यदि(आधारः_सन्देशः->stored == false || db.shutdown){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));

	event_दत्त.दत्त.store_id = आधारः_सन्देशः->दत्त.store_id;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_आधारः_सन्देशः_delete, cb_आधारः, cb_अग्र){
		cb_आधारः->cb(MOSQ_EVT_PERSIST_आधारः_सन्देशः_DELETE, &event_दत्त, cb_आधारः->userdata);
	}
	आधारः_सन्देशः->stored = false;
}


शून्य plugin_persist_handle_retain_सन्देशः_समुच्चयः(संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः)
{
	संरचना mosquitto_evt_persist_retain_सन्देशः event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;
	संरचना mosquitto__security_options *opts;

	यदि(db.shutdown){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));

	event_दत्त.store_id = आधारः_सन्देशः->दत्त.store_id;
	event_दत्त.topic = आधारः_सन्देशः->दत्त.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_सन्देशः_समुच्चयः, cb_आधारः, cb_अग्र){
		cb_आधारः->cb(MOSQ_EVT_PERSIST_RETAIN_सन्देशः_समुच्चयः, &event_दत्त, cb_आधारः->userdata);
	}
}


शून्य plugin_persist_handle_retain_सन्देशः_delete(संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः)
{
	संरचना mosquitto_evt_persist_retain_सन्देशः event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;
	संरचना mosquitto__security_options *opts;

	यदि(db.shutdown){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));

	event_दत्त.topic = आधारः_सन्देशः->दत्त.topic;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_retain_सन्देशः_delete, cb_आधारः, cb_अग्र){
		cb_आधारः->cb(MOSQ_EVT_PERSIST_RETAIN_सन्देशः_DELETE, &event_दत्त, cb_आधारः->userdata);
	}
}


शून्य plugin_persist_handle_will_सङ्कलय(संरचना mosquitto *context)
{
	संरचना mosquitto_evt_persist_will_सन्देशः event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;
	संरचना mosquitto__security_options *opts;
	संरचना mosquitto_सन्देशः *will_सन्देशः;

	यदि(db.shutdown || !context->will){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	will_सन्देशः = &context->will->सन्देशः;
	स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));
	event_दत्त.दत्त.clientid = context->id;
	event_दत्त.दत्त.topic = will_सन्देशः->topic;
	event_दत्त.दत्त.payload = will_सन्देशः->payload;
	event_दत्त.दत्त.payloadlen = (uint32_t)will_सन्देशः->payloadlen;
	event_दत्त.दत्त.qos = (uint8_t)will_सन्देशः->qos;
	event_दत्त.दत्त.retain = will_सन्देशः->retain;
	event_दत्त.दत्त.properties = context->will->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_सङ्कलय, cb_आधारः, cb_अग्र){
		cb_आधारः->cb(MOSQ_EVT_PERSIST_WILL_सङ्कलय, &event_दत्त, cb_आधारः->userdata);
	}
}


शून्य plugin_persist__handle_will_delete(संरचना mosquitto *context)
{
	संरचना mosquitto_evt_persist_will_सन्देशः event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;
	संरचना mosquitto__security_options *opts;

	स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));
	event_दत्त.दत्त.clientid = context->id;

	यदि(db.shutdown){
		प्रत्यागम;
	}

	opts = &db.config->security_options;
	DL_FOREACH_SAFE(opts->plugin_callbacks.persist_will_delete, cb_आधारः, cb_अग्र){
		cb_आधारः->cb(MOSQ_EVT_PERSIST_WILL_सङ्कलय, &event_दत्त, cb_आधारः->userdata);
	}

}
