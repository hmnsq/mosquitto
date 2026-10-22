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
bool db__ready_for_flight(بنية mosquitto *context, تعداد mosquitto_ئۇچۇر_direction dir, صحيح qos)
{
	بنية mosquitto_ئۇچۇر_data *msgs;
	bool valid_bytes;
	bool valid_سانى;

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
			valid_سانى = context->out_packet_سانى < db.config->max_queued_messages;
		}وإلا{
			valid_سانى = msgs->inflight_سانى - msgs->inflight_maximum < db.config->max_queued_messages;
		}

		إذا(db.config->max_queued_messages == 0){
			أرجع valid_bytes;
		}
		إذا(db.config->max_queued_bytes == 0){
			أرجع valid_سانى;
		}
	}وإلا{
		valid_bytes = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_bytes;
		valid_سانى = msgs->inflight_quota > 0;

		إذا(msgs->inflight_maximum == 0){
			أرجع valid_bytes;
		}
		إذا(db.config->max_inflight_bytes == 0){
			أرجع valid_سانى;
		}
	}

	أرجع valid_bytes && valid_سانى;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db__ready_for_queue(بنية mosquitto *context, صحيح qos, بنية mosquitto_ئۇچۇر_data *ئۇچۇر_data)
{
	صحيح source_سانى;
	صحيح adjust_سانى;
	long source_bytes;
	ssize_t adjust_bytes = (ssize_t)db.config->max_inflight_bytes;
	bool valid_bytes;
	bool valid_سانى;

	إذا(db.config->max_queued_messages == 0 && db.config->max_queued_bytes == 0){
		أرجع true;
	}

	إذا(qos == 0 && db.config->queue_qos0_messages == false){
		أرجع false; /* This case is handled in db__ready_for_flight() */
	}وإلا{
		source_bytes = (ssize_t)ئۇچۇر_data->queued_bytes12;
		source_سانى = ئۇچۇر_data->queued_count12;
	}
	adjust_سانى = ئۇچۇر_data->inflight_maximum;

	/* nothing in flight for offline clients */
	إذا(!net__is_connected(context)){
		adjust_bytes = 0;
		adjust_سانى = 0;
	}

	valid_bytes = (source_bytes - (ssize_t)adjust_bytes) < (ssize_t)db.config->max_queued_bytes;
	valid_سانى = source_سانى - adjust_سانى < db.config->max_queued_messages;

	إذا(db.config->max_queued_bytes == 0){
		أرجع valid_سانى;
	}
	إذا(db.config->max_queued_messages == 0){
		أرجع valid_bytes;
	}

	أرجع valid_bytes && valid_سانى;
}


فراغ db_ئۇچۇر_add_to_inflight_stats(بنية mosquitto_ئۇچۇر_data *ئۇچۇر_data, بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر)
{
	ئۇچۇر_data->inflight_سانى++;
	ئۇچۇر_data->inflight_bytes += client_ئۇچۇر->base_ئۇچۇر->data.payloadlen;
	إذا(client_ئۇچۇر->data.qos != 0){
		ئۇچۇر_data->inflight_count12++;
		ئۇچۇر_data->inflight_bytes12 += client_ئۇچۇر->base_ئۇچۇر->data.payloadlen;
	}
}


ساكن فراغ db_ئۇچۇر_remove_from_inflight_stats(بنية mosquitto_ئۇچۇر_data *ئۇچۇر_data, بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر)
{
	ئۇچۇر_data->inflight_سانى--;
	ئۇچۇر_data->inflight_bytes -= client_ئۇچۇر->base_ئۇچۇر->data.payloadlen;
	إذا(client_ئۇچۇر->data.qos != 0){
		ئۇچۇر_data->inflight_count12--;
		ئۇچۇر_data->inflight_bytes12 -= client_ئۇچۇر->base_ئۇچۇر->data.payloadlen;
	}
}


فراغ db_ئۇچۇر_add_to_queued_stats(بنية mosquitto_ئۇچۇر_data *ئۇچۇر_data, بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر)
{
	ئۇچۇر_data->queued_سانى++;
	ئۇچۇر_data->queued_bytes += client_ئۇچۇر->base_ئۇچۇر->data.payloadlen;
	إذا(client_ئۇچۇر->data.qos != 0){
		ئۇچۇر_data->queued_count12++;
		ئۇچۇر_data->queued_bytes12 += client_ئۇچۇر->base_ئۇچۇر->data.payloadlen;
	}
}


