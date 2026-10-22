/*
Copyright (c) 2009-2021 Roger Light <roger@atchoo.org>

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

#अन्तर्भाव "config.h"
#अन्तर्भाव <time.h>
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव <utlist.h>


/* This contains code for checking whether clients have exceeded their keepalive timeouts.
 * There are two versions.
 *
 * The old version can be used by compiling with `make WITH_OLD_KEEPALIVE=yes`.
 * It will scan the entire list of connected clients every 5 seconds to see if
 * they have expired. Hence it scales with O(n) and with e.g. 60000 clients can
 * have a measurable effect on CPU usage in the low single digit percent range.
 *
 * The new version scales with O(1). It uses a ring buffer that contains
 * max_keepalive*1.5+1 entries. The current time in integer seconds, modulus
 * the number of entries, points to the head of the ring buffer. Any clients
 * will appear after this point at the position indexed by the time at which
 * they will expire if they do not send another message, assuming they do not
 * have keepalive==0 - in which case they are not part of this check. So a
 * client that connects with keepalive=60 will be added at `now + 60*1.5`.
 *
 * A client is added to an entry with a doubly linked list. When the client
 * sends a new message, it is removed from the old position and added to the
 * new.
 *
 * As time moves on, if the linked list at the current entry is not empty, all
 * of the clients are expired.
 *
 * The ring buffer size is determined by max_keepalive. At the default, it is
 * 65535*1.5+1=98303 entries long. On a 64-bit machine that is 786424 bytes.
 * If this is too big a burden and you do not need many clients connected, then
 * the old check is sufficient. You can reduce the number of entries by setting
 * a lower max_keepalive value. A value as low as 600 still gives a 10 minute
 * keepalive and reduces the memory for the ring buffer to 7208 bytes.
 *
 * *NOTE* It is likely that the old check routine will be removed in the
 * future, and max_keepalive set to a sensible default value. If this is a
 * problem for you please get in touch.
 */

स्थिर समय_प्रकार last_keepalive_check = 0;
#ifndef WITH_OLD_KEEPALIVE
स्थिर पूर्णांक keepalive_list_max = 0;
स्थिर संरचना mosquitto **keepalive_list = रिक्त;
#endif

#ifndef WITH_OLD_KEEPALIVE


स्थिर पूर्णांक calc_index(संरचना mosquitto *context)
{
	परत (पूर्णांक)(context->last_संदेश_in + context->keepalive*3/2) % keepalive_list_max;
}
#endif


पूर्णांक keepalive__init(शून्य)
{
#ifndef WITH_OLD_KEEPALIVE
	संरचना mosquitto *context, *ctxt_tmp;

	जर(db.config->max_keepalive <= 0){
		keepalive_list_max = (UINT16_MAX * 3)/2 + 1;
	}अन्यथा{
		keepalive_list_max = (db.config->max_keepalive * 3)/2 + 1;
	}
	keepalive_list = mosquitto_calloc((माप_प्रकार)keepalive_list_max, आकार(संरचना mosquitto *));
	जर(keepalive_list == रिक्त){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		keepalive_list_max = 0;
		परत MOSQ_ERR_NOMEM;
	}

	/* Add existing clients - should only be applicable on MOSQ_EVT_RELOAD */
	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		जर(net__is_connected(context) && !context->bridge && context->keepalive > 0){
			keepalive__add(context);
		}
	}
#endif
	last_keepalive_check = db.now_s;
	परत MOSQ_ERR_SUCCESS;
}


शून्य keepalive__cleanup(शून्य)
{
#ifndef WITH_OLD_KEEPALIVE
	साठी(पूर्णांक idx=0; idx<keepalive_list_max; idx++){
		संरचना mosquitto *context, *ctxt_tmp;
		DL_FOREACH_SAFE2(keepalive_list[idx], context, ctxt_tmp, keepalive_next){
			DL_DELETE2(keepalive_list[idx], context, keepalive_prev, keepalive_next);
		}
	}
	mosquitto_free(keepalive_list);
	keepalive_list = रिक्त;
	keepalive_list_max = 0;
#endif
}


