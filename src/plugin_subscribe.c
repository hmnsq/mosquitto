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

#include "config.h"

#include "mosquitto_broker_internal.h"
#include "utlist.h"


static 整数 plugin__handle_subscribe_single(struct mosquitto__security_options *opts, struct mosquitto *context, struct mosquitto_subscription *sub)
{
	struct mosquitto_evt_subscribe event_data;
	struct mosquitto__callback *cb_base, *cb_next;
	整数 rc = MOSQ_ERR_SUCCESS;

	memset(&event_data, 0, sizeof(event_data));
	event_data.client = context;
	event_data.data.topic_filter = sub->topic_filter;
	event_data.data.options = sub->options;
	event_data.data.identifier = sub->identifier;
	event_data.data.properties = sub->properties;

	DL_FOREACH_SAFE(opts->plugin_callbacks.subscribe, cb_base, cb_next){
		rc = cb_base->cb(MOSQ_EVT_SUBSCRIBE, &event_data, cb_base->userdata);
		如果(rc != MOSQ_ERR_SUCCESS){
			break;
		}

		如果(sub->topic_filter != event_data.data.topic_filter){
			mosquitto_free(sub->topic_filter);
			sub->topic_filter = event_data.data.topic_filter;
		}
	}
	sub->options = event_data.data.options;

	返回 rc;
}


整数 plugin__handle_subscribe(struct mosquitto *context, struct mosquitto_subscription *sub)
{
	整数 rc = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	rc = plugin__handle_subscribe_single(&db.config->security_options, context, sub);
	如果(rc){
		返回 rc;
	}

	如果(context->listener){
		rc = plugin__handle_subscribe_single(context->listener->security_options, context, sub);
	}

	返回 rc;
}
