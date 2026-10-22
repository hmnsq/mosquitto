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

#समावेश "config.h"

#समावेश <assert.h>
#समावेश <stdio.h>
#समावेश <utlist.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "send_mosq.h"
#समावेश "sys_tree.h"
#समावेश "util_mosq.h"


/**
 * Is this context ready to take more in flight messages right now?
 * @param context the client context of interest
 * @param qos qos for the packet of interest
 * @return true if more in flight are allowed.
 */
bool db__ready_for_flight(काष्ठा mosquitto *context, क्रमागत mosquitto_संदेश_direction dir, पूर्णांक qos)
{
	काष्ठा mosquitto_संदेश_तथ्य *msgs;
	bool valid_bytes;
	bool valid_गिनती;

	अगर(dir == mosq_md_out){
		msgs = &context->msgs_out;
	}अन्यथा{
		msgs = &context->msgs_in;
	}

	अगर(msgs->inflight_maximum == 0 && db.config->max_inflight_bytes == 0){
		वापस true;
	}

	अगर(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		अगर(db.config->max_queued_messages == 0 && db.config->max_inflight_bytes == 0){
			वापस true;
		}
		valid_bytes = ((msgs->inflight_bytes - (ssize_t)db.config->max_inflight_bytes) < (ssize_t)db.config->max_queued_bytes);
		अगर(dir == mosq_md_out){
			valid_गिनती = context->out_packet_गिनती < db.config->max_queued_messages;
		}अन्यथा{
			valid_गिनती = msgs->inflight_गिनती - msgs->inflight_maximum < db.config->max_queued_messages;
		}

		अगर(db.config->max_queued_messages == 0){
			वापस valid_bytes;
		}
		अगर(db.config->max_queued_bytes == 0){
			वापस valid_गिनती;
		}
	}अन्यथा{
		valid_bytes = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_bytes;
		valid_गिनती = msgs->inflight_quota > 0;

		अगर(msgs->inflight_maximum == 0){
			वापस valid_bytes;
		}
		अगर(db.config->max_inflight_bytes == 0){
			वापस valid_गिनती;
		}
	}

	वापस valid_bytes && valid_गिनती;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db__ready_for_queue(काष्ठा mosquitto *context, पूर्णांक qos, काष्ठा mosquitto_संदेश_तथ्य *संदेश_तथ्य)
{
	पूर्णांक source_गिनती;
	पूर्णांक adjust_गिनती;
	दीर्घ source_bytes;
	ssize_t adjust_bytes = (ssize_t)db.config->max_inflight_bytes;
	bool valid_bytes;
	bool valid_गिनती;

	अगर(db.config->max_queued_messages == 0 && db.config->max_queued_bytes == 0){
		वापस true;
	}

	अगर(qos == 0 && db.config->queue_qos0_messages == false){
		वापस false; /* This case is handled in db__ready_for_flight() */
	}अन्यथा{
		source_bytes = (ssize_t)संदेश_तथ्य->queued_bytes12;
		source_गिनती = संदेश_तथ्य->queued_count12;
	}
	adjust_गिनती = संदेश_तथ्य->inflight_maximum;

	/* nothing in flight for offline clients */
	अगर(!net__is_connected(context)){
		adjust_bytes = 0;
		adjust_गिनती = 0;
	}

	valid_bytes = (source_bytes - (ssize_t)adjust_bytes) < (ssize_t)db.config->max_queued_bytes;
	valid_गिनती = source_गिनती - adjust_गिनती < db.config->max_queued_messages;

	अगर(db.config->max_queued_bytes == 0){
		वापस valid_गिनती;
	}
	अगर(db.config->max_queued_messages == 0){
		वापस valid_bytes;
	}

	वापस valid_bytes && valid_गिनती;
}


व्योम db_संदेश_add_to_inflight_stats(काष्ठा mosquitto_संदेश_तथ्य *संदेश_तथ्य, काष्ठा mosquitto_client_संदेश *client_संदेश)
{
	संदेश_तथ्य->inflight_गिनती++;
	संदेश_तथ्य->inflight_bytes += client_संदेश->base_संदेश->तथ्य.payloadlen;
	अगर(client_संदेश->तथ्य.qos != 0){
		संदेश_तथ्य->inflight_count12++;
		संदेश_तथ्य->inflight_bytes12 += client_संदेश->base_संदेश->तथ्य.payloadlen;
	}
}


अटल व्योम db_संदेश_remove_from_inflight_stats(काष्ठा mosquitto_संदेश_तथ्य *संदेश_तथ्य, काष्ठा mosquitto_client_संदेश *client_संदेश)
{
	संदेश_तथ्य->inflight_गिनती--;
	संदेश_तथ्य->inflight_bytes -= client_संदेश->base_संदेश->तथ्य.payloadlen;
	अगर(client_संदेश->तथ्य.qos != 0){
		संदेश_तथ्य->inflight_count12--;
		संदेश_तथ्य->inflight_bytes12 -= client_संदेश->base_संदेश->तथ्य.payloadlen;
	}
}


व्योम db_संदेश_add_to_queued_stats(काष्ठा mosquitto_संदेश_तथ्य *संदेश_तथ्य, काष्ठा mosquitto_client_संदेश *client_संदेश)
{
	संदेश_तथ्य->queued_गिनती++;
	संदेश_तथ्य->queued_bytes += client_संदेश->base_संदेश->तथ्य.payloadlen;
	अगर(client_संदेश->तथ्य.qos != 0){
		संदेश_तथ्य->queued_count12++;
		संदेश_तथ्य->queued_bytes12 += client_संदेश->base_संदेश->तथ्य.payloadlen;
	}
}


अटल व्योम db_संदेश_remove_from_queued_stats(काष्ठा mosquitto_संदेश_तथ्य *संदेश_तथ्य, काष्ठा mosquitto_client_संदेश *client_संदेश)
{
	संदेश_तथ्य->queued_गिनती--;
	संदेश_तथ्य->queued_bytes -= client_संदेश->base_संदेश->तथ्य.payloadlen;
	अगर(client_संदेश->तथ्य.qos != 0){
		संदेश_तथ्य->queued_count12--;
		संदेश_तथ्य->queued_bytes12 -= client_संदेश->base_संदेश->तथ्य.payloadlen;
	}
}


पूर्णांक db__open(काष्ठा mosquitto__config *config)
{
	अगर(!config){
		वापस MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = शून्य;
	db.contexts_by_sock = शून्य;
	db.contexts_for_free = शून्य;
#अगर_घोषित WITH_BRIDGE
	db.bridges = शून्य;
	db.bridge_गिनती = 0;
#पूर्ण_अगर

	/* Initialize the hashtable */
	db.clientid_सूचकांक_hash = शून्य;

	db.normal_subs = शून्य;
	db.shared_subs = शून्य;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = शून्य;

#अगर_घोषित WITH_PERSISTENCE
	अगर(persist__restore()){
		वापस 1;
	}
#पूर्ण_अगर

	वापस MOSQ_ERR_SUCCESS;
}


अटल व्योम subhier_clean(काष्ठा mosquitto__subhier **subhier)
{
	काष्ठा mosquitto__subhier *peer, *subhier_tmp;
	काष्ठा mosquitto__subleaf *leaf, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		leaf = peer->subs;
		जबतक(leaf){
			nextleaf = leaf->अग्र;
			mosquitto_FREE(leaf);
			leaf = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


पूर्णांक db__close(व्योम)
{
	subhier_clean(&db.normal_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db_संदेश_store_clean();

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक db_संदेश_store_add(काष्ठा mosquitto_base_संदेश *base_संदेश)
{
	काष्ठा mosquitto_base_संदेश *found;
	अचिन्हित hashv;

	HASH_मूल्य(&base_संदेश->तथ्य.store_id, माप(base_संदेश->तथ्य.store_id), hashv);
	HASH_FIND_BYHASHVALUE(hh, db.संदेश_store, &base_संदेश->तथ्य.store_id, माप(base_संदेश->तथ्य.store_id), hashv, found);
	अगर(found == शून्य){
		HASH_ADD_KEYPTR_BYHASHVALUE(hh, db.संदेश_store, &base_संदेश->तथ्य.store_id, माप(base_संदेश->तथ्य.store_id), hashv, base_संदेश);
		वापस MOSQ_ERR_SUCCESS;
	}अन्यथा{
		वापस MOSQ_ERR_ALREADY_EXISTS;
	}
}


व्योम db_संदेश_store_free(काष्ठा mosquitto_base_संदेश *base_संदेश)
{
	mosquitto_FREE(base_संदेश->तथ्य.source_id);
	mosquitto_FREE(base_संदेश->तथ्य.source_username);
	अगर(base_संदेश->dest_ids){
		क्रम(पूर्णांक i=0; i<base_संदेश->dest_id_गिनती; i++){
			mosquitto_FREE(base_संदेश->dest_ids[i]);
		}
		mosquitto_FREE(base_संदेश->dest_ids);
	}
	mosquitto_FREE(base_संदेश->तथ्य.topic);
	mosquitto_property_free_all(&base_संदेश->तथ्य.properties);
	mosquitto_FREE(base_संदेश->तथ्य.payload);
	mosquitto_FREE(base_संदेश);
}


व्योम db_संदेश_store_remove(काष्ठा mosquitto_base_संदेश *base_संदेश, bool notify)
{
	अगर(base_संदेश == शून्य){
		वापस;
	}
	HASH_DELETE(hh, db.संदेश_store, base_संदेश);
	db.संदेश_store_गिनती--;
	db.संदेश_store_bytes -= base_संदेश->तथ्य.payloadlen;
	अगर(notify == true){
		plugin_persist_handle_base_संदेश_delete(base_संदेश);
	}
	db_संदेश_store_free(base_संदेश);
}


व्योम db_संदेश_store_clean(व्योम)
{
	काष्ठा mosquitto_base_संदेश *base_संदेश, *base_संदेश_tmp;

	HASH_ITER(hh, db.संदेश_store, base_संदेश, base_संदेश_tmp){
		db_संदेश_store_remove(base_संदेश, false);
	}
}


व्योम db_संदेश_store_ref_inc(काष्ठा mosquitto_base_संदेश *base_संदेश)
{
	base_संदेश->ref_गिनती++;
}


व्योम db_संदेश_store_ref_dec(काष्ठा mosquitto_base_संदेश **base_संदेश)
{
	(*base_संदेश)->ref_गिनती--;
	अगर((*base_संदेश)->ref_गिनती == 0){
		db_संदेश_store_remove(*base_संदेश, true);
		*base_संदेश = शून्य;
	}
}


व्योम db_संदेश_store_compact(व्योम)
{
	काष्ठा mosquitto_base_संदेश *base_संदेश, *base_संदेश_tmp;

	HASH_ITER(hh, db.संदेश_store, base_संदेश, base_संदेश_tmp){
		अगर(base_संदेश->ref_गिनती < 1){
			db_संदेश_store_remove(base_संदेश, true);
		}
	}
}


अटल व्योम db__message_remove_inflight(काष्ठा mosquitto *context, काष्ठा mosquitto_संदेश_तथ्य *संदेश_तथ्य, काष्ठा mosquitto_client_संदेश *item)
{
	अगर(!context || !संदेश_तथ्य || !item){
		वापस;
	}

	plugin_persist_handle_client_संदेश_delete(context, item);

	DL_DELETE(संदेश_तथ्य->inflight, item);
	अगर(item->base_संदेश){
		db_संदेश_remove_from_inflight_stats(संदेश_तथ्य, item);
		db_संदेश_store_ref_dec(&item->base_संदेश);
	}

	mosquitto_FREE(item);
}


अटल व्योम db__message_remove_queued(काष्ठा mosquitto *context, काष्ठा mosquitto_संदेश_तथ्य *संदेश_तथ्य, काष्ठा mosquitto_client_संदेश *item)
{
	अगर(!context || !संदेश_तथ्य || !item){
		वापस;
	}

	plugin_persist_handle_client_संदेश_delete(context, item);

	DL_DELETE(संदेश_तथ्य->queued, item);
	अगर(item->base_संदेश){
		db_संदेश_remove_from_queued_stats(संदेश_तथ्य, item);
		db_संदेश_store_ref_dec(&item->base_संदेश);
	}

	mosquitto_FREE(item);
}


अटल व्योम db__fill_inflight_out_from_queue(काष्ठा mosquitto *context)
{
	काष्ठा mosquitto_client_संदेश *client_संदेश, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.queued, client_संदेश, tmp){
		अगर(!db__ready_for_flight(context, mosq_md_out, client_संदेश->तथ्य.qos)){
			वापस;
		}
		चयन(client_संदेश->तथ्य.qos){
			हाल 0:
				client_संदेश->तथ्य.state = mosq_ms_publish_qos0;
				अवरोध;
			हाल 1:
				client_संदेश->तथ्य.state = mosq_ms_publish_qos1;
				अवरोध;
			हाल 2:
				client_संदेश->तथ्य.state = mosq_ms_publish_qos2;
				अवरोध;
		}
		अगर(client_संदेश->base_संदेश->तथ्य.expiry_time && db.now_real_s > client_संदेश->base_संदेश->तथ्य.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_संदेश);
			जारी;
		}
		plugin_persist_handle_client_संदेश_update(context, client_संदेश);
		db__message_dequeue_first(context, &context->msgs_out);
	}
}


व्योम db__message_dequeue_first(काष्ठा mosquitto *context, काष्ठा mosquitto_संदेश_तथ्य *संदेश_तथ्य)
{
	काष्ठा mosquitto_client_संदेश *client_संदेश;

	UNUSED(context);

	client_संदेश = संदेश_तथ्य->queued;
	DL_DELETE(संदेश_तथ्य->queued, client_संदेश);
	DL_APPEND(संदेश_तथ्य->inflight, client_संदेश);
	अगर(संदेश_तथ्य->inflight_quota > 0){
		संदेश_तथ्य->inflight_quota--;
	}

	db_संदेश_remove_from_queued_stats(संदेश_तथ्य, client_संदेश);
	db_संदेश_add_to_inflight_stats(संदेश_तथ्य, client_संदेश);
}


पूर्णांक db__message_delete_outgoing(काष्ठा mosquitto *context, uint16_t mid, क्रमागत mosquitto_संदेश_state expect_state, पूर्णांक qos)
{
	काष्ठा mosquitto_client_संदेश *client_संदेश, *tmp;
	bool deleted = false;

	अगर(!context){
		वापस MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_संदेश, tmp){
		अगर(client_संदेश->तथ्य.mid == mid){
			अगर(client_संदेश->तथ्य.qos != qos){
				log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_संदेश->तथ्य.qos, qos);
				वापस MOSQ_ERR_PROTOCOL;
			}अन्यथा अगर(qos == 2 && client_संदेश->तथ्य.state != expect_state && expect_state != mosq_ms_any){
				log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_संदेश->तथ्य.state, expect_state);
				वापस MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_out, client_संदेश);
			deleted = true;
			अवरोध;
		}
	}

	अगर(deleted == false){
		DL_FOREACH_SAFE(context->msgs_out.queued, client_संदेश, tmp){
			अगर(client_संदेश->तथ्य.mid == mid){
				अगर(client_संदेश->तथ्य.qos != qos){
					वापस MOSQ_ERR_PROTOCOL;
				}अन्यथा अगर(qos == 2 && client_संदेश->तथ्य.state != expect_state && expect_state != mosq_ms_any){
					वापस MOSQ_ERR_PROTOCOL;
				}
				db__message_remove_queued(context, &context->msgs_out, client_संदेश);
				अवरोध;
			}
		}
	}
	db__fill_inflight_out_from_queue(context);
#अगर_घोषित WITH_PERSISTENCE
	db.persistence_changes++;
#पूर्ण_अगर

	वापस db__message_write_inflight_out_latest(context);
}


/* Only for QoS 2 messages */
पूर्णांक db_message_जोड़ो_incoming(काष्ठा mosquitto *context, uint64_t cmsg_id, काष्ठा mosquitto_base_संदेश *base_संदेश, bool persist)
{
	काष्ठा mosquitto_client_संदेश *client_संदेश;
	काष्ठा mosquitto_संदेश_तथ्य *संदेश_तथ्य;
	क्रमागत mosquitto_संदेश_state state = mosq_ms_invalid;
	पूर्णांक rc = 0;

	निश्चित(base_संदेश);
	अगर(!context){
		वापस MOSQ_ERR_INVAL;
	}
	अगर(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		वापस MOSQ_ERR_SUCCESS;

	}
	संदेश_तथ्य = &context->msgs_in;

	अगर(db__ready_for_flight(context, mosq_md_in, base_संदेश->तथ्य.qos)){
		state = mosq_ms_wait_for_pubrel;
	}अन्यथा अगर(base_संदेश->तथ्य.qos != 0 && db__ready_for_queue(context, base_संदेश->तथ्य.qos, संदेश_तथ्य)){
		state = mosq_ms_queued;
		rc = 2;
	}अन्यथा{
		/* Dropping message due to full queue. */
		अगर(context->is_dropping == false){
			context->is_dropping = true;
			log__printf(शून्य, MOSQ_LOG_NOTICE,
					"Outgoing messages are being dropped for client %s.",
					context->id);
		}
		metrics__int_inc(mosq_गणक_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		वापस 2;
	}

	निश्चित(state != mosq_ms_invalid);

#अगर_घोषित WITH_PERSISTENCE
	अगर(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#पूर्ण_अगर

	client_संदेश = mosquitto_malloc(माप(काष्ठा mosquitto_client_संदेश));
	अगर(!client_संदेश){
		वापस MOSQ_ERR_NOMEM;
	}
	client_संदेश->पूर्व = शून्य;
	client_संदेश->अग्र = शून्य;
	अगर(cmsg_id){
		client_संदेश->तथ्य.cmsg_id = cmsg_id;
	}अन्यथा{
		client_संदेश->तथ्य.cmsg_id = ++context->last_cmsg_id;
	}
	client_संदेश->base_संदेश = base_संदेश;
	db_संदेश_store_ref_inc(client_संदेश->base_संदेश);
	client_संदेश->तथ्य.mid = base_संदेश->तथ्य.source_mid;
	client_संदेश->तथ्य.direction = mosq_md_in;
	client_संदेश->तथ्य.state = (uint8_t)state;
	client_संदेश->तथ्य.dup = false;
	अगर(base_संदेश->तथ्य.qos > context->max_qos){
		client_संदेश->तथ्य.qos = context->max_qos;
	}अन्यथा{
		client_संदेश->तथ्य.qos = base_संदेश->तथ्य.qos;
	}
	client_संदेश->तथ्य.retain = base_संदेश->तथ्य.retain;
	client_संदेश->तथ्य.subscription_identifier = 0;

	अगर(state == mosq_ms_queued){
		DL_APPEND(संदेश_तथ्य->queued, client_संदेश);
		db_संदेश_add_to_queued_stats(संदेश_तथ्य, client_संदेश);
	}अन्यथा{
		DL_APPEND(संदेश_तथ्य->inflight, client_संदेश);
		db_संदेश_add_to_inflight_stats(संदेश_तथ्य, client_संदेश);
	}

	अगर(persist && context->is_persisted){
		plugin_persist_handle_base_संदेश_add(client_संदेश->base_संदेश);
		plugin_persist_handle_client_संदेश_add(context, client_संदेश);
	}

	अगर(client_संदेश->base_संदेश->तथ्य.qos > 0){
		util__decrement_receive_quota(context);
	}
	वापस rc;
}


पूर्णांक db_message_जोड़ो_outgoing(काष्ठा mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, काष्ठा mosquitto_base_संदेश *base_संदेश, uint32_t subscription_identifier, bool update, bool persist)
{
	काष्ठा mosquitto_client_संदेश *client_संदेश;
	काष्ठा mosquitto_संदेश_तथ्य *संदेश_तथ्य;
	क्रमागत mosquitto_संदेश_state state = mosq_ms_invalid;
	पूर्णांक rc = 0;
	अक्षर **dest_ids;

	निश्चित(base_संदेश);
	अगर(!context){
		वापस MOSQ_ERR_INVAL;
	}
	अगर(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		वापस MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_sent++;

	संदेश_तथ्य = &context->msgs_out;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	अगर(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && base_संदेश->dest_ids){

		क्रम(पूर्णांक i=0; i<base_संदेश->dest_id_गिनती; i++){
			अगर(base_संदेश->dest_ids[i] && !म_भेद(base_संदेश->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				वापस MOSQ_ERR_SUCCESS;
			}
		}
	}
	अगर(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		अगर(qos == 0 && !db.config->queue_qos0_messages){
			अगर(!context->bridge){
				वापस 2;
			}अन्यथा{
				अगर(context->bridge->start_type != bst_lazy){
					वापस 2;
				}
			}
		}
		अगर(context->bridge && context->bridge->clean_start_local == true){
			वापस 2;
		}
	}

	अगर(net__is_connected(context)){
		अगर(db__ready_for_flight(context, mosq_md_out, qos)){
			चयन(qos){
				हाल 0:
					state = mosq_ms_publish_qos0;
					अवरोध;
				हाल 1:
					state = mosq_ms_publish_qos1;
					अवरोध;
				हाल 2:
					state = mosq_ms_publish_qos2;
					अवरोध;
			}
		}अन्यथा अगर(qos != 0 && db__ready_for_queue(context, qos, संदेश_तथ्य)){
			state = mosq_ms_queued;
			rc = 2;
		}अन्यथा{
			/* Dropping message due to full queue. */
			अगर(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(शून्य, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_गणक_mqtt_publish_dropped, 1);
			वापस 2;
		}
	}अन्यथा{
		अगर(db__ready_for_queue(context, qos, संदेश_तथ्य)){
			state = mosq_ms_queued;
		}अन्यथा{
			metrics__int_inc(mosq_गणक_mqtt_publish_dropped, 1);
			अगर(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(शून्य, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			वापस 2;
		}
	}
	निश्चित(state != mosq_ms_invalid);

#अगर_घोषित WITH_PERSISTENCE
	अगर(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#पूर्ण_अगर

	client_संदेश = mosquitto_malloc(माप(काष्ठा mosquitto_client_संदेश));
	अगर(!client_संदेश){
		वापस MOSQ_ERR_NOMEM;
	}
	client_संदेश->पूर्व = शून्य;
	client_संदेश->अग्र = शून्य;
	अगर(cmsg_id){
		client_संदेश->तथ्य.cmsg_id = cmsg_id;
	}अन्यथा{
		client_संदेश->तथ्य.cmsg_id = ++context->last_cmsg_id;
	}
	client_संदेश->base_संदेश = base_संदेश;
	db_संदेश_store_ref_inc(client_संदेश->base_संदेश);
	client_संदेश->तथ्य.mid = mid;
	client_संदेश->तथ्य.direction = mosq_md_out;
	client_संदेश->तथ्य.state = (uint8_t)state;
	client_संदेश->तथ्य.dup = false;
	अगर(qos > context->max_qos){
		client_संदेश->तथ्य.qos = context->max_qos;
	}अन्यथा{
		client_संदेश->तथ्य.qos = qos;
	}
	client_संदेश->तथ्य.retain = retain;
	client_संदेश->तथ्य.subscription_identifier = subscription_identifier;

	अगर(state == mosq_ms_queued){
		DL_APPEND(संदेश_तथ्य->queued, client_संदेश);
		db_संदेश_add_to_queued_stats(संदेश_तथ्य, client_संदेश);
	}अन्यथा{
		DL_APPEND(संदेश_तथ्य->inflight, client_संदेश);
		db_संदेश_add_to_inflight_stats(संदेश_तथ्य, client_संदेश);
	}

	अगर(persist && context->is_persisted){
		plugin_persist_handle_base_संदेश_add(client_संदेश->base_संदेश);
		plugin_persist_handle_client_संदेश_add(context, client_संदेश);
	}

	अगर(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(base_संदेश->dest_ids, माप(अक्षर *)*(माप_प्रकार)(base_संदेश->dest_id_गिनती+1));
		अगर(dest_ids){
			base_संदेश->dest_ids = dest_ids;
			base_संदेश->dest_id_गिनती++;
			base_संदेश->dest_ids[base_संदेश->dest_id_गिनती-1] = mosquitto_strdup(context->id);
			अगर(!base_संदेश->dest_ids[base_संदेश->dest_id_गिनती-1]){
				वापस MOSQ_ERR_NOMEM;
			}
		}अन्यथा{
			वापस MOSQ_ERR_NOMEM;
		}
	}
#अगर_घोषित WITH_BRIDGE
	अगर(context->bridge && context->bridge->start_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_out.inflight_गिनती + context->msgs_out.queued_गिनती >= context->bridge->threshold){

		context->bridge->lazy_reconnect = true;
	}
#पूर्ण_अगर

	अगर(client_संदेश->तथ्य.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	अगर(update){
		rc = db__message_write_inflight_out_latest(context);
		अगर(rc){
			वापस rc;
		}
		rc = db__message_write_queued_out(context);
		अगर(rc){
			वापस rc;
		}
	}

	वापस rc;
}


अटल अंतरभूत पूर्णांक db__message_update_outgoing_state(काष्ठा mosquitto *context, काष्ठा mosquitto_client_संदेश *शीर्ष,
		uint16_t mid, क्रमागत mosquitto_संदेश_state state, पूर्णांक qos, bool persist)
{
	काष्ठा mosquitto_client_संदेश *client_संदेश;

	DL_FOREACH(शीर्ष, client_संदेश){
		अगर(client_संदेश->तथ्य.mid == mid){
			अगर(client_संदेश->तथ्य.qos != qos){
				log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_संदेश->तथ्य.qos, qos);
				वापस MOSQ_ERR_PROTOCOL;
			}
			client_संदेश->तथ्य.state = (uint8_t)state;
			अगर(persist){
				plugin_persist_handle_client_संदेश_update(context, client_संदेश);
			}
			वापस MOSQ_ERR_SUCCESS;
		}
	}
	वापस MOSQ_ERR_NOT_FOUND;
}


पूर्णांक db__message_update_outgoing(काष्ठा mosquitto *context, uint16_t mid, क्रमागत mosquitto_संदेश_state state, पूर्णांक qos, bool persist)
{
	पूर्णांक rc;

	rc = db__message_update_outgoing_state(context, context->msgs_out.inflight, mid, state, qos, persist);
	अगर(!persist && rc == MOSQ_ERR_NOT_FOUND){
		rc = db__message_update_outgoing_state(context, context->msgs_out.queued, mid, state, qos, persist);
	}
	वापस rc;
}


अटल व्योम db__messages_delete_list(काष्ठा mosquitto_client_संदेश **शीर्ष)
{
	काष्ठा mosquitto_client_संदेश *client_संदेश, *tmp;

	DL_FOREACH_SAFE(*शीर्ष, client_संदेश, tmp){
		DL_DELETE(*शीर्ष, client_संदेश);
		db_संदेश_store_ref_dec(&client_संदेश->base_संदेश);
		mosquitto_FREE(client_संदेश);
	}
	*शीर्ष = शून्य;
}


पूर्णांक db__messages_delete_incoming(काष्ठा mosquitto *context)
{
	अगर(!context){
		वापस MOSQ_ERR_INVAL;
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

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक db__messages_delete_outgoing(काष्ठा mosquitto *context)
{
	अगर(!context){
		वापस MOSQ_ERR_INVAL;
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

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक db__messages_delete(काष्ठा mosquitto *context, bool force_free)
{
	अगर(!context){
		वापस MOSQ_ERR_INVAL;
	}

	अगर(force_free || context->clean_start || (context->bridge && context->bridge->clean_start)){
		db__messages_delete_incoming(context);
	}

	अगर(force_free || (context->bridge && context->bridge->clean_start_local)
			|| (context->bridge == शून्य && context->clean_start)){

		db__messages_delete_outgoing(context);
	}

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक db__messages_easy_queue(काष्ठा mosquitto *context, स्थिर अक्षर *topic, uint8_t qos, uint32_t payloadlen, स्थिर व्योम *payload, पूर्णांक retain, uint32_t message_expiry_interval, mosquitto_property **properties)
{
	काष्ठा mosquitto_base_संदेश *base_संदेश;
	स्थिर अक्षर *source_id;
	क्रमागत mosquitto_संदेश_origin origin;

	अगर(!topic){
		वापस MOSQ_ERR_INVAL;
	}

	base_संदेश = mosquitto_calloc(1, माप(काष्ठा mosquitto_base_संदेश));
	अगर(base_संदेश == शून्य){
		वापस MOSQ_ERR_NOMEM;
	}

	base_संदेश->तथ्य.topic = mosquitto_strdup(topic);
	अगर(base_संदेश->तथ्य.topic == शून्य){
		db_संदेश_store_free(base_संदेश);
		वापस MOSQ_ERR_INVAL;
	}

	base_संदेश->तथ्य.qos = qos;
	अगर(db.config->retain_available == false){
		base_संदेश->तथ्य.retain = 0;
	}अन्यथा{
		base_संदेश->तथ्य.retain = retain;
	}

	base_संदेश->तथ्य.payloadlen = payloadlen;
	अगर(payloadlen > 0){
		base_संदेश->तथ्य.payload = mosquitto_malloc(base_संदेश->तथ्य.payloadlen+1);
		अगर(base_संदेश->तथ्य.payload == शून्य){
			db_संदेश_store_free(base_संदेश);
			वापस MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_संदेश->तथ्य.payload)[base_संदेश->तथ्य.payloadlen] = 0;
		स_नकल(base_संदेश->तथ्य.payload, payload, base_संदेश->तथ्य.payloadlen);
	}

	अगर(context && context->id){
		source_id = context->id;
	}अन्यथा{
		source_id = "";
	}
	अगर(properties){
		base_संदेश->तथ्य.properties = *properties;
		*properties = शून्य;
	}

	अगर(context){
		origin = mosq_mo_client;
	}अन्यथा{
		origin = mosq_mo_broker;
	}
	अगर(db__message_store(context, base_संदेश, &message_expiry_interval, origin)){
		वापस 1;
	}

	वापस sub__messages_queue(source_id, base_संदेश->तथ्य.qos, &base_संदेश);
}


#घोषणा MOSQ_UUID_EPOCH 1637168273


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
uint64_t db_new_संदेश_id(व्योम)
{
#अगर_घोषित WIN32
	FILETIME ftime;
	uint64_t ftime64;
#अन्यथा
	काष्ठा timespec ts;
#पूर्ण_अगर
	uint64_t id;
	uint64_t tmp;
	समय_प्रकार sec;
	दीर्घ nsec;

	id = db.गाँठ_id_shifted; /* Top 10-bits */

#अगर_घोषित WIN32
	GetSystemTimePreciseAsFileTime(&ftime);
	ftime64 = (((uint64_t)ftime.dw_ऊपरी_Date_Time)<<32) + ftime.dw_निचला_Date_Time;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (दीर्घ)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#अन्यथा
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#पूर्ण_अगर
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	अगर(id <= db.last_db_id){
		id = db.last_db_id + 1;
	}
	db.last_db_id = id;

	वापस id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
पूर्णांक db__message_store(स्थिर काष्ठा mosquitto *source, काष्ठा mosquitto_base_संदेश *base_संदेश, uint32_t *message_expiry_interval, क्रमागत mosquitto_संदेश_origin origin)
{
	पूर्णांक rc;

	निश्चित(base_संदेश);

	अगर(source && source->id){
		base_संदेश->तथ्य.source_id = mosquitto_strdup(source->id);
	}अन्यथा{
		base_संदेश->तथ्य.source_id = mosquitto_strdup("");
	}
	अगर(!base_संदेश->तथ्य.source_id){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_संदेश_store_free(base_संदेश);
		वापस MOSQ_ERR_NOMEM;
	}

	अगर(source && source->username){
		base_संदेश->तथ्य.source_username = mosquitto_strdup(source->username);
		अगर(!base_संदेश->तथ्य.source_username){
			db_संदेश_store_free(base_संदेश);
			वापस MOSQ_ERR_NOMEM;
		}
	}
	अगर(source){
		base_संदेश->source_listener = source->listener;
	}
	base_संदेश->origin = origin;
	अगर(message_expiry_interval){
		अगर(*message_expiry_interval > 0 && *message_expiry_interval != संदेश_EXPIRY_INFINITE){
			base_संदेश->तथ्य.expiry_time = db.now_real_s + *message_expiry_interval;
		}अन्यथा{
			base_संदेश->तथ्य.expiry_time = 0;
		}
	}

	base_संदेश->dest_ids = शून्य;
	base_संदेश->dest_id_गिनती = 0;
	db.संदेश_store_गिनती++;
	db.संदेश_store_bytes += base_संदेश->तथ्य.payloadlen;

	अगर(!base_संदेश->तथ्य.store_id){
		base_संदेश->तथ्य.store_id = db_new_संदेश_id();
	}

	rc = db_संदेश_store_add(base_संदेश);
	अगर(rc){
		db_संदेश_store_free(base_संदेश);
		वापस rc;
	}

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक db__message_store_find(काष्ठा mosquitto *context, uint16_t mid, काष्ठा mosquitto_client_संदेश **client_संदेश)
{
	काष्ठा mosquitto_client_संदेश *cmsg;

	*client_संदेश = शून्य;

	अगर(!context){
		वापस MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_in.inflight, cmsg){
		अगर(cmsg->base_संदेश->तथ्य.source_mid == mid){
			*client_संदेश = cmsg;
			वापस MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_in.queued, cmsg){
		अगर(cmsg->base_संदेश->तथ्य.source_mid == mid){
			*client_संदेश = cmsg;
			वापस MOSQ_ERR_SUCCESS;
		}
	}

	वापस 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
अटल पूर्णांक db__message_reconnect_reset_outgoing(काष्ठा mosquitto *context)
{
	काष्ठा mosquitto_client_संदेश *client_संदेश, *tmp;

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
		अगर(client_संदेश->तथ्य.qos > 0){
			util__decrement_send_quota(context);
		}

		चयन(client_संदेश->तथ्य.qos){
			हाल 0:
				client_संदेश->तथ्य.state = mosq_ms_publish_qos0;
				अवरोध;
			हाल 1:
				client_संदेश->तथ्य.state = mosq_ms_publish_qos1;
				अवरोध;
			हाल 2:
				अगर(client_संदेश->तथ्य.state == mosq_ms_wait_for_pubcomp){
					client_संदेश->तथ्य.state = mosq_ms_resend_pubrel;
				}अन्यथा{
					client_संदेश->तथ्य.state = mosq_ms_publish_qos2;
				}
				अवरोध;
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

	वापस MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
अटल पूर्णांक db__message_reconnect_reset_incoming(काष्ठा mosquitto *context)
{
	काष्ठा mosquitto_client_संदेश *client_संदेश, *tmp;

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
		अगर(client_संदेश->तथ्य.qos > 0){
			util__decrement_receive_quota(context);
		}

		अगर(client_संदेश->तथ्य.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db__message_remove_inflight(context, &context->msgs_in, client_संदेश);
		}अन्यथा{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_संदेश->तथ्य.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_in.queued, client_संदेश, tmp){
		client_संदेश->तथ्य.dup = 0;
		db_संदेश_add_to_queued_stats(&context->msgs_in, client_संदेश);
		अगर(db__ready_for_flight(context, mosq_md_in, client_संदेश->तथ्य.qos)){
			चयन(client_संदेश->तथ्य.qos){
				हाल 0:
					client_संदेश->तथ्य.state = mosq_ms_publish_qos0;
					अवरोध;
				हाल 1:
					client_संदेश->तथ्य.state = mosq_ms_publish_qos1;
					अवरोध;
				हाल 2:
					client_संदेश->तथ्य.state = mosq_ms_publish_qos2;
					अवरोध;
			}
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_संदेश_update(context, client_संदेश);
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक db__message_reconnect_reset(काष्ठा mosquitto *context)
{
	पूर्णांक rc;

	rc = db__message_reconnect_reset_outgoing(context);
	अगर(rc){
		वापस rc;
	}
	वापस db__message_reconnect_reset_incoming(context);
}


पूर्णांक db__message_remove_incoming(काष्ठा mosquitto *context, uint16_t mid)
{
	काष्ठा mosquitto_client_संदेश *client_संदेश, *tmp;

	अगर(!context){
		वापस MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_संदेश, tmp){
		अगर(client_संदेश->तथ्य.mid == mid){
			अगर(client_संदेश->base_संदेश->तथ्य.qos != 2){
				log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_संदेश->base_संदेश->तथ्य.qos);
				वापस MOSQ_ERR_PROTOCOL;
			}
			db__message_remove_inflight(context, &context->msgs_in, client_संदेश);
			वापस MOSQ_ERR_SUCCESS;
		}
	}

	वापस MOSQ_ERR_NOT_FOUND;
}


पूर्णांक db__message_release_incoming(काष्ठा mosquitto *context, uint16_t mid)
{
	काष्ठा mosquitto_client_संदेश *client_संदेश, *tmp;
	bool deleted = false;
	पूर्णांक rc;

	अगर(!context){
		वापस MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_in.inflight, client_संदेश, tmp){
		अगर(client_संदेश->तथ्य.mid == mid){
			अगर(client_संदेश->base_संदेश->तथ्य.qos != 2){
				log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_संदेश->base_संदेश->तथ्य.qos);
				वापस MOSQ_ERR_PROTOCOL;
			}
			स्थिर अक्षर *topic = client_संदेश->base_संदेश->तथ्य.topic;
			स्थिर अक्षर *source_id = client_संदेश->base_संदेश->तथ्य.source_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			अगर(topic == शून्य){
				db__message_remove_inflight(context, &context->msgs_in, client_संदेश);
				deleted = true;
			}अन्यथा{
				rc = sub__messages_queue(source_id, 2, &client_संदेश->base_संदेश);
				अगर(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db__message_remove_inflight(context, &context->msgs_in, client_संदेश);
					deleted = true;
				}अन्यथा{
					वापस 1;
				}
			}
		}
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_संदेश, tmp){
		अगर(db__ready_for_flight(context, mosq_md_in, client_संदेश->तथ्य.qos)){
			अवरोध;
		}

		अगर(client_संदेश->तथ्य.qos == 2){
			send__pubrec(context, client_संदेश->तथ्य.mid, 0, शून्य);
			client_संदेश->तथ्य.state = mosq_ms_wait_for_pubrel;
			db__message_dequeue_first(context, &context->msgs_in);
			plugin_persist_handle_client_संदेश_update(context, client_संदेश);
		}
	}
	अगर(deleted){
		वापस MOSQ_ERR_SUCCESS;
	}अन्यथा{
		वापस MOSQ_ERR_NOT_FOUND;
	}
}


व्योम db__expire_all_messages(काष्ठा mosquitto *context)
{
	काष्ठा mosquitto_client_संदेश *client_संदेश, *tmp;

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_संदेश, tmp){
		अगर(client_संदेश->base_संदेश->तथ्य.expiry_time && db.now_real_s > client_संदेश->base_संदेश->तथ्य.expiry_time){
			अगर(client_संदेश->तथ्य.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_संदेश);
		}
	}
	db__fill_inflight_out_from_queue(context);
	DL_FOREACH_SAFE(context->msgs_out.queued, client_संदेश, tmp){
		अगर(client_संदेश->base_संदेश->तथ्य.expiry_time && db.now_real_s > client_संदेश->base_संदेश->तथ्य.expiry_time){
			db__message_remove_queued(context, &context->msgs_out, client_संदेश);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.inflight, client_संदेश, tmp){
		अगर(client_संदेश->base_संदेश->तथ्य.expiry_time && db.now_real_s > client_संदेश->base_संदेश->तथ्य.expiry_time){
			अगर(client_संदेश->तथ्य.qos > 0){
				util__increment_receive_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_in, client_संदेश);
		}
	}
	DL_FOREACH_SAFE(context->msgs_in.queued, client_संदेश, tmp){
		अगर(client_संदेश->base_संदेश->तथ्य.expiry_time && db.now_real_s > client_संदेश->base_संदेश->तथ्य.expiry_time){
			db__message_remove_queued(context, &context->msgs_in, client_संदेश);
		}
	}
}


अटल व्योम db__client_messages_check_acl(काष्ठा mosquitto *context, काष्ठा mosquitto_संदेश_तथ्य *संदेश_तथ्य, काष्ठा mosquitto_client_संदेश **शीर्ष,
		व्योम (*decrement_stats_fn)(काष्ठा mosquitto_संदेश_तथ्य *संदेश_तथ्य, काष्ठा mosquitto_client_संदेश *client_संदेश))
{
	काष्ठा mosquitto_client_संदेश *client_संदेश, *tmp;
	काष्ठा mosquitto_base_संदेश *base_संदेश;
	पूर्णांक access;

	DL_FOREACH_SAFE((*शीर्ष), client_संदेश, tmp){
		base_संदेश = client_संदेश->base_संदेश;
		अगर(client_संदेश->तथ्य.direction == mosq_md_out){
			access = MOSQ_ACL_READ;
		}अन्यथा{
			access = MOSQ_ACL_WRITE;
		}
		अगर(mosquitto_acl_check(context, base_संदेश->तथ्य.topic,
				base_संदेश->तथ्य.payloadlen, base_संदेश->तथ्य.payload,
				base_संदेश->तथ्य.qos, base_संदेश->तथ्य.retain,
				base_संदेश->तथ्य.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*शीर्ष), client_संदेश);
			decrement_stats_fn(संदेश_तथ्य, client_संदेश);
			plugin_persist_handle_client_संदेश_delete(context, client_संदेश);
			db_संदेश_store_ref_dec(&client_संदेश->base_संदेश);
			mosquitto_FREE(client_संदेश);
		}
	}
}


व्योम db__check_acl_of_all_messages(काष्ठा mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.inflight, &db_संदेश_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_in, &context->msgs_in.queued, &db_संदेश_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.inflight, &db_संदेश_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_out, &context->msgs_out.queued, &db_संदेश_remove_from_queued_stats);
}


अटल पूर्णांक db__message_write_inflight_out_single(काष्ठा mosquitto *context, काष्ठा mosquitto_client_संदेश *client_संदेश)
{
	काष्ठा mosquitto_base_संदेश *base_संदेश;
	mosquitto_property *base_संदेश_props = शून्य;
	पूर्णांक rc;
	uint16_t mid;
	पूर्णांक retries;
	पूर्णांक retain;
	स्थिर अक्षर *topic;
	uint8_t qos;
	uint32_t payloadlen;
	स्थिर व्योम *payload;
	uint32_t expiry_interval;
	uint32_t subscription_id;

	base_संदेश = client_संदेश->base_संदेश;

	expiry_interval = 0;
	अगर(base_संदेश->तथ्य.expiry_time){
		अगर(db.now_real_s > base_संदेश->तथ्य.expiry_time){
			/* Message is expired, must not send. */
			अगर(client_संदेश->तथ्य.direction == mosq_md_out && client_संदेश->तथ्य.qos > 0){
				util__increment_send_quota(context);
			}
			db__message_remove_inflight(context, &context->msgs_out, client_संदेश);
			db__fill_inflight_out_from_queue(context);
			वापस MOSQ_ERR_SUCCESS;
		}अन्यथा{
			expiry_interval = (uint32_t)(base_संदेश->तथ्य.expiry_time - db.now_real_s);
		}
	}
	mid = client_संदेश->तथ्य.mid;
	retries = client_संदेश->तथ्य.dup;
	retain = client_संदेश->तथ्य.retain;
	topic = base_संदेश->तथ्य.topic;
	qos = (uint8_t)client_संदेश->तथ्य.qos;
	payloadlen = base_संदेश->तथ्य.payloadlen;
	payload = base_संदेश->तथ्य.payload;
	subscription_id = client_संदेश->तथ्य.subscription_identifier;
	base_संदेश_props = base_संदेश->तथ्य.properties;

	चयन(client_संदेश->तथ्य.state){
		हाल mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_संदेश_props, expiry_interval);
			अगर(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_संदेश);
			}अन्यथा{
				वापस rc;
			}
			अवरोध;

		हाल mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_संदेश_props, expiry_interval);
			अगर(rc == MOSQ_ERR_SUCCESS){
				client_संदेश->तथ्य.dup = 1; /* Any retry attempts are a duplicate. */
				client_संदेश->तथ्य.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_संदेश_update(context, client_संदेश);
			}अन्यथा अगर(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_संदेश);
			}अन्यथा{
				वापस rc;
			}
			अवरोध;

		हाल mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, base_संदेश_props, expiry_interval);
			अगर(rc == MOSQ_ERR_SUCCESS){
				client_संदेश->तथ्य.dup = 1; /* Any retry attempts are a duplicate. */
				client_संदेश->तथ्य.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_संदेश_update(context, client_संदेश);
			}अन्यथा अगर(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db__message_remove_inflight(context, &context->msgs_out, client_संदेश);
			}अन्यथा{
				वापस rc;
			}
			अवरोध;

		हाल mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, शून्य);
			अगर(!rc){
				client_संदेश->तथ्य.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_संदेश_update(context, client_संदेश);
			}अन्यथा{
				वापस rc;
			}
			अवरोध;

		हाल mosq_ms_invalid:
		हाल mosq_ms_send_pubrec:
		हाल mosq_ms_resend_pubcomp:
		हाल mosq_ms_wait_for_puback:
		हाल mosq_ms_wait_for_pubrec:
		हाल mosq_ms_wait_for_pubrel:
		हाल mosq_ms_wait_for_pubcomp:
		हाल mosq_ms_queued:
			अवरोध;
	}
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक db__message_write_inflight_out_all(काष्ठा mosquitto *context)
{
	काष्ठा mosquitto_client_संदेश *client_संदेश, *tmp;
	पूर्णांक rc;

	अगर(context->state != mosq_cs_active || !net__is_connected(context)){
		वापस MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_out.inflight, client_संदेश, tmp){
		rc = db__message_write_inflight_out_single(context, client_संदेश);
		अगर(rc){
			वापस rc;
		}
	}
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक db__message_write_inflight_out_latest(काष्ठा mosquitto *context)
{
	काष्ठा mosquitto_client_संदेश *client_संदेश, *अग्र;
	पूर्णांक rc;

	अगर(context->state != mosq_cs_active
			|| !net__is_connected(context)
			|| context->msgs_out.inflight == शून्य){

		वापस MOSQ_ERR_SUCCESS;
	}

	अगर(context->msgs_out.inflight->पूर्व == context->msgs_out.inflight){
		/* Only one message */
		वापस db__message_write_inflight_out_single(context, context->msgs_out.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_संदेश = context->msgs_out.inflight->पूर्व;
	जबतक(client_संदेश != context->msgs_out.inflight &&
			(client_संदेश->तथ्य.state == mosq_ms_publish_qos0
			|| client_संदेश->तथ्य.state == mosq_ms_publish_qos1
			|| client_संदेश->तथ्य.state == mosq_ms_publish_qos2)){

		client_संदेश = client_संदेश->पूर्व;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	अगर(client_संदेश != context->msgs_out.inflight){
		client_संदेश = client_संदेश->अग्र;
	}

	जबतक(client_संदेश){
		अग्र = client_संदेश->अग्र;
		rc = db__message_write_inflight_out_single(context, client_संदेश);
		अगर(rc){
			वापस rc;
		}
		client_संदेश = अग्र;
	}
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक db__message_write_queued_in(काष्ठा mosquitto *context)
{
	काष्ठा mosquitto_client_संदेश *client_संदेश, *tmp;
	पूर्णांक rc;

	अगर(context->state != mosq_cs_active){
		वापस MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_in.queued, client_संदेश, tmp){
		अगर(context->msgs_in.inflight_maximum != 0 && context->msgs_in.inflight_quota == 0){
			अवरोध;
		}

		अगर(client_संदेश->तथ्य.qos == 2){
			client_संदेश->तथ्य.state = mosq_ms_send_pubrec;
			db__message_dequeue_first(context, &context->msgs_in);
			rc = send__pubrec(context, client_संदेश->तथ्य.mid, 0, शून्य);
			अगर(!rc){
				client_संदेश->तथ्य.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_संदेश_update(context, client_संदेश);
			}अन्यथा{
				plugin_persist_handle_client_संदेश_update(context, client_संदेश);
				वापस rc;
			}
		}
	}
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक db__message_write_queued_out(काष्ठा mosquitto *context)
{
	अगर(context->state != mosq_cs_active){
		वापस MOSQ_ERR_SUCCESS;
	}

	db__fill_inflight_out_from_queue(context);

	वापस MOSQ_ERR_SUCCESS;
}
