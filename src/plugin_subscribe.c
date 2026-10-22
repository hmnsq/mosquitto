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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "utlist.h"


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__handle_subscribe_single(ସଂରଚନା mosquitto__security_options *opts, ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_subscription *sub)
{
	ସଂରଚନା mosquitto_evt_subscribe event_data;
	ସଂରଚନା mosquitto__callback *cb_base, *cb_next;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_SUCCESS;

	memset(&event_data, 0, ଆକାର(event_data));
	event_data.client = context;
	event_data.data.topic_filter = sub->topic_filter;
	event_data.data.options = sub->options;
	event_data.data.identifier = sub->identifier;
	event_data.data.properties = sub->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.subscribe, cb_base, cb_next){
		rc = cb_base->cb(MOSQ_EVT_SUBSCRIBE, &event_data, cb_base->userdata);
		ଯଦି(rc != MOSQ_ERR_SUCCESS){
			ଭାଙ୍ଗ;
		}

		ଯଦି(sub->topic_filter != event_data.data.topic_filter){
			mosquitto_free(sub->topic_filter);
			sub->topic_filter = event_data.data.topic_filter;
		}
	}
	sub->options = event_data.data.options;

	ଫେରନ୍ତୁ rc;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__handle_subscribe(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_subscription *sub)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	rc = plugin__handle_subscribe_single(&db.config->security_options, context, sub);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	ଯଦି(context->listener){
		rc = plugin__handle_subscribe_single(context->listener->security_options, context, sub);
	}

	ଫେରନ୍ତୁ rc;
}
