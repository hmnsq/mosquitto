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


ساكن صحيح plugin__handle_unsubscribe_single(بنية mosquitto__security_options *opts, بنية mosquitto *context, بنية mosquitto_subscription *sub)
{
	بنية mosquitto_evt_unsubscribe event_انکڑے;
	بنية mosquitto__callback *cb_ادھار, *cb_اگلا;
	صحيح rc = MOSQ_ERR_SUCCESS;

	memset(&event_انکڑے, 0, حجم(event_انکڑے));
	event_انکڑے.client = context;
	event_انکڑے.انکڑے.topic_filter = sub->topic_filter;
	event_انکڑے.انکڑے.properties = sub->properties;

	DL_FOREACH_محفوظ(opts->plugin_callbacks.unsubscribe, cb_ادھار, cb_اگلا){
		rc = cb_ادھار->cb(MOSQ_EVT_UNSUBSCRIBE, &event_انکڑے, cb_ادھار->userdata);
		إذا(rc != MOSQ_ERR_SUCCESS){
			اكسر;
		}

		إذا(sub->topic_filter != event_انکڑے.انکڑے.topic_filter){
			mosquitto_free(sub->topic_filter);
			sub->topic_filter = event_انکڑے.انکڑے.topic_filter;
		}
	}

	أرجع rc;
}


صحيح plugin__handle_unsubscribe(بنية mosquitto *context, بنية mosquitto_subscription *sub)
{
	صحيح rc = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	rc = plugin__handle_unsubscribe_single(&db.config->security_options,
			context, sub);
	إذا(rc){
		أرجع rc;
	}

	إذا(context->listener){
		rc = plugin__handle_unsubscribe_single(context->listener->security_options,
				context, sub);
	}

	أرجع rc;
}