ساكن فراغ db_ئۇچۇر_remove_from_queued_stats(بنية mosquitto_ئۇچۇر_data *ئۇچۇر_data, بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر)
{
	ئۇچۇر_data->queued_سانى--;
	ئۇچۇر_data->queued_bytes -= client_ئۇچۇر->base_ئۇچۇر->data.payloadlen;
	إذا(client_ئۇچۇر->data.qos != 0){
		ئۇچۇر_data->queued_count12--;
		ئۇچۇر_data->queued_bytes12 -= client_ئۇچۇر->base_ئۇچۇر->data.payloadlen;
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
	db.bridge_سانى = 0;
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
	db_ئۇچۇر_store_clean();

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_ئۇچۇر_store_add(بنية mosquitto_base_ئۇچۇر *base_ئۇچۇر)
{
	بنية mosquitto_base_ئۇچۇر *found;
	unsigned hashv;

	HASH_قىممەت(&base_ئۇچۇر->data.store_id, حجم(base_ئۇچۇر->data.store_id), hashv);
	HASH_FIND_BYHASHVALUE(hh, db.ئۇچۇر_store, &base_ئۇچۇر->data.store_id, حجم(base_ئۇچۇر->data.store_id), hashv, found);
	إذا(found == عدم){
		HASH_ADD_KEYPTR_BYHASHVALUE(hh, db.ئۇچۇر_store, &base_ئۇچۇر->data.store_id, حجم(base_ئۇچۇر->data.store_id), hashv, base_ئۇچۇر);
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_ALREADY_EXISTS;
	}
}


فراغ db_ئۇچۇر_store_free(بنية mosquitto_base_ئۇچۇر *base_ئۇچۇر)
{
	mosquitto_FREE(base_ئۇچۇر->data.source_id);
	mosquitto_FREE(base_ئۇچۇر->data.source_username);
	إذا(base_ئۇچۇر->dest_ids){
		لكل(صحيح i=0; i<base_ئۇچۇر->dest_id_سانى; i++){
			mosquitto_FREE(base_ئۇچۇر->dest_ids[i]);
		}
		mosquitto_FREE(base_ئۇچۇر->dest_ids);
	}
	mosquitto_FREE(base_ئۇچۇر->data.topic);
	mosquitto_property_free_all(&base_ئۇچۇر->data.properties);
	mosquitto_FREE(base_ئۇچۇر->data.payload);
	mosquitto_FREE(base_ئۇچۇر);
}


فراغ db_ئۇچۇر_store_remove(بنية mosquitto_base_ئۇچۇر *base_ئۇچۇر, bool notify)
{
	إذا(base_ئۇچۇر == عدم){
		أرجع;
	}
	HASH_DELETE(hh, db.ئۇچۇر_store, base_ئۇچۇر);
	db.ئۇچۇر_store_سانى--;
	db.ئۇچۇر_store_bytes -= base_ئۇچۇر->data.payloadlen;
	إذا(notify == true){
		plugin_persist_handle_base_ئۇچۇر_delete(base_ئۇچۇر);
	}
	db_ئۇچۇر_store_free(base_ئۇچۇر);
}


فراغ db_ئۇچۇر_store_clean(فراغ)
{
	بنية mosquitto_base_ئۇچۇر *base_ئۇچۇر, *base_ئۇچۇر_tmp;

	HASH_ITER(hh, db.ئۇچۇر_store, base_ئۇچۇر, base_ئۇچۇر_tmp){
		db_ئۇچۇر_store_remove(base_ئۇچۇر, false);
	}
}


فراغ db_ئۇچۇر_store_ref_inc(بنية mosquitto_base_ئۇچۇر *base_ئۇچۇر)
{
	base_ئۇچۇر->ref_سانى++;
}


فراغ db_ئۇچۇر_store_ref_dec(بنية mosquitto_base_ئۇچۇر **base_ئۇچۇر)
{
	(*base_ئۇچۇر)->ref_سانى--;
	إذا((*base_ئۇچۇر)->ref_سانى == 0){
		db_ئۇچۇر_store_remove(*base_ئۇچۇر, true);
		*base_ئۇچۇر = عدم;
	}
}


فراغ db_ئۇچۇر_store_compact(فراغ)
{
	بنية mosquitto_base_ئۇچۇر *base_ئۇچۇر, *base_ئۇچۇر_tmp;

	HASH_ITER(hh, db.ئۇچۇر_store, base_ئۇچۇر, base_ئۇچۇر_tmp){
		إذا(base_ئۇچۇر->ref_سانى < 1){
			db_ئۇچۇر_store_remove(base_ئۇچۇر, true);
		}
	}
}


ساكن فراغ db__message_remove_inflight(بنية mosquitto *context, بنية mosquitto_ئۇچۇر_data *ئۇچۇر_data, بنية mosquitto_client_ئۇچۇر *item)
{
	إذا(!context || !ئۇچۇر_data || !item){
		أرجع;
	}

	plugin_persist_handle_client_ئۇچۇر_delete(context, item);

	DL_DELETE(ئۇچۇر_data->inflight, item);
	إذا(item->base_ئۇچۇر){
		db_ئۇچۇر_remove_from_inflight_stats(ئۇچۇر_data, item);
		db_ئۇچۇر_store_ref_dec(&item->base_ئۇچۇر);
	}

	mosquitto_FREE(item);
}


ساكن فراغ db__message_remove_queued(بنية mosquitto *context, بنية mosquitto_ئۇچۇر_data *ئۇچۇر_data, بنية mosquitto_client_ئۇچۇر *item)
{
	إذا(!context || !ئۇچۇر_data || !item){
		أرجع;
	}

	plugin_persist_handle_client_ئۇچۇر_delete(context, item);

	DL_DELETE(ئۇچۇر_data->queued, item);
	إذا(item->base_ئۇچۇر){
		db_ئۇچۇر_remove_from_queued_stats(ئۇچۇر_data, item);
		db_ئۇچۇر_store_ref_dec(&item->base_ئۇچۇر);
	}

	mosquitto_FREE(item);
}


ساكن فراغ db__fill_inflight_out_from_queue(بنية mosquitto *context)
{
	بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.queued, client_ئۇچۇر, tmp){
		إذا(!db__ready_for_flight(context, mosq_md_out, client_ئۇچۇر->data.qos)){
			أرجع;
		}
		بدل(client_ئۇچۇر->data.qos){
			حالة 0:
				client_ئۇچۇر->data.state = mosq_ms_publish_qos0;
				اكسر;
			حالة 1:
				client_ئۇچۇر->data.state = mosq_ms_publish_qos1;
				اكسر;
			حالة 2:
				client_ئۇچۇر->data.state = mosq_ms_publish_qos2;
				اكسر;
		}
		إذا(client_ئۇچۇر->base_ئۇچۇر->data.expiry_time && db.now_real_s > client_ئۇچۇر->base_ئۇچۇر->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_ئۇچۇر);
			استمر;
		}
		plugin_persist_handle_client_ئۇچۇر_update(context, client_ئۇچۇر);
		db__message_dequeue_first(context, &context->msgs_out);
	}
}


