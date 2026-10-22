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

#ضمن "config.h"

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto_internal.h"
#ضمن "mosquitto/broker.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "send_mosq.h"
#ضمن "util_mosq.h"
#ضمن "utlist.h"
#ضمن "lib_load.h"
#ضمن "will_mosq.h"
#ضمن <stdint.h>


فراغ plugin_persist__handle_restore(فراغ)
{
	بنية mosquitto_evt_persist_restore event_معلومات;
	بنية mosquitto__callback *cb_بنياد, *cb_ايندڙ;
	بنية mosquitto__security_options *opts;

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_restore, cb_بنياد, cb_ايندڙ){
		cb_بنياد->cb(MOSQ_EVT_PERSIST_RESTORE, &event_معلومات, cb_بنياد->userdata);
	}
}


فراغ plugin_persist_handle_client_جوڙيو(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_client event_معلومات;
	بنية mosquitto__callback *cb_بنياد, *cb_ايندڙ;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown || context->is_persisted){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));
	event_معلومات.معلومات.clientid = context->id;
	event_معلومات.معلومات.username = context->username;
	event_معلومات.معلومات.auth_طريقو = context->auth_طريقو;
	event_معلومات.معلومات.will_delay_وقت = context->will_delay_وقت;
	event_معلومات.معلومات.session_expiry_وقت = context->session_expiry_وقت;
	event_معلومات.معلومات.will_delay_interval = context->will_delay_interval;
	event_معلومات.معلومات.session_expiry_interval = context->session_expiry_interval;
	إذا(context->listener){
		event_معلومات.معلومات.listener_پورٽ = context->listener->پورٽ;
	}وإلا{
		event_معلومات.معلومات.listener_پورٽ = 0;
	}
	event_معلومات.معلومات.max_qos = context->max_qos;
	event_معلومات.معلومات.retain_موجود = context->retain_موجود;
	event_معلومات.معلومات.max_packet_سائيز = context->وڌ_کان_وڌ_packet_سائيز;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_client_جوڙيو, cb_بنياد, cb_ايندڙ){
		cb_بنياد->cb(MOSQ_EVT_PERSIST_CLIENT_جوڙيو, &event_معلومات, cb_بنياد->userdata);
	}

	إذا(context->will){
		plugin_persist_handle_will_جوڙيو(context);
	}

	context->is_persisted = true;
}


فراغ plugin_persist__handle_client_update(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_client event_معلومات;
	بنية mosquitto__callback *cb_بنياد, *cb_ايندڙ;
	بنية mosquitto__security_options *opts;
	بنية mosquitto_پيغام_v5_2 will;

	اڻ_استعمال_ٿيل(will); /* FIXME */

	إذا(db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));
	event_معلومات.معلومات.clientid = context->id;
	event_معلومات.معلومات.username = context->username;
	event_معلومات.معلومات.auth_طريقو = context->auth_طريقو;
	event_معلومات.معلومات.will_delay_وقت = context->will_delay_وقت;
	event_معلومات.معلومات.session_expiry_وقت = context->session_expiry_وقت;
	event_معلومات.معلومات.will_delay_interval = context->will_delay_interval;
	event_معلومات.معلومات.session_expiry_interval = context->session_expiry_interval;
	إذا(context->listener){
		event_معلومات.معلومات.listener_پورٽ = context->listener->پورٽ;
	}وإلا{
		event_معلومات.معلومات.listener_پورٽ = 0;
	}
	event_معلومات.معلومات.max_qos = context->max_qos;
	event_معلومات.معلومات.retain_موجود = context->retain_موجود;
	event_معلومات.معلومات.max_packet_سائيز = context->وڌ_کان_وڌ_packet_سائيز;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_client_update, cb_بنياد, cb_ايندڙ){
		cb_بنياد->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_معلومات, cb_بنياد->userdata);
	}

	إذا(context->will){
		plugin_persist_handle_will_جوڙيو(context);
	}وإلا{
		plugin_persist__handle_will_delete(context);
	}
}


