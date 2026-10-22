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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utlist.h"


ਸਥਿਰ ਪੂਰਨ plugin__handle_reload_single(ਬਣਤਰ mosquitto__security_options *opts)
{
	ਬਣਤਰ mosquitto_evt_reload event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;

	memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));

	// Using DL_FOREACH_SAFE here, as reload callbacks might unregister themself
	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.reload, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		ਪੂਰਨ rc = cb_ਅਧਾਰ->cb(MOSQ_EVT_RELOAD, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
		ਜੇ(rc){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Plugin %s produced error on reload: %s",
					cb_ਅਧਾਰ->identifier->plugin_name?cb_ਅਧਾਰ->identifier->plugin_name:"",
					mosquitto_strerror(rc));

			ਵਾਪਸ rc;
		}
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ plugin__handle_reload(ਖਾਲੀ)
{
	ਬਣਤਰ mosquitto__security_options *opts;
	ਪੂਰਨ rc;

	/* Global plugins */
	rc = plugin__handle_reload_single(&db.config->security_options);
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	ਲਈ(ਪੂਰਨ i=0; i<db.config->listener_ਗਿਣਤੀ; i++){
		opts = db.config->listeners[i].security_options;
		ਜੇ(opts && opts->plugin_callbacks.reload){
			rc = plugin__handle_reload_single(opts);
			ਜੇ(rc){
				ਵਾਪਸ rc;
			}
		}
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}
