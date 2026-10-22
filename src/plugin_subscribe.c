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


স্থির পূর্ণসংখ্যা plugin__handle_subscribe_single(কাঠামো mosquitto__security_options *opts, কাঠামো mosquitto *context, কাঠামো mosquitto_subscription *sub)
{
	কাঠামো mosquitto_evt_subscribe event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;
	পূর্ণসংখ্যা rc = MOSQ_ERR_SUCCESS;

	memset(&event_তথ্য, 0, আকার(event_তথ্য));
	event_তথ্য.client = context;
	event_তথ্য.তথ্য.topic_filter = sub->topic_filter;
	event_তথ্য.তথ্য.options = sub->options;
	event_তথ্য.তথ্য.identifier = sub->identifier;
	event_তথ্য.তথ্য.properties = sub->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.subscribe, cb_base, cb_পরবর্তী){
		rc = cb_base->cb(MOSQ_EVT_SUBSCRIBE, &event_তথ্য, cb_base->userdata);
		যদি(rc != MOSQ_ERR_SUCCESS){
			বিরতি;
		}

		যদি(sub->topic_filter != event_তথ্য.তথ্য.topic_filter){
			mosquitto_free(sub->topic_filter);
			sub->topic_filter = event_তথ্য.তথ্য.topic_filter;
		}
	}
	sub->options = event_তথ্য.তথ্য.options;

	ফেরত rc;
}


পূর্ণসংখ্যা plugin__handle_subscribe(কাঠামো mosquitto *context, কাঠামো mosquitto_subscription *sub)
{
	পূর্ণসংখ্যা rc = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	rc = plugin__handle_subscribe_single(&db.config->security_options, context, sub);
	যদি(rc){
		ফেরত rc;
	}

	যদি(context->listener){
		rc = plugin__handle_subscribe_single(context->listener->security_options, context, sub);
	}

	ফেরত rc;
}