فراغ plugin_persist__handle_client_delete(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_client event_معلومات;
	بنية mosquitto__callback *cb_بنياد, *cb_ايندڙ;
	بنية mosquitto__security_options *opts;

	إذا(context->id == عدم
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		أرجع;
	}

	plugin_persist__handle_will_delete(context);

	إذا(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوري){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));
	event_معلومات.معلومات.clientid = context->id;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_client_delete, cb_بنياد, cb_ايندڙ){
		cb_بنياد->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_معلومات, cb_بنياد->userdata);
	}
	context->is_persisted = false;
}


فراغ plugin_persist_handle_subscription_جوڙيو(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub)
{
	بنية mosquitto_evt_persist_subscription event_معلومات;
	بنية mosquitto__callback *cb_بنياد, *cb_ايندڙ;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown || context->is_persisted == false){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));
	event_معلومات.معلومات.clientid = context->id;
	event_معلومات.معلومات.topic_filter = sub->topic_filter;
	event_معلومات.معلومات.identifier = sub->identifier;
	event_معلومات.معلومات.options = sub->options;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_subscription_جوڙيو, cb_بنياد, cb_ايندڙ){
		cb_بنياد->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_جوڙيو, &event_معلومات, cb_بنياد->userdata);
	}
}


فراغ plugin_persist__handle_subscription_delete(بنية mosquitto *context, حرف *sub)
{
	بنية mosquitto_evt_persist_subscription event_معلومات;
	بنية mosquitto__callback *cb_بنياد, *cb_ايندڙ;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown || context->is_persisted == false){
		أرجع;
	}
	إذا(!sub){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));
	event_معلومات.معلومات.clientid = context->id;
	event_معلومات.معلومات.topic_filter = sub;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_subscription_delete, cb_بنياد, cb_ايندڙ){
		cb_بنياد->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_معلومات, cb_بنياد->userdata);
	}
}


ساكن مضمن فراغ سيٽ_client_نياپو_event_معلومات(بنية mosquitto_evt_persist_client_نياپو *event_معلومات, بنية mosquitto *context, ثابت بنية mosquitto_client_نياپو *client_نياپو)
{
	event_معلومات->معلومات.clientid = context->id;
	event_معلومات->معلومات.cmsg_id = client_نياپو->معلومات.cmsg_id;
	event_معلومات->معلومات.direction = (uint8_t)client_نياپو->معلومات.direction;
	event_معلومات->معلومات.dup = client_نياپو->معلومات.dup;
	event_معلومات->معلومات.mid = client_نياپو->معلومات.mid;
	event_معلومات->معلومات.qos = client_نياپو->معلومات.qos;
	event_معلومات->معلومات.retain = client_نياپو->معلومات.retain;
	event_معلومات->معلومات.state = (uint8_t)client_نياپو->معلومات.state;
	event_معلومات->معلومات.store_id = client_نياپو->بنياد_نياپو->معلومات.store_id;
	event_معلومات->معلومات.subscription_identifier = client_نياپو->معلومات.subscription_identifier;
}


فراغ plugin_persist_handle_client_نياپو_جوڙيو(بنية mosquitto *context, ثابت بنية mosquitto_client_نياپو *client_نياپو)
{
	بنية mosquitto_evt_persist_client_نياپو event_معلومات;
	بنية mosquitto__callback *cb_بنياد, *cb_ايندڙ;
	بنية mosquitto__security_options *opts;

	إذا(context->is_persisted == false
			|| (client_نياپو->معلومات.qos == 0 && db.config->قطار_qos0_messages == false)
			|| db.shutdown){

		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));

	سيٽ_client_نياپو_event_معلومات(&event_معلومات, context, client_نياپو);

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_client_نياپو_جوڙيو, cb_بنياد, cb_ايندڙ){
		cb_بنياد->cb(MOSQ_EVT_PERSIST_CLIENT_نياپو_جوڙيو, &event_معلومات, cb_بنياد->userdata);
	}
}


