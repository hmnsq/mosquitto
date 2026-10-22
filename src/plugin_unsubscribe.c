/*
Copyright (c) 2016-2022 Roger Light <roger@atchoo.org>
Copyright (c) 2022 Cedalo GmbH

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


స్థిర పూర్ణాంకం plugin__handle_unsubscribe_single(నిర్మాణం mosquitto__security_options *opts, నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_subscription *sub)
{
	నిర్మాణం mosquitto_evt_unsubscribe event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;
	పూర్ణాంకం rc = MOSQ_ERR_SUCCESS;

	memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));
	event_దత్తాంశం.client = context;
	event_దత్తాంశం.దత్తాంశం.topic_filter = sub->topic_filter;
	event_దత్తాంశం.దత్తాంశం.properties = sub->properties;

	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.unsubscribe, cb_పునాది, cb_తదుపరి){
		rc = cb_పునాది->cb(MOSQ_EVT_UNSUBSCRIBE, &event_దత్తాంశం, cb_పునాది->userdata);
		ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
			ఆపు;
		}

		ఒకవేళ(sub->topic_filter != event_దత్తాంశం.దత్తాంశం.topic_filter){
			mosquitto_free(sub->topic_filter);
			sub->topic_filter = event_దత్తాంశం.దత్తాంశం.topic_filter;
		}
	}

	తిరిగి rc;
}


పూర్ణాంకం plugin__handle_unsubscribe(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_subscription *sub)
{
	పూర్ణాంకం rc = MOSQ_ERR_SUCCESS;

	/* Global plugins */
	rc = plugin__handle_unsubscribe_single(&db.config->security_options,
			context, sub);
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	ఒకవేళ(context->listener){
		rc = plugin__handle_unsubscribe_single(context->listener->security_options,
				context, sub);
	}

	తిరిగి rc;
}
