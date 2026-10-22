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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <utlist.h>

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "send_mosq.h"
#অন্তর্ভাব "sys_tree.h"
#অন্তর্ভাব "util_mosq.h"


/**
 * Is this context ready to take more in flight messages right now?
 * @param context the client context of interest
 * @param qos qos for the packet of interest
 * @return true if more in flight are allowed.
 */
bool db__ready_for_flight(কাঠামো mosquitto *context, গণনা mosquitto_বার্তা_direction dir, পূর্ণসংখ্যা qos)
{
	কাঠামো mosquitto_বার্তা_তথ্য *msgs;
	bool valid_bytes;
	bool valid_গুনতি;

	যদি(dir == mosq_md_out){
		msgs = &context->msgs_out;
	}অন্যথা{
		msgs = &context->msgs_in;
	}

	যদি(msgs->inflight_maximum == 0 && db.config->max_inflight_bytes == 0){
		ফেরত true;
	}

	যদি(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		যদি(db.config->max_queued_messages == 0 && db.config->max_inflight_bytes == 0){
			ফেরত true;
		}
		valid_bytes = ((msgs->inflight_bytes - (ssize_t)db.config->max_inflight_bytes) < (ssize_t)db.config->max_queued_bytes);
		যদি(dir == mosq_md_out){
			valid_গুনতি = context->out_packet_গুনতি < db.config->max_queued_messages;
		}অন্যথা{
			valid_গুনতি = msgs->inflight_গুনতি - msgs->inflight_maximum < db.config->max_queued_messages;
		}

		যদি(db.config->max_queued_messages == 0){
			ফেরত valid_bytes;
		}
		যদি(db.config->max_queued_bytes == 0){
			ফেরত valid_গুনতি;
		}
	}অন্যথা{
		valid_bytes = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_bytes;
		valid_গুনতি = msgs->inflight_quota > 0;

		যদি(msgs->inflight_maximum == 0){
			ফেরত valid_bytes;
		}
		যদি(db.config->max_inflight_bytes == 0){
			ফেরত valid_গুনতি;
		}
	}

	ফেরত valid_bytes && valid_গুনতি;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db__ready_for_queue(কাঠামো mosquitto *context, পূর্ণসংখ্যা qos, কাঠামো mosquitto_বার্তা_তথ্য *বার্তা_তথ্য)
{
	পূর্ণসংখ্যা source_গুনতি;
	পূর্ণসংখ্যা adjust_গুনতি;
	long source_bytes;
	ssize_t adjust_bytes = (ssize_t)db.config->max_inflight_bytes;
	bool valid_bytes;
	bool valid_গুনতি;

	যদি(db.config->max_queued_messages == 0 && db.config->max_queued_bytes == 0){
		ফেরত true;
	}

	যদি(qos == 0 && db.config->queue_qos0_messages == false){
		ফেরত false; /* This case is handled in db__ready_for_flight() */
	}অন্যথা{
		source_bytes = (ssize_t)বার্তা_তথ্য->queued_bytes12;
		source_গুনতি = বার্তা_তথ্য->queued_count12;
	}
	adjust_গুনতি = বার্তা_তথ্য->inflight_maximum;

	/* nothing in flight for offline clients */
	যদি(!net__is_connected(context)){
		adjust_bytes = 0;
		adjust_গুনতি = 0;
	}

	valid_bytes = (source_bytes - (ssize_t)adjust_bytes) < (ssize_t)db.config->max_queued_bytes;
	valid_গুনতি = source_গুনতি - adjust_গুনতি < db.config->max_queued_messages;

	যদি(db.config->max_queued_bytes == 0){
		ফেরত valid_গুনতি;
	}
	যদি(db.config->max_queued_messages == 0){
		ফেরত valid_bytes;
	}

	ফেরত valid_bytes && valid_গুনতি;
}


শূন্য db_বার্তা_add_to_inflight_stats(কাঠামো mosquitto_বার্তা_তথ্য *বার্তা_তথ্য, কাঠামো mosquitto_client_বার্তা *client_বার্তা)
{
	বার্তা_তথ্য->inflight_গুনতি++;
	বার্তা_তথ্য->inflight_bytes += client_বার্তা->base_বার্তা->তথ্য.payloadlen;
	যদি(client_বার্তা->তথ্য.qos != 0){
		বার্তা_তথ্য->inflight_count12++;
		বার্তা_তথ্য->inflight_bytes12 += client_বার্তা->base_বার্তা->তথ্য.payloadlen;
	}
}


স্থির শূন্য db_বার্তা_remove_from_inflight_stats(কাঠামো mosquitto_বার্তা_তথ্য *বার্তা_তথ্য, কাঠামো mosquitto_client_বার্তা *client_বার্তা)
{
	বার্তা_তথ্য->inflight_গুনতি--;
	বার্তা_তথ্য->inflight_bytes -= client_বার্তা->base_বার্তা->তথ্য.payloadlen;
	যদি(client_বার্তা->তথ্য.qos != 0){
		বার্তা_তথ্য->inflight_count12--;
		বার্তা_তথ্য->inflight_bytes12 -= client_বার্তা->base_বার্তা->তথ্য.payloadlen;
	}
}


শূন্য db_বার্তা_add_to_queued_stats(কাঠামো mosquitto_বার্তা_তথ্য *বার্তা_তথ্য, কাঠামো mosquitto_client_বার্তা *client_বার্তা)
{
	বার্তা_তথ্য->queued_গুনতি++;
	বার্তা_তথ্য->queued_bytes += client_বার্তা->base_বার্তা->তথ্য.payloadlen;
	যদি(client_বার্তা->তথ্য.qos != 0){
		বার্তা_তথ্য->queued_count12++;
		বার্তা_তথ্য->queued_bytes12 += client_বার্তা->base_বার্তা->তথ্য.payloadlen;
	}
}


স্থির শূন্য db_বার্তা_remove_from_queued_stats(কাঠামো mosquitto_বার্তা_তথ্য *বার্তা_তথ্য, কাঠামো mosquitto_client_বার্তা *client_বার্তা)
{
	বার্তা_তথ্য->queued_গুনতি--;
	বার্তা_তথ্য->queued_bytes -= client_বার্তা->base_বার্তা->তথ্য.payloadlen;
	যদি(client_বার্তা->তথ্য.qos != 0){
		বার্তা_তথ্য->queued_count12--;
		বার্তা_তথ্য->queued_bytes12 -= client_বার্তা->base_বার্তা->তথ্য.payloadlen;
	}
}


পূর্ণসংখ্যা db__open(কাঠামো mosquitto__config *config)
{
	যদি(!config){
		ফেরত MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = ফাঁকা;
	db.contexts_by_sock = ফাঁকা;
	db.contexts_for_free = ফাঁকা;
#ifdef WITH_BRIDGE
	db.bridges = ফাঁকা;
	db.bridge_গুনতি = 0;
#endif

	/* Initialize the hashtable */
	db.clientid_সূচক_hash = ফাঁকা;

	db.normal_subs = ফাঁকা;
	db.shared_subs = ফাঁকা;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = ফাঁকা;

#ifdef WITH_PERSISTENCE
	যদি(persist__restore()){
		ফেরত 1;
	}
#endif

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির শূন্য subhier_clean(কাঠামো mosquitto__subhier **subhier)
{
	কাঠামো mosquitto__subhier *peer, *subhier_tmp;
	কাঠামো mosquitto__subleaf *leaf, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		leaf = peer->subs;
		যখন(leaf){
			nextleaf = leaf->পরবর্তী;
			mosquitto_FREE(leaf);
			leaf = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


পূর্ণসংখ্যা db__close(শূন্য)
{
	subhier_clean(&db.normal_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db_বার্তা_store_clean();

	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা db_বার্তা_store_add(কাঠামো mosquitto_base_বার্তা *base_বার্তা)
{
	কাঠামো mosquitto_base_বার্তা *found;
	unsigned hashv;

	HASH_মান(&base_বার্তা->তথ্য.store_id, আকার(base_বার্তা->তথ্য.store_id), hashv);
	HASH_FIND_BYHASHVALUE(hh, db.বার্তা_store, &base_বার্তা->তথ্য.store_id, আকার(base_বার্তা->তথ্য.store_id), hashv, found);
	যদি(found == ফাঁকা){
		HASH_ADD_KEYPTR_BYHASHVALUE(hh, db.বার্তা_store, &base_বার্তা->তথ্য.store_id, আকার(base_বার্তা->তথ্য.store_id), hashv, base_বার্তা);
		ফেরত MOSQ_ERR_SUCCESS;
	}অন্যথা{
		ফেরত MOSQ_ERR_ALREADY_EXISTS;
	}
}


শূন্য db_বার্তা_store_free(কাঠামো mosquitto_base_বার্তা *base_বার্তা)
{
	mosquitto_FREE(base_বার্তা->তথ্য.source_id);
	mosquitto_FREE(base_বার্তা->তথ্য.source_username);
	যদি(base_বার্তা->dest_ids){
		জন্য(পূর্ণসংখ্যা i=0; i<base_বার্তা->dest_id_গুনতি; i++){
			mosquitto_FREE(base_বার্তা->dest_ids[i]);
		}
		mosquitto_FREE(base_বার্তা->dest_ids);
	}
	mosquitto_FREE(base_বার্তা->তথ্য.topic);
	mosquitto_property_free_all(&base_বার্তা->তথ্য.properties);
	mosquitto_FREE(base_বার্তা->তথ্য.payload);
	mosquitto_FREE(base_বার্তা);
}


শূন্য db_বার্তা_store_remove(কাঠামো mosquitto_base_বার্তা *base_বার্তা, bool notify)
{
	যদি(base_বার্তা == ফাঁকা){
		ফেরত;
	}
	HASH_DELETE(hh, db.বার্তা_store, base_বার্তা);
	db.বার্তা_store_গুনতি--;
	db.বার্তা_store_bytes -= base_বার্তা->তথ্য.payloadlen;
	যদি(notify == true){
		plugin_persist_handle_base_বার্তা_delete(base_বার্তা);
	}
	db_বার্তা_store_free(base_বার্তা);
}


শূন্য db_বার্তা_store_clean(শূন্য)
{
	কাঠামো mosquitto_base_বার্তা *base_বার্তা, *base_বার্তা_tmp;

	HASH_ITER(hh, db.বার্তা_store, base_বার্তা, base_বার্তা_tmp){
		db_বার্তা_store_remove(base_বার্তা, false);
	}
}


শূন্য db_বার্তা_store_ref_inc(কাঠামো mosquitto_base_বার্তা *base_বার্তা)
{
	base_বার্তা->ref_গুনতি++;
}


শূন্য db_বার্তা_store_ref_dec(কাঠামো mosquitto_base_বার্তা **base_বার্তা)
{
	(*base_বার্তা)->ref_গুনতি--;
	যদি((*base_বার্তা)->ref_গুনতি == 0){
		db_বার্তা_store_remove(*base_বার্তা, true);
		*base_বার্তা = ফাঁকা;
	}
}


শূন্য db_বার্তা_store_compact(শূন্য)
{
	কাঠামো mosquitto_base_বার্তা *base_বার্তা, *base_বার্তা_tmp;

	HASH_ITER(hh, db.বার্তা_store, base_বার্তা, base_বার্তা_tmp){
		যদি(base_বার্তা->ref_গুনতি < 1){
			db_বার্তা_store_remove(base_বার্তা, true);
		}
	}
}


স্থির শূন্য db__message_remove_inflight(কাঠামো mosquitto *context, কাঠামো mosquitto_বার্তা_তথ্য *বার্তা_তথ্য, কাঠামো mosquitto_client_বার্তা *item)
{
	যদি(!context || !বার্তা_তথ্য || !item){
		ফেরত;
	}

	plugin_persist_handle_client_বার্তা_delete(context, item);

	DL_DELETE(বার্তা_তথ্য->inflight, item);
	যদি(item->base_বার্তা){
		db_বার্তা_remove_from_inflight_stats(বার্তা_তথ্য, item);
		db_বার্তা_store_ref_dec(&item->base_বার্তা);
	}

	mosquitto_FREE(item);
}


স্থির শূন্য db__message_remove_queued(কাঠামো mosquitto *context, কাঠামো mosquitto_বার্তা_তথ্য *বার্তা_তথ্য, কাঠামো mosquitto_client_বার্তা *item)
{
	যদি(!context || !বার্তা_তথ্য || !item){
		ফেরত;
	}

	plugin_persist_handle_client_বার্তা_delete(context, item);

	DL_DELETE(বার্তা_তথ্য->queued, item);
	যদি(item->base_বার্তা){
		db_বার্তা_remove_from_queued_stats(বার্তা_তথ্য, item);
		db_বার্তা_store_ref_dec(&item->base_বার্তা);
	}

	mosquitto_FREE(item);
}


স্থির শূন্য db__fill_inflight_out_from_queue(কাঠামো mosquitto *context)
{
	কাঠামো mosquitto_client_বার্তা *client_বার্তা, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.queued, client_বার্তা, tmp){
		যদি(!db__ready_for_flight(context, mosq_md_out, client_বার্তা->তথ্য.qos)){
			ফেরত;
		}
		নির্বাচন(client_বার্তা->তথ্য.qos){
			ক্ষেত্র 0:
				client_বার্তা->তথ্য.state = mosq_ms_publish_qos0;
				বিরতি;
			ক্ষেত্র 1:
				client_বার্তা->তথ্য.state = mosq_ms_publish_qos1;
				বিরতি;
			ক্ষেত্র 2:
				client_বার্তা->তথ্য.state = mosq_ms_publish_qos2;
				বিরতি;
		}
		যদি(client_বার্তা->base_বার্তা->তথ্য.expiry_time && db.now_real_s > client_বার্তা->base_বার্তা->তথ্য.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_বার্তা);
			চালু;
		}
		plugin_persist_handle_client_বার্তা_update(context, client_বার্তা);
		db__message_dequeue_first(context, &context->msgs_out);
	}
}


শূন্য db__message_dequeue_first(কাঠামো mosquitto *context, কাঠামো mosquitto_বার্তা_তথ্য *বার্তা_তথ্য)
{
	কাঠামো mosquitto_client_বার্তা *client_বার্তা;

	UNUSED(context);

	client_বার্তা = বার্তা_তথ্য->queued;
	DL_DELETE(বার্তা_তথ্য->queued, client_বার্তা);
	DL_APPEND(বার্তা_তথ্য->inflight, client_বার্তা);
	যদি(বার্তা_তথ্য->inflight_quota > 0){
		বার্তা_তথ্য->inflight_quota--;
	}

	db_বার্তা_remove_from_queued_stats(বার্তা_তথ্য, client_বার্তা);
	db_বার্তা_add_to_inflight_stats(বার্তা_তথ্য, client_বার্তা);
}


পূর্ণসংখ্যা db__message_delete_outgoing(কাঠামো mosquitto *context, uint16_t mid, গণনা mosquitto_বার্তা_state expect_state, পূর্ণসংখ্যা qos)
{
	কাঠামো mosquitto_client_বার্তা *client_বার্তা, *tmp;
	bool deleted = false;

	যদি(!context){
		ফেরত MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_বার্তা, tmp){
		যদি(client_বার্তা->তথ্য.mid == mid){
			যদি(client_বার্তা->তথ্য.qos != qos){
				log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_বার্তা->তথ্য.qos, qos);
				ফেরত MOSQ_ERR_PROTOCOL;
			}অন্যথা যদি(qos == 2 && client_বার্তা->তথ্য.state != expect_state && expect_state != mosq_ms_any){
				log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_বার্তা->তথ্য.state, expect_state);
				ফেরত MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_out, client_বার্তা);
			deleted = true;
			বিরতি;
		}
	}

	যদি(deleted == false){
		DL_FOREACH_SAFE(context->msgs_out.queued, client_বার্তা, tmp){
			যদি(client_বার্তা->তথ্য.mid == mid){
				যদি(client_বার্তা->তথ্য.qos != qos){
					ফেরত MOSQ_ERR_PROTOCOL;
				}অন্যথা যদি(qos == 2 && client_বার্তা->তথ্য.state != expect_state && expect_state != mosq_ms_any){
					ফেরত MOSQ_ERR_PROTOCOL;
				}
				db__message_remove_queued(context, &context->msgs_out, client_বার্তা);
				বিরতি;
			}
		}
	}
	db__fill_inflight_out_from_queue(context);
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	ফেরত db__message_write_inflight_out_latest(context);
}


/* Only for QoS 2 messages */
পূর্ণসংখ্যা db_message_ঢোকাও_incoming(কাঠামো mosquitto *context, uint64_t cmsg_id, কাঠামো mosquitto_base_বার্তা *base_বার্তা, bool persist)
{
	কাঠামো mosquitto_client_বার্তা *client_বার্তা;
	কাঠামো mosquitto_বার্তা_তথ্য *বার্তা_তথ্য;
	গণনা mosquitto_বার্তা_state state = mosq_ms_invalid;
	পূর্ণসংখ্যা rc = 0;

	প্রতিজ্ঞা(base_বার্তা);
	যদি(!context){
		ফেরত MOSQ_ERR_INVAL;
	}
	যদি(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		ফেরত MOSQ_ERR_SUCCESS;

	}
	বার্তা_তথ্য = &context->msgs_in;

	যদি(db__ready_for_flight(context, mosq_md_in, base_বার্তা->তথ্য.qos)){
		state = mosq_ms_wait_for_pubrel;
	}অন্যথা যদি(base_বার্তা->তথ্য.qos != 0 && db__ready_for_queue(context, base_বার্তা->তথ্য.qos, বার্তা_তথ্য)){
		state = mosq_ms_queued;
		rc = 2;
	}অন্যথা{
		/* Dropping message due to full queue. */
		যদি(context->is_dropping == false){
			context->is_dropping = true;
			log__printf(ফাঁকা, MOSQ_LOG_NOTICE,
					"Outgoing messages are being dropped for client %s.",
					context->id);
		}
		metrics__int_inc(mosq_গণক_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		ফেরত 2;
	}

	প্রতিজ্ঞা(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	যদি(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_বার্তা = mosquitto_malloc(আকার(কাঠামো mosquitto_client_বার্তা));
	যদি(!client_বার্তা){
		ফেরত MOSQ_ERR_NOMEM;
	}
	client_বার্তা->পূর্ব = ফাঁকা;
	client_বার্তা->পরবর্তী = ফাঁকা;
	যদি(cmsg_id){
		client_বার্তা->তথ্য.cmsg_id = cmsg_id;
	}অন্যথা{
		client_বার্তা->তথ্য.cmsg_id = ++context->last_cmsg_id;
	}
	client_বার্তা->base_বার্তা = base_বার্তা;
	db_বার্তা_store_ref_inc(client_বার্তা->base_বার্তা);
	client_বার্তা->তথ্য.mid = base_বার্তা->তথ্য.source_mid;
	client_বার্তা->তথ্য.direction = mosq_md_in;
	client_বার্তা->তথ্য.state = (uint8_t)state;
	client_বার্তা->তথ্য.dup = false;
	যদি(base_বার্তা->তথ্য.qos > context->max_qos){
		client_বার্তা->তথ্য.qos = context->max_qos;
	}অন্যথা{
		client_বার্তা->তথ্য.qos = base_বার্তা->তথ্য.qos;
	}
	client_বার্তা->তথ্য.retain = base_বার্তা->তথ্য.retain;
	client_বার্তা->তথ্য.subscription_identifier = 0;

	যদি(state == mosq_ms_queued){
		DL_APPEND(বার্তা_তথ্য->queued, client_বার্তা);
		db_বার্তা_add_to_queued_stats(বার্তা_তথ্য, client_বার্তা);
	}অন্যথা{
		DL_APPEND(বার্তা_তথ্য->inflight, client_বার্তা);
		db_বার্তা_add_to_inflight_stats(বার্তা_তথ্য, client_বার্তা);
	}

	যদি(persist && context->is_persisted){
		plugin_persist_handle_base_বার্তা_add(client_বার্তা->base_বার্তা);
		plugin_persist_handle_client_বার্তা_add(context, client_বার্তা);
	}

	যদি(client_বার্তা->base_বার্তা->তথ্য.qos > 0){
		util__decrement_receive_quota(context);
	}
	ফেরত rc;
}


পূর্ণসংখ্যা db_message_ঢোকাও_outgoing(কাঠামো mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, কাঠামো mosquitto_base_বার্তা *base_বার্তা, uint32_t subscription_identifier, bool update, bool persist)
{
	কাঠামো mosquitto_client_বার্তা *client_বার্তা;
	কাঠামো mosquitto_বার্তা_তথ্য *বার্তা_তথ্য;
	গণনা mosquitto_বার্তা_state state = mosq_ms_invalid;
	পূর্ণসংখ্যা rc = 0;
	অক্ষর **dest_ids;

	প্রতিজ্ঞা(base_বার্তা);
	যদি(!context){
		ফেরত MOSQ_ERR_INVAL;
	}
	যদি(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		ফেরত MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_sent++;

	বার্তা_তথ্য = &context->msgs_out;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	যদি(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && base_বার্তা->dest_ids){

		জন্য(পূর্ণসংখ্যা i=0; i<base_বার্তা->dest_id_গুনতি; i++){
			যদি(base_বার্তা->dest_ids[i] && !strcmp(base_বার্তা->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				ফেরত MOSQ_ERR_SUCCESS;
			}
		}
	}
	যদি(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		যদি(qos == 0 && !db.config->queue_qos0_messages){
			যদি(!context->bridge){
				ফেরত 2;
			}অন্যথা{
				যদি(context->bridge->start_type != bst_lazy){
					ফেরত 2;
				}
			}
		}
		যদি(context->bridge && context->bridge->clean_start_local == true){
			ফেরত 2;
		}
	}

	যদি(net__is_connected(context)){
		যদি(db__ready_for_flight(context, mosq_md_out, qos)){
			নির্বাচন(qos){
				ক্ষেত্র 0:
					state = mosq_ms_publish_qos0;
					বিরতি;
				ক্ষেত্র 1:
					state = mosq_ms_publish_qos1;
					বিরতি;
				ক্ষেত্র 2:
					state = mosq_ms_publish_qos2;
					বিরতি;
			}
		}অন্যথা যদি(qos != 0 && db__ready_for_queue(context, qos, বার্তা_তথ্য)){
			state = mosq_ms_queued;
			rc = 2;
		}অন্যথা{
			/* Dropping message due to full queue. */
			যদি(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(ফাঁকা, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_গণক_mqtt_publish_dropped, 1);
			ফেরত 2;
		}
	}অন্যথা{
		যদি(db__ready_for_queue(context, qos, বার্তা_তথ্য)){
			state = mosq_ms_queued;
		}অন্যথা{
			metrics__int_inc(mosq_গণক_mqtt_publish_dropped, 1);
			যদি(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(ফাঁকা, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			ফেরত 2;
		}
	}
	প্রতিজ্ঞা(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	যদি(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_বার্তা = mosquitto_malloc(আকার(কাঠামো mosquitto_client_বার্তা));
	যদি(!client_বার্তা){
		ফেরত MOSQ_ERR_NOMEM;
	}
	client_বার্তা->পূর্ব = ফাঁকা;
	client_বার্তা->পরবর্তী = ফাঁকা;
	যদি(cmsg_id){
		client_বার্তা->তথ্য.cmsg_id = cmsg_id;
	}অন্যথা{
		client_বার্তা->তথ্য.cmsg_id = ++context->last_cmsg_id;
	}
	client_বার্তা->base_বার্তা = base_বার্তা;
	db_বার্তা_store_ref_inc(client_বার্তা->base_বার্তা);
	client_বার্তা->তথ্য.mid = mid;
	client_বার্তা->তথ্য.direction = mosq_md_out;
	client_বার্তা->তথ্য.state = (uint8_t)state;
	client_বার্তা->তথ্য.dup = false;
	যদি(qos > context->max_qos){
		client_বার্তা->তথ্য.qos = context->max_qos;
	}অন্যথা{
		client_বার্তা->তথ্য.qos = qos;
	}
	client_বার্তা->তথ্য.retain = retain;
	client_বার্তা->তথ্য.subscription_identifier = subscription_identifier;

	যদি(state == mosq_ms_queued){
		DL_APPEND(বার্তা_তথ্য->queued, client_বার্তা);
		db_বার্তা_add_to_queued_stats(বার্তা_তথ্য, client_বার্তা);
	}অন্যথা{
		DL_APPEND(বার্তা_তথ্য->inflight, client_বার্তা);
		db_বার্তা_add_to_inflight_stats(বার্তা_তথ্য, client_বার্তা);
	}

	যদি(persist && context->is_persisted){
		plugin_persist_handle_base_বার্তা_add(client_বার্তা->base_বার্তা);
		plugin_persist_handle_client_বার্তা_add(context, client_বার্তা);
	}

	যদি(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(base_বার্তা->dest_ids, আকার(অক্ষর *)*(size_t)(base_বার্তা->dest_id_গুনতি+1));
		যদি(dest_ids){
			base_বার্তা->dest_ids = dest_ids;
			base_বার্তা->dest_id_গুনতি++;
			base_বার্তা->dest_ids[base_বার্তা->dest_id_গুনতি-1] = mosquitto_strdup(context->id);
			যদি(!base_বার্তা->dest_ids[base_বার্তা->dest_id_গুনতি-1]){
				ফেরত MOSQ_ERR_NOMEM;
			}
		}অন্যথা{
			ফেরত MOSQ_ERR_NOMEM;
		}
	}
#ifdef WITH_BRIDGE
	যদি(context->bridge && context->bridge->start_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_out.inflight_গুনতি + context->msgs_out.queued_গুনতি >= context->bridge->threshold){

		context->bridge->lazy_reconnect = true;
	}
#endif

	যদি(client_বার্তা->তথ্য.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	যদি(update){
		rc = db__message_write_inflight_out_latest(context);
		যদি(rc){
			ফেরত rc;
		}
		rc = db__message_write_queued_out(context);
		যদি(rc){
			ফেরত rc;
		}
	}

	ফেরত rc;
}


স্থির অন্তঃরেখ পূর্ণসংখ্যা db__message_update_outgoing_state(কাঠামো mosquitto *context, কাঠামো mosquitto_client_বার্তা *শীর্ষ,
		uint16_t mid, গণনা mosquitto_বার্তা_state state, পূর্ণসংখ্যা qos, bool persist)
{
	কাঠামো mosquitto_client_বার্তা *client_বার্তা;

	DL_FOREACH(শীর্ষ, client_বার্তা){
		যদি(client_বার্তা->তথ্য.mid == mid){
			যদি(client_বার্তা->তথ্য.qos != qos){
				log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_বার্তা->তথ্য.qos, qos);
				ফেরত MOSQ_ERR_PROTOCOL;
			}
			client_বার্তা->তথ্য.state = (uint8_t)state;
			যদি(persist){
				plugin_persist_handle_client_বার্তা_update(context, client_বার্তা);
			}
			ফেরত MOSQ_ERR_SUCCESS;
		}
	}
	ফেরত MOSQ_ERR_NOT_FOUND;
}


পূর্ণসংখ্যা db__message_update_outgoing(কাঠামো mosquitto *context, uint16_t mid, গণনা mosquitto_বার্তা_state state, পূর্ণসংখ্যা qos, bool persist)
{
	পূর্ণসংখ্যা rc;

	rc = db__message_update_outgoing_state(context, context->msgs_out.inflight, mid, state, qos, persist);
	যদি(!persist && rc == MOSQ_ERR_NOT_FOUND){
		rc = db__message_update_outgoing_state(context, context->msgs_out.queued, mid, state, qos, persist);
	}
	ফেরত rc;
}


স্থির শূন্য db__messages_delete_list(কাঠামো mosquitto_client_বার্তা **শীর্ষ)
{
	কাঠামো mosquitto_client_বার্তা *client_বার্তা, *tmp;

	DL_FOREACH_SAFE(*শীর্ষ, client_বার্তা, tmp){
		DL_DELETE(*শীর্ষ, client_বার্তা);
		db_বার্তা_store_ref_dec(&client_বার্তা->base_বার্তা);
		mosquitto_FREE(client_বার্তা);
	}
	*শীর্ষ = ফাঁকা;
}


পূর্ণসংখ্যা db__messages_delete_incoming(কাঠামো mosquitto *context)
{
	যদি(!context){
		ফেরত MOSQ_ERR_INVAL;
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

	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা db__messages_delete_outgoing(কাঠামো mosquitto *context)
{
	যদি(!context){
		ফেরত MOSQ_ERR_INVAL;
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

	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা db__messages_delete(কাঠামো mosquitto *context, bool force_free)
{
	যদি(!context){
		ফেরত MOSQ_ERR_INVAL;
	}

	যদি(force_free || context->clean_start || (context->bridge && context->bridge->clean_start)){
		db__messages_delete_incoming(context);
	}

	যদি(force_free || (context->bridge && context->bridge->clean_start_local)
			|| (context->bridge == ফাঁকা && context->clean_start)){

		db__messages_delete_outgoing(context);
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা db__messages_easy_queue(কাঠামো mosquitto *context, ধ্রুবক অক্ষর *topic, uint8_t qos, uint32_t payloadlen, ধ্রুবক শূন্য *payload, পূর্ণসংখ্যা retain, uint32_t message_expiry_interval, mosquitto_property **properties)
{
	কাঠামো mosquitto_base_বার্তা *base_বার্তা;
	ধ্রুবক অক্ষর *source_id;
	গণনা mosquitto_বার্তা_origin origin;

	যদি(!topic){
		ফেরত MOSQ_ERR_INVAL;
	}

	base_বার্তা = mosquitto_calloc(1, আকার(কাঠামো mosquitto_base_বার্তা));
	যদি(base_বার্তা == ফাঁকা){
		ফেরত MOSQ_ERR_NOMEM;
	}

	base_বার্তা->তথ্য.topic = mosquitto_strdup(topic);
	যদি(base_বার্তা->তথ্য.topic == ফাঁকা){
		db_বার্তা_store_free(base_বার্তা);
		ফেরত MOSQ_ERR_INVAL;
	}

	base_বার্তা->তথ্য.qos = qos;
	যদি(db.config->retain_available == false){
		base_বার্তা->তথ্য.retain = 0;
	}অন্যথা{
		base_বার্তা->তথ্য.retain = retain;
	}

	base_বার্তা->তথ্য.payloadlen = payloadlen;
	যদি(payloadlen > 0){
		base_বার্তা->তথ্য.payload = mosquitto_malloc(base_বার্তা->তথ্য.payloadlen+1);
		যদি(base_বার্তা->তথ্য.payload == ফাঁকা){
			db_বার্তা_store_free(base_বার্তা);
			ফেরত MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_বার্তা->তথ্য.payload)[base_বার্তা->তথ্য.payloadlen] = 0;
		memcpy(base_বার্তা->তথ্য.payload, payload, base_বার্তা->তথ্য.payloadlen);
	}

	যদি(context && context->id){
		source_id = context->id;
	}অন্যথা{
		source_id = "";
	}
	যদি(properties){
		base_বার্তা->তথ্য.properties = *properties;
		*properties = ফাঁকা;
	}

	যদি(context){
		origin = mosq_mo_client;
	}অন্যথা{
		origin = mosq_mo_broker;
	}
	যদি(db__message_store(context, base_বার্তা, &message_expiry_interval, origin)){
		ফেরত 1;
	}

	ফেরত sub__messages_queue(source_id, base_বার্তা->তথ্য.qos, &base_বার্তা);
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
uint64_t db_new_বার্তা_id(শূন্য)
{
#ifdef WIN32
	FILETIME ftime;
	uint64_t ftime64;
#অন্যথা
	কাঠামো timespec ts;
#endif
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	long nsec;

	id = db.গ্রন্থি_id_shifted; /* Top 10-bits */

#ifdef WIN32
	GetSystemTimePreciseAsFileTime(&ftime);
	ftime64 = (((uint64_t)ftime.dw_উপরের_Date_Time)<<32) + ftime.dw_নিচের_Date_Time;
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

	ফেরত id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
পূর্ণসংখ্যা db__message_store(ধ্রুবক কাঠামো mosquitto *source, কাঠামো mosquitto_base_বার্তা *base_বার্তা, uint32_t *message_expiry_interval, গণনা mosquitto_বার্তা_origin origin)
{
	পূর্ণসংখ্যা rc;

	প্রতিজ্ঞা(base_বার্তা);

	যদি(source && source->id){
		base_বার্তা->তথ্য.source_id = mosquitto_strdup(source->id);
	}অন্যথা{
		base_বার্তা->তথ্য.source_id = mosquitto_strdup("");
	}
	যদি(!base_বার্তা->তথ্য.source_id){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_বার্তা_store_free(base_বার্তা);
		ফেরত MOSQ_ERR_NOMEM;
	}

	যদি(source && source->username){
		base_বার্তা->তথ্য.source_username = mosquitto_strdup(source->username);
		যদি(!base_বার্তা->তথ্য.source_username){
			db_বার্তা_store_free(base_বার্তা);
			ফেরত MOSQ_ERR_NOMEM;
		}
	}
	যদি(source){
		base_বার্তা->source_listener = source->listener;
	}
	base_বার্তা->origin = origin;
	যদি(message_expiry_interval){
		যদি(*message_expiry_interval > 0 && *message_expiry_interval != বার্তা_EXPIRY_INFINITE){
			base_বার্তা->তথ্য.expiry_time = db.now_real_s + *message_expiry_interval;
		}অন্যথা{
			base_বার্তা->তথ্য.expiry_time = 0;
		}
	}

	base_বার্তা->dest_ids = ফাঁকা;
	base_বার্তা->dest_id_গুনতি = 0;
	db.বার্তা_store_গুনতি++;
	db.বার্তা_store_bytes += base_বার্তা->তথ্য.payloadlen;

	যদি(!base_বার্তা->তথ্য.store_id){
		base_বার্তা->তথ্য.store_id = db_new_বার্তা_id();
	}

	rc = db_বার্তা_store_add(base_বার্তা);
	যদি(rc){
		db_বার্তা_store_free(base_বার্তা);
		ফেরত rc;
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা db__message_store_find(কাঠামো mosquitto *context, uint16_t mid, কাঠামো mosquitto_client_বার্তা **client_বার্তা)
{
	কাঠামো mosquitto_client_বার্তা *cmsg;

	*client_বার্তা = ফাঁকা;

	যদি(!context){
		ফেরত MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_in.inflight, cmsg){
		যদি(cmsg->base_বার্তা->তথ্য.source_mid == mid){
			*client_বার্তা = cmsg;
			ফেরত MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_in.queued, cmsg){
		যদি(cmsg->base_বার্তা->তথ্য.source_mid == mid){
			*client_বার্তা = cmsg;
			ফেরত MOSQ_ERR_SUCCESS;
		}
	}

	ফেরত 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
স্থির পূর্ণসংখ্যা db__message_reconnect_reset_outgoing(কাঠামো mosquitto *context)
{
	কাঠামো mosquitto_client_বার্তা *client_বার্তা, *tmp;

	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_গুনতি = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_গুনতি = 0;
	context->msgs_out.queued_count12 = 0;
	context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_বার্তা, tmp){
		db_বার্তা_add_to_inflight_stats(&context->msgs_out, client_বার্তা);
		যদি(client_বার্তা->তথ্য.qos > 0){
			util__decrement_send_quota(context);
		}

		নির্বাচন(client_বার্তা->তথ্য.qos){
			ক্ষেত্র 0:
				client_বার্তা->তথ্য.state = mosq_ms_publish_qos0;
				বিরতি;
			ক্ষেত্র 1:
				client_বার্তা->তথ্য.state = mosq_ms_publish_qos1;
				বিরতি;
			ক্ষেত্র 2:
				যদি(client_বার্তা->তথ্য.state == mosq_ms_wait_for_pubcomp){
					client_বার্তা->তথ্য.state = mosq_ms_resend_pubrel;
				}অন্যথা{
					client_বার্তা->তথ্য.state = mosq_ms_publish_qos2;
				}
				বিরতি;
		}
		plugin_persist_handle_client_বার্তা_update(context, client_বার্তা);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_out.queued, client_বার্তা, tmp){
		db_বার্তা_add_to_queued_stats(&context->msgs_out, client_বার্তা);
	}
	db__fill_inflight_out_from_queue(context);

	ফেরত MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
স্থির পূর্ণসংখ্যা db__message_reconnect_reset_incoming(কাঠামো mosquitto *context)
{
	কাঠামো mosquitto_client_বার্তা *client_বার্তা, *tmp;

	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_গুনতি = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_গুনতি = 0;
	context->msgs_in.queued_count12 = 0;
	context->msgs_in.inflight_quota = context->msgs_in.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_বার্তা, tmp){
		db_বার্তা_add_to_inflight_stats(&context->msgs_in, client_বার্তা);
		যদি(client_বার্তা->তথ্য.qos > 0){
			util__decrement_receive_quota(context);
		}

		যদি(client_বার্তা->তথ্য.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db__message_remove_inflight(context, &context->msgs_in, client_বার্তা);
		}অন্যথা{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_বার্তা->তথ্য.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_in.queued, client_বার্তা, tmp){
		client_বার্তা->তথ্য.dup = 0;
		db_বার্তা_add_to_queued_stats(&context->msgs_in, client_বার্তা);
		যদি(db__ready_for_flight(context, mosq_md_in, client_বার্তা->তথ্য.qos)){
			নির্বাচন(client_বার্তা->তথ্য.qos){
				ক্ষেত্র 0:
					client_বার্তা->তথ্য.state = mosq_ms_publish_qos0;
					বিরতি;
				ক্ষেত্র 1:
					client_বার্তা->তথ্য.state = mosq_ms_publish_qos1;
					বিরতি;
				ক্ষেত্র 2:
					client_বার্তা->তথ্য.state = mosq_ms_publish_qos2;
					বিরতি;
			}
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_বার্তা_update(context, client_বার্তা);
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা db__message_reconnect_reset(কাঠামো mosquitto *context)
{
	পূর্ণসংখ্যা rc;

	rc = db__message_reconnect_reset_outgoing(context);
	যদি(rc){
		ফেরত rc;
	}
	ফেরত db__message_reconnect_reset_incoming(context);
}


পূর্ণসংখ্যা db__message_remove_incoming(কাঠামো mosquitto *context, uint16_t mid)
{
	কাঠামো mosquitto_client_বার্তা *client_বার্তা, *tmp;

	যদি(!context){
		ফেরত MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_বার্তা, tmp){
		যদি(client_বার্তা->তথ্য.mid == mid){
			যদি(client_বার্তা->base_বার্তা->তথ্য.qos != 2){
				log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_বার্তা->base_বার্তা->তথ্য.qos);
				ফেরত MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_in, client_বার্তা);
			ফেরত MOSQ_ERR_SUCCESS;
		}
	}

	ফেরত MOSQ_ERR_NOT_FOUND;
}


পূর্ণসংখ্যা db__message_release_incoming(কাঠামো mosquitto *context, uint16_t mid)
{
	কাঠামো mosquitto_client_বার্তা *client_বার্তা, *tmp;
	bool deleted = false;
	পূর্ণসংখ্যা rc;

	যদি(!context){
		ফেরত MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_বার্তা, tmp){
		যদি(client_বার্তা->তথ্য.mid == mid){
			যদি(client_বার্তা->base_বার্তা->তথ্য.qos != 2){
				log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_বার্তা->base_বার্তা->তথ্য.qos);
				ফেরত MOSQ_ERR_PROTOCOL;
			}
			ধ্রুবক অক্ষর *topic = client_বার্তা->base_বার্তা->তথ্য.topic;
			ধ্রুবক অক্ষর *source_id = client_বার্তা->base_বার্তা->তথ্য.source_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			যদি(topic == ফাঁকা){
				db__message_remove_inflight(context, &context->msgs_in, client_বার্তা);
				deleted = true;
			}অন্যথা{
				rc = sub__messages_queue(source_id, 2, &client_বার্তা->base_বার্তা);
				যদি(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db__message_remove_inflight(context, &context->msgs_in, client_বার্তা);
					deleted = true;
				}অন্যথা{
					ফেরত 1;
				}
			}
		}
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_বার্তা, tmp){
		যদি(db__ready_for_flight(context, mosq_md_in, client_বার্তা->তথ্য.qos)){
			বিরতি;
		}

		যদি(client_বার্তা->তথ্য.qos == 2){
			send__pubrec(context, client_বার্তা->তথ্য.mid, 0, ফাঁকা);
			client_বার্তা->তথ্য.state = mosq_ms_wait_for_pubrel;
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_বার্তা_update(context, client_বার্তা);
		}
	}
	যদি(deleted){
		ফেরত MOSQ_ERR_SUCCESS;
	}অন্যথা{
		ফেরত MOSQ_ERR_NOT_FOUND;
	}
}


শূন্য db__expire_all_messages(কাঠামো mosquitto *context)
{
	কাঠামো mosquitto_client_বার্তা *client_বার্তা, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_বার্তা, tmp){
		যদি(client_বার্তা->base_বার্তা->তথ্য.expiry_time && db.now_real_s > client_বার্তা->base_বার্তা->তথ্য.expiry_time){
			যদি(client_বার্তা->তথ্য.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_বার্তা);
		}
	}
	db__fill_inflight_out_from_queue(context);
	DL_FOREACH_SAFE(context->msgs_out.queued, client_বার্তা, tmp){
		যদি(client_বার্তা->base_বার্তা->তথ্য.expiry_time && db.now_real_s > client_বার্তা->base_বার্তা->তথ্য.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_বার্তা);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.inflight, client_বার্তা, tmp){
		যদি(client_বার্তা->base_বার্তা->তথ্য.expiry_time && db.now_real_s > client_বার্তা->base_বার্তা->তথ্য.expiry_time){
			যদি(client_বার্তা->তথ্য.qos > 0){
				util__increment_receive_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_in, client_বার্তা);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.queued, client_বার্তা, tmp){
		যদি(client_বার্তা->base_বার্তা->তথ্য.expiry_time && db.now_real_s > client_বার্তা->base_বার্তা->তথ্য.expiry_time){
			db__message_remove_queued(context, &context->msgs_in, client_বার্তা);
		}
	}
}


স্থির শূন্য db__client_messages_check_acl(কাঠামো mosquitto *context, কাঠামো mosquitto_বার্তা_তথ্য *বার্তা_তথ্য, কাঠামো mosquitto_client_বার্তা **শীর্ষ,
		শূন্য (*decrement_stats_fn)(কাঠামো mosquitto_বার্তা_তথ্য *বার্তা_তথ্য, কাঠামো mosquitto_client_বার্তা *client_বার্তা))
{
	কাঠামো mosquitto_client_বার্তা *client_বার্তা, *tmp;
	কাঠামো mosquitto_base_বার্তা *base_বার্তা;
	পূর্ণসংখ্যা access;

	DL_FOREACH_SAFE((*শীর্ষ), client_বার্তা, tmp){
		base_বার্তা = client_বার্তা->base_বার্তা;
		যদি(client_বার্তা->তথ্য.direction == mosq_md_out){
			access = MOSQ_ACL_READ;
		}অন্যথা{
			access = MOSQ_ACL_WRITE;
		}
		যদি(mosquitto_acl_check(context, base_বার্তা->তথ্য.topic,
				base_বার্তা->তথ্য.payloadlen, base_বার্তা->তথ্য.payload,
				base_বার্তা->তথ্য.qos, base_বার্তা->তথ্য.retain,
				base_বার্তা->তথ্য.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*শীর্ষ), client_বার্তা);
			decrement_stats_fn(বার্তা_তথ্য, client_বার্তা);
			plugin_persist_handle_client_বার্তা_delete(context, client_বার্তা);
			db_বার্তা_store_ref_dec(&client_বার্তা->base_বার্তা);
			mosquitto_FREE(client_বার্তা);
		}
	}
}


শূন্য db__check_acl_of_all_messages(কাঠামো mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.inflight, &db_বার্তা_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.queued, &db_বার্তা_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.inflight, &db_বার্তা_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.queued, &db_বার্তা_remove_from_queued_stats);
}


স্থির পূর্ণসংখ্যা db__message_write_inflight_out_single(কাঠামো mosquitto *context, কাঠামো mosquitto_client_বার্তা *client_বার্তা)
{
	কাঠামো mosquitto_base_বার্তা *base_বার্তা;
	mosquitto_property *base_বার্তা_props = ফাঁকা;
	পূর্ণসংখ্যা rc;
	uint16_t mid;
	পূর্ণসংখ্যা retries;
	পূর্ণসংখ্যা retain;
	ধ্রুবক অক্ষর *topic;
	uint8_t qos;
	uint32_t payloadlen;
	ধ্রুবক শূন্য *payload;
	uint32_t expiry_interval;
	uint32_t subscription_id;

	base_বার্তা = client_বার্তা->base_বার্তা;

	expiry_interval = 0;
	যদি(base_বার্তা->তথ্য.expiry_time){
		যদি(db.now_real_s > base_বার্তা->তথ্য.expiry_time){
			/* Message is expired, must not send. */
			যদি(client_বার্তা->তথ্য.direction == mosq_md_out && client_বার্তা->তথ্য.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_বার্তা);
			db__fill_inflight_out_from_queue(context);
			ফেরত MOSQ_ERR_SUCCESS;
		}অন্যথা{
			expiry_interval = (uint32_t)(base_বার্তা->তথ্য.expiry_time - db.now_real_s);
		}
	}
	mid = client_বার্তা->তথ্য.mid;
	retries = client_বার্তা->তথ্য.dup;
	retain = client_বার্তা->তথ্য.retain;
	topic = base_বার্তা->তথ্য.topic;
	qos = (uint8_t)client_বার্তা->তথ্য.qos;
	payloadlen = base_বার্তা->তথ্য.payloadlen;
	payload = base_বার্তা->তথ্য.payload;
	subscription_id = client_বার্তা->তথ্য.subscription_identifier;
	base_বার্তা_props = base_বার্তা->তথ্য.properties;

	নির্বাচন(client_বার্তা->তথ্য.state){
		ক্ষেত্র mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_বার্তা_props, expiry_interval);
			যদি(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_বার্তা);
			}অন্যথা{
				ফেরত rc;
			}
			বিরতি;

		ক্ষেত্র mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_বার্তা_props, expiry_interval);
			যদি(rc == MOSQ_ERR_SUCCESS){
				client_বার্তা->তথ্য.dup = 1; /* Any retry attempts are a duplicate. */
				client_বার্তা->তথ্য.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_বার্তা_update(context, client_বার্তা);
			}অন্যথা যদি(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_বার্তা);
			}অন্যথা{
				ফেরত rc;
			}
			বিরতি;

		ক্ষেত্র mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_বার্তা_props, expiry_interval);
			যদি(rc == MOSQ_ERR_SUCCESS){
				client_বার্তা->তথ্য.dup = 1; /* Any retry attempts are a duplicate. */
				client_বার্তা->তথ্য.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_বার্তা_update(context, client_বার্তা);
			}অন্যথা যদি(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_বার্তা);
			}অন্যথা{
				ফেরত rc;
			}
			বিরতি;

		ক্ষেত্র mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, ফাঁকা);
			যদি(!rc){
				client_বার্তা->তথ্য.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_বার্তা_update(context, client_বার্তা);
			}অন্যথা{
				ফেরত rc;
			}
			বিরতি;

		ক্ষেত্র mosq_ms_invalid:
		ক্ষেত্র mosq_ms_send_pubrec:
		ক্ষেত্র mosq_ms_resend_pubcomp:
		ক্ষেত্র mosq_ms_wait_for_puback:
		ক্ষেত্র mosq_ms_wait_for_pubrec:
		ক্ষেত্র mosq_ms_wait_for_pubrel:
		ক্ষেত্র mosq_ms_wait_for_pubcomp:
		ক্ষেত্র mosq_ms_queued:
			বিরতি;
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা db__message_write_inflight_out_all(কাঠামো mosquitto *context)
{
	কাঠামো mosquitto_client_বার্তা *client_বার্তা, *tmp;
	পূর্ণসংখ্যা rc;

	যদি(context->state != mosq_cs_active || !net__is_connected(context)){
		ফেরত MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_বার্তা, tmp){
		rc = db__message_write_inflight_out_single(context, client_বার্তা);
		যদি(rc){
			ফেরত rc;
		}
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা db__message_write_inflight_out_latest(কাঠামো mosquitto *context)
{
	কাঠামো mosquitto_client_বার্তা *client_বার্তা, *পরবর্তী;
	পূর্ণসংখ্যা rc;

	যদি(context->state != mosq_cs_active
			|| !net__is_connected(context)
			|| context->msgs_out.inflight == ফাঁকা){

		ফেরত MOSQ_ERR_SUCCESS;
	}

	যদি(context->msgs_out.inflight->পূর্ব == context->msgs_out.inflight){
		/* Only one message */
		ফেরত db__message_write_inflight_out_single(context, context->msgs_out.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_বার্তা = context->msgs_out.inflight->পূর্ব;
	যখন(client_বার্তা != context->msgs_out.inflight &&
			(client_বার্তা->তথ্য.state == mosq_ms_publish_qos0
			|| client_বার্তা->তথ্য.state == mosq_ms_publish_qos1
			|| client_বার্তা->তথ্য.state == mosq_ms_publish_qos2)){

		client_বার্তা = client_বার্তা->পূর্ব;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	যদি(client_বার্তা != context->msgs_out.inflight){
		client_বার্তা = client_বার্তা->পরবর্তী;
	}

	যখন(client_বার্তা){
		পরবর্তী = client_বার্তা->পরবর্তী;
		rc = db__message_write_inflight_out_single(context, client_বার্তা);
		যদি(rc){
			ফেরত rc;
		}
		client_বার্তা = পরবর্তী;
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা db__message_write_queued_in(কাঠামো mosquitto *context)
{
	কাঠামো mosquitto_client_বার্তা *client_বার্তা, *tmp;
	পূর্ণসংখ্যা rc;

	যদি(context->state != mosq_cs_active){
		ফেরত MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_বার্তা, tmp){
		যদি(context->msgs_in.inflight_maximum != 0 && context->msgs_in.inflight_quota == 0){
			বিরতি;
		}

		যদি(client_বার্তা->তথ্য.qos == 2){
			client_বার্তা->তথ্য.state = mosq_ms_send_pubrec;
			db__message_dequeue_first(context, &context->msgs_in);
			rc = send__pubrec(context, client_বার্তা->তথ্য.mid, 0, ফাঁকা);
			যদি(!rc){
				client_বার্তা->তথ্য.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_বার্তা_update(context, client_বার্তা);
			}অন্যথা{
				plugin_persist_handle_client_বার্তা_update(context, client_বার্তা);
				ফেরত rc;
			}
		}
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা db__message_write_queued_out(কাঠামো mosquitto *context)
{
	যদি(context->state != mosq_cs_active){
		ফেরত MOSQ_ERR_SUCCESS;
	}

	db__fill_inflight_out_from_queue(context);

	ফেরত MOSQ_ERR_SUCCESS;
}
