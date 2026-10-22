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

#समावेश "config.h"

#समावेश "mosquitto_broker_internal.h"
#समावेश "utlist.h"


अटल पूर्णांक plugin__handle_unsubscribe_single(काष्ठा mosquitto__security_options *opts, काष्ठा mosquitto *context, काष्ठा mosquitto_subscription *sub)
{
	काष्ठा mosquitto_evt_unsubscribe event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;
	पूर्णांक rc = MOSQ_ERR_SUCCESS;

	स_रखो(&event_तथ्य, 0, माप(event_तथ्य));
	event_तथ्य.client = context;
	event_तथ्य.तथ्य.topic_filter = sub->topic_filter;
	event_तथ्य.तथ्य.properties = sub->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.unsubscribe, cb_base, cb_अग्र){
		rc = cb_base->cb(MOSQ_EVT_UNSUBSCRIBE, &event_तथ्य, cb_base->userdata);
		अगर(rc != MOSQ_ERR_SUCCESS){
			अवरोध;
		}

		अगर(sub->topic_filter != event_तथ्य.तथ्य.topic_filter){
			mosquitto_free(sub->topic_filter);
			sub->topic_filter = event_तथ्य.तथ्य.topic_filter;
		}
	}

	वापस rc;
}


पूर्णांक plugin__handle_unsubscribe(काष्ठा mosquitto *context, काष्ठा mosquitto_subscription *sub)
{
	पूर्णांक rc = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	rc = plugin__handle_unsubscribe_single(&db.config->security_options,
			context, sub);
	अगर(rc){
		वापस rc;
	}

	अगर(context->listener){
		rc = plugin__handle_unsubscribe_single(context->listener->security_options,
				context, sub);
	}

	वापस rc;
}
