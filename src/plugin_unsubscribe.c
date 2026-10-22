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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "utlist.h"


સ્થિર પૂર્ણાંક plugin__handle_unsubscribe_single(સંરચના mosquitto__security_options *opts, સંરચના mosquitto *context, સંરચના mosquitto_subscription *sub)
{
	સંરચના mosquitto_evt_unsubscribe event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;
	પૂર્ણાંક rc = MOSQ_ERR_SUCCESS;

	memset(&event_data, 0, કદ(event_data));
	event_data.client = context;
	event_data.data.topic_filter = sub->topic_filter;
	event_data.data.properties = sub->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.unsubscribe, cb_base, cb_next){
		rc = cb_base->cb(MOSQ_EVT_UNSUBSCRIBE, &event_data, cb_base->userdata);
		જો(rc != MOSQ_ERR_SUCCESS){
			તોડો;
		}

		જો(sub->topic_filter != event_data.data.topic_filter){
			mosquitto_free(sub->topic_filter);
			sub->topic_filter = event_data.data.topic_filter;
		}
	}

	પાછા rc;
}


પૂર્ણાંક plugin__handle_unsubscribe(સંરચના mosquitto *context, સંરચના mosquitto_subscription *sub)
{
	પૂર્ણાંક rc = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	rc = plugin__handle_unsubscribe_single(&db.config->security_options,
			context, sub);
	જો(rc){
		પાછા rc;
	}

	જો(context->listener){
		rc = plugin__handle_unsubscribe_single(context->listener->security_options,
				context, sub);
	}

	પાછા rc;
}
