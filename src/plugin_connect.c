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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utlist.h"


ਸਥਿਰ ਖਾਲੀ plugin__handle_connect_single(ਬਣਤਰ mosquitto__security_options *opts, ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ mosquitto_evt_connect event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;

	memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));
	event_ਅੰਕੜੇ.client = context;
	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.connect, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		cb_ਅਧਾਰ->cb(MOSQ_EVT_CONNECT, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
	}
}


ਖਾਲੀ plugin__handle_connect(ਬਣਤਰ mosquitto *context)
{
	/* Global plugins */
	plugin__handle_connect_single(&db.config->security_options, context);

	/* Per listener plugins */
	ਜੇ(context->listener){
		plugin__handle_connect_single(context->listener->security_options, context);
	}
}
