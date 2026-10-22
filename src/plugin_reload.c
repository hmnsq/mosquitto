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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/broker.h"
#અન્તર્ભાવ "utlist.h"


સ્થિર પૂર્ણાંક plugin__handle_reload_single(સંરચના mosquitto__security_options *opts)
{
	સંરચના mosquitto_evt_reload event_data;
	સંરચના mosquitto__callback *cb_base, *cb_next;

	memset(&event_data, 0, કદ(event_data));

	// Using DL_FOREACH_SAFE here, as reload callbacks might unregister themself
	DL_FOREACH_SAFE(opts->plugin_callbacks.reload, cb_base, cb_next){
		પૂર્ણાંક rc = cb_base->cb(MOSQ_EVT_RELOAD, &event_data, cb_base->userdata);
		જો(rc){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Plugin %s produced error on reload: %s",
					cb_base->identifier->plugin_name?cb_base->identifier->plugin_name:"",
					mosquitto_strerror(rc));

			પાછા rc;
		}
	}
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક plugin__handle_reload(શૂન્ય)
{
	સંરચના mosquitto__security_options *opts;
	પૂર્ણાંક rc;

	/* Global plugins */
	rc = plugin__handle_reload_single(&db.config->security_options);
	જો(rc){
		પાછા rc;
	}

	માટે(પૂર્ણાંક i=0; i<db.config->listener_ગણતરી; i++){
		opts = db.config->listeners[i].security_options;
		જો(opts && opts->plugin_callbacks.reload){
			rc = plugin__handle_reload_single(opts);
			જો(rc){
				પાછા rc;
			}
		}
	}
	પાછા MOSQ_ERR_SUCCESS;
}