पूर्णांक keepalive__add(संरचना mosquitto *context)
{
#ifndef WITH_OLD_KEEPALIVE
	जर(context->keepalive <= 0 || !net__is_connected(context)){
		परत MOSQ_ERR_SUCCESS;
	}
#ifdef WITH_BRIDGE
	जर(context->bridge){
		परत MOSQ_ERR_SUCCESS;
	}
#endif

	DL_APPEND2(keepalive_list[calc_index(context)], context, keepalive_prev, keepalive_next);
	context->keepalive_add_time = db.now_s;
#अन्यथा
	UNUSED(context);
#endif
	परत MOSQ_ERR_SUCCESS;
}


#ifndef WITH_OLD_KEEPALIVE


शून्य keepalive__check(शून्य)
{
	संरचना mosquitto *context, *ctxt_tmp;
	समय_प्रकार timeout;

	जर(db.contexts_by_sock){
		/* Check the next 5 seconds for upcoming expiries */
		/* FIXME - find the actual next entry without having to iterate over
		 * the whole list */
		timeout = 5;
		साठी(समय_प्रकार i=5; i>0; i--){
			जर(keepalive_list[(db.now_s + i) % keepalive_list_max]){
				timeout = i;
			}
		}
		loop__update_next_event(timeout*1000);
	}
	साठी(समय_प्रकार i=last_keepalive_check; i<db.now_s; i++){
		पूर्णांक idx = (पूर्णांक)(i % keepalive_list_max);
		जर(keepalive_list[idx]){
			DL_FOREACH_SAFE2(keepalive_list[idx], context, ctxt_tmp, keepalive_next){
				/* keepalive_add_time lets us account for the client adding itself to the keepalive
				 * list when its last_msg_in value is greater than the last_keepalive_check.
				 * Without this, the client would be expired if it has keepalive == max_keepalive.
				 */
				जर(context->keepalive_add_time <= last_keepalive_check && net__is_connected(context)){
					/* Client has exceeded keepalive*1.5 */
					do_disconnect(context, MOSQ_ERR_KEEPALIVE);
				}
			}
		}
	}

	last_keepalive_check = db.now_s;
}
#अन्यथा


शून्य keepalive__check(शून्य)
{
	संरचना mosquitto *context, *ctxt_tmp;
	समय_प्रकार timeout;

	जर(db.contexts_by_sock){
		timeout = (last_keepalive_check + 5 - db.now_s);
		जर(timeout <= 0){
			timeout = 5;
		}
		loop__update_next_event(timeout*1000);
	}
	जर(last_keepalive_check + 5 <= db.now_s){
		last_keepalive_check = db.now_s;

		HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
			जर(net__is_connected(context)){
				/* Local bridges never time out in this fashion. */
				जर(!(context->keepalive)
						|| context->bridge
						|| db.now_s - context->last_संदेश_in <= (समय_प्रकार)(context->keepalive)*3/2){

				}अन्यथा{
					/* Client has exceeded keepalive*1.5 */
					do_disconnect(context, MOSQ_ERR_KEEPALIVE);
				}
			}
		}
	}
}
#endif


पूर्णांक keepalive__remove(संरचना mosquitto *context)
{
#ifndef WITH_OLD_KEEPALIVE
	पूर्णांक idx;

	जर(context->keepalive <= 0 || context->keepalive_prev == रिक्त){
		परत MOSQ_ERR_SUCCESS;
	}

	idx = calc_index(context);
	जर(keepalive_list[idx]){
		DL_DELETE2(keepalive_list[idx], context, keepalive_prev, keepalive_next);
		context->keepalive_next = रिक्त;
		context->keepalive_prev = रिक्त;
	}
#अन्यथा
	UNUSED(context);
#endif
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक keepalive__update(संरचना mosquitto *context)
{
#ifndef WITH_OLD_KEEPALIVE
	keepalive__remove(context);
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->last_संदेश_in = db.now_s;
	keepalive__add(context);
#अन्यथा
	UNUSED(context);
#endif
	परत MOSQ_ERR_SUCCESS;
}
