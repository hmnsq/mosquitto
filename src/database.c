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

#অন্তর্ভাৱ "config.h"

#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <utlist.h>

#অন্তর্ভাৱ "mosquitto_broker_internal.h"
#অন্তর্ভাৱ "send_mosq.h"
#অন্তর্ভাৱ "sys_tree.h"
#অন্তর্ভাৱ "util_mosq.h"


/**
 * Is this context ready to take more in flight messages right now?
 * @param context the client context of interest
 * @param qos qos for the packet of interest
 * @return true if more in flight are allowed.
 */
bool db__ready_for_flight(সংরচনা mosquitto *context, গণনা mosquitto_বাৰ্তা_direction dir, পূর্ণাঙ্ক qos)
{
	সংরচনা mosquitto_বাৰ্তা_data *msgs;
	bool valid_bytes;
	bool valid_গুনতি;

	যদি(dir == mosq_md_out){
		msgs = &context->msgs_out;
	}অন্যথা{
		msgs = &context->msgs_in;
	}

	যদি(msgs->inflight_maximum == 0 && db.config->max_inflight_bytes == 0){
		ঘূৰাই true;
	}

	যদি(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		যদি(db.config->max_queued_messages == 0 && db.config->max_inflight_bytes == 0){
			ঘূৰাই true;
		}
		valid_bytes = ((msgs->inflight_bytes - (ssize_t)db.config->max_inflight_bytes) < (ssize_t)db.config->max_queued_bytes);
		যদি(dir == mosq_md_out){
			valid_গুনতি = context->out_packet_গুনতি < db.config->max_queued_messages;
		}অন্যথা{
			valid_গুনতি = msgs->inflight_গুনতি - msgs->inflight_maximum < db.config->max_queued_messages;
		}

		যদি(db.config->max_queued_messages == 0){
			ঘূৰাই valid_bytes;
		}
		যদি(db.config->max_queued_bytes == 0){
			ঘূৰাই valid_গুনতি;
		}
	}অন্যথা{
		valid_bytes = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_bytes;
		valid_গুনতি = msgs->inflight_quota > 0;

		যদি(msgs->inflight_maximum == 0){
			ঘূৰাই valid_bytes;
		}
		যদি(db.config->max_inflight_bytes == 0){
			ঘূৰাই valid_গুনতি;
		}
	}

	ঘূৰাই valid_bytes && valid_গুনতি;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db__ready_for_queue(সংরচনা mosquitto *context, পূর্ণাঙ্ক qos, সংরচনা mosquitto_বাৰ্তা_data *বাৰ্তা_data)
{
	পূর্ণাঙ্ক source_গুনতি;
	পূর্ণাঙ্ক adjust_গুনতি;
	long source_bytes;
	ssize_t adjust_bytes = (ssize_t)db.config->max_inflight_bytes;
	bool valid_bytes;
	bool valid_গুনতি;

	যদি(db.config->max_queued_messages == 0 && db.config->max_queued_bytes == 0){
		ঘূৰাই true;
	}

	যদি(qos == 0 && db.config->queue_qos0_messages == false){
		ঘূৰাই false; /* This case is handled in db__ready_for_flight() */
	}অন্যথা{
		source_bytes = (ssize_t)বাৰ্তা_data->queued_bytes12;
		source_গুনতি = বাৰ্তা_data->queued_count12;
	}
	adjust_গুনতি = বাৰ্তা_data->inflight_maximum;

	/* nothing in flight for offline clients */
	যদি(!net__is_connected(context)){
		adjust_bytes = 0;
		adjust_গুনতি = 0;
	}

	valid_bytes = (source_bytes - (ssize_t)adjust_bytes) < (ssize_t)db.config->max_queued_bytes;
	valid_গুনতি = source_গুনতি - adjust_গুনতি < db.config->max_queued_messages;

	যদি(db.config->max_queued_bytes == 0){
		ঘূৰাই valid_গুনতি;
	}
	যদি(db.config->max_queued_messages == 0){
		ঘূৰাই valid_bytes;
	}

	ঘূৰাই valid_bytes && valid_গুনতি;
}


শূন্য db_বাৰ্তা_add_to_inflight_stats(সংরচনা mosquitto_বাৰ্তা_data *বাৰ্তা_data, সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা)
{
	বাৰ্তা_data->inflight_গুনতি++;
	বাৰ্তা_data->inflight_bytes += client_বাৰ্তা->base_বাৰ্তা->data.payloadlen;
	যদি(client_বাৰ্তা->data.qos != 0){
		বাৰ্তা_data->inflight_count12++;
		বাৰ্তা_data->inflight_bytes12 += client_বাৰ্তা->base_বাৰ্তা->data.payloadlen;
	}
}


স্থির শূন্য db_বাৰ্তা_remove_from_inflight_stats(সংরচনা mosquitto_বাৰ্তা_data *বাৰ্তা_data, সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা)
{
	বাৰ্তা_data->inflight_গুনতি--;
	বাৰ্তা_data->inflight_bytes -= client_বাৰ্তা->base_বাৰ্তা->data.payloadlen;
	যদি(client_বাৰ্তা->data.qos != 0){
		বাৰ্তা_data->inflight_count12--;
		বাৰ্তা_data->inflight_bytes12 -= client_বাৰ্তা->base_বাৰ্তা->data.payloadlen;
	}
}


শূন্য db_বাৰ্তা_add_to_queued_stats(সংরচনা mosquitto_বাৰ্তা_data *বাৰ্তা_data, সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা)
{
	বাৰ্তা_data->queued_গুনতি++;
	বাৰ্তা_data->queued_bytes += client_বাৰ্তা->base_বাৰ্তা->data.payloadlen;
	যদি(client_বাৰ্তা->data.qos != 0){
		বাৰ্তা_data->queued_count12++;
		বাৰ্তা_data->queued_bytes12 += client_বাৰ্তা->base_বাৰ্তা->data.payloadlen;
	}
}


স্থির শূন্য db_বাৰ্তা_remove_from_queued_stats(সংরচনা mosquitto_বাৰ্তা_data *বাৰ্তা_data, সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা)
{
	বাৰ্তা_data->queued_গুনতি--;
	বাৰ্তা_data->queued_bytes -= client_বাৰ্তা->base_বাৰ্তা->data.payloadlen;
	যদি(client_বাৰ্তা->data.qos != 0){
		বাৰ্তা_data->queued_count12--;
		বাৰ্তা_data->queued_bytes12 -= client_বাৰ্তা->base_বাৰ্তা->data.payloadlen;
	}
}


পূর্ণাঙ্ক db__open(সংরচনা mosquitto__config *config)
{
	যদি(!config){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = রিক্ত;
	db.contexts_by_sock = রিক্ত;
	db.contexts_for_free = রিক্ত;
#ifdef WITH_BRIDGE
	db.bridges = রিক্ত;
	db.bridge_গুনতি = 0;
#endif

	/* Initialize the hashtable */
	db.clientid_index_hash = রিক্ত;

	db.normal_subs = রিক্ত;
	db.shared_subs = রিক্ত;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = রিক্ত;

#ifdef WITH_PERSISTENCE
	যদি(persist__restore()){
		ঘূৰাই 1;
	}
#endif

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


স্থির শূন্য subhier_clean(সংরচনা mosquitto__subhier **subhier)
{
	সংরচনা mosquitto__subhier *peer, *subhier_tmp;
	সংরচনা mosquitto__subleaf *leaf, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		leaf = peer->subs;
		যেতিয়ালৈ(leaf){
			nextleaf = leaf->next;
			mosquitto_FREE(leaf);
			leaf = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


পূর্ণাঙ্ক db__close(শূন্য)
{
	subhier_clean(&db.normal_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db_বাৰ্তা_store_clean();

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক db_বাৰ্তা_store_add(সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা)
{
	সংরচনা mosquitto_base_বাৰ্তা *found;
	unsigned hashv;

	HASH_মান(&base_বাৰ্তা->data.store_id, পরিমাণ(base_বাৰ্তা->data.store_id), hashv);
	HASH_FIND_BYHASHVALUE(hh, db.বাৰ্তা_store, &base_বাৰ্তা->data.store_id, পরিমাণ(base_বাৰ্তা->data.store_id), hashv, found);
	যদি(found == রিক্ত){
		HASH_ADD_KEYPTR_BYHASHVALUE(hh, db.বাৰ্তা_store, &base_বাৰ্তা->data.store_id, পরিমাণ(base_বাৰ্তা->data.store_id), hashv, base_বাৰ্তা);
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}অন্যথা{
		ঘূৰাই MOSQ_ERR_ALREADY_EXISTS;
	}
}


শূন্য db_বাৰ্তা_store_free(সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা)
{
	mosquitto_FREE(base_বাৰ্তা->data.source_id);
	mosquitto_FREE(base_বাৰ্তা->data.source_username);
	যদি(base_বাৰ্তা->dest_ids){
		বাবে(পূর্ণাঙ্ক i=0; i<base_বাৰ্তা->dest_id_গুনতি; i++){
			mosquitto_FREE(base_বাৰ্তা->dest_ids[i]);
		}
		mosquitto_FREE(base_বাৰ্তা->dest_ids);
	}
	mosquitto_FREE(base_বাৰ্তা->data.topic);
	mosquitto_property_free_all(&base_বাৰ্তা->data.properties);
	mosquitto_FREE(base_বাৰ্তা->data.payload);
	mosquitto_FREE(base_বাৰ্তা);
}


শূন্য db_বাৰ্তা_store_remove(সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা, bool notify)
{
	যদি(base_বাৰ্তা == রিক্ত){
		ঘূৰাই;
	}
	HASH_DELETE(hh, db.বাৰ্তা_store, base_বাৰ্তা);
	db.বাৰ্তা_store_গুনতি--;
	db.বাৰ্তা_store_bytes -= base_বাৰ্তা->data.payloadlen;
	যদি(notify == true){
		plugin_persist_handle_base_বাৰ্তা_delete(base_বাৰ্তা);
	}
	db_বাৰ্তা_store_free(base_বাৰ্তা);
}


শূন্য db_বাৰ্তা_store_clean(শূন্য)
{
	সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা, *base_বাৰ্তা_tmp;

	HASH_ITER(hh, db.বাৰ্তা_store, base_বাৰ্তা, base_বাৰ্তা_tmp){
		db_বাৰ্তা_store_remove(base_বাৰ্তা, false);
	}
}


শূন্য db_বাৰ্তা_store_ref_inc(সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা)
{
	base_বাৰ্তা->ref_গুনতি++;
}


শূন্য db_বাৰ্তা_store_ref_dec(সংরচনা mosquitto_base_বাৰ্তা **base_বাৰ্তা)
{
	(*base_বাৰ্তা)->ref_গুনতি--;
	যদি((*base_বাৰ্তা)->ref_গুনতি == 0){
		db_বাৰ্তা_store_remove(*base_বাৰ্তা, true);
		*base_বাৰ্তা = রিক্ত;
	}
}


শূন্য db_বাৰ্তা_store_compact(শূন্য)
{
	সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা, *base_বাৰ্তা_tmp;

	HASH_ITER(hh, db.বাৰ্তা_store, base_বাৰ্তা, base_বাৰ্তা_tmp){
		যদি(base_বাৰ্তা->ref_গুনতি < 1){
			db_বাৰ্তা_store_remove(base_বাৰ্তা, true);
		}
	}
}


স্থির শূন্য db__message_remove_inflight(সংরচনা mosquitto *context, সংরচনা mosquitto_বাৰ্তা_data *বাৰ্তা_data, সংরচনা mosquitto_client_বাৰ্তা *item)
{
	যদি(!context || !বাৰ্তা_data || !item){
		ঘূৰাই;
	}

	plugin_persist_handle_client_বাৰ্তা_delete(context, item);

	DL_DELETE(বাৰ্তা_data->inflight, item);
	যদি(item->base_বাৰ্তা){
		db_বাৰ্তা_remove_from_inflight_stats(বাৰ্তা_data, item);
		db_বাৰ্তা_store_ref_dec(&item->base_বাৰ্তা);
	}

	mosquitto_FREE(item);
}


স্থির শূন্য db__message_remove_queued(সংরচনা mosquitto *context, সংরচনা mosquitto_বাৰ্তা_data *বাৰ্তা_data, সংরচনা mosquitto_client_বাৰ্তা *item)
{
	যদি(!context || !বাৰ্তা_data || !item){
		ঘূৰাই;
	}

	plugin_persist_handle_client_বাৰ্তা_delete(context, item);

	DL_DELETE(বাৰ্তা_data->queued, item);
	যদি(item->base_বাৰ্তা){
		db_বাৰ্তা_remove_from_queued_stats(বাৰ্তা_data, item);
		db_বাৰ্তা_store_ref_dec(&item->base_বাৰ্তা);
	}

	mosquitto_FREE(item);
}


স্থির শূন্য db__fill_inflight_out_from_queue(সংরচনা mosquitto *context)
{
	সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.queued, client_বাৰ্তা, tmp){
		যদি(!db__ready_for_flight(context, mosq_md_out, client_বাৰ্তা->data.qos)){
			ঘূৰাই;
		}
		ৱিকল্প(client_বাৰ্তা->data.qos){
			পক্ষ 0:
				client_বাৰ্তা->data.state = mosq_ms_publish_qos0;
				ভঙ্গ;
			পক্ষ 1:
				client_বাৰ্তা->data.state = mosq_ms_publish_qos1;
				ভঙ্গ;
			পক্ষ 2:
				client_বাৰ্তা->data.state = mosq_ms_publish_qos2;
				ভঙ্গ;
		}
		যদি(client_বাৰ্তা->base_বাৰ্তা->data.expiry_time && db.now_real_s > client_বাৰ্তা->base_বাৰ্তা->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_বাৰ্তা);
			অনুৱর্ত;
		}
		plugin_persist_handle_client_বাৰ্তা_update(context, client_বাৰ্তা);
		db__message_dequeue_first(context, &context->msgs_out);
	}
}


শূন্য db__message_dequeue_first(সংরচনা mosquitto *context, সংরচনা mosquitto_বাৰ্তা_data *বাৰ্তা_data)
{
	সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা;

	UNUSED(context);

	client_বাৰ্তা = বাৰ্তা_data->queued;
	DL_DELETE(বাৰ্তা_data->queued, client_বাৰ্তা);
	DL_APPEND(বাৰ্তা_data->inflight, client_বাৰ্তা);
	যদি(বাৰ্তা_data->inflight_quota > 0){
		বাৰ্তা_data->inflight_quota--;
	}

	db_বাৰ্তা_remove_from_queued_stats(বাৰ্তা_data, client_বাৰ্তা);
	db_বাৰ্তা_add_to_inflight_stats(বাৰ্তা_data, client_বাৰ্তা);
}


পূর্ণাঙ্ক db__message_delete_outgoing(সংরচনা mosquitto *context, uint16_t mid, গণনা mosquitto_বাৰ্তা_state expect_state, পূর্ণাঙ্ক qos)
{
	সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা, *tmp;
	bool deleted = false;

	যদি(!context){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_বাৰ্তা, tmp){
		যদি(client_বাৰ্তা->data.mid == mid){
			যদি(client_বাৰ্তা->data.qos != qos){
				log__printf(রিক্ত, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_বাৰ্তা->data.qos, qos);
				ঘূৰাই MOSQ_ERR_PROTOCOL;
			}অন্যথা যদি(qos == 2 && client_বাৰ্তা->data.state != expect_state && expect_state != mosq_ms_any){
				log__printf(রিক্ত, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_বাৰ্তা->data.state, expect_state);
				ঘূৰাই MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_out, client_বাৰ্তা);
			deleted = true;
			ভঙ্গ;
		}
	}

	যদি(deleted == false){
		DL_FOREACH_SAFE(context->msgs_out.queued, client_বাৰ্তা, tmp){
			যদি(client_বাৰ্তা->data.mid == mid){
				যদি(client_বাৰ্তা->data.qos != qos){
					ঘূৰাই MOSQ_ERR_PROTOCOL;
				}অন্যথা যদি(qos == 2 && client_বাৰ্তা->data.state != expect_state && expect_state != mosq_ms_any){
					ঘূৰাই MOSQ_ERR_PROTOCOL;
				}
				db__message_remove_queued(context, &context->msgs_out, client_বাৰ্তা);
				ভঙ্গ;
			}
		}
	}
	db__fill_inflight_out_from_queue(context);
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	ঘূৰাই db__message_write_inflight_out_latest(context);
}


/* Only for QoS 2 messages */
পূর্ণাঙ্ক db_message_ঢোকাও_incoming(সংরচনা mosquitto *context, uint64_t cmsg_id, সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা, bool persist)
{
	সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা;
	সংরচনা mosquitto_বাৰ্তা_data *বাৰ্তা_data;
	গণনা mosquitto_বাৰ্তা_state state = mosq_ms_invalid;
	পূর্ণাঙ্ক rc = 0;

	প্রতিজ্ঞা(base_বাৰ্তা);
	যদি(!context){
		ঘূৰাই MOSQ_ERR_INVAL;
	}
	যদি(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		ঘূৰাই MOSQ_ERR_SUCCESS;

	}
	বাৰ্তা_data = &context->msgs_in;

	যদি(db__ready_for_flight(context, mosq_md_in, base_বাৰ্তা->data.qos)){
		state = mosq_ms_wait_for_pubrel;
	}অন্যথা যদি(base_বাৰ্তা->data.qos != 0 && db__ready_for_queue(context, base_বাৰ্তা->data.qos, বাৰ্তা_data)){
		state = mosq_ms_queued;
		rc = 2;
	}অন্যথা{
		/* Dropping message due to full queue. */
		যদি(context->is_dropping == false){
			context->is_dropping = true;
			log__printf(রিক্ত, MOSQ_LOG_NOTICE,
					"Outgoing messages are being dropped for client %s.",
					context->id);
		}
		metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		ঘূৰাই 2;
	}

	প্রতিজ্ঞা(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	যদি(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_বাৰ্তা = mosquitto_malloc(পরিমাণ(সংরচনা mosquitto_client_বাৰ্তা));
	যদি(!client_বাৰ্তা){
		ঘূৰাই MOSQ_ERR_NOMEM;
	}
	client_বাৰ্তা->prev = রিক্ত;
	client_বাৰ্তা->next = রিক্ত;
	যদি(cmsg_id){
		client_বাৰ্তা->data.cmsg_id = cmsg_id;
	}অন্যথা{
		client_বাৰ্তা->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_বাৰ্তা->base_বাৰ্তা = base_বাৰ্তা;
	db_বাৰ্তা_store_ref_inc(client_বাৰ্তা->base_বাৰ্তা);
	client_বাৰ্তা->data.mid = base_বাৰ্তা->data.source_mid;
	client_বাৰ্তা->data.direction = mosq_md_in;
	client_বাৰ্তা->data.state = (uint8_t)state;
	client_বাৰ্তা->data.dup = false;
	যদি(base_বাৰ্তা->data.qos > context->max_qos){
		client_বাৰ্তা->data.qos = context->max_qos;
	}অন্যথা{
		client_বাৰ্তা->data.qos = base_বাৰ্তা->data.qos;
	}
	client_বাৰ্তা->data.retain = base_বাৰ্তা->data.retain;
	client_বাৰ্তা->data.subscription_identifier = 0;

	যদি(state == mosq_ms_queued){
		DL_APPEND(বাৰ্তা_data->queued, client_বাৰ্তা);
		db_বাৰ্তা_add_to_queued_stats(বাৰ্তা_data, client_বাৰ্তা);
	}অন্যথা{
		DL_APPEND(বাৰ্তা_data->inflight, client_বাৰ্তা);
		db_বাৰ্তা_add_to_inflight_stats(বাৰ্তা_data, client_বাৰ্তা);
	}

	যদি(persist && context->is_persisted){
		plugin_persist_handle_base_বাৰ্তা_add(client_বাৰ্তা->base_বাৰ্তা);
		plugin_persist_handle_client_বাৰ্তা_add(context, client_বাৰ্তা);
	}

	যদি(client_বাৰ্তা->base_বাৰ্তা->data.qos > 0){
		util__decrement_receive_quota(context);
	}
	ঘূৰাই rc;
}


পূর্ণাঙ্ক db_message_ঢোকাও_outgoing(সংরচনা mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা, uint32_t subscription_identifier, bool update, bool persist)
{
	সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা;
	সংরচনা mosquitto_বাৰ্তা_data *বাৰ্তা_data;
	গণনা mosquitto_বাৰ্তা_state state = mosq_ms_invalid;
	পূর্ণাঙ্ক rc = 0;
	ৱর্ণ **dest_ids;

	প্রতিজ্ঞা(base_বাৰ্তা);
	যদি(!context){
		ঘূৰাই MOSQ_ERR_INVAL;
	}
	যদি(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		ঘূৰাই MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_sent++;

	বাৰ্তা_data = &context->msgs_out;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	যদি(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && base_বাৰ্তা->dest_ids){

		বাবে(পূর্ণাঙ্ক i=0; i<base_বাৰ্তা->dest_id_গুনতি; i++){
			যদি(base_বাৰ্তা->dest_ids[i] && !strcmp(base_বাৰ্তা->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				ঘূৰাই MOSQ_ERR_SUCCESS;
			}
		}
	}
	যদি(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		যদি(qos == 0 && !db.config->queue_qos0_messages){
			যদি(!context->bridge){
				ঘূৰাই 2;
			}অন্যথা{
				যদি(context->bridge->start_type != bst_lazy){
					ঘূৰাই 2;
				}
			}
		}
		যদি(context->bridge && context->bridge->clean_start_local == true){
			ঘূৰাই 2;
		}
	}

	যদি(net__is_connected(context)){
		যদি(db__ready_for_flight(context, mosq_md_out, qos)){
			ৱিকল্প(qos){
				পক্ষ 0:
					state = mosq_ms_publish_qos0;
					ভঙ্গ;
				পক্ষ 1:
					state = mosq_ms_publish_qos1;
					ভঙ্গ;
				পক্ষ 2:
					state = mosq_ms_publish_qos2;
					ভঙ্গ;
			}
		}অন্যথা যদি(qos != 0 && db__ready_for_queue(context, qos, বাৰ্তা_data)){
			state = mosq_ms_queued;
			rc = 2;
		}অন্যথা{
			/* Dropping message due to full queue. */
			যদি(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(রিক্ত, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			ঘূৰাই 2;
		}
	}অন্যথা{
		যদি(db__ready_for_queue(context, qos, বাৰ্তা_data)){
			state = mosq_ms_queued;
		}অন্যথা{
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			যদি(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(রিক্ত, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			ঘূৰাই 2;
		}
	}
	প্রতিজ্ঞা(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	যদি(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_বাৰ্তা = mosquitto_malloc(পরিমাণ(সংরচনা mosquitto_client_বাৰ্তা));
	যদি(!client_বাৰ্তা){
		ঘূৰাই MOSQ_ERR_NOMEM;
	}
	client_বাৰ্তা->prev = রিক্ত;
	client_বাৰ্তা->next = রিক্ত;
	যদি(cmsg_id){
		client_বাৰ্তা->data.cmsg_id = cmsg_id;
	}অন্যথা{
		client_বাৰ্তা->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_বাৰ্তা->base_বাৰ্তা = base_বাৰ্তা;
	db_বাৰ্তা_store_ref_inc(client_বাৰ্তা->base_বাৰ্তা);
	client_বাৰ্তা->data.mid = mid;
	client_বাৰ্তা->data.direction = mosq_md_out;
	client_বাৰ্তা->data.state = (uint8_t)state;
	client_বাৰ্তা->data.dup = false;
	যদি(qos > context->max_qos){
		client_বাৰ্তা->data.qos = context->max_qos;
	}অন্যথা{
		client_বাৰ্তা->data.qos = qos;
	}
	client_বাৰ্তা->data.retain = retain;
	client_বাৰ্তা->data.subscription_identifier = subscription_identifier;

	যদি(state == mosq_ms_queued){
		DL_APPEND(বাৰ্তা_data->queued, client_বাৰ্তা);
		db_বাৰ্তা_add_to_queued_stats(বাৰ্তা_data, client_বাৰ্তা);
	}অন্যথা{
		DL_APPEND(বাৰ্তা_data->inflight, client_বাৰ্তা);
		db_বাৰ্তা_add_to_inflight_stats(বাৰ্তা_data, client_বাৰ্তা);
	}

	যদি(persist && context->is_persisted){
		plugin_persist_handle_base_বাৰ্তা_add(client_বাৰ্তা->base_বাৰ্তা);
		plugin_persist_handle_client_বাৰ্তা_add(context, client_বাৰ্তা);
	}

	যদি(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(base_বাৰ্তা->dest_ids, পরিমাণ(ৱর্ণ *)*(size_t)(base_বাৰ্তা->dest_id_গুনতি+1));
		যদি(dest_ids){
			base_বাৰ্তা->dest_ids = dest_ids;
			base_বাৰ্তা->dest_id_গুনতি++;
			base_বাৰ্তা->dest_ids[base_বাৰ্তা->dest_id_গুনতি-1] = mosquitto_strdup(context->id);
			যদি(!base_বাৰ্তা->dest_ids[base_বাৰ্তা->dest_id_গুনতি-1]){
				ঘূৰাই MOSQ_ERR_NOMEM;
			}
		}অন্যথা{
			ঘূৰাই MOSQ_ERR_NOMEM;
		}
	}
#ifdef WITH_BRIDGE
	যদি(context->bridge && context->bridge->start_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_out.inflight_গুনতি + context->msgs_out.queued_গুনতি >= context->bridge->threshold){

		context->bridge->lazy_reconnect = true;
	}
#endif

	যদি(client_বাৰ্তা->data.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	যদি(update){
		rc = db__message_write_inflight_out_latest(context);
		যদি(rc){
			ঘূৰাই rc;
		}
		rc = db__message_write_queued_out(context);
		যদি(rc){
			ঘূৰাই rc;
		}
	}

	ঘূৰাই rc;
}


স্থির অন্তঃস্থ পূর্ণাঙ্ক db__message_update_outgoing_state(সংরচনা mosquitto *context, সংরচনা mosquitto_client_বাৰ্তা *head,
		uint16_t mid, গণনা mosquitto_বাৰ্তা_state state, পূর্ণাঙ্ক qos, bool persist)
{
	সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা;

	DL_FOREACH(head, client_বাৰ্তা){
		যদি(client_বাৰ্তা->data.mid == mid){
			যদি(client_বাৰ্তা->data.qos != qos){
				log__printf(রিক্ত, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_বাৰ্তা->data.qos, qos);
				ঘূৰাই MOSQ_ERR_PROTOCOL;
			}
			client_বাৰ্তা->data.state = (uint8_t)state;
			যদি(persist){
				plugin_persist_handle_client_বাৰ্তা_update(context, client_বাৰ্তা);
			}
			ঘূৰাই MOSQ_ERR_SUCCESS;
		}
	}
	ঘূৰাই MOSQ_ERR_NOT_FOUND;
}


পূর্ণাঙ্ক db__message_update_outgoing(সংরচনা mosquitto *context, uint16_t mid, গণনা mosquitto_বাৰ্তা_state state, পূর্ণাঙ্ক qos, bool persist)
{
	পূর্ণাঙ্ক rc;

	rc = db__message_update_outgoing_state(context, context->msgs_out.inflight, mid, state, qos, persist);
	যদি(!persist && rc == MOSQ_ERR_NOT_FOUND){
		rc = db__message_update_outgoing_state(context, context->msgs_out.queued, mid, state, qos, persist);
	}
	ঘূৰাই rc;
}


স্থির শূন্য db__messages_delete_list(সংরচনা mosquitto_client_বাৰ্তা **head)
{
	সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা, *tmp;

	DL_FOREACH_SAFE(*head, client_বাৰ্তা, tmp){
		DL_DELETE(*head, client_বাৰ্তা);
		db_বাৰ্তা_store_ref_dec(&client_বাৰ্তা->base_বাৰ্তা);
		mosquitto_FREE(client_বাৰ্তা);
	}
	*head = রিক্ত;
}


পূর্ণাঙ্ক db__messages_delete_incoming(সংরচনা mosquitto *context)
{
	যদি(!context){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_in.inflight);
	db__messages_delete_list(&context->msgs_in.queued);
	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_গুনতি = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_গুনতি = 0;
	context->msgs_in.queued_count12 = 0;

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক db__messages_delete_outgoing(সংরচনা mosquitto *context)
{
	যদি(!context){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_out.inflight);
	db__messages_delete_list(&context->msgs_out.queued);
	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_গুনতি = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_গুনতি = 0;
	context->msgs_out.queued_count12 = 0;

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক db__messages_delete(সংরচনা mosquitto *context, bool force_free)
{
	যদি(!context){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	যদি(force_free || context->clean_start || (context->bridge && context->bridge->clean_start)){
		db__messages_delete_incoming(context);
	}

	যদি(force_free || (context->bridge && context->bridge->clean_start_local)
			|| (context->bridge == রিক্ত && context->clean_start)){

		db__messages_delete_outgoing(context);
	}

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক db__messages_easy_queue(সংরচনা mosquitto *context, ধ্রুৱ ৱর্ণ *topic, uint8_t qos, uint32_t payloadlen, ধ্রুৱ শূন্য *payload, পূর্ণাঙ্ক retain, uint32_t message_expiry_interval, mosquitto_property **properties)
{
	সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা;
	ধ্রুৱ ৱর্ণ *source_id;
	গণনা mosquitto_বাৰ্তা_origin origin;

	যদি(!topic){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	base_বাৰ্তা = mosquitto_calloc(1, পরিমাণ(সংরচনা mosquitto_base_বাৰ্তা));
	যদি(base_বাৰ্তা == রিক্ত){
		ঘূৰাই MOSQ_ERR_NOMEM;
	}

	base_বাৰ্তা->data.topic = mosquitto_strdup(topic);
	যদি(base_বাৰ্তা->data.topic == রিক্ত){
		db_বাৰ্তা_store_free(base_বাৰ্তা);
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	base_বাৰ্তা->data.qos = qos;
	যদি(db.config->retain_available == false){
		base_বাৰ্তা->data.retain = 0;
	}অন্যথা{
		base_বাৰ্তা->data.retain = retain;
	}

	base_বাৰ্তা->data.payloadlen = payloadlen;
	যদি(payloadlen > 0){
		base_বাৰ্তা->data.payload = mosquitto_malloc(base_বাৰ্তা->data.payloadlen+1);
		যদি(base_বাৰ্তা->data.payload == রিক্ত){
			db_বাৰ্তা_store_free(base_বাৰ্তা);
			ঘূৰাই MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_বাৰ্তা->data.payload)[base_বাৰ্তা->data.payloadlen] = 0;
		memcpy(base_বাৰ্তা->data.payload, payload, base_বাৰ্তা->data.payloadlen);
	}

	যদি(context && context->id){
		source_id = context->id;
	}অন্যথা{
		source_id = "";
	}
	যদি(properties){
		base_বাৰ্তা->data.properties = *properties;
		*properties = রিক্ত;
	}

	যদি(context){
		origin = mosq_mo_client;
	}অন্যথা{
		origin = mosq_mo_broker;
	}
	যদি(db__message_store(context, base_বাৰ্তা, &message_expiry_interval, origin)){
		ঘূৰাই 1;
	}

	ঘূৰাই sub__messages_queue(source_id, base_বাৰ্তা->data.qos, &base_বাৰ্তা);
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
uint64_t db_new_বাৰ্তা_id(শূন্য)
{
#ifdef WIN32
	FILETIME ftime;
	uint64_t ftime64;
#অন্যথা
	সংরচনা timespec ts;
#endif
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	long nsec;

	id = db.গ্ৰন্থি_id_shifted; /* Top 10-bits */

#ifdef WIN32
	GetSystemTimePreciseAsFileTime(&ftime);
	ftime64 = (((uint64_t)ftime.dw_উপৰেৰ_Date_Time)<<32) + ftime.dw_নিচেৰ_Date_Time;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (long)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#অন্যথা
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#endif
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	যদি(id <= db.last_db_id){
		id = db.last_db_id + 1;
	}
	db.last_db_id = id;

	ঘূৰাই id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
পূর্ণাঙ্ক db__message_store(ধ্রুৱ সংরচনা mosquitto *source, সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা, uint32_t *message_expiry_interval, গণনা mosquitto_বাৰ্তা_origin origin)
{
	পূর্ণাঙ্ক rc;

	প্রতিজ্ঞা(base_বাৰ্তা);

	যদি(source && source->id){
		base_বাৰ্তা->data.source_id = mosquitto_strdup(source->id);
	}অন্যথা{
		base_বাৰ্তা->data.source_id = mosquitto_strdup("");
	}
	যদি(!base_বাৰ্তা->data.source_id){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_বাৰ্তা_store_free(base_বাৰ্তা);
		ঘূৰাই MOSQ_ERR_NOMEM;
	}

	যদি(source && source->username){
		base_বাৰ্তা->data.source_username = mosquitto_strdup(source->username);
		যদি(!base_বাৰ্তা->data.source_username){
			db_বাৰ্তা_store_free(base_বাৰ্তা);
			ঘূৰাই MOSQ_ERR_NOMEM;
		}
	}
	যদি(source){
		base_বাৰ্তা->source_listener = source->listener;
	}
	base_বাৰ্তা->origin = origin;
	যদি(message_expiry_interval){
		যদি(*message_expiry_interval > 0 && *message_expiry_interval != বাৰ্তা_EXPIRY_INFINITE){
			base_বাৰ্তা->data.expiry_time = db.now_real_s + *message_expiry_interval;
		}অন্যথা{
			base_বাৰ্তা->data.expiry_time = 0;
		}
	}

	base_বাৰ্তা->dest_ids = রিক্ত;
	base_বাৰ্তা->dest_id_গুনতি = 0;
	db.বাৰ্তা_store_গুনতি++;
	db.বাৰ্তা_store_bytes += base_বাৰ্তা->data.payloadlen;

	যদি(!base_বাৰ্তা->data.store_id){
		base_বাৰ্তা->data.store_id = db_new_বাৰ্তা_id();
	}

	rc = db_বাৰ্তা_store_add(base_বাৰ্তা);
	যদি(rc){
		db_বাৰ্তা_store_free(base_বাৰ্তা);
		ঘূৰাই rc;
	}

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক db__message_store_find(সংরচনা mosquitto *context, uint16_t mid, সংরচনা mosquitto_client_বাৰ্তা **client_বাৰ্তা)
{
	সংরচনা mosquitto_client_বাৰ্তা *cmsg;

	*client_বাৰ্তা = রিক্ত;

	যদি(!context){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_in.inflight, cmsg){
		যদি(cmsg->base_বাৰ্তা->data.source_mid == mid){
			*client_বাৰ্তা = cmsg;
			ঘূৰাই MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_in.queued, cmsg){
		যদি(cmsg->base_বাৰ্তা->data.source_mid == mid){
			*client_বাৰ্তা = cmsg;
			ঘূৰাই MOSQ_ERR_SUCCESS;
		}
	}

	ঘূৰাই 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
স্থির পূর্ণাঙ্ক db__message_reconnect_reset_outgoing(সংরচনা mosquitto *context)
{
	সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা, *tmp;

	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_গুনতি = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_গুনতি = 0;
	context->msgs_out.queued_count12 = 0;
	context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_বাৰ্তা, tmp){
		db_বাৰ্তা_add_to_inflight_stats(&context->msgs_out, client_বাৰ্তা);
		যদি(client_বাৰ্তা->data.qos > 0){
			util__decrement_send_quota(context);
		}

		ৱিকল্প(client_বাৰ্তা->data.qos){
			পক্ষ 0:
				client_বাৰ্তা->data.state = mosq_ms_publish_qos0;
				ভঙ্গ;
			পক্ষ 1:
				client_বাৰ্তা->data.state = mosq_ms_publish_qos1;
				ভঙ্গ;
			পক্ষ 2:
				যদি(client_বাৰ্তা->data.state == mosq_ms_wait_for_pubcomp){
					client_বাৰ্তা->data.state = mosq_ms_resend_pubrel;
				}অন্যথা{
					client_বাৰ্তা->data.state = mosq_ms_publish_qos2;
				}
				ভঙ্গ;
		}
		plugin_persist_handle_client_বাৰ্তা_update(context, client_বাৰ্তা);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_out.queued, client_বাৰ্তা, tmp){
		db_বাৰ্তা_add_to_queued_stats(&context->msgs_out, client_বাৰ্তা);
	}
	db__fill_inflight_out_from_queue(context);

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
স্থির পূর্ণাঙ্ক db__message_reconnect_reset_incoming(সংরচনা mosquitto *context)
{
	সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা, *tmp;

	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_গুনতি = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_গুনতি = 0;
	context->msgs_in.queued_count12 = 0;
	context->msgs_in.inflight_quota = context->msgs_in.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_বাৰ্তা, tmp){
		db_বাৰ্তা_add_to_inflight_stats(&context->msgs_in, client_বাৰ্তা);
		যদি(client_বাৰ্তা->data.qos > 0){
			util__decrement_receive_quota(context);
		}

		যদি(client_বাৰ্তা->data.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db__message_remove_inflight(context, &context->msgs_in, client_বাৰ্তা);
		}অন্যথা{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_বাৰ্তা->data.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_in.queued, client_বাৰ্তা, tmp){
		client_বাৰ্তা->data.dup = 0;
		db_বাৰ্তা_add_to_queued_stats(&context->msgs_in, client_বাৰ্তা);
		যদি(db__ready_for_flight(context, mosq_md_in, client_বাৰ্তা->data.qos)){
			ৱিকল্প(client_বাৰ্তা->data.qos){
				পক্ষ 0:
					client_বাৰ্তা->data.state = mosq_ms_publish_qos0;
					ভঙ্গ;
				পক্ষ 1:
					client_বাৰ্তা->data.state = mosq_ms_publish_qos1;
					ভঙ্গ;
				পক্ষ 2:
					client_বাৰ্তা->data.state = mosq_ms_publish_qos2;
					ভঙ্গ;
			}
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_বাৰ্তা_update(context, client_বাৰ্তা);
		}
	}

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক db__message_reconnect_reset(সংরচনা mosquitto *context)
{
	পূর্ণাঙ্ক rc;

	rc = db__message_reconnect_reset_outgoing(context);
	যদি(rc){
		ঘূৰাই rc;
	}
	ঘূৰাই db__message_reconnect_reset_incoming(context);
}


পূর্ণাঙ্ক db__message_remove_incoming(সংরচনা mosquitto *context, uint16_t mid)
{
	সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা, *tmp;

	যদি(!context){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_বাৰ্তা, tmp){
		যদি(client_বাৰ্তা->data.mid == mid){
			যদি(client_বাৰ্তা->base_বাৰ্তা->data.qos != 2){
				log__printf(রিক্ত, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_বাৰ্তা->base_বাৰ্তা->data.qos);
				ঘূৰাই MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_in, client_বাৰ্তা);
			ঘূৰাই MOSQ_ERR_SUCCESS;
		}
	}

	ঘূৰাই MOSQ_ERR_NOT_FOUND;
}


পূর্ণাঙ্ক db__message_release_incoming(সংরচনা mosquitto *context, uint16_t mid)
{
	সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা, *tmp;
	bool deleted = false;
	পূর্ণাঙ্ক rc;

	যদি(!context){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_বাৰ্তা, tmp){
		যদি(client_বাৰ্তা->data.mid == mid){
			যদি(client_বাৰ্তা->base_বাৰ্তা->data.qos != 2){
				log__printf(রিক্ত, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_বাৰ্তা->base_বাৰ্তা->data.qos);
				ঘূৰাই MOSQ_ERR_PROTOCOL;
			}
			ধ্রুৱ ৱর্ণ *topic = client_বাৰ্তা->base_বাৰ্তা->data.topic;
			ধ্রুৱ ৱর্ণ *source_id = client_বাৰ্তা->base_বাৰ্তা->data.source_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			যদি(topic == রিক্ত){
				db__message_remove_inflight(context, &context->msgs_in, client_বাৰ্তা);
				deleted = true;
			}অন্যথা{
				rc = sub__messages_queue(source_id, 2, &client_বাৰ্তা->base_বাৰ্তা);
				যদি(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db__message_remove_inflight(context, &context->msgs_in, client_বাৰ্তা);
					deleted = true;
				}অন্যথা{
					ঘূৰাই 1;
				}
			}
		}
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_বাৰ্তা, tmp){
		যদি(db__ready_for_flight(context, mosq_md_in, client_বাৰ্তা->data.qos)){
			ভঙ্গ;
		}

		যদি(client_বাৰ্তা->data.qos == 2){
			send__pubrec(context, client_বাৰ্তা->data.mid, 0, রিক্ত);
			client_বাৰ্তা->data.state = mosq_ms_wait_for_pubrel;
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_বাৰ্তা_update(context, client_বাৰ্তা);
		}
	}
	যদি(deleted){
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}অন্যথা{
		ঘূৰাই MOSQ_ERR_NOT_FOUND;
	}
}


শূন্য db__expire_all_messages(সংরচনা mosquitto *context)
{
	সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_বাৰ্তা, tmp){
		যদি(client_বাৰ্তা->base_বাৰ্তা->data.expiry_time && db.now_real_s > client_বাৰ্তা->base_বাৰ্তা->data.expiry_time){
			যদি(client_বাৰ্তা->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_বাৰ্তা);
		}
	}
	db__fill_inflight_out_from_queue(context);
	DL_FOREACH_SAFE(context->msgs_out.queued, client_বাৰ্তা, tmp){
		যদি(client_বাৰ্তা->base_বাৰ্তা->data.expiry_time && db.now_real_s > client_বাৰ্তা->base_বাৰ্তা->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_বাৰ্তা);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.inflight, client_বাৰ্তা, tmp){
		যদি(client_বাৰ্তা->base_বাৰ্তা->data.expiry_time && db.now_real_s > client_বাৰ্তা->base_বাৰ্তা->data.expiry_time){
			যদি(client_বাৰ্তা->data.qos > 0){
				util__increment_receive_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_in, client_বাৰ্তা);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.queued, client_বাৰ্তা, tmp){
		যদি(client_বাৰ্তা->base_বাৰ্তা->data.expiry_time && db.now_real_s > client_বাৰ্তা->base_বাৰ্তা->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_in, client_বাৰ্তা);
		}
	}
}


স্থির শূন্য db__client_messages_check_acl(সংরচনা mosquitto *context, সংরচনা mosquitto_বাৰ্তা_data *বাৰ্তা_data, সংরচনা mosquitto_client_বাৰ্তা **head,
		শূন্য (*decrement_stats_fn)(সংরচনা mosquitto_বাৰ্তা_data *বাৰ্তা_data, সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা))
{
	সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা, *tmp;
	সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা;
	পূর্ণাঙ্ক access;

	DL_FOREACH_SAFE((*head), client_বাৰ্তা, tmp){
		base_বাৰ্তা = client_বাৰ্তা->base_বাৰ্তা;
		যদি(client_বাৰ্তা->data.direction == mosq_md_out){
			access = MOSQ_ACL_READ;
		}অন্যথা{
			access = MOSQ_ACL_WRITE;
		}
		যদি(mosquitto_acl_check(context, base_বাৰ্তা->data.topic,
				base_বাৰ্তা->data.payloadlen, base_বাৰ্তা->data.payload,
				base_বাৰ্তা->data.qos, base_বাৰ্তা->data.retain,
				base_বাৰ্তা->data.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*head), client_বাৰ্তা);
			decrement_stats_fn(বাৰ্তা_data, client_বাৰ্তা);
			plugin_persist_handle_client_বাৰ্তা_delete(context, client_বাৰ্তা);
			db_বাৰ্তা_store_ref_dec(&client_বাৰ্তা->base_বাৰ্তা);
			mosquitto_FREE(client_বাৰ্তা);
		}
	}
}


শূন্য db__check_acl_of_all_messages(সংরচনা mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.inflight, &db_বাৰ্তা_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.queued, &db_বাৰ্তা_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.inflight, &db_বাৰ্তা_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.queued, &db_বাৰ্তা_remove_from_queued_stats);
}


স্থির পূর্ণাঙ্ক db__message_write_inflight_out_single(সংরচনা mosquitto *context, সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা)
{
	সংরচনা mosquitto_base_বাৰ্তা *base_বাৰ্তা;
	mosquitto_property *base_বাৰ্তা_props = রিক্ত;
	পূর্ণাঙ্ক rc;
	uint16_t mid;
	পূর্ণাঙ্ক retries;
	পূর্ণাঙ্ক retain;
	ধ্রুৱ ৱর্ণ *topic;
	uint8_t qos;
	uint32_t payloadlen;
	ধ্রুৱ শূন্য *payload;
	uint32_t expiry_interval;
	uint32_t subscription_id;

	base_বাৰ্তা = client_বাৰ্তা->base_বাৰ্তা;

	expiry_interval = 0;
	যদি(base_বাৰ্তা->data.expiry_time){
		যদি(db.now_real_s > base_বাৰ্তা->data.expiry_time){
			/* Message is expired, must not send. */
			যদি(client_বাৰ্তা->data.direction == mosq_md_out && client_বাৰ্তা->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_বাৰ্তা);
			db__fill_inflight_out_from_queue(context);
			ঘূৰাই MOSQ_ERR_SUCCESS;
		}অন্যথা{
			expiry_interval = (uint32_t)(base_বাৰ্তা->data.expiry_time - db.now_real_s);
		}
	}
	mid = client_বাৰ্তা->data.mid;
	retries = client_বাৰ্তা->data.dup;
	retain = client_বাৰ্তা->data.retain;
	topic = base_বাৰ্তা->data.topic;
	qos = (uint8_t)client_বাৰ্তা->data.qos;
	payloadlen = base_বাৰ্তা->data.payloadlen;
	payload = base_বাৰ্তা->data.payload;
	subscription_id = client_বাৰ্তা->data.subscription_identifier;
	base_বাৰ্তা_props = base_বাৰ্তা->data.properties;

	ৱিকল্প(client_বাৰ্তা->data.state){
		পক্ষ mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_বাৰ্তা_props, expiry_interval);
			যদি(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_বাৰ্তা);
			}অন্যথা{
				ঘূৰাই rc;
			}
			ভঙ্গ;

		পক্ষ mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_বাৰ্তা_props, expiry_interval);
			যদি(rc == MOSQ_ERR_SUCCESS){
				client_বাৰ্তা->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_বাৰ্তা->data.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_বাৰ্তা_update(context, client_বাৰ্তা);
			}অন্যথা যদি(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_বাৰ্তা);
			}অন্যথা{
				ঘূৰাই rc;
			}
			ভঙ্গ;

		পক্ষ mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_বাৰ্তা_props, expiry_interval);
			যদি(rc == MOSQ_ERR_SUCCESS){
				client_বাৰ্তা->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_বাৰ্তা->data.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_বাৰ্তা_update(context, client_বাৰ্তা);
			}অন্যথা যদি(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_বাৰ্তা);
			}অন্যথা{
				ঘূৰাই rc;
			}
			ভঙ্গ;

		পক্ষ mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, রিক্ত);
			যদি(!rc){
				client_বাৰ্তা->data.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_বাৰ্তা_update(context, client_বাৰ্তা);
			}অন্যথা{
				ঘূৰাই rc;
			}
			ভঙ্গ;

		পক্ষ mosq_ms_invalid:
		পক্ষ mosq_ms_send_pubrec:
		পক্ষ mosq_ms_resend_pubcomp:
		পক্ষ mosq_ms_wait_for_puback:
		পক্ষ mosq_ms_wait_for_pubrec:
		পক্ষ mosq_ms_wait_for_pubrel:
		পক্ষ mosq_ms_wait_for_pubcomp:
		পক্ষ mosq_ms_queued:
			ভঙ্গ;
	}
	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক db__message_write_inflight_out_all(সংরচনা mosquitto *context)
{
	সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা, *tmp;
	পূর্ণাঙ্ক rc;

	যদি(context->state != mosq_cs_active || !net__is_connected(context)){
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_বাৰ্তা, tmp){
		rc = db__message_write_inflight_out_single(context, client_বাৰ্তা);
		যদি(rc){
			ঘূৰাই rc;
		}
	}
	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক db__message_write_inflight_out_latest(সংরচনা mosquitto *context)
{
	সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা, *next;
	পূর্ণাঙ্ক rc;

	যদি(context->state != mosq_cs_active
			|| !net__is_connected(context)
			|| context->msgs_out.inflight == রিক্ত){

		ঘূৰাই MOSQ_ERR_SUCCESS;
	}

	যদি(context->msgs_out.inflight->prev == context->msgs_out.inflight){
		/* Only one message */
		ঘূৰাই db__message_write_inflight_out_single(context, context->msgs_out.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_বাৰ্তা = context->msgs_out.inflight->prev;
	যেতিয়ালৈ(client_বাৰ্তা != context->msgs_out.inflight &&
			(client_বাৰ্তা->data.state == mosq_ms_publish_qos0
			|| client_বাৰ্তা->data.state == mosq_ms_publish_qos1
			|| client_বাৰ্তা->data.state == mosq_ms_publish_qos2)){

		client_বাৰ্তা = client_বাৰ্তা->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	যদি(client_বাৰ্তা != context->msgs_out.inflight){
		client_বাৰ্তা = client_বাৰ্তা->next;
	}

	যেতিয়ালৈ(client_বাৰ্তা){
		next = client_বাৰ্তা->next;
		rc = db__message_write_inflight_out_single(context, client_বাৰ্তা);
		যদি(rc){
			ঘূৰাই rc;
		}
		client_বাৰ্তা = next;
	}
	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক db__message_write_queued_in(সংরচনা mosquitto *context)
{
	সংরচনা mosquitto_client_বাৰ্তা *client_বাৰ্তা, *tmp;
	পূর্ণাঙ্ক rc;

	যদি(context->state != mosq_cs_active){
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_বাৰ্তা, tmp){
		যদি(context->msgs_in.inflight_maximum != 0 && context->msgs_in.inflight_quota == 0){
			ভঙ্গ;
		}

		যদি(client_বাৰ্তা->data.qos == 2){
			client_বাৰ্তা->data.state = mosq_ms_send_pubrec;
			db__message_dequeue_first(context, &context->msgs_in);
			rc = send__pubrec(context, client_বাৰ্তা->data.mid, 0, রিক্ত);
			যদি(!rc){
				client_বাৰ্তা->data.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_বাৰ্তা_update(context, client_বাৰ্তা);
			}অন্যথা{
				plugin_persist_handle_client_বাৰ্তা_update(context, client_বাৰ্তা);
				ঘূৰাই rc;
			}
		}
	}
	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক db__message_write_queued_out(সংরচনা mosquitto *context)
{
	যদি(context->state != mosq_cs_active){
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}

	db__fill_inflight_out_from_queue(context);

	ঘূৰাই MOSQ_ERR_SUCCESS;
}
