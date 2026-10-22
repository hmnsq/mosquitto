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
	بنية mosquitto_evt_persist_restore event_انکڑے;
	بنية mosquitto__callback *cb_ادھار, *cb_اگلا;
	بنية mosquitto__security_options *opts;

	opts = &db.config->security_options;
	memset(&event_انکڑے, 0, حجم(event_انکڑے));

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_restore, cb_ادھار, cb_اگلا){
		cb_ادھار->cb(MOSQ_EVT_PERSIST_RESTORE, &event_انکڑے, cb_ادھار->userdata);
	}
}


فراغ plugin_persist_handle_client_جوڑو(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_client event_انکڑے;
	بنية mosquitto__callback *cb_ادھار, *cb_اگلا;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown || context->is_persisted){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_انکڑے, 0, حجم(event_انکڑے));
	event_انکڑے.انکڑے.clientid = context->id;
	event_انکڑے.انکڑے.username = context->username;
	event_انکڑے.انکڑے.auth_طریقہ = context->auth_طریقہ;
	event_انکڑے.انکڑے.will_delay_ویلا = context->will_delay_ویلا;
	event_انکڑے.انکڑے.session_expiry_ویلا = context->session_expiry_ویلا;
	event_انکڑے.انکڑے.will_delay_interval = context->will_delay_interval;
	event_انکڑے.انکڑے.session_expiry_interval = context->session_expiry_interval;
	إذا(context->listener){
		event_انکڑے.انکڑے.listener_پورٹ = context->listener->پورٹ;
	}وإلا{
		event_انکڑے.انکڑے.listener_پورٹ = 0;
	}
	event_انکڑے.انکڑے.max_qos = context->max_qos;
	event_انکڑے.انکڑے.retain_دستیاب = context->retain_دستیاب;
	event_انکڑے.انکڑے.max_packet_ناپ = context->ودھ_توں_ودھ_packet_ناپ;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_client_جوڑو, cb_ادھار, cb_اگلا){
		cb_ادھار->cb(MOSQ_EVT_PERSIST_CLIENT_جوڑو, &event_انکڑے, cb_ادھار->userdata);
	}

	إذا(context->will){
		plugin_persist_handle_will_جوڑو(context);
	}

	context->is_persisted = true;
}


فراغ plugin_persist__handle_client_update(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_client event_انکڑے;
	بنية mosquitto__callback *cb_ادھار, *cb_اگلا;
	بنية mosquitto__security_options *opts;
	بنية mosquitto_سنیہا_v5_2 will;

	بن_ورتیا(will); /* FIXME */

	إذا(db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_انکڑے, 0, حجم(event_انکڑے));
	event_انکڑے.انکڑے.clientid = context->id;
	event_انکڑے.انکڑے.username = context->username;
	event_انکڑے.انکڑے.auth_طریقہ = context->auth_طریقہ;
	event_انکڑے.انکڑے.will_delay_ویلا = context->will_delay_ویلا;
	event_انکڑے.انکڑے.session_expiry_ویلا = context->session_expiry_ویلا;
	event_انکڑے.انکڑے.will_delay_interval = context->will_delay_interval;
	event_انکڑے.انکڑے.session_expiry_interval = context->session_expiry_interval;
	إذا(context->listener){
		event_انکڑے.انکڑے.listener_پورٹ = context->listener->پورٹ;
	}وإلا{
		event_انکڑے.انکڑے.listener_پورٹ = 0;
	}
	event_انکڑے.انکڑے.max_qos = context->max_qos;
	event_انکڑے.انکڑے.retain_دستیاب = context->retain_دستیاب;
	event_انکڑے.انکڑے.max_packet_ناپ = context->ودھ_توں_ودھ_packet_ناپ;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_client_update, cb_ادھار, cb_اگلا){
		cb_ادھار->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_انکڑے, cb_ادھار->userdata);
	}

	إذا(context->will){
		plugin_persist_handle_will_جوڑو(context);
	}وإلا{
		plugin_persist__handle_will_delete(context);
	}
}


