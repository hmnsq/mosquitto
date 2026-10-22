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
bool db__ready_for_flight(بنية mosquitto *context, تعداد mosquitto_رسالة_direction dir, صحيح qos)
{
	بنية mosquitto_رسالة_data *msgs;
	bool valid_bytes;
	bool valid_عدد;

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
			valid_عدد = context->out_packet_عدد < db.config->max_queued_messages;
		}وإلا{
			valid_عدد = msgs->inflight_عدد - msgs->inflight_maximum < db.config->max_queued_messages;
		}

		إذا(db.config->max_queued_messages == 0){
			أرجع valid_bytes;
		}
		إذا(db.config->max_queued_bytes == 0){
			أرجع valid_عدد;
		}
	}وإلا{
		valid_bytes = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_bytes;
		valid_عدد = msgs->inflight_quota > 0;

		إذا(msgs->inflight_maximum == 0){
			أرجع valid_bytes;
		}
		إذا(db.config->max_inflight_bytes == 0){
			أرجع valid_عدد;
		}
	}

	أرجع valid_bytes && valid_عدد;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db__ready_for_queue(بنية mosquitto *context, صحيح qos, بنية mosquitto_رسالة_data *رسالة_data)
{
	صحيح source_عدد;
	صحيح adjust_عدد;
	long source_bytes;
	ssize_t adjust_bytes = (ssize_t)db.config->max_inflight_bytes;
	bool valid_bytes;
	bool valid_عدد;

	إذا(db.config->max_queued_messages == 0 && db.config->max_queued_bytes == 0){
		أرجع true;
	}

	إذا(qos == 0 && db.config->queue_qos0_messages == false){
		أرجع false; /* This case is handled in db__ready_for_flight() */
	}وإلا{
		source_bytes = (ssize_t)رسالة_data->queued_bytes12;
		source_عدد = رسالة_data->queued_count12;
	}
	adjust_عدد = رسالة_data->inflight_maximum;

	/* nothing in flight for offline clients */
	إذا(!net__is_connected(context)){
		adjust_bytes = 0;
		adjust_عدد = 0;
	}

	valid_bytes = (source_bytes - (ssize_t)adjust_bytes) < (ssize_t)db.config->max_queued_bytes;
	valid_عدد = source_عدد - adjust_عدد < db.config->max_queued_messages;

	إذا(db.config->max_queued_bytes == 0){
		أرجع valid_عدد;
	}
	إذا(db.config->max_queued_messages == 0){
		أرجع valid_bytes;
	}

	أرجع valid_bytes && valid_عدد;
}


فراغ db_رسالة_add_to_inflight_stats(بنية mosquitto_رسالة_data *رسالة_data, بنية mosquitto_client_رسالة *client_رسالة)
{
	رسالة_data->inflight_عدد++;
	رسالة_data->inflight_bytes += client_رسالة->base_رسالة->data.payloadlen;
	إذا(client_رسالة->data.qos != 0){
		رسالة_data->inflight_count12++;
		رسالة_data->inflight_bytes12 += client_رسالة->base_رسالة->data.payloadlen;
	}
}


ساكن فراغ db_رسالة_remove_from_inflight_stats(بنية mosquitto_رسالة_data *رسالة_data, بنية mosquitto_client_رسالة *client_رسالة)
{
	رسالة_data->inflight_عدد--;
	رسالة_data->inflight_bytes -= client_رسالة->base_رسالة->data.payloadlen;
	إذا(client_رسالة->data.qos != 0){
		رسالة_data->inflight_count12--;
		رسالة_data->inflight_bytes12 -= client_رسالة->base_رسالة->data.payloadlen;
	}
}


فراغ db_رسالة_add_to_queued_stats(بنية mosquitto_رسالة_data *رسالة_data, بنية mosquitto_client_رسالة *client_رسالة)
{
	رسالة_data->queued_عدد++;
	رسالة_data->queued_bytes += client_رسالة->base_رسالة->data.payloadlen;
	إذا(client_رسالة->data.qos != 0){
		رسالة_data->queued_count12++;
		رسالة_data->queued_bytes12 += client_رسالة->base_رسالة->data.payloadlen;
	}
}


