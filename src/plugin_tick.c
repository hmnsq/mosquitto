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
	بنية mosquitto_evt_tick event_معلومات;
	بنية mosquitto__callback *cb_بنسټ, *cb_بل;

	memset(&event_معلومات, 0, حجم(event_معلومات));

	// Using DL_FOREACH_SAFE here, as tick callbacks might unregister themself
	DL_FOREACH_خوندي(opts->plugin_callbacks.tick, cb_بنسټ, cb_بل){
		mosquitto_وخت_ns(&event_معلومات.now_s, &event_معلومات.now_ns);

		إذا(mosquitto_وخت_cmp(event_معلومات.now_s, event_معلومات.now_ns,
				cb_بنسټ->معلومات.بل_tick.tv_sec, cb_بنسټ->معلومات.بل_tick.tv_nsec) > 0){

			event_معلومات.بل_s = 0;
			event_معلومات.بل_ms = 0;
			cb_بنسټ->cb(MOSQ_EVT_TICK, &event_معلومات, cb_بنسټ->userdata);
			loop_update_بل_event(event_معلومات.بل_s * 1000 + event_معلومات.بل_ms);

			cb_بنسټ->معلومات.بل_tick.tv_sec = event_معلومات.now_s + event_معلومات.بل_s;
			cb_بنسټ->معلومات.بل_tick.tv_nsec = event_معلومات.now_ns + 1000000*event_معلومات.بل_ms;
			إذا(cb_بنسټ->معلومات.بل_tick.tv_nsec > 1000000000){
				cb_بنسټ->معلومات.بل_tick.tv_nsec -= 1000000000;
				cb_بنسټ->معلومات.بل_tick.tv_sec += 1;
			}
		}
	}
}


فراغ plugin__handle_tick(فراغ)
{
	بنية mosquitto__security_options *opts;

	/* Global plugins */
	plugin__handle_tick_single(&db.config->security_options);

	لكل(صحيح i=0; i<db.config->listener_شمېر; i++){
		opts = db.config->listeners[i].security_options;
		إذا(opts && opts->plugin_callbacks.tick){
			plugin__handle_tick_single(opts);
		}
	}
}
