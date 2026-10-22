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

#అన్తర్భావ "config.h"

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto_internal.h"
#అన్తర్భావ "mosquitto/broker.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "send_mosq.h"
#అన్తర్భావ "util_mosq.h"
#అన్తర్భావ "utlist.h"
#అన్తర్భావ "lib_load.h"
#అన్తర్భావ "will_mosq.h"
#అన్తర్భావ <stdint.h>


శూన్య plugin_persist__handle_restore(శూన్య)
{
	నిర్మాణం mosquitto_evt_persist_restore event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	నిర్మాణం mosquitto__security_options *opts;

	opts = &db.config->security_options;
	memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.persist_restore, cb_పునాది, cb_తదుపరి){
		cb_పునాది->cb(MOSQ_EVT_PERSIST_RESTORE, &event_దత్తాంశం, cb_పునాది->userdata);
	}
}


శూన్య plugin_persist_handle_client_కూడు(నిర్మాణం mosquitto *context)
{
	నిర్మాణం mosquitto_evt_persist_client event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	నిర్మాణం mosquitto__security_options *opts;

	ఒకవేళ(db.shutdown || context->is_persisted){
		తిరిగి;
	}

	opts = &db.config->security_options;
	memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));
	event_దత్తాంశం.దత్తాంశం.clientid = context->id;
	event_దత్తాంశం.దత్తాంశం.username = context->username;
	event_దత్తాంశం.దత్తాంశం.auth_పద్ధతి = context->auth_పద్ధతి;
	event_దత్తాంశం.దత్తాంశం.will_delay_సమయం = context->will_delay_సమయం;
	event_దత్తాంశం.దత్తాంశం.session_expiry_సమయం = context->session_expiry_సమయం;
	event_దత్తాంశం.దత్తాంశం.will_delay_interval = context->will_delay_interval;
	event_దత్తాంశం.దత్తాంశం.session_expiry_interval = context->session_expiry_interval;
	ఒకవేళ(context->listener){
		event_దత్తాంశం.దత్తాంశం.listener_పోర్టు = context->listener->పోర్టు;
	}లేకపోతే{
		event_దత్తాంశం.దత్తాంశం.listener_పోర్టు = 0;
	}
	event_దత్తాంశం.దత్తాంశం.max_qos = context->max_qos;
	event_దత్తాంశం.దత్తాంశం.retain_అందుబాటు = context->retain_అందుబాటు;
	event_దత్తాంశం.దత్తాంశం.max_packet_కొలత = context->గరిష్ఠ_packet_కొలత;

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.persist_client_కూడు, cb_పునాది, cb_తదుపరి){
		cb_పునాది->cb(MOSQ_EVT_PERSIST_CLIENT_కూడు, &event_దత్తాంశం, cb_పునాది->userdata);
	}

	ఒకవేళ(context->will){
		plugin_persist_handle_will_కూడు(context);
	}

	context->is_persisted = true;
}


శూన్య plugin_persist__handle_client_update(నిర్మాణం mosquitto *context)
{
	నిర్మాణం mosquitto_evt_persist_client event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	నిర్మాణం mosquitto__security_options *opts;
	నిర్మాణం mosquitto_సందేశం_v5_2 will;

	వాడని(will); /* FIXME */

	ఒకవేళ(db.shutdown){
		తిరిగి;
	}

	opts = &db.config->security_options;
	memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));
	event_దత్తాంశం.దత్తాంశం.clientid = context->id;
	event_దత్తాంశం.దత్తాంశం.username = context->username;
	event_దత్తాంశం.దత్తాంశం.auth_పద్ధతి = context->auth_పద్ధతి;
	event_దత్తాంశం.దత్తాంశం.will_delay_సమయం = context->will_delay_సమయం;
	event_దత్తాంశం.దత్తాంశం.session_expiry_సమయం = context->session_expiry_సమయం;
	event_దత్తాంశం.దత్తాంశం.will_delay_interval = context->will_delay_interval;
	event_దత్తాంశం.దత్తాంశం.session_expiry_interval = context->session_expiry_interval;
	ఒకవేళ(context->listener){
		event_దత్తాంశం.దత్తాంశం.listener_పోర్టు = context->listener->పోర్టు;
	}లేకపోతే{
		event_దత్తాంశం.దత్తాంశం.listener_పోర్టు = 0;
	}
	event_దత్తాంశం.దత్తాంశం.max_qos = context->max_qos;
	event_దత్తాంశం.దత్తాంశం.retain_అందుబాటు = context->retain_అందుబాటు;
	event_దత్తాంశం.దత్తాంశం.max_packet_కొలత = context->గరిష్ఠ_packet_కొలత;

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.persist_client_update, cb_పునాది, cb_తదుపరి){
		cb_పునాది->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_దత్తాంశం, cb_పునాది->userdata);
	}

	ఒకవేళ(context->will){
		plugin_persist_handle_will_కూడు(context);
	}లేకపోతే{
		plugin_persist__handle_will_delete(context);
	}
}


