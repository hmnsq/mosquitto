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

#అన్తర్భావ "config.h"

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "utlist.h"


స్థిర శూన్య plugin__handle_disconnect_single(నిర్మాణం mosquitto__security_options *opts, నిర్మాణం mosquitto *context, పూర్ణాంకం reason)
{
	నిర్మాణం mosquitto_evt_disconnect event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;

	ఒకవేళ(context->id == ఖాళీ){
		తిరిగి;
	}

	memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));
	event_దత్తాంశం.client = context;
	event_దత్తాంశం.reason = reason;
	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.disconnect, cb_పునాది, cb_తదుపరి){
		cb_పునాది->cb(MOSQ_EVT_DISCONNECT, &event_దత్తాంశం, cb_పునాది->userdata);
	}
}


శూన్య plugin__handle_disconnect(నిర్మాణం mosquitto *context, పూర్ణాంకం reason)
{
	/* Global plugins */
	plugin__handle_disconnect_single(&db.config->security_options, context, reason);

	/* Per listener plugins */
	ఒకవేళ(context->listener){
		plugin__handle_disconnect_single(context->listener->security_options, context, reason);
	}
}
