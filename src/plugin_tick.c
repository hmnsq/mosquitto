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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utlist.h"


ਸਥਿਰ ਖਾਲੀ plugin__handle_tick_single(ਬਣਤਰ mosquitto__security_options *opts)
{
	ਬਣਤਰ mosquitto_evt_tick event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, *cb_ਅਗਲਾ;

	memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));

	// Using DL_FOREACH_SAFE here, as tick callbacks might unregister themself
	DL_FOREACH_ਸੁਰੱਖਿਅਤ(opts->plugin_callbacks.tick, cb_ਅਧਾਰ, cb_ਅਗਲਾ){
		mosquitto_ਸਮਾਂ_ns(&event_ਅੰਕੜੇ.now_s, &event_ਅੰਕੜੇ.now_ns);

		ਜੇ(mosquitto_ਸਮਾਂ_cmp(event_ਅੰਕੜੇ.now_s, event_ਅੰਕੜੇ.now_ns,
				cb_ਅਧਾਰ->ਅੰਕੜੇ.ਅਗਲਾ_tick.tv_sec, cb_ਅਧਾਰ->ਅੰਕੜੇ.ਅਗਲਾ_tick.tv_nsec) > 0){

			event_ਅੰਕੜੇ.ਅਗਲਾ_s = 0;
			event_ਅੰਕੜੇ.ਅਗਲਾ_ms = 0;
			cb_ਅਧਾਰ->cb(MOSQ_EVT_TICK, &event_ਅੰਕੜੇ, cb_ਅਧਾਰ->userdata);
			loop_update_ਅਗਲਾ_event(event_ਅੰਕੜੇ.ਅਗਲਾ_s * 1000 + event_ਅੰਕੜੇ.ਅਗਲਾ_ms);

			cb_ਅਧਾਰ->ਅੰਕੜੇ.ਅਗਲਾ_tick.tv_sec = event_ਅੰਕੜੇ.now_s + event_ਅੰਕੜੇ.ਅਗਲਾ_s;
			cb_ਅਧਾਰ->ਅੰਕੜੇ.ਅਗਲਾ_tick.tv_nsec = event_ਅੰਕੜੇ.now_ns + 1000000*event_ਅੰਕੜੇ.ਅਗਲਾ_ms;
			ਜੇ(cb_ਅਧਾਰ->ਅੰਕੜੇ.ਅਗਲਾ_tick.tv_nsec > 1000000000){
				cb_ਅਧਾਰ->ਅੰਕੜੇ.ਅਗਲਾ_tick.tv_nsec -= 1000000000;
				cb_ਅਧਾਰ->ਅੰਕੜੇ.ਅਗਲਾ_tick.tv_sec += 1;
			}
		}
	}
}


ਖਾਲੀ plugin__handle_tick(ਖਾਲੀ)
{
	ਬਣਤਰ mosquitto__security_options *opts;

	/* Global plugins */
	plugin__handle_tick_single(&db.config->security_options);

	ਲਈ(ਪੂਰਨ i=0; i<db.config->listener_ਗਿਣਤੀ; i++){
		opts = db.config->listeners[i].security_options;
		ਜੇ(opts && opts->plugin_callbacks.tick){
			plugin__handle_tick_single(opts);
		}
	}
}