ساكن فراغ db_رسالة_remove_from_queued_stats(بنية mosquitto_رسالة_data *رسالة_data, بنية mosquitto_client_رسالة *client_رسالة)
{
	رسالة_data->queued_عدد--;
	رسالة_data->queued_bytes -= client_رسالة->base_رسالة->data.payloadlen;
	إذا(client_رسالة->data.qos != 0){
		رسالة_data->queued_count12--;
		رسالة_data->queued_bytes12 -= client_رسالة->base_رسالة->data.payloadlen;
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
	db.bridge_عدد = 0;
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
	db_رسالة_store_clean();

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_رسالة_store_add(بنية mosquitto_base_رسالة *base_رسالة)
{
	بنية mosquitto_base_رسالة *found;
	unsigned hashv;

	HASH_قيمة(&base_رسالة->data.store_id, حجم(base_رسالة->data.store_id), hashv);
	HASH_FIND_BYHASHVALUE(hh, db.رسالة_store, &base_رسالة->data.store_id, حجم(base_رسالة->data.store_id), hashv, found);
	إذا(found == عدم){
		HASH_ADD_KEYPTR_BYHASHVALUE(hh, db.رسالة_store, &base_رسالة->data.store_id, حجم(base_رسالة->data.store_id), hashv, base_رسالة);
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_ALREADY_EXISTS;
	}
}


فراغ db_رسالة_store_free(بنية mosquitto_base_رسالة *base_رسالة)
{
	mosquitto_FREE(base_رسالة->data.source_id);
	mosquitto_FREE(base_رسالة->data.source_username);
	إذا(base_رسالة->dest_ids){
		لكل(صحيح i=0; i<base_رسالة->dest_id_عدد; i++){
			mosquitto_FREE(base_رسالة->dest_ids[i]);
		}
		mosquitto_FREE(base_رسالة->dest_ids);
	}
	mosquitto_FREE(base_رسالة->data.topic);
	mosquitto_property_free_all(&base_رسالة->data.properties);
	mosquitto_FREE(base_رسالة->data.payload);
	mosquitto_FREE(base_رسالة);
}


فراغ db_رسالة_store_remove(بنية mosquitto_base_رسالة *base_رسالة, bool notify)
{
	إذا(base_رسالة == عدم){
		أرجع;
	}
	HASH_DELETE(hh, db.رسالة_store, base_رسالة);
	db.رسالة_store_عدد--;
	db.رسالة_store_bytes -= base_رسالة->data.payloadlen;
	إذا(notify == true){
		plugin_persist_handle_base_رسالة_delete(base_رسالة);
	}
	db_رسالة_store_free(base_رسالة);
}


فراغ db_رسالة_store_clean(فراغ)
{
	بنية mosquitto_base_رسالة *base_رسالة, *base_رسالة_tmp;

	HASH_ITER(hh, db.رسالة_store, base_رسالة, base_رسالة_tmp){
		db_رسالة_store_remove(base_رسالة, false);
	}
}


فراغ db_رسالة_store_ref_inc(بنية mosquitto_base_رسالة *base_رسالة)
{
	base_رسالة->ref_عدد++;
}


فراغ db_رسالة_store_ref_dec(بنية mosquitto_base_رسالة **base_رسالة)
{
	(*base_رسالة)->ref_عدد--;
	إذا((*base_رسالة)->ref_عدد == 0){
		db_رسالة_store_remove(*base_رسالة, true);
		*base_رسالة = عدم;
	}
}


فراغ db_رسالة_store_compact(فراغ)
{
	بنية mosquitto_base_رسالة *base_رسالة, *base_رسالة_tmp;

	HASH_ITER(hh, db.رسالة_store, base_رسالة, base_رسالة_tmp){
		إذا(base_رسالة->ref_عدد < 1){
			db_رسالة_store_remove(base_رسالة, true);
		}
	}
}


ساكن فراغ db__message_remove_inflight(بنية mosquitto *context, بنية mosquitto_رسالة_data *رسالة_data, بنية mosquitto_client_رسالة *item)
{
	إذا(!context || !رسالة_data || !item){
		أرجع;
	}

	plugin_persist_handle_client_رسالة_delete(context, item);

	DL_DELETE(رسالة_data->inflight, item);
	إذا(item->base_رسالة){
		db_رسالة_remove_from_inflight_stats(رسالة_data, item);
		db_رسالة_store_ref_dec(&item->base_رسالة);
	}

	mosquitto_FREE(item);
}


ساكن فراغ db__message_remove_queued(بنية mosquitto *context, بنية mosquitto_رسالة_data *رسالة_data, بنية mosquitto_client_رسالة *item)
{
	إذا(!context || !رسالة_data || !item){
		أرجع;
	}

	plugin_persist_handle_client_رسالة_delete(context, item);

	DL_DELETE(رسالة_data->queued, item);
	إذا(item->base_رسالة){
		db_رسالة_remove_from_queued_stats(رسالة_data, item);
		db_رسالة_store_ref_dec(&item->base_رسالة);
	}

	mosquitto_FREE(item);
}


ساكن فراغ db__fill_inflight_out_from_queue(بنية mosquitto *context)
{
	بنية mosquitto_client_رسالة *client_رسالة, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.queued, client_رسالة, tmp){
		إذا(!db__ready_for_flight(context, mosq_md_out, client_رسالة->data.qos)){
			أرجع;
		}
		بدل(client_رسالة->data.qos){
			حالة 0:
				client_رسالة->data.state = mosq_ms_publish_qos0;
				اكسر;
			حالة 1:
				client_رسالة->data.state = mosq_ms_publish_qos1;
				اكسر;
			حالة 2:
				client_رسالة->data.state = mosq_ms_publish_qos2;
				اكسر;
		}
		إذا(client_رسالة->base_رسالة->data.expiry_time && db.now_real_s > client_رسالة->base_رسالة->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_رسالة);
			استمر;
		}
		plugin_persist_handle_client_رسالة_update(context, client_رسالة);
		db__message_dequeue_first(context, &context->msgs_out);
	}
}


