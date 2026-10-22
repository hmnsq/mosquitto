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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <utlist.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "send_mosq.h"
#അന്തര്ഭാവ "sys_tree.h"
#അന്തര്ഭാവ "util_mosq.h"


/**
 * Is this context ready to take more in flight messages right now?
 * @param context the client context of interest
 * @param qos qos for the packet of interest
 * @return true if more in flight are allowed.
 */
bool db__ready_for_flight(ഘടന mosquitto *context, എണ്ണൽ mosquitto_സന്ദേശം_direction dir, പൂർണ്ണസംഖ്യ qos)
{
	ഘടന mosquitto_സന്ദേശം_data *msgs;
	bool valid_bytes;
	bool valid_എണ്ണം;

	എങ്കിൽ(dir == mosq_md_out){
		msgs = &context->msgs_out;
	}അല്ലെങ്കിൽ{
		msgs = &context->msgs_in;
	}

	എങ്കിൽ(msgs->inflight_maximum == 0 && db.config->max_inflight_bytes == 0){
		തിരികെ true;
	}

	എങ്കിൽ(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		എങ്കിൽ(db.config->max_queued_messages == 0 && db.config->max_inflight_bytes == 0){
			തിരികെ true;
		}
		valid_bytes = ((msgs->inflight_bytes - (ssize_t)db.config->max_inflight_bytes) < (ssize_t)db.config->max_queued_bytes);
		എങ്കിൽ(dir == mosq_md_out){
			valid_എണ്ണം = context->out_packet_എണ്ണം < db.config->max_queued_messages;
		}അല്ലെങ്കിൽ{
			valid_എണ്ണം = msgs->inflight_എണ്ണം - msgs->inflight_maximum < db.config->max_queued_messages;
		}

		എങ്കിൽ(db.config->max_queued_messages == 0){
			തിരികെ valid_bytes;
		}
		എങ്കിൽ(db.config->max_queued_bytes == 0){
			തിരികെ valid_എണ്ണം;
		}
	}അല്ലെങ്കിൽ{
		valid_bytes = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_bytes;
		valid_എണ്ണം = msgs->inflight_quota > 0;

		എങ്കിൽ(msgs->inflight_maximum == 0){
			തിരികെ valid_bytes;
		}
		എങ്കിൽ(db.config->max_inflight_bytes == 0){
			തിരികെ valid_എണ്ണം;
		}
	}

	തിരികെ valid_bytes && valid_എണ്ണം;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db__ready_for_queue(ഘടന mosquitto *context, പൂർണ്ണസംഖ്യ qos, ഘടന mosquitto_സന്ദേശം_data *സന്ദേശം_data)
{
	പൂർണ്ണസംഖ്യ source_എണ്ണം;
	പൂർണ്ണസംഖ്യ adjust_എണ്ണം;
	long source_bytes;
	ssize_t adjust_bytes = (ssize_t)db.config->max_inflight_bytes;
	bool valid_bytes;
	bool valid_എണ്ണം;

	എങ്കിൽ(db.config->max_queued_messages == 0 && db.config->max_queued_bytes == 0){
		തിരികെ true;
	}

	എങ്കിൽ(qos == 0 && db.config->queue_qos0_messages == false){
		തിരികെ false; /* This case is handled in db__ready_for_flight() */
	}അല്ലെങ്കിൽ{
		source_bytes = (ssize_t)സന്ദേശം_data->queued_bytes12;
		source_എണ്ണം = സന്ദേശം_data->queued_count12;
	}
	adjust_എണ്ണം = സന്ദേശം_data->inflight_maximum;

	/* nothing in flight for offline clients */
	എങ്കിൽ(!net__is_connected(context)){
		adjust_bytes = 0;
		adjust_എണ്ണം = 0;
	}

	valid_bytes = (source_bytes - (ssize_t)adjust_bytes) < (ssize_t)db.config->max_queued_bytes;
	valid_എണ്ണം = source_എണ്ണം - adjust_എണ്ണം < db.config->max_queued_messages;

	എങ്കിൽ(db.config->max_queued_bytes == 0){
		തിരികെ valid_എണ്ണം;
	}
	എങ്കിൽ(db.config->max_queued_messages == 0){
		തിരികെ valid_bytes;
	}

	തിരികെ valid_bytes && valid_എണ്ണം;
}


ശൂന്യം db_സന്ദേശം_add_to_inflight_stats(ഘടന mosquitto_സന്ദേശം_data *സന്ദേശം_data, ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം)
{
	സന്ദേശം_data->inflight_എണ്ണം++;
	സന്ദേശം_data->inflight_bytes += client_സന്ദേശം->base_സന്ദേശം->data.payloadlen;
	എങ്കിൽ(client_സന്ദേശം->data.qos != 0){
		സന്ദേശം_data->inflight_count12++;
		സന്ദേശം_data->inflight_bytes12 += client_സന്ദേശം->base_സന്ദേശം->data.payloadlen;
	}
}


സ്ഥിതം ശൂന്യം db_സന്ദേശം_remove_from_inflight_stats(ഘടന mosquitto_സന്ദേശം_data *സന്ദേശം_data, ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം)
{
	സന്ദേശം_data->inflight_എണ്ണം--;
	സന്ദേശം_data->inflight_bytes -= client_സന്ദേശം->base_സന്ദേശം->data.payloadlen;
	എങ്കിൽ(client_സന്ദേശം->data.qos != 0){
		സന്ദേശം_data->inflight_count12--;
		സന്ദേശം_data->inflight_bytes12 -= client_സന്ദേശം->base_സന്ദേശം->data.payloadlen;
	}
}


ശൂന്യം db_സന്ദേശം_add_to_queued_stats(ഘടന mosquitto_സന്ദേശം_data *സന്ദേശം_data, ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം)
{
	സന്ദേശം_data->queued_എണ്ണം++;
	സന്ദേശം_data->queued_bytes += client_സന്ദേശം->base_സന്ദേശം->data.payloadlen;
	എങ്കിൽ(client_സന്ദേശം->data.qos != 0){
		സന്ദേശം_data->queued_count12++;
		സന്ദേശം_data->queued_bytes12 += client_സന്ദേശം->base_സന്ദേശം->data.payloadlen;
	}
}


സ്ഥിതം ശൂന്യം db_സന്ദേശം_remove_from_queued_stats(ഘടന mosquitto_സന്ദേശം_data *സന്ദേശം_data, ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം)
{
	സന്ദേശം_data->queued_എണ്ണം--;
	സന്ദേശം_data->queued_bytes -= client_സന്ദേശം->base_സന്ദേശം->data.payloadlen;
	എങ്കിൽ(client_സന്ദേശം->data.qos != 0){
		സന്ദേശം_data->queued_count12--;
		സന്ദേശം_data->queued_bytes12 -= client_സന്ദേശം->base_സന്ദേശം->data.payloadlen;
	}
}


പൂർണ്ണസംഖ്യ db__open(ഘടന mosquitto__config *config)
{
	എങ്കിൽ(!config){
		തിരികെ MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = ശൂന്യ;
	db.contexts_by_sock = ശൂന്യ;
	db.contexts_for_free = ശൂന്യ;
#ifdef WITH_BRIDGE
	db.bridges = ശൂന്യ;
	db.bridge_എണ്ണം = 0;
#endif

	/* Initialize the hashtable */
	db.clientid_index_hash = ശൂന്യ;

	db.normal_subs = ശൂന്യ;
	db.shared_subs = ശൂന്യ;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = ശൂന്യ;

#ifdef WITH_PERSISTENCE
	എങ്കിൽ(persist__restore()){
		തിരികെ 1;
	}
#endif

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം ശൂന്യം subhier_clean(ഘടന mosquitto__subhier **subhier)
{
	ഘടന mosquitto__subhier *peer, *subhier_tmp;
	ഘടന mosquitto__subleaf *leaf, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		leaf = peer->subs;
		ആകുമ്പോൾ(leaf){
			nextleaf = leaf->next;
			mosquitto_FREE(leaf);
			leaf = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


പൂർണ്ണസംഖ്യ db__close(ശൂന്യം)
{
	subhier_clean(&db.normal_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db_സന്ദേശം_store_clean();

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ db_സന്ദേശം_store_add(ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം)
{
	ഘടന mosquitto_base_സന്ദേശം *found;
	unsigned hashv;

	HASH_മൂല്യം(&base_സന്ദേശം->data.store_id, വലുപ്പം(base_സന്ദേശം->data.store_id), hashv);
	HASH_FIND_BYHASHVALUE(hh, db.സന്ദേശം_store, &base_സന്ദേശം->data.store_id, വലുപ്പം(base_സന്ദേശം->data.store_id), hashv, found);
	എങ്കിൽ(found == ശൂന്യ){
		HASH_ADD_KEYPTR_BYHASHVALUE(hh, db.സന്ദേശം_store, &base_സന്ദേശം->data.store_id, വലുപ്പം(base_സന്ദേശം->data.store_id), hashv, base_സന്ദേശം);
		തിരികെ MOSQ_ERR_SUCCESS;
	}അല്ലെങ്കിൽ{
		തിരികെ MOSQ_ERR_ALREADY_EXISTS;
	}
}


ശൂന്യം db_സന്ദേശം_store_free(ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം)
{
	mosquitto_FREE(base_സന്ദേശം->data.source_id);
	mosquitto_FREE(base_സന്ദേശം->data.source_username);
	എങ്കിൽ(base_സന്ദേശം->dest_ids){
		വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<base_സന്ദേശം->dest_id_എണ്ണം; i++){
			mosquitto_FREE(base_സന്ദേശം->dest_ids[i]);
		}
		mosquitto_FREE(base_സന്ദേശം->dest_ids);
	}
	mosquitto_FREE(base_സന്ദേശം->data.topic);
	mosquitto_property_free_all(&base_സന്ദേശം->data.properties);
	mosquitto_FREE(base_സന്ദേശം->data.payload);
	mosquitto_FREE(base_സന്ദേശം);
}


ശൂന്യം db_സന്ദേശം_store_remove(ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം, bool notify)
{
	എങ്കിൽ(base_സന്ദേശം == ശൂന്യ){
		തിരികെ;
	}
	HASH_DELETE(hh, db.സന്ദേശം_store, base_സന്ദേശം);
	db.സന്ദേശം_store_എണ്ണം--;
	db.സന്ദേശം_store_bytes -= base_സന്ദേശം->data.payloadlen;
	എങ്കിൽ(notify == true){
		plugin_persist_handle_base_സന്ദേശം_delete(base_സന്ദേശം);
	}
	db_സന്ദേശം_store_free(base_സന്ദേശം);
}


ശൂന്യം db_സന്ദേശം_store_clean(ശൂന്യം)
{
	ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം, *base_സന്ദേശം_tmp;

	HASH_ITER(hh, db.സന്ദേശം_store, base_സന്ദേശം, base_സന്ദേശം_tmp){
		db_സന്ദേശം_store_remove(base_സന്ദേശം, false);
	}
}


ശൂന്യം db_സന്ദേശം_store_ref_inc(ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം)
{
	base_സന്ദേശം->ref_എണ്ണം++;
}


ശൂന്യം db_സന്ദേശം_store_ref_dec(ഘടന mosquitto_base_സന്ദേശം **base_സന്ദേശം)
{
	(*base_സന്ദേശം)->ref_എണ്ണം--;
	എങ്കിൽ((*base_സന്ദേശം)->ref_എണ്ണം == 0){
		db_സന്ദേശം_store_remove(*base_സന്ദേശം, true);
		*base_സന്ദേശം = ശൂന്യ;
	}
}


ശൂന്യം db_സന്ദേശം_store_compact(ശൂന്യം)
{
	ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം, *base_സന്ദേശം_tmp;

	HASH_ITER(hh, db.സന്ദേശം_store, base_സന്ദേശം, base_സന്ദേശം_tmp){
		എങ്കിൽ(base_സന്ദേശം->ref_എണ്ണം < 1){
			db_സന്ദേശം_store_remove(base_സന്ദേശം, true);
		}
	}
}


സ്ഥിതം ശൂന്യം db__message_remove_inflight(ഘടന mosquitto *context, ഘടന mosquitto_സന്ദേശം_data *സന്ദേശം_data, ഘടന mosquitto_client_സന്ദേശം *item)
{
	എങ്കിൽ(!context || !സന്ദേശം_data || !item){
		തിരികെ;
	}

	plugin_persist_handle_client_സന്ദേശം_delete(context, item);

	DL_DELETE(സന്ദേശം_data->inflight, item);
	എങ്കിൽ(item->base_സന്ദേശം){
		db_സന്ദേശം_remove_from_inflight_stats(സന്ദേശം_data, item);
		db_സന്ദേശം_store_ref_dec(&item->base_സന്ദേശം);
	}

	mosquitto_FREE(item);
}


സ്ഥിതം ശൂന്യം db__message_remove_queued(ഘടന mosquitto *context, ഘടന mosquitto_സന്ദേശം_data *സന്ദേശം_data, ഘടന mosquitto_client_സന്ദേശം *item)
{
	എങ്കിൽ(!context || !സന്ദേശം_data || !item){
		തിരികെ;
	}

	plugin_persist_handle_client_സന്ദേശം_delete(context, item);

	DL_DELETE(സന്ദേശം_data->queued, item);
	എങ്കിൽ(item->base_സന്ദേശം){
		db_സന്ദേശം_remove_from_queued_stats(സന്ദേശം_data, item);
		db_സന്ദേശം_store_ref_dec(&item->base_സന്ദേശം);
	}

	mosquitto_FREE(item);
}


സ്ഥിതം ശൂന്യം db__fill_inflight_out_from_queue(ഘടന mosquitto *context)
{
	ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.queued, client_സന്ദേശം, tmp){
		എങ്കിൽ(!db__ready_for_flight(context, mosq_md_out, client_സന്ദേശം->data.qos)){
			തിരികെ;
		}
		തിരഞ്ഞെടുപ്പ്(client_സന്ദേശം->data.qos){
			അവസ്ഥ 0:
				client_സന്ദേശം->data.state = mosq_ms_publish_qos0;
				നിർത്തുക;
			അവസ്ഥ 1:
				client_സന്ദേശം->data.state = mosq_ms_publish_qos1;
				നിർത്തുക;
			അവസ്ഥ 2:
				client_സന്ദേശം->data.state = mosq_ms_publish_qos2;
				നിർത്തുക;
		}
		എങ്കിൽ(client_സന്ദേശം->base_സന്ദേശം->data.expiry_time && db.now_real_s > client_സന്ദേശം->base_സന്ദേശം->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_സന്ദേശം);
			തുടരുക;
		}
		plugin_persist_handle_client_സന്ദേശം_update(context, client_സന്ദേശം);
		db__message_dequeue_first(context, &context->msgs_out);
	}
}


ശൂന്യം db__message_dequeue_first(ഘടന mosquitto *context, ഘടന mosquitto_സന്ദേശം_data *സന്ദേശം_data)
{
	ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം;

	UNUSED(context);

	client_സന്ദേശം = സന്ദേശം_data->queued;
	DL_DELETE(സന്ദേശം_data->queued, client_സന്ദേശം);
	DL_APPEND(സന്ദേശം_data->inflight, client_സന്ദേശം);
	എങ്കിൽ(സന്ദേശം_data->inflight_quota > 0){
		സന്ദേശം_data->inflight_quota--;
	}

	db_സന്ദേശം_remove_from_queued_stats(സന്ദേശം_data, client_സന്ദേശം);
	db_സന്ദേശം_add_to_inflight_stats(സന്ദേശം_data, client_സന്ദേശം);
}


പൂർണ്ണസംഖ്യ db__message_delete_outgoing(ഘടന mosquitto *context, uint16_t mid, എണ്ണൽ mosquitto_സന്ദേശം_state expect_state, പൂർണ്ണസംഖ്യ qos)
{
	ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം, *tmp;
	bool deleted = false;

	എങ്കിൽ(!context){
		തിരികെ MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_സന്ദേശം, tmp){
		എങ്കിൽ(client_സന്ദേശം->data.mid == mid){
			എങ്കിൽ(client_സന്ദേശം->data.qos != qos){
				log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_സന്ദേശം->data.qos, qos);
				തിരികെ MOSQ_ERR_PROTOCOL;
			}അല്ലെങ്കിൽ എങ്കിൽ(qos == 2 && client_സന്ദേശം->data.state != expect_state && expect_state != mosq_ms_any){
				log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_സന്ദേശം->data.state, expect_state);
				തിരികെ MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_out, client_സന്ദേശം);
			deleted = true;
			നിർത്തുക;
		}
	}

	എങ്കിൽ(deleted == false){
		DL_FOREACH_SAFE(context->msgs_out.queued, client_സന്ദേശം, tmp){
			എങ്കിൽ(client_സന്ദേശം->data.mid == mid){
				എങ്കിൽ(client_സന്ദേശം->data.qos != qos){
					തിരികെ MOSQ_ERR_PROTOCOL;
				}അല്ലെങ്കിൽ എങ്കിൽ(qos == 2 && client_സന്ദേശം->data.state != expect_state && expect_state != mosq_ms_any){
					തിരികെ MOSQ_ERR_PROTOCOL;
				}
				db__message_remove_queued(context, &context->msgs_out, client_സന്ദേശം);
				നിർത്തുക;
			}
		}
	}
	db__fill_inflight_out_from_queue(context);
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	തിരികെ db__message_write_inflight_out_latest(context);
}


/* Only for QoS 2 messages */
പൂർണ്ണസംഖ്യ db_message_തിരുകുക_incoming(ഘടന mosquitto *context, uint64_t cmsg_id, ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം, bool persist)
{
	ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം;
	ഘടന mosquitto_സന്ദേശം_data *സന്ദേശം_data;
	എണ്ണൽ mosquitto_സന്ദേശം_state state = mosq_ms_invalid;
	പൂർണ്ണസംഖ്യ rc = 0;

	പ്രതിജ്ഞാ(base_സന്ദേശം);
	എങ്കിൽ(!context){
		തിരികെ MOSQ_ERR_INVAL;
	}
	എങ്കിൽ(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		തിരികെ MOSQ_ERR_SUCCESS;

	}
	സന്ദേശം_data = &context->msgs_in;

	എങ്കിൽ(db__ready_for_flight(context, mosq_md_in, base_സന്ദേശം->data.qos)){
		state = mosq_ms_wait_for_pubrel;
	}അല്ലെങ്കിൽ എങ്കിൽ(base_സന്ദേശം->data.qos != 0 && db__ready_for_queue(context, base_സന്ദേശം->data.qos, സന്ദേശം_data)){
		state = mosq_ms_queued;
		rc = 2;
	}അല്ലെങ്കിൽ{
		/* Dropping message due to full queue. */
		എങ്കിൽ(context->is_dropping == false){
			context->is_dropping = true;
			log__printf(ശൂന്യ, MOSQ_LOG_NOTICE,
					"Outgoing messages are being dropped for client %s.",
					context->id);
		}
		metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		തിരികെ 2;
	}

	പ്രതിജ്ഞാ(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	എങ്കിൽ(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_സന്ദേശം = mosquitto_malloc(വലുപ്പം(ഘടന mosquitto_client_സന്ദേശം));
	എങ്കിൽ(!client_സന്ദേശം){
		തിരികെ MOSQ_ERR_NOMEM;
	}
	client_സന്ദേശം->prev = ശൂന്യ;
	client_സന്ദേശം->next = ശൂന്യ;
	എങ്കിൽ(cmsg_id){
		client_സന്ദേശം->data.cmsg_id = cmsg_id;
	}അല്ലെങ്കിൽ{
		client_സന്ദേശം->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_സന്ദേശം->base_സന്ദേശം = base_സന്ദേശം;
	db_സന്ദേശം_store_ref_inc(client_സന്ദേശം->base_സന്ദേശം);
	client_സന്ദേശം->data.mid = base_സന്ദേശം->data.source_mid;
	client_സന്ദേശം->data.direction = mosq_md_in;
	client_സന്ദേശം->data.state = (uint8_t)state;
	client_സന്ദേശം->data.dup = false;
	എങ്കിൽ(base_സന്ദേശം->data.qos > context->max_qos){
		client_സന്ദേശം->data.qos = context->max_qos;
	}അല്ലെങ്കിൽ{
		client_സന്ദേശം->data.qos = base_സന്ദേശം->data.qos;
	}
	client_സന്ദേശം->data.retain = base_സന്ദേശം->data.retain;
	client_സന്ദേശം->data.subscription_identifier = 0;

	എങ്കിൽ(state == mosq_ms_queued){
		DL_APPEND(സന്ദേശം_data->queued, client_സന്ദേശം);
		db_സന്ദേശം_add_to_queued_stats(സന്ദേശം_data, client_സന്ദേശം);
	}അല്ലെങ്കിൽ{
		DL_APPEND(സന്ദേശം_data->inflight, client_സന്ദേശം);
		db_സന്ദേശം_add_to_inflight_stats(സന്ദേശം_data, client_സന്ദേശം);
	}

	എങ്കിൽ(persist && context->is_persisted){
		plugin_persist_handle_base_സന്ദേശം_add(client_സന്ദേശം->base_സന്ദേശം);
		plugin_persist_handle_client_സന്ദേശം_add(context, client_സന്ദേശം);
	}

	എങ്കിൽ(client_സന്ദേശം->base_സന്ദേശം->data.qos > 0){
		util__decrement_receive_quota(context);
	}
	തിരികെ rc;
}


പൂർണ്ണസംഖ്യ db_message_തിരുകുക_outgoing(ഘടന mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം, uint32_t subscription_identifier, bool update, bool persist)
{
	ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം;
	ഘടന mosquitto_സന്ദേശം_data *സന്ദേശം_data;
	എണ്ണൽ mosquitto_സന്ദേശം_state state = mosq_ms_invalid;
	പൂർണ്ണസംഖ്യ rc = 0;
	അക്ഷരം **dest_ids;

	പ്രതിജ്ഞാ(base_സന്ദേശം);
	എങ്കിൽ(!context){
		തിരികെ MOSQ_ERR_INVAL;
	}
	എങ്കിൽ(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		തിരികെ MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_sent++;

	സന്ദേശം_data = &context->msgs_out;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	എങ്കിൽ(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && base_സന്ദേശം->dest_ids){

		വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<base_സന്ദേശം->dest_id_എണ്ണം; i++){
			എങ്കിൽ(base_സന്ദേശം->dest_ids[i] && !strcmp(base_സന്ദേശം->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				തിരികെ MOSQ_ERR_SUCCESS;
			}
		}
	}
	എങ്കിൽ(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		എങ്കിൽ(qos == 0 && !db.config->queue_qos0_messages){
			എങ്കിൽ(!context->bridge){
				തിരികെ 2;
			}അല്ലെങ്കിൽ{
				എങ്കിൽ(context->bridge->start_type != bst_lazy){
					തിരികെ 2;
				}
			}
		}
		എങ്കിൽ(context->bridge && context->bridge->clean_start_local == true){
			തിരികെ 2;
		}
	}

	എങ്കിൽ(net__is_connected(context)){
		എങ്കിൽ(db__ready_for_flight(context, mosq_md_out, qos)){
			തിരഞ്ഞെടുപ്പ്(qos){
				അവസ്ഥ 0:
					state = mosq_ms_publish_qos0;
					നിർത്തുക;
				അവസ്ഥ 1:
					state = mosq_ms_publish_qos1;
					നിർത്തുക;
				അവസ്ഥ 2:
					state = mosq_ms_publish_qos2;
					നിർത്തുക;
			}
		}അല്ലെങ്കിൽ എങ്കിൽ(qos != 0 && db__ready_for_queue(context, qos, സന്ദേശം_data)){
			state = mosq_ms_queued;
			rc = 2;
		}അല്ലെങ്കിൽ{
			/* Dropping message due to full queue. */
			എങ്കിൽ(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(ശൂന്യ, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			തിരികെ 2;
		}
	}അല്ലെങ്കിൽ{
		എങ്കിൽ(db__ready_for_queue(context, qos, സന്ദേശം_data)){
			state = mosq_ms_queued;
		}അല്ലെങ്കിൽ{
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			എങ്കിൽ(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(ശൂന്യ, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			തിരികെ 2;
		}
	}
	പ്രതിജ്ഞാ(state != mosq_ms_invalid);

#ifdef WITH_PERSISTENCE
	എങ്കിൽ(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#endif

	client_സന്ദേശം = mosquitto_malloc(വലുപ്പം(ഘടന mosquitto_client_സന്ദേശം));
	എങ്കിൽ(!client_സന്ദേശം){
		തിരികെ MOSQ_ERR_NOMEM;
	}
	client_സന്ദേശം->prev = ശൂന്യ;
	client_സന്ദേശം->next = ശൂന്യ;
	എങ്കിൽ(cmsg_id){
		client_സന്ദേശം->data.cmsg_id = cmsg_id;
	}അല്ലെങ്കിൽ{
		client_സന്ദേശം->data.cmsg_id = ++context->last_cmsg_id;
	}
	client_സന്ദേശം->base_സന്ദേശം = base_സന്ദേശം;
	db_സന്ദേശം_store_ref_inc(client_സന്ദേശം->base_സന്ദേശം);
	client_സന്ദേശം->data.mid = mid;
	client_സന്ദേശം->data.direction = mosq_md_out;
	client_സന്ദേശം->data.state = (uint8_t)state;
	client_സന്ദേശം->data.dup = false;
	എങ്കിൽ(qos > context->max_qos){
		client_സന്ദേശം->data.qos = context->max_qos;
	}അല്ലെങ്കിൽ{
		client_സന്ദേശം->data.qos = qos;
	}
	client_സന്ദേശം->data.retain = retain;
	client_സന്ദേശം->data.subscription_identifier = subscription_identifier;

	എങ്കിൽ(state == mosq_ms_queued){
		DL_APPEND(സന്ദേശം_data->queued, client_സന്ദേശം);
		db_സന്ദേശം_add_to_queued_stats(സന്ദേശം_data, client_സന്ദേശം);
	}അല്ലെങ്കിൽ{
		DL_APPEND(സന്ദേശം_data->inflight, client_സന്ദേശം);
		db_സന്ദേശം_add_to_inflight_stats(സന്ദേശം_data, client_സന്ദേശം);
	}

	എങ്കിൽ(persist && context->is_persisted){
		plugin_persist_handle_base_സന്ദേശം_add(client_സന്ദേശം->base_സന്ദേശം);
		plugin_persist_handle_client_സന്ദേശം_add(context, client_സന്ദേശം);
	}

	എങ്കിൽ(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(base_സന്ദേശം->dest_ids, വലുപ്പം(അക്ഷരം *)*(size_t)(base_സന്ദേശം->dest_id_എണ്ണം+1));
		എങ്കിൽ(dest_ids){
			base_സന്ദേശം->dest_ids = dest_ids;
			base_സന്ദേശം->dest_id_എണ്ണം++;
			base_സന്ദേശം->dest_ids[base_സന്ദേശം->dest_id_എണ്ണം-1] = mosquitto_strdup(context->id);
			എങ്കിൽ(!base_സന്ദേശം->dest_ids[base_സന്ദേശം->dest_id_എണ്ണം-1]){
				തിരികെ MOSQ_ERR_NOMEM;
			}
		}അല്ലെങ്കിൽ{
			തിരികെ MOSQ_ERR_NOMEM;
		}
	}
#ifdef WITH_BRIDGE
	എങ്കിൽ(context->bridge && context->bridge->start_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_out.inflight_എണ്ണം + context->msgs_out.queued_എണ്ണം >= context->bridge->threshold){

		context->bridge->lazy_reconnect = true;
	}
#endif

	എങ്കിൽ(client_സന്ദേശം->data.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	എങ്കിൽ(update){
		rc = db__message_write_inflight_out_latest(context);
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
		rc = db__message_write_queued_out(context);
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
	}

	തിരികെ rc;
}


സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ db__message_update_outgoing_state(ഘടന mosquitto *context, ഘടന mosquitto_client_സന്ദേശം *head,
		uint16_t mid, എണ്ണൽ mosquitto_സന്ദേശം_state state, പൂർണ്ണസംഖ്യ qos, bool persist)
{
	ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം;

	DL_FOREACH(head, client_സന്ദേശം){
		എങ്കിൽ(client_സന്ദേശം->data.mid == mid){
			എങ്കിൽ(client_സന്ദേശം->data.qos != qos){
				log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_സന്ദേശം->data.qos, qos);
				തിരികെ MOSQ_ERR_PROTOCOL;
			}
			client_സന്ദേശം->data.state = (uint8_t)state;
			എങ്കിൽ(persist){
				plugin_persist_handle_client_സന്ദേശം_update(context, client_സന്ദേശം);
			}
			തിരികെ MOSQ_ERR_SUCCESS;
		}
	}
	തിരികെ MOSQ_ERR_NOT_FOUND;
}


പൂർണ്ണസംഖ്യ db__message_update_outgoing(ഘടന mosquitto *context, uint16_t mid, എണ്ണൽ mosquitto_സന്ദേശം_state state, പൂർണ്ണസംഖ്യ qos, bool persist)
{
	പൂർണ്ണസംഖ്യ rc;

	rc = db__message_update_outgoing_state(context, context->msgs_out.inflight, mid, state, qos, persist);
	എങ്കിൽ(!persist && rc == MOSQ_ERR_NOT_FOUND){
		rc = db__message_update_outgoing_state(context, context->msgs_out.queued, mid, state, qos, persist);
	}
	തിരികെ rc;
}


സ്ഥിതം ശൂന്യം db__messages_delete_list(ഘടന mosquitto_client_സന്ദേശം **head)
{
	ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം, *tmp;

	DL_FOREACH_SAFE(*head, client_സന്ദേശം, tmp){
		DL_DELETE(*head, client_സന്ദേശം);
		db_സന്ദേശം_store_ref_dec(&client_സന്ദേശം->base_സന്ദേശം);
		mosquitto_FREE(client_സന്ദേശം);
	}
	*head = ശൂന്യ;
}


പൂർണ്ണസംഖ്യ db__messages_delete_incoming(ഘടന mosquitto *context)
{
	എങ്കിൽ(!context){
		തിരികെ MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_in.inflight);
	db__messages_delete_list(&context->msgs_in.queued);
	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_എണ്ണം = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_എണ്ണം = 0;
	context->msgs_in.queued_count12 = 0;

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ db__messages_delete_outgoing(ഘടന mosquitto *context)
{
	എങ്കിൽ(!context){
		തിരികെ MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_out.inflight);
	db__messages_delete_list(&context->msgs_out.queued);
	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_എണ്ണം = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_എണ്ണം = 0;
	context->msgs_out.queued_count12 = 0;

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ db__messages_delete(ഘടന mosquitto *context, bool force_free)
{
	എങ്കിൽ(!context){
		തിരികെ MOSQ_ERR_INVAL;
	}

	എങ്കിൽ(force_free || context->clean_start || (context->bridge && context->bridge->clean_start)){
		db__messages_delete_incoming(context);
	}

	എങ്കിൽ(force_free || (context->bridge && context->bridge->clean_start_local)
			|| (context->bridge == ശൂന്യ && context->clean_start)){

		db__messages_delete_outgoing(context);
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ db__messages_easy_queue(ഘടന mosquitto *context, സ്ഥിരം അക്ഷരം *topic, uint8_t qos, uint32_t payloadlen, സ്ഥിരം ശൂന്യം *payload, പൂർണ്ണസംഖ്യ retain, uint32_t message_expiry_interval, mosquitto_property **properties)
{
	ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം;
	സ്ഥിരം അക്ഷരം *source_id;
	എണ്ണൽ mosquitto_സന്ദേശം_origin origin;

	എങ്കിൽ(!topic){
		തിരികെ MOSQ_ERR_INVAL;
	}

	base_സന്ദേശം = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto_base_സന്ദേശം));
	എങ്കിൽ(base_സന്ദേശം == ശൂന്യ){
		തിരികെ MOSQ_ERR_NOMEM;
	}

	base_സന്ദേശം->data.topic = mosquitto_strdup(topic);
	എങ്കിൽ(base_സന്ദേശം->data.topic == ശൂന്യ){
		db_സന്ദേശം_store_free(base_സന്ദേശം);
		തിരികെ MOSQ_ERR_INVAL;
	}

	base_സന്ദേശം->data.qos = qos;
	എങ്കിൽ(db.config->retain_available == false){
		base_സന്ദേശം->data.retain = 0;
	}അല്ലെങ്കിൽ{
		base_സന്ദേശം->data.retain = retain;
	}

	base_സന്ദേശം->data.payloadlen = payloadlen;
	എങ്കിൽ(payloadlen > 0){
		base_സന്ദേശം->data.payload = mosquitto_malloc(base_സന്ദേശം->data.payloadlen+1);
		എങ്കിൽ(base_സന്ദേശം->data.payload == ശൂന്യ){
			db_സന്ദേശം_store_free(base_സന്ദേശം);
			തിരികെ MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_സന്ദേശം->data.payload)[base_സന്ദേശം->data.payloadlen] = 0;
		memcpy(base_സന്ദേശം->data.payload, payload, base_സന്ദേശം->data.payloadlen);
	}

	എങ്കിൽ(context && context->id){
		source_id = context->id;
	}അല്ലെങ്കിൽ{
		source_id = "";
	}
	എങ്കിൽ(properties){
		base_സന്ദേശം->data.properties = *properties;
		*properties = ശൂന്യ;
	}

	എങ്കിൽ(context){
		origin = mosq_mo_client;
	}അല്ലെങ്കിൽ{
		origin = mosq_mo_broker;
	}
	എങ്കിൽ(db__message_store(context, base_സന്ദേശം, &message_expiry_interval, origin)){
		തിരികെ 1;
	}

	തിരികെ sub__messages_queue(source_id, base_സന്ദേശം->data.qos, &base_സന്ദേശം);
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
uint64_t db_new_സന്ദേശം_id(ശൂന്യം)
{
#ifdef WIN32
	FILETIME ftime;
	uint64_t ftime64;
#അല്ലെങ്കിൽ
	ഘടന timespec ts;
#endif
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	long nsec;

	id = db.കണ്ണി_id_shifted; /* Top 10-bits */

#ifdef WIN32
	GetSystemTimePreciseAsFileTime(&ftime);
	ftime64 = (((uint64_t)ftime.dw_മുകളിലെ_Date_Time)<<32) + ftime.dw_താഴെയുള്ള_Date_Time;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (long)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#അല്ലെങ്കിൽ
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#endif
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	എങ്കിൽ(id <= db.last_db_id){
		id = db.last_db_id + 1;
	}
	db.last_db_id = id;

	തിരികെ id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
പൂർണ്ണസംഖ്യ db__message_store(സ്ഥിരം ഘടന mosquitto *source, ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം, uint32_t *message_expiry_interval, എണ്ണൽ mosquitto_സന്ദേശം_origin origin)
{
	പൂർണ്ണസംഖ്യ rc;

	പ്രതിജ്ഞാ(base_സന്ദേശം);

	എങ്കിൽ(source && source->id){
		base_സന്ദേശം->data.source_id = mosquitto_strdup(source->id);
	}അല്ലെങ്കിൽ{
		base_സന്ദേശം->data.source_id = mosquitto_strdup("");
	}
	എങ്കിൽ(!base_സന്ദേശം->data.source_id){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_സന്ദേശം_store_free(base_സന്ദേശം);
		തിരികെ MOSQ_ERR_NOMEM;
	}

	എങ്കിൽ(source && source->username){
		base_സന്ദേശം->data.source_username = mosquitto_strdup(source->username);
		എങ്കിൽ(!base_സന്ദേശം->data.source_username){
			db_സന്ദേശം_store_free(base_സന്ദേശം);
			തിരികെ MOSQ_ERR_NOMEM;
		}
	}
	എങ്കിൽ(source){
		base_സന്ദേശം->source_listener = source->listener;
	}
	base_സന്ദേശം->origin = origin;
	എങ്കിൽ(message_expiry_interval){
		എങ്കിൽ(*message_expiry_interval > 0 && *message_expiry_interval != സന്ദേശം_EXPIRY_INFINITE){
			base_സന്ദേശം->data.expiry_time = db.now_real_s + *message_expiry_interval;
		}അല്ലെങ്കിൽ{
			base_സന്ദേശം->data.expiry_time = 0;
		}
	}

	base_സന്ദേശം->dest_ids = ശൂന്യ;
	base_സന്ദേശം->dest_id_എണ്ണം = 0;
	db.സന്ദേശം_store_എണ്ണം++;
	db.സന്ദേശം_store_bytes += base_സന്ദേശം->data.payloadlen;

	എങ്കിൽ(!base_സന്ദേശം->data.store_id){
		base_സന്ദേശം->data.store_id = db_new_സന്ദേശം_id();
	}

	rc = db_സന്ദേശം_store_add(base_സന്ദേശം);
	എങ്കിൽ(rc){
		db_സന്ദേശം_store_free(base_സന്ദേശം);
		തിരികെ rc;
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ db__message_store_find(ഘടന mosquitto *context, uint16_t mid, ഘടന mosquitto_client_സന്ദേശം **client_സന്ദേശം)
{
	ഘടന mosquitto_client_സന്ദേശം *cmsg;

	*client_സന്ദേശം = ശൂന്യ;

	എങ്കിൽ(!context){
		തിരികെ MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_in.inflight, cmsg){
		എങ്കിൽ(cmsg->base_സന്ദേശം->data.source_mid == mid){
			*client_സന്ദേശം = cmsg;
			തിരികെ MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_in.queued, cmsg){
		എങ്കിൽ(cmsg->base_സന്ദേശം->data.source_mid == mid){
			*client_സന്ദേശം = cmsg;
			തിരികെ MOSQ_ERR_SUCCESS;
		}
	}

	തിരികെ 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
സ്ഥിതം പൂർണ്ണസംഖ്യ db__message_reconnect_reset_outgoing(ഘടന mosquitto *context)
{
	ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം, *tmp;

	context->msgs_out.inflight_bytes = 0;
	context->msgs_out.inflight_bytes12 = 0;
	context->msgs_out.inflight_എണ്ണം = 0;
	context->msgs_out.inflight_count12 = 0;
	context->msgs_out.queued_bytes = 0;
	context->msgs_out.queued_bytes12 = 0;
	context->msgs_out.queued_എണ്ണം = 0;
	context->msgs_out.queued_count12 = 0;
	context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_സന്ദേശം, tmp){
		db_സന്ദേശം_add_to_inflight_stats(&context->msgs_out, client_സന്ദേശം);
		എങ്കിൽ(client_സന്ദേശം->data.qos > 0){
			util__decrement_send_quota(context);
		}

		തിരഞ്ഞെടുപ്പ്(client_സന്ദേശം->data.qos){
			അവസ്ഥ 0:
				client_സന്ദേശം->data.state = mosq_ms_publish_qos0;
				നിർത്തുക;
			അവസ്ഥ 1:
				client_സന്ദേശം->data.state = mosq_ms_publish_qos1;
				നിർത്തുക;
			അവസ്ഥ 2:
				എങ്കിൽ(client_സന്ദേശം->data.state == mosq_ms_wait_for_pubcomp){
					client_സന്ദേശം->data.state = mosq_ms_resend_pubrel;
				}അല്ലെങ്കിൽ{
					client_സന്ദേശം->data.state = mosq_ms_publish_qos2;
				}
				നിർത്തുക;
		}
		plugin_persist_handle_client_സന്ദേശം_update(context, client_സന്ദേശം);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_out.queued, client_സന്ദേശം, tmp){
		db_സന്ദേശം_add_to_queued_stats(&context->msgs_out, client_സന്ദേശം);
	}
	db__fill_inflight_out_from_queue(context);

	തിരികെ MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
സ്ഥിതം പൂർണ്ണസംഖ്യ db__message_reconnect_reset_incoming(ഘടന mosquitto *context)
{
	ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം, *tmp;

	context->msgs_in.inflight_bytes = 0;
	context->msgs_in.inflight_bytes12 = 0;
	context->msgs_in.inflight_എണ്ണം = 0;
	context->msgs_in.inflight_count12 = 0;
	context->msgs_in.queued_bytes = 0;
	context->msgs_in.queued_bytes12 = 0;
	context->msgs_in.queued_എണ്ണം = 0;
	context->msgs_in.queued_count12 = 0;
	context->msgs_in.inflight_quota = context->msgs_in.inflight_maximum;

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_സന്ദേശം, tmp){
		db_സന്ദേശം_add_to_inflight_stats(&context->msgs_in, client_സന്ദേശം);
		എങ്കിൽ(client_സന്ദേശം->data.qos > 0){
			util__decrement_receive_quota(context);
		}

		എങ്കിൽ(client_സന്ദേശം->data.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db__message_remove_inflight(context, &context->msgs_in, client_സന്ദേശം);
		}അല്ലെങ്കിൽ{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_സന്ദേശം->data.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_in.queued, client_സന്ദേശം, tmp){
		client_സന്ദേശം->data.dup = 0;
		db_സന്ദേശം_add_to_queued_stats(&context->msgs_in, client_സന്ദേശം);
		എങ്കിൽ(db__ready_for_flight(context, mosq_md_in, client_സന്ദേശം->data.qos)){
			തിരഞ്ഞെടുപ്പ്(client_സന്ദേശം->data.qos){
				അവസ്ഥ 0:
					client_സന്ദേശം->data.state = mosq_ms_publish_qos0;
					നിർത്തുക;
				അവസ്ഥ 1:
					client_സന്ദേശം->data.state = mosq_ms_publish_qos1;
					നിർത്തുക;
				അവസ്ഥ 2:
					client_സന്ദേശം->data.state = mosq_ms_publish_qos2;
					നിർത്തുക;
			}
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_സന്ദേശം_update(context, client_സന്ദേശം);
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ db__message_reconnect_reset(ഘടന mosquitto *context)
{
	പൂർണ്ണസംഖ്യ rc;

	rc = db__message_reconnect_reset_outgoing(context);
	എങ്കിൽ(rc){
		തിരികെ rc;
	}
	തിരികെ db__message_reconnect_reset_incoming(context);
}


പൂർണ്ണസംഖ്യ db__message_remove_incoming(ഘടന mosquitto *context, uint16_t mid)
{
	ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം, *tmp;

	എങ്കിൽ(!context){
		തിരികെ MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_സന്ദേശം, tmp){
		എങ്കിൽ(client_സന്ദേശം->data.mid == mid){
			എങ്കിൽ(client_സന്ദേശം->base_സന്ദേശം->data.qos != 2){
				log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_സന്ദേശം->base_സന്ദേശം->data.qos);
				തിരികെ MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_in, client_സന്ദേശം);
			തിരികെ MOSQ_ERR_SUCCESS;
		}
	}

	തിരികെ MOSQ_ERR_NOT_FOUND;
}


പൂർണ്ണസംഖ്യ db__message_release_incoming(ഘടന mosquitto *context, uint16_t mid)
{
	ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം, *tmp;
	bool deleted = false;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(!context){
		തിരികെ MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_സന്ദേശം, tmp){
		എങ്കിൽ(client_സന്ദേശം->data.mid == mid){
			എങ്കിൽ(client_സന്ദേശം->base_സന്ദേശം->data.qos != 2){
				log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_സന്ദേശം->base_സന്ദേശം->data.qos);
				തിരികെ MOSQ_ERR_PROTOCOL;
			}
			സ്ഥിരം അക്ഷരം *topic = client_സന്ദേശം->base_സന്ദേശം->data.topic;
			സ്ഥിരം അക്ഷരം *source_id = client_സന്ദേശം->base_സന്ദേശം->data.source_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			എങ്കിൽ(topic == ശൂന്യ){
				db__message_remove_inflight(context, &context->msgs_in, client_സന്ദേശം);
				deleted = true;
			}അല്ലെങ്കിൽ{
				rc = sub__messages_queue(source_id, 2, &client_സന്ദേശം->base_സന്ദേശം);
				എങ്കിൽ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db__message_remove_inflight(context, &context->msgs_in, client_സന്ദേശം);
					deleted = true;
				}അല്ലെങ്കിൽ{
					തിരികെ 1;
				}
			}
		}
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_സന്ദേശം, tmp){
		എങ്കിൽ(db__ready_for_flight(context, mosq_md_in, client_സന്ദേശം->data.qos)){
			നിർത്തുക;
		}

		എങ്കിൽ(client_സന്ദേശം->data.qos == 2){
			send__pubrec(context, client_സന്ദേശം->data.mid, 0, ശൂന്യ);
			client_സന്ദേശം->data.state = mosq_ms_wait_for_pubrel;
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_സന്ദേശം_update(context, client_സന്ദേശം);
		}
	}
	എങ്കിൽ(deleted){
		തിരികെ MOSQ_ERR_SUCCESS;
	}അല്ലെങ്കിൽ{
		തിരികെ MOSQ_ERR_NOT_FOUND;
	}
}


ശൂന്യം db__expire_all_messages(ഘടന mosquitto *context)
{
	ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_സന്ദേശം, tmp){
		എങ്കിൽ(client_സന്ദേശം->base_സന്ദേശം->data.expiry_time && db.now_real_s > client_സന്ദേശം->base_സന്ദേശം->data.expiry_time){
			എങ്കിൽ(client_സന്ദേശം->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_സന്ദേശം);
		}
	}
	db__fill_inflight_out_from_queue(context);
	DL_FOREACH_SAFE(context->msgs_out.queued, client_സന്ദേശം, tmp){
		എങ്കിൽ(client_സന്ദേശം->base_സന്ദേശം->data.expiry_time && db.now_real_s > client_സന്ദേശം->base_സന്ദേശം->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_സന്ദേശം);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.inflight, client_സന്ദേശം, tmp){
		എങ്കിൽ(client_സന്ദേശം->base_സന്ദേശം->data.expiry_time && db.now_real_s > client_സന്ദേശം->base_സന്ദേശം->data.expiry_time){
			എങ്കിൽ(client_സന്ദേശം->data.qos > 0){
				util__increment_receive_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_in, client_സന്ദേശം);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.queued, client_സന്ദേശം, tmp){
		എങ്കിൽ(client_സന്ദേശം->base_സന്ദേശം->data.expiry_time && db.now_real_s > client_സന്ദേശം->base_സന്ദേശം->data.expiry_time){
			db__message_remove_queued(context, &context->msgs_in, client_സന്ദേശം);
		}
	}
}


സ്ഥിതം ശൂന്യം db__client_messages_check_acl(ഘടന mosquitto *context, ഘടന mosquitto_സന്ദേശം_data *സന്ദേശം_data, ഘടന mosquitto_client_സന്ദേശം **head,
		ശൂന്യം (*decrement_stats_fn)(ഘടന mosquitto_സന്ദേശം_data *സന്ദേശം_data, ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം))
{
	ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം, *tmp;
	ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം;
	പൂർണ്ണസംഖ്യ access;

	DL_FOREACH_SAFE((*head), client_സന്ദേശം, tmp){
		base_സന്ദേശം = client_സന്ദേശം->base_സന്ദേശം;
		എങ്കിൽ(client_സന്ദേശം->data.direction == mosq_md_out){
			access = MOSQ_ACL_READ;
		}അല്ലെങ്കിൽ{
			access = MOSQ_ACL_WRITE;
		}
		എങ്കിൽ(mosquitto_acl_check(context, base_സന്ദേശം->data.topic,
				base_സന്ദേശം->data.payloadlen, base_സന്ദേശം->data.payload,
				base_സന്ദേശം->data.qos, base_സന്ദേശം->data.retain,
				base_സന്ദേശം->data.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*head), client_സന്ദേശം);
			decrement_stats_fn(സന്ദേശം_data, client_സന്ദേശം);
			plugin_persist_handle_client_സന്ദേശം_delete(context, client_സന്ദേശം);
			db_സന്ദേശം_store_ref_dec(&client_സന്ദേശം->base_സന്ദേശം);
			mosquitto_FREE(client_സന്ദേശം);
		}
	}
}


ശൂന്യം db__check_acl_of_all_messages(ഘടന mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.inflight, &db_സന്ദേശം_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.queued, &db_സന്ദേശം_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.inflight, &db_സന്ദേശം_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.queued, &db_സന്ദേശം_remove_from_queued_stats);
}


സ്ഥിതം പൂർണ്ണസംഖ്യ db__message_write_inflight_out_single(ഘടന mosquitto *context, ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം)
{
	ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം;
	mosquitto_property *base_സന്ദേശം_props = ശൂന്യ;
	പൂർണ്ണസംഖ്യ rc;
	uint16_t mid;
	പൂർണ്ണസംഖ്യ retries;
	പൂർണ്ണസംഖ്യ retain;
	സ്ഥിരം അക്ഷരം *topic;
	uint8_t qos;
	uint32_t payloadlen;
	സ്ഥിരം ശൂന്യം *payload;
	uint32_t expiry_interval;
	uint32_t subscription_id;

	base_സന്ദേശം = client_സന്ദേശം->base_സന്ദേശം;

	expiry_interval = 0;
	എങ്കിൽ(base_സന്ദേശം->data.expiry_time){
		എങ്കിൽ(db.now_real_s > base_സന്ദേശം->data.expiry_time){
			/* Message is expired, must not send. */
			എങ്കിൽ(client_സന്ദേശം->data.direction == mosq_md_out && client_സന്ദേശം->data.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_സന്ദേശം);
			db__fill_inflight_out_from_queue(context);
			തിരികെ MOSQ_ERR_SUCCESS;
		}അല്ലെങ്കിൽ{
			expiry_interval = (uint32_t)(base_സന്ദേശം->data.expiry_time - db.now_real_s);
		}
	}
	mid = client_സന്ദേശം->data.mid;
	retries = client_സന്ദേശം->data.dup;
	retain = client_സന്ദേശം->data.retain;
	topic = base_സന്ദേശം->data.topic;
	qos = (uint8_t)client_സന്ദേശം->data.qos;
	payloadlen = base_സന്ദേശം->data.payloadlen;
	payload = base_സന്ദേശം->data.payload;
	subscription_id = client_സന്ദേശം->data.subscription_identifier;
	base_സന്ദേശം_props = base_സന്ദേശം->data.properties;

	തിരഞ്ഞെടുപ്പ്(client_സന്ദേശം->data.state){
		അവസ്ഥ mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_സന്ദേശം_props, expiry_interval);
			എങ്കിൽ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_സന്ദേശം);
			}അല്ലെങ്കിൽ{
				തിരികെ rc;
			}
			നിർത്തുക;

		അവസ്ഥ mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_സന്ദേശം_props, expiry_interval);
			എങ്കിൽ(rc == MOSQ_ERR_SUCCESS){
				client_സന്ദേശം->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_സന്ദേശം->data.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_സന്ദേശം_update(context, client_സന്ദേശം);
			}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_സന്ദേശം);
			}അല്ലെങ്കിൽ{
				തിരികെ rc;
			}
			നിർത്തുക;

		അവസ്ഥ mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_സന്ദേശം_props, expiry_interval);
			എങ്കിൽ(rc == MOSQ_ERR_SUCCESS){
				client_സന്ദേശം->data.dup = 1; /* Any retry attempts are a duplicate. */
				client_സന്ദേശം->data.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_സന്ദേശം_update(context, client_സന്ദേശം);
			}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_സന്ദേശം);
			}അല്ലെങ്കിൽ{
				തിരികെ rc;
			}
			നിർത്തുക;

		അവസ്ഥ mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, ശൂന്യ);
			എങ്കിൽ(!rc){
				client_സന്ദേശം->data.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_സന്ദേശം_update(context, client_സന്ദേശം);
			}അല്ലെങ്കിൽ{
				തിരികെ rc;
			}
			നിർത്തുക;

		അവസ്ഥ mosq_ms_invalid:
		അവസ്ഥ mosq_ms_send_pubrec:
		അവസ്ഥ mosq_ms_resend_pubcomp:
		അവസ്ഥ mosq_ms_wait_for_puback:
		അവസ്ഥ mosq_ms_wait_for_pubrec:
		അവസ്ഥ mosq_ms_wait_for_pubrel:
		അവസ്ഥ mosq_ms_wait_for_pubcomp:
		അവസ്ഥ mosq_ms_queued:
			നിർത്തുക;
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ db__message_write_inflight_out_all(ഘടന mosquitto *context)
{
	ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം, *tmp;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(context->state != mosq_cs_active || !net__is_connected(context)){
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_സന്ദേശം, tmp){
		rc = db__message_write_inflight_out_single(context, client_സന്ദേശം);
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ db__message_write_inflight_out_latest(ഘടന mosquitto *context)
{
	ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം, *next;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(context->state != mosq_cs_active
			|| !net__is_connected(context)
			|| context->msgs_out.inflight == ശൂന്യ){

		തിരികെ MOSQ_ERR_SUCCESS;
	}

	എങ്കിൽ(context->msgs_out.inflight->prev == context->msgs_out.inflight){
		/* Only one message */
		തിരികെ db__message_write_inflight_out_single(context, context->msgs_out.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_സന്ദേശം = context->msgs_out.inflight->prev;
	ആകുമ്പോൾ(client_സന്ദേശം != context->msgs_out.inflight &&
			(client_സന്ദേശം->data.state == mosq_ms_publish_qos0
			|| client_സന്ദേശം->data.state == mosq_ms_publish_qos1
			|| client_സന്ദേശം->data.state == mosq_ms_publish_qos2)){

		client_സന്ദേശം = client_സന്ദേശം->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	എങ്കിൽ(client_സന്ദേശം != context->msgs_out.inflight){
		client_സന്ദേശം = client_സന്ദേശം->next;
	}

	ആകുമ്പോൾ(client_സന്ദേശം){
		next = client_സന്ദേശം->next;
		rc = db__message_write_inflight_out_single(context, client_സന്ദേശം);
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
		client_സന്ദേശം = next;
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ db__message_write_queued_in(ഘടന mosquitto *context)
{
	ഘടന mosquitto_client_സന്ദേശം *client_സന്ദേശം, *tmp;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(context->state != mosq_cs_active){
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_സന്ദേശം, tmp){
		എങ്കിൽ(context->msgs_in.inflight_maximum != 0 && context->msgs_in.inflight_quota == 0){
			നിർത്തുക;
		}

		എങ്കിൽ(client_സന്ദേശം->data.qos == 2){
			client_സന്ദേശം->data.state = mosq_ms_send_pubrec;
			db__message_dequeue_first(context, &context->msgs_in);
			rc = send__pubrec(context, client_സന്ദേശം->data.mid, 0, ശൂന്യ);
			എങ്കിൽ(!rc){
				client_സന്ദേശം->data.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_സന്ദേശം_update(context, client_സന്ദേശം);
			}അല്ലെങ്കിൽ{
				plugin_persist_handle_client_സന്ദേശം_update(context, client_സന്ദേശം);
				തിരികെ rc;
			}
		}
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ db__message_write_queued_out(ഘടന mosquitto *context)
{
	എങ്കിൽ(context->state != mosq_cs_active){
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	db__fill_inflight_out_from_queue(context);

	തിരികെ MOSQ_ERR_SUCCESS;
}
