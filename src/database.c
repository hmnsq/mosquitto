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
bool db__ready_for_flight(संरचना mosquitto *context, गणना mosquitto_संदेश_direction dir, पूर्णांक qos)
{
	संरचना mosquitto_संदेश_data *msgs;
	bool valid_bytes;
	bool valid_मोजणी;

	जर(dir == mosq_md_out){
		msgs = &context->msgs_out;
	}अन्यथा{
		msgs = &context->msgs_in;
	}

	जर(msgs->inflight_maximum == 0 && db.config->max_inflight_bytes == 0){
		परत true;
	}

	जर(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		जर(db.config->max_queued_messages == 0 && db.config->max_inflight_bytes == 0){
			परत true;
		}
		valid_bytes = ((msgs->inflight_bytes - (ssize_t)db.config->max_inflight_bytes) < (ssize_t)db.config->max_queued_bytes);
		जर(dir == mosq_md_out){
			valid_मोजणी = context->out_packet_मोजणी < db.config->max_queued_messages;
		}अन्यथा{
			valid_मोजणी = msgs->inflight_मोजणी - msgs->inflight_maximum < db.config->max_queued_messages;
		}

		जर(db.config->max_queued_messages == 0){
			परत valid_bytes;
		}
		जर(db.config->max_queued_bytes == 0){
			परत valid_मोजणी;
		}
	}अन्यथा{
		valid_bytes = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_bytes;
		valid_मोजणी = msgs->inflight_quota > 0;

		जर(msgs->inflight_maximum == 0){
			परत valid_bytes;
		}
		जर(db.config->max_inflight_bytes == 0){
			परत valid_मोजणी;
		}
	}

	परत valid_bytes && valid_मोजणी;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db__ready_for_queue(संरचना mosquitto *context, पूर्णांक qos, संरचना mosquitto_संदेश_data *संदेश_data)
{
	पूर्णांक source_मोजणी;
	पूर्णांक adjust_मोजणी;
	दीर्घ source_bytes;
	ssize_t adjust_bytes = (ssize_t)db.config->max_inflight_bytes;
	bool valid_bytes;
	bool valid_मोजणी;

	जर(db.config->max_queued_messages == 0 && db.config->max_queued_bytes == 0){
		परत true;
	}

	जर(qos == 0 && db.config->queue_qos0_messages == false){
		परत false; /* This case is handled in db__ready_for_flight() */
	}अन्यथा{
		source_bytes = (ssize_t)संदेश_data->queued_bytes12;
		source_मोजणी = संदेश_data->queued_count12;
	}
	adjust_मोजणी = संदेश_data->inflight_maximum;

	/* nothing in flight for offline clients */
	जर(!net__is_connected(context)){
		adjust_bytes = 0;
		adjust_मोजणी = 0;
	}

	valid_bytes = (source_bytes - (ssize_t)adjust_bytes) < (ssize_t)db.config->max_queued_bytes;
	valid_मोजणी = source_मोजणी - adjust_मोजणी < db.config->max_queued_messages;

	जर(db.config->max_queued_bytes == 0){
		परत valid_मोजणी;
	}
	जर(db.config->max_queued_messages == 0){
		परत valid_bytes;
	}

	परत valid_bytes && valid_मोजणी;
}


शून्य db_संदेश_add_to_inflight_stats(संरचना mosquitto_संदेश_data *संदेश_data, संरचना mosquitto_client_संदेश *client_संदेश)
{
	संदेश_data->inflight_मोजणी++;
	संदेश_data->inflight_bytes += client_संदेश->base_संदेश->data.payloadlen;
	जर(client_संदेश->data.qos != 0){
		संदेश_data->inflight_count12++;
		संदेश_data->inflight_bytes12 += client_संदेश->base_संदेश->data.payloadlen;
	}
}


स्थिर शून्य db_संदेश_remove_from_inflight_stats(संरचना mosquitto_संदेश_data *संदेश_data, संरचना mosquitto_client_संदेश *client_संदेश)
{
	संदेश_data->inflight_मोजणी--;
	संदेश_data->inflight_bytes -= client_संदेश->base_संदेश->data.payloadlen;
	जर(client_संदेश->data.qos != 0){
		संदेश_data->inflight_count12--;
		संदेश_data->inflight_bytes12 -= client_संदेश->base_संदेश->data.payloadlen;
	}
}


शून्य db_संदेश_add_to_queued_stats(संरचना mosquitto_संदेश_data *संदेश_data, संरचना mosquitto_client_संदेश *client_संदेश)
{
	संदेश_data->queued_मोजणी++;
	संदेश_data->queued_bytes += client_संदेश->base_संदेश->data.payloadlen;
	जर(client_संदेश->data.qos != 0){
		संदेश_data->queued_count12++;
		संदेश_data->queued_bytes12 += client_संदेश->base_संदेश->data.payloadlen;
	}
}


स्थिर शून्य db_संदेश_remove_from_queued_stats(संरचना mosquitto_संदेश_data *संदेश_data, संरचना mosquitto_client_संदेश *client_संदेश)
{
	संदेश_data->queued_मोजणी--;
	संदेश_data->queued_bytes -= client_संदेश->base_संदेश->data.payloadlen;
	जर(client_संदेश->data.qos != 0){
		संदेश_data->queued_count12--;
		संदेश_data->queued_bytes12 -= client_संदेश->base_संदेश->data.payloadlen;
	}
}


पूर्णांक db__open(संरचना mosquitto__config *config)
{
	जर(!config){
		परत MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = रिक्त;
	db.contexts_by_sock = रिक्त;
	db.contexts_for_free = रिक्त;
#ifdef WITH_BRIDGE
	db.bridges = रिक्त;
	db.bridge_मोजणी = 0;
#endif

	/* Initialize the hashtable */
	db.clientid_index_hash = रिक्त;

	db.normal_subs = रिक्त;
	db.shared_subs = रिक्त;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = रिक्त;

#ifdef WITH_PERSISTENCE
	जर(persist__restore()){
		परत 1;
	}
#endif

	परत MOSQ_ERR_SUCCESS;
}


स्थिर शून्य subhier_clean(संरचना mosquitto__subhier **subhier)
{
	संरचना mosquitto__subhier *peer, *subhier_tmp;
	संरचना mosquitto__subleaf *leaf, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		leaf = peer->subs;
		पर्यंत(leaf){
			nextleaf = leaf->next;
			mosquitto_FREE(leaf);
			leaf = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


पूर्णांक db__close(शून्य)
{
	subhier_clean(&db.normal_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db_संदेश_store_clean();

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक db_संदेश_store_add(संरचना mosquitto_base_संदेश *base_संदेश)
{
	संरचना mosquitto_base_संदेश *found;
	अचिन्हित hashv;

	HASH_मूल्य(&base_संदेश->data.store_id, आकार(base_संदेश->data.store_id), hashv);
	HASH_FIND_BYHASHVALUE(hh, db.संदेश_store, &base_संदेश->data.store_id, आकार(base_संदेश->data.store_id), hashv, found);
	जर(found == रिक्त){
		HASH_ADD_KEYPTR_BYHASHVALUE(hh, db.संदेश_store, &base_संदेश->data.store_id, आकार(base_संदेश->data.store_id), hashv, base_संदेश);
		परत MOSQ_ERR_SUCCESS;
	}अन्यथा{
		परत MOSQ_ERR_ALREADY_EXISTS;
	}
}


शून्य db_संदेश_store_free(संरचना mosquitto_base_संदेश *base_संदेश)
{
	mosquitto_FREE(base_संदेश->data.source_id);
	mosquitto_FREE(base_संदेश->data.source_username);
	जर(base_संदेश->dest_ids){
		साठी(पूर्णांक i=0; i<base_संदेश->dest_id_मोजणी; i++){
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
	जर(base_संदेश == रिक्त){
		परत;
	}
	HASH_DELETE(hh, db.संदेश_store, base_संदेश);
	db.संदेश_store_मोजणी--;
	db.संदेश_store_bytes -= base_संदेश->data.payloadlen;
	जर(notify == true){
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
	base_संदेश->ref_मोजणी++;
}


शून्य db_संदेश_store_ref_dec(संरचना mosquitto_base_संदेश **base_संदेश)
{
	(*base_संदेश)->ref_मोजणी--;
	जर((*base_संदेश)->ref_मोजणी == 0){
		db_संदेश_store_remove(*base_संदेश, true);
		*base_संदेश = रिक्त;
	}
}


शून्य db_संदेश_store_compact(शून्य)
{
	संरचना mosquitto_base_संदेश *base_संदेश, *base_संदेश_tmp;

	HASH_ITER(hh, db.संदेश_store, base_संदेश, base_संदेश_tmp){
		जर(base_संदेश->ref_मोजणी < 1){
			db_संदेश_store_remove(base_संदेश, true);
		}
	}
}


स्थिर शून्य db__message_remove_inflight(संरचना mosquitto *context, संरचना mosquitto_संदेश_data *संदेश_data, संरचना mosquitto_client_संदेश *item)
{
	जर(!context || !संदेश_data || !item){
		परत;
	}

	plugin_persist_handle_client_संदेश_delete(context, item);

	DL_DELETE(संदेश_data->inflight, item);
	जर(item->base_संदेश){
		db_संदेश_remove_from_inflight_stats(संदेश_data, item);
		db_संदेश_store_ref_dec(&item->base_संदेश);
	}

	mosquitto_FREE(item);
}


स्थिर शून्य db__message_remove_queued(संरचना mosquitto *context, संरचना mosquitto_संदेश_data *संदेश_data, संरचना mosquitto_client_संदेश *item)
{
	जर(!context || !संदेश_data || !item){
		परत;
	}

	plugin_persist_handle_client_संदेश_delete(context, item);

	DL_DELETE(संदेश_data->queued, item);
	जर(item->base_संदेश){
		db_संदेश_remove_from_queued_stats(संदेश_data, item);
		db_संदेश_store_ref_dec(&item->base_संदेश);
	}

	mosquitto_FREE(item);
}


स्थिर शून्य db__fill_inflight_out_from_queue(संरचना mosquitto *context)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.queued, client_संदेश, tmp){
		जर(!db__ready_for_flight(context, mosq_md_out, client_संदेश->data.qos)){
			परत;
		}
		निवड(client_संदेश->data.qos){
			स्थिती 0:
				client_संदेश->data.state = mosq_ms_publish_qos0;
				थांबा;
			स्थिती 1:
				client_संदेश->data.state = mosq_ms_publish_qos1;
				थांबा;
			स्थिती 2:
				client_संदेश->data.state = mosq_ms_publish_qos2;
				थांबा;
		}
		जर(client_संदेश->base_संदेश->data.expiry_time && db.now_real_s > client_संदेश->base_संदेश->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_संदेश);
			सुरूठेवा;
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
	जर(संदेश_data->inflight_quota > 0){
		संदेश_data->inflight_quota--;
	}

	db_संदेश_remove_from_queued_stats(संदेश_data, client_संदेश);
	db_संदेश_add_to_inflight_stats(संदेश_data, client_संदेश);
}


पूर्णांक db__message_delete_outgoing(संरचना mosquitto *context, uint16_t mid, गणना mosquitto_संदेश_state expect_state, पूर्णांक qos)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;
	bool deleted = false;

	जर(!context){
		परत MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_संदेश, tmp){
		जर(client_संदेश->data.mid == mid){
			जर(client_संदेश->data.qos != qos){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_संदेश->data.qos, qos);
				परत MOSQ_ERR_PROTOCOL;
			}अन्यथा जर(qos == 2 && client_संदेश->data.state != expect_state && expect_state != mosq_ms_any){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_संदेश->data.state, expect_state);
				परत MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_out, client_संदेश);
			deleted = true;
			थांबा;
		}
	}

	जर(deleted == false){
		DL_FOREACH_SAFE(context->msgs_out.queued, client_संदेश, tmp){
			जर(client_संदेश->data.mid == mid){
				जर(client_संदेश->data.qos != qos){
					परत MOSQ_ERR_PROTOCOL;
				}अन्यथा जर(qos == 2 && client_संदेश->data.state != expect_state && expect_state != mosq_ms_any){
					परत MOSQ_ERR_PROTOCOL;
				}
				db__message_remove_queued(context, &context->msgs_out, client_संदेश);
				थांबा;
			}
		}
	}
	db__fill_inflight_out_from_queue(context);
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	परत db__message_write_inflight_out_latest(context);
}


/* Only for QoS 2 messages */
पूर्णांक db_message_घाला_incoming(संरचना mosquitto *context, uint64_t cmsg_id, संरचना mosquitto_base_संदेश *base_संदेश, bool persist)
{
	संरचना mosquitto_client_संदेश *client_संदेश;
	संरचना mosquitto_संदेश_data *संदेश_data;
	गणना mosquitto_संदेश_state state = mosq_ms_invalid;
	पूर्णांक rc = 0;

	प्रतिज्ञा(base_संदेश);
	जर(!context){
		परत MOSQ_ERR_INVAL;
	}
	जर(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		परत MOSQ_ERR_SUCCESS;

	}
	संदेश_data = &context->msgs_in;

	जर(db__ready_for_flight(context, mosq_md_in, base_संदेश->data.qos)){
		state = mosq_ms_wait_for_pubrel;
	}अन्यथा जर(base_संदेश->data.qos != 0 && db__ready_for_queue(context, base_संदेश->data.qos, संदेश_data)){
		state = mosq_ms_queued;
		rc = 2;
	}अन्यथा{
		/* Dropping message due to full queue. */
		जर(context->is_dropping == false){
			context->is_dropping = true;
			log__printf(रिक्त, MOSQ_LOG_NOTICE,
					"Outgoing messages are being dropped for client %s.",
					context->id);
		}
		metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		परत 2;
	}

	प्रतिज्ञा(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	जर(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_संदेश = mosquitto_malloc(आकार(संरचना mosquitto_client_संदेश));
	जर(!client_संदेश){
		परत MOSQ_ERR_NOMEM;
	}
	client_संदेश->prev = रिक्त;
	client_संदेश->next = रिक्त;
	जर(cmsg_id){
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
	जर(base_संदेश->data.qos > context->max_qos){
		client_संदेश->data.qos = context->max_qos;
	}अन्यथा{
		client_संदेश->data.qos = base_संदेश->data.qos;
	}
	client_संदेश->data.retain = base_संदेश->data.retain;
	client_संदेश->data.subscription_identifier = 0;

	जर(state == mosq_ms_queued){
		DL_APPEND(संदेश_data->queued, client_संदेश);
		db_संदेश_add_to_queued_stats(संदेश_data, client_संदेश);
	}अन्यथा{
		DL_APPEND(संदेश_data->inflight, client_संदेश);
		db_संदेश_add_to_inflight_stats(संदेश_data, client_संदेश);
	}

	जर(persist && context->is_persisted){
		plugin_persist_handle_base_संदेश_add(client_संदेश->base_संदेश);
		plugin_persist_handle_client_संदेश_add(context, client_संदेश);
	}

	जर(client_संदेश->base_संदेश->data.qos > 0){
		util__decrement_receive_quota(context);
	}
	परत rc;
}


पूर्णांक db_message_घाला_outgoing(संरचना mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, संरचना mosquitto_base_संदेश *base_संदेश, uint32_t subscription_identifier, bool update, bool persist)
{
	संरचना mosquitto_client_संदेश *client_संदेश;
	संरचना mosquitto_संदेश_data *संदेश_data;
	गणना mosquitto_संदेश_state state = mosq_ms_invalid;
	पूर्णांक rc = 0;
	अक्षर **dest_ids;

	प्रतिज्ञा(base_संदेश);
	जर(!context){
		परत MOSQ_ERR_INVAL;
	}
	जर(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		परत MOSQ_ERR_SUCCESS;

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
	जर(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && base_संदेश->dest_ids){

		साठी(पूर्णांक i=0; i<base_संदेश->dest_id_मोजणी; i++){
			जर(base_संदेश->dest_ids[i] && !म_भेद(base_संदेश->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				परत MOSQ_ERR_SUCCESS;
			}
		}
	}
	जर(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		जर(qos == 0 && !db.config->queue_qos0_messages){
			जर(!context->bridge){
				परत 2;
			}अन्यथा{
				जर(context->bridge->start_type != bst_lazy){
					परत 2;
				}
			}
		}
		जर(context->bridge && context->bridge->clean_start_local == true){
			परत 2;
		}
	}

	जर(net__is_connected(context)){
		जर(db__ready_for_flight(context, mosq_md_out, qos)){
			निवड(qos){
				स्थिती 0:
					state = mosq_ms_publish_qos0;
					थांबा;
				स्थिती 1:
					state = mosq_ms_publish_qos1;
					थांबा;
				स्थिती 2:
					state = mosq_ms_publish_qos2;
					थांबा;
			}
		}अन्यथा जर(qos != 0 && db__ready_for_queue(context, qos, संदेश_data)){
			state = mosq_ms_queued;
			rc = 2;
		}अन्यथा{
			/* Dropping message due to full queue. */
			जर(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(रिक्त, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			परत 2;
		}
	}अन्यथा{
		जर(db__ready_for_queue(context, qos, संदेश_data)){
			state = mosq_ms_queued;
		}अन्यथा{
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			जर(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(रिक्त, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			परत 2;
		}
	}
	प्रतिज्ञा(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	जर(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_संदेश = mosquitto_malloc(आकार(संरचना mosquitto_client_संदेश));
	जर(!client_संदेश){
		परत MOSQ_ERR_NOMEM;
	}
	client_संदेश->prev = रिक्त;
	client_संदेश->next = रिक्त;
	जर(cmsg_id){
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
	जर(qos > context->max_qos){
		client_संदेश->data.qos = context->max_qos;
	}अन्यथा{
		client_संदेश->data.qos = qos;
	}
	client_संदेश->data.retain = retain;
	client_संदेश->data.subscription_identifier = subscription_identifier;

	जर(state == mosq_ms_queued){
		DL_APPEND(संदेश_data->queued, client_संदेश);
		db_संदेश_add_to_queued_stats(संदेश_data, client_संदेश);
	}अन्यथा{
		DL_APPEND(संदेश_data->inflight, client_संदेश);
		db_संदेश_add_to_inflight_stats(संदेश_data, client_संदेश);
	}

	जर(persist && context->is_persisted){
		plugin_persist_handle_base_संदेश_add(client_संदेश->base_संदेश);
		plugin_persist_handle_client_संदेश_add(context, client_संदेश);
	}

	जर(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(base_संदेश->dest_ids, आकार(अक्षर *)*(माप_प्रकार)(base_संदेश->dest_id_मोजणी+1));
		जर(dest_ids){
			base_संदेश->dest_ids = dest_ids;
			base_संदेश->dest_id_मोजणी++;
			base_संदेश->dest_ids[base_संदेश->dest_id_मोजणी-1] = mosquitto_strdup(context->id);
			जर(!base_संदेश->dest_ids[base_संदेश->dest_id_मोजणी-1]){
				परत MOSQ_ERR_NOMEM;
			}
		}अन्यथा{
			परत MOSQ_ERR_NOMEM;
		}
	}
#ifdef WITH_BRIDGE
	जर(context->bridge && context->bridge->start_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_out.inflight_मोजणी + context->msgs_out.queued_मोजणी >= context->bridge->threshold){

		context->bridge->lazy_reconnect = true;
	}
#endif

	जर(client_संदेश->data.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	जर(update){
		rc = db__message_write_inflight_out_latest(context);
		जर(rc){
			परत rc;
		}
		rc = db__message_write_queued_out(context);
		जर(rc){
			परत rc;
		}
	}

	परत rc;
}


स्थिर ओळीत पूर्णांक db__message_update_outgoing_state(संरचना mosquitto *context, संरचना mosquitto_client_संदेश *head,
		uint16_t mid, गणना mosquitto_संदेश_state state, पूर्णांक qos, bool persist)
{
	संरचना mosquitto_client_संदेश *client_संदेश;

	DL_FOREACH(head, client_संदेश){
		जर(client_संदेश->data.mid == mid){
			जर(client_संदेश->data.qos != qos){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_संदेश->data.qos, qos);
				परत MOSQ_ERR_PROTOCOL;
			}
			client_संदेश->data.state = (uint8_t)state;
			जर(persist){
				plugin_persist_handle_client_संदेश_update(context, client_संदेश);
			}
			परत MOSQ_ERR_SUCCESS;
		}
	}
	परत MOSQ_ERR_NOT_FOUND;
}


पूर्णांक db__message_update_outgoing(संरचना mosquitto *context, uint16_t mid, गणना mosquitto_संदेश_state state, पूर्णांक qos, bool persist)
{
	पूर्णांक rc;

	rc = db__message_update_outgoing_state(context, context->msgs_out.inflight, mid, state, qos, persist);
	जर(!persist && rc == MOSQ_ERR_NOT_FOUND){
		rc = db__message_update_outgoing_state(context, context->msgs_out.queued, mid, state, qos, persist);
	}
	परत rc;
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


पूर्णांक db__messages_delete_incoming(संरचना mosquitto *context)
{
	जर(!context){
		परत MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_in.inflight);
	db__messages_delete_list(&context->msgs_in.queued);
	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_मोजणी = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_मोजणी = 0;
	context->msgs_in.queued_count12 = 0;

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक db__messages_delete_outgoing(संरचना mosquitto *context)
{
	जर(!context){
		परत MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_out.inflight);
	db__messages_delete_list(&context->msgs_out.queued);
	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_मोजणी = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_मोजणी = 0;
	context->msgs_out.queued_count12 = 0;

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक db__messages_delete(संरचना mosquitto *context, bool force_free)
{
	जर(!context){
		परत MOSQ_ERR_INVAL;
	}

	जर(force_free || context->clean_start || (context->bridge && context->bridge->clean_start)){
		db__messages_delete_incoming(context);
	}

	जर(force_free || (context->bridge && context->bridge->clean_start_local)
			|| (context->bridge == रिक्त && context->clean_start)){

		db__messages_delete_outgoing(context);
	}

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक db__messages_easy_queue(संरचना mosquitto *context, स्थिरांक अक्षर *topic, uint8_t qos, uint32_t payloadlen, स्थिरांक शून्य *payload, पूर्णांक retain, uint32_t message_expiry_interval, mosquitto_property **properties)
{
	संरचना mosquitto_base_संदेश *base_संदेश;
	स्थिरांक अक्षर *source_id;
	गणना mosquitto_संदेश_origin origin;

	जर(!topic){
		परत MOSQ_ERR_INVAL;
	}

	base_संदेश = mosquitto_calloc(1, आकार(संरचना mosquitto_base_संदेश));
	जर(base_संदेश == रिक्त){
		परत MOSQ_ERR_NOMEM;
	}

	base_संदेश->data.topic = mosquitto_strdup(topic);
	जर(base_संदेश->data.topic == रिक्त){
		db_संदेश_store_free(base_संदेश);
		परत MOSQ_ERR_INVAL;
	}

	base_संदेश->data.qos = qos;
	जर(db.config->retain_available == false){
		base_संदेश->data.retain = 0;
	}अन्यथा{
		base_संदेश->data.retain = retain;
	}

	base_संदेश->data.payloadlen = payloadlen;
	जर(payloadlen > 0){
		base_संदेश->data.payload = mosquitto_malloc(base_संदेश->data.payloadlen+1);
		जर(base_संदेश->data.payload == रिक्त){
			db_संदेश_store_free(base_संदेश);
			परत MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_संदेश->data.payload)[base_संदेश->data.payloadlen] = 0;
		स_नकल(base_संदेश->data.payload, payload, base_संदेश->data.payloadlen);
	}

	जर(context && context->id){
		source_id = context->id;
	}अन्यथा{
		source_id = "";
	}
	जर(properties){
		base_संदेश->data.properties = *properties;
		*properties = रिक्त;
	}

	जर(context){
		origin = mosq_mo_client;
	}अन्यथा{
		origin = mosq_mo_broker;
	}
	जर(db__message_store(context, base_संदेश, &message_expiry_interval, origin)){
		परत 1;
	}

	परत sub__messages_queue(source_id, base_संदेश->data.qos, &base_संदेश);
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

	id = db.गाठ_id_shifted; /* Top 10-bits */

#ifdef WIN32
	GetSystemTimePreciseAsFileTime(&ftime);
	ftime64 = (((uint64_t)ftime.dw_वरचा_Date_Time)<<32) + ftime.dw_खालचा_Date_Time;
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

	जर(id <= db.last_db_id){
		id = db.last_db_id + 1;
	}
	db.last_db_id = id;

	परत id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
पूर्णांक db__message_store(स्थिरांक संरचना mosquitto *source, संरचना mosquitto_base_संदेश *base_संदेश, uint32_t *message_expiry_interval, गणना mosquitto_संदेश_origin origin)
{
	पूर्णांक rc;

	प्रतिज्ञा(base_संदेश);

	जर(source && source->id){
		base_संदेश->data.source_id = mosquitto_strdup(source->id);
	}अन्यथा{
		base_संदेश->data.source_id = mosquitto_strdup("");
	}
	जर(!base_संदेश->data.source_id){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_संदेश_store_free(base_संदेश);
		परत MOSQ_ERR_NOMEM;
	}

	जर(source && source->username){
		base_संदेश->data.source_username = mosquitto_strdup(source->username);
		जर(!base_संदेश->data.source_username){
			db_संदेश_store_free(base_संदेश);
			परत MOSQ_ERR_NOMEM;
		}
	}
	जर(source){
		base_संदेश->source_listener = source->listener;
	}
	base_संदेश->origin = origin;
	जर(message_expiry_interval){
		जर(*message_expiry_interval > 0 && *message_expiry_interval != संदेश_EXPIRY_INFINITE){
			base_संदेश->data.expiry_time = db.now_real_s + *message_expiry_interval;
		}अन्यथा{
			base_संदेश->data.expiry_time = 0;
		}
	}

	base_संदेश->dest_ids = रिक्त;
	base_संदेश->dest_id_मोजणी = 0;
	db.संदेश_store_मोजणी++;
	db.संदेश_store_bytes += base_संदेश->data.payloadlen;

	जर(!base_संदेश->data.store_id){
		base_संदेश->data.store_id = db_new_संदेश_id();
	}

	rc = db_संदेश_store_add(base_संदेश);
	जर(rc){
		db_संदेश_store_free(base_संदेश);
		परत rc;
	}

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक db__message_store_find(संरचना mosquitto *context, uint16_t mid, संरचना mosquitto_client_संदेश **client_संदेश)
{
	संरचना mosquitto_client_संदेश *cmsg;

	*client_संदेश = रिक्त;

	जर(!context){
		परत MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_in.inflight, cmsg){
		जर(cmsg->base_संदेश->data.source_mid == mid){
			*client_संदेश = cmsg;
			परत MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_in.queued, cmsg){
		जर(cmsg->base_संदेश->data.source_mid == mid){
			*client_संदेश = cmsg;
			परत MOSQ_ERR_SUCCESS;
		}
	}

	परत 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
स्थिर पूर्णांक db__message_reconnect_reset_outgoing(संरचना mosquitto *context)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;

	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_मोजणी = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_मोजणी = 0;
	context->msgs_out.queued_count12 = 0;
	context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_संदेश, tmp){
		db_संदेश_add_to_inflight_stats(&context->msgs_out, client_संदेश);
		जर(client_संदेश->data.qos > 0){
			util__decrement_send_quota(context);
		}

		निवड(client_संदेश->data.qos){
			स्थिती 0:
				client_संदेश->data.state = mosq_ms_publish_qos0;
				थांबा;
			स्थिती 1:
				client_संदेश->data.state = mosq_ms_publish_qos1;
				थांबा;
			स्थिती 2:
				जर(client_संदेश->data.state == mosq_ms_wait_for_pubcomp){
					client_संदेश->data.state = mosq_ms_resend_pubrel;
				}अन्यथा{
					client_संदेश->data.state = mosq_ms_publish_qos2;
				}
				थांबा;
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

	परत MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
स्थिर पूर्णांक db__message_reconnect_reset_incoming(संरचना mosquitto *context)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;

	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_मोजणी = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_मोजणी = 0;
	context->msgs_in.queued_count12 = 0;
	context->msgs_in.inflight_quota = context->msgs_in.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_संदेश, tmp){
		db_संदेश_add_to_inflight_stats(&context->msgs_in, client_संदेश);
		जर(client_संदेश->data.qos > 0){
			util__decrement_receive_quota(context);
		}

		जर(client_संदेश->data.qos != 2){
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
		जर(db__ready_for_flight(context, mosq_md_in, client_संदेश->data.qos)){
			निवड(client_संदेश->data.qos){
				स्थिती 0:
					client_संदेश->data.state = mosq_ms_publish_qos0;
					थांबा;
				स्थिती 1:
					client_संदेश->data.state = mosq_ms_publish_qos1;
					थांबा;
				स्थिती 2:
					client_संदेश->data.state = mosq_ms_publish_qos2;
					थांबा;
			}
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_संदेश_update(context, client_संदेश);
		}
	}

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक db__message_reconnect_reset(संरचना mosquitto *context)
{
	पूर्णांक rc;

	rc = db__message_reconnect_reset_outgoing(context);
	जर(rc){
		परत rc;
	}
	परत db__message_reconnect_reset_incoming(context);
}


पूर्णांक db__message_remove_incoming(संरचना mosquitto *context, uint16_t mid)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;

	जर(!context){
		परत MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_संदेश, tmp){
		जर(client_संदेश->data.mid == mid){
			जर(client_संदेश->base_संदेश->data.qos != 2){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_संदेश->base_संदेश->data.qos);
				परत MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_in, client_संदेश);
			परत MOSQ_ERR_SUCCESS;
		}
	}

	परत MOSQ_ERR_NOT_FOUND;
}


पूर्णांक db__message_release_incoming(संरचना mosquitto *context, uint16_t mid)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;
	bool deleted = false;
	पूर्णांक rc;

	जर(!context){
		परत MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_संदेश, tmp){
		जर(client_संदेश->data.mid == mid){
			जर(client_संदेश->base_संदेश->data.qos != 2){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_संदेश->base_संदेश->data.qos);
				परत MOSQ_ERR_PROTOCOL;
			}
			स्थिरांक अक्षर *topic = client_संदेश->base_संदेश->data.topic;
			स्थिरांक अक्षर *source_id = client_संदेश->base_संदेश->data.source_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			जर(topic == रिक्त){
				db__message_remove_inflight(context, &context->msgs_in, client_संदेश);
				deleted = true;
			}अन्यथा{
				rc = sub__messages_queue(source_id, 2, &client_संदेश->base_संदेश);
				जर(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db__message_remove_inflight(context, &context->msgs_in, client_संदेश);
					deleted = true;
				}अन्यथा{
					परत 1;
				}
			}
		}
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_संदेश, tmp){
		जर(db__ready_for_flight(context, mosq_md_in, client_संदेश->data.qos)){
			थांबा;
		}

		जर(client_संदेश->data.qos == 2){
			send__pubrec(context, client_संदेश->data.mid, 0, रिक्त);
			client_संदेश->data.state = mosq_ms_wait_for_pubrel;
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_संदेश_update(context, client_संदेश);
		}
	}
	जर(deleted){
		परत MOSQ_ERR_SUCCESS;
	}अन्यथा{
		परत MOSQ_ERR_NOT_FOUND;
	}
}


शून्य db__expire_all_messages(संरचना mosquitto *context)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_संदेश, tmp){
		जर(client_संदेश->base_संदेश->data.expiry_time && db.now_real_s > client_संदेश->base_संदेश->data.expiry_time){
			जर(client_संदेश->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_संदेश);
		}
	}
	db__fill_inflight_out_from_queue(context);
	DL_FOREACH_SAFE(context->msgs_out.queued, client_संदेश, tmp){
		जर(client_संदेश->base_संदेश->data.expiry_time && db.now_real_s > client_संदेश->base_संदेश->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_संदेश);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.inflight, client_संदेश, tmp){
		जर(client_संदेश->base_संदेश->data.expiry_time && db.now_real_s > client_संदेश->base_संदेश->data.expiry_time){
			जर(client_संदेश->data.qos > 0){
				util__increment_receive_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_in, client_संदेश);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.queued, client_संदेश, tmp){
		जर(client_संदेश->base_संदेश->data.expiry_time && db.now_real_s > client_संदेश->base_संदेश->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_in, client_संदेश);
		}
	}
}


स्थिर शून्य db__client_messages_check_acl(संरचना mosquitto *context, संरचना mosquitto_संदेश_data *संदेश_data, संरचना mosquitto_client_संदेश **head,
		शून्य (*decrement_stats_fn)(संरचना mosquitto_संदेश_data *संदेश_data, संरचना mosquitto_client_संदेश *client_संदेश))
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;
	संरचना mosquitto_base_संदेश *base_संदेश;
	पूर्णांक access;

	DL_FOREACH_SAFE((*head), client_संदेश, tmp){
		base_संदेश = client_संदेश->base_संदेश;
		जर(client_संदेश->data.direction == mosq_md_out){
			access = MOSQ_ACL_READ;
		}अन्यथा{
			access = MOSQ_ACL_WRITE;
		}
		जर(mosquitto_acl_check(context, base_संदेश->data.topic,
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


स्थिर पूर्णांक db__message_write_inflight_out_single(संरचना mosquitto *context, संरचना mosquitto_client_संदेश *client_संदेश)
{
	संरचना mosquitto_base_संदेश *base_संदेश;
	mosquitto_property *base_संदेश_props = रिक्त;
	पूर्णांक rc;
	uint16_t mid;
	पूर्णांक retries;
	पूर्णांक retain;
	स्थिरांक अक्षर *topic;
	uint8_t qos;
	uint32_t payloadlen;
	स्थिरांक शून्य *payload;
	uint32_t expiry_interval;
	uint32_t subscription_id;

	base_संदेश = client_संदेश->base_संदेश;

	expiry_interval = 0;
	जर(base_संदेश->data.expiry_time){
		जर(db.now_real_s > base_संदेश->data.expiry_time){
			/* Message is expired, must not send. */
			जर(client_संदेश->data.direction == mosq_md_out && client_संदेश->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_संदेश);
			db__fill_inflight_out_from_queue(context);
			परत MOSQ_ERR_SUCCESS;
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

	निवड(client_संदेश->data.state){
		स्थिती mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_संदेश_props, expiry_interval);
			जर(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_संदेश);
			}अन्यथा{
				परत rc;
			}
			थांबा;

		स्थिती mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_संदेश_props, expiry_interval);
			जर(rc == MOSQ_ERR_SUCCESS){
				client_संदेश->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_संदेश->data.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_संदेश_update(context, client_संदेश);
			}अन्यथा जर(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_संदेश);
			}अन्यथा{
				परत rc;
			}
			थांबा;

		स्थिती mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_संदेश_props, expiry_interval);
			जर(rc == MOSQ_ERR_SUCCESS){
				client_संदेश->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_संदेश->data.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_संदेश_update(context, client_संदेश);
			}अन्यथा जर(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_संदेश);
			}अन्यथा{
				परत rc;
			}
			थांबा;

		स्थिती mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, रिक्त);
			जर(!rc){
				client_संदेश->data.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_संदेश_update(context, client_संदेश);
			}अन्यथा{
				परत rc;
			}
			थांबा;

		स्थिती mosq_ms_invalid:
		स्थिती mosq_ms_send_pubrec:
		स्थिती mosq_ms_resend_pubcomp:
		स्थिती mosq_ms_wait_for_puback:
		स्थिती mosq_ms_wait_for_pubrec:
		स्थिती mosq_ms_wait_for_pubrel:
		स्थिती mosq_ms_wait_for_pubcomp:
		स्थिती mosq_ms_queued:
			थांबा;
	}
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक db__message_write_inflight_out_all(संरचना mosquitto *context)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;
	पूर्णांक rc;

	जर(context->state != mosq_cs_active || !net__is_connected(context)){
		परत MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_संदेश, tmp){
		rc = db__message_write_inflight_out_single(context, client_संदेश);
		जर(rc){
			परत rc;
		}
	}
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक db__message_write_inflight_out_latest(संरचना mosquitto *context)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *next;
	पूर्णांक rc;

	जर(context->state != mosq_cs_active
			|| !net__is_connected(context)
			|| context->msgs_out.inflight == रिक्त){

		परत MOSQ_ERR_SUCCESS;
	}

	जर(context->msgs_out.inflight->prev == context->msgs_out.inflight){
		/* Only one message */
		परत db__message_write_inflight_out_single(context, context->msgs_out.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_संदेश = context->msgs_out.inflight->prev;
	पर्यंत(client_संदेश != context->msgs_out.inflight &&
			(client_संदेश->data.state == mosq_ms_publish_qos0
			|| client_संदेश->data.state == mosq_ms_publish_qos1
			|| client_संदेश->data.state == mosq_ms_publish_qos2)){

		client_संदेश = client_संदेश->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	जर(client_संदेश != context->msgs_out.inflight){
		client_संदेश = client_संदेश->next;
	}

	पर्यंत(client_संदेश){
		next = client_संदेश->next;
		rc = db__message_write_inflight_out_single(context, client_संदेश);
		जर(rc){
			परत rc;
		}
		client_संदेश = next;
	}
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक db__message_write_queued_in(संरचना mosquitto *context)
{
	संरचना mosquitto_client_संदेश *client_संदेश, *tmp;
	पूर्णांक rc;

	जर(context->state != mosq_cs_active){
		परत MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_संदेश, tmp){
		जर(context->msgs_in.inflight_maximum != 0 && context->msgs_in.inflight_quota == 0){
			थांबा;
		}

		जर(client_संदेश->data.qos == 2){
			client_संदेश->data.state = mosq_ms_send_pubrec;
			db__message_dequeue_first(context, &context->msgs_in);
			rc = send__pubrec(context, client_संदेश->data.mid, 0, रिक्त);
			जर(!rc){
				client_संदेश->data.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_संदेश_update(context, client_संदेश);
			}अन्यथा{
				plugin_persist_handle_client_संदेश_update(context, client_संदेश);
				परत rc;
			}
		}
	}
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक db__message_write_queued_out(संरचना mosquitto *context)
{
	जर(context->state != mosq_cs_active){
		परत MOSQ_ERR_SUCCESS;
	}

	db__fill_inflight_out_from_queue(context);

	परत MOSQ_ERR_SUCCESS;
}