శూన్య plugin_persist__handle_client_delete(నిర్మాణం mosquitto *context)
{
	నిర్మాణం mosquitto_evt_persist_client event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	నిర్మాణం mosquitto__security_options *opts;

	ఒకవేళ(context->id == ఖాళీ
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		తిరిగి;
	}

	plugin_persist__handle_will_delete(context);

	ఒకవేళ(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_తక్షణ){
		తిరిగి;
	}

	opts = &db.config->security_options;
	memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));
	event_దత్తాంశం.దత్తాంశం.clientid = context->id;

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.persist_client_delete, cb_పునాది, cb_తదుపరి){
		cb_పునాది->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_దత్తాంశం, cb_పునాది->userdata);
	}
	context->is_persisted = false;
}


శూన్య plugin_persist_handle_subscription_కూడు(నిర్మాణం mosquitto *context, స్థిరరాశి నిర్మాణం mosquitto_subscription *sub)
{
	నిర్మాణం mosquitto_evt_persist_subscription event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	నిర్మాణం mosquitto__security_options *opts;

	ఒకవేళ(db.shutdown || context->is_persisted == false){
		తిరిగి;
	}

	opts = &db.config->security_options;
	memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));
	event_దత్తాంశం.దత్తాంశం.clientid = context->id;
	event_దత్తాంశం.దత్తాంశం.topic_filter = sub->topic_filter;
	event_దత్తాంశం.దత్తాంశం.identifier = sub->identifier;
	event_దత్తాంశం.దత్తాంశం.options = sub->options;

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.persist_subscription_కూడు, cb_పునాది, cb_తదుపరి){
		cb_పునాది->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_కూడు, &event_దత్తాంశం, cb_పునాది->userdata);
	}
}


శూన్య plugin_persist__handle_subscription_delete(నిర్మాణం mosquitto *context, అక్షరం *sub)
{
	నిర్మాణం mosquitto_evt_persist_subscription event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	నిర్మాణం mosquitto__security_options *opts;

	ఒకవేళ(db.shutdown || context->is_persisted == false){
		తిరిగి;
	}
	ఒకవేళ(!sub){
		తిరిగి;
	}

	opts = &db.config->security_options;
	memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));
	event_దత్తాంశం.దత్తాంశం.clientid = context->id;
	event_దత్తాంశం.దత్తాంశం.topic_filter = sub;

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.persist_subscription_delete, cb_పునాది, cb_తదుపరి){
		cb_పునాది->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_దత్తాంశం, cb_పునాది->userdata);
	}
}


స్థిర అంతఃవరుస శూన్య సమితి_client_సందేశం_event_దత్తాంశం(నిర్మాణం mosquitto_evt_persist_client_సందేశం *event_దత్తాంశం, నిర్మాణం mosquitto *context, స్థిరరాశి నిర్మాణం mosquitto_client_సందేశం *client_సందేశం)
{
	event_దత్తాంశం->దత్తాంశం.clientid = context->id;
	event_దత్తాంశం->దత్తాంశం.cmsg_id = client_సందేశం->దత్తాంశం.cmsg_id;
	event_దత్తాంశం->దత్తాంశం.direction = (uint8_t)client_సందేశం->దత్తాంశం.direction;
	event_దత్తాంశం->దత్తాంశం.dup = client_సందేశం->దత్తాంశం.dup;
	event_దత్తాంశం->దత్తాంశం.mid = client_సందేశం->దత్తాంశం.mid;
	event_దత్తాంశం->దత్తాంశం.qos = client_సందేశం->దత్తాంశం.qos;
	event_దత్తాంశం->దత్తాంశం.retain = client_సందేశం->దత్తాంశం.retain;
	event_దత్తాంశం->దత్తాంశం.state = (uint8_t)client_సందేశం->దత్తాంశం.state;
	event_దత్తాంశం->దత్తాంశం.store_id = client_సందేశం->పునాది_సందేశం->దత్తాంశం.store_id;
	event_దత్తాంశం->దత్తాంశం.subscription_identifier = client_సందేశం->దత్తాంశం.subscription_identifier;
}


శూన్య plugin_persist_handle_client_సందేశం_కూడు(నిర్మాణం mosquitto *context, స్థిరరాశి నిర్మాణం mosquitto_client_సందేశం *client_సందేశం)
{
	నిర్మాణం mosquitto_evt_persist_client_సందేశం event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	నిర్మాణం mosquitto__security_options *opts;

	ఒకవేళ(context->is_persisted == false
			|| (client_సందేశం->దత్తాంశం.qos == 0 && db.config->వరుస_qos0_messages == false)
			|| db.shutdown){

		తిరిగి;
	}

	opts = &db.config->security_options;
	memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));

	సమితి_client_సందేశం_event_దత్తాంశం(&event_దత్తాంశం, context, client_సందేశం);

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.persist_client_సందేశం_కూడు, cb_పునాది, cb_తదుపరి){
		cb_పునాది->cb(MOSQ_EVT_PERSIST_CLIENT_సందేశం_కూడు, &event_దత్తాంశం, cb_పునాది->userdata);
	}
}


