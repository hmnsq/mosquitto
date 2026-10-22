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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utlist.h"


ਸਥਿਰ ਪੂਰਨ plugin__handle_unsubscribe_single(ਬਣਤਰ mosquitto__security_options *opts, ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_subscription *sub)
{
	ਬਣਤਰ mosquitto_evt_unsubscribe event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;
	ਪੂਰਨ rc = MOSQ_ERR_SUCCESS;

	memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));
	event_ਅੰਕੜੇ.client = context;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.topic_filter = sub->topic_filter;
	event_ਅੰਕੜੇ.ਅੰਕੜੇ.properties = sub->properties;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.unsubscribe, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		rc = cb_ਅਧਾਰ->cb(MOSQ_EVT_UNSUBSCRIBE, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
		ਜੇ(rc != MOSQ_ERR_SUCCESS){
			ਰੋਕੋ;
		}

		ਜੇ(sub->topic_filter != event_ਅੰਕੜੇ.ਅੰਕੜੇ.topic_filter){
			mosquitto_free(sub->topic_filter);
			sub->topic_filter = event_ਅੰਕੜੇ.ਅੰਕੜੇ.topic_filter;
		}
	}

	ਵਾਪਸ rc;
}


ਪੂਰਨ plugin__handle_unsubscribe(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_subscription *sub)
{
	ਪੂਰਨ rc = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	rc = plugin__handle_unsubscribe_single(&db.config->security_options,
			context, sub);
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	ਜੇ(context->listener){
		rc = plugin__handle_unsubscribe_single(context->listener->security_options,
				context, sub);
	}

	ਵਾਪਸ rc;
}
