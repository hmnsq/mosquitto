/*
Copyright (c) 2016-2021 Roger Light <roger@atchoo.org>

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

#ضمن "config.h"

#ضمن <string.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/broker.h"
#ضمن "utlist.h"


ساكن فراغ plugin__handle_tick_single(بنية mosquitto__security_options *opts)
{
	بنية mosquitto_evt_tick event_data;
	بنية mosquitto__callback *cb_base, *cb_next;

	memset(&event_data, 0, حجم(event_data));

	// Using DL_FOREACH_SAFE here, as tick callbacks might unregister themself
	DL_FOREACH_SAFE(opts->plugin_callbacks.tick, cb_base, cb_next){
		mosquitto_time_ns(&event_data.now_s, &event_data.now_ns);

		إذا(mosquitto_time_cmp(event_data.now_s, event_data.now_ns,
				cb_base->data.next_tick.tv_sec, cb_base->data.next_tick.tv_nsec) > 0){

			event_data.next_s = 0;
			event_data.next_ms = 0;
			cb_base->cb(MOSQ_EVT_TICK, &event_data, cb_base->userdata);
			loop__update_next_event(event_data.next_s * 1000 + event_data.next_ms);

			cb_base->data.next_tick.tv_sec = event_data.now_s + event_data.next_s;
			cb_base->data.next_tick.tv_nsec = event_data.now_ns + 1000000*event_data.next_ms;
			إذا(cb_base->data.next_tick.tv_nsec > 1000000000){
				cb_base->data.next_tick.tv_nsec -= 1000000000;
				cb_base->data.next_tick.tv_sec += 1;
			}
		}
	}
}


فراغ plugin__handle_tick(فراغ)
{
	بنية mosquitto__security_options *opts;

	/* Global plugins */
	plugin__handle_tick_single(&db.config->security_options);

	لكل(صحيح i=0; i<db.config->listener_كدي; i++){
		opts = db.config->listeners[i].security_options;
		إذا(opts && opts->plugin_callbacks.tick){
			plugin__handle_tick_single(opts);
		}
	}
}