శూన్య plugin_persist_handle_client_సందేశం_delete(నిర్మాణం mosquitto *context, స్థిరరాశి నిర్మాణం mosquitto_client_సందేశం *client_సందేశం)
{
	నిర్మాణం mosquitto_evt_persist_client_సందేశం event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	నిర్మాణం mosquitto__security_options *opts;

	ఒకవేళ(context->is_persisted == false
			|| (client_సందేశం->దత్తాంశం.qos == 0 && db.config->వరుస_qos0_messages == false)
			|| db.shutdown){

		తిరిగి;
	}

	opts = &db.config->security_options;
	memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));

	సమితి_client_సందేశం_event_దత్తాంశం(&event_దత్తాంశం, context, client_సందేశం);

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.persist_client_సందేశం_delete, cb_పునాది, cb_తదుపరి){
		cb_పునాది->cb(MOSQ_EVT_PERSIST_CLIENT_సందేశం_DELETE, &event_దత్తాంశం, cb_పునాది->userdata);
	}
}


శూన్య plugin_persist_handle_client_సందేశం_update(నిర్మాణం mosquitto *context, స్థిరరాశి నిర్మాణం mosquitto_client_సందేశం *client_సందేశం)
{
	నిర్మాణం mosquitto_evt_persist_client_సందేశం event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	నిర్మాణం mosquitto__security_options *opts;

	ఒకవేళ(context->is_persisted == false
			|| (client_సందేశం->దత్తాంశం.qos == 0 && db.config->వరుస_qos0_messages == false)
			|| db.shutdown){

		తిరిగి;
	}

	opts = &db.config->security_options;
	memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));

	సమితి_client_సందేశం_event_దత్తాంశం(&event_దత్తాంశం, context, client_సందేశం);

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.persist_client_సందేశం_update, cb_పునాది, cb_తదుపరి){
		cb_పునాది->cb(MOSQ_EVT_PERSIST_CLIENT_సందేశం_UPDATE, &event_దత్తాంశం, cb_పునాది->userdata);
	}
}


శూన్య plugin_persist_handle_పునాది_సందేశం_కూడు(నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం)
{
	నిర్మాణం mosquitto_evt_persist_పునాది_సందేశం event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	నిర్మాణం mosquitto__security_options *opts;

	ఒకవేళ(పునాది_సందేశం->stored || db.shutdown){
		తిరిగి;
	}

	opts = &db.config->security_options;
	memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));

	event_దత్తాంశం.దత్తాంశం.store_id = పునాది_సందేశం->దత్తాంశం.store_id;
	event_దత్తాంశం.దత్తాంశం.expiry_సమయం = పునాది_సందేశం->దత్తాంశం.expiry_సమయం;
	event_దత్తాంశం.దత్తాంశం.topic = పునాది_సందేశం->దత్తాంశం.topic;
	event_దత్తాంశం.దత్తాంశం.payload = పునాది_సందేశం->దత్తాంశం.payload;
	event_దత్తాంశం.దత్తాంశం.ఆధారం_id = పునాది_సందేశం->దత్తాంశం.ఆధారం_id;
	event_దత్తాంశం.దత్తాంశం.ఆధారం_username = పునాది_సందేశం->దత్తాంశం.ఆధారం_username;
	event_దత్తాంశం.దత్తాంశం.properties = పునాది_సందేశం->దత్తాంశం.properties;
	event_దత్తాంశం.దత్తాంశం.payloadlen = పునాది_సందేశం->దత్తాంశం.payloadlen;
	event_దత్తాంశం.దత్తాంశం.ఆధారం_mid = పునాది_సందేశం->దత్తాంశం.ఆధారం_mid;
	ఒకవేళ(పునాది_సందేశం->ఆధారం_listener){
		event_దత్తాంశం.దత్తాంశం.ఆధారం_పోర్టు = పునాది_సందేశం->ఆధారం_listener->పోర్టు;
	}లేకపోతే{
		event_దత్తాంశం.దత్తాంశం.ఆధారం_పోర్టు = 0;
	}
	event_దత్తాంశం.దత్తాంశం.qos = పునాది_సందేశం->దత్తాంశం.qos;
	event_దత్తాంశం.దత్తాంశం.retain = పునాది_సందేశం->దత్తాంశం.retain;

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.persist_పునాది_సందేశం_కూడు, cb_పునాది, cb_తదుపరి){
		cb_పునాది->cb(MOSQ_EVT_PERSIST_పునాది_సందేశం_కూడు, &event_దత్తాంశం, cb_పునాది->userdata);
	}
	పునాది_సందేశం->stored = true;
}