فراغ plugin_persist__handle_client_delete(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_client event_انکڑے;
	بنية mosquitto__callback *cb_ادھار, *cb_اگلا;
	بنية mosquitto__security_options *opts;

	إذا(context->id == عدم
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		أرجع;
	}

	plugin_persist__handle_will_delete(context);

	إذا(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_فوری){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_انکڑے, 0, حجم(event_انکڑے));
	event_انکڑے.انکڑے.clientid = context->id;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_client_delete, cb_ادھار, cb_اگلا){
		cb_ادھار->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_انکڑے, cb_ادھار->userdata);
	}
	context->is_persisted = false;
}


فراغ plugin_persist_handle_subscription_جوڑو(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub)
{
	بنية mosquitto_evt_persist_subscription event_انکڑے;
	بنية mosquitto__callback *cb_ادھار, *cb_اگلا;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown || context->is_persisted == false){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_انکڑے, 0, حجم(event_انکڑے));
	event_انکڑے.انکڑے.clientid = context->id;
	event_انکڑے.انکڑے.topic_filter = sub->topic_filter;
	event_انکڑے.انکڑے.identifier = sub->identifier;
	event_انکڑے.انکڑے.options = sub->options;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_subscription_جوڑو, cb_ادھار, cb_اگلا){
		cb_ادھار->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_جوڑو, &event_انکڑے, cb_ادھار->userdata);
	}
}


فراغ plugin_persist__handle_subscription_delete(بنية mosquitto *context, حرف *sub)
{
	بنية mosquitto_evt_persist_subscription event_انکڑے;
	بنية mosquitto__callback *cb_ادھار, *cb_اگلا;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown || context->is_persisted == false){
		أرجع;
	}
	إذا(!sub){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_انکڑے, 0, حجم(event_انکڑے));
	event_انکڑے.انکڑے.clientid = context->id;
	event_انکڑے.انکڑے.topic_filter = sub;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_subscription_delete, cb_ادھار, cb_اگلا){
		cb_ادھار->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_انکڑے, cb_ادھار->userdata);
	}
}


ساكن مضمن فراغ سیٹ_client_پیغام_event_انکڑے(بنية mosquitto_evt_persist_client_پیغام *event_انکڑے, بنية mosquitto *context, ثابت بنية mosquitto_client_پیغام *client_پیغام)
{
	event_انکڑے->انکڑے.clientid = context->id;
	event_انکڑے->انکڑے.cmsg_id = client_پیغام->انکڑے.cmsg_id;
	event_انکڑے->انکڑے.direction = (uint8_t)client_پیغام->انکڑے.direction;
	event_انکڑے->انکڑے.dup = client_پیغام->انکڑے.dup;
	event_انکڑے->انکڑے.mid = client_پیغام->انکڑے.mid;
	event_انکڑے->انکڑے.qos = client_پیغام->انکڑے.qos;
	event_انکڑے->انکڑے.retain = client_پیغام->انکڑے.retain;
	event_انکڑے->انکڑے.state = (uint8_t)client_پیغام->انکڑے.state;
	event_انکڑے->انکڑے.store_id = client_پیغام->ادھار_پیغام->انکڑے.store_id;
	event_انکڑے->انکڑے.subscription_identifier = client_پیغام->انکڑے.subscription_identifier;
}


فراغ plugin_persist_handle_client_پیغام_جوڑو(بنية mosquitto *context, ثابت بنية mosquitto_client_پیغام *client_پیغام)
{
	بنية mosquitto_evt_persist_client_پیغام event_انکڑے;
	بنية mosquitto__callback *cb_ادھار, *cb_اگلا;
	بنية mosquitto__security_options *opts;

	إذا(context->is_persisted == false
			|| (client_پیغام->انکڑے.qos == 0 && db.config->قطار_qos0_messages == false)
			|| db.shutdown){

		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_انکڑے, 0, حجم(event_انکڑے));

	سیٹ_client_پیغام_event_انکڑے(&event_انکڑے, context, client_پیغام);

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_client_پیغام_جوڑو, cb_ادھار, cb_اگلا){
		cb_ادھار->cb(MOSQ_EVT_PERSIST_CLIENT_پیغام_جوڑو, &event_انکڑے, cb_ادھار->userdata);
	}
}


