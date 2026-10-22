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

#अन्तर्भाव "config.h"

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "utlist.h"


स्थिर पूर्णाङ्क plugin__handle_subscribe_single(संरचना mosquitto__security_options *opts, संरचना mosquitto *context, संरचना mosquitto_subscription *sub)
{
	संरचना mosquitto_evt_subscribe event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;
	पूर्णाङ्क rc = MOSQ_ERR_SUCCESS;

	स_रखो(&event_data, 0, परिमाण(event_data));
	event_data.client = context;
	event_data.data.topic_filter = sub->topic_filter;
	event_data.data.options = sub->options;
	event_data.data.identifier = sub->identifier;
	event_data.data.properties = sub->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.subscribe, cb_base, cb_next){
		rc = cb_base->cb(MOSQ_EVT_SUBSCRIBE, &event_data, cb_base->userdata);
		यदि(rc != MOSQ_ERR_SUCCESS){
			भङ्ग;
		}

		यदि(sub->topic_filter != event_data.data.topic_filter){
			mosquitto_free(sub->topic_filter);
			sub->topic_filter = event_data.data.topic_filter;
		}
	}
	sub->options = event_data.data.options;

	प्रत्यागम rc;
}


पूर्णाङ्क plugin__handle_subscribe(संरचना mosquitto *context, संरचना mosquitto_subscription *sub)
{
	पूर्णाङ्क rc = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	rc = plugin__handle_subscribe_single(&db.config->security_options, context, sub);
	यदि(rc){
		प्रत्यागम rc;
	}

	यदि(context->listener){
		rc = plugin__handle_subscribe_single(context->listener->security_options, context, sub);
	}

	प्रत्यागम rc;
}