فراغ db__message_dequeue_first(بنية mosquitto *context, بنية mosquitto_رسالة_data *رسالة_data)
{
	بنية mosquitto_client_رسالة *client_رسالة;

	UNUSED(context);

	client_رسالة = رسالة_data->queued;
	DL_DELETE(رسالة_data->queued, client_رسالة);
	DL_APPEND(رسالة_data->inflight, client_رسالة);
	إذا(رسالة_data->inflight_quota > 0){
		رسالة_data->inflight_quota--;
	}

	db_رسالة_remove_from_queued_stats(رسالة_data, client_رسالة);
	db_رسالة_add_to_inflight_stats(رسالة_data, client_رسالة);
}


صحيح db__message_delete_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_رسالة_state expect_state, صحيح qos)
{
	بنية mosquitto_client_رسالة *client_رسالة, *tmp;
	bool deleted = false;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_رسالة, tmp){
		إذا(client_رسالة->data.mid == mid){
			إذا(client_رسالة->data.qos != qos){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_رسالة->data.qos, qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}وإلا إذا(qos == 2 && client_رسالة->data.state != expect_state && expect_state != mosq_ms_any){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_رسالة->data.state, expect_state);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_out, client_رسالة);
			deleted = true;
			اكسر;
		}
	}

	إذا(deleted == false){
		DL_FOREACH_SAFE(context->msgs_out.queued, client_رسالة, tmp){
			إذا(client_رسالة->data.mid == mid){
				إذا(client_رسالة->data.qos != qos){
					أرجع MOSQ_ERR_PROTOCOL;
				}وإلا إذا(qos == 2 && client_رسالة->data.state != expect_state && expect_state != mosq_ms_any){
					أرجع MOSQ_ERR_PROTOCOL;
				}
				db__message_remove_queued(context, &context->msgs_out, client_رسالة);
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
صحيح db_message_أدرج_incoming(بنية mosquitto *context, uint64_t cmsg_id, بنية mosquitto_base_رسالة *base_رسالة, bool persist)
{
	بنية mosquitto_client_رسالة *client_رسالة;
	بنية mosquitto_رسالة_data *رسالة_data;
	تعداد mosquitto_رسالة_state state = mosq_ms_invalid;
	صحيح rc = 0;

	أكد(base_رسالة);
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		أرجع MOSQ_ERR_SUCCESS;

	}
	رسالة_data = &context->msgs_in;

	إذا(db__ready_for_flight(context, mosq_md_in, base_رسالة->data.qos)){
		state = mosq_ms_wait_for_pubrel;
	}وإلا إذا(base_رسالة->data.qos != 0 && db__ready_for_queue(context, base_رسالة->data.qos, رسالة_data)){
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

	client_رسالة = mosquitto_malloc(حجم(بنية mosquitto_client_رسالة));
	إذا(!client_رسالة){
		أرجع MOSQ_ERR_NOMEM;
	}
	client_رسالة->prev = عدم;
	client_رسالة->next = عدم;
	إذا(cmsg_id){
		client_رسالة->data.cmsg_id = cmsg_id;
	}وإلا{
		client_رسالة->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_رسالة->base_رسالة = base_رسالة;
	db_رسالة_store_ref_inc(client_رسالة->base_رسالة);
	client_رسالة->data.mid = base_رسالة->data.source_mid;
	client_رسالة->data.direction = mosq_md_in;
	client_رسالة->data.state = (uint8_t)state;
	client_رسالة->data.dup = false;
	إذا(base_رسالة->data.qos > context->max_qos){
		client_رسالة->data.qos = context->max_qos;
	}وإلا{
		client_رسالة->data.qos = base_رسالة->data.qos;
	}
	client_رسالة->data.retain = base_رسالة->data.retain;
	client_رسالة->data.subscription_identifier = 0;

	إذا(state == mosq_ms_queued){
		DL_APPEND(رسالة_data->queued, client_رسالة);
		db_رسالة_add_to_queued_stats(رسالة_data, client_رسالة);
	}وإلا{
		DL_APPEND(رسالة_data->inflight, client_رسالة);
		db_رسالة_add_to_inflight_stats(رسالة_data, client_رسالة);
	}

	إذا(persist && context->is_persisted){
		plugin_persist_handle_base_رسالة_add(client_رسالة->base_رسالة);
		plugin_persist_handle_client_رسالة_add(context, client_رسالة);
	}

	إذا(client_رسالة->base_رسالة->data.qos > 0){
		util__decrement_receive_quota(context);
	}
	أرجع rc;
}


صحيح db_message_أدرج_outgoing(بنية mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, بنية mosquitto_base_رسالة *base_رسالة, uint32_t subscription_identifier, bool update, bool persist)
{
	بنية mosquitto_client_رسالة *client_رسالة;
	بنية mosquitto_رسالة_data *رسالة_data;
	تعداد mosquitto_رسالة_state state = mosq_ms_invalid;
	صحيح rc = 0;
	حرف **dest_ids;

	أكد(base_رسالة);
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		أرجع MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_sent++;

	رسالة_data = &context->msgs_out;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	إذا(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && base_رسالة->dest_ids){

		لكل(صحيح i=0; i<base_رسالة->dest_id_عدد; i++){
			إذا(base_رسالة->dest_ids[i] && !strcmp(base_رسالة->dest_ids[i], context->id)){
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
		}وإلا إذا(qos != 0 && db__ready_for_queue(context, qos, رسالة_data)){
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
		إذا(db__ready_for_queue(context, qos, رسالة_data)){
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

	client_رسالة = mosquitto_malloc(حجم(بنية mosquitto_client_رسالة));
	إذا(!client_رسالة){
		أرجع MOSQ_ERR_NOMEM;
	}
	client_رسالة->prev = عدم;
	client_رسالة->next = عدم;
	إذا(cmsg_id){
		client_رسالة->data.cmsg_id = cmsg_id;
	}وإلا{
		client_رسالة->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_رسالة->base_رسالة = base_رسالة;
	db_رسالة_store_ref_inc(client_رسالة->base_رسالة);
	client_رسالة->data.mid = mid;
	client_رسالة->data.direction = mosq_md_out;
	client_رسالة->data.state = (uint8_t)state;
	client_رسالة->data.dup = false;
	إذا(qos > context->max_qos){
		client_رسالة->data.qos = context->max_qos;
	}وإلا{
		client_رسالة->data.qos = qos;
	}
	client_رسالة->data.retain = retain;
	client_رسالة->data.subscription_identifier = subscription_identifier;

	إذا(state == mosq_ms_queued){
		DL_APPEND(رسالة_data->queued, client_رسالة);
		db_رسالة_add_to_queued_stats(رسالة_data, client_رسالة);
	}وإلا{
		DL_APPEND(رسالة_data->inflight, client_رسالة);
		db_رسالة_add_to_inflight_stats(رسالة_data, client_رسالة);
	}

	إذا(persist && context->is_persisted){
		plugin_persist_handle_base_رسالة_add(client_رسالة->base_رسالة);
		plugin_persist_handle_client_رسالة_add(context, client_رسالة);
	}

	إذا(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(base_رسالة->dest_ids, حجم(حرف *)*(size_t)(base_رسالة->dest_id_عدد+1));
		إذا(dest_ids){
			base_رسالة->dest_ids = dest_ids;
			base_رسالة->dest_id_عدد++;
			base_رسالة->dest_ids[base_رسالة->dest_id_عدد-1] = mosquitto_strdup(context->id);
			إذا(!base_رسالة->dest_ids[base_رسالة->dest_id_عدد-1]){
				أرجع MOSQ_ERR_NOMEM;
			}
		}وإلا{
			أرجع MOSQ_ERR_NOMEM;
		}
	}
#ifdef WITH_BRIDGE
	إذا(context->bridge && context->bridge->start_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_out.inflight_عدد + context->msgs_out.queued_عدد >= context->bridge->threshold){

		context->bridge->lazy_reconnect = true;
	}
#endif

	إذا(client_رسالة->data.qos > 0 && state != mosq_ms_queued){
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


ساكن مضمن صحيح db__message_update_outgoing_state(بنية mosquitto *context, بنية mosquitto_client_رسالة *head,
		uint16_t mid, تعداد mosquitto_رسالة_state state, صحيح qos, bool persist)
{
	بنية mosquitto_client_رسالة *client_رسالة;

	DL_FOREACH(head, client_رسالة){
		إذا(client_رسالة->data.mid == mid){
			إذا(client_رسالة->data.qos != qos){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_رسالة->data.qos, qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			client_رسالة->data.state = (uint8_t)state;
			إذا(persist){
				plugin_persist_handle_client_رسالة_update(context, client_رسالة);
			}
			أرجع MOSQ_ERR_SUCCESS;
		}
	}
	أرجع MOSQ_ERR_NOT_FOUND;
}


صحيح db__message_update_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_رسالة_state state, صحيح qos, bool persist)
{
	صحيح rc;

	rc = db__message_update_outgoing_state(context, context->msgs_out.inflight, mid, state, qos, persist);
	إذا(!persist && rc == MOSQ_ERR_NOT_FOUND){
		rc = db__message_update_outgoing_state(context, context->msgs_out.queued, mid, state, qos, persist);
	}
	أرجع rc;
}


ساكن فراغ db__messages_delete_list(بنية mosquitto_client_رسالة **head)
{
	بنية mosquitto_client_رسالة *client_رسالة, *tmp;

	DL_FOREACH_SAFE(*head, client_رسالة, tmp){
		DL_DELETE(*head, client_رسالة);
		db_رسالة_store_ref_dec(&client_رسالة->base_رسالة);
		mosquitto_FREE(client_رسالة);
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
	context->msgs_in.inflight_عدد = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_عدد = 0;
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
	context->msgs_out.inflight_عدد = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_عدد = 0;
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
	بنية mosquitto_base_رسالة *base_رسالة;
	ثابت حرف *source_id;
	تعداد mosquitto_رسالة_origin origin;

	إذا(!topic){
		أرجع MOSQ_ERR_INVAL;
	}

	base_رسالة = mosquitto_calloc(1, حجم(بنية mosquitto_base_رسالة));
	إذا(base_رسالة == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	base_رسالة->data.topic = mosquitto_strdup(topic);
	إذا(base_رسالة->data.topic == عدم){
		db_رسالة_store_free(base_رسالة);
		أرجع MOSQ_ERR_INVAL;
	}

	base_رسالة->data.qos = qos;
	إذا(db.config->retain_available == false){
		base_رسالة->data.retain = 0;
	}وإلا{
		base_رسالة->data.retain = retain;
	}

	base_رسالة->data.payloadlen = payloadlen;
	إذا(payloadlen > 0){
		base_رسالة->data.payload = mosquitto_malloc(base_رسالة->data.payloadlen+1);
		إذا(base_رسالة->data.payload == عدم){
			db_رسالة_store_free(base_رسالة);
			أرجع MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_رسالة->data.payload)[base_رسالة->data.payloadlen] = 0;
		memcpy(base_رسالة->data.payload, payload, base_رسالة->data.payloadlen);
	}

	إذا(context && context->id){
		source_id = context->id;
	}وإلا{
		source_id = "";
	}
	إذا(properties){
		base_رسالة->data.properties = *properties;
		*properties = عدم;
	}

	إذا(context){
		origin = mosq_mo_client;
	}وإلا{
		origin = mosq_mo_broker;
	}
	إذا(db__message_store(context, base_رسالة, &message_expiry_interval, origin)){
		أرجع 1;
	}

	أرجع sub__messages_queue(source_id, base_رسالة->data.qos, &base_رسالة);
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
uint64_t db_new_رسالة_id(فراغ)
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

	id = db.عقدة_id_shifted; /* Top 10-bits */

#ifdef WIN32
	GetSystemTimePreciseAsFileTime(&ftime);
	ftime64 = (((uint64_t)ftime.dw_عالي_Date_Time)<<32) + ftime.dw_أدنى_Date_Time;
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
صحيح db__message_store(ثابت بنية mosquitto *source, بنية mosquitto_base_رسالة *base_رسالة, uint32_t *message_expiry_interval, تعداد mosquitto_رسالة_origin origin)
{
	صحيح rc;

	أكد(base_رسالة);

	إذا(source && source->id){
		base_رسالة->data.source_id = mosquitto_strdup(source->id);
	}وإلا{
		base_رسالة->data.source_id = mosquitto_strdup("");
	}
	إذا(!base_رسالة->data.source_id){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_رسالة_store_free(base_رسالة);
		أرجع MOSQ_ERR_NOMEM;
	}

	إذا(source && source->username){
		base_رسالة->data.source_username = mosquitto_strdup(source->username);
		إذا(!base_رسالة->data.source_username){
			db_رسالة_store_free(base_رسالة);
			أرجع MOSQ_ERR_NOMEM;
		}
	}
	إذا(source){
		base_رسالة->source_listener = source->listener;
	}
	base_رسالة->origin = origin;
	إذا(message_expiry_interval){
		إذا(*message_expiry_interval > 0 && *message_expiry_interval != رسالة_EXPIRY_INFINITE){
			base_رسالة->data.expiry_time = db.now_real_s + *message_expiry_interval;
		}وإلا{
			base_رسالة->data.expiry_time = 0;
		}
	}

	base_رسالة->dest_ids = عدم;
	base_رسالة->dest_id_عدد = 0;
	db.رسالة_store_عدد++;
	db.رسالة_store_bytes += base_رسالة->data.payloadlen;

	إذا(!base_رسالة->data.store_id){
		base_رسالة->data.store_id = db_new_رسالة_id();
	}

	rc = db_رسالة_store_add(base_رسالة);
	إذا(rc){
		db_رسالة_store_free(base_رسالة);
		أرجع rc;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__message_store_find(بنية mosquitto *context, uint16_t mid, بنية mosquitto_client_رسالة **client_رسالة)
{
	بنية mosquitto_client_رسالة *cmsg;

	*client_رسالة = عدم;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_in.inflight, cmsg){
		إذا(cmsg->base_رسالة->data.source_mid == mid){
			*client_رسالة = cmsg;
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_in.queued, cmsg){
		إذا(cmsg->base_رسالة->data.source_mid == mid){
			*client_رسالة = cmsg;
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	أرجع 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
ساكن صحيح db__message_reconnect_reset_outgoing(بنية mosquitto *context)
{
	بنية mosquitto_client_رسالة *client_رسالة, *tmp;

	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_عدد = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_عدد = 0;
	context->msgs_out.queued_count12 = 0;
	context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_رسالة, tmp){
		db_رسالة_add_to_inflight_stats(&context->msgs_out, client_رسالة);
		إذا(client_رسالة->data.qos > 0){
			util__decrement_send_quota(context);
		}

		بدل(client_رسالة->data.qos){
			حالة 0:
				client_رسالة->data.state = mosq_ms_publish_qos0;
				اكسر;
			حالة 1:
				client_رسالة->data.state = mosq_ms_publish_qos1;
				اكسر;
			حالة 2:
				إذا(client_رسالة->data.state == mosq_ms_wait_for_pubcomp){
					client_رسالة->data.state = mosq_ms_resend_pubrel;
				}وإلا{
					client_رسالة->data.state = mosq_ms_publish_qos2;
				}
				اكسر;
		}
		plugin_persist_handle_client_رسالة_update(context, client_رسالة);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_out.queued, client_رسالة, tmp){
		db_رسالة_add_to_queued_stats(&context->msgs_out, client_رسالة);
	}
	db__fill_inflight_out_from_queue(context);

	أرجع MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
ساكن صحيح db__message_reconnect_reset_incoming(بنية mosquitto *context)
{
	بنية mosquitto_client_رسالة *client_رسالة, *tmp;

	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_عدد = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_عدد = 0;
	context->msgs_in.queued_count12 = 0;
	context->msgs_in.inflight_quota = context->msgs_in.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_رسالة, tmp){
		db_رسالة_add_to_inflight_stats(&context->msgs_in, client_رسالة);
		إذا(client_رسالة->data.qos > 0){
			util__decrement_receive_quota(context);
		}

		إذا(client_رسالة->data.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db__message_remove_inflight(context, &context->msgs_in, client_رسالة);
		}وإلا{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_رسالة->data.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_in.queued, client_رسالة, tmp){
		client_رسالة->data.dup = 0;
		db_رسالة_add_to_queued_stats(&context->msgs_in, client_رسالة);
		إذا(db__ready_for_flight(context, mosq_md_in, client_رسالة->data.qos)){
			بدل(client_رسالة->data.qos){
				حالة 0:
					client_رسالة->data.state = mosq_ms_publish_qos0;
					اكسر;
				حالة 1:
					client_رسالة->data.state = mosq_ms_publish_qos1;
					اكسر;
				حالة 2:
					client_رسالة->data.state = mosq_ms_publish_qos2;
					اكسر;
			}
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_رسالة_update(context, client_رسالة);
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
	بنية mosquitto_client_رسالة *client_رسالة, *tmp;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_رسالة, tmp){
		إذا(client_رسالة->data.mid == mid){
			إذا(client_رسالة->base_رسالة->data.qos != 2){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_رسالة->base_رسالة->data.qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_in, client_رسالة);
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	أرجع MOSQ_ERR_NOT_FOUND;
}


صحيح db__message_release_incoming(بنية mosquitto *context, uint16_t mid)
{
	بنية mosquitto_client_رسالة *client_رسالة, *tmp;
	bool deleted = false;
	صحيح rc;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_رسالة, tmp){
		إذا(client_رسالة->data.mid == mid){
			إذا(client_رسالة->base_رسالة->data.qos != 2){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_رسالة->base_رسالة->data.qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			ثابت حرف *topic = client_رسالة->base_رسالة->data.topic;
			ثابت حرف *source_id = client_رسالة->base_رسالة->data.source_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			إذا(topic == عدم){
				db__message_remove_inflight(context, &context->msgs_in, client_رسالة);
				deleted = true;
			}وإلا{
				rc = sub__messages_queue(source_id, 2, &client_رسالة->base_رسالة);
				إذا(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db__message_remove_inflight(context, &context->msgs_in, client_رسالة);
					deleted = true;
				}وإلا{
					أرجع 1;
				}
			}
		}
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_رسالة, tmp){
		إذا(db__ready_for_flight(context, mosq_md_in, client_رسالة->data.qos)){
			اكسر;
		}

		إذا(client_رسالة->data.qos == 2){
			send__pubrec(context, client_رسالة->data.mid, 0, عدم);
			client_رسالة->data.state = mosq_ms_wait_for_pubrel;
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_رسالة_update(context, client_رسالة);
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
	بنية mosquitto_client_رسالة *client_رسالة, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_رسالة, tmp){
		إذا(client_رسالة->base_رسالة->data.expiry_time && db.now_real_s > client_رسالة->base_رسالة->data.expiry_time){
			إذا(client_رسالة->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_رسالة);
		}
	}
	db__fill_inflight_out_from_queue(context);
	DL_FOREACH_SAFE(context->msgs_out.queued, client_رسالة, tmp){
		إذا(client_رسالة->base_رسالة->data.expiry_time && db.now_real_s > client_رسالة->base_رسالة->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_رسالة);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.inflight, client_رسالة, tmp){
		إذا(client_رسالة->base_رسالة->data.expiry_time && db.now_real_s > client_رسالة->base_رسالة->data.expiry_time){
			إذا(client_رسالة->data.qos > 0){
				util__increment_receive_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_in, client_رسالة);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.queued, client_رسالة, tmp){
		إذا(client_رسالة->base_رسالة->data.expiry_time && db.now_real_s > client_رسالة->base_رسالة->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_in, client_رسالة);
		}
	}
}


ساكن فراغ db__client_messages_check_acl(بنية mosquitto *context, بنية mosquitto_رسالة_data *رسالة_data, بنية mosquitto_client_رسالة **head,
		فراغ (*decrement_stats_fn)(بنية mosquitto_رسالة_data *رسالة_data, بنية mosquitto_client_رسالة *client_رسالة))
{
	بنية mosquitto_client_رسالة *client_رسالة, *tmp;
	بنية mosquitto_base_رسالة *base_رسالة;
	صحيح access;

	DL_FOREACH_SAFE((*head), client_رسالة, tmp){
		base_رسالة = client_رسالة->base_رسالة;
		إذا(client_رسالة->data.direction == mosq_md_out){
			access = MOSQ_ACL_READ;
		}وإلا{
			access = MOSQ_ACL_WRITE;
		}
		إذا(mosquitto_acl_check(context, base_رسالة->data.topic,
				base_رسالة->data.payloadlen, base_رسالة->data.payload,
				base_رسالة->data.qos, base_رسالة->data.retain,
				base_رسالة->data.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*head), client_رسالة);
			decrement_stats_fn(رسالة_data, client_رسالة);
			plugin_persist_handle_client_رسالة_delete(context, client_رسالة);
			db_رسالة_store_ref_dec(&client_رسالة->base_رسالة);
			mosquitto_FREE(client_رسالة);
		}
	}
}


فراغ db__check_acl_of_all_messages(بنية mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.inflight, &db_رسالة_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.queued, &db_رسالة_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.inflight, &db_رسالة_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.queued, &db_رسالة_remove_from_queued_stats);
}


ساكن صحيح db__message_write_inflight_out_single(بنية mosquitto *context, بنية mosquitto_client_رسالة *client_رسالة)
{
	بنية mosquitto_base_رسالة *base_رسالة;
	mosquitto_property *base_رسالة_props = عدم;
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

	base_رسالة = client_رسالة->base_رسالة;

	expiry_interval = 0;
	إذا(base_رسالة->data.expiry_time){
		إذا(db.now_real_s > base_رسالة->data.expiry_time){
			/* Message is expired, must not send. */
			إذا(client_رسالة->data.direction == mosq_md_out && client_رسالة->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_رسالة);
			db__fill_inflight_out_from_queue(context);
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			expiry_interval = (uint32_t)(base_رسالة->data.expiry_time - db.now_real_s);
		}
	}
	mid = client_رسالة->data.mid;
	retries = client_رسالة->data.dup;
	retain = client_رسالة->data.retain;
	topic = base_رسالة->data.topic;
	qos = (uint8_t)client_رسالة->data.qos;
	payloadlen = base_رسالة->data.payloadlen;
	payload = base_رسالة->data.payload;
	subscription_id = client_رسالة->data.subscription_identifier;
	base_رسالة_props = base_رسالة->data.properties;

	بدل(client_رسالة->data.state){
		حالة mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_رسالة_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_رسالة);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_رسالة_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS){
				client_رسالة->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_رسالة->data.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_رسالة_update(context, client_رسالة);
			}وإلا إذا(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_رسالة);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_رسالة_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS){
				client_رسالة->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_رسالة->data.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_رسالة_update(context, client_رسالة);
			}وإلا إذا(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_رسالة);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, عدم);
			إذا(!rc){
				client_رسالة->data.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_رسالة_update(context, client_رسالة);
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
	بنية mosquitto_client_رسالة *client_رسالة, *tmp;
	صحيح rc;

	إذا(context->state != mosq_cs_active || !net__is_connected(context)){
		أرجع MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_رسالة, tmp){
		rc = db__message_write_inflight_out_single(context, client_رسالة);
		إذا(rc){
			أرجع rc;
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__message_write_inflight_out_latest(بنية mosquitto *context)
{
	بنية mosquitto_client_رسالة *client_رسالة, *next;
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
	client_رسالة = context->msgs_out.inflight->prev;
	طالما(client_رسالة != context->msgs_out.inflight &&
			(client_رسالة->data.state == mosq_ms_publish_qos0
			|| client_رسالة->data.state == mosq_ms_publish_qos1
			|| client_رسالة->data.state == mosq_ms_publish_qos2)){

		client_رسالة = client_رسالة->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	إذا(client_رسالة != context->msgs_out.inflight){
		client_رسالة = client_رسالة->next;
	}

	طالما(client_رسالة){
		next = client_رسالة->next;
		rc = db__message_write_inflight_out_single(context, client_رسالة);
		إذا(rc){
			أرجع rc;
		}
		client_رسالة = next;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__message_write_queued_in(بنية mosquitto *context)
{
	بنية mosquitto_client_رسالة *client_رسالة, *tmp;
	صحيح rc;

	إذا(context->state != mosq_cs_active){
		أرجع MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_رسالة, tmp){
		إذا(context->msgs_in.inflight_maximum != 0 && context->msgs_in.inflight_quota == 0){
			اكسر;
		}

		إذا(client_رسالة->data.qos == 2){
			client_رسالة->data.state = mosq_ms_send_pubrec;
			db__message_dequeue_first(context, &context->msgs_in);
			rc = send__pubrec(context, client_رسالة->data.mid, 0, عدم);
			إذا(!rc){
				client_رسالة->data.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_رسالة_update(context, client_رسالة);
			}وإلا{
				plugin_persist_handle_client_رسالة_update(context, client_رسالة);
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
