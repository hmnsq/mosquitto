/*
Copyright (c) 2016-2021 Roger Light <roger@atchoo.org>

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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "utlist.h"

સંરચના should_free {
	bool topic;
	bool payload;
	bool properties;
};


સ્થિર પૂર્ણાંક plugin__handle_message_single(સંરચના mosquitto__callback *callbacks, ગણના mosquitto_plugin_event ev_type, સંરચના should_free *to_free, સંરચના mosquitto *context, સંરચના mosquitto_base_સંદેશ_2 *stored)
{
	સંરચના mosquitto_evt_message event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	પૂર્ણાંક rc = MOSQ_ERR_SUCCESS;

	memset(&event_data, 0, કદ(event_data));
	event_data.client = context;
	event_data.topic = stored->topic;
	event_data.payloadlen = stored->payloadlen;
	event_data.payload = stored->payload;
	event_data.qos = stored->qos;
	event_data.retain = stored->retain;
	event_data.properties = stored->properties;

	DL_FOREACH_SAFE(callbacks, cb_base, cb_next){
		rc = cb_base->cb((પૂર્ણાંક)ev_type, &event_data, cb_base->userdata);
		જો(rc != MOSQ_ERR_SUCCESS){
			તોડો;
		}

		જો(stored->topic != event_data.topic){
			જો(to_free->topic){
				mosquitto_FREE(stored->topic);
			}
			stored->topic = event_data.topic;
			to_free->topic = true;
		}

		જો(stored->payload != event_data.payload){
			જો(to_free->payload){
				mosquitto_FREE(stored->payload);
			}
			stored->payload = event_data.payload;
			stored->payloadlen = event_data.payloadlen;
			to_free->payload = true;
		}

		જો(stored->properties != event_data.properties){
			જો(to_free->properties){
				mosquitto_property_free_all(&stored->properties);
			}
			stored->properties = event_data.properties;
			to_free->properties = true;
		}
	}

	stored->retain = event_data.retain;
	જો(ev_type == MOSQ_EVT_MESSAGE_OUT){
		stored->qos = event_data.qos;
	}

	પાછા rc;
}


પૂર્ણાંક plugin__handle_message_out(સંરચના mosquitto *context, સંરચના mosquitto_base_સંદેશ_2 *stored)
{
	પૂર્ણાંક rc = MOSQ_ERR_SUCCESS;
	સંરચના should_free to_free = {false, false, false}; /* in msg_out, original data will be freed later */

	/* Global plugins */
	rc = plugin__handle_message_single(db.config->security_options.plugin_callbacks.message_out,
			MOSQ_EVT_MESSAGE_OUT, &to_free, context, stored);
	જો(rc){
		પાછા rc;
	}

	જો(context->listener){
		rc = plugin__handle_message_single(context->listener->security_options->plugin_callbacks.message_out,
				MOSQ_EVT_MESSAGE_OUT, &to_free, context, stored);
	}

	પાછા rc;
}


પૂર્ણાંક plugin__handle_message_in(સંરચના mosquitto *context, સંરચના mosquitto_base_સંદેશ_2 *stored)
{
	પૂર્ણાંક rc = MOSQ_ERR_SUCCESS;
	સંરચના should_free to_free = {true, true, true}; /* in msg_in, original data should be freed */

	/* Global plugins */
	rc = plugin__handle_message_single(db.config->security_options.plugin_callbacks.message_in,
			MOSQ_EVT_MESSAGE_IN, &to_free, context, stored);
	જો(rc){
		પાછા rc;
	}

	જો(context->listener){
		rc = plugin__handle_message_single(context->listener->security_options->plugin_callbacks.message_in,
				MOSQ_EVT_MESSAGE_IN, &to_free, context, stored);
	}

	પાછા rc;
}
