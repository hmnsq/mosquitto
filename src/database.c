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
bool db__ready_for_flight(संरचना mosquitto *context, गणना mosquitto_संदेश_direction dir, पूर्णाङ्क qos)
{
	संरचना mosquitto_संदेश_data *msgs;
	bool valid_bytes;
	bool valid_गिनती;

	यदि(dir == mosq_md_out){
		msgs = &context->msgs_out;
	}अन्यथा{
		msgs = &context->msgs_in;
	}

	यदि(msgs->inflight_maximum == 0 && db.config->max_inflight_bytes == 0){
		प्रत्यागम true;
	}

	यदि(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		यदि(db.config->max_queued_messages == 0 && db.config->max_inflight_bytes == 0){
			प्रत्यागम true;
		}
		valid_bytes = ((msgs->inflight_bytes - (ssize_t)db.config->max_inflight_bytes) < (ssize_t)db.config->max_queued_bytes);
		यदि(dir == mosq_md_out){
			valid_गिनती = context->out_packet_गिनती < db.config->max_queued_messages;
		}अन्यथा{
			valid_गिनती = msgs->inflight_गिनती - msgs->inflight_maximum < db.config->max_queued_messages;
		}

		यदि(db.config->max_queued_messages == 0){
			प्रत्यागम valid_bytes;
		}
		यदि(db.config->max_queued_bytes == 0){
			प्रत्यागम valid_गिनती;
		}
	}अन्यथा{
		valid_bytes = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_bytes;
		valid_गिनती = msgs->inflight_quota > 0;

		यदि(msgs->inflight_maximum == 0){
			प्रत्यागम valid_bytes;
		}
		यदि(db.config->max_inflight_bytes == 0){
			प्रत्यागम valid_गिनती;
		}
	}

	प्रत्यागम valid_bytes && valid_गिनती;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db__ready_for_queue(संरचना mosquitto *context, पूर्णाङ्क qos, संरचना mosquitto_संदेश_data *संदेश_data)
{
	पूर्णाङ्क source_गिनती;
	पूर्णाङ्क adjust_गिनती;
	दीर्घ source_bytes;
	ssize_t adjust_bytes = (ssize_t)db.config->max_inflight_bytes;
	bool valid_bytes;
	bool valid_गिनती;

	यदि(db.config->max_queued_messages == 0 && db.config->max_queued_bytes == 0){
		प्रत्यागम true;
	}

	यदि(qos == 0 && db.config->queue_qos0_messages == false){
		प्रत्यागम false; /* This case is handled in db__ready_for_flight() */
	}अन्यथा{
		source_bytes = (ssize_t)संदेश_data->queued_bytes12;
		source_गिनती = संदेश_data->queued_count12;
	}
	adjust_गिनती = संदेश_data->inflight_maximum;

	/* nothing in flight for offline clients */
	यदि(!net__is_connected(context)){
		adjust_bytes = 0;
		adjust_गिनती = 0;
	}

	valid_bytes = (source_bytes - (ssize_t)adjust_bytes) < (ssize_t)db.config->max_queued_bytes;
	valid_गिनती = source_गिनती - adjust_गिनती < db.config->max_queued_messages;

	यदि(db.config->max_queued_bytes == 0){
		प्रत्यागम valid_गिनती;
	}
	यदि(db.config->max_queued_messages == 0){
		प्रत्यागम valid_bytes;
	}

	प्रत्यागम valid_bytes && valid_गिनती;
}


शून्य db_संदेश_add_to_inflight_stats(संरचना mosquitto_संदेश_data *संदेश_data, संरचना mosquitto_client_संदेश *client_संदेश)
{
	संदेश_data->inflight_गिनती++;
	संदेश_data->inflight_bytes += client_संदेश->base_संदेश->data.payloadlen;
	यदि(client_संदेश->data.qos != 0){
		संदेश_data->inflight_count12++;
		संदेश_data->inflight_bytes12 += client_संदेश->base_संदेश->data.payloadlen;
	}
}


स्थिर शून्य db_संदेश_remove_from_inflight_stats(संरचना mosquitto_संदेश_data *संदेश_data, संरचना mosquitto_client_संदेश *client_संदेश)
{
	संदेश_data->inflight_गिनती--;
	संदेश_data->inflight_bytes -= client_संदेश->base_संदेश->data.payloadlen;
	यदि(client_संदेश->data.qos != 0){
		संदेश_data->inflight_count12--;
		संदेश_data->inflight_bytes12 -= client_संदेश->base_संदेश->data.payloadlen;
	}
}


शून्य db_संदेश_add_to_queued_stats(संरचना mosquitto_संदेश_data *संदेश_data, संरचना mosquitto_client_संदेश *client_संदेश)
{
	संदेश_data->queued_गिनती++;
	संदेश_data->queued_bytes += client_संदेश->base_संदेश->data.payloadlen;
	यदि(client_संदेश->data.qos != 0){
		संदेश_data->queued_count12++;
		संदेश_data->queued_bytes12 += client_संदेश->base_संदेश->data.payloadlen;
	}
}


स्थिर शून्य db_संदेश_remove_from_queued_stats(संरचना mosquitto_संदेश_data *संदेश_data, संरचना mosquitto_client_संदेश *client_संदेश)
{
	संदेश_data->queued_गिनती--;
	संदेश_data->queued_bytes -= client_संदेश->base_संदेश->data.payloadlen;
	यदि(client_संदेश->data.qos != 0){
		संदेश_data->queued_count12--;
		संदेश_data->queued_bytes12 -= client_संदेश->base_संदेश->data.payloadlen;
	}
}


पूर्णाङ्क db__open(संरचना mosquitto__config *config)
{
	यदि(!config){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = रिक्त;
	db.contexts_by_sock = रिक्त;
	db.contexts_for_free = रिक्त;
#ifdef WITH_BRIDGE
	db.bridges = रिक्त;
	db.bridge_गिनती = 0;
#endif

	/* Initialize the hashtable */
	db.clientid_index_hash = रिक्त;

	db.normal_subs = रिक्त;
	db.shared_subs = रिक्त;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = रिक्त;

#ifdef WITH_PERSISTENCE
	यदि(persist__restore()){
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
		यावत्(leaf){
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
	db_संदेश_store_clean();

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db_संदेश_store_add(संरचना mosquitto_base_संदेश *base_संदेश)
{
	संरचना mosquitto_base_संदेश *found;
	अचिन्हित hashv;

	HASH_मूल्य(&base_संदेश->data.store_id, परिमाण(base_संदेश->data.store_id), hashv);
	HASH_FIND_BYHASHVALUE(hh, db.संदेश_store, &base_संदेश->data.store_id, परिमाण(base_संदेश->data.store_id), hashv, found);
	यदि(found == रिक्त){
		HASH_ADD_KEYPTR_BYHASHVALUE(hh, db.संदेश_store, &base_संदेश->data.store_id, परिमाण(base_संदेश->data.store_id), hashv, base_संदेश);
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_ALREADY_EXISTS;
	}
}


शून्य db_संदेश_store_free(संरचना mosquitto_base_संदेश *base_संदेश)
{
	mosquitto_FREE(base_संदेश->data.source_id);
	mosquitto_FREE(base_संदेश->data.source_username);
	यदि(base_संदेश->dest_ids){
		प्रति(पूर्णाङ्क i=0; i<base_संदेश->dest_id_गिनती; i++){
			mosquitto_FREE(base_संदेश->dest_ids[i]);
		}
		mosquitto_FREE(base_संदेश->dest_ids);
	}
	mosquitto_FREE(base_संदेश->data.topic);
	mosquitto_property_free_all(&base_संदेश->data.properties);
	mosquitto_FREE(base_संदेश->data.payload);
	mosquitto_FREE(base_संदेश);
}


शून्य db_संदेश_store_remove(संरचना mosquitto_base_संदेश *base_संदेश, bool notify)
{
	यदि(base_संदेश == रिक्त){
		प्रत्यागम;
	}
	HASH_DELETE(hh, db.संदेश_store, base_संदेश);
	db.संदेश_store_गिनती--;
	db.संदेश_store_bytes -= base_संदेश->data.payloadlen;
	यदि(notify == true){
		plugin_persist_handle_base_संदेश_delete(base_संदेश);
	}
	db_संदेश_store_free(base_संदेश);
}


शून्य db_संदेश_store_clean(शून्य)
{
	संरचना mosquitto_base_संदेश *base_संदेश, *base_संदेश_tmp;

	HASH_ITER(hh, db.संदेश_store, base_संदेश, base_संदेश_tmp){
		db_संदेश_store_remove(base_संदेश, false);
	}
}


शून्य db_संदेश_store_ref_inc(संरचना mosquitto_base_संदेश *base_संदेश)
{
	base_संदेश->ref_गिनती++;
}


शून्य db_संदेश_store_ref_dec(संरचना mosquitto_base_संदेश **base_संदेश)
{
	(*base_संदेश)->ref_गिनती--;
	यदि((*base_संदेश)->ref_गिनती == 0){
		db_संदेश_store_remove(*base_संदेश, true);
		*base_संदेश = रिक्त;
	}
}


शून्य db_संदेश_store_compact(शून्य)
{
	संरचना mosquitto_base_संदेश *base_संदेश, *base_संदेश_tmp;

	HASH_ITER(hh, db.संदेश_store, base_संदेश, base_संदेश_tmp){
		यदि(base_संदेश->ref_गिनती < 1){
			db_संदेश_store_remove(base_संदेश, true);
		}
	}
}


स्थिर शून्य db__message_remove_inflight(संरचना mosquitto *context, संरचना mosquitto_संदेश_data *संदेश_data, संरचना mosquitto_client_संदेश *item)
{
	यदि(!context || !संदेश_data || !item){
		प्रत्यागम;
	}

	plugin_persist_handle_client_संदेश_delete(context, item);

	DL_DELETE(संदेश_data->inflight, item);
	यदि(item->base_संदेश){
		db_संदेश_remove_from_inflight_stats(संदेश_data, item);
		db_संदेश_store_ref_dec(&item->base_संदेश);
	}

	mosquitto_FREE(item);
}


स्थिर शून्य db__message_remove_queued(संरचना mosquitto *context, संरचना mosquitto_संदेश_data *संदेश_data, संरचना mosquitto_client_संदेश *item)
{
	यदि(!context || !संदेश_data || !item){
		प्रत्यागम;
	}

	plugin_persist_handle_client_संदेश_delete(context, item);

	DL_DELETE(संदेश_data->queued, item);
	यदि(item->base_संदेश){
		db_संदेश_remove_from_queued_stats(संदेश_data, item);
		db_संदेश_store_ref_dec(&item->base_संदेश);
	}

	mosquitto_FREE(item);
}


स्थिर शून्य db__fill_inflight_out_from_queue(संरचना mosquitto *context)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.queued, client_संदेश, tmp){
		यदि(!db__ready_for_flight(context, mosq_md_out, client_संदेश->data.qos)){
			प्रत्यागम;
		}
		विकल्प(client_संदेश->data.qos){
			पक्ष 0:
				client_संदेश->data.state = mosq_ms_publish_qos0;
				भङ्ग;
			पक्ष 1:
				client_संदेश->data.state = mosq_ms_publish_qos1;
				भङ्ग;
			पक्ष 2:
				client_संदेश->data.state = mosq_ms_publish_qos2;
				भङ्ग;
		}
		यदि(client_संदेश->base_संदेश->data.expiry_time && db.now_real_s > client_संदेश->base_संदेश->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_संदेश);
			अनुवर्त;
		}
		plugin_persist_handle_client_संदेश_update(context, client_संदेश);
		db__message_dequeue_first(context, &context->msgs_out);
	}
}


शून्य db__message_dequeue_first(संरचना mosquitto *context, संरचना mosquitto_संदेश_data *संदेश_data)
{
	संरचना mosquitto_client_संदेश *client_संदेश;

	UNUSED(context);

	client_संदेश = संदेश_data->queued;
	DL_DELETE(संदेश_data->queued, client_संदेश);
	DL_APPEND(संदेश_data->inflight, client_संदेश);
	यदि(संदेश_data->inflight_quota > 0){
		संदेश_data->inflight_quota--;
	}

	db_संदेश_remove_from_queued_stats(संदेश_data, client_संदेश);
	db_संदेश_add_to_inflight_stats(संदेश_data, client_संदेश);
}


पूर्णाङ्क db__message_delete_outgoing(संरचना mosquitto *context, uint16_t mid, गणना mosquitto_संदेश_state expect_state, पूर्णाङ्क qos)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;
	bool deleted = false;

	यदि(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_संदेश, tmp){
		यदि(client_संदेश->data.mid == mid){
			यदि(client_संदेश->data.qos != qos){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_संदेश->data.qos, qos);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}अन्यथा यदि(qos == 2 && client_संदेश->data.state != expect_state && expect_state != mosq_ms_any){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_संदेश->data.state, expect_state);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_out, client_संदेश);
			deleted = true;
			भङ्ग;
		}
	}

	यदि(deleted == false){
		DL_FOREACH_SAFE(context->msgs_out.queued, client_संदेश, tmp){
			यदि(client_संदेश->data.mid == mid){
				यदि(client_संदेश->data.qos != qos){
					प्रत्यागम MOSQ_ERR_PROTOCOL;
				}अन्यथा यदि(qos == 2 && client_संदेश->data.state != expect_state && expect_state != mosq_ms_any){
					प्रत्यागम MOSQ_ERR_PROTOCOL;
				}
				db__message_remove_queued(context, &context->msgs_out, client_संदेश);
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
पूर्णाङ्क db_message_जोड़ो_incoming(संरचना mosquitto *context, uint64_t cmsg_id, संरचना mosquitto_base_संदेश *base_संदेश, bool persist)
{
	संरचना mosquitto_client_संदेश *client_संदेश;
	संरचना mosquitto_संदेश_data *संदेश_data;
	गणना mosquitto_संदेश_state state = mosq_ms_invalid;
	पूर्णाङ्क rc = 0;

	प्रतिज्ञा(base_संदेश);
	यदि(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		प्रत्यागम MOSQ_ERR_SUCCESS;

	}
	संदेश_data = &context->msgs_in;

	यदि(db__ready_for_flight(context, mosq_md_in, base_संदेश->data.qos)){
		state = mosq_ms_wait_for_pubrel;
	}अन्यथा यदि(base_संदेश->data.qos != 0 && db__ready_for_queue(context, base_संदेश->data.qos, संदेश_data)){
		state = mosq_ms_queued;
		rc = 2;
	}अन्यथा{
		/* Dropping message due to full queue. */
		यदि(context->is_dropping == false){
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
	यदि(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_संदेश = mosquitto_malloc(परिमाण(संरचना mosquitto_client_संदेश));
	यदि(!client_संदेश){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	client_संदेश->prev = रिक्त;
	client_संदेश->next = रिक्त;
	यदि(cmsg_id){
		client_संदेश->data.cmsg_id = cmsg_id;
	}अन्यथा{
		client_संदेश->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_संदेश->base_संदेश = base_संदेश;
	db_संदेश_store_ref_inc(client_संदेश->base_संदेश);
	client_संदेश->data.mid = base_संदेश->data.source_mid;
	client_संदेश->data.direction = mosq_md_in;
	client_संदेश->data.state = (uint8_t)state;
	client_संदेश->data.dup = false;
	यदि(base_संदेश->data.qos > context->max_qos){
		client_संदेश->data.qos = context->max_qos;
	}अन्यथा{
		client_संदेश->data.qos = base_संदेश->data.qos;
	}
	client_संदेश->data.retain = base_संदेश->data.retain;
	client_संदेश->data.subscription_identifier = 0;

	यदि(state == mosq_ms_queued){
		DL_APPEND(संदेश_data->queued, client_संदेश);
		db_संदेश_add_to_queued_stats(संदेश_data, client_संदेश);
	}अन्यथा{
		DL_APPEND(संदेश_data->inflight, client_संदेश);
		db_संदेश_add_to_inflight_stats(संदेश_data, client_संदेश);
	}

	यदि(persist && context->is_persisted){
		plugin_persist_handle_base_संदेश_add(client_संदेश->base_संदेश);
		plugin_persist_handle_client_संदेश_add(context, client_संदेश);
	}

	यदि(client_संदेश->base_संदेश->data.qos > 0){
		util__decrement_receive_quota(context);
	}
	प्रत्यागम rc;
}


पूर्णाङ्क db_message_जोड़ो_outgoing(संरचना mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, संरचना mosquitto_base_संदेश *base_संदेश, uint32_t subscription_identifier, bool update, bool persist)
{
	संरचना mosquitto_client_संदेश *client_संदेश;
	संरचना mosquitto_संदेश_data *संदेश_data;
	गणना mosquitto_संदेश_state state = mosq_ms_invalid;
	पूर्णाङ्क rc = 0;
	वर्ण **dest_ids;

	प्रतिज्ञा(base_संदेश);
	यदि(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		प्रत्यागम MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_sent++;

	संदेश_data = &context->msgs_out;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	यदि(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && base_संदेश->dest_ids){

		प्रति(पूर्णाङ्क i=0; i<base_संदेश->dest_id_गिनती; i++){
			यदि(base_संदेश->dest_ids[i] && !म_भेद(base_संदेश->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				प्रत्यागम MOSQ_ERR_SUCCESS;
			}
		}
	}
	यदि(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		यदि(qos == 0 && !db.config->queue_qos0_messages){
			यदि(!context->bridge){
				प्रत्यागम 2;
			}अन्यथा{
				यदि(context->bridge->start_type != bst_lazy){
					प्रत्यागम 2;
				}
			}
		}
		यदि(context->bridge && context->bridge->clean_start_local == true){
			प्रत्यागम 2;
		}
	}

	यदि(net__is_connected(context)){
		यदि(db__ready_for_flight(context, mosq_md_out, qos)){
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
		}अन्यथा यदि(qos != 0 && db__ready_for_queue(context, qos, संदेश_data)){
			state = mosq_ms_queued;
			rc = 2;
		}अन्यथा{
			/* Dropping message due to full queue. */
			यदि(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(रिक्त, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			प्रत्यागम 2;
		}
	}अन्यथा{
		यदि(db__ready_for_queue(context, qos, संदेश_data)){
			state = mosq_ms_queued;
		}अन्यथा{
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			यदि(context->is_dropping == false){
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
	यदि(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_संदेश = mosquitto_malloc(परिमाण(संरचना mosquitto_client_संदेश));
	यदि(!client_संदेश){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	client_संदेश->prev = रिक्त;
	client_संदेश->next = रिक्त;
	यदि(cmsg_id){
		client_संदेश->data.cmsg_id = cmsg_id;
	}अन्यथा{
		client_संदेश->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_संदेश->base_संदेश = base_संदेश;
	db_संदेश_store_ref_inc(client_संदेश->base_संदेश);
	client_संदेश->data.mid = mid;
	client_संदेश->data.direction = mosq_md_out;
	client_संदेश->data.state = (uint8_t)state;
	client_संदेश->data.dup = false;
	यदि(qos > context->max_qos){
		client_संदेश->data.qos = context->max_qos;
	}अन्यथा{
		client_संदेश->data.qos = qos;
	}
	client_संदेश->data.retain = retain;
	client_संदेश->data.subscription_identifier = subscription_identifier;

	यदि(state == mosq_ms_queued){
		DL_APPEND(संदेश_data->queued, client_संदेश);
		db_संदेश_add_to_queued_stats(संदेश_data, client_संदेश);
	}अन्यथा{
		DL_APPEND(संदेश_data->inflight, client_संदेश);
		db_संदेश_add_to_inflight_stats(संदेश_data, client_संदेश);
	}

	यदि(persist && context->is_persisted){
		plugin_persist_handle_base_संदेश_add(client_संदेश->base_संदेश);
		plugin_persist_handle_client_संदेश_add(context, client_संदेश);
	}

	यदि(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(base_संदेश->dest_ids, परिमाण(वर्ण *)*(माप_प्रकार)(base_संदेश->dest_id_गिनती+1));
		यदि(dest_ids){
			base_संदेश->dest_ids = dest_ids;
			base_संदेश->dest_id_गिनती++;
			base_संदेश->dest_ids[base_संदेश->dest_id_गिनती-1] = mosquitto_strdup(context->id);
			यदि(!base_संदेश->dest_ids[base_संदेश->dest_id_गिनती-1]){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
		}अन्यथा{
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}
#ifdef WITH_BRIDGE
	यदि(context->bridge && context->bridge->start_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_out.inflight_गिनती + context->msgs_out.queued_गिनती >= context->bridge->threshold){

		context->bridge->lazy_reconnect = true;
	}
#endif

	यदि(client_संदेश->data.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	यदि(update){
		rc = db__message_write_inflight_out_latest(context);
		यदि(rc){
			प्रत्यागम rc;
		}
		rc = db__message_write_queued_out(context);
		यदि(rc){
			प्रत्यागम rc;
		}
	}

	प्रत्यागम rc;
}


स्थिर अन्तःस्थ पूर्णाङ्क db__message_update_outgoing_state(संरचना mosquitto *context, संरचना mosquitto_client_संदेश *head,
		uint16_t mid, गणना mosquitto_संदेश_state state, पूर्णाङ्क qos, bool persist)
{
	संरचना mosquitto_client_संदेश *client_संदेश;

	DL_FOREACH(head, client_संदेश){
		यदि(client_संदेश->data.mid == mid){
			यदि(client_संदेश->data.qos != qos){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_संदेश->data.qos, qos);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}
			client_संदेश->data.state = (uint8_t)state;
			यदि(persist){
				plugin_persist_handle_client_संदेश_update(context, client_संदेश);
			}
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}
	}
	प्रत्यागम MOSQ_ERR_NOT_FOUND;
}


पूर्णाङ्क db__message_update_outgoing(संरचना mosquitto *context, uint16_t mid, गणना mosquitto_संदेश_state state, पूर्णाङ्क qos, bool persist)
{
	पूर्णाङ्क rc;

	rc = db__message_update_outgoing_state(context, context->msgs_out.inflight, mid, state, qos, persist);
	यदि(!persist && rc == MOSQ_ERR_NOT_FOUND){
		rc = db__message_update_outgoing_state(context, context->msgs_out.queued, mid, state, qos, persist);
	}
	प्रत्यागम rc;
}


स्थिर शून्य db__messages_delete_list(संरचना mosquitto_client_संदेश **head)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;

	DL_FOREACH_SAFE(*head, client_संदेश, tmp){
		DL_DELETE(*head, client_संदेश);
		db_संदेश_store_ref_dec(&client_संदेश->base_संदेश);
		mosquitto_FREE(client_संदेश);
	}
	*head = रिक्त;
}


पूर्णाङ्क db__messages_delete_incoming(संरचना mosquitto *context)
{
	यदि(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_in.inflight);
	db__messages_delete_list(&context->msgs_in.queued);
	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_गिनती = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_गिनती = 0;
	context->msgs_in.queued_count12 = 0;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db__messages_delete_outgoing(संरचना mosquitto *context)
{
	यदि(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_out.inflight);
	db__messages_delete_list(&context->msgs_out.queued);
	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_गिनती = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_गिनती = 0;
	context->msgs_out.queued_count12 = 0;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db__messages_delete(संरचना mosquitto *context, bool force_free)
{
	यदि(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	यदि(force_free || context->clean_start || (context->bridge && context->bridge->clean_start)){
		db__messages_delete_incoming(context);
	}

	यदि(force_free || (context->bridge && context->bridge->clean_start_local)
			|| (context->bridge == रिक्त && context->clean_start)){

		db__messages_delete_outgoing(context);
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db__messages_easy_queue(संरचना mosquitto *context, ध्रुव वर्ण *topic, uint8_t qos, uint32_t payloadlen, ध्रुव शून्य *payload, पूर्णाङ्क retain, uint32_t message_expiry_interval, mosquitto_property **properties)
{
	संरचना mosquitto_base_संदेश *base_संदेश;
	ध्रुव वर्ण *source_id;
	गणना mosquitto_संदेश_origin origin;

	यदि(!topic){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	base_संदेश = mosquitto_calloc(1, परिमाण(संरचना mosquitto_base_संदेश));
	यदि(base_संदेश == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	base_संदेश->data.topic = mosquitto_strdup(topic);
	यदि(base_संदेश->data.topic == रिक्त){
		db_संदेश_store_free(base_संदेश);
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	base_संदेश->data.qos = qos;
	यदि(db.config->retain_available == false){
		base_संदेश->data.retain = 0;
	}अन्यथा{
		base_संदेश->data.retain = retain;
	}

	base_संदेश->data.payloadlen = payloadlen;
	यदि(payloadlen > 0){
		base_संदेश->data.payload = mosquitto_malloc(base_संदेश->data.payloadlen+1);
		यदि(base_संदेश->data.payload == रिक्त){
			db_संदेश_store_free(base_संदेश);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_संदेश->data.payload)[base_संदेश->data.payloadlen] = 0;
		स_नकल(base_संदेश->data.payload, payload, base_संदेश->data.payloadlen);
	}

	यदि(context && context->id){
		source_id = context->id;
	}अन्यथा{
		source_id = "";
	}
	यदि(properties){
		base_संदेश->data.properties = *properties;
		*properties = रिक्त;
	}

	यदि(context){
		origin = mosq_mo_client;
	}अन्यथा{
		origin = mosq_mo_broker;
	}
	यदि(db__message_store(context, base_संदेश, &message_expiry_interval, origin)){
		प्रत्यागम 1;
	}

	प्रत्यागम sub__messages_queue(source_id, base_संदेश->data.qos, &base_संदेश);
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
uint64_t db_new_संदेश_id(शून्य)
{
#ifdef WIN32
	FILETIME ftime;
	uint64_t ftime64;
#अन्यथा
	संरचना timespec ts;
#endif
	uint64_t id;
	uint64_t tmp;
	समय_प्रकार sec;
	दीर्घ nsec;

	id = db.गाँठ_id_shifted; /* Top 10-bits */

#ifdef WIN32
	GetSystemTimePreciseAsFileTime(&ftime);
	ftime64 = (((uint64_t)ftime.dw_ऊपरी_Date_Time)<<32) + ftime.dw_निचला_Date_Time;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (दीर्घ)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#अन्यथा
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#endif
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	यदि(id <= db.last_db_id){
		id = db.last_db_id + 1;
	}
	db.last_db_id = id;

	प्रत्यागम id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
पूर्णाङ्क db__message_store(ध्रुव संरचना mosquitto *source, संरचना mosquitto_base_संदेश *base_संदेश, uint32_t *message_expiry_interval, गणना mosquitto_संदेश_origin origin)
{
	पूर्णाङ्क rc;

	प्रतिज्ञा(base_संदेश);

	यदि(source && source->id){
		base_संदेश->data.source_id = mosquitto_strdup(source->id);
	}अन्यथा{
		base_संदेश->data.source_id = mosquitto_strdup("");
	}
	यदि(!base_संदेश->data.source_id){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_संदेश_store_free(base_संदेश);
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	यदि(source && source->username){
		base_संदेश->data.source_username = mosquitto_strdup(source->username);
		यदि(!base_संदेश->data.source_username){
			db_संदेश_store_free(base_संदेश);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}
	यदि(source){
		base_संदेश->source_listener = source->listener;
	}
	base_संदेश->origin = origin;
	यदि(message_expiry_interval){
		यदि(*message_expiry_interval > 0 && *message_expiry_interval != संदेश_EXPIRY_INFINITE){
			base_संदेश->data.expiry_time = db.now_real_s + *message_expiry_interval;
		}अन्यथा{
			base_संदेश->data.expiry_time = 0;
		}
	}

	base_संदेश->dest_ids = रिक्त;
	base_संदेश->dest_id_गिनती = 0;
	db.संदेश_store_गिनती++;
	db.संदेश_store_bytes += base_संदेश->data.payloadlen;

	यदि(!base_संदेश->data.store_id){
		base_संदेश->data.store_id = db_new_संदेश_id();
	}

	rc = db_संदेश_store_add(base_संदेश);
	यदि(rc){
		db_संदेश_store_free(base_संदेश);
		प्रत्यागम rc;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db__message_store_find(संरचना mosquitto *context, uint16_t mid, संरचना mosquitto_client_संदेश **client_संदेश)
{
	संरचना mosquitto_client_संदेश *cmsg;

	*client_संदेश = रिक्त;

	यदि(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_in.inflight, cmsg){
		यदि(cmsg->base_संदेश->data.source_mid == mid){
			*client_संदेश = cmsg;
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_in.queued, cmsg){
		यदि(cmsg->base_संदेश->data.source_mid == mid){
			*client_संदेश = cmsg;
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}
	}

	प्रत्यागम 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
स्थिर पूर्णाङ्क db__message_reconnect_reset_outgoing(संरचना mosquitto *context)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;

	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_गिनती = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_गिनती = 0;
	context->msgs_out.queued_count12 = 0;
	context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_संदेश, tmp){
		db_संदेश_add_to_inflight_stats(&context->msgs_out, client_संदेश);
		यदि(client_संदेश->data.qos > 0){
			util__decrement_send_quota(context);
		}

		विकल्प(client_संदेश->data.qos){
			पक्ष 0:
				client_संदेश->data.state = mosq_ms_publish_qos0;
				भङ्ग;
			पक्ष 1:
				client_संदेश->data.state = mosq_ms_publish_qos1;
				भङ्ग;
			पक्ष 2:
				यदि(client_संदेश->data.state == mosq_ms_wait_for_pubcomp){
					client_संदेश->data.state = mosq_ms_resend_pubrel;
				}अन्यथा{
					client_संदेश->data.state = mosq_ms_publish_qos2;
				}
				भङ्ग;
		}
		plugin_persist_handle_client_संदेश_update(context, client_संदेश);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_out.queued, client_संदेश, tmp){
		db_संदेश_add_to_queued_stats(&context->msgs_out, client_संदेश);
	}
	db__fill_inflight_out_from_queue(context);

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
स्थिर पूर्णाङ्क db__message_reconnect_reset_incoming(संरचना mosquitto *context)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;

	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_गिनती = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_गिनती = 0;
	context->msgs_in.queued_count12 = 0;
	context->msgs_in.inflight_quota = context->msgs_in.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_संदेश, tmp){
		db_संदेश_add_to_inflight_stats(&context->msgs_in, client_संदेश);
		यदि(client_संदेश->data.qos > 0){
			util__decrement_receive_quota(context);
		}

		यदि(client_संदेश->data.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db__message_remove_inflight(context, &context->msgs_in, client_संदेश);
		}अन्यथा{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_संदेश->data.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_in.queued, client_संदेश, tmp){
		client_संदेश->data.dup = 0;
		db_संदेश_add_to_queued_stats(&context->msgs_in, client_संदेश);
		यदि(db__ready_for_flight(context, mosq_md_in, client_संदेश->data.qos)){
			विकल्प(client_संदेश->data.qos){
				पक्ष 0:
					client_संदेश->data.state = mosq_ms_publish_qos0;
					भङ्ग;
				पक्ष 1:
					client_संदेश->data.state = mosq_ms_publish_qos1;
					भङ्ग;
				पक्ष 2:
					client_संदेश->data.state = mosq_ms_publish_qos2;
					भङ्ग;
			}
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_संदेश_update(context, client_संदेश);
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db__message_reconnect_reset(संरचना mosquitto *context)
{
	पूर्णाङ्क rc;

	rc = db__message_reconnect_reset_outgoing(context);
	यदि(rc){
		प्रत्यागम rc;
	}
	प्रत्यागम db__message_reconnect_reset_incoming(context);
}


पूर्णाङ्क db__message_remove_incoming(संरचना mosquitto *context, uint16_t mid)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;

	यदि(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_संदेश, tmp){
		यदि(client_संदेश->data.mid == mid){
			यदि(client_संदेश->base_संदेश->data.qos != 2){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_संदेश->base_संदेश->data.qos);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_in, client_संदेश);
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}
	}

	प्रत्यागम MOSQ_ERR_NOT_FOUND;
}


पूर्णाङ्क db__message_release_incoming(संरचना mosquitto *context, uint16_t mid)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;
	bool deleted = false;
	पूर्णाङ्क rc;

	यदि(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_संदेश, tmp){
		यदि(client_संदेश->data.mid == mid){
			यदि(client_संदेश->base_संदेश->data.qos != 2){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_संदेश->base_संदेश->data.qos);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}
			ध्रुव वर्ण *topic = client_संदेश->base_संदेश->data.topic;
			ध्रुव वर्ण *source_id = client_संदेश->base_संदेश->data.source_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			यदि(topic == रिक्त){
				db__message_remove_inflight(context, &context->msgs_in, client_संदेश);
				deleted = true;
			}अन्यथा{
				rc = sub__messages_queue(source_id, 2, &client_संदेश->base_संदेश);
				यदि(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db__message_remove_inflight(context, &context->msgs_in, client_संदेश);
					deleted = true;
				}अन्यथा{
					प्रत्यागम 1;
				}
			}
		}
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_संदेश, tmp){
		यदि(db__ready_for_flight(context, mosq_md_in, client_संदेश->data.qos)){
			भङ्ग;
		}

		यदि(client_संदेश->data.qos == 2){
			send__pubrec(context, client_संदेश->data.mid, 0, रिक्त);
			client_संदेश->data.state = mosq_ms_wait_for_pubrel;
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_संदेश_update(context, client_संदेश);
		}
	}
	यदि(deleted){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_NOT_FOUND;
	}
}


शून्य db__expire_all_messages(संरचना mosquitto *context)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_संदेश, tmp){
		यदि(client_संदेश->base_संदेश->data.expiry_time && db.now_real_s > client_संदेश->base_संदेश->data.expiry_time){
			यदि(client_संदेश->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_संदेश);
		}
	}
	db__fill_inflight_out_from_queue(context);
	DL_FOREACH_SAFE(context->msgs_out.queued, client_संदेश, tmp){
		यदि(client_संदेश->base_संदेश->data.expiry_time && db.now_real_s > client_संदेश->base_संदेश->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_संदेश);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.inflight, client_संदेश, tmp){
		यदि(client_संदेश->base_संदेश->data.expiry_time && db.now_real_s > client_संदेश->base_संदेश->data.expiry_time){
			यदि(client_संदेश->data.qos > 0){
				util__increment_receive_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_in, client_संदेश);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.queued, client_संदेश, tmp){
		यदि(client_संदेश->base_संदेश->data.expiry_time && db.now_real_s > client_संदेश->base_संदेश->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_in, client_संदेश);
		}
	}
}


स्थिर शून्य db__client_messages_check_acl(संरचना mosquitto *context, संरचना mosquitto_संदेश_data *संदेश_data, संरचना mosquitto_client_संदेश **head,
		शून्य (*decrement_stats_fn)(संरचना mosquitto_संदेश_data *संदेश_data, संरचना mosquitto_client_संदेश *client_संदेश))
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;
	संरचना mosquitto_base_संदेश *base_संदेश;
	पूर्णाङ्क access;

	DL_FOREACH_SAFE((*head), client_संदेश, tmp){
		base_संदेश = client_संदेश->base_संदेश;
		यदि(client_संदेश->data.direction == mosq_md_out){
			access = MOSQ_ACL_READ;
		}अन्यथा{
			access = MOSQ_ACL_WRITE;
		}
		यदि(mosquitto_acl_check(context, base_संदेश->data.topic,
				base_संदेश->data.payloadlen, base_संदेश->data.payload,
				base_संदेश->data.qos, base_संदेश->data.retain,
				base_संदेश->data.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*head), client_संदेश);
			decrement_stats_fn(संदेश_data, client_संदेश);
			plugin_persist_handle_client_संदेश_delete(context, client_संदेश);
			db_संदेश_store_ref_dec(&client_संदेश->base_संदेश);
			mosquitto_FREE(client_संदेश);
		}
	}
}


शून्य db__check_acl_of_all_messages(संरचना mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.inflight, &db_संदेश_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.queued, &db_संदेश_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.inflight, &db_संदेश_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.queued, &db_संदेश_remove_from_queued_stats);
}


स्थिर पूर्णाङ्क db__message_write_inflight_out_single(संरचना mosquitto *context, संरचना mosquitto_client_संदेश *client_संदेश)
{
	संरचना mosquitto_base_संदेश *base_संदेश;
	mosquitto_property *base_संदेश_props = रिक्त;
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

	base_संदेश = client_संदेश->base_संदेश;

	expiry_interval = 0;
	यदि(base_संदेश->data.expiry_time){
		यदि(db.now_real_s > base_संदेश->data.expiry_time){
			/* Message is expired, must not send. */
			यदि(client_संदेश->data.direction == mosq_md_out && client_संदेश->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_संदेश);
			db__fill_inflight_out_from_queue(context);
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}अन्यथा{
			expiry_interval = (uint32_t)(base_संदेश->data.expiry_time - db.now_real_s);
		}
	}
	mid = client_संदेश->data.mid;
	retries = client_संदेश->data.dup;
	retain = client_संदेश->data.retain;
	topic = base_संदेश->data.topic;
	qos = (uint8_t)client_संदेश->data.qos;
	payloadlen = base_संदेश->data.payloadlen;
	payload = base_संदेश->data.payload;
	subscription_id = client_संदेश->data.subscription_identifier;
	base_संदेश_props = base_संदेश->data.properties;

	विकल्प(client_संदेश->data.state){
		पक्ष mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_संदेश_props, expiry_interval);
			यदि(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_संदेश);
			}अन्यथा{
				प्रत्यागम rc;
			}
			भङ्ग;

		पक्ष mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_संदेश_props, expiry_interval);
			यदि(rc == MOSQ_ERR_SUCCESS){
				client_संदेश->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_संदेश->data.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_संदेश_update(context, client_संदेश);
			}अन्यथा यदि(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_संदेश);
			}अन्यथा{
				प्रत्यागम rc;
			}
			भङ्ग;

		पक्ष mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_संदेश_props, expiry_interval);
			यदि(rc == MOSQ_ERR_SUCCESS){
				client_संदेश->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_संदेश->data.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_संदेश_update(context, client_संदेश);
			}अन्यथा यदि(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_संदेश);
			}अन्यथा{
				प्रत्यागम rc;
			}
			भङ्ग;

		पक्ष mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, रिक्त);
			यदि(!rc){
				client_संदेश->data.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_संदेश_update(context, client_संदेश);
			}अन्यथा{
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
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;
	पूर्णाङ्क rc;

	यदि(context->state != mosq_cs_active || !net__is_connected(context)){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_संदेश, tmp){
		rc = db__message_write_inflight_out_single(context, client_संदेश);
		यदि(rc){
			प्रत्यागम rc;
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db__message_write_inflight_out_latest(संरचना mosquitto *context)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *next;
	पूर्णाङ्क rc;

	यदि(context->state != mosq_cs_active
			|| !net__is_connected(context)
			|| context->msgs_out.inflight == रिक्त){

		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	यदि(context->msgs_out.inflight->prev == context->msgs_out.inflight){
		/* Only one message */
		प्रत्यागम db__message_write_inflight_out_single(context, context->msgs_out.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_संदेश = context->msgs_out.inflight->prev;
	यावत्(client_संदेश != context->msgs_out.inflight &&
			(client_संदेश->data.state == mosq_ms_publish_qos0
			|| client_संदेश->data.state == mosq_ms_publish_qos1
			|| client_संदेश->data.state == mosq_ms_publish_qos2)){

		client_संदेश = client_संदेश->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	यदि(client_संदेश != context->msgs_out.inflight){
		client_संदेश = client_संदेश->next;
	}

	यावत्(client_संदेश){
		next = client_संदेश->next;
		rc = db__message_write_inflight_out_single(context, client_संदेश);
		यदि(rc){
			प्रत्यागम rc;
		}
		client_संदेश = next;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db__message_write_queued_in(संरचना mosquitto *context)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;
	पूर्णाङ्क rc;

	यदि(context->state != mosq_cs_active){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_संदेश, tmp){
		यदि(context->msgs_in.inflight_maximum != 0 && context->msgs_in.inflight_quota == 0){
			भङ्ग;
		}

		यदि(client_संदेश->data.qos == 2){
			client_संदेश->data.state = mosq_ms_send_pubrec;
			db__message_dequeue_first(context, &context->msgs_in);
			rc = send__pubrec(context, client_संदेश->data.mid, 0, रिक्त);
			यदि(!rc){
				client_संदेश->data.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_संदेश_update(context, client_संदेश);
			}अन्यथा{
				plugin_persist_handle_client_संदेश_update(context, client_संदेश);
				प्रत्यागम rc;
			}
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db__message_write_queued_out(संरचना mosquitto *context)
{
	यदि(context->state != mosq_cs_active){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	db__fill_inflight_out_from_queue(context);

	प्रत्यागम MOSQ_ERR_SUCCESS;
}
