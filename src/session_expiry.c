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

#समावेश "config.h"

#समावेश <math.h>
#समावेश <stdio.h>
#समावेश <utlist.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "sys_tree.h"

अटल काष्ठा session_expiry_list *expiry_list = शून्य;
अटल समय_प्रकार last_check = 0;


अटल पूर्णांक session_expiry__cmp(काष्ठा session_expiry_list *i1, काष्ठा session_expiry_list *i2)
{
	अगर(i1->context->session_expiry_time == i2->context->session_expiry_time){
		वापस 0;
	}अन्यथा अगर(i1->context->session_expiry_time > i2->context->session_expiry_time){
		वापस 1;
	}अन्यथा{
		वापस -1;
	}
}


अटल व्योम set_session_expiry_time(काष्ठा mosquitto *context)
{
	context->session_expiry_time = db.now_real_s;

	अगर(db.config->persistent_client_expiration == 0){
		/* No global expiry, so use the client expiration interval */
		context->session_expiry_time += context->session_expiry_interval;
	}अन्यथा{
		/* We have a global expiry interval */
		अगर(db.config->persistent_client_expiration < context->session_expiry_interval){
			/* The client expiry is longer than the global expiry, so use the global */
			context->session_expiry_time += db.config->persistent_client_expiration;
		}अन्यथा{
			/* The global expiry is longer than the client expiry, so use the client */
			context->session_expiry_time += context->session_expiry_interval;
		}
	}
}


पूर्णांक session_expiry__add(काष्ठा mosquitto *context)
{
	काष्ठा session_expiry_list *item;

	अगर(context->expiry_list_item){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		वापस MOSQ_ERR_SUCCESS;
	}

	अगर(db.config->persistent_client_expiration == 0){
		अगर(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			वापस MOSQ_ERR_SUCCESS;
		}
	}

	item = mosquitto_calloc(1, माप(काष्ठा session_expiry_list));
	अगर(!item){
		वापस MOSQ_ERR_NOMEM;
	}

	item->context = context;
	set_session_expiry_time(item->context);
	context->expiry_list_item = item;

	DL_जोड़ो_मध्यक्रम(expiry_list, item, session_expiry__cmp);

	plugin_persist__handle_client_update(context);

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक session_expiry__add_from_persistence(काष्ठा mosquitto *context, समय_प्रकार expiry_time)
{
	काष्ठा session_expiry_list *item;

	अगर(context->expiry_list_item){
		/* Already added, may happen in cases like a client disconnecting then
		 * being kicked by a plugin. */
		वापस MOSQ_ERR_SUCCESS;
	}

	अगर(db.config->persistent_client_expiration == 0){
		अगर(context->session_expiry_interval == MQTT_SESSION_EXPIRY_NEVER){
			/* There isn't a global expiry set, and the client has asked to
			 * never expire, so we don't add it to the list. */
			वापस MOSQ_ERR_SUCCESS;
		}
	}

	item = mosquitto_calloc(1, माप(काष्ठा session_expiry_list));
	अगर(!item){
		वापस MOSQ_ERR_NOMEM;
	}

	item->context = context;
	अगर(expiry_time){
		item->context->session_expiry_time = expiry_time;
	}अन्यथा{
		set_session_expiry_time(item->context);

	}
	context->expiry_list_item = item;

	DL_जोड़ो_मध्यक्रम(expiry_list, item, session_expiry__cmp);

	वापस MOSQ_ERR_SUCCESS;
}


व्योम session_expiry__add_on_shutdown(व्योम)
{
	काष्ठा mosquitto *ctxt, *ctxt_tmp;

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		अगर(ctxt->bridge == शून्य
				&& ctxt->is_persisted
				&& ctxt->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				){

			session_expiry__add(ctxt);
		}
	}
}


व्योम session_expiry__remove(काष्ठा mosquitto *context)
{
	अगर(context->expiry_list_item){
		DL_DELETE(expiry_list, context->expiry_list_item);
		mosquitto_FREE(context->expiry_list_item);
	}
}


/* Call on broker shutdown only */
व्योम session_expiry__remove_all(व्योम)
{
	काष्ठा session_expiry_list *item, *tmp;
	काष्ठा mosquitto *context;

	DL_FOREACH_SAFE(expiry_list, item, tmp){
		context = item->context;
		session_expiry__remove(context);
		context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
		context->will_delay_interval = 0;
		will_delay__remove(context);
		context__disconnect(context, -1);
	}
}


व्योम session_expiry__check(व्योम)
{
	काष्ठा session_expiry_list *item, *tmp;
	काष्ठा mosquitto *context;
	समय_प्रकार timeout;

	अगर(last_check != 0 && db.now_real_s <= last_check){
		अगर(expiry_list){
			/* Next event is the first item of the list, we must set the timeout even if we aren't
			 * checking the full list */
			timeout = (expiry_list->context->session_expiry_time - db.now_real_s) * 1000;
			अगर(timeout <= 0){
				timeout = 100;
			}
			loop_update_अग्र_event(timeout);
		}
		वापस;
	}

	last_check = db.now_real_s;

	DL_FOREACH_SAFE(expiry_list, item, tmp){
		अगर(item->context->session_expiry_time < db.now_real_s){

			context = item->context;
			session_expiry__remove(context);

			अगर(context->id){
				log__printf(शून्य, MOSQ_LOG_NOTICE, "Expiring client %s due to timeout.", context->id);
			}
			metrics__int_inc(mosq_गणक_clients_expired, 1);

			/* Session has now expired, so clear interval */
			context->session_expiry_interval = MQTT_SESSION_EXPIRY_IMMEDIATE;
			/* Session has expired, so will delay should be cleared. */
			context->will_delay_interval = 0;
			will_delay__remove(context);
			context__send_will(context);
			plugin_persist__handle_client_delete(context);
			context__add_to_disused(context);
		}अन्यथा{
			timeout = (item->context->session_expiry_time - db.now_real_s + 1) * 1000;
			अगर(timeout <= 0){
				timeout = 100;
			}
			loop_update_अग्र_event(timeout);
			वापस;
		}
	}
}
