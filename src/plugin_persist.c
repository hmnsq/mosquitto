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
	ರಚನೆ mosquitto_evt_persist_restore event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;
	ರಚನೆ mosquitto__security_options *opts;

	opts = &db.config->security_options;
	memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));

	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.persist_restore, cb_ತಳ, cb_ಮುಂದಿನ){
		cb_ತಳ->cb(MOSQ_EVT_PERSIST_RESTORE, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
	}
}


ಶೂನ್ಯ plugin_persist_handle_client_ಕೂಡಿಸು(ರಚನೆ mosquitto *context)
{
	ರಚನೆ mosquitto_evt_persist_client event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;
	ರಚನೆ mosquitto__security_options *opts;

	ಆದರೆ(db.shutdown || context->is_persisted){
		ಹಿಂತಿರುಗಿ;
	}

	opts = &db.config->security_options;
	memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.clientid = context->id;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.username = context->username;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.auth_ವಿಧಾನ = context->auth_ವಿಧಾನ;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.will_delay_ಸಮಯ = context->will_delay_ಸಮಯ;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.session_expiry_ಸಮಯ = context->session_expiry_ಸಮಯ;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.will_delay_interval = context->will_delay_interval;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.session_expiry_interval = context->session_expiry_interval;
	ಆದರೆ(context->listener){
		event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.listener_ಪೋರ್ಟ್ = context->listener->ಪೋರ್ಟ್;
	}ಇಲ್ಲದಿದ್ದರೆ{
		event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.listener_ಪೋರ್ಟ್ = 0;
	}
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.max_qos = context->max_qos;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.retain_ಲಭ್ಯ = context->retain_ಲಭ್ಯ;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.max_packet_ಗಾತ್ರ = context->ಗರಿಷ್ಠ_packet_ಗಾತ್ರ;

	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.persist_client_ಕೂಡಿಸು, cb_ತಳ, cb_ಮುಂದಿನ){
		cb_ತಳ->cb(MOSQ_EVT_PERSIST_CLIENT_ಕೂಡಿಸು, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
	}

	ಆದರೆ(context->will){
		plugin_persist_handle_will_ಕೂಡಿಸು(context);
	}

	context->is_persisted = true;
}


ಶೂನ್ಯ plugin_persist__handle_client_update(ರಚನೆ mosquitto *context)
{
	ರಚನೆ mosquitto_evt_persist_client event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;
	ರಚನೆ mosquitto__security_options *opts;
	ರಚನೆ mosquitto_ಸಂದೇಶ_v5_2 will;

	ಬಳಸದ(will); /* FIXME */

	ಆದರೆ(db.shutdown){
		ಹಿಂತಿರುಗಿ;
	}

	opts = &db.config->security_options;
	memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.clientid = context->id;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.username = context->username;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.auth_ವಿಧಾನ = context->auth_ವಿಧಾನ;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.will_delay_ಸಮಯ = context->will_delay_ಸಮಯ;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.session_expiry_ಸಮಯ = context->session_expiry_ಸಮಯ;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.will_delay_interval = context->will_delay_interval;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.session_expiry_interval = context->session_expiry_interval;
	ಆದರೆ(context->listener){
		event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.listener_ಪೋರ್ಟ್ = context->listener->ಪೋರ್ಟ್;
	}ಇಲ್ಲದಿದ್ದರೆ{
		event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.listener_ಪೋರ್ಟ್ = 0;
	}
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.max_qos = context->max_qos;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.retain_ಲಭ್ಯ = context->retain_ಲಭ್ಯ;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.max_packet_ಗಾತ್ರ = context->ಗರಿಷ್ಠ_packet_ಗಾತ್ರ;

	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.persist_client_update, cb_ತಳ, cb_ಮುಂದಿನ){
		cb_ತಳ->cb(MOSQ_EVT_PERSIST_CLIENT_UPDATE, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
	}

	ಆದರೆ(context->will){
		plugin_persist_handle_will_ಕೂಡಿಸು(context);
	}ಇಲ್ಲದಿದ್ದರೆ{
		plugin_persist__handle_will_delete(context);
	}
}


