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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <string.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/broker.h"
#அந்தர்பாவ "utlist.h"


நிலையான முழு plugin__handle_reload_single(கட்டமைப்பு mosquitto__security_options *opts)
{
	கட்டமைப்பு mosquitto_evt_reload event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;

	memset(&event_data, 0, அளவு(event_data));

	// Using DL_FOREACH_SAFE here, as reload callbacks might unregister themself
	DL_FOREACH_SAFE(opts->plugin_callbacks.reload, cb_base, cb_next){
		முழு rc = cb_base->cb(MOSQ_EVT_RELOAD, &event_data, cb_base->userdata);
		எனில்(rc){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Plugin %s produced error on reload: %s",
					cb_base->identifier->plugin_name?cb_base->identifier->plugin_name:"",
					mosquitto_strerror(rc));

			திரும்பு rc;
		}
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு plugin__handle_reload(வெறுமை)
{
	கட்டமைப்பு mosquitto__security_options *opts;
	முழு rc;

	/* Global plugins */
	rc = plugin__handle_reload_single(&db.config->security_options);
	எனில்(rc){
		திரும்பு rc;
	}

	க்கு(முழு i=0; i<db.config->listener_மொத்தம்; i++){
		opts = db.config->listeners[i].security_options;
		எனில்(opts && opts->plugin_callbacks.reload){
			rc = plugin__handle_reload_single(opts);
			எனில்(rc){
				திரும்பு rc;
			}
		}
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}
