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

#समावेश "config.h"

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto_internal.h"
#समावेश "utlist.h"


अटल व्योम plugin__handle_connect_single(काष्ठा mosquitto__security_options *opts, काष्ठा mosquitto *context)
{
	काष्ठा mosquitto_evt_connect event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;

	स_रखो(&event_तथ्य, 0, माप(event_तथ्य));
	event_तथ्य.client = context;
	DL_FOREACH_SAFE(opts->plugin_callbacks.connect, cb_base, cb_अग्र){
		cb_base->cb(MOSQ_EVT_CONNECT, &event_तथ्य, cb_base->userdata);
	}
}


व्योम plugin__handle_connect(काष्ठा mosquitto *context)
{
	/* Global plugins */
	plugin__handle_connect_single(&db.config->security_options, context);

	/* Per listener plugins */
	अगर(context->listener){
		plugin__handle_connect_single(context->listener->security_options, context);
	}
}
