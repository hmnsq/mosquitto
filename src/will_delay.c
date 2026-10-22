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

ساکن ساخت will_delay_list *delay_list = تہی;
ساکن time_t آخری_check = 0;


ساکن صحیح will_delay__cmp(ساخت will_delay_list *i1, ساخت will_delay_list *i2)
{
	واپس (صحیح)(i1->context->will_delay_interval - i2->context->will_delay_interval);
}


صحیح will_delay_جوڑو(ساخت mosquitto *context)
{
	ساخت will_delay_list *شے;

	اگر(context->will_delay_اندراج){
		واپس MOSQ_ERR_SUCCESS;
	}

	شے = mosquitto_calloc(1, حجم(ساخت will_delay_list));
	اگر(!شے){
		واپس MOSQ_ERR_NOMEM;
	}

	شے->context = context;
	context->will_delay_اندراج = شے;
	شے->context->will_delay_وقت = db.now_real_s + شے->context->will_delay_interval;

	DL_داخل_کرو_درمیانی_ترتیب(delay_list, شے, will_delay__cmp);

	loop_update_اگلا_event(شے->context->will_delay_interval*1000);
	plugin_persist__handle_client_update(context);

	واپس MOSQ_ERR_SUCCESS;
}


/* Call on broker shutdown only */
خالی will_delay__send_all(خالی)
{
	ساخت will_delay_list *شے, *tmp;

	DL_FOREACH_محفوظ(delay_list, شے, tmp){
		DL_DELETE(delay_list, شے);
		شے->context->will_delay_interval = 0;
		شے->context->will_delay_اندراج = تہی;
		context__send_will(شے->context);
		mosquitto_FREE(شے);
	}
}


خالی will_delay__check(خالی)
{
	ساخت will_delay_list *شے, *tmp;

	اگر(db.now_real_s <= آخری_check){
		اگر(delay_list){
			loop_update_اگلا_event(1000);
		}
		واپس;
	}

	آخری_check = db.now_real_s;

	DL_FOREACH_محفوظ(delay_list, شے, tmp){
		اگر(شے->context->will_delay_وقت <= db.now_real_s){
			DL_DELETE(delay_list, شے);
			شے->context->will_delay_interval = 0;
			شے->context->will_delay_اندراج = تہی;
			context__send_will(شے->context);
			اگر(شے->context->session_expiry_interval == MQTT_SESSION_EXPIRY_فوری){
				context_جوڑو_مقصد_disused(شے->context);
			}
			mosquitto_FREE(شے);
		}ورنہ{
			loop_update_اگلا_event((شے->context->will_delay_وقت - db.now_real_s)*1000);
			واپس;
		}
	}
}


خالی will_delay__remove(ساخت mosquitto *mosq)
{
	اگر(mosq->will_delay_اندراج != تہی){
		DL_DELETE(delay_list, mosq->will_delay_اندراج);
		mosquitto_FREE(mosq->will_delay_اندراج);
	}
}

