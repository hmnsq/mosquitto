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

ساکن ساخت session_expiry_list *expiry_list = تہی;
ساکن time_t آخری_check = 0;


ساکن صحیح session_expiry__cmp(ساخت session_expiry_list *i1, ساخت session_expiry_list *i2)
{
	اگر(i1->context->session_expiry_وقت == i2->context->session_expiry_وقت){
		واپس 0;
	}ورنہ اگر(i1->context->session_expiry_وقت > i2->context->session_expiry_وقت){
		واپس 1;
	}ورنہ{
		واپس -1;
	}
}


ساکن خالی سیٹ_session_expiry_وقت(ساخت mosquitto *context)
{
	context->session_expiry_وقت = db.now_real_s;

	اگر(db.config->persistent_client_expiration == 0){
		/* No global expiry, so use the client expiration interval */
		context->session_expiry_وقت += context->session_expiry_interval;
	}ورنہ{
		/* We have a global expiry interval */
		اگر(db.config->persistent_client_expiration < context->session_expiry_interval){
			/* The client expiry is longer than the global expiry, so use the global */
			context->session_expiry_وقت += db.config->persistent_client_expiration;
		}ورنہ{
			/* The global expiry is longer than the client expiry, so use the client */
			context->session_expiry_وقت += context->session_expiry_interval;
		}
	}
}


صحیح session_expiry_جوڑو(ساخت mosquitto *context)
{
	ساخت session_expiry_list *شے;

	اگر(context->expiry_list_شے){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		واپس MOSQ_ERR_SUCCESS;
	}

	اگر(db.config->persistent_client_expiration == 0){
		اگر(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			واپس MOSQ_ERR_SUCCESS;
		}
	}

	شے = mosquitto_calloc(1, حجم(ساخت session_expiry_list));
	اگر(!شے){
		واپس MOSQ_ERR_NOMEM;
	}

	شے->context = context;
	سیٹ_session_expiry_وقت(شے->context);
	context->expiry_list_شے = شے;

	DL_داخل_کرو_درمیانی_ترتیب(expiry_list, شے, session_expiry__cmp);

	plugin_persist__handle_client_update(context);

	واپس MOSQ_ERR_SUCCESS;
}


صحیح session_expiry_جوڑو_from_persistence(ساخت mosquitto *context, time_t expiry_وقت)
{
	ساخت session_expiry_list *شے;

	اگر(context->expiry_list_شے){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		واپس MOSQ_ERR_SUCCESS;
	}

	اگر(db.config->persistent_client_expiration == 0){
		اگر(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			واپس MOSQ_ERR_SUCCESS;
		}
	}

	شے = mosquitto_calloc(1, حجم(ساخت session_expiry_list));
	اگر(!شے){
		واپس MOSQ_ERR_NOMEM;
	}

	شے->context = context;
	اگر(expiry_وقت){
		شے->context->session_expiry_وقت = expiry_وقت;
	}ورنہ{
		سیٹ_session_expiry_وقت(شے->context);

	}
	context->expiry_list_شے = شے;

	DL_داخل_کرو_درمیانی_ترتیب(expiry_list, شے, session_expiry__cmp);

	واپس MOSQ_ERR_SUCCESS;
}


خالی session_expiry_جوڑو_on_shutdown(خالی)
{
	ساخت mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		اگر(ctxt->bridge == تہی
				&& ctxt->is_persisted
				&& ctxt->session_expiry_interval != MQTT_SESSION_EXPIRY_فوری
				){

			session_expiry_جوڑو(ctxt);
		}
	}
}


خالی session_expiry__remove(ساخت mosquitto *context)
{
	اگر(context->expiry_list_شے){
		DL_DELETE(expiry_list, context->expiry_list_شے);
		mosquitto_FREE(context->expiry_list_شے);
	}
}


/* Call on broker shutdown only */
خالی session_expiry__remove_all(خالی)
{
	ساخت session_expiry_list *شے, *tmp;
	ساخت mosquitto *context;

	DL_FOREACH_محفوظ(expiry_list, شے, tmp){
		context = شے->context;
		session_expiry__remove(context);
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوری;
		context->will_delay_interval = 0;
		will_delay__remove(context);
		context__disconnect(context, -1);
	}
}


خالی session_expiry__check(خالی)
{
	ساخت session_expiry_list *شے, *tmp;
	ساخت mosquitto *context;
	time_t timeout;

	اگر(آخری_check != 0 && db.now_real_s <= آخری_check){
		اگر(expiry_list){
			/* Next event is the first item of the list, we must set the timeout even if we aren't
			 * checking the full list */
			timeout = (expiry_list->context->session_expiry_وقت - db.now_real_s) * 1000;
			اگر(timeout <= 0){
				timeout = 100;
			}
			loop_update_اگلا_event(timeout);
		}
		واپس;
	}

	آخری_check = db.now_real_s;

	DL_FOREACH_محفوظ(expiry_list, شے, tmp){
		اگر(شے->context->session_expiry_وقت < db.now_real_s){

			context = شے->context;
			session_expiry__remove(context);

			اگر(context->id){
				log__printf(تہی, MOSQ_LOG_NOTICE, "Expiring client %s due to timeout.", context->id);
			}
			metrics__int_inc(mosq_شمار_clients_expired, 1);

			/* Session has now expired, so clear interval */
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_فوری;
			/* Session has expired, so will delay should be cleared. */
			context->will_delay_interval = 0;
			will_delay__remove(context);
			context__send_will(context);
			plugin_persist__handle_client_delete(context);
			context_جوڑو_مقصد_disused(context);
		}ورنہ{
			timeout = (شے->context->session_expiry_وقت - db.now_real_s + 1) * 1000;
			اگر(timeout <= 0){
				timeout = 100;
			}
			loop_update_اگلا_event(timeout);
			واپس;
		}
	}
}
