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

#समावेश "config.h"

#समावेश <string.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/broker.h"
#समावेश "utlist.h"


अटल व्योम plugin__handle_tick_single(काष्ठा mosquitto__security_options *opts)
{
	काष्ठा mosquitto_evt_tick event_तथ्य;
	काष्ठा mosquitto__callback *cb_base, *cb_अग्र;

	स_रखो(&event_तथ्य, 0, माप(event_तथ्य));

	// Using DL_FOREACH_SAFE here, as tick callbacks might unregister themself
	DL_FOREACH_SAFE(opts->plugin_callbacks.tick, cb_base, cb_अग्र){
		mosquitto_time_ns(&event_तथ्य.now_s, &event_तथ्य.now_ns);

		अगर(mosquitto_time_cmp(event_तथ्य.now_s, event_तथ्य.now_ns,
				cb_base->तथ्य.अग्र_tick.tv_sec, cb_base->तथ्य.अग्र_tick.tv_nsec) > 0){

			event_तथ्य.अग्र_s = 0;
			event_तथ्य.अग्र_ms = 0;
			cb_base->cb(MOSQ_EVT_TICK, &event_तथ्य, cb_base->userdata);
			loop_update_अग्र_event(event_तथ्य.अग्र_s * 1000 + event_तथ्य.अग्र_ms);

			cb_base->तथ्य.अग्र_tick.tv_sec = event_तथ्य.now_s + event_तथ्य.अग्र_s;
			cb_base->तथ्य.अग्र_tick.tv_nsec = event_तथ्य.now_ns + 1000000*event_तथ्य.अग्र_ms;
			अगर(cb_base->तथ्य.अग्र_tick.tv_nsec > 1000000000){
				cb_base->तथ्य.अग्र_tick.tv_nsec -= 1000000000;
				cb_base->तथ्य.अग्र_tick.tv_sec += 1;
			}
		}
	}
}


व्योम plugin__handle_tick(व्योम)
{
	काष्ठा mosquitto__security_options *opts;

	/* Global plugins */
	plugin__handle_tick_single(&db.config->security_options);

	क्रम(पूर्णांक i=0; i<db.config->listener_गिनती; i++){
		opts = db.config->listeners[i].security_options;
		अगर(opts && opts->plugin_callbacks.tick){
			plugin__handle_tick_single(opts);
		}
	}
}
