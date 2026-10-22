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

#include "config.h"

#include "mosquitto_broker_internal.h"
#include "utlist.h"

sa_nracanaa should_free {
	bool topic;
	bool payload;
	bool properties;
};


static ཧྲིལ plugin__handle_message_single(sa_nracanaa mosquitto__callback *callbacks, ga_nanaa mosquitto_plugin_event ev_type, sa_nracanaa should_free *to_free, sa_nracanaa mosquitto *context, sa_nracanaa mosquitto_base_msg *stored)
{
	sa_nracanaa mosquitto_evt_message event_data;
	sa_nracanaa mosquitto__callback *cb_base, *cb_next;
	ཧྲིལ rc = MOSQ_ERR_SUCCESS;

	memset(&event_data, 0, parimaa_na(event_data));
	event_data.client = context;
	event_data.topic = stored->topic;
	event_data.payloadlen = stored->payloadlen;
	event_data.payload = stored->payload;
	event_data.qos = stored->qos;
	event_data.retain = stored->retain;
	event_data.properties = stored->properties;

	DL_FOREACH_SAFE(callbacks, cb_base, cb_next){
		rc = cb_base->cb((ཧྲིལ)ev_type, &event_data, cb_base->userdata);
		གལ་སྲིད(rc != MOSQ_ERR_SUCCESS){
			bha_nga;
		}

		གལ་སྲིད(stored->topic != event_data.topic){
			གལ་སྲིད(to_free->topic){
				mosquitto_FREE(stored->topic);
			}
			stored->topic = event_data.topic;
			to_free->topic = true;
		}

		གལ་སྲིད(stored->payload != event_data.payload){
			གལ་སྲིད(to_free->payload){
				mosquitto_FREE(stored->payload);
			}
			stored->payload = event_data.payload;
			stored->payloadlen = event_data.payloadlen;
			to_free->payload = true;
		}

		གལ་སྲིད(stored->properties != event_data.properties){
			གལ་སྲིད(to_free->properties){
				mosquitto_property_free_all(&stored->properties);
			}
			stored->properties = event_data.properties;
			to_free->properties = true;
		}
	}

	stored->retain = event_data.retain;
	གལ་སྲིད(ev_type == MOSQ_EVT_MESSAGE_OUT){
		stored->qos = event_data.qos;
	}

	ལོག rc;
}


ཧྲིལ plugin__handle_message_out(sa_nracanaa mosquitto *context, sa_nracanaa mosquitto_base_msg *stored)
{
	ཧྲིལ rc = MOSQ_ERR_SUCCESS;
	sa_nracanaa should_free to_free = {false, false, false}; /* in msg_out, original data will be freed later */

	/* Global plugins */
	rc = plugin__handle_message_single(db.config->security_options.plugin_callbacks.message_out,
			MOSQ_EVT_MESSAGE_OUT, &to_free, context, stored);
	གལ་སྲིད(rc){
		ལོག rc;
	}

	གལ་སྲིད(context->listener){
		rc = plugin__handle_message_single(context->listener->security_options->plugin_callbacks.message_out,
				MOSQ_EVT_MESSAGE_OUT, &to_free, context, stored);
	}

	ལོག rc;
}


ཧྲིལ plugin__handle_message_in(sa_nracanaa mosquitto *context, sa_nracanaa mosquitto_base_msg *stored)
{
	ཧྲིལ rc = MOSQ_ERR_SUCCESS;
	sa_nracanaa should_free to_free = {true, true, true}; /* in msg_in, original data should be freed */

	/* Global plugins */
	rc = plugin__handle_message_single(db.config->security_options.plugin_callbacks.message_in,
			MOSQ_EVT_MESSAGE_IN, &to_free, context, stored);
	གལ་སྲིད(rc){
		ལོག rc;
	}

	གལ་སྲིད(context->listener){
		rc = plugin__handle_message_single(context->listener->security_options->plugin_callbacks.message_in,
				MOSQ_EVT_MESSAGE_IN, &to_free, context, stored);
	}

	ལོག rc;
}
