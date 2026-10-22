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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <utlist.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sys_tree.h"

ਸਥਿਰ ਬਣਤਰ session_expiry_list *expiry_list = ਨਿਰਾ;
ਸਥਿਰ time_t ਆਖਰੀ_check = 0;


ਸਥਿਰ ਪੂਰਨ session_expiry__cmp(ਬਣਤਰ session_expiry_list *i1, ਬਣਤਰ session_expiry_list *i2)
{
	ਜੇ(i1->context->session_expiry_ਸਮਾਂ == i2->context->session_expiry_ਸਮਾਂ){
		ਵਾਪਸ 0;
	}ਨਹੀਂਤਾਂ ਜੇ(i1->context->session_expiry_ਸਮਾਂ > i2->context->session_expiry_ਸਮਾਂ){
		ਵਾਪਸ 1;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ -1;
	}
}


ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_session_expiry_ਸਮਾਂ(ਬਣਤਰ mosquitto *context)
{
	context->session_expiry_ਸਮਾਂ = db.now_real_s;

	ਜੇ(db.config->persistent_client_expiration == 0){
		/* No global expiry, so use the client expiration interval */
		context->session_expiry_ਸਮਾਂ += context->session_expiry_interval;
	}ਨਹੀਂਤਾਂ{
		/* We have a global expiry interval */
		ਜੇ(db.config->persistent_client_expiration < context->session_expiry_interval){
			/* The client expiry is longer than the global expiry, so use the global */
			context->session_expiry_ਸਮਾਂ += db.config->persistent_client_expiration;
		}ਨਹੀਂਤਾਂ{
			/* The global expiry is longer than the client expiry, so use the client */
			context->session_expiry_ਸਮਾਂ += context->session_expiry_interval;
		}
	}
}


ਪੂਰਨ session_expiry_ਜੋੜੋ(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ session_expiry_list *ਵਸਤੂ;

	ਜੇ(context->expiry_list_ਵਸਤੂ){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	ਜੇ(db.config->persistent_client_expiration == 0){
		ਜੇ(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}
	}

	ਵਸਤੂ = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ session_expiry_list));
	ਜੇ(!ਵਸਤੂ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	ਵਸਤੂ->context = context;
	ਸੈੱਟ_session_expiry_ਸਮਾਂ(ਵਸਤੂ->context);
	context->expiry_list_ਵਸਤੂ = ਵਸਤੂ;

	DL_ਪਾਓ_ਮੱਧਕ੍ਰਮ(expiry_list, ਵਸਤੂ, session_expiry__cmp);

	plugin_persist__handle_client_update(context);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ session_expiry_ਜੋੜੋ_from_persistence(ਬਣਤਰ mosquitto *context, time_t expiry_ਸਮਾਂ)
{
	ਬਣਤਰ session_expiry_list *ਵਸਤੂ;

	ਜੇ(context->expiry_list_ਵਸਤੂ){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	ਜੇ(db.config->persistent_client_expiration == 0){
		ਜੇ(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}
	}

	ਵਸਤੂ = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ session_expiry_list));
	ਜੇ(!ਵਸਤੂ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	ਵਸਤੂ->context = context;
	ਜੇ(expiry_ਸਮਾਂ){
		ਵਸਤੂ->context->session_expiry_ਸਮਾਂ = expiry_ਸਮਾਂ;
	}ਨਹੀਂਤਾਂ{
		ਸੈੱਟ_session_expiry_ਸਮਾਂ(ਵਸਤੂ->context);

	}
	context->expiry_list_ਵਸਤੂ = ਵਸਤੂ;

	DL_ਪਾਓ_ਮੱਧਕ੍ਰਮ(expiry_list, ਵਸਤੂ, session_expiry__cmp);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਖਾਲੀ session_expiry_ਜੋੜੋ_on_shutdown(ਖਾਲੀ)
{
	ਬਣਤਰ mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		ਜੇ(ctxt->bridge == ਨਿਰਾ
				&& ctxt->is_persisted
				&& ctxt->session_expiry_interval != MQTT_SESSION_EXPIRY_ਤੁਰੰਤ
				){

			session_expiry_ਜੋੜੋ(ctxt);
		}
	}
}


ਖਾਲੀ session_expiry__remove(ਬਣਤਰ mosquitto *context)
{
	ਜੇ(context->expiry_list_ਵਸਤੂ){
		DL_DELETE(expiry_list, context->expiry_list_ਵਸਤੂ);
		mosquitto_FREE(context->expiry_list_ਵਸਤੂ);
	}
}


/* Call on broker shutdown only */
ਖਾਲੀ session_expiry__remove_all(ਖਾਲੀ)
{
	ਬਣਤਰ session_expiry_list *ਵਸਤੂ, *tmp;
	ਬਣਤਰ mosquitto *context;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(expiry_list, ਵਸਤੂ, tmp){
		context = ਵਸਤੂ->context;
		session_expiry__remove(context);
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_ਤੁਰੰਤ;
		context->will_delay_interval = 0;
		will_delay__remove(context);
		context__disconnect(context, -1);
	}
}


ਖਾਲੀ session_expiry__check(ਖਾਲੀ)
{
	ਬਣਤਰ session_expiry_list *ਵਸਤੂ, *tmp;
	ਬਣਤਰ mosquitto *context;
	time_t timeout;

	ਜੇ(ਆਖਰੀ_check != 0 && db.now_real_s <= ਆਖਰੀ_check){
		ਜੇ(expiry_list){
			/* Next event is the first item of the list, we must set the timeout even if we aren't
			 * checking the full list */
			timeout = (expiry_list->context->session_expiry_ਸਮਾਂ - db.now_real_s) * 1000;
			ਜੇ(timeout <= 0){
				timeout = 100;
			}
			loop_update_ਅਗਲਾ_event(timeout);
		}
		ਵਾਪਸ;
	}

	ਆਖਰੀ_check = db.now_real_s;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(expiry_list, ਵਸਤੂ, tmp){
		ਜੇ(ਵਸਤੂ->context->session_expiry_ਸਮਾਂ < db.now_real_s){

			context = ਵਸਤੂ->context;
			session_expiry__remove(context);

			ਜੇ(context->id){
				log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Expiring client %s due to timeout.", context->id);
			}
			metrics__int_inc(mosq_counter_clients_expired, 1);

			/* Session has now expired, so clear interval */
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_ਤੁਰੰਤ;
			/* Session has expired, so will delay should be cleared. */
			context->will_delay_interval = 0;
			will_delay__remove(context);
			context__send_will(context);
			plugin_persist__handle_client_delete(context);
			context_ਜੋੜੋ_ਮੰਜ਼ਿਲ_disused(context);
		}ਨਹੀਂਤਾਂ{
			timeout = (ਵਸਤੂ->context->session_expiry_ਸਮਾਂ - db.now_real_s + 1) * 1000;
			ਜੇ(timeout <= 0){
				timeout = 100;
			}
			loop_update_ਅਗਲਾ_event(timeout);
			ਵਾਪਸ;
		}
	}
}
