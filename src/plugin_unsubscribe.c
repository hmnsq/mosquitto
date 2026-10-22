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

#شامل "config.h"

#شامل "mosquitto_broker_internal.h"
#شامل "utlist.h"


ساکن صحیح plugin__handle_unsubscribe_single(ساخت mosquitto__security_options *opts, ساخت mosquitto *context, ساخت mosquitto_subscription *sub)
{
	ساخت mosquitto_evt_unsubscribe event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;
	صحیح rc = MOSQ_ERR_SUCCESS;

	memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));
	event_ڈیٹا.client = context;
	event_ڈیٹا.ڈیٹا.topic_filter = sub->topic_filter;
	event_ڈیٹا.ڈیٹا.properties = sub->properties;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.unsubscribe, cb_بنیاد, cb_اگلا){
		rc = cb_بنیاد->cb(MOSQ_EVT_UNSUBSCRIBE, &event_ڈیٹا, cb_بنیاد->userdata);
		اگر(rc != MOSQ_ERR_SUCCESS){
			توڑو;
		}

		اگر(sub->topic_filter != event_ڈیٹا.ڈیٹا.topic_filter){
			mosquitto_free(sub->topic_filter);
			sub->topic_filter = event_ڈیٹا.ڈیٹا.topic_filter;
		}
	}

	واپس rc;
}


صحیح plugin__handle_unsubscribe(ساخت mosquitto *context, ساخت mosquitto_subscription *sub)
{
	صحیح rc = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	rc = plugin__handle_unsubscribe_single(&db.config->security_options,
			context, sub);
	اگر(rc){
		واپس rc;
	}

	اگر(context->listener){
		rc = plugin__handle_unsubscribe_single(context->listener->security_options,
				context, sub);
	}

	واپس rc;
}
