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

#అన్తర్భావ "config.h"

#అన్తర్భావ <math.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <utlist.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "sys_tree.h"

స్థిర నిర్మాణం session_expiry_list *expiry_list = ఖాళీ;
స్థిర time_t చివరి_check = 0;


స్థిర పూర్ణాంకం session_expiry__cmp(నిర్మాణం session_expiry_list *i1, నిర్మాణం session_expiry_list *i2)
{
	ఒకవేళ(i1->context->session_expiry_సమయం == i2->context->session_expiry_సమయం){
		తిరిగి 0;
	}లేకపోతే ఒకవేళ(i1->context->session_expiry_సమయం > i2->context->session_expiry_సమయం){
		తిరిగి 1;
	}లేకపోతే{
		తిరిగి -1;
	}
}


స్థిర శూన్య సమితి_session_expiry_సమయం(నిర్మాణం mosquitto *context)
{
	context->session_expiry_సమయం = db.now_real_s;

	ఒకవేళ(db.config->persistent_client_expiration == 0){
		/* No global expiry, so use the client expiration interval */
		context->session_expiry_సమయం += context->session_expiry_interval;
	}లేకపోతే{
		/* We have a global expiry interval */
		ఒకవేళ(db.config->persistent_client_expiration < context->session_expiry_interval){
			/* The client expiry is longer than the global expiry, so use the global */
			context->session_expiry_సమయం += db.config->persistent_client_expiration;
		}లేకపోతే{
			/* The global expiry is longer than the client expiry, so use the client */
			context->session_expiry_సమయం += context->session_expiry_interval;
		}
	}
}


పూర్ణాంకం session_expiry_కూడు(నిర్మాణం mosquitto *context)
{
	నిర్మాణం session_expiry_list *అంశం;

	ఒకవేళ(context->expiry_list_అంశం){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	ఒకవేళ(db.config->persistent_client_expiration == 0){
		ఒకవేళ(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			తిరిగి MOSQ_ERR_SUCCESS;
		}
	}

	అంశం = mosquitto_calloc(1, పరిమాణం(నిర్మాణం session_expiry_list));
	ఒకవేళ(!అంశం){
		తిరిగి MOSQ_ERR_NOMEM;
	}

	అంశం->context = context;
	సమితి_session_expiry_సమయం(అంశం->context);
	context->expiry_list_అంశం = అంశం;

	DL_చొప్పించు_మధ్యక్రమం(expiry_list, అంశం, session_expiry__cmp);

	plugin_persist__handle_client_update(context);

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం session_expiry_కూడు_from_persistence(నిర్మాణం mosquitto *context, time_t expiry_సమయం)
{
	నిర్మాణం session_expiry_list *అంశం;

	ఒకవేళ(context->expiry_list_అంశం){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	ఒకవేళ(db.config->persistent_client_expiration == 0){
		ఒకవేళ(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			తిరిగి MOSQ_ERR_SUCCESS;
		}
	}

	అంశం = mosquitto_calloc(1, పరిమాణం(నిర్మాణం session_expiry_list));
	ఒకవేళ(!అంశం){
		తిరిగి MOSQ_ERR_NOMEM;
	}

	అంశం->context = context;
	ఒకవేళ(expiry_సమయం){
		అంశం->context->session_expiry_సమయం = expiry_సమయం;
	}లేకపోతే{
		సమితి_session_expiry_సమయం(అంశం->context);

	}
	context->expiry_list_అంశం = అంశం;

	DL_చొప్పించు_మధ్యక్రమం(expiry_list, అంశం, session_expiry__cmp);

	తిరిగి MOSQ_ERR_SUCCESS;
}


శూన్య session_expiry_కూడు_on_shutdown(శూన్య)
{
	నిర్మాణం mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		ఒకవేళ(ctxt->bridge == ఖాళీ
				&& ctxt->is_persisted
				&& ctxt->session_expiry_interval != MQTT_SESSION_EXPIRY_తక్షణ
				){

			session_expiry_కూడు(ctxt);
		}
	}
}


శూన్య session_expiry__remove(నిర్మాణం mosquitto *context)
{
	ఒకవేళ(context->expiry_list_అంశం){
		DL_DELETE(expiry_list, context->expiry_list_అంశం);
		mosquitto_FREE(context->expiry_list_అంశం);
	}
}


/* Call on broker shutdown only */
శూన్య session_expiry__remove_all(శూన్య)
{
	నిర్మాణం session_expiry_list *అంశం, *tmp;
	నిర్మాణం mosquitto *context;

	DL_FOREACH_సురక్షిత(expiry_list, అంశం, tmp){
		context = అంశం->context;
		session_expiry__remove(context);
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_తక్షణ;
		context->will_delay_interval = 0;
		will_delay__remove(context);
		context__disconnect(context, -1);
	}
}


శూన్య session_expiry__check(శూన్య)
{
	నిర్మాణం session_expiry_list *అంశం, *tmp;
	నిర్మాణం mosquitto *context;
	time_t timeout;

	ఒకవేళ(చివరి_check != 0 && db.now_real_s <= చివరి_check){
		ఒకవేళ(expiry_list){
			/* Next event is the first item of the list, we must set the timeout even if we aren't
			 * checking the full list */
			timeout = (expiry_list->context->session_expiry_సమయం - db.now_real_s) * 1000;
			ఒకవేళ(timeout <= 0){
				timeout = 100;
			}
			loop_update_తదుపరి_event(timeout);
		}
		తిరిగి;
	}

	చివరి_check = db.now_real_s;

	DL_FOREACH_సురక్షిత(expiry_list, అంశం, tmp){
		ఒకవేళ(అంశం->context->session_expiry_సమయం < db.now_real_s){

			context = అంశం->context;
			session_expiry__remove(context);

			ఒకవేళ(context->id){
				log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Expiring client %s due to timeout.", context->id);
			}
			metrics__int_inc(mosq_counter_clients_expired, 1);

			/* Session has now expired, so clear interval */
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_తక్షణ;
			/* Session has expired, so will delay should be cleared. */
			context->will_delay_interval = 0;
			will_delay__remove(context);
			context__send_will(context);
			plugin_persist__handle_client_delete(context);
			context_కూడు_గమ్యం_disused(context);
		}లేకపోతే{
			timeout = (అంశం->context->session_expiry_సమయం - db.now_real_s + 1) * 1000;
			ఒకవేళ(timeout <= 0){
				timeout = 100;
			}
			loop_update_తదుపరి_event(timeout);
			తిరిగి;
		}
	}
}
