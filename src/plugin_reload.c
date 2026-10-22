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

#ضمن "config.h"

#ضمن <string.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/broker.h"
#ضمن "utlist.h"


ساكن صحيح plugin__handle_reload_single(بنية mosquitto__security_options *opts)
{
	بنية mosquitto_evt_reload event_data;
	بنية mosquitto__callback *cb_base, *cb_next;

	memset(&event_data, 0, حجم(event_data));

	// Using DL_FOREACH_SAFE here, as reload callbacks might unregister themself
	DL_FOREACH_SAFE(opts->plugin_callbacks.reload, cb_base, cb_next){
		صحيح rc = cb_base->cb(MOSQ_EVT_RELOAD, &event_data, cb_base->userdata);
		إذا(rc){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Plugin %s produced error on reload: %s",
					cb_base->identifier->plugin_name?cb_base->identifier->plugin_name:"",
					mosquitto_strerror(rc));

			أرجع rc;
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح plugin__handle_reload(فراغ)
{
	بنية mosquitto__security_options *opts;
	صحيح rc;

	/* Global plugins */
	rc = plugin__handle_reload_single(&db.config->security_options);
	إذا(rc){
		أرجع rc;
	}

	لكل(صحيح i=0; i<db.config->listener_كدي; i++){
		opts = db.config->listeners[i].security_options;
		إذا(opts && opts->plugin_callbacks.reload){
			rc = plugin__handle_reload_single(opts);
			إذا(rc){
				أرجع rc;
			}
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}