ಶೂನ್ಯ plugin_persist__handle_client_delete(ರಚನೆ mosquitto *context)
{
	ರಚನೆ mosquitto_evt_persist_client event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;
	ರಚನೆ mosquitto__security_options *opts;

	ಆದರೆ(context->id == ಖಾಲಿ
			|| context->state == mosq_cs_duplicate
			|| db.shutdown){
		ಹಿಂತಿರುಗಿ;
	}

	plugin_persist__handle_will_delete(context);

	ಆದರೆ(context->is_persisted == false
			|| context->session_expiry_interval != MQTT_SESSION_EXPIRY_ತಕ್ಷಣ){
		ಹಿಂತಿರುಗಿ;
	}

	opts = &db.config->security_options;
	memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.clientid = context->id;

	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.persist_client_delete, cb_ತಳ, cb_ಮುಂದಿನ){
		cb_ತಳ->cb(MOSQ_EVT_PERSIST_CLIENT_DELETE, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
	}
	context->is_persisted = false;
}


ಶೂನ್ಯ plugin_persist_handle_subscription_ಕೂಡಿಸು(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_subscription *sub)
{
	ರಚನೆ mosquitto_evt_persist_subscription event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;
	ರಚನೆ mosquitto__security_options *opts;

	ಆದರೆ(db.shutdown || context->is_persisted == false){
		ಹಿಂತಿರುಗಿ;
	}

	opts = &db.config->security_options;
	memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.clientid = context->id;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.topic_filter = sub->topic_filter;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.identifier = sub->identifier;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.options = sub->options;

	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.persist_subscription_ಕೂಡಿಸು, cb_ತಳ, cb_ಮುಂದಿನ){
		cb_ತಳ->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_ಕೂಡಿಸು, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
	}
}


ಶೂನ್ಯ plugin_persist__handle_subscription_delete(ರಚನೆ mosquitto *context, ಅಕ್ಷರ *sub)
{
	ರಚನೆ mosquitto_evt_persist_subscription event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;
	ರಚನೆ mosquitto__security_options *opts;

	ಆದರೆ(db.shutdown || context->is_persisted == false){
		ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ(!sub){
		ಹಿಂತಿರುಗಿ;
	}

	opts = &db.config->security_options;
	memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.clientid = context->id;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.topic_filter = sub;

	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.persist_subscription_delete, cb_ತಳ, cb_ಮುಂದಿನ){
		cb_ತಳ->cb(MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
	}
}


ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಗಣ_client_ಸಂದೇಶ_event_ದತ್ತಾಂಶ(ರಚನೆ mosquitto_evt_persist_client_ಸಂದೇಶ *event_ದತ್ತಾಂಶ, ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ)
{
	event_ದತ್ತಾಂಶ->ದತ್ತಾಂಶ.clientid = context->id;
	event_ದತ್ತಾಂಶ->ದತ್ತಾಂಶ.cmsg_id = client_ಸಂದೇಶ->ದತ್ತಾಂಶ.cmsg_id;
	event_ದತ್ತಾಂಶ->ದತ್ತಾಂಶ.direction = (uint8_t)client_ಸಂದೇಶ->ದತ್ತಾಂಶ.direction;
	event_ದತ್ತಾಂಶ->ದತ್ತಾಂಶ.dup = client_ಸಂದೇಶ->ದತ್ತಾಂಶ.dup;
	event_ದತ್ತಾಂಶ->ದತ್ತಾಂಶ.mid = client_ಸಂದೇಶ->ದತ್ತಾಂಶ.mid;
	event_ದತ್ತಾಂಶ->ದತ್ತಾಂಶ.qos = client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos;
	event_ದತ್ತಾಂಶ->ದತ್ತಾಂಶ.retain = client_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain;
	event_ದತ್ತಾಂಶ->ದತ್ತಾಂಶ.state = (uint8_t)client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state;
	event_ದತ್ತಾಂಶ->ದತ್ತಾಂಶ.store_id = client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.store_id;
	event_ದತ್ತಾಂಶ->ದತ್ತಾಂಶ.subscription_identifier = client_ಸಂದೇಶ->ದತ್ತಾಂಶ.subscription_identifier;
}


ಶೂನ್ಯ plugin_persist_handle_client_ಸಂದೇಶ_ಕೂಡಿಸು(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ)
{
	ರಚನೆ mosquitto_evt_persist_client_ಸಂದೇಶ event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;
	ರಚನೆ mosquitto__security_options *opts;

	ಆದರೆ(context->is_persisted == false
			|| (client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos == 0 && db.config->ಸರತಿ_qos0_messages == false)
			|| db.shutdown){

		ಹಿಂತಿರುಗಿ;
	}

	opts = &db.config->security_options;
	memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));

	ಗಣ_client_ಸಂದೇಶ_event_ದತ್ತಾಂಶ(&event_ದತ್ತಾಂಶ, context, client_ಸಂದೇಶ);

	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.persist_client_ಸಂದೇಶ_ಕೂಡಿಸು, cb_ತಳ, cb_ಮುಂದಿನ){
		cb_ತಳ->cb(MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_ಕೂಡಿಸು, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
	}
}


