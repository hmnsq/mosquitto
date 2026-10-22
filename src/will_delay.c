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
ساكن time_t آخري_check = 0;


ساكن صحيح will_delay__cmp(بنية will_delay_list *i1, بنية will_delay_list *i2)
{
	أرجع (صحيح)(i1->context->will_delay_interval - i2->context->will_delay_interval);
}


صحيح will_delay_جوڙيو(بنية mosquitto *context)
{
	بنية will_delay_list *شي;

	إذا(context->will_delay_داخلا){
		أرجع MOSQ_ERR_SUCCESS;
	}

	شي = mosquitto_calloc(1, حجم(بنية will_delay_list));
	إذا(!شي){
		أرجع MOSQ_ERR_NOMEM;
	}

	شي->context = context;
	context->will_delay_داخلا = شي;
	شي->context->will_delay_وقت = db.now_real_s + شي->context->will_delay_interval;

	DL_وجهو_وچين_ترتيب(delay_list, شي, will_delay__cmp);

	loop_update_ايندڙ_event(شي->context->will_delay_interval*1000);
	plugin_persist__handle_client_update(context);

	أرجع MOSQ_ERR_SUCCESS;
}


/* Call on broker shutdown only */
فراغ will_delay__send_all(فراغ)
{
	بنية will_delay_list *شي, *tmp;

	DL_FOREACH_محفوظ(delay_list, شي, tmp){
		DL_DELETE(delay_list, شي);
		شي->context->will_delay_interval = 0;
		شي->context->will_delay_داخلا = عدم;
		context__send_will(شي->context);
		mosquitto_FREE(شي);
	}
}


فراغ will_delay__check(فراغ)
{
	بنية will_delay_list *شي, *tmp;

	إذا(db.now_real_s <= آخري_check){
		إذا(delay_list){
			loop_update_ايندڙ_event(1000);
		}
		أرجع;
	}

	آخري_check = db.now_real_s;

	DL_FOREACH_محفوظ(delay_list, شي, tmp){
		إذا(شي->context->will_delay_وقت <= db.now_real_s){
			DL_DELETE(delay_list, شي);
			شي->context->will_delay_interval = 0;
			شي->context->will_delay_داخلا = عدم;
			context__send_will(شي->context);
			إذا(شي->context->session_expiry_interval == MQTT_SESSION_EXPIRY_فوري){
				context_جوڙيو_منزل_disused(شي->context);
			}
			mosquitto_FREE(شي);
		}وإلا{
			loop_update_ايندڙ_event((شي->context->will_delay_وقت - db.now_real_s)*1000);
			أرجع;
		}
	}
}


فراغ will_delay__remove(بنية mosquitto *mosq)
{
	إذا(mosq->will_delay_داخلا != عدم){
		DL_DELETE(delay_list, mosq->will_delay_داخلا);
		mosquitto_FREE(mosq->will_delay_داخلا);
	}
}

