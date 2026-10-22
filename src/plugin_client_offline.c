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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "utlist.h"


ಸ್ಥಿರ ಶೂನ್ಯ plugin__handle_client_offline_single(ರಚನೆ mosquitto__security_options *opts, ರಚನೆ mosquitto *context, ಪೂರ್ಣಾಂಕ reason)
{
	ರಚನೆ mosquitto_evt_client_offline event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;

	ಆದರೆ(context->id == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ;
	}

	memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));
	event_ದತ್ತಾಂಶ.client = context;
	event_ದತ್ತಾಂಶ.reason = reason;
	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.client_offline, cb_ತಳ, cb_ಮುಂದಿನ){
		cb_ತಳ->cb(MOSQ_EVT_CLIENT_OFFLINE, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
	}
}


ಶೂನ್ಯ plugin__handle_client_offline(ರಚನೆ mosquitto *context, ಪೂರ್ಣಾಂಕ reason)
{
	/* Global plugins */
	plugin__handle_client_offline_single(&db.config->security_options, context, reason);

	/* Per listener plugins */
	ಆದರೆ(context->listener){
		plugin__handle_client_offline_single(context->listener->security_options, context, reason);
	}
}