ಶೂನ್ಯ plugin_persist_handle_client_ಸಂದೇಶ_delete(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ)
{
	ರಚನೆ mosquitto_evt_persist_client_ಸಂದೇಶ event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;
	ರಚನೆ mosquitto__security_options *opts;

	ಆದರೆ(context->is_persisted == false
			|| (client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos == 0 && db.config->ಸರತಿ_qos0_messages == false)
			|| db.shutdown){

		ಹಿಂತಿರುಗಿ;
	}

	opts = &db.config->security_options;
	memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));

	ಗಣ_client_ಸಂದೇಶ_event_ದತ್ತಾಂಶ(&event_ದತ್ತಾಂಶ, context, client_ಸಂದೇಶ);

	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.persist_client_ಸಂದೇಶ_delete, cb_ತಳ, cb_ಮುಂದಿನ){
		cb_ತಳ->cb(MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_DELETE, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
	}
}


ಶೂನ್ಯ plugin_persist_handle_client_ಸಂದೇಶ_update(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ)
{
	ರಚನೆ mosquitto_evt_persist_client_ಸಂದೇಶ event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;
	ರಚನೆ mosquitto__security_options *opts;

	ಆದರೆ(context->is_persisted == false
			|| (client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos == 0 && db.config->ಸರತಿ_qos0_messages == false)
			|| db.shutdown){

		ಹಿಂತಿರುಗಿ;
	}

	opts = &db.config->security_options;
	memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));

	ಗಣ_client_ಸಂದೇಶ_event_ದತ್ತಾಂಶ(&event_ದತ್ತಾಂಶ, context, client_ಸಂದೇಶ);

	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.persist_client_ಸಂದೇಶ_update, cb_ತಳ, cb_ಮುಂದಿನ){
		cb_ತಳ->cb(MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_UPDATE, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
	}
}


ಶೂನ್ಯ plugin_persist_handle_ತಳ_ಸಂದೇಶ_ಕೂಡಿಸು(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ)
{
	ರಚನೆ mosquitto_evt_persist_ತಳ_ಸಂದೇಶ event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;
	ರಚನೆ mosquitto__security_options *opts;

	ಆದರೆ(ತಳ_ಸಂದೇಶ->stored || db.shutdown){
		ಹಿಂತಿರುಗಿ;
	}

	opts = &db.config->security_options;
	memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));

	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.store_id = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.store_id;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.expiry_ಸಮಯ = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.expiry_ಸಮಯ;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.topic = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.payload = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.ಮೂಲ_id = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_id;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.ಮೂಲ_username = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_username;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.properties = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.properties;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.payloadlen = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.ಮೂಲ_mid = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid;
	ಆದರೆ(ತಳ_ಸಂದೇಶ->ಮೂಲ_listener){
		event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.ಮೂಲ_ಪೋರ್ಟ್ = ತಳ_ಸಂದೇಶ->ಮೂಲ_listener->ಪೋರ್ಟ್;
	}ಇಲ್ಲದಿದ್ದರೆ{
		event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.ಮೂಲ_ಪೋರ್ಟ್ = 0;
	}
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.qos = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.retain = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain;

	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.persist_ತಳ_ಸಂದೇಶ_ಕೂಡಿಸು, cb_ತಳ, cb_ಮುಂದಿನ){
		cb_ತಳ->cb(MOSQ_EVT_PERSIST_ತಳ_ಸಂದೇಶ_ಕೂಡಿಸು, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
	}
	ತಳ_ಸಂದೇಶ->stored = true;
}


