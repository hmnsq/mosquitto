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
#ଅନ୍ତର୍ଭାଵ "sys_tree.h"

ସ୍ଥିର ସଂରଚନା session_expiry_list *expiry_list = ଖାଲି;
ସ୍ଥିର time_t last_check = 0;


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା session_expiry__cmp(ସଂରଚନା session_expiry_list *i1, ସଂରଚନା session_expiry_list *i2)
{
	ଯଦି(i1->context->session_expiry_time == i2->context->session_expiry_time){
		ଫେରନ୍ତୁ 0;
	}ଅନ୍ୟଥା ଯଦି(i1->context->session_expiry_time > i2->context->session_expiry_time){
		ଫେରନ୍ତୁ 1;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ -1;
	}
}


ସ୍ଥିର ଶୂନ୍ୟ set_session_expiry_time(ସଂରଚନା mosquitto *context)
{
	context->session_expiry_time = db.now_real_s;

	ଯଦି(db.config->persistent_client_expiration == 0){
		/* No global expiry, so use the client expiration interval */
		context->session_expiry_time += context->session_expiry_interval;
	}ଅନ୍ୟଥା{
		/* We have a global expiry interval */
		ଯଦି(db.config->persistent_client_expiration < context->session_expiry_interval){
			/* The client expiry is longer than the global expiry, so use the global */
			context->session_expiry_time += db.config->persistent_client_expiration;
		}ଅନ୍ୟଥା{
			/* The global expiry is longer than the client expiry, so use the client */
			context->session_expiry_time += context->session_expiry_interval;
		}
	}
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା session_expiry__add(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା session_expiry_list *item;

	ଯଦି(context->expiry_list_item){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	ଯଦି(db.config->persistent_client_expiration == 0){
		ଯଦି(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}
	}

	item = mosquitto_calloc(1, ଆକାର(ସଂରଚନା session_expiry_list));
	ଯଦି(!item){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	item->context = context;
	set_session_expiry_time(item->context);
	context->expiry_list_item = item;

	DL_ଭର୍ତ୍ତି_ମଧ୍ୟକ୍ରମ(expiry_list, item, session_expiry__cmp);

	plugin_persist__handle_client_update(context);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା session_expiry__add_from_persistence(ସଂରଚନା mosquitto *context, time_t expiry_time)
{
	ସଂରଚନା session_expiry_list *item;

	ଯଦି(context->expiry_list_item){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	ଯଦି(db.config->persistent_client_expiration == 0){
		ଯଦି(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}
	}

	item = mosquitto_calloc(1, ଆକାର(ସଂରଚନା session_expiry_list));
	ଯଦି(!item){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	item->context = context;
	ଯଦି(expiry_time){
		item->context->session_expiry_time = expiry_time;
	}ଅନ୍ୟଥା{
		set_session_expiry_time(item->context);

	}
	context->expiry_list_item = item;

	DL_ଭର୍ତ୍ତି_ମଧ୍ୟକ୍ରମ(expiry_list, item, session_expiry__cmp);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ଶୂନ୍ୟ session_expiry__add_on_shutdown(ଶୂନ୍ୟ)
{
	ସଂରଚନା mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		ଯଦି(ctxt->bridge == ଖାଲି
				&& ctxt->is_persisted
				&& ctxt->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				){

			session_expiry__add(ctxt);
		}
	}
}


ଶୂନ୍ୟ session_expiry__remove(ସଂରଚନା mosquitto *context)
{
	ଯଦି(context->expiry_list_item){
		DL_DELETE(expiry_list, context->expiry_list_item);
		mosquitto_FREE(context->expiry_list_item);
	}
}


/* Call on broker shutdown only */
ଶୂନ୍ୟ session_expiry__remove_all(ଶୂନ୍ୟ)
{
	ସଂରଚନା session_expiry_list *item, *tmp;
	ସଂରଚନା mosquitto *context;

	DL_FOREACH_SAFE(expiry_list, item, tmp){
		context = item->context;
		session_expiry__remove(context);
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
		context->will_delay_interval = 0;
		will_delay__remove(context);
		context__disconnect(context, -1);
	}
}


ଶୂନ୍ୟ session_expiry__check(ଶୂନ୍ୟ)
{
	ସଂରଚନା session_expiry_list *item, *tmp;
	ସଂରଚନା mosquitto *context;
	time_t timeout;

	ଯଦି(last_check != 0 && db.now_real_s <= last_check){
		ଯଦି(expiry_list){
			/* Next event is the first item of the list, we must set the timeout even if we aren't
			 * checking the full list */
			timeout = (expiry_list->context->session_expiry_time - db.now_real_s) * 1000;
			ଯଦି(timeout <= 0){
				timeout = 100;
			}
			loop__update_next_event(timeout);
		}
		ଫେରନ୍ତୁ;
	}

	last_check = db.now_real_s;

	DL_FOREACH_SAFE(expiry_list, item, tmp){
		ଯଦି(item->context->session_expiry_time < db.now_real_s){

			context = item->context;
			session_expiry__remove(context);

			ଯଦି(context->id){
				log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Expiring client %s due to timeout.", context->id);
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
		}ଅନ୍ୟଥା{
			timeout = (item->context->session_expiry_time - db.now_real_s + 1) * 1000;
			ଯଦି(timeout <= 0){
				timeout = 100;
			}
			loop__update_next_event(timeout);
			ଫେରନ୍ତୁ;
		}
	}
}
