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

#అన్తర్భావ "config.h"
#అన్తర్భావ <time.h>
#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ <utlist.h>


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

స్థిర time_t చివరి_keepalive_check = 0;
#నిర్వచితం_కాకపోతే WITH_OLD_KEEPALIVE
స్థిర పూర్ణాంకం keepalive_list_max = 0;
స్థిర నిర్మాణం mosquitto **keepalive_list = ఖాళీ;
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే WITH_OLD_KEEPALIVE


స్థిర పూర్ణాంకం calc_సూచీ(నిర్మాణం mosquitto *context)
{
	తిరిగి (పూర్ణాంకం)(context->చివరి_సందేశం_లోపల + context->keepalive*3/2) % keepalive_list_max;
}
#అయితే_ముగింపు


పూర్ణాంకం keepalive__init(శూన్య)
{
#నిర్వచితం_కాకపోతే WITH_OLD_KEEPALIVE
	నిర్మాణం mosquitto *context, *ctxt_tmp;

	ఒకవేళ(db.config->max_keepalive <= 0){
		keepalive_list_max = (UINT16_MAX * 3)/2 + 1;
	}లేకపోతే{
		keepalive_list_max = (db.config->max_keepalive * 3)/2 + 1;
	}
	keepalive_list = mosquitto_calloc((size_t)keepalive_list_max, పరిమాణం(నిర్మాణం mosquitto *));
	ఒకవేళ(keepalive_list == ఖాళీ){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
		keepalive_list_max = 0;
		తిరిగి MOSQ_ERR_NOMEM;
	}

	/* Add existing clients - should only be applicable on MOSQ_EVT_RELOAD */
	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		ఒకవేళ(net__is_connected(context) && !context->bridge && context->keepalive > 0){
			keepalive_కూడు(context);
		}
	}
#అయితే_ముగింపు
	చివరి_keepalive_check = db.now_s;
	తిరిగి MOSQ_ERR_SUCCESS;
}


శూన్య keepalive__cleanup(శూన్య)
{
#నిర్వచితం_కాకపోతే WITH_OLD_KEEPALIVE
	కొరకు(పూర్ణాంకం idx=0; idx<keepalive_list_max; idx++){
		నిర్మాణం mosquitto *context, *ctxt_tmp;
		DL_FOREACH_SAFE2(keepalive_list[idx], context, ctxt_tmp, keepalive_తదుపరి){
			DL_DELETE2(keepalive_list[idx], context, keepalive_prev, keepalive_తదుపరి);
		}
	}
	mosquitto_free(keepalive_list);
	keepalive_list = ఖాళీ;
	keepalive_list_max = 0;
#అయితే_ముగింపు
}


పూర్ణాంకం keepalive_కూడు(నిర్మాణం mosquitto *context)
{
#నిర్వచితం_కాకపోతే WITH_OLD_KEEPALIVE
	ఒకవేళ(context->keepalive <= 0 || !net__is_connected(context)){
		తిరిగి MOSQ_ERR_SUCCESS;
	}
#నిర్వచితమైతే WITH_BRIDGE
	ఒకవేళ(context->bridge){
		తిరిగి MOSQ_ERR_SUCCESS;
	}
#అయితే_ముగింపు

	DL_APPEND2(keepalive_list[calc_సూచీ(context)], context, keepalive_prev, keepalive_తదుపరి);
	context->keepalive_కూడు_సమయం = db.now_s;
#లేకపోతే
	వాడని(context);
#అయితే_ముగింపు
	తిరిగి MOSQ_ERR_SUCCESS;
}


#నిర్వచితం_కాకపోతే WITH_OLD_KEEPALIVE


శూన్య keepalive__check(శూన్య)
{
	నిర్మాణం mosquitto *context, *ctxt_tmp;
	time_t timeout;

	ఒకవేళ(db.contexts_by_sock){
		/* Check the next 5 seconds for upcoming expiries */
		/* FIXME - find the actual next entry without having to iterate over
		 * the whole list */
		timeout = 5;
		కొరకు(time_t i=5; i>0; i--){
			ఒకవేళ(keepalive_list[(db.now_s + i) % keepalive_list_max]){
				timeout = i;
			}
		}
		loop_update_తదుపరి_event(timeout*1000);
	}
	కొరకు(time_t i=చివరి_keepalive_check; i<db.now_s; i++){
		పూర్ణాంకం idx = (పూర్ణాంకం)(i % keepalive_list_max);
		ఒకవేళ(keepalive_list[idx]){
			DL_FOREACH_SAFE2(keepalive_list[idx], context, ctxt_tmp, keepalive_తదుపరి){
				/* keepalive_add_time lets us account for the client adding itself to the keepalive
				 * list when its last_msg_in value is greater than the last_keepalive_check.
				 * Without this, the client would be expired if it has keepalive == max_keepalive.
				 */
				ఒకవేళ(context->keepalive_కూడు_సమయం <= చివరి_keepalive_check && net__is_connected(context)){
					/* Client has exceeded keepalive*1.5 */
					do_disconnect(context, MOSQ_ERR_KEEPALIVE);
				}
			}
		}
	}

	చివరి_keepalive_check = db.now_s;
}
#లేకపోతే


శూన్య keepalive__check(శూన్య)
{
	నిర్మాణం mosquitto *context, *ctxt_tmp;
	time_t timeout;

	ఒకవేళ(db.contexts_by_sock){
		timeout = (చివరి_keepalive_check + 5 - db.now_s);
		ఒకవేళ(timeout <= 0){
			timeout = 5;
		}
		loop_update_తదుపరి_event(timeout*1000);
	}
	ఒకవేళ(చివరి_keepalive_check + 5 <= db.now_s){
		చివరి_keepalive_check = db.now_s;

		HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
			ఒకవేళ(net__is_connected(context)){
				/* Local bridges never time out in this fashion. */
				ఒకవేళ(!(context->keepalive)
						|| context->bridge
						|| db.now_s - context->చివరి_సందేశం_లోపల <= (time_t)(context->keepalive)*3/2){

				}లేకపోతే{
					/* Client has exceeded keepalive*1.5 */
					do_disconnect(context, MOSQ_ERR_KEEPALIVE);
				}
			}
		}
	}
}
#అయితే_ముగింపు


పూర్ణాంకం keepalive__remove(నిర్మాణం mosquitto *context)
{
#నిర్వచితం_కాకపోతే WITH_OLD_KEEPALIVE
	పూర్ణాంకం idx;

	ఒకవేళ(context->keepalive <= 0 || context->keepalive_prev == ఖాళీ){
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	idx = calc_సూచీ(context);
	ఒకవేళ(keepalive_list[idx]){
		DL_DELETE2(keepalive_list[idx], context, keepalive_prev, keepalive_తదుపరి);
		context->keepalive_తదుపరి = ఖాళీ;
		context->keepalive_prev = ఖాళీ;
	}
#లేకపోతే
	వాడని(context);
#అయితే_ముగింపు
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం keepalive__update(నిర్మాణం mosquitto *context)
{
#నిర్వచితం_కాకపోతే WITH_OLD_KEEPALIVE
	keepalive__remove(context);
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->చివరి_సందేశం_లోపల = db.now_s;
	keepalive_కూడు(context);
#లేకపోతే
	వాడని(context);
#అయితే_ముగింపు
	తిరిగి MOSQ_ERR_SUCCESS;
}
