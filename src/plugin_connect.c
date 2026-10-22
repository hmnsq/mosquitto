/*
Copyright (c) 2016-2021 Roger Light <roger@atchoo.org>

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
#ضمن "mosquitto_internal.h"
#ضمن "utlist.h"


ساكن فراغ plugin__handle_connect_single(بنية mosquitto__security_options *opts, بنية mosquitto *context)
{
	بنية mosquitto_evt_connect event_معلومات;
	بنية mosquitto__callback *cb_بنياد, *cb_ايندڙ;

	memset(&event_معلومات, 0, حجم(event_معلومات));
	event_معلومات.client = context;
	DL_FOREACH_محفوظ(opts->plugin_callbacks.connect, cb_بنياد, cb_ايندڙ){
		cb_بنياد->cb(MOSQ_EVT_CONNECT, &event_معلومات, cb_بنياد->userdata);
	}
}


فراغ plugin__handle_connect(بنية mosquitto *context)
{
	/* Global plugins */
	plugin__handle_connect_single(&db.config->security_options, context);

	/* Per listener plugins */
	إذا(context->listener){
		plugin__handle_connect_single(context->listener->security_options, context);
	}
}
