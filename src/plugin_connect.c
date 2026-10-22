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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto_internal.h"
#അന്തര്ഭാവ "utlist.h"


സ്ഥിതം ശൂന്യം plugin__handle_connect_single(ഘടന mosquitto__security_options *opts, ഘടന mosquitto *context)
{
	ഘടന mosquitto_evt_connect event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;

	memset(&event_data, 0, വലുപ്പം(event_data));
	event_data.client = context;
	DL_FOREACH_SAFE(opts->plugin_callbacks.connect, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_CONNECT, &event_data, cb_base->userdata);
	}
}


ശൂന്യം plugin__handle_connect(ഘടന mosquitto *context)
{
	/* Global plugins */
	plugin__handle_connect_single(&db.config->security_options, context);

	/* Per listener plugins */
	എങ്കിൽ(context->listener){
		plugin__handle_connect_single(context->listener->security_options, context);
	}
}
