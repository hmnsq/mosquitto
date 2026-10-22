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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "utlist.h"


স্থির পূর্ণাঙ্ক plugin__handle_subscribe_single(সংরচনা mosquitto__security_options *opts, সংরচনা mosquitto *context, সংরচনা mosquitto_subscription *sub)
{
	সংরচনা mosquitto_evt_subscribe event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;
	পূর্ণাঙ্ক rc = MOSQ_ERR_SUCCESS;

	memset(&event_data, 0, পরিমাণ(event_data));
	event_data.client = context;
	event_data.data.topic_filter = sub->topic_filter;
	event_data.data.options = sub->options;
	event_data.data.identifier = sub->identifier;
	event_data.data.properties = sub->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.subscribe, cb_base, cb_next){
		rc = cb_base->cb(MOSQ_EVT_SUBSCRIBE, &event_data, cb_base->userdata);
		যদি(rc != MOSQ_ERR_SUCCESS){
			ভঙ্গ;
		}

		যদি(sub->topic_filter != event_data.data.topic_filter){
			mosquitto_free(sub->topic_filter);
			sub->topic_filter = event_data.data.topic_filter;
		}
	}
	sub->options = event_data.data.options;

	প্রত্যাগম rc;
}


পূর্ণাঙ্ক plugin__handle_subscribe(সংরচনা mosquitto *context, সংরচনা mosquitto_subscription *sub)
{
	পূর্ণাঙ্ক rc = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	rc = plugin__handle_subscribe_single(&db.config->security_options, context, sub);
	যদি(rc){
		প্রত্যাগম rc;
	}

	যদি(context->listener){
		rc = plugin__handle_subscribe_single(context->listener->security_options, context, sub);
	}

	প্রত্যাগম rc;
}
