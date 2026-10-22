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
स्थिर time_t चरमः_check = 0;


स्थिर पूर्णाङ्क will_delay__cmp(संरचना will_delay_list *i1, संरचना will_delay_list *i2)
{
	प्रत्यागम (पूर्णाङ्क)(i1->context->will_delay_interval - i2->context->will_delay_interval);
}


पूर्णाङ्क will_delay_सङ्कलय(संरचना mosquitto *context)
{
	संरचना will_delay_list *item;

	यदि(context->will_delay_प्रविष्टिः){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	item = mosquitto_calloc(1, परिमाण(संरचना will_delay_list));
	यदि(!item){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	item->context = context;
	context->will_delay_प्रविष्टिः = item;
	item->context->will_delay_time = db.now_real_s + item->context->will_delay_interval;

	DL_निवेशय_मध्यक्रमः(delay_list, item, will_delay__cmp);

	loop_update_अग्र_event(item->context->will_delay_interval*1000);
	plugin_persist__handle_client_update(context);

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


/* Call on broker shutdown only */
शून्य will_delay__send_all(शून्य)
{
	संरचना will_delay_list *item, *tmp;

	DL_FOREACH_SAFE(delay_list, item, tmp){
		DL_DELETE(delay_list, item);
		item->context->will_delay_interval = 0;
		item->context->will_delay_प्रविष्टिः = रिक्त;
		context__send_will(item->context);
		mosquitto_FREE(item);
	}
}


शून्य will_delay__check(शून्य)
{
	संरचना will_delay_list *item, *tmp;

	यदि(db.now_real_s <= चरमः_check){
		यदि(delay_list){
			loop_update_अग्र_event(1000);
		}
		प्रत्यागम;
	}

	चरमः_check = db.now_real_s;

	DL_FOREACH_SAFE(delay_list, item, tmp){
		यदि(item->context->will_delay_time <= db.now_real_s){
			DL_DELETE(delay_list, item);
			item->context->will_delay_interval = 0;
			item->context->will_delay_प्रविष्टिः = रिक्त;
			context__send_will(item->context);
			यदि(item->context->session_expiry_interval == MQTT_SESSION_EXPIRY_तात्कालिकम्){
				context_सङ्कलय_गन्तव्यम्_disused(item->context);
			}
			mosquitto_FREE(item);
		}अन्यथा{
			loop_update_अग्र_event((item->context->will_delay_time - db.now_real_s)*1000);
			प्रत्यागम;
		}
	}
}


शून्य will_delay__remove(संरचना mosquitto *mosq)
{
	यदि(mosq->will_delay_प्रविष्टिः != रिक्त){
		DL_DELETE(delay_list, mosq->will_delay_प्रविष्टिः);
		mosquitto_FREE(mosq->will_delay_प्रविष्टिः);
	}
}

