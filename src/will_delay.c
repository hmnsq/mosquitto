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
ساكن time_t آخرین_check = 0;


ساكن صحيح will_delay__cmp(بنية will_delay_list *i1, بنية will_delay_list *i2)
{
	أرجع (صحيح)(i1->context->will_delay_interval - i2->context->will_delay_interval);
}


صحيح will_delay_جمع_کن(بنية mosquitto *context)
{
	بنية will_delay_list *قلم;

	إذا(context->will_delay_مدخل){
		أرجع MOSQ_ERR_SUCCESS;
	}

	قلم = mosquitto_calloc(1, حجم(بنية will_delay_list));
	إذا(!قلم){
		أرجع MOSQ_ERR_NOMEM;
	}

	قلم->context = context;
	context->will_delay_مدخل = قلم;
	قلم->context->will_delay_زمان = db.now_real_s + قلم->context->will_delay_interval;

	DL_درج_میان_ترتیب(delay_list, قلم, will_delay__cmp);

	loop_update_بعدی_event(قلم->context->will_delay_interval*1000);
	plugin_persist__handle_client_update(context);

	أرجع MOSQ_ERR_SUCCESS;
}


/* Call on broker shutdown only */
فراغ will_delay__send_all(فراغ)
{
	بنية will_delay_list *قلم, *tmp;

	DL_FOREACH_امن(delay_list, قلم, tmp){
		DL_DELETE(delay_list, قلم);
		قلم->context->will_delay_interval = 0;
		قلم->context->will_delay_مدخل = عدم;
		context__send_will(قلم->context);
		mosquitto_FREE(قلم);
	}
}


فراغ will_delay__check(فراغ)
{
	بنية will_delay_list *قلم, *tmp;

	إذا(db.now_real_s <= آخرین_check){
		إذا(delay_list){
			loop_update_بعدی_event(1000);
		}
		أرجع;
	}

	آخرین_check = db.now_real_s;

	DL_FOREACH_امن(delay_list, قلم, tmp){
		إذا(قلم->context->will_delay_زمان <= db.now_real_s){
			DL_DELETE(delay_list, قلم);
			قلم->context->will_delay_interval = 0;
			قلم->context->will_delay_مدخل = عدم;
			context__send_will(قلم->context);
			إذا(قلم->context->session_expiry_interval == MQTT_SESSION_EXPIRY_فوری){
				context_جمع_کن_مقصد_disused(قلم->context);
			}
			mosquitto_FREE(قلم);
		}وإلا{
			loop_update_بعدی_event((قلم->context->will_delay_زمان - db.now_real_s)*1000);
			أرجع;
		}
	}
}


فراغ will_delay__remove(بنية mosquitto *mosq)
{
	إذا(mosq->will_delay_مدخل != عدم){
		DL_DELETE(delay_list, mosq->will_delay_مدخل);
		mosquitto_FREE(mosq->will_delay_مدخل);
	}
}

