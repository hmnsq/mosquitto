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
#ضمن "utlist.h"


ساكن فراغ plugin__handle_disconnect_single(بنية mosquitto__security_options *opts, بنية mosquitto *context, صحيح reason)
{
	بنية mosquitto_evt_disconnect event_data;
	بنية mosquitto__callback *cb_base, *cb_next;

	إذا(context->id == عدم){
		أرجع;
	}

	memset(&event_data, 0, حجم(event_data));
	event_data.client = context;
	event_data.reason = reason;
	DL_FOREACH_SAFE(opts->plugin_callbacks.disconnect, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_DISCONNECT, &event_data, cb_base->userdata);
	}
}


فراغ plugin__handle_disconnect(بنية mosquitto *context, صحيح reason)
{
	/* Global plugins */
	plugin__handle_disconnect_single(&db.config->security_options, context, reason);

	/* Per listener plugins */
	إذا(context->listener){
		plugin__handle_disconnect_single(context->listener->security_options, context, reason);
	}
}
