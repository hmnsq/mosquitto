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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <utlist.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"


/**
 * Is this context ready to take more in flight messages right now?
 * @param context the client context of interest
 * @param qos qos for the packet of interest
 * @return true if more in flight are allowed.
 */
bool db__ready_for_flight(ಸಂರಚನಾ mosquitto *context, ಗಣನಾ mosquitto_ಸಂದೇಶ_direction dir, ಪೂರ್ಣಾಙ್ಕ qos)
{
	ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data *msgs;
	bool valid_bytes;
	bool valid_ಎಣಿಕೆ;

	ಯದಿ(dir == mosq_md_out){
		msgs = &context->msgs_out;
	}ಅನ್ಯಥಾ{
		msgs = &context->msgs_in;
	}

	ಯದಿ(msgs->inflight_maximum == 0 && db.config->max_inflight_bytes == 0){
		ಪ್ರತ್ಯಾಗಮ true;
	}

	ಯದಿ(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		ಯದಿ(db.config->max_queued_messages == 0 && db.config->max_inflight_bytes == 0){
			ಪ್ರತ್ಯಾಗಮ true;
		}
		valid_bytes = ((msgs->inflight_bytes - (ssize_t)db.config->max_inflight_bytes) < (ssize_t)db.config->max_queued_bytes);
		ಯದಿ(dir == mosq_md_out){
			valid_ಎಣಿಕೆ = context->out_packet_ಎಣಿಕೆ < db.config->max_queued_messages;
		}ಅನ್ಯಥಾ{
			valid_ಎಣಿಕೆ = msgs->inflight_ಎಣಿಕೆ - msgs->inflight_maximum < db.config->max_queued_messages;
		}

		ಯದಿ(db.config->max_queued_messages == 0){
			ಪ್ರತ್ಯಾಗಮ valid_bytes;
		}
		ಯದಿ(db.config->max_queued_bytes == 0){
			ಪ್ರತ್ಯಾಗಮ valid_ಎಣಿಕೆ;
		}
	}ಅನ್ಯಥಾ{
		valid_bytes = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_bytes;
		valid_ಎಣಿಕೆ = msgs->inflight_quota > 0;

		ಯದಿ(msgs->inflight_maximum == 0){
			ಪ್ರತ್ಯಾಗಮ valid_bytes;
		}
		ಯದಿ(db.config->max_inflight_bytes == 0){
			ಪ್ರತ್ಯಾಗಮ valid_ಎಣಿಕೆ;
		}
	}

	ಪ್ರತ್ಯಾಗಮ valid_bytes && valid_ಎಣಿಕೆ;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db__ready_for_queue(ಸಂರಚನಾ mosquitto *context, ಪೂರ್ಣಾಙ್ಕ qos, ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data *ಸಂದೇಶ_data)
{
	ಪೂರ್ಣಾಙ್ಕ source_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಙ್ಕ adjust_ಎಣಿಕೆ;
	long source_bytes;
	ssize_t adjust_bytes = (ssize_t)db.config->max_inflight_bytes;
	bool valid_bytes;
	bool valid_ಎಣಿಕೆ;

	ಯದಿ(db.config->max_queued_messages == 0 && db.config->max_queued_bytes == 0){
		ಪ್ರತ್ಯಾಗಮ true;
	}

	ಯದಿ(qos == 0 && db.config->queue_qos0_messages == false){
		ಪ್ರತ್ಯಾಗಮ false; /* This case is handled in db__ready_for_flight() */
	}ಅನ್ಯಥಾ{
		source_bytes = (ssize_t)ಸಂದೇಶ_data->queued_bytes12;
		source_ಎಣಿಕೆ = ಸಂದೇಶ_data->queued_count12;
	}
	adjust_ಎಣಿಕೆ = ಸಂದೇಶ_data->inflight_maximum;

	/* nothing in flight for offline clients */
	ಯದಿ(!net__is_connected(context)){
		adjust_bytes = 0;
		adjust_ಎಣಿಕೆ = 0;
	}

	valid_bytes = (source_bytes - (ssize_t)adjust_bytes) < (ssize_t)db.config->max_queued_bytes;
	valid_ಎಣಿಕೆ = source_ಎಣಿಕೆ - adjust_ಎಣಿಕೆ < db.config->max_queued_messages;

	ಯದಿ(db.config->max_queued_bytes == 0){
		ಪ್ರತ್ಯಾಗಮ valid_ಎಣಿಕೆ;
	}
	ಯದಿ(db.config->max_queued_messages == 0){
		ಪ್ರತ್ಯಾಗಮ valid_bytes;
	}

	ಪ್ರತ್ಯಾಗಮ valid_bytes && valid_ಎಣಿಕೆ;
}


ಶೂನ್ಯ db_ಸಂದೇಶ_add_to_inflight_stats(ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data *ಸಂದೇಶ_data, ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ)
{
	ಸಂದೇಶ_data->inflight_ಎಣಿಕೆ++;
	ಸಂದೇಶ_data->inflight_bytes += client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.payloadlen;
	ಯದಿ(client_ಸಂದೇಶ->data.qos != 0){
		ಸಂದೇಶ_data->inflight_count12++;
		ಸಂದೇಶ_data->inflight_bytes12 += client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.payloadlen;
	}
}


ಸ್ಥಿರ ಶೂನ್ಯ db_ಸಂದೇಶ_remove_from_inflight_stats(ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data *ಸಂದೇಶ_data, ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ)
{
	ಸಂದೇಶ_data->inflight_ಎಣಿಕೆ--;
	ಸಂದೇಶ_data->inflight_bytes -= client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.payloadlen;
	ಯದಿ(client_ಸಂದೇಶ->data.qos != 0){
		ಸಂದೇಶ_data->inflight_count12--;
		ಸಂದೇಶ_data->inflight_bytes12 -= client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.payloadlen;
	}
}


ಶೂನ್ಯ db_ಸಂದೇಶ_add_to_queued_stats(ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data *ಸಂದೇಶ_data, ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ)
{
	ಸಂದೇಶ_data->queued_ಎಣಿಕೆ++;
	ಸಂದೇಶ_data->queued_bytes += client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.payloadlen;
	ಯದಿ(client_ಸಂದೇಶ->data.qos != 0){
		ಸಂದೇಶ_data->queued_count12++;
		ಸಂದೇಶ_data->queued_bytes12 += client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.payloadlen;
	}
}


ಸ್ಥಿರ ಶೂನ್ಯ db_ಸಂದೇಶ_remove_from_queued_stats(ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data *ಸಂದೇಶ_data, ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ)
{
	ಸಂದೇಶ_data->queued_ಎಣಿಕೆ--;
	ಸಂದೇಶ_data->queued_bytes -= client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.payloadlen;
	ಯದಿ(client_ಸಂದೇಶ->data.qos != 0){
		ಸಂದೇಶ_data->queued_count12--;
		ಸಂದೇಶ_data->queued_bytes12 -= client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.payloadlen;
	}
}


ಪೂರ್ಣಾಙ್ಕ db__open(ಸಂರಚನಾ mosquitto__config *config)
{
	ಯದಿ(!config){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = ರಿಕ್ತ;
	db.contexts_by_sock = ರಿಕ್ತ;
	db.contexts_for_free = ರಿಕ್ತ;
#ifdef WITH_BRIDGE
	db.bridges = ರಿಕ್ತ;
	db.bridge_ಎಣಿಕೆ = 0;
#endif

	/* Initialize the hashtable */
	db.clientid_index_hash = ರಿಕ್ತ;

	db.normal_subs = ರಿಕ್ತ;
	db.shared_subs = ರಿಕ್ತ;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = ರಿಕ್ತ;

#ifdef WITH_PERSISTENCE
	ಯದಿ(persist__restore()){
		ಪ್ರತ್ಯಾಗಮ 1;
	}
#endif

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಶೂನ್ಯ subhier_clean(ಸಂರಚನಾ mosquitto__subhier **subhier)
{
	ಸಂರಚನಾ mosquitto__subhier *peer, *subhier_tmp;
	ಸಂರಚನಾ mosquitto__subleaf *leaf, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		leaf = peer->subs;
		ಯಾವತ್(leaf){
			nextleaf = leaf->next;
			mosquitto_FREE(leaf);
			leaf = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


ಪೂರ್ಣಾಙ್ಕ db__close(ಶೂನ್ಯ)
{
	subhier_clean(&db.normal_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db_ಸಂದೇಶ_store_clean();

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ db_ಸಂದೇಶ_store_add(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ)
{
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *found;
	unsigned hashv;

	HASH_ಮೌಲ್ಯ(&base_ಸಂದೇಶ->data.store_id, ಪರಿಮಾಣ(base_ಸಂದೇಶ->data.store_id), hashv);
	HASH_FIND_BYHASHVALUE(hh, db.ಸಂದೇಶ_store, &base_ಸಂದೇಶ->data.store_id, ಪರಿಮಾಣ(base_ಸಂದೇಶ->data.store_id), hashv, found);
	ಯದಿ(found == ರಿಕ್ತ){
		HASH_ADD_KEYPTR_BYHASHVALUE(hh, db.ಸಂದೇಶ_store, &base_ಸಂದೇಶ->data.store_id, ಪರಿಮಾಣ(base_ಸಂದೇಶ->data.store_id), hashv, base_ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ALREADY_EXISTS;
	}
}


ಶೂನ್ಯ db_ಸಂದೇಶ_store_free(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ)
{
	mosquitto_FREE(base_ಸಂದೇಶ->data.source_id);
	mosquitto_FREE(base_ಸಂದೇಶ->data.source_username);
	ಯದಿ(base_ಸಂದೇಶ->dest_ids){
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<base_ಸಂದೇಶ->dest_id_ಎಣಿಕೆ; i++){
			mosquitto_FREE(base_ಸಂದೇಶ->dest_ids[i]);
		}
		mosquitto_FREE(base_ಸಂದೇಶ->dest_ids);
	}
	mosquitto_FREE(base_ಸಂದೇಶ->data.topic);
	mosquitto_property_free_all(&base_ಸಂದೇಶ->data.properties);
	mosquitto_FREE(base_ಸಂದೇಶ->data.payload);
	mosquitto_FREE(base_ಸಂದೇಶ);
}


ಶೂನ್ಯ db_ಸಂದೇಶ_store_remove(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ, bool notify)
{
	ಯದಿ(base_ಸಂದೇಶ == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ;
	}
	HASH_DELETE(hh, db.ಸಂದೇಶ_store, base_ಸಂದೇಶ);
	db.ಸಂದೇಶ_store_ಎಣಿಕೆ--;
	db.ಸಂದೇಶ_store_bytes -= base_ಸಂದೇಶ->data.payloadlen;
	ಯದಿ(notify == true){
		plugin_persist_handle_base_ಸಂದೇಶ_delete(base_ಸಂದೇಶ);
	}
	db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
}


ಶೂನ್ಯ db_ಸಂದೇಶ_store_clean(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ, *base_ಸಂದೇಶ_tmp;

	HASH_ITER(hh, db.ಸಂದೇಶ_store, base_ಸಂದೇಶ, base_ಸಂದೇಶ_tmp){
		db_ಸಂದೇಶ_store_remove(base_ಸಂದೇಶ, false);
	}
}


ಶೂನ್ಯ db_ಸಂದೇಶ_store_ref_inc(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ)
{
	base_ಸಂದೇಶ->ref_ಎಣಿಕೆ++;
}


ಶೂನ್ಯ db_ಸಂದೇಶ_store_ref_dec(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ **base_ಸಂದೇಶ)
{
	(*base_ಸಂದೇಶ)->ref_ಎಣಿಕೆ--;
	ಯದಿ((*base_ಸಂದೇಶ)->ref_ಎಣಿಕೆ == 0){
		db_ಸಂದೇಶ_store_remove(*base_ಸಂದೇಶ, true);
		*base_ಸಂದೇಶ = ರಿಕ್ತ;
	}
}


ಶೂನ್ಯ db_ಸಂದೇಶ_store_compact(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ, *base_ಸಂದೇಶ_tmp;

	HASH_ITER(hh, db.ಸಂದೇಶ_store, base_ಸಂದೇಶ, base_ಸಂದೇಶ_tmp){
		ಯದಿ(base_ಸಂದೇಶ->ref_ಎಣಿಕೆ < 1){
			db_ಸಂದೇಶ_store_remove(base_ಸಂದೇಶ, true);
		}
	}
}


ಸ್ಥಿರ ಶೂನ್ಯ db__message_remove_inflight(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data *ಸಂದೇಶ_data, ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *item)
{
	ಯದಿ(!context || !ಸಂದೇಶ_data || !item){
		ಪ್ರತ್ಯಾಗಮ;
	}

	plugin_persist_handle_client_ಸಂದೇಶ_delete(context, item);

	DL_DELETE(ಸಂದೇಶ_data->inflight, item);
	ಯದಿ(item->base_ಸಂದೇಶ){
		db_ಸಂದೇಶ_remove_from_inflight_stats(ಸಂದೇಶ_data, item);
		db_ಸಂದೇಶ_store_ref_dec(&item->base_ಸಂದೇಶ);
	}

	mosquitto_FREE(item);
}


ಸ್ಥಿರ ಶೂನ್ಯ db__message_remove_queued(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data *ಸಂದೇಶ_data, ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *item)
{
	ಯದಿ(!context || !ಸಂದೇಶ_data || !item){
		ಪ್ರತ್ಯಾಗಮ;
	}

	plugin_persist_handle_client_ಸಂದೇಶ_delete(context, item);

	DL_DELETE(ಸಂದೇಶ_data->queued, item);
	ಯದಿ(item->base_ಸಂದೇಶ){
		db_ಸಂದೇಶ_remove_from_queued_stats(ಸಂದೇಶ_data, item);
		db_ಸಂದೇಶ_store_ref_dec(&item->base_ಸಂದೇಶ);
	}

	mosquitto_FREE(item);
}


ಸ್ಥಿರ ಶೂನ್ಯ db__fill_inflight_out_from_queue(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.queued, client_ಸಂದೇಶ, tmp){
		ಯದಿ(!db__ready_for_flight(context, mosq_md_out, client_ಸಂದೇಶ->data.qos)){
			ಪ್ರತ್ಯಾಗಮ;
		}
		ವಿಕಲ್ಪ(client_ಸಂದೇಶ->data.qos){
			ಪಕ್ಷ 0:
				client_ಸಂದೇಶ->data.state = mosq_ms_publish_qos0;
				ಭಙ್ಗ;
			ಪಕ್ಷ 1:
				client_ಸಂದೇಶ->data.state = mosq_ms_publish_qos1;
				ಭಙ್ಗ;
			ಪಕ್ಷ 2:
				client_ಸಂದೇಶ->data.state = mosq_ms_publish_qos2;
				ಭಙ್ಗ;
		}
		ಯದಿ(client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.expiry_time && db.now_real_s > client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_ಸಂದೇಶ);
			ಅನುವರ್ತ;
		}
		plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
		db__message_dequeue_first(context, &context->msgs_out);
	}
}


ಶೂನ್ಯ db__message_dequeue_first(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data *ಸಂದೇಶ_data)
{
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ;

	UNUSED(context);

	client_ಸಂದೇಶ = ಸಂದೇಶ_data->queued;
	DL_DELETE(ಸಂದೇಶ_data->queued, client_ಸಂದೇಶ);
	DL_APPEND(ಸಂದೇಶ_data->inflight, client_ಸಂದೇಶ);
	ಯದಿ(ಸಂದೇಶ_data->inflight_quota > 0){
		ಸಂದೇಶ_data->inflight_quota--;
	}

	db_ಸಂದೇಶ_remove_from_queued_stats(ಸಂದೇಶ_data, client_ಸಂದೇಶ);
	db_ಸಂದೇಶ_add_to_inflight_stats(ಸಂದೇಶ_data, client_ಸಂದೇಶ);
}


ಪೂರ್ಣಾಙ್ಕ db__message_delete_outgoing(ಸಂರಚನಾ mosquitto *context, uint16_t mid, ಗಣನಾ mosquitto_ಸಂದೇಶ_state expect_state, ಪೂರ್ಣಾಙ್ಕ qos)
{
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;
	bool deleted = false;

	ಯದಿ(!context){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_ಸಂದೇಶ, tmp){
		ಯದಿ(client_ಸಂದೇಶ->data.mid == mid){
			ಯದಿ(client_ಸಂದೇಶ->data.qos != qos){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_ಸಂದೇಶ->data.qos, qos);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
			}ಅನ್ಯಥಾ ಯದಿ(qos == 2 && client_ಸಂದೇಶ->data.state != expect_state && expect_state != mosq_ms_any){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_ಸಂದೇಶ->data.state, expect_state);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_out, client_ಸಂದೇಶ);
			deleted = true;
			ಭಙ್ಗ;
		}
	}

	ಯದಿ(deleted == false){
		DL_FOREACH_SAFE(context->msgs_out.queued, client_ಸಂದೇಶ, tmp){
			ಯದಿ(client_ಸಂದೇಶ->data.mid == mid){
				ಯದಿ(client_ಸಂದೇಶ->data.qos != qos){
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
				}ಅನ್ಯಥಾ ಯದಿ(qos == 2 && client_ಸಂದೇಶ->data.state != expect_state && expect_state != mosq_ms_any){
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
				}
				db__message_remove_queued(context, &context->msgs_out, client_ಸಂದೇಶ);
				ಭಙ್ಗ;
			}
		}
	}
	db__fill_inflight_out_from_queue(context);
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	ಪ್ರತ್ಯಾಗಮ db__message_write_inflight_out_latest(context);
}


/* Only for QoS 2 messages */
ಪೂರ್ಣಾಙ್ಕ db_message_ಸೇರಿಸು_incoming(ಸಂರಚನಾ mosquitto *context, uint64_t cmsg_id, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ, bool persist)
{
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ;
	ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data *ಸಂದೇಶ_data;
	ಗಣನಾ mosquitto_ಸಂದೇಶ_state state = mosq_ms_invalid;
	ಪೂರ್ಣಾಙ್ಕ rc = 0;

	ಪ್ರತಿಜ್ಞಾ(base_ಸಂದೇಶ);
	ಯದಿ(!context){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಯದಿ(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;

	}
	ಸಂದೇಶ_data = &context->msgs_in;

	ಯದಿ(db__ready_for_flight(context, mosq_md_in, base_ಸಂದೇಶ->data.qos)){
		state = mosq_ms_wait_for_pubrel;
	}ಅನ್ಯಥಾ ಯದಿ(base_ಸಂದೇಶ->data.qos != 0 && db__ready_for_queue(context, base_ಸಂದೇಶ->data.qos, ಸಂದೇಶ_data)){
		state = mosq_ms_queued;
		rc = 2;
	}ಅನ್ಯಥಾ{
		/* Dropping message due to full queue. */
		ಯದಿ(context->is_dropping == false){
			context->is_dropping = true;
			log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE,
					"Outgoing messages are being dropped for client %s.",
					context->id);
		}
		metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		ಪ್ರತ್ಯಾಗಮ 2;
	}

	ಪ್ರತಿಜ್ಞಾ(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	ಯದಿ(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_ಸಂದೇಶ = mosquitto_malloc(ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ));
	ಯದಿ(!client_ಸಂದೇಶ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	client_ಸಂದೇಶ->prev = ರಿಕ್ತ;
	client_ಸಂದೇಶ->next = ರಿಕ್ತ;
	ಯದಿ(cmsg_id){
		client_ಸಂದೇಶ->data.cmsg_id = cmsg_id;
	}ಅನ್ಯಥಾ{
		client_ಸಂದೇಶ->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_ಸಂದೇಶ->base_ಸಂದೇಶ = base_ಸಂದೇಶ;
	db_ಸಂದೇಶ_store_ref_inc(client_ಸಂದೇಶ->base_ಸಂದೇಶ);
	client_ಸಂದೇಶ->data.mid = base_ಸಂದೇಶ->data.source_mid;
	client_ಸಂದೇಶ->data.direction = mosq_md_in;
	client_ಸಂದೇಶ->data.state = (uint8_t)state;
	client_ಸಂದೇಶ->data.dup = false;
	ಯದಿ(base_ಸಂದೇಶ->data.qos > context->max_qos){
		client_ಸಂದೇಶ->data.qos = context->max_qos;
	}ಅನ್ಯಥಾ{
		client_ಸಂದೇಶ->data.qos = base_ಸಂದೇಶ->data.qos;
	}
	client_ಸಂದೇಶ->data.retain = base_ಸಂದೇಶ->data.retain;
	client_ಸಂದೇಶ->data.subscription_identifier = 0;

	ಯದಿ(state == mosq_ms_queued){
		DL_APPEND(ಸಂದೇಶ_data->queued, client_ಸಂದೇಶ);
		db_ಸಂದೇಶ_add_to_queued_stats(ಸಂದೇಶ_data, client_ಸಂದೇಶ);
	}ಅನ್ಯಥಾ{
		DL_APPEND(ಸಂದೇಶ_data->inflight, client_ಸಂದೇಶ);
		db_ಸಂದೇಶ_add_to_inflight_stats(ಸಂದೇಶ_data, client_ಸಂದೇಶ);
	}

	ಯದಿ(persist && context->is_persisted){
		plugin_persist_handle_base_ಸಂದೇಶ_add(client_ಸಂದೇಶ->base_ಸಂದೇಶ);
		plugin_persist_handle_client_ಸಂದೇಶ_add(context, client_ಸಂದೇಶ);
	}

	ಯದಿ(client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.qos > 0){
		util__decrement_receive_quota(context);
	}
	ಪ್ರತ್ಯಾಗಮ rc;
}


ಪೂರ್ಣಾಙ್ಕ db_message_ಸೇರಿಸು_outgoing(ಸಂರಚನಾ mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ, uint32_t subscription_identifier, bool update, bool persist)
{
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ;
	ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data *ಸಂದೇಶ_data;
	ಗಣನಾ mosquitto_ಸಂದೇಶ_state state = mosq_ms_invalid;
	ಪೂರ್ಣಾಙ್ಕ rc = 0;
	ವರ್ಣ **dest_ids;

	ಪ್ರತಿಜ್ಞಾ(base_ಸಂದೇಶ);
	ಯದಿ(!context){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಯದಿ(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_sent++;

	ಸಂದೇಶ_data = &context->msgs_out;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	ಯದಿ(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && base_ಸಂದೇಶ->dest_ids){

		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<base_ಸಂದೇಶ->dest_id_ಎಣಿಕೆ; i++){
			ಯದಿ(base_ಸಂದೇಶ->dest_ids[i] && !strcmp(base_ಸಂದೇಶ->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
			}
		}
	}
	ಯದಿ(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		ಯದಿ(qos == 0 && !db.config->queue_qos0_messages){
			ಯದಿ(!context->bridge){
				ಪ್ರತ್ಯಾಗಮ 2;
			}ಅನ್ಯಥಾ{
				ಯದಿ(context->bridge->start_type != bst_lazy){
					ಪ್ರತ್ಯಾಗಮ 2;
				}
			}
		}
		ಯದಿ(context->bridge && context->bridge->clean_start_local == true){
			ಪ್ರತ್ಯಾಗಮ 2;
		}
	}

	ಯದಿ(net__is_connected(context)){
		ಯದಿ(db__ready_for_flight(context, mosq_md_out, qos)){
			ವಿಕಲ್ಪ(qos){
				ಪಕ್ಷ 0:
					state = mosq_ms_publish_qos0;
					ಭಙ್ಗ;
				ಪಕ್ಷ 1:
					state = mosq_ms_publish_qos1;
					ಭಙ್ಗ;
				ಪಕ್ಷ 2:
					state = mosq_ms_publish_qos2;
					ಭಙ್ಗ;
			}
		}ಅನ್ಯಥಾ ಯದಿ(qos != 0 && db__ready_for_queue(context, qos, ಸಂದೇಶ_data)){
			state = mosq_ms_queued;
			rc = 2;
		}ಅನ್ಯಥಾ{
			/* Dropping message due to full queue. */
			ಯದಿ(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			ಪ್ರತ್ಯಾಗಮ 2;
		}
	}ಅನ್ಯಥಾ{
		ಯದಿ(db__ready_for_queue(context, qos, ಸಂದೇಶ_data)){
			state = mosq_ms_queued;
		}ಅನ್ಯಥಾ{
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			ಯದಿ(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			ಪ್ರತ್ಯಾಗಮ 2;
		}
	}
	ಪ್ರತಿಜ್ಞಾ(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	ಯದಿ(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_ಸಂದೇಶ = mosquitto_malloc(ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ));
	ಯದಿ(!client_ಸಂದೇಶ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	client_ಸಂದೇಶ->prev = ರಿಕ್ತ;
	client_ಸಂದೇಶ->next = ರಿಕ್ತ;
	ಯದಿ(cmsg_id){
		client_ಸಂದೇಶ->data.cmsg_id = cmsg_id;
	}ಅನ್ಯಥಾ{
		client_ಸಂದೇಶ->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_ಸಂದೇಶ->base_ಸಂದೇಶ = base_ಸಂದೇಶ;
	db_ಸಂದೇಶ_store_ref_inc(client_ಸಂದೇಶ->base_ಸಂದೇಶ);
	client_ಸಂದೇಶ->data.mid = mid;
	client_ಸಂದೇಶ->data.direction = mosq_md_out;
	client_ಸಂದೇಶ->data.state = (uint8_t)state;
	client_ಸಂದೇಶ->data.dup = false;
	ಯದಿ(qos > context->max_qos){
		client_ಸಂದೇಶ->data.qos = context->max_qos;
	}ಅನ್ಯಥಾ{
		client_ಸಂದೇಶ->data.qos = qos;
	}
	client_ಸಂದೇಶ->data.retain = retain;
	client_ಸಂದೇಶ->data.subscription_identifier = subscription_identifier;

	ಯದಿ(state == mosq_ms_queued){
		DL_APPEND(ಸಂದೇಶ_data->queued, client_ಸಂದೇಶ);
		db_ಸಂದೇಶ_add_to_queued_stats(ಸಂದೇಶ_data, client_ಸಂದೇಶ);
	}ಅನ್ಯಥಾ{
		DL_APPEND(ಸಂದೇಶ_data->inflight, client_ಸಂದೇಶ);
		db_ಸಂದೇಶ_add_to_inflight_stats(ಸಂದೇಶ_data, client_ಸಂದೇಶ);
	}

	ಯದಿ(persist && context->is_persisted){
		plugin_persist_handle_base_ಸಂದೇಶ_add(client_ಸಂದೇಶ->base_ಸಂದೇಶ);
		plugin_persist_handle_client_ಸಂದೇಶ_add(context, client_ಸಂದೇಶ);
	}

	ಯದಿ(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(base_ಸಂದೇಶ->dest_ids, ಪರಿಮಾಣ(ವರ್ಣ *)*(size_t)(base_ಸಂದೇಶ->dest_id_ಎಣಿಕೆ+1));
		ಯದಿ(dest_ids){
			base_ಸಂದೇಶ->dest_ids = dest_ids;
			base_ಸಂದೇಶ->dest_id_ಎಣಿಕೆ++;
			base_ಸಂದೇಶ->dest_ids[base_ಸಂದೇಶ->dest_id_ಎಣಿಕೆ-1] = mosquitto_strdup(context->id);
			ಯದಿ(!base_ಸಂದೇಶ->dest_ids[base_ಸಂದೇಶ->dest_id_ಎಣಿಕೆ-1]){
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
		}ಅನ್ಯಥಾ{
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
	}
#ifdef WITH_BRIDGE
	ಯದಿ(context->bridge && context->bridge->start_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_out.inflight_ಎಣಿಕೆ + context->msgs_out.queued_ಎಣಿಕೆ >= context->bridge->threshold){

		context->bridge->lazy_reconnect = true;
	}
#endif

	ಯದಿ(client_ಸಂದೇಶ->data.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	ಯದಿ(update){
		rc = db__message_write_inflight_out_latest(context);
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
		rc = db__message_write_queued_out(context);
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	ಪ್ರತ್ಯಾಗಮ rc;
}


ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ db__message_update_outgoing_state(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *head,
		uint16_t mid, ಗಣನಾ mosquitto_ಸಂದೇಶ_state state, ಪೂರ್ಣಾಙ್ಕ qos, bool persist)
{
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ;

	DL_FOREACH(head, client_ಸಂದೇಶ){
		ಯದಿ(client_ಸಂದೇಶ->data.mid == mid){
			ಯದಿ(client_ಸಂದೇಶ->data.qos != qos){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_ಸಂದೇಶ->data.qos, qos);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
			}
			client_ಸಂದೇಶ->data.state = (uint8_t)state;
			ಯದಿ(persist){
				plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
			}
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
		}
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_FOUND;
}


ಪೂರ್ಣಾಙ್ಕ db__message_update_outgoing(ಸಂರಚನಾ mosquitto *context, uint16_t mid, ಗಣನಾ mosquitto_ಸಂದೇಶ_state state, ಪೂರ್ಣಾಙ್ಕ qos, bool persist)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	rc = db__message_update_outgoing_state(context, context->msgs_out.inflight, mid, state, qos, persist);
	ಯದಿ(!persist && rc == MOSQ_ERR_NOT_FOUND){
		rc = db__message_update_outgoing_state(context, context->msgs_out.queued, mid, state, qos, persist);
	}
	ಪ್ರತ್ಯಾಗಮ rc;
}


ಸ್ಥಿರ ಶೂನ್ಯ db__messages_delete_list(ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ **head)
{
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;

	DL_FOREACH_SAFE(*head, client_ಸಂದೇಶ, tmp){
		DL_DELETE(*head, client_ಸಂದೇಶ);
		db_ಸಂದೇಶ_store_ref_dec(&client_ಸಂದೇಶ->base_ಸಂದೇಶ);
		mosquitto_FREE(client_ಸಂದೇಶ);
	}
	*head = ರಿಕ್ತ;
}


ಪೂರ್ಣಾಙ್ಕ db__messages_delete_incoming(ಸಂರಚನಾ mosquitto *context)
{
	ಯದಿ(!context){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_in.inflight);
	db__messages_delete_list(&context->msgs_in.queued);
	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_ಎಣಿಕೆ = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_ಎಣಿಕೆ = 0;
	context->msgs_in.queued_count12 = 0;

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ db__messages_delete_outgoing(ಸಂರಚನಾ mosquitto *context)
{
	ಯದಿ(!context){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_out.inflight);
	db__messages_delete_list(&context->msgs_out.queued);
	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_ಎಣಿಕೆ = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_ಎಣಿಕೆ = 0;
	context->msgs_out.queued_count12 = 0;

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ db__messages_delete(ಸಂರಚನಾ mosquitto *context, bool force_free)
{
	ಯದಿ(!context){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	ಯದಿ(force_free || context->clean_start || (context->bridge && context->bridge->clean_start)){
		db__messages_delete_incoming(context);
	}

	ಯದಿ(force_free || (context->bridge && context->bridge->clean_start_local)
			|| (context->bridge == ರಿಕ್ತ && context->clean_start)){

		db__messages_delete_outgoing(context);
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ db__messages_easy_queue(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ವರ್ಣ *topic, uint8_t qos, uint32_t payloadlen, ಧ್ರುವ ಶೂನ್ಯ *payload, ಪೂರ್ಣಾಙ್ಕ retain, uint32_t message_expiry_interval, mosquitto_property **properties)
{
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ;
	ಧ್ರುವ ವರ್ಣ *source_id;
	ಗಣನಾ mosquitto_ಸಂದೇಶ_origin origin;

	ಯದಿ(!topic){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	base_ಸಂದೇಶ = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ));
	ಯದಿ(base_ಸಂದೇಶ == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	base_ಸಂದೇಶ->data.topic = mosquitto_strdup(topic);
	ಯದಿ(base_ಸಂದೇಶ->data.topic == ರಿಕ್ತ){
		db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	base_ಸಂದೇಶ->data.qos = qos;
	ಯದಿ(db.config->retain_available == false){
		base_ಸಂದೇಶ->data.retain = 0;
	}ಅನ್ಯಥಾ{
		base_ಸಂದೇಶ->data.retain = retain;
	}

	base_ಸಂದೇಶ->data.payloadlen = payloadlen;
	ಯದಿ(payloadlen > 0){
		base_ಸಂದೇಶ->data.payload = mosquitto_malloc(base_ಸಂದೇಶ->data.payloadlen+1);
		ಯದಿ(base_ಸಂದೇಶ->data.payload == ರಿಕ್ತ){
			db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_ಸಂದೇಶ->data.payload)[base_ಸಂದೇಶ->data.payloadlen] = 0;
		memcpy(base_ಸಂದೇಶ->data.payload, payload, base_ಸಂದೇಶ->data.payloadlen);
	}

	ಯದಿ(context && context->id){
		source_id = context->id;
	}ಅನ್ಯಥಾ{
		source_id = "";
	}
	ಯದಿ(properties){
		base_ಸಂದೇಶ->data.properties = *properties;
		*properties = ರಿಕ್ತ;
	}

	ಯದಿ(context){
		origin = mosq_mo_client;
	}ಅನ್ಯಥಾ{
		origin = mosq_mo_broker;
	}
	ಯದಿ(db__message_store(context, base_ಸಂದೇಶ, &message_expiry_interval, origin)){
		ಪ್ರತ್ಯಾಗಮ 1;
	}

	ಪ್ರತ್ಯಾಗಮ sub__messages_queue(source_id, base_ಸಂದೇಶ->data.qos, &base_ಸಂದೇಶ);
}


#define MOSQ_UUID_EPOCH 1637168273


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
uint64_t db_new_ಸಂದೇಶ_id(ಶೂನ್ಯ)
{
#ifdef WIN32
	FILETIME ftime;
	uint64_t ftime64;
#ಅನ್ಯಥಾ
	ಸಂರಚನಾ timespec ts;
#endif
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	long nsec;

	id = db.ಗಂಟು_id_shifted; /* Top 10-bits */

#ifdef WIN32
	GetSystemTimePreciseAsFileTime(&ftime);
	ftime64 = (((uint64_t)ftime.dw_ಮೇಲಿನ_Date_Time)<<32) + ftime.dw_ಕೆಳಗಿನ_Date_Time;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (long)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#ಅನ್ಯಥಾ
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#endif
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	ಯದಿ(id <= db.last_db_id){
		id = db.last_db_id + 1;
	}
	db.last_db_id = id;

	ಪ್ರತ್ಯಾಗಮ id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
ಪೂರ್ಣಾಙ್ಕ db__message_store(ಧ್ರುವ ಸಂರಚನಾ mosquitto *source, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ, uint32_t *message_expiry_interval, ಗಣನಾ mosquitto_ಸಂದೇಶ_origin origin)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಪ್ರತಿಜ್ಞಾ(base_ಸಂದೇಶ);

	ಯದಿ(source && source->id){
		base_ಸಂದೇಶ->data.source_id = mosquitto_strdup(source->id);
	}ಅನ್ಯಥಾ{
		base_ಸಂದೇಶ->data.source_id = mosquitto_strdup("");
	}
	ಯದಿ(!base_ಸಂದೇಶ->data.source_id){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	ಯದಿ(source && source->username){
		base_ಸಂದೇಶ->data.source_username = mosquitto_strdup(source->username);
		ಯದಿ(!base_ಸಂದೇಶ->data.source_username){
			db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
	}
	ಯದಿ(source){
		base_ಸಂದೇಶ->source_listener = source->listener;
	}
	base_ಸಂದೇಶ->origin = origin;
	ಯದಿ(message_expiry_interval){
		ಯದಿ(*message_expiry_interval > 0 && *message_expiry_interval != ಸಂದೇಶ_EXPIRY_INFINITE){
			base_ಸಂದೇಶ->data.expiry_time = db.now_real_s + *message_expiry_interval;
		}ಅನ್ಯಥಾ{
			base_ಸಂದೇಶ->data.expiry_time = 0;
		}
	}

	base_ಸಂದೇಶ->dest_ids = ರಿಕ್ತ;
	base_ಸಂದೇಶ->dest_id_ಎಣಿಕೆ = 0;
	db.ಸಂದೇಶ_store_ಎಣಿಕೆ++;
	db.ಸಂದೇಶ_store_bytes += base_ಸಂದೇಶ->data.payloadlen;

	ಯದಿ(!base_ಸಂದೇಶ->data.store_id){
		base_ಸಂದೇಶ->data.store_id = db_new_ಸಂದೇಶ_id();
	}

	rc = db_ಸಂದೇಶ_store_add(base_ಸಂದೇಶ);
	ಯದಿ(rc){
		db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ db__message_store_find(ಸಂರಚನಾ mosquitto *context, uint16_t mid, ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ **client_ಸಂದೇಶ)
{
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *cmsg;

	*client_ಸಂದೇಶ = ರಿಕ್ತ;

	ಯದಿ(!context){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_in.inflight, cmsg){
		ಯದಿ(cmsg->base_ಸಂದೇಶ->data.source_mid == mid){
			*client_ಸಂದೇಶ = cmsg;
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_in.queued, cmsg){
		ಯದಿ(cmsg->base_ಸಂದೇಶ->data.source_mid == mid){
			*client_ಸಂದೇಶ = cmsg;
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
		}
	}

	ಪ್ರತ್ಯಾಗಮ 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ db__message_reconnect_reset_outgoing(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;

	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_ಎಣಿಕೆ = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_ಎಣಿಕೆ = 0;
	context->msgs_out.queued_count12 = 0;
	context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_ಸಂದೇಶ, tmp){
		db_ಸಂದೇಶ_add_to_inflight_stats(&context->msgs_out, client_ಸಂದೇಶ);
		ಯದಿ(client_ಸಂದೇಶ->data.qos > 0){
			util__decrement_send_quota(context);
		}

		ವಿಕಲ್ಪ(client_ಸಂದೇಶ->data.qos){
			ಪಕ್ಷ 0:
				client_ಸಂದೇಶ->data.state = mosq_ms_publish_qos0;
				ಭಙ್ಗ;
			ಪಕ್ಷ 1:
				client_ಸಂದೇಶ->data.state = mosq_ms_publish_qos1;
				ಭಙ್ಗ;
			ಪಕ್ಷ 2:
				ಯದಿ(client_ಸಂದೇಶ->data.state == mosq_ms_wait_for_pubcomp){
					client_ಸಂದೇಶ->data.state = mosq_ms_resend_pubrel;
				}ಅನ್ಯಥಾ{
					client_ಸಂದೇಶ->data.state = mosq_ms_publish_qos2;
				}
				ಭಙ್ಗ;
		}
		plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_out.queued, client_ಸಂದೇಶ, tmp){
		db_ಸಂದೇಶ_add_to_queued_stats(&context->msgs_out, client_ಸಂದೇಶ);
	}
	db__fill_inflight_out_from_queue(context);

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ db__message_reconnect_reset_incoming(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;

	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_ಎಣಿಕೆ = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_ಎಣಿಕೆ = 0;
	context->msgs_in.queued_count12 = 0;
	context->msgs_in.inflight_quota = context->msgs_in.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_ಸಂದೇಶ, tmp){
		db_ಸಂದೇಶ_add_to_inflight_stats(&context->msgs_in, client_ಸಂದೇಶ);
		ಯದಿ(client_ಸಂದೇಶ->data.qos > 0){
			util__decrement_receive_quota(context);
		}

		ಯದಿ(client_ಸಂದೇಶ->data.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db__message_remove_inflight(context, &context->msgs_in, client_ಸಂದೇಶ);
		}ಅನ್ಯಥಾ{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_ಸಂದೇಶ->data.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_in.queued, client_ಸಂದೇಶ, tmp){
		client_ಸಂದೇಶ->data.dup = 0;
		db_ಸಂದೇಶ_add_to_queued_stats(&context->msgs_in, client_ಸಂದೇಶ);
		ಯದಿ(db__ready_for_flight(context, mosq_md_in, client_ಸಂದೇಶ->data.qos)){
			ವಿಕಲ್ಪ(client_ಸಂದೇಶ->data.qos){
				ಪಕ್ಷ 0:
					client_ಸಂದೇಶ->data.state = mosq_ms_publish_qos0;
					ಭಙ್ಗ;
				ಪಕ್ಷ 1:
					client_ಸಂದೇಶ->data.state = mosq_ms_publish_qos1;
					ಭಙ್ಗ;
				ಪಕ್ಷ 2:
					client_ಸಂದೇಶ->data.state = mosq_ms_publish_qos2;
					ಭಙ್ಗ;
			}
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ db__message_reconnect_reset(ಸಂರಚನಾ mosquitto *context)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	rc = db__message_reconnect_reset_outgoing(context);
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}
	ಪ್ರತ್ಯಾಗಮ db__message_reconnect_reset_incoming(context);
}


ಪೂರ್ಣಾಙ್ಕ db__message_remove_incoming(ಸಂರಚನಾ mosquitto *context, uint16_t mid)
{
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;

	ಯದಿ(!context){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_ಸಂದೇಶ, tmp){
		ಯದಿ(client_ಸಂದೇಶ->data.mid == mid){
			ಯದಿ(client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.qos != 2){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.qos);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_in, client_ಸಂದೇಶ);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_FOUND;
}


ಪೂರ್ಣಾಙ್ಕ db__message_release_incoming(ಸಂರಚನಾ mosquitto *context, uint16_t mid)
{
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;
	bool deleted = false;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(!context){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_ಸಂದೇಶ, tmp){
		ಯದಿ(client_ಸಂದೇಶ->data.mid == mid){
			ಯದಿ(client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.qos != 2){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.qos);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
			}
			ಧ್ರುವ ವರ್ಣ *topic = client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.topic;
			ಧ್ರುವ ವರ್ಣ *source_id = client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.source_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			ಯದಿ(topic == ರಿಕ್ತ){
				db__message_remove_inflight(context, &context->msgs_in, client_ಸಂದೇಶ);
				deleted = true;
			}ಅನ್ಯಥಾ{
				rc = sub__messages_queue(source_id, 2, &client_ಸಂದೇಶ->base_ಸಂದೇಶ);
				ಯದಿ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db__message_remove_inflight(context, &context->msgs_in, client_ಸಂದೇಶ);
					deleted = true;
				}ಅನ್ಯಥಾ{
					ಪ್ರತ್ಯಾಗಮ 1;
				}
			}
		}
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_ಸಂದೇಶ, tmp){
		ಯದಿ(db__ready_for_flight(context, mosq_md_in, client_ಸಂದೇಶ->data.qos)){
			ಭಙ್ಗ;
		}

		ಯದಿ(client_ಸಂದೇಶ->data.qos == 2){
			send__pubrec(context, client_ಸಂದೇಶ->data.mid, 0, ರಿಕ್ತ);
			client_ಸಂದೇಶ->data.state = mosq_ms_wait_for_pubrel;
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
		}
	}
	ಯದಿ(deleted){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_FOUND;
	}
}


ಶೂನ್ಯ db__expire_all_messages(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_ಸಂದೇಶ, tmp){
		ಯದಿ(client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.expiry_time && db.now_real_s > client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.expiry_time){
			ಯದಿ(client_ಸಂದೇಶ->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_ಸಂದೇಶ);
		}
	}
	db__fill_inflight_out_from_queue(context);
	DL_FOREACH_SAFE(context->msgs_out.queued, client_ಸಂದೇಶ, tmp){
		ಯದಿ(client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.expiry_time && db.now_real_s > client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_ಸಂದೇಶ);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.inflight, client_ಸಂದೇಶ, tmp){
		ಯದಿ(client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.expiry_time && db.now_real_s > client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.expiry_time){
			ಯದಿ(client_ಸಂದೇಶ->data.qos > 0){
				util__increment_receive_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_in, client_ಸಂದೇಶ);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.queued, client_ಸಂದೇಶ, tmp){
		ಯದಿ(client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.expiry_time && db.now_real_s > client_ಸಂದೇಶ->base_ಸಂದೇಶ->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_in, client_ಸಂದೇಶ);
		}
	}
}


ಸ್ಥಿರ ಶೂನ್ಯ db__client_messages_check_acl(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data *ಸಂದೇಶ_data, ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ **head,
		ಶೂನ್ಯ (*decrement_stats_fn)(ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data *ಸಂದೇಶ_data, ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ))
{
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ;
	ಪೂರ್ಣಾಙ್ಕ access;

	DL_FOREACH_SAFE((*head), client_ಸಂದೇಶ, tmp){
		base_ಸಂದೇಶ = client_ಸಂದೇಶ->base_ಸಂದೇಶ;
		ಯದಿ(client_ಸಂದೇಶ->data.direction == mosq_md_out){
			access = MOSQ_ACL_READ;
		}ಅನ್ಯಥಾ{
			access = MOSQ_ACL_WRITE;
		}
		ಯದಿ(mosquitto_acl_check(context, base_ಸಂದೇಶ->data.topic,
				base_ಸಂದೇಶ->data.payloadlen, base_ಸಂದೇಶ->data.payload,
				base_ಸಂದೇಶ->data.qos, base_ಸಂದೇಶ->data.retain,
				base_ಸಂದೇಶ->data.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*head), client_ಸಂದೇಶ);
			decrement_stats_fn(ಸಂದೇಶ_data, client_ಸಂದೇಶ);
			plugin_persist_handle_client_ಸಂದೇಶ_delete(context, client_ಸಂದೇಶ);
			db_ಸಂದೇಶ_store_ref_dec(&client_ಸಂದೇಶ->base_ಸಂದೇಶ);
			mosquitto_FREE(client_ಸಂದೇಶ);
		}
	}
}


ಶೂನ್ಯ db__check_acl_of_all_messages(ಸಂರಚನಾ mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.inflight, &db_ಸಂದೇಶ_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.queued, &db_ಸಂದೇಶ_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.inflight, &db_ಸಂದೇಶ_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.queued, &db_ಸಂದೇಶ_remove_from_queued_stats);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ db__message_write_inflight_out_single(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ)
{
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ;
	mosquitto_property *base_ಸಂದೇಶ_props = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ rc;
	uint16_t mid;
	ಪೂರ್ಣಾಙ್ಕ retries;
	ಪೂರ್ಣಾಙ್ಕ retain;
	ಧ್ರುವ ವರ್ಣ *topic;
	uint8_t qos;
	uint32_t payloadlen;
	ಧ್ರುವ ಶೂನ್ಯ *payload;
	uint32_t expiry_interval;
	uint32_t subscription_id;

	base_ಸಂದೇಶ = client_ಸಂದೇಶ->base_ಸಂದೇಶ;

	expiry_interval = 0;
	ಯದಿ(base_ಸಂದೇಶ->data.expiry_time){
		ಯದಿ(db.now_real_s > base_ಸಂದೇಶ->data.expiry_time){
			/* Message is expired, must not send. */
			ಯದಿ(client_ಸಂದೇಶ->data.direction == mosq_md_out && client_ಸಂದೇಶ->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_ಸಂದೇಶ);
			db__fill_inflight_out_from_queue(context);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
		}ಅನ್ಯಥಾ{
			expiry_interval = (uint32_t)(base_ಸಂದೇಶ->data.expiry_time - db.now_real_s);
		}
	}
	mid = client_ಸಂದೇಶ->data.mid;
	retries = client_ಸಂದೇಶ->data.dup;
	retain = client_ಸಂದೇಶ->data.retain;
	topic = base_ಸಂದೇಶ->data.topic;
	qos = (uint8_t)client_ಸಂದೇಶ->data.qos;
	payloadlen = base_ಸಂದೇಶ->data.payloadlen;
	payload = base_ಸಂದೇಶ->data.payload;
	subscription_id = client_ಸಂದೇಶ->data.subscription_identifier;
	base_ಸಂದೇಶ_props = base_ಸಂದೇಶ->data.properties;

	ವಿಕಲ್ಪ(client_ಸಂದೇಶ->data.state){
		ಪಕ್ಷ mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_ಸಂದೇಶ_props, expiry_interval);
			ಯದಿ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_ಸಂದೇಶ);
			}ಅನ್ಯಥಾ{
				ಪ್ರತ್ಯಾಗಮ rc;
			}
			ಭಙ್ಗ;

		ಪಕ್ಷ mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_ಸಂದೇಶ_props, expiry_interval);
			ಯದಿ(rc == MOSQ_ERR_SUCCESS){
				client_ಸಂದೇಶ->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_ಸಂದೇಶ->data.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
			}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_ಸಂದೇಶ);
			}ಅನ್ಯಥಾ{
				ಪ್ರತ್ಯಾಗಮ rc;
			}
			ಭಙ್ಗ;

		ಪಕ್ಷ mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_ಸಂದೇಶ_props, expiry_interval);
			ಯದಿ(rc == MOSQ_ERR_SUCCESS){
				client_ಸಂದೇಶ->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_ಸಂದೇಶ->data.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
			}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_ಸಂದೇಶ);
			}ಅನ್ಯಥಾ{
				ಪ್ರತ್ಯಾಗಮ rc;
			}
			ಭಙ್ಗ;

		ಪಕ್ಷ mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, ರಿಕ್ತ);
			ಯದಿ(!rc){
				client_ಸಂದೇಶ->data.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
			}ಅನ್ಯಥಾ{
				ಪ್ರತ್ಯಾಗಮ rc;
			}
			ಭಙ್ಗ;

		ಪಕ್ಷ mosq_ms_invalid:
		ಪಕ್ಷ mosq_ms_send_pubrec:
		ಪಕ್ಷ mosq_ms_resend_pubcomp:
		ಪಕ್ಷ mosq_ms_wait_for_puback:
		ಪಕ್ಷ mosq_ms_wait_for_pubrec:
		ಪಕ್ಷ mosq_ms_wait_for_pubrel:
		ಪಕ್ಷ mosq_ms_wait_for_pubcomp:
		ಪಕ್ಷ mosq_ms_queued:
			ಭಙ್ಗ;
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ db__message_write_inflight_out_all(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(context->state != mosq_cs_active || !net__is_connected(context)){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_ಸಂದೇಶ, tmp){
		rc = db__message_write_inflight_out_single(context, client_ಸಂದೇಶ);
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ db__message_write_inflight_out_latest(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *next;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(context->state != mosq_cs_active
			|| !net__is_connected(context)
			|| context->msgs_out.inflight == ರಿಕ್ತ){

		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	ಯದಿ(context->msgs_out.inflight->prev == context->msgs_out.inflight){
		/* Only one message */
		ಪ್ರತ್ಯಾಗಮ db__message_write_inflight_out_single(context, context->msgs_out.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_ಸಂದೇಶ = context->msgs_out.inflight->prev;
	ಯಾವತ್(client_ಸಂದೇಶ != context->msgs_out.inflight &&
			(client_ಸಂದೇಶ->data.state == mosq_ms_publish_qos0
			|| client_ಸಂದೇಶ->data.state == mosq_ms_publish_qos1
			|| client_ಸಂದೇಶ->data.state == mosq_ms_publish_qos2)){

		client_ಸಂದೇಶ = client_ಸಂದೇಶ->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	ಯದಿ(client_ಸಂದೇಶ != context->msgs_out.inflight){
		client_ಸಂದೇಶ = client_ಸಂದೇಶ->next;
	}

	ಯಾವತ್(client_ಸಂದೇಶ){
		next = client_ಸಂದೇಶ->next;
		rc = db__message_write_inflight_out_single(context, client_ಸಂದೇಶ);
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
		client_ಸಂದೇಶ = next;
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ db__message_write_queued_in(ಸಂರಚನಾ mosquitto *context)
{
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(context->state != mosq_cs_active){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_ಸಂದೇಶ, tmp){
		ಯದಿ(context->msgs_in.inflight_maximum != 0 && context->msgs_in.inflight_quota == 0){
			ಭಙ್ಗ;
		}

		ಯದಿ(client_ಸಂದೇಶ->data.qos == 2){
			client_ಸಂದೇಶ->data.state = mosq_ms_send_pubrec;
			db__message_dequeue_first(context, &context->msgs_in);
			rc = send__pubrec(context, client_ಸಂದೇಶ->data.mid, 0, ರಿಕ್ತ);
			ಯದಿ(!rc){
				client_ಸಂದೇಶ->data.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
			}ಅನ್ಯಥಾ{
				plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ db__message_write_queued_out(ಸಂರಚನಾ mosquitto *context)
{
	ಯದಿ(context->state != mosq_cs_active){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	db__fill_inflight_out_from_queue(context);

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}
