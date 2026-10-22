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

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <utlist.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "send_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sys_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"


/**
 * Is this context ready to take more in flight messages right now?
 * @param context the client context of interest
 * @param qos qos for the packet of interest
 * @return true if more in flight are allowed.
 */
bool db__ready_for_flight(ਬਣਤਰ mosquitto *context, ਗਿਣਤੀ mosquitto_ਸੁਨੇਹਾ_direction dir, ਪੂਰਨ qos)
{
	ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ *msgs;
	bool valid_ਬਾਈਟਾਂ;
	bool valid_ਗਿਣਤੀ;

	ਜੇ(dir == mosq_md_ਬਾਹਰਲਾ){
		msgs = &context->msgs_ਬਾਹਰਲਾ;
	}ਨਹੀਂਤਾਂ{
		msgs = &context->msgs_ਵਿੱਚ;
	}

	ਜੇ(msgs->inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ == 0 && db.config->max_inflight_ਬਾਈਟਾਂ == 0){
		ਵਾਪਸ true;
	}

	ਜੇ(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		ਜੇ(db.config->max_queued_messages == 0 && db.config->max_inflight_ਬਾਈਟਾਂ == 0){
			ਵਾਪਸ true;
		}
		valid_ਬਾਈਟਾਂ = ((msgs->inflight_ਬਾਈਟਾਂ - (ssize_t)db.config->max_inflight_ਬਾਈਟਾਂ) < (ssize_t)db.config->max_queued_ਬਾਈਟਾਂ);
		ਜੇ(dir == mosq_md_ਬਾਹਰਲਾ){
			valid_ਗਿਣਤੀ = context->ਬਾਹਰਲਾ_packet_ਗਿਣਤੀ < db.config->max_queued_messages;
		}ਨਹੀਂਤਾਂ{
			valid_ਗਿਣਤੀ = msgs->inflight_ਗਿਣਤੀ - msgs->inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ < db.config->max_queued_messages;
		}

		ਜੇ(db.config->max_queued_messages == 0){
			ਵਾਪਸ valid_ਬਾਈਟਾਂ;
		}
		ਜੇ(db.config->max_queued_ਬਾਈਟਾਂ == 0){
			ਵਾਪਸ valid_ਗਿਣਤੀ;
		}
	}ਨਹੀਂਤਾਂ{
		valid_ਬਾਈਟਾਂ = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_ਬਾਈਟਾਂ;
		valid_ਗਿਣਤੀ = msgs->inflight_quota > 0;

		ਜੇ(msgs->inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ == 0){
			ਵਾਪਸ valid_ਬਾਈਟਾਂ;
		}
		ਜੇ(db.config->max_inflight_ਬਾਈਟਾਂ == 0){
			ਵਾਪਸ valid_ਗਿਣਤੀ;
		}
	}

	ਵਾਪਸ valid_ਬਾਈਟਾਂ && valid_ਗਿਣਤੀ;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db_ready_for_ਕਤਾਰ(ਬਣਤਰ mosquitto *context, ਪੂਰਨ qos, ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ *ਸੁਨੇਹਾ_ਅੰਕੜੇ)
{
	ਪੂਰਨ ਸਰੋਤ_ਗਿਣਤੀ;
	ਪੂਰਨ adjust_ਗਿਣਤੀ;
	ਲੰਮਾ ਸਰੋਤ_ਬਾਈਟਾਂ;
	ssize_t adjust_ਬਾਈਟਾਂ = (ssize_t)db.config->max_inflight_ਬਾਈਟਾਂ;
	bool valid_ਬਾਈਟਾਂ;
	bool valid_ਗਿਣਤੀ;

	ਜੇ(db.config->max_queued_messages == 0 && db.config->max_queued_ਬਾਈਟਾਂ == 0){
		ਵਾਪਸ true;
	}

	ਜੇ(qos == 0 && db.config->ਕਤਾਰ_qos0_messages == false){
		ਵਾਪਸ false; /* This case is handled in db__ready_for_flight() */
	}ਨਹੀਂਤਾਂ{
		ਸਰੋਤ_ਬਾਈਟਾਂ = (ssize_t)ਸੁਨੇਹਾ_ਅੰਕੜੇ->queued_bytes12;
		ਸਰੋਤ_ਗਿਣਤੀ = ਸੁਨੇਹਾ_ਅੰਕੜੇ->queued_count12;
	}
	adjust_ਗਿਣਤੀ = ਸੁਨੇਹਾ_ਅੰਕੜੇ->inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ;

	/* nothing in flight for offline clients */
	ਜੇ(!net__is_connected(context)){
		adjust_ਬਾਈਟਾਂ = 0;
		adjust_ਗਿਣਤੀ = 0;
	}

	valid_ਬਾਈਟਾਂ = (ਸਰੋਤ_ਬਾਈਟਾਂ - (ssize_t)adjust_ਬਾਈਟਾਂ) < (ssize_t)db.config->max_queued_ਬਾਈਟਾਂ;
	valid_ਗਿਣਤੀ = ਸਰੋਤ_ਗਿਣਤੀ - adjust_ਗਿਣਤੀ < db.config->max_queued_messages;

	ਜੇ(db.config->max_queued_ਬਾਈਟਾਂ == 0){
		ਵਾਪਸ valid_ਗਿਣਤੀ;
	}
	ਜੇ(db.config->max_queued_messages == 0){
		ਵਾਪਸ valid_ਬਾਈਟਾਂ;
	}

	ਵਾਪਸ valid_ਬਾਈਟਾਂ && valid_ਗਿਣਤੀ;
}


ਖਾਲੀ db_ਸੁਨੇਹਾ_ਜੋੜੋ_ਮੰਜ਼ਿਲ_inflight_stats(ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ *ਸੁਨੇਹਾ_ਅੰਕੜੇ, ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ)
{
	ਸੁਨੇਹਾ_ਅੰਕੜੇ->inflight_ਗਿਣਤੀ++;
	ਸੁਨੇਹਾ_ਅੰਕੜੇ->inflight_ਬਾਈਟਾਂ += client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen;
	ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos != 0){
		ਸੁਨੇਹਾ_ਅੰਕੜੇ->inflight_count12++;
		ਸੁਨੇਹਾ_ਅੰਕੜੇ->inflight_bytes12 += client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen;
	}
}


ਸਥਿਰ ਖਾਲੀ db_ਸੁਨੇਹਾ_remove_from_inflight_stats(ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ *ਸੁਨੇਹਾ_ਅੰਕੜੇ, ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ)
{
	ਸੁਨੇਹਾ_ਅੰਕੜੇ->inflight_ਗਿਣਤੀ--;
	ਸੁਨੇਹਾ_ਅੰਕੜੇ->inflight_ਬਾਈਟਾਂ -= client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen;
	ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos != 0){
		ਸੁਨੇਹਾ_ਅੰਕੜੇ->inflight_count12--;
		ਸੁਨੇਹਾ_ਅੰਕੜੇ->inflight_bytes12 -= client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen;
	}
}


ਖਾਲੀ db_ਸੁਨੇਹਾ_ਜੋੜੋ_ਮੰਜ਼ਿਲ_queued_stats(ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ *ਸੁਨੇਹਾ_ਅੰਕੜੇ, ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ)
{
	ਸੁਨੇਹਾ_ਅੰਕੜੇ->queued_ਗਿਣਤੀ++;
	ਸੁਨੇਹਾ_ਅੰਕੜੇ->queued_ਬਾਈਟਾਂ += client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen;
	ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos != 0){
		ਸੁਨੇਹਾ_ਅੰਕੜੇ->queued_count12++;
		ਸੁਨੇਹਾ_ਅੰਕੜੇ->queued_bytes12 += client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen;
	}
}


ਸਥਿਰ ਖਾਲੀ db_ਸੁਨੇਹਾ_remove_from_queued_stats(ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ *ਸੁਨੇਹਾ_ਅੰਕੜੇ, ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ)
{
	ਸੁਨੇਹਾ_ਅੰਕੜੇ->queued_ਗਿਣਤੀ--;
	ਸੁਨੇਹਾ_ਅੰਕੜੇ->queued_ਬਾਈਟਾਂ -= client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen;
	ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos != 0){
		ਸੁਨੇਹਾ_ਅੰਕੜੇ->queued_count12--;
		ਸੁਨੇਹਾ_ਅੰਕੜੇ->queued_bytes12 -= client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen;
	}
}


