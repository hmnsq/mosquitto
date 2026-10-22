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

#અન્તર્ભાવ "config.h"
#અન્તર્ભાવ <time.h>
#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ <utlist.h>


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

સ્થિર time_t last_keepalive_check = 0;
#ifndef WITH_OLD_KEEPALIVE
સ્થિર પૂર્ણાંક keepalive_list_max = 0;
સ્થિર સંરચના mosquitto **keepalive_list = ખાલી;
#endif

#ifndef WITH_OLD_KEEPALIVE


સ્થિર પૂર્ણાંક calc_index(સંરચના mosquitto *context)
{
	પાછા (પૂર્ણાંક)(context->last_સંદેશ_in + context->keepalive*3/2) % keepalive_list_max;
}
#endif


પૂર્ણાંક keepalive__init(શૂન્ય)
{
#ifndef WITH_OLD_KEEPALIVE
	સંરચના mosquitto *context, *ctxt_tmp;

	જો(db.config->max_keepalive <= 0){
		keepalive_list_max = (UINT16_MAX * 3)/2 + 1;
	}નહીંતર{
		keepalive_list_max = (db.config->max_keepalive * 3)/2 + 1;
	}
	keepalive_list = mosquitto_calloc((size_t)keepalive_list_max, કદ(સંરચના mosquitto *));
	જો(keepalive_list == ખાલી){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
		keepalive_list_max = 0;
		પાછા MOSQ_ERR_NOMEM;
	}

	/* Add existing clients - should only be applicable on MOSQ_EVT_RELOAD */
	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		જો(net__is_connected(context) && !context->bridge && context->keepalive > 0){
			keepalive__add(context);
		}
	}
#endif
	last_keepalive_check = db.now_s;
	પાછા MOSQ_ERR_SUCCESS;
}


શૂન્ય keepalive__cleanup(શૂન્ય)
{
#ifndef WITH_OLD_KEEPALIVE
	માટે(પૂર્ણાંક idx=0; idx<keepalive_list_max; idx++){
		સંરચના mosquitto *context, *ctxt_tmp;
		DL_FOREACH_SAFE2(keepalive_list[idx], context, ctxt_tmp, keepalive_next){
			DL_DELETE2(keepalive_list[idx], context, keepalive_prev, keepalive_next);
		}
	}
	mosquitto_free(keepalive_list);
	keepalive_list = ખાલી;
	keepalive_list_max = 0;
#endif
}


પૂર્ણાંક keepalive__add(સંરચના mosquitto *context)
{
#ifndef WITH_OLD_KEEPALIVE
	જો(context->keepalive <= 0 || !net__is_connected(context)){
		પાછા MOSQ_ERR_SUCCESS;
	}
#ifdef WITH_BRIDGE
	જો(context->bridge){
		પાછા MOSQ_ERR_SUCCESS;
	}
#endif

	DL_APPEND2(keepalive_list[calc_index(context)], context, keepalive_prev, keepalive_next);
	context->keepalive_add_time = db.now_s;
#નહીંતર
	UNUSED(context);
#endif
	પાછા MOSQ_ERR_SUCCESS;
}


#ifndef WITH_OLD_KEEPALIVE


શૂન્ય keepalive__check(શૂન્ય)
{
	સંરચના mosquitto *context, *ctxt_tmp;
	time_t timeout;

	જો(db.contexts_by_sock){
		/* Check the next 5 seconds for upcoming expiries */
		/* FIXME - find the actual next entry without having to iterate over
		 * the whole list */
		timeout = 5;
		માટે(time_t i=5; i>0; i--){
			જો(keepalive_list[(db.now_s + i) % keepalive_list_max]){
				timeout = i;
			}
		}
		loop__update_next_event(timeout*1000);
	}
	માટે(time_t i=last_keepalive_check; i<db.now_s; i++){
		પૂર્ણાંક idx = (પૂર્ણાંક)(i % keepalive_list_max);
		જો(keepalive_list[idx]){
			DL_FOREACH_SAFE2(keepalive_list[idx], context, ctxt_tmp, keepalive_next){
				/* keepalive_add_time lets us account for the client adding itself to the keepalive
				 * list when its last_msg_in value is greater than the last_keepalive_check.
				 * Without this, the client would be expired if it has keepalive == max_keepalive.
				 */
				જો(context->keepalive_add_time <= last_keepalive_check && net__is_connected(context)){
					/* Client has exceeded keepalive*1.5 */
					do_disconnect(context, MOSQ_ERR_KEEPALIVE);
				}
			}
		}
	}

	last_keepalive_check = db.now_s;
}
#નહીંતર


શૂન્ય keepalive__check(શૂન્ય)
{
	સંરચના mosquitto *context, *ctxt_tmp;
	time_t timeout;

	જો(db.contexts_by_sock){
		timeout = (last_keepalive_check + 5 - db.now_s);
		જો(timeout <= 0){
			timeout = 5;
		}
		loop__update_next_event(timeout*1000);
	}
	જો(last_keepalive_check + 5 <= db.now_s){
		last_keepalive_check = db.now_s;

		HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
			જો(net__is_connected(context)){
				/* Local bridges never time out in this fashion. */
				જો(!(context->keepalive)
						|| context->bridge
						|| db.now_s - context->last_સંદેશ_in <= (time_t)(context->keepalive)*3/2){

				}નહીંતર{
					/* Client has exceeded keepalive*1.5 */
					do_disconnect(context, MOSQ_ERR_KEEPALIVE);
				}
			}
		}
	}
}
#endif


પૂર્ણાંક keepalive__remove(સંરચના mosquitto *context)
{
#ifndef WITH_OLD_KEEPALIVE
	પૂર્ણાંક idx;

	જો(context->keepalive <= 0 || context->keepalive_prev == ખાલી){
		પાછા MOSQ_ERR_SUCCESS;
	}

	idx = calc_index(context);
	જો(keepalive_list[idx]){
		DL_DELETE2(keepalive_list[idx], context, keepalive_prev, keepalive_next);
		context->keepalive_next = ખાલી;
		context->keepalive_prev = ખાલી;
	}
#નહીંતર
	UNUSED(context);
#endif
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક keepalive__update(સંરચના mosquitto *context)
{
#ifndef WITH_OLD_KEEPALIVE
	keepalive__remove(context);
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->last_સંદેશ_in = db.now_s;
	keepalive__add(context);
#નહીંતર
	UNUSED(context);
#endif
	પાછા MOSQ_ERR_SUCCESS;
}
