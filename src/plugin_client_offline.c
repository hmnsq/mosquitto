/*
Copyright (c) 2023 Roger Light <roger@atchoo.org>
Copyright (c) 2023 Cedalo Gmbh

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


ساکن خالی plugin__handle_client_offline_single(ساخت mosquitto__security_options *opts, ساخت mosquitto *context, صحیح reason)
{
	ساخت mosquitto_evt_client_offline event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;

	اگر(context->id == تہی){
		واپس;
	}

	memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));
	event_ڈیٹا.client = context;
	event_ڈیٹا.reason = reason;
	DL_FOREACH_محفوظ(opts->plugin_callbacks.client_offline, cb_بنیاد, cb_اگلا){
		cb_بنیاد->cb(MOSQ_EVT_CLIENT_OFFLINE, &event_ڈیٹا, cb_بنیاد->userdata);
	}
}


خالی plugin__handle_client_offline(ساخت mosquitto *context, صحیح reason)
{
	/* Global plugins */
	plugin__handle_client_offline_single(&db.config->security_options, context, reason);

	/* Per listener plugins */
	اگر(context->listener){
		plugin__handle_client_offline_single(context->listener->security_options, context, reason);
	}
}
