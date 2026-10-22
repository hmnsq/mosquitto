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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <utlist.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "send_mosq.h"
#ଅନ୍ତର୍ଭାଵ "sys_tree.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"


/**
 * Is this context ready to take more in flight messages right now?
 * @param context the client context of interest
 * @param qos qos for the packet of interest
 * @return true if more in flight are allowed.
 */
bool db__ready_for_flight(ସଂରଚନା mosquitto *context, ଗଣନା mosquitto_ସନ୍ଦେଶ_direction dir, ପୂର୍ଣ୍ଣସଂଖ୍ୟା qos)
{
	ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data *msgs;
	bool valid_bytes;
	bool valid_ଗଣନା;

	ଯଦି(dir == mosq_md_out){
		msgs = &context->msgs_out;
	}ଅନ୍ୟଥା{
		msgs = &context->msgs_in;
	}

	ଯଦି(msgs->inflight_maximum == 0 && db.config->max_inflight_bytes == 0){
		ଫେରନ୍ତୁ true;
	}

	ଯଦି(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		ଯଦି(db.config->max_queued_messages == 0 && db.config->max_inflight_bytes == 0){
			ଫେରନ୍ତୁ true;
		}
		valid_bytes = ((msgs->inflight_bytes - (ssize_t)db.config->max_inflight_bytes) < (ssize_t)db.config->max_queued_bytes);
		ଯଦି(dir == mosq_md_out){
			valid_ଗଣନା = context->out_packet_ଗଣନା < db.config->max_queued_messages;
		}ଅନ୍ୟଥା{
			valid_ଗଣନା = msgs->inflight_ଗଣନା - msgs->inflight_maximum < db.config->max_queued_messages;
		}

		ଯଦି(db.config->max_queued_messages == 0){
			ଫେରନ୍ତୁ valid_bytes;
		}
		ଯଦି(db.config->max_queued_bytes == 0){
			ଫେରନ୍ତୁ valid_ଗଣନା;
		}
	}ଅନ୍ୟଥା{
		valid_bytes = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_bytes;
		valid_ଗଣନା = msgs->inflight_quota > 0;

		ଯଦି(msgs->inflight_maximum == 0){
			ଫେରନ୍ତୁ valid_bytes;
		}
		ଯଦି(db.config->max_inflight_bytes == 0){
			ଫେରନ୍ତୁ valid_ଗଣନା;
		}
	}

	ଫେରନ୍ତୁ valid_bytes && valid_ଗଣନା;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db__ready_for_queue(ସଂରଚନା mosquitto *context, ପୂର୍ଣ୍ଣସଂଖ୍ୟା qos, ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data *ସନ୍ଦେଶ_data)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା source_ଗଣନା;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା adjust_ଗଣନା;
	long source_bytes;
	ssize_t adjust_bytes = (ssize_t)db.config->max_inflight_bytes;
	bool valid_bytes;
	bool valid_ଗଣନା;

	ଯଦି(db.config->max_queued_messages == 0 && db.config->max_queued_bytes == 0){
		ଫେରନ୍ତୁ true;
	}

	ଯଦି(qos == 0 && db.config->queue_qos0_messages == false){
		ଫେରନ୍ତୁ false; /* This case is handled in db__ready_for_flight() */
	}ଅନ୍ୟଥା{
		source_bytes = (ssize_t)ସନ୍ଦେଶ_data->queued_bytes12;
		source_ଗଣନା = ସନ୍ଦେଶ_data->queued_count12;
	}
	adjust_ଗଣନା = ସନ୍ଦେଶ_data->inflight_maximum;

	/* nothing in flight for offline clients */
	ଯଦି(!net__is_connected(context)){
		adjust_bytes = 0;
		adjust_ଗଣନା = 0;
	}

	valid_bytes = (source_bytes - (ssize_t)adjust_bytes) < (ssize_t)db.config->max_queued_bytes;
	valid_ଗଣନା = source_ଗଣନା - adjust_ଗଣନା < db.config->max_queued_messages;

	ଯଦି(db.config->max_queued_bytes == 0){
		ଫେରନ୍ତୁ valid_ଗଣନା;
	}
	ଯଦି(db.config->max_queued_messages == 0){
		ଫେରନ୍ତୁ valid_bytes;
	}

	ଫେରନ୍ତୁ valid_bytes && valid_ଗଣନା;
}


ଶୂନ୍ୟ db_ସନ୍ଦେଶ_add_to_inflight_stats(ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data *ସନ୍ଦେଶ_data, ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ)
{
	ସନ୍ଦେଶ_data->inflight_ଗଣନା++;
	ସନ୍ଦେଶ_data->inflight_bytes += client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.payloadlen;
	ଯଦି(client_ସନ୍ଦେଶ->data.qos != 0){
		ସନ୍ଦେଶ_data->inflight_count12++;
		ସନ୍ଦେଶ_data->inflight_bytes12 += client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.payloadlen;
	}
}


ସ୍ଥିର ଶୂନ୍ୟ db_ସନ୍ଦେଶ_remove_from_inflight_stats(ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data *ସନ୍ଦେଶ_data, ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ)
{
	ସନ୍ଦେଶ_data->inflight_ଗଣନା--;
	ସନ୍ଦେଶ_data->inflight_bytes -= client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.payloadlen;
	ଯଦି(client_ସନ୍ଦେଶ->data.qos != 0){
		ସନ୍ଦେଶ_data->inflight_count12--;
		ସନ୍ଦେଶ_data->inflight_bytes12 -= client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.payloadlen;
	}
}


ଶୂନ୍ୟ db_ସନ୍ଦେଶ_add_to_queued_stats(ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data *ସନ୍ଦେଶ_data, ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ)
{
	ସନ୍ଦେଶ_data->queued_ଗଣନା++;
	ସନ୍ଦେଶ_data->queued_bytes += client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.payloadlen;
	ଯଦି(client_ସନ୍ଦେଶ->data.qos != 0){
		ସନ୍ଦେଶ_data->queued_count12++;
		ସନ୍ଦେଶ_data->queued_bytes12 += client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.payloadlen;
	}
}


