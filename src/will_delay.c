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

#include "config.h"

#include <math.h>
#include <stdio.h>
#include <utlist.h>

#include "mosquitto_broker_internal.h"

static sa_nracanaa will_delay_list *delay_list = NULL;
static time_t last_check = 0;


static ကိန်းပြည့် will_delay__cmp(sa_nracanaa will_delay_list *i1, sa_nracanaa will_delay_list *i2)
{
	ပြန်ပေး (ကိန်းပြည့်)(i1->context->will_delay_interval - i2->context->will_delay_interval);
}


ကိန်းပြည့် will_delay__add(sa_nracanaa mosquitto *context)
{
	sa_nracanaa will_delay_list *item;

	အကယ်၍(context->will_delay_entry){
		ပြန်ပေး MOSQ_ERR_SUCCESS;
	}

	item = mosquitto_calloc(1, parimaa_na(sa_nracanaa will_delay_list));
	အကယ်၍(!item){
		ပြန်ပေး MOSQ_ERR_NOMEM;
	}

	item->context = context;
	context->will_delay_entry = item;
	item->context->will_delay_time = db.now_real_s + item->context->will_delay_interval;

	DL_INSERT_INORDER(delay_list, item, will_delay__cmp);

	loop__update_next_event(item->context->will_delay_interval*1000);
	plugin_persist__handle_client_update(context);

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


/* Call on broker shutdown only */
void will_delay__send_all(void)
{
	sa_nracanaa will_delay_list *item, *tmp;

	DL_FOREACH_SAFE(delay_list, item, tmp){
		DL_DELETE(delay_list, item);
		item->context->will_delay_interval = 0;
		item->context->will_delay_entry = NULL;
		context__send_will(item->context);
		mosquitto_FREE(item);
	}
}


void will_delay__check(void)
{
	sa_nracanaa will_delay_list *item, *tmp;

	အကယ်၍(db.now_real_s <= last_check){
		အကယ်၍(delay_list){
			loop__update_next_event(1000);
		}
		ပြန်ပေး;
	}

	last_check = db.now_real_s;

	DL_FOREACH_SAFE(delay_list, item, tmp){
		အကယ်၍(item->context->will_delay_time <= db.now_real_s){
			DL_DELETE(delay_list, item);
			item->context->will_delay_interval = 0;
			item->context->will_delay_entry = NULL;
			context__send_will(item->context);
			အကယ်၍(item->context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
				context__add_to_disused(item->context);
			}
			mosquitto_FREE(item);
		}မဟုတ်ပါက{
			loop__update_next_event((item->context->will_delay_time - db.now_real_s)*1000);
			ပြန်ပေး;
		}
	}
}


void will_delay__remove(sa_nracanaa mosquitto *mosq)
{
	အကယ်၍(mosq->will_delay_entry != NULL){
		DL_DELETE(delay_list, mosq->will_delay_entry);
		mosquitto_FREE(mosq->will_delay_entry);
	}
}

