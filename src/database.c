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
bool db__ready_for_flight(بنية mosquitto *context, تعداد mosquitto_سكو_direction dir, صحيح qos)
{
	بنية mosquitto_سكو_data *msgs;
	bool valid_bytes;
	bool valid_كدي;

	إذا(dir == mosq_md_out){
		msgs = &context->msgs_out;
	}وإلا{
		msgs = &context->msgs_in;
	}

	إذا(msgs->inflight_maximum == 0 && db.config->max_inflight_bytes == 0){
		أرجع true;
	}

	إذا(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		إذا(db.config->max_queued_messages == 0 && db.config->max_inflight_bytes == 0){
			أرجع true;
		}
		valid_bytes = ((msgs->inflight_bytes - (ssize_t)db.config->max_inflight_bytes) < (ssize_t)db.config->max_queued_bytes);
		إذا(dir == mosq_md_out){
			valid_كدي = context->out_packet_كدي < db.config->max_queued_messages;
		}وإلا{
			valid_كدي = msgs->inflight_كدي - msgs->inflight_maximum < db.config->max_queued_messages;
		}

		إذا(db.config->max_queued_messages == 0){
			أرجع valid_bytes;
		}
		إذا(db.config->max_queued_bytes == 0){
			أرجع valid_كدي;
		}
	}وإلا{
		valid_bytes = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_bytes;
		valid_كدي = msgs->inflight_quota > 0;

		إذا(msgs->inflight_maximum == 0){
			أرجع valid_bytes;
		}
		إذا(db.config->max_inflight_bytes == 0){
			أرجع valid_كدي;
		}
	}

	أرجع valid_bytes && valid_كدي;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db__ready_for_queue(بنية mosquitto *context, صحيح qos, بنية mosquitto_سكو_data *سكو_data)
{
	صحيح source_كدي;
	صحيح adjust_كدي;
	long source_bytes;
	ssize_t adjust_bytes = (ssize_t)db.config->max_inflight_bytes;
	bool valid_bytes;
	bool valid_كدي;

	إذا(db.config->max_queued_messages == 0 && db.config->max_queued_bytes == 0){
		أرجع true;
	}

	إذا(qos == 0 && db.config->queue_qos0_messages == false){
		أرجع false; /* This case is handled in db__ready_for_flight() */
	}وإلا{
		source_bytes = (ssize_t)سكو_data->queued_bytes12;
		source_كدي = سكو_data->queued_count12;
	}
	adjust_كدي = سكو_data->inflight_maximum;

	/* nothing in flight for offline clients */
	إذا(!net__is_connected(context)){
		adjust_bytes = 0;
		adjust_كدي = 0;
	}

	valid_bytes = (source_bytes - (ssize_t)adjust_bytes) < (ssize_t)db.config->max_queued_bytes;
	valid_كدي = source_كدي - adjust_كدي < db.config->max_queued_messages;

	إذا(db.config->max_queued_bytes == 0){
		أرجع valid_كدي;
	}
	إذا(db.config->max_queued_messages == 0){
		أرجع valid_bytes;
	}

	أرجع valid_bytes && valid_كدي;
}


فراغ db_سكو_add_to_inflight_stats(بنية mosquitto_سكو_data *سكو_data, بنية mosquitto_client_سكو *client_سكو)
{
	سكو_data->inflight_كدي++;
	سكو_data->inflight_bytes += client_سكو->base_سكو->data.payloadlen;
	إذا(client_سكو->data.qos != 0){
		سكو_data->inflight_count12++;
		سكو_data->inflight_bytes12 += client_سكو->base_سكو->data.payloadlen;
	}
}


ساكن فراغ db_سكو_remove_from_inflight_stats(بنية mosquitto_سكو_data *سكو_data, بنية mosquitto_client_سكو *client_سكو)
{
	سكو_data->inflight_كدي--;
	سكو_data->inflight_bytes -= client_سكو->base_سكو->data.payloadlen;
	إذا(client_سكو->data.qos != 0){
		سكو_data->inflight_count12--;
		سكو_data->inflight_bytes12 -= client_سكو->base_سكو->data.payloadlen;
	}
}


فراغ db_سكو_add_to_queued_stats(بنية mosquitto_سكو_data *سكو_data, بنية mosquitto_client_سكو *client_سكو)
{
	سكو_data->queued_كدي++;
	سكو_data->queued_bytes += client_سكو->base_سكو->data.payloadlen;
	إذا(client_سكو->data.qos != 0){
		سكو_data->queued_count12++;
		سكو_data->queued_bytes12 += client_سكو->base_سكو->data.payloadlen;
	}
}


ساكن فراغ db_سكو_remove_from_queued_stats(بنية mosquitto_سكو_data *سكو_data, بنية mosquitto_client_سكو *client_سكو)
{
	سكو_data->queued_كدي--;
	سكو_data->queued_bytes -= client_سكو->base_سكو->data.payloadlen;
	إذا(client_سكو->data.qos != 0){
		سكو_data->queued_count12--;
		سكو_data->queued_bytes12 -= client_سكو->base_سكو->data.payloadlen;
	}
}


صحيح db__open(بنية mosquitto__config *config)
{
	إذا(!config){
		أرجع MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = عدم;
	db.contexts_by_sock = عدم;
	db.contexts_for_free = عدم;
#ifdef WITH_BRIDGE
	db.bridges = عدم;
	db.bridge_كدي = 0;
#endif

	/* Initialize the hashtable */
	db.clientid_index_hash = عدم;

	db.normal_subs = عدم;
	db.shared_subs = عدم;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = عدم;

#ifdef WITH_PERSISTENCE
	إذا(persist__restore()){
		أرجع 1;
	}
#endif

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن فراغ subhier_clean(بنية mosquitto__subhier **subhier)
{
	بنية mosquitto__subhier *peer, *subhier_tmp;
	بنية mosquitto__subleaf *leaf, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		leaf = peer->subs;
		طالما(leaf){
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
	db_سكو_store_clean();

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_سكو_store_add(بنية mosquitto_base_سكو *base_سكو)
{
	بنية mosquitto_base_سكو *found;
	unsigned hashv;

	HASH_درج(&base_سكو->data.store_id, حجم(base_سكو->data.store_id), hashv);
	HASH_FIND_BYHASHVALUE(hh, db.سكو_store, &base_سكو->data.store_id, حجم(base_سكو->data.store_id), hashv, found);
	إذا(found == عدم){
		HASH_ADD_KEYPTR_BYHASHVALUE(hh, db.سكو_store, &base_سكو->data.store_id, حجم(base_سكو->data.store_id), hashv, base_سكو);
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_ALREADY_EXISTS;
	}
}


فراغ db_سكو_store_free(بنية mosquitto_base_سكو *base_سكو)
{
	mosquitto_FREE(base_سكو->data.source_id);
	mosquitto_FREE(base_سكو->data.source_username);
	إذا(base_سكو->dest_ids){
		لكل(صحيح i=0; i<base_سكو->dest_id_كدي; i++){
			mosquitto_FREE(base_سكو->dest_ids[i]);
		}
		mosquitto_FREE(base_سكو->dest_ids);
	}
	mosquitto_FREE(base_سكو->data.topic);
	mosquitto_property_free_all(&base_سكو->data.properties);
	mosquitto_FREE(base_سكو->data.payload);
	mosquitto_FREE(base_سكو);
}


فراغ db_سكو_store_remove(بنية mosquitto_base_سكو *base_سكو, bool notify)
{
	إذا(base_سكو == عدم){
		أرجع;
	}
	HASH_DELETE(hh, db.سكو_store, base_سكو);
	db.سكو_store_كدي--;
	db.سكو_store_bytes -= base_سكو->data.payloadlen;
	إذا(notify == true){
		plugin_persist_handle_base_سكو_delete(base_سكو);
	}
	db_سكو_store_free(base_سكو);
}


فراغ db_سكو_store_clean(فراغ)
{
	بنية mosquitto_base_سكو *base_سكو, *base_سكو_tmp;

	HASH_ITER(hh, db.سكو_store, base_سكو, base_سكو_tmp){
		db_سكو_store_remove(base_سكو, false);
	}
}


فراغ db_سكو_store_ref_inc(بنية mosquitto_base_سكو *base_سكو)
{
	base_سكو->ref_كدي++;
}


فراغ db_سكو_store_ref_dec(بنية mosquitto_base_سكو **base_سكو)
{
	(*base_سكو)->ref_كدي--;
	إذا((*base_سكو)->ref_كدي == 0){
		db_سكو_store_remove(*base_سكو, true);
		*base_سكو = عدم;
	}
}


فراغ db_سكو_store_compact(فراغ)
{
	بنية mosquitto_base_سكو *base_سكو, *base_سكو_tmp;

	HASH_ITER(hh, db.سكو_store, base_سكو, base_سكو_tmp){
		إذا(base_سكو->ref_كدي < 1){
			db_سكو_store_remove(base_سكو, true);
		}
	}
}


ساكن فراغ db__message_remove_inflight(بنية mosquitto *context, بنية mosquitto_سكو_data *سكو_data, بنية mosquitto_client_سكو *item)
{
	إذا(!context || !سكو_data || !item){
		أرجع;
	}

	plugin_persist_handle_client_سكو_delete(context, item);

	DL_DELETE(سكو_data->inflight, item);
	إذا(item->base_سكو){
		db_سكو_remove_from_inflight_stats(سكو_data, item);
		db_سكو_store_ref_dec(&item->base_سكو);
	}

	mosquitto_FREE(item);
}


ساكن فراغ db__message_remove_queued(بنية mosquitto *context, بنية mosquitto_سكو_data *سكو_data, بنية mosquitto_client_سكو *item)
{
	إذا(!context || !سكو_data || !item){
		أرجع;
	}

	plugin_persist_handle_client_سكو_delete(context, item);

	DL_DELETE(سكو_data->queued, item);
	إذا(item->base_سكو){
		db_سكو_remove_from_queued_stats(سكو_data, item);
		db_سكو_store_ref_dec(&item->base_سكو);
	}

	mosquitto_FREE(item);
}


ساكن فراغ db__fill_inflight_out_from_queue(بنية mosquitto *context)
{
	بنية mosquitto_client_سكو *client_سكو, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.queued, client_سكو, tmp){
		إذا(!db__ready_for_flight(context, mosq_md_out, client_سكو->data.qos)){
			أرجع;
		}
		بدل(client_سكو->data.qos){
			حالة 0:
				client_سكو->data.state = mosq_ms_publish_qos0;
				اكسر;
			حالة 1:
				client_سكو->data.state = mosq_ms_publish_qos1;
				اكسر;
			حالة 2:
				client_سكو->data.state = mosq_ms_publish_qos2;
				اكسر;
		}
		إذا(client_سكو->base_سكو->data.expiry_time && db.now_real_s > client_سكو->base_سكو->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_سكو);
			استمر;
		}
		plugin_persist_handle_client_سكو_update(context, client_سكو);
		db__message_dequeue_first(context, &context->msgs_out);
	}
}


فراغ db__message_dequeue_first(بنية mosquitto *context, بنية mosquitto_سكو_data *سكو_data)
{
	بنية mosquitto_client_سكو *client_سكو;

	UNUSED(context);

	client_سكو = سكو_data->queued;
	DL_DELETE(سكو_data->queued, client_سكو);
	DL_APPEND(سكو_data->inflight, client_سكو);
	إذا(سكو_data->inflight_quota > 0){
		سكو_data->inflight_quota--;
	}

	db_سكو_remove_from_queued_stats(سكو_data, client_سكو);
	db_سكو_add_to_inflight_stats(سكو_data, client_سكو);
}


صحيح db__message_delete_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_سكو_state expect_state, صحيح qos)
{
	بنية mosquitto_client_سكو *client_سكو, *tmp;
	bool deleted = false;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_سكو, tmp){
		إذا(client_سكو->data.mid == mid){
			إذا(client_سكو->data.qos != qos){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_سكو->data.qos, qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}وإلا إذا(qos == 2 && client_سكو->data.state != expect_state && expect_state != mosq_ms_any){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_سكو->data.state, expect_state);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_out, client_سكو);
			deleted = true;
			اكسر;
		}
	}

	إذا(deleted == false){
		DL_FOREACH_SAFE(context->msgs_out.queued, client_سكو, tmp){
			إذا(client_سكو->data.mid == mid){
				إذا(client_سكو->data.qos != qos){
					أرجع MOSQ_ERR_PROTOCOL;
				}وإلا إذا(qos == 2 && client_سكو->data.state != expect_state && expect_state != mosq_ms_any){
					أرجع MOSQ_ERR_PROTOCOL;
				}
				db__message_remove_queued(context, &context->msgs_out, client_سكو);
				اكسر;
			}
		}
	}
	db__fill_inflight_out_from_queue(context);
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	أرجع db__message_write_inflight_out_latest(context);
}