ସ୍ଥିର ଶୂନ୍ୟ db_ସନ୍ଦେଶ_remove_from_queued_stats(ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data *ସନ୍ଦେଶ_data, ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ)
{
	ସନ୍ଦେଶ_data->queued_ଗଣନା--;
	ସନ୍ଦେଶ_data->queued_bytes -= client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.payloadlen;
	ଯଦି(client_ସନ୍ଦେଶ->data.qos != 0){
		ସନ୍ଦେଶ_data->queued_count12--;
		ସନ୍ଦେଶ_data->queued_bytes12 -= client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.payloadlen;
	}
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__open(ସଂରଚନା mosquitto__config *config)
{
	ଯଦି(!config){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = ଖାଲି;
	db.contexts_by_sock = ଖାଲି;
	db.contexts_for_free = ଖାଲି;
#ifdef WITH_BRIDGE
	db.bridges = ଖାଲି;
	db.bridge_ଗଣନା = 0;
#endif

	/* Initialize the hashtable */
	db.clientid_index_hash = ଖାଲି;

	db.normal_subs = ଖାଲି;
	db.shared_subs = ଖାଲି;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = ଖାଲି;

#ifdef WITH_PERSISTENCE
	ଯଦି(persist__restore()){
		ଫେରନ୍ତୁ 1;
	}
#endif

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ଶୂନ୍ୟ subhier_clean(ସଂରଚନା mosquitto__subhier **subhier)
{
	ସଂରଚନା mosquitto__subhier *peer, *subhier_tmp;
	ସଂରଚନା mosquitto__subleaf *leaf, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		leaf = peer->subs;
		ଯେତେବେଳେ(leaf){
			nextleaf = leaf->next;
			mosquitto_FREE(leaf);
			leaf = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__close(ଶୂନ୍ୟ)
{
	subhier_clean(&db.normal_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db_ସନ୍ଦେଶ_store_clean();

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା db_ସନ୍ଦେଶ_store_add(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ)
{
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *found;
	unsigned hashv;

	HASH_ମୂଲ୍ୟ(&base_ସନ୍ଦେଶ->data.store_id, ଆକାର(base_ସନ୍ଦେଶ->data.store_id), hashv);
	HASH_FIND_BYHASHVALUE(hh, db.ସନ୍ଦେଶ_store, &base_ସନ୍ଦେଶ->data.store_id, ଆକାର(base_ସନ୍ଦେଶ->data.store_id), hashv, found);
	ଯଦି(found == ଖାଲି){
		HASH_ADD_KEYPTR_BYHASHVALUE(hh, db.ସନ୍ଦେଶ_store, &base_ସନ୍ଦେଶ->data.store_id, ଆକାର(base_ସନ୍ଦେଶ->data.store_id), hashv, base_ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ MOSQ_ERR_ALREADY_EXISTS;
	}
}


ଶୂନ୍ୟ db_ସନ୍ଦେଶ_store_free(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ)
{
	mosquitto_FREE(base_ସନ୍ଦେଶ->data.source_id);
	mosquitto_FREE(base_ସନ୍ଦେଶ->data.source_username);
	ଯଦି(base_ସନ୍ଦେଶ->dest_ids){
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<base_ସନ୍ଦେଶ->dest_id_ଗଣନା; i++){
			mosquitto_FREE(base_ସନ୍ଦେଶ->dest_ids[i]);
		}
		mosquitto_FREE(base_ସନ୍ଦେଶ->dest_ids);
	}
	mosquitto_FREE(base_ସନ୍ଦେଶ->data.topic);
	mosquitto_property_free_all(&base_ସନ୍ଦେଶ->data.properties);
	mosquitto_FREE(base_ସନ୍ଦେଶ->data.payload);
	mosquitto_FREE(base_ସନ୍ଦେଶ);
}


ଶୂନ୍ୟ db_ସନ୍ଦେଶ_store_remove(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ, bool notify)
{
	ଯଦି(base_ସନ୍ଦେଶ == ଖାଲି){
		ଫେରନ୍ତୁ;
	}
	HASH_DELETE(hh, db.ସନ୍ଦେଶ_store, base_ସନ୍ଦେଶ);
	db.ସନ୍ଦେଶ_store_ଗଣନା--;
	db.ସନ୍ଦେଶ_store_bytes -= base_ସନ୍ଦେଶ->data.payloadlen;
	ଯଦି(notify == true){
		plugin_persist_handle_base_ସନ୍ଦେଶ_delete(base_ସନ୍ଦେଶ);
	}
	db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
}


ଶୂନ୍ୟ db_ସନ୍ଦେଶ_store_clean(ଶୂନ୍ୟ)
{
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ, *base_ସନ୍ଦେଶ_tmp;

	HASH_ITER(hh, db.ସନ୍ଦେଶ_store, base_ସନ୍ଦେଶ, base_ସନ୍ଦେଶ_tmp){
		db_ସନ୍ଦେଶ_store_remove(base_ସନ୍ଦେଶ, false);
	}
}


ଶୂନ୍ୟ db_ସନ୍ଦେଶ_store_ref_inc(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ)
{
	base_ସନ୍ଦେଶ->ref_ଗଣନା++;
}


ଶୂନ୍ୟ db_ସନ୍ଦେଶ_store_ref_dec(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ **base_ସନ୍ଦେଶ)
{
	(*base_ସନ୍ଦେଶ)->ref_ଗଣନା--;
	ଯଦି((*base_ସନ୍ଦେଶ)->ref_ଗଣନା == 0){
		db_ସନ୍ଦେଶ_store_remove(*base_ସନ୍ଦେଶ, true);
		*base_ସନ୍ଦେଶ = ଖାଲି;
	}
}


ଶୂନ୍ୟ db_ସନ୍ଦେଶ_store_compact(ଶୂନ୍ୟ)
{
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ, *base_ସନ୍ଦେଶ_tmp;

	HASH_ITER(hh, db.ସନ୍ଦେଶ_store, base_ସନ୍ଦେଶ, base_ସନ୍ଦେଶ_tmp){
		ଯଦି(base_ସନ୍ଦେଶ->ref_ଗଣନା < 1){
			db_ସନ୍ଦେଶ_store_remove(base_ସନ୍ଦେଶ, true);
		}
	}
}


ସ୍ଥିର ଶୂନ୍ୟ db__message_remove_inflight(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data *ସନ୍ଦେଶ_data, ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *item)
{
	ଯଦି(!context || !ସନ୍ଦେଶ_data || !item){
		ଫେରନ୍ତୁ;
	}

	plugin_persist_handle_client_ସନ୍ଦେଶ_delete(context, item);

	DL_DELETE(ସନ୍ଦେଶ_data->inflight, item);
	ଯଦି(item->base_ସନ୍ଦେଶ){
		db_ସନ୍ଦେଶ_remove_from_inflight_stats(ସନ୍ଦେଶ_data, item);
		db_ସନ୍ଦେଶ_store_ref_dec(&item->base_ସନ୍ଦେଶ);
	}

	mosquitto_FREE(item);
}


ସ୍ଥିର ଶୂନ୍ୟ db__message_remove_queued(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data *ସନ୍ଦେଶ_data, ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *item)
{
	ଯଦି(!context || !ସନ୍ଦେଶ_data || !item){
		ଫେରନ୍ତୁ;
	}

	plugin_persist_handle_client_ସନ୍ଦେଶ_delete(context, item);

	DL_DELETE(ସନ୍ଦେଶ_data->queued, item);
	ଯଦି(item->base_ସନ୍ଦେଶ){
		db_ସନ୍ଦେଶ_remove_from_queued_stats(ସନ୍ଦେଶ_data, item);
		db_ସନ୍ଦେଶ_store_ref_dec(&item->base_ସନ୍ଦେଶ);
	}

	mosquitto_FREE(item);
}


ସ୍ଥିର ଶୂନ୍ୟ db__fill_inflight_out_from_queue(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.queued, client_ସନ୍ଦେଶ, tmp){
		ଯଦି(!db__ready_for_flight(context, mosq_md_out, client_ସନ୍ଦେଶ->data.qos)){
			ଫେରନ୍ତୁ;
		}
		ବାଛ(client_ସନ୍ଦେଶ->data.qos){
			ସ୍ଥିତି 0:
				client_ସନ୍ଦେଶ->data.state = mosq_ms_publish_qos0;
				ଭାଙ୍ଗ;
			ସ୍ଥିତି 1:
				client_ସନ୍ଦେଶ->data.state = mosq_ms_publish_qos1;
				ଭାଙ୍ଗ;
			ସ୍ଥିତି 2:
				client_ସନ୍ଦେଶ->data.state = mosq_ms_publish_qos2;
				ଭାଙ୍ଗ;
		}
		ଯଦି(client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.expiry_time && db.now_real_s > client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_ସନ୍ଦେଶ);
			ଜାରି;
		}
		plugin_persist_handle_client_ସନ୍ଦେଶ_update(context, client_ସନ୍ଦେଶ);
		db__message_dequeue_first(context, &context->msgs_out);
	}
}


ଶୂନ୍ୟ db__message_dequeue_first(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data *ସନ୍ଦେଶ_data)
{
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ;

	UNUSED(context);

	client_ସନ୍ଦେଶ = ସନ୍ଦେଶ_data->queued;
	DL_DELETE(ସନ୍ଦେଶ_data->queued, client_ସନ୍ଦେଶ);
	DL_APPEND(ସନ୍ଦେଶ_data->inflight, client_ସନ୍ଦେଶ);
	ଯଦି(ସନ୍ଦେଶ_data->inflight_quota > 0){
		ସନ୍ଦେଶ_data->inflight_quota--;
	}

	db_ସନ୍ଦେଶ_remove_from_queued_stats(ସନ୍ଦେଶ_data, client_ସନ୍ଦେଶ);
	db_ସନ୍ଦେଶ_add_to_inflight_stats(ସନ୍ଦେଶ_data, client_ସନ୍ଦେଶ);
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_delete_outgoing(ସଂରଚନା mosquitto *context, uint16_t mid, ଗଣନା mosquitto_ସନ୍ଦେଶ_state expect_state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା qos)
{
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ, *tmp;
	bool deleted = false;

	ଯଦି(!context){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_ସନ୍ଦେଶ, tmp){
		ଯଦି(client_ସନ୍ଦେଶ->data.mid == mid){
			ଯଦି(client_ସନ୍ଦେଶ->data.qos != qos){
				log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_ସନ୍ଦେଶ->data.qos, qos);
				ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
			}ଅନ୍ୟଥା ଯଦି(qos == 2 && client_ସନ୍ଦେଶ->data.state != expect_state && expect_state != mosq_ms_any){
				log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_ସନ୍ଦେଶ->data.state, expect_state);
				ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_out, client_ସନ୍ଦେଶ);
			deleted = true;
			ଭାଙ୍ଗ;
		}
	}

	ଯଦି(deleted == false){
		DL_FOREACH_SAFE(context->msgs_out.queued, client_ସନ୍ଦେଶ, tmp){
			ଯଦି(client_ସନ୍ଦେଶ->data.mid == mid){
				ଯଦି(client_ସନ୍ଦେଶ->data.qos != qos){
					ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
				}ଅନ୍ୟଥା ଯଦି(qos == 2 && client_ସନ୍ଦେଶ->data.state != expect_state && expect_state != mosq_ms_any){
					ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
				}
				db__message_remove_queued(context, &context->msgs_out, client_ସନ୍ଦେଶ);
				ଭାଙ୍ଗ;
			}
		}
	}
	db__fill_inflight_out_from_queue(context);
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	ଫେରନ୍ତୁ db__message_write_inflight_out_latest(context);
}


/* Only for QoS 2 messages */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db_message_ଭର୍ତ୍ତି_incoming(ସଂରଚନା mosquitto *context, uint64_t cmsg_id, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ, bool persist)
{
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ;
	ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data *ସନ୍ଦେଶ_data;
	ଗଣନା mosquitto_ସନ୍ଦେଶ_state state = mosq_ms_invalid;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;

	ପ୍ରତିଜ୍ଞା(base_ସନ୍ଦେଶ);
	ଯଦି(!context){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଯଦି(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;

	}
	ସନ୍ଦେଶ_data = &context->msgs_in;

	ଯଦି(db__ready_for_flight(context, mosq_md_in, base_ସନ୍ଦେଶ->data.qos)){
		state = mosq_ms_wait_for_pubrel;
	}ଅନ୍ୟଥା ଯଦି(base_ସନ୍ଦେଶ->data.qos != 0 && db__ready_for_queue(context, base_ସନ୍ଦେଶ->data.qos, ସନ୍ଦେଶ_data)){
		state = mosq_ms_queued;
		rc = 2;
	}ଅନ୍ୟଥା{
		/* Dropping message due to full queue. */
		ଯଦି(context->is_dropping == false){
			context->is_dropping = true;
			log__printf(ଖାଲି, MOSQ_LOG_NOTICE,
					"Outgoing messages are being dropped for client %s.",
					context->id);
		}
		metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		ଫେରନ୍ତୁ 2;
	}

	ପ୍ରତିଜ୍ଞା(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	ଯଦି(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_ସନ୍ଦେଶ = mosquitto_malloc(ଆକାର(ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ));
	ଯଦି(!client_ସନ୍ଦେଶ){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	client_ସନ୍ଦେଶ->prev = ଖାଲି;
	client_ସନ୍ଦେଶ->next = ଖାଲି;
	ଯଦି(cmsg_id){
		client_ସନ୍ଦେଶ->data.cmsg_id = cmsg_id;
	}ଅନ୍ୟଥା{
		client_ସନ୍ଦେଶ->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ = base_ସନ୍ଦେଶ;
	db_ସନ୍ଦେଶ_store_ref_inc(client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ);
	client_ସନ୍ଦେଶ->data.mid = base_ସନ୍ଦେଶ->data.source_mid;
	client_ସନ୍ଦେଶ->data.direction = mosq_md_in;
	client_ସନ୍ଦେଶ->data.state = (uint8_t)state;
	client_ସନ୍ଦେଶ->data.dup = false;
	ଯଦି(base_ସନ୍ଦେଶ->data.qos > context->max_qos){
		client_ସନ୍ଦେଶ->data.qos = context->max_qos;
	}ଅନ୍ୟଥା{
		client_ସନ୍ଦେଶ->data.qos = base_ସନ୍ଦେଶ->data.qos;
	}
	client_ସନ୍ଦେଶ->data.retain = base_ସନ୍ଦେଶ->data.retain;
	client_ସନ୍ଦେଶ->data.subscription_identifier = 0;

	ଯଦି(state == mosq_ms_queued){
		DL_APPEND(ସନ୍ଦେଶ_data->queued, client_ସନ୍ଦେଶ);
		db_ସନ୍ଦେଶ_add_to_queued_stats(ସନ୍ଦେଶ_data, client_ସନ୍ଦେଶ);
	}ଅନ୍ୟଥା{
		DL_APPEND(ସନ୍ଦେଶ_data->inflight, client_ସନ୍ଦେଶ);
		db_ସନ୍ଦେଶ_add_to_inflight_stats(ସନ୍ଦେଶ_data, client_ସନ୍ଦେଶ);
	}

	ଯଦି(persist && context->is_persisted){
		plugin_persist_handle_base_ସନ୍ଦେଶ_add(client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ);
		plugin_persist_handle_client_ସନ୍ଦେଶ_add(context, client_ସନ୍ଦେଶ);
	}

	ଯଦି(client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.qos > 0){
		util__decrement_receive_quota(context);
	}
	ଫେରନ୍ତୁ rc;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା db_message_ଭର୍ତ୍ତି_outgoing(ସଂରଚନା mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ, uint32_t subscription_identifier, bool update, bool persist)
{
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ;
	ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data *ସନ୍ଦେଶ_data;
	ଗଣନା mosquitto_ସନ୍ଦେଶ_state state = mosq_ms_invalid;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
	ଅକ୍ଷର **dest_ids;

	ପ୍ରତିଜ୍ଞା(base_ସନ୍ଦେଶ);
	ଯଦି(!context){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଯଦି(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_sent++;

	ସନ୍ଦେଶ_data = &context->msgs_out;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	ଯଦି(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && base_ସନ୍ଦେଶ->dest_ids){

		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<base_ସନ୍ଦେଶ->dest_id_ଗଣନା; i++){
			ଯଦି(base_ସନ୍ଦେଶ->dest_ids[i] && !strcmp(base_ସନ୍ଦେଶ->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
			}
		}
	}
	ଯଦି(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		ଯଦି(qos == 0 && !db.config->queue_qos0_messages){
			ଯଦି(!context->bridge){
				ଫେରନ୍ତୁ 2;
			}ଅନ୍ୟଥା{
				ଯଦି(context->bridge->start_type != bst_lazy){
					ଫେରନ୍ତୁ 2;
				}
			}
		}
		ଯଦି(context->bridge && context->bridge->clean_start_local == true){
			ଫେରନ୍ତୁ 2;
		}
	}

	ଯଦି(net__is_connected(context)){
		ଯଦି(db__ready_for_flight(context, mosq_md_out, qos)){
			ବାଛ(qos){
				ସ୍ଥିତି 0:
					state = mosq_ms_publish_qos0;
					ଭାଙ୍ଗ;
				ସ୍ଥିତି 1:
					state = mosq_ms_publish_qos1;
					ଭାଙ୍ଗ;
				ସ୍ଥିତି 2:
					state = mosq_ms_publish_qos2;
					ଭାଙ୍ଗ;
			}
		}ଅନ୍ୟଥା ଯଦି(qos != 0 && db__ready_for_queue(context, qos, ସନ୍ଦେଶ_data)){
			state = mosq_ms_queued;
			rc = 2;
		}ଅନ୍ୟଥା{
			/* Dropping message due to full queue. */
			ଯଦି(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(ଖାଲି, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			ଫେରନ୍ତୁ 2;
		}
	}ଅନ୍ୟଥା{
		ଯଦି(db__ready_for_queue(context, qos, ସନ୍ଦେଶ_data)){
			state = mosq_ms_queued;
		}ଅନ୍ୟଥା{
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			ଯଦି(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(ଖାଲି, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			ଫେରନ୍ତୁ 2;
		}
	}
	ପ୍ରତିଜ୍ଞା(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	ଯଦି(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_ସନ୍ଦେଶ = mosquitto_malloc(ଆକାର(ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ));
	ଯଦି(!client_ସନ୍ଦେଶ){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	client_ସନ୍ଦେଶ->prev = ଖାଲି;
	client_ସନ୍ଦେଶ->next = ଖାଲି;
	ଯଦି(cmsg_id){
		client_ସନ୍ଦେଶ->data.cmsg_id = cmsg_id;
	}ଅନ୍ୟଥା{
		client_ସନ୍ଦେଶ->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ = base_ସନ୍ଦେଶ;
	db_ସନ୍ଦେଶ_store_ref_inc(client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ);
	client_ସନ୍ଦେଶ->data.mid = mid;
	client_ସନ୍ଦେଶ->data.direction = mosq_md_out;
	client_ସନ୍ଦେଶ->data.state = (uint8_t)state;
	client_ସନ୍ଦେଶ->data.dup = false;
	ଯଦି(qos > context->max_qos){
		client_ସନ୍ଦେଶ->data.qos = context->max_qos;
	}ଅନ୍ୟଥା{
		client_ସନ୍ଦେଶ->data.qos = qos;
	}
	client_ସନ୍ଦେଶ->data.retain = retain;
	client_ସନ୍ଦେଶ->data.subscription_identifier = subscription_identifier;

	ଯଦି(state == mosq_ms_queued){
		DL_APPEND(ସନ୍ଦେଶ_data->queued, client_ସନ୍ଦେଶ);
		db_ସନ୍ଦେଶ_add_to_queued_stats(ସନ୍ଦେଶ_data, client_ସନ୍ଦେଶ);
	}ଅନ୍ୟଥା{
		DL_APPEND(ସନ୍ଦେଶ_data->inflight, client_ସନ୍ଦେଶ);
		db_ସନ୍ଦେଶ_add_to_inflight_stats(ସନ୍ଦେଶ_data, client_ସନ୍ଦେଶ);
	}

	ଯଦି(persist && context->is_persisted){
		plugin_persist_handle_base_ସନ୍ଦେଶ_add(client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ);
		plugin_persist_handle_client_ସନ୍ଦେଶ_add(context, client_ସନ୍ଦେଶ);
	}

	ଯଦି(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(base_ସନ୍ଦେଶ->dest_ids, ଆକାର(ଅକ୍ଷର *)*(size_t)(base_ସନ୍ଦେଶ->dest_id_ଗଣନା+1));
		ଯଦି(dest_ids){
			base_ସନ୍ଦେଶ->dest_ids = dest_ids;
			base_ସନ୍ଦେଶ->dest_id_ଗଣନା++;
			base_ସନ୍ଦେଶ->dest_ids[base_ସନ୍ଦେଶ->dest_id_ଗଣନା-1] = mosquitto_strdup(context->id);
			ଯଦି(!base_ସନ୍ଦେଶ->dest_ids[base_ସନ୍ଦେଶ->dest_id_ଗଣନା-1]){
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
		}ଅନ୍ୟଥା{
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
	}
#ifdef WITH_BRIDGE
	ଯଦି(context->bridge && context->bridge->start_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_out.inflight_ଗଣନା + context->msgs_out.queued_ଗଣନା >= context->bridge->threshold){

		context->bridge->lazy_reconnect = true;
	}
#endif

	ଯଦି(client_ସନ୍ଦେଶ->data.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	ଯଦି(update){
		rc = db__message_write_inflight_out_latest(context);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
		rc = db__message_write_queued_out(context);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
	}

	ଫେରନ୍ତୁ rc;
}


ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_update_outgoing_state(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *head,
		uint16_t mid, ଗଣନା mosquitto_ସନ୍ଦେଶ_state state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା qos, bool persist)
{
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ;

	DL_FOREACH(head, client_ସନ୍ଦେଶ){
		ଯଦି(client_ସନ୍ଦେଶ->data.mid == mid){
			ଯଦି(client_ସନ୍ଦେଶ->data.qos != qos){
				log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_ସନ୍ଦେଶ->data.qos, qos);
				ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
			}
			client_ସନ୍ଦେଶ->data.state = (uint8_t)state;
			ଯଦି(persist){
				plugin_persist_handle_client_ସନ୍ଦେଶ_update(context, client_ସନ୍ଦେଶ);
			}
			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}
	}
	ଫେରନ୍ତୁ MOSQ_ERR_NOT_FOUND;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_update_outgoing(ସଂରଚନା mosquitto *context, uint16_t mid, ଗଣନା mosquitto_ସନ୍ଦେଶ_state state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା qos, bool persist)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = db__message_update_outgoing_state(context, context->msgs_out.inflight, mid, state, qos, persist);
	ଯଦି(!persist && rc == MOSQ_ERR_NOT_FOUND){
		rc = db__message_update_outgoing_state(context, context->msgs_out.queued, mid, state, qos, persist);
	}
	ଫେରନ୍ତୁ rc;
}


ସ୍ଥିର ଶୂନ୍ୟ db__messages_delete_list(ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ **head)
{
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ, *tmp;

	DL_FOREACH_SAFE(*head, client_ସନ୍ଦେଶ, tmp){
		DL_DELETE(*head, client_ସନ୍ଦେଶ);
		db_ସନ୍ଦେଶ_store_ref_dec(&client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ);
		mosquitto_FREE(client_ସନ୍ଦେଶ);
	}
	*head = ଖାଲି;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__messages_delete_incoming(ସଂରଚନା mosquitto *context)
{
	ଯଦି(!context){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_in.inflight);
	db__messages_delete_list(&context->msgs_in.queued);
	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_ଗଣନା = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_ଗଣନା = 0;
	context->msgs_in.queued_count12 = 0;

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__messages_delete_outgoing(ସଂରଚନା mosquitto *context)
{
	ଯଦି(!context){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_out.inflight);
	db__messages_delete_list(&context->msgs_out.queued);
	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_ଗଣନା = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_ଗଣନା = 0;
	context->msgs_out.queued_count12 = 0;

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__messages_delete(ସଂରଚନା mosquitto *context, bool force_free)
{
	ଯଦି(!context){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	ଯଦି(force_free || context->clean_start || (context->bridge && context->bridge->clean_start)){
		db__messages_delete_incoming(context);
	}

	ଯଦି(force_free || (context->bridge && context->bridge->clean_start_local)
			|| (context->bridge == ଖାଲି && context->clean_start)){

		db__messages_delete_outgoing(context);
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__messages_easy_queue(ସଂରଚନା mosquitto *context, ଅଚଳ ଅକ୍ଷର *topic, uint8_t qos, uint32_t payloadlen, ଅଚଳ ଶୂନ୍ୟ *payload, ପୂର୍ଣ୍ଣସଂଖ୍ୟା retain, uint32_t message_expiry_interval, mosquitto_property **properties)
{
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ;
	ଅଚଳ ଅକ୍ଷର *source_id;
	ଗଣନା mosquitto_ସନ୍ଦେଶ_origin origin;

	ଯଦି(!topic){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	base_ସନ୍ଦେଶ = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ));
	ଯଦି(base_ସନ୍ଦେଶ == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	base_ସନ୍ଦେଶ->data.topic = mosquitto_strdup(topic);
	ଯଦି(base_ସନ୍ଦେଶ->data.topic == ଖାଲି){
		db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	base_ସନ୍ଦେଶ->data.qos = qos;
	ଯଦି(db.config->retain_available == false){
		base_ସନ୍ଦେଶ->data.retain = 0;
	}ଅନ୍ୟଥା{
		base_ସନ୍ଦେଶ->data.retain = retain;
	}

	base_ସନ୍ଦେଶ->data.payloadlen = payloadlen;
	ଯଦି(payloadlen > 0){
		base_ସନ୍ଦେଶ->data.payload = mosquitto_malloc(base_ସନ୍ଦେଶ->data.payloadlen+1);
		ଯଦି(base_ସନ୍ଦେଶ->data.payload == ଖାଲି){
			db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_ସନ୍ଦେଶ->data.payload)[base_ସନ୍ଦେଶ->data.payloadlen] = 0;
		memcpy(base_ସନ୍ଦେଶ->data.payload, payload, base_ସନ୍ଦେଶ->data.payloadlen);
	}

	ଯଦି(context && context->id){
		source_id = context->id;
	}ଅନ୍ୟଥା{
		source_id = "";
	}
	ଯଦି(properties){
		base_ସନ୍ଦେଶ->data.properties = *properties;
		*properties = ଖାଲି;
	}

	ଯଦି(context){
		origin = mosq_mo_client;
	}ଅନ୍ୟଥା{
		origin = mosq_mo_broker;
	}
	ଯଦି(db__message_store(context, base_ସନ୍ଦେଶ, &message_expiry_interval, origin)){
		ଫେରନ୍ତୁ 1;
	}

	ଫେରନ୍ତୁ sub__messages_queue(source_id, base_ସନ୍ଦେଶ->data.qos, &base_ସନ୍ଦେଶ);
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
uint64_t db_new_ସନ୍ଦେଶ_id(ଶୂନ୍ୟ)
{
#ifdef WIN32
	FILETIME ftime;
	uint64_t ftime64;
#ଅନ୍ୟଥା
	ସଂରଚନା timespec ts;
#endif
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	long nsec;

	id = db.ଗଣ୍ଠି_id_shifted; /* Top 10-bits */

#ifdef WIN32
	GetSystemTimePreciseAsFileTime(&ftime);
	ftime64 = (((uint64_t)ftime.dw_ଉପର_Date_Time)<<32) + ftime.dw_ତଳ_Date_Time;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (long)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#ଅନ୍ୟଥା
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#endif
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	ଯଦି(id <= db.last_db_id){
		id = db.last_db_id + 1;
	}
	db.last_db_id = id;

	ଫେରନ୍ତୁ id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_store(ଅଚଳ ସଂରଚନା mosquitto *source, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ, uint32_t *message_expiry_interval, ଗଣନା mosquitto_ସନ୍ଦେଶ_origin origin)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ପ୍ରତିଜ୍ଞା(base_ସନ୍ଦେଶ);

	ଯଦି(source && source->id){
		base_ସନ୍ଦେଶ->data.source_id = mosquitto_strdup(source->id);
	}ଅନ୍ୟଥା{
		base_ସନ୍ଦେଶ->data.source_id = mosquitto_strdup("");
	}
	ଯଦି(!base_ସନ୍ଦେଶ->data.source_id){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	ଯଦି(source && source->username){
		base_ସନ୍ଦେଶ->data.source_username = mosquitto_strdup(source->username);
		ଯଦି(!base_ସନ୍ଦେଶ->data.source_username){
			db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
	}
	ଯଦି(source){
		base_ସନ୍ଦେଶ->source_listener = source->listener;
	}
	base_ସନ୍ଦେଶ->origin = origin;
	ଯଦି(message_expiry_interval){
		ଯଦି(*message_expiry_interval > 0 && *message_expiry_interval != ସନ୍ଦେଶ_EXPIRY_INFINITE){
			base_ସନ୍ଦେଶ->data.expiry_time = db.now_real_s + *message_expiry_interval;
		}ଅନ୍ୟଥା{
			base_ସନ୍ଦେଶ->data.expiry_time = 0;
		}
	}

	base_ସନ୍ଦେଶ->dest_ids = ଖାଲି;
	base_ସନ୍ଦେଶ->dest_id_ଗଣନା = 0;
	db.ସନ୍ଦେଶ_store_ଗଣନା++;
	db.ସନ୍ଦେଶ_store_bytes += base_ସନ୍ଦେଶ->data.payloadlen;

	ଯଦି(!base_ସନ୍ଦେଶ->data.store_id){
		base_ସନ୍ଦେଶ->data.store_id = db_new_ସନ୍ଦେଶ_id();
	}

	rc = db_ସନ୍ଦେଶ_store_add(base_ସନ୍ଦେଶ);
	ଯଦି(rc){
		db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_store_find(ସଂରଚନା mosquitto *context, uint16_t mid, ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ **client_ସନ୍ଦେଶ)
{
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *cmsg;

	*client_ସନ୍ଦେଶ = ଖାଲି;

	ଯଦି(!context){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_in.inflight, cmsg){
		ଯଦି(cmsg->base_ସନ୍ଦେଶ->data.source_mid == mid){
			*client_ସନ୍ଦେଶ = cmsg;
			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_in.queued, cmsg){
		ଯଦି(cmsg->base_ସନ୍ଦେଶ->data.source_mid == mid){
			*client_ସନ୍ଦେଶ = cmsg;
			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}
	}

	ଫେରନ୍ତୁ 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_reconnect_reset_outgoing(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ, *tmp;

	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_ଗଣନା = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_ଗଣନା = 0;
	context->msgs_out.queued_count12 = 0;
	context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_ସନ୍ଦେଶ, tmp){
		db_ସନ୍ଦେଶ_add_to_inflight_stats(&context->msgs_out, client_ସନ୍ଦେଶ);
		ଯଦି(client_ସନ୍ଦେଶ->data.qos > 0){
			util__decrement_send_quota(context);
		}

		ବାଛ(client_ସନ୍ଦେଶ->data.qos){
			ସ୍ଥିତି 0:
				client_ସନ୍ଦେଶ->data.state = mosq_ms_publish_qos0;
				ଭାଙ୍ଗ;
			ସ୍ଥିତି 1:
				client_ସନ୍ଦେଶ->data.state = mosq_ms_publish_qos1;
				ଭାଙ୍ଗ;
			ସ୍ଥିତି 2:
				ଯଦି(client_ସନ୍ଦେଶ->data.state == mosq_ms_wait_for_pubcomp){
					client_ସନ୍ଦେଶ->data.state = mosq_ms_resend_pubrel;
				}ଅନ୍ୟଥା{
					client_ସନ୍ଦେଶ->data.state = mosq_ms_publish_qos2;
				}
				ଭାଙ୍ଗ;
		}
		plugin_persist_handle_client_ସନ୍ଦେଶ_update(context, client_ସନ୍ଦେଶ);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_out.queued, client_ସନ୍ଦେଶ, tmp){
		db_ସନ୍ଦେଶ_add_to_queued_stats(&context->msgs_out, client_ସନ୍ଦେଶ);
	}
	db__fill_inflight_out_from_queue(context);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_reconnect_reset_incoming(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ, *tmp;

	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_ଗଣନା = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_ଗଣନା = 0;
	context->msgs_in.queued_count12 = 0;
	context->msgs_in.inflight_quota = context->msgs_in.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_ସନ୍ଦେଶ, tmp){
		db_ସନ୍ଦେଶ_add_to_inflight_stats(&context->msgs_in, client_ସନ୍ଦେଶ);
		ଯଦି(client_ସନ୍ଦେଶ->data.qos > 0){
			util__decrement_receive_quota(context);
		}

		ଯଦି(client_ସନ୍ଦେଶ->data.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db__message_remove_inflight(context, &context->msgs_in, client_ସନ୍ଦେଶ);
		}ଅନ୍ୟଥା{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_ସନ୍ଦେଶ->data.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_in.queued, client_ସନ୍ଦେଶ, tmp){
		client_ସନ୍ଦେଶ->data.dup = 0;
		db_ସନ୍ଦେଶ_add_to_queued_stats(&context->msgs_in, client_ସନ୍ଦେଶ);
		ଯଦି(db__ready_for_flight(context, mosq_md_in, client_ସନ୍ଦେଶ->data.qos)){
			ବାଛ(client_ସନ୍ଦେଶ->data.qos){
				ସ୍ଥିତି 0:
					client_ସନ୍ଦେଶ->data.state = mosq_ms_publish_qos0;
					ଭାଙ୍ଗ;
				ସ୍ଥିତି 1:
					client_ସନ୍ଦେଶ->data.state = mosq_ms_publish_qos1;
					ଭାଙ୍ଗ;
				ସ୍ଥିତି 2:
					client_ସନ୍ଦେଶ->data.state = mosq_ms_publish_qos2;
					ଭାଙ୍ଗ;
			}
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_ସନ୍ଦେଶ_update(context, client_ସନ୍ଦେଶ);
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_reconnect_reset(ସଂରଚନା mosquitto *context)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = db__message_reconnect_reset_outgoing(context);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}
	ଫେରନ୍ତୁ db__message_reconnect_reset_incoming(context);
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_remove_incoming(ସଂରଚନା mosquitto *context, uint16_t mid)
{
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ, *tmp;

	ଯଦି(!context){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_ସନ୍ଦେଶ, tmp){
		ଯଦି(client_ସନ୍ଦେଶ->data.mid == mid){
			ଯଦି(client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.qos != 2){
				log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.qos);
				ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_in, client_ସନ୍ଦେଶ);
			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_NOT_FOUND;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_release_incoming(ସଂରଚନା mosquitto *context, uint16_t mid)
{
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ, *tmp;
	bool deleted = false;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(!context){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_ସନ୍ଦେଶ, tmp){
		ଯଦି(client_ସନ୍ଦେଶ->data.mid == mid){
			ଯଦି(client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.qos != 2){
				log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.qos);
				ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
			}
			ଅଚଳ ଅକ୍ଷର *topic = client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.topic;
			ଅଚଳ ଅକ୍ଷର *source_id = client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.source_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			ଯଦି(topic == ଖାଲି){
				db__message_remove_inflight(context, &context->msgs_in, client_ସନ୍ଦେଶ);
				deleted = true;
			}ଅନ୍ୟଥା{
				rc = sub__messages_queue(source_id, 2, &client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ);
				ଯଦି(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db__message_remove_inflight(context, &context->msgs_in, client_ସନ୍ଦେଶ);
					deleted = true;
				}ଅନ୍ୟଥା{
					ଫେରନ୍ତୁ 1;
				}
			}
		}
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_ସନ୍ଦେଶ, tmp){
		ଯଦି(db__ready_for_flight(context, mosq_md_in, client_ସନ୍ଦେଶ->data.qos)){
			ଭାଙ୍ଗ;
		}

		ଯଦି(client_ସନ୍ଦେଶ->data.qos == 2){
			send__pubrec(context, client_ସନ୍ଦେଶ->data.mid, 0, ଖାଲି);
			client_ସନ୍ଦେଶ->data.state = mosq_ms_wait_for_pubrel;
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_ସନ୍ଦେଶ_update(context, client_ସନ୍ଦେଶ);
		}
	}
	ଯଦି(deleted){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ MOSQ_ERR_NOT_FOUND;
	}
}


ଶୂନ୍ୟ db__expire_all_messages(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_ସନ୍ଦେଶ, tmp){
		ଯଦି(client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.expiry_time && db.now_real_s > client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.expiry_time){
			ଯଦି(client_ସନ୍ଦେଶ->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_ସନ୍ଦେଶ);
		}
	}
	db__fill_inflight_out_from_queue(context);
	DL_FOREACH_SAFE(context->msgs_out.queued, client_ସନ୍ଦେଶ, tmp){
		ଯଦି(client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.expiry_time && db.now_real_s > client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_ସନ୍ଦେଶ);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.inflight, client_ସନ୍ଦେଶ, tmp){
		ଯଦି(client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.expiry_time && db.now_real_s > client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.expiry_time){
			ଯଦି(client_ସନ୍ଦେଶ->data.qos > 0){
				util__increment_receive_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_in, client_ସନ୍ଦେଶ);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.queued, client_ସନ୍ଦେଶ, tmp){
		ଯଦି(client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.expiry_time && db.now_real_s > client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_in, client_ସନ୍ଦେଶ);
		}
	}
}


ସ୍ଥିର ଶୂନ୍ୟ db__client_messages_check_acl(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data *ସନ୍ଦେଶ_data, ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ **head,
		ଶୂନ୍ୟ (*decrement_stats_fn)(ସଂରଚନା mosquitto_ସନ୍ଦେଶ_data *ସନ୍ଦେଶ_data, ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ))
{
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ, *tmp;
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା access;

	DL_FOREACH_SAFE((*head), client_ସନ୍ଦେଶ, tmp){
		base_ସନ୍ଦେଶ = client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ;
		ଯଦି(client_ସନ୍ଦେଶ->data.direction == mosq_md_out){
			access = MOSQ_ACL_READ;
		}ଅନ୍ୟଥା{
			access = MOSQ_ACL_WRITE;
		}
		ଯଦି(mosquitto_acl_check(context, base_ସନ୍ଦେଶ->data.topic,
				base_ସନ୍ଦେଶ->data.payloadlen, base_ସନ୍ଦେଶ->data.payload,
				base_ସନ୍ଦେଶ->data.qos, base_ସନ୍ଦେଶ->data.retain,
				base_ସନ୍ଦେଶ->data.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*head), client_ସନ୍ଦେଶ);
			decrement_stats_fn(ସନ୍ଦେଶ_data, client_ସନ୍ଦେଶ);
			plugin_persist_handle_client_ସନ୍ଦେଶ_delete(context, client_ସନ୍ଦେଶ);
			db_ସନ୍ଦେଶ_store_ref_dec(&client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ);
			mosquitto_FREE(client_ସନ୍ଦେଶ);
		}
	}
}


ଶୂନ୍ୟ db__check_acl_of_all_messages(ସଂରଚନା mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.inflight, &db_ସନ୍ଦେଶ_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.queued, &db_ସନ୍ଦେଶ_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.inflight, &db_ସନ୍ଦେଶ_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.queued, &db_ସନ୍ଦେଶ_remove_from_queued_stats);
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_write_inflight_out_single(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ)
{
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ;
	mosquitto_property *base_ସନ୍ଦେଶ_props = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	uint16_t mid;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା retries;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା retain;
	ଅଚଳ ଅକ୍ଷର *topic;
	uint8_t qos;
	uint32_t payloadlen;
	ଅଚଳ ଶୂନ୍ୟ *payload;
	uint32_t expiry_interval;
	uint32_t subscription_id;

	base_ସନ୍ଦେଶ = client_ସନ୍ଦେଶ->base_ସନ୍ଦେଶ;

	expiry_interval = 0;
	ଯଦି(base_ସନ୍ଦେଶ->data.expiry_time){
		ଯଦି(db.now_real_s > base_ସନ୍ଦେଶ->data.expiry_time){
			/* Message is expired, must not send. */
			ଯଦି(client_ସନ୍ଦେଶ->data.direction == mosq_md_out && client_ସନ୍ଦେଶ->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_ସନ୍ଦେଶ);
			db__fill_inflight_out_from_queue(context);
			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}ଅନ୍ୟଥା{
			expiry_interval = (uint32_t)(base_ସନ୍ଦେଶ->data.expiry_time - db.now_real_s);
		}
	}
	mid = client_ସନ୍ଦେଶ->data.mid;
	retries = client_ସନ୍ଦେଶ->data.dup;
	retain = client_ସନ୍ଦେଶ->data.retain;
	topic = base_ସନ୍ଦେଶ->data.topic;
	qos = (uint8_t)client_ସନ୍ଦେଶ->data.qos;
	payloadlen = base_ସନ୍ଦେଶ->data.payloadlen;
	payload = base_ସନ୍ଦେଶ->data.payload;
	subscription_id = client_ସନ୍ଦେଶ->data.subscription_identifier;
	base_ସନ୍ଦେଶ_props = base_ସନ୍ଦେଶ->data.properties;

	ବାଛ(client_ସନ୍ଦେଶ->data.state){
		ସ୍ଥିତି mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_ସନ୍ଦେଶ_props, expiry_interval);
			ଯଦି(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_ସନ୍ଦେଶ);
			}ଅନ୍ୟଥା{
				ଫେରନ୍ତୁ rc;
			}
			ଭାଙ୍ଗ;

		ସ୍ଥିତି mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_ସନ୍ଦେଶ_props, expiry_interval);
			ଯଦି(rc == MOSQ_ERR_SUCCESS){
				client_ସନ୍ଦେଶ->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_ସନ୍ଦେଶ->data.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_ସନ୍ଦେଶ_update(context, client_ସନ୍ଦେଶ);
			}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_ସନ୍ଦେଶ);
			}ଅନ୍ୟଥା{
				ଫେରନ୍ତୁ rc;
			}
			ଭାଙ୍ଗ;

		ସ୍ଥିତି mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_ସନ୍ଦେଶ_props, expiry_interval);
			ଯଦି(rc == MOSQ_ERR_SUCCESS){
				client_ସନ୍ଦେଶ->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_ସନ୍ଦେଶ->data.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_ସନ୍ଦେଶ_update(context, client_ସନ୍ଦେଶ);
			}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_ସନ୍ଦେଶ);
			}ଅନ୍ୟଥା{
				ଫେରନ୍ତୁ rc;
			}
			ଭାଙ୍ଗ;

		ସ୍ଥିତି mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, ଖାଲି);
			ଯଦି(!rc){
				client_ସନ୍ଦେଶ->data.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_ସନ୍ଦେଶ_update(context, client_ସନ୍ଦେଶ);
			}ଅନ୍ୟଥା{
				ଫେରନ୍ତୁ rc;
			}
			ଭାଙ୍ଗ;

		ସ୍ଥିତି mosq_ms_invalid:
		ସ୍ଥିତି mosq_ms_send_pubrec:
		ସ୍ଥିତି mosq_ms_resend_pubcomp:
		ସ୍ଥିତି mosq_ms_wait_for_puback:
		ସ୍ଥିତି mosq_ms_wait_for_pubrec:
		ସ୍ଥିତି mosq_ms_wait_for_pubrel:
		ସ୍ଥିତି mosq_ms_wait_for_pubcomp:
		ସ୍ଥିତି mosq_ms_queued:
			ଭାଙ୍ଗ;
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_write_inflight_out_all(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ, *tmp;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(context->state != mosq_cs_active || !net__is_connected(context)){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_ସନ୍ଦେଶ, tmp){
		rc = db__message_write_inflight_out_single(context, client_ସନ୍ଦେଶ);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_write_inflight_out_latest(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ, *next;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(context->state != mosq_cs_active
			|| !net__is_connected(context)
			|| context->msgs_out.inflight == ଖାଲି){

		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	ଯଦି(context->msgs_out.inflight->prev == context->msgs_out.inflight){
		/* Only one message */
		ଫେରନ୍ତୁ db__message_write_inflight_out_single(context, context->msgs_out.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_ସନ୍ଦେଶ = context->msgs_out.inflight->prev;
	ଯେତେବେଳେ(client_ସନ୍ଦେଶ != context->msgs_out.inflight &&
			(client_ସନ୍ଦେଶ->data.state == mosq_ms_publish_qos0
			|| client_ସନ୍ଦେଶ->data.state == mosq_ms_publish_qos1
			|| client_ସନ୍ଦେଶ->data.state == mosq_ms_publish_qos2)){

		client_ସନ୍ଦେଶ = client_ସନ୍ଦେଶ->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	ଯଦି(client_ସନ୍ଦେଶ != context->msgs_out.inflight){
		client_ସନ୍ଦେଶ = client_ସନ୍ଦେଶ->next;
	}

	ଯେତେବେଳେ(client_ସନ୍ଦେଶ){
		next = client_ସନ୍ଦେଶ->next;
		rc = db__message_write_inflight_out_single(context, client_ସନ୍ଦେଶ);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
		client_ସନ୍ଦେଶ = next;
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_write_queued_in(ସଂରଚନା mosquitto *context)
{
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *client_ସନ୍ଦେଶ, *tmp;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(context->state != mosq_cs_active){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_ସନ୍ଦେଶ, tmp){
		ଯଦି(context->msgs_in.inflight_maximum != 0 && context->msgs_in.inflight_quota == 0){
			ଭାଙ୍ଗ;
		}

		ଯଦି(client_ସନ୍ଦେଶ->data.qos == 2){
			client_ସନ୍ଦେଶ->data.state = mosq_ms_send_pubrec;
			db__message_dequeue_first(context, &context->msgs_in);
			rc = send__pubrec(context, client_ସନ୍ଦେଶ->data.mid, 0, ଖାଲି);
			ଯଦି(!rc){
				client_ସନ୍ଦେଶ->data.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_ସନ୍ଦେଶ_update(context, client_ସନ୍ଦେଶ);
			}ଅନ୍ୟଥା{
				plugin_persist_handle_client_ସନ୍ଦେଶ_update(context, client_ସନ୍ଦେଶ);
				ଫେରନ୍ତୁ rc;
			}
		}
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା db__message_write_queued_out(ସଂରଚନା mosquitto *context)
{
	ଯଦି(context->state != mosq_cs_active){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	db__fill_inflight_out_from_queue(context);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}
