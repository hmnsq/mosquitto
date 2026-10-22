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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "utlist.h"


ಸ್ಥಿರ ಶೂನ್ಯ plugin__handle_disconnect_single(ಸಂರಚನಾ mosquitto__security_options *opts, ಸಂರಚನಾ mosquitto *context, ಪೂರ್ಣಾಙ್ಕ reason)
{
	ಸಂರಚನಾ mosquitto_evt_disconnect event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;

	ಯದಿ(context->id == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ;
	}

	memset(&event_data, 0, ಪರಿಮಾಣ(event_data));
	event_data.client = context;
	event_data.reason = reason;
	DL_FOREACH_SAFE(opts->plugin_callbacks.disconnect, cb_base, cb_next){
		cb_base->cb(MOSQ_EVT_DISCONNECT, &event_data, cb_base->userdata);
	}
}


ಶೂನ್ಯ plugin__handle_disconnect(ಸಂರಚನಾ mosquitto *context, ಪೂರ್ಣಾಙ್ಕ reason)
{
	/* Global plugins */
	plugin__handle_disconnect_single(&db.config->security_options, context, reason);

	/* Per listener plugins */
	ಯದಿ(context->listener){
		plugin__handle_disconnect_single(context->listener->security_options, context, reason);
	}
}