/* Only for QoS 2 messages */
صحيح db_message_سك_incoming(بنية mosquitto *context, uint64_t cmsg_id, بنية mosquitto_base_سكو *base_سكو, bool persist)
{
	بنية mosquitto_client_سكو *client_سكو;
	بنية mosquitto_سكو_data *سكو_data;
	تعداد mosquitto_سكو_state state = mosq_ms_invalid;
	صحيح rc = 0;

	أكد(base_سكو);
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		أرجع MOSQ_ERR_SUCCESS;

	}
	سكو_data = &context->msgs_in;

	إذا(db__ready_for_flight(context, mosq_md_in, base_سكو->data.qos)){
		state = mosq_ms_wait_for_pubrel;
	}وإلا إذا(base_سكو->data.qos != 0 && db__ready_for_queue(context, base_سكو->data.qos, سكو_data)){
		state = mosq_ms_queued;
		rc = 2;
	}وإلا{
		/* Dropping message due to full queue. */
		إذا(context->is_dropping == false){
			context->is_dropping = true;
			log__printf(عدم, MOSQ_LOG_NOTICE,
					"Outgoing messages are being dropped for client %s.",
					context->id);
		}
		metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		أرجع 2;
	}

	أكد(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	إذا(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_سكو = mosquitto_malloc(حجم(بنية mosquitto_client_سكو));
	إذا(!client_سكو){
		أرجع MOSQ_ERR_NOMEM;
	}
	client_سكو->prev = عدم;
	client_سكو->next = عدم;
	إذا(cmsg_id){
		client_سكو->data.cmsg_id = cmsg_id;
	}وإلا{
		client_سكو->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_سكو->base_سكو = base_سكو;
	db_سكو_store_ref_inc(client_سكو->base_سكو);
	client_سكو->data.mid = base_سكو->data.source_mid;
	client_سكو->data.direction = mosq_md_in;
	client_سكو->data.state = (uint8_t)state;
	client_سكو->data.dup = false;
	إذا(base_سكو->data.qos > context->max_qos){
		client_سكو->data.qos = context->max_qos;
	}وإلا{
		client_سكو->data.qos = base_سكو->data.qos;
	}
	client_سكو->data.retain = base_سكو->data.retain;
	client_سكو->data.subscription_identifier = 0;

	إذا(state == mosq_ms_queued){
		DL_APPEND(سكو_data->queued, client_سكو);
		db_سكو_add_to_queued_stats(سكو_data, client_سكو);
	}وإلا{
		DL_APPEND(سكو_data->inflight, client_سكو);
		db_سكو_add_to_inflight_stats(سكو_data, client_سكو);
	}

	إذا(persist && context->is_persisted){
		plugin_persist_handle_base_سكو_add(client_سكو->base_سكو);
		plugin_persist_handle_client_سكو_add(context, client_سكو);
	}

	إذا(client_سكو->base_سكو->data.qos > 0){
		util__decrement_receive_quota(context);
	}
	أرجع rc;
}


صحيح db_message_سك_outgoing(بنية mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, بنية mosquitto_base_سكو *base_سكو, uint32_t subscription_identifier, bool update, bool persist)
{
	بنية mosquitto_client_سكو *client_سكو;
	بنية mosquitto_سكو_data *سكو_data;
	تعداد mosquitto_سكو_state state = mosq_ms_invalid;
	صحيح rc = 0;
	حرف **dest_ids;

	أكد(base_سكو);
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		أرجع MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_sent++;

	سكو_data = &context->msgs_out;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	إذا(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && base_سكو->dest_ids){

		لكل(صحيح i=0; i<base_سكو->dest_id_كدي; i++){
			إذا(base_سكو->dest_ids[i] && !strcmp(base_سكو->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				أرجع MOSQ_ERR_SUCCESS;
			}
		}
	}
	إذا(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		إذا(qos == 0 && !db.config->queue_qos0_messages){
			إذا(!context->bridge){
				أرجع 2;
			}وإلا{
				إذا(context->bridge->start_type != bst_lazy){
					أرجع 2;
				}
			}
		}
		إذا(context->bridge && context->bridge->clean_start_local == true){
			أرجع 2;
		}
	}

	إذا(net__is_connected(context)){
		إذا(db__ready_for_flight(context, mosq_md_out, qos)){
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
		}وإلا إذا(qos != 0 && db__ready_for_queue(context, qos, سكو_data)){
			state = mosq_ms_queued;
			rc = 2;
		}وإلا{
			/* Dropping message due to full queue. */
			إذا(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(عدم, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			أرجع 2;
		}
	}وإلا{
		إذا(db__ready_for_queue(context, qos, سكو_data)){
			state = mosq_ms_queued;
		}وإلا{
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			إذا(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(عدم, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			أرجع 2;
		}
	}
	أكد(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	إذا(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_سكو = mosquitto_malloc(حجم(بنية mosquitto_client_سكو));
	إذا(!client_سكو){
		أرجع MOSQ_ERR_NOMEM;
	}
	client_سكو->prev = عدم;
	client_سكو->next = عدم;
	إذا(cmsg_id){
		client_سكو->data.cmsg_id = cmsg_id;
	}وإلا{
		client_سكو->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_سكو->base_سكو = base_سكو;
	db_سكو_store_ref_inc(client_سكو->base_سكو);
	client_سكو->data.mid = mid;
	client_سكو->data.direction = mosq_md_out;
	client_سكو->data.state = (uint8_t)state;
	client_سكو->data.dup = false;
	إذا(qos > context->max_qos){
		client_سكو->data.qos = context->max_qos;
	}وإلا{
		client_سكو->data.qos = qos;
	}
	client_سكو->data.retain = retain;
	client_سكو->data.subscription_identifier = subscription_identifier;

	إذا(state == mosq_ms_queued){
		DL_APPEND(سكو_data->queued, client_سكو);
		db_سكو_add_to_queued_stats(سكو_data, client_سكو);
	}وإلا{
		DL_APPEND(سكو_data->inflight, client_سكو);
		db_سكو_add_to_inflight_stats(سكو_data, client_سكو);
	}

	إذا(persist && context->is_persisted){
		plugin_persist_handle_base_سكو_add(client_سكو->base_سكو);
		plugin_persist_handle_client_سكو_add(context, client_سكو);
	}

	إذا(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(base_سكو->dest_ids, حجم(حرف *)*(size_t)(base_سكو->dest_id_كدي+1));
		إذا(dest_ids){
			base_سكو->dest_ids = dest_ids;
			base_سكو->dest_id_كدي++;
			base_سكو->dest_ids[base_سكو->dest_id_كدي-1] = mosquitto_strdup(context->id);
			إذا(!base_سكو->dest_ids[base_سكو->dest_id_كدي-1]){
				أرجع MOSQ_ERR_NOMEM;
			}
		}وإلا{
			أرجع MOSQ_ERR_NOMEM;
		}
	}
#ifdef WITH_BRIDGE
	إذا(context->bridge && context->bridge->start_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_out.inflight_كدي + context->msgs_out.queued_كدي >= context->bridge->threshold){

		context->bridge->lazy_reconnect = true;
	}
#endif

	إذا(client_سكو->data.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	إذا(update){
		rc = db__message_write_inflight_out_latest(context);
		إذا(rc){
			أرجع rc;
		}
		rc = db__message_write_queued_out(context);
		إذا(rc){
			أرجع rc;
		}
	}

	أرجع rc;
}


ساكن مضمن صحيح db__message_update_outgoing_state(بنية mosquitto *context, بنية mosquitto_client_سكو *head,
		uint16_t mid, تعداد mosquitto_سكو_state state, صحيح qos, bool persist)
{
	بنية mosquitto_client_سكو *client_سكو;

	DL_FOREACH(head, client_سكو){
		إذا(client_سكو->data.mid == mid){
			إذا(client_سكو->data.qos != qos){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_سكو->data.qos, qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			client_سكو->data.state = (uint8_t)state;
			إذا(persist){
				plugin_persist_handle_client_سكو_update(context, client_سكو);
			}
			أرجع MOSQ_ERR_SUCCESS;
		}
	}
	أرجع MOSQ_ERR_NOT_FOUND;
}


صحيح db__message_update_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_سكو_state state, صحيح qos, bool persist)
{
	صحيح rc;

	rc = db__message_update_outgoing_state(context, context->msgs_out.inflight, mid, state, qos, persist);
	إذا(!persist && rc == MOSQ_ERR_NOT_FOUND){
		rc = db__message_update_outgoing_state(context, context->msgs_out.queued, mid, state, qos, persist);
	}
	أرجع rc;
}


ساكن فراغ db__messages_delete_list(بنية mosquitto_client_سكو **head)
{
	بنية mosquitto_client_سكو *client_سكو, *tmp;

	DL_FOREACH_SAFE(*head, client_سكو, tmp){
		DL_DELETE(*head, client_سكو);
		db_سكو_store_ref_dec(&client_سكو->base_سكو);
		mosquitto_FREE(client_سكو);
	}
	*head = عدم;
}


صحيح db__messages_delete_incoming(بنية mosquitto *context)
{
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_in.inflight);
	db__messages_delete_list(&context->msgs_in.queued);
	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_كدي = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_كدي = 0;
	context->msgs_in.queued_count12 = 0;

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__messages_delete_outgoing(بنية mosquitto *context)
{
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_out.inflight);
	db__messages_delete_list(&context->msgs_out.queued);
	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_كدي = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_كدي = 0;
	context->msgs_out.queued_count12 = 0;

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__messages_delete(بنية mosquitto *context, bool force_free)
{
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(force_free || context->clean_start || (context->bridge && context->bridge->clean_start)){
		db__messages_delete_incoming(context);
	}

	إذا(force_free || (context->bridge && context->bridge->clean_start_local)
			|| (context->bridge == عدم && context->clean_start)){

		db__messages_delete_outgoing(context);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__messages_easy_queue(بنية mosquitto *context, ثابت حرف *topic, uint8_t qos, uint32_t payloadlen, ثابت فراغ *payload, صحيح retain, uint32_t message_expiry_interval, mosquitto_property **properties)
{
	بنية mosquitto_base_سكو *base_سكو;
	ثابت حرف *source_id;
	تعداد mosquitto_سكو_origin origin;

	إذا(!topic){
		أرجع MOSQ_ERR_INVAL;
	}

	base_سكو = mosquitto_calloc(1, حجم(بنية mosquitto_base_سكو));
	إذا(base_سكو == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	base_سكو->data.topic = mosquitto_strdup(topic);
	إذا(base_سكو->data.topic == عدم){
		db_سكو_store_free(base_سكو);
		أرجع MOSQ_ERR_INVAL;
	}

	base_سكو->data.qos = qos;
	إذا(db.config->retain_available == false){
		base_سكو->data.retain = 0;
	}وإلا{
		base_سكو->data.retain = retain;
	}

	base_سكو->data.payloadlen = payloadlen;
	إذا(payloadlen > 0){
		base_سكو->data.payload = mosquitto_malloc(base_سكو->data.payloadlen+1);
		إذا(base_سكو->data.payload == عدم){
			db_سكو_store_free(base_سكو);
			أرجع MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_سكو->data.payload)[base_سكو->data.payloadlen] = 0;
		memcpy(base_سكو->data.payload, payload, base_سكو->data.payloadlen);
	}

	إذا(context && context->id){
		source_id = context->id;
	}وإلا{
		source_id = "";
	}
	إذا(properties){
		base_سكو->data.properties = *properties;
		*properties = عدم;
	}

	إذا(context){
		origin = mosq_mo_client;
	}وإلا{
		origin = mosq_mo_broker;
	}
	إذا(db__message_store(context, base_سكو, &message_expiry_interval, origin)){
		أرجع 1;
	}

	أرجع sub__messages_queue(source_id, base_سكو->data.qos, &base_سكو);
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
uint64_t db_new_سكو_id(فراغ)
{
#ifdef WIN32
	FILETIME ftime;
	uint64_t ftime64;
#وإلا
	بنية timespec ts;
#endif
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	long nsec;

	id = db.كلي_id_shifted; /* Top 10-bits */

#ifdef WIN32
	GetSystemTimePreciseAsFileTime(&ftime);
	ftime64 = (((uint64_t)ftime.dw_سم_Date_Time)<<32) + ftime.dw_كس_Date_Time;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (long)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#وإلا
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#endif
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	إذا(id <= db.last_db_id){
		id = db.last_db_id + 1;
	}
	db.last_db_id = id;

	أرجع id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
صحيح db__message_store(ثابت بنية mosquitto *source, بنية mosquitto_base_سكو *base_سكو, uint32_t *message_expiry_interval, تعداد mosquitto_سكو_origin origin)
{
	صحيح rc;

	أكد(base_سكو);

	إذا(source && source->id){
		base_سكو->data.source_id = mosquitto_strdup(source->id);
	}وإلا{
		base_سكو->data.source_id = mosquitto_strdup("");
	}
	إذا(!base_سكو->data.source_id){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_سكو_store_free(base_سكو);
		أرجع MOSQ_ERR_NOMEM;
	}

	إذا(source && source->username){
		base_سكو->data.source_username = mosquitto_strdup(source->username);
		إذا(!base_سكو->data.source_username){
			db_سكو_store_free(base_سكو);
			أرجع MOSQ_ERR_NOMEM;
		}
	}
	إذا(source){
		base_سكو->source_listener = source->listener;
	}
	base_سكو->origin = origin;
	إذا(message_expiry_interval){
		إذا(*message_expiry_interval > 0 && *message_expiry_interval != سكو_EXPIRY_INFINITE){
			base_سكو->data.expiry_time = db.now_real_s + *message_expiry_interval;
		}وإلا{
			base_سكو->data.expiry_time = 0;
		}
	}

	base_سكو->dest_ids = عدم;
	base_سكو->dest_id_كدي = 0;
	db.سكو_store_كدي++;
	db.سكو_store_bytes += base_سكو->data.payloadlen;

	إذا(!base_سكو->data.store_id){
		base_سكو->data.store_id = db_new_سكو_id();
	}

	rc = db_سكو_store_add(base_سكو);
	إذا(rc){
		db_سكو_store_free(base_سكو);
		أرجع rc;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__message_store_find(بنية mosquitto *context, uint16_t mid, بنية mosquitto_client_سكو **client_سكو)
{
	بنية mosquitto_client_سكو *cmsg;

	*client_سكو = عدم;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_in.inflight, cmsg){
		إذا(cmsg->base_سكو->data.source_mid == mid){
			*client_سكو = cmsg;
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_in.queued, cmsg){
		إذا(cmsg->base_سكو->data.source_mid == mid){
			*client_سكو = cmsg;
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	أرجع 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
ساكن صحيح db__message_reconnect_reset_outgoing(بنية mosquitto *context)
{
	بنية mosquitto_client_سكو *client_سكو, *tmp;

	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_كدي = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_كدي = 0;
	context->msgs_out.queued_count12 = 0;
	context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_سكو, tmp){
		db_سكو_add_to_inflight_stats(&context->msgs_out, client_سكو);
		إذا(client_سكو->data.qos > 0){
			util__decrement_send_quota(context);
		}

		بدل(client_سكو->data.qos){
			حالة 0:
				client_سكو->data.state = mosq_ms_publish_qos0;
				اكسر;
			حالة 1:
				client_سكو->data.state = mosq_ms_publish_qos1;
				اكسر;
			حالة 2:
				إذا(client_سكو->data.state == mosq_ms_wait_for_pubcomp){
					client_سكو->data.state = mosq_ms_resend_pubrel;
				}وإلا{
					client_سكو->data.state = mosq_ms_publish_qos2;
				}
				اكسر;
		}
		plugin_persist_handle_client_سكو_update(context, client_سكو);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_out.queued, client_سكو, tmp){
		db_سكو_add_to_queued_stats(&context->msgs_out, client_سكو);
	}
	db__fill_inflight_out_from_queue(context);

	أرجع MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
ساكن صحيح db__message_reconnect_reset_incoming(بنية mosquitto *context)
{
	بنية mosquitto_client_سكو *client_سكو, *tmp;

	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_كدي = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_كدي = 0;
	context->msgs_in.queued_count12 = 0;
	context->msgs_in.inflight_quota = context->msgs_in.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_سكو, tmp){
		db_سكو_add_to_inflight_stats(&context->msgs_in, client_سكو);
		إذا(client_سكو->data.qos > 0){
			util__decrement_receive_quota(context);
		}

		إذا(client_سكو->data.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db__message_remove_inflight(context, &context->msgs_in, client_سكو);
		}وإلا{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_سكو->data.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_in.queued, client_سكو, tmp){
		client_سكو->data.dup = 0;
		db_سكو_add_to_queued_stats(&context->msgs_in, client_سكو);
		إذا(db__ready_for_flight(context, mosq_md_in, client_سكو->data.qos)){
			بدل(client_سكو->data.qos){
				حالة 0:
					client_سكو->data.state = mosq_ms_publish_qos0;
					اكسر;
				حالة 1:
					client_سكو->data.state = mosq_ms_publish_qos1;
					اكسر;
				حالة 2:
					client_سكو->data.state = mosq_ms_publish_qos2;
					اكسر;
			}
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_سكو_update(context, client_سكو);
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__message_reconnect_reset(بنية mosquitto *context)
{
	صحيح rc;

	rc = db__message_reconnect_reset_outgoing(context);
	إذا(rc){
		أرجع rc;
	}
	أرجع db__message_reconnect_reset_incoming(context);
}


صحيح db__message_remove_incoming(بنية mosquitto *context, uint16_t mid)
{
	بنية mosquitto_client_سكو *client_سكو, *tmp;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_سكو, tmp){
		إذا(client_سكو->data.mid == mid){
			إذا(client_سكو->base_سكو->data.qos != 2){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_سكو->base_سكو->data.qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_in, client_سكو);
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	أرجع MOSQ_ERR_NOT_FOUND;
}


صحيح db__message_release_incoming(بنية mosquitto *context, uint16_t mid)
{
	بنية mosquitto_client_سكو *client_سكو, *tmp;
	bool deleted = false;
	صحيح rc;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_سكو, tmp){
		إذا(client_سكو->data.mid == mid){
			إذا(client_سكو->base_سكو->data.qos != 2){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_سكو->base_سكو->data.qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			ثابت حرف *topic = client_سكو->base_سكو->data.topic;
			ثابت حرف *source_id = client_سكو->base_سكو->data.source_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			إذا(topic == عدم){
				db__message_remove_inflight(context, &context->msgs_in, client_سكو);
				deleted = true;
			}وإلا{
				rc = sub__messages_queue(source_id, 2, &client_سكو->base_سكو);
				إذا(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db__message_remove_inflight(context, &context->msgs_in, client_سكو);
					deleted = true;
				}وإلا{
					أرجع 1;
				}
			}
		}
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_سكو, tmp){
		إذا(db__ready_for_flight(context, mosq_md_in, client_سكو->data.qos)){
			اكسر;
		}

		إذا(client_سكو->data.qos == 2){
			send__pubrec(context, client_سكو->data.mid, 0, عدم);
			client_سكو->data.state = mosq_ms_wait_for_pubrel;
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_سكو_update(context, client_سكو);
		}
	}
	إذا(deleted){
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_NOT_FOUND;
	}
}


فراغ db__expire_all_messages(بنية mosquitto *context)
{
	بنية mosquitto_client_سكو *client_سكو, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_سكو, tmp){
		إذا(client_سكو->base_سكو->data.expiry_time && db.now_real_s > client_سكو->base_سكو->data.expiry_time){
			إذا(client_سكو->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_سكو);
		}
	}
	db__fill_inflight_out_from_queue(context);
	DL_FOREACH_SAFE(context->msgs_out.queued, client_سكو, tmp){
		إذا(client_سكو->base_سكو->data.expiry_time && db.now_real_s > client_سكو->base_سكو->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_سكو);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.inflight, client_سكو, tmp){
		إذا(client_سكو->base_سكو->data.expiry_time && db.now_real_s > client_سكو->base_سكو->data.expiry_time){
			إذا(client_سكو->data.qos > 0){
				util__increment_receive_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_in, client_سكو);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.queued, client_سكو, tmp){
		إذا(client_سكو->base_سكو->data.expiry_time && db.now_real_s > client_سكو->base_سكو->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_in, client_سكو);
		}
	}
}


ساكن فراغ db__client_messages_check_acl(بنية mosquitto *context, بنية mosquitto_سكو_data *سكو_data, بنية mosquitto_client_سكو **head,
		فراغ (*decrement_stats_fn)(بنية mosquitto_سكو_data *سكو_data, بنية mosquitto_client_سكو *client_سكو))
{
	بنية mosquitto_client_سكو *client_سكو, *tmp;
	بنية mosquitto_base_سكو *base_سكو;
	صحيح access;

	DL_FOREACH_SAFE((*head), client_سكو, tmp){
		base_سكو = client_سكو->base_سكو;
		إذا(client_سكو->data.direction == mosq_md_out){
			access = MOSQ_ACL_READ;
		}وإلا{
			access = MOSQ_ACL_WRITE;
		}
		إذا(mosquitto_acl_check(context, base_سكو->data.topic,
				base_سكو->data.payloadlen, base_سكو->data.payload,
				base_سكو->data.qos, base_سكو->data.retain,
				base_سكو->data.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*head), client_سكو);
			decrement_stats_fn(سكو_data, client_سكو);
			plugin_persist_handle_client_سكو_delete(context, client_سكو);
			db_سكو_store_ref_dec(&client_سكو->base_سكو);
			mosquitto_FREE(client_سكو);
		}
	}
}


فراغ db__check_acl_of_all_messages(بنية mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.inflight, &db_سكو_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.queued, &db_سكو_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.inflight, &db_سكو_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.queued, &db_سكو_remove_from_queued_stats);
}


ساكن صحيح db__message_write_inflight_out_single(بنية mosquitto *context, بنية mosquitto_client_سكو *client_سكو)
{
	بنية mosquitto_base_سكو *base_سكو;
	mosquitto_property *base_سكو_props = عدم;
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

	base_سكو = client_سكو->base_سكو;

	expiry_interval = 0;
	إذا(base_سكو->data.expiry_time){
		إذا(db.now_real_s > base_سكو->data.expiry_time){
			/* Message is expired, must not send. */
			إذا(client_سكو->data.direction == mosq_md_out && client_سكو->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_سكو);
			db__fill_inflight_out_from_queue(context);
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			expiry_interval = (uint32_t)(base_سكو->data.expiry_time - db.now_real_s);
		}
	}
	mid = client_سكو->data.mid;
	retries = client_سكو->data.dup;
	retain = client_سكو->data.retain;
	topic = base_سكو->data.topic;
	qos = (uint8_t)client_سكو->data.qos;
	payloadlen = base_سكو->data.payloadlen;
	payload = base_سكو->data.payload;
	subscription_id = client_سكو->data.subscription_identifier;
	base_سكو_props = base_سكو->data.properties;

	بدل(client_سكو->data.state){
		حالة mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_سكو_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_سكو);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_سكو_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS){
				client_سكو->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_سكو->data.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_سكو_update(context, client_سكو);
			}وإلا إذا(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_سكو);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_سكو_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS){
				client_سكو->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_سكو->data.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_سكو_update(context, client_سكو);
			}وإلا إذا(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_سكو);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, عدم);
			إذا(!rc){
				client_سكو->data.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_سكو_update(context, client_سكو);
			}وإلا{
				أرجع rc;
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
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__message_write_inflight_out_all(بنية mosquitto *context)
{
	بنية mosquitto_client_سكو *client_سكو, *tmp;
	صحيح rc;

	إذا(context->state != mosq_cs_active || !net__is_connected(context)){
		أرجع MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_سكو, tmp){
		rc = db__message_write_inflight_out_single(context, client_سكو);
		إذا(rc){
			أرجع rc;
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__message_write_inflight_out_latest(بنية mosquitto *context)
{
	بنية mosquitto_client_سكو *client_سكو, *next;
	صحيح rc;

	إذا(context->state != mosq_cs_active
			|| !net__is_connected(context)
			|| context->msgs_out.inflight == عدم){

		أرجع MOSQ_ERR_SUCCESS;
	}

	إذا(context->msgs_out.inflight->prev == context->msgs_out.inflight){
		/* Only one message */
		أرجع db__message_write_inflight_out_single(context, context->msgs_out.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_سكو = context->msgs_out.inflight->prev;
	طالما(client_سكو != context->msgs_out.inflight &&
			(client_سكو->data.state == mosq_ms_publish_qos0
			|| client_سكو->data.state == mosq_ms_publish_qos1
			|| client_سكو->data.state == mosq_ms_publish_qos2)){

		client_سكو = client_سكو->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	إذا(client_سكو != context->msgs_out.inflight){
		client_سكو = client_سكو->next;
	}

	طالما(client_سكو){
		next = client_سكو->next;
		rc = db__message_write_inflight_out_single(context, client_سكو);
		إذا(rc){
			أرجع rc;
		}
		client_سكو = next;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__message_write_queued_in(بنية mosquitto *context)
{
	بنية mosquitto_client_سكو *client_سكو, *tmp;
	صحيح rc;

	إذا(context->state != mosq_cs_active){
		أرجع MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_سكو, tmp){
		إذا(context->msgs_in.inflight_maximum != 0 && context->msgs_in.inflight_quota == 0){
			اكسر;
		}

		إذا(client_سكو->data.qos == 2){
			client_سكو->data.state = mosq_ms_send_pubrec;
			db__message_dequeue_first(context, &context->msgs_in);
			rc = send__pubrec(context, client_سكو->data.mid, 0, عدم);
			إذا(!rc){
				client_سكو->data.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_سكو_update(context, client_سكو);
			}وإلا{
				plugin_persist_handle_client_سكو_update(context, client_سكو);
				أرجع rc;
			}
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__message_write_queued_out(بنية mosquitto *context)
{
	إذا(context->state != mosq_cs_active){
		أرجع MOSQ_ERR_SUCCESS;
	}

	db__fill_inflight_out_from_queue(context);

	أرجع MOSQ_ERR_SUCCESS;
}
