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

#شامل "config.h"

#شامل <string.h>

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/broker.h"
#شامل "utlist.h"


ساکن صحیح plugin__handle_reload_single(ساخت mosquitto__security_options *opts)
{
	ساخت mosquitto_evt_reload event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;

	memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));

	// Using DL_FOREACH_SAFE here, as reload callbacks might unregister themself
	DL_FOREACH_محفوظ(opts->plugin_callbacks.reload, cb_بنیاد, cb_اگلا){
		صحیح rc = cb_بنیاد->cb(MOSQ_EVT_RELOAD, &event_ڈیٹا, cb_بنیاد->userdata);
		اگر(rc){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Plugin %s produced error on reload: %s",
					cb_بنیاد->identifier->plugin_name?cb_بنیاد->identifier->plugin_name:"",
					mosquitto_strerror(rc));

			واپس rc;
		}
	}
	واپس MOSQ_ERR_SUCCESS;
}


صحیح plugin__handle_reload(خالی)
{
	ساخت mosquitto__security_options *opts;
	صحیح rc;

	/* Global plugins */
	rc = plugin__handle_reload_single(&db.config->security_options);
	اگر(rc){
		واپس rc;
	}

	برائے(صحیح i=0; i<db.config->listener_گنتی; i++){
		opts = db.config->listeners[i].security_options;
		اگر(opts && opts->plugin_callbacks.reload){
			rc = plugin__handle_reload_single(opts);
			اگر(rc){
				واپس rc;
			}
		}
	}
	واپس MOSQ_ERR_SUCCESS;
}
