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


স্থির পূর্ণসংখ্যা plugin__handle_reload_single(কাঠামো mosquitto__security_options *opts)
{
	কাঠামো mosquitto_evt_reload event_তথ্য;
	কাঠামো mosquitto__callback *cb_base, *cb_পরবর্তী;

	memset(&event_তথ্য, 0, আকার(event_তথ্য));

	// Using DL_FOREACH_SAFE here, as reload callbacks might unregister themself
	DL_FOREACH_SAFE(opts->plugin_callbacks.reload, cb_base, cb_পরবর্তী){
		পূর্ণসংখ্যা rc = cb_base->cb(MOSQ_EVT_RELOAD, &event_তথ্য, cb_base->userdata);
		যদি(rc){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Plugin %s produced error on reload: %s",
					cb_base->identifier->plugin_name?cb_base->identifier->plugin_name:"",
					mosquitto_strerror(rc));

			ফেরত rc;
		}
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা plugin__handle_reload(শূন্য)
{
	কাঠামো mosquitto__security_options *opts;
	পূর্ণসংখ্যা rc;

	/* Global plugins */
	rc = plugin__handle_reload_single(&db.config->security_options);
	যদি(rc){
		ফেরত rc;
	}

	জন্য(পূর্ণসংখ্যা i=0; i<db.config->listener_গুনতি; i++){
		opts = db.config->listeners[i].security_options;
		যদি(opts && opts->plugin_callbacks.reload){
			rc = plugin__handle_reload_single(opts);
			যদি(rc){
				ফেরত rc;
			}
		}
	}
	ফেরত MOSQ_ERR_SUCCESS;
}
