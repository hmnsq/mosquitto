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

#شامل "config.h"
#شامل <time.h>
#شامل "mosquitto_broker_internal.h"
#شامل <utlist.h>


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

ساکن time_t آخری_keepalive_check = 0;
#اگر_غیر_تعریف_شدہ WITH_OLD_KEEPALIVE
ساکن صحیح keepalive_list_max = 0;
ساکن ساخت mosquitto **keepalive_list = تہی;
#اگر_ختم

#اگر_غیر_تعریف_شدہ WITH_OLD_KEEPALIVE


ساکن صحیح calc_اشاریہ(ساخت mosquitto *context)
{
	واپس (صحیح)(context->آخری_پیغام_اندر + context->keepalive*3/2) % keepalive_list_max;
}
#اگر_ختم


صحیح keepalive__init(خالی)
{
#اگر_غیر_تعریف_شدہ WITH_OLD_KEEPALIVE
	ساخت mosquitto *context, *ctxt_tmp;

	اگر(db.config->max_keepalive <= 0){
		keepalive_list_max = (UINT16_MAX * 3)/2 + 1;
	}ورنہ{
		keepalive_list_max = (db.config->max_keepalive * 3)/2 + 1;
	}
	keepalive_list = mosquitto_calloc((size_t)keepalive_list_max, حجم(ساخت mosquitto *));
	اگر(keepalive_list == تہی){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
		keepalive_list_max = 0;
		واپس MOSQ_ERR_NOMEM;
	}

	/* Add existing clients - should only be applicable on MOSQ_EVT_RELOAD */
	HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
		اگر(net__is_connected(context) && !context->bridge && context->keepalive > 0){
			keepalive_جوڑو(context);
		}
	}
#اگر_ختم
	آخری_keepalive_check = db.now_s;
	واپس MOSQ_ERR_SUCCESS;
}


خالی keepalive__cleanup(خالی)
{
#اگر_غیر_تعریف_شدہ WITH_OLD_KEEPALIVE
	برائے(صحیح idx=0; idx<keepalive_list_max; idx++){
		ساخت mosquitto *context, *ctxt_tmp;
		DL_FOREACH_SAFE2(keepalive_list[idx], context, ctxt_tmp, keepalive_اگلا){
			DL_DELETE2(keepalive_list[idx], context, keepalive_پچھلا, keepalive_اگلا);
		}
	}
	mosquitto_free(keepalive_list);
	keepalive_list = تہی;
	keepalive_list_max = 0;
#اگر_ختم
}


صحیح keepalive_جوڑو(ساخت mosquitto *context)
{
#اگر_غیر_تعریف_شدہ WITH_OLD_KEEPALIVE
	اگر(context->keepalive <= 0 || !net__is_connected(context)){
		واپس MOSQ_ERR_SUCCESS;
	}
#اگر_تعریف_شدہ WITH_BRIDGE
	اگر(context->bridge){
		واپس MOSQ_ERR_SUCCESS;
	}
#اگر_ختم

	DL_APPEND2(keepalive_list[calc_اشاریہ(context)], context, keepalive_پچھلا, keepalive_اگلا);
	context->keepalive_جوڑو_وقت = db.now_s;
#ورنہ
	غیر_مستعمل(context);
#اگر_ختم
	واپس MOSQ_ERR_SUCCESS;
}


#اگر_غیر_تعریف_شدہ WITH_OLD_KEEPALIVE


خالی keepalive__check(خالی)
{
	ساخت mosquitto *context, *ctxt_tmp;
	time_t timeout;

	اگر(db.contexts_by_sock){
		/* Check the next 5 seconds for upcoming expiries */
		/* FIXME - find the actual next entry without having to iterate over
		 * the whole list */
		timeout = 5;
		برائے(time_t i=5; i>0; i--){
			اگر(keepalive_list[(db.now_s + i) % keepalive_list_max]){
				timeout = i;
			}
		}
		loop_update_اگلا_event(timeout*1000);
	}
	برائے(time_t i=آخری_keepalive_check; i<db.now_s; i++){
		صحیح idx = (صحیح)(i % keepalive_list_max);
		اگر(keepalive_list[idx]){
			DL_FOREACH_SAFE2(keepalive_list[idx], context, ctxt_tmp, keepalive_اگلا){
				/* keepalive_add_time lets us account for the client adding itself to the keepalive
				 * list when its last_msg_in value is greater than the last_keepalive_check.
				 * Without this, the client would be expired if it has keepalive == max_keepalive.
				 */
				اگر(context->keepalive_جوڑو_وقت <= آخری_keepalive_check && net__is_connected(context)){
					/* Client has exceeded keepalive*1.5 */
					do_disconnect(context, MOSQ_ERR_KEEPALIVE);
				}
			}
		}
	}

	آخری_keepalive_check = db.now_s;
}
#ورنہ


خالی keepalive__check(خالی)
{
	ساخت mosquitto *context, *ctxt_tmp;
	time_t timeout;

	اگر(db.contexts_by_sock){
		timeout = (آخری_keepalive_check + 5 - db.now_s);
		اگر(timeout <= 0){
			timeout = 5;
		}
		loop_update_اگلا_event(timeout*1000);
	}
	اگر(آخری_keepalive_check + 5 <= db.now_s){
		آخری_keepalive_check = db.now_s;

		HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
			اگر(net__is_connected(context)){
				/* Local bridges never time out in this fashion. */
				اگر(!(context->keepalive)
						|| context->bridge
						|| db.now_s - context->آخری_پیغام_اندر <= (time_t)(context->keepalive)*3/2){

				}ورنہ{
					/* Client has exceeded keepalive*1.5 */
					do_disconnect(context, MOSQ_ERR_KEEPALIVE);
				}
			}
		}
	}
}
#اگر_ختم


صحیح keepalive__remove(ساخت mosquitto *context)
{
#اگر_غیر_تعریف_شدہ WITH_OLD_KEEPALIVE
	صحیح idx;

	اگر(context->keepalive <= 0 || context->keepalive_پچھلا == تہی){
		واپس MOSQ_ERR_SUCCESS;
	}

	idx = calc_اشاریہ(context);
	اگر(keepalive_list[idx]){
		DL_DELETE2(keepalive_list[idx], context, keepalive_پچھلا, keepalive_اگلا);
		context->keepalive_اگلا = تہی;
		context->keepalive_پچھلا = تہی;
	}
#ورنہ
	غیر_مستعمل(context);
#اگر_ختم
	واپس MOSQ_ERR_SUCCESS;
}


صحیح keepalive__update(ساخت mosquitto *context)
{
#اگر_غیر_تعریف_شدہ WITH_OLD_KEEPALIVE
	keepalive__remove(context);
	/* coverity[missing_lock] - broker is single threaded, so no lock required */
	context->آخری_پیغام_اندر = db.now_s;
	keepalive_جوڑو(context);
#ورنہ
	غیر_مستعمل(context);
#اگر_ختم
	واپس MOSQ_ERR_SUCCESS;
}
