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
#ಅನ್ತರ್ಭಾವ "mosquitto_internal.h"
#ಅನ್ತರ್ಭಾವ "utlist.h"


ಸ್ಥಿರ ಶೂನ್ಯ plugin__handle_connect_single(ರಚನೆ mosquitto__security_options *opts, ರಚನೆ mosquitto *context)
{
	ರಚನೆ mosquitto_evt_connect event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;

	memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));
	event_ದತ್ತಾಂಶ.client = context;
	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.connect, cb_ತಳ, cb_ಮುಂದಿನ){
		cb_ತಳ->cb(MOSQ_EVT_CONNECT, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
	}
}


ಶೂನ್ಯ plugin__handle_connect(ರಚನೆ mosquitto *context)
{
	/* Global plugins */
	plugin__handle_connect_single(&db.config->security_options, context);

	/* Per listener plugins */
	ಆದರೆ(context->listener){
		plugin__handle_connect_single(context->listener->security_options, context);
	}
}