فراغ plugin_persist_handle_client_پیغام_delete(بنية mosquitto *context, ثابت بنية mosquitto_client_پیغام *client_پیغام)
{
	بنية mosquitto_evt_persist_client_پیغام event_انکڑے;
	بنية mosquitto__callback *cb_ادھار, *cb_اگلا;
	بنية mosquitto__security_options *opts;

	إذا(context->is_persisted == false
			|| (client_پیغام->انکڑے.qos == 0 && db.config->قطار_qos0_messages == false)
			|| db.shutdown){

		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_انکڑے, 0, حجم(event_انکڑے));

	سیٹ_client_پیغام_event_انکڑے(&event_انکڑے, context, client_پیغام);

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_client_پیغام_delete, cb_ادھار, cb_اگلا){
		cb_ادھار->cb(MOSQ_EVT_PERSIST_CLIENT_پیغام_DELETE, &event_انکڑے, cb_ادھار->userdata);
	}
}


فراغ plugin_persist_handle_client_پیغام_update(بنية mosquitto *context, ثابت بنية mosquitto_client_پیغام *client_پیغام)
{
	بنية mosquitto_evt_persist_client_پیغام event_انکڑے;
	بنية mosquitto__callback *cb_ادھار, *cb_اگلا;
	بنية mosquitto__security_options *opts;

	إذا(context->is_persisted == false
			|| (client_پیغام->انکڑے.qos == 0 && db.config->قطار_qos0_messages == false)
			|| db.shutdown){

		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_انکڑے, 0, حجم(event_انکڑے));

	سیٹ_client_پیغام_event_انکڑے(&event_انکڑے, context, client_پیغام);

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_client_پیغام_update, cb_ادھار, cb_اگلا){
		cb_ادھار->cb(MOSQ_EVT_PERSIST_CLIENT_پیغام_UPDATE, &event_انکڑے, cb_ادھار->userdata);
	}
}


فراغ plugin_persist_handle_ادھار_پیغام_جوڑو(بنية mosquitto_ادھار_پیغام *ادھار_پیغام)
{
	بنية mosquitto_evt_persist_ادھار_پیغام event_انکڑے;
	بنية mosquitto__callback *cb_ادھار, *cb_اگلا;
	بنية mosquitto__security_options *opts;

	إذا(ادھار_پیغام->stored || db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_انکڑے, 0, حجم(event_انکڑے));

	event_انکڑے.انکڑے.store_id = ادھار_پیغام->انکڑے.store_id;
	event_انکڑے.انکڑے.expiry_ویلا = ادھار_پیغام->انکڑے.expiry_ویلا;
	event_انکڑے.انکڑے.topic = ادھار_پیغام->انکڑے.topic;
	event_انکڑے.انکڑے.payload = ادھار_پیغام->انکڑے.payload;
	event_انکڑے.انکڑے.سروت_id = ادھار_پیغام->انکڑے.سروت_id;
	event_انکڑے.انکڑے.سروت_username = ادھار_پیغام->انکڑے.سروت_username;
	event_انکڑے.انکڑے.properties = ادھار_پیغام->انکڑے.properties;
	event_انکڑے.انکڑے.payloadlen = ادھار_پیغام->انکڑے.payloadlen;
	event_انکڑے.انکڑے.سروت_mid = ادھار_پیغام->انکڑے.سروت_mid;
	إذا(ادھار_پیغام->سروت_listener){
		event_انکڑے.انکڑے.سروت_پورٹ = ادھار_پیغام->سروت_listener->پورٹ;
	}وإلا{
		event_انکڑے.انکڑے.سروت_پورٹ = 0;
	}
	event_انکڑے.انکڑے.qos = ادھار_پیغام->انکڑے.qos;
	event_انکڑے.انکڑے.retain = ادھار_پیغام->انکڑے.retain;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_ادھار_پیغام_جوڑو, cb_ادھار, cb_اگلا){
		cb_ادھار->cb(MOSQ_EVT_PERSIST_ادھار_پیغام_جوڑو, &event_انکڑے, cb_ادھار->userdata);
	}
	ادھار_پیغام->stored = true;
}


