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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <math.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <utlist.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"

ସ୍ଥିର ସଂରଚନା will_delay_list *delay_list = ଖାଲି;
ସ୍ଥିର time_t last_check = 0;


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା will_delay__cmp(ସଂରଚନା will_delay_list *i1, ସଂରଚନା will_delay_list *i2)
{
	ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(i1->context->will_delay_interval - i2->context->will_delay_interval);
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା will_delay__add(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା will_delay_list *item;

	ଯଦି(context->will_delay_entry){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	item = mosquitto_calloc(1, ଆକାର(ସଂରଚନା will_delay_list));
	ଯଦି(!item){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	item->context = context;
	context->will_delay_entry = item;
	item->context->will_delay_time = db.now_real_s + item->context->will_delay_interval;

	DL_ଭର୍ତ୍ତି_ମଧ୍ୟକ୍ରମ(delay_list, item, will_delay__cmp);

	loop__update_next_event(item->context->will_delay_interval*1000);
	plugin_persist__handle_client_update(context);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


/* Call on broker shutdown only */
ଶୂନ୍ୟ will_delay__send_all(ଶୂନ୍ୟ)
{
	ସଂରଚନା will_delay_list *item, *tmp;

	DL_FOREACH_SAFE(delay_list, item, tmp){
		DL_DELETE(delay_list, item);
		item->context->will_delay_interval = 0;
		item->context->will_delay_entry = ଖାଲି;
		context__send_will(item->context);
		mosquitto_FREE(item);
	}
}


ଶୂନ୍ୟ will_delay__check(ଶୂନ୍ୟ)
{
	ସଂରଚନା will_delay_list *item, *tmp;

	ଯଦି(db.now_real_s <= last_check){
		ଯଦି(delay_list){
			loop__update_next_event(1000);
		}
		ଫେରନ୍ତୁ;
	}

	last_check = db.now_real_s;

	DL_FOREACH_SAFE(delay_list, item, tmp){
		ଯଦି(item->context->will_delay_time <= db.now_real_s){
			DL_DELETE(delay_list, item);
			item->context->will_delay_interval = 0;
			item->context->will_delay_entry = ଖାଲି;
			context__send_will(item->context);
			ଯଦି(item->context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
				context__add_to_disused(item->context);
			}
			mosquitto_FREE(item);
		}ଅନ୍ୟଥା{
			loop__update_next_event((item->context->will_delay_time - db.now_real_s)*1000);
			ଫେରନ୍ତୁ;
		}
	}
}


ଶୂନ୍ୟ will_delay__remove(ସଂରଚନା mosquitto *mosq)
{
	ଯଦି(mosq->will_delay_entry != ଖାଲି){
		DL_DELETE(delay_list, mosq->will_delay_entry);
		mosquitto_FREE(mosq->will_delay_entry);
	}
}

