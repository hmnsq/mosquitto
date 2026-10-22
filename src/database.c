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

#అన్తర్భావ "config.h"

#అన్తర్భావ <assert.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <utlist.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "send_mosq.h"
#అన్తర్భావ "sys_tree.h"
#అన్తర్భావ "util_mosq.h"


/**
 * Is this context ready to take more in flight messages right now?
 * @param context the client context of interest
 * @param qos qos for the packet of interest
 * @return true if more in flight are allowed.
 */
bool db__ready_for_flight(నిర్మాణం mosquitto *context, గణన mosquitto_సందేశం_direction dir, పూర్ణాంకం qos)
{
	నిర్మాణం mosquitto_సందేశం_దత్తాంశం *msgs;
	bool valid_బైట్లు;
	bool valid_లెక్క;

	ఒకవేళ(dir == mosq_md_బహిర్గతం){
		msgs = &context->msgs_బహిర్గతం;
	}లేకపోతే{
		msgs = &context->msgs_లోపల;
	}

	ఒకవేళ(msgs->inflight_గరిష్ఠ == 0 && db.config->max_inflight_బైట్లు == 0){
		తిరిగి true;
	}

	ఒకవేళ(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		ఒకవేళ(db.config->max_queued_messages == 0 && db.config->max_inflight_బైట్లు == 0){
			తిరిగి true;
		}
		valid_బైట్లు = ((msgs->inflight_బైట్లు - (ssize_t)db.config->max_inflight_బైట్లు) < (ssize_t)db.config->max_queued_బైట్లు);
		ఒకవేళ(dir == mosq_md_బహిర్గతం){
			valid_లెక్క = context->బహిర్గతం_packet_లెక్క < db.config->max_queued_messages;
		}లేకపోతే{
			valid_లెక్క = msgs->inflight_లెక్క - msgs->inflight_గరిష్ఠ < db.config->max_queued_messages;
		}

		ఒకవేళ(db.config->max_queued_messages == 0){
			తిరిగి valid_బైట్లు;
		}
		ఒకవేళ(db.config->max_queued_బైట్లు == 0){
			తిరిగి valid_లెక్క;
		}
	}లేకపోతే{
		valid_బైట్లు = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_బైట్లు;
		valid_లెక్క = msgs->inflight_quota > 0;

		ఒకవేళ(msgs->inflight_గరిష్ఠ == 0){
			తిరిగి valid_బైట్లు;
		}
		ఒకవేళ(db.config->max_inflight_బైట్లు == 0){
			తిరిగి valid_లెక్క;
		}
	}

	తిరిగి valid_బైట్లు && valid_లెక్క;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db_ready_for_వరుస(నిర్మాణం mosquitto *context, పూర్ణాంకం qos, నిర్మాణం mosquitto_సందేశం_దత్తాంశం *సందేశం_దత్తాంశం)
{
	పూర్ణాంకం ఆధారం_లెక్క;
	పూర్ణాంకం adjust_లెక్క;
	దీర్ఘ ఆధారం_బైట్లు;
	ssize_t adjust_బైట్లు = (ssize_t)db.config->max_inflight_బైట్లు;
	bool valid_బైట్లు;
	bool valid_లెక్క;

	ఒకవేళ(db.config->max_queued_messages == 0 && db.config->max_queued_బైట్లు == 0){
		తిరిగి true;
	}

	ఒకవేళ(qos == 0 && db.config->వరుస_qos0_messages == false){
		తిరిగి false; /* This case is handled in db__ready_for_flight() */
	}లేకపోతే{
		ఆధారం_బైట్లు = (ssize_t)సందేశం_దత్తాంశం->queued_bytes12;
		ఆధారం_లెక్క = సందేశం_దత్తాంశం->queued_count12;
	}
	adjust_లెక్క = సందేశం_దత్తాంశం->inflight_గరిష్ఠ;

	/* nothing in flight for offline clients */
	ఒకవేళ(!net__is_connected(context)){
		adjust_బైట్లు = 0;
		adjust_లెక్క = 0;
	}

	valid_బైట్లు = (ఆధారం_బైట్లు - (ssize_t)adjust_బైట్లు) < (ssize_t)db.config->max_queued_బైట్లు;
	valid_లెక్క = ఆధారం_లెక్క - adjust_లెక్క < db.config->max_queued_messages;

	ఒకవేళ(db.config->max_queued_బైట్లు == 0){
		తిరిగి valid_లెక్క;
	}
	ఒకవేళ(db.config->max_queued_messages == 0){
		తిరిగి valid_బైట్లు;
	}

	తిరిగి valid_బైట్లు && valid_లెక్క;
}


శూన్య db_సందేశం_కూడు_గమ్యం_inflight_stats(నిర్మాణం mosquitto_సందేశం_దత్తాంశం *సందేశం_దత్తాంశం, నిర్మాణం mosquitto_client_సందేశం *client_సందేశం)
{
	సందేశం_దత్తాంశం->inflight_లెక్క++;
	సందేశం_దత్తాంశం->inflight_బైట్లు += client_సందేశం->పునాది_సందేశం->దత్తాంశం.payloadlen;
	ఒకవేళ(client_సందేశం->దత్తాంశం.qos != 0){
		సందేశం_దత్తాంశం->inflight_count12++;
		సందేశం_దత్తాంశం->inflight_bytes12 += client_సందేశం->పునాది_సందేశం->దత్తాంశం.payloadlen;
	}
}


స్థిర శూన్య db_సందేశం_remove_from_inflight_stats(నిర్మాణం mosquitto_సందేశం_దత్తాంశం *సందేశం_దత్తాంశం, నిర్మాణం mosquitto_client_సందేశం *client_సందేశం)
{
	సందేశం_దత్తాంశం->inflight_లెక్క--;
	సందేశం_దత్తాంశం->inflight_బైట్లు -= client_సందేశం->పునాది_సందేశం->దత్తాంశం.payloadlen;
	ఒకవేళ(client_సందేశం->దత్తాంశం.qos != 0){
		సందేశం_దత్తాంశం->inflight_count12--;
		సందేశం_దత్తాంశం->inflight_bytes12 -= client_సందేశం->పునాది_సందేశం->దత్తాంశం.payloadlen;
	}
}


శూన్య db_సందేశం_కూడు_గమ్యం_queued_stats(నిర్మాణం mosquitto_సందేశం_దత్తాంశం *సందేశం_దత్తాంశం, నిర్మాణం mosquitto_client_సందేశం *client_సందేశం)
{
	సందేశం_దత్తాంశం->queued_లెక్క++;
	సందేశం_దత్తాంశం->queued_బైట్లు += client_సందేశం->పునాది_సందేశం->దత్తాంశం.payloadlen;
	ఒకవేళ(client_సందేశం->దత్తాంశం.qos != 0){
		సందేశం_దత్తాంశం->queued_count12++;
		సందేశం_దత్తాంశం->queued_bytes12 += client_సందేశం->పునాది_సందేశం->దత్తాంశం.payloadlen;
	}
}


స్థిర శూన్య db_సందేశం_remove_from_queued_stats(నిర్మాణం mosquitto_సందేశం_దత్తాంశం *సందేశం_దత్తాంశం, నిర్మాణం mosquitto_client_సందేశం *client_సందేశం)
{
	సందేశం_దత్తాంశం->queued_లెక్క--;
	సందేశం_దత్తాంశం->queued_బైట్లు -= client_సందేశం->పునాది_సందేశం->దత్తాంశం.payloadlen;
	ఒకవేళ(client_సందేశం->దత్తాంశం.qos != 0){
		సందేశం_దత్తాంశం->queued_count12--;
		సందేశం_దత్తాంశం->queued_bytes12 -= client_సందేశం->పునాది_సందేశం->దత్తాంశం.payloadlen;
	}
}


పూర్ణాంకం db__open(నిర్మాణం mosquitto__config *config)
{
	ఒకవేళ(!config){
		తిరిగి MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = ఖాళీ;
	db.contexts_by_sock = ఖాళీ;
	db.contexts_for_free = ఖాళీ;
#నిర్వచితమైతే WITH_BRIDGE
	db.bridges = ఖాళీ;
	db.bridge_లెక్క = 0;
#అయితే_ముగింపు

	/* Initialize the hashtable */
	db.clientid_సూచీ_hash = ఖాళీ;

	db.సాధారణ_subs = ఖాళీ;
	db.shared_subs = ఖాళీ;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = ఖాళీ;

#నిర్వచితమైతే WITH_PERSISTENCE
	ఒకవేళ(persist__restore()){
		తిరిగి 1;
	}
#అయితే_ముగింపు

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర శూన్య subhier_clean(నిర్మాణం mosquitto__subhier **subhier)
{
	నిర్మాణం mosquitto__subhier *peer, *subhier_tmp;
	నిర్మాణం mosquitto__subleaf *ఆకు, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		ఆకు = peer->subs;
		అయితే(ఆకు){
			nextleaf = ఆకు->తదుపరి;
			mosquitto_FREE(ఆకు);
			ఆకు = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


పూర్ణాంకం db__close(శూన్య)
{
	subhier_clean(&db.సాధారణ_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db_సందేశం_store_clean();

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం db_సందేశం_store_కూడు(నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం)
{
	నిర్మాణం mosquitto_పునాది_సందేశం *దొరికింది;
	గుర్తులేని hashv;

	HASH_విలువ(&పునాది_సందేశం->దత్తాంశం.store_id, పరిమాణం(పునాది_సందేశం->దత్తాంశం.store_id), hashv);
	HASH_కనుగొను_BYHASHVALUE(hh, db.సందేశం_store, &పునాది_సందేశం->దత్తాంశం.store_id, పరిమాణం(పునాది_సందేశం->దత్తాంశం.store_id), hashv, దొరికింది);
	ఒకవేళ(దొరికింది == ఖాళీ){
		HASH_కూడు_KEYPTR_BYHASHVALUE(hh, db.సందేశం_store, &పునాది_సందేశం->దత్తాంశం.store_id, పరిమాణం(పునాది_సందేశం->దత్తాంశం.store_id), hashv, పునాది_సందేశం);
		తిరిగి MOSQ_ERR_SUCCESS;
	}లేకపోతే{
		తిరిగి MOSQ_ERR_ALREADY_EXISTS;
	}
}


శూన్య db_సందేశం_store_free(నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం)
{
	mosquitto_FREE(పునాది_సందేశం->దత్తాంశం.ఆధారం_id);
	mosquitto_FREE(పునాది_సందేశం->దత్తాంశం.ఆధారం_username);
	ఒకవేళ(పునాది_సందేశం->dest_ids){
		కొరకు(పూర్ణాంకం i=0; i<పునాది_సందేశం->dest_id_లెక్క; i++){
			mosquitto_FREE(పునాది_సందేశం->dest_ids[i]);
		}
		mosquitto_FREE(పునాది_సందేశం->dest_ids);
	}
	mosquitto_FREE(పునాది_సందేశం->దత్తాంశం.topic);
	mosquitto_property_free_all(&పునాది_సందేశం->దత్తాంశం.properties);
	mosquitto_FREE(పునాది_సందేశం->దత్తాంశం.payload);
	mosquitto_FREE(పునాది_సందేశం);
}


శూన్య db_సందేశం_store_remove(నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం, bool notify)
{
	ఒకవేళ(పునాది_సందేశం == ఖాళీ){
		తిరిగి;
	}
	HASH_DELETE(hh, db.సందేశం_store, పునాది_సందేశం);
	db.సందేశం_store_లెక్క--;
	db.సందేశం_store_బైట్లు -= పునాది_సందేశం->దత్తాంశం.payloadlen;
	ఒకవేళ(notify == true){
		plugin_persist_handle_పునాది_సందేశం_delete(పునాది_సందేశం);
	}
	db_సందేశం_store_free(పునాది_సందేశం);
}


శూన్య db_సందేశం_store_clean(శూన్య)
{
	నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం, *పునాది_సందేశం_tmp;

	HASH_ITER(hh, db.సందేశం_store, పునాది_సందేశం, పునాది_సందేశం_tmp){
		db_సందేశం_store_remove(పునాది_సందేశం, false);
	}
}


శూన్య db_సందేశం_store_ref_inc(నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం)
{
	పునాది_సందేశం->ref_లెక్క++;
}


శూన్య db_సందేశం_store_ref_dec(నిర్మాణం mosquitto_పునాది_సందేశం **పునాది_సందేశం)
{
	(*పునాది_సందేశం)->ref_లెక్క--;
	ఒకవేళ((*పునాది_సందేశం)->ref_లెక్క == 0){
		db_సందేశం_store_remove(*పునాది_సందేశం, true);
		*పునాది_సందేశం = ఖాళీ;
	}
}


శూన్య db_సందేశం_store_compact(శూన్య)
{
	నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం, *పునాది_సందేశం_tmp;

	HASH_ITER(hh, db.సందేశం_store, పునాది_సందేశం, పునాది_సందేశం_tmp){
		ఒకవేళ(పునాది_సందేశం->ref_లెక్క < 1){
			db_సందేశం_store_remove(పునాది_సందేశం, true);
		}
	}
}


స్థిర శూన్య db_సందేశం_remove_inflight(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_సందేశం_దత్తాంశం *సందేశం_దత్తాంశం, నిర్మాణం mosquitto_client_సందేశం *అంశం)
{
	ఒకవేళ(!context || !సందేశం_దత్తాంశం || !అంశం){
		తిరిగి;
	}

	plugin_persist_handle_client_సందేశం_delete(context, అంశం);

	DL_DELETE(సందేశం_దత్తాంశం->inflight, అంశం);
	ఒకవేళ(అంశం->పునాది_సందేశం){
		db_సందేశం_remove_from_inflight_stats(సందేశం_దత్తాంశం, అంశం);
		db_సందేశం_store_ref_dec(&అంశం->పునాది_సందేశం);
	}

	mosquitto_FREE(అంశం);
}


స్థిర శూన్య db_సందేశం_remove_queued(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_సందేశం_దత్తాంశం *సందేశం_దత్తాంశం, నిర్మాణం mosquitto_client_సందేశం *అంశం)
{
	ఒకవేళ(!context || !సందేశం_దత్తాంశం || !అంశం){
		తిరిగి;
	}

	plugin_persist_handle_client_సందేశం_delete(context, అంశం);

	DL_DELETE(సందేశం_దత్తాంశం->queued, అంశం);
	ఒకవేళ(అంశం->పునాది_సందేశం){
		db_సందేశం_remove_from_queued_stats(సందేశం_దత్తాంశం, అంశం);
		db_సందేశం_store_ref_dec(&అంశం->పునాది_సందేశం);
	}

	mosquitto_FREE(అంశం);
}


స్థిర శూన్య db_fill_inflight_బహిర్గతం_from_వరుస(నిర్మాణం mosquitto *context)
{
	నిర్మాణం mosquitto_client_సందేశం *client_సందేశం, *tmp;

	DL_FOREACH_సురక్షిత(context->msgs_బహిర్గతం.queued, client_సందేశం, tmp){
		ఒకవేళ(!db__ready_for_flight(context, mosq_md_బహిర్గతం, client_సందేశం->దత్తాంశం.qos)){
			తిరిగి;
		}
		ఎంపిక(client_సందేశం->దత్తాంశం.qos){
			స్థితి 0:
				client_సందేశం->దత్తాంశం.state = mosq_ms_publish_qos0;
				ఆపు;
			స్థితి 1:
				client_సందేశం->దత్తాంశం.state = mosq_ms_publish_qos1;
				ఆపు;
			స్థితి 2:
				client_సందేశం->దత్తాంశం.state = mosq_ms_publish_qos2;
				ఆపు;
		}
		ఒకవేళ(client_సందేశం->పునాది_సందేశం->దత్తాంశం.expiry_సమయం && db.now_real_s > client_సందేశం->పునాది_సందేశం->దత్తాంశం.expiry_సమయం){
			db_సందేశం_remove_queued(context, &context->msgs_బహిర్గతం, client_సందేశం);
			కొనసాగు;
		}
		plugin_persist_handle_client_సందేశం_update(context, client_సందేశం);
		db_సందేశం_వరుస_నుండి_తీయి_మొదటి(context, &context->msgs_బహిర్గతం);
	}
}


శూన్య db_సందేశం_వరుస_నుండి_తీయి_మొదటి(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_సందేశం_దత్తాంశం *సందేశం_దత్తాంశం)
{
	నిర్మాణం mosquitto_client_సందేశం *client_సందేశం;

	వాడని(context);

	client_సందేశం = సందేశం_దత్తాంశం->queued;
	DL_DELETE(సందేశం_దత్తాంశం->queued, client_సందేశం);
	DL_APPEND(సందేశం_దత్తాంశం->inflight, client_సందేశం);
	ఒకవేళ(సందేశం_దత్తాంశం->inflight_quota > 0){
		సందేశం_దత్తాంశం->inflight_quota--;
	}

	db_సందేశం_remove_from_queued_stats(సందేశం_దత్తాంశం, client_సందేశం);
	db_సందేశం_కూడు_గమ్యం_inflight_stats(సందేశం_దత్తాంశం, client_సందేశం);
}


పూర్ణాంకం db_సందేశం_delete_outgoing(నిర్మాణం mosquitto *context, uint16_t mid, గణన mosquitto_సందేశం_state expect_state, పూర్ణాంకం qos)
{
	నిర్మాణం mosquitto_client_సందేశం *client_సందేశం, *tmp;
	bool deleted = false;

	ఒకవేళ(!context){
		తిరిగి MOSQ_ERR_INVAL;
	}

	DL_FOREACH_సురక్షిత(context->msgs_బహిర్గతం.inflight, client_సందేశం, tmp){
		ఒకవేళ(client_సందేశం->దత్తాంశం.mid == mid){
			ఒకవేళ(client_సందేశం->దత్తాంశం.qos != qos){
				log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_సందేశం->దత్తాంశం.qos, qos);
				తిరిగి MOSQ_ERR_PROTOCOL;
			}లేకపోతే ఒకవేళ(qos == 2 && client_సందేశం->దత్తాంశం.state != expect_state && expect_state != mosq_ms_any){
				log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_సందేశం->దత్తాంశం.state, expect_state);
				తిరిగి MOSQ_ERR_PROTOCOL;
			}
			db_సందేశం_remove_inflight(context, &context->msgs_బహిర్గతం, client_సందేశం);
			deleted = true;
			ఆపు;
		}
	}

	ఒకవేళ(deleted == false){
		DL_FOREACH_సురక్షిత(context->msgs_బహిర్గతం.queued, client_సందేశం, tmp){
			ఒకవేళ(client_సందేశం->దత్తాంశం.mid == mid){
				ఒకవేళ(client_సందేశం->దత్తాంశం.qos != qos){
					తిరిగి MOSQ_ERR_PROTOCOL;
				}లేకపోతే ఒకవేళ(qos == 2 && client_సందేశం->దత్తాంశం.state != expect_state && expect_state != mosq_ms_any){
					తిరిగి MOSQ_ERR_PROTOCOL;
				}
				db_సందేశం_remove_queued(context, &context->msgs_బహిర్గతం, client_సందేశం);
				ఆపు;
			}
		}
	}
	db_fill_inflight_బహిర్గతం_from_వరుస(context);
#నిర్వచితమైతే WITH_PERSISTENCE
	db.persistence_changes++;
#అయితే_ముగింపు

	తిరిగి db_సందేశం_write_inflight_బహిర్గతం_latest(context);
}


/* Only for QoS 2 messages */
పూర్ణాంకం db_సందేశం_చొప్పించు_incoming(నిర్మాణం mosquitto *context, uint64_t cmsg_id, నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం, bool persist)
{
	నిర్మాణం mosquitto_client_సందేశం *client_సందేశం;
	నిర్మాణం mosquitto_సందేశం_దత్తాంశం *సందేశం_దత్తాంశం;
	గణన mosquitto_సందేశం_state state = mosq_ms_invalid;
	పూర్ణాంకం rc = 0;

	ప్రతిజ్ఞా(పునాది_సందేశం);
	ఒకవేళ(!context){
		తిరిగి MOSQ_ERR_INVAL;
	}
	ఒకవేళ(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		తిరిగి MOSQ_ERR_SUCCESS;

	}
	సందేశం_దత్తాంశం = &context->msgs_లోపల;

	ఒకవేళ(db__ready_for_flight(context, mosq_md_లోపల, పునాది_సందేశం->దత్తాంశం.qos)){
		state = mosq_ms_wait_for_pubrel;
	}లేకపోతే ఒకవేళ(పునాది_సందేశం->దత్తాంశం.qos != 0 && db_ready_for_వరుస(context, పునాది_సందేశం->దత్తాంశం.qos, సందేశం_దత్తాంశం)){
		state = mosq_ms_queued;
		rc = 2;
	}లేకపోతే{
		/* Dropping message due to full queue. */
		ఒకవేళ(context->is_dropping == false){
			context->is_dropping = true;
			log__printf(ఖాళీ, MOSQ_LOG_NOTICE,
					"Outgoing messages are being dropped for client %s.",
					context->id);
		}
		metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		తిరిగి 2;
	}

	ప్రతిజ్ఞా(state != mosq_ms_invalid);

#నిర్వచితమైతే WITH_PERSISTENCE
	ఒకవేళ(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#అయితే_ముగింపు

	client_సందేశం = mosquitto_malloc(పరిమాణం(నిర్మాణం mosquitto_client_సందేశం));
	ఒకవేళ(!client_సందేశం){
		తిరిగి MOSQ_ERR_NOMEM;
	}
	client_సందేశం->prev = ఖాళీ;
	client_సందేశం->తదుపరి = ఖాళీ;
	ఒకవేళ(cmsg_id){
		client_సందేశం->దత్తాంశం.cmsg_id = cmsg_id;
	}లేకపోతే{
		client_సందేశం->దత్తాంశం.cmsg_id = ++context->చివరి_cmsg_id;
	}
	client_సందేశం->పునాది_సందేశం = పునాది_సందేశం;
	db_సందేశం_store_ref_inc(client_సందేశం->పునాది_సందేశం);
	client_సందేశం->దత్తాంశం.mid = పునాది_సందేశం->దత్తాంశం.ఆధారం_mid;
	client_సందేశం->దత్తాంశం.direction = mosq_md_లోపల;
	client_సందేశం->దత్తాంశం.state = (uint8_t)state;
	client_సందేశం->దత్తాంశం.dup = false;
	ఒకవేళ(పునాది_సందేశం->దత్తాంశం.qos > context->max_qos){
		client_సందేశం->దత్తాంశం.qos = context->max_qos;
	}లేకపోతే{
		client_సందేశం->దత్తాంశం.qos = పునాది_సందేశం->దత్తాంశం.qos;
	}
	client_సందేశం->దత్తాంశం.retain = పునాది_సందేశం->దత్తాంశం.retain;
	client_సందేశం->దత్తాంశం.subscription_identifier = 0;

	ఒకవేళ(state == mosq_ms_queued){
		DL_APPEND(సందేశం_దత్తాంశం->queued, client_సందేశం);
		db_సందేశం_కూడు_గమ్యం_queued_stats(సందేశం_దత్తాంశం, client_సందేశం);
	}లేకపోతే{
		DL_APPEND(సందేశం_దత్తాంశం->inflight, client_సందేశం);
		db_సందేశం_కూడు_గమ్యం_inflight_stats(సందేశం_దత్తాంశం, client_సందేశం);
	}

	ఒకవేళ(persist && context->is_persisted){
		plugin_persist_handle_పునాది_సందేశం_కూడు(client_సందేశం->పునాది_సందేశం);
		plugin_persist_handle_client_సందేశం_కూడు(context, client_సందేశం);
	}

	ఒకవేళ(client_సందేశం->పునాది_సందేశం->దత్తాంశం.qos > 0){
		util__decrement_receive_quota(context);
	}
	తిరిగి rc;
}


పూర్ణాంకం db_సందేశం_చొప్పించు_outgoing(నిర్మాణం mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం, uint32_t subscription_identifier, bool update, bool persist)
{
	నిర్మాణం mosquitto_client_సందేశం *client_సందేశం;
	నిర్మాణం mosquitto_సందేశం_దత్తాంశం *సందేశం_దత్తాంశం;
	గణన mosquitto_సందేశం_state state = mosq_ms_invalid;
	పూర్ణాంకం rc = 0;
	అక్షరం **dest_ids;

	ప్రతిజ్ఞా(పునాది_సందేశం);
	ఒకవేళ(!context){
		తిరిగి MOSQ_ERR_INVAL;
	}
	ఒకవేళ(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		తిరిగి MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_పంపిన++;

	సందేశం_దత్తాంశం = &context->msgs_బహిర్గతం;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	ఒకవేళ(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && పునాది_సందేశం->dest_ids){

		కొరకు(పూర్ణాంకం i=0; i<పునాది_సందేశం->dest_id_లెక్క; i++){
			ఒకవేళ(పునాది_సందేశం->dest_ids[i] && !strcmp(పునాది_సందేశం->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				తిరిగి MOSQ_ERR_SUCCESS;
			}
		}
	}
	ఒకవేళ(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		ఒకవేళ(qos == 0 && !db.config->వరుస_qos0_messages){
			ఒకవేళ(!context->bridge){
				తిరిగి 2;
			}లేకపోతే{
				ఒకవేళ(context->bridge->ప్రారంభం_type != bst_lazy){
					తిరిగి 2;
				}
			}
		}
		ఒకవేళ(context->bridge && context->bridge->clean_ప్రారంభం_local == true){
			తిరిగి 2;
		}
	}

	ఒకవేళ(net__is_connected(context)){
		ఒకవేళ(db__ready_for_flight(context, mosq_md_బహిర్గతం, qos)){
			ఎంపిక(qos){
				స్థితి 0:
					state = mosq_ms_publish_qos0;
					ఆపు;
				స్థితి 1:
					state = mosq_ms_publish_qos1;
					ఆపు;
				స్థితి 2:
					state = mosq_ms_publish_qos2;
					ఆపు;
			}
		}లేకపోతే ఒకవేళ(qos != 0 && db_ready_for_వరుస(context, qos, సందేశం_దత్తాంశం)){
			state = mosq_ms_queued;
			rc = 2;
		}లేకపోతే{
			/* Dropping message due to full queue. */
			ఒకవేళ(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(ఖాళీ, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			తిరిగి 2;
		}
	}లేకపోతే{
		ఒకవేళ(db_ready_for_వరుస(context, qos, సందేశం_దత్తాంశం)){
			state = mosq_ms_queued;
		}లేకపోతే{
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			ఒకవేళ(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(ఖాళీ, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			తిరిగి 2;
		}
	}
	ప్రతిజ్ఞా(state != mosq_ms_invalid);

#నిర్వచితమైతే WITH_PERSISTENCE
	ఒకవేళ(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#అయితే_ముగింపు

	client_సందేశం = mosquitto_malloc(పరిమాణం(నిర్మాణం mosquitto_client_సందేశం));
	ఒకవేళ(!client_సందేశం){
		తిరిగి MOSQ_ERR_NOMEM;
	}
	client_సందేశం->prev = ఖాళీ;
	client_సందేశం->తదుపరి = ఖాళీ;
	ఒకవేళ(cmsg_id){
		client_సందేశం->దత్తాంశం.cmsg_id = cmsg_id;
	}లేకపోతే{
		client_సందేశం->దత్తాంశం.cmsg_id = ++context->చివరి_cmsg_id;
	}
	client_సందేశం->పునాది_సందేశం = పునాది_సందేశం;
	db_సందేశం_store_ref_inc(client_సందేశం->పునాది_సందేశం);
	client_సందేశం->దత్తాంశం.mid = mid;
	client_సందేశం->దత్తాంశం.direction = mosq_md_బహిర్గతం;
	client_సందేశం->దత్తాంశం.state = (uint8_t)state;
	client_సందేశం->దత్తాంశం.dup = false;
	ఒకవేళ(qos > context->max_qos){
		client_సందేశం->దత్తాంశం.qos = context->max_qos;
	}లేకపోతే{
		client_సందేశం->దత్తాంశం.qos = qos;
	}
	client_సందేశం->దత్తాంశం.retain = retain;
	client_సందేశం->దత్తాంశం.subscription_identifier = subscription_identifier;

	ఒకవేళ(state == mosq_ms_queued){
		DL_APPEND(సందేశం_దత్తాంశం->queued, client_సందేశం);
		db_సందేశం_కూడు_గమ్యం_queued_stats(సందేశం_దత్తాంశం, client_సందేశం);
	}లేకపోతే{
		DL_APPEND(సందేశం_దత్తాంశం->inflight, client_సందేశం);
		db_సందేశం_కూడు_గమ్యం_inflight_stats(సందేశం_దత్తాంశం, client_సందేశం);
	}

	ఒకవేళ(persist && context->is_persisted){
		plugin_persist_handle_పునాది_సందేశం_కూడు(client_సందేశం->పునాది_సందేశం);
		plugin_persist_handle_client_సందేశం_కూడు(context, client_సందేశం);
	}

	ఒకవేళ(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(పునాది_సందేశం->dest_ids, పరిమాణం(అక్షరం *)*(size_t)(పునాది_సందేశం->dest_id_లెక్క+1));
		ఒకవేళ(dest_ids){
			పునాది_సందేశం->dest_ids = dest_ids;
			పునాది_సందేశం->dest_id_లెక్క++;
			పునాది_సందేశం->dest_ids[పునాది_సందేశం->dest_id_లెక్క-1] = mosquitto_strdup(context->id);
			ఒకవేళ(!పునాది_సందేశం->dest_ids[పునాది_సందేశం->dest_id_లెక్క-1]){
				తిరిగి MOSQ_ERR_NOMEM;
			}
		}లేకపోతే{
			తిరిగి MOSQ_ERR_NOMEM;
		}
	}
#నిర్వచితమైతే WITH_BRIDGE
	ఒకవేళ(context->bridge && context->bridge->ప్రారంభం_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_బహిర్గతం.inflight_లెక్క + context->msgs_బహిర్గతం.queued_లెక్క >= context->bridge->పరిమితి){

		context->bridge->lazy_reconnect = true;
	}
#అయితే_ముగింపు

	ఒకవేళ(client_సందేశం->దత్తాంశం.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	ఒకవేళ(update){
		rc = db_సందేశం_write_inflight_బహిర్గతం_latest(context);
		ఒకవేళ(rc){
			తిరిగి rc;
		}
		rc = db_సందేశం_write_queued_బహిర్గతం(context);
		ఒకవేళ(rc){
			తిరిగి rc;
		}
	}

	తిరిగి rc;
}


స్థిర అంతఃవరుస పూర్ణాంకం db_సందేశం_update_outgoing_state(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_client_సందేశం *తల,
		uint16_t mid, గణన mosquitto_సందేశం_state state, పూర్ణాంకం qos, bool persist)
{
	నిర్మాణం mosquitto_client_సందేశం *client_సందేశం;

	DL_FOREACH(తల, client_సందేశం){
		ఒకవేళ(client_సందేశం->దత్తాంశం.mid == mid){
			ఒకవేళ(client_సందేశం->దత్తాంశం.qos != qos){
				log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_సందేశం->దత్తాంశం.qos, qos);
				తిరిగి MOSQ_ERR_PROTOCOL;
			}
			client_సందేశం->దత్తాంశం.state = (uint8_t)state;
			ఒకవేళ(persist){
				plugin_persist_handle_client_సందేశం_update(context, client_సందేశం);
			}
			తిరిగి MOSQ_ERR_SUCCESS;
		}
	}
	తిరిగి MOSQ_ERR_NOT_దొరికింది;
}


పూర్ణాంకం db_సందేశం_update_outgoing(నిర్మాణం mosquitto *context, uint16_t mid, గణన mosquitto_సందేశం_state state, పూర్ణాంకం qos, bool persist)
{
	పూర్ణాంకం rc;

	rc = db_సందేశం_update_outgoing_state(context, context->msgs_బహిర్గతం.inflight, mid, state, qos, persist);
	ఒకవేళ(!persist && rc == MOSQ_ERR_NOT_దొరికింది){
		rc = db_సందేశం_update_outgoing_state(context, context->msgs_బహిర్గతం.queued, mid, state, qos, persist);
	}
	తిరిగి rc;
}


స్థిర శూన్య db__messages_delete_list(నిర్మాణం mosquitto_client_సందేశం **తల)
{
	నిర్మాణం mosquitto_client_సందేశం *client_సందేశం, *tmp;

	DL_FOREACH_సురక్షిత(*తల, client_సందేశం, tmp){
		DL_DELETE(*తల, client_సందేశం);
		db_సందేశం_store_ref_dec(&client_సందేశం->పునాది_సందేశం);
		mosquitto_FREE(client_సందేశం);
	}
	*తల = ఖాళీ;
}


పూర్ణాంకం db__messages_delete_incoming(నిర్మాణం mosquitto *context)
{
	ఒకవేళ(!context){
		తిరిగి MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_లోపల.inflight);
	db__messages_delete_list(&context->msgs_లోపల.queued);
	context->msgs_లోపల.inflight_బైట్లు = 0;
	context->msgs_లోపల.inflight_bytes12 = 0;
	context->msgs_లోపల.inflight_లెక్క = 0;
	context->msgs_లోపల.inflight_count12 = 0;
	context->msgs_లోపల.queued_బైట్లు = 0;
	context->msgs_లోపల.queued_bytes12 = 0;
	context->msgs_లోపల.queued_లెక్క = 0;
	context->msgs_లోపల.queued_count12 = 0;

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం db__messages_delete_outgoing(నిర్మాణం mosquitto *context)
{
	ఒకవేళ(!context){
		తిరిగి MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_బహిర్గతం.inflight);
	db__messages_delete_list(&context->msgs_బహిర్గతం.queued);
	context->msgs_బహిర్గతం.inflight_బైట్లు = 0;
	context->msgs_బహిర్గతం.inflight_bytes12 = 0;
	context->msgs_బహిర్గతం.inflight_లెక్క = 0;
	context->msgs_బహిర్గతం.inflight_count12 = 0;
	context->msgs_బహిర్గతం.queued_బైట్లు = 0;
	context->msgs_బహిర్గతం.queued_bytes12 = 0;
	context->msgs_బహిర్గతం.queued_లెక్క = 0;
	context->msgs_బహిర్గతం.queued_count12 = 0;

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం db__messages_delete(నిర్మాణం mosquitto *context, bool force_free)
{
	ఒకవేళ(!context){
		తిరిగి MOSQ_ERR_INVAL;
	}

	ఒకవేళ(force_free || context->clean_ప్రారంభం || (context->bridge && context->bridge->clean_ప్రారంభం)){
		db__messages_delete_incoming(context);
	}

	ఒకవేళ(force_free || (context->bridge && context->bridge->clean_ప్రారంభం_local)
			|| (context->bridge == ఖాళీ && context->clean_ప్రారంభం)){

		db__messages_delete_outgoing(context);
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం db_messages_easy_వరుస(నిర్మాణం mosquitto *context, స్థిరరాశి అక్షరం *topic, uint8_t qos, uint32_t payloadlen, స్థిరరాశి శూన్య *payload, పూర్ణాంకం retain, uint32_t సందేశం_expiry_interval, mosquitto_property **properties)
{
	నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం;
	స్థిరరాశి అక్షరం *ఆధారం_id;
	గణన mosquitto_సందేశం_origin origin;

	ఒకవేళ(!topic){
		తిరిగి MOSQ_ERR_INVAL;
	}

	పునాది_సందేశం = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto_పునాది_సందేశం));
	ఒకవేళ(పునాది_సందేశం == ఖాళీ){
		తిరిగి MOSQ_ERR_NOMEM;
	}

	పునాది_సందేశం->దత్తాంశం.topic = mosquitto_strdup(topic);
	ఒకవేళ(పునాది_సందేశం->దత్తాంశం.topic == ఖాళీ){
		db_సందేశం_store_free(పునాది_సందేశం);
		తిరిగి MOSQ_ERR_INVAL;
	}

	పునాది_సందేశం->దత్తాంశం.qos = qos;
	ఒకవేళ(db.config->retain_అందుబాటు == false){
		పునాది_సందేశం->దత్తాంశం.retain = 0;
	}లేకపోతే{
		పునాది_సందేశం->దత్తాంశం.retain = retain;
	}

	పునాది_సందేశం->దత్తాంశం.payloadlen = payloadlen;
	ఒకవేళ(payloadlen > 0){
		పునాది_సందేశం->దత్తాంశం.payload = mosquitto_malloc(పునాది_సందేశం->దత్తాంశం.payloadlen+1);
		ఒకవేళ(పునాది_సందేశం->దత్తాంశం.payload == ఖాళీ){
			db_సందేశం_store_free(పునాది_సందేశం);
			తిరిగి MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)పునాది_సందేశం->దత్తాంశం.payload)[పునాది_సందేశం->దత్తాంశం.payloadlen] = 0;
		జ్ఞాపకం_నకలు(పునాది_సందేశం->దత్తాంశం.payload, payload, పునాది_సందేశం->దత్తాంశం.payloadlen);
	}

	ఒకవేళ(context && context->id){
		ఆధారం_id = context->id;
	}లేకపోతే{
		ఆధారం_id = "";
	}
	ఒకవేళ(properties){
		పునాది_సందేశం->దత్తాంశం.properties = *properties;
		*properties = ఖాళీ;
	}

	ఒకవేళ(context){
		origin = mosq_mo_client;
	}లేకపోతే{
		origin = mosq_mo_broker;
	}
	ఒకవేళ(db_సందేశం_store(context, పునాది_సందేశం, &సందేశం_expiry_interval, origin)){
		తిరిగి 1;
	}

	తిరిగి sub_messages_వరుస(ఆధారం_id, పునాది_సందేశం->దత్తాంశం.qos, &పునాది_సందేశం);
}


#నిర్వచించు MOSQ_UUID_EPOCH 1637168273


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
uint64_t db_new_సందేశం_id(శూన్య)
{
#నిర్వచితమైతే WIN32
	FILETIME ftime;
	uint64_t ftime64;
#లేకపోతే
	నిర్మాణం timespec ts;
#అయితే_ముగింపు
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	దీర్ఘ nsec;

	id = db.కణుపు_id_shifted; /* Top 10-bits */

#నిర్వచితమైతే WIN32
	Get_System_సమయం_Precise_As_File_సమయం(&ftime);
	ftime64 = (((uint64_t)ftime.dw_ఎగువ_Date_సమయం)<<32) + ftime.dw_దిగువ_Date_సమయం;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (దీర్ఘ)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#లేకపోతే
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#అయితే_ముగింపు
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	ఒకవేళ(id <= db.చివరి_db_id){
		id = db.చివరి_db_id + 1;
	}
	db.చివరి_db_id = id;

	తిరిగి id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
పూర్ణాంకం db_సందేశం_store(స్థిరరాశి నిర్మాణం mosquitto *ఆధారం, నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం, uint32_t *సందేశం_expiry_interval, గణన mosquitto_సందేశం_origin origin)
{
	పూర్ణాంకం rc;

	ప్రతిజ్ఞా(పునాది_సందేశం);

	ఒకవేళ(ఆధారం && ఆధారం->id){
		పునాది_సందేశం->దత్తాంశం.ఆధారం_id = mosquitto_strdup(ఆధారం->id);
	}లేకపోతే{
		పునాది_సందేశం->దత్తాంశం.ఆధారం_id = mosquitto_strdup("");
	}
	ఒకవేళ(!పునాది_సందేశం->దత్తాంశం.ఆధారం_id){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_సందేశం_store_free(పునాది_సందేశం);
		తిరిగి MOSQ_ERR_NOMEM;
	}

	ఒకవేళ(ఆధారం && ఆధారం->username){
		పునాది_సందేశం->దత్తాంశం.ఆధారం_username = mosquitto_strdup(ఆధారం->username);
		ఒకవేళ(!పునాది_సందేశం->దత్తాంశం.ఆధారం_username){
			db_సందేశం_store_free(పునాది_సందేశం);
			తిరిగి MOSQ_ERR_NOMEM;
		}
	}
	ఒకవేళ(ఆధారం){
		పునాది_సందేశం->ఆధారం_listener = ఆధారం->listener;
	}
	పునాది_సందేశం->origin = origin;
	ఒకవేళ(సందేశం_expiry_interval){
		ఒకవేళ(*సందేశం_expiry_interval > 0 && *సందేశం_expiry_interval != సందేశం_EXPIRY_INFINITE){
			పునాది_సందేశం->దత్తాంశం.expiry_సమయం = db.now_real_s + *సందేశం_expiry_interval;
		}లేకపోతే{
			పునాది_సందేశం->దత్తాంశం.expiry_సమయం = 0;
		}
	}

	పునాది_సందేశం->dest_ids = ఖాళీ;
	పునాది_సందేశం->dest_id_లెక్క = 0;
	db.సందేశం_store_లెక్క++;
	db.సందేశం_store_బైట్లు += పునాది_సందేశం->దత్తాంశం.payloadlen;

	ఒకవేళ(!పునాది_సందేశం->దత్తాంశం.store_id){
		పునాది_సందేశం->దత్తాంశం.store_id = db_new_సందేశం_id();
	}

	rc = db_సందేశం_store_కూడు(పునాది_సందేశం);
	ఒకవేళ(rc){
		db_సందేశం_store_free(పునాది_సందేశం);
		తిరిగి rc;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం db_సందేశం_store_కనుగొను(నిర్మాణం mosquitto *context, uint16_t mid, నిర్మాణం mosquitto_client_సందేశం **client_సందేశం)
{
	నిర్మాణం mosquitto_client_సందేశం *cmsg;

	*client_సందేశం = ఖాళీ;

	ఒకవేళ(!context){
		తిరిగి MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_లోపల.inflight, cmsg){
		ఒకవేళ(cmsg->పునాది_సందేశం->దత్తాంశం.ఆధారం_mid == mid){
			*client_సందేశం = cmsg;
			తిరిగి MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_లోపల.queued, cmsg){
		ఒకవేళ(cmsg->పునాది_సందేశం->దత్తాంశం.ఆధారం_mid == mid){
			*client_సందేశం = cmsg;
			తిరిగి MOSQ_ERR_SUCCESS;
		}
	}

	తిరిగి 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
స్థిర పూర్ణాంకం db_సందేశం_reconnect_reset_outgoing(నిర్మాణం mosquitto *context)
{
	నిర్మాణం mosquitto_client_సందేశం *client_సందేశం, *tmp;

	context->msgs_బహిర్గతం.inflight_బైట్లు = 0;
	context->msgs_బహిర్గతం.inflight_bytes12 = 0;
	context->msgs_బహిర్గతం.inflight_లెక్క = 0;
	context->msgs_బహిర్గతం.inflight_count12 = 0;
	context->msgs_బహిర్గతం.queued_బైట్లు = 0;
	context->msgs_బహిర్గతం.queued_bytes12 = 0;
	context->msgs_బహిర్గతం.queued_లెక్క = 0;
	context->msgs_బహిర్గతం.queued_count12 = 0;
	context->msgs_బహిర్గతం.inflight_quota = context->msgs_బహిర్గతం.inflight_గరిష్ఠ;

	DL_FOREACH_సురక్షిత(context->msgs_బహిర్గతం.inflight, client_సందేశం, tmp){
		db_సందేశం_కూడు_గమ్యం_inflight_stats(&context->msgs_బహిర్గతం, client_సందేశం);
		ఒకవేళ(client_సందేశం->దత్తాంశం.qos > 0){
			util__decrement_send_quota(context);
		}

		ఎంపిక(client_సందేశం->దత్తాంశం.qos){
			స్థితి 0:
				client_సందేశం->దత్తాంశం.state = mosq_ms_publish_qos0;
				ఆపు;
			స్థితి 1:
				client_సందేశం->దత్తాంశం.state = mosq_ms_publish_qos1;
				ఆపు;
			స్థితి 2:
				ఒకవేళ(client_సందేశం->దత్తాంశం.state == mosq_ms_wait_for_pubcomp){
					client_సందేశం->దత్తాంశం.state = mosq_ms_resend_pubrel;
				}లేకపోతే{
					client_సందేశం->దత్తాంశం.state = mosq_ms_publish_qos2;
				}
				ఆపు;
		}
		plugin_persist_handle_client_సందేశం_update(context, client_సందేశం);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_సురక్షిత(context->msgs_బహిర్గతం.queued, client_సందేశం, tmp){
		db_సందేశం_కూడు_గమ్యం_queued_stats(&context->msgs_బహిర్గతం, client_సందేశం);
	}
	db_fill_inflight_బహిర్గతం_from_వరుస(context);

	తిరిగి MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
స్థిర పూర్ణాంకం db_సందేశం_reconnect_reset_incoming(నిర్మాణం mosquitto *context)
{
	నిర్మాణం mosquitto_client_సందేశం *client_సందేశం, *tmp;

	context->msgs_లోపల.inflight_బైట్లు = 0;
	context->msgs_లోపల.inflight_bytes12 = 0;
	context->msgs_లోపల.inflight_లెక్క = 0;
	context->msgs_లోపల.inflight_count12 = 0;
	context->msgs_లోపల.queued_బైట్లు = 0;
	context->msgs_లోపల.queued_bytes12 = 0;
	context->msgs_లోపల.queued_లెక్క = 0;
	context->msgs_లోపల.queued_count12 = 0;
	context->msgs_లోపల.inflight_quota = context->msgs_లోపల.inflight_గరిష్ఠ;

	DL_FOREACH_సురక్షిత(context->msgs_లోపల.inflight, client_సందేశం, tmp){
		db_సందేశం_కూడు_గమ్యం_inflight_stats(&context->msgs_లోపల, client_సందేశం);
		ఒకవేళ(client_సందేశం->దత్తాంశం.qos > 0){
			util__decrement_receive_quota(context);
		}

		ఒకవేళ(client_సందేశం->దత్తాంశం.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db_సందేశం_remove_inflight(context, &context->msgs_లోపల, client_సందేశం);
		}లేకపోతే{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_సందేశం->దత్తాంశం.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_సురక్షిత(context->msgs_లోపల.queued, client_సందేశం, tmp){
		client_సందేశం->దత్తాంశం.dup = 0;
		db_సందేశం_కూడు_గమ్యం_queued_stats(&context->msgs_లోపల, client_సందేశం);
		ఒకవేళ(db__ready_for_flight(context, mosq_md_లోపల, client_సందేశం->దత్తాంశం.qos)){
			ఎంపిక(client_సందేశం->దత్తాంశం.qos){
				స్థితి 0:
					client_సందేశం->దత్తాంశం.state = mosq_ms_publish_qos0;
					ఆపు;
				స్థితి 1:
					client_సందేశం->దత్తాంశం.state = mosq_ms_publish_qos1;
					ఆపు;
				స్థితి 2:
					client_సందేశం->దత్తాంశం.state = mosq_ms_publish_qos2;
					ఆపు;
			}
			db_సందేశం_వరుస_నుండి_తీయి_మొదటి(context, &context->msgs_లోపల);
			plugin_persist_handle_client_సందేశం_update(context, client_సందేశం);
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం db_సందేశం_reconnect_reset(నిర్మాణం mosquitto *context)
{
	పూర్ణాంకం rc;

	rc = db_సందేశం_reconnect_reset_outgoing(context);
	ఒకవేళ(rc){
		తిరిగి rc;
	}
	తిరిగి db_సందేశం_reconnect_reset_incoming(context);
}


పూర్ణాంకం db_సందేశం_remove_incoming(నిర్మాణం mosquitto *context, uint16_t mid)
{
	నిర్మాణం mosquitto_client_సందేశం *client_సందేశం, *tmp;

	ఒకవేళ(!context){
		తిరిగి MOSQ_ERR_INVAL;
	}

	DL_FOREACH_సురక్షిత(context->msgs_లోపల.inflight, client_సందేశం, tmp){
		ఒకవేళ(client_సందేశం->దత్తాంశం.mid == mid){
			ఒకవేళ(client_సందేశం->పునాది_సందేశం->దత్తాంశం.qos != 2){
				log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_సందేశం->పునాది_సందేశం->దత్తాంశం.qos);
				తిరిగి MOSQ_ERR_PROTOCOL;
			}
			db_సందేశం_remove_inflight(context, &context->msgs_లోపల, client_సందేశం);
			తిరిగి MOSQ_ERR_SUCCESS;
		}
	}

	తిరిగి MOSQ_ERR_NOT_దొరికింది;
}


పూర్ణాంకం db_సందేశం_release_incoming(నిర్మాణం mosquitto *context, uint16_t mid)
{
	నిర్మాణం mosquitto_client_సందేశం *client_సందేశం, *tmp;
	bool deleted = false;
	పూర్ణాంకం rc;

	ఒకవేళ(!context){
		తిరిగి MOSQ_ERR_INVAL;
	}

	DL_FOREACH_సురక్షిత(context->msgs_లోపల.inflight, client_సందేశం, tmp){
		ఒకవేళ(client_సందేశం->దత్తాంశం.mid == mid){
			ఒకవేళ(client_సందేశం->పునాది_సందేశం->దత్తాంశం.qos != 2){
				log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_సందేశం->పునాది_సందేశం->దత్తాంశం.qos);
				తిరిగి MOSQ_ERR_PROTOCOL;
			}
			స్థిరరాశి అక్షరం *topic = client_సందేశం->పునాది_సందేశం->దత్తాంశం.topic;
			స్థిరరాశి అక్షరం *ఆధారం_id = client_సందేశం->పునాది_సందేశం->దత్తాంశం.ఆధారం_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			ఒకవేళ(topic == ఖాళీ){
				db_సందేశం_remove_inflight(context, &context->msgs_లోపల, client_సందేశం);
				deleted = true;
			}లేకపోతే{
				rc = sub_messages_వరుస(ఆధారం_id, 2, &client_సందేశం->పునాది_సందేశం);
				ఒకవేళ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db_సందేశం_remove_inflight(context, &context->msgs_లోపల, client_సందేశం);
					deleted = true;
				}లేకపోతే{
					తిరిగి 1;
				}
			}
		}
	}

	DL_FOREACH_సురక్షిత(context->msgs_లోపల.queued, client_సందేశం, tmp){
		ఒకవేళ(db__ready_for_flight(context, mosq_md_లోపల, client_సందేశం->దత్తాంశం.qos)){
			ఆపు;
		}

		ఒకవేళ(client_సందేశం->దత్తాంశం.qos == 2){
			send__pubrec(context, client_సందేశం->దత్తాంశం.mid, 0, ఖాళీ);
			client_సందేశం->దత్తాంశం.state = mosq_ms_wait_for_pubrel;
			db_సందేశం_వరుస_నుండి_తీయి_మొదటి(context, &context->msgs_లోపల);
			plugin_persist_handle_client_సందేశం_update(context, client_సందేశం);
		}
	}
	ఒకవేళ(deleted){
		తిరిగి MOSQ_ERR_SUCCESS;
	}లేకపోతే{
		తిరిగి MOSQ_ERR_NOT_దొరికింది;
	}
}


శూన్య db__expire_all_messages(నిర్మాణం mosquitto *context)
{
	నిర్మాణం mosquitto_client_సందేశం *client_సందేశం, *tmp;

	DL_FOREACH_సురక్షిత(context->msgs_బహిర్గతం.inflight, client_సందేశం, tmp){
		ఒకవేళ(client_సందేశం->పునాది_సందేశం->దత్తాంశం.expiry_సమయం && db.now_real_s > client_సందేశం->పునాది_సందేశం->దత్తాంశం.expiry_సమయం){
			ఒకవేళ(client_సందేశం->దత్తాంశం.qos > 0){
				util__increment_send_quota(context);
			}
			db_సందేశం_remove_inflight(context, &context->msgs_బహిర్గతం, client_సందేశం);
		}
	}
	db_fill_inflight_బహిర్గతం_from_వరుస(context);
	DL_FOREACH_సురక్షిత(context->msgs_బహిర్గతం.queued, client_సందేశం, tmp){
		ఒకవేళ(client_సందేశం->పునాది_సందేశం->దత్తాంశం.expiry_సమయం && db.now_real_s > client_సందేశం->పునాది_సందేశం->దత్తాంశం.expiry_సమయం){
			db_సందేశం_remove_queued(context, &context->msgs_బహిర్గతం, client_సందేశం);
		}
	}
	DL_FOREACH_సురక్షిత(context->msgs_లోపల.inflight, client_సందేశం, tmp){
		ఒకవేళ(client_సందేశం->పునాది_సందేశం->దత్తాంశం.expiry_సమయం && db.now_real_s > client_సందేశం->పునాది_సందేశం->దత్తాంశం.expiry_సమయం){
			ఒకవేళ(client_సందేశం->దత్తాంశం.qos > 0){
				util__increment_receive_quota(context);
			}
			db_సందేశం_remove_inflight(context, &context->msgs_లోపల, client_సందేశం);
		}
	}
	DL_FOREACH_సురక్షిత(context->msgs_లోపల.queued, client_సందేశం, tmp){
		ఒకవేళ(client_సందేశం->పునాది_సందేశం->దత్తాంశం.expiry_సమయం && db.now_real_s > client_సందేశం->పునాది_సందేశం->దత్తాంశం.expiry_సమయం){
			db_సందేశం_remove_queued(context, &context->msgs_లోపల, client_సందేశం);
		}
	}
}


స్థిర శూన్య db__client_messages_check_acl(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_సందేశం_దత్తాంశం *సందేశం_దత్తాంశం, నిర్మాణం mosquitto_client_సందేశం **తల,
		శూన్య (*decrement_stats_fn)(నిర్మాణం mosquitto_సందేశం_దత్తాంశం *సందేశం_దత్తాంశం, నిర్మాణం mosquitto_client_సందేశం *client_సందేశం))
{
	నిర్మాణం mosquitto_client_సందేశం *client_సందేశం, *tmp;
	నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం;
	పూర్ణాంకం access;

	DL_FOREACH_సురక్షిత((*తల), client_సందేశం, tmp){
		పునాది_సందేశం = client_సందేశం->పునాది_సందేశం;
		ఒకవేళ(client_సందేశం->దత్తాంశం.direction == mosq_md_బహిర్గతం){
			access = MOSQ_ACL_READ;
		}లేకపోతే{
			access = MOSQ_ACL_WRITE;
		}
		ఒకవేళ(mosquitto_acl_check(context, పునాది_సందేశం->దత్తాంశం.topic,
				పునాది_సందేశం->దత్తాంశం.payloadlen, పునాది_సందేశం->దత్తాంశం.payload,
				పునాది_సందేశం->దత్తాంశం.qos, పునాది_సందేశం->దత్తాంశం.retain,
				పునాది_సందేశం->దత్తాంశం.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*తల), client_సందేశం);
			decrement_stats_fn(సందేశం_దత్తాంశం, client_సందేశం);
			plugin_persist_handle_client_సందేశం_delete(context, client_సందేశం);
			db_సందేశం_store_ref_dec(&client_సందేశం->పునాది_సందేశం);
			mosquitto_FREE(client_సందేశం);
		}
	}
}


శూన్య db_check_acl_యొక్క_all_messages(నిర్మాణం mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_లోపల, &context->msgs_లోపల.inflight, &db_సందేశం_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_లోపల, &context->msgs_లోపల.queued, &db_సందేశం_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_బహిర్గతం, &context->msgs_బహిర్గతం.inflight, &db_సందేశం_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_బహిర్గతం, &context->msgs_బహిర్గతం.queued, &db_సందేశం_remove_from_queued_stats);
}


స్థిర పూర్ణాంకం db_సందేశం_write_inflight_బహిర్గతం_single(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_client_సందేశం *client_సందేశం)
{
	నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం;
	mosquitto_property *పునాది_సందేశం_props = ఖాళీ;
	పూర్ణాంకం rc;
	uint16_t mid;
	పూర్ణాంకం retries;
	పూర్ణాంకం retain;
	స్థిరరాశి అక్షరం *topic;
	uint8_t qos;
	uint32_t payloadlen;
	స్థిరరాశి శూన్య *payload;
	uint32_t expiry_interval;
	uint32_t subscription_id;

	పునాది_సందేశం = client_సందేశం->పునాది_సందేశం;

	expiry_interval = 0;
	ఒకవేళ(పునాది_సందేశం->దత్తాంశం.expiry_సమయం){
		ఒకవేళ(db.now_real_s > పునాది_సందేశం->దత్తాంశం.expiry_సమయం){
			/* Message is expired, must not send. */
			ఒకవేళ(client_సందేశం->దత్తాంశం.direction == mosq_md_బహిర్గతం && client_సందేశం->దత్తాంశం.qos > 0){
				util__increment_send_quota(context);
			}
			db_సందేశం_remove_inflight(context, &context->msgs_బహిర్గతం, client_సందేశం);
			db_fill_inflight_బహిర్గతం_from_వరుస(context);
			తిరిగి MOSQ_ERR_SUCCESS;
		}లేకపోతే{
			expiry_interval = (uint32_t)(పునాది_సందేశం->దత్తాంశం.expiry_సమయం - db.now_real_s);
		}
	}
	mid = client_సందేశం->దత్తాంశం.mid;
	retries = client_సందేశం->దత్తాంశం.dup;
	retain = client_సందేశం->దత్తాంశం.retain;
	topic = పునాది_సందేశం->దత్తాంశం.topic;
	qos = (uint8_t)client_సందేశం->దత్తాంశం.qos;
	payloadlen = పునాది_సందేశం->దత్తాంశం.payloadlen;
	payload = పునాది_సందేశం->దత్తాంశం.payload;
	subscription_id = client_సందేశం->దత్తాంశం.subscription_identifier;
	పునాది_సందేశం_props = పునాది_సందేశం->దత్తాంశం.properties;

	ఎంపిక(client_సందేశం->దత్తాంశం.state){
		స్థితి mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, పునాది_సందేశం_props, expiry_interval);
			ఒకవేళ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_సందేశం_remove_inflight(context, &context->msgs_బహిర్గతం, client_సందేశం);
			}లేకపోతే{
				తిరిగి rc;
			}
			ఆపు;

		స్థితి mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, పునాది_సందేశం_props, expiry_interval);
			ఒకవేళ(rc == MOSQ_ERR_SUCCESS){
				client_సందేశం->దత్తాంశం.dup = 1; /* Any retry attempts are a duplicate. */
				client_సందేశం->దత్తాంశం.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_సందేశం_update(context, client_సందేశం);
			}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_సందేశం_remove_inflight(context, &context->msgs_బహిర్గతం, client_సందేశం);
			}లేకపోతే{
				తిరిగి rc;
			}
			ఆపు;

		స్థితి mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, పునాది_సందేశం_props, expiry_interval);
			ఒకవేళ(rc == MOSQ_ERR_SUCCESS){
				client_సందేశం->దత్తాంశం.dup = 1; /* Any retry attempts are a duplicate. */
				client_సందేశం->దత్తాంశం.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_సందేశం_update(context, client_సందేశం);
			}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_సందేశం_remove_inflight(context, &context->msgs_బహిర్గతం, client_సందేశం);
			}లేకపోతే{
				తిరిగి rc;
			}
			ఆపు;

		స్థితి mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, ఖాళీ);
			ఒకవేళ(!rc){
				client_సందేశం->దత్తాంశం.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_సందేశం_update(context, client_సందేశం);
			}లేకపోతే{
				తిరిగి rc;
			}
			ఆపు;

		స్థితి mosq_ms_invalid:
		స్థితి mosq_ms_send_pubrec:
		స్థితి mosq_ms_resend_pubcomp:
		స్థితి mosq_ms_wait_for_puback:
		స్థితి mosq_ms_wait_for_pubrec:
		స్థితి mosq_ms_wait_for_pubrel:
		స్థితి mosq_ms_wait_for_pubcomp:
		స్థితి mosq_ms_queued:
			ఆపు;
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం db_సందేశం_write_inflight_బహిర్గతం_all(నిర్మాణం mosquitto *context)
{
	నిర్మాణం mosquitto_client_సందేశం *client_సందేశం, *tmp;
	పూర్ణాంకం rc;

	ఒకవేళ(context->state != mosq_cs_చురుకైన || !net__is_connected(context)){
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_సురక్షిత(context->msgs_బహిర్గతం.inflight, client_సందేశం, tmp){
		rc = db_సందేశం_write_inflight_బహిర్గతం_single(context, client_సందేశం);
		ఒకవేళ(rc){
			తిరిగి rc;
		}
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం db_సందేశం_write_inflight_బహిర్గతం_latest(నిర్మాణం mosquitto *context)
{
	నిర్మాణం mosquitto_client_సందేశం *client_సందేశం, *తదుపరి;
	పూర్ణాంకం rc;

	ఒకవేళ(context->state != mosq_cs_చురుకైన
			|| !net__is_connected(context)
			|| context->msgs_బహిర్గతం.inflight == ఖాళీ){

		తిరిగి MOSQ_ERR_SUCCESS;
	}

	ఒకవేళ(context->msgs_బహిర్గతం.inflight->prev == context->msgs_బహిర్గతం.inflight){
		/* Only one message */
		తిరిగి db_సందేశం_write_inflight_బహిర్గతం_single(context, context->msgs_బహిర్గతం.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_సందేశం = context->msgs_బహిర్గతం.inflight->prev;
	అయితే(client_సందేశం != context->msgs_బహిర్గతం.inflight &&
			(client_సందేశం->దత్తాంశం.state == mosq_ms_publish_qos0
			|| client_సందేశం->దత్తాంశం.state == mosq_ms_publish_qos1
			|| client_సందేశం->దత్తాంశం.state == mosq_ms_publish_qos2)){

		client_సందేశం = client_సందేశం->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	ఒకవేళ(client_సందేశం != context->msgs_బహిర్గతం.inflight){
		client_సందేశం = client_సందేశం->తదుపరి;
	}

	అయితే(client_సందేశం){
		తదుపరి = client_సందేశం->తదుపరి;
		rc = db_సందేశం_write_inflight_బహిర్గతం_single(context, client_సందేశం);
		ఒకవేళ(rc){
			తిరిగి rc;
		}
		client_సందేశం = తదుపరి;
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం db_సందేశం_write_queued_లోపల(నిర్మాణం mosquitto *context)
{
	నిర్మాణం mosquitto_client_సందేశం *client_సందేశం, *tmp;
	పూర్ణాంకం rc;

	ఒకవేళ(context->state != mosq_cs_చురుకైన){
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_సురక్షిత(context->msgs_లోపల.queued, client_సందేశం, tmp){
		ఒకవేళ(context->msgs_లోపల.inflight_గరిష్ఠ != 0 && context->msgs_లోపల.inflight_quota == 0){
			ఆపు;
		}

		ఒకవేళ(client_సందేశం->దత్తాంశం.qos == 2){
			client_సందేశం->దత్తాంశం.state = mosq_ms_send_pubrec;
			db_సందేశం_వరుస_నుండి_తీయి_మొదటి(context, &context->msgs_లోపల);
			rc = send__pubrec(context, client_సందేశం->దత్తాంశం.mid, 0, ఖాళీ);
			ఒకవేళ(!rc){
				client_సందేశం->దత్తాంశం.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_సందేశం_update(context, client_సందేశం);
			}లేకపోతే{
				plugin_persist_handle_client_సందేశం_update(context, client_సందేశం);
				తిరిగి rc;
			}
		}
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం db_సందేశం_write_queued_బహిర్గతం(నిర్మాణం mosquitto *context)
{
	ఒకవేళ(context->state != mosq_cs_చురుకైన){
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	db_fill_inflight_బహిర్గతం_from_వరుస(context);

	తిరిగి MOSQ_ERR_SUCCESS;
}
