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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <string.h>

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/broker.h"
#অন্তর্ভাব "utlist.h"


স্থির পূর্ণাঙ্ক plugin__handle_reload_single(সংরচনা mosquitto__security_options *opts)
{
	সংরচনা mosquitto_evt_reload event_data;
	সংরচনা mosquitto__callback *cb_base, *cb_next;

	memset(&event_data, 0, পরিমাণ(event_data));

	// Using DL_FOREACH_SAFE here, as reload callbacks might unregister themself
	DL_FOREACH_SAFE(opts->plugin_callbacks.reload, cb_base, cb_next){
		পূর্ণাঙ্ক rc = cb_base->cb(MOSQ_EVT_RELOAD, &event_data, cb_base->userdata);
		যদি(rc){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Plugin %s produced error on reload: %s",
					cb_base->identifier->plugin_name?cb_base->identifier->plugin_name:"",
					mosquitto_strerror(rc));

			প্রত্যাগম rc;
		}
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক plugin__handle_reload(শূন্য)
{
	সংরচনা mosquitto__security_options *opts;
	পূর্ণাঙ্ক rc;

	/* Global plugins */
	rc = plugin__handle_reload_single(&db.config->security_options);
	যদি(rc){
		প্রত্যাগম rc;
	}

	প্রতি(পূর্ণাঙ্ক i=0; i<db.config->listener_গুনতি; i++){
		opts = db.config->listeners[i].security_options;
		যদি(opts && opts->plugin_callbacks.reload){
			rc = plugin__handle_reload_single(opts);
			যদি(rc){
				প্রত্যাগম rc;
			}
		}
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}
