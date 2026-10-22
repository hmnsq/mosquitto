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

#ضمن "config.h"

#ضمن <assert.h>
#ضمن <stdio.h>
#ضمن <utlist.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "send_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"


/**
 * Is this context ready to take more in flight messages right now?
 * @param context the client context of interest
 * @param qos qos for the packet of interest
 * @return true if more in flight are allowed.
 */
bool db__ready_for_flight(بنية mosquitto *context, تعداد mosquitto_msg_direction dir, صحيح qos)
{
	بنية mosquitto_msg_data *msgs;
	bool valid_bytes;
	bool valid_count;

	ܐܢ(dir == mosq_md_out){
		msgs = &context->msgs_out;
	}ܐܚܪܢܐ{
		msgs = &context->msgs_in;
	}

	ܐܢ(msgs->inflight_maximum == 0 && db.config->max_inflight_bytes == 0){
		ܦܢܐ true;
	}

	ܐܢ(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		ܐܢ(db.config->max_queued_messages == 0 && db.config->max_inflight_bytes == 0){
			ܦܢܐ true;
		}
		valid_bytes = ((msgs->inflight_bytes - (ssize_t)db.config->max_inflight_bytes) < (ssize_t)db.config->max_queued_bytes);
		ܐܢ(dir == mosq_md_out){
			valid_count = context->out_packet_count < db.config->max_queued_messages;
		}ܐܚܪܢܐ{
			valid_count = msgs->inflight_count - msgs->inflight_maximum < db.config->max_queued_messages;
		}

		ܐܢ(db.config->max_queued_messages == 0){
			ܦܢܐ valid_bytes;
		}
		ܐܢ(db.config->max_queued_bytes == 0){
			ܦܢܐ valid_count;
		}
	}ܐܚܪܢܐ{
		valid_bytes = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_bytes;
		valid_count = msgs->inflight_quota > 0;

		ܐܢ(msgs->inflight_maximum == 0){
			ܦܢܐ valid_bytes;
		}
		ܐܢ(db.config->max_inflight_bytes == 0){
			ܦܢܐ valid_count;
		}
	}

	ܦܢܐ valid_bytes && valid_count;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db__ready_for_queue(بنية mosquitto *context, صحيح qos, بنية mosquitto_msg_data *msg_data)
{
	صحيح source_count;
	صحيح adjust_count;
	long source_bytes;
	ssize_t adjust_bytes = (ssize_t)db.config->max_inflight_bytes;
	bool valid_bytes;
	bool valid_count;

	ܐܢ(db.config->max_queued_messages == 0 && db.config->max_queued_bytes == 0){
		ܦܢܐ true;
	}

	ܐܢ(qos == 0 && db.config->queue_qos0_messages == false){
		ܦܢܐ false; /* This case is handled in db__ready_for_flight() */
	}ܐܚܪܢܐ{
		source_bytes = (ssize_t)msg_data->queued_bytes12;
		source_count = msg_data->queued_count12;
	}
	adjust_count = msg_data->inflight_maximum;

	/* nothing in flight for offline clients */
	ܐܢ(!net__is_connected(context)){
		adjust_bytes = 0;
		adjust_count = 0;
	}

	valid_bytes = (source_bytes - (ssize_t)adjust_bytes) < (ssize_t)db.config->max_queued_bytes;
	valid_count = source_count - adjust_count < db.config->max_queued_messages;

	ܐܢ(db.config->max_queued_bytes == 0){
		ܦܢܐ valid_count;
	}
	ܐܢ(db.config->max_queued_messages == 0){
		ܦܢܐ valid_bytes;
	}

	ܦܢܐ valid_bytes && valid_count;
}


فراغ db__msg_add_to_inflight_stats(بنية mosquitto_msg_data *msg_data, بنية mosquitto__client_msg *client_msg)
{
	msg_data->inflight_count++;
	msg_data->inflight_bytes += client_msg->base_msg->data.payloadlen;
	ܐܢ(client_msg->data.qos != 0){
		msg_data->inflight_count12++;
		msg_data->inflight_bytes12 += client_msg->base_msg->data.payloadlen;
	}
}


ساكن فراغ db__msg_remove_from_inflight_stats(بنية mosquitto_msg_data *msg_data, بنية mosquitto__client_msg *client_msg)
{
	msg_data->inflight_count--;
	msg_data->inflight_bytes -= client_msg->base_msg->data.payloadlen;
	ܐܢ(client_msg->data.qos != 0){
		msg_data->inflight_count12--;
		msg_data->inflight_bytes12 -= client_msg->base_msg->data.payloadlen;
	}
}


فراغ db__msg_add_to_queued_stats(بنية mosquitto_msg_data *msg_data, بنية mosquitto__client_msg *client_msg)
{
	msg_data->queued_count++;
	msg_data->queued_bytes += client_msg->base_msg->data.payloadlen;
	ܐܢ(client_msg->data.qos != 0){
		msg_data->queued_count12++;
		msg_data->queued_bytes12 += client_msg->base_msg->data.payloadlen;
	}
}


ساكن فراغ db__msg_remove_from_queued_stats(بنية mosquitto_msg_data *msg_data, بنية mosquitto__client_msg *client_msg)
{
	msg_data->queued_count--;
	msg_data->queued_bytes -= client_msg->base_msg->data.payloadlen;
	ܐܢ(client_msg->data.qos != 0){
		msg_data->queued_count12--;
		msg_data->queued_bytes12 -= client_msg->base_msg->data.payloadlen;
	}
}


صحيح db__open(بنية mosquitto__config *config)
{
	ܐܢ(!config){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = عدم;
	db.contexts_by_sock = عدم;
	db.contexts_for_free = عدم;
#ifdef WITH_BRIDGE
	db.bridges = عدم;
	db.bridge_count = 0;
#endif

	/* Initialize the hashtable */
	db.clientid_index_hash = عدم;

	db.normal_subs = عدم;
	db.shared_subs = عدم;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = عدم;

#ifdef WITH_PERSISTENCE
	ܐܢ(persist__restore()){
		ܦܢܐ 1;
	}
#endif

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


ساكن فراغ subhier_clean(بنية mosquitto__subhier **subhier)
{
	بنية mosquitto__subhier *peer, *subhier_tmp;
	بنية mosquitto__subleaf *leaf, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		leaf = peer->subs;
		ܟܡܐ(leaf){
			nextleaf = leaf->next;
			mosquitto_FREE(leaf);
			leaf = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


صحيح db__close(فراغ)
{
	subhier_clean(&db.normal_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db__msg_store_clean();

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح db__msg_store_add(بنية mosquitto__base_msg *base_msg)
{
	بنية mosquitto__base_msg *found;
	unsigned hashv;

	HASH_VALUE(&base_msg->data.store_id, حجم(base_msg->data.store_id), hashv);
	HASH_FIND_BYHASHVALUE(hh, db.msg_store, &base_msg->data.store_id, حجم(base_msg->data.store_id), hashv, found);
	ܐܢ(found == عدم){
		HASH_ADD_KEYPTR_BYHASHVALUE(hh, db.msg_store, &base_msg->data.store_id, حجم(base_msg->data.store_id), hashv, base_msg);
		ܦܢܐ MOSQ_ERR_SUCCESS;
	}ܐܚܪܢܐ{
		ܦܢܐ MOSQ_ERR_ALREADY_EXISTS;
	}
}


فراغ db__msg_store_free(بنية mosquitto__base_msg *base_msg)
{
	mosquitto_FREE(base_msg->data.source_id);
	mosquitto_FREE(base_msg->data.source_username);
	ܐܢ(base_msg->dest_ids){
		لكل(صحيح i=0; i<base_msg->dest_id_count; i++){
			mosquitto_FREE(base_msg->dest_ids[i]);
		}
		mosquitto_FREE(base_msg->dest_ids);
	}
	mosquitto_FREE(base_msg->data.topic);
	mosquitto_property_free_all(&base_msg->data.properties);
	mosquitto_FREE(base_msg->data.payload);
	mosquitto_FREE(base_msg);
}


فراغ db__msg_store_remove(بنية mosquitto__base_msg *base_msg, bool notify)
{
	ܐܢ(base_msg == عدم){
		ܦܢܐ;
	}
	HASH_DELETE(hh, db.msg_store, base_msg);
	db.msg_store_count--;
	db.msg_store_bytes -= base_msg->data.payloadlen;
	ܐܢ(notify == true){
		plugin_persist__handle_base_msg_delete(base_msg);
	}
	db__msg_store_free(base_msg);
}


فراغ db__msg_store_clean(فراغ)
{
	بنية mosquitto__base_msg *base_msg, *base_msg_tmp;

	HASH_ITER(hh, db.msg_store, base_msg, base_msg_tmp){
		db__msg_store_remove(base_msg, false);
	}
}


فراغ db__msg_store_ref_inc(بنية mosquitto__base_msg *base_msg)
{
	base_msg->ref_count++;
}


فراغ db__msg_store_ref_dec(بنية mosquitto__base_msg **base_msg)
{
	(*base_msg)->ref_count--;
	ܐܢ((*base_msg)->ref_count == 0){
		db__msg_store_remove(*base_msg, true);
		*base_msg = عدم;
	}
}


فراغ db__msg_store_compact(فراغ)
{
	بنية mosquitto__base_msg *base_msg, *base_msg_tmp;

	HASH_ITER(hh, db.msg_store, base_msg, base_msg_tmp){
		ܐܢ(base_msg->ref_count < 1){
			db__msg_store_remove(base_msg, true);
		}
	}
}


ساكن فراغ db__message_remove_inflight(بنية mosquitto *context, بنية mosquitto_msg_data *msg_data, بنية mosquitto__client_msg *item)
{
	ܐܢ(!context || !msg_data || !item){
		ܦܢܐ;
	}

	plugin_persist__handle_client_msg_delete(context, item);

	DL_DELETE(msg_data->inflight, item);
	ܐܢ(item->base_msg){
		db__msg_remove_from_inflight_stats(msg_data, item);
		db__msg_store_ref_dec(&item->base_msg);
	}

	mosquitto_FREE(item);
}


ساكن فراغ db__message_remove_queued(بنية mosquitto *context, بنية mosquitto_msg_data *msg_data, بنية mosquitto__client_msg *item)
{
	ܐܢ(!context || !msg_data || !item){
		ܦܢܐ;
	}

	plugin_persist__handle_client_msg_delete(context, item);

	DL_DELETE(msg_data->queued, item);
	ܐܢ(item->base_msg){
		db__msg_remove_from_queued_stats(msg_data, item);
		db__msg_store_ref_dec(&item->base_msg);
	}

	mosquitto_FREE(item);
}


ساكن فراغ db__fill_inflight_out_from_queue(بنية mosquitto *context)
{
	بنية mosquitto__client_msg *client_msg, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.queued, client_msg, tmp){
		ܐܢ(!db__ready_for_flight(context, mosq_md_out, client_msg->data.qos)){
			ܦܢܐ;
		}
		بدل(client_msg->data.qos){
			حالة 0:
				client_msg->data.state = mosq_ms_publish_qos0;
				اكسر;
			حالة 1:
				client_msg->data.state = mosq_ms_publish_qos1;
				اكسر;
			حالة 2:
				client_msg->data.state = mosq_ms_publish_qos2;
				اكسر;
		}
		ܐܢ(client_msg->base_msg->data.expiry_time && db.now_real_s > client_msg->base_msg->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_msg);
			استمر;
		}
		plugin_persist__handle_client_msg_update(context, client_msg);
		db__message_dequeue_first(context, &context->msgs_out);
	}
}


فراغ db__message_dequeue_first(بنية mosquitto *context, بنية mosquitto_msg_data *msg_data)
{
	بنية mosquitto__client_msg *client_msg;

	UNUSED(context);

	client_msg = msg_data->queued;
	DL_DELETE(msg_data->queued, client_msg);
	DL_APPEND(msg_data->inflight, client_msg);
	ܐܢ(msg_data->inflight_quota > 0){
		msg_data->inflight_quota--;
	}

	db__msg_remove_from_queued_stats(msg_data, client_msg);
	db__msg_add_to_inflight_stats(msg_data, client_msg);
}


صحيح db__message_delete_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_msg_state expect_state, صحيح qos)
{
	بنية mosquitto__client_msg *client_msg, *tmp;
	bool deleted = false;

	ܐܢ(!context){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_msg, tmp){
		ܐܢ(client_msg->data.mid == mid){
			ܐܢ(client_msg->data.qos != qos){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_msg->data.qos, qos);
				ܦܢܐ MOSQ_ERR_PROTOCOL;
			}ܐܚܪܢܐ ܐܢ(qos == 2 && client_msg->data.state != expect_state && expect_state != mosq_ms_any){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_msg->data.state, expect_state);
				ܦܢܐ MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_out, client_msg);
			deleted = true;
			اكسر;
		}
	}

	ܐܢ(deleted == false){
		DL_FOREACH_SAFE(context->msgs_out.queued, client_msg, tmp){
			ܐܢ(client_msg->data.mid == mid){
				ܐܢ(client_msg->data.qos != qos){
					ܦܢܐ MOSQ_ERR_PROTOCOL;
				}ܐܚܪܢܐ ܐܢ(qos == 2 && client_msg->data.state != expect_state && expect_state != mosq_ms_any){
					ܦܢܐ MOSQ_ERR_PROTOCOL;
				}
				db__message_remove_queued(context, &context->msgs_out, client_msg);
				اكسر;
			}
		}
	}
	db__fill_inflight_out_from_queue(context);
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	ܦܢܐ db__message_write_inflight_out_latest(context);
}


/* Only for QoS 2 messages */
صحيح db__message_insert_incoming(بنية mosquitto *context, uint64_t cmsg_id, بنية mosquitto__base_msg *base_msg, bool persist)
{
	بنية mosquitto__client_msg *client_msg;
	بنية mosquitto_msg_data *msg_data;
	تعداد mosquitto_msg_state state = mosq_ms_invalid;
	صحيح rc = 0;

	أكد(base_msg);
	ܐܢ(!context){
		ܦܢܐ MOSQ_ERR_INVAL;
	}
	ܐܢ(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		ܦܢܐ MOSQ_ERR_SUCCESS;

	}
	msg_data = &context->msgs_in;

	ܐܢ(db__ready_for_flight(context, mosq_md_in, base_msg->data.qos)){
		state = mosq_ms_wait_for_pubrel;
	}ܐܚܪܢܐ ܐܢ(base_msg->data.qos != 0 && db__ready_for_queue(context, base_msg->data.qos, msg_data)){
		state = mosq_ms_queued;
		rc = 2;
	}ܐܚܪܢܐ{
		/* Dropping message due to full queue. */
		ܐܢ(context->is_dropping == false){
			context->is_dropping = true;
			log__printf(عدم, MOSQ_LOG_NOTICE,
					"Outgoing messages are being dropped for client %s.",
					context->id);
		}
		metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		ܦܢܐ 2;
	}

	أكد(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	ܐܢ(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_msg = mosquitto_malloc(حجم(بنية mosquitto__client_msg));
	ܐܢ(!client_msg){
		ܦܢܐ MOSQ_ERR_NOMEM;
	}
	client_msg->prev = عدم;
	client_msg->next = عدم;
	ܐܢ(cmsg_id){
		client_msg->data.cmsg_id = cmsg_id;
	}ܐܚܪܢܐ{
		client_msg->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_msg->base_msg = base_msg;
	db__msg_store_ref_inc(client_msg->base_msg);
	client_msg->data.mid = base_msg->data.source_mid;
	client_msg->data.direction = mosq_md_in;
	client_msg->data.state = (uint8_t)state;
	client_msg->data.dup = false;
	ܐܢ(base_msg->data.qos > context->max_qos){
		client_msg->data.qos = context->max_qos;
	}ܐܚܪܢܐ{
		client_msg->data.qos = base_msg->data.qos;
	}
	client_msg->data.retain = base_msg->data.retain;
	client_msg->data.subscription_identifier = 0;

	ܐܢ(state == mosq_ms_queued){
		DL_APPEND(msg_data->queued, client_msg);
		db__msg_add_to_queued_stats(msg_data, client_msg);
	}ܐܚܪܢܐ{
		DL_APPEND(msg_data->inflight, client_msg);
		db__msg_add_to_inflight_stats(msg_data, client_msg);
	}

	ܐܢ(persist && context->is_persisted){
		plugin_persist__handle_base_msg_add(client_msg->base_msg);
		plugin_persist__handle_client_msg_add(context, client_msg);
	}

	ܐܢ(client_msg->base_msg->data.qos > 0){
		util__decrement_receive_quota(context);
	}
	ܦܢܐ rc;
}


صحيح db__message_insert_outgoing(بنية mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, بنية mosquitto__base_msg *base_msg, uint32_t subscription_identifier, bool update, bool persist)
{
	بنية mosquitto__client_msg *client_msg;
	بنية mosquitto_msg_data *msg_data;
	تعداد mosquitto_msg_state state = mosq_ms_invalid;
	صحيح rc = 0;
	حرف **dest_ids;

	أكد(base_msg);
	ܐܢ(!context){
		ܦܢܐ MOSQ_ERR_INVAL;
	}
	ܐܢ(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		ܦܢܐ MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_sent++;

	msg_data = &context->msgs_out;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	ܐܢ(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && base_msg->dest_ids){

		لكل(صحيح i=0; i<base_msg->dest_id_count; i++){
			ܐܢ(base_msg->dest_ids[i] && !strcmp(base_msg->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				ܦܢܐ MOSQ_ERR_SUCCESS;
			}
		}
	}
	ܐܢ(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		ܐܢ(qos == 0 && !db.config->queue_qos0_messages){
			ܐܢ(!context->bridge){
				ܦܢܐ 2;
			}ܐܚܪܢܐ{
				ܐܢ(context->bridge->start_type != bst_lazy){
					ܦܢܐ 2;
				}
			}
		}
		ܐܢ(context->bridge && context->bridge->clean_start_local == true){
			ܦܢܐ 2;
		}
	}

	ܐܢ(net__is_connected(context)){
		ܐܢ(db__ready_for_flight(context, mosq_md_out, qos)){
			بدل(qos){
				حالة 0:
					state = mosq_ms_publish_qos0;
					اكسر;
				حالة 1:
					state = mosq_ms_publish_qos1;
					اكسر;
				حالة 2:
					state = mosq_ms_publish_qos2;
					اكسر;
			}
		}ܐܚܪܢܐ ܐܢ(qos != 0 && db__ready_for_queue(context, qos, msg_data)){
			state = mosq_ms_queued;
			rc = 2;
		}ܐܚܪܢܐ{
			/* Dropping message due to full queue. */
			ܐܢ(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(عدم, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			ܦܢܐ 2;
		}
	}ܐܚܪܢܐ{
		ܐܢ(db__ready_for_queue(context, qos, msg_data)){
			state = mosq_ms_queued;
		}ܐܚܪܢܐ{
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			ܐܢ(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(عدم, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			ܦܢܐ 2;
		}
	}
	أكد(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	ܐܢ(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_msg = mosquitto_malloc(حجم(بنية mosquitto__client_msg));
	ܐܢ(!client_msg){
		ܦܢܐ MOSQ_ERR_NOMEM;
	}
	client_msg->prev = عدم;
	client_msg->next = عدم;
	ܐܢ(cmsg_id){
		client_msg->data.cmsg_id = cmsg_id;
	}ܐܚܪܢܐ{
		client_msg->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_msg->base_msg = base_msg;
	db__msg_store_ref_inc(client_msg->base_msg);
	client_msg->data.mid = mid;
	client_msg->data.direction = mosq_md_out;
	client_msg->data.state = (uint8_t)state;
	client_msg->data.dup = false;
	ܐܢ(qos > context->max_qos){
		client_msg->data.qos = context->max_qos;
	}ܐܚܪܢܐ{
		client_msg->data.qos = qos;
	}
	client_msg->data.retain = retain;
	client_msg->data.subscription_identifier = subscription_identifier;

	ܐܢ(state == mosq_ms_queued){
		DL_APPEND(msg_data->queued, client_msg);
		db__msg_add_to_queued_stats(msg_data, client_msg);
	}ܐܚܪܢܐ{
		DL_APPEND(msg_data->inflight, client_msg);
		db__msg_add_to_inflight_stats(msg_data, client_msg);
	}

	ܐܢ(persist && context->is_persisted){
		plugin_persist__handle_base_msg_add(client_msg->base_msg);
		plugin_persist__handle_client_msg_add(context, client_msg);
	}

	ܐܢ(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(base_msg->dest_ids, حجم(حرف *)*(size_t)(base_msg->dest_id_count+1));
		ܐܢ(dest_ids){
			base_msg->dest_ids = dest_ids;
			base_msg->dest_id_count++;
			base_msg->dest_ids[base_msg->dest_id_count-1] = mosquitto_strdup(context->id);
			ܐܢ(!base_msg->dest_ids[base_msg->dest_id_count-1]){
				ܦܢܐ MOSQ_ERR_NOMEM;
			}
		}ܐܚܪܢܐ{
			ܦܢܐ MOSQ_ERR_NOMEM;
		}
	}
#ifdef WITH_BRIDGE
	ܐܢ(context->bridge && context->bridge->start_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_out.inflight_count + context->msgs_out.queued_count >= context->bridge->threshold){

		context->bridge->lazy_reconnect = true;
	}
#endif

	ܐܢ(client_msg->data.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	ܐܢ(update){
		rc = db__message_write_inflight_out_latest(context);
		ܐܢ(rc){
			ܦܢܐ rc;
		}
		rc = db__message_write_queued_out(context);
		ܐܢ(rc){
			ܦܢܐ rc;
		}
	}

	ܦܢܐ rc;
}


ساكن مضمن صحيح db__message_update_outgoing_state(بنية mosquitto *context, بنية mosquitto__client_msg *head,
		uint16_t mid, تعداد mosquitto_msg_state state, صحيح qos, bool persist)
{
	بنية mosquitto__client_msg *client_msg;

	DL_FOREACH(head, client_msg){
		ܐܢ(client_msg->data.mid == mid){
			ܐܢ(client_msg->data.qos != qos){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_msg->data.qos, qos);
				ܦܢܐ MOSQ_ERR_PROTOCOL;
			}
			client_msg->data.state = (uint8_t)state;
			ܐܢ(persist){
				plugin_persist__handle_client_msg_update(context, client_msg);
			}
			ܦܢܐ MOSQ_ERR_SUCCESS;
		}
	}
	ܦܢܐ MOSQ_ERR_NOT_FOUND;
}


صحيح db__message_update_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_msg_state state, صحيح qos, bool persist)
{
	صحيح rc;

	rc = db__message_update_outgoing_state(context, context->msgs_out.inflight, mid, state, qos, persist);
	ܐܢ(!persist && rc == MOSQ_ERR_NOT_FOUND){
		rc = db__message_update_outgoing_state(context, context->msgs_out.queued, mid, state, qos, persist);
	}
	ܦܢܐ rc;
}


ساكن فراغ db__messages_delete_list(بنية mosquitto__client_msg **head)
{
	بنية mosquitto__client_msg *client_msg, *tmp;

	DL_FOREACH_SAFE(*head, client_msg, tmp){
		DL_DELETE(*head, client_msg);
		db__msg_store_ref_dec(&client_msg->base_msg);
		mosquitto_FREE(client_msg);
	}
	*head = عدم;
}


صحيح db__messages_delete_incoming(بنية mosquitto *context)
{
	ܐܢ(!context){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_in.inflight);
	db__messages_delete_list(&context->msgs_in.queued);
	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_count = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_count = 0;
	context->msgs_in.queued_count12 = 0;

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح db__messages_delete_outgoing(بنية mosquitto *context)
{
	ܐܢ(!context){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_out.inflight);
	db__messages_delete_list(&context->msgs_out.queued);
	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_count = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_count = 0;
	context->msgs_out.queued_count12 = 0;

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح db__messages_delete(بنية mosquitto *context, bool force_free)
{
	ܐܢ(!context){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	ܐܢ(force_free || context->clean_start || (context->bridge && context->bridge->clean_start)){
		db__messages_delete_incoming(context);
	}

	ܐܢ(force_free || (context->bridge && context->bridge->clean_start_local)
			|| (context->bridge == عدم && context->clean_start)){

		db__messages_delete_outgoing(context);
	}

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح db__messages_easy_queue(بنية mosquitto *context, ثابت حرف *topic, uint8_t qos, uint32_t payloadlen, ثابت فراغ *payload, صحيح retain, uint32_t message_expiry_interval, mosquitto_property **properties)
{
	بنية mosquitto__base_msg *base_msg;
	ثابت حرف *source_id;
	تعداد mosquitto_msg_origin origin;

	ܐܢ(!topic){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	base_msg = mosquitto_calloc(1, حجم(بنية mosquitto__base_msg));
	ܐܢ(base_msg == عدم){
		ܦܢܐ MOSQ_ERR_NOMEM;
	}

	base_msg->data.topic = mosquitto_strdup(topic);
	ܐܢ(base_msg->data.topic == عدم){
		db__msg_store_free(base_msg);
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	base_msg->data.qos = qos;
	ܐܢ(db.config->retain_available == false){
		base_msg->data.retain = 0;
	}ܐܚܪܢܐ{
		base_msg->data.retain = retain;
	}

	base_msg->data.payloadlen = payloadlen;
	ܐܢ(payloadlen > 0){
		base_msg->data.payload = mosquitto_malloc(base_msg->data.payloadlen+1);
		ܐܢ(base_msg->data.payload == عدم){
			db__msg_store_free(base_msg);
			ܦܢܐ MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_msg->data.payload)[base_msg->data.payloadlen] = 0;
		memcpy(base_msg->data.payload, payload, base_msg->data.payloadlen);
	}

	ܐܢ(context && context->id){
		source_id = context->id;
	}ܐܚܪܢܐ{
		source_id = "";
	}
	ܐܢ(properties){
		base_msg->data.properties = *properties;
		*properties = عدم;
	}

	ܐܢ(context){
		origin = mosq_mo_client;
	}ܐܚܪܢܐ{
		origin = mosq_mo_broker;
	}
	ܐܢ(db__message_store(context, base_msg, &message_expiry_interval, origin)){
		ܦܢܐ 1;
	}

	ܦܢܐ sub__messages_queue(source_id, base_msg->data.qos, &base_msg);
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
uint64_t db__new_msg_id(فراغ)
{
#ifdef WIN32
	FILETIME ftime;
	uint64_t ftime64;
#ܐܚܪܢܐ
	بنية timespec ts;
#endif
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	long nsec;

	id = db.node_id_shifted; /* Top 10-bits */

#ifdef WIN32
	GetSystemTimePreciseAsFileTime(&ftime);
	ftime64 = (((uint64_t)ftime.dwHighDateTime)<<32) + ftime.dwLowDateTime;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (long)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#ܐܚܪܢܐ
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#endif
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	ܐܢ(id <= db.last_db_id){
		id = db.last_db_id + 1;
	}
	db.last_db_id = id;

	ܦܢܐ id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
صحيح db__message_store(ثابت بنية mosquitto *source, بنية mosquitto__base_msg *base_msg, uint32_t *message_expiry_interval, تعداد mosquitto_msg_origin origin)
{
	صحيح rc;

	أكد(base_msg);

	ܐܢ(source && source->id){
		base_msg->data.source_id = mosquitto_strdup(source->id);
	}ܐܚܪܢܐ{
		base_msg->data.source_id = mosquitto_strdup("");
	}
	ܐܢ(!base_msg->data.source_id){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		db__msg_store_free(base_msg);
		ܦܢܐ MOSQ_ERR_NOMEM;
	}

	ܐܢ(source && source->username){
		base_msg->data.source_username = mosquitto_strdup(source->username);
		ܐܢ(!base_msg->data.source_username){
			db__msg_store_free(base_msg);
			ܦܢܐ MOSQ_ERR_NOMEM;
		}
	}
	ܐܢ(source){
		base_msg->source_listener = source->listener;
	}
	base_msg->origin = origin;
	ܐܢ(message_expiry_interval){
		ܐܢ(*message_expiry_interval > 0 && *message_expiry_interval != MSG_EXPIRY_INFINITE){
			base_msg->data.expiry_time = db.now_real_s + *message_expiry_interval;
		}ܐܚܪܢܐ{
			base_msg->data.expiry_time = 0;
		}
	}

	base_msg->dest_ids = عدم;
	base_msg->dest_id_count = 0;
	db.msg_store_count++;
	db.msg_store_bytes += base_msg->data.payloadlen;

	ܐܢ(!base_msg->data.store_id){
		base_msg->data.store_id = db__new_msg_id();
	}

	rc = db__msg_store_add(base_msg);
	ܐܢ(rc){
		db__msg_store_free(base_msg);
		ܦܢܐ rc;
	}

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح db__message_store_find(بنية mosquitto *context, uint16_t mid, بنية mosquitto__client_msg **client_msg)
{
	بنية mosquitto__client_msg *cmsg;

	*client_msg = عدم;

	ܐܢ(!context){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_in.inflight, cmsg){
		ܐܢ(cmsg->base_msg->data.source_mid == mid){
			*client_msg = cmsg;
			ܦܢܐ MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_in.queued, cmsg){
		ܐܢ(cmsg->base_msg->data.source_mid == mid){
			*client_msg = cmsg;
			ܦܢܐ MOSQ_ERR_SUCCESS;
		}
	}

	ܦܢܐ 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
ساكن صحيح db__message_reconnect_reset_outgoing(بنية mosquitto *context)
{
	بنية mosquitto__client_msg *client_msg, *tmp;

	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_count = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_count = 0;
	context->msgs_out.queued_count12 = 0;
	context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_msg, tmp){
		db__msg_add_to_inflight_stats(&context->msgs_out, client_msg);
		ܐܢ(client_msg->data.qos > 0){
			util__decrement_send_quota(context);
		}

		بدل(client_msg->data.qos){
			حالة 0:
				client_msg->data.state = mosq_ms_publish_qos0;
				اكسر;
			حالة 1:
				client_msg->data.state = mosq_ms_publish_qos1;
				اكسر;
			حالة 2:
				ܐܢ(client_msg->data.state == mosq_ms_wait_for_pubcomp){
					client_msg->data.state = mosq_ms_resend_pubrel;
				}ܐܚܪܢܐ{
					client_msg->data.state = mosq_ms_publish_qos2;
				}
				اكسر;
		}
		plugin_persist__handle_client_msg_update(context, client_msg);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_out.queued, client_msg, tmp){
		db__msg_add_to_queued_stats(&context->msgs_out, client_msg);
	}
	db__fill_inflight_out_from_queue(context);

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
ساكن صحيح db__message_reconnect_reset_incoming(بنية mosquitto *context)
{
	بنية mosquitto__client_msg *client_msg, *tmp;

	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_count = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_count = 0;
	context->msgs_in.queued_count12 = 0;
	context->msgs_in.inflight_quota = context->msgs_in.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_msg, tmp){
		db__msg_add_to_inflight_stats(&context->msgs_in, client_msg);
		ܐܢ(client_msg->data.qos > 0){
			util__decrement_receive_quota(context);
		}

		ܐܢ(client_msg->data.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db__message_remove_inflight(context, &context->msgs_in, client_msg);
		}ܐܚܪܢܐ{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_msg->data.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_in.queued, client_msg, tmp){
		client_msg->data.dup = 0;
		db__msg_add_to_queued_stats(&context->msgs_in, client_msg);
		ܐܢ(db__ready_for_flight(context, mosq_md_in, client_msg->data.qos)){
			بدل(client_msg->data.qos){
				حالة 0:
					client_msg->data.state = mosq_ms_publish_qos0;
					اكسر;
				حالة 1:
					client_msg->data.state = mosq_ms_publish_qos1;
					اكسر;
				حالة 2:
					client_msg->data.state = mosq_ms_publish_qos2;
					اكسر;
			}
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist__handle_client_msg_update(context, client_msg);
		}
	}

	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح db__message_reconnect_reset(بنية mosquitto *context)
{
	صحيح rc;

	rc = db__message_reconnect_reset_outgoing(context);
	ܐܢ(rc){
		ܦܢܐ rc;
	}
	ܦܢܐ db__message_reconnect_reset_incoming(context);
}


صحيح db__message_remove_incoming(بنية mosquitto *context, uint16_t mid)
{
	بنية mosquitto__client_msg *client_msg, *tmp;

	ܐܢ(!context){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_msg, tmp){
		ܐܢ(client_msg->data.mid == mid){
			ܐܢ(client_msg->base_msg->data.qos != 2){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_msg->base_msg->data.qos);
				ܦܢܐ MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_in, client_msg);
			ܦܢܐ MOSQ_ERR_SUCCESS;
		}
	}

	ܦܢܐ MOSQ_ERR_NOT_FOUND;
}


صحيح db__message_release_incoming(بنية mosquitto *context, uint16_t mid)
{
	بنية mosquitto__client_msg *client_msg, *tmp;
	bool deleted = false;
	صحيح rc;

	ܐܢ(!context){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_msg, tmp){
		ܐܢ(client_msg->data.mid == mid){
			ܐܢ(client_msg->base_msg->data.qos != 2){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_msg->base_msg->data.qos);
				ܦܢܐ MOSQ_ERR_PROTOCOL;
			}
			ثابت حرف *topic = client_msg->base_msg->data.topic;
			ثابت حرف *source_id = client_msg->base_msg->data.source_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			ܐܢ(topic == عدم){
				db__message_remove_inflight(context, &context->msgs_in, client_msg);
				deleted = true;
			}ܐܚܪܢܐ{
				rc = sub__messages_queue(source_id, 2, &client_msg->base_msg);
				ܐܢ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db__message_remove_inflight(context, &context->msgs_in, client_msg);
					deleted = true;
				}ܐܚܪܢܐ{
					ܦܢܐ 1;
				}
			}
		}
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_msg, tmp){
		ܐܢ(db__ready_for_flight(context, mosq_md_in, client_msg->data.qos)){
			اكسر;
		}

		ܐܢ(client_msg->data.qos == 2){
			send__pubrec(context, client_msg->data.mid, 0, عدم);
			client_msg->data.state = mosq_ms_wait_for_pubrel;
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist__handle_client_msg_update(context, client_msg);
		}
	}
	ܐܢ(deleted){
		ܦܢܐ MOSQ_ERR_SUCCESS;
	}ܐܚܪܢܐ{
		ܦܢܐ MOSQ_ERR_NOT_FOUND;
	}
}


فراغ db__expire_all_messages(بنية mosquitto *context)
{
	بنية mosquitto__client_msg *client_msg, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_msg, tmp){
		ܐܢ(client_msg->base_msg->data.expiry_time && db.now_real_s > client_msg->base_msg->data.expiry_time){
			ܐܢ(client_msg->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_msg);
		}
	}
	db__fill_inflight_out_from_queue(context);
	DL_FOREACH_SAFE(context->msgs_out.queued, client_msg, tmp){
		ܐܢ(client_msg->base_msg->data.expiry_time && db.now_real_s > client_msg->base_msg->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_msg);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.inflight, client_msg, tmp){
		ܐܢ(client_msg->base_msg->data.expiry_time && db.now_real_s > client_msg->base_msg->data.expiry_time){
			ܐܢ(client_msg->data.qos > 0){
				util__increment_receive_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_in, client_msg);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.queued, client_msg, tmp){
		ܐܢ(client_msg->base_msg->data.expiry_time && db.now_real_s > client_msg->base_msg->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_in, client_msg);
		}
	}
}


ساكن فراغ db__client_messages_check_acl(بنية mosquitto *context, بنية mosquitto_msg_data *msg_data, بنية mosquitto__client_msg **head,
		فراغ (*decrement_stats_fn)(بنية mosquitto_msg_data *msg_data, بنية mosquitto__client_msg *client_msg))
{
	بنية mosquitto__client_msg *client_msg, *tmp;
	بنية mosquitto__base_msg *base_msg;
	صحيح access;

	DL_FOREACH_SAFE((*head), client_msg, tmp){
		base_msg = client_msg->base_msg;
		ܐܢ(client_msg->data.direction == mosq_md_out){
			access = MOSQ_ACL_READ;
		}ܐܚܪܢܐ{
			access = MOSQ_ACL_WRITE;
		}
		ܐܢ(mosquitto_acl_check(context, base_msg->data.topic,
				base_msg->data.payloadlen, base_msg->data.payload,
				base_msg->data.qos, base_msg->data.retain,
				base_msg->data.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*head), client_msg);
			decrement_stats_fn(msg_data, client_msg);
			plugin_persist__handle_client_msg_delete(context, client_msg);
			db__msg_store_ref_dec(&client_msg->base_msg);
			mosquitto_FREE(client_msg);
		}
	}
}


فراغ db__check_acl_of_all_messages(بنية mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.inflight, &db__msg_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.queued, &db__msg_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.inflight, &db__msg_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.queued, &db__msg_remove_from_queued_stats);
}


ساكن صحيح db__message_write_inflight_out_single(بنية mosquitto *context, بنية mosquitto__client_msg *client_msg)
{
	بنية mosquitto__base_msg *base_msg;
	mosquitto_property *base_msg_props = عدم;
	صحيح rc;
	uint16_t mid;
	صحيح retries;
	صحيح retain;
	ثابت حرف *topic;
	uint8_t qos;
	uint32_t payloadlen;
	ثابت فراغ *payload;
	uint32_t expiry_interval;
	uint32_t subscription_id;

	base_msg = client_msg->base_msg;

	expiry_interval = 0;
	ܐܢ(base_msg->data.expiry_time){
		ܐܢ(db.now_real_s > base_msg->data.expiry_time){
			/* Message is expired, must not send. */
			ܐܢ(client_msg->data.direction == mosq_md_out && client_msg->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_msg);
			db__fill_inflight_out_from_queue(context);
			ܦܢܐ MOSQ_ERR_SUCCESS;
		}ܐܚܪܢܐ{
			expiry_interval = (uint32_t)(base_msg->data.expiry_time - db.now_real_s);
		}
	}
	mid = client_msg->data.mid;
	retries = client_msg->data.dup;
	retain = client_msg->data.retain;
	topic = base_msg->data.topic;
	qos = (uint8_t)client_msg->data.qos;
	payloadlen = base_msg->data.payloadlen;
	payload = base_msg->data.payload;
	subscription_id = client_msg->data.subscription_identifier;
	base_msg_props = base_msg->data.properties;

	بدل(client_msg->data.state){
		حالة mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_msg_props, expiry_interval);
			ܐܢ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_msg);
			}ܐܚܪܢܐ{
				ܦܢܐ rc;
			}
			اكسر;

		حالة mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_msg_props, expiry_interval);
			ܐܢ(rc == MOSQ_ERR_SUCCESS){
				client_msg->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_msg->data.state = mosq_ms_wait_for_puback;
				plugin_persist__handle_client_msg_update(context, client_msg);
			}ܐܚܪܢܐ ܐܢ(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_msg);
			}ܐܚܪܢܐ{
				ܦܢܐ rc;
			}
			اكسر;

		حالة mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_msg_props, expiry_interval);
			ܐܢ(rc == MOSQ_ERR_SUCCESS){
				client_msg->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_msg->data.state = mosq_ms_wait_for_pubrec;
				plugin_persist__handle_client_msg_update(context, client_msg);
			}ܐܚܪܢܐ ܐܢ(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_msg);
			}ܐܚܪܢܐ{
				ܦܢܐ rc;
			}
			اكسر;

		حالة mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, عدم);
			ܐܢ(!rc){
				client_msg->data.state = mosq_ms_wait_for_pubcomp;
				plugin_persist__handle_client_msg_update(context, client_msg);
			}ܐܚܪܢܐ{
				ܦܢܐ rc;
			}
			اكسر;

		حالة mosq_ms_invalid:
		حالة mosq_ms_send_pubrec:
		حالة mosq_ms_resend_pubcomp:
		حالة mosq_ms_wait_for_puback:
		حالة mosq_ms_wait_for_pubrec:
		حالة mosq_ms_wait_for_pubrel:
		حالة mosq_ms_wait_for_pubcomp:
		حالة mosq_ms_queued:
			اكسر;
	}
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح db__message_write_inflight_out_all(بنية mosquitto *context)
{
	بنية mosquitto__client_msg *client_msg, *tmp;
	صحيح rc;

	ܐܢ(context->state != mosq_cs_active || !net__is_connected(context)){
		ܦܢܐ MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_msg, tmp){
		rc = db__message_write_inflight_out_single(context, client_msg);
		ܐܢ(rc){
			ܦܢܐ rc;
		}
	}
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح db__message_write_inflight_out_latest(بنية mosquitto *context)
{
	بنية mosquitto__client_msg *client_msg, *next;
	صحيح rc;

	ܐܢ(context->state != mosq_cs_active
			|| !net__is_connected(context)
			|| context->msgs_out.inflight == عدم){

		ܦܢܐ MOSQ_ERR_SUCCESS;
	}

	ܐܢ(context->msgs_out.inflight->prev == context->msgs_out.inflight){
		/* Only one message */
		ܦܢܐ db__message_write_inflight_out_single(context, context->msgs_out.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_msg = context->msgs_out.inflight->prev;
	ܟܡܐ(client_msg != context->msgs_out.inflight &&
			(client_msg->data.state == mosq_ms_publish_qos0
			|| client_msg->data.state == mosq_ms_publish_qos1
			|| client_msg->data.state == mosq_ms_publish_qos2)){

		client_msg = client_msg->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	ܐܢ(client_msg != context->msgs_out.inflight){
		client_msg = client_msg->next;
	}

	ܟܡܐ(client_msg){
		next = client_msg->next;
		rc = db__message_write_inflight_out_single(context, client_msg);
		ܐܢ(rc){
			ܦܢܐ rc;
		}
		client_msg = next;
	}
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح db__message_write_queued_in(بنية mosquitto *context)
{
	بنية mosquitto__client_msg *client_msg, *tmp;
	صحيح rc;

	ܐܢ(context->state != mosq_cs_active){
		ܦܢܐ MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_msg, tmp){
		ܐܢ(context->msgs_in.inflight_maximum != 0 && context->msgs_in.inflight_quota == 0){
			اكسر;
		}

		ܐܢ(client_msg->data.qos == 2){
			client_msg->data.state = mosq_ms_send_pubrec;
			db__message_dequeue_first(context, &context->msgs_in);
			rc = send__pubrec(context, client_msg->data.mid, 0, عدم);
			ܐܢ(!rc){
				client_msg->data.state = mosq_ms_wait_for_pubrel;
				plugin_persist__handle_client_msg_update(context, client_msg);
			}ܐܚܪܢܐ{
				plugin_persist__handle_client_msg_update(context, client_msg);
				ܦܢܐ rc;
			}
		}
	}
	ܦܢܐ MOSQ_ERR_SUCCESS;
}


صحيح db__message_write_queued_out(بنية mosquitto *context)
{
	ܐܢ(context->state != mosq_cs_active){
		ܦܢܐ MOSQ_ERR_SUCCESS;
	}

	db__fill_inflight_out_from_queue(context);

	ܦܢܐ MOSQ_ERR_SUCCESS;
}
