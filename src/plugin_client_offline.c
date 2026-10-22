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

#ضمن "config.h"

#ضمن "mosquitto_broker_internal.h"
#ضمن "utlist.h"


ساكن فراغ plugin__handle_client_offline_single(بنية mosquitto__security_options *opts, بنية mosquitto *context, صحيح reason)
{
	بنية mosquitto_evt_client_offline event_داده;
	بنية mosquitto__callback *cb_پایه, *cb_بعدی;

	إذا(context->id == عدم){
		أرجع;
	}

	memset(&event_داده, 0, حجم(event_داده));
	event_داده.client = context;
	event_داده.reason = reason;
	DL_FOREACH_امن(opts->plugin_callbacks.client_offline, cb_پایه, cb_بعدی){
		cb_پایه->cb(MOSQ_EVT_CLIENT_OFFLINE, &event_داده, cb_پایه->userdata);
	}
}


فراغ plugin__handle_client_offline(بنية mosquitto *context, صحيح reason)
{
	/* Global plugins */
	plugin__handle_client_offline_single(&db.config->security_options, context, reason);

	/* Per listener plugins */
	إذا(context->listener){
		plugin__handle_client_offline_single(context->listener->security_options, context, reason);
	}
}
