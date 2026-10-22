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

#अन्तर्भाव "config.h"

#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/broker.h"
#अन्तर्भाव "utlist.h"


स्थिर पूर्णाङ्क plugin__handle_reload_single(संरचना mosquitto__security_options *opts)
{
	संरचना mosquitto_evt_reload event_data;
	संरचना mosquitto__callback *cb_base, *cb_next;

	स_रखो(&event_data, 0, परिमाण(event_data));

	// Using DL_FOREACH_SAFE here, as reload callbacks might unregister themself
	DL_FOREACH_SAFE(opts->plugin_callbacks.reload, cb_base, cb_next){
		पूर्णाङ्क rc = cb_base->cb(MOSQ_EVT_RELOAD, &event_data, cb_base->userdata);
		यदि(rc){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Plugin %s produced error on reload: %s",
					cb_base->identifier->plugin_name?cb_base->identifier->plugin_name:"",
					mosquitto_strerror(rc));

			प्रत्यागम rc;
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क plugin__handle_reload(शून्य)
{
	संरचना mosquitto__security_options *opts;
	पूर्णाङ्क rc;

	/* Global plugins */
	rc = plugin__handle_reload_single(&db.config->security_options);
	यदि(rc){
		प्रत्यागम rc;
	}

	प्रति(पूर्णाङ्क i=0; i<db.config->listener_गिनती; i++){
		opts = db.config->listeners[i].security_options;
		यदि(opts && opts->plugin_callbacks.reload){
			rc = plugin__handle_reload_single(opts);
			यदि(rc){
				प्रत्यागम rc;
			}
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}
