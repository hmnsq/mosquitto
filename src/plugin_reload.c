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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/broker.h"
#അന്തര്ഭാവ "utlist.h"


സ്ഥിതം പൂർണ്ണസംഖ്യ plugin__handle_reload_single(ഘടന mosquitto__security_options *opts)
{
	ഘടന mosquitto_evt_reload event_data;
	ഘടന mosquitto__callback *cb_base, *cb_next;

	memset(&event_data, 0, വലുപ്പം(event_data));

	// Using DL_FOREACH_SAFE here, as reload callbacks might unregister themself
	DL_FOREACH_SAFE(opts->plugin_callbacks.reload, cb_base, cb_next){
		പൂർണ്ണസംഖ്യ rc = cb_base->cb(MOSQ_EVT_RELOAD, &event_data, cb_base->userdata);
		എങ്കിൽ(rc){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Plugin %s produced error on reload: %s",
					cb_base->identifier->plugin_name?cb_base->identifier->plugin_name:"",
					mosquitto_strerror(rc));

			തിരികെ rc;
		}
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ plugin__handle_reload(ശൂന്യം)
{
	ഘടന mosquitto__security_options *opts;
	പൂർണ്ണസംഖ്യ rc;

	/* Global plugins */
	rc = plugin__handle_reload_single(&db.config->security_options);
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<db.config->listener_എണ്ണം; i++){
		opts = db.config->listeners[i].security_options;
		എങ്കിൽ(opts && opts->plugin_callbacks.reload){
			rc = plugin__handle_reload_single(opts);
			എങ്കിൽ(rc){
				തിരികെ rc;
			}
		}
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}