فراغ plugin_persist_handle_client_نياپو_delete(بنية mosquitto *context, ثابت بنية mosquitto_client_نياپو *client_نياپو)
{
	بنية mosquitto_evt_persist_client_نياپو event_معلومات;
	بنية mosquitto__callback *cb_بنياد, *cb_ايندڙ;
	بنية mosquitto__security_options *opts;

	إذا(context->is_persisted == false
			|| (client_نياپو->معلومات.qos == 0 && db.config->قطار_qos0_messages == false)
			|| db.shutdown){

		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));

	سيٽ_client_نياپو_event_معلومات(&event_معلومات, context, client_نياپو);

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_client_نياپو_delete, cb_بنياد, cb_ايندڙ){
		cb_بنياد->cb(MOSQ_EVT_PERSIST_CLIENT_نياپو_DELETE, &event_معلومات, cb_بنياد->userdata);
	}
}


فراغ plugin_persist_handle_client_نياپو_update(بنية mosquitto *context, ثابت بنية mosquitto_client_نياپو *client_نياپو)
{
	بنية mosquitto_evt_persist_client_نياپو event_معلومات;
	بنية mosquitto__callback *cb_بنياد, *cb_ايندڙ;
	بنية mosquitto__security_options *opts;

	إذا(context->is_persisted == false
			|| (client_نياپو->معلومات.qos == 0 && db.config->قطار_qos0_messages == false)
			|| db.shutdown){

		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));

	سيٽ_client_نياپو_event_معلومات(&event_معلومات, context, client_نياپو);

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_client_نياپو_update, cb_بنياد, cb_ايندڙ){
		cb_بنياد->cb(MOSQ_EVT_PERSIST_CLIENT_نياپو_UPDATE, &event_معلومات, cb_بنياد->userdata);
	}
}


فراغ plugin_persist_handle_بنياد_نياپو_جوڙيو(بنية mosquitto_بنياد_نياپو *بنياد_نياپو)
{
	بنية mosquitto_evt_persist_بنياد_نياپو event_معلومات;
	بنية mosquitto__callback *cb_بنياد, *cb_ايندڙ;
	بنية mosquitto__security_options *opts;

	إذا(بنياد_نياپو->stored || db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));

	event_معلومات.معلومات.store_id = بنياد_نياپو->معلومات.store_id;
	event_معلومات.معلومات.expiry_وقت = بنياد_نياپو->معلومات.expiry_وقت;
	event_معلومات.معلومات.topic = بنياد_نياپو->معلومات.topic;
	event_معلومات.معلومات.payload = بنياد_نياپو->معلومات.payload;
	event_معلومات.معلومات.ذريعو_id = بنياد_نياپو->معلومات.ذريعو_id;
	event_معلومات.معلومات.ذريعو_username = بنياد_نياپو->معلومات.ذريعو_username;
	event_معلومات.معلومات.properties = بنياد_نياپو->معلومات.properties;
	event_معلومات.معلومات.payloadlen = بنياد_نياپو->معلومات.payloadlen;
	event_معلومات.معلومات.ذريعو_mid = بنياد_نياپو->معلومات.ذريعو_mid;
	إذا(بنياد_نياپو->ذريعو_listener){
		event_معلومات.معلومات.ذريعو_پورٽ = بنياد_نياپو->ذريعو_listener->پورٽ;
	}وإلا{
		event_معلومات.معلومات.ذريعو_پورٽ = 0;
	}
	event_معلومات.معلومات.qos = بنياد_نياپو->معلومات.qos;
	event_معلومات.معلومات.retain = بنياد_نياپو->معلومات.retain;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_بنياد_نياپو_جوڙيو, cb_بنياد, cb_ايندڙ){
		cb_بنياد->cb(MOSQ_EVT_PERSIST_بنياد_نياپو_جوڙيو, &event_معلومات, cb_بنياد->userdata);
	}
	بنياد_نياپو->stored = true;
}


