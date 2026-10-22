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

#include "config.h"

#include <assert.h>
#include <stdio.h>
#include <utlist.h>

#include "mosquitto_broker_internal.h"
#include "send_mosq.h"
#include "sys_tree.h"
#include "util_mosq.h"


/**
 * Is this context ready to take more in flight messages right now?
 * @param context the client context of interest
 * @param qos qos for the packet of interest
 * @return true if more in flight are allowed.
 */
bool db__ready_for_flight(struct mosquitto *context, enum mosquitto_msg_direction dir, цяло qos)
{
	struct mosquitto_msg_data *msgs;
	bool valid_bytes;
	bool valid_count;

	ако(dir == mosq_md_out){
		msgs = &context->msgs_out;
	}иначе{
		msgs = &context->msgs_in;
	}

	ако(msgs->inflight_maximum == 0 && db.config->max_inflight_bytes == 0){
		върни true;
	}

	ако(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		ако(db.config->max_queued_messages == 0 && db.config->max_inflight_bytes == 0){
			върни true;
		}
		valid_bytes = ((msgs->inflight_bytes - (ssize_t)db.config->max_inflight_bytes) < (ssize_t)db.config->max_queued_bytes);
		ако(dir == mosq_md_out){
			valid_count = context->out_packet_count < db.config->max_queued_messages;
		}иначе{
			valid_count = msgs->inflight_count - msgs->inflight_maximum < db.config->max_queued_messages;
		}

		ако(db.config->max_queued_messages == 0){
			върни valid_bytes;
		}
		ако(db.config->max_queued_bytes == 0){
			върни valid_count;
		}
	}иначе{
		valid_bytes = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_bytes;
		valid_count = msgs->inflight_quota > 0;

		ако(msgs->inflight_maximum == 0){
			върни valid_bytes;
		}
		ако(db.config->max_inflight_bytes == 0){
			върни valid_count;
		}
	}

	върни valid_bytes && valid_count;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db__ready_for_queue(struct mosquitto *context, цяло qos, struct mosquitto_msg_data *msg_data)
{
	цяло source_count;
	цяло adjust_count;
	long source_bytes;
	ssize_t adjust_bytes = (ssize_t)db.config->max_inflight_bytes;
	bool valid_bytes;
	bool valid_count;

	ако(db.config->max_queued_messages == 0 && db.config->max_queued_bytes == 0){
		върни true;
	}

	ако(qos == 0 && db.config->queue_qos0_messages == false){
		върни false; /* This case is handled in db__ready_for_flight() */
	}иначе{
		source_bytes = (ssize_t)msg_data->queued_bytes12;
		source_count = msg_data->queued_count12;
	}
	adjust_count = msg_data->inflight_maximum;

	/* nothing in flight for offline clients */
	ако(!net__is_connected(context)){
		adjust_bytes = 0;
		adjust_count = 0;
	}

	valid_bytes = (source_bytes - (ssize_t)adjust_bytes) < (ssize_t)db.config->max_queued_bytes;
	valid_count = source_count - adjust_count < db.config->max_queued_messages;

	ако(db.config->max_queued_bytes == 0){
		върни valid_count;
	}
	ако(db.config->max_queued_messages == 0){
		върни valid_bytes;
	}

	върни valid_bytes && valid_count;
}


void db__msg_add_to_inflight_stats(struct mosquitto_msg_data *msg_data, struct mosquitto__client_msg *client_msg)
{
	msg_data->inflight_count++;
	msg_data->inflight_bytes += client_msg->base_msg->data.payloadlen;
	ако(client_msg->data.qos != 0){
		msg_data->inflight_count12++;
		msg_data->inflight_bytes12 += client_msg->base_msg->data.payloadlen;
	}
}


static void db__msg_remove_from_inflight_stats(struct mosquitto_msg_data *msg_data, struct mosquitto__client_msg *client_msg)
{
	msg_data->inflight_count--;
	msg_data->inflight_bytes -= client_msg->base_msg->data.payloadlen;
	ако(client_msg->data.qos != 0){
		msg_data->inflight_count12--;
		msg_data->inflight_bytes12 -= client_msg->base_msg->data.payloadlen;
	}
}


void db__msg_add_to_queued_stats(struct mosquitto_msg_data *msg_data, struct mosquitto__client_msg *client_msg)
{
	msg_data->queued_count++;
	msg_data->queued_bytes += client_msg->base_msg->data.payloadlen;
	ако(client_msg->data.qos != 0){
		msg_data->queued_count12++;
		msg_data->queued_bytes12 += client_msg->base_msg->data.payloadlen;
	}
}


static void db__msg_remove_from_queued_stats(struct mosquitto_msg_data *msg_data, struct mosquitto__client_msg *client_msg)
{
	msg_data->queued_count--;
	msg_data->queued_bytes -= client_msg->base_msg->data.payloadlen;
	ако(client_msg->data.qos != 0){
		msg_data->queued_count12--;
		msg_data->queued_bytes12 -= client_msg->base_msg->data.payloadlen;
	}
}


цяло db__open(struct mosquitto__config *config)
{
	ако(!config){
		върни MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = NULL;
	db.contexts_by_sock = NULL;
	db.contexts_for_free = NULL;
#ifdef WITH_BRIDGE
	db.bridges = NULL;
	db.bridge_count = 0;
#endif

	/* Initialize the hashtable */
	db.clientid_index_hash = NULL;

	db.normal_subs = NULL;
	db.shared_subs = NULL;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = NULL;

#ifdef WITH_PERSISTENCE
	ако(persist__restore()){
		върни 1;
	}
#endif

	върни MOSQ_ERR_SUCCESS;
}


static void subhier_clean(struct mosquitto__subhier **subhier)
{
	struct mosquitto__subhier *peer, *subhier_tmp;
	struct mosquitto__subleaf *leaf, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		leaf = peer->subs;
		докато(leaf){
			nextleaf = leaf->next;
			mosquitto_FREE(leaf);
			leaf = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


цяло db__close(void)
{
	subhier_clean(&db.normal_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db__msg_store_clean();

	върни MOSQ_ERR_SUCCESS;
}


цяло db__msg_store_add(struct mosquitto__base_msg *base_msg)
{
	struct mosquitto__base_msg *found;
	unsigned hashv;

	HASH_VALUE(&base_msg->data.store_id, sizeof(base_msg->data.store_id), hashv);
	HASH_FIND_BYHASHVALUE(hh, db.msg_store, &base_msg->data.store_id, sizeof(base_msg->data.store_id), hashv, found);
	ако(found == NULL){
		HASH_ADD_KEYPTR_BYHASHVALUE(hh, db.msg_store, &base_msg->data.store_id, sizeof(base_msg->data.store_id), hashv, base_msg);
		върни MOSQ_ERR_SUCCESS;
	}иначе{
		върни MOSQ_ERR_ALREADY_EXISTS;
	}
}


void db__msg_store_free(struct mosquitto__base_msg *base_msg)
{
	mosquitto_FREE(base_msg->data.source_id);
	mosquitto_FREE(base_msg->data.source_username);
	ако(base_msg->dest_ids){
		за(цяло i=0; i<base_msg->dest_id_count; i++){
			mosquitto_FREE(base_msg->dest_ids[i]);
		}
		mosquitto_FREE(base_msg->dest_ids);
	}
	mosquitto_FREE(base_msg->data.topic);
	mosquitto_property_free_all(&base_msg->data.properties);
	mosquitto_FREE(base_msg->data.payload);
	mosquitto_FREE(base_msg);
}


void db__msg_store_remove(struct mosquitto__base_msg *base_msg, bool notify)
{
	ако(base_msg == NULL){
		върни;
	}
	HASH_DELETE(hh, db.msg_store, base_msg);
	db.msg_store_count--;
	db.msg_store_bytes -= base_msg->data.payloadlen;
	ако(notify == true){
		plugin_persist__handle_base_msg_delete(base_msg);
	}
	db__msg_store_free(base_msg);
}


void db__msg_store_clean(void)
{
	struct mosquitto__base_msg *base_msg, *base_msg_tmp;

	HASH_ITER(hh, db.msg_store, base_msg, base_msg_tmp){
		db__msg_store_remove(base_msg, false);
	}
}


void db__msg_store_ref_inc(struct mosquitto__base_msg *base_msg)
{
	base_msg->ref_count++;
}


void db__msg_store_ref_dec(struct mosquitto__base_msg **base_msg)
{
	(*base_msg)->ref_count--;
	ако((*base_msg)->ref_count == 0){
		db__msg_store_remove(*base_msg, true);
		*base_msg = NULL;
	}
}


void db__msg_store_compact(void)
{
	struct mosquitto__base_msg *base_msg, *base_msg_tmp;

	HASH_ITER(hh, db.msg_store, base_msg, base_msg_tmp){
		ако(base_msg->ref_count < 1){
			db__msg_store_remove(base_msg, true);
		}
	}
}


static void db__message_remove_inflight(struct mosquitto *context, struct mosquitto_msg_data *msg_data, struct mosquitto__client_msg *item)
{
	ако(!context || !msg_data || !item){
		върни;
	}

	plugin_persist__handle_client_msg_delete(context, item);

	DL_DELETE(msg_data->inflight, item);
	ако(item->base_msg){
		db__msg_remove_from_inflight_stats(msg_data, item);
		db__msg_store_ref_dec(&item->base_msg);
	}

	mosquitto_FREE(item);
}


static void db__message_remove_queued(struct mosquitto *context, struct mosquitto_msg_data *msg_data, struct mosquitto__client_msg *item)
{
	ако(!context || !msg_data || !item){
		върни;
	}

	plugin_persist__handle_client_msg_delete(context, item);

	DL_DELETE(msg_data->queued, item);
	ако(item->base_msg){
		db__msg_remove_from_queued_stats(msg_data, item);
		db__msg_store_ref_dec(&item->base_msg);
	}

	mosquitto_FREE(item);
}


static void db__fill_inflight_out_from_queue(struct mosquitto *context)
{
	struct mosquitto__client_msg *client_msg, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.queued, client_msg, tmp){
		ако(!db__ready_for_flight(context, mosq_md_out, client_msg->data.qos)){
			върни;
		}
		switch(client_msg->data.qos){
			case 0:
				client_msg->data.state = mosq_ms_publish_qos0;
				break;
			case 1:
				client_msg->data.state = mosq_ms_publish_qos1;
				break;
			case 2:
				client_msg->data.state = mosq_ms_publish_qos2;
				break;
		}
		ако(client_msg->base_msg->data.expiry_time && db.now_real_s > client_msg->base_msg->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_msg);
			continue;
		}
		plugin_persist__handle_client_msg_update(context, client_msg);
		db__message_dequeue_first(context, &context->msgs_out);
	}
}


void db__message_dequeue_first(struct mosquitto *context, struct mosquitto_msg_data *msg_data)
{
	struct mosquitto__client_msg *client_msg;

	UNUSED(context);

	client_msg = msg_data->queued;
	DL_DELETE(msg_data->queued, client_msg);
	DL_APPEND(msg_data->inflight, client_msg);
	ако(msg_data->inflight_quota > 0){
		msg_data->inflight_quota--;
	}

	db__msg_remove_from_queued_stats(msg_data, client_msg);
	db__msg_add_to_inflight_stats(msg_data, client_msg);
}


цяло db__message_delete_outgoing(struct mosquitto *context, uint16_t mid, enum mosquitto_msg_state expect_state, цяло qos)
{
	struct mosquitto__client_msg *client_msg, *tmp;
	bool deleted = false;

	ако(!context){
		върни MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_msg, tmp){
		ако(client_msg->data.mid == mid){
			ако(client_msg->data.qos != qos){
				log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_msg->data.qos, qos);
				върни MOSQ_ERR_PROTOCOL;
			}иначе ако(qos == 2 && client_msg->data.state != expect_state && expect_state != mosq_ms_any){
				log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_msg->data.state, expect_state);
				върни MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_out, client_msg);
			deleted = true;
			break;
		}
	}

	ако(deleted == false){
		DL_FOREACH_SAFE(context->msgs_out.queued, client_msg, tmp){
			ако(client_msg->data.mid == mid){
				ако(client_msg->data.qos != qos){
					върни MOSQ_ERR_PROTOCOL;
				}иначе ако(qos == 2 && client_msg->data.state != expect_state && expect_state != mosq_ms_any){
					върни MOSQ_ERR_PROTOCOL;
				}
				db__message_remove_queued(context, &context->msgs_out, client_msg);
				break;
			}
		}
	}
	db__fill_inflight_out_from_queue(context);
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	върни db__message_write_inflight_out_latest(context);
}


/* Only for QoS 2 messages */
цяло db__message_insert_incoming(struct mosquitto *context, uint64_t cmsg_id, struct mosquitto__base_msg *base_msg, bool persist)
{
	struct mosquitto__client_msg *client_msg;
	struct mosquitto_msg_data *msg_data;
	enum mosquitto_msg_state state = mosq_ms_invalid;
	цяло rc = 0;

	assert(base_msg);
	ако(!context){
		върни MOSQ_ERR_INVAL;
	}
	ако(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		върни MOSQ_ERR_SUCCESS;

	}
	msg_data = &context->msgs_in;

	ако(db__ready_for_flight(context, mosq_md_in, base_msg->data.qos)){
		state = mosq_ms_wait_for_pubrel;
	}иначе ако(base_msg->data.qos != 0 && db__ready_for_queue(context, base_msg->data.qos, msg_data)){
		state = mosq_ms_queued;
		rc = 2;
	}иначе{
		/* Dropping message due to full queue. */
		ако(context->is_dropping == false){
			context->is_dropping = true;
			log__printf(NULL, MOSQ_LOG_NOTICE,
					"Outgoing messages are being dropped for client %s.",
					context->id);
		}
		metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		върни 2;
	}

	assert(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	ако(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_msg = mosquitto_malloc(sizeof(struct mosquitto__client_msg));
	ако(!client_msg){
		върни MOSQ_ERR_NOMEM;
	}
	client_msg->prev = NULL;
	client_msg->next = NULL;
	ако(cmsg_id){
		client_msg->data.cmsg_id = cmsg_id;
	}иначе{
		client_msg->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_msg->base_msg = base_msg;
	db__msg_store_ref_inc(client_msg->base_msg);
	client_msg->data.mid = base_msg->data.source_mid;
	client_msg->data.direction = mosq_md_in;
	client_msg->data.state = (uint8_t)state;
	client_msg->data.dup = false;
	ако(base_msg->data.qos > context->max_qos){
		client_msg->data.qos = context->max_qos;
	}иначе{
		client_msg->data.qos = base_msg->data.qos;
	}
	client_msg->data.retain = base_msg->data.retain;
	client_msg->data.subscription_identifier = 0;

	ако(state == mosq_ms_queued){
		DL_APPEND(msg_data->queued, client_msg);
		db__msg_add_to_queued_stats(msg_data, client_msg);
	}иначе{
		DL_APPEND(msg_data->inflight, client_msg);
		db__msg_add_to_inflight_stats(msg_data, client_msg);
	}

	ако(persist && context->is_persisted){
		plugin_persist__handle_base_msg_add(client_msg->base_msg);
		plugin_persist__handle_client_msg_add(context, client_msg);
	}

	ако(client_msg->base_msg->data.qos > 0){
		util__decrement_receive_quota(context);
	}
	върни rc;
}


цяло db__message_insert_outgoing(struct mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, struct mosquitto__base_msg *base_msg, uint32_t subscription_identifier, bool update, bool persist)
{
	struct mosquitto__client_msg *client_msg;
	struct mosquitto_msg_data *msg_data;
	enum mosquitto_msg_state state = mosq_ms_invalid;
	цяло rc = 0;
	char **dest_ids;

	assert(base_msg);
	ако(!context){
		върни MOSQ_ERR_INVAL;
	}
	ако(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		върни MOSQ_ERR_SUCCESS;

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
	ако(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && base_msg->dest_ids){

		за(цяло i=0; i<base_msg->dest_id_count; i++){
			ако(base_msg->dest_ids[i] && !strcmp(base_msg->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				върни MOSQ_ERR_SUCCESS;
			}
		}
	}
	ако(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		ако(qos == 0 && !db.config->queue_qos0_messages){
			ако(!context->bridge){
				върни 2;
			}иначе{
				ако(context->bridge->start_type != bst_lazy){
					върни 2;
				}
			}
		}
		ако(context->bridge && context->bridge->clean_start_local == true){
			върни 2;
		}
	}

	ако(net__is_connected(context)){
		ако(db__ready_for_flight(context, mosq_md_out, qos)){
			switch(qos){
				case 0:
					state = mosq_ms_publish_qos0;
					break;
				case 1:
					state = mosq_ms_publish_qos1;
					break;
				case 2:
					state = mosq_ms_publish_qos2;
					break;
			}
		}иначе ако(qos != 0 && db__ready_for_queue(context, qos, msg_data)){
			state = mosq_ms_queued;
			rc = 2;
		}иначе{
			/* Dropping message due to full queue. */
			ако(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(NULL, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			върни 2;
		}
	}иначе{
		ако(db__ready_for_queue(context, qos, msg_data)){
			state = mosq_ms_queued;
		}иначе{
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			ако(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(NULL, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			върни 2;
		}
	}
	assert(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	ако(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_msg = mosquitto_malloc(sizeof(struct mosquitto__client_msg));
	ако(!client_msg){
		върни MOSQ_ERR_NOMEM;
	}
	client_msg->prev = NULL;
	client_msg->next = NULL;
	ако(cmsg_id){
		client_msg->data.cmsg_id = cmsg_id;
	}иначе{
		client_msg->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_msg->base_msg = base_msg;
	db__msg_store_ref_inc(client_msg->base_msg);
	client_msg->data.mid = mid;
	client_msg->data.direction = mosq_md_out;
	client_msg->data.state = (uint8_t)state;
	client_msg->data.dup = false;
	ако(qos > context->max_qos){
		client_msg->data.qos = context->max_qos;
	}иначе{
		client_msg->data.qos = qos;
	}
	client_msg->data.retain = retain;
	client_msg->data.subscription_identifier = subscription_identifier;

	ако(state == mosq_ms_queued){
		DL_APPEND(msg_data->queued, client_msg);
		db__msg_add_to_queued_stats(msg_data, client_msg);
	}иначе{
		DL_APPEND(msg_data->inflight, client_msg);
		db__msg_add_to_inflight_stats(msg_data, client_msg);
	}

	ако(persist && context->is_persisted){
		plugin_persist__handle_base_msg_add(client_msg->base_msg);
		plugin_persist__handle_client_msg_add(context, client_msg);
	}

	ако(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(base_msg->dest_ids, sizeof(char *)*(size_t)(base_msg->dest_id_count+1));
		ако(dest_ids){
			base_msg->dest_ids = dest_ids;
			base_msg->dest_id_count++;
			base_msg->dest_ids[base_msg->dest_id_count-1] = mosquitto_strdup(context->id);
			ако(!base_msg->dest_ids[base_msg->dest_id_count-1]){
				върни MOSQ_ERR_NOMEM;
			}
		}иначе{
			върни MOSQ_ERR_NOMEM;
		}
	}
#ifdef WITH_BRIDGE
	ако(context->bridge && context->bridge->start_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_out.inflight_count + context->msgs_out.queued_count >= context->bridge->threshold){

		context->bridge->lazy_reconnect = true;
	}
#endif

	ако(client_msg->data.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	ако(update){
		rc = db__message_write_inflight_out_latest(context);
		ако(rc){
			върни rc;
		}
		rc = db__message_write_queued_out(context);
		ако(rc){
			върни rc;
		}
	}

	върни rc;
}


static inline цяло db__message_update_outgoing_state(struct mosquitto *context, struct mosquitto__client_msg *head,
		uint16_t mid, enum mosquitto_msg_state state, цяло qos, bool persist)
{
	struct mosquitto__client_msg *client_msg;

	DL_FOREACH(head, client_msg){
		ако(client_msg->data.mid == mid){
			ако(client_msg->data.qos != qos){
				log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_msg->data.qos, qos);
				върни MOSQ_ERR_PROTOCOL;
			}
			client_msg->data.state = (uint8_t)state;
			ако(persist){
				plugin_persist__handle_client_msg_update(context, client_msg);
			}
			върни MOSQ_ERR_SUCCESS;
		}
	}
	върни MOSQ_ERR_NOT_FOUND;
}


цяло db__message_update_outgoing(struct mosquitto *context, uint16_t mid, enum mosquitto_msg_state state, цяло qos, bool persist)
{
	цяло rc;

	rc = db__message_update_outgoing_state(context, context->msgs_out.inflight, mid, state, qos, persist);
	ако(!persist && rc == MOSQ_ERR_NOT_FOUND){
		rc = db__message_update_outgoing_state(context, context->msgs_out.queued, mid, state, qos, persist);
	}
	върни rc;
}


static void db__messages_delete_list(struct mosquitto__client_msg **head)
{
	struct mosquitto__client_msg *client_msg, *tmp;

	DL_FOREACH_SAFE(*head, client_msg, tmp){
		DL_DELETE(*head, client_msg);
		db__msg_store_ref_dec(&client_msg->base_msg);
		mosquitto_FREE(client_msg);
	}
	*head = NULL;
}


цяло db__messages_delete_incoming(struct mosquitto *context)
{
	ако(!context){
		върни MOSQ_ERR_INVAL;
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

	върни MOSQ_ERR_SUCCESS;
}


цяло db__messages_delete_outgoing(struct mosquitto *context)
{
	ако(!context){
		върни MOSQ_ERR_INVAL;
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

	върни MOSQ_ERR_SUCCESS;
}


цяло db__messages_delete(struct mosquitto *context, bool force_free)
{
	ако(!context){
		върни MOSQ_ERR_INVAL;
	}

	ако(force_free || context->clean_start || (context->bridge && context->bridge->clean_start)){
		db__messages_delete_incoming(context);
	}

	ако(force_free || (context->bridge && context->bridge->clean_start_local)
			|| (context->bridge == NULL && context->clean_start)){

		db__messages_delete_outgoing(context);
	}

	върни MOSQ_ERR_SUCCESS;
}


цяло db__messages_easy_queue(struct mosquitto *context, const char *topic, uint8_t qos, uint32_t payloadlen, const void *payload, цяло retain, uint32_t message_expiry_interval, mosquitto_property **properties)
{
	struct mosquitto__base_msg *base_msg;
	const char *source_id;
	enum mosquitto_msg_origin origin;

	ако(!topic){
		върни MOSQ_ERR_INVAL;
	}

	base_msg = mosquitto_calloc(1, sizeof(struct mosquitto__base_msg));
	ако(base_msg == NULL){
		върни MOSQ_ERR_NOMEM;
	}

	base_msg->data.topic = mosquitto_strdup(topic);
	ако(base_msg->data.topic == NULL){
		db__msg_store_free(base_msg);
		върни MOSQ_ERR_INVAL;
	}

	base_msg->data.qos = qos;
	ако(db.config->retain_available == false){
		base_msg->data.retain = 0;
	}иначе{
		base_msg->data.retain = retain;
	}

	base_msg->data.payloadlen = payloadlen;
	ако(payloadlen > 0){
		base_msg->data.payload = mosquitto_malloc(base_msg->data.payloadlen+1);
		ако(base_msg->data.payload == NULL){
			db__msg_store_free(base_msg);
			върни MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_msg->data.payload)[base_msg->data.payloadlen] = 0;
		memcpy(base_msg->data.payload, payload, base_msg->data.payloadlen);
	}

	ако(context && context->id){
		source_id = context->id;
	}иначе{
		source_id = "";
	}
	ако(properties){
		base_msg->data.properties = *properties;
		*properties = NULL;
	}

	ако(context){
		origin = mosq_mo_client;
	}иначе{
		origin = mosq_mo_broker;
	}
	ако(db__message_store(context, base_msg, &message_expiry_interval, origin)){
		върни 1;
	}

	върни sub__messages_queue(source_id, base_msg->data.qos, &base_msg);
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
uint64_t db__new_msg_id(void)
{
#ifdef WIN32
	FILETIME ftime;
	uint64_t ftime64;
#иначе
	struct timespec ts;
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
#иначе
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#endif
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	ако(id <= db.last_db_id){
		id = db.last_db_id + 1;
	}
	db.last_db_id = id;

	върни id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
цяло db__message_store(const struct mosquitto *source, struct mosquitto__base_msg *base_msg, uint32_t *message_expiry_interval, enum mosquitto_msg_origin origin)
{
	цяло rc;

	assert(base_msg);

	ако(source && source->id){
		base_msg->data.source_id = mosquitto_strdup(source->id);
	}иначе{
		base_msg->data.source_id = mosquitto_strdup("");
	}
	ако(!base_msg->data.source_id){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		db__msg_store_free(base_msg);
		върни MOSQ_ERR_NOMEM;
	}

	ако(source && source->username){
		base_msg->data.source_username = mosquitto_strdup(source->username);
		ако(!base_msg->data.source_username){
			db__msg_store_free(base_msg);
			върни MOSQ_ERR_NOMEM;
		}
	}
	ако(source){
		base_msg->source_listener = source->listener;
	}
	base_msg->origin = origin;
	ако(message_expiry_interval){
		ако(*message_expiry_interval > 0 && *message_expiry_interval != MSG_EXPIRY_INFINITE){
			base_msg->data.expiry_time = db.now_real_s + *message_expiry_interval;
		}иначе{
			base_msg->data.expiry_time = 0;
		}
	}

	base_msg->dest_ids = NULL;
	base_msg->dest_id_count = 0;
	db.msg_store_count++;
	db.msg_store_bytes += base_msg->data.payloadlen;

	ако(!base_msg->data.store_id){
		base_msg->data.store_id = db__new_msg_id();
	}

	rc = db__msg_store_add(base_msg);
	ако(rc){
		db__msg_store_free(base_msg);
		върни rc;
	}

	върни MOSQ_ERR_SUCCESS;
}


цяло db__message_store_find(struct mosquitto *context, uint16_t mid, struct mosquitto__client_msg **client_msg)
{
	struct mosquitto__client_msg *cmsg;

	*client_msg = NULL;

	ако(!context){
		върни MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_in.inflight, cmsg){
		ако(cmsg->base_msg->data.source_mid == mid){
			*client_msg = cmsg;
			върни MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_in.queued, cmsg){
		ако(cmsg->base_msg->data.source_mid == mid){
			*client_msg = cmsg;
			върни MOSQ_ERR_SUCCESS;
		}
	}

	върни 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
static цяло db__message_reconnect_reset_outgoing(struct mosquitto *context)
{
	struct mosquitto__client_msg *client_msg, *tmp;

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
		ако(client_msg->data.qos > 0){
			util__decrement_send_quota(context);
		}

		switch(client_msg->data.qos){
			case 0:
				client_msg->data.state = mosq_ms_publish_qos0;
				break;
			case 1:
				client_msg->data.state = mosq_ms_publish_qos1;
				break;
			case 2:
				ако(client_msg->data.state == mosq_ms_wait_for_pubcomp){
					client_msg->data.state = mosq_ms_resend_pubrel;
				}иначе{
					client_msg->data.state = mosq_ms_publish_qos2;
				}
				break;
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

	върни MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
static цяло db__message_reconnect_reset_incoming(struct mosquitto *context)
{
	struct mosquitto__client_msg *client_msg, *tmp;

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
		ако(client_msg->data.qos > 0){
			util__decrement_receive_quota(context);
		}

		ако(client_msg->data.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db__message_remove_inflight(context, &context->msgs_in, client_msg);
		}иначе{
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
		ако(db__ready_for_flight(context, mosq_md_in, client_msg->data.qos)){
			switch(client_msg->data.qos){
				case 0:
					client_msg->data.state = mosq_ms_publish_qos0;
					break;
				case 1:
					client_msg->data.state = mosq_ms_publish_qos1;
					break;
				case 2:
					client_msg->data.state = mosq_ms_publish_qos2;
					break;
			}
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist__handle_client_msg_update(context, client_msg);
		}
	}

	върни MOSQ_ERR_SUCCESS;
}


цяло db__message_reconnect_reset(struct mosquitto *context)
{
	цяло rc;

	rc = db__message_reconnect_reset_outgoing(context);
	ако(rc){
		върни rc;
	}
	върни db__message_reconnect_reset_incoming(context);
}


цяло db__message_remove_incoming(struct mosquitto *context, uint16_t mid)
{
	struct mosquitto__client_msg *client_msg, *tmp;

	ако(!context){
		върни MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_msg, tmp){
		ако(client_msg->data.mid == mid){
			ако(client_msg->base_msg->data.qos != 2){
				log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_msg->base_msg->data.qos);
				върни MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_in, client_msg);
			върни MOSQ_ERR_SUCCESS;
		}
	}

	върни MOSQ_ERR_NOT_FOUND;
}


цяло db__message_release_incoming(struct mosquitto *context, uint16_t mid)
{
	struct mosquitto__client_msg *client_msg, *tmp;
	bool deleted = false;
	цяло rc;

	ако(!context){
		върни MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_msg, tmp){
		ако(client_msg->data.mid == mid){
			ако(client_msg->base_msg->data.qos != 2){
				log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_msg->base_msg->data.qos);
				върни MOSQ_ERR_PROTOCOL;
			}
			const char *topic = client_msg->base_msg->data.topic;
			const char *source_id = client_msg->base_msg->data.source_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			ако(topic == NULL){
				db__message_remove_inflight(context, &context->msgs_in, client_msg);
				deleted = true;
			}иначе{
				rc = sub__messages_queue(source_id, 2, &client_msg->base_msg);
				ако(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db__message_remove_inflight(context, &context->msgs_in, client_msg);
					deleted = true;
				}иначе{
					върни 1;
				}
			}
		}
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_msg, tmp){
		ако(db__ready_for_flight(context, mosq_md_in, client_msg->data.qos)){
			break;
		}

		ако(client_msg->data.qos == 2){
			send__pubrec(context, client_msg->data.mid, 0, NULL);
			client_msg->data.state = mosq_ms_wait_for_pubrel;
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist__handle_client_msg_update(context, client_msg);
		}
	}
	ако(deleted){
		върни MOSQ_ERR_SUCCESS;
	}иначе{
		върни MOSQ_ERR_NOT_FOUND;
	}
}


void db__expire_all_messages(struct mosquitto *context)
{
	struct mosquitto__client_msg *client_msg, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_msg, tmp){
		ако(client_msg->base_msg->data.expiry_time && db.now_real_s > client_msg->base_msg->data.expiry_time){
			ако(client_msg->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_msg);
		}
	}
	db__fill_inflight_out_from_queue(context);
	DL_FOREACH_SAFE(context->msgs_out.queued, client_msg, tmp){
		ако(client_msg->base_msg->data.expiry_time && db.now_real_s > client_msg->base_msg->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_msg);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.inflight, client_msg, tmp){
		ако(client_msg->base_msg->data.expiry_time && db.now_real_s > client_msg->base_msg->data.expiry_time){
			ако(client_msg->data.qos > 0){
				util__increment_receive_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_in, client_msg);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.queued, client_msg, tmp){
		ако(client_msg->base_msg->data.expiry_time && db.now_real_s > client_msg->base_msg->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_in, client_msg);
		}
	}
}


static void db__client_messages_check_acl(struct mosquitto *context, struct mosquitto_msg_data *msg_data, struct mosquitto__client_msg **head,
		void (*decrement_stats_fn)(struct mosquitto_msg_data *msg_data, struct mosquitto__client_msg *client_msg))
{
	struct mosquitto__client_msg *client_msg, *tmp;
	struct mosquitto__base_msg *base_msg;
	цяло access;

	DL_FOREACH_SAFE((*head), client_msg, tmp){
		base_msg = client_msg->base_msg;
		ако(client_msg->data.direction == mosq_md_out){
			access = MOSQ_ACL_READ;
		}иначе{
			access = MOSQ_ACL_WRITE;
		}
		ако(mosquitto_acl_check(context, base_msg->data.topic,
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


void db__check_acl_of_all_messages(struct mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.inflight, &db__msg_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.queued, &db__msg_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.inflight, &db__msg_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.queued, &db__msg_remove_from_queued_stats);
}


static цяло db__message_write_inflight_out_single(struct mosquitto *context, struct mosquitto__client_msg *client_msg)
{
	struct mosquitto__base_msg *base_msg;
	mosquitto_property *base_msg_props = NULL;
	цяло rc;
	uint16_t mid;
	цяло retries;
	цяло retain;
	const char *topic;
	uint8_t qos;
	uint32_t payloadlen;
	const void *payload;
	uint32_t expiry_interval;
	uint32_t subscription_id;

	base_msg = client_msg->base_msg;

	expiry_interval = 0;
	ако(base_msg->data.expiry_time){
		ако(db.now_real_s > base_msg->data.expiry_time){
			/* Message is expired, must not send. */
			ако(client_msg->data.direction == mosq_md_out && client_msg->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_msg);
			db__fill_inflight_out_from_queue(context);
			върни MOSQ_ERR_SUCCESS;
		}иначе{
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

	switch(client_msg->data.state){
		case mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_msg_props, expiry_interval);
			ако(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_msg);
			}иначе{
				върни rc;
			}
			break;

		case mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_msg_props, expiry_interval);
			ако(rc == MOSQ_ERR_SUCCESS){
				client_msg->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_msg->data.state = mosq_ms_wait_for_puback;
				plugin_persist__handle_client_msg_update(context, client_msg);
			}иначе ако(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_msg);
			}иначе{
				върни rc;
			}
			break;

		case mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_msg_props, expiry_interval);
			ако(rc == MOSQ_ERR_SUCCESS){
				client_msg->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_msg->data.state = mosq_ms_wait_for_pubrec;
				plugin_persist__handle_client_msg_update(context, client_msg);
			}иначе ако(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_msg);
			}иначе{
				върни rc;
			}
			break;

		case mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, NULL);
			ако(!rc){
				client_msg->data.state = mosq_ms_wait_for_pubcomp;
				plugin_persist__handle_client_msg_update(context, client_msg);
			}иначе{
				върни rc;
			}
			break;

		case mosq_ms_invalid:
		case mosq_ms_send_pubrec:
		case mosq_ms_resend_pubcomp:
		case mosq_ms_wait_for_puback:
		case mosq_ms_wait_for_pubrec:
		case mosq_ms_wait_for_pubrel:
		case mosq_ms_wait_for_pubcomp:
		case mosq_ms_queued:
			break;
	}
	върни MOSQ_ERR_SUCCESS;
}


цяло db__message_write_inflight_out_all(struct mosquitto *context)
{
	struct mosquitto__client_msg *client_msg, *tmp;
	цяло rc;

	ако(context->state != mosq_cs_active || !net__is_connected(context)){
		върни MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_msg, tmp){
		rc = db__message_write_inflight_out_single(context, client_msg);
		ако(rc){
			върни rc;
		}
	}
	върни MOSQ_ERR_SUCCESS;
}


цяло db__message_write_inflight_out_latest(struct mosquitto *context)
{
	struct mosquitto__client_msg *client_msg, *next;
	цяло rc;

	ако(context->state != mosq_cs_active
			|| !net__is_connected(context)
			|| context->msgs_out.inflight == NULL){

		върни MOSQ_ERR_SUCCESS;
	}

	ако(context->msgs_out.inflight->prev == context->msgs_out.inflight){
		/* Only one message */
		върни db__message_write_inflight_out_single(context, context->msgs_out.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_msg = context->msgs_out.inflight->prev;
	докато(client_msg != context->msgs_out.inflight &&
			(client_msg->data.state == mosq_ms_publish_qos0
			|| client_msg->data.state == mosq_ms_publish_qos1
			|| client_msg->data.state == mosq_ms_publish_qos2)){

		client_msg = client_msg->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	ако(client_msg != context->msgs_out.inflight){
		client_msg = client_msg->next;
	}

	докато(client_msg){
		next = client_msg->next;
		rc = db__message_write_inflight_out_single(context, client_msg);
		ако(rc){
			върни rc;
		}
		client_msg = next;
	}
	върни MOSQ_ERR_SUCCESS;
}


цяло db__message_write_queued_in(struct mosquitto *context)
{
	struct mosquitto__client_msg *client_msg, *tmp;
	цяло rc;

	ако(context->state != mosq_cs_active){
		върни MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_msg, tmp){
		ако(context->msgs_in.inflight_maximum != 0 && context->msgs_in.inflight_quota == 0){
			break;
		}

		ако(client_msg->data.qos == 2){
			client_msg->data.state = mosq_ms_send_pubrec;
			db__message_dequeue_first(context, &context->msgs_in);
			rc = send__pubrec(context, client_msg->data.mid, 0, NULL);
			ако(!rc){
				client_msg->data.state = mosq_ms_wait_for_pubrel;
				plugin_persist__handle_client_msg_update(context, client_msg);
			}иначе{
				plugin_persist__handle_client_msg_update(context, client_msg);
				върни rc;
			}
		}
	}
	върни MOSQ_ERR_SUCCESS;
}


цяло db__message_write_queued_out(struct mosquitto *context)
{
	ако(context->state != mosq_cs_active){
		върни MOSQ_ERR_SUCCESS;
	}

	db__fill_inflight_out_from_queue(context);

	върни MOSQ_ERR_SUCCESS;
}
