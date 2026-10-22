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
	بنية mosquitto_evt_tick event_داده;
	بنية mosquitto__callback *cb_پایه, *cb_بعدی;

	memset(&event_داده, 0, حجم(event_داده));

	// Using DL_FOREACH_SAFE here, as tick callbacks might unregister themself
	DL_FOREACH_امن(opts->plugin_callbacks.tick, cb_پایه, cb_بعدی){
		mosquitto_زمان_ns(&event_داده.now_s, &event_داده.now_ns);

		إذا(mosquitto_زمان_cmp(event_داده.now_s, event_داده.now_ns,
				cb_پایه->داده.بعدی_tick.tv_sec, cb_پایه->داده.بعدی_tick.tv_nsec) > 0){

			event_داده.بعدی_s = 0;
			event_داده.بعدی_ms = 0;
			cb_پایه->cb(MOSQ_EVT_TICK, &event_داده, cb_پایه->userdata);
			loop_update_بعدی_event(event_داده.بعدی_s * 1000 + event_داده.بعدی_ms);

			cb_پایه->داده.بعدی_tick.tv_sec = event_داده.now_s + event_داده.بعدی_s;
			cb_پایه->داده.بعدی_tick.tv_nsec = event_داده.now_ns + 1000000*event_داده.بعدی_ms;
			إذا(cb_پایه->داده.بعدی_tick.tv_nsec > 1000000000){
				cb_پایه->داده.بعدی_tick.tv_nsec -= 1000000000;
				cb_پایه->داده.بعدی_tick.tv_sec += 1;
			}
		}
	}
}


فراغ plugin__handle_tick(فراغ)
{
	بنية mosquitto__security_options *opts;

	/* Global plugins */
	plugin__handle_tick_single(&db.config->security_options);

	لكل(صحيح i=0; i<db.config->listener_شمار; i++){
		opts = db.config->listeners[i].security_options;
		إذا(opts && opts->plugin_callbacks.tick){
			plugin__handle_tick_single(opts);
		}
	}
}
