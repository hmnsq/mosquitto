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
	संरचना mosquitto_सन्देशः_दत्त *msgs;
	bool valid_अष्टकानि;
	bool valid_गणनम्;

	यदि(dir == mosq_md_बहिर्गमनम्){
		msgs = &context->msgs_बहिर्गमनम्;
	}अन्यथा{
		msgs = &context->msgs_अन्तर्;
	}

	यदि(msgs->inflight_अधिकतमम् == 0 && db.config->max_inflight_अष्टकानि == 0){
		प्रत्यागम true;
	}

	यदि(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		यदि(db.config->max_queued_messages == 0 && db.config->max_inflight_अष्टकानि == 0){
			प्रत्यागम true;
		}
		valid_अष्टकानि = ((msgs->inflight_अष्टकानि - (ssize_t)db.config->max_inflight_अष्टकानि) < (ssize_t)db.config->max_queued_अष्टकानि);
		यदि(dir == mosq_md_बहिर्गमनम्){
			valid_गणनम् = context->बहिर्गमनम्_packet_गणनम् < db.config->max_queued_messages;
		}अन्यथा{
			valid_गणनम् = msgs->inflight_गणनम् - msgs->inflight_अधिकतमम् < db.config->max_queued_messages;
		}

		यदि(db.config->max_queued_messages == 0){
			प्रत्यागम valid_अष्टकानि;
		}
		यदि(db.config->max_queued_अष्टकानि == 0){
			प्रत्यागम valid_गणनम्;
		}
	}अन्यथा{
		valid_अष्टकानि = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_अष्टकानि;
		valid_गणनम् = msgs->inflight_quota > 0;

		यदि(msgs->inflight_अधिकतमम् == 0){
			प्रत्यागम valid_अष्टकानि;
		}
		यदि(db.config->max_inflight_अष्टकानि == 0){
			प्रत्यागम valid_गणनम्;
		}
	}

	प्रत्यागम valid_अष्टकानि && valid_गणनम्;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db__ready_for_queue(संरचना mosquitto *context, पूर्णाङ्क qos, संरचना mosquitto_सन्देशः_दत्त *सन्देशः_दत्त)
{
	पूर्णाङ्क source_गणनम्;
	पूर्णाङ्क adjust_गणनम्;
	दीर्घ source_अष्टकानि;
	ssize_t adjust_अष्टकानि = (ssize_t)db.config->max_inflight_अष्टकानि;
	bool valid_अष्टकानि;
	bool valid_गणनम्;

	यदि(db.config->max_queued_messages == 0 && db.config->max_queued_अष्टकानि == 0){
		प्रत्यागम true;
	}

	यदि(qos == 0 && db.config->queue_qos0_messages == false){
		प्रत्यागम false; /* This case is handled in db__ready_for_flight() */
	}अन्यथा{
		source_अष्टकानि = (ssize_t)सन्देशः_दत्त->queued_bytes12;
		source_गणनम् = सन्देशः_दत्त->queued_count12;
	}
	adjust_गणनम् = सन्देशः_दत्त->inflight_अधिकतमम्;

	/* nothing in flight for offline clients */
	यदि(!net__is_connected(context)){
		adjust_अष्टकानि = 0;
		adjust_गणनम् = 0;
	}

	valid_अष्टकानि = (source_अष्टकानि - (ssize_t)adjust_अष्टकानि) < (ssize_t)db.config->max_queued_अष्टकानि;
	valid_गणनम् = source_गणनम् - adjust_गणनम् < db.config->max_queued_messages;

	यदि(db.config->max_queued_अष्टकानि == 0){
		प्रत्यागम valid_गणनम्;
	}
	यदि(db.config->max_queued_messages == 0){
		प्रत्यागम valid_अष्टकानि;
	}

	प्रत्यागम valid_अष्टकानि && valid_गणनम्;
}


शून्य db_सन्देशः_सङ्कलय_गन्तव्यम्_inflight_stats(संरचना mosquitto_सन्देशः_दत्त *सन्देशः_दत्त, संरचना mosquitto_client_सन्देशः *client_सन्देशः)
{
	सन्देशः_दत्त->inflight_गणनम्++;
	सन्देशः_दत्त->inflight_अष्टकानि += client_सन्देशः->आधारः_सन्देशः->दत्त.payloadlen;
	यदि(client_सन्देशः->दत्त.qos != 0){
		सन्देशः_दत्त->inflight_count12++;
		सन्देशः_दत्त->inflight_bytes12 += client_सन्देशः->आधारः_सन्देशः->दत्त.payloadlen;
	}
}


स्थिर शून्य db_सन्देशः_remove_from_inflight_stats(संरचना mosquitto_सन्देशः_दत्त *सन्देशः_दत्त, संरचना mosquitto_client_सन्देशः *client_सन्देशः)
{
	सन्देशः_दत्त->inflight_गणनम्--;
	सन्देशः_दत्त->inflight_अष्टकानि -= client_सन्देशः->आधारः_सन्देशः->दत्त.payloadlen;
	यदि(client_सन्देशः->दत्त.qos != 0){
		सन्देशः_दत्त->inflight_count12--;
		सन्देशः_दत्त->inflight_bytes12 -= client_सन्देशः->आधारः_सन्देशः->दत्त.payloadlen;
	}
}


शून्य db_सन्देशः_सङ्कलय_गन्तव्यम्_queued_stats(संरचना mosquitto_सन्देशः_दत्त *सन्देशः_दत्त, संरचना mosquitto_client_सन्देशः *client_सन्देशः)
{
	सन्देशः_दत्त->queued_गणनम्++;
	सन्देशः_दत्त->queued_अष्टकानि += client_सन्देशः->आधारः_सन्देशः->दत्त.payloadlen;
	यदि(client_सन्देशः->दत्त.qos != 0){
		सन्देशः_दत्त->queued_count12++;
		सन्देशः_दत्त->queued_bytes12 += client_सन्देशः->आधारः_सन्देशः->दत्त.payloadlen;
	}
}


