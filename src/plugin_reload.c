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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/broker.h"
#ಅನ್ತರ್ಭಾವ "utlist.h"


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ plugin__handle_reload_single(ಸಂರಚನಾ mosquitto__security_options *opts)
{
	ಸಂರಚನಾ mosquitto_evt_reload event_data;
	ಸಂರಚನಾ mosquitto__callback *cb_base, *cb_next;

	memset(&event_data, 0, ಪರಿಮಾಣ(event_data));

	// Using DL_FOREACH_SAFE here, as reload callbacks might unregister themself
	DL_FOREACH_SAFE(opts->plugin_callbacks.reload, cb_base, cb_next){
		ಪೂರ್ಣಾಙ್ಕ rc = cb_base->cb(MOSQ_EVT_RELOAD, &event_data, cb_base->userdata);
		ಯದಿ(rc){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Plugin %s produced error on reload: %s",
					cb_base->identifier->plugin_name?cb_base->identifier->plugin_name:"",
					mosquitto_strerror(rc));

			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ plugin__handle_reload(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ mosquitto__security_options *opts;
	ಪೂರ್ಣಾಙ್ಕ rc;

	/* Global plugins */
	rc = plugin__handle_reload_single(&db.config->security_options);
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
		opts = db.config->listeners[i].security_options;
		ಯದಿ(opts && opts->plugin_callbacks.reload){
			rc = plugin__handle_reload_single(opts);
			ಯದಿ(rc){
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}
