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

#अन्तर्भाव "config.h"

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "utlist.h"


स्थिर शून्य plugin__handle_disconnect_single(संरचना mosquitto__security_options *opts, संरचना mosquitto *context, पूर्णाङ्क reason)
{
	संरचना mosquitto_evt_disconnect event_दत्त;
	संरचना mosquitto__callback *cb_आधारः, *cb_अग्र;

	यदि(context->id == रिक्त){
		प्रत्यागम;
	}

	स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));
	event_दत्त.client = context;
	event_दत्त.reason = reason;
	DL_FOREACH_SAFE(opts->plugin_callbacks.disconnect, cb_आधारः, cb_अग्र){
		cb_आधारः->cb(MOSQ_EVT_DISCONNECT, &event_दत्त, cb_आधारः->userdata);
	}
}


शून्य plugin__handle_disconnect(संरचना mosquitto *context, पूर्णाङ्क reason)
{
	/* Global plugins */
	plugin__handle_disconnect_single(&db.config->security_options, context, reason);

	/* Per listener plugins */
	यदि(context->listener){
		plugin__handle_disconnect_single(context->listener->security_options, context, reason);
	}
}
