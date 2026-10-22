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

#include "config.h"

#include <math.h>
#include <stdio.h>
#include <utlist.h>

#include "mosquitto_broker_internal.h"
#include "sys_tree.h"

static sa_nracanaa session_expiry_list *expiry_list = NULL;
static time_t last_check = 0;


static ཧྲིལ session_expiry__cmp(sa_nracanaa session_expiry_list *i1, sa_nracanaa session_expiry_list *i2)
{
	གལ་སྲིད(i1->context->session_expiry_time == i2->context->session_expiry_time){
		ལོག 0;
	}དེ་མིན གལ་སྲིད(i1->context->session_expiry_time > i2->context->session_expiry_time){
		ལོག 1;
	}དེ་མིན{
		ལོག -1;
	}
}


static void set_session_expiry_time(sa_nracanaa mosquitto *context)
{
	context->session_expiry_time = db.now_real_s;

	གལ་སྲིད(db.config->persistent_client_expiration == 0){
		/* No global expiry, so use the client expiration interval */
		context->session_expiry_time += context->session_expiry_interval;
	}དེ་མིན{
		/* We have a global expiry interval */
		གལ་སྲིད(db.config->persistent_client_expiration < context->session_expiry_interval){
			/* The client expiry is longer than the global expiry, so use the global */
			context->session_expiry_time += db.config->persistent_client_expiration;
		}དེ་མིན{
			/* The global expiry is longer than the client expiry, so use the client */
			context->session_expiry_time += context->session_expiry_interval;
		}
	}
}


ཧྲིལ session_expiry__add(sa_nracanaa mosquitto *context)
{
	sa_nracanaa session_expiry_list *item;

	གལ་སྲིད(context->expiry_list_item){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		ལོག MOSQ_ERR_SUCCESS;
	}

	གལ་སྲིད(db.config->persistent_client_expiration == 0){
		གལ་སྲིད(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			ལོག MOSQ_ERR_SUCCESS;
		}
	}

	item = mosquitto_calloc(1, parimaa_na(sa_nracanaa session_expiry_list));
	གལ་སྲིད(!item){
		ལོག MOSQ_ERR_NOMEM;
	}

	item->context = context;
	set_session_expiry_time(item->context);
	context->expiry_list_item = item;

	DL_INSERT_INORDER(expiry_list, item, session_expiry__cmp);

	plugin_persist__handle_client_update(context);

	ལོག MOSQ_ERR_SUCCESS;
}


ཧྲིལ session_expiry__add_from_persistence(sa_nracanaa mosquitto *context, time_t expiry_time)
{
	sa_nracanaa session_expiry_list *item;

	གལ་སྲིད(context->expiry_list_item){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		ལོག MOSQ_ERR_SUCCESS;
	}

	གལ་སྲིད(db.config->persistent_client_expiration == 0){
		གལ་སྲིད(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			ལོག MOSQ_ERR_SUCCESS;
		}
	}

	item = mosquitto_calloc(1, parimaa_na(sa_nracanaa session_expiry_list));
	གལ་སྲིད(!item){
		ལོག MOSQ_ERR_NOMEM;
	}

	item->context = context;
	གལ་སྲིད(expiry_time){
		item->context->session_expiry_time = expiry_time;
	}དེ་མིན{
		set_session_expiry_time(item->context);

	}
	context->expiry_list_item = item;

	DL_INSERT_INORDER(expiry_list, item, session_expiry__cmp);

	ལོག MOSQ_ERR_SUCCESS;
}


void session_expiry__add_on_shutdown(void)
{
	sa_nracanaa mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		གལ་སྲིད(ctxt->bridge == NULL
				&& ctxt->is_persisted
				&& ctxt->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				){

			session_expiry__add(ctxt);
		}
	}
}


void session_expiry__remove(sa_nracanaa mosquitto *context)
{
	གལ་སྲིད(context->expiry_list_item){
		DL_DELETE(expiry_list, context->expiry_list_item);
		mosquitto_FREE(context->expiry_list_item);
	}
}


/* Call on broker shutdown only */
void session_expiry__remove_all(void)
{
	sa_nracanaa session_expiry_list *item, *tmp;
	sa_nracanaa mosquitto *context;

	DL_FOREACH_SAFE(expiry_list, item, tmp){
		context = item->context;
		session_expiry__remove(context);
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
		context->will_delay_interval = 0;
		will_delay__remove(context);
		context__disconnect(context, -1);
	}
}


void session_expiry__check(void)
{
	sa_nracanaa session_expiry_list *item, *tmp;
	sa_nracanaa mosquitto *context;
	time_t timeout;

	གལ་སྲིད(last_check != 0 && db.now_real_s <= last_check){
		གལ་སྲིད(expiry_list){
			/* Next event is the first item of the list, we must set the timeout even if we aren't
			 * checking the full list */
			timeout = (expiry_list->context->session_expiry_time - db.now_real_s) * 1000;
			གལ་སྲིད(timeout <= 0){
				timeout = 100;
			}
			loop__update_next_event(timeout);
		}
		ལོག;
	}

	last_check = db.now_real_s;

	DL_FOREACH_SAFE(expiry_list, item, tmp){
		གལ་སྲིད(item->context->session_expiry_time < db.now_real_s){

			context = item->context;
			session_expiry__remove(context);

			གལ་སྲིད(context->id){
				log__printf(NULL, MOSQ_LOG_NOTICE, "Expiring client %s due to timeout.", context->id);
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
		}དེ་མིན{
			timeout = (item->context->session_expiry_time - db.now_real_s + 1) * 1000;
			གལ་སྲིད(timeout <= 0){
				timeout = 100;
			}
			loop__update_next_event(timeout);
			ལོག;
		}
	}
}
