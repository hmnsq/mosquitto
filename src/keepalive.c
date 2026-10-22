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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <utlist.h>


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

ਸਥਿਰ time_t ਆਖਰੀ_keepalive_check = 0;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WITH_OLD_KEEPALIVE
ਸਥਿਰ ਪੂਰਨ keepalive_list_max = 0;
ਸਥਿਰ ਬਣਤਰ mosquitto **keepalive_list = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WITH_OLD_KEEPALIVE


ਸਥਿਰ ਪੂਰਨ calc_ਸੂਚਕਾਂਕ(ਬਣਤਰ mosquitto *context)
{
	ਵਾਪਸ (ਪੂਰਨ)(context->ਆਖਰੀ_ਸੁਨੇਹਾ_ਵਿੱਚ + context->keepalive*3/2) % keepalive_list_max;
}
#ਜੇ_ਅੰਤ


ਪੂਰਨ keepalive__init(ਖਾਲੀ)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WITH_OLD_KEEPALIVE
	ਬਣਤਰ mosquitto *context, *ctxt_tmp;

	ਜੇ(db.config->max_keepalive <= 0){
		keepalive_list_max = (UINT16_MAX * 3)/2 + 1;
	}ਨਹੀਂਤਾਂ{
		keepalive_list_max = (db.config->max_keepalive * 3)/2 + 1;
	}
	keepalive_list = mosquitto_calloc((size_t)keepalive_list_max, ਆਕਾਰ(ਬਣਤਰ mosquitto *));
	ਜੇ(keepalive_list == ਨਿਰਾ){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
		keepalive_list_max = 0;
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	/* Add existing clients - should only be applicable on MOSQ_EVT_RELOAD */
	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ਜੇ(net__is_connected(context) && !context->bridge && context->keepalive > 0){
			keepalive_ਜੋੜੋ(context);
		}
	}
#ਜੇ_ਅੰਤ
	ਆਖਰੀ_keepalive_check = db.now_s;
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਖਾਲੀ keepalive__cleanup(ਖਾਲੀ)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WITH_OLD_KEEPALIVE
	ਲਈ(ਪੂਰਨ idx=0; idx<keepalive_list_max; idx++){
		ਬਣਤਰ mosquitto *context, *ctxt_tmp;
		DL_FOREACH_SAFE2(keepalive_list[idx], context, ctxt_tmp, keepalive_ਅਗਲਾ){
			DL_DELETE2(keepalive_list[idx], context, keepalive_prev, keepalive_ਅਗਲਾ);
		}
	}
	mosquitto_free(keepalive_list);
	keepalive_list = ਨਿਰਾ;
	keepalive_list_max = 0;
#ਜੇ_ਅੰਤ
}


ਪੂਰਨ keepalive_ਜੋੜੋ(ਬਣਤਰ mosquitto *context)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WITH_OLD_KEEPALIVE
	ਜੇ(context->keepalive <= 0 || !net__is_connected(context)){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
	ਜੇ(context->bridge){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}
#ਜੇ_ਅੰਤ

	DL_APPEND2(keepalive_list[calc_ਸੂਚਕਾਂਕ(context)], context, keepalive_prev, keepalive_ਅਗਲਾ);
	context->keepalive_ਜੋੜੋ_ਸਮਾਂ = db.now_s;
#ਨਹੀਂਤਾਂ
	ਅਣਵਰਤਿਆ(context);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WITH_OLD_KEEPALIVE


ਖਾਲੀ keepalive__check(ਖਾਲੀ)
{
	ਬਣਤਰ mosquitto *context, *ctxt_tmp;
	time_t timeout;

	ਜੇ(db.contexts_by_sock){
		/* Check the next 5 seconds for upcoming expiries */
		/* FIXME - find the actual next entry without having to iterate over
		 * the whole list */
		timeout = 5;
		ਲਈ(time_t i=5; i>0; i--){
			ਜੇ(keepalive_list[(db.now_s + i) % keepalive_list_max]){
				timeout = i;
			}
		}
		loop_update_ਅਗਲਾ_event(timeout*1000);
	}
	ਲਈ(time_t i=ਆਖਰੀ_keepalive_check; i<db.now_s; i++){
		ਪੂਰਨ idx = (ਪੂਰਨ)(i % keepalive_list_max);
		ਜੇ(keepalive_list[idx]){
			DL_FOREACH_SAFE2(keepalive_list[idx], context, ctxt_tmp, keepalive_ਅਗਲਾ){
				/* keepalive_add_time lets us account for the client adding itself to the keepalive
				 * list when its last_msg_in value is greater than the last_keepalive_check.
				 * Without this, the client would be expired if it has keepalive == max_keepalive.
				 */
				ਜੇ(context->keepalive_ਜੋੜੋ_ਸਮਾਂ <= ਆਖਰੀ_keepalive_check && net__is_connected(context)){
					/* Client has exceeded keepalive*1.5 */
					do_disconnect(context, MOSQ_ERR_KEEPALIVE);
				}
			}
		}
	}

	ਆਖਰੀ_keepalive_check = db.now_s;
}
#ਨਹੀਂਤਾਂ


ਖਾਲੀ keepalive__check(ਖਾਲੀ)
{
	ਬਣਤਰ mosquitto *context, *ctxt_tmp;
	time_t timeout;

	ਜੇ(db.contexts_by_sock){
		timeout = (ਆਖਰੀ_keepalive_check + 5 - db.now_s);
		ਜੇ(timeout <= 0){
			timeout = 5;
		}
		loop_update_ਅਗਲਾ_event(timeout*1000);
	}
	ਜੇ(ਆਖਰੀ_keepalive_check + 5 <= db.now_s){
		ਆਖਰੀ_keepalive_check = db.now_s;

		HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
			ਜੇ(net__is_connected(context)){
				/* Local bridges never time out in this fashion. */
				ਜੇ(!(context->keepalive)
						|| context->bridge
						|| db.now_s - context->ਆਖਰੀ_ਸੁਨੇਹਾ_ਵਿੱਚ <= (time_t)(context->keepalive)*3/2){

				}ਨਹੀਂਤਾਂ{
					/* Client has exceeded keepalive*1.5 */
					do_disconnect(context, MOSQ_ERR_KEEPALIVE);
				}
			}
		}
	}
}
#ਜੇ_ਅੰਤ


ਪੂਰਨ keepalive__remove(ਬਣਤਰ mosquitto *context)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WITH_OLD_KEEPALIVE
	ਪੂਰਨ idx;

	ਜੇ(context->keepalive <= 0 || context->keepalive_prev == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	idx = calc_ਸੂਚਕਾਂਕ(context);
	ਜੇ(keepalive_list[idx]){
		DL_DELETE2(keepalive_list[idx], context, keepalive_prev, keepalive_ਅਗਲਾ);
		context->keepalive_ਅਗਲਾ = ਨਿਰਾ;
		context->keepalive_prev = ਨਿਰਾ;
	}
#ਨਹੀਂਤਾਂ
	ਅਣਵਰਤਿਆ(context);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ keepalive__update(ਬਣਤਰ mosquitto *context)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WITH_OLD_KEEPALIVE
	keepalive__remove(context);
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->ਆਖਰੀ_ਸੁਨੇਹਾ_ਵਿੱਚ = db.now_s;
	keepalive_ਜੋੜੋ(context);
#ਨਹੀਂਤਾਂ
	ਅਣਵਰਤਿਆ(context);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}
