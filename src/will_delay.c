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

#अन्तर्भाव "config.h"

#अन्तर्भाव <math.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <utlist.h>

#अन्तर्भाव "mosquitto_broker_internal.h"

स्थिर संरचना will_delay_list *delay_list = रिक्त;
स्थिर समय_प्रकार last_check = 0;


स्थिर पूर्णांक will_delay__cmp(संरचना will_delay_list *i1, संरचना will_delay_list *i2)
{
	परत (पूर्णांक)(i1->context->will_delay_interval - i2->context->will_delay_interval);
}


पूर्णांक will_delay__add(संरचना mosquitto *context)
{
	संरचना will_delay_list *item;

	जर(context->will_delay_entry){
		परत MOSQ_ERR_SUCCESS;
	}

	item = mosquitto_calloc(1, आकार(संरचना will_delay_list));
	जर(!item){
		परत MOSQ_ERR_NOMEM;
	}

	item->context = context;
	context->will_delay_entry = item;
	item->context->will_delay_time = db.now_real_s + item->context->will_delay_interval;

	DL_घाला_मध्यक्रम(delay_list, item, will_delay__cmp);

	loop__update_next_event(item->context->will_delay_interval*1000);
	plugin_persist__handle_client_update(context);

	परत MOSQ_ERR_SUCCESS;
}


/* Call on broker shutdown only */
शून्य will_delay__send_all(शून्य)
{
	संरचना will_delay_list *item, *tmp;

	DL_FOREACH_SAFE(delay_list, item, tmp){
		DL_DELETE(delay_list, item);
		item->context->will_delay_interval = 0;
		item->context->will_delay_entry = रिक्त;
		context__send_will(item->context);
		mosquitto_FREE(item);
	}
}


शून्य will_delay__check(शून्य)
{
	संरचना will_delay_list *item, *tmp;

	जर(db.now_real_s <= last_check){
		जर(delay_list){
			loop__update_next_event(1000);
		}
		परत;
	}

	last_check = db.now_real_s;

	DL_FOREACH_SAFE(delay_list, item, tmp){
		जर(item->context->will_delay_time <= db.now_real_s){
			DL_DELETE(delay_list, item);
			item->context->will_delay_interval = 0;
			item->context->will_delay_entry = रिक्त;
			context__send_will(item->context);
			जर(item->context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE){
				context__add_to_disused(item->context);
			}
			mosquitto_FREE(item);
		}अन्यथा{
			loop__update_next_event((item->context->will_delay_time - db.now_real_s)*1000);
			परत;
		}
	}
}


शून्य will_delay__remove(संरचना mosquitto *mosq)
{
	जर(mosq->will_delay_entry != रिक्त){
		DL_DELETE(delay_list, mosq->will_delay_entry);
		mosquitto_FREE(mosq->will_delay_entry);
	}
}

