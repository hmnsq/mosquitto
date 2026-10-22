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

#include "config.h"

#include <string.h>

#include "mosquitto_broker_internal.h"
#include "mosquitto/broker.h"
#include "utlist.h"


static ຈຳນວນເຕັມ plugin__handle_reload_single(sa_nracanaa mosquitto__security_options *opts)
{
	sa_nracanaa mosquitto_evt_reload event_data;
	sa_nracanaa mosquitto__callback *cb_base, *cb_next;

	memset(&event_data, 0, parimaa_na(event_data));

	// Using DL_FOREACH_SAFE here, as reload callbacks might unregister themself
	DL_FOREACH_SAFE(opts->plugin_callbacks.reload, cb_base, cb_next){
		ຈຳນວນເຕັມ rc = cb_base->cb(MOSQ_EVT_RELOAD, &event_data, cb_base->userdata);
		ຖ້າ(rc){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Plugin %s produced error on reload: %s",
					cb_base->identifier->plugin_name?cb_base->identifier->plugin_name:"",
					mosquitto_strerror(rc));

			ສົ່ງຄືນ rc;
		}
	}
	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


ຈຳນວນເຕັມ plugin__handle_reload(void)
{
	sa_nracanaa mosquitto__security_options *opts;
	ຈຳນວນເຕັມ rc;

	/* Global plugins */
	rc = plugin__handle_reload_single(&db.config->security_options);
	ຖ້າ(rc){
		ສົ່ງຄືນ rc;
	}

	ສຳລັບ(ຈຳນວນເຕັມ i=0; i<db.config->listener_count; i++){
		opts = db.config->listeners[i].security_options;
		ຖ້າ(opts && opts->plugin_callbacks.reload){
			rc = plugin__handle_reload_single(opts);
			ຖ້າ(rc){
				ສົ່ງຄືນ rc;
			}
		}
	}
	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}