ਪੂਰਨ db__open(ਬਣਤਰ mosquitto__config *config)
{
	ਜੇ(!config){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = ਨਿਰਾ;
	db.contexts_by_sock = ਨਿਰਾ;
	db.contexts_for_free = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
	db.bridges = ਨਿਰਾ;
	db.bridge_ਗਿਣਤੀ = 0;
#ਜੇ_ਅੰਤ

	/* Initialize the hashtable */
	db.clientid_ਸੂਚਕਾਂਕ_hash = ਨਿਰਾ;

	db.ਸਾਧਾਰਨ_subs = ਨਿਰਾ;
	db.shared_subs = ਨਿਰਾ;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE
	ਜੇ(persist__restore()){
		ਵਾਪਸ 1;
	}
#ਜੇ_ਅੰਤ

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਖਾਲੀ subhier_clean(ਬਣਤਰ mosquitto__subhier **subhier)
{
	ਬਣਤਰ mosquitto__subhier *peer, *subhier_tmp;
	ਬਣਤਰ mosquitto__subleaf *ਪੱਤੀ, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		ਪੱਤੀ = peer->subs;
		ਜਦੋਂ(ਪੱਤੀ){
			nextleaf = ਪੱਤੀ->ਅਗਲਾ;
			mosquitto_FREE(ਪੱਤੀ);
			ਪੱਤੀ = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


ਪੂਰਨ db__close(ਖਾਲੀ)
{
	subhier_clean(&db.ਸਾਧਾਰਨ_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db_ਸੁਨੇਹਾ_store_clean();

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ db_ਸੁਨੇਹਾ_store_ਜੋੜੋ(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ)
{
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਮਿਲਿਆ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ hashv;

	HASH_ਮੁੱਲ(&ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.store_id, ਆਕਾਰ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.store_id), hashv);
	HASH_ਲੱਭੋ_BYHASHVALUE(hh, db.ਸੁਨੇਹਾ_store, &ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.store_id, ਆਕਾਰ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.store_id), hashv, ਮਿਲਿਆ);
	ਜੇ(ਮਿਲਿਆ == ਨਿਰਾ){
		HASH_ਜੋੜੋ_KEYPTR_BYHASHVALUE(hh, db.ਸੁਨੇਹਾ_store, &ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.store_id, ਆਕਾਰ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.store_id), hashv, ਅਧਾਰ_ਸੁਨੇਹਾ);
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ MOSQ_ERR_ALREADY_EXISTS;
	}
}


ਖਾਲੀ db_ਸੁਨੇਹਾ_store_free(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ)
{
	mosquitto_FREE(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_id);
	mosquitto_FREE(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_username);
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->dest_ids){
		ਲਈ(ਪੂਰਨ i=0; i<ਅਧਾਰ_ਸੁਨੇਹਾ->dest_id_ਗਿਣਤੀ; i++){
			mosquitto_FREE(ਅਧਾਰ_ਸੁਨੇਹਾ->dest_ids[i]);
		}
		mosquitto_FREE(ਅਧਾਰ_ਸੁਨੇਹਾ->dest_ids);
	}
	mosquitto_FREE(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic);
	mosquitto_property_free_all(&ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.properties);
	mosquitto_FREE(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload);
	mosquitto_FREE(ਅਧਾਰ_ਸੁਨੇਹਾ);
}


ਖਾਲੀ db_ਸੁਨੇਹਾ_store_remove(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ, bool notify)
{
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ == ਨਿਰਾ){
		ਵਾਪਸ;
	}
	HASH_DELETE(hh, db.ਸੁਨੇਹਾ_store, ਅਧਾਰ_ਸੁਨੇਹਾ);
	db.ਸੁਨੇਹਾ_store_ਗਿਣਤੀ--;
	db.ਸੁਨੇਹਾ_store_ਬਾਈਟਾਂ -= ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen;
	ਜੇ(notify == true){
		plugin_persist_handle_ਅਧਾਰ_ਸੁਨੇਹਾ_delete(ਅਧਾਰ_ਸੁਨੇਹਾ);
	}
	db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
}


ਖਾਲੀ db_ਸੁਨੇਹਾ_store_clean(ਖਾਲੀ)
{
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ, *ਅਧਾਰ_ਸੁਨੇਹਾ_tmp;

	HASH_ITER(hh, db.ਸੁਨੇਹਾ_store, ਅਧਾਰ_ਸੁਨੇਹਾ, ਅਧਾਰ_ਸੁਨੇਹਾ_tmp){
		db_ਸੁਨੇਹਾ_store_remove(ਅਧਾਰ_ਸੁਨੇਹਾ, false);
	}
}


ਖਾਲੀ db_ਸੁਨੇਹਾ_store_ref_inc(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ)
{
	ਅਧਾਰ_ਸੁਨੇਹਾ->ref_ਗਿਣਤੀ++;
}


ਖਾਲੀ db_ਸੁਨੇਹਾ_store_ref_dec(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ **ਅਧਾਰ_ਸੁਨੇਹਾ)
{
	(*ਅਧਾਰ_ਸੁਨੇਹਾ)->ref_ਗਿਣਤੀ--;
	ਜੇ((*ਅਧਾਰ_ਸੁਨੇਹਾ)->ref_ਗਿਣਤੀ == 0){
		db_ਸੁਨੇਹਾ_store_remove(*ਅਧਾਰ_ਸੁਨੇਹਾ, true);
		*ਅਧਾਰ_ਸੁਨੇਹਾ = ਨਿਰਾ;
	}
}


ਖਾਲੀ db_ਸੁਨੇਹਾ_store_compact(ਖਾਲੀ)
{
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ, *ਅਧਾਰ_ਸੁਨੇਹਾ_tmp;

	HASH_ITER(hh, db.ਸੁਨੇਹਾ_store, ਅਧਾਰ_ਸੁਨੇਹਾ, ਅਧਾਰ_ਸੁਨੇਹਾ_tmp){
		ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ref_ਗਿਣਤੀ < 1){
			db_ਸੁਨੇਹਾ_store_remove(ਅਧਾਰ_ਸੁਨੇਹਾ, true);
		}
	}
}


ਸਥਿਰ ਖਾਲੀ db_ਸੁਨੇਹਾ_remove_inflight(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ *ਸੁਨੇਹਾ_ਅੰਕੜੇ, ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *ਵਸਤੂ)
{
	ਜੇ(!context || !ਸੁਨੇਹਾ_ਅੰਕੜੇ || !ਵਸਤੂ){
		ਵਾਪਸ;
	}

	plugin_persist_handle_client_ਸੁਨੇਹਾ_delete(context, ਵਸਤੂ);

	DL_DELETE(ਸੁਨੇਹਾ_ਅੰਕੜੇ->inflight, ਵਸਤੂ);
	ਜੇ(ਵਸਤੂ->ਅਧਾਰ_ਸੁਨੇਹਾ){
		db_ਸੁਨੇਹਾ_remove_from_inflight_stats(ਸੁਨੇਹਾ_ਅੰਕੜੇ, ਵਸਤੂ);
		db_ਸੁਨੇਹਾ_store_ref_dec(&ਵਸਤੂ->ਅਧਾਰ_ਸੁਨੇਹਾ);
	}

	mosquitto_FREE(ਵਸਤੂ);
}


ਸਥਿਰ ਖਾਲੀ db_ਸੁਨੇਹਾ_remove_queued(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ *ਸੁਨੇਹਾ_ਅੰਕੜੇ, ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *ਵਸਤੂ)
{
	ਜੇ(!context || !ਸੁਨੇਹਾ_ਅੰਕੜੇ || !ਵਸਤੂ){
		ਵਾਪਸ;
	}

	plugin_persist_handle_client_ਸੁਨੇਹਾ_delete(context, ਵਸਤੂ);

	DL_DELETE(ਸੁਨੇਹਾ_ਅੰਕੜੇ->queued, ਵਸਤੂ);
	ਜੇ(ਵਸਤੂ->ਅਧਾਰ_ਸੁਨੇਹਾ){
		db_ਸੁਨੇਹਾ_remove_from_queued_stats(ਸੁਨੇਹਾ_ਅੰਕੜੇ, ਵਸਤੂ);
		db_ਸੁਨੇਹਾ_store_ref_dec(&ਵਸਤੂ->ਅਧਾਰ_ਸੁਨੇਹਾ);
	}

	mosquitto_FREE(ਵਸਤੂ);
}