فراغ plugin_persist_handle_ادھار_پیغام_delete(بنية mosquitto_ادھار_پیغام *ادھار_پیغام)
{
	بنية mosquitto_evt_persist_ادھار_پیغام event_انکڑے;
	بنية mosquitto__callback *cb_ادھار, *cb_اگلا;
	بنية mosquitto__security_options *opts;

	إذا(ادھار_پیغام->stored == false || db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_انکڑے, 0, حجم(event_انکڑے));

	event_انکڑے.انکڑے.store_id = ادھار_پیغام->انکڑے.store_id;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_ادھار_پیغام_delete, cb_ادھار, cb_اگلا){
		cb_ادھار->cb(MOSQ_EVT_PERSIST_ادھار_پیغام_DELETE, &event_انکڑے, cb_ادھار->userdata);
	}
	ادھار_پیغام->stored = false;
}


فراغ plugin_persist_handle_retain_پیغام_سیٹ(بنية mosquitto_ادھار_پیغام *ادھار_پیغام)
{
	بنية mosquitto_evt_persist_retain_پیغام event_انکڑے;
	بنية mosquitto__callback *cb_ادھار, *cb_اگلا;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_انکڑے, 0, حجم(event_انکڑے));

	event_انکڑے.store_id = ادھار_پیغام->انکڑے.store_id;
	event_انکڑے.topic = ادھار_پیغام->انکڑے.topic;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_retain_پیغام_سیٹ, cb_ادھار, cb_اگلا){
		cb_ادھار->cb(MOSQ_EVT_PERSIST_RETAIN_پیغام_سیٹ, &event_انکڑے, cb_ادھار->userdata);
	}
}


فراغ plugin_persist_handle_retain_پیغام_delete(بنية mosquitto_ادھار_پیغام *ادھار_پیغام)
{
	بنية mosquitto_evt_persist_retain_پیغام event_انکڑے;
	بنية mosquitto__callback *cb_ادھار, *cb_اگلا;
	بنية mosquitto__security_options *opts;

	إذا(db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	memset(&event_انکڑے, 0, حجم(event_انکڑے));

	event_انکڑے.topic = ادھار_پیغام->انکڑے.topic;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_retain_پیغام_delete, cb_ادھار, cb_اگلا){
		cb_ادھار->cb(MOSQ_EVT_PERSIST_RETAIN_پیغام_DELETE, &event_انکڑے, cb_ادھار->userdata);
	}
}


فراغ plugin_persist_handle_will_جوڑو(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_will_پیغام event_انکڑے;
	بنية mosquitto__callback *cb_ادھار, *cb_اگلا;
	بنية mosquitto__security_options *opts;
	بنية mosquitto_سنیہا *will_پیغام;

	إذا(db.shutdown || !context->will){
		أرجع;
	}

	opts = &db.config->security_options;
	will_پیغام = &context->will->پیغام;
	memset(&event_انکڑے, 0, حجم(event_انکڑے));
	event_انکڑے.انکڑے.clientid = context->id;
	event_انکڑے.انکڑے.topic = will_پیغام->topic;
	event_انکڑے.انکڑے.payload = will_پیغام->payload;
	event_انکڑے.انکڑے.payloadlen = (uint32_t)will_پیغام->payloadlen;
	event_انکڑے.انکڑے.qos = (uint8_t)will_پیغام->qos;
	event_انکڑے.انکڑے.retain = will_پیغام->retain;
	event_انکڑے.انکڑے.properties = context->will->properties;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_will_جوڑو, cb_ادھار, cb_اگلا){
		cb_ادھار->cb(MOSQ_EVT_PERSIST_WILL_جوڑو, &event_انکڑے, cb_ادھار->userdata);
	}
}


فراغ plugin_persist__handle_will_delete(بنية mosquitto *context)
{
	بنية mosquitto_evt_persist_will_پیغام event_انکڑے;
	بنية mosquitto__callback *cb_ادھار, *cb_اگلا;
	بنية mosquitto__security_options *opts;

	memset(&event_انکڑے, 0, حجم(event_انکڑے));
	event_انکڑے.انکڑے.clientid = context->id;

	إذا(db.shutdown){
		أرجع;
	}

	opts = &db.config->security_options;
	DL_FOREACH_محفوظ(opts->plugin_callbacks.persist_will_delete, cb_ادھار, cb_اگلا){
		cb_ادھار->cb(MOSQ_EVT_PERSIST_WILL_جوڑو, &event_انکڑے, cb_ادھار->userdata);
	}

}