శూన్య plugin_persist_handle_పునాది_సందేశం_delete(నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం)
{
	నిర్మాణం mosquitto_evt_persist_పునాది_సందేశం event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	నిర్మాణం mosquitto__security_options *opts;

	ఒకవేళ(పునాది_సందేశం->stored == false || db.shutdown){
		తిరిగి;
	}

	opts = &db.config->security_options;
	memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));

	event_దత్తాంశం.దత్తాంశం.store_id = పునాది_సందేశం->దత్తాంశం.store_id;

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.persist_పునాది_సందేశం_delete, cb_పునాది, cb_తదుపరి){
		cb_పునాది->cb(MOSQ_EVT_PERSIST_పునాది_సందేశం_DELETE, &event_దత్తాంశం, cb_పునాది->userdata);
	}
	పునాది_సందేశం->stored = false;
}


శూన్య plugin_persist_handle_retain_సందేశం_సమితి(నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం)
{
	నిర్మాణం mosquitto_evt_persist_retain_సందేశం event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	నిర్మాణం mosquitto__security_options *opts;

	ఒకవేళ(db.shutdown){
		తిరిగి;
	}

	opts = &db.config->security_options;
	memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));

	event_దత్తాంశం.store_id = పునాది_సందేశం->దత్తాంశం.store_id;
	event_దత్తాంశం.topic = పునాది_సందేశం->దత్తాంశం.topic;

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.persist_retain_సందేశం_సమితి, cb_పునాది, cb_తదుపరి){
		cb_పునాది->cb(MOSQ_EVT_PERSIST_RETAIN_సందేశం_సమితి, &event_దత్తాంశం, cb_పునాది->userdata);
	}
}


శూన్య plugin_persist_handle_retain_సందేశం_delete(నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం)
{
	నిర్మాణం mosquitto_evt_persist_retain_సందేశం event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	నిర్మాణం mosquitto__security_options *opts;

	ఒకవేళ(db.shutdown){
		తిరిగి;
	}

	opts = &db.config->security_options;
	memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));

	event_దత్తాంశం.topic = పునాది_సందేశం->దత్తాంశం.topic;

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.persist_retain_సందేశం_delete, cb_పునాది, cb_తదుపరి){
		cb_పునాది->cb(MOSQ_EVT_PERSIST_RETAIN_సందేశం_DELETE, &event_దత్తాంశం, cb_పునాది->userdata);
	}
}


శూన్య plugin_persist_handle_will_కూడు(నిర్మాణం mosquitto *context)
{
	నిర్మాణం mosquitto_evt_persist_will_సందేశం event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	నిర్మాణం mosquitto__security_options *opts;
	నిర్మాణం mosquitto_సందేశం *will_సందేశం;

	ఒకవేళ(db.shutdown || !context->will){
		తిరిగి;
	}

	opts = &db.config->security_options;
	will_సందేశం = &context->will->సందేశం;
	memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));
	event_దత్తాంశం.దత్తాంశం.clientid = context->id;
	event_దత్తాంశం.దత్తాంశం.topic = will_సందేశం->topic;
	event_దత్తాంశం.దత్తాంశం.payload = will_సందేశం->payload;
	event_దత్తాంశం.దత్తాంశం.payloadlen = (uint32_t)will_సందేశం->payloadlen;
	event_దత్తాంశం.దత్తాంశం.qos = (uint8_t)will_సందేశం->qos;
	event_దత్తాంశం.దత్తాంశం.retain = will_సందేశం->retain;
	event_దత్తాంశం.దత్తాంశం.properties = context->will->properties;

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.persist_will_కూడు, cb_పునాది, cb_తదుపరి){
		cb_పునాది->cb(MOSQ_EVT_PERSIST_WILL_కూడు, &event_దత్తాంశం, cb_పునాది->userdata);
	}
}


శూన్య plugin_persist__handle_will_delete(నిర్మాణం mosquitto *context)
{
	నిర్మాణం mosquitto_evt_persist_will_సందేశం event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	నిర్మాణం mosquitto__security_options *opts;

	memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));
	event_దత్తాంశం.దత్తాంశం.clientid = context->id;

	ఒకవేళ(db.shutdown){
		తిరిగి;
	}

	opts = &db.config->security_options;
	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.persist_will_delete, cb_పునాది, cb_తదుపరి){
		cb_పునాది->cb(MOSQ_EVT_PERSIST_WILL_కూడు, &event_దత్తాంశం, cb_పునాది->userdata);
	}

}
