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
ساكن time_t وروستی_check = 0;


ساكن صحيح will_delay__cmp(بنية will_delay_list *i1, بنية will_delay_list *i2)
{
	أرجع (صحيح)(i1->context->will_delay_interval - i2->context->will_delay_interval);
}


صحيح will_delay_جمع_کړه(بنية mosquitto *context)
{
	بنية will_delay_list *توکی;

	إذا(context->will_delay_ثبت){
		أرجع MOSQ_ERR_SUCCESS;
	}

	توکی = mosquitto_calloc(1, حجم(بنية will_delay_list));
	إذا(!توکی){
		أرجع MOSQ_ERR_NOMEM;
	}

	توکی->context = context;
	context->will_delay_ثبت = توکی;
	توکی->context->will_delay_وخت = db.now_real_s + توکی->context->will_delay_interval;

	DL_ورننباسه_منځنی_ترتیب(delay_list, توکی, will_delay__cmp);

	loop_update_بل_event(توکی->context->will_delay_interval*1000);
	plugin_persist__handle_client_update(context);

	أرجع MOSQ_ERR_SUCCESS;
}


/* Call on broker shutdown only */
فراغ will_delay__send_all(فراغ)
{
	بنية will_delay_list *توکی, *tmp;

	DL_FOREACH_خوندي(delay_list, توکی, tmp){
		DL_DELETE(delay_list, توکی);
		توکی->context->will_delay_interval = 0;
		توکی->context->will_delay_ثبت = عدم;
		context__send_will(توکی->context);
		mosquitto_FREE(توکی);
	}
}


فراغ will_delay__check(فراغ)
{
	بنية will_delay_list *توکی, *tmp;

	إذا(db.now_real_s <= وروستی_check){
		إذا(delay_list){
			loop_update_بل_event(1000);
		}
		أرجع;
	}

	وروستی_check = db.now_real_s;

	DL_FOREACH_خوندي(delay_list, توکی, tmp){
		إذا(توکی->context->will_delay_وخت <= db.now_real_s){
			DL_DELETE(delay_list, توکی);
			توکی->context->will_delay_interval = 0;
			توکی->context->will_delay_ثبت = عدم;
			context__send_will(توکی->context);
			إذا(توکی->context->session_expiry_interval == MQTT_SESSION_EXPIRY_سمدستي){
				context_جمع_کړه_منزل_disused(توکی->context);
			}
			mosquitto_FREE(توکی);
		}وإلا{
			loop_update_بل_event((توکی->context->will_delay_وخت - db.now_real_s)*1000);
			أرجع;
		}
	}
}


فراغ will_delay__remove(بنية mosquitto *mosq)
{
	إذا(mosq->will_delay_ثبت != عدم){
		DL_DELETE(delay_list, mosq->will_delay_ثبت);
		mosquitto_FREE(mosq->will_delay_ثبت);
	}
}