ਸਥਿਰ ਖਾਲੀ db_fill_inflight_ਬਾਹਰਲਾ_from_ਕਤਾਰ(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ, *tmp;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(context->msgs_ਬਾਹਰਲਾ.queued, client_ਸੁਨੇਹਾ, tmp){
		ਜੇ(!db__ready_for_flight(context, mosq_md_ਬਾਹਰਲਾ, client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos)){
			ਵਾਪਸ;
		}
		ਚੋਣ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos){
			ਹਾਲਤ 0:
				client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state = mosq_ms_publish_qos0;
				ਰੋਕੋ;
			ਹਾਲਤ 1:
				client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state = mosq_ms_publish_qos1;
				ਰੋਕੋ;
			ਹਾਲਤ 2:
				client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state = mosq_ms_publish_qos2;
				ਰੋਕੋ;
		}
		ਜੇ(client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.expiry_ਸਮਾਂ && db.now_real_s > client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.expiry_ਸਮਾਂ){
			db_ਸੁਨੇਹਾ_remove_queued(context, &context->msgs_ਬਾਹਰਲਾ, client_ਸੁਨੇਹਾ);
			ਜਾਰੀ;
		}
		plugin_persist_handle_client_ਸੁਨੇਹਾ_update(context, client_ਸੁਨੇਹਾ);
		db_ਸੁਨੇਹਾ_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ_ਪਹਿਲਾ(context, &context->msgs_ਬਾਹਰਲਾ);
	}
}


ਖਾਲੀ db_ਸੁਨੇਹਾ_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ_ਪਹਿਲਾ(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ *ਸੁਨੇਹਾ_ਅੰਕੜੇ)
{
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ;

	ਅਣਵਰਤਿਆ(context);

	client_ਸੁਨੇਹਾ = ਸੁਨੇਹਾ_ਅੰਕੜੇ->queued;
	DL_DELETE(ਸੁਨੇਹਾ_ਅੰਕੜੇ->queued, client_ਸੁਨੇਹਾ);
	DL_APPEND(ਸੁਨੇਹਾ_ਅੰਕੜੇ->inflight, client_ਸੁਨੇਹਾ);
	ਜੇ(ਸੁਨੇਹਾ_ਅੰਕੜੇ->inflight_quota > 0){
		ਸੁਨੇਹਾ_ਅੰਕੜੇ->inflight_quota--;
	}

	db_ਸੁਨੇਹਾ_remove_from_queued_stats(ਸੁਨੇਹਾ_ਅੰਕੜੇ, client_ਸੁਨੇਹਾ);
	db_ਸੁਨੇਹਾ_ਜੋੜੋ_ਮੰਜ਼ਿਲ_inflight_stats(ਸੁਨੇਹਾ_ਅੰਕੜੇ, client_ਸੁਨੇਹਾ);
}


ਪੂਰਨ db_ਸੁਨੇਹਾ_delete_outgoing(ਬਣਤਰ mosquitto *context, uint16_t mid, ਗਿਣਤੀ mosquitto_ਸੁਨੇਹਾ_state expect_state, ਪੂਰਨ qos)
{
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ, *tmp;
	bool deleted = false;

	ਜੇ(!context){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(context->msgs_ਬਾਹਰਲਾ.inflight, client_ਸੁਨੇਹਾ, tmp){
		ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.mid == mid){
			ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos != qos){
				log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos, qos);
				ਵਾਪਸ MOSQ_ERR_PROTOCOL;
			}ਨਹੀਂਤਾਂ ਜੇ(qos == 2 && client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state != expect_state && expect_state != mosq_ms_any){
				log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state, expect_state);
				ਵਾਪਸ MOSQ_ERR_PROTOCOL;
			}
			db_ਸੁਨੇਹਾ_remove_inflight(context, &context->msgs_ਬਾਹਰਲਾ, client_ਸੁਨੇਹਾ);
			deleted = true;
			ਰੋਕੋ;
		}
	}

	ਜੇ(deleted == false){
		DL_FOREACH_ਸੁਰੱਖਿਅਤ(context->msgs_ਬਾਹਰਲਾ.queued, client_ਸੁਨੇਹਾ, tmp){
			ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.mid == mid){
				ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos != qos){
					ਵਾਪਸ MOSQ_ERR_PROTOCOL;
				}ਨਹੀਂਤਾਂ ਜੇ(qos == 2 && client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state != expect_state && expect_state != mosq_ms_any){
					ਵਾਪਸ MOSQ_ERR_PROTOCOL;
				}
				db_ਸੁਨੇਹਾ_remove_queued(context, &context->msgs_ਬਾਹਰਲਾ, client_ਸੁਨੇਹਾ);
				ਰੋਕੋ;
			}
		}
	}
	db_fill_inflight_ਬਾਹਰਲਾ_from_ਕਤਾਰ(context);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE
	db.persistence_changes++;
#ਜੇ_ਅੰਤ

	ਵਾਪਸ db_ਸੁਨੇਹਾ_write_inflight_ਬਾਹਰਲਾ_latest(context);
}


/* Only for QoS 2 messages */
ਪੂਰਨ db_ਸੁਨੇਹਾ_ਪਾਓ_incoming(ਬਣਤਰ mosquitto *context, uint64_t cmsg_id, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ, bool persist)
{
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ;
	ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ *ਸੁਨੇਹਾ_ਅੰਕੜੇ;
	ਗਿਣਤੀ mosquitto_ਸੁਨੇਹਾ_state state = mosq_ms_invalid;
	ਪੂਰਨ rc = 0;

	ਪ੍ਰਤਿਜ੍ਞਾ(ਅਧਾਰ_ਸੁਨੇਹਾ);
	ਜੇ(!context){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਜੇ(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		ਵਾਪਸ MOSQ_ERR_SUCCESS;

	}
	ਸੁਨੇਹਾ_ਅੰਕੜੇ = &context->msgs_ਵਿੱਚ;

	ਜੇ(db__ready_for_flight(context, mosq_md_ਵਿੱਚ, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos)){
		state = mosq_ms_wait_for_pubrel;
	}ਨਹੀਂਤਾਂ ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos != 0 && db_ready_for_ਕਤਾਰ(context, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos, ਸੁਨੇਹਾ_ਅੰਕੜੇ)){
		state = mosq_ms_queued;
		rc = 2;
	}ਨਹੀਂਤਾਂ{
		/* Dropping message due to full queue. */
		ਜੇ(context->is_dropping == false){
			context->is_dropping = true;
			log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE,
					"Outgoing messages are being dropped for client %s.",
					context->id);
		}
		metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		ਵਾਪਸ 2;
	}

	ਪ੍ਰਤਿਜ੍ਞਾ(state != mosq_ms_invalid);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE
	ਜੇ(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#ਜੇ_ਅੰਤ

	client_ਸੁਨੇਹਾ = mosquitto_malloc(ਆਕਾਰ(ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ));
	ਜੇ(!client_ਸੁਨੇਹਾ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	client_ਸੁਨੇਹਾ->prev = ਨਿਰਾ;
	client_ਸੁਨੇਹਾ->ਅਗਲਾ = ਨਿਰਾ;
	ਜੇ(cmsg_id){
		client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.cmsg_id = cmsg_id;
	}ਨਹੀਂਤਾਂ{
		client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.cmsg_id = ++context->ਆਖਰੀ_cmsg_id;
	}
	client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ = ਅਧਾਰ_ਸੁਨੇਹਾ;
	db_ਸੁਨੇਹਾ_store_ref_inc(client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ);
	client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.mid = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid;
	client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.direction = mosq_md_ਵਿੱਚ;
	client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state = (uint8_t)state;
	client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.dup = false;
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos > context->max_qos){
		client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos = context->max_qos;
	}ਨਹੀਂਤਾਂ{
		client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos;
	}
	client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain;
	client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.subscription_identifier = 0;

	ਜੇ(state == mosq_ms_queued){
		DL_APPEND(ਸੁਨੇਹਾ_ਅੰਕੜੇ->queued, client_ਸੁਨੇਹਾ);
		db_ਸੁਨੇਹਾ_ਜੋੜੋ_ਮੰਜ਼ਿਲ_queued_stats(ਸੁਨੇਹਾ_ਅੰਕੜੇ, client_ਸੁਨੇਹਾ);
	}ਨਹੀਂਤਾਂ{
		DL_APPEND(ਸੁਨੇਹਾ_ਅੰਕੜੇ->inflight, client_ਸੁਨੇਹਾ);
		db_ਸੁਨੇਹਾ_ਜੋੜੋ_ਮੰਜ਼ਿਲ_inflight_stats(ਸੁਨੇਹਾ_ਅੰਕੜੇ, client_ਸੁਨੇਹਾ);
	}

	ਜੇ(persist && context->is_persisted){
		plugin_persist_handle_ਅਧਾਰ_ਸੁਨੇਹਾ_ਜੋੜੋ(client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ);
		plugin_persist_handle_client_ਸੁਨੇਹਾ_ਜੋੜੋ(context, client_ਸੁਨੇਹਾ);
	}

	ਜੇ(client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos > 0){
		util__decrement_receive_quota(context);
	}
	ਵਾਪਸ rc;
}


