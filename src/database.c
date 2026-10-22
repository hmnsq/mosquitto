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

#अन्तर्भाव "config.h"

#अन्तर्भाव <assert.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <utlist.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "send_mosq.h"
#अन्तर्भाव "sys_tree.h"
#अन्तर्भाव "util_mosq.h"


/**
 * Is this context ready to take more in flight messages right now?
 * @param context the client context of interest
 * @param qos qos for the packet of interest
 * @return true if more in flight are allowed.
 */
bool db__ready_for_flight(संरचना mosquitto *context, गणना mosquitto_सन्देशः_direction dir, पूर्णाङ्क qos)
{
	संरचना mosquitto_सन्देशः_data *msgs;
	bool valid_bytes;
	bool valid_गणनम्;

	जइ(dir == mosq_md_out){
		msgs = &context->msgs_out;
	}अण्णहा{
		msgs = &context->msgs_in;
	}

	जइ(msgs->inflight_maximum == 0 && db.config->max_inflight_bytes == 0){
		प्रत्यागम true;
	}

	जइ(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		जइ(db.config->max_queued_messages == 0 && db.config->max_inflight_bytes == 0){
			प्रत्यागम true;
		}
		valid_bytes = ((msgs->inflight_bytes - (ssize_t)db.config->max_inflight_bytes) < (ssize_t)db.config->max_queued_bytes);
		जइ(dir == mosq_md_out){
			valid_गणनम् = context->out_packet_गणनम् < db.config->max_queued_messages;
		}अण्णहा{
			valid_गणनम् = msgs->inflight_गणनम् - msgs->inflight_maximum < db.config->max_queued_messages;
		}

		जइ(db.config->max_queued_messages == 0){
			प्रत्यागम valid_bytes;
		}
		जइ(db.config->max_queued_bytes == 0){
			प्रत्यागम valid_गणनम्;
		}
	}अण्णहा{
		valid_bytes = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_bytes;
		valid_गणनम् = msgs->inflight_quota > 0;

		जइ(msgs->inflight_maximum == 0){
			प्रत्यागम valid_bytes;
		}
		जइ(db.config->max_inflight_bytes == 0){
			प्रत्यागम valid_गणनम्;
		}
	}

	प्रत्यागम valid_bytes && valid_गणनम्;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db__ready_for_queue(संरचना mosquitto *context, पूर्णाङ्क qos, संरचना mosquitto_सन्देशः_data *सन्देशः_data)
{
	पूर्णाङ्क source_गणनम्;
	पूर्णाङ्क adjust_गणनम्;
	long source_bytes;
	ssize_t adjust_bytes = (ssize_t)db.config->max_inflight_bytes;
	bool valid_bytes;
	bool valid_गणनम्;

	जइ(db.config->max_queued_messages == 0 && db.config->max_queued_bytes == 0){
		प्रत्यागम true;
	}

	जइ(qos == 0 && db.config->queue_qos0_messages == false){
		प्रत्यागम false; /* This case is handled in db__ready_for_flight() */
	}अण्णहा{
		source_bytes = (ssize_t)सन्देशः_data->queued_bytes12;
		source_गणनम् = सन्देशः_data->queued_count12;
	}
	adjust_गणनम् = सन्देशः_data->inflight_maximum;

	/* nothing in flight for offline clients */
	जइ(!net__is_connected(context)){
		adjust_bytes = 0;
		adjust_गणनम् = 0;
	}

	valid_bytes = (source_bytes - (ssize_t)adjust_bytes) < (ssize_t)db.config->max_queued_bytes;
	valid_गणनम् = source_गणनम् - adjust_गणनम् < db.config->max_queued_messages;

	जइ(db.config->max_queued_bytes == 0){
		प्रत्यागम valid_गणनम्;
	}
	जइ(db.config->max_queued_messages == 0){
		प्रत्यागम valid_bytes;
	}

	प्रत्यागम valid_bytes && valid_गणनम्;
}


शून्य db_सन्देशः_add_to_inflight_stats(संरचना mosquitto_सन्देशः_data *सन्देशः_data, संरचना mosquitto_client_सन्देशः *client_सन्देशः)
{
	सन्देशः_data->inflight_गणनम्++;
	सन्देशः_data->inflight_bytes += client_सन्देशः->base_सन्देशः->data.payloadlen;
	जइ(client_सन्देशः->data.qos != 0){
		सन्देशः_data->inflight_count12++;
		सन्देशः_data->inflight_bytes12 += client_सन्देशः->base_सन्देशः->data.payloadlen;
	}
}


स्थिर शून्य db_सन्देशः_remove_from_inflight_stats(संरचना mosquitto_सन्देशः_data *सन्देशः_data, संरचना mosquitto_client_सन्देशः *client_सन्देशः)
{
	सन्देशः_data->inflight_गणनम्--;
	सन्देशः_data->inflight_bytes -= client_सन्देशः->base_सन्देशः->data.payloadlen;
	जइ(client_सन्देशः->data.qos != 0){
		सन्देशः_data->inflight_count12--;
		सन्देशः_data->inflight_bytes12 -= client_सन्देशः->base_सन्देशः->data.payloadlen;
	}
}


शून्य db_सन्देशः_add_to_queued_stats(संरचना mosquitto_सन्देशः_data *सन्देशः_data, संरचना mosquitto_client_सन्देशः *client_सन्देशः)
{
	सन्देशः_data->queued_गणनम्++;
	सन्देशः_data->queued_bytes += client_सन्देशः->base_सन्देशः->data.payloadlen;
	जइ(client_सन्देशः->data.qos != 0){
		सन्देशः_data->queued_count12++;
		सन्देशः_data->queued_bytes12 += client_सन्देशः->base_सन्देशः->data.payloadlen;
	}
}


स्थिर शून्य db_सन्देशः_remove_from_queued_stats(संरचना mosquitto_सन्देशः_data *सन्देशः_data, संरचना mosquitto_client_सन्देशः *client_सन्देशः)
{
	सन्देशः_data->queued_गणनम्--;
	सन्देशः_data->queued_bytes -= client_सन्देशः->base_सन्देशः->data.payloadlen;
	जइ(client_सन्देशः->data.qos != 0){
		सन्देशः_data->queued_count12--;
		सन्देशः_data->queued_bytes12 -= client_सन्देशः->base_सन्देशः->data.payloadlen;
	}
}


पूर्णाङ्क db__open(संरचना mosquitto__config *config)
{
	जइ(!config){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = रिक्त;
	db.contexts_by_sock = रिक्त;
	db.contexts_for_free = रिक्त;
#ifdef WITH_BRIDGE
	db.bridges = रिक्त;
	db.bridge_गणनम् = 0;
#endif

	/* Initialize the hashtable */
	db.clientid_index_hash = रिक्त;

	db.normal_subs = रिक्त;
	db.shared_subs = रिक्त;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = रिक्त;

#ifdef WITH_PERSISTENCE
	जइ(persist__restore()){
		प्रत्यागम 1;
	}
#endif

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर शून्य subhier_clean(संरचना mosquitto__subhier **subhier)
{
	संरचना mosquitto__subhier *peer, *subhier_tmp;
	संरचना mosquitto__subleaf *leaf, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		leaf = peer->subs;
		जाव(leaf){
			nextleaf = leaf->next;
			mosquitto_FREE(leaf);
			leaf = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


पूर्णाङ्क db__close(शून्य)
{
	subhier_clean(&db.normal_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db_सन्देशः_store_clean();

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db_सन्देशः_store_add(संरचना mosquitto_base_सन्देशः *base_सन्देशः)
{
	संरचना mosquitto_base_सन्देशः *found;
	unsigned hashv;

	HASH_मानम्(&base_सन्देशः->data.store_id, परिमाण(base_सन्देशः->data.store_id), hashv);
	HASH_FIND_BYHASHVALUE(hh, db.सन्देशः_store, &base_सन्देशः->data.store_id, परिमाण(base_सन्देशः->data.store_id), hashv, found);
	जइ(found == रिक्त){
		HASH_ADD_KEYPTR_BYHASHVALUE(hh, db.सन्देशः_store, &base_सन्देशः->data.store_id, परिमाण(base_सन्देशः->data.store_id), hashv, base_सन्देशः);
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अण्णहा{
		प्रत्यागम MOSQ_ERR_ALREADY_EXISTS;
	}
}


शून्य db_सन्देशः_store_free(संरचना mosquitto_base_सन्देशः *base_सन्देशः)
{
	mosquitto_FREE(base_सन्देशः->data.source_id);
	mosquitto_FREE(base_सन्देशः->data.source_username);
	जइ(base_सन्देशः->dest_ids){
		प्रति(पूर्णाङ्क i=0; i<base_सन्देशः->dest_id_गणनम्; i++){
			mosquitto_FREE(base_सन्देशः->dest_ids[i]);
		}
		mosquitto_FREE(base_सन्देशः->dest_ids);
	}
	mosquitto_FREE(base_सन्देशः->data.topic);
	mosquitto_property_free_all(&base_सन्देशः->data.properties);
	mosquitto_FREE(base_सन्देशः->data.payload);
	mosquitto_FREE(base_सन्देशः);
}


शून्य db_सन्देशः_store_remove(संरचना mosquitto_base_सन्देशः *base_सन्देशः, bool notify)
{
	जइ(base_सन्देशः == रिक्त){
		प्रत्यागम;
	}
	HASH_DELETE(hh, db.सन्देशः_store, base_सन्देशः);
	db.सन्देशः_store_गणनम्--;
	db.सन्देशः_store_bytes -= base_सन्देशः->data.payloadlen;
	जइ(notify == true){
		plugin_persist_handle_base_सन्देशः_delete(base_सन्देशः);
	}
	db_सन्देशः_store_free(base_सन्देशः);
}


शून्य db_सन्देशः_store_clean(शून्य)
{
	संरचना mosquitto_base_सन्देशः *base_सन्देशः, *base_सन्देशः_tmp;

	HASH_ITER(hh, db.सन्देशः_store, base_सन्देशः, base_सन्देशः_tmp){
		db_सन्देशः_store_remove(base_सन्देशः, false);
	}
}


शून्य db_सन्देशः_store_ref_inc(संरचना mosquitto_base_सन्देशः *base_सन्देशः)
{
	base_सन्देशः->ref_गणनम्++;
}


शून्य db_सन्देशः_store_ref_dec(संरचना mosquitto_base_सन्देशः **base_सन्देशः)
{
	(*base_सन्देशः)->ref_गणनम्--;
	जइ((*base_सन्देशः)->ref_गणनम् == 0){
		db_सन्देशः_store_remove(*base_सन्देशः, true);
		*base_सन्देशः = रिक्त;
	}
}


शून्य db_सन्देशः_store_compact(शून्य)
{
	संरचना mosquitto_base_सन्देशः *base_सन्देशः, *base_सन्देशः_tmp;

	HASH_ITER(hh, db.सन्देशः_store, base_सन्देशः, base_सन्देशः_tmp){
		जइ(base_सन्देशः->ref_गणनम् < 1){
			db_सन्देशः_store_remove(base_सन्देशः, true);
		}
	}
}


स्थिर शून्य db__message_remove_inflight(संरचना mosquitto *context, संरचना mosquitto_सन्देशः_data *सन्देशः_data, संरचना mosquitto_client_सन्देशः *item)
{
	जइ(!context || !सन्देशः_data || !item){
		प्रत्यागम;
	}

	plugin_persist_handle_client_सन्देशः_delete(context, item);

	DL_DELETE(सन्देशः_data->inflight, item);
	जइ(item->base_सन्देशः){
		db_सन्देशः_remove_from_inflight_stats(सन्देशः_data, item);
		db_सन्देशः_store_ref_dec(&item->base_सन्देशः);
	}

	mosquitto_FREE(item);
}


स्थिर शून्य db__message_remove_queued(संरचना mosquitto *context, संरचना mosquitto_सन्देशः_data *सन्देशः_data, संरचना mosquitto_client_सन्देशः *item)
{
	जइ(!context || !सन्देशः_data || !item){
		प्रत्यागम;
	}

	plugin_persist_handle_client_सन्देशः_delete(context, item);

	DL_DELETE(सन्देशः_data->queued, item);
	जइ(item->base_सन्देशः){
		db_सन्देशः_remove_from_queued_stats(सन्देशः_data, item);
		db_सन्देशः_store_ref_dec(&item->base_सन्देशः);
	}

	mosquitto_FREE(item);
}


स्थिर शून्य db__fill_inflight_out_from_queue(संरचना mosquitto *context)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.queued, client_सन्देशः, tmp){
		जइ(!db__ready_for_flight(context, mosq_md_out, client_सन्देशः->data.qos)){
			प्रत्यागम;
		}
		विकल्प(client_सन्देशः->data.qos){
			पक्ष 0:
				client_सन्देशः->data.state = mosq_ms_publish_qos0;
				भङ्ग;
			पक्ष 1:
				client_सन्देशः->data.state = mosq_ms_publish_qos1;
				भङ्ग;
			पक्ष 2:
				client_सन्देशः->data.state = mosq_ms_publish_qos2;
				भङ्ग;
		}
		जइ(client_सन्देशः->base_सन्देशः->data.expiry_time && db.now_real_s > client_सन्देशः->base_सन्देशः->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_सन्देशः);
			अनुवर्त;
		}
		plugin_persist_handle_client_सन्देशः_update(context, client_सन्देशः);
		db__message_dequeue_first(context, &context->msgs_out);
	}
}


शून्य db__message_dequeue_first(संरचना mosquitto *context, संरचना mosquitto_सन्देशः_data *सन्देशः_data)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः;

	UNUSED(context);

	client_सन्देशः = सन्देशः_data->queued;
	DL_DELETE(सन्देशः_data->queued, client_सन्देशः);
	DL_APPEND(सन्देशः_data->inflight, client_सन्देशः);
	जइ(सन्देशः_data->inflight_quota > 0){
		सन्देशः_data->inflight_quota--;
	}

	db_सन्देशः_remove_from_queued_stats(सन्देशः_data, client_सन्देशः);
	db_सन्देशः_add_to_inflight_stats(सन्देशः_data, client_सन्देशः);
}


पूर्णाङ्क db__message_delete_outgoing(संरचना mosquitto *context, uint16_t mid, गणना mosquitto_सन्देशः_state expect_state, पूर्णाङ्क qos)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;
	bool deleted = false;

	जइ(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_सन्देशः, tmp){
		जइ(client_सन्देशः->data.mid == mid){
			जइ(client_सन्देशः->data.qos != qos){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_सन्देशः->data.qos, qos);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}अण्णहा जइ(qos == 2 && client_सन्देशः->data.state != expect_state && expect_state != mosq_ms_any){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_सन्देशः->data.state, expect_state);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_out, client_सन्देशः);
			deleted = true;
			भङ्ग;
		}
	}

	जइ(deleted == false){
		DL_FOREACH_SAFE(context->msgs_out.queued, client_सन्देशः, tmp){
			जइ(client_सन्देशः->data.mid == mid){
				जइ(client_सन्देशः->data.qos != qos){
					प्रत्यागम MOSQ_ERR_PROTOCOL;
				}अण्णहा जइ(qos == 2 && client_सन्देशः->data.state != expect_state && expect_state != mosq_ms_any){
					प्रत्यागम MOSQ_ERR_PROTOCOL;
				}
				db__message_remove_queued(context, &context->msgs_out, client_सन्देशः);
				भङ्ग;
			}
		}
	}
	db__fill_inflight_out_from_queue(context);
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	प्रत्यागम db__message_write_inflight_out_latest(context);
}


/* Only for QoS 2 messages */
पूर्णाङ्क db_message_निवेशय_incoming(संरचना mosquitto *context, uint64_t cmsg_id, संरचना mosquitto_base_सन्देशः *base_सन्देशः, bool persist)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः;
	संरचना mosquitto_सन्देशः_data *सन्देशः_data;
	गणना mosquitto_सन्देशः_state state = mosq_ms_invalid;
	पूर्णाङ्क rc = 0;

	प्रतिज्ञा(base_सन्देशः);
	जइ(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	जइ(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		प्रत्यागम MOSQ_ERR_SUCCESS;

	}
	सन्देशः_data = &context->msgs_in;

	जइ(db__ready_for_flight(context, mosq_md_in, base_सन्देशः->data.qos)){
		state = mosq_ms_wait_for_pubrel;
	}अण्णहा जइ(base_सन्देशः->data.qos != 0 && db__ready_for_queue(context, base_सन्देशः->data.qos, सन्देशः_data)){
		state = mosq_ms_queued;
		rc = 2;
	}अण्णहा{
		/* Dropping message due to full queue. */
		जइ(context->is_dropping == false){
			context->is_dropping = true;
			log__printf(रिक्त, MOSQ_LOG_NOTICE,
					"Outgoing messages are being dropped for client %s.",
					context->id);
		}
		metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		प्रत्यागम 2;
	}

	प्रतिज्ञा(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	जइ(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_सन्देशः = mosquitto_malloc(परिमाण(संरचना mosquitto_client_सन्देशः));
	जइ(!client_सन्देशः){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	client_सन्देशः->prev = रिक्त;
	client_सन्देशः->next = रिक्त;
	जइ(cmsg_id){
		client_सन्देशः->data.cmsg_id = cmsg_id;
	}अण्णहा{
		client_सन्देशः->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_सन्देशः->base_सन्देशः = base_सन्देशः;
	db_सन्देशः_store_ref_inc(client_सन्देशः->base_सन्देशः);
	client_सन्देशः->data.mid = base_सन्देशः->data.source_mid;
	client_सन्देशः->data.direction = mosq_md_in;
	client_सन्देशः->data.state = (uint8_t)state;
	client_सन्देशः->data.dup = false;
	जइ(base_सन्देशः->data.qos > context->max_qos){
		client_सन्देशः->data.qos = context->max_qos;
	}अण्णहा{
		client_सन्देशः->data.qos = base_सन्देशः->data.qos;
	}
	client_सन्देशः->data.retain = base_सन्देशः->data.retain;
	client_सन्देशः->data.subscription_identifier = 0;

	जइ(state == mosq_ms_queued){
		DL_APPEND(सन्देशः_data->queued, client_सन्देशः);
		db_सन्देशः_add_to_queued_stats(सन्देशः_data, client_सन्देशः);
	}अण्णहा{
		DL_APPEND(सन्देशः_data->inflight, client_सन्देशः);
		db_सन्देशः_add_to_inflight_stats(सन्देशः_data, client_सन्देशः);
	}

	जइ(persist && context->is_persisted){
		plugin_persist_handle_base_सन्देशः_add(client_सन्देशः->base_सन्देशः);
		plugin_persist_handle_client_सन्देशः_add(context, client_सन्देशः);
	}

	जइ(client_सन्देशः->base_सन्देशः->data.qos > 0){
		util__decrement_receive_quota(context);
	}
	प्रत्यागम rc;
}


पूर्णाङ्क db_message_निवेशय_outgoing(संरचना mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, संरचना mosquitto_base_सन्देशः *base_सन्देशः, uint32_t subscription_identifier, bool update, bool persist)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः;
	संरचना mosquitto_सन्देशः_data *सन्देशः_data;
	गणना mosquitto_सन्देशः_state state = mosq_ms_invalid;
	पूर्णाङ्क rc = 0;
	वर्ण **dest_ids;

	प्रतिज्ञा(base_सन्देशः);
	जइ(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	जइ(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		प्रत्यागम MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_sent++;

	सन्देशः_data = &context->msgs_out;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	जइ(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && base_सन्देशः->dest_ids){

		प्रति(पूर्णाङ्क i=0; i<base_सन्देशः->dest_id_गणनम्; i++){
			जइ(base_सन्देशः->dest_ids[i] && !strcmp(base_सन्देशः->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				प्रत्यागम MOSQ_ERR_SUCCESS;
			}
		}
	}
	जइ(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		जइ(qos == 0 && !db.config->queue_qos0_messages){
			जइ(!context->bridge){
				प्रत्यागम 2;
			}अण्णहा{
				जइ(context->bridge->start_type != bst_lazy){
					प्रत्यागम 2;
				}
			}
		}
		जइ(context->bridge && context->bridge->clean_start_local == true){
			प्रत्यागम 2;
		}
	}

	जइ(net__is_connected(context)){
		जइ(db__ready_for_flight(context, mosq_md_out, qos)){
			विकल्प(qos){
				पक्ष 0:
					state = mosq_ms_publish_qos0;
					भङ्ग;
				पक्ष 1:
					state = mosq_ms_publish_qos1;
					भङ्ग;
				पक्ष 2:
					state = mosq_ms_publish_qos2;
					भङ्ग;
			}
		}अण्णहा जइ(qos != 0 && db__ready_for_queue(context, qos, सन्देशः_data)){
			state = mosq_ms_queued;
			rc = 2;
		}अण्णहा{
			/* Dropping message due to full queue. */
			जइ(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(रिक्त, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			प्रत्यागम 2;
		}
	}अण्णहा{
		जइ(db__ready_for_queue(context, qos, सन्देशः_data)){
			state = mosq_ms_queued;
		}अण्णहा{
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			जइ(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(रिक्त, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			प्रत्यागम 2;
		}
	}
	प्रतिज्ञा(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	जइ(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_सन्देशः = mosquitto_malloc(परिमाण(संरचना mosquitto_client_सन्देशः));
	जइ(!client_सन्देशः){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	client_सन्देशः->prev = रिक्त;
	client_सन्देशः->next = रिक्त;
	जइ(cmsg_id){
		client_सन्देशः->data.cmsg_id = cmsg_id;
	}अण्णहा{
		client_सन्देशः->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_सन्देशः->base_सन्देशः = base_सन्देशः;
	db_सन्देशः_store_ref_inc(client_सन्देशः->base_सन्देशः);
	client_सन्देशः->data.mid = mid;
	client_सन्देशः->data.direction = mosq_md_out;
	client_सन्देशः->data.state = (uint8_t)state;
	client_सन्देशः->data.dup = false;
	जइ(qos > context->max_qos){
		client_सन्देशः->data.qos = context->max_qos;
	}अण्णहा{
		client_सन्देशः->data.qos = qos;
	}
	client_सन्देशः->data.retain = retain;
	client_सन्देशः->data.subscription_identifier = subscription_identifier;

	जइ(state == mosq_ms_queued){
		DL_APPEND(सन्देशः_data->queued, client_सन्देशः);
		db_सन्देशः_add_to_queued_stats(सन्देशः_data, client_सन्देशः);
	}अण्णहा{
		DL_APPEND(सन्देशः_data->inflight, client_सन्देशः);
		db_सन्देशः_add_to_inflight_stats(सन्देशः_data, client_सन्देशः);
	}

	जइ(persist && context->is_persisted){
		plugin_persist_handle_base_सन्देशः_add(client_सन्देशः->base_सन्देशः);
		plugin_persist_handle_client_सन्देशः_add(context, client_सन्देशः);
	}

	जइ(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(base_सन्देशः->dest_ids, परिमाण(वर्ण *)*(size_t)(base_सन्देशः->dest_id_गणनम्+1));
		जइ(dest_ids){
			base_सन्देशः->dest_ids = dest_ids;
			base_सन्देशः->dest_id_गणनम्++;
			base_सन्देशः->dest_ids[base_सन्देशः->dest_id_गणनम्-1] = mosquitto_strdup(context->id);
			जइ(!base_सन्देशः->dest_ids[base_सन्देशः->dest_id_गणनम्-1]){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
		}अण्णहा{
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}
#ifdef WITH_BRIDGE
	जइ(context->bridge && context->bridge->start_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_out.inflight_गणनम् + context->msgs_out.queued_गणनम् >= context->bridge->threshold){

		context->bridge->lazy_reconnect = true;
	}
#endif

	जइ(client_सन्देशः->data.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	जइ(update){
		rc = db__message_write_inflight_out_latest(context);
		जइ(rc){
			प्रत्यागम rc;
		}
		rc = db__message_write_queued_out(context);
		जइ(rc){
			प्रत्यागम rc;
		}
	}

	प्रत्यागम rc;
}


स्थिर अन्तःस्थ पूर्णाङ्क db__message_update_outgoing_state(संरचना mosquitto *context, संरचना mosquitto_client_सन्देशः *head,
		uint16_t mid, गणना mosquitto_सन्देशः_state state, पूर्णाङ्क qos, bool persist)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः;

	DL_FOREACH(head, client_सन्देशः){
		जइ(client_सन्देशः->data.mid == mid){
			जइ(client_सन्देशः->data.qos != qos){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_सन्देशः->data.qos, qos);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}
			client_सन्देशः->data.state = (uint8_t)state;
			जइ(persist){
				plugin_persist_handle_client_सन्देशः_update(context, client_सन्देशः);
			}
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}
	}
	प्रत्यागम MOSQ_ERR_NOT_FOUND;
}


पूर्णाङ्क db__message_update_outgoing(संरचना mosquitto *context, uint16_t mid, गणना mosquitto_सन्देशः_state state, पूर्णाङ्क qos, bool persist)
{
	पूर्णाङ्क rc;

	rc = db__message_update_outgoing_state(context, context->msgs_out.inflight, mid, state, qos, persist);
	जइ(!persist && rc == MOSQ_ERR_NOT_FOUND){
		rc = db__message_update_outgoing_state(context, context->msgs_out.queued, mid, state, qos, persist);
	}
	प्रत्यागम rc;
}


स्थिर शून्य db__messages_delete_list(संरचना mosquitto_client_सन्देशः **head)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;

	DL_FOREACH_SAFE(*head, client_सन्देशः, tmp){
		DL_DELETE(*head, client_सन्देशः);
		db_सन्देशः_store_ref_dec(&client_सन्देशः->base_सन्देशः);
		mosquitto_FREE(client_सन्देशः);
	}
	*head = रिक्त;
}


पूर्णाङ्क db__messages_delete_incoming(संरचना mosquitto *context)
{
	जइ(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_in.inflight);
	db__messages_delete_list(&context->msgs_in.queued);
	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_गणनम् = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_गणनम् = 0;
	context->msgs_in.queued_count12 = 0;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db__messages_delete_outgoing(संरचना mosquitto *context)
{
	जइ(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_out.inflight);
	db__messages_delete_list(&context->msgs_out.queued);
	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_गणनम् = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_गणनम् = 0;
	context->msgs_out.queued_count12 = 0;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db__messages_delete(संरचना mosquitto *context, bool force_free)
{
	जइ(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	जइ(force_free || context->clean_start || (context->bridge && context->bridge->clean_start)){
		db__messages_delete_incoming(context);
	}

	जइ(force_free || (context->bridge && context->bridge->clean_start_local)
			|| (context->bridge == रिक्त && context->clean_start)){

		db__messages_delete_outgoing(context);
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db__messages_easy_queue(संरचना mosquitto *context, ध्रुव वर्ण *topic, uint8_t qos, uint32_t payloadlen, ध्रुव शून्य *payload, पूर्णाङ्क retain, uint32_t message_expiry_interval, mosquitto_property **properties)
{
	संरचना mosquitto_base_सन्देशः *base_सन्देशः;
	ध्रुव वर्ण *source_id;
	गणना mosquitto_सन्देशः_origin origin;

	जइ(!topic){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	base_सन्देशः = mosquitto_calloc(1, परिमाण(संरचना mosquitto_base_सन्देशः));
	जइ(base_सन्देशः == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	base_सन्देशः->data.topic = mosquitto_strdup(topic);
	जइ(base_सन्देशः->data.topic == रिक्त){
		db_सन्देशः_store_free(base_सन्देशः);
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	base_सन्देशः->data.qos = qos;
	जइ(db.config->retain_available == false){
		base_सन्देशः->data.retain = 0;
	}अण्णहा{
		base_सन्देशः->data.retain = retain;
	}

	base_सन्देशः->data.payloadlen = payloadlen;
	जइ(payloadlen > 0){
		base_सन्देशः->data.payload = mosquitto_malloc(base_सन्देशः->data.payloadlen+1);
		जइ(base_सन्देशः->data.payload == रिक्त){
			db_सन्देशः_store_free(base_सन्देशः);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_सन्देशः->data.payload)[base_सन्देशः->data.payloadlen] = 0;
		memcpy(base_सन्देशः->data.payload, payload, base_सन्देशः->data.payloadlen);
	}

	जइ(context && context->id){
		source_id = context->id;
	}अण्णहा{
		source_id = "";
	}
	जइ(properties){
		base_सन्देशः->data.properties = *properties;
		*properties = रिक्त;
	}

	जइ(context){
		origin = mosq_mo_client;
	}अण्णहा{
		origin = mosq_mo_broker;
	}
	जइ(db__message_store(context, base_सन्देशः, &message_expiry_interval, origin)){
		प्रत्यागम 1;
	}

	प्रत्यागम sub__messages_queue(source_id, base_सन्देशः->data.qos, &base_सन्देशः);
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
uint64_t db_new_सन्देशः_id(शून्य)
{
#ifdef WIN32
	FILETIME ftime;
	uint64_t ftime64;
#अण्णहा
	संरचना timespec ts;
#endif
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	long nsec;

	id = db.ग्रन्थिः_id_shifted; /* Top 10-bits */

#ifdef WIN32
	GetSystemTimePreciseAsFileTime(&ftime);
	ftime64 = (((uint64_t)ftime.dw_उच्चः_Date_Time)<<32) + ftime.dw_नीचः_Date_Time;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (long)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#अण्णहा
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#endif
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	जइ(id <= db.last_db_id){
		id = db.last_db_id + 1;
	}
	db.last_db_id = id;

	प्रत्यागम id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
पूर्णाङ्क db__message_store(ध्रुव संरचना mosquitto *source, संरचना mosquitto_base_सन्देशः *base_सन्देशः, uint32_t *message_expiry_interval, गणना mosquitto_सन्देशः_origin origin)
{
	पूर्णाङ्क rc;

	प्रतिज्ञा(base_सन्देशः);

	जइ(source && source->id){
		base_सन्देशः->data.source_id = mosquitto_strdup(source->id);
	}अण्णहा{
		base_सन्देशः->data.source_id = mosquitto_strdup("");
	}
	जइ(!base_सन्देशः->data.source_id){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_सन्देशः_store_free(base_सन्देशः);
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	जइ(source && source->username){
		base_सन्देशः->data.source_username = mosquitto_strdup(source->username);
		जइ(!base_सन्देशः->data.source_username){
			db_सन्देशः_store_free(base_सन्देशः);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}
	जइ(source){
		base_सन्देशः->source_listener = source->listener;
	}
	base_सन्देशः->origin = origin;
	जइ(message_expiry_interval){
		जइ(*message_expiry_interval > 0 && *message_expiry_interval != सन्देशः_EXPIRY_INFINITE){
			base_सन्देशः->data.expiry_time = db.now_real_s + *message_expiry_interval;
		}अण्णहा{
			base_सन्देशः->data.expiry_time = 0;
		}
	}

	base_सन्देशः->dest_ids = रिक्त;
	base_सन्देशः->dest_id_गणनम् = 0;
	db.सन्देशः_store_गणनम्++;
	db.सन्देशः_store_bytes += base_सन्देशः->data.payloadlen;

	जइ(!base_सन्देशः->data.store_id){
		base_सन्देशः->data.store_id = db_new_सन्देशः_id();
	}

	rc = db_सन्देशः_store_add(base_सन्देशः);
	जइ(rc){
		db_सन्देशः_store_free(base_सन्देशः);
		प्रत्यागम rc;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db__message_store_find(संरचना mosquitto *context, uint16_t mid, संरचना mosquitto_client_सन्देशः **client_सन्देशः)
{
	संरचना mosquitto_client_सन्देशः *cmsg;

	*client_सन्देशः = रिक्त;

	जइ(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_in.inflight, cmsg){
		जइ(cmsg->base_सन्देशः->data.source_mid == mid){
			*client_सन्देशः = cmsg;
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_in.queued, cmsg){
		जइ(cmsg->base_सन्देशः->data.source_mid == mid){
			*client_सन्देशः = cmsg;
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}
	}

	प्रत्यागम 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
स्थिर पूर्णाङ्क db__message_reconnect_reset_outgoing(संरचना mosquitto *context)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;

	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_गणनम् = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_गणनम् = 0;
	context->msgs_out.queued_count12 = 0;
	context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_सन्देशः, tmp){
		db_सन्देशः_add_to_inflight_stats(&context->msgs_out, client_सन्देशः);
		जइ(client_सन्देशः->data.qos > 0){
			util__decrement_send_quota(context);
		}

		विकल्प(client_सन्देशः->data.qos){
			पक्ष 0:
				client_सन्देशः->data.state = mosq_ms_publish_qos0;
				भङ्ग;
			पक्ष 1:
				client_सन्देशः->data.state = mosq_ms_publish_qos1;
				भङ्ग;
			पक्ष 2:
				जइ(client_सन्देशः->data.state == mosq_ms_wait_for_pubcomp){
					client_सन्देशः->data.state = mosq_ms_resend_pubrel;
				}अण्णहा{
					client_सन्देशः->data.state = mosq_ms_publish_qos2;
				}
				भङ्ग;
		}
		plugin_persist_handle_client_सन्देशः_update(context, client_सन्देशः);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_out.queued, client_सन्देशः, tmp){
		db_सन्देशः_add_to_queued_stats(&context->msgs_out, client_सन्देशः);
	}
	db__fill_inflight_out_from_queue(context);

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
स्थिर पूर्णाङ्क db__message_reconnect_reset_incoming(संरचना mosquitto *context)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;

	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_गणनम् = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_गणनम् = 0;
	context->msgs_in.queued_count12 = 0;
	context->msgs_in.inflight_quota = context->msgs_in.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_सन्देशः, tmp){
		db_सन्देशः_add_to_inflight_stats(&context->msgs_in, client_सन्देशः);
		जइ(client_सन्देशः->data.qos > 0){
			util__decrement_receive_quota(context);
		}

		जइ(client_सन्देशः->data.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db__message_remove_inflight(context, &context->msgs_in, client_सन्देशः);
		}अण्णहा{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_सन्देशः->data.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_in.queued, client_सन्देशः, tmp){
		client_सन्देशः->data.dup = 0;
		db_सन्देशः_add_to_queued_stats(&context->msgs_in, client_सन्देशः);
		जइ(db__ready_for_flight(context, mosq_md_in, client_सन्देशः->data.qos)){
			विकल्प(client_सन्देशः->data.qos){
				पक्ष 0:
					client_सन्देशः->data.state = mosq_ms_publish_qos0;
					भङ्ग;
				पक्ष 1:
					client_सन्देशः->data.state = mosq_ms_publish_qos1;
					भङ्ग;
				पक्ष 2:
					client_सन्देशः->data.state = mosq_ms_publish_qos2;
					भङ्ग;
			}
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_सन्देशः_update(context, client_सन्देशः);
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db__message_reconnect_reset(संरचना mosquitto *context)
{
	पूर्णाङ्क rc;

	rc = db__message_reconnect_reset_outgoing(context);
	जइ(rc){
		प्रत्यागम rc;
	}
	प्रत्यागम db__message_reconnect_reset_incoming(context);
}


पूर्णाङ्क db__message_remove_incoming(संरचना mosquitto *context, uint16_t mid)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;

	जइ(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_सन्देशः, tmp){
		जइ(client_सन्देशः->data.mid == mid){
			जइ(client_सन्देशः->base_सन्देशः->data.qos != 2){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_सन्देशः->base_सन्देशः->data.qos);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_in, client_सन्देशः);
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}
	}

	प्रत्यागम MOSQ_ERR_NOT_FOUND;
}


पूर्णाङ्क db__message_release_incoming(संरचना mosquitto *context, uint16_t mid)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;
	bool deleted = false;
	पूर्णाङ्क rc;

	जइ(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_सन्देशः, tmp){
		जइ(client_सन्देशः->data.mid == mid){
			जइ(client_सन्देशः->base_सन्देशः->data.qos != 2){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_सन्देशः->base_सन्देशः->data.qos);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}
			ध्रुव वर्ण *topic = client_सन्देशः->base_सन्देशः->data.topic;
			ध्रुव वर्ण *source_id = client_सन्देशः->base_सन्देशः->data.source_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			जइ(topic == रिक्त){
				db__message_remove_inflight(context, &context->msgs_in, client_सन्देशः);
				deleted = true;
			}अण्णहा{
				rc = sub__messages_queue(source_id, 2, &client_सन्देशः->base_सन्देशः);
				जइ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db__message_remove_inflight(context, &context->msgs_in, client_सन्देशः);
					deleted = true;
				}अण्णहा{
					प्रत्यागम 1;
				}
			}
		}
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_सन्देशः, tmp){
		जइ(db__ready_for_flight(context, mosq_md_in, client_सन्देशः->data.qos)){
			भङ्ग;
		}

		जइ(client_सन्देशः->data.qos == 2){
			send__pubrec(context, client_सन्देशः->data.mid, 0, रिक्त);
			client_सन्देशः->data.state = mosq_ms_wait_for_pubrel;
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_सन्देशः_update(context, client_सन्देशः);
		}
	}
	जइ(deleted){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अण्णहा{
		प्रत्यागम MOSQ_ERR_NOT_FOUND;
	}
}


शून्य db__expire_all_messages(संरचना mosquitto *context)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_सन्देशः, tmp){
		जइ(client_सन्देशः->base_सन्देशः->data.expiry_time && db.now_real_s > client_सन्देशः->base_सन्देशः->data.expiry_time){
			जइ(client_सन्देशः->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_सन्देशः);
		}
	}
	db__fill_inflight_out_from_queue(context);
	DL_FOREACH_SAFE(context->msgs_out.queued, client_सन्देशः, tmp){
		जइ(client_सन्देशः->base_सन्देशः->data.expiry_time && db.now_real_s > client_सन्देशः->base_सन्देशः->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_सन्देशः);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.inflight, client_सन्देशः, tmp){
		जइ(client_सन्देशः->base_सन्देशः->data.expiry_time && db.now_real_s > client_सन्देशः->base_सन्देशः->data.expiry_time){
			जइ(client_सन्देशः->data.qos > 0){
				util__increment_receive_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_in, client_सन्देशः);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.queued, client_सन्देशः, tmp){
		जइ(client_सन्देशः->base_सन्देशः->data.expiry_time && db.now_real_s > client_सन्देशः->base_सन्देशः->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_in, client_सन्देशः);
		}
	}
}


स्थिर शून्य db__client_messages_check_acl(संरचना mosquitto *context, संरचना mosquitto_सन्देशः_data *सन्देशः_data, संरचना mosquitto_client_सन्देशः **head,
		शून्य (*decrement_stats_fn)(संरचना mosquitto_सन्देशः_data *सन्देशः_data, संरचना mosquitto_client_सन्देशः *client_सन्देशः))
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;
	संरचना mosquitto_base_सन्देशः *base_सन्देशः;
	पूर्णाङ्क access;

	DL_FOREACH_SAFE((*head), client_सन्देशः, tmp){
		base_सन्देशः = client_सन्देशः->base_सन्देशः;
		जइ(client_सन्देशः->data.direction == mosq_md_out){
			access = MOSQ_ACL_READ;
		}अण्णहा{
			access = MOSQ_ACL_WRITE;
		}
		जइ(mosquitto_acl_check(context, base_सन्देशः->data.topic,
				base_सन्देशः->data.payloadlen, base_सन्देशः->data.payload,
				base_सन्देशः->data.qos, base_सन्देशः->data.retain,
				base_सन्देशः->data.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*head), client_सन्देशः);
			decrement_stats_fn(सन्देशः_data, client_सन्देशः);
			plugin_persist_handle_client_सन्देशः_delete(context, client_सन्देशः);
			db_सन्देशः_store_ref_dec(&client_सन्देशः->base_सन्देशः);
			mosquitto_FREE(client_सन्देशः);
		}
	}
}


शून्य db__check_acl_of_all_messages(संरचना mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.inflight, &db_सन्देशः_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.queued, &db_सन्देशः_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.inflight, &db_सन्देशः_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.queued, &db_सन्देशः_remove_from_queued_stats);
}


स्थिर पूर्णाङ्क db__message_write_inflight_out_single(संरचना mosquitto *context, संरचना mosquitto_client_सन्देशः *client_सन्देशः)
{
	संरचना mosquitto_base_सन्देशः *base_सन्देशः;
	mosquitto_property *base_सन्देशः_props = रिक्त;
	पूर्णाङ्क rc;
	uint16_t mid;
	पूर्णाङ्क retries;
	पूर्णाङ्क retain;
	ध्रुव वर्ण *topic;
	uint8_t qos;
	uint32_t payloadlen;
	ध्रुव शून्य *payload;
	uint32_t expiry_interval;
	uint32_t subscription_id;

	base_सन्देशः = client_सन्देशः->base_सन्देशः;

	expiry_interval = 0;
	जइ(base_सन्देशः->data.expiry_time){
		जइ(db.now_real_s > base_सन्देशः->data.expiry_time){
			/* Message is expired, must not send. */
			जइ(client_सन्देशः->data.direction == mosq_md_out && client_सन्देशः->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_सन्देशः);
			db__fill_inflight_out_from_queue(context);
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}अण्णहा{
			expiry_interval = (uint32_t)(base_सन्देशः->data.expiry_time - db.now_real_s);
		}
	}
	mid = client_सन्देशः->data.mid;
	retries = client_सन्देशः->data.dup;
	retain = client_सन्देशः->data.retain;
	topic = base_सन्देशः->data.topic;
	qos = (uint8_t)client_सन्देशः->data.qos;
	payloadlen = base_सन्देशः->data.payloadlen;
	payload = base_सन्देशः->data.payload;
	subscription_id = client_सन्देशः->data.subscription_identifier;
	base_सन्देशः_props = base_सन्देशः->data.properties;

	विकल्प(client_सन्देशः->data.state){
		पक्ष mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_सन्देशः_props, expiry_interval);
			जइ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_सन्देशः);
			}अण्णहा{
				प्रत्यागम rc;
			}
			भङ्ग;

		पक्ष mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_सन्देशः_props, expiry_interval);
			जइ(rc == MOSQ_ERR_SUCCESS){
				client_सन्देशः->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_सन्देशः->data.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_सन्देशः_update(context, client_सन्देशः);
			}अण्णहा जइ(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_सन्देशः);
			}अण्णहा{
				प्रत्यागम rc;
			}
			भङ्ग;

		पक्ष mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_सन्देशः_props, expiry_interval);
			जइ(rc == MOSQ_ERR_SUCCESS){
				client_सन्देशः->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_सन्देशः->data.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_सन्देशः_update(context, client_सन्देशः);
			}अण्णहा जइ(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_सन्देशः);
			}अण्णहा{
				प्रत्यागम rc;
			}
			भङ्ग;

		पक्ष mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, रिक्त);
			जइ(!rc){
				client_सन्देशः->data.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_सन्देशः_update(context, client_सन्देशः);
			}अण्णहा{
				प्रत्यागम rc;
			}
			भङ्ग;

		पक्ष mosq_ms_invalid:
		पक्ष mosq_ms_send_pubrec:
		पक्ष mosq_ms_resend_pubcomp:
		पक्ष mosq_ms_wait_for_puback:
		पक्ष mosq_ms_wait_for_pubrec:
		पक्ष mosq_ms_wait_for_pubrel:
		पक्ष mosq_ms_wait_for_pubcomp:
		पक्ष mosq_ms_queued:
			भङ्ग;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db__message_write_inflight_out_all(संरचना mosquitto *context)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;
	पूर्णाङ्क rc;

	जइ(context->state != mosq_cs_active || !net__is_connected(context)){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_सन्देशः, tmp){
		rc = db__message_write_inflight_out_single(context, client_सन्देशः);
		जइ(rc){
			प्रत्यागम rc;
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db__message_write_inflight_out_latest(संरचना mosquitto *context)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *next;
	पूर्णाङ्क rc;

	जइ(context->state != mosq_cs_active
			|| !net__is_connected(context)
			|| context->msgs_out.inflight == रिक्त){

		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	जइ(context->msgs_out.inflight->prev == context->msgs_out.inflight){
		/* Only one message */
		प्रत्यागम db__message_write_inflight_out_single(context, context->msgs_out.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_सन्देशः = context->msgs_out.inflight->prev;
	जाव(client_सन्देशः != context->msgs_out.inflight &&
			(client_सन्देशः->data.state == mosq_ms_publish_qos0
			|| client_सन्देशः->data.state == mosq_ms_publish_qos1
			|| client_सन्देशः->data.state == mosq_ms_publish_qos2)){

		client_सन्देशः = client_सन्देशः->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	जइ(client_सन्देशः != context->msgs_out.inflight){
		client_सन्देशः = client_सन्देशः->next;
	}

	जाव(client_सन्देशः){
		next = client_सन्देशः->next;
		rc = db__message_write_inflight_out_single(context, client_सन्देशः);
		जइ(rc){
			प्रत्यागम rc;
		}
		client_सन्देशः = next;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db__message_write_queued_in(संरचना mosquitto *context)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;
	पूर्णाङ्क rc;

	जइ(context->state != mosq_cs_active){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_सन्देशः, tmp){
		जइ(context->msgs_in.inflight_maximum != 0 && context->msgs_in.inflight_quota == 0){
			भङ्ग;
		}

		जइ(client_सन्देशः->data.qos == 2){
			client_सन्देशः->data.state = mosq_ms_send_pubrec;
			db__message_dequeue_first(context, &context->msgs_in);
			rc = send__pubrec(context, client_सन्देशः->data.mid, 0, रिक्त);
			जइ(!rc){
				client_सन्देशः->data.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_सन्देशः_update(context, client_सन्देशः);
			}अण्णहा{
				plugin_persist_handle_client_सन्देशः_update(context, client_सन्देशः);
				प्रत्यागम rc;
			}
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db__message_write_queued_out(संरचना mosquitto *context)
{
	जइ(context->state != mosq_cs_active){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	db__fill_inflight_out_from_queue(context);

	प्रत्यागम MOSQ_ERR_SUCCESS;
}
