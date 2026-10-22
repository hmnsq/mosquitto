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

#شامل <assert.h>
#شامل <stdio.h>
#شامل <utlist.h>

#شامل "mosquitto_broker_internal.h"
#شامل "send_mosq.h"
#شامل "sys_tree.h"
#شامل "util_mosq.h"


/**
 * Is this context ready to take more in flight messages right now?
 * @param context the client context of interest
 * @param qos qos for the packet of interest
 * @return true if more in flight are allowed.
 */
bool db__ready_for_flight(ساختار mosquitto *context, شمارش mosquitto_پیام_direction dir, صحیح qos)
{
	ساختار mosquitto_پیام_data *msgs;
	bool valid_bytes;
	bool valid_شمار;

	اگر(dir == mosq_md_out){
		msgs = &context->msgs_out;
	}وگرنه{
		msgs = &context->msgs_in;
	}

	اگر(msgs->inflight_maximum == 0 && db.config->max_inflight_bytes == 0){
		بازگشت true;
	}

	اگر(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		اگر(db.config->max_queued_messages == 0 && db.config->max_inflight_bytes == 0){
			بازگشت true;
		}
		valid_bytes = ((msgs->inflight_bytes - (ssize_t)db.config->max_inflight_bytes) < (ssize_t)db.config->max_queued_bytes);
		اگر(dir == mosq_md_out){
			valid_شمار = context->out_packet_شمار < db.config->max_queued_messages;
		}وگرنه{
			valid_شمار = msgs->inflight_شمار - msgs->inflight_maximum < db.config->max_queued_messages;
		}

		اگر(db.config->max_queued_messages == 0){
			بازگشت valid_bytes;
		}
		اگر(db.config->max_queued_bytes == 0){
			بازگشت valid_شمار;
		}
	}وگرنه{
		valid_bytes = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_bytes;
		valid_شمار = msgs->inflight_quota > 0;

		اگر(msgs->inflight_maximum == 0){
			بازگشت valid_bytes;
		}
		اگر(db.config->max_inflight_bytes == 0){
			بازگشت valid_شمار;
		}
	}

	بازگشت valid_bytes && valid_شمار;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db__ready_for_queue(ساختار mosquitto *context, صحیح qos, ساختار mosquitto_پیام_data *پیام_data)
{
	صحیح source_شمار;
	صحیح adjust_شمار;
	long source_bytes;
	ssize_t adjust_bytes = (ssize_t)db.config->max_inflight_bytes;
	bool valid_bytes;
	bool valid_شمار;

	اگر(db.config->max_queued_messages == 0 && db.config->max_queued_bytes == 0){
		بازگشت true;
	}

	اگر(qos == 0 && db.config->queue_qos0_messages == false){
		بازگشت false; /* This case is handled in db__ready_for_flight() */
	}وگرنه{
		source_bytes = (ssize_t)پیام_data->queued_bytes12;
		source_شمار = پیام_data->queued_count12;
	}
	adjust_شمار = پیام_data->inflight_maximum;

	/* nothing in flight for offline clients */
	اگر(!net__is_connected(context)){
		adjust_bytes = 0;
		adjust_شمار = 0;
	}

	valid_bytes = (source_bytes - (ssize_t)adjust_bytes) < (ssize_t)db.config->max_queued_bytes;
	valid_شمار = source_شمار - adjust_شمار < db.config->max_queued_messages;

	اگر(db.config->max_queued_bytes == 0){
		بازگشت valid_شمار;
	}
	اگر(db.config->max_queued_messages == 0){
		بازگشت valid_bytes;
	}

	بازگشت valid_bytes && valid_شمار;
}


تهی db_پیام_add_to_inflight_stats(ساختار mosquitto_پیام_data *پیام_data, ساختار mosquitto_client_پیام *client_پیام)
{
	پیام_data->inflight_شمار++;
	پیام_data->inflight_bytes += client_پیام->base_پیام->data.payloadlen;
	اگر(client_پیام->data.qos != 0){
		پیام_data->inflight_count12++;
		پیام_data->inflight_bytes12 += client_پیام->base_پیام->data.payloadlen;
	}
}


ایستا تهی db_پیام_remove_from_inflight_stats(ساختار mosquitto_پیام_data *پیام_data, ساختار mosquitto_client_پیام *client_پیام)
{
	پیام_data->inflight_شمار--;
	پیام_data->inflight_bytes -= client_پیام->base_پیام->data.payloadlen;
	اگر(client_پیام->data.qos != 0){
		پیام_data->inflight_count12--;
		پیام_data->inflight_bytes12 -= client_پیام->base_پیام->data.payloadlen;
	}
}


تهی db_پیام_add_to_queued_stats(ساختار mosquitto_پیام_data *پیام_data, ساختار mosquitto_client_پیام *client_پیام)
{
	پیام_data->queued_شمار++;
	پیام_data->queued_bytes += client_پیام->base_پیام->data.payloadlen;
	اگر(client_پیام->data.qos != 0){
		پیام_data->queued_count12++;
		پیام_data->queued_bytes12 += client_پیام->base_پیام->data.payloadlen;
	}
}


ایستا تهی db_پیام_remove_from_queued_stats(ساختار mosquitto_پیام_data *پیام_data, ساختار mosquitto_client_پیام *client_پیام)
{
	پیام_data->queued_شمار--;
	پیام_data->queued_bytes -= client_پیام->base_پیام->data.payloadlen;
	اگر(client_پیام->data.qos != 0){
		پیام_data->queued_count12--;
		پیام_data->queued_bytes12 -= client_پیام->base_پیام->data.payloadlen;
	}
}


صحیح db__open(ساختار mosquitto__config *config)
{
	اگر(!config){
		بازگشت MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = پوچ;
	db.contexts_by_sock = پوچ;
	db.contexts_for_free = پوچ;
#ifdef WITH_BRIDGE
	db.bridges = پوچ;
	db.bridge_شمار = 0;
#endif

	/* Initialize the hashtable */
	db.clientid_index_hash = پوچ;

	db.normal_subs = پوچ;
	db.shared_subs = پوچ;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = پوچ;

#ifdef WITH_PERSISTENCE
	اگر(persist__restore()){
		بازگشت 1;
	}
#endif

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا تهی subhier_clean(ساختار mosquitto__subhier **subhier)
{
	ساختار mosquitto__subhier *peer, *subhier_tmp;
	ساختار mosquitto__subleaf *leaf, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		leaf = peer->subs;
		وقتی(leaf){
			nextleaf = leaf->next;
			mosquitto_FREE(leaf);
			leaf = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


صحیح db__close(تهی)
{
	subhier_clean(&db.normal_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db_پیام_store_clean();

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح db_پیام_store_add(ساختار mosquitto_base_پیام *base_پیام)
{
	ساختار mosquitto_base_پیام *found;
	unsigned hashv;

	HASH_مقدار(&base_پیام->data.store_id, اندازه(base_پیام->data.store_id), hashv);
	HASH_FIND_BYHASHVALUE(hh, db.پیام_store, &base_پیام->data.store_id, اندازه(base_پیام->data.store_id), hashv, found);
	اگر(found == پوچ){
		HASH_ADD_KEYPTR_BYHASHVALUE(hh, db.پیام_store, &base_پیام->data.store_id, اندازه(base_پیام->data.store_id), hashv, base_پیام);
		بازگشت MOSQ_ERR_SUCCESS;
	}وگرنه{
		بازگشت MOSQ_ERR_ALREADY_EXISTS;
	}
}


تهی db_پیام_store_free(ساختار mosquitto_base_پیام *base_پیام)
{
	mosquitto_FREE(base_پیام->data.source_id);
	mosquitto_FREE(base_پیام->data.source_username);
	اگر(base_پیام->dest_ids){
		برای(صحیح i=0; i<base_پیام->dest_id_شمار; i++){
			mosquitto_FREE(base_پیام->dest_ids[i]);
		}
		mosquitto_FREE(base_پیام->dest_ids);
	}
	mosquitto_FREE(base_پیام->data.topic);
	mosquitto_property_free_all(&base_پیام->data.properties);
	mosquitto_FREE(base_پیام->data.payload);
	mosquitto_FREE(base_پیام);
}


تهی db_پیام_store_remove(ساختار mosquitto_base_پیام *base_پیام, bool notify)
{
	اگر(base_پیام == پوچ){
		بازگشت;
	}
	HASH_DELETE(hh, db.پیام_store, base_پیام);
	db.پیام_store_شمار--;
	db.پیام_store_bytes -= base_پیام->data.payloadlen;
	اگر(notify == true){
		plugin_persist_handle_base_پیام_delete(base_پیام);
	}
	db_پیام_store_free(base_پیام);
}


تهی db_پیام_store_clean(تهی)
{
	ساختار mosquitto_base_پیام *base_پیام, *base_پیام_tmp;

	HASH_ITER(hh, db.پیام_store, base_پیام, base_پیام_tmp){
		db_پیام_store_remove(base_پیام, false);
	}
}


تهی db_پیام_store_ref_inc(ساختار mosquitto_base_پیام *base_پیام)
{
	base_پیام->ref_شمار++;
}


تهی db_پیام_store_ref_dec(ساختار mosquitto_base_پیام **base_پیام)
{
	(*base_پیام)->ref_شمار--;
	اگر((*base_پیام)->ref_شمار == 0){
		db_پیام_store_remove(*base_پیام, true);
		*base_پیام = پوچ;
	}
}


تهی db_پیام_store_compact(تهی)
{
	ساختار mosquitto_base_پیام *base_پیام, *base_پیام_tmp;

	HASH_ITER(hh, db.پیام_store, base_پیام, base_پیام_tmp){
		اگر(base_پیام->ref_شمار < 1){
			db_پیام_store_remove(base_پیام, true);
		}
	}
}


ایستا تهی db__message_remove_inflight(ساختار mosquitto *context, ساختار mosquitto_پیام_data *پیام_data, ساختار mosquitto_client_پیام *item)
{
	اگر(!context || !پیام_data || !item){
		بازگشت;
	}

	plugin_persist_handle_client_پیام_delete(context, item);

	DL_DELETE(پیام_data->inflight, item);
	اگر(item->base_پیام){
		db_پیام_remove_from_inflight_stats(پیام_data, item);
		db_پیام_store_ref_dec(&item->base_پیام);
	}

	mosquitto_FREE(item);
}


ایستا تهی db__message_remove_queued(ساختار mosquitto *context, ساختار mosquitto_پیام_data *پیام_data, ساختار mosquitto_client_پیام *item)
{
	اگر(!context || !پیام_data || !item){
		بازگشت;
	}

	plugin_persist_handle_client_پیام_delete(context, item);

	DL_DELETE(پیام_data->queued, item);
	اگر(item->base_پیام){
		db_پیام_remove_from_queued_stats(پیام_data, item);
		db_پیام_store_ref_dec(&item->base_پیام);
	}

	mosquitto_FREE(item);
}


ایستا تهی db__fill_inflight_out_from_queue(ساختار mosquitto *context)
{
	ساختار mosquitto_client_پیام *client_پیام, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.queued, client_پیام, tmp){
		اگر(!db__ready_for_flight(context, mosq_md_out, client_پیام->data.qos)){
			بازگشت;
		}
		انتخاب(client_پیام->data.qos){
			حالت 0:
				client_پیام->data.state = mosq_ms_publish_qos0;
				بشکن;
			حالت 1:
				client_پیام->data.state = mosq_ms_publish_qos1;
				بشکن;
			حالت 2:
				client_پیام->data.state = mosq_ms_publish_qos2;
				بشکن;
		}
		اگر(client_پیام->base_پیام->data.expiry_time && db.now_real_s > client_پیام->base_پیام->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_پیام);
			ادامه;
		}
		plugin_persist_handle_client_پیام_update(context, client_پیام);
		db__message_dequeue_first(context, &context->msgs_out);
	}
}


تهی db__message_dequeue_first(ساختار mosquitto *context, ساختار mosquitto_پیام_data *پیام_data)
{
	ساختار mosquitto_client_پیام *client_پیام;

	UNUSED(context);

	client_پیام = پیام_data->queued;
	DL_DELETE(پیام_data->queued, client_پیام);
	DL_APPEND(پیام_data->inflight, client_پیام);
	اگر(پیام_data->inflight_quota > 0){
		پیام_data->inflight_quota--;
	}

	db_پیام_remove_from_queued_stats(پیام_data, client_پیام);
	db_پیام_add_to_inflight_stats(پیام_data, client_پیام);
}


صحیح db__message_delete_outgoing(ساختار mosquitto *context, uint16_t mid, شمارش mosquitto_پیام_state expect_state, صحیح qos)
{
	ساختار mosquitto_client_پیام *client_پیام, *tmp;
	bool deleted = false;

	اگر(!context){
		بازگشت MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_پیام, tmp){
		اگر(client_پیام->data.mid == mid){
			اگر(client_پیام->data.qos != qos){
				log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_پیام->data.qos, qos);
				بازگشت MOSQ_ERR_PROTOCOL;
			}وگرنه اگر(qos == 2 && client_پیام->data.state != expect_state && expect_state != mosq_ms_any){
				log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_پیام->data.state, expect_state);
				بازگشت MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_out, client_پیام);
			deleted = true;
			بشکن;
		}
	}

	اگر(deleted == false){
		DL_FOREACH_SAFE(context->msgs_out.queued, client_پیام, tmp){
			اگر(client_پیام->data.mid == mid){
				اگر(client_پیام->data.qos != qos){
					بازگشت MOSQ_ERR_PROTOCOL;
				}وگرنه اگر(qos == 2 && client_پیام->data.state != expect_state && expect_state != mosq_ms_any){
					بازگشت MOSQ_ERR_PROTOCOL;
				}
				db__message_remove_queued(context, &context->msgs_out, client_پیام);
				بشکن;
			}
		}
	}
	db__fill_inflight_out_from_queue(context);
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	بازگشت db__message_write_inflight_out_latest(context);
}


/* Only for QoS 2 messages */
صحیح db_message_درج_incoming(ساختار mosquitto *context, uint64_t cmsg_id, ساختار mosquitto_base_پیام *base_پیام, bool persist)
{
	ساختار mosquitto_client_پیام *client_پیام;
	ساختار mosquitto_پیام_data *پیام_data;
	شمارش mosquitto_پیام_state state = mosq_ms_invalid;
	صحیح rc = 0;

	أكد(base_پیام);
	اگر(!context){
		بازگشت MOSQ_ERR_INVAL;
	}
	اگر(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		بازگشت MOSQ_ERR_SUCCESS;

	}
	پیام_data = &context->msgs_in;

	اگر(db__ready_for_flight(context, mosq_md_in, base_پیام->data.qos)){
		state = mosq_ms_wait_for_pubrel;
	}وگرنه اگر(base_پیام->data.qos != 0 && db__ready_for_queue(context, base_پیام->data.qos, پیام_data)){
		state = mosq_ms_queued;
		rc = 2;
	}وگرنه{
		/* Dropping message due to full queue. */
		اگر(context->is_dropping == false){
			context->is_dropping = true;
			log__printf(پوچ, MOSQ_LOG_NOTICE,
					"Outgoing messages are being dropped for client %s.",
					context->id);
		}
		metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		بازگشت 2;
	}

	أكد(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	اگر(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_پیام = mosquitto_malloc(اندازه(ساختار mosquitto_client_پیام));
	اگر(!client_پیام){
		بازگشت MOSQ_ERR_NOMEM;
	}
	client_پیام->prev = پوچ;
	client_پیام->next = پوچ;
	اگر(cmsg_id){
		client_پیام->data.cmsg_id = cmsg_id;
	}وگرنه{
		client_پیام->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_پیام->base_پیام = base_پیام;
	db_پیام_store_ref_inc(client_پیام->base_پیام);
	client_پیام->data.mid = base_پیام->data.source_mid;
	client_پیام->data.direction = mosq_md_in;
	client_پیام->data.state = (uint8_t)state;
	client_پیام->data.dup = false;
	اگر(base_پیام->data.qos > context->max_qos){
		client_پیام->data.qos = context->max_qos;
	}وگرنه{
		client_پیام->data.qos = base_پیام->data.qos;
	}
	client_پیام->data.retain = base_پیام->data.retain;
	client_پیام->data.subscription_identifier = 0;

	اگر(state == mosq_ms_queued){
		DL_APPEND(پیام_data->queued, client_پیام);
		db_پیام_add_to_queued_stats(پیام_data, client_پیام);
	}وگرنه{
		DL_APPEND(پیام_data->inflight, client_پیام);
		db_پیام_add_to_inflight_stats(پیام_data, client_پیام);
	}

	اگر(persist && context->is_persisted){
		plugin_persist_handle_base_پیام_add(client_پیام->base_پیام);
		plugin_persist_handle_client_پیام_add(context, client_پیام);
	}

	اگر(client_پیام->base_پیام->data.qos > 0){
		util__decrement_receive_quota(context);
	}
	بازگشت rc;
}


صحیح db_message_درج_outgoing(ساختار mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, ساختار mosquitto_base_پیام *base_پیام, uint32_t subscription_identifier, bool update, bool persist)
{
	ساختار mosquitto_client_پیام *client_پیام;
	ساختار mosquitto_پیام_data *پیام_data;
	شمارش mosquitto_پیام_state state = mosq_ms_invalid;
	صحیح rc = 0;
	نویسه **dest_ids;

	أكد(base_پیام);
	اگر(!context){
		بازگشت MOSQ_ERR_INVAL;
	}
	اگر(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		بازگشت MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_sent++;

	پیام_data = &context->msgs_out;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	اگر(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && base_پیام->dest_ids){

		برای(صحیح i=0; i<base_پیام->dest_id_شمار; i++){
			اگر(base_پیام->dest_ids[i] && !strcmp(base_پیام->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				بازگشت MOSQ_ERR_SUCCESS;
			}
		}
	}
	اگر(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		اگر(qos == 0 && !db.config->queue_qos0_messages){
			اگر(!context->bridge){
				بازگشت 2;
			}وگرنه{
				اگر(context->bridge->start_type != bst_lazy){
					بازگشت 2;
				}
			}
		}
		اگر(context->bridge && context->bridge->clean_start_local == true){
			بازگشت 2;
		}
	}

	اگر(net__is_connected(context)){
		اگر(db__ready_for_flight(context, mosq_md_out, qos)){
			انتخاب(qos){
				حالت 0:
					state = mosq_ms_publish_qos0;
					بشکن;
				حالت 1:
					state = mosq_ms_publish_qos1;
					بشکن;
				حالت 2:
					state = mosq_ms_publish_qos2;
					بشکن;
			}
		}وگرنه اگر(qos != 0 && db__ready_for_queue(context, qos, پیام_data)){
			state = mosq_ms_queued;
			rc = 2;
		}وگرنه{
			/* Dropping message due to full queue. */
			اگر(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(پوچ, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			بازگشت 2;
		}
	}وگرنه{
		اگر(db__ready_for_queue(context, qos, پیام_data)){
			state = mosq_ms_queued;
		}وگرنه{
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			اگر(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(پوچ, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			بازگشت 2;
		}
	}
	أكد(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	اگر(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_پیام = mosquitto_malloc(اندازه(ساختار mosquitto_client_پیام));
	اگر(!client_پیام){
		بازگشت MOSQ_ERR_NOMEM;
	}
	client_پیام->prev = پوچ;
	client_پیام->next = پوچ;
	اگر(cmsg_id){
		client_پیام->data.cmsg_id = cmsg_id;
	}وگرنه{
		client_پیام->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_پیام->base_پیام = base_پیام;
	db_پیام_store_ref_inc(client_پیام->base_پیام);
	client_پیام->data.mid = mid;
	client_پیام->data.direction = mosq_md_out;
	client_پیام->data.state = (uint8_t)state;
	client_پیام->data.dup = false;
	اگر(qos > context->max_qos){
		client_پیام->data.qos = context->max_qos;
	}وگرنه{
		client_پیام->data.qos = qos;
	}
	client_پیام->data.retain = retain;
	client_پیام->data.subscription_identifier = subscription_identifier;

	اگر(state == mosq_ms_queued){
		DL_APPEND(پیام_data->queued, client_پیام);
		db_پیام_add_to_queued_stats(پیام_data, client_پیام);
	}وگرنه{
		DL_APPEND(پیام_data->inflight, client_پیام);
		db_پیام_add_to_inflight_stats(پیام_data, client_پیام);
	}

	اگر(persist && context->is_persisted){
		plugin_persist_handle_base_پیام_add(client_پیام->base_پیام);
		plugin_persist_handle_client_پیام_add(context, client_پیام);
	}

	اگر(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(base_پیام->dest_ids, اندازه(نویسه *)*(size_t)(base_پیام->dest_id_شمار+1));
		اگر(dest_ids){
			base_پیام->dest_ids = dest_ids;
			base_پیام->dest_id_شمار++;
			base_پیام->dest_ids[base_پیام->dest_id_شمار-1] = mosquitto_strdup(context->id);
			اگر(!base_پیام->dest_ids[base_پیام->dest_id_شمار-1]){
				بازگشت MOSQ_ERR_NOMEM;
			}
		}وگرنه{
			بازگشت MOSQ_ERR_NOMEM;
		}
	}
#ifdef WITH_BRIDGE
	اگر(context->bridge && context->bridge->start_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_out.inflight_شمار + context->msgs_out.queued_شمار >= context->bridge->threshold){

		context->bridge->lazy_reconnect = true;
	}
#endif

	اگر(client_پیام->data.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	اگر(update){
		rc = db__message_write_inflight_out_latest(context);
		اگر(rc){
			بازگشت rc;
		}
		rc = db__message_write_queued_out(context);
		اگر(rc){
			بازگشت rc;
		}
	}

	بازگشت rc;
}


ایستا درونخطی صحیح db__message_update_outgoing_state(ساختار mosquitto *context, ساختار mosquitto_client_پیام *head,
		uint16_t mid, شمارش mosquitto_پیام_state state, صحیح qos, bool persist)
{
	ساختار mosquitto_client_پیام *client_پیام;

	DL_FOREACH(head, client_پیام){
		اگر(client_پیام->data.mid == mid){
			اگر(client_پیام->data.qos != qos){
				log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_پیام->data.qos, qos);
				بازگشت MOSQ_ERR_PROTOCOL;
			}
			client_پیام->data.state = (uint8_t)state;
			اگر(persist){
				plugin_persist_handle_client_پیام_update(context, client_پیام);
			}
			بازگشت MOSQ_ERR_SUCCESS;
		}
	}
	بازگشت MOSQ_ERR_NOT_FOUND;
}


صحیح db__message_update_outgoing(ساختار mosquitto *context, uint16_t mid, شمارش mosquitto_پیام_state state, صحیح qos, bool persist)
{
	صحیح rc;

	rc = db__message_update_outgoing_state(context, context->msgs_out.inflight, mid, state, qos, persist);
	اگر(!persist && rc == MOSQ_ERR_NOT_FOUND){
		rc = db__message_update_outgoing_state(context, context->msgs_out.queued, mid, state, qos, persist);
	}
	بازگشت rc;
}


ایستا تهی db__messages_delete_list(ساختار mosquitto_client_پیام **head)
{
	ساختار mosquitto_client_پیام *client_پیام, *tmp;

	DL_FOREACH_SAFE(*head, client_پیام, tmp){
		DL_DELETE(*head, client_پیام);
		db_پیام_store_ref_dec(&client_پیام->base_پیام);
		mosquitto_FREE(client_پیام);
	}
	*head = پوچ;
}


صحیح db__messages_delete_incoming(ساختار mosquitto *context)
{
	اگر(!context){
		بازگشت MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_in.inflight);
	db__messages_delete_list(&context->msgs_in.queued);
	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_شمار = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_شمار = 0;
	context->msgs_in.queued_count12 = 0;

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح db__messages_delete_outgoing(ساختار mosquitto *context)
{
	اگر(!context){
		بازگشت MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_out.inflight);
	db__messages_delete_list(&context->msgs_out.queued);
	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_شمار = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_شمار = 0;
	context->msgs_out.queued_count12 = 0;

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح db__messages_delete(ساختار mosquitto *context, bool force_free)
{
	اگر(!context){
		بازگشت MOSQ_ERR_INVAL;
	}

	اگر(force_free || context->clean_start || (context->bridge && context->bridge->clean_start)){
		db__messages_delete_incoming(context);
	}

	اگر(force_free || (context->bridge && context->bridge->clean_start_local)
			|| (context->bridge == پوچ && context->clean_start)){

		db__messages_delete_outgoing(context);
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح db__messages_easy_queue(ساختار mosquitto *context, ثابت نویسه *topic, uint8_t qos, uint32_t payloadlen, ثابت تهی *payload, صحیح retain, uint32_t message_expiry_interval, mosquitto_property **properties)
{
	ساختار mosquitto_base_پیام *base_پیام;
	ثابت نویسه *source_id;
	شمارش mosquitto_پیام_origin origin;

	اگر(!topic){
		بازگشت MOSQ_ERR_INVAL;
	}

	base_پیام = mosquitto_calloc(1, اندازه(ساختار mosquitto_base_پیام));
	اگر(base_پیام == پوچ){
		بازگشت MOSQ_ERR_NOMEM;
	}

	base_پیام->data.topic = mosquitto_strdup(topic);
	اگر(base_پیام->data.topic == پوچ){
		db_پیام_store_free(base_پیام);
		بازگشت MOSQ_ERR_INVAL;
	}

	base_پیام->data.qos = qos;
	اگر(db.config->retain_available == false){
		base_پیام->data.retain = 0;
	}وگرنه{
		base_پیام->data.retain = retain;
	}

	base_پیام->data.payloadlen = payloadlen;
	اگر(payloadlen > 0){
		base_پیام->data.payload = mosquitto_malloc(base_پیام->data.payloadlen+1);
		اگر(base_پیام->data.payload == پوچ){
			db_پیام_store_free(base_پیام);
			بازگشت MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_پیام->data.payload)[base_پیام->data.payloadlen] = 0;
		memcpy(base_پیام->data.payload, payload, base_پیام->data.payloadlen);
	}

	اگر(context && context->id){
		source_id = context->id;
	}وگرنه{
		source_id = "";
	}
	اگر(properties){
		base_پیام->data.properties = *properties;
		*properties = پوچ;
	}

	اگر(context){
		origin = mosq_mo_client;
	}وگرنه{
		origin = mosq_mo_broker;
	}
	اگر(db__message_store(context, base_پیام, &message_expiry_interval, origin)){
		بازگشت 1;
	}

	بازگشت sub__messages_queue(source_id, base_پیام->data.qos, &base_پیام);
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
uint64_t db_new_پیام_id(تهی)
{
#ifdef WIN32
	FILETIME ftime;
	uint64_t ftime64;
#وگرنه
	ساختار timespec ts;
#endif
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	long nsec;

	id = db.گره_id_shifted; /* Top 10-bits */

#ifdef WIN32
	GetSystemTimePreciseAsFileTime(&ftime);
	ftime64 = (((uint64_t)ftime.dw_بالا_Date_Time)<<32) + ftime.dw_کم_Date_Time;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (long)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#وگرنه
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#endif
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	اگر(id <= db.last_db_id){
		id = db.last_db_id + 1;
	}
	db.last_db_id = id;

	بازگشت id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
صحیح db__message_store(ثابت ساختار mosquitto *source, ساختار mosquitto_base_پیام *base_پیام, uint32_t *message_expiry_interval, شمارش mosquitto_پیام_origin origin)
{
	صحیح rc;

	أكد(base_پیام);

	اگر(source && source->id){
		base_پیام->data.source_id = mosquitto_strdup(source->id);
	}وگرنه{
		base_پیام->data.source_id = mosquitto_strdup("");
	}
	اگر(!base_پیام->data.source_id){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_پیام_store_free(base_پیام);
		بازگشت MOSQ_ERR_NOMEM;
	}

	اگر(source && source->username){
		base_پیام->data.source_username = mosquitto_strdup(source->username);
		اگر(!base_پیام->data.source_username){
			db_پیام_store_free(base_پیام);
			بازگشت MOSQ_ERR_NOMEM;
		}
	}
	اگر(source){
		base_پیام->source_listener = source->listener;
	}
	base_پیام->origin = origin;
	اگر(message_expiry_interval){
		اگر(*message_expiry_interval > 0 && *message_expiry_interval != پیام_EXPIRY_INFINITE){
			base_پیام->data.expiry_time = db.now_real_s + *message_expiry_interval;
		}وگرنه{
			base_پیام->data.expiry_time = 0;
		}
	}

	base_پیام->dest_ids = پوچ;
	base_پیام->dest_id_شمار = 0;
	db.پیام_store_شمار++;
	db.پیام_store_bytes += base_پیام->data.payloadlen;

	اگر(!base_پیام->data.store_id){
		base_پیام->data.store_id = db_new_پیام_id();
	}

	rc = db_پیام_store_add(base_پیام);
	اگر(rc){
		db_پیام_store_free(base_پیام);
		بازگشت rc;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح db__message_store_find(ساختار mosquitto *context, uint16_t mid, ساختار mosquitto_client_پیام **client_پیام)
{
	ساختار mosquitto_client_پیام *cmsg;

	*client_پیام = پوچ;

	اگر(!context){
		بازگشت MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_in.inflight, cmsg){
		اگر(cmsg->base_پیام->data.source_mid == mid){
			*client_پیام = cmsg;
			بازگشت MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_in.queued, cmsg){
		اگر(cmsg->base_پیام->data.source_mid == mid){
			*client_پیام = cmsg;
			بازگشت MOSQ_ERR_SUCCESS;
		}
	}

	بازگشت 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
ایستا صحیح db__message_reconnect_reset_outgoing(ساختار mosquitto *context)
{
	ساختار mosquitto_client_پیام *client_پیام, *tmp;

	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_شمار = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_شمار = 0;
	context->msgs_out.queued_count12 = 0;
	context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_پیام, tmp){
		db_پیام_add_to_inflight_stats(&context->msgs_out, client_پیام);
		اگر(client_پیام->data.qos > 0){
			util__decrement_send_quota(context);
		}

		انتخاب(client_پیام->data.qos){
			حالت 0:
				client_پیام->data.state = mosq_ms_publish_qos0;
				بشکن;
			حالت 1:
				client_پیام->data.state = mosq_ms_publish_qos1;
				بشکن;
			حالت 2:
				اگر(client_پیام->data.state == mosq_ms_wait_for_pubcomp){
					client_پیام->data.state = mosq_ms_resend_pubrel;
				}وگرنه{
					client_پیام->data.state = mosq_ms_publish_qos2;
				}
				بشکن;
		}
		plugin_persist_handle_client_پیام_update(context, client_پیام);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_out.queued, client_پیام, tmp){
		db_پیام_add_to_queued_stats(&context->msgs_out, client_پیام);
	}
	db__fill_inflight_out_from_queue(context);

	بازگشت MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
ایستا صحیح db__message_reconnect_reset_incoming(ساختار mosquitto *context)
{
	ساختار mosquitto_client_پیام *client_پیام, *tmp;

	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_شمار = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_شمار = 0;
	context->msgs_in.queued_count12 = 0;
	context->msgs_in.inflight_quota = context->msgs_in.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_پیام, tmp){
		db_پیام_add_to_inflight_stats(&context->msgs_in, client_پیام);
		اگر(client_پیام->data.qos > 0){
			util__decrement_receive_quota(context);
		}

		اگر(client_پیام->data.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db__message_remove_inflight(context, &context->msgs_in, client_پیام);
		}وگرنه{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_پیام->data.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_in.queued, client_پیام, tmp){
		client_پیام->data.dup = 0;
		db_پیام_add_to_queued_stats(&context->msgs_in, client_پیام);
		اگر(db__ready_for_flight(context, mosq_md_in, client_پیام->data.qos)){
			انتخاب(client_پیام->data.qos){
				حالت 0:
					client_پیام->data.state = mosq_ms_publish_qos0;
					بشکن;
				حالت 1:
					client_پیام->data.state = mosq_ms_publish_qos1;
					بشکن;
				حالت 2:
					client_پیام->data.state = mosq_ms_publish_qos2;
					بشکن;
			}
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_پیام_update(context, client_پیام);
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح db__message_reconnect_reset(ساختار mosquitto *context)
{
	صحیح rc;

	rc = db__message_reconnect_reset_outgoing(context);
	اگر(rc){
		بازگشت rc;
	}
	بازگشت db__message_reconnect_reset_incoming(context);
}


صحیح db__message_remove_incoming(ساختار mosquitto *context, uint16_t mid)
{
	ساختار mosquitto_client_پیام *client_پیام, *tmp;

	اگر(!context){
		بازگشت MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_پیام, tmp){
		اگر(client_پیام->data.mid == mid){
			اگر(client_پیام->base_پیام->data.qos != 2){
				log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_پیام->base_پیام->data.qos);
				بازگشت MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_in, client_پیام);
			بازگشت MOSQ_ERR_SUCCESS;
		}
	}

	بازگشت MOSQ_ERR_NOT_FOUND;
}


صحیح db__message_release_incoming(ساختار mosquitto *context, uint16_t mid)
{
	ساختار mosquitto_client_پیام *client_پیام, *tmp;
	bool deleted = false;
	صحیح rc;

	اگر(!context){
		بازگشت MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_پیام, tmp){
		اگر(client_پیام->data.mid == mid){
			اگر(client_پیام->base_پیام->data.qos != 2){
				log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_پیام->base_پیام->data.qos);
				بازگشت MOSQ_ERR_PROTOCOL;
			}
			ثابت نویسه *topic = client_پیام->base_پیام->data.topic;
			ثابت نویسه *source_id = client_پیام->base_پیام->data.source_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			اگر(topic == پوچ){
				db__message_remove_inflight(context, &context->msgs_in, client_پیام);
				deleted = true;
			}وگرنه{
				rc = sub__messages_queue(source_id, 2, &client_پیام->base_پیام);
				اگر(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db__message_remove_inflight(context, &context->msgs_in, client_پیام);
					deleted = true;
				}وگرنه{
					بازگشت 1;
				}
			}
		}
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_پیام, tmp){
		اگر(db__ready_for_flight(context, mosq_md_in, client_پیام->data.qos)){
			بشکن;
		}

		اگر(client_پیام->data.qos == 2){
			send__pubrec(context, client_پیام->data.mid, 0, پوچ);
			client_پیام->data.state = mosq_ms_wait_for_pubrel;
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_پیام_update(context, client_پیام);
		}
	}
	اگر(deleted){
		بازگشت MOSQ_ERR_SUCCESS;
	}وگرنه{
		بازگشت MOSQ_ERR_NOT_FOUND;
	}
}


تهی db__expire_all_messages(ساختار mosquitto *context)
{
	ساختار mosquitto_client_پیام *client_پیام, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_پیام, tmp){
		اگر(client_پیام->base_پیام->data.expiry_time && db.now_real_s > client_پیام->base_پیام->data.expiry_time){
			اگر(client_پیام->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_پیام);
		}
	}
	db__fill_inflight_out_from_queue(context);
	DL_FOREACH_SAFE(context->msgs_out.queued, client_پیام, tmp){
		اگر(client_پیام->base_پیام->data.expiry_time && db.now_real_s > client_پیام->base_پیام->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_پیام);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.inflight, client_پیام, tmp){
		اگر(client_پیام->base_پیام->data.expiry_time && db.now_real_s > client_پیام->base_پیام->data.expiry_time){
			اگر(client_پیام->data.qos > 0){
				util__increment_receive_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_in, client_پیام);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.queued, client_پیام, tmp){
		اگر(client_پیام->base_پیام->data.expiry_time && db.now_real_s > client_پیام->base_پیام->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_in, client_پیام);
		}
	}
}


ایستا تهی db__client_messages_check_acl(ساختار mosquitto *context, ساختار mosquitto_پیام_data *پیام_data, ساختار mosquitto_client_پیام **head,
		تهی (*decrement_stats_fn)(ساختار mosquitto_پیام_data *پیام_data, ساختار mosquitto_client_پیام *client_پیام))
{
	ساختار mosquitto_client_پیام *client_پیام, *tmp;
	ساختار mosquitto_base_پیام *base_پیام;
	صحیح access;

	DL_FOREACH_SAFE((*head), client_پیام, tmp){
		base_پیام = client_پیام->base_پیام;
		اگر(client_پیام->data.direction == mosq_md_out){
			access = MOSQ_ACL_READ;
		}وگرنه{
			access = MOSQ_ACL_WRITE;
		}
		اگر(mosquitto_acl_check(context, base_پیام->data.topic,
				base_پیام->data.payloadlen, base_پیام->data.payload,
				base_پیام->data.qos, base_پیام->data.retain,
				base_پیام->data.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*head), client_پیام);
			decrement_stats_fn(پیام_data, client_پیام);
			plugin_persist_handle_client_پیام_delete(context, client_پیام);
			db_پیام_store_ref_dec(&client_پیام->base_پیام);
			mosquitto_FREE(client_پیام);
		}
	}
}


تهی db__check_acl_of_all_messages(ساختار mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.inflight, &db_پیام_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.queued, &db_پیام_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.inflight, &db_پیام_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.queued, &db_پیام_remove_from_queued_stats);
}


ایستا صحیح db__message_write_inflight_out_single(ساختار mosquitto *context, ساختار mosquitto_client_پیام *client_پیام)
{
	ساختار mosquitto_base_پیام *base_پیام;
	mosquitto_property *base_پیام_props = پوچ;
	صحیح rc;
	uint16_t mid;
	صحیح retries;
	صحیح retain;
	ثابت نویسه *topic;
	uint8_t qos;
	uint32_t payloadlen;
	ثابت تهی *payload;
	uint32_t expiry_interval;
	uint32_t subscription_id;

	base_پیام = client_پیام->base_پیام;

	expiry_interval = 0;
	اگر(base_پیام->data.expiry_time){
		اگر(db.now_real_s > base_پیام->data.expiry_time){
			/* Message is expired, must not send. */
			اگر(client_پیام->data.direction == mosq_md_out && client_پیام->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_پیام);
			db__fill_inflight_out_from_queue(context);
			بازگشت MOSQ_ERR_SUCCESS;
		}وگرنه{
			expiry_interval = (uint32_t)(base_پیام->data.expiry_time - db.now_real_s);
		}
	}
	mid = client_پیام->data.mid;
	retries = client_پیام->data.dup;
	retain = client_پیام->data.retain;
	topic = base_پیام->data.topic;
	qos = (uint8_t)client_پیام->data.qos;
	payloadlen = base_پیام->data.payloadlen;
	payload = base_پیام->data.payload;
	subscription_id = client_پیام->data.subscription_identifier;
	base_پیام_props = base_پیام->data.properties;

	انتخاب(client_پیام->data.state){
		حالت mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_پیام_props, expiry_interval);
			اگر(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_پیام);
			}وگرنه{
				بازگشت rc;
			}
			بشکن;

		حالت mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_پیام_props, expiry_interval);
			اگر(rc == MOSQ_ERR_SUCCESS){
				client_پیام->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_پیام->data.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_پیام_update(context, client_پیام);
			}وگرنه اگر(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_پیام);
			}وگرنه{
				بازگشت rc;
			}
			بشکن;

		حالت mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_پیام_props, expiry_interval);
			اگر(rc == MOSQ_ERR_SUCCESS){
				client_پیام->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_پیام->data.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_پیام_update(context, client_پیام);
			}وگرنه اگر(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_پیام);
			}وگرنه{
				بازگشت rc;
			}
			بشکن;

		حالت mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, پوچ);
			اگر(!rc){
				client_پیام->data.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_پیام_update(context, client_پیام);
			}وگرنه{
				بازگشت rc;
			}
			بشکن;

		حالت mosq_ms_invalid:
		حالت mosq_ms_send_pubrec:
		حالت mosq_ms_resend_pubcomp:
		حالت mosq_ms_wait_for_puback:
		حالت mosq_ms_wait_for_pubrec:
		حالت mosq_ms_wait_for_pubrel:
		حالت mosq_ms_wait_for_pubcomp:
		حالت mosq_ms_queued:
			بشکن;
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح db__message_write_inflight_out_all(ساختار mosquitto *context)
{
	ساختار mosquitto_client_پیام *client_پیام, *tmp;
	صحیح rc;

	اگر(context->state != mosq_cs_active || !net__is_connected(context)){
		بازگشت MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_پیام, tmp){
		rc = db__message_write_inflight_out_single(context, client_پیام);
		اگر(rc){
			بازگشت rc;
		}
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح db__message_write_inflight_out_latest(ساختار mosquitto *context)
{
	ساختار mosquitto_client_پیام *client_پیام, *next;
	صحیح rc;

	اگر(context->state != mosq_cs_active
			|| !net__is_connected(context)
			|| context->msgs_out.inflight == پوچ){

		بازگشت MOSQ_ERR_SUCCESS;
	}

	اگر(context->msgs_out.inflight->prev == context->msgs_out.inflight){
		/* Only one message */
		بازگشت db__message_write_inflight_out_single(context, context->msgs_out.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_پیام = context->msgs_out.inflight->prev;
	وقتی(client_پیام != context->msgs_out.inflight &&
			(client_پیام->data.state == mosq_ms_publish_qos0
			|| client_پیام->data.state == mosq_ms_publish_qos1
			|| client_پیام->data.state == mosq_ms_publish_qos2)){

		client_پیام = client_پیام->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	اگر(client_پیام != context->msgs_out.inflight){
		client_پیام = client_پیام->next;
	}

	وقتی(client_پیام){
		next = client_پیام->next;
		rc = db__message_write_inflight_out_single(context, client_پیام);
		اگر(rc){
			بازگشت rc;
		}
		client_پیام = next;
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح db__message_write_queued_in(ساختار mosquitto *context)
{
	ساختار mosquitto_client_پیام *client_پیام, *tmp;
	صحیح rc;

	اگر(context->state != mosq_cs_active){
		بازگشت MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_پیام, tmp){
		اگر(context->msgs_in.inflight_maximum != 0 && context->msgs_in.inflight_quota == 0){
			بشکن;
		}

		اگر(client_پیام->data.qos == 2){
			client_پیام->data.state = mosq_ms_send_pubrec;
			db__message_dequeue_first(context, &context->msgs_in);
			rc = send__pubrec(context, client_پیام->data.mid, 0, پوچ);
			اگر(!rc){
				client_پیام->data.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_پیام_update(context, client_پیام);
			}وگرنه{
				plugin_persist_handle_client_پیام_update(context, client_پیام);
				بازگشت rc;
			}
		}
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح db__message_write_queued_out(ساختار mosquitto *context)
{
	اگر(context->state != mosq_cs_active){
		بازگشت MOSQ_ERR_SUCCESS;
	}

	db__fill_inflight_out_from_queue(context);

	بازگشت MOSQ_ERR_SUCCESS;
}
