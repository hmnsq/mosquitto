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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <utlist.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "send_mosq.h"
#அந்தர்பாவ "sys_tree.h"
#அந்தர்பாவ "util_mosq.h"


/**
 * Is this context ready to take more in flight messages right now?
 * @param context the client context of interest
 * @param qos qos for the packet of interest
 * @return true if more in flight are allowed.
 */
bool db__ready_for_flight(கட்டமைப்பு mosquitto *context, எண்ணிக்கை mosquitto_செய்தி_direction dir, முழு qos)
{
	கட்டமைப்பு mosquitto_செய்தி_data *msgs;
	bool valid_bytes;
	bool valid_மொத்தம்;

	எனில்(dir == mosq_md_out){
		msgs = &context->msgs_out;
	}இல்லையெனில்{
		msgs = &context->msgs_in;
	}

	எனில்(msgs->inflight_maximum == 0 && db.config->max_inflight_bytes == 0){
		திரும்பு true;
	}

	எனில்(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		எனில்(db.config->max_queued_messages == 0 && db.config->max_inflight_bytes == 0){
			திரும்பு true;
		}
		valid_bytes = ((msgs->inflight_bytes - (ssize_t)db.config->max_inflight_bytes) < (ssize_t)db.config->max_queued_bytes);
		எனில்(dir == mosq_md_out){
			valid_மொத்தம் = context->out_packet_மொத்தம் < db.config->max_queued_messages;
		}இல்லையெனில்{
			valid_மொத்தம் = msgs->inflight_மொத்தம் - msgs->inflight_maximum < db.config->max_queued_messages;
		}

		எனில்(db.config->max_queued_messages == 0){
			திரும்பு valid_bytes;
		}
		எனில்(db.config->max_queued_bytes == 0){
			திரும்பு valid_மொத்தம்;
		}
	}இல்லையெனில்{
		valid_bytes = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_bytes;
		valid_மொத்தம் = msgs->inflight_quota > 0;

		எனில்(msgs->inflight_maximum == 0){
			திரும்பு valid_bytes;
		}
		எனில்(db.config->max_inflight_bytes == 0){
			திரும்பு valid_மொத்தம்;
		}
	}

	திரும்பு valid_bytes && valid_மொத்தம்;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db__ready_for_queue(கட்டமைப்பு mosquitto *context, முழு qos, கட்டமைப்பு mosquitto_செய்தி_data *செய்தி_data)
{
	முழு source_மொத்தம்;
	முழு adjust_மொத்தம்;
	long source_bytes;
	ssize_t adjust_bytes = (ssize_t)db.config->max_inflight_bytes;
	bool valid_bytes;
	bool valid_மொத்தம்;

	எனில்(db.config->max_queued_messages == 0 && db.config->max_queued_bytes == 0){
		திரும்பு true;
	}

	எனில்(qos == 0 && db.config->queue_qos0_messages == false){
		திரும்பு false; /* This case is handled in db__ready_for_flight() */
	}இல்லையெனில்{
		source_bytes = (ssize_t)செய்தி_data->queued_bytes12;
		source_மொத்தம் = செய்தி_data->queued_count12;
	}
	adjust_மொத்தம் = செய்தி_data->inflight_maximum;

	/* nothing in flight for offline clients */
	எனில்(!net__is_connected(context)){
		adjust_bytes = 0;
		adjust_மொத்தம் = 0;
	}

	valid_bytes = (source_bytes - (ssize_t)adjust_bytes) < (ssize_t)db.config->max_queued_bytes;
	valid_மொத்தம் = source_மொத்தம் - adjust_மொத்தம் < db.config->max_queued_messages;

	எனில்(db.config->max_queued_bytes == 0){
		திரும்பு valid_மொத்தம்;
	}
	எனில்(db.config->max_queued_messages == 0){
		திரும்பு valid_bytes;
	}

	திரும்பு valid_bytes && valid_மொத்தம்;
}


வெறுமை db_செய்தி_add_to_inflight_stats(கட்டமைப்பு mosquitto_செய்தி_data *செய்தி_data, கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி)
{
	செய்தி_data->inflight_மொத்தம்++;
	செய்தி_data->inflight_bytes += client_செய்தி->base_செய்தி->data.payloadlen;
	எனில்(client_செய்தி->data.qos != 0){
		செய்தி_data->inflight_count12++;
		செய்தி_data->inflight_bytes12 += client_செய்தி->base_செய்தி->data.payloadlen;
	}
}


நிலையான வெறுமை db_செய்தி_remove_from_inflight_stats(கட்டமைப்பு mosquitto_செய்தி_data *செய்தி_data, கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி)
{
	செய்தி_data->inflight_மொத்தம்--;
	செய்தி_data->inflight_bytes -= client_செய்தி->base_செய்தி->data.payloadlen;
	எனில்(client_செய்தி->data.qos != 0){
		செய்தி_data->inflight_count12--;
		செய்தி_data->inflight_bytes12 -= client_செய்தி->base_செய்தி->data.payloadlen;
	}
}


வெறுமை db_செய்தி_add_to_queued_stats(கட்டமைப்பு mosquitto_செய்தி_data *செய்தி_data, கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி)
{
	செய்தி_data->queued_மொத்தம்++;
	செய்தி_data->queued_bytes += client_செய்தி->base_செய்தி->data.payloadlen;
	எனில்(client_செய்தி->data.qos != 0){
		செய்தி_data->queued_count12++;
		செய்தி_data->queued_bytes12 += client_செய்தி->base_செய்தி->data.payloadlen;
	}
}


நிலையான வெறுமை db_செய்தி_remove_from_queued_stats(கட்டமைப்பு mosquitto_செய்தி_data *செய்தி_data, கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி)
{
	செய்தி_data->queued_மொத்தம்--;
	செய்தி_data->queued_bytes -= client_செய்தி->base_செய்தி->data.payloadlen;
	எனில்(client_செய்தி->data.qos != 0){
		செய்தி_data->queued_count12--;
		செய்தி_data->queued_bytes12 -= client_செய்தி->base_செய்தி->data.payloadlen;
	}
}


முழு db__open(கட்டமைப்பு mosquitto__config *config)
{
	எனில்(!config){
		திரும்பு MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = வெற்று;
	db.contexts_by_sock = வெற்று;
	db.contexts_for_free = வெற்று;
#ifdef WITH_BRIDGE
	db.bridges = வெற்று;
	db.bridge_மொத்தம் = 0;
#endif

	/* Initialize the hashtable */
	db.clientid_index_hash = வெற்று;

	db.normal_subs = வெற்று;
	db.shared_subs = வெற்று;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = வெற்று;

#ifdef WITH_PERSISTENCE
	எனில்(persist__restore()){
		திரும்பு 1;
	}
#endif

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான வெறுமை subhier_clean(கட்டமைப்பு mosquitto__subhier **subhier)
{
	கட்டமைப்பு mosquitto__subhier *peer, *subhier_tmp;
	கட்டமைப்பு mosquitto__subleaf *leaf, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		leaf = peer->subs;
		போது(leaf){
			nextleaf = leaf->next;
			mosquitto_FREE(leaf);
			leaf = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


முழு db__close(வெறுமை)
{
	subhier_clean(&db.normal_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db_செய்தி_store_clean();

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு db_செய்தி_store_add(கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி)
{
	கட்டமைப்பு mosquitto_base_செய்தி *found;
	unsigned hashv;

	HASH_மதிப்பு(&base_செய்தி->data.store_id, அளவு(base_செய்தி->data.store_id), hashv);
	HASH_FIND_BYHASHVALUE(hh, db.செய்தி_store, &base_செய்தி->data.store_id, அளவு(base_செய்தி->data.store_id), hashv, found);
	எனில்(found == வெற்று){
		HASH_ADD_KEYPTR_BYHASHVALUE(hh, db.செய்தி_store, &base_செய்தி->data.store_id, அளவு(base_செய்தி->data.store_id), hashv, base_செய்தி);
		திரும்பு MOSQ_ERR_SUCCESS;
	}இல்லையெனில்{
		திரும்பு MOSQ_ERR_ALREADY_EXISTS;
	}
}


வெறுமை db_செய்தி_store_free(கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி)
{
	mosquitto_FREE(base_செய்தி->data.source_id);
	mosquitto_FREE(base_செய்தி->data.source_username);
	எனில்(base_செய்தி->dest_ids){
		க்கு(முழு i=0; i<base_செய்தி->dest_id_மொத்தம்; i++){
			mosquitto_FREE(base_செய்தி->dest_ids[i]);
		}
		mosquitto_FREE(base_செய்தி->dest_ids);
	}
	mosquitto_FREE(base_செய்தி->data.topic);
	mosquitto_property_free_all(&base_செய்தி->data.properties);
	mosquitto_FREE(base_செய்தி->data.payload);
	mosquitto_FREE(base_செய்தி);
}


வெறுமை db_செய்தி_store_remove(கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி, bool notify)
{
	எனில்(base_செய்தி == வெற்று){
		திரும்பு;
	}
	HASH_DELETE(hh, db.செய்தி_store, base_செய்தி);
	db.செய்தி_store_மொத்தம்--;
	db.செய்தி_store_bytes -= base_செய்தி->data.payloadlen;
	எனில்(notify == true){
		plugin_persist_handle_base_செய்தி_delete(base_செய்தி);
	}
	db_செய்தி_store_free(base_செய்தி);
}


வெறுமை db_செய்தி_store_clean(வெறுமை)
{
	கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி, *base_செய்தி_tmp;

	HASH_ITER(hh, db.செய்தி_store, base_செய்தி, base_செய்தி_tmp){
		db_செய்தி_store_remove(base_செய்தி, false);
	}
}


வெறுமை db_செய்தி_store_ref_inc(கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி)
{
	base_செய்தி->ref_மொத்தம்++;
}


வெறுமை db_செய்தி_store_ref_dec(கட்டமைப்பு mosquitto_base_செய்தி **base_செய்தி)
{
	(*base_செய்தி)->ref_மொத்தம்--;
	எனில்((*base_செய்தி)->ref_மொத்தம் == 0){
		db_செய்தி_store_remove(*base_செய்தி, true);
		*base_செய்தி = வெற்று;
	}
}


வெறுமை db_செய்தி_store_compact(வெறுமை)
{
	கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி, *base_செய்தி_tmp;

	HASH_ITER(hh, db.செய்தி_store, base_செய்தி, base_செய்தி_tmp){
		எனில்(base_செய்தி->ref_மொத்தம் < 1){
			db_செய்தி_store_remove(base_செய்தி, true);
		}
	}
}


நிலையான வெறுமை db__message_remove_inflight(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto_செய்தி_data *செய்தி_data, கட்டமைப்பு mosquitto_client_செய்தி *item)
{
	எனில்(!context || !செய்தி_data || !item){
		திரும்பு;
	}

	plugin_persist_handle_client_செய்தி_delete(context, item);

	DL_DELETE(செய்தி_data->inflight, item);
	எனில்(item->base_செய்தி){
		db_செய்தி_remove_from_inflight_stats(செய்தி_data, item);
		db_செய்தி_store_ref_dec(&item->base_செய்தி);
	}

	mosquitto_FREE(item);
}


நிலையான வெறுமை db__message_remove_queued(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto_செய்தி_data *செய்தி_data, கட்டமைப்பு mosquitto_client_செய்தி *item)
{
	எனில்(!context || !செய்தி_data || !item){
		திரும்பு;
	}

	plugin_persist_handle_client_செய்தி_delete(context, item);

	DL_DELETE(செய்தி_data->queued, item);
	எனில்(item->base_செய்தி){
		db_செய்தி_remove_from_queued_stats(செய்தி_data, item);
		db_செய்தி_store_ref_dec(&item->base_செய்தி);
	}

	mosquitto_FREE(item);
}


நிலையான வெறுமை db__fill_inflight_out_from_queue(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.queued, client_செய்தி, tmp){
		எனில்(!db__ready_for_flight(context, mosq_md_out, client_செய்தி->data.qos)){
			திரும்பு;
		}
		தேர்வு(client_செய்தி->data.qos){
			நிலை 0:
				client_செய்தி->data.state = mosq_ms_publish_qos0;
				நிறுத்து;
			நிலை 1:
				client_செய்தி->data.state = mosq_ms_publish_qos1;
				நிறுத்து;
			நிலை 2:
				client_செய்தி->data.state = mosq_ms_publish_qos2;
				நிறுத்து;
		}
		எனில்(client_செய்தி->base_செய்தி->data.expiry_time && db.now_real_s > client_செய்தி->base_செய்தி->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_செய்தி);
			தொடர்;
		}
		plugin_persist_handle_client_செய்தி_update(context, client_செய்தி);
		db__message_dequeue_first(context, &context->msgs_out);
	}
}


வெறுமை db__message_dequeue_first(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto_செய்தி_data *செய்தி_data)
{
	கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி;

	UNUSED(context);

	client_செய்தி = செய்தி_data->queued;
	DL_DELETE(செய்தி_data->queued, client_செய்தி);
	DL_APPEND(செய்தி_data->inflight, client_செய்தி);
	எனில்(செய்தி_data->inflight_quota > 0){
		செய்தி_data->inflight_quota--;
	}

	db_செய்தி_remove_from_queued_stats(செய்தி_data, client_செய்தி);
	db_செய்தி_add_to_inflight_stats(செய்தி_data, client_செய்தி);
}


முழு db__message_delete_outgoing(கட்டமைப்பு mosquitto *context, uint16_t mid, எண்ணிக்கை mosquitto_செய்தி_state expect_state, முழு qos)
{
	கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி, *tmp;
	bool deleted = false;

	எனில்(!context){
		திரும்பு MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_செய்தி, tmp){
		எனில்(client_செய்தி->data.mid == mid){
			எனில்(client_செய்தி->data.qos != qos){
				log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_செய்தி->data.qos, qos);
				திரும்பு MOSQ_ERR_PROTOCOL;
			}இல்லையெனில் எனில்(qos == 2 && client_செய்தி->data.state != expect_state && expect_state != mosq_ms_any){
				log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_செய்தி->data.state, expect_state);
				திரும்பு MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_out, client_செய்தி);
			deleted = true;
			நிறுத்து;
		}
	}

	எனில்(deleted == false){
		DL_FOREACH_SAFE(context->msgs_out.queued, client_செய்தி, tmp){
			எனில்(client_செய்தி->data.mid == mid){
				எனில்(client_செய்தி->data.qos != qos){
					திரும்பு MOSQ_ERR_PROTOCOL;
				}இல்லையெனில் எனில்(qos == 2 && client_செய்தி->data.state != expect_state && expect_state != mosq_ms_any){
					திரும்பு MOSQ_ERR_PROTOCOL;
				}
				db__message_remove_queued(context, &context->msgs_out, client_செய்தி);
				நிறுத்து;
			}
		}
	}
	db__fill_inflight_out_from_queue(context);
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	திரும்பு db__message_write_inflight_out_latest(context);
}


/* Only for QoS 2 messages */
முழு db_message_செருகு_incoming(கட்டமைப்பு mosquitto *context, uint64_t cmsg_id, கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி, bool persist)
{
	கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி;
	கட்டமைப்பு mosquitto_செய்தி_data *செய்தி_data;
	எண்ணிக்கை mosquitto_செய்தி_state state = mosq_ms_invalid;
	முழு rc = 0;

	ப்ரதிஜ்ஞா(base_செய்தி);
	எனில்(!context){
		திரும்பு MOSQ_ERR_INVAL;
	}
	எனில்(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		திரும்பு MOSQ_ERR_SUCCESS;

	}
	செய்தி_data = &context->msgs_in;

	எனில்(db__ready_for_flight(context, mosq_md_in, base_செய்தி->data.qos)){
		state = mosq_ms_wait_for_pubrel;
	}இல்லையெனில் எனில்(base_செய்தி->data.qos != 0 && db__ready_for_queue(context, base_செய்தி->data.qos, செய்தி_data)){
		state = mosq_ms_queued;
		rc = 2;
	}இல்லையெனில்{
		/* Dropping message due to full queue. */
		எனில்(context->is_dropping == false){
			context->is_dropping = true;
			log__printf(வெற்று, MOSQ_LOG_NOTICE,
					"Outgoing messages are being dropped for client %s.",
					context->id);
		}
		metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		திரும்பு 2;
	}

	ப்ரதிஜ்ஞா(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	எனில்(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_செய்தி = mosquitto_malloc(அளவு(கட்டமைப்பு mosquitto_client_செய்தி));
	எனில்(!client_செய்தி){
		திரும்பு MOSQ_ERR_NOMEM;
	}
	client_செய்தி->prev = வெற்று;
	client_செய்தி->next = வெற்று;
	எனில்(cmsg_id){
		client_செய்தி->data.cmsg_id = cmsg_id;
	}இல்லையெனில்{
		client_செய்தி->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_செய்தி->base_செய்தி = base_செய்தி;
	db_செய்தி_store_ref_inc(client_செய்தி->base_செய்தி);
	client_செய்தி->data.mid = base_செய்தி->data.source_mid;
	client_செய்தி->data.direction = mosq_md_in;
	client_செய்தி->data.state = (uint8_t)state;
	client_செய்தி->data.dup = false;
	எனில்(base_செய்தி->data.qos > context->max_qos){
		client_செய்தி->data.qos = context->max_qos;
	}இல்லையெனில்{
		client_செய்தி->data.qos = base_செய்தி->data.qos;
	}
	client_செய்தி->data.retain = base_செய்தி->data.retain;
	client_செய்தி->data.subscription_identifier = 0;

	எனில்(state == mosq_ms_queued){
		DL_APPEND(செய்தி_data->queued, client_செய்தி);
		db_செய்தி_add_to_queued_stats(செய்தி_data, client_செய்தி);
	}இல்லையெனில்{
		DL_APPEND(செய்தி_data->inflight, client_செய்தி);
		db_செய்தி_add_to_inflight_stats(செய்தி_data, client_செய்தி);
	}

	எனில்(persist && context->is_persisted){
		plugin_persist_handle_base_செய்தி_add(client_செய்தி->base_செய்தி);
		plugin_persist_handle_client_செய்தி_add(context, client_செய்தி);
	}

	எனில்(client_செய்தி->base_செய்தி->data.qos > 0){
		util__decrement_receive_quota(context);
	}
	திரும்பு rc;
}


முழு db_message_செருகு_outgoing(கட்டமைப்பு mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி, uint32_t subscription_identifier, bool update, bool persist)
{
	கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி;
	கட்டமைப்பு mosquitto_செய்தி_data *செய்தி_data;
	எண்ணிக்கை mosquitto_செய்தி_state state = mosq_ms_invalid;
	முழு rc = 0;
	எழுத்து **dest_ids;

	ப்ரதிஜ்ஞா(base_செய்தி);
	எனில்(!context){
		திரும்பு MOSQ_ERR_INVAL;
	}
	எனில்(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		திரும்பு MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_sent++;

	செய்தி_data = &context->msgs_out;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	எனில்(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && base_செய்தி->dest_ids){

		க்கு(முழு i=0; i<base_செய்தி->dest_id_மொத்தம்; i++){
			எனில்(base_செய்தி->dest_ids[i] && !strcmp(base_செய்தி->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				திரும்பு MOSQ_ERR_SUCCESS;
			}
		}
	}
	எனில்(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		எனில்(qos == 0 && !db.config->queue_qos0_messages){
			எனில்(!context->bridge){
				திரும்பு 2;
			}இல்லையெனில்{
				எனில்(context->bridge->start_type != bst_lazy){
					திரும்பு 2;
				}
			}
		}
		எனில்(context->bridge && context->bridge->clean_start_local == true){
			திரும்பு 2;
		}
	}

	எனில்(net__is_connected(context)){
		எனில்(db__ready_for_flight(context, mosq_md_out, qos)){
			தேர்வு(qos){
				நிலை 0:
					state = mosq_ms_publish_qos0;
					நிறுத்து;
				நிலை 1:
					state = mosq_ms_publish_qos1;
					நிறுத்து;
				நிலை 2:
					state = mosq_ms_publish_qos2;
					நிறுத்து;
			}
		}இல்லையெனில் எனில்(qos != 0 && db__ready_for_queue(context, qos, செய்தி_data)){
			state = mosq_ms_queued;
			rc = 2;
		}இல்லையெனில்{
			/* Dropping message due to full queue. */
			எனில்(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(வெற்று, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			திரும்பு 2;
		}
	}இல்லையெனில்{
		எனில்(db__ready_for_queue(context, qos, செய்தி_data)){
			state = mosq_ms_queued;
		}இல்லையெனில்{
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			எனில்(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(வெற்று, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			திரும்பு 2;
		}
	}
	ப்ரதிஜ்ஞா(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	எனில்(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_செய்தி = mosquitto_malloc(அளவு(கட்டமைப்பு mosquitto_client_செய்தி));
	எனில்(!client_செய்தி){
		திரும்பு MOSQ_ERR_NOMEM;
	}
	client_செய்தி->prev = வெற்று;
	client_செய்தி->next = வெற்று;
	எனில்(cmsg_id){
		client_செய்தி->data.cmsg_id = cmsg_id;
	}இல்லையெனில்{
		client_செய்தி->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_செய்தி->base_செய்தி = base_செய்தி;
	db_செய்தி_store_ref_inc(client_செய்தி->base_செய்தி);
	client_செய்தி->data.mid = mid;
	client_செய்தி->data.direction = mosq_md_out;
	client_செய்தி->data.state = (uint8_t)state;
	client_செய்தி->data.dup = false;
	எனில்(qos > context->max_qos){
		client_செய்தி->data.qos = context->max_qos;
	}இல்லையெனில்{
		client_செய்தி->data.qos = qos;
	}
	client_செய்தி->data.retain = retain;
	client_செய்தி->data.subscription_identifier = subscription_identifier;

	எனில்(state == mosq_ms_queued){
		DL_APPEND(செய்தி_data->queued, client_செய்தி);
		db_செய்தி_add_to_queued_stats(செய்தி_data, client_செய்தி);
	}இல்லையெனில்{
		DL_APPEND(செய்தி_data->inflight, client_செய்தி);
		db_செய்தி_add_to_inflight_stats(செய்தி_data, client_செய்தி);
	}

	எனில்(persist && context->is_persisted){
		plugin_persist_handle_base_செய்தி_add(client_செய்தி->base_செய்தி);
		plugin_persist_handle_client_செய்தி_add(context, client_செய்தி);
	}

	எனில்(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(base_செய்தி->dest_ids, அளவு(எழுத்து *)*(size_t)(base_செய்தி->dest_id_மொத்தம்+1));
		எனில்(dest_ids){
			base_செய்தி->dest_ids = dest_ids;
			base_செய்தி->dest_id_மொத்தம்++;
			base_செய்தி->dest_ids[base_செய்தி->dest_id_மொத்தம்-1] = mosquitto_strdup(context->id);
			எனில்(!base_செய்தி->dest_ids[base_செய்தி->dest_id_மொத்தம்-1]){
				திரும்பு MOSQ_ERR_NOMEM;
			}
		}இல்லையெனில்{
			திரும்பு MOSQ_ERR_NOMEM;
		}
	}
#ifdef WITH_BRIDGE
	எனில்(context->bridge && context->bridge->start_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_out.inflight_மொத்தம் + context->msgs_out.queued_மொத்தம் >= context->bridge->threshold){

		context->bridge->lazy_reconnect = true;
	}
#endif

	எனில்(client_செய்தி->data.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	எனில்(update){
		rc = db__message_write_inflight_out_latest(context);
		எனில்(rc){
			திரும்பு rc;
		}
		rc = db__message_write_queued_out(context);
		எனில்(rc){
			திரும்பு rc;
		}
	}

	திரும்பு rc;
}


நிலையான உள்வரி முழு db__message_update_outgoing_state(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto_client_செய்தி *head,
		uint16_t mid, எண்ணிக்கை mosquitto_செய்தி_state state, முழு qos, bool persist)
{
	கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி;

	DL_FOREACH(head, client_செய்தி){
		எனில்(client_செய்தி->data.mid == mid){
			எனில்(client_செய்தி->data.qos != qos){
				log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_செய்தி->data.qos, qos);
				திரும்பு MOSQ_ERR_PROTOCOL;
			}
			client_செய்தி->data.state = (uint8_t)state;
			எனில்(persist){
				plugin_persist_handle_client_செய்தி_update(context, client_செய்தி);
			}
			திரும்பு MOSQ_ERR_SUCCESS;
		}
	}
	திரும்பு MOSQ_ERR_NOT_FOUND;
}


முழு db__message_update_outgoing(கட்டமைப்பு mosquitto *context, uint16_t mid, எண்ணிக்கை mosquitto_செய்தி_state state, முழு qos, bool persist)
{
	முழு rc;

	rc = db__message_update_outgoing_state(context, context->msgs_out.inflight, mid, state, qos, persist);
	எனில்(!persist && rc == MOSQ_ERR_NOT_FOUND){
		rc = db__message_update_outgoing_state(context, context->msgs_out.queued, mid, state, qos, persist);
	}
	திரும்பு rc;
}


நிலையான வெறுமை db__messages_delete_list(கட்டமைப்பு mosquitto_client_செய்தி **head)
{
	கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி, *tmp;

	DL_FOREACH_SAFE(*head, client_செய்தி, tmp){
		DL_DELETE(*head, client_செய்தி);
		db_செய்தி_store_ref_dec(&client_செய்தி->base_செய்தி);
		mosquitto_FREE(client_செய்தி);
	}
	*head = வெற்று;
}


முழு db__messages_delete_incoming(கட்டமைப்பு mosquitto *context)
{
	எனில்(!context){
		திரும்பு MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_in.inflight);
	db__messages_delete_list(&context->msgs_in.queued);
	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_மொத்தம் = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_மொத்தம் = 0;
	context->msgs_in.queued_count12 = 0;

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு db__messages_delete_outgoing(கட்டமைப்பு mosquitto *context)
{
	எனில்(!context){
		திரும்பு MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_out.inflight);
	db__messages_delete_list(&context->msgs_out.queued);
	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_மொத்தம் = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_மொத்தம் = 0;
	context->msgs_out.queued_count12 = 0;

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு db__messages_delete(கட்டமைப்பு mosquitto *context, bool force_free)
{
	எனில்(!context){
		திரும்பு MOSQ_ERR_INVAL;
	}

	எனில்(force_free || context->clean_start || (context->bridge && context->bridge->clean_start)){
		db__messages_delete_incoming(context);
	}

	எனில்(force_free || (context->bridge && context->bridge->clean_start_local)
			|| (context->bridge == வெற்று && context->clean_start)){

		db__messages_delete_outgoing(context);
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு db__messages_easy_queue(கட்டமைப்பு mosquitto *context, மாறிலி எழுத்து *topic, uint8_t qos, uint32_t payloadlen, மாறிலி வெறுமை *payload, முழு retain, uint32_t message_expiry_interval, mosquitto_property **properties)
{
	கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி;
	மாறிலி எழுத்து *source_id;
	எண்ணிக்கை mosquitto_செய்தி_origin origin;

	எனில்(!topic){
		திரும்பு MOSQ_ERR_INVAL;
	}

	base_செய்தி = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto_base_செய்தி));
	எனில்(base_செய்தி == வெற்று){
		திரும்பு MOSQ_ERR_NOMEM;
	}

	base_செய்தி->data.topic = mosquitto_strdup(topic);
	எனில்(base_செய்தி->data.topic == வெற்று){
		db_செய்தி_store_free(base_செய்தி);
		திரும்பு MOSQ_ERR_INVAL;
	}

	base_செய்தி->data.qos = qos;
	எனில்(db.config->retain_available == false){
		base_செய்தி->data.retain = 0;
	}இல்லையெனில்{
		base_செய்தி->data.retain = retain;
	}

	base_செய்தி->data.payloadlen = payloadlen;
	எனில்(payloadlen > 0){
		base_செய்தி->data.payload = mosquitto_malloc(base_செய்தி->data.payloadlen+1);
		எனில்(base_செய்தி->data.payload == வெற்று){
			db_செய்தி_store_free(base_செய்தி);
			திரும்பு MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_செய்தி->data.payload)[base_செய்தி->data.payloadlen] = 0;
		memcpy(base_செய்தி->data.payload, payload, base_செய்தி->data.payloadlen);
	}

	எனில்(context && context->id){
		source_id = context->id;
	}இல்லையெனில்{
		source_id = "";
	}
	எனில்(properties){
		base_செய்தி->data.properties = *properties;
		*properties = வெற்று;
	}

	எனில்(context){
		origin = mosq_mo_client;
	}இல்லையெனில்{
		origin = mosq_mo_broker;
	}
	எனில்(db__message_store(context, base_செய்தி, &message_expiry_interval, origin)){
		திரும்பு 1;
	}

	திரும்பு sub__messages_queue(source_id, base_செய்தி->data.qos, &base_செய்தி);
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
uint64_t db_new_செய்தி_id(வெறுமை)
{
#ifdef WIN32
	FILETIME ftime;
	uint64_t ftime64;
#இல்லையெனில்
	கட்டமைப்பு timespec ts;
#endif
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	long nsec;

	id = db.முனை_id_shifted; /* Top 10-bits */

#ifdef WIN32
	GetSystemTimePreciseAsFileTime(&ftime);
	ftime64 = (((uint64_t)ftime.dw_உயர்வு_Date_Time)<<32) + ftime.dw_கீழ்_Date_Time;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (long)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#இல்லையெனில்
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#endif
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	எனில்(id <= db.last_db_id){
		id = db.last_db_id + 1;
	}
	db.last_db_id = id;

	திரும்பு id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
முழு db__message_store(மாறிலி கட்டமைப்பு mosquitto *source, கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி, uint32_t *message_expiry_interval, எண்ணிக்கை mosquitto_செய்தி_origin origin)
{
	முழு rc;

	ப்ரதிஜ்ஞா(base_செய்தி);

	எனில்(source && source->id){
		base_செய்தி->data.source_id = mosquitto_strdup(source->id);
	}இல்லையெனில்{
		base_செய்தி->data.source_id = mosquitto_strdup("");
	}
	எனில்(!base_செய்தி->data.source_id){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_செய்தி_store_free(base_செய்தி);
		திரும்பு MOSQ_ERR_NOMEM;
	}

	எனில்(source && source->username){
		base_செய்தி->data.source_username = mosquitto_strdup(source->username);
		எனில்(!base_செய்தி->data.source_username){
			db_செய்தி_store_free(base_செய்தி);
			திரும்பு MOSQ_ERR_NOMEM;
		}
	}
	எனில்(source){
		base_செய்தி->source_listener = source->listener;
	}
	base_செய்தி->origin = origin;
	எனில்(message_expiry_interval){
		எனில்(*message_expiry_interval > 0 && *message_expiry_interval != செய்தி_EXPIRY_INFINITE){
			base_செய்தி->data.expiry_time = db.now_real_s + *message_expiry_interval;
		}இல்லையெனில்{
			base_செய்தி->data.expiry_time = 0;
		}
	}

	base_செய்தி->dest_ids = வெற்று;
	base_செய்தி->dest_id_மொத்தம் = 0;
	db.செய்தி_store_மொத்தம்++;
	db.செய்தி_store_bytes += base_செய்தி->data.payloadlen;

	எனில்(!base_செய்தி->data.store_id){
		base_செய்தி->data.store_id = db_new_செய்தி_id();
	}

	rc = db_செய்தி_store_add(base_செய்தி);
	எனில்(rc){
		db_செய்தி_store_free(base_செய்தி);
		திரும்பு rc;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு db__message_store_find(கட்டமைப்பு mosquitto *context, uint16_t mid, கட்டமைப்பு mosquitto_client_செய்தி **client_செய்தி)
{
	கட்டமைப்பு mosquitto_client_செய்தி *cmsg;

	*client_செய்தி = வெற்று;

	எனில்(!context){
		திரும்பு MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_in.inflight, cmsg){
		எனில்(cmsg->base_செய்தி->data.source_mid == mid){
			*client_செய்தி = cmsg;
			திரும்பு MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_in.queued, cmsg){
		எனில்(cmsg->base_செய்தி->data.source_mid == mid){
			*client_செய்தி = cmsg;
			திரும்பு MOSQ_ERR_SUCCESS;
		}
	}

	திரும்பு 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
நிலையான முழு db__message_reconnect_reset_outgoing(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி, *tmp;

	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_மொத்தம் = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_மொத்தம் = 0;
	context->msgs_out.queued_count12 = 0;
	context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_செய்தி, tmp){
		db_செய்தி_add_to_inflight_stats(&context->msgs_out, client_செய்தி);
		எனில்(client_செய்தி->data.qos > 0){
			util__decrement_send_quota(context);
		}

		தேர்வு(client_செய்தி->data.qos){
			நிலை 0:
				client_செய்தி->data.state = mosq_ms_publish_qos0;
				நிறுத்து;
			நிலை 1:
				client_செய்தி->data.state = mosq_ms_publish_qos1;
				நிறுத்து;
			நிலை 2:
				எனில்(client_செய்தி->data.state == mosq_ms_wait_for_pubcomp){
					client_செய்தி->data.state = mosq_ms_resend_pubrel;
				}இல்லையெனில்{
					client_செய்தி->data.state = mosq_ms_publish_qos2;
				}
				நிறுத்து;
		}
		plugin_persist_handle_client_செய்தி_update(context, client_செய்தி);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_out.queued, client_செய்தி, tmp){
		db_செய்தி_add_to_queued_stats(&context->msgs_out, client_செய்தி);
	}
	db__fill_inflight_out_from_queue(context);

	திரும்பு MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
நிலையான முழு db__message_reconnect_reset_incoming(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி, *tmp;

	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_மொத்தம் = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_மொத்தம் = 0;
	context->msgs_in.queued_count12 = 0;
	context->msgs_in.inflight_quota = context->msgs_in.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_செய்தி, tmp){
		db_செய்தி_add_to_inflight_stats(&context->msgs_in, client_செய்தி);
		எனில்(client_செய்தி->data.qos > 0){
			util__decrement_receive_quota(context);
		}

		எனில்(client_செய்தி->data.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db__message_remove_inflight(context, &context->msgs_in, client_செய்தி);
		}இல்லையெனில்{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_செய்தி->data.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_in.queued, client_செய்தி, tmp){
		client_செய்தி->data.dup = 0;
		db_செய்தி_add_to_queued_stats(&context->msgs_in, client_செய்தி);
		எனில்(db__ready_for_flight(context, mosq_md_in, client_செய்தி->data.qos)){
			தேர்வு(client_செய்தி->data.qos){
				நிலை 0:
					client_செய்தி->data.state = mosq_ms_publish_qos0;
					நிறுத்து;
				நிலை 1:
					client_செய்தி->data.state = mosq_ms_publish_qos1;
					நிறுத்து;
				நிலை 2:
					client_செய்தி->data.state = mosq_ms_publish_qos2;
					நிறுத்து;
			}
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_செய்தி_update(context, client_செய்தி);
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு db__message_reconnect_reset(கட்டமைப்பு mosquitto *context)
{
	முழு rc;

	rc = db__message_reconnect_reset_outgoing(context);
	எனில்(rc){
		திரும்பு rc;
	}
	திரும்பு db__message_reconnect_reset_incoming(context);
}


முழு db__message_remove_incoming(கட்டமைப்பு mosquitto *context, uint16_t mid)
{
	கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி, *tmp;

	எனில்(!context){
		திரும்பு MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_செய்தி, tmp){
		எனில்(client_செய்தி->data.mid == mid){
			எனில்(client_செய்தி->base_செய்தி->data.qos != 2){
				log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_செய்தி->base_செய்தி->data.qos);
				திரும்பு MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_in, client_செய்தி);
			திரும்பு MOSQ_ERR_SUCCESS;
		}
	}

	திரும்பு MOSQ_ERR_NOT_FOUND;
}


முழு db__message_release_incoming(கட்டமைப்பு mosquitto *context, uint16_t mid)
{
	கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி, *tmp;
	bool deleted = false;
	முழு rc;

	எனில்(!context){
		திரும்பு MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_செய்தி, tmp){
		எனில்(client_செய்தி->data.mid == mid){
			எனில்(client_செய்தி->base_செய்தி->data.qos != 2){
				log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_செய்தி->base_செய்தி->data.qos);
				திரும்பு MOSQ_ERR_PROTOCOL;
			}
			மாறிலி எழுத்து *topic = client_செய்தி->base_செய்தி->data.topic;
			மாறிலி எழுத்து *source_id = client_செய்தி->base_செய்தி->data.source_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			எனில்(topic == வெற்று){
				db__message_remove_inflight(context, &context->msgs_in, client_செய்தி);
				deleted = true;
			}இல்லையெனில்{
				rc = sub__messages_queue(source_id, 2, &client_செய்தி->base_செய்தி);
				எனில்(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db__message_remove_inflight(context, &context->msgs_in, client_செய்தி);
					deleted = true;
				}இல்லையெனில்{
					திரும்பு 1;
				}
			}
		}
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_செய்தி, tmp){
		எனில்(db__ready_for_flight(context, mosq_md_in, client_செய்தி->data.qos)){
			நிறுத்து;
		}

		எனில்(client_செய்தி->data.qos == 2){
			send__pubrec(context, client_செய்தி->data.mid, 0, வெற்று);
			client_செய்தி->data.state = mosq_ms_wait_for_pubrel;
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_செய்தி_update(context, client_செய்தி);
		}
	}
	எனில்(deleted){
		திரும்பு MOSQ_ERR_SUCCESS;
	}இல்லையெனில்{
		திரும்பு MOSQ_ERR_NOT_FOUND;
	}
}


வெறுமை db__expire_all_messages(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_செய்தி, tmp){
		எனில்(client_செய்தி->base_செய்தி->data.expiry_time && db.now_real_s > client_செய்தி->base_செய்தி->data.expiry_time){
			எனில்(client_செய்தி->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_செய்தி);
		}
	}
	db__fill_inflight_out_from_queue(context);
	DL_FOREACH_SAFE(context->msgs_out.queued, client_செய்தி, tmp){
		எனில்(client_செய்தி->base_செய்தி->data.expiry_time && db.now_real_s > client_செய்தி->base_செய்தி->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_செய்தி);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.inflight, client_செய்தி, tmp){
		எனில்(client_செய்தி->base_செய்தி->data.expiry_time && db.now_real_s > client_செய்தி->base_செய்தி->data.expiry_time){
			எனில்(client_செய்தி->data.qos > 0){
				util__increment_receive_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_in, client_செய்தி);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.queued, client_செய்தி, tmp){
		எனில்(client_செய்தி->base_செய்தி->data.expiry_time && db.now_real_s > client_செய்தி->base_செய்தி->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_in, client_செய்தி);
		}
	}
}


நிலையான வெறுமை db__client_messages_check_acl(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto_செய்தி_data *செய்தி_data, கட்டமைப்பு mosquitto_client_செய்தி **head,
		வெறுமை (*decrement_stats_fn)(கட்டமைப்பு mosquitto_செய்தி_data *செய்தி_data, கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி))
{
	கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி, *tmp;
	கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி;
	முழு access;

	DL_FOREACH_SAFE((*head), client_செய்தி, tmp){
		base_செய்தி = client_செய்தி->base_செய்தி;
		எனில்(client_செய்தி->data.direction == mosq_md_out){
			access = MOSQ_ACL_READ;
		}இல்லையெனில்{
			access = MOSQ_ACL_WRITE;
		}
		எனில்(mosquitto_acl_check(context, base_செய்தி->data.topic,
				base_செய்தி->data.payloadlen, base_செய்தி->data.payload,
				base_செய்தி->data.qos, base_செய்தி->data.retain,
				base_செய்தி->data.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*head), client_செய்தி);
			decrement_stats_fn(செய்தி_data, client_செய்தி);
			plugin_persist_handle_client_செய்தி_delete(context, client_செய்தி);
			db_செய்தி_store_ref_dec(&client_செய்தி->base_செய்தி);
			mosquitto_FREE(client_செய்தி);
		}
	}
}


வெறுமை db__check_acl_of_all_messages(கட்டமைப்பு mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.inflight, &db_செய்தி_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.queued, &db_செய்தி_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.inflight, &db_செய்தி_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.queued, &db_செய்தி_remove_from_queued_stats);
}


நிலையான முழு db__message_write_inflight_out_single(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி)
{
	கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி;
	mosquitto_property *base_செய்தி_props = வெற்று;
	முழு rc;
	uint16_t mid;
	முழு retries;
	முழு retain;
	மாறிலி எழுத்து *topic;
	uint8_t qos;
	uint32_t payloadlen;
	மாறிலி வெறுமை *payload;
	uint32_t expiry_interval;
	uint32_t subscription_id;

	base_செய்தி = client_செய்தி->base_செய்தி;

	expiry_interval = 0;
	எனில்(base_செய்தி->data.expiry_time){
		எனில்(db.now_real_s > base_செய்தி->data.expiry_time){
			/* Message is expired, must not send. */
			எனில்(client_செய்தி->data.direction == mosq_md_out && client_செய்தி->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_செய்தி);
			db__fill_inflight_out_from_queue(context);
			திரும்பு MOSQ_ERR_SUCCESS;
		}இல்லையெனில்{
			expiry_interval = (uint32_t)(base_செய்தி->data.expiry_time - db.now_real_s);
		}
	}
	mid = client_செய்தி->data.mid;
	retries = client_செய்தி->data.dup;
	retain = client_செய்தி->data.retain;
	topic = base_செய்தி->data.topic;
	qos = (uint8_t)client_செய்தி->data.qos;
	payloadlen = base_செய்தி->data.payloadlen;
	payload = base_செய்தி->data.payload;
	subscription_id = client_செய்தி->data.subscription_identifier;
	base_செய்தி_props = base_செய்தி->data.properties;

	தேர்வு(client_செய்தி->data.state){
		நிலை mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_செய்தி_props, expiry_interval);
			எனில்(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_செய்தி);
			}இல்லையெனில்{
				திரும்பு rc;
			}
			நிறுத்து;

		நிலை mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_செய்தி_props, expiry_interval);
			எனில்(rc == MOSQ_ERR_SUCCESS){
				client_செய்தி->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_செய்தி->data.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_செய்தி_update(context, client_செய்தி);
			}இல்லையெனில் எனில்(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_செய்தி);
			}இல்லையெனில்{
				திரும்பு rc;
			}
			நிறுத்து;

		நிலை mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_செய்தி_props, expiry_interval);
			எனில்(rc == MOSQ_ERR_SUCCESS){
				client_செய்தி->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_செய்தி->data.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_செய்தி_update(context, client_செய்தி);
			}இல்லையெனில் எனில்(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_செய்தி);
			}இல்லையெனில்{
				திரும்பு rc;
			}
			நிறுத்து;

		நிலை mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, வெற்று);
			எனில்(!rc){
				client_செய்தி->data.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_செய்தி_update(context, client_செய்தி);
			}இல்லையெனில்{
				திரும்பு rc;
			}
			நிறுத்து;

		நிலை mosq_ms_invalid:
		நிலை mosq_ms_send_pubrec:
		நிலை mosq_ms_resend_pubcomp:
		நிலை mosq_ms_wait_for_puback:
		நிலை mosq_ms_wait_for_pubrec:
		நிலை mosq_ms_wait_for_pubrel:
		நிலை mosq_ms_wait_for_pubcomp:
		நிலை mosq_ms_queued:
			நிறுத்து;
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு db__message_write_inflight_out_all(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி, *tmp;
	முழு rc;

	எனில்(context->state != mosq_cs_active || !net__is_connected(context)){
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_செய்தி, tmp){
		rc = db__message_write_inflight_out_single(context, client_செய்தி);
		எனில்(rc){
			திரும்பு rc;
		}
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு db__message_write_inflight_out_latest(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி, *next;
	முழு rc;

	எனில்(context->state != mosq_cs_active
			|| !net__is_connected(context)
			|| context->msgs_out.inflight == வெற்று){

		திரும்பு MOSQ_ERR_SUCCESS;
	}

	எனில்(context->msgs_out.inflight->prev == context->msgs_out.inflight){
		/* Only one message */
		திரும்பு db__message_write_inflight_out_single(context, context->msgs_out.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_செய்தி = context->msgs_out.inflight->prev;
	போது(client_செய்தி != context->msgs_out.inflight &&
			(client_செய்தி->data.state == mosq_ms_publish_qos0
			|| client_செய்தி->data.state == mosq_ms_publish_qos1
			|| client_செய்தி->data.state == mosq_ms_publish_qos2)){

		client_செய்தி = client_செய்தி->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	எனில்(client_செய்தி != context->msgs_out.inflight){
		client_செய்தி = client_செய்தி->next;
	}

	போது(client_செய்தி){
		next = client_செய்தி->next;
		rc = db__message_write_inflight_out_single(context, client_செய்தி);
		எனில்(rc){
			திரும்பு rc;
		}
		client_செய்தி = next;
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு db__message_write_queued_in(கட்டமைப்பு mosquitto *context)
{
	கட்டமைப்பு mosquitto_client_செய்தி *client_செய்தி, *tmp;
	முழு rc;

	எனில்(context->state != mosq_cs_active){
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_செய்தி, tmp){
		எனில்(context->msgs_in.inflight_maximum != 0 && context->msgs_in.inflight_quota == 0){
			நிறுத்து;
		}

		எனில்(client_செய்தி->data.qos == 2){
			client_செய்தி->data.state = mosq_ms_send_pubrec;
			db__message_dequeue_first(context, &context->msgs_in);
			rc = send__pubrec(context, client_செய்தி->data.mid, 0, வெற்று);
			எனில்(!rc){
				client_செய்தி->data.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_செய்தி_update(context, client_செய்தி);
			}இல்லையெனில்{
				plugin_persist_handle_client_செய்தி_update(context, client_செய்தி);
				திரும்பு rc;
			}
		}
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு db__message_write_queued_out(கட்டமைப்பு mosquitto *context)
{
	எனில்(context->state != mosq_cs_active){
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	db__fill_inflight_out_from_queue(context);

	திரும்பு MOSQ_ERR_SUCCESS;
}
