/*
Copyright (c) 2019-2021 Roger Light <roger@atchoo.org>

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

#ضمن <math.h>
#ضمن <stdio.h>
#ضمن <utlist.h>

#ضمن "mosquitto_broker_internal.h"

ساكن بنية will_delay_list *delay_list = عدم;
ساكن time_t last_check = 0;


ساكن صحيح will_delay__cmp(بنية will_delay_list *i1, بنية will_delay_list *i2)
{
	ܦܢܐ (صحيح)(i1->context->will_delay_interval - i2->context->will_delay_interval);
}


صحيح will_delay__add(بنية mosquitto *context)
{
	بنية will_delay_list *item;

	ܐܢ(context->will_delay_entry){
		ܦܢܐ MOSQ_ERR_SUCCESS;
	}

	item = mosquitto_calloc(1, حجم(بنية will_delay_list));
	ܐܢ(!item){
		ܦܢܐ MOSQ_ERR_NOMEM;
	}

	item->context = context;
	context->will_delay_entry = item;
	item->context->will_delay_time = db.now_real_s + item->context->will_delay_interval;

	DL_INSERT_INORDER(delay_list, item, will_delay__cmp);

	loop__update_next_event(item->context->will_delay_interval*1000);
	plugin_persist__handle_client_update(context);

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


/* Call on broker shutdown only */
فراغ will_delay__send_all(فراغ)
{
	بنية will_delay_list *item, *tmp;

	DL_FOREACH_SAFE(delay_list, item, tmp){
		DL_DELETE(delay_list, item);
		item->context->will_delay_interval = 0;
		item->context->will_delay_entry = عدم;
		context__send_will(item->context);
		mosquitto_FREE(item);
	}
}


فراغ will_delay__check(فراغ)
{
	بنية will_delay_list *item, *tmp;

	ܐܢ(db.now_real_s <= last_check){
		ܐܢ(delay_list){
			loop__update_next_event(1000);
		}
		ܦܢܐ;
	}

	last_check = db.now_real_s;

	DL_FOREACH_SAFE(delay_list, item, tmp){
		ܐܢ(item->context->will_delay_time <= db.now_real_s){
			DL_DELETE(delay_list, item);
			item->context->will_delay_interval = 0;
			item->context->will_delay_entry = عدم;
			context__send_will(item->context);
			ܐܢ(item->context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
				context__add_to_disused(item->context);
			}
			mosquitto_FREE(item);
		}ܐܚܪܢܐ{
			loop__update_next_event((item->context->will_delay_time - db.now_real_s)*1000);
			ܦܢܐ;
		}
	}
}


فراغ will_delay__remove(بنية mosquitto *mosq)
{
	ܐܢ(mosq->will_delay_entry != عدم){
		DL_DELETE(delay_list, mosq->will_delay_entry);
		mosquitto_FREE(mosq->will_delay_entry);
	}
}

