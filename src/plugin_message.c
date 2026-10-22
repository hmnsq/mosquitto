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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "utlist.h"

ସଂରଚନା should_free {
	bool topic;
	bool payload;
	bool properties;
};


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__handle_message_single(ସଂରଚନା mosquitto__callback *callbacks, ଗଣନା mosquitto_plugin_event ev_type, ସଂରଚନା should_free *to_free, ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ_2 *stored)
{
	ସଂରଚନା mosquitto_evt_message event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_SUCCESS;

	memset(&event_data, 0, ଆକାର(event_data));
	event_data.client = context;
	event_data.topic = stored->topic;
	event_data.payloadlen = stored->payloadlen;
	event_data.payload = stored->payload;
	event_data.qos = stored->qos;
	event_data.retain = stored->retain;
	event_data.properties = stored->properties;

	DL_FOREACH_SAFE(callbacks, cb_base, cb_next){
		rc = cb_base->cb((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ev_type, &event_data, cb_base->userdata);
		ଯଦି(rc != MOSQ_ERR_SUCCESS){
			ଭାଙ୍ଗ;
		}

		ଯଦି(stored->topic != event_data.topic){
			ଯଦି(to_free->topic){
				mosquitto_FREE(stored->topic);
			}
			stored->topic = event_data.topic;
			to_free->topic = true;
		}

		ଯଦି(stored->payload != event_data.payload){
			ଯଦି(to_free->payload){
				mosquitto_FREE(stored->payload);
			}
			stored->payload = event_data.payload;
			stored->payloadlen = event_data.payloadlen;
			to_free->payload = true;
		}

		ଯଦି(stored->properties != event_data.properties){
			ଯଦି(to_free->properties){
				mosquitto_property_free_all(&stored->properties);
			}
			stored->properties = event_data.properties;
			to_free->properties = true;
		}
	}

	stored->retain = event_data.retain;
	ଯଦି(ev_type == MOSQ_EVT_MESSAGE_OUT){
		stored->qos = event_data.qos;
	}

	ଫେରନ୍ତୁ rc;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__handle_message_out(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ_2 *stored)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_SUCCESS;
	ସଂରଚନା should_free to_free = {false, false, false}; /* in msg_out, original data will be freed later */

	/* Global plugins */
	rc = plugin__handle_message_single(db.config->security_options.plugin_callbacks.message_out,
			MOSQ_EVT_MESSAGE_OUT, &to_free, context, stored);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	ଯଦି(context->listener){
		rc = plugin__handle_message_single(context->listener->security_options->plugin_callbacks.message_out,
				MOSQ_EVT_MESSAGE_OUT, &to_free, context, stored);
	}

	ଫେରନ୍ତୁ rc;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__handle_message_in(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ_2 *stored)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_SUCCESS;
	ସଂରଚନା should_free to_free = {true, true, true}; /* in msg_in, original data should be freed */

	/* Global plugins */
	rc = plugin__handle_message_single(db.config->security_options.plugin_callbacks.message_in,
			MOSQ_EVT_MESSAGE_IN, &to_free, context, stored);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	ଯଦି(context->listener){
		rc = plugin__handle_message_single(context->listener->security_options->plugin_callbacks.message_in,
				MOSQ_EVT_MESSAGE_IN, &to_free, context, stored);
	}

	ଫେରନ୍ତୁ rc;
}
