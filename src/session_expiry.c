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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <math.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <utlist.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "sys_tree.h"

સ્થિર સંરચના session_expiry_list *expiry_list = ખાલી;
સ્થિર time_t last_check = 0;


સ્થિર પૂર્ણાંક session_expiry__cmp(સંરચના session_expiry_list *i1, સંરચના session_expiry_list *i2)
{
	જો(i1->context->session_expiry_time == i2->context->session_expiry_time){
		પાછા 0;
	}નહીંતર જો(i1->context->session_expiry_time > i2->context->session_expiry_time){
		પાછા 1;
	}નહીંતર{
		પાછા -1;
	}
}


સ્થિર શૂન્ય set_session_expiry_time(સંરચના mosquitto *context)
{
	context->session_expiry_time = db.now_real_s;

	જો(db.config->persistent_client_expiration == 0){
		/* No global expiry, so use the client expiration interval */
		context->session_expiry_time += context->session_expiry_interval;
	}નહીંતર{
		/* We have a global expiry interval */
		જો(db.config->persistent_client_expiration < context->session_expiry_interval){
			/* The client expiry is longer than the global expiry, so use the global */
			context->session_expiry_time += db.config->persistent_client_expiration;
		}નહીંતર{
			/* The global expiry is longer than the client expiry, so use the client */
			context->session_expiry_time += context->session_expiry_interval;
		}
	}
}


પૂર્ણાંક session_expiry__add(સંરચના mosquitto *context)
{
	સંરચના session_expiry_list *item;

	જો(context->expiry_list_item){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		પાછા MOSQ_ERR_SUCCESS;
	}

	જો(db.config->persistent_client_expiration == 0){
		જો(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			પાછા MOSQ_ERR_SUCCESS;
		}
	}

	item = mosquitto_calloc(1, કદ(સંરચના session_expiry_list));
	જો(!item){
		પાછા MOSQ_ERR_NOMEM;
	}

	item->context = context;
	set_session_expiry_time(item->context);
	context->expiry_list_item = item;

	DL_ઉમેરો_મધ્યક્રમ(expiry_list, item, session_expiry__cmp);

	plugin_persist__handle_client_update(context);

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક session_expiry__add_from_persistence(સંરચના mosquitto *context, time_t expiry_time)
{
	સંરચના session_expiry_list *item;

	જો(context->expiry_list_item){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		પાછા MOSQ_ERR_SUCCESS;
	}

	જો(db.config->persistent_client_expiration == 0){
		જો(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			પાછા MOSQ_ERR_SUCCESS;
		}
	}

	item = mosquitto_calloc(1, કદ(સંરચના session_expiry_list));
	જો(!item){
		પાછા MOSQ_ERR_NOMEM;
	}

	item->context = context;
	જો(expiry_time){
		item->context->session_expiry_time = expiry_time;
	}નહીંતર{
		set_session_expiry_time(item->context);

	}
	context->expiry_list_item = item;

	DL_ઉમેરો_મધ્યક્રમ(expiry_list, item, session_expiry__cmp);

	પાછા MOSQ_ERR_SUCCESS;
}


શૂન્ય session_expiry__add_on_shutdown(શૂન્ય)
{
	સંરચના mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		જો(ctxt->bridge == ખાલી
				&& ctxt->is_persisted
				&& ctxt->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				){

			session_expiry__add(ctxt);
		}
	}
}


શૂન્ય session_expiry__remove(સંરચના mosquitto *context)
{
	જો(context->expiry_list_item){
		DL_DELETE(expiry_list, context->expiry_list_item);
		mosquitto_FREE(context->expiry_list_item);
	}
}


/* Call on broker shutdown only */
શૂન્ય session_expiry__remove_all(શૂન્ય)
{
	સંરચના session_expiry_list *item, *tmp;
	સંરચના mosquitto *context;

	DL_FOREACH_SAFE(expiry_list, item, tmp){
		context = item->context;
		session_expiry__remove(context);
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
		context->will_delay_interval = 0;
		will_delay__remove(context);
		context__disconnect(context, -1);
	}
}


શૂન્ય session_expiry__check(શૂન્ય)
{
	સંરચના session_expiry_list *item, *tmp;
	સંરચના mosquitto *context;
	time_t timeout;

	જો(last_check != 0 && db.now_real_s <= last_check){
		જો(expiry_list){
			/* Next event is the first item of the list, we must set the timeout even if we aren't
			 * checking the full list */
			timeout = (expiry_list->context->session_expiry_time - db.now_real_s) * 1000;
			જો(timeout <= 0){
				timeout = 100;
			}
			loop__update_next_event(timeout);
		}
		પાછા;
	}

	last_check = db.now_real_s;

	DL_FOREACH_SAFE(expiry_list, item, tmp){
		જો(item->context->session_expiry_time < db.now_real_s){

			context = item->context;
			session_expiry__remove(context);

			જો(context->id){
				log__printf(ખાલી, MOSQ_LOG_NOTICE, "Expiring client %s due to timeout.", context->id);
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
		}નહીંતર{
			timeout = (item->context->session_expiry_time - db.now_real_s + 1) * 1000;
			જો(timeout <= 0){
				timeout = 100;
			}
			loop__update_next_event(timeout);
			પાછા;
		}
	}
}
