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

#شامل "config.h"

#شامل <string.h>

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/broker.h"
#شامل "utlist.h"


ساکن خالی plugin__handle_tick_single(ساخت mosquitto__security_options *opts)
{
	ساخت mosquitto_evt_tick event_ڈیٹا;
	ساخت mosquitto__callback *cb_بنیاد, *cb_اگلا;

	memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));

	// Using DL_FOREACH_SAFE here, as tick callbacks might unregister themself
	DL_FOREACH_محفوظ(opts->plugin_callbacks.tick, cb_بنیاد, cb_اگلا){
		mosquitto_وقت_ns(&event_ڈیٹا.now_s, &event_ڈیٹا.now_ns);

		اگر(mosquitto_وقت_cmp(event_ڈیٹا.now_s, event_ڈیٹا.now_ns,
				cb_بنیاد->ڈیٹا.اگلا_tick.tv_sec, cb_بنیاد->ڈیٹا.اگلا_tick.tv_nsec) > 0){

			event_ڈیٹا.اگلا_s = 0;
			event_ڈیٹا.اگلا_ms = 0;
			cb_بنیاد->cb(MOSQ_EVT_TICK, &event_ڈیٹا, cb_بنیاد->userdata);
			loop_update_اگلا_event(event_ڈیٹا.اگلا_s * 1000 + event_ڈیٹا.اگلا_ms);

			cb_بنیاد->ڈیٹا.اگلا_tick.tv_sec = event_ڈیٹا.now_s + event_ڈیٹا.اگلا_s;
			cb_بنیاد->ڈیٹا.اگلا_tick.tv_nsec = event_ڈیٹا.now_ns + 1000000*event_ڈیٹا.اگلا_ms;
			اگر(cb_بنیاد->ڈیٹا.اگلا_tick.tv_nsec > 1000000000){
				cb_بنیاد->ڈیٹا.اگلا_tick.tv_nsec -= 1000000000;
				cb_بنیاد->ڈیٹا.اگلا_tick.tv_sec += 1;
			}
		}
	}
}


خالی plugin__handle_tick(خالی)
{
	ساخت mosquitto__security_options *opts;

	/* Global plugins */
	plugin__handle_tick_single(&db.config->security_options);

	برائے(صحیح i=0; i<db.config->listener_گنتی; i++){
		opts = db.config->listeners[i].security_options;
		اگر(opts && opts->plugin_callbacks.tick){
			plugin__handle_tick_single(opts);
		}
	}
}
