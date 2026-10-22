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


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ plugin__handle_reload_single(ರಚನೆ mosquitto__security_options *opts)
{
	ರಚನೆ mosquitto_evt_reload event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__callback *cb_ತಳ, *cb_ಮುಂದಿನ;

	memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));

	// Using DL_FOREACH_SAFE here, as reload callbacks might unregister themself
	DL_FOREACH_ಸುರಕ್ಷಿತ(opts->plugin_callbacks.reload, cb_ತಳ, cb_ಮುಂದಿನ){
		ಪೂರ್ಣಾಂಕ rc = cb_ತಳ->cb(MOSQ_EVT_RELOAD, &event_ದತ್ತಾಂಶ, cb_ತಳ->userdata);
		ಆದರೆ(rc){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Plugin %s produced error on reload: %s",
					cb_ತಳ->identifier->plugin_name?cb_ತಳ->identifier->plugin_name:"",
					mosquitto_strerror(rc));

			ಹಿಂತಿರುಗಿ rc;
		}
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ plugin__handle_reload(ಶೂನ್ಯ)
{
	ರಚನೆ mosquitto__security_options *opts;
	ಪೂರ್ಣಾಂಕ rc;

	/* Global plugins */
	rc = plugin__handle_reload_single(&db.config->security_options);
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}

	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
		opts = db.config->listeners[i].security_options;
		ಆದರೆ(opts && opts->plugin_callbacks.reload){
			rc = plugin__handle_reload_single(opts);
			ಆದರೆ(rc){
				ಹಿಂತಿರುಗಿ rc;
			}
		}
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}
