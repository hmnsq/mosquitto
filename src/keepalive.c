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

#অন্তর্ভাব "config.h"
#অন্তর্ভাব <time.h>
#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব <utlist.h>


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

স্থির time_t last_keepalive_check = 0;
#ifndef WITH_OLD_KEEPALIVE
স্থির পূর্ণাঙ্ক keepalive_list_max = 0;
স্থির সংরচনা mosquitto **keepalive_list = রিক্ত;
#endif

#ifndef WITH_OLD_KEEPALIVE


স্থির পূর্ণাঙ্ক calc_index(সংরচনা mosquitto *context)
{
	প্রত্যাগম (পূর্ণাঙ্ক)(context->last_বার্তা_in + context->keepalive*3/2) % keepalive_list_max;
}
#endif


পূর্ণাঙ্ক keepalive__init(শূন্য)
{
#ifndef WITH_OLD_KEEPALIVE
	সংরচনা mosquitto *context, *ctxt_tmp;

	যদি(db.config->max_keepalive <= 0){
		keepalive_list_max = (UINT16_MAX * 3)/2 + 1;
	}অন্যথা{
		keepalive_list_max = (db.config->max_keepalive * 3)/2 + 1;
	}
	keepalive_list = mosquitto_calloc((size_t)keepalive_list_max, পরিমাণ(সংরচনা mosquitto *));
	যদি(keepalive_list == রিক্ত){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
		keepalive_list_max = 0;
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}

	/* Add existing clients - should only be applicable on MOSQ_EVT_RELOAD */
	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		যদি(net__is_connected(context) && !context->bridge && context->keepalive > 0){
			keepalive__add(context);
		}
	}
#endif
	last_keepalive_check = db.now_s;
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


শূন্য keepalive__cleanup(শূন্য)
{
#ifndef WITH_OLD_KEEPALIVE
	প্রতি(পূর্ণাঙ্ক idx=0; idx<keepalive_list_max; idx++){
		সংরচনা mosquitto *context, *ctxt_tmp;
		DL_FOREACH_SAFE2(keepalive_list[idx], context, ctxt_tmp, keepalive_next){
			DL_DELETE2(keepalive_list[idx], context, keepalive_prev, keepalive_next);
		}
	}
	mosquitto_free(keepalive_list);
	keepalive_list = রিক্ত;
	keepalive_list_max = 0;
#endif
}


পূর্ণাঙ্ক keepalive__add(সংরচনা mosquitto *context)
{
#ifndef WITH_OLD_KEEPALIVE
	যদি(context->keepalive <= 0 || !net__is_connected(context)){
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}
#ifdef WITH_BRIDGE
	যদি(context->bridge){
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}
#endif

	DL_APPEND2(keepalive_list[calc_index(context)], context, keepalive_prev, keepalive_next);
	context->keepalive_add_time = db.now_s;
#অন্যথা
	UNUSED(context);
#endif
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


#ifndef WITH_OLD_KEEPALIVE


শূন্য keepalive__check(শূন্য)
{
	সংরচনা mosquitto *context, *ctxt_tmp;
	time_t timeout;

	যদি(db.contexts_by_sock){
		/* Check the next 5 seconds for upcoming expiries */
		/* FIXME - find the actual next entry without having to iterate over
		 * the whole list */
		timeout = 5;
		প্রতি(time_t i=5; i>0; i--){
			যদি(keepalive_list[(db.now_s + i) % keepalive_list_max]){
				timeout = i;
			}
		}
		loop__update_next_event(timeout*1000);
	}
	প্রতি(time_t i=last_keepalive_check; i<db.now_s; i++){
		পূর্ণাঙ্ক idx = (পূর্ণাঙ্ক)(i % keepalive_list_max);
		যদি(keepalive_list[idx]){
			DL_FOREACH_SAFE2(keepalive_list[idx], context, ctxt_tmp, keepalive_next){
				/* keepalive_add_time lets us account for the client adding itself to the keepalive
				 * list when its last_msg_in value is greater than the last_keepalive_check.
				 * Without this, the client would be expired if it has keepalive == max_keepalive.
				 */
				যদি(context->keepalive_add_time <= last_keepalive_check && net__is_connected(context)){
					/* Client has exceeded keepalive*1.5 */
					do_disconnect(context, MOSQ_ERR_KEEPALIVE);
				}
			}
		}
	}

	last_keepalive_check = db.now_s;
}
#অন্যথা


শূন্য keepalive__check(শূন্য)
{
	সংরচনা mosquitto *context, *ctxt_tmp;
	time_t timeout;

	যদি(db.contexts_by_sock){
		timeout = (last_keepalive_check + 5 - db.now_s);
		যদি(timeout <= 0){
			timeout = 5;
		}
		loop__update_next_event(timeout*1000);
	}
	যদি(last_keepalive_check + 5 <= db.now_s){
		last_keepalive_check = db.now_s;

		HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
			যদি(net__is_connected(context)){
				/* Local bridges never time out in this fashion. */
				যদি(!(context->keepalive)
						|| context->bridge
						|| db.now_s - context->last_বার্তা_in <= (time_t)(context->keepalive)*3/2){

				}অন্যথা{
					/* Client has exceeded keepalive*1.5 */
					do_disconnect(context, MOSQ_ERR_KEEPALIVE);
				}
			}
		}
	}
}
#endif


পূর্ণাঙ্ক keepalive__remove(সংরচনা mosquitto *context)
{
#ifndef WITH_OLD_KEEPALIVE
	পূর্ণাঙ্ক idx;

	যদি(context->keepalive <= 0 || context->keepalive_prev == রিক্ত){
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}

	idx = calc_index(context);
	যদি(keepalive_list[idx]){
		DL_DELETE2(keepalive_list[idx], context, keepalive_prev, keepalive_next);
		context->keepalive_next = রিক্ত;
		context->keepalive_prev = রিক্ত;
	}
#অন্যথা
	UNUSED(context);
#endif
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক keepalive__update(সংরচনা mosquitto *context)
{
#ifndef WITH_OLD_KEEPALIVE
	keepalive__remove(context);
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->last_বার্তা_in = db.now_s;
	keepalive__add(context);
#অন্যথা
	UNUSED(context);
#endif
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}
