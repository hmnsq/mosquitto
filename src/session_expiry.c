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
ساكن time_t وروستی_check = 0;


ساكن صحيح session_expiry__cmp(بنية session_expiry_list *i1, بنية session_expiry_list *i2)
{
	إذا(i1->context->session_expiry_وخت == i2->context->session_expiry_وخت){
		أرجع 0;
	}وإلا إذا(i1->context->session_expiry_وخت > i2->context->session_expiry_وخت){
		أرجع 1;
	}وإلا{
		أرجع -1;
	}
}


ساكن فراغ سېټ_session_expiry_وخت(بنية mosquitto *context)
{
	context->session_expiry_وخت = db.now_real_s;

	إذا(db.config->persistent_client_expiration == 0){
		/* No global expiry, so use the client expiration interval */
		context->session_expiry_وخت += context->session_expiry_interval;
	}وإلا{
		/* We have a global expiry interval */
		إذا(db.config->persistent_client_expiration < context->session_expiry_interval){
			/* The client expiry is longer than the global expiry, so use the global */
			context->session_expiry_وخت += db.config->persistent_client_expiration;
		}وإلا{
			/* The global expiry is longer than the client expiry, so use the client */
			context->session_expiry_وخت += context->session_expiry_interval;
		}
	}
}


صحيح session_expiry_جمع_کړه(بنية mosquitto *context)
{
	بنية session_expiry_list *توکی;

	إذا(context->expiry_list_توکی){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		أرجع MOSQ_ERR_SUCCESS;
	}

	إذا(db.config->persistent_client_expiration == 0){
		إذا(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	توکی = mosquitto_calloc(1, حجم(بنية session_expiry_list));
	إذا(!توکی){
		أرجع MOSQ_ERR_NOMEM;
	}

	توکی->context = context;
	سېټ_session_expiry_وخت(توکی->context);
	context->expiry_list_توکی = توکی;

	DL_ورننباسه_منځنی_ترتیب(expiry_list, توکی, session_expiry__cmp);

	plugin_persist__handle_client_update(context);

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح session_expiry_جمع_کړه_from_persistence(بنية mosquitto *context, time_t expiry_وخت)
{
	بنية session_expiry_list *توکی;

	إذا(context->expiry_list_توکی){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		أرجع MOSQ_ERR_SUCCESS;
	}

	إذا(db.config->persistent_client_expiration == 0){
		إذا(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	توکی = mosquitto_calloc(1, حجم(بنية session_expiry_list));
	إذا(!توکی){
		أرجع MOSQ_ERR_NOMEM;
	}

	توکی->context = context;
	إذا(expiry_وخت){
		توکی->context->session_expiry_وخت = expiry_وخت;
	}وإلا{
		سېټ_session_expiry_وخت(توکی->context);

	}
	context->expiry_list_توکی = توکی;

	DL_ورننباسه_منځنی_ترتیب(expiry_list, توکی, session_expiry__cmp);

	أرجع MOSQ_ERR_SUCCESS;
}


فراغ session_expiry_جمع_کړه_on_shutdown(فراغ)
{
	بنية mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		إذا(ctxt->bridge == عدم
				&& ctxt->is_persisted
				&& ctxt->session_expiry_interval != MQTT_SESSION_EXPIRY_سمدستي
				){

			session_expiry_جمع_کړه(ctxt);
		}
	}
}


فراغ session_expiry__remove(بنية mosquitto *context)
{
	إذا(context->expiry_list_توکی){
		DL_DELETE(expiry_list, context->expiry_list_توکی);
		mosquitto_FREE(context->expiry_list_توکی);
	}
}


/* Call on broker shutdown only */
فراغ session_expiry__remove_all(فراغ)
{
	بنية session_expiry_list *توکی, *tmp;
	بنية mosquitto *context;

	DL_FOREACH_خوندي(expiry_list, توکی, tmp){
		context = توکی->context;
		session_expiry__remove(context);
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_سمدستي;
		context->will_delay_interval = 0;
		will_delay__remove(context);
		context__disconnect(context, -1);
	}
}


فراغ session_expiry__check(فراغ)
{
	بنية session_expiry_list *توکی, *tmp;
	بنية mosquitto *context;
	time_t timeout;

	إذا(وروستی_check != 0 && db.now_real_s <= وروستی_check){
		إذا(expiry_list){
			/* Next event is the first item of the list, we must set the timeout even if we aren't
			 * checking the full list */
			timeout = (expiry_list->context->session_expiry_وخت - db.now_real_s) * 1000;
			إذا(timeout <= 0){
				timeout = 100;
			}
			loop_update_بل_event(timeout);
		}
		أرجع;
	}

	وروستی_check = db.now_real_s;

	DL_FOREACH_خوندي(expiry_list, توکی, tmp){
		إذا(توکی->context->session_expiry_وخت < db.now_real_s){

			context = توکی->context;
			session_expiry__remove(context);

			إذا(context->id){
				log__printf(عدم, MOSQ_LOG_NOTICE, "Expiring client %s due to timeout.", context->id);
			}
			metrics__int_inc(mosq_counter_clients_expired, 1);

			/* Session has now expired, so clear interval */
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_سمدستي;
			/* Session has expired, so will delay should be cleared. */
			context->will_delay_interval = 0;
			will_delay__remove(context);
			context__send_will(context);
			plugin_persist__handle_client_delete(context);
			context_جمع_کړه_منزل_disused(context);
		}وإلا{
			timeout = (توکی->context->session_expiry_وخت - db.now_real_s + 1) * 1000;
			إذا(timeout <= 0){
				timeout = 100;
			}
			loop_update_بل_event(timeout);
			أرجع;
		}
	}
}
