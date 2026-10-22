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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "utlist.h"


স্থির শূন্য plugin__handle_disconnect_single(কাঠামো mosquitto__security_options *opts, কাঠামো mosquitto *context, পূর্ণসংখ্যা reason)
{
	কাঠামো mosquitto_evt_disconnect event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;

	যদি(context->id == ফাঁকা){
		ফেরত;
	}

	memset(&event_তথ্য, 0, আকার(event_তথ্য));
	event_তথ্য.client = context;
	event_তথ্য.reason = reason;
	DL_FOREACH_SAFE(opts->plugin_callbacks.disconnect, cb_base, cb_পরবর্তী){
		cb_base->cb(MOSQ_EVT_DISCONNECT, &event_তথ্য, cb_base->userdata);
	}
}


শূন্য plugin__handle_disconnect(কাঠামো mosquitto *context, পূর্ণসংখ্যা reason)
{
	/* Global plugins */
	plugin__handle_disconnect_single(&db.config->security_options, context, reason);

	/* Per listener plugins */
	যদি(context->listener){
		plugin__handle_disconnect_single(context->listener->security_options, context, reason);
	}
}
