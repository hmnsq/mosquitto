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

#شامل "config.h"

#شامل <math.h>
#شامل <stdio.h>
#شامل <utlist.h>

#شامل "mosquitto_broker_internal.h"

ایستا ساختار will_delay_list *delay_list = پوچ;
ایستا time_t last_check = 0;


ایستا صحیح will_delay__cmp(ساختار will_delay_list *i1, ساختار will_delay_list *i2)
{
	بازگشت (صحیح)(i1->context->will_delay_interval - i2->context->will_delay_interval);
}


صحیح will_delay__add(ساختار mosquitto *context)
{
	ساختار will_delay_list *item;

	اگر(context->will_delay_entry){
		بازگشت MOSQ_ERR_SUCCESS;
	}

	item = mosquitto_calloc(1, اندازه(ساختار will_delay_list));
	اگر(!item){
		بازگشت MOSQ_ERR_NOMEM;
	}

	item->context = context;
	context->will_delay_entry = item;
	item->context->will_delay_time = db.now_real_s + item->context->will_delay_interval;

	DL_درج_میان_ترتیب(delay_list, item, will_delay__cmp);

	loop__update_next_event(item->context->will_delay_interval*1000);
	plugin_persist__handle_client_update(context);

	بازگشت MOSQ_ERR_SUCCESS;
}


/* Call on broker shutdown only */
تهی will_delay__send_all(تهی)
{
	ساختار will_delay_list *item, *tmp;

	DL_FOREACH_SAFE(delay_list, item, tmp){
		DL_DELETE(delay_list, item);
		item->context->will_delay_interval = 0;
		item->context->will_delay_entry = پوچ;
		context__send_will(item->context);
		mosquitto_FREE(item);
	}
}


تهی will_delay__check(تهی)
{
	ساختار will_delay_list *item, *tmp;

	اگر(db.now_real_s <= last_check){
		اگر(delay_list){
			loop__update_next_event(1000);
		}
		بازگشت;
	}

	last_check = db.now_real_s;

	DL_FOREACH_SAFE(delay_list, item, tmp){
		اگر(item->context->will_delay_time <= db.now_real_s){
			DL_DELETE(delay_list, item);
			item->context->will_delay_interval = 0;
			item->context->will_delay_entry = پوچ;
			context__send_will(item->context);
			اگر(item->context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
				context__add_to_disused(item->context);
			}
			mosquitto_FREE(item);
		}وگرنه{
			loop__update_next_event((item->context->will_delay_time - db.now_real_s)*1000);
			بازگشت;
		}
	}
}


تهی will_delay__remove(ساختار mosquitto *mosq)
{
	اگر(mosq->will_delay_entry != پوچ){
		DL_DELETE(delay_list, mosq->will_delay_entry);
		mosquitto_FREE(mosq->will_delay_entry);
	}
}