فراغ db__message_dequeue_first(بنية mosquitto *context, بنية mosquitto_ئۇچۇر_data *ئۇچۇر_data)
{
	بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر;

	UNUSED(context);

	client_ئۇچۇر = ئۇچۇر_data->queued;
	DL_DELETE(ئۇچۇر_data->queued, client_ئۇچۇر);
	DL_APPEND(ئۇچۇر_data->inflight, client_ئۇچۇر);
	إذا(ئۇچۇر_data->inflight_quota > 0){
		ئۇچۇر_data->inflight_quota--;
	}

	db_ئۇچۇر_remove_from_queued_stats(ئۇچۇر_data, client_ئۇچۇر);
	db_ئۇچۇر_add_to_inflight_stats(ئۇچۇر_data, client_ئۇچۇر);
}


صحيح db__message_delete_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_ئۇچۇر_state expect_state, صحيح qos)
{
	بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر, *tmp;
	bool deleted = false;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_ئۇچۇر, tmp){
		إذا(client_ئۇچۇر->data.mid == mid){
			إذا(client_ئۇچۇر->data.qos != qos){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_ئۇچۇر->data.qos, qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}وإلا إذا(qos == 2 && client_ئۇچۇر->data.state != expect_state && expect_state != mosq_ms_any){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_ئۇچۇر->data.state, expect_state);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_out, client_ئۇچۇر);
			deleted = true;
			اكسر;
		}
	}

	إذا(deleted == false){
		DL_FOREACH_SAFE(context->msgs_out.queued, client_ئۇچۇر, tmp){
			إذا(client_ئۇچۇر->data.mid == mid){
				إذا(client_ئۇچۇر->data.qos != qos){
					أرجع MOSQ_ERR_PROTOCOL;
				}وإلا إذا(qos == 2 && client_ئۇچۇر->data.state != expect_state && expect_state != mosq_ms_any){
					أرجع MOSQ_ERR_PROTOCOL;
				}
				db__message_remove_queued(context, &context->msgs_out, client_ئۇچۇر);
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
صحيح db_message_قىستۇرۇش_incoming(بنية mosquitto *context, uint64_t cmsg_id, بنية mosquitto_base_ئۇچۇر *base_ئۇچۇر, bool persist)
{
	بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر;
	بنية mosquitto_ئۇچۇر_data *ئۇچۇر_data;
	تعداد mosquitto_ئۇچۇر_state state = mosq_ms_invalid;
	صحيح rc = 0;

	أكد(base_ئۇچۇر);
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		أرجع MOSQ_ERR_SUCCESS;

	}
	ئۇچۇر_data = &context->msgs_in;

	إذا(db__ready_for_flight(context, mosq_md_in, base_ئۇچۇر->data.qos)){
		state = mosq_ms_wait_for_pubrel;
	}وإلا إذا(base_ئۇچۇر->data.qos != 0 && db__ready_for_queue(context, base_ئۇچۇر->data.qos, ئۇچۇر_data)){
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

	client_ئۇچۇر = mosquitto_malloc(حجم(بنية mosquitto_client_ئۇچۇر));
	إذا(!client_ئۇچۇر){
		أرجع MOSQ_ERR_NOMEM;
	}
	client_ئۇچۇر->prev = عدم;
	client_ئۇچۇر->next = عدم;
	إذا(cmsg_id){
		client_ئۇچۇر->data.cmsg_id = cmsg_id;
	}وإلا{
		client_ئۇچۇر->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_ئۇچۇر->base_ئۇچۇر = base_ئۇچۇر;
	db_ئۇچۇر_store_ref_inc(client_ئۇچۇر->base_ئۇچۇر);
	client_ئۇچۇر->data.mid = base_ئۇچۇر->data.source_mid;
	client_ئۇچۇر->data.direction = mosq_md_in;
	client_ئۇچۇر->data.state = (uint8_t)state;
	client_ئۇچۇر->data.dup = false;
	إذا(base_ئۇچۇر->data.qos > context->max_qos){
		client_ئۇچۇر->data.qos = context->max_qos;
	}وإلا{
		client_ئۇچۇر->data.qos = base_ئۇچۇر->data.qos;
	}
	client_ئۇچۇر->data.retain = base_ئۇچۇر->data.retain;
	client_ئۇچۇر->data.subscription_identifier = 0;

	إذا(state == mosq_ms_queued){
		DL_APPEND(ئۇچۇر_data->queued, client_ئۇچۇر);
		db_ئۇچۇر_add_to_queued_stats(ئۇچۇر_data, client_ئۇچۇر);
	}وإلا{
		DL_APPEND(ئۇچۇر_data->inflight, client_ئۇچۇر);
		db_ئۇچۇر_add_to_inflight_stats(ئۇچۇر_data, client_ئۇچۇر);
	}

	إذا(persist && context->is_persisted){
		plugin_persist_handle_base_ئۇچۇر_add(client_ئۇچۇر->base_ئۇچۇر);
		plugin_persist_handle_client_ئۇچۇر_add(context, client_ئۇچۇر);
	}

	إذا(client_ئۇچۇر->base_ئۇچۇر->data.qos > 0){
		util__decrement_receive_quota(context);
	}
	أرجع rc;
}


صحيح db_message_قىستۇرۇش_outgoing(بنية mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, بنية mosquitto_base_ئۇچۇر *base_ئۇچۇر, uint32_t subscription_identifier, bool update, bool persist)
{
	بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر;
	بنية mosquitto_ئۇچۇر_data *ئۇچۇر_data;
	تعداد mosquitto_ئۇچۇر_state state = mosq_ms_invalid;
	صحيح rc = 0;
	حرف **dest_ids;

	أكد(base_ئۇچۇر);
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		أرجع MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_sent++;

	ئۇچۇر_data = &context->msgs_out;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	إذا(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && base_ئۇچۇر->dest_ids){

		لكل(صحيح i=0; i<base_ئۇچۇر->dest_id_سانى; i++){
			إذا(base_ئۇچۇر->dest_ids[i] && !strcmp(base_ئۇچۇر->dest_ids[i], context->id)){
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
		}وإلا إذا(qos != 0 && db__ready_for_queue(context, qos, ئۇچۇر_data)){
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
		إذا(db__ready_for_queue(context, qos, ئۇچۇر_data)){
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

	client_ئۇچۇر = mosquitto_malloc(حجم(بنية mosquitto_client_ئۇچۇر));
	إذا(!client_ئۇچۇر){
		أرجع MOSQ_ERR_NOMEM;
	}
	client_ئۇچۇر->prev = عدم;
	client_ئۇچۇر->next = عدم;
	إذا(cmsg_id){
		client_ئۇچۇر->data.cmsg_id = cmsg_id;
	}وإلا{
		client_ئۇچۇر->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_ئۇچۇر->base_ئۇچۇر = base_ئۇچۇر;
	db_ئۇچۇر_store_ref_inc(client_ئۇچۇر->base_ئۇچۇر);
	client_ئۇچۇر->data.mid = mid;
	client_ئۇچۇر->data.direction = mosq_md_out;
	client_ئۇچۇر->data.state = (uint8_t)state;
	client_ئۇچۇر->data.dup = false;
	إذا(qos > context->max_qos){
		client_ئۇچۇر->data.qos = context->max_qos;
	}وإلا{
		client_ئۇچۇر->data.qos = qos;
	}
	client_ئۇچۇر->data.retain = retain;
	client_ئۇچۇر->data.subscription_identifier = subscription_identifier;

	إذا(state == mosq_ms_queued){
		DL_APPEND(ئۇچۇر_data->queued, client_ئۇچۇر);
		db_ئۇچۇر_add_to_queued_stats(ئۇچۇر_data, client_ئۇچۇر);
	}وإلا{
		DL_APPEND(ئۇچۇر_data->inflight, client_ئۇچۇر);
		db_ئۇچۇر_add_to_inflight_stats(ئۇچۇر_data, client_ئۇچۇر);
	}

	إذا(persist && context->is_persisted){
		plugin_persist_handle_base_ئۇچۇر_add(client_ئۇچۇر->base_ئۇچۇر);
		plugin_persist_handle_client_ئۇچۇر_add(context, client_ئۇچۇر);
	}

	إذا(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(base_ئۇچۇر->dest_ids, حجم(حرف *)*(size_t)(base_ئۇچۇر->dest_id_سانى+1));
		إذا(dest_ids){
			base_ئۇچۇر->dest_ids = dest_ids;
			base_ئۇچۇر->dest_id_سانى++;
			base_ئۇچۇر->dest_ids[base_ئۇچۇر->dest_id_سانى-1] = mosquitto_strdup(context->id);
			إذا(!base_ئۇچۇر->dest_ids[base_ئۇچۇر->dest_id_سانى-1]){
				أرجع MOSQ_ERR_NOMEM;
			}
		}وإلا{
			أرجع MOSQ_ERR_NOMEM;
		}
	}
#ifdef WITH_BRIDGE
	إذا(context->bridge && context->bridge->start_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_out.inflight_سانى + context->msgs_out.queued_سانى >= context->bridge->threshold){

		context->bridge->lazy_reconnect = true;
	}
#endif

	إذا(client_ئۇچۇر->data.qos > 0 && state != mosq_ms_queued){
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


ساكن مضمن صحيح db__message_update_outgoing_state(بنية mosquitto *context, بنية mosquitto_client_ئۇچۇر *head,
		uint16_t mid, تعداد mosquitto_ئۇچۇر_state state, صحيح qos, bool persist)
{
	بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر;

	DL_FOREACH(head, client_ئۇچۇر){
		إذا(client_ئۇچۇر->data.mid == mid){
			إذا(client_ئۇچۇر->data.qos != qos){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_ئۇچۇر->data.qos, qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			client_ئۇچۇر->data.state = (uint8_t)state;
			إذا(persist){
				plugin_persist_handle_client_ئۇچۇر_update(context, client_ئۇچۇر);
			}
			أرجع MOSQ_ERR_SUCCESS;
		}
	}
	أرجع MOSQ_ERR_NOT_FOUND;
}


صحيح db__message_update_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_ئۇچۇر_state state, صحيح qos, bool persist)
{
	صحيح rc;

	rc = db__message_update_outgoing_state(context, context->msgs_out.inflight, mid, state, qos, persist);
	إذا(!persist && rc == MOSQ_ERR_NOT_FOUND){
		rc = db__message_update_outgoing_state(context, context->msgs_out.queued, mid, state, qos, persist);
	}
	أرجع rc;
}


ساكن فراغ db__messages_delete_list(بنية mosquitto_client_ئۇچۇر **head)
{
	بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر, *tmp;

	DL_FOREACH_SAFE(*head, client_ئۇچۇر, tmp){
		DL_DELETE(*head, client_ئۇچۇر);
		db_ئۇچۇر_store_ref_dec(&client_ئۇچۇر->base_ئۇچۇر);
		mosquitto_FREE(client_ئۇچۇر);
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
	context->msgs_in.inflight_سانى = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_سانى = 0;
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
	context->msgs_out.inflight_سانى = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_سانى = 0;
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
	بنية mosquitto_base_ئۇچۇر *base_ئۇچۇر;
	ثابت حرف *source_id;
	تعداد mosquitto_ئۇچۇر_origin origin;

	إذا(!topic){
		أرجع MOSQ_ERR_INVAL;
	}

	base_ئۇچۇر = mosquitto_calloc(1, حجم(بنية mosquitto_base_ئۇچۇر));
	إذا(base_ئۇچۇر == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	base_ئۇچۇر->data.topic = mosquitto_strdup(topic);
	إذا(base_ئۇچۇر->data.topic == عدم){
		db_ئۇچۇر_store_free(base_ئۇچۇر);
		أرجع MOSQ_ERR_INVAL;
	}

	base_ئۇچۇر->data.qos = qos;
	إذا(db.config->retain_available == false){
		base_ئۇچۇر->data.retain = 0;
	}وإلا{
		base_ئۇچۇر->data.retain = retain;
	}

	base_ئۇچۇر->data.payloadlen = payloadlen;
	إذا(payloadlen > 0){
		base_ئۇچۇر->data.payload = mosquitto_malloc(base_ئۇچۇر->data.payloadlen+1);
		إذا(base_ئۇچۇر->data.payload == عدم){
			db_ئۇچۇر_store_free(base_ئۇچۇر);
			أرجع MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_ئۇچۇر->data.payload)[base_ئۇچۇر->data.payloadlen] = 0;
		memcpy(base_ئۇچۇر->data.payload, payload, base_ئۇچۇر->data.payloadlen);
	}

	إذا(context && context->id){
		source_id = context->id;
	}وإلا{
		source_id = "";
	}
	إذا(properties){
		base_ئۇچۇر->data.properties = *properties;
		*properties = عدم;
	}

	إذا(context){
		origin = mosq_mo_client;
	}وإلا{
		origin = mosq_mo_broker;
	}
	إذا(db__message_store(context, base_ئۇچۇر, &message_expiry_interval, origin)){
		أرجع 1;
	}

	أرجع sub__messages_queue(source_id, base_ئۇچۇر->data.qos, &base_ئۇچۇر);
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
uint64_t db_new_ئۇچۇر_id(فراغ)
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

	id = db.تۈگۈن_id_shifted; /* Top 10-bits */

#ifdef WIN32
	GetSystemTimePreciseAsFileTime(&ftime);
	ftime64 = (((uint64_t)ftime.dw_يۇقىرى_Date_Time)<<32) + ftime.dw_تۆۋەن_Date_Time;
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
صحيح db__message_store(ثابت بنية mosquitto *source, بنية mosquitto_base_ئۇچۇر *base_ئۇچۇر, uint32_t *message_expiry_interval, تعداد mosquitto_ئۇچۇر_origin origin)
{
	صحيح rc;

	أكد(base_ئۇچۇر);

	إذا(source && source->id){
		base_ئۇچۇر->data.source_id = mosquitto_strdup(source->id);
	}وإلا{
		base_ئۇچۇر->data.source_id = mosquitto_strdup("");
	}
	إذا(!base_ئۇچۇر->data.source_id){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_ئۇچۇر_store_free(base_ئۇچۇر);
		أرجع MOSQ_ERR_NOMEM;
	}

	إذا(source && source->username){
		base_ئۇچۇر->data.source_username = mosquitto_strdup(source->username);
		إذا(!base_ئۇچۇر->data.source_username){
			db_ئۇچۇر_store_free(base_ئۇچۇر);
			أرجع MOSQ_ERR_NOMEM;
		}
	}
	إذا(source){
		base_ئۇچۇر->source_listener = source->listener;
	}
	base_ئۇچۇر->origin = origin;
	إذا(message_expiry_interval){
		إذا(*message_expiry_interval > 0 && *message_expiry_interval != ئۇچۇر_EXPIRY_INFINITE){
			base_ئۇچۇر->data.expiry_time = db.now_real_s + *message_expiry_interval;
		}وإلا{
			base_ئۇچۇر->data.expiry_time = 0;
		}
	}

	base_ئۇچۇر->dest_ids = عدم;
	base_ئۇچۇر->dest_id_سانى = 0;
	db.ئۇچۇر_store_سانى++;
	db.ئۇچۇر_store_bytes += base_ئۇچۇر->data.payloadlen;

	إذا(!base_ئۇچۇر->data.store_id){
		base_ئۇچۇر->data.store_id = db_new_ئۇچۇر_id();
	}

	rc = db_ئۇچۇر_store_add(base_ئۇچۇر);
	إذا(rc){
		db_ئۇچۇر_store_free(base_ئۇچۇر);
		أرجع rc;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__message_store_find(بنية mosquitto *context, uint16_t mid, بنية mosquitto_client_ئۇچۇر **client_ئۇچۇر)
{
	بنية mosquitto_client_ئۇچۇر *cmsg;

	*client_ئۇچۇر = عدم;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_in.inflight, cmsg){
		إذا(cmsg->base_ئۇچۇر->data.source_mid == mid){
			*client_ئۇچۇر = cmsg;
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_in.queued, cmsg){
		إذا(cmsg->base_ئۇچۇر->data.source_mid == mid){
			*client_ئۇچۇر = cmsg;
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	أرجع 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
ساكن صحيح db__message_reconnect_reset_outgoing(بنية mosquitto *context)
{
	بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر, *tmp;

	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_سانى = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_سانى = 0;
	context->msgs_out.queued_count12 = 0;
	context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_ئۇچۇر, tmp){
		db_ئۇچۇر_add_to_inflight_stats(&context->msgs_out, client_ئۇچۇر);
		إذا(client_ئۇچۇر->data.qos > 0){
			util__decrement_send_quota(context);
		}

		بدل(client_ئۇچۇر->data.qos){
			حالة 0:
				client_ئۇچۇر->data.state = mosq_ms_publish_qos0;
				اكسر;
			حالة 1:
				client_ئۇچۇر->data.state = mosq_ms_publish_qos1;
				اكسر;
			حالة 2:
				إذا(client_ئۇچۇر->data.state == mosq_ms_wait_for_pubcomp){
					client_ئۇچۇر->data.state = mosq_ms_resend_pubrel;
				}وإلا{
					client_ئۇچۇر->data.state = mosq_ms_publish_qos2;
				}
				اكسر;
		}
		plugin_persist_handle_client_ئۇچۇر_update(context, client_ئۇچۇر);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_out.queued, client_ئۇچۇر, tmp){
		db_ئۇچۇر_add_to_queued_stats(&context->msgs_out, client_ئۇچۇر);
	}
	db__fill_inflight_out_from_queue(context);

	أرجع MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
ساكن صحيح db__message_reconnect_reset_incoming(بنية mosquitto *context)
{
	بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر, *tmp;

	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_سانى = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_سانى = 0;
	context->msgs_in.queued_count12 = 0;
	context->msgs_in.inflight_quota = context->msgs_in.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_ئۇچۇر, tmp){
		db_ئۇچۇر_add_to_inflight_stats(&context->msgs_in, client_ئۇچۇر);
		إذا(client_ئۇچۇر->data.qos > 0){
			util__decrement_receive_quota(context);
		}

		إذا(client_ئۇچۇر->data.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db__message_remove_inflight(context, &context->msgs_in, client_ئۇچۇر);
		}وإلا{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_ئۇچۇر->data.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_in.queued, client_ئۇچۇر, tmp){
		client_ئۇچۇر->data.dup = 0;
		db_ئۇچۇر_add_to_queued_stats(&context->msgs_in, client_ئۇچۇر);
		إذا(db__ready_for_flight(context, mosq_md_in, client_ئۇچۇر->data.qos)){
			بدل(client_ئۇچۇر->data.qos){
				حالة 0:
					client_ئۇچۇر->data.state = mosq_ms_publish_qos0;
					اكسر;
				حالة 1:
					client_ئۇچۇر->data.state = mosq_ms_publish_qos1;
					اكسر;
				حالة 2:
					client_ئۇچۇر->data.state = mosq_ms_publish_qos2;
					اكسر;
			}
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_ئۇچۇر_update(context, client_ئۇچۇر);
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
	بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر, *tmp;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_ئۇچۇر, tmp){
		إذا(client_ئۇچۇر->data.mid == mid){
			إذا(client_ئۇچۇر->base_ئۇچۇر->data.qos != 2){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_ئۇچۇر->base_ئۇچۇر->data.qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_in, client_ئۇچۇر);
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	أرجع MOSQ_ERR_NOT_FOUND;
}


صحيح db__message_release_incoming(بنية mosquitto *context, uint16_t mid)
{
	بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر, *tmp;
	bool deleted = false;
	صحيح rc;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_ئۇچۇر, tmp){
		إذا(client_ئۇچۇر->data.mid == mid){
			إذا(client_ئۇچۇر->base_ئۇچۇر->data.qos != 2){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_ئۇچۇر->base_ئۇچۇر->data.qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			ثابت حرف *topic = client_ئۇچۇر->base_ئۇچۇر->data.topic;
			ثابت حرف *source_id = client_ئۇچۇر->base_ئۇچۇر->data.source_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			إذا(topic == عدم){
				db__message_remove_inflight(context, &context->msgs_in, client_ئۇچۇر);
				deleted = true;
			}وإلا{
				rc = sub__messages_queue(source_id, 2, &client_ئۇچۇر->base_ئۇچۇر);
				إذا(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db__message_remove_inflight(context, &context->msgs_in, client_ئۇچۇر);
					deleted = true;
				}وإلا{
					أرجع 1;
				}
			}
		}
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_ئۇچۇر, tmp){
		إذا(db__ready_for_flight(context, mosq_md_in, client_ئۇچۇر->data.qos)){
			اكسر;
		}

		إذا(client_ئۇچۇر->data.qos == 2){
			send__pubrec(context, client_ئۇچۇر->data.mid, 0, عدم);
			client_ئۇچۇر->data.state = mosq_ms_wait_for_pubrel;
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_ئۇچۇر_update(context, client_ئۇچۇر);
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
	بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_ئۇچۇر, tmp){
		إذا(client_ئۇچۇر->base_ئۇچۇر->data.expiry_time && db.now_real_s > client_ئۇچۇر->base_ئۇچۇر->data.expiry_time){
			إذا(client_ئۇچۇر->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_ئۇچۇر);
		}
	}
	db__fill_inflight_out_from_queue(context);
	DL_FOREACH_SAFE(context->msgs_out.queued, client_ئۇچۇر, tmp){
		إذا(client_ئۇچۇر->base_ئۇچۇر->data.expiry_time && db.now_real_s > client_ئۇچۇر->base_ئۇچۇر->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_ئۇچۇر);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.inflight, client_ئۇچۇر, tmp){
		إذا(client_ئۇچۇر->base_ئۇچۇر->data.expiry_time && db.now_real_s > client_ئۇچۇر->base_ئۇچۇر->data.expiry_time){
			إذا(client_ئۇچۇر->data.qos > 0){
				util__increment_receive_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_in, client_ئۇچۇر);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.queued, client_ئۇچۇر, tmp){
		إذا(client_ئۇچۇر->base_ئۇچۇر->data.expiry_time && db.now_real_s > client_ئۇچۇر->base_ئۇچۇر->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_in, client_ئۇچۇر);
		}
	}
}


ساكن فراغ db__client_messages_check_acl(بنية mosquitto *context, بنية mosquitto_ئۇچۇر_data *ئۇچۇر_data, بنية mosquitto_client_ئۇچۇر **head,
		فراغ (*decrement_stats_fn)(بنية mosquitto_ئۇچۇر_data *ئۇچۇر_data, بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر))
{
	بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر, *tmp;
	بنية mosquitto_base_ئۇچۇر *base_ئۇچۇر;
	صحيح access;

	DL_FOREACH_SAFE((*head), client_ئۇچۇر, tmp){
		base_ئۇچۇر = client_ئۇچۇر->base_ئۇچۇر;
		إذا(client_ئۇچۇر->data.direction == mosq_md_out){
			access = MOSQ_ACL_READ;
		}وإلا{
			access = MOSQ_ACL_WRITE;
		}
		إذا(mosquitto_acl_check(context, base_ئۇچۇر->data.topic,
				base_ئۇچۇر->data.payloadlen, base_ئۇچۇر->data.payload,
				base_ئۇچۇر->data.qos, base_ئۇچۇر->data.retain,
				base_ئۇچۇر->data.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*head), client_ئۇچۇر);
			decrement_stats_fn(ئۇچۇر_data, client_ئۇچۇر);
			plugin_persist_handle_client_ئۇچۇر_delete(context, client_ئۇچۇر);
			db_ئۇچۇر_store_ref_dec(&client_ئۇچۇر->base_ئۇچۇر);
			mosquitto_FREE(client_ئۇچۇر);
		}
	}
}


فراغ db__check_acl_of_all_messages(بنية mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.inflight, &db_ئۇچۇر_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.queued, &db_ئۇچۇر_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.inflight, &db_ئۇچۇر_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.queued, &db_ئۇچۇر_remove_from_queued_stats);
}


ساكن صحيح db__message_write_inflight_out_single(بنية mosquitto *context, بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر)
{
	بنية mosquitto_base_ئۇچۇر *base_ئۇچۇر;
	mosquitto_property *base_ئۇچۇر_props = عدم;
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

	base_ئۇچۇر = client_ئۇچۇر->base_ئۇچۇر;

	expiry_interval = 0;
	إذا(base_ئۇچۇر->data.expiry_time){
		إذا(db.now_real_s > base_ئۇچۇر->data.expiry_time){
			/* Message is expired, must not send. */
			إذا(client_ئۇچۇر->data.direction == mosq_md_out && client_ئۇچۇر->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_ئۇچۇر);
			db__fill_inflight_out_from_queue(context);
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			expiry_interval = (uint32_t)(base_ئۇچۇر->data.expiry_time - db.now_real_s);
		}
	}
	mid = client_ئۇچۇر->data.mid;
	retries = client_ئۇچۇر->data.dup;
	retain = client_ئۇچۇر->data.retain;
	topic = base_ئۇچۇر->data.topic;
	qos = (uint8_t)client_ئۇچۇر->data.qos;
	payloadlen = base_ئۇچۇر->data.payloadlen;
	payload = base_ئۇچۇر->data.payload;
	subscription_id = client_ئۇچۇر->data.subscription_identifier;
	base_ئۇچۇر_props = base_ئۇچۇر->data.properties;

	بدل(client_ئۇچۇر->data.state){
		حالة mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_ئۇچۇر_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_ئۇچۇر);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_ئۇچۇر_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS){
				client_ئۇچۇر->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_ئۇچۇر->data.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_ئۇچۇر_update(context, client_ئۇچۇر);
			}وإلا إذا(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_ئۇچۇر);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_ئۇچۇر_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS){
				client_ئۇچۇر->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_ئۇچۇر->data.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_ئۇچۇر_update(context, client_ئۇچۇر);
			}وإلا إذا(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_ئۇچۇر);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, عدم);
			إذا(!rc){
				client_ئۇچۇر->data.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_ئۇچۇر_update(context, client_ئۇچۇر);
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
	بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر, *tmp;
	صحيح rc;

	إذا(context->state != mosq_cs_active || !net__is_connected(context)){
		أرجع MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_ئۇچۇر, tmp){
		rc = db__message_write_inflight_out_single(context, client_ئۇچۇر);
		إذا(rc){
			أرجع rc;
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__message_write_inflight_out_latest(بنية mosquitto *context)
{
	بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر, *next;
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
	client_ئۇچۇر = context->msgs_out.inflight->prev;
	طالما(client_ئۇچۇر != context->msgs_out.inflight &&
			(client_ئۇچۇر->data.state == mosq_ms_publish_qos0
			|| client_ئۇچۇر->data.state == mosq_ms_publish_qos1
			|| client_ئۇچۇر->data.state == mosq_ms_publish_qos2)){

		client_ئۇچۇر = client_ئۇچۇر->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	إذا(client_ئۇچۇر != context->msgs_out.inflight){
		client_ئۇچۇر = client_ئۇچۇر->next;
	}

	طالما(client_ئۇچۇر){
		next = client_ئۇچۇر->next;
		rc = db__message_write_inflight_out_single(context, client_ئۇچۇر);
		إذا(rc){
			أرجع rc;
		}
		client_ئۇچۇر = next;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__message_write_queued_in(بنية mosquitto *context)
{
	بنية mosquitto_client_ئۇچۇر *client_ئۇچۇر, *tmp;
	صحيح rc;

	إذا(context->state != mosq_cs_active){
		أرجع MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_ئۇچۇر, tmp){
		إذا(context->msgs_in.inflight_maximum != 0 && context->msgs_in.inflight_quota == 0){
			اكسر;
		}

		إذا(client_ئۇچۇر->data.qos == 2){
			client_ئۇچۇر->data.state = mosq_ms_send_pubrec;
			db__message_dequeue_first(context, &context->msgs_in);
			rc = send__pubrec(context, client_ئۇچۇر->data.mid, 0, عدم);
			إذا(!rc){
				client_ئۇچۇر->data.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_ئۇچۇر_update(context, client_ئۇچۇر);
			}وإلا{
				plugin_persist_handle_client_ئۇچۇر_update(context, client_ئۇچۇر);
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