ਪੂਰਨ db_ਸੁਨੇਹਾ_ਪਾਓ_outgoing(ਬਣਤਰ mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ, uint32_t subscription_identifier, bool update, bool persist)
{
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ;
	ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ *ਸੁਨੇਹਾ_ਅੰਕੜੇ;
	ਗਿਣਤੀ mosquitto_ਸੁਨੇਹਾ_state state = mosq_ms_invalid;
	ਪੂਰਨ rc = 0;
	ਅੱਖਰ **dest_ids;

	ਪ੍ਰਤਿਜ੍ਞਾ(ਅਧਾਰ_ਸੁਨੇਹਾ);
	ਜੇ(!context){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਜੇ(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		ਵਾਪਸ MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_ਭੇਜਿਆ++;

	ਸੁਨੇਹਾ_ਅੰਕੜੇ = &context->msgs_ਬਾਹਰਲਾ;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	ਜੇ(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && ਅਧਾਰ_ਸੁਨੇਹਾ->dest_ids){

		ਲਈ(ਪੂਰਨ i=0; i<ਅਧਾਰ_ਸੁਨੇਹਾ->dest_id_ਗਿਣਤੀ; i++){
			ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->dest_ids[i] && !strcmp(ਅਧਾਰ_ਸੁਨੇਹਾ->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				ਵਾਪਸ MOSQ_ERR_SUCCESS;
			}
		}
	}
	ਜੇ(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		ਜੇ(qos == 0 && !db.config->ਕਤਾਰ_qos0_messages){
			ਜੇ(!context->bridge){
				ਵਾਪਸ 2;
			}ਨਹੀਂਤਾਂ{
				ਜੇ(context->bridge->ਸ਼ੁਰੂ_type != bst_lazy){
					ਵਾਪਸ 2;
				}
			}
		}
		ਜੇ(context->bridge && context->bridge->clean_ਸ਼ੁਰੂ_local == true){
			ਵਾਪਸ 2;
		}
	}

	ਜੇ(net__is_connected(context)){
		ਜੇ(db__ready_for_flight(context, mosq_md_ਬਾਹਰਲਾ, qos)){
			ਚੋਣ(qos){
				ਹਾਲਤ 0:
					state = mosq_ms_publish_qos0;
					ਰੋਕੋ;
				ਹਾਲਤ 1:
					state = mosq_ms_publish_qos1;
					ਰੋਕੋ;
				ਹਾਲਤ 2:
					state = mosq_ms_publish_qos2;
					ਰੋਕੋ;
			}
		}ਨਹੀਂਤਾਂ ਜੇ(qos != 0 && db_ready_for_ਕਤਾਰ(context, qos, ਸੁਨੇਹਾ_ਅੰਕੜੇ)){
			state = mosq_ms_queued;
			rc = 2;
		}ਨਹੀਂਤਾਂ{
			/* Dropping message due to full queue. */
			ਜੇ(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			ਵਾਪਸ 2;
		}
	}ਨਹੀਂਤਾਂ{
		ਜੇ(db_ready_for_ਕਤਾਰ(context, qos, ਸੁਨੇਹਾ_ਅੰਕੜੇ)){
			state = mosq_ms_queued;
		}ਨਹੀਂਤਾਂ{
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			ਜੇ(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			ਵਾਪਸ 2;
		}
	}
	ਪ੍ਰਤਿਜ੍ਞਾ(state != mosq_ms_invalid);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE
	ਜੇ(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#ਜੇ_ਅੰਤ

	client_ਸੁਨੇਹਾ = mosquitto_malloc(ਆਕਾਰ(ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ));
	ਜੇ(!client_ਸੁਨੇਹਾ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	client_ਸੁਨੇਹਾ->prev = ਨਿਰਾ;
	client_ਸੁਨੇਹਾ->ਅਗਲਾ = ਨਿਰਾ;
	ਜੇ(cmsg_id){
		client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.cmsg_id = cmsg_id;
	}ਨਹੀਂਤਾਂ{
		client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.cmsg_id = ++context->ਆਖਰੀ_cmsg_id;
	}
	client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ = ਅਧਾਰ_ਸੁਨੇਹਾ;
	db_ਸੁਨੇਹਾ_store_ref_inc(client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ);
	client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.mid = mid;
	client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.direction = mosq_md_ਬਾਹਰਲਾ;
	client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state = (uint8_t)state;
	client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.dup = false;
	ਜੇ(qos > context->max_qos){
		client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos = context->max_qos;
	}ਨਹੀਂਤਾਂ{
		client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos = qos;
	}
	client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain = retain;
	client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.subscription_identifier = subscription_identifier;

	ਜੇ(state == mosq_ms_queued){
		DL_APPEND(ਸੁਨੇਹਾ_ਅੰਕੜੇ->queued, client_ਸੁਨੇਹਾ);
		db_ਸੁਨੇਹਾ_ਜੋੜੋ_ਮੰਜ਼ਿਲ_queued_stats(ਸੁਨੇਹਾ_ਅੰਕੜੇ, client_ਸੁਨੇਹਾ);
	}ਨਹੀਂਤਾਂ{
		DL_APPEND(ਸੁਨੇਹਾ_ਅੰਕੜੇ->inflight, client_ਸੁਨੇਹਾ);
		db_ਸੁਨੇਹਾ_ਜੋੜੋ_ਮੰਜ਼ਿਲ_inflight_stats(ਸੁਨੇਹਾ_ਅੰਕੜੇ, client_ਸੁਨੇਹਾ);
	}

	ਜੇ(persist && context->is_persisted){
		plugin_persist_handle_ਅਧਾਰ_ਸੁਨੇਹਾ_ਜੋੜੋ(client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ);
		plugin_persist_handle_client_ਸੁਨੇਹਾ_ਜੋੜੋ(context, client_ਸੁਨੇਹਾ);
	}

	ਜੇ(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(ਅਧਾਰ_ਸੁਨੇਹਾ->dest_ids, ਆਕਾਰ(ਅੱਖਰ *)*(size_t)(ਅਧਾਰ_ਸੁਨੇਹਾ->dest_id_ਗਿਣਤੀ+1));
		ਜੇ(dest_ids){
			ਅਧਾਰ_ਸੁਨੇਹਾ->dest_ids = dest_ids;
			ਅਧਾਰ_ਸੁਨੇਹਾ->dest_id_ਗਿਣਤੀ++;
			ਅਧਾਰ_ਸੁਨੇਹਾ->dest_ids[ਅਧਾਰ_ਸੁਨੇਹਾ->dest_id_ਗਿਣਤੀ-1] = mosquitto_strdup(context->id);
			ਜੇ(!ਅਧਾਰ_ਸੁਨੇਹਾ->dest_ids[ਅਧਾਰ_ਸੁਨੇਹਾ->dest_id_ਗਿਣਤੀ-1]){
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
		}ਨਹੀਂਤਾਂ{
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
	ਜੇ(context->bridge && context->bridge->ਸ਼ੁਰੂ_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_ਬਾਹਰਲਾ.inflight_ਗਿਣਤੀ + context->msgs_ਬਾਹਰਲਾ.queued_ਗਿਣਤੀ >= context->bridge->ਹੱਦ){

		context->bridge->lazy_reconnect = true;
	}
#ਜੇ_ਅੰਤ

	ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	ਜੇ(update){
		rc = db_ਸੁਨੇਹਾ_write_inflight_ਬਾਹਰਲਾ_latest(context);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
		rc = db_ਸੁਨੇਹਾ_write_queued_ਬਾਹਰਲਾ(context);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
	}

	ਵਾਪਸ rc;
}


ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ db_ਸੁਨੇਹਾ_update_outgoing_state(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *ਸਿਰ,
		uint16_t mid, ਗਿਣਤੀ mosquitto_ਸੁਨੇਹਾ_state state, ਪੂਰਨ qos, bool persist)
{
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ;

	DL_FOREACH(ਸਿਰ, client_ਸੁਨੇਹਾ){
		ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.mid == mid){
			ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos != qos){
				log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos, qos);
				ਵਾਪਸ MOSQ_ERR_PROTOCOL;
			}
			client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state = (uint8_t)state;
			ਜੇ(persist){
				plugin_persist_handle_client_ਸੁਨੇਹਾ_update(context, client_ਸੁਨੇਹਾ);
			}
			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}
	}
	ਵਾਪਸ MOSQ_ERR_NOT_ਮਿਲਿਆ;
}


ਪੂਰਨ db_ਸੁਨੇਹਾ_update_outgoing(ਬਣਤਰ mosquitto *context, uint16_t mid, ਗਿਣਤੀ mosquitto_ਸੁਨੇਹਾ_state state, ਪੂਰਨ qos, bool persist)
{
	ਪੂਰਨ rc;

	rc = db_ਸੁਨੇਹਾ_update_outgoing_state(context, context->msgs_ਬਾਹਰਲਾ.inflight, mid, state, qos, persist);
	ਜੇ(!persist && rc == MOSQ_ERR_NOT_ਮਿਲਿਆ){
		rc = db_ਸੁਨੇਹਾ_update_outgoing_state(context, context->msgs_ਬਾਹਰਲਾ.queued, mid, state, qos, persist);
	}
	ਵਾਪਸ rc;
}


ਸਥਿਰ ਖਾਲੀ db__messages_delete_list(ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ **ਸਿਰ)
{
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ, *tmp;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(*ਸਿਰ, client_ਸੁਨੇਹਾ, tmp){
		DL_DELETE(*ਸਿਰ, client_ਸੁਨੇਹਾ);
		db_ਸੁਨੇਹਾ_store_ref_dec(&client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ);
		mosquitto_FREE(client_ਸੁਨੇਹਾ);
	}
	*ਸਿਰ = ਨਿਰਾ;
}


ਪੂਰਨ db__messages_delete_incoming(ਬਣਤਰ mosquitto *context)
{
	ਜੇ(!context){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_ਵਿੱਚ.inflight);
	db__messages_delete_list(&context->msgs_ਵਿੱਚ.queued);
	context->msgs_ਵਿੱਚ.inflight_ਬਾਈਟਾਂ = 0;
	context->msgs_ਵਿੱਚ.inflight_bytes12 = 0;
	context->msgs_ਵਿੱਚ.inflight_ਗਿਣਤੀ = 0;
	context->msgs_ਵਿੱਚ.inflight_count12 = 0;
	context->msgs_ਵਿੱਚ.queued_ਬਾਈਟਾਂ = 0;
	context->msgs_ਵਿੱਚ.queued_bytes12 = 0;
	context->msgs_ਵਿੱਚ.queued_ਗਿਣਤੀ = 0;
	context->msgs_ਵਿੱਚ.queued_count12 = 0;

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ db__messages_delete_outgoing(ਬਣਤਰ mosquitto *context)
{
	ਜੇ(!context){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_ਬਾਹਰਲਾ.inflight);
	db__messages_delete_list(&context->msgs_ਬਾਹਰਲਾ.queued);
	context->msgs_ਬਾਹਰਲਾ.inflight_ਬਾਈਟਾਂ = 0;
	context->msgs_ਬਾਹਰਲਾ.inflight_bytes12 = 0;
	context->msgs_ਬਾਹਰਲਾ.inflight_ਗਿਣਤੀ = 0;
	context->msgs_ਬਾਹਰਲਾ.inflight_count12 = 0;
	context->msgs_ਬਾਹਰਲਾ.queued_ਬਾਈਟਾਂ = 0;
	context->msgs_ਬਾਹਰਲਾ.queued_bytes12 = 0;
	context->msgs_ਬਾਹਰਲਾ.queued_ਗਿਣਤੀ = 0;
	context->msgs_ਬਾਹਰਲਾ.queued_count12 = 0;

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ db__messages_delete(ਬਣਤਰ mosquitto *context, bool force_free)
{
	ਜੇ(!context){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਜੇ(force_free || context->clean_ਸ਼ੁਰੂ || (context->bridge && context->bridge->clean_ਸ਼ੁਰੂ)){
		db__messages_delete_incoming(context);
	}

	ਜੇ(force_free || (context->bridge && context->bridge->clean_ਸ਼ੁਰੂ_local)
			|| (context->bridge == ਨਿਰਾ && context->clean_ਸ਼ੁਰੂ)){

		db__messages_delete_outgoing(context);
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ db_messages_easy_ਕਤਾਰ(ਬਣਤਰ mosquitto *context, ਅਚਲ ਅੱਖਰ *topic, uint8_t qos, uint32_t payloadlen, ਅਚਲ ਖਾਲੀ *payload, ਪੂਰਨ retain, uint32_t ਸੁਨੇਹਾ_expiry_interval, mosquitto_property **properties)
{
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ;
	ਅਚਲ ਅੱਖਰ *ਸਰੋਤ_id;
	ਗਿਣਤੀ mosquitto_ਸੁਨੇਹਾ_origin origin;

	ਜੇ(!topic){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਅਧਾਰ_ਸੁਨੇਹਾ = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ));
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic = mosquitto_strdup(topic);
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic == ਨਿਰਾ){
		db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos = qos;
	ਜੇ(db.config->retain_ਉਪਲਬਧ == false){
		ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain = 0;
	}ਨਹੀਂਤਾਂ{
		ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain = retain;
	}

	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen = payloadlen;
	ਜੇ(payloadlen > 0){
		ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload = mosquitto_malloc(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen+1);
		ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload == ਨਿਰਾ){
			db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload)[ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen] = 0;
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload, payload, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen);
	}

	ਜੇ(context && context->id){
		ਸਰੋਤ_id = context->id;
	}ਨਹੀਂਤਾਂ{
		ਸਰੋਤ_id = "";
	}
	ਜੇ(properties){
		ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.properties = *properties;
		*properties = ਨਿਰਾ;
	}

	ਜੇ(context){
		origin = mosq_mo_client;
	}ਨਹੀਂਤਾਂ{
		origin = mosq_mo_broker;
	}
	ਜੇ(db_ਸੁਨੇਹਾ_store(context, ਅਧਾਰ_ਸੁਨੇਹਾ, &ਸੁਨੇਹਾ_expiry_interval, origin)){
		ਵਾਪਸ 1;
	}

	ਵਾਪਸ sub_messages_ਕਤਾਰ(ਸਰੋਤ_id, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos, &ਅਧਾਰ_ਸੁਨੇਹਾ);
}


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOSQ_UUID_EPOCH 1637168273


/* db__new_msg_id() attempts to generate a new unique id on the broker, or a
 * number of brokers. It uses the 10-bit node ID, which can be set by plugins
 * to allow different brokers to share the same plugin persistence database
 * without overlapping one another.
 *
 * The message ID is a 64-bit unsigned integer arranged as follows:
 *
 * 10-bit ID  31-bit seconds                 23-bit fractional seconds
 * iiiiiiiiiisssssssssssssssssssssssssssssssnnnnnnnnnnnnnnnnnnnnnnn
 *
 * 10-bit ID gives a total of 1024 brokers can produce unique values (complete overkill)
 * 31-bit seconds gives a roll over date of 68 years after MOSQ_UUID_EPOCH - 2089.
 *    This roll over date would affect messages that have been queued waiting
 *    for a client to receive them, or retained messages only. If either of
 *    those remains for 68 years unchanged, then there will potentially be a
 *    collision. Ideally we need to ensure, however, that the message id is
 *    continually increasing for sorting purposes.
 * 23-bit fractional seconds gives a resolution of 120ns, or 8.4 million
 *    messages per second per broker.
 */
uint64_t db_new_ਸੁਨੇਹਾ_id(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
	FILETIME ftime;
	uint64_t ftime64;
#ਨਹੀਂਤਾਂ
	ਬਣਤਰ timespec ts;
#ਜੇ_ਅੰਤ
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	ਲੰਮਾ nsec;

	id = db.ਗੰਢ_id_shifted; /* Top 10-bits */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
	Get_System_ਸਮਾਂ_Precise_As_File_ਸਮਾਂ(&ftime);
	ftime64 = (((uint64_t)ftime.dw_ਉੱਚਾ_Date_ਸਮਾਂ)<<32) + ftime.dw_ਨੀਵਾਂ_Date_ਸਮਾਂ;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (ਲੰਮਾ)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#ਨਹੀਂਤਾਂ
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#ਜੇ_ਅੰਤ
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	ਜੇ(id <= db.ਆਖਰੀ_db_id){
		id = db.ਆਖਰੀ_db_id + 1;
	}
	db.ਆਖਰੀ_db_id = id;

	ਵਾਪਸ id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
ਪੂਰਨ db_ਸੁਨੇਹਾ_store(ਅਚਲ ਬਣਤਰ mosquitto *ਸਰੋਤ, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ, uint32_t *ਸੁਨੇਹਾ_expiry_interval, ਗਿਣਤੀ mosquitto_ਸੁਨੇਹਾ_origin origin)
{
	ਪੂਰਨ rc;

	ਪ੍ਰਤਿਜ੍ਞਾ(ਅਧਾਰ_ਸੁਨੇਹਾ);

	ਜੇ(ਸਰੋਤ && ਸਰੋਤ->id){
		ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_id = mosquitto_strdup(ਸਰੋਤ->id);
	}ਨਹੀਂਤਾਂ{
		ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_id = mosquitto_strdup("");
	}
	ਜੇ(!ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_id){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	ਜੇ(ਸਰੋਤ && ਸਰੋਤ->username){
		ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_username = mosquitto_strdup(ਸਰੋਤ->username);
		ਜੇ(!ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_username){
			db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
	}
	ਜੇ(ਸਰੋਤ){
		ਅਧਾਰ_ਸੁਨੇਹਾ->ਸਰੋਤ_listener = ਸਰੋਤ->listener;
	}
	ਅਧਾਰ_ਸੁਨੇਹਾ->origin = origin;
	ਜੇ(ਸੁਨੇਹਾ_expiry_interval){
		ਜੇ(*ਸੁਨੇਹਾ_expiry_interval > 0 && *ਸੁਨੇਹਾ_expiry_interval != ਸੁਨੇਹਾ_EXPIRY_INFINITE){
			ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.expiry_ਸਮਾਂ = db.now_real_s + *ਸੁਨੇਹਾ_expiry_interval;
		}ਨਹੀਂਤਾਂ{
			ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.expiry_ਸਮਾਂ = 0;
		}
	}

	ਅਧਾਰ_ਸੁਨੇਹਾ->dest_ids = ਨਿਰਾ;
	ਅਧਾਰ_ਸੁਨੇਹਾ->dest_id_ਗਿਣਤੀ = 0;
	db.ਸੁਨੇਹਾ_store_ਗਿਣਤੀ++;
	db.ਸੁਨੇਹਾ_store_ਬਾਈਟਾਂ += ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen;

	ਜੇ(!ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.store_id){
		ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.store_id = db_new_ਸੁਨੇਹਾ_id();
	}

	rc = db_ਸੁਨੇਹਾ_store_ਜੋੜੋ(ਅਧਾਰ_ਸੁਨੇਹਾ);
	ਜੇ(rc){
		db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
		ਵਾਪਸ rc;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ db_ਸੁਨੇਹਾ_store_ਲੱਭੋ(ਬਣਤਰ mosquitto *context, uint16_t mid, ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ **client_ਸੁਨੇਹਾ)
{
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *cmsg;

	*client_ਸੁਨੇਹਾ = ਨਿਰਾ;

	ਜੇ(!context){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_ਵਿੱਚ.inflight, cmsg){
		ਜੇ(cmsg->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid == mid){
			*client_ਸੁਨੇਹਾ = cmsg;
			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_ਵਿੱਚ.queued, cmsg){
		ਜੇ(cmsg->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid == mid){
			*client_ਸੁਨੇਹਾ = cmsg;
			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}
	}

	ਵਾਪਸ 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
ਸਥਿਰ ਪੂਰਨ db_ਸੁਨੇਹਾ_reconnect_reset_outgoing(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ, *tmp;

	context->msgs_ਬਾਹਰਲਾ.inflight_ਬਾਈਟਾਂ = 0;
	context->msgs_ਬਾਹਰਲਾ.inflight_bytes12 = 0;
	context->msgs_ਬਾਹਰਲਾ.inflight_ਗਿਣਤੀ = 0;
	context->msgs_ਬਾਹਰਲਾ.inflight_count12 = 0;
	context->msgs_ਬਾਹਰਲਾ.queued_ਬਾਈਟਾਂ = 0;
	context->msgs_ਬਾਹਰਲਾ.queued_bytes12 = 0;
	context->msgs_ਬਾਹਰਲਾ.queued_ਗਿਣਤੀ = 0;
	context->msgs_ਬਾਹਰਲਾ.queued_count12 = 0;
	context->msgs_ਬਾਹਰਲਾ.inflight_quota = context->msgs_ਬਾਹਰਲਾ.inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(context->msgs_ਬਾਹਰਲਾ.inflight, client_ਸੁਨੇਹਾ, tmp){
		db_ਸੁਨੇਹਾ_ਜੋੜੋ_ਮੰਜ਼ਿਲ_inflight_stats(&context->msgs_ਬਾਹਰਲਾ, client_ਸੁਨੇਹਾ);
		ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos > 0){
			util__decrement_send_quota(context);
		}

		ਚੋਣ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos){
			ਹਾਲਤ 0:
				client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state = mosq_ms_publish_qos0;
				ਰੋਕੋ;
			ਹਾਲਤ 1:
				client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state = mosq_ms_publish_qos1;
				ਰੋਕੋ;
			ਹਾਲਤ 2:
				ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state == mosq_ms_wait_for_pubcomp){
					client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state = mosq_ms_resend_pubrel;
				}ਨਹੀਂਤਾਂ{
					client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state = mosq_ms_publish_qos2;
				}
				ਰੋਕੋ;
		}
		plugin_persist_handle_client_ਸੁਨੇਹਾ_update(context, client_ਸੁਨੇਹਾ);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_ਸੁਰੱਖਿਅਤ(context->msgs_ਬਾਹਰਲਾ.queued, client_ਸੁਨੇਹਾ, tmp){
		db_ਸੁਨੇਹਾ_ਜੋੜੋ_ਮੰਜ਼ਿਲ_queued_stats(&context->msgs_ਬਾਹਰਲਾ, client_ਸੁਨੇਹਾ);
	}
	db_fill_inflight_ਬਾਹਰਲਾ_from_ਕਤਾਰ(context);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
ਸਥਿਰ ਪੂਰਨ db_ਸੁਨੇਹਾ_reconnect_reset_incoming(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ, *tmp;

	context->msgs_ਵਿੱਚ.inflight_ਬਾਈਟਾਂ = 0;
	context->msgs_ਵਿੱਚ.inflight_bytes12 = 0;
	context->msgs_ਵਿੱਚ.inflight_ਗਿਣਤੀ = 0;
	context->msgs_ਵਿੱਚ.inflight_count12 = 0;
	context->msgs_ਵਿੱਚ.queued_ਬਾਈਟਾਂ = 0;
	context->msgs_ਵਿੱਚ.queued_bytes12 = 0;
	context->msgs_ਵਿੱਚ.queued_ਗਿਣਤੀ = 0;
	context->msgs_ਵਿੱਚ.queued_count12 = 0;
	context->msgs_ਵਿੱਚ.inflight_quota = context->msgs_ਵਿੱਚ.inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(context->msgs_ਵਿੱਚ.inflight, client_ਸੁਨੇਹਾ, tmp){
		db_ਸੁਨੇਹਾ_ਜੋੜੋ_ਮੰਜ਼ਿਲ_inflight_stats(&context->msgs_ਵਿੱਚ, client_ਸੁਨੇਹਾ);
		ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos > 0){
			util__decrement_receive_quota(context);
		}

		ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db_ਸੁਨੇਹਾ_remove_inflight(context, &context->msgs_ਵਿੱਚ, client_ਸੁਨੇਹਾ);
		}ਨਹੀਂਤਾਂ{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_ਸੁਰੱਖਿਅਤ(context->msgs_ਵਿੱਚ.queued, client_ਸੁਨੇਹਾ, tmp){
		client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.dup = 0;
		db_ਸੁਨੇਹਾ_ਜੋੜੋ_ਮੰਜ਼ਿਲ_queued_stats(&context->msgs_ਵਿੱਚ, client_ਸੁਨੇਹਾ);
		ਜੇ(db__ready_for_flight(context, mosq_md_ਵਿੱਚ, client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos)){
			ਚੋਣ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos){
				ਹਾਲਤ 0:
					client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state = mosq_ms_publish_qos0;
					ਰੋਕੋ;
				ਹਾਲਤ 1:
					client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state = mosq_ms_publish_qos1;
					ਰੋਕੋ;
				ਹਾਲਤ 2:
					client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state = mosq_ms_publish_qos2;
					ਰੋਕੋ;
			}
			db_ਸੁਨੇਹਾ_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ_ਪਹਿਲਾ(context, &context->msgs_ਵਿੱਚ);
			plugin_persist_handle_client_ਸੁਨੇਹਾ_update(context, client_ਸੁਨੇਹਾ);
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ db_ਸੁਨੇਹਾ_reconnect_reset(ਬਣਤਰ mosquitto *context)
{
	ਪੂਰਨ rc;

	rc = db_ਸੁਨੇਹਾ_reconnect_reset_outgoing(context);
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}
	ਵਾਪਸ db_ਸੁਨੇਹਾ_reconnect_reset_incoming(context);
}


ਪੂਰਨ db_ਸੁਨੇਹਾ_remove_incoming(ਬਣਤਰ mosquitto *context, uint16_t mid)
{
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ, *tmp;

	ਜੇ(!context){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(context->msgs_ਵਿੱਚ.inflight, client_ਸੁਨੇਹਾ, tmp){
		ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.mid == mid){
			ਜੇ(client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos != 2){
				log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos);
				ਵਾਪਸ MOSQ_ERR_PROTOCOL;
			}
			db_ਸੁਨੇਹਾ_remove_inflight(context, &context->msgs_ਵਿੱਚ, client_ਸੁਨੇਹਾ);
			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}
	}

	ਵਾਪਸ MOSQ_ERR_NOT_ਮਿਲਿਆ;
}


ਪੂਰਨ db_ਸੁਨੇਹਾ_release_incoming(ਬਣਤਰ mosquitto *context, uint16_t mid)
{
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ, *tmp;
	bool deleted = false;
	ਪੂਰਨ rc;

	ਜੇ(!context){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(context->msgs_ਵਿੱਚ.inflight, client_ਸੁਨੇਹਾ, tmp){
		ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.mid == mid){
			ਜੇ(client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos != 2){
				log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos);
				ਵਾਪਸ MOSQ_ERR_PROTOCOL;
			}
			ਅਚਲ ਅੱਖਰ *topic = client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic;
			ਅਚਲ ਅੱਖਰ *ਸਰੋਤ_id = client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			ਜੇ(topic == ਨਿਰਾ){
				db_ਸੁਨੇਹਾ_remove_inflight(context, &context->msgs_ਵਿੱਚ, client_ਸੁਨੇਹਾ);
				deleted = true;
			}ਨਹੀਂਤਾਂ{
				rc = sub_messages_ਕਤਾਰ(ਸਰੋਤ_id, 2, &client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ);
				ਜੇ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db_ਸੁਨੇਹਾ_remove_inflight(context, &context->msgs_ਵਿੱਚ, client_ਸੁਨੇਹਾ);
					deleted = true;
				}ਨਹੀਂਤਾਂ{
					ਵਾਪਸ 1;
				}
			}
		}
	}

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(context->msgs_ਵਿੱਚ.queued, client_ਸੁਨੇਹਾ, tmp){
		ਜੇ(db__ready_for_flight(context, mosq_md_ਵਿੱਚ, client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos)){
			ਰੋਕੋ;
		}

		ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos == 2){
			send__pubrec(context, client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.mid, 0, ਨਿਰਾ);
			client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state = mosq_ms_wait_for_pubrel;
			db_ਸੁਨੇਹਾ_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ_ਪਹਿਲਾ(context, &context->msgs_ਵਿੱਚ);
			plugin_persist_handle_client_ਸੁਨੇਹਾ_update(context, client_ਸੁਨੇਹਾ);
		}
	}
	ਜੇ(deleted){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ MOSQ_ERR_NOT_ਮਿਲਿਆ;
	}
}


ਖਾਲੀ db__expire_all_messages(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ, *tmp;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(context->msgs_ਬਾਹਰਲਾ.inflight, client_ਸੁਨੇਹਾ, tmp){
		ਜੇ(client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.expiry_ਸਮਾਂ && db.now_real_s > client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.expiry_ਸਮਾਂ){
			ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos > 0){
				util__increment_send_quota(context);
			}
			db_ਸੁਨੇਹਾ_remove_inflight(context, &context->msgs_ਬਾਹਰਲਾ, client_ਸੁਨੇਹਾ);
		}
	}
	db_fill_inflight_ਬਾਹਰਲਾ_from_ਕਤਾਰ(context);
	DL_FOREACH_ਸੁਰੱਖਿਅਤ(context->msgs_ਬਾਹਰਲਾ.queued, client_ਸੁਨੇਹਾ, tmp){
		ਜੇ(client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.expiry_ਸਮਾਂ && db.now_real_s > client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.expiry_ਸਮਾਂ){
			db_ਸੁਨੇਹਾ_remove_queued(context, &context->msgs_ਬਾਹਰਲਾ, client_ਸੁਨੇਹਾ);
		}
	}
	DL_FOREACH_ਸੁਰੱਖਿਅਤ(context->msgs_ਵਿੱਚ.inflight, client_ਸੁਨੇਹਾ, tmp){
		ਜੇ(client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.expiry_ਸਮਾਂ && db.now_real_s > client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.expiry_ਸਮਾਂ){
			ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos > 0){
				util__increment_receive_quota(context);
			}
			db_ਸੁਨੇਹਾ_remove_inflight(context, &context->msgs_ਵਿੱਚ, client_ਸੁਨੇਹਾ);
		}
	}
	DL_FOREACH_ਸੁਰੱਖਿਅਤ(context->msgs_ਵਿੱਚ.queued, client_ਸੁਨੇਹਾ, tmp){
		ਜੇ(client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.expiry_ਸਮਾਂ && db.now_real_s > client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.expiry_ਸਮਾਂ){
			db_ਸੁਨੇਹਾ_remove_queued(context, &context->msgs_ਵਿੱਚ, client_ਸੁਨੇਹਾ);
		}
	}
}


ਸਥਿਰ ਖਾਲੀ db__client_messages_check_acl(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ *ਸੁਨੇਹਾ_ਅੰਕੜੇ, ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ **ਸਿਰ,
		ਖਾਲੀ (*decrement_stats_fn)(ਬਣਤਰ mosquitto_ਸੁਨੇਹਾ_ਅੰਕੜੇ *ਸੁਨੇਹਾ_ਅੰਕੜੇ, ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ))
{
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ, *tmp;
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ;
	ਪੂਰਨ access;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ((*ਸਿਰ), client_ਸੁਨੇਹਾ, tmp){
		ਅਧਾਰ_ਸੁਨੇਹਾ = client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ;
		ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.direction == mosq_md_ਬਾਹਰਲਾ){
			access = MOSQ_ACL_READ;
		}ਨਹੀਂਤਾਂ{
			access = MOSQ_ACL_WRITE;
		}
		ਜੇ(mosquitto_acl_check(context, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic,
				ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload,
				ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain,
				ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*ਸਿਰ), client_ਸੁਨੇਹਾ);
			decrement_stats_fn(ਸੁਨੇਹਾ_ਅੰਕੜੇ, client_ਸੁਨੇਹਾ);
			plugin_persist_handle_client_ਸੁਨੇਹਾ_delete(context, client_ਸੁਨੇਹਾ);
			db_ਸੁਨੇਹਾ_store_ref_dec(&client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ);
			mosquitto_FREE(client_ਸੁਨੇਹਾ);
		}
	}
}


ਖਾਲੀ db_check_acl_ਦਾ_all_messages(ਬਣਤਰ mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_ਵਿੱਚ, &context->msgs_ਵਿੱਚ.inflight, &db_ਸੁਨੇਹਾ_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_ਵਿੱਚ, &context->msgs_ਵਿੱਚ.queued, &db_ਸੁਨੇਹਾ_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_ਬਾਹਰਲਾ, &context->msgs_ਬਾਹਰਲਾ.inflight, &db_ਸੁਨੇਹਾ_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_ਬਾਹਰਲਾ, &context->msgs_ਬਾਹਰਲਾ.queued, &db_ਸੁਨੇਹਾ_remove_from_queued_stats);
}


ਸਥਿਰ ਪੂਰਨ db_ਸੁਨੇਹਾ_write_inflight_ਬਾਹਰਲਾ_single(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ)
{
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ;
	mosquitto_property *ਅਧਾਰ_ਸੁਨੇਹਾ_props = ਨਿਰਾ;
	ਪੂਰਨ rc;
	uint16_t mid;
	ਪੂਰਨ retries;
	ਪੂਰਨ retain;
	ਅਚਲ ਅੱਖਰ *topic;
	uint8_t qos;
	uint32_t payloadlen;
	ਅਚਲ ਖਾਲੀ *payload;
	uint32_t expiry_interval;
	uint32_t subscription_id;

	ਅਧਾਰ_ਸੁਨੇਹਾ = client_ਸੁਨੇਹਾ->ਅਧਾਰ_ਸੁਨੇਹਾ;

	expiry_interval = 0;
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.expiry_ਸਮਾਂ){
		ਜੇ(db.now_real_s > ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.expiry_ਸਮਾਂ){
			/* Message is expired, must not send. */
			ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.direction == mosq_md_ਬਾਹਰਲਾ && client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos > 0){
				util__increment_send_quota(context);
			}
			db_ਸੁਨੇਹਾ_remove_inflight(context, &context->msgs_ਬਾਹਰਲਾ, client_ਸੁਨੇਹਾ);
			db_fill_inflight_ਬਾਹਰਲਾ_from_ਕਤਾਰ(context);
			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}ਨਹੀਂਤਾਂ{
			expiry_interval = (uint32_t)(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.expiry_ਸਮਾਂ - db.now_real_s);
		}
	}
	mid = client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.mid;
	retries = client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.dup;
	retain = client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain;
	topic = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic;
	qos = (uint8_t)client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos;
	payloadlen = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen;
	payload = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload;
	subscription_id = client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.subscription_identifier;
	ਅਧਾਰ_ਸੁਨੇਹਾ_props = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.properties;

	ਚੋਣ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state){
		ਹਾਲਤ mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, ਅਧਾਰ_ਸੁਨੇਹਾ_props, expiry_interval);
			ਜੇ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_ਸੁਨੇਹਾ_remove_inflight(context, &context->msgs_ਬਾਹਰਲਾ, client_ਸੁਨੇਹਾ);
			}ਨਹੀਂਤਾਂ{
				ਵਾਪਸ rc;
			}
			ਰੋਕੋ;

		ਹਾਲਤ mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, ਅਧਾਰ_ਸੁਨੇਹਾ_props, expiry_interval);
			ਜੇ(rc == MOSQ_ERR_SUCCESS){
				client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.dup = 1; /* Any retry attempts are a duplicate. */
				client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_ਸੁਨੇਹਾ_update(context, client_ਸੁਨੇਹਾ);
			}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_ਸੁਨੇਹਾ_remove_inflight(context, &context->msgs_ਬਾਹਰਲਾ, client_ਸੁਨੇਹਾ);
			}ਨਹੀਂਤਾਂ{
				ਵਾਪਸ rc;
			}
			ਰੋਕੋ;

		ਹਾਲਤ mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, ਅਧਾਰ_ਸੁਨੇਹਾ_props, expiry_interval);
			ਜੇ(rc == MOSQ_ERR_SUCCESS){
				client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.dup = 1; /* Any retry attempts are a duplicate. */
				client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_ਸੁਨੇਹਾ_update(context, client_ਸੁਨੇਹਾ);
			}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_ਸੁਨੇਹਾ_remove_inflight(context, &context->msgs_ਬਾਹਰਲਾ, client_ਸੁਨੇਹਾ);
			}ਨਹੀਂਤਾਂ{
				ਵਾਪਸ rc;
			}
			ਰੋਕੋ;

		ਹਾਲਤ mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, ਨਿਰਾ);
			ਜੇ(!rc){
				client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_ਸੁਨੇਹਾ_update(context, client_ਸੁਨੇਹਾ);
			}ਨਹੀਂਤਾਂ{
				ਵਾਪਸ rc;
			}
			ਰੋਕੋ;

		ਹਾਲਤ mosq_ms_invalid:
		ਹਾਲਤ mosq_ms_send_pubrec:
		ਹਾਲਤ mosq_ms_resend_pubcomp:
		ਹਾਲਤ mosq_ms_wait_for_puback:
		ਹਾਲਤ mosq_ms_wait_for_pubrec:
		ਹਾਲਤ mosq_ms_wait_for_pubrel:
		ਹਾਲਤ mosq_ms_wait_for_pubcomp:
		ਹਾਲਤ mosq_ms_queued:
			ਰੋਕੋ;
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ db_ਸੁਨੇਹਾ_write_inflight_ਬਾਹਰਲਾ_all(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ, *tmp;
	ਪੂਰਨ rc;

	ਜੇ(context->state != mosq_cs_ਸਰਗਰਮ || !net__is_connected(context)){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(context->msgs_ਬਾਹਰਲਾ.inflight, client_ਸੁਨੇਹਾ, tmp){
		rc = db_ਸੁਨੇਹਾ_write_inflight_ਬਾਹਰਲਾ_single(context, client_ਸੁਨੇਹਾ);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ db_ਸੁਨੇਹਾ_write_inflight_ਬਾਹਰਲਾ_latest(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ, *ਅਗਲਾ;
	ਪੂਰਨ rc;

	ਜੇ(context->state != mosq_cs_ਸਰਗਰਮ
			|| !net__is_connected(context)
			|| context->msgs_ਬਾਹਰਲਾ.inflight == ਨਿਰਾ){

		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	ਜੇ(context->msgs_ਬਾਹਰਲਾ.inflight->prev == context->msgs_ਬਾਹਰਲਾ.inflight){
		/* Only one message */
		ਵਾਪਸ db_ਸੁਨੇਹਾ_write_inflight_ਬਾਹਰਲਾ_single(context, context->msgs_ਬਾਹਰਲਾ.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_ਸੁਨੇਹਾ = context->msgs_ਬਾਹਰਲਾ.inflight->prev;
	ਜਦੋਂ(client_ਸੁਨੇਹਾ != context->msgs_ਬਾਹਰਲਾ.inflight &&
			(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state == mosq_ms_publish_qos0
			|| client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state == mosq_ms_publish_qos1
			|| client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state == mosq_ms_publish_qos2)){

		client_ਸੁਨੇਹਾ = client_ਸੁਨੇਹਾ->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	ਜੇ(client_ਸੁਨੇਹਾ != context->msgs_ਬਾਹਰਲਾ.inflight){
		client_ਸੁਨੇਹਾ = client_ਸੁਨੇਹਾ->ਅਗਲਾ;
	}

	ਜਦੋਂ(client_ਸੁਨੇਹਾ){
		ਅਗਲਾ = client_ਸੁਨੇਹਾ->ਅਗਲਾ;
		rc = db_ਸੁਨੇਹਾ_write_inflight_ਬਾਹਰਲਾ_single(context, client_ਸੁਨੇਹਾ);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
		client_ਸੁਨੇਹਾ = ਅਗਲਾ;
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ db_ਸੁਨੇਹਾ_write_queued_ਵਿੱਚ(ਬਣਤਰ mosquitto *context)
{
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *client_ਸੁਨੇਹਾ, *tmp;
	ਪੂਰਨ rc;

	ਜੇ(context->state != mosq_cs_ਸਰਗਰਮ){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(context->msgs_ਵਿੱਚ.queued, client_ਸੁਨੇਹਾ, tmp){
		ਜੇ(context->msgs_ਵਿੱਚ.inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ != 0 && context->msgs_ਵਿੱਚ.inflight_quota == 0){
			ਰੋਕੋ;
		}

		ਜੇ(client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos == 2){
			client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state = mosq_ms_send_pubrec;
			db_ਸੁਨੇਹਾ_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ_ਪਹਿਲਾ(context, &context->msgs_ਵਿੱਚ);
			rc = send__pubrec(context, client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.mid, 0, ਨਿਰਾ);
			ਜੇ(!rc){
				client_ਸੁਨੇਹਾ->ਅੰਕੜੇ.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_ਸੁਨੇਹਾ_update(context, client_ਸੁਨੇਹਾ);
			}ਨਹੀਂਤਾਂ{
				plugin_persist_handle_client_ਸੁਨੇਹਾ_update(context, client_ਸੁਨੇਹਾ);
				ਵਾਪਸ rc;
			}
		}
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ db_ਸੁਨੇਹਾ_write_queued_ਬਾਹਰਲਾ(ਬਣਤਰ mosquitto *context)
{
	ਜੇ(context->state != mosq_cs_ਸਰਗਰਮ){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	db_fill_inflight_ਬਾਹਰਲਾ_from_ਕਤਾਰ(context);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}