فراغ plugin_persist_handle_بنياد_نياپو_delete(بنية mosquitto_بنياد_نياپو *بنياد_نياپو)
{
	بنية mosquitto_evt_persist_بنياد_نياپو event_معلومات;
	بنية mosquitto__callback *cb_بنياد, *cb_ايندڙ;
	بنية mosquitto__security_options *opts;

	إذا(بنياد_نياپو->stored == false || db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));

	event_معلومات.معلومات.store_id = بنياد_نياپو->معلومات.store_id;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_بنياد_نياپو_delete, cb_بنياد, cb_ايندڙ){
		cb_بنياد->cb(MOSQ_EVT_PERSIST_بنياد_نياپو_DELETE, &event_معلومات, cb_بنياد->userdata);
	}
	بنياد_نياپو->stored = false;
}


فراغ plugin_persist_handle_retain_نياپو_سيٽ(بنية mosquitto_بنياد_نياپو *بنياد_نياپو)
{
	بنية mosquitto_evt_persist_retain_نياپو event_معلومات;
	بنية mosquitto__callback *cb_بنياد, *cb_ايندڙ;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));

	event_معلومات.store_id = بنياد_نياپو->معلومات.store_id;
	event_معلومات.topic = بنياد_نياپو->معلومات.topic;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_retain_نياپو_سيٽ, cb_بنياد, cb_ايندڙ){
		cb_بنياد->cb(MOSQ_EVT_PERSIST_RETAIN_نياپو_سيٽ, &event_معلومات, cb_بنياد->userdata);
	}
}


فراغ plugin_persist_handle_retain_نياپو_delete(بنية mosquitto_بنياد_نياپو *بنياد_نياپو)
{
	بنية mosquitto_evt_persist_retain_نياپو event_معلومات;
	بنية mosquitto__callback *cb_بنياد, *cb_ايندڙ;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_معلومات, 0, حجم(event_معلومات));

	event_معلومات.topic = بنياد_نياپو->معلومات.topic;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_retain_نياپو_delete, cb_بنياد, cb_ايندڙ){
		cb_بنياد->cb(MOSQ_EVT_PERSIST_RETAIN_نياپو_DELETE, &event_معلومات, cb_بنياد->userdata);
	}
}


فراغ plugin_persist_handle_will_جوڙيو(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_will_نياپو event_معلومات;
	بنية mosquitto__callback *cb_بنياد, *cb_ايندڙ;
	بنية mosquitto__security_options *opts;
	بنية mosquitto_پيغام *will_نياپو;

	إذا(db.shutdown || !context->will){
		أرجع;
	}

	opts = &db.config->security_options;
	will_نياپو = &context->will->نياپو;
	memset(&event_معلومات, 0, حجم(event_معلومات));
	event_معلومات.معلومات.clientid = context->id;
	event_معلومات.معلومات.topic = will_نياپو->topic;
	event_معلومات.معلومات.payload = will_نياپو->payload;
	event_معلومات.معلومات.payloadlen = (uint32_t)will_نياپو->payloadlen;
	event_معلومات.معلومات.qos = (uint8_t)will_نياپو->qos;
	event_معلومات.معلومات.retain = will_نياپو->retain;
	event_معلومات.معلومات.properties = context->will->properties;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_will_جوڙيو, cb_بنياد, cb_ايندڙ){
		cb_بنياد->cb(MOSQ_EVT_PERSIST_WILL_جوڙيو, &event_معلومات, cb_بنياد->userdata);
	}
}


فراغ plugin_persist__handle_will_delete(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_will_نياپو event_معلومات;
	بنية mosquitto__callback *cb_بنياد, *cb_ايندڙ;
	بنية mosquitto__security_options *opts;

	memset(&event_معلومات, 0, حجم(event_معلومات));
	event_معلومات.معلومات.clientid = context->id;

	إذا(db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_will_delete, cb_بنياد, cb_ايندڙ){
		cb_بنياد->cb(MOSQ_EVT_PERSIST_WILL_جوڙيو, &event_معلومات, cb_بنياد->userdata);
	}

}
