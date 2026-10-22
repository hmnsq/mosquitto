/*
Copyright (c) 2016-2022 Roger Light <roger@atchoo.org>
Copyright (c) 2022 Cedalo GmbH

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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "utlist.h"


நிலையான முழு plugin__handle_subscribe_single(கட்டமைப்பு mosquitto__security_options *opts, கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto_subscription *sub)
{
	கட்டமைப்பு mosquitto_evt_subscribe event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;
	முழு rc = MOSQ_ERR_SUCCESS;

	memset(&event_data, 0, அளவு(event_data));
	event_data.client = context;
	event_data.data.topic_filter = sub->topic_filter;
	event_data.data.options = sub->options;
	event_data.data.identifier = sub->identifier;
	event_data.data.properties = sub->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.subscribe, cb_base, cb_next){
		rc = cb_base->cb(MOSQ_EVT_SUBSCRIBE, &event_data, cb_base->userdata);
		எனில்(rc != MOSQ_ERR_SUCCESS){
			நிறுத்து;
		}

		எனில்(sub->topic_filter != event_data.data.topic_filter){
			mosquitto_free(sub->topic_filter);
			sub->topic_filter = event_data.data.topic_filter;
		}
	}
	sub->options = event_data.data.options;

	திரும்பு rc;
}


முழு plugin__handle_subscribe(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto_subscription *sub)
{
	முழு rc = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	rc = plugin__handle_subscribe_single(&db.config->security_options, context, sub);
	எனில்(rc){
		திரும்பு rc;
	}

	எனில்(context->listener){
		rc = plugin__handle_subscribe_single(context->listener->security_options, context, sub);
	}

	திரும்பு rc;
}
