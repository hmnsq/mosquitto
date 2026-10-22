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
#ضمن "sys_tree.h"

ساكن بنية session_expiry_list *expiry_list = عدم;
ساكن time_t last_check = 0;


ساكن صحيح session_expiry__cmp(بنية session_expiry_list *i1, بنية session_expiry_list *i2)
{
	אם(i1->context->session_expiry_time == i2->context->session_expiry_time){
		החזר 0;
	}אחרת אם(i1->context->session_expiry_time > i2->context->session_expiry_time){
		החזר 1;
	}אחרת{
		החזר -1;
	}
}


ساكن فراغ set_session_expiry_time(بنية mosquitto *context)
{
	context->session_expiry_time = db.now_real_s;

	אם(db.config->persistent_client_expiration == 0){
		/* No global expiry, so use the client expiration interval */
		context->session_expiry_time += context->session_expiry_interval;
	}אחרת{
		/* We have a global expiry interval */
		אם(db.config->persistent_client_expiration < context->session_expiry_interval){
			/* The client expiry is longer than the global expiry, so use the global */
			context->session_expiry_time += db.config->persistent_client_expiration;
		}אחרת{
			/* The global expiry is longer than the client expiry, so use the client */
			context->session_expiry_time += context->session_expiry_interval;
		}
	}
}


صحيح session_expiry__add(بنية mosquitto *context)
{
	بنية session_expiry_list *item;

	אם(context->expiry_list_item){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		החזר MOSQ_ERR_SUCCESS;
	}

	אם(db.config->persistent_client_expiration == 0){
		אם(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			החזר MOSQ_ERR_SUCCESS;
		}
	}

	item = mosquitto_calloc(1, حجم(بنية session_expiry_list));
	אם(!item){
		החזר MOSQ_ERR_NOMEM;
	}

	item->context = context;
	set_session_expiry_time(item->context);
	context->expiry_list_item = item;

	DL_INSERT_INORDER(expiry_list, item, session_expiry__cmp);

	plugin_persist__handle_client_update(context);

	החזר MOSQ_ERR_SUCCESS;
}


صحيح session_expiry__add_from_persistence(بنية mosquitto *context, time_t expiry_time)
{
	بنية session_expiry_list *item;

	אם(context->expiry_list_item){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		החזר MOSQ_ERR_SUCCESS;
	}

	אם(db.config->persistent_client_expiration == 0){
		אם(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			החזר MOSQ_ERR_SUCCESS;
		}
	}

	item = mosquitto_calloc(1, حجم(بنية session_expiry_list));
	אם(!item){
		החזר MOSQ_ERR_NOMEM;
	}

	item->context = context;
	אם(expiry_time){
		item->context->session_expiry_time = expiry_time;
	}אחרת{
		set_session_expiry_time(item->context);

	}
	context->expiry_list_item = item;

	DL_INSERT_INORDER(expiry_list, item, session_expiry__cmp);

	החזר MOSQ_ERR_SUCCESS;
}


فراغ session_expiry__add_on_shutdown(فراغ)
{
	بنية mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		אם(ctxt->bridge == عدم
				&& ctxt->is_persisted
				&& ctxt->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				){

			session_expiry__add(ctxt);
		}
	}
}


فراغ session_expiry__remove(بنية mosquitto *context)
{
	אם(context->expiry_list_item){
		DL_DELETE(expiry_list, context->expiry_list_item);
		mosquitto_FREE(context->expiry_list_item);
	}
}


/* Call on broker shutdown only */
فراغ session_expiry__remove_all(فراغ)
{
	بنية session_expiry_list *item, *tmp;
	بنية mosquitto *context;

	DL_FOREACH_SAFE(expiry_list, item, tmp){
		context = item->context;
		session_expiry__remove(context);
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
		context->will_delay_interval = 0;
		will_delay__remove(context);
		context__disconnect(context, -1);
	}
}


فراغ session_expiry__check(فراغ)
{
	بنية session_expiry_list *item, *tmp;
	بنية mosquitto *context;
	time_t timeout;

	אם(last_check != 0 && db.now_real_s <= last_check){
		אם(expiry_list){
			/* Next event is the first item of the list, we must set the timeout even if we aren't
			 * checking the full list */
			timeout = (expiry_list->context->session_expiry_time - db.now_real_s) * 1000;
			אם(timeout <= 0){
				timeout = 100;
			}
			loop__update_next_event(timeout);
		}
		החזר;
	}

	last_check = db.now_real_s;

	DL_FOREACH_SAFE(expiry_list, item, tmp){
		אם(item->context->session_expiry_time < db.now_real_s){

			context = item->context;
			session_expiry__remove(context);

			אם(context->id){
				log__printf(عدم, MOSQ_LOG_NOTICE, "Expiring client %s due to timeout.", context->id);
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
		}אחרת{
			timeout = (item->context->session_expiry_time - db.now_real_s + 1) * 1000;
			אם(timeout <= 0){
				timeout = 100;
			}
			loop__update_next_event(timeout);
			החזר;
		}
	}
}
