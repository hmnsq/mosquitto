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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <math.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <utlist.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "sys_tree.h"

நிலையான கட்டமைப்பு session_expiry_list *expiry_list = வெற்று;
நிலையான time_t last_check = 0;


நிலையான முழு session_expiry__cmp(கட்டமைப்பு session_expiry_list *i1, கட்டமைப்பு session_expiry_list *i2)
{
	எனில்(i1->context->session_expiry_time == i2->context->session_expiry_time){
		திரும்பு 0;
	}இல்லையெனில் எனில்(i1->context->session_expiry_time > i2->context->session_expiry_time){
		திரும்பு 1;
	}இல்லையெனில்{
		திரும்பு -1;
	}
}


நிலையான வெறுமை set_session_expiry_time(கட்டமைப்பு mosquitto *context)
{
	context->session_expiry_time = db.now_real_s;

	எனில்(db.config->persistent_client_expiration == 0){
		/* No global expiry, so use the client expiration interval */
		context->session_expiry_time += context->session_expiry_interval;
	}இல்லையெனில்{
		/* We have a global expiry interval */
		எனில்(db.config->persistent_client_expiration < context->session_expiry_interval){
			/* The client expiry is longer than the global expiry, so use the global */
			context->session_expiry_time += db.config->persistent_client_expiration;
		}இல்லையெனில்{
			/* The global expiry is longer than the client expiry, so use the client */
			context->session_expiry_time += context->session_expiry_interval;
		}
	}
}


முழு session_expiry__add(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு session_expiry_list *item;

	எனில்(context->expiry_list_item){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	எனில்(db.config->persistent_client_expiration == 0){
		எனில்(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			திரும்பு MOSQ_ERR_SUCCESS;
		}
	}

	item = mosquitto_calloc(1, அளவு(கட்டமைப்பு session_expiry_list));
	எனில்(!item){
		திரும்பு MOSQ_ERR_NOMEM;
	}

	item->context = context;
	set_session_expiry_time(item->context);
	context->expiry_list_item = item;

	DL_செருகு_இடைவரிசை(expiry_list, item, session_expiry__cmp);

	plugin_persist__handle_client_update(context);

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு session_expiry__add_from_persistence(கட்டமைப்பு mosquitto *context, time_t expiry_time)
{
	கட்டமைப்பு session_expiry_list *item;

	எனில்(context->expiry_list_item){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	எனில்(db.config->persistent_client_expiration == 0){
		எனில்(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			திரும்பு MOSQ_ERR_SUCCESS;
		}
	}

	item = mosquitto_calloc(1, அளவு(கட்டமைப்பு session_expiry_list));
	எனில்(!item){
		திரும்பு MOSQ_ERR_NOMEM;
	}

	item->context = context;
	எனில்(expiry_time){
		item->context->session_expiry_time = expiry_time;
	}இல்லையெனில்{
		set_session_expiry_time(item->context);

	}
	context->expiry_list_item = item;

	DL_செருகு_இடைவரிசை(expiry_list, item, session_expiry__cmp);

	திரும்பு MOSQ_ERR_SUCCESS;
}


வெறுமை session_expiry__add_on_shutdown(வெறுமை)
{
	கட்டமைப்பு mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		எனில்(ctxt->bridge == வெற்று
				&& ctxt->is_persisted
				&& ctxt->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				){

			session_expiry__add(ctxt);
		}
	}
}


வெறுமை session_expiry__remove(கட்டமைப்பு mosquitto *context)
{
	எனில்(context->expiry_list_item){
		DL_DELETE(expiry_list, context->expiry_list_item);
		mosquitto_FREE(context->expiry_list_item);
	}
}


/* Call on broker shutdown only */
வெறுமை session_expiry__remove_all(வெறுமை)
{
	கட்டமைப்பு session_expiry_list *item, *tmp;
	கட்டமைப்பு mosquitto *context;

	DL_FOREACH_SAFE(expiry_list, item, tmp){
		context = item->context;
		session_expiry__remove(context);
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
		context->will_delay_interval = 0;
		will_delay__remove(context);
		context__disconnect(context, -1);
	}
}


வெறுமை session_expiry__check(வெறுமை)
{
	கட்டமைப்பு session_expiry_list *item, *tmp;
	கட்டமைப்பு mosquitto *context;
	time_t timeout;

	எனில்(last_check != 0 && db.now_real_s <= last_check){
		எனில்(expiry_list){
			/* Next event is the first item of the list, we must set the timeout even if we aren't
			 * checking the full list */
			timeout = (expiry_list->context->session_expiry_time - db.now_real_s) * 1000;
			எனில்(timeout <= 0){
				timeout = 100;
			}
			loop__update_next_event(timeout);
		}
		திரும்பு;
	}

	last_check = db.now_real_s;

	DL_FOREACH_SAFE(expiry_list, item, tmp){
		எனில்(item->context->session_expiry_time < db.now_real_s){

			context = item->context;
			session_expiry__remove(context);

			எனில்(context->id){
				log__printf(வெற்று, MOSQ_LOG_NOTICE, "Expiring client %s due to timeout.", context->id);
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
		}இல்லையெனில்{
			timeout = (item->context->session_expiry_time - db.now_real_s + 1) * 1000;
			எனில்(timeout <= 0){
				timeout = 100;
			}
			loop__update_next_event(timeout);
			திரும்பு;
		}
	}
}
