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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <string.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/broker.h"
#அந்தர்பாவ "utlist.h"


நிலையான வெறுமை plugin__handle_tick_single(கட்டமைப்பு mosquitto__security_options *opts)
{
	கட்டமைப்பு mosquitto_evt_tick event_data;
	கட்டமைப்பு mosquitto__callback *cb_base, *cb_next;

	memset(&event_data, 0, அளவு(event_data));

	// Using DL_FOREACH_SAFE here, as tick callbacks might unregister themself
	DL_FOREACH_SAFE(opts->plugin_callbacks.tick, cb_base, cb_next){
		mosquitto_time_ns(&event_data.now_s, &event_data.now_ns);

		எனில்(mosquitto_time_cmp(event_data.now_s, event_data.now_ns,
				cb_base->data.next_tick.tv_sec, cb_base->data.next_tick.tv_nsec) > 0){

			event_data.next_s = 0;
			event_data.next_ms = 0;
			cb_base->cb(MOSQ_EVT_TICK, &event_data, cb_base->userdata);
			loop__update_next_event(event_data.next_s * 1000 + event_data.next_ms);

			cb_base->data.next_tick.tv_sec = event_data.now_s + event_data.next_s;
			cb_base->data.next_tick.tv_nsec = event_data.now_ns + 1000000*event_data.next_ms;
			எனில்(cb_base->data.next_tick.tv_nsec > 1000000000){
				cb_base->data.next_tick.tv_nsec -= 1000000000;
				cb_base->data.next_tick.tv_sec += 1;
			}
		}
	}
}


வெறுமை plugin__handle_tick(வெறுமை)
{
	கட்டமைப்பு mosquitto__security_options *opts;

	/* Global plugins */
	plugin__handle_tick_single(&db.config->security_options);

	க்கு(முழு i=0; i<db.config->listener_மொத்தம்; i++){
		opts = db.config->listeners[i].security_options;
		எனில்(opts && opts->plugin_callbacks.tick){
			plugin__handle_tick_single(opts);
		}
	}
}
