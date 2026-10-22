/*
Copyright (c) 2016-2025 Roger Light <roger@atchoo.org>

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

#అన్తర్భావ <string.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/broker.h"
#అన్తర్భావ "utlist.h"


స్థిర పూర్ణాంకం plugin__handle_reload_single(నిర్మాణం mosquitto__security_options *opts)
{
	నిర్మాణం mosquitto_evt_reload event_దత్తాంశం;
	నిర్మాణం mosquitto__callback *cb_పునాది, *cb_తదుపరి;

	memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));

	// Using DL_FOREACH_SAFE here, as reload callbacks might unregister themself
	DL_FOREACH_సురక్షిత(opts->plugin_callbacks.reload, cb_పునాది, cb_తదుపరి){
		పూర్ణాంకం rc = cb_పునాది->cb(MOSQ_EVT_RELOAD, &event_దత్తాంశం, cb_పునాది->userdata);
		ఒకవేళ(rc){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Plugin %s produced error on reload: %s",
					cb_పునాది->identifier->plugin_name?cb_పునాది->identifier->plugin_name:"",
					mosquitto_strerror(rc));

			తిరిగి rc;
		}
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం plugin__handle_reload(శూన్య)
{
	నిర్మాణం mosquitto__security_options *opts;
	పూర్ణాంకం rc;

	/* Global plugins */
	rc = plugin__handle_reload_single(&db.config->security_options);
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	కొరకు(పూర్ణాంకం i=0; i<db.config->listener_లెక్క; i++){
		opts = db.config->listeners[i].security_options;
		ఒకవేళ(opts && opts->plugin_callbacks.reload){
			rc = plugin__handle_reload_single(opts);
			ఒకవేళ(rc){
				తిరిగి rc;
			}
		}
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}