स्थिर शून्य db_सन्देशः_remove_from_queued_stats(संरचना mosquitto_सन्देशः_दत्त *सन्देशः_दत्त, संरचना mosquitto_client_सन्देशः *client_सन्देशः)
{
	सन्देशः_दत्त->queued_गणनम्--;
	सन्देशः_दत्त->queued_अष्टकानि -= client_सन्देशः->आधारः_सन्देशः->दत्त.payloadlen;
	यदि(client_सन्देशः->दत्त.qos != 0){
		सन्देशः_दत्त->queued_count12--;
		सन्देशः_दत्त->queued_bytes12 -= client_सन्देशः->आधारः_सन्देशः->दत्त.payloadlen;
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
#यदि_परिभाषितम् WITH_BRIDGE
	db.bridges = रिक्त;
	db.bridge_गणनम् = 0;
#यदि_अन्तः

	/* Initialize the hashtable */
	db.clientid_सूचकांक_hash = रिक्त;

	db.प्रसामान्यम्_subs = रिक्त;
	db.shared_subs = रिक्त;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = रिक्त;

#यदि_परिभाषितम् WITH_PERSISTENCE
	यदि(persist__restore()){
		प्रत्यागम 1;
	}
#यदि_अन्तः

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर शून्य subhier_clean(संरचना mosquitto__subhier **subhier)
{
	संरचना mosquitto__subhier *peer, *subhier_tmp;
	संरचना mosquitto__subleaf *पर्णम्, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		पर्णम् = peer->subs;
		यावत्(पर्णम्){
			nextleaf = पर्णम्->अग्र;
			mosquitto_FREE(पर्णम्);
			पर्णम् = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


पूर्णाङ्क db__close(शून्य)
{
	subhier_clean(&db.प्रसामान्यम्_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db_सन्देशः_store_clean();

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db_सन्देशः_store_सङ्कलय(संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः)
{
	संरचना mosquitto_आधारः_सन्देशः *प्राप्तम्;
	अचिह्नित hashv;

	HASH_मानम्(&आधारः_सन्देशः->दत्त.store_id, परिमाण(आधारः_सन्देशः->दत्त.store_id), hashv);
	HASH_FIND_BYHASHVALUE(hh, db.सन्देशः_store, &आधारः_सन्देशः->दत्त.store_id, परिमाण(आधारः_सन्देशः->दत्त.store_id), hashv, प्राप्तम्);
	यदि(प्राप्तम् == रिक्त){
		HASH_सङ्कलय_KEYPTR_BYHASHVALUE(hh, db.सन्देशः_store, &आधारः_सन्देशः->दत्त.store_id, परिमाण(आधारः_सन्देशः->दत्त.store_id), hashv, आधारः_सन्देशः);
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_ALREADY_EXISTS;
	}
}


शून्य db_सन्देशः_store_free(संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः)
{
	mosquitto_FREE(आधारः_सन्देशः->दत्त.source_id);
	mosquitto_FREE(आधारः_सन्देशः->दत्त.source_username);
	यदि(आधारः_सन्देशः->dest_ids){
		प्रति(पूर्णाङ्क i=0; i<आधारः_सन्देशः->dest_id_गणनम्; i++){
			mosquitto_FREE(आधारः_सन्देशः->dest_ids[i]);
		}
		mosquitto_FREE(आधारः_सन्देशः->dest_ids);
	}
	mosquitto_FREE(आधारः_सन्देशः->दत्त.topic);
	mosquitto_property_free_all(&आधारः_सन्देशः->दत्त.properties);
	mosquitto_FREE(आधारः_सन्देशः->दत्त.payload);
	mosquitto_FREE(आधारः_सन्देशः);
}


शून्य db_सन्देशः_store_remove(संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः, bool notify)
{
	यदि(आधारः_सन्देशः == रिक्त){
		प्रत्यागम;
	}
	HASH_DELETE(hh, db.सन्देशः_store, आधारः_सन्देशः);
	db.सन्देशः_store_गणनम्--;
	db.सन्देशः_store_अष्टकानि -= आधारः_सन्देशः->दत्त.payloadlen;
	यदि(notify == true){
		plugin_persist_handle_आधारः_सन्देशः_delete(आधारः_सन्देशः);
	}
	db_सन्देशः_store_free(आधारः_सन्देशः);
}


शून्य db_सन्देशः_store_clean(शून्य)
{
	संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः, *आधारः_सन्देशः_tmp;

	HASH_ITER(hh, db.सन्देशः_store, आधारः_सन्देशः, आधारः_सन्देशः_tmp){
		db_सन्देशः_store_remove(आधारः_सन्देशः, false);
	}
}


शून्य db_सन्देशः_store_ref_inc(संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः)
{
	आधारः_सन्देशः->ref_गणनम्++;
}


शून्य db_सन्देशः_store_ref_dec(संरचना mosquitto_आधारः_सन्देशः **आधारः_सन्देशः)
{
	(*आधारः_सन्देशः)->ref_गणनम्--;
	यदि((*आधारः_सन्देशः)->ref_गणनम् == 0){
		db_सन्देशः_store_remove(*आधारः_सन्देशः, true);
		*आधारः_सन्देशः = रिक्त;
	}
}


शून्य db_सन्देशः_store_compact(शून्य)
{
	संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः, *आधारः_सन्देशः_tmp;

	HASH_ITER(hh, db.सन्देशः_store, आधारः_सन्देशः, आधारः_सन्देशः_tmp){
		यदि(आधारः_सन्देशः->ref_गणनम् < 1){
			db_सन्देशः_store_remove(आधारः_सन्देशः, true);
		}
	}
}


स्थिर शून्य db_सन्देशः_remove_inflight(संरचना mosquitto *context, संरचना mosquitto_सन्देशः_दत्त *सन्देशः_दत्त, संरचना mosquitto_client_सन्देशः *item)
{
	यदि(!context || !सन्देशः_दत्त || !item){
		प्रत्यागम;
	}

	plugin_persist_handle_client_सन्देशः_delete(context, item);

	DL_DELETE(सन्देशः_दत्त->inflight, item);
	यदि(item->आधारः_सन्देशः){
		db_सन्देशः_remove_from_inflight_stats(सन्देशः_दत्त, item);
		db_सन्देशः_store_ref_dec(&item->आधारः_सन्देशः);
	}

	mosquitto_FREE(item);
}


स्थिर शून्य db_सन्देशः_remove_queued(संरचना mosquitto *context, संरचना mosquitto_सन्देशः_दत्त *सन्देशः_दत्त, संरचना mosquitto_client_सन्देशः *item)
{
	यदि(!context || !सन्देशः_दत्त || !item){
		प्रत्यागम;
	}

	plugin_persist_handle_client_सन्देशः_delete(context, item);

	DL_DELETE(सन्देशः_दत्त->queued, item);
	यदि(item->आधारः_सन्देशः){
		db_सन्देशः_remove_from_queued_stats(सन्देशः_दत्त, item);
		db_सन्देशः_store_ref_dec(&item->आधारः_सन्देशः);
	}

	mosquitto_FREE(item);
}


स्थिर शून्य db_fill_inflight_बहिर्गमनम्_from_queue(संरचना mosquitto *context)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;

	DL_FOREACH_SAFE(context->msgs_बहिर्गमनम्.queued, client_सन्देशः, tmp){
		यदि(!db__ready_for_flight(context, mosq_md_बहिर्गमनम्, client_सन्देशः->दत्त.qos)){
			प्रत्यागम;
		}
		विकल्प(client_सन्देशः->दत्त.qos){
			पक्ष 0:
				client_सन्देशः->दत्त.state = mosq_ms_publish_qos0;
				भङ्ग;
			पक्ष 1:
				client_सन्देशः->दत्त.state = mosq_ms_publish_qos1;
				भङ्ग;
			पक्ष 2:
				client_सन्देशः->दत्त.state = mosq_ms_publish_qos2;
				भङ्ग;
		}
		यदि(client_सन्देशः->आधारः_सन्देशः->दत्त.expiry_time && db.now_real_s > client_सन्देशः->आधारः_सन्देशः->दत्त.expiry_time){
			db_सन्देशः_remove_queued(context, &context->msgs_बहिर्गमनम्, client_सन्देशः);
			अनुवर्त;
		}
		plugin_persist_handle_client_सन्देशः_update(context, client_सन्देशः);
		db_सन्देशः_पङ्क्तेः_निष्कासय_first(context, &context->msgs_बहिर्गमनम्);
	}
}


शून्य db_सन्देशः_पङ्क्तेः_निष्कासय_first(संरचना mosquitto *context, संरचना mosquitto_सन्देशः_दत्त *सन्देशः_दत्त)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः;

	अप्रयुक्तम्(context);

	client_सन्देशः = सन्देशः_दत्त->queued;
	DL_DELETE(सन्देशः_दत्त->queued, client_सन्देशः);
	DL_APPEND(सन्देशः_दत्त->inflight, client_सन्देशः);
	यदि(सन्देशः_दत्त->inflight_quota > 0){
		सन्देशः_दत्त->inflight_quota--;
	}

	db_सन्देशः_remove_from_queued_stats(सन्देशः_दत्त, client_सन्देशः);
	db_सन्देशः_सङ्कलय_गन्तव्यम्_inflight_stats(सन्देशः_दत्त, client_सन्देशः);
}


पूर्णाङ्क db_सन्देशः_delete_outgoing(संरचना mosquitto *context, uint16_t mid, गणना mosquitto_सन्देशः_state expect_state, पूर्णाङ्क qos)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;
	bool deleted = false;

	यदि(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_बहिर्गमनम्.inflight, client_सन्देशः, tmp){
		यदि(client_सन्देशः->दत्त.mid == mid){
			यदि(client_सन्देशः->दत्त.qos != qos){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_सन्देशः->दत्त.qos, qos);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}अन्यथा यदि(qos == 2 && client_सन्देशः->दत्त.state != expect_state && expect_state != mosq_ms_any){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_सन्देशः->दत्त.state, expect_state);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}
			db_सन्देशः_remove_inflight(context, &context->msgs_बहिर्गमनम्, client_सन्देशः);
			deleted = true;
			भङ्ग;
		}
	}

	यदि(deleted == false){
		DL_FOREACH_SAFE(context->msgs_बहिर्गमनम्.queued, client_सन्देशः, tmp){
			यदि(client_सन्देशः->दत्त.mid == mid){
				यदि(client_सन्देशः->दत्त.qos != qos){
					प्रत्यागम MOSQ_ERR_PROTOCOL;
				}अन्यथा यदि(qos == 2 && client_सन्देशः->दत्त.state != expect_state && expect_state != mosq_ms_any){
					प्रत्यागम MOSQ_ERR_PROTOCOL;
				}
				db_सन्देशः_remove_queued(context, &context->msgs_बहिर्गमनम्, client_सन्देशः);
				भङ्ग;
			}
		}
	}
	db_fill_inflight_बहिर्गमनम्_from_queue(context);
#यदि_परिभाषितम् WITH_PERSISTENCE
	db.persistence_changes++;
#यदि_अन्तः

	प्रत्यागम db_सन्देशः_write_inflight_बहिर्गमनम्_latest(context);
}


/* Only for QoS 2 messages */
पूर्णाङ्क db_सन्देशः_निवेशय_incoming(संरचना mosquitto *context, uint64_t cmsg_id, संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः, bool persist)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः;
	संरचना mosquitto_सन्देशः_दत्त *सन्देशः_दत्त;
	गणना mosquitto_सन्देशः_state state = mosq_ms_invalid;
	पूर्णाङ्क rc = 0;

	प्रतिज्ञा(आधारः_सन्देशः);
	यदि(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		प्रत्यागम MOSQ_ERR_SUCCESS;

	}
	सन्देशः_दत्त = &context->msgs_अन्तर्;

	यदि(db__ready_for_flight(context, mosq_md_अन्तर्, आधारः_सन्देशः->दत्त.qos)){
		state = mosq_ms_wait_for_pubrel;
	}अन्यथा यदि(आधारः_सन्देशः->दत्त.qos != 0 && db__ready_for_queue(context, आधारः_सन्देशः->दत्त.qos, सन्देशः_दत्त)){
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
		metrics__int_inc(mosq_गणक_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		प्रत्यागम 2;
	}

	प्रतिज्ञा(state != mosq_ms_invalid);

#यदि_परिभाषितम् WITH_PERSISTENCE
	यदि(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#यदि_अन्तः

	client_सन्देशः = mosquitto_malloc(परिमाण(संरचना mosquitto_client_सन्देशः));
	यदि(!client_सन्देशः){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	client_सन्देशः->पूर्व = रिक्त;
	client_सन्देशः->अग्र = रिक्त;
	यदि(cmsg_id){
		client_सन्देशः->दत्त.cmsg_id = cmsg_id;
	}अन्यथा{
		client_सन्देशः->दत्त.cmsg_id = ++context->चरमः_cmsg_id;
	}
	client_सन्देशः->आधारः_सन्देशः = आधारः_सन्देशः;
	db_सन्देशः_store_ref_inc(client_सन्देशः->आधारः_सन्देशः);
	client_सन्देशः->दत्त.mid = आधारः_सन्देशः->दत्त.source_mid;
	client_सन्देशः->दत्त.direction = mosq_md_अन्तर्;
	client_सन्देशः->दत्त.state = (uint8_t)state;
	client_सन्देशः->दत्त.dup = false;
	यदि(आधारः_सन्देशः->दत्त.qos > context->max_qos){
		client_सन्देशः->दत्त.qos = context->max_qos;
	}अन्यथा{
		client_सन्देशः->दत्त.qos = आधारः_सन्देशः->दत्त.qos;
	}
	client_सन्देशः->दत्त.retain = आधारः_सन्देशः->दत्त.retain;
	client_सन्देशः->दत्त.subscription_identifier = 0;

	यदि(state == mosq_ms_queued){
		DL_APPEND(सन्देशः_दत्त->queued, client_सन्देशः);
		db_सन्देशः_सङ्कलय_गन्तव्यम्_queued_stats(सन्देशः_दत्त, client_सन्देशः);
	}अन्यथा{
		DL_APPEND(सन्देशः_दत्त->inflight, client_सन्देशः);
		db_सन्देशः_सङ्कलय_गन्तव्यम्_inflight_stats(सन्देशः_दत्त, client_सन्देशः);
	}

	यदि(persist && context->is_persisted){
		plugin_persist_handle_आधारः_सन्देशः_सङ्कलय(client_सन्देशः->आधारः_सन्देशः);
		plugin_persist_handle_client_सन्देशः_सङ्कलय(context, client_सन्देशः);
	}

	यदि(client_सन्देशः->आधारः_सन्देशः->दत्त.qos > 0){
		util__decrement_receive_quota(context);
	}
	प्रत्यागम rc;
}


पूर्णाङ्क db_सन्देशः_निवेशय_outgoing(संरचना mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः, uint32_t subscription_identifier, bool update, bool persist)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः;
	संरचना mosquitto_सन्देशः_दत्त *सन्देशः_दत्त;
	गणना mosquitto_सन्देशः_state state = mosq_ms_invalid;
	पूर्णाङ्क rc = 0;
	वर्ण **dest_ids;

	प्रतिज्ञा(आधारः_सन्देशः);
	यदि(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		प्रत्यागम MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_प्रेषितम्++;

	सन्देशः_दत्त = &context->msgs_बहिर्गमनम्;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	यदि(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && आधारः_सन्देशः->dest_ids){

		प्रति(पूर्णाङ्क i=0; i<आधारः_सन्देशः->dest_id_गणनम्; i++){
			यदि(आधारः_सन्देशः->dest_ids[i] && !वाक्यं_तुलय(आधारः_सन्देशः->dest_ids[i], context->id)){
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
		यदि(db__ready_for_flight(context, mosq_md_बहिर्गमनम्, qos)){
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
		}अन्यथा यदि(qos != 0 && db__ready_for_queue(context, qos, सन्देशः_दत्त)){
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
			metrics__int_inc(mosq_गणक_mqtt_publish_dropped, 1);
			प्रत्यागम 2;
		}
	}अन्यथा{
		यदि(db__ready_for_queue(context, qos, सन्देशः_दत्त)){
			state = mosq_ms_queued;
		}अन्यथा{
			metrics__int_inc(mosq_गणक_mqtt_publish_dropped, 1);
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

#यदि_परिभाषितम् WITH_PERSISTENCE
	यदि(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#यदि_अन्तः

	client_सन्देशः = mosquitto_malloc(परिमाण(संरचना mosquitto_client_सन्देशः));
	यदि(!client_सन्देशः){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	client_सन्देशः->पूर्व = रिक्त;
	client_सन्देशः->अग्र = रिक्त;
	यदि(cmsg_id){
		client_सन्देशः->दत्त.cmsg_id = cmsg_id;
	}अन्यथा{
		client_सन्देशः->दत्त.cmsg_id = ++context->चरमः_cmsg_id;
	}
	client_सन्देशः->आधारः_सन्देशः = आधारः_सन्देशः;
	db_सन्देशः_store_ref_inc(client_सन्देशः->आधारः_सन्देशः);
	client_सन्देशः->दत्त.mid = mid;
	client_सन्देशः->दत्त.direction = mosq_md_बहिर्गमनम्;
	client_सन्देशः->दत्त.state = (uint8_t)state;
	client_सन्देशः->दत्त.dup = false;
	यदि(qos > context->max_qos){
		client_सन्देशः->दत्त.qos = context->max_qos;
	}अन्यथा{
		client_सन्देशः->दत्त.qos = qos;
	}
	client_सन्देशः->दत्त.retain = retain;
	client_सन्देशः->दत्त.subscription_identifier = subscription_identifier;

	यदि(state == mosq_ms_queued){
		DL_APPEND(सन्देशः_दत्त->queued, client_सन्देशः);
		db_सन्देशः_सङ्कलय_गन्तव्यम्_queued_stats(सन्देशः_दत्त, client_सन्देशः);
	}अन्यथा{
		DL_APPEND(सन्देशः_दत्त->inflight, client_सन्देशः);
		db_सन्देशः_सङ्कलय_गन्तव्यम्_inflight_stats(सन्देशः_दत्त, client_सन्देशः);
	}

	यदि(persist && context->is_persisted){
		plugin_persist_handle_आधारः_सन्देशः_सङ्कलय(client_सन्देशः->आधारः_सन्देशः);
		plugin_persist_handle_client_सन्देशः_सङ्कलय(context, client_सन्देशः);
	}

	यदि(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(आधारः_सन्देशः->dest_ids, परिमाण(वर्ण *)*(size_t)(आधारः_सन्देशः->dest_id_गणनम्+1));
		यदि(dest_ids){
			आधारः_सन्देशः->dest_ids = dest_ids;
			आधारः_सन्देशः->dest_id_गणनम्++;
			आधारः_सन्देशः->dest_ids[आधारः_सन्देशः->dest_id_गणनम्-1] = mosquitto_strdup(context->id);
			यदि(!आधारः_सन्देशः->dest_ids[आधारः_सन्देशः->dest_id_गणनम्-1]){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
		}अन्यथा{
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}
#यदि_परिभाषितम् WITH_BRIDGE
	यदि(context->bridge && context->bridge->start_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_बहिर्गमनम्.inflight_गणनम् + context->msgs_बहिर्गमनम्.queued_गणनम् >= context->bridge->देहली){

		context->bridge->lazy_reconnect = true;
	}
#यदि_अन्तः

	यदि(client_सन्देशः->दत्त.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	यदि(update){
		rc = db_सन्देशः_write_inflight_बहिर्गमनम्_latest(context);
		यदि(rc){
			प्रत्यागम rc;
		}
		rc = db_सन्देशः_write_queued_बहिर्गमनम्(context);
		यदि(rc){
			प्रत्यागम rc;
		}
	}

	प्रत्यागम rc;
}


स्थिर अन्तःस्थ पूर्णाङ्क db_सन्देशः_update_outgoing_state(संरचना mosquitto *context, संरचना mosquitto_client_सन्देशः *शीर्ष,
		uint16_t mid, गणना mosquitto_सन्देशः_state state, पूर्णाङ्क qos, bool persist)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः;

	DL_FOREACH(शीर्ष, client_सन्देशः){
		यदि(client_सन्देशः->दत्त.mid == mid){
			यदि(client_सन्देशः->दत्त.qos != qos){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_सन्देशः->दत्त.qos, qos);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}
			client_सन्देशः->दत्त.state = (uint8_t)state;
			यदि(persist){
				plugin_persist_handle_client_सन्देशः_update(context, client_सन्देशः);
			}
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}
	}
	प्रत्यागम MOSQ_ERR_NOT_प्राप्तम्;
}


पूर्णाङ्क db_सन्देशः_update_outgoing(संरचना mosquitto *context, uint16_t mid, गणना mosquitto_सन्देशः_state state, पूर्णाङ्क qos, bool persist)
{
	पूर्णाङ्क rc;

	rc = db_सन्देशः_update_outgoing_state(context, context->msgs_बहिर्गमनम्.inflight, mid, state, qos, persist);
	यदि(!persist && rc == MOSQ_ERR_NOT_प्राप्तम्){
		rc = db_सन्देशः_update_outgoing_state(context, context->msgs_बहिर्गमनम्.queued, mid, state, qos, persist);
	}
	प्रत्यागम rc;
}


स्थिर शून्य db__messages_delete_list(संरचना mosquitto_client_सन्देशः **शीर्ष)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;

	DL_FOREACH_SAFE(*शीर्ष, client_सन्देशः, tmp){
		DL_DELETE(*शीर्ष, client_सन्देशः);
		db_सन्देशः_store_ref_dec(&client_सन्देशः->आधारः_सन्देशः);
		mosquitto_FREE(client_सन्देशः);
	}
	*शीर्ष = रिक्त;
}


पूर्णाङ्क db__messages_delete_incoming(संरचना mosquitto *context)
{
	यदि(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_अन्तर्.inflight);
	db__messages_delete_list(&context->msgs_अन्तर्.queued);
	context->msgs_अन्तर्.inflight_अष्टकानि = 0;
	context->msgs_अन्तर्.inflight_bytes12 = 0;
	context->msgs_अन्तर्.inflight_गणनम् = 0;
	context->msgs_अन्तर्.inflight_count12 = 0;
	context->msgs_अन्तर्.queued_अष्टकानि = 0;
	context->msgs_अन्तर्.queued_bytes12 = 0;
	context->msgs_अन्तर्.queued_गणनम् = 0;
	context->msgs_अन्तर्.queued_count12 = 0;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db__messages_delete_outgoing(संरचना mosquitto *context)
{
	यदि(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_बहिर्गमनम्.inflight);
	db__messages_delete_list(&context->msgs_बहिर्गमनम्.queued);
	context->msgs_बहिर्गमनम्.inflight_अष्टकानि = 0;
	context->msgs_बहिर्गमनम्.inflight_bytes12 = 0;
	context->msgs_बहिर्गमनम्.inflight_गणनम् = 0;
	context->msgs_बहिर्गमनम्.inflight_count12 = 0;
	context->msgs_बहिर्गमनम्.queued_अष्टकानि = 0;
	context->msgs_बहिर्गमनम्.queued_bytes12 = 0;
	context->msgs_बहिर्गमनम्.queued_गणनम् = 0;
	context->msgs_बहिर्गमनम्.queued_count12 = 0;

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


पूर्णाङ्क db__messages_easy_queue(संरचना mosquitto *context, ध्रुव वर्ण *topic, uint8_t qos, uint32_t payloadlen, ध्रुव शून्य *payload, पूर्णाङ्क retain, uint32_t सन्देशः_expiry_interval, mosquitto_property **properties)
{
	संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः;
	ध्रुव वर्ण *source_id;
	गणना mosquitto_सन्देशः_origin origin;

	यदि(!topic){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	आधारः_सन्देशः = mosquitto_calloc(1, परिमाण(संरचना mosquitto_आधारः_सन्देशः));
	यदि(आधारः_सन्देशः == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	आधारः_सन्देशः->दत्त.topic = mosquitto_strdup(topic);
	यदि(आधारः_सन्देशः->दत्त.topic == रिक्त){
		db_सन्देशः_store_free(आधारः_सन्देशः);
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	आधारः_सन्देशः->दत्त.qos = qos;
	यदि(db.config->retain_उपलब्धम् == false){
		आधारः_सन्देशः->दत्त.retain = 0;
	}अन्यथा{
		आधारः_सन्देशः->दत्त.retain = retain;
	}

	आधारः_सन्देशः->दत्त.payloadlen = payloadlen;
	यदि(payloadlen > 0){
		आधारः_सन्देशः->दत्त.payload = mosquitto_malloc(आधारः_सन्देशः->दत्त.payloadlen+1);
		यदि(आधारः_सन्देशः->दत्त.payload == रिक्त){
			db_सन्देशः_store_free(आधारः_सन्देशः);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)आधारः_सन्देशः->दत्त.payload)[आधारः_सन्देशः->दत्त.payloadlen] = 0;
		स्मृतिं_प्रतिलिख(आधारः_सन्देशः->दत्त.payload, payload, आधारः_सन्देशः->दत्त.payloadlen);
	}

	यदि(context && context->id){
		source_id = context->id;
	}अन्यथा{
		source_id = "";
	}
	यदि(properties){
		आधारः_सन्देशः->दत्त.properties = *properties;
		*properties = रिक्त;
	}

	यदि(context){
		origin = mosq_mo_client;
	}अन्यथा{
		origin = mosq_mo_broker;
	}
	यदि(db_सन्देशः_store(context, आधारः_सन्देशः, &सन्देशः_expiry_interval, origin)){
		प्रत्यागम 1;
	}

	प्रत्यागम sub__messages_queue(source_id, आधारः_सन्देशः->दत्त.qos, &आधारः_सन्देशः);
}


#परिभाषय MOSQ_UUID_EPOCH 1637168273


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
#यदि_परिभाषितम् WIN32
	FILETIME ftime;
	uint64_t ftime64;
#अन्यथा
	संरचना timespec ts;
#यदि_अन्तः
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	दीर्घ nsec;

	id = db.ग्रन्थिः_id_shifted; /* Top 10-bits */

#यदि_परिभाषितम् WIN32
	GetSystemTimePreciseAsFileTime(&ftime);
	ftime64 = (((uint64_t)ftime.dw_उच्चः_Date_Time)<<32) + ftime.dw_नीचः_Date_Time;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (दीर्घ)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#अन्यथा
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#यदि_अन्तः
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	यदि(id <= db.चरमः_db_id){
		id = db.चरमः_db_id + 1;
	}
	db.चरमः_db_id = id;

	प्रत्यागम id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
पूर्णाङ्क db_सन्देशः_store(ध्रुव संरचना mosquitto *source, संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः, uint32_t *सन्देशः_expiry_interval, गणना mosquitto_सन्देशः_origin origin)
{
	पूर्णाङ्क rc;

	प्रतिज्ञा(आधारः_सन्देशः);

	यदि(source && source->id){
		आधारः_सन्देशः->दत्त.source_id = mosquitto_strdup(source->id);
	}अन्यथा{
		आधारः_सन्देशः->दत्त.source_id = mosquitto_strdup("");
	}
	यदि(!आधारः_सन्देशः->दत्त.source_id){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_सन्देशः_store_free(आधारः_सन्देशः);
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	यदि(source && source->username){
		आधारः_सन्देशः->दत्त.source_username = mosquitto_strdup(source->username);
		यदि(!आधारः_सन्देशः->दत्त.source_username){
			db_सन्देशः_store_free(आधारः_सन्देशः);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}
	यदि(source){
		आधारः_सन्देशः->source_listener = source->listener;
	}
	आधारः_सन्देशः->origin = origin;
	यदि(सन्देशः_expiry_interval){
		यदि(*सन्देशः_expiry_interval > 0 && *सन्देशः_expiry_interval != सन्देशः_EXPIRY_INFINITE){
			आधारः_सन्देशः->दत्त.expiry_time = db.now_real_s + *सन्देशः_expiry_interval;
		}अन्यथा{
			आधारः_सन्देशः->दत्त.expiry_time = 0;
		}
	}

	आधारः_सन्देशः->dest_ids = रिक्त;
	आधारः_सन्देशः->dest_id_गणनम् = 0;
	db.सन्देशः_store_गणनम्++;
	db.सन्देशः_store_अष्टकानि += आधारः_सन्देशः->दत्त.payloadlen;

	यदि(!आधारः_सन्देशः->दत्त.store_id){
		आधारः_सन्देशः->दत्त.store_id = db_new_सन्देशः_id();
	}

	rc = db_सन्देशः_store_सङ्कलय(आधारः_सन्देशः);
	यदि(rc){
		db_सन्देशः_store_free(आधारः_सन्देशः);
		प्रत्यागम rc;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db_सन्देशः_store_find(संरचना mosquitto *context, uint16_t mid, संरचना mosquitto_client_सन्देशः **client_सन्देशः)
{
	संरचना mosquitto_client_सन्देशः *cmsg;

	*client_सन्देशः = रिक्त;

	यदि(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_अन्तर्.inflight, cmsg){
		यदि(cmsg->आधारः_सन्देशः->दत्त.source_mid == mid){
			*client_सन्देशः = cmsg;
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_अन्तर्.queued, cmsg){
		यदि(cmsg->आधारः_सन्देशः->दत्त.source_mid == mid){
			*client_सन्देशः = cmsg;
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}
	}

	प्रत्यागम 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
स्थिर पूर्णाङ्क db_सन्देशः_reconnect_reset_outgoing(संरचना mosquitto *context)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;

	context->msgs_बहिर्गमनम्.inflight_अष्टकानि = 0;
	context->msgs_बहिर्गमनम्.inflight_bytes12 = 0;
	context->msgs_बहिर्गमनम्.inflight_गणनम् = 0;
	context->msgs_बहिर्गमनम्.inflight_count12 = 0;
	context->msgs_बहिर्गमनम्.queued_अष्टकानि = 0;
	context->msgs_बहिर्गमनम्.queued_bytes12 = 0;
	context->msgs_बहिर्गमनम्.queued_गणनम् = 0;
	context->msgs_बहिर्गमनम्.queued_count12 = 0;
	context->msgs_बहिर्गमनम्.inflight_quota = context->msgs_बहिर्गमनम्.inflight_अधिकतमम्;

	DL_FOREACH_SAFE(context->msgs_बहिर्गमनम्.inflight, client_सन्देशः, tmp){
		db_सन्देशः_सङ्कलय_गन्तव्यम्_inflight_stats(&context->msgs_बहिर्गमनम्, client_सन्देशः);
		यदि(client_सन्देशः->दत्त.qos > 0){
			util__decrement_send_quota(context);
		}

		विकल्प(client_सन्देशः->दत्त.qos){
			पक्ष 0:
				client_सन्देशः->दत्त.state = mosq_ms_publish_qos0;
				भङ्ग;
			पक्ष 1:
				client_सन्देशः->दत्त.state = mosq_ms_publish_qos1;
				भङ्ग;
			पक्ष 2:
				यदि(client_सन्देशः->दत्त.state == mosq_ms_wait_for_pubcomp){
					client_सन्देशः->दत्त.state = mosq_ms_resend_pubrel;
				}अन्यथा{
					client_सन्देशः->दत्त.state = mosq_ms_publish_qos2;
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
	DL_FOREACH_SAFE(context->msgs_बहिर्गमनम्.queued, client_सन्देशः, tmp){
		db_सन्देशः_सङ्कलय_गन्तव्यम्_queued_stats(&context->msgs_बहिर्गमनम्, client_सन्देशः);
	}
	db_fill_inflight_बहिर्गमनम्_from_queue(context);

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
स्थिर पूर्णाङ्क db_सन्देशः_reconnect_reset_incoming(संरचना mosquitto *context)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;

	context->msgs_अन्तर्.inflight_अष्टकानि = 0;
	context->msgs_अन्तर्.inflight_bytes12 = 0;
	context->msgs_अन्तर्.inflight_गणनम् = 0;
	context->msgs_अन्तर्.inflight_count12 = 0;
	context->msgs_अन्तर्.queued_अष्टकानि = 0;
	context->msgs_अन्तर्.queued_bytes12 = 0;
	context->msgs_अन्तर्.queued_गणनम् = 0;
	context->msgs_अन्तर्.queued_count12 = 0;
	context->msgs_अन्तर्.inflight_quota = context->msgs_अन्तर्.inflight_अधिकतमम्;

	DL_FOREACH_SAFE(context->msgs_अन्तर्.inflight, client_सन्देशः, tmp){
		db_सन्देशः_सङ्कलय_गन्तव्यम्_inflight_stats(&context->msgs_अन्तर्, client_सन्देशः);
		यदि(client_सन्देशः->दत्त.qos > 0){
			util__decrement_receive_quota(context);
		}

		यदि(client_सन्देशः->दत्त.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db_सन्देशः_remove_inflight(context, &context->msgs_अन्तर्, client_सन्देशः);
		}अन्यथा{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_सन्देशः->दत्त.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_SAFE(context->msgs_अन्तर्.queued, client_सन्देशः, tmp){
		client_सन्देशः->दत्त.dup = 0;
		db_सन्देशः_सङ्कलय_गन्तव्यम्_queued_stats(&context->msgs_अन्तर्, client_सन्देशः);
		यदि(db__ready_for_flight(context, mosq_md_अन्तर्, client_सन्देशः->दत्त.qos)){
			विकल्प(client_सन्देशः->दत्त.qos){
				पक्ष 0:
					client_सन्देशः->दत्त.state = mosq_ms_publish_qos0;
					भङ्ग;
				पक्ष 1:
					client_सन्देशः->दत्त.state = mosq_ms_publish_qos1;
					भङ्ग;
				पक्ष 2:
					client_सन्देशः->दत्त.state = mosq_ms_publish_qos2;
					भङ्ग;
			}
			db_सन्देशः_पङ्क्तेः_निष्कासय_first(context, &context->msgs_अन्तर्);
			plugin_persist_handle_client_सन्देशः_update(context, client_सन्देशः);
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db_सन्देशः_reconnect_reset(संरचना mosquitto *context)
{
	पूर्णाङ्क rc;

	rc = db_सन्देशः_reconnect_reset_outgoing(context);
	यदि(rc){
		प्रत्यागम rc;
	}
	प्रत्यागम db_सन्देशः_reconnect_reset_incoming(context);
}


पूर्णाङ्क db_सन्देशः_remove_incoming(संरचना mosquitto *context, uint16_t mid)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;

	यदि(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_अन्तर्.inflight, client_सन्देशः, tmp){
		यदि(client_सन्देशः->दत्त.mid == mid){
			यदि(client_सन्देशः->आधारः_सन्देशः->दत्त.qos != 2){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_सन्देशः->आधारः_सन्देशः->दत्त.qos);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}
			db_सन्देशः_remove_inflight(context, &context->msgs_अन्तर्, client_सन्देशः);
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}
	}

	प्रत्यागम MOSQ_ERR_NOT_प्राप्तम्;
}


पूर्णाङ्क db_सन्देशः_release_incoming(संरचना mosquitto *context, uint16_t mid)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;
	bool deleted = false;
	पूर्णाङ्क rc;

	यदि(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	DL_FOREACH_SAFE(context->msgs_अन्तर्.inflight, client_सन्देशः, tmp){
		यदि(client_सन्देशः->दत्त.mid == mid){
			यदि(client_सन्देशः->आधारः_सन्देशः->दत्त.qos != 2){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_सन्देशः->आधारः_सन्देशः->दत्त.qos);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}
			ध्रुव वर्ण *topic = client_सन्देशः->आधारः_सन्देशः->दत्त.topic;
			ध्रुव वर्ण *source_id = client_सन्देशः->आधारः_सन्देशः->दत्त.source_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			यदि(topic == रिक्त){
				db_सन्देशः_remove_inflight(context, &context->msgs_अन्तर्, client_सन्देशः);
				deleted = true;
			}अन्यथा{
				rc = sub__messages_queue(source_id, 2, &client_सन्देशः->आधारः_सन्देशः);
				यदि(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db_सन्देशः_remove_inflight(context, &context->msgs_अन्तर्, client_सन्देशः);
					deleted = true;
				}अन्यथा{
					प्रत्यागम 1;
				}
			}
		}
	}

	DL_FOREACH_SAFE(context->msgs_अन्तर्.queued, client_सन्देशः, tmp){
		यदि(db__ready_for_flight(context, mosq_md_अन्तर्, client_सन्देशः->दत्त.qos)){
			भङ्ग;
		}

		यदि(client_सन्देशः->दत्त.qos == 2){
			send__pubrec(context, client_सन्देशः->दत्त.mid, 0, रिक्त);
			client_सन्देशः->दत्त.state = mosq_ms_wait_for_pubrel;
			db_सन्देशः_पङ्क्तेः_निष्कासय_first(context, &context->msgs_अन्तर्);
			plugin_persist_handle_client_सन्देशः_update(context, client_सन्देशः);
		}
	}
	यदि(deleted){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अन्यथा{
		प्रत्यागम MOSQ_ERR_NOT_प्राप्तम्;
	}
}


शून्य db__expire_all_messages(संरचना mosquitto *context)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;

	DL_FOREACH_SAFE(context->msgs_बहिर्गमनम्.inflight, client_सन्देशः, tmp){
		यदि(client_सन्देशः->आधारः_सन्देशः->दत्त.expiry_time && db.now_real_s > client_सन्देशः->आधारः_सन्देशः->दत्त.expiry_time){
			यदि(client_सन्देशः->दत्त.qos > 0){
				util__increment_send_quota(context);
			}
			db_सन्देशः_remove_inflight(context, &context->msgs_बहिर्गमनम्, client_सन्देशः);
		}
	}
	db_fill_inflight_बहिर्गमनम्_from_queue(context);
	DL_FOREACH_SAFE(context->msgs_बहिर्गमनम्.queued, client_सन्देशः, tmp){
		यदि(client_सन्देशः->आधारः_सन्देशः->दत्त.expiry_time && db.now_real_s > client_सन्देशः->आधारः_सन्देशः->दत्त.expiry_time){
			db_सन्देशः_remove_queued(context, &context->msgs_बहिर्गमनम्, client_सन्देशः);
		}
	}
	DL_FOREACH_SAFE(context->msgs_अन्तर्.inflight, client_सन्देशः, tmp){
		यदि(client_सन्देशः->आधारः_सन्देशः->दत्त.expiry_time && db.now_real_s > client_सन्देशः->आधारः_सन्देशः->दत्त.expiry_time){
			यदि(client_सन्देशः->दत्त.qos > 0){
				util__increment_receive_quota(context);
			}
			db_सन्देशः_remove_inflight(context, &context->msgs_अन्तर्, client_सन्देशः);
		}
	}
	DL_FOREACH_SAFE(context->msgs_अन्तर्.queued, client_सन्देशः, tmp){
		यदि(client_सन्देशः->आधारः_सन्देशः->दत्त.expiry_time && db.now_real_s > client_सन्देशः->आधारः_सन्देशः->दत्त.expiry_time){
			db_सन्देशः_remove_queued(context, &context->msgs_अन्तर्, client_सन्देशः);
		}
	}
}


स्थिर शून्य db__client_messages_check_acl(संरचना mosquitto *context, संरचना mosquitto_सन्देशः_दत्त *सन्देशः_दत्त, संरचना mosquitto_client_सन्देशः **शीर्ष,
		शून्य (*decrement_stats_fn)(संरचना mosquitto_सन्देशः_दत्त *सन्देशः_दत्त, संरचना mosquitto_client_सन्देशः *client_सन्देशः))
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;
	संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः;
	पूर्णाङ्क access;

	DL_FOREACH_SAFE((*शीर्ष), client_सन्देशः, tmp){
		आधारः_सन्देशः = client_सन्देशः->आधारः_सन्देशः;
		यदि(client_सन्देशः->दत्त.direction == mosq_md_बहिर्गमनम्){
			access = MOSQ_ACL_READ;
		}अन्यथा{
			access = MOSQ_ACL_WRITE;
		}
		यदि(mosquitto_acl_check(context, आधारः_सन्देशः->दत्त.topic,
				आधारः_सन्देशः->दत्त.payloadlen, आधारः_सन्देशः->दत्त.payload,
				आधारः_सन्देशः->दत्त.qos, आधारः_सन्देशः->दत्त.retain,
				आधारः_सन्देशः->दत्त.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*शीर्ष), client_सन्देशः);
			decrement_stats_fn(सन्देशः_दत्त, client_सन्देशः);
			plugin_persist_handle_client_सन्देशः_delete(context, client_सन्देशः);
			db_सन्देशः_store_ref_dec(&client_सन्देशः->आधारः_सन्देशः);
			mosquitto_FREE(client_सन्देशः);
		}
	}
}


शून्य db_check_acl_सम्बन्धि_all_messages(संरचना mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_अन्तर्, &context->msgs_अन्तर्.inflight, &db_सन्देशः_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_अन्तर्, &context->msgs_अन्तर्.queued, &db_सन्देशः_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_बहिर्गमनम्, &context->msgs_बहिर्गमनम्.inflight, &db_सन्देशः_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_बहिर्गमनम्, &context->msgs_बहिर्गमनम्.queued, &db_सन्देशः_remove_from_queued_stats);
}


स्थिर पूर्णाङ्क db_सन्देशः_write_inflight_बहिर्गमनम्_single(संरचना mosquitto *context, संरचना mosquitto_client_सन्देशः *client_सन्देशः)
{
	संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः;
	mosquitto_property *आधारः_सन्देशः_props = रिक्त;
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

	आधारः_सन्देशः = client_सन्देशः->आधारः_सन्देशः;

	expiry_interval = 0;
	यदि(आधारः_सन्देशः->दत्त.expiry_time){
		यदि(db.now_real_s > आधारः_सन्देशः->दत्त.expiry_time){
			/* Message is expired, must not send. */
			यदि(client_सन्देशः->दत्त.direction == mosq_md_बहिर्गमनम् && client_सन्देशः->दत्त.qos > 0){
				util__increment_send_quota(context);
			}
			db_सन्देशः_remove_inflight(context, &context->msgs_बहिर्गमनम्, client_सन्देशः);
			db_fill_inflight_बहिर्गमनम्_from_queue(context);
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}अन्यथा{
			expiry_interval = (uint32_t)(आधारः_सन्देशः->दत्त.expiry_time - db.now_real_s);
		}
	}
	mid = client_सन्देशः->दत्त.mid;
	retries = client_सन्देशः->दत्त.dup;
	retain = client_सन्देशः->दत्त.retain;
	topic = आधारः_सन्देशः->दत्त.topic;
	qos = (uint8_t)client_सन्देशः->दत्त.qos;
	payloadlen = आधारः_सन्देशः->दत्त.payloadlen;
	payload = आधारः_सन्देशः->दत्त.payload;
	subscription_id = client_सन्देशः->दत्त.subscription_identifier;
	आधारः_सन्देशः_props = आधारः_सन्देशः->दत्त.properties;

	विकल्प(client_सन्देशः->दत्त.state){
		पक्ष mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, आधारः_सन्देशः_props, expiry_interval);
			यदि(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_सन्देशः_remove_inflight(context, &context->msgs_बहिर्गमनम्, client_सन्देशः);
			}अन्यथा{
				प्रत्यागम rc;
			}
			भङ्ग;

		पक्ष mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, आधारः_सन्देशः_props, expiry_interval);
			यदि(rc == MOSQ_ERR_SUCCESS){
				client_सन्देशः->दत्त.dup = 1; /* Any retry attempts are a duplicate. */
				client_सन्देशः->दत्त.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_सन्देशः_update(context, client_सन्देशः);
			}अन्यथा यदि(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_सन्देशः_remove_inflight(context, &context->msgs_बहिर्गमनम्, client_सन्देशः);
			}अन्यथा{
				प्रत्यागम rc;
			}
			भङ्ग;

		पक्ष mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, आधारः_सन्देशः_props, expiry_interval);
			यदि(rc == MOSQ_ERR_SUCCESS){
				client_सन्देशः->दत्त.dup = 1; /* Any retry attempts are a duplicate. */
				client_सन्देशः->दत्त.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_सन्देशः_update(context, client_सन्देशः);
			}अन्यथा यदि(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_सन्देशः_remove_inflight(context, &context->msgs_बहिर्गमनम्, client_सन्देशः);
			}अन्यथा{
				प्रत्यागम rc;
			}
			भङ्ग;

		पक्ष mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, रिक्त);
			यदि(!rc){
				client_सन्देशः->दत्त.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_सन्देशः_update(context, client_सन्देशः);
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


पूर्णाङ्क db_सन्देशः_write_inflight_बहिर्गमनम्_all(संरचना mosquitto *context)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;
	पूर्णाङ्क rc;

	यदि(context->state != mosq_cs_सक्रिय || !net__is_connected(context)){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_बहिर्गमनम्.inflight, client_सन्देशः, tmp){
		rc = db_सन्देशः_write_inflight_बहिर्गमनम्_single(context, client_सन्देशः);
		यदि(rc){
			प्रत्यागम rc;
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db_सन्देशः_write_inflight_बहिर्गमनम्_latest(संरचना mosquitto *context)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *अग्र;
	पूर्णाङ्क rc;

	यदि(context->state != mosq_cs_सक्रिय
			|| !net__is_connected(context)
			|| context->msgs_बहिर्गमनम्.inflight == रिक्त){

		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	यदि(context->msgs_बहिर्गमनम्.inflight->पूर्व == context->msgs_बहिर्गमनम्.inflight){
		/* Only one message */
		प्रत्यागम db_सन्देशः_write_inflight_बहिर्गमनम्_single(context, context->msgs_बहिर्गमनम्.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_सन्देशः = context->msgs_बहिर्गमनम्.inflight->पूर्व;
	यावत्(client_सन्देशः != context->msgs_बहिर्गमनम्.inflight &&
			(client_सन्देशः->दत्त.state == mosq_ms_publish_qos0
			|| client_सन्देशः->दत्त.state == mosq_ms_publish_qos1
			|| client_सन्देशः->दत्त.state == mosq_ms_publish_qos2)){

		client_सन्देशः = client_सन्देशः->पूर्व;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	यदि(client_सन्देशः != context->msgs_बहिर्गमनम्.inflight){
		client_सन्देशः = client_सन्देशः->अग्र;
	}

	यावत्(client_सन्देशः){
		अग्र = client_सन्देशः->अग्र;
		rc = db_सन्देशः_write_inflight_बहिर्गमनम्_single(context, client_सन्देशः);
		यदि(rc){
			प्रत्यागम rc;
		}
		client_सन्देशः = अग्र;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db_सन्देशः_write_queued_अन्तर्(संरचना mosquitto *context)
{
	संरचना mosquitto_client_सन्देशः *client_सन्देशः, *tmp;
	पूर्णाङ्क rc;

	यदि(context->state != mosq_cs_सक्रिय){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_SAFE(context->msgs_अन्तर्.queued, client_सन्देशः, tmp){
		यदि(context->msgs_अन्तर्.inflight_अधिकतमम् != 0 && context->msgs_अन्तर्.inflight_quota == 0){
			भङ्ग;
		}

		यदि(client_सन्देशः->दत्त.qos == 2){
			client_सन्देशः->दत्त.state = mosq_ms_send_pubrec;
			db_सन्देशः_पङ्क्तेः_निष्कासय_first(context, &context->msgs_अन्तर्);
			rc = send__pubrec(context, client_सन्देशः->दत्त.mid, 0, रिक्त);
			यदि(!rc){
				client_सन्देशः->दत्त.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_सन्देशः_update(context, client_सन्देशः);
			}अन्यथा{
				plugin_persist_handle_client_सन्देशः_update(context, client_सन्देशः);
				प्रत्यागम rc;
			}
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क db_सन्देशः_write_queued_बहिर्गमनम्(संरचना mosquitto *context)
{
	यदि(context->state != mosq_cs_सक्रिय){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	db_fill_inflight_बहिर्गमनम्_from_queue(context);

	प्रत्यागम MOSQ_ERR_SUCCESS;
}
