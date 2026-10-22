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

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <utlist.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "send_mosq.h"
#અન્તર્ભાવ "sys_tree.h"
#અન્તર્ભાવ "util_mosq.h"


/**
 * Is this context ready to take more in flight messages right now?
 * @param context the client context of interest
 * @param qos qos for the packet of interest
 * @return true if more in flight are allowed.
 */
bool db__ready_for_flight(સંરચના mosquitto *context, ગણના mosquitto_સંદેશ_direction dir, પૂર્ણાંક qos)
{
	સંરચના mosquitto_સંદેશ_data *msgs;
	bool valid_bytes;
	bool valid_ગણતરી;

	જો(dir == mosq_md_out){
		msgs = &context->msgs_out;
	}નહીંતર{
		msgs = &context->msgs_in;
	}

	જો(msgs->inflight_maximum == 0 && db.config->max_inflight_bytes == 0){
		પાછા true;
	}

	જો(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		જો(db.config->max_queued_messages == 0 && db.config->max_inflight_bytes == 0){
			પાછા true;
		}
		valid_bytes = ((msgs->inflight_bytes - (ssize_t)db.config->max_inflight_bytes) < (ssize_t)db.config->max_queued_bytes);
		જો(dir == mosq_md_out){
			valid_ગણતરી = context->out_packet_ગણતરી < db.config->max_queued_messages;
		}નહીંતર{
			valid_ગણતરી = msgs->inflight_ગણતરી - msgs->inflight_maximum < db.config->max_queued_messages;
		}

		જો(db.config->max_queued_messages == 0){
			પાછા valid_bytes;
		}
		જો(db.config->max_queued_bytes == 0){
			પાછા valid_ગણતરી;
		}
	}નહીંતર{
		valid_bytes = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_bytes;
		valid_ગણતરી = msgs->inflight_quota > 0;

		જો(msgs->inflight_maximum == 0){
			પાછા valid_bytes;
		}
		જો(db.config->max_inflight_bytes == 0){
			પાછા valid_ગણતરી;
		}
	}

	પાછા valid_bytes && valid_ગણતરી;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db__ready_for_queue(સંરચના mosquitto *context, પૂર્ણાંક qos, સંરચના mosquitto_સંદેશ_data *સંદેશ_data)
{
	પૂર્ણાંક source_ગણતરી;
	પૂર્ણાંક adjust_ગણતરી;
	long source_bytes;
	ssize_t adjust_bytes = (ssize_t)db.config->max_inflight_bytes;
	bool valid_bytes;
	bool valid_ગણતરી;

	જો(db.config->max_queued_messages == 0 && db.config->max_queued_bytes == 0){
		પાછા true;
	}

	જો(qos == 0 && db.config->queue_qos0_messages == false){
		પાછા false; /* This case is handled in db__ready_for_flight() */
	}નહીંતર{
		source_bytes = (ssize_t)સંદેશ_data->queued_bytes12;
		source_ગણતરી = સંદેશ_data->queued_count12;
	}
	adjust_ગણતરી = સંદેશ_data->inflight_maximum;

	/* nothing in flight for offline clients */
	જો(!net__is_connected(context)){
		adjust_bytes = 0;
		adjust_ગણતરી = 0;
	}

	valid_bytes = (source_bytes - (ssize_t)adjust_bytes) < (ssize_t)db.config->max_queued_bytes;
	valid_ગણતરી = source_ગણતરી - adjust_ગણતરી < db.config->max_queued_messages;

	જો(db.config->max_queued_bytes == 0){
		પાછા valid_ગણતરી;
	}
	જો(db.config->max_queued_messages == 0){
		પાછા valid_bytes;
	}

	પાછા valid_bytes && valid_ગણતરી;
}


શૂન્ય db_સંદેશ_add_to_inflight_stats(સંરચના mosquitto_સંદેશ_data *સંદેશ_data, સંરચના mosquitto_client_સંદેશ *client_સંદેશ)
{
	સંદેશ_data->inflight_ગણતરી++;
	સંદેશ_data->inflight_bytes += client_સંદેશ->base_સંદેશ->data.payloadlen;
	જો(client_સંદેશ->data.qos != 0){
		સંદેશ_data->inflight_count12++;
		સંદેશ_data->inflight_bytes12 += client_સંદેશ->base_સંદેશ->data.payloadlen;
	}
}


સ્થિર શૂન્ય db_સંદેશ_remove_from_inflight_stats(સંરચના mosquitto_સંદેશ_data *સંદેશ_data, સંરચના mosquitto_client_સંદેશ *client_સંદેશ)
{
	સંદેશ_data->inflight_ગણતરી--;
	સંદેશ_data->inflight_bytes -= client_સંદેશ->base_સંદેશ->data.payloadlen;
	જો(client_સંદેશ->data.qos != 0){
		સંદેશ_data->inflight_count12--;
		સંદેશ_data->inflight_bytes12 -= client_સંદેશ->base_સંદેશ->data.payloadlen;
	}
}


શૂન્ય db_સંદેશ_add_to_queued_stats(સંરચના mosquitto_સંદેશ_data *સંદેશ_data, સંરચના mosquitto_client_સંદેશ *client_સંદેશ)
{
	સંદેશ_data->queued_ગણતરી++;
	સંદેશ_data->queued_bytes += client_સંદેશ->base_સંદેશ->data.payloadlen;
	જો(client_સંદેશ->data.qos != 0){
		સંદેશ_data->queued_count12++;
		સંદેશ_data->queued_bytes12 += client_સંદેશ->base_સંદેશ->data.payloadlen;
	}
}


સ્થિર શૂન્ય db_સંદેશ_remove_from_queued_stats(સંરચના mosquitto_સંદેશ_data *સંદેશ_data, સંરચના mosquitto_client_સંદેશ *client_સંદેશ)
{
	સંદેશ_data->queued_ગણતરી--;
	સંદેશ_data->queued_bytes -= client_સંદેશ->base_સંદેશ->data.payloadlen;
	જો(client_સંદેશ->data.qos != 0){
		સંદેશ_data->queued_count12--;
		સંદેશ_data->queued_bytes12 -= client_સંદેશ->base_સંદેશ->data.payloadlen;
	}
}


પૂર્ણાંક db__open(સંરચના mosquitto__config *config)
{
	જો(!config){
		પાછા MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = ખાલી;
	db.contexts_by_sock = ખાલી;
	db.contexts_for_free = ખાલી;
#ifdef WITH_BRIDGE
	db.bridges = ખાલી;
	db.bridge_ગણતરી = 0;
#endif

	/* Initialize the hashtable */
	db.clientid_index_hash = ખાલી;

	db.normal_subs = ખાલી;
	db.shared_subs = ખાલી;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = ખાલી;

#ifdef WITH_PERSISTENCE
	જો(persist__restore()){
		પાછા 1;
	}
#endif

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર શૂન્ય subhier_clean(સંરચના mosquitto__subhier **subhier)
{
	સંરચના mosquitto__subhier *peer, *subhier_tmp;
	સંરચના mosquitto__subleaf *leaf, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		leaf = peer->subs;
		જ્યારે(leaf){
			nextleaf = leaf->next;
			mosquitto_FREE(leaf);
			leaf = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


પૂર્ણાંક db__close(શૂન્ય)
{
	subhier_clean(&db.normal_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db_સંદેશ_store_clean();

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક db_સંદેશ_store_add(સંરચના mosquitto_base_સંદેશ *base_સંદેશ)
{
	સંરચના mosquitto_base_સંદેશ *found;
	unsigned hashv;

	HASH_કિંમત(&base_સંદેશ->data.store_id, કદ(base_સંદેશ->data.store_id), hashv);
	HASH_FIND_BYHASHVALUE(hh, db.સંદેશ_store, &base_સંદેશ->data.store_id, કદ(base_સંદેશ->data.store_id), hashv, found);
	જો(found == ખાલી){
		HASH_ADD_KEYPTR_BYHASHVALUE(hh, db.સંદેશ_store, &base_સંદેશ->data.store_id, કદ(base_સંદેશ->data.store_id), hashv, base_સંદેશ);
		પાછા MOSQ_ERR_SUCCESS;
	}નહીંતર{
		પાછા MOSQ_ERR_ALREADY_EXISTS;
	}
}


શૂન્ય db_સંદેશ_store_free(સંરચના mosquitto_base_સંદેશ *base_સંદેશ)
{
	mosquitto_FREE(base_સંદેશ->data.source_id);
	mosquitto_FREE(base_સંદેશ->data.source_username);
	જો(base_સંદેશ->dest_ids){
		માટે(પૂર્ણાંક i=0; i<base_સંદેશ->dest_id_ગણતરી; i++){
			mosquitto_FREE(base_સંદેશ->dest_ids[i]);
		}
		mosquitto_FREE(base_સંદેશ->dest_ids);
	}
	mosquitto_FREE(base_સંદેશ->data.topic);
	mosquitto_property_free_all(&base_સંદેશ->data.properties);
	mosquitto_FREE(base_સંદેશ->data.payload);
	mosquitto_FREE(base_સંદેશ);
}


શૂન્ય db_સંદેશ_store_remove(સંરચના mosquitto_base_સંદેશ *base_સંદેશ, bool notify)
{
	જો(base_સંદેશ == ખાલી){
		પાછા;
	}
	HASH_DELETE(hh, db.સંદેશ_store, base_સંદેશ);
	db.સંદેશ_store_ગણતરી--;
	db.સંદેશ_store_bytes -= base_સંદેશ->data.payloadlen;
	જો(notify == true){
		plugin_persist_handle_base_સંદેશ_delete(base_સંદેશ);
	}
	db_સંદેશ_store_free(base_સંદેશ);
}


શૂન્ય db_સંદેશ_store_clean(શૂન્ય)
{
	સંરચના mosquitto_base_સંદેશ *base_સંદેશ, *base_સંદેશ_tmp;

	HASH_ITER(hh, db.સંદેશ_store, base_સંદેશ, base_સંદેશ_tmp){
		db_સંદેશ_store_remove(base_સંદેશ, false);
	}
}


શૂન્ય db_સંદેશ_store_ref_inc(સંરચના mosquitto_base_સંદેશ *base_સંદેશ)
{
	base_સંદેશ->ref_ગણતરી++;
}


શૂન્ય db_સંદેશ_store_ref_dec(સંરચના mosquitto_base_સંદેશ **base_સંદેશ)
{
	(*base_સંદેશ)->ref_ગણતરી--;
	જો((*base_સંદેશ)->ref_ગણતરી == 0){
		db_સંદેશ_store_remove(*base_સંદેશ, true);
		*base_સંદેશ = ખાલી;
	}
}


શૂન્ય db_સંદેશ_store_compact(શૂન્ય)
{
	સંરચના mosquitto_base_સંદેશ *base_સંદેશ, *base_સંદેશ_tmp;

	HASH_ITER(hh, db.સંદેશ_store, base_સંદેશ, base_સંદેશ_tmp){
		જો(base_સંદેશ->ref_ગણતરી < 1){
			db_સંદેશ_store_remove(base_સંદેશ, true);
		}
	}
}


સ્થિર શૂન્ય db__message_remove_inflight(સંરચના mosquitto *context, સંરચના mosquitto_સંદેશ_data *સંદેશ_data, સંરચના mosquitto_client_સંદેશ *item)
{
	જો(!context || !સંદેશ_data || !item){
		પાછા;
	}

	plugin_persist_handle_client_સંદેશ_delete(context, item);

	DL_DELETE(સંદેશ_data->inflight, item);
	જો(item->base_સંદેશ){
		db_સંદેશ_remove_from_inflight_stats(સંદેશ_data, item);
		db_સંદેશ_store_ref_dec(&item->base_સંદેશ);
	}

	mosquitto_FREE(item);
}


સ્થિર શૂન્ય db__message_remove_queued(સંરચના mosquitto *context, સંરચના mosquitto_સંદેશ_data *સંદેશ_data, સંરચના mosquitto_client_સંદેશ *item)
{
	જો(!context || !સંદેશ_data || !item){
		પાછા;
	}

	plugin_persist_handle_client_સંદેશ_delete(context, item);

	DL_DELETE(સંદેશ_data->queued, item);
	જો(item->base_સંદેશ){
		db_સંદેશ_remove_from_queued_stats(સંદેશ_data, item);
		db_સંદેશ_store_ref_dec(&item->base_સંદેશ);
	}

	mosquitto_FREE(item);
}


સ્થિર શૂન્ય db__fill_inflight_out_from_queue(સંરચના mosquitto *context)
{
	સંરચના mosquitto_client_સંદેશ *client_સંદેશ, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.queued, client_સંદેશ, tmp){
		જો(!db__ready_for_flight(context, mosq_md_out, client_સંદેશ->data.qos)){
			પાછા;
		}
		પસંદગી(client_સંદેશ->data.qos){
			સ્થિતિ 0:
				client_સંદેશ->data.state = mosq_ms_publish_qos0;
				તોડો;
			સ્થિતિ 1:
				client_સંદેશ->data.state = mosq_ms_publish_qos1;
				તોડો;
			સ્થિતિ 2:
				client_સંદેશ->data.state = mosq_ms_publish_qos2;
				તોડો;
		}
		જો(client_સંદેશ->base_સંદેશ->data.expiry_time && db.now_real_s > client_સંદેશ->base_સંદેશ->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_સંદેશ);
			ચાલુ;
		}
		plugin_persist_handle_client_સંદેશ_update(context, client_સંદેશ);
		db__message_dequeue_first(context, &context->msgs_out);
	}
}


શૂન્ય db__message_dequeue_first(સંરચના mosquitto *context, સંરચના mosquitto_સંદેશ_data *સંદેશ_data)
{
	સંરચના mosquitto_client_સંદેશ *client_સંદેશ;

	UNUSED(context);

	client_સંદેશ = સંદેશ_data->queued;
	DL_DELETE(સંદેશ_data->queued, client_સંદેશ);
	DL_APPEND(સંદેશ_data->inflight, client_સંદેશ);
	જો(સંદેશ_data->inflight_quota > 0){
		સંદેશ_data->inflight_quota--;
	}

	db_સંદેશ_remove_from_queued_stats(સંદેશ_data, client_સંદેશ);
	db_સંદેશ_add_to_inflight_stats(સંદેશ_data, client_સંદેશ);
}


પૂર્ણાંક db__message_delete_outgoing(સંરચના mosquitto *context, uint16_t mid, ગણના mosquitto_સંદેશ_state expect_state, પૂર્ણાંક qos)
{
	સંરચના mosquitto_client_સંદેશ *client_સંદેશ, *tmp;
	bool deleted = false;

	જો(!context){
		પાછા MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_સંદેશ, tmp){
		જો(client_સંદેશ->data.mid == mid){
			જો(client_સંદેશ->data.qos != qos){
				log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_સંદેશ->data.qos, qos);
				પાછા MOSQ_ERR_PROTOCOL;
			}નહીંતર જો(qos == 2 && client_સંદેશ->data.state != expect_state && expect_state != mosq_ms_any){
				log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_સંદેશ->data.state, expect_state);
				પાછા MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_out, client_સંદેશ);
			deleted = true;
			તોડો;
		}
	}

	જો(deleted == false){
		DL_FOREACH_SAFE(context->msgs_out.queued, client_સંદેશ, tmp){
			જો(client_સંદેશ->data.mid == mid){
				જો(client_સંદેશ->data.qos != qos){
					પાછા MOSQ_ERR_PROTOCOL;
				}નહીંતર જો(qos == 2 && client_સંદેશ->data.state != expect_state && expect_state != mosq_ms_any){
					પાછા MOSQ_ERR_PROTOCOL;
				}
				db__message_remove_queued(context, &context->msgs_out, client_સંદેશ);
				તોડો;
			}
		}
	}
	db__fill_inflight_out_from_queue(context);
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	પાછા db__message_write_inflight_out_latest(context);
}


/* Only for QoS 2 messages */
પૂર્ણાંક db_message_ઉમેરો_incoming(સંરચના mosquitto *context, uint64_t cmsg_id, સંરચના mosquitto_base_સંદેશ *base_સંદેશ, bool persist)
{
	સંરચના mosquitto_client_સંદેશ *client_સંદેશ;
	સંરચના mosquitto_સંદેશ_data *સંદેશ_data;
	ગણના mosquitto_સંદેશ_state state = mosq_ms_invalid;
	પૂર્ણાંક rc = 0;

	પ્રતિજ્ઞા(base_સંદેશ);
	જો(!context){
		પાછા MOSQ_ERR_INVAL;
	}
	જો(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		પાછા MOSQ_ERR_SUCCESS;

	}
	સંદેશ_data = &context->msgs_in;

	જો(db__ready_for_flight(context, mosq_md_in, base_સંદેશ->data.qos)){
		state = mosq_ms_wait_for_pubrel;
	}નહીંતર જો(base_સંદેશ->data.qos != 0 && db__ready_for_queue(context, base_સંદેશ->data.qos, સંદેશ_data)){
		state = mosq_ms_queued;
		rc = 2;
	}નહીંતર{
		/* Dropping message due to full queue. */
		જો(context->is_dropping == false){
			context->is_dropping = true;
			log__printf(ખાલી, MOSQ_LOG_NOTICE,
					"Outgoing messages are being dropped for client %s.",
					context->id);
		}
		metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		પાછા 2;
	}

	પ્રતિજ્ઞા(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	જો(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_સંદેશ = mosquitto_malloc(કદ(સંરચના mosquitto_client_સંદેશ));
	જો(!client_સંદેશ){
		પાછા MOSQ_ERR_NOMEM;
	}
	client_સંદેશ->prev = ખાલી;
	client_સંદેશ->next = ખાલી;
	જો(cmsg_id){
		client_સંદેશ->data.cmsg_id = cmsg_id;
	}નહીંતર{
		client_સંદેશ->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_સંદેશ->base_સંદેશ = base_સંદેશ;
	db_સંદેશ_store_ref_inc(client_સંદેશ->base_સંદેશ);
	client_સંદેશ->data.mid = base_સંદેશ->data.source_mid;
	client_સંદેશ->data.direction = mosq_md_in;
	client_સંદેશ->data.state = (uint8_t)state;
	client_સંદેશ->data.dup = false;
	જો(base_સંદેશ->data.qos > context->max_qos){
		client_સંદેશ->data.qos = context->max_qos;
	}નહીંતર{
		client_સંદેશ->data.qos = base_સંદેશ->data.qos;
	}
	client_સંદેશ->data.retain = base_સંદેશ->data.retain;
	client_સંદેશ->data.subscription_identifier = 0;

	જો(state == mosq_ms_queued){
		DL_APPEND(સંદેશ_data->queued, client_સંદેશ);
		db_સંદેશ_add_to_queued_stats(સંદેશ_data, client_સંદેશ);
	}નહીંતર{
		DL_APPEND(સંદેશ_data->inflight, client_સંદેશ);
		db_સંદેશ_add_to_inflight_stats(સંદેશ_data, client_સંદેશ);
	}

	જો(persist && context->is_persisted){
		plugin_persist_handle_base_સંદેશ_add(client_સંદેશ->base_સંદેશ);
		plugin_persist_handle_client_સંદેશ_add(context, client_સંદેશ);
	}

	જો(client_સંદેશ->base_સંદેશ->data.qos > 0){
		util__decrement_receive_quota(context);
	}
	પાછા rc;
}


પૂર્ણાંક db_message_ઉમેરો_outgoing(સંરચના mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, સંરચના mosquitto_base_સંદેશ *base_સંદેશ, uint32_t subscription_identifier, bool update, bool persist)
{
	સંરચના mosquitto_client_સંદેશ *client_સંદેશ;
	સંરચના mosquitto_સંદેશ_data *સંદેશ_data;
	ગણના mosquitto_સંદેશ_state state = mosq_ms_invalid;
	પૂર્ણાંક rc = 0;
	અક્ષર **dest_ids;

	પ્રતિજ્ઞા(base_સંદેશ);
	જો(!context){
		પાછા MOSQ_ERR_INVAL;
	}
	જો(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		પાછા MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_sent++;

	સંદેશ_data = &context->msgs_out;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	જો(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && base_સંદેશ->dest_ids){

		માટે(પૂર્ણાંક i=0; i<base_સંદેશ->dest_id_ગણતરી; i++){
			જો(base_સંદેશ->dest_ids[i] && !strcmp(base_સંદેશ->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				પાછા MOSQ_ERR_SUCCESS;
			}
		}
	}
	જો(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		જો(qos == 0 && !db.config->queue_qos0_messages){
			જો(!context->bridge){
				પાછા 2;
			}નહીંતર{
				જો(context->bridge->start_type != bst_lazy){
					પાછા 2;
				}
			}
		}
		જો(context->bridge && context->bridge->clean_start_local == true){
			પાછા 2;
		}
	}

	જો(net__is_connected(context)){
		જો(db__ready_for_flight(context, mosq_md_out, qos)){
			પસંદગી(qos){
				સ્થિતિ 0:
					state = mosq_ms_publish_qos0;
					તોડો;
				સ્થિતિ 1:
					state = mosq_ms_publish_qos1;
					તોડો;
				સ્થિતિ 2:
					state = mosq_ms_publish_qos2;
					તોડો;
			}
		}નહીંતર જો(qos != 0 && db__ready_for_queue(context, qos, સંદેશ_data)){
			state = mosq_ms_queued;
			rc = 2;
		}નહીંતર{
			/* Dropping message due to full queue. */
			જો(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(ખાલી, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			પાછા 2;
		}
	}નહીંતર{
		જો(db__ready_for_queue(context, qos, સંદેશ_data)){
			state = mosq_ms_queued;
		}નહીંતર{
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			જો(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(ખાલી, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			પાછા 2;
		}
	}
	પ્રતિજ્ઞા(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	જો(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_સંદેશ = mosquitto_malloc(કદ(સંરચના mosquitto_client_સંદેશ));
	જો(!client_સંદેશ){
		પાછા MOSQ_ERR_NOMEM;
	}
	client_સંદેશ->prev = ખાલી;
	client_સંદેશ->next = ખાલી;
	જો(cmsg_id){
		client_સંદેશ->data.cmsg_id = cmsg_id;
	}નહીંતર{
		client_સંદેશ->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_સંદેશ->base_સંદેશ = base_સંદેશ;
	db_સંદેશ_store_ref_inc(client_સંદેશ->base_સંદેશ);
	client_સંદેશ->data.mid = mid;
	client_સંદેશ->data.direction = mosq_md_out;
	client_સંદેશ->data.state = (uint8_t)state;
	client_સંદેશ->data.dup = false;
	જો(qos > context->max_qos){
		client_સંદેશ->data.qos = context->max_qos;
	}નહીંતર{
		client_સંદેશ->data.qos = qos;
	}
	client_સંદેશ->data.retain = retain;
	client_સંદેશ->data.subscription_identifier = subscription_identifier;

	જો(state == mosq_ms_queued){
		DL_APPEND(સંદેશ_data->queued, client_સંદેશ);
		db_સંદેશ_add_to_queued_stats(સંદેશ_data, client_સંદેશ);
	}નહીંતર{
		DL_APPEND(સંદેશ_data->inflight, client_સંદેશ);
		db_સંદેશ_add_to_inflight_stats(સંદેશ_data, client_સંદેશ);
	}

	જો(persist && context->is_persisted){
		plugin_persist_handle_base_સંદેશ_add(client_સંદેશ->base_સંદેશ);
		plugin_persist_handle_client_સંદેશ_add(context, client_સંદેશ);
	}

	જો(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(base_સંદેશ->dest_ids, કદ(અક્ષર *)*(size_t)(base_સંદેશ->dest_id_ગણતરી+1));
		જો(dest_ids){
			base_સંદેશ->dest_ids = dest_ids;
			base_સંદેશ->dest_id_ગણતરી++;
			base_સંદેશ->dest_ids[base_સંદેશ->dest_id_ગણતરી-1] = mosquitto_strdup(context->id);
			જો(!base_સંદેશ->dest_ids[base_સંદેશ->dest_id_ગણતરી-1]){
				પાછા MOSQ_ERR_NOMEM;
			}
		}નહીંતર{
			પાછા MOSQ_ERR_NOMEM;
		}
	}
#ifdef WITH_BRIDGE
	જો(context->bridge && context->bridge->start_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_out.inflight_ગણતરી + context->msgs_out.queued_ગણતરી >= context->bridge->threshold){

		context->bridge->lazy_reconnect = true;
	}
#endif

	જો(client_સંદેશ->data.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	જો(update){
		rc = db__message_write_inflight_out_latest(context);
		જો(rc){
			પાછા rc;
		}
		rc = db__message_write_queued_out(context);
		જો(rc){
			પાછા rc;
		}
	}

	પાછા rc;
}


સ્થિર અંતઃરેખ પૂર્ણાંક db__message_update_outgoing_state(સંરચના mosquitto *context, સંરચના mosquitto_client_સંદેશ *head,
		uint16_t mid, ગણના mosquitto_સંદેશ_state state, પૂર્ણાંક qos, bool persist)
{
	સંરચના mosquitto_client_સંદેશ *client_સંદેશ;

	DL_FOREACH(head, client_સંદેશ){
		જો(client_સંદેશ->data.mid == mid){
			જો(client_સંદેશ->data.qos != qos){
				log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_સંદેશ->data.qos, qos);
				પાછા MOSQ_ERR_PROTOCOL;
			}
			client_સંદેશ->data.state = (uint8_t)state;
			જો(persist){
				plugin_persist_handle_client_સંદેશ_update(context, client_સંદેશ);
			}
			પાછા MOSQ_ERR_SUCCESS;
		}
	}
	પાછા MOSQ_ERR_NOT_FOUND;
}


પૂર્ણાંક db__message_update_outgoing(સંરચના mosquitto *context, uint16_t mid, ગણના mosquitto_સંદેશ_state state, પૂર્ણાંક qos, bool persist)
{
	પૂર્ણાંક rc;

	rc = db__message_update_outgoing_state(context, context->msgs_out.inflight, mid, state, qos, persist);
	જો(!persist && rc == MOSQ_ERR_NOT_FOUND){
		rc = db__message_update_outgoing_state(context, context->msgs_out.queued, mid, state, qos, persist);
	}
	પાછા rc;
}


સ્થિર શૂન્ય db__messages_delete_list(સંરચના mosquitto_client_સંદેશ **head)
{
	સંરચના mosquitto_client_સંદેશ *client_સંદેશ, *tmp;

	DL_FOREACH_SAFE(*head, client_સંદેશ, tmp){
		DL_DELETE(*head, client_સંદેશ);
		db_સંદેશ_store_ref_dec(&client_સંદેશ->base_સંદેશ);
		mosquitto_FREE(client_સંદેશ);
	}
	*head = ખાલી;
}


પૂર્ણાંક db__messages_delete_incoming(સંરચના mosquitto *context)
{
	જો(!context){
		પાછા MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_in.inflight);
	db__messages_delete_list(&context->msgs_in.queued);
	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_ગણતરી = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_ગણતરી = 0;
	context->msgs_in.queued_count12 = 0;

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક db__messages_delete_outgoing(સંરચના mosquitto *context)
{
	જો(!context){
		પાછા MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_out.inflight);
	db__messages_delete_list(&context->msgs_out.queued);
	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_ગણતરી = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_ગણતરી = 0;
	context->msgs_out.queued_count12 = 0;

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક db__messages_delete(સંરચના mosquitto *context, bool force_free)
{
	જો(!context){
		પાછા MOSQ_ERR_INVAL;
	}

	જો(force_free || context->clean_start || (context->bridge && context->bridge->clean_start)){
		db__messages_delete_incoming(context);
	}

	જો(force_free || (context->bridge && context->bridge->clean_start_local)
			|| (context->bridge == ખાલી && context->clean_start)){

		db__messages_delete_outgoing(context);
	}

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક db__messages_easy_queue(સંરચના mosquitto *context, અચલ અક્ષર *topic, uint8_t qos, uint32_t payloadlen, અચલ શૂન્ય *payload, પૂર્ણાંક retain, uint32_t message_expiry_interval, mosquitto_property **properties)
{
	સંરચના mosquitto_base_સંદેશ *base_સંદેશ;
	અચલ અક્ષર *source_id;
	ગણના mosquitto_સંદેશ_origin origin;

	જો(!topic){
		પાછા MOSQ_ERR_INVAL;
	}

	base_સંદેશ = mosquitto_calloc(1, કદ(સંરચના mosquitto_base_સંદેશ));
	જો(base_સંદેશ == ખાલી){
		પાછા MOSQ_ERR_NOMEM;
	}

	base_સંદેશ->data.topic = mosquitto_strdup(topic);
	જો(base_સંદેશ->data.topic == ખાલી){
		db_સંદેશ_store_free(base_સંદેશ);
		પાછા MOSQ_ERR_INVAL;
	}

	base_સંદેશ->data.qos = qos;
	જો(db.config->retain_available == false){
		base_સંદેશ->data.retain = 0;
	}નહીંતર{
		base_સંદેશ->data.retain = retain;
	}

	base_સંદેશ->data.payloadlen = payloadlen;
	જો(payloadlen > 0){
		base_સંદેશ->data.payload = mosquitto_malloc(base_સંદેશ->data.payloadlen+1);
		જો(base_સંદેશ->data.payload == ખાલી){
			db_સંદેશ_store_free(base_સંદેશ);
			પાછા MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_સંદેશ->data.payload)[base_સંદેશ->data.payloadlen] = 0;
		memcpy(base_સંદેશ->data.payload, payload, base_સંદેશ->data.payloadlen);
	}

	જો(context && context->id){
		source_id = context->id;
	}નહીંતર{
		source_id = "";
	}
	જો(properties){
		base_સંદેશ->data.properties = *properties;
		*properties = ખાલી;
	}

	જો(context){
		origin = mosq_mo_client;
	}નહીંતર{
		origin = mosq_mo_broker;
	}
	જો(db__message_store(context, base_સંદેશ, &message_expiry_interval, origin)){
		પાછા 1;
	}

	પાછા sub__messages_queue(source_id, base_સંદેશ->data.qos, &base_સંદેશ);
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
uint64_t db_new_સંદેશ_id(શૂન્ય)
{
#ifdef WIN32
	FILETIME ftime;
	uint64_t ftime64;
#નહીંતર
	સંરચના timespec ts;
#endif
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	long nsec;

	id = db.ગાંઠ_id_shifted; /* Top 10-bits */

#ifdef WIN32
	GetSystemTimePreciseAsFileTime(&ftime);
	ftime64 = (((uint64_t)ftime.dw_ઉપલું_Date_Time)<<32) + ftime.dw_નીચલું_Date_Time;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (long)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#નહીંતર
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#endif
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	જો(id <= db.last_db_id){
		id = db.last_db_id + 1;
	}
	db.last_db_id = id;

	પાછા id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
પૂર્ણાંક db__message_store(અચલ સંરચના mosquitto *source, સંરચના mosquitto_base_સંદેશ *base_સંદેશ, uint32_t *message_expiry_interval, ગણના mosquitto_સંદેશ_origin origin)
{
	પૂર્ણાંક rc;

	પ્રતિજ્ઞા(base_સંદેશ);

	જો(source && source->id){
		base_સંદેશ->data.source_id = mosquitto_strdup(source->id);
	}નહીંતર{
		base_સંદેશ->data.source_id = mosquitto_strdup("");
	}
	જો(!base_સંદેશ->data.source_id){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_સંદેશ_store_free(base_સંદેશ);
		પાછા MOSQ_ERR_NOMEM;
	}

	જો(source && source->username){
		base_સંદેશ->data.source_username = mosquitto_strdup(source->username);
		જો(!base_સંદેશ->data.source_username){
			db_સંદેશ_store_free(base_સંદેશ);
			પાછા MOSQ_ERR_NOMEM;
		}
	}
	જો(source){
		base_સંદેશ->source_listener = source->listener;
	}
	base_સંદેશ->origin = origin;
	જો(message_expiry_interval){
		જો(*message_expiry_interval > 0 && *message_expiry_interval != સંદેશ_EXPIRY_INFINITE){
			base_સંદેશ->data.expiry_time = db.now_real_s + *message_expiry_interval;
		}નહીંતર{
			base_સંદેશ->data.expiry_time = 0;
		}
	}

	base_સંદેશ->dest_ids = ખાલી;
	base_સંદેશ->dest_id_ગણતરી = 0;
	db.સંદેશ_store_ગણતરી++;
	db.સંદેશ_store_bytes += base_સંદેશ->data.payloadlen;

	જો(!base_સંદેશ->data.store_id){
		base_સંદેશ->data.store_id = db_new_સંદેશ_id();
	}

	rc = db_સંદેશ_store_add(base_સંદેશ);
	જો(rc){
		db_સંદેશ_store_free(base_સંદેશ);
		પાછા rc;
	}

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક db__message_store_find(સંરચના mosquitto *context, uint16_t mid, સંરચના mosquitto_client_સંદેશ **client_સંદેશ)
{
	સંરચના mosquitto_client_સંદેશ *cmsg;

	*client_સંદેશ = ખાલી;

	જો(!context){
		પાછા MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_in.inflight, cmsg){
		જો(cmsg->base_સંદેશ->data.source_mid == mid){
			*client_સંદેશ = cmsg;
			પાછા MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_in.queued, cmsg){
		જો(cmsg->base_સંદેશ->data.source_mid == mid){
			*client_સંદેશ = cmsg;
			પાછા MOSQ_ERR_SUCCESS;
		}
	}

	પાછા 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
સ્થિર પૂર્ણાંક db__message_reconnect_reset_outgoing(સંરચના mosquitto *context)
{
	સંરચના mosquitto_client_સંદેશ *client_સંદેશ, *tmp;

	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_ગણતરી = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_ગણતરી = 0;
	context->msgs_out.queued_count12 = 0;
	context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_સંદેશ, tmp){
		db_સંદેશ_add_to_inflight_stats(&context->msgs_out, client_સંદેશ);
		જો(client_સંદેશ->data.qos > 0){
			util__decrement_send_quota(context);
		}

		પસંદગી(client_સંદેશ->data.qos){
			સ્થિતિ 0:
				client_સંદેશ->data.state = mosq_ms_publish_qos0;
				તોડો;
			સ્થિતિ 1:
				client_સંદેશ->data.state = mosq_ms_publish_qos1;
				તોડો;
			સ્થિતિ 2:
				જો(client_સંદેશ->data.state == mosq_ms_wait_for_pubcomp){
					client_સંદેશ->data.state = mosq_ms_resend_pubrel;
				}નહીંતર{
					client_સંદેશ->data.state = mosq_ms_publish_qos2;
				}
				તોડો;
		}
		plugin_persist_handle_client_સંદેશ_update(context, client_સંદેશ);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_out.queued, client_સંદેશ, tmp){
		db_સંદેશ_add_to_queued_stats(&context->msgs_out, client_સંદેશ);
	}
	db__fill_inflight_out_from_queue(context);

	પાછા MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
સ્થિર પૂર્ણાંક db__message_reconnect_reset_incoming(સંરચના mosquitto *context)
{
	સંરચના mosquitto_client_સંદેશ *client_સંદેશ, *tmp;

	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_ગણતરી = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_ગણતરી = 0;
	context->msgs_in.queued_count12 = 0;
	context->msgs_in.inflight_quota = context->msgs_in.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_સંદેશ, tmp){
		db_સંદેશ_add_to_inflight_stats(&context->msgs_in, client_સંદેશ);
		જો(client_સંદેશ->data.qos > 0){
			util__decrement_receive_quota(context);
		}

		જો(client_સંદેશ->data.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db__message_remove_inflight(context, &context->msgs_in, client_સંદેશ);
		}નહીંતર{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_સંદેશ->data.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_in.queued, client_સંદેશ, tmp){
		client_સંદેશ->data.dup = 0;
		db_સંદેશ_add_to_queued_stats(&context->msgs_in, client_સંદેશ);
		જો(db__ready_for_flight(context, mosq_md_in, client_સંદેશ->data.qos)){
			પસંદગી(client_સંદેશ->data.qos){
				સ્થિતિ 0:
					client_સંદેશ->data.state = mosq_ms_publish_qos0;
					તોડો;
				સ્થિતિ 1:
					client_સંદેશ->data.state = mosq_ms_publish_qos1;
					તોડો;
				સ્થિતિ 2:
					client_સંદેશ->data.state = mosq_ms_publish_qos2;
					તોડો;
			}
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_સંદેશ_update(context, client_સંદેશ);
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક db__message_reconnect_reset(સંરચના mosquitto *context)
{
	પૂર્ણાંક rc;

	rc = db__message_reconnect_reset_outgoing(context);
	જો(rc){
		પાછા rc;
	}
	પાછા db__message_reconnect_reset_incoming(context);
}


પૂર્ણાંક db__message_remove_incoming(સંરચના mosquitto *context, uint16_t mid)
{
	સંરચના mosquitto_client_સંદેશ *client_સંદેશ, *tmp;

	જો(!context){
		પાછા MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_સંદેશ, tmp){
		જો(client_સંદેશ->data.mid == mid){
			જો(client_સંદેશ->base_સંદેશ->data.qos != 2){
				log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_સંદેશ->base_સંદેશ->data.qos);
				પાછા MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_in, client_સંદેશ);
			પાછા MOSQ_ERR_SUCCESS;
		}
	}

	પાછા MOSQ_ERR_NOT_FOUND;
}


પૂર્ણાંક db__message_release_incoming(સંરચના mosquitto *context, uint16_t mid)
{
	સંરચના mosquitto_client_સંદેશ *client_સંદેશ, *tmp;
	bool deleted = false;
	પૂર્ણાંક rc;

	જો(!context){
		પાછા MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_સંદેશ, tmp){
		જો(client_સંદેશ->data.mid == mid){
			જો(client_સંદેશ->base_સંદેશ->data.qos != 2){
				log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_સંદેશ->base_સંદેશ->data.qos);
				પાછા MOSQ_ERR_PROTOCOL;
			}
			અચલ અક્ષર *topic = client_સંદેશ->base_સંદેશ->data.topic;
			અચલ અક્ષર *source_id = client_સંદેશ->base_સંદેશ->data.source_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			જો(topic == ખાલી){
				db__message_remove_inflight(context, &context->msgs_in, client_સંદેશ);
				deleted = true;
			}નહીંતર{
				rc = sub__messages_queue(source_id, 2, &client_સંદેશ->base_સંદેશ);
				જો(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db__message_remove_inflight(context, &context->msgs_in, client_સંદેશ);
					deleted = true;
				}નહીંતર{
					પાછા 1;
				}
			}
		}
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_સંદેશ, tmp){
		જો(db__ready_for_flight(context, mosq_md_in, client_સંદેશ->data.qos)){
			તોડો;
		}

		જો(client_સંદેશ->data.qos == 2){
			send__pubrec(context, client_સંદેશ->data.mid, 0, ખાલી);
			client_સંદેશ->data.state = mosq_ms_wait_for_pubrel;
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_સંદેશ_update(context, client_સંદેશ);
		}
	}
	જો(deleted){
		પાછા MOSQ_ERR_SUCCESS;
	}નહીંતર{
		પાછા MOSQ_ERR_NOT_FOUND;
	}
}


શૂન્ય db__expire_all_messages(સંરચના mosquitto *context)
{
	સંરચના mosquitto_client_સંદેશ *client_સંદેશ, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_સંદેશ, tmp){
		જો(client_સંદેશ->base_સંદેશ->data.expiry_time && db.now_real_s > client_સંદેશ->base_સંદેશ->data.expiry_time){
			જો(client_સંદેશ->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_સંદેશ);
		}
	}
	db__fill_inflight_out_from_queue(context);
	DL_FOREACH_SAFE(context->msgs_out.queued, client_સંદેશ, tmp){
		જો(client_સંદેશ->base_સંદેશ->data.expiry_time && db.now_real_s > client_સંદેશ->base_સંદેશ->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_સંદેશ);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.inflight, client_સંદેશ, tmp){
		જો(client_સંદેશ->base_સંદેશ->data.expiry_time && db.now_real_s > client_સંદેશ->base_સંદેશ->data.expiry_time){
			જો(client_સંદેશ->data.qos > 0){
				util__increment_receive_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_in, client_સંદેશ);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.queued, client_સંદેશ, tmp){
		જો(client_સંદેશ->base_સંદેશ->data.expiry_time && db.now_real_s > client_સંદેશ->base_સંદેશ->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_in, client_સંદેશ);
		}
	}
}


સ્થિર શૂન્ય db__client_messages_check_acl(સંરચના mosquitto *context, સંરચના mosquitto_સંદેશ_data *સંદેશ_data, સંરચના mosquitto_client_સંદેશ **head,
		શૂન્ય (*decrement_stats_fn)(સંરચના mosquitto_સંદેશ_data *સંદેશ_data, સંરચના mosquitto_client_સંદેશ *client_સંદેશ))
{
	સંરચના mosquitto_client_સંદેશ *client_સંદેશ, *tmp;
	સંરચના mosquitto_base_સંદેશ *base_સંદેશ;
	પૂર્ણાંક access;

	DL_FOREACH_SAFE((*head), client_સંદેશ, tmp){
		base_સંદેશ = client_સંદેશ->base_સંદેશ;
		જો(client_સંદેશ->data.direction == mosq_md_out){
			access = MOSQ_ACL_READ;
		}નહીંતર{
			access = MOSQ_ACL_WRITE;
		}
		જો(mosquitto_acl_check(context, base_સંદેશ->data.topic,
				base_સંદેશ->data.payloadlen, base_સંદેશ->data.payload,
				base_સંદેશ->data.qos, base_સંદેશ->data.retain,
				base_સંદેશ->data.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*head), client_સંદેશ);
			decrement_stats_fn(સંદેશ_data, client_સંદેશ);
			plugin_persist_handle_client_સંદેશ_delete(context, client_સંદેશ);
			db_સંદેશ_store_ref_dec(&client_સંદેશ->base_સંદેશ);
			mosquitto_FREE(client_સંદેશ);
		}
	}
}


શૂન્ય db__check_acl_of_all_messages(સંરચના mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.inflight, &db_સંદેશ_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.queued, &db_સંદેશ_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.inflight, &db_સંદેશ_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.queued, &db_સંદેશ_remove_from_queued_stats);
}


સ્થિર પૂર્ણાંક db__message_write_inflight_out_single(સંરચના mosquitto *context, સંરચના mosquitto_client_સંદેશ *client_સંદેશ)
{
	સંરચના mosquitto_base_સંદેશ *base_સંદેશ;
	mosquitto_property *base_સંદેશ_props = ખાલી;
	પૂર્ણાંક rc;
	uint16_t mid;
	પૂર્ણાંક retries;
	પૂર્ણાંક retain;
	અચલ અક્ષર *topic;
	uint8_t qos;
	uint32_t payloadlen;
	અચલ શૂન્ય *payload;
	uint32_t expiry_interval;
	uint32_t subscription_id;

	base_સંદેશ = client_સંદેશ->base_સંદેશ;

	expiry_interval = 0;
	જો(base_સંદેશ->data.expiry_time){
		જો(db.now_real_s > base_સંદેશ->data.expiry_time){
			/* Message is expired, must not send. */
			જો(client_સંદેશ->data.direction == mosq_md_out && client_સંદેશ->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_સંદેશ);
			db__fill_inflight_out_from_queue(context);
			પાછા MOSQ_ERR_SUCCESS;
		}નહીંતર{
			expiry_interval = (uint32_t)(base_સંદેશ->data.expiry_time - db.now_real_s);
		}
	}
	mid = client_સંદેશ->data.mid;
	retries = client_સંદેશ->data.dup;
	retain = client_સંદેશ->data.retain;
	topic = base_સંદેશ->data.topic;
	qos = (uint8_t)client_સંદેશ->data.qos;
	payloadlen = base_સંદેશ->data.payloadlen;
	payload = base_સંદેશ->data.payload;
	subscription_id = client_સંદેશ->data.subscription_identifier;
	base_સંદેશ_props = base_સંદેશ->data.properties;

	પસંદગી(client_સંદેશ->data.state){
		સ્થિતિ mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_સંદેશ_props, expiry_interval);
			જો(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_સંદેશ);
			}નહીંતર{
				પાછા rc;
			}
			તોડો;

		સ્થિતિ mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_સંદેશ_props, expiry_interval);
			જો(rc == MOSQ_ERR_SUCCESS){
				client_સંદેશ->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_સંદેશ->data.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_સંદેશ_update(context, client_સંદેશ);
			}નહીંતર જો(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_સંદેશ);
			}નહીંતર{
				પાછા rc;
			}
			તોડો;

		સ્થિતિ mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_સંદેશ_props, expiry_interval);
			જો(rc == MOSQ_ERR_SUCCESS){
				client_સંદેશ->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_સંદેશ->data.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_સંદેશ_update(context, client_સંદેશ);
			}નહીંતર જો(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_સંદેશ);
			}નહીંતર{
				પાછા rc;
			}
			તોડો;

		સ્થિતિ mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, ખાલી);
			જો(!rc){
				client_સંદેશ->data.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_સંદેશ_update(context, client_સંદેશ);
			}નહીંતર{
				પાછા rc;
			}
			તોડો;

		સ્થિતિ mosq_ms_invalid:
		સ્થિતિ mosq_ms_send_pubrec:
		સ્થિતિ mosq_ms_resend_pubcomp:
		સ્થિતિ mosq_ms_wait_for_puback:
		સ્થિતિ mosq_ms_wait_for_pubrec:
		સ્થિતિ mosq_ms_wait_for_pubrel:
		સ્થિતિ mosq_ms_wait_for_pubcomp:
		સ્થિતિ mosq_ms_queued:
			તોડો;
	}
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક db__message_write_inflight_out_all(સંરચના mosquitto *context)
{
	સંરચના mosquitto_client_સંદેશ *client_સંદેશ, *tmp;
	પૂર્ણાંક rc;

	જો(context->state != mosq_cs_active || !net__is_connected(context)){
		પાછા MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_સંદેશ, tmp){
		rc = db__message_write_inflight_out_single(context, client_સંદેશ);
		જો(rc){
			પાછા rc;
		}
	}
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક db__message_write_inflight_out_latest(સંરચના mosquitto *context)
{
	સંરચના mosquitto_client_સંદેશ *client_સંદેશ, *next;
	પૂર્ણાંક rc;

	જો(context->state != mosq_cs_active
			|| !net__is_connected(context)
			|| context->msgs_out.inflight == ખાલી){

		પાછા MOSQ_ERR_SUCCESS;
	}

	જો(context->msgs_out.inflight->prev == context->msgs_out.inflight){
		/* Only one message */
		પાછા db__message_write_inflight_out_single(context, context->msgs_out.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_સંદેશ = context->msgs_out.inflight->prev;
	જ્યારે(client_સંદેશ != context->msgs_out.inflight &&
			(client_સંદેશ->data.state == mosq_ms_publish_qos0
			|| client_સંદેશ->data.state == mosq_ms_publish_qos1
			|| client_સંદેશ->data.state == mosq_ms_publish_qos2)){

		client_સંદેશ = client_સંદેશ->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	જો(client_સંદેશ != context->msgs_out.inflight){
		client_સંદેશ = client_સંદેશ->next;
	}

	જ્યારે(client_સંદેશ){
		next = client_સંદેશ->next;
		rc = db__message_write_inflight_out_single(context, client_સંદેશ);
		જો(rc){
			પાછા rc;
		}
		client_સંદેશ = next;
	}
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક db__message_write_queued_in(સંરચના mosquitto *context)
{
	સંરચના mosquitto_client_સંદેશ *client_સંદેશ, *tmp;
	પૂર્ણાંક rc;

	જો(context->state != mosq_cs_active){
		પાછા MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_સંદેશ, tmp){
		જો(context->msgs_in.inflight_maximum != 0 && context->msgs_in.inflight_quota == 0){
			તોડો;
		}

		જો(client_સંદેશ->data.qos == 2){
			client_સંદેશ->data.state = mosq_ms_send_pubrec;
			db__message_dequeue_first(context, &context->msgs_in);
			rc = send__pubrec(context, client_સંદેશ->data.mid, 0, ખાલી);
			જો(!rc){
				client_સંદેશ->data.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_સંદેશ_update(context, client_સંદેશ);
			}નહીંતર{
				plugin_persist_handle_client_સંદેશ_update(context, client_સંદેશ);
				પાછા rc;
			}
		}
	}
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક db__message_write_queued_out(સંરચના mosquitto *context)
{
	જો(context->state != mosq_cs_active){
		પાછા MOSQ_ERR_SUCCESS;
	}

	db__fill_inflight_out_from_queue(context);

	પાછા MOSQ_ERR_SUCCESS;
}
