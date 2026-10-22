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
#شامل "sys_tree.h"

ایستا ساختار session_expiry_list *expiry_list = پوچ;
ایستا time_t last_check = 0;


ایستا صحیح session_expiry__cmp(ساختار session_expiry_list *i1, ساختار session_expiry_list *i2)
{
	اگر(i1->context->session_expiry_time == i2->context->session_expiry_time){
		بازگشت 0;
	}وگرنه اگر(i1->context->session_expiry_time > i2->context->session_expiry_time){
		بازگشت 1;
	}وگرنه{
		بازگشت -1;
	}
}


ایستا تهی set_session_expiry_time(ساختار mosquitto *context)
{
	context->session_expiry_time = db.now_real_s;

	اگر(db.config->persistent_client_expiration == 0){
		/* No global expiry, so use the client expiration interval */
		context->session_expiry_time += context->session_expiry_interval;
	}وگرنه{
		/* We have a global expiry interval */
		اگر(db.config->persistent_client_expiration < context->session_expiry_interval){
			/* The client expiry is longer than the global expiry, so use the global */
			context->session_expiry_time += db.config->persistent_client_expiration;
		}وگرنه{
			/* The global expiry is longer than the client expiry, so use the client */
			context->session_expiry_time += context->session_expiry_interval;
		}
	}
}


صحیح session_expiry__add(ساختار mosquitto *context)
{
	ساختار session_expiry_list *item;

	اگر(context->expiry_list_item){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		بازگشت MOSQ_ERR_SUCCESS;
	}

	اگر(db.config->persistent_client_expiration == 0){
		اگر(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			بازگشت MOSQ_ERR_SUCCESS;
		}
	}

	item = mosquitto_calloc(1, اندازه(ساختار session_expiry_list));
	اگر(!item){
		بازگشت MOSQ_ERR_NOMEM;
	}

	item->context = context;
	set_session_expiry_time(item->context);
	context->expiry_list_item = item;

	DL_درج_میان_ترتیب(expiry_list, item, session_expiry__cmp);

	plugin_persist__handle_client_update(context);

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح session_expiry__add_from_persistence(ساختار mosquitto *context, time_t expiry_time)
{
	ساختار session_expiry_list *item;

	اگر(context->expiry_list_item){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		بازگشت MOSQ_ERR_SUCCESS;
	}

	اگر(db.config->persistent_client_expiration == 0){
		اگر(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			بازگشت MOSQ_ERR_SUCCESS;
		}
	}

	item = mosquitto_calloc(1, اندازه(ساختار session_expiry_list));
	اگر(!item){
		بازگشت MOSQ_ERR_NOMEM;
	}

	item->context = context;
	اگر(expiry_time){
		item->context->session_expiry_time = expiry_time;
	}وگرنه{
		set_session_expiry_time(item->context);

	}
	context->expiry_list_item = item;

	DL_درج_میان_ترتیب(expiry_list, item, session_expiry__cmp);

	بازگشت MOSQ_ERR_SUCCESS;
}


تهی session_expiry__add_on_shutdown(تهی)
{
	ساختار mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		اگر(ctxt->bridge == پوچ
				&& ctxt->is_persisted
				&& ctxt->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				){

			session_expiry__add(ctxt);
		}
	}
}


تهی session_expiry__remove(ساختار mosquitto *context)
{
	اگر(context->expiry_list_item){
		DL_DELETE(expiry_list, context->expiry_list_item);
		mosquitto_FREE(context->expiry_list_item);
	}
}


/* Call on broker shutdown only */
تهی session_expiry__remove_all(تهی)
{
	ساختار session_expiry_list *item, *tmp;
	ساختار mosquitto *context;

	DL_FOREACH_SAFE(expiry_list, item, tmp){
		context = item->context;
		session_expiry__remove(context);
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
		context->will_delay_interval = 0;
		will_delay__remove(context);
		context__disconnect(context, -1);
	}
}


تهی session_expiry__check(تهی)
{
	ساختار session_expiry_list *item, *tmp;
	ساختار mosquitto *context;
	time_t timeout;

	اگر(last_check != 0 && db.now_real_s <= last_check){
		اگر(expiry_list){
			/* Next event is the first item of the list, we must set the timeout even if we aren't
			 * checking the full list */
			timeout = (expiry_list->context->session_expiry_time - db.now_real_s) * 1000;
			اگر(timeout <= 0){
				timeout = 100;
			}
			loop__update_next_event(timeout);
		}
		بازگشت;
	}

	last_check = db.now_real_s;

	DL_FOREACH_SAFE(expiry_list, item, tmp){
		اگر(item->context->session_expiry_time < db.now_real_s){

			context = item->context;
			session_expiry__remove(context);

			اگر(context->id){
				log__printf(پوچ, MOSQ_LOG_NOTICE, "Expiring client %s due to timeout.", context->id);
			}
			metrics__int_inc(mosq_counter_clients_expired, 1);

			/* Session has now expired, so clear interval */
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			/* Session has expired, so will delay should be cleared. */
			context->will_delay_interval = 0;
			will_delay__remove(context);
			context__send_will(context);
			plugin_persist__handle_client_delete(context);
			context__add_to_disused(context);
		}وگرنه{
			timeout = (item->context->session_expiry_time - db.now_real_s + 1) * 1000;
			اگر(timeout <= 0){
				timeout = 100;
			}
			loop__update_next_event(timeout);
			بازگشت;
		}
	}
}