ಶೂನ್ಯ plugin_persist_handle_ತಳ_ಸಂದೇಶ_delete(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ)
{
	ರಚನೆ mosquitto_evt_persist_ತಳ_ಸಂದೇಶ event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;
	ರಚನೆ mosquitto__security_options *opts;

	ಆದರೆ(ತಳ_ಸಂದೇಶ->stored == false || db.shutdown){
		ಹಿಂತಿರುಗಿ;
	}

	opts = &db.config->security_options;
	memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));

	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.store_id = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.store_id;

	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.persist_ತಳ_ಸಂದೇಶ_delete, cb_ತಳ, cb_ಮುಂದಿನ){
		cb_ತಳ->cb(MOSQ_EVT_PERSIST_ತಳ_ಸಂದೇಶ_DELETE, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
	}
	ತಳ_ಸಂದೇಶ->stored = false;
}


ಶೂನ್ಯ plugin_persist_handle_retain_ಸಂದೇಶ_ಗಣ(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ)
{
	ರಚನೆ mosquitto_evt_persist_retain_ಸಂದೇಶ event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;
	ರಚನೆ mosquitto__security_options *opts;

	ಆದರೆ(db.shutdown){
		ಹಿಂತಿರುಗಿ;
	}

	opts = &db.config->security_options;
	memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));

	event_ದತ್ತಾಂಶ.store_id = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.store_id;
	event_ದತ್ತಾಂಶ.topic = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic;

	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.persist_retain_ಸಂದೇಶ_ಗಣ, cb_ತಳ, cb_ಮುಂದಿನ){
		cb_ತಳ->cb(MOSQ_EVT_PERSIST_RETAIN_ಸಂದೇಶ_ಗಣ, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
	}
}


ಶೂನ್ಯ plugin_persist_handle_retain_ಸಂದೇಶ_delete(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ)
{
	ರಚನೆ mosquitto_evt_persist_retain_ಸಂದೇಶ event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;
	ರಚನೆ mosquitto__security_options *opts;

	ಆದರೆ(db.shutdown){
		ಹಿಂತಿರುಗಿ;
	}

	opts = &db.config->security_options;
	memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));

	event_ದತ್ತಾಂಶ.topic = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic;

	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.persist_retain_ಸಂದೇಶ_delete, cb_ತಳ, cb_ಮುಂದಿನ){
		cb_ತಳ->cb(MOSQ_EVT_PERSIST_RETAIN_ಸಂದೇಶ_DELETE, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
	}
}


ಶೂನ್ಯ plugin_persist_handle_will_ಕೂಡಿಸು(ರಚನೆ mosquitto *context)
{
	ರಚನೆ mosquitto_evt_persist_will_ಸಂದೇಶ event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;
	ರಚನೆ mosquitto__security_options *opts;
	ರಚನೆ mosquitto_ಸಂದೇಶ *will_ಸಂದೇಶ;

	ಆದರೆ(db.shutdown || !context->will){
		ಹಿಂತಿರುಗಿ;
	}

	opts = &db.config->security_options;
	will_ಸಂದೇಶ = &context->will->ಸಂದೇಶ;
	memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.clientid = context->id;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.topic = will_ಸಂದೇಶ->topic;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.payload = will_ಸಂದೇಶ->payload;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.payloadlen = (uint32_t)will_ಸಂದೇಶ->payloadlen;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.qos = (uint8_t)will_ಸಂದೇಶ->qos;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.retain = will_ಸಂದೇಶ->retain;
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.properties = context->will->properties;

	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.persist_will_ಕೂಡಿಸು, cb_ತಳ, cb_ಮುಂದಿನ){
		cb_ತಳ->cb(MOSQ_EVT_PERSIST_WILL_ಕೂಡಿಸು, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
	}
}


ಶೂನ್ಯ plugin_persist__handle_will_delete(ರಚನೆ mosquitto *context)
{
	ರಚನೆ mosquitto_evt_persist_will_ಸಂದೇಶ event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;
	ರಚನೆ mosquitto__security_options *opts;

	memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));
	event_ದತ್ತಾಂಶ.ದತ್ತಾಂಶ.clientid = context->id;

	ಆದರೆ(db.shutdown){
		ಹಿಂತಿರುಗಿ;
	}

	opts = &db.config->security_options;
	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.persist_will_delete, cb_ತಳ, cb_ಮುಂದಿನ){
		cb_ತಳ->cb(MOSQ_EVT_PERSIST_WILL_ಕೂಡಿಸು, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
	}

}
