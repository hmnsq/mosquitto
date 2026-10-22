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

#ضمن "config.h"

#ضمن "mosquitto_broker_internal.h"
#ضمن "utlist.h"


ساكن صحيح plugin__handle_subscribe_single(بنية mosquitto__security_options *opts, بنية mosquitto *context, بنية mosquitto_subscription *sub)
{
	بنية mosquitto_evt_subscribe event_معلومات;
	بنية mosquitto__callback *cb_بنياد, *cb_ايندڙ;
	صحيح rc = MOSQ_ERR_SUCCESS;

	memset(&event_معلومات, 0, حجم(event_معلومات));
	event_معلومات.client = context;
	event_معلومات.معلومات.topic_filter = sub->topic_filter;
	event_معلومات.معلومات.options = sub->options;
	event_معلومات.معلومات.identifier = sub->identifier;
	event_معلومات.معلومات.properties = sub->properties;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.subscribe, cb_بنياد, cb_ايندڙ){
		rc = cb_بنياد->cb(MOSQ_EVT_SUBSCRIBE, &event_معلومات, cb_بنياد->userdata);
		إذا(rc != MOSQ_ERR_SUCCESS){
			اكسر;
		}

		إذا(sub->topic_filter != event_معلومات.معلومات.topic_filter){
			mosquitto_free(sub->topic_filter);
			sub->topic_filter = event_معلومات.معلومات.topic_filter;
		}
	}
	sub->options = event_معلومات.معلومات.options;

	أرجع rc;
}


صحيح plugin__handle_subscribe(بنية mosquitto *context, بنية mosquitto_subscription *sub)
{
	صحيح rc = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	rc = plugin__handle_subscribe_single(&db.config->security_options, context, sub);
	إذا(rc){
		أرجع rc;
	}

	إذا(context->listener){
		rc = plugin__handle_subscribe_single(context->listener->security_options, context, sub);
	}

	أرجع rc;
}
