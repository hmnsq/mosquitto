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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <utlist.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"


/**
 * Is this context ready to take more in flight messages right now?
 * @param context the client context of interest
 * @param qos qos for the packet of interest
 * @return true if more in flight are allowed.
 */
bool db__ready_for_flight(ರಚನೆ mosquitto *context, ಎಣಿಕೆ mosquitto_ಸಂದೇಶ_direction dir, ಪೂರ್ಣಾಂಕ qos)
{
	ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ *msgs;
	bool valid_ಬೈಟುಗಳು;
	bool valid_ಎಣಿಕೆ;

	ಆದರೆ(dir == mosq_md_ಹೊರಹೋಗು){
		msgs = &context->msgs_ಹೊರಹೋಗು;
	}ಇಲ್ಲದಿದ್ದರೆ{
		msgs = &context->msgs_ಒಳ;
	}

	ಆದರೆ(msgs->inflight_ಗರಿಷ್ಠ == 0 && db.config->max_inflight_ಬೈಟುಗಳು == 0){
		ಹಿಂತಿರುಗಿ true;
	}

	ಆದರೆ(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		ಆದರೆ(db.config->max_queued_messages == 0 && db.config->max_inflight_ಬೈಟುಗಳು == 0){
			ಹಿಂತಿರುಗಿ true;
		}
		valid_ಬೈಟುಗಳು = ((msgs->inflight_ಬೈಟುಗಳು - (ssize_t)db.config->max_inflight_ಬೈಟುಗಳು) < (ssize_t)db.config->max_queued_ಬೈಟುಗಳು);
		ಆದರೆ(dir == mosq_md_ಹೊರಹೋಗು){
			valid_ಎಣಿಕೆ = context->ಹೊರಹೋಗು_packet_ಎಣಿಕೆ < db.config->max_queued_messages;
		}ಇಲ್ಲದಿದ್ದರೆ{
			valid_ಎಣಿಕೆ = msgs->inflight_ಎಣಿಕೆ - msgs->inflight_ಗರಿಷ್ಠ < db.config->max_queued_messages;
		}

		ಆದರೆ(db.config->max_queued_messages == 0){
			ಹಿಂತಿರುಗಿ valid_ಬೈಟುಗಳು;
		}
		ಆದರೆ(db.config->max_queued_ಬೈಟುಗಳು == 0){
			ಹಿಂತಿರುಗಿ valid_ಎಣಿಕೆ;
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		valid_ಬೈಟುಗಳು = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_ಬೈಟುಗಳು;
		valid_ಎಣಿಕೆ = msgs->inflight_quota > 0;

		ಆದರೆ(msgs->inflight_ಗರಿಷ್ಠ == 0){
			ಹಿಂತಿರುಗಿ valid_ಬೈಟುಗಳು;
		}
		ಆದರೆ(db.config->max_inflight_ಬೈಟುಗಳು == 0){
			ಹಿಂತಿರುಗಿ valid_ಎಣಿಕೆ;
		}
	}

	ಹಿಂತಿರುಗಿ valid_ಬೈಟುಗಳು && valid_ಎಣಿಕೆ;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db_ready_for_ಸರತಿ(ರಚನೆ mosquitto *context, ಪೂರ್ಣಾಂಕ qos, ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ *ಸಂದೇಶ_ದತ್ತಾಂಶ)
{
	ಪೂರ್ಣಾಂಕ ಮೂಲ_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಂಕ adjust_ಎಣಿಕೆ;
	ದೀರ್ಘ ಮೂಲ_ಬೈಟುಗಳು;
	ssize_t adjust_ಬೈಟುಗಳು = (ssize_t)db.config->max_inflight_ಬೈಟುಗಳು;
	bool valid_ಬೈಟುಗಳು;
	bool valid_ಎಣಿಕೆ;

	ಆದರೆ(db.config->max_queued_messages == 0 && db.config->max_queued_ಬೈಟುಗಳು == 0){
		ಹಿಂತಿರುಗಿ true;
	}

	ಆದರೆ(qos == 0 && db.config->ಸರತಿ_qos0_messages == false){
		ಹಿಂತಿರುಗಿ false; /* This case is handled in db__ready_for_flight() */
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಮೂಲ_ಬೈಟುಗಳು = (ssize_t)ಸಂದೇಶ_ದತ್ತಾಂಶ->queued_bytes12;
		ಮೂಲ_ಎಣಿಕೆ = ಸಂದೇಶ_ದತ್ತಾಂಶ->queued_count12;
	}
	adjust_ಎಣಿಕೆ = ಸಂದೇಶ_ದತ್ತಾಂಶ->inflight_ಗರಿಷ್ಠ;

	/* nothing in flight for offline clients */
	ಆದರೆ(!net__is_connected(context)){
		adjust_ಬೈಟುಗಳು = 0;
		adjust_ಎಣಿಕೆ = 0;
	}

	valid_ಬೈಟುಗಳು = (ಮೂಲ_ಬೈಟುಗಳು - (ssize_t)adjust_ಬೈಟುಗಳು) < (ssize_t)db.config->max_queued_ಬೈಟುಗಳು;
	valid_ಎಣಿಕೆ = ಮೂಲ_ಎಣಿಕೆ - adjust_ಎಣಿಕೆ < db.config->max_queued_messages;

	ಆದರೆ(db.config->max_queued_ಬೈಟುಗಳು == 0){
		ಹಿಂತಿರುಗಿ valid_ಎಣಿಕೆ;
	}
	ಆದರೆ(db.config->max_queued_messages == 0){
		ಹಿಂತಿರುಗಿ valid_ಬೈಟುಗಳು;
	}

	ಹಿಂತಿರುಗಿ valid_ಬೈಟುಗಳು && valid_ಎಣಿಕೆ;
}


ಶೂನ್ಯ db_ಸಂದೇಶ_ಕೂಡಿಸು_ಗಮ್ಯ_inflight_stats(ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ *ಸಂದೇಶ_ದತ್ತಾಂಶ, ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ)
{
	ಸಂದೇಶ_ದತ್ತಾಂಶ->inflight_ಎಣಿಕೆ++;
	ಸಂದೇಶ_ದತ್ತಾಂಶ->inflight_ಬೈಟುಗಳು += client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen;
	ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos != 0){
		ಸಂದೇಶ_ದತ್ತಾಂಶ->inflight_count12++;
		ಸಂದೇಶ_ದತ್ತಾಂಶ->inflight_bytes12 += client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen;
	}
}


ಸ್ಥಿರ ಶೂನ್ಯ db_ಸಂದೇಶ_remove_from_inflight_stats(ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ *ಸಂದೇಶ_ದತ್ತಾಂಶ, ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ)
{
	ಸಂದೇಶ_ದತ್ತಾಂಶ->inflight_ಎಣಿಕೆ--;
	ಸಂದೇಶ_ದತ್ತಾಂಶ->inflight_ಬೈಟುಗಳು -= client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen;
	ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos != 0){
		ಸಂದೇಶ_ದತ್ತಾಂಶ->inflight_count12--;
		ಸಂದೇಶ_ದತ್ತಾಂಶ->inflight_bytes12 -= client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen;
	}
}


ಶೂನ್ಯ db_ಸಂದೇಶ_ಕೂಡಿಸು_ಗಮ್ಯ_queued_stats(ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ *ಸಂದೇಶ_ದತ್ತಾಂಶ, ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ)
{
	ಸಂದೇಶ_ದತ್ತಾಂಶ->queued_ಎಣಿಕೆ++;
	ಸಂದೇಶ_ದತ್ತಾಂಶ->queued_ಬೈಟುಗಳು += client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen;
	ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos != 0){
		ಸಂದೇಶ_ದತ್ತಾಂಶ->queued_count12++;
		ಸಂದೇಶ_ದತ್ತಾಂಶ->queued_bytes12 += client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen;
	}
}


ಸ್ಥಿರ ಶೂನ್ಯ db_ಸಂದೇಶ_remove_from_queued_stats(ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ *ಸಂದೇಶ_ದತ್ತಾಂಶ, ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ)
{
	ಸಂದೇಶ_ದತ್ತಾಂಶ->queued_ಎಣಿಕೆ--;
	ಸಂದೇಶ_ದತ್ತಾಂಶ->queued_ಬೈಟುಗಳು -= client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen;
	ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos != 0){
		ಸಂದೇಶ_ದತ್ತಾಂಶ->queued_count12--;
		ಸಂದೇಶ_ದತ್ತಾಂಶ->queued_bytes12 -= client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen;
	}
}


ಪೂರ್ಣಾಂಕ db__open(ರಚನೆ mosquitto__config *config)
{
	ಆದರೆ(!config){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = ಖಾಲಿ;
	db.contexts_by_sock = ಖಾಲಿ;
	db.contexts_for_free = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
	db.bridges = ಖಾಲಿ;
	db.bridge_ಎಣಿಕೆ = 0;
#ಆದರೆ_ಅಂತ್ಯ

	/* Initialize the hashtable */
	db.clientid_ಸೂಚ್ಯಂಕ_hash = ಖಾಲಿ;

	db.ಸಾಮಾನ್ಯ_subs = ಖಾಲಿ;
	db.shared_subs = ಖಾಲಿ;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_PERSISTENCE
	ಆದರೆ(persist__restore()){
		ಹಿಂತಿರುಗಿ 1;
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಶೂನ್ಯ subhier_clean(ರಚನೆ mosquitto__subhier **subhier)
{
	ರಚನೆ mosquitto__subhier *peer, *subhier_tmp;
	ರಚನೆ mosquitto__subleaf *ಎಲೆ, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		ಎಲೆ = peer->subs;
		ಆಗ(ಎಲೆ){
			nextleaf = ಎಲೆ->ಮುಂದಿನ;
			mosquitto_FREE(ಎಲೆ);
			ಎಲೆ = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


ಪೂರ್ಣಾಂಕ db__close(ಶೂನ್ಯ)
{
	subhier_clean(&db.ಸಾಮಾನ್ಯ_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db_ಸಂದೇಶ_store_clean();

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_store_ಕೂಡಿಸು(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ)
{
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ಸಿಕ್ಕಿತು;
	ಚಿಹ್ನೆರಹಿತ hashv;

	HASH_ಮೌಲ್ಯ(&ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.store_id, ಗಾತ್ರ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.store_id), hashv);
	HASH_ಕಂಡುಹಿಡಿ_BYHASHVALUE(hh, db.ಸಂದೇಶ_store, &ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.store_id, ಗಾತ್ರ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.store_id), hashv, ಸಿಕ್ಕಿತು);
	ಆದರೆ(ಸಿಕ್ಕಿತು == ಖಾಲಿ){
		HASH_ಕೂಡಿಸು_KEYPTR_BYHASHVALUE(hh, db.ಸಂದೇಶ_store, &ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.store_id, ಗಾತ್ರ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.store_id), hashv, ತಳ_ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ MOSQ_ERR_ALREADY_EXISTS;
	}
}


ಶೂನ್ಯ db_ಸಂದೇಶ_store_free(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ)
{
	mosquitto_FREE(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_id);
	mosquitto_FREE(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_username);
	ಆದರೆ(ತಳ_ಸಂದೇಶ->dest_ids){
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<ತಳ_ಸಂದೇಶ->dest_id_ಎಣಿಕೆ; i++){
			mosquitto_FREE(ತಳ_ಸಂದೇಶ->dest_ids[i]);
		}
		mosquitto_FREE(ತಳ_ಸಂದೇಶ->dest_ids);
	}
	mosquitto_FREE(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic);
	mosquitto_property_free_all(&ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.properties);
	mosquitto_FREE(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload);
	mosquitto_FREE(ತಳ_ಸಂದೇಶ);
}


ಶೂನ್ಯ db_ಸಂದೇಶ_store_remove(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ, bool notify)
{
	ಆದರೆ(ತಳ_ಸಂದೇಶ == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ;
	}
	HASH_DELETE(hh, db.ಸಂದೇಶ_store, ತಳ_ಸಂದೇಶ);
	db.ಸಂದೇಶ_store_ಎಣಿಕೆ--;
	db.ಸಂದೇಶ_store_ಬೈಟುಗಳು -= ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen;
	ಆದರೆ(notify == true){
		plugin_persist_handle_ತಳ_ಸಂದೇಶ_delete(ತಳ_ಸಂದೇಶ);
	}
	db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
}


ಶೂನ್ಯ db_ಸಂದೇಶ_store_clean(ಶೂನ್ಯ)
{
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ, *ತಳ_ಸಂದೇಶ_tmp;

	HASH_ITER(hh, db.ಸಂದೇಶ_store, ತಳ_ಸಂದೇಶ, ತಳ_ಸಂದೇಶ_tmp){
		db_ಸಂದೇಶ_store_remove(ತಳ_ಸಂದೇಶ, false);
	}
}


ಶೂನ್ಯ db_ಸಂದೇಶ_store_ref_inc(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ)
{
	ತಳ_ಸಂದೇಶ->ref_ಎಣಿಕೆ++;
}


ಶೂನ್ಯ db_ಸಂದೇಶ_store_ref_dec(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ **ತಳ_ಸಂದೇಶ)
{
	(*ತಳ_ಸಂದೇಶ)->ref_ಎಣಿಕೆ--;
	ಆದರೆ((*ತಳ_ಸಂದೇಶ)->ref_ಎಣಿಕೆ == 0){
		db_ಸಂದೇಶ_store_remove(*ತಳ_ಸಂದೇಶ, true);
		*ತಳ_ಸಂದೇಶ = ಖಾಲಿ;
	}
}


ಶೂನ್ಯ db_ಸಂದೇಶ_store_compact(ಶೂನ್ಯ)
{
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ, *ತಳ_ಸಂದೇಶ_tmp;

	HASH_ITER(hh, db.ಸಂದೇಶ_store, ತಳ_ಸಂದೇಶ, ತಳ_ಸಂದೇಶ_tmp){
		ಆದರೆ(ತಳ_ಸಂದೇಶ->ref_ಎಣಿಕೆ < 1){
			db_ಸಂದೇಶ_store_remove(ತಳ_ಸಂದೇಶ, true);
		}
	}
}


ಸ್ಥಿರ ಶೂನ್ಯ db_ಸಂದೇಶ_remove_inflight(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ *ಸಂದೇಶ_ದತ್ತಾಂಶ, ರಚನೆ mosquitto_client_ಸಂದೇಶ *ಅಂಶ)
{
	ಆದರೆ(!context || !ಸಂದೇಶ_ದತ್ತಾಂಶ || !ಅಂಶ){
		ಹಿಂತಿರುಗಿ;
	}

	plugin_persist_handle_client_ಸಂದೇಶ_delete(context, ಅಂಶ);

	DL_DELETE(ಸಂದೇಶ_ದತ್ತಾಂಶ->inflight, ಅಂಶ);
	ಆದರೆ(ಅಂಶ->ತಳ_ಸಂದೇಶ){
		db_ಸಂದೇಶ_remove_from_inflight_stats(ಸಂದೇಶ_ದತ್ತಾಂಶ, ಅಂಶ);
		db_ಸಂದೇಶ_store_ref_dec(&ಅಂಶ->ತಳ_ಸಂದೇಶ);
	}

	mosquitto_FREE(ಅಂಶ);
}


ಸ್ಥಿರ ಶೂನ್ಯ db_ಸಂದೇಶ_remove_queued(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ *ಸಂದೇಶ_ದತ್ತಾಂಶ, ರಚನೆ mosquitto_client_ಸಂದೇಶ *ಅಂಶ)
{
	ಆದರೆ(!context || !ಸಂದೇಶ_ದತ್ತಾಂಶ || !ಅಂಶ){
		ಹಿಂತಿರುಗಿ;
	}

	plugin_persist_handle_client_ಸಂದೇಶ_delete(context, ಅಂಶ);

	DL_DELETE(ಸಂದೇಶ_ದತ್ತಾಂಶ->queued, ಅಂಶ);
	ಆದರೆ(ಅಂಶ->ತಳ_ಸಂದೇಶ){
		db_ಸಂದೇಶ_remove_from_queued_stats(ಸಂದೇಶ_ದತ್ತಾಂಶ, ಅಂಶ);
		db_ಸಂದೇಶ_store_ref_dec(&ಅಂಶ->ತಳ_ಸಂದೇಶ);
	}

	mosquitto_FREE(ಅಂಶ);
}


ಸ್ಥಿರ ಶೂನ್ಯ db_fill_inflight_ಹೊರಹೋಗು_from_ಸರತಿ(ರಚನೆ mosquitto *context)
{
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;

	DL_FOREACH_ಸುರಕ್ಷಿತ(context->msgs_ಹೊರಹೋಗು.queued, client_ಸಂದೇಶ, tmp){
		ಆದರೆ(!db__ready_for_flight(context, mosq_md_ಹೊರಹೋಗು, client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos)){
			ಹಿಂತಿರುಗಿ;
		}
		ಆಯ್ಕೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos){
			ಸ್ಥಿತಿ 0:
				client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state = mosq_ms_publish_qos0;
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 1:
				client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state = mosq_ms_publish_qos1;
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 2:
				client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state = mosq_ms_publish_qos2;
				ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ(client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.expiry_ಸಮಯ && db.now_real_s > client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.expiry_ಸಮಯ){
			db_ಸಂದೇಶ_remove_queued(context, &context->msgs_ಹೊರಹೋಗು, client_ಸಂದೇಶ);
			ಮುಂದುವರಿ;
		}
		plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
		db_ಸಂದೇಶ_ಸರತಿಯಿಂದ_ತೆಗೆ_ಮೊದಲ(context, &context->msgs_ಹೊರಹೋಗು);
	}
}


ಶೂನ್ಯ db_ಸಂದೇಶ_ಸರತಿಯಿಂದ_ತೆಗೆ_ಮೊದಲ(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ *ಸಂದೇಶ_ದತ್ತಾಂಶ)
{
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ;

	ಬಳಸದ(context);

	client_ಸಂದೇಶ = ಸಂದೇಶ_ದತ್ತಾಂಶ->queued;
	DL_DELETE(ಸಂದೇಶ_ದತ್ತಾಂಶ->queued, client_ಸಂದೇಶ);
	DL_APPEND(ಸಂದೇಶ_ದತ್ತಾಂಶ->inflight, client_ಸಂದೇಶ);
	ಆದರೆ(ಸಂದೇಶ_ದತ್ತಾಂಶ->inflight_quota > 0){
		ಸಂದೇಶ_ದತ್ತಾಂಶ->inflight_quota--;
	}

	db_ಸಂದೇಶ_remove_from_queued_stats(ಸಂದೇಶ_ದತ್ತಾಂಶ, client_ಸಂದೇಶ);
	db_ಸಂದೇಶ_ಕೂಡಿಸು_ಗಮ್ಯ_inflight_stats(ಸಂದೇಶ_ದತ್ತಾಂಶ, client_ಸಂದೇಶ);
}


ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_delete_outgoing(ರಚನೆ mosquitto *context, uint16_t mid, ಎಣಿಕೆ mosquitto_ಸಂದೇಶ_state expect_state, ಪೂರ್ಣಾಂಕ qos)
{
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;
	bool deleted = false;

	ಆದರೆ(!context){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	DL_FOREACH_ಸುರಕ್ಷಿತ(context->msgs_ಹೊರಹೋಗು.inflight, client_ಸಂದೇಶ, tmp){
		ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.mid == mid){
			ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos != qos){
				log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos, qos);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
			}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(qos == 2 && client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state != expect_state && expect_state != mosq_ms_any){
				log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state, expect_state);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
			}
			db_ಸಂದೇಶ_remove_inflight(context, &context->msgs_ಹೊರಹೋಗು, client_ಸಂದೇಶ);
			deleted = true;
			ನಿಲ್ಲಿಸು;
		}
	}

	ಆದರೆ(deleted == false){
		DL_FOREACH_ಸುರಕ್ಷಿತ(context->msgs_ಹೊರಹೋಗು.queued, client_ಸಂದೇಶ, tmp){
			ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.mid == mid){
				ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos != qos){
					ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(qos == 2 && client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state != expect_state && expect_state != mosq_ms_any){
					ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
				}
				db_ಸಂದೇಶ_remove_queued(context, &context->msgs_ಹೊರಹೋಗು, client_ಸಂದೇಶ);
				ನಿಲ್ಲಿಸು;
			}
		}
	}
	db_fill_inflight_ಹೊರಹೋಗು_from_ಸರತಿ(context);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_PERSISTENCE
	db.persistence_changes++;
#ಆದರೆ_ಅಂತ್ಯ

	ಹಿಂತಿರುಗಿ db_ಸಂದೇಶ_write_inflight_ಹೊರಹೋಗು_latest(context);
}


/* Only for QoS 2 messages */
ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_ಸೇರಿಸು_incoming(ರಚನೆ mosquitto *context, uint64_t cmsg_id, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ, bool persist)
{
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ;
	ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ *ಸಂದೇಶ_ದತ್ತಾಂಶ;
	ಎಣಿಕೆ mosquitto_ಸಂದೇಶ_state state = mosq_ms_invalid;
	ಪೂರ್ಣಾಂಕ rc = 0;

	ಪ್ರತಿಜ್ಞಾ(ತಳ_ಸಂದೇಶ);
	ಆದರೆ(!context){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	ಆದರೆ(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;

	}
	ಸಂದೇಶ_ದತ್ತಾಂಶ = &context->msgs_ಒಳ;

	ಆದರೆ(db__ready_for_flight(context, mosq_md_ಒಳ, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos)){
		state = mosq_ms_wait_for_pubrel;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos != 0 && db_ready_for_ಸರತಿ(context, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos, ಸಂದೇಶ_ದತ್ತಾಂಶ)){
		state = mosq_ms_queued;
		rc = 2;
	}ಇಲ್ಲದಿದ್ದರೆ{
		/* Dropping message due to full queue. */
		ಆದರೆ(context->is_dropping == false){
			context->is_dropping = true;
			log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE,
					"Outgoing messages are being dropped for client %s.",
					context->id);
		}
		metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		ಹಿಂತಿರುಗಿ 2;
	}

	ಪ್ರತಿಜ್ಞಾ(state != mosq_ms_invalid);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_PERSISTENCE
	ಆದರೆ(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#ಆದರೆ_ಅಂತ್ಯ

	client_ಸಂದೇಶ = mosquitto_malloc(ಗಾತ್ರ(ರಚನೆ mosquitto_client_ಸಂದೇಶ));
	ಆದರೆ(!client_ಸಂದೇಶ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	client_ಸಂದೇಶ->prev = ಖಾಲಿ;
	client_ಸಂದೇಶ->ಮುಂದಿನ = ಖಾಲಿ;
	ಆದರೆ(cmsg_id){
		client_ಸಂದೇಶ->ದತ್ತಾಂಶ.cmsg_id = cmsg_id;
	}ಇಲ್ಲದಿದ್ದರೆ{
		client_ಸಂದೇಶ->ದತ್ತಾಂಶ.cmsg_id = ++context->ಕೊನೆಯ_cmsg_id;
	}
	client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ = ತಳ_ಸಂದೇಶ;
	db_ಸಂದೇಶ_store_ref_inc(client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ);
	client_ಸಂದೇಶ->ದತ್ತಾಂಶ.mid = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid;
	client_ಸಂದೇಶ->ದತ್ತಾಂಶ.direction = mosq_md_ಒಳ;
	client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state = (uint8_t)state;
	client_ಸಂದೇಶ->ದತ್ತಾಂಶ.dup = false;
	ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos > context->max_qos){
		client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos = context->max_qos;
	}ಇಲ್ಲದಿದ್ದರೆ{
		client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos;
	}
	client_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain;
	client_ಸಂದೇಶ->ದತ್ತಾಂಶ.subscription_identifier = 0;

	ಆದರೆ(state == mosq_ms_queued){
		DL_APPEND(ಸಂದೇಶ_ದತ್ತಾಂಶ->queued, client_ಸಂದೇಶ);
		db_ಸಂದೇಶ_ಕೂಡಿಸು_ಗಮ್ಯ_queued_stats(ಸಂದೇಶ_ದತ್ತಾಂಶ, client_ಸಂದೇಶ);
	}ಇಲ್ಲದಿದ್ದರೆ{
		DL_APPEND(ಸಂದೇಶ_ದತ್ತಾಂಶ->inflight, client_ಸಂದೇಶ);
		db_ಸಂದೇಶ_ಕೂಡಿಸು_ಗಮ್ಯ_inflight_stats(ಸಂದೇಶ_ದತ್ತಾಂಶ, client_ಸಂದೇಶ);
	}

	ಆದರೆ(persist && context->is_persisted){
		plugin_persist_handle_ತಳ_ಸಂದೇಶ_ಕೂಡಿಸು(client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ);
		plugin_persist_handle_client_ಸಂದೇಶ_ಕೂಡಿಸು(context, client_ಸಂದೇಶ);
	}

	ಆದರೆ(client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos > 0){
		util__decrement_receive_quota(context);
	}
	ಹಿಂತಿರುಗಿ rc;
}


ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_ಸೇರಿಸು_outgoing(ರಚನೆ mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ, uint32_t subscription_identifier, bool update, bool persist)
{
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ;
	ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ *ಸಂದೇಶ_ದತ್ತಾಂಶ;
	ಎಣಿಕೆ mosquitto_ಸಂದೇಶ_state state = mosq_ms_invalid;
	ಪೂರ್ಣಾಂಕ rc = 0;
	ಅಕ್ಷರ **dest_ids;

	ಪ್ರತಿಜ್ಞಾ(ತಳ_ಸಂದೇಶ);
	ಆದರೆ(!context){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	ಆದರೆ(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_ಕಳುಹಿಸಿದ++;

	ಸಂದೇಶ_ದತ್ತಾಂಶ = &context->msgs_ಹೊರಹೋಗು;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	ಆದರೆ(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && ತಳ_ಸಂದೇಶ->dest_ids){

		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<ತಳ_ಸಂದೇಶ->dest_id_ಎಣಿಕೆ; i++){
			ಆದರೆ(ತಳ_ಸಂದೇಶ->dest_ids[i] && !strcmp(ತಳ_ಸಂದೇಶ->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
			}
		}
	}
	ಆದರೆ(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		ಆದರೆ(qos == 0 && !db.config->ಸರತಿ_qos0_messages){
			ಆದರೆ(!context->bridge){
				ಹಿಂತಿರುಗಿ 2;
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಆದರೆ(context->bridge->ಆರಂಭ_type != bst_lazy){
					ಹಿಂತಿರುಗಿ 2;
				}
			}
		}
		ಆದರೆ(context->bridge && context->bridge->clean_ಆರಂಭ_local == true){
			ಹಿಂತಿರುಗಿ 2;
		}
	}

	ಆದರೆ(net__is_connected(context)){
		ಆದರೆ(db__ready_for_flight(context, mosq_md_ಹೊರಹೋಗು, qos)){
			ಆಯ್ಕೆ(qos){
				ಸ್ಥಿತಿ 0:
					state = mosq_ms_publish_qos0;
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ 1:
					state = mosq_ms_publish_qos1;
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ 2:
					state = mosq_ms_publish_qos2;
					ನಿಲ್ಲಿಸು;
			}
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(qos != 0 && db_ready_for_ಸರತಿ(context, qos, ಸಂದೇಶ_ದತ್ತಾಂಶ)){
			state = mosq_ms_queued;
			rc = 2;
		}ಇಲ್ಲದಿದ್ದರೆ{
			/* Dropping message due to full queue. */
			ಆದರೆ(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			ಹಿಂತಿರುಗಿ 2;
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಆದರೆ(db_ready_for_ಸರತಿ(context, qos, ಸಂದೇಶ_ದತ್ತಾಂಶ)){
			state = mosq_ms_queued;
		}ಇಲ್ಲದಿದ್ದರೆ{
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			ಆದರೆ(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			ಹಿಂತಿರುಗಿ 2;
		}
	}
	ಪ್ರತಿಜ್ಞಾ(state != mosq_ms_invalid);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_PERSISTENCE
	ಆದರೆ(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#ಆದರೆ_ಅಂತ್ಯ

	client_ಸಂದೇಶ = mosquitto_malloc(ಗಾತ್ರ(ರಚನೆ mosquitto_client_ಸಂದೇಶ));
	ಆದರೆ(!client_ಸಂದೇಶ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	client_ಸಂದೇಶ->prev = ಖಾಲಿ;
	client_ಸಂದೇಶ->ಮುಂದಿನ = ಖಾಲಿ;
	ಆದರೆ(cmsg_id){
		client_ಸಂದೇಶ->ದತ್ತಾಂಶ.cmsg_id = cmsg_id;
	}ಇಲ್ಲದಿದ್ದರೆ{
		client_ಸಂದೇಶ->ದತ್ತಾಂಶ.cmsg_id = ++context->ಕೊನೆಯ_cmsg_id;
	}
	client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ = ತಳ_ಸಂದೇಶ;
	db_ಸಂದೇಶ_store_ref_inc(client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ);
	client_ಸಂದೇಶ->ದತ್ತಾಂಶ.mid = mid;
	client_ಸಂದೇಶ->ದತ್ತಾಂಶ.direction = mosq_md_ಹೊರಹೋಗು;
	client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state = (uint8_t)state;
	client_ಸಂದೇಶ->ದತ್ತಾಂಶ.dup = false;
	ಆದರೆ(qos > context->max_qos){
		client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos = context->max_qos;
	}ಇಲ್ಲದಿದ್ದರೆ{
		client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos = qos;
	}
	client_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain = retain;
	client_ಸಂದೇಶ->ದತ್ತಾಂಶ.subscription_identifier = subscription_identifier;

	ಆದರೆ(state == mosq_ms_queued){
		DL_APPEND(ಸಂದೇಶ_ದತ್ತಾಂಶ->queued, client_ಸಂದೇಶ);
		db_ಸಂದೇಶ_ಕೂಡಿಸು_ಗಮ್ಯ_queued_stats(ಸಂದೇಶ_ದತ್ತಾಂಶ, client_ಸಂದೇಶ);
	}ಇಲ್ಲದಿದ್ದರೆ{
		DL_APPEND(ಸಂದೇಶ_ದತ್ತಾಂಶ->inflight, client_ಸಂದೇಶ);
		db_ಸಂದೇಶ_ಕೂಡಿಸು_ಗಮ್ಯ_inflight_stats(ಸಂದೇಶ_ದತ್ತಾಂಶ, client_ಸಂದೇಶ);
	}

	ಆದರೆ(persist && context->is_persisted){
		plugin_persist_handle_ತಳ_ಸಂದೇಶ_ಕೂಡಿಸು(client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ);
		plugin_persist_handle_client_ಸಂದೇಶ_ಕೂಡಿಸು(context, client_ಸಂದೇಶ);
	}

	ಆದರೆ(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(ತಳ_ಸಂದೇಶ->dest_ids, ಗಾತ್ರ(ಅಕ್ಷರ *)*(size_t)(ತಳ_ಸಂದೇಶ->dest_id_ಎಣಿಕೆ+1));
		ಆದರೆ(dest_ids){
			ತಳ_ಸಂದೇಶ->dest_ids = dest_ids;
			ತಳ_ಸಂದೇಶ->dest_id_ಎಣಿಕೆ++;
			ತಳ_ಸಂದೇಶ->dest_ids[ತಳ_ಸಂದೇಶ->dest_id_ಎಣಿಕೆ-1] = mosquitto_strdup(context->id);
			ಆದರೆ(!ತಳ_ಸಂದೇಶ->dest_ids[ತಳ_ಸಂದೇಶ->dest_id_ಎಣಿಕೆ-1]){
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
	ಆದರೆ(context->bridge && context->bridge->ಆರಂಭ_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_ಹೊರಹೋಗು.inflight_ಎಣಿಕೆ + context->msgs_ಹೊರಹೋಗು.queued_ಎಣಿಕೆ >= context->bridge->ಮಿತಿ){

		context->bridge->lazy_reconnect = true;
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	ಆದರೆ(update){
		rc = db_ಸಂದೇಶ_write_inflight_ಹೊರಹೋಗು_latest(context);
		ಆದರೆ(rc){
			ಹಿಂತಿರುಗಿ rc;
		}
		rc = db_ಸಂದೇಶ_write_queued_ಹೊರಹೋಗು(context);
		ಆದರೆ(rc){
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	ಹಿಂತಿರುಗಿ rc;
}


ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_update_outgoing_state(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto_client_ಸಂದೇಶ *ತಲೆ,
		uint16_t mid, ಎಣಿಕೆ mosquitto_ಸಂದೇಶ_state state, ಪೂರ್ಣಾಂಕ qos, bool persist)
{
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ;

	DL_FOREACH(ತಲೆ, client_ಸಂದೇಶ){
		ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.mid == mid){
			ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos != qos){
				log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos, qos);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
			}
			client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state = (uint8_t)state;
			ಆದರೆ(persist){
				plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
			}
			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
		}
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_ಸಿಕ್ಕಿತು;
}


ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_update_outgoing(ರಚನೆ mosquitto *context, uint16_t mid, ಎಣಿಕೆ mosquitto_ಸಂದೇಶ_state state, ಪೂರ್ಣಾಂಕ qos, bool persist)
{
	ಪೂರ್ಣಾಂಕ rc;

	rc = db_ಸಂದೇಶ_update_outgoing_state(context, context->msgs_ಹೊರಹೋಗು.inflight, mid, state, qos, persist);
	ಆದರೆ(!persist && rc == MOSQ_ERR_NOT_ಸಿಕ್ಕಿತು){
		rc = db_ಸಂದೇಶ_update_outgoing_state(context, context->msgs_ಹೊರಹೋಗು.queued, mid, state, qos, persist);
	}
	ಹಿಂತಿರುಗಿ rc;
}


ಸ್ಥಿರ ಶೂನ್ಯ db__messages_delete_list(ರಚನೆ mosquitto_client_ಸಂದೇಶ **ತಲೆ)
{
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;

	DL_FOREACH_ಸುರಕ್ಷಿತ(*ತಲೆ, client_ಸಂದೇಶ, tmp){
		DL_DELETE(*ತಲೆ, client_ಸಂದೇಶ);
		db_ಸಂದೇಶ_store_ref_dec(&client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ);
		mosquitto_FREE(client_ಸಂದೇಶ);
	}
	*ತಲೆ = ಖಾಲಿ;
}


ಪೂರ್ಣಾಂಕ db__messages_delete_incoming(ರಚನೆ mosquitto *context)
{
	ಆದರೆ(!context){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_ಒಳ.inflight);
	db__messages_delete_list(&context->msgs_ಒಳ.queued);
	context->msgs_ಒಳ.inflight_ಬೈಟುಗಳು = 0;
	context->msgs_ಒಳ.inflight_bytes12 = 0;
	context->msgs_ಒಳ.inflight_ಎಣಿಕೆ = 0;
	context->msgs_ಒಳ.inflight_count12 = 0;
	context->msgs_ಒಳ.queued_ಬೈಟುಗಳು = 0;
	context->msgs_ಒಳ.queued_bytes12 = 0;
	context->msgs_ಒಳ.queued_ಎಣಿಕೆ = 0;
	context->msgs_ಒಳ.queued_count12 = 0;

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ db__messages_delete_outgoing(ರಚನೆ mosquitto *context)
{
	ಆದರೆ(!context){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_ಹೊರಹೋಗು.inflight);
	db__messages_delete_list(&context->msgs_ಹೊರಹೋಗು.queued);
	context->msgs_ಹೊರಹೋಗು.inflight_ಬೈಟುಗಳು = 0;
	context->msgs_ಹೊರಹೋಗು.inflight_bytes12 = 0;
	context->msgs_ಹೊರಹೋಗು.inflight_ಎಣಿಕೆ = 0;
	context->msgs_ಹೊರಹೋಗು.inflight_count12 = 0;
	context->msgs_ಹೊರಹೋಗು.queued_ಬೈಟುಗಳು = 0;
	context->msgs_ಹೊರಹೋಗು.queued_bytes12 = 0;
	context->msgs_ಹೊರಹೋಗು.queued_ಎಣಿಕೆ = 0;
	context->msgs_ಹೊರಹೋಗು.queued_count12 = 0;

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ db__messages_delete(ರಚನೆ mosquitto *context, bool force_free)
{
	ಆದರೆ(!context){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	ಆದರೆ(force_free || context->clean_ಆರಂಭ || (context->bridge && context->bridge->clean_ಆರಂಭ)){
		db__messages_delete_incoming(context);
	}

	ಆದರೆ(force_free || (context->bridge && context->bridge->clean_ಆರಂಭ_local)
			|| (context->bridge == ಖಾಲಿ && context->clean_ಆರಂಭ)){

		db__messages_delete_outgoing(context);
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ db_messages_easy_ಸರತಿ(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, uint8_t qos, uint32_t payloadlen, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *payload, ಪೂರ್ಣಾಂಕ retain, uint32_t ಸಂದೇಶ_expiry_interval, mosquitto_property **properties)
{
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ_id;
	ಎಣಿಕೆ mosquitto_ಸಂದೇಶ_origin origin;

	ಆದರೆ(!topic){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	ತಳ_ಸಂದೇಶ = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ));
	ಆದರೆ(ತಳ_ಸಂದೇಶ == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic = mosquitto_strdup(topic);
	ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic == ಖಾಲಿ){
		db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos = qos;
	ಆದರೆ(db.config->retain_ಲಭ್ಯ == false){
		ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain = 0;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain = retain;
	}

	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen = payloadlen;
	ಆದರೆ(payloadlen > 0){
		ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload = mosquitto_malloc(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen+1);
		ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload == ಖಾಲಿ){
			db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload)[ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen] = 0;
		ಸ್ಮರಣೆ_ನಕಲು(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload, payload, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen);
	}

	ಆದರೆ(context && context->id){
		ಮೂಲ_id = context->id;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಮೂಲ_id = "";
	}
	ಆದರೆ(properties){
		ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.properties = *properties;
		*properties = ಖಾಲಿ;
	}

	ಆದರೆ(context){
		origin = mosq_mo_client;
	}ಇಲ್ಲದಿದ್ದರೆ{
		origin = mosq_mo_broker;
	}
	ಆದರೆ(db_ಸಂದೇಶ_store(context, ತಳ_ಸಂದೇಶ, &ಸಂದೇಶ_expiry_interval, origin)){
		ಹಿಂತಿರುಗಿ 1;
	}

	ಹಿಂತಿರುಗಿ sub_messages_ಸರತಿ(ಮೂಲ_id, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos, &ತಳ_ಸಂದೇಶ);
}


#ವ್ಯಾಖ್ಯಾನಿಸು MOSQ_UUID_EPOCH 1637168273


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
uint64_t db_new_ಸಂದೇಶ_id(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
	FILETIME ftime;
	uint64_t ftime64;
#ಇಲ್ಲದಿದ್ದರೆ
	ರಚನೆ timespec ts;
#ಆದರೆ_ಅಂತ್ಯ
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	ದೀರ್ಘ nsec;

	id = db.ಗಂಟು_id_shifted; /* Top 10-bits */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
	Get_System_ಸಮಯ_Precise_As_File_ಸಮಯ(&ftime);
	ftime64 = (((uint64_t)ftime.dw_ಮೇಲಿನ_Date_ಸಮಯ)<<32) + ftime.dw_ಕೆಳಗಿನ_Date_ಸಮಯ;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (ದೀರ್ಘ)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#ಇಲ್ಲದಿದ್ದರೆ
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#ಆದರೆ_ಅಂತ್ಯ
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	ಆದರೆ(id <= db.ಕೊನೆಯ_db_id){
		id = db.ಕೊನೆಯ_db_id + 1;
	}
	db.ಕೊನೆಯ_db_id = id;

	ಹಿಂತಿರುಗಿ id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_store(ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto *ಮೂಲ, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ, uint32_t *ಸಂದೇಶ_expiry_interval, ಎಣಿಕೆ mosquitto_ಸಂದೇಶ_origin origin)
{
	ಪೂರ್ಣಾಂಕ rc;

	ಪ್ರತಿಜ್ಞಾ(ತಳ_ಸಂದೇಶ);

	ಆದರೆ(ಮೂಲ && ಮೂಲ->id){
		ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_id = mosquitto_strdup(ಮೂಲ->id);
	}ಇಲ್ಲದಿದ್ದರೆ{
		ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_id = mosquitto_strdup("");
	}
	ಆದರೆ(!ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_id){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	ಆದರೆ(ಮೂಲ && ಮೂಲ->username){
		ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_username = mosquitto_strdup(ಮೂಲ->username);
		ಆದರೆ(!ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_username){
			db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
	}
	ಆದರೆ(ಮೂಲ){
		ತಳ_ಸಂದೇಶ->ಮೂಲ_listener = ಮೂಲ->listener;
	}
	ತಳ_ಸಂದೇಶ->origin = origin;
	ಆದರೆ(ಸಂದೇಶ_expiry_interval){
		ಆದರೆ(*ಸಂದೇಶ_expiry_interval > 0 && *ಸಂದೇಶ_expiry_interval != ಸಂದೇಶ_EXPIRY_INFINITE){
			ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.expiry_ಸಮಯ = db.now_real_s + *ಸಂದೇಶ_expiry_interval;
		}ಇಲ್ಲದಿದ್ದರೆ{
			ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.expiry_ಸಮಯ = 0;
		}
	}

	ತಳ_ಸಂದೇಶ->dest_ids = ಖಾಲಿ;
	ತಳ_ಸಂದೇಶ->dest_id_ಎಣಿಕೆ = 0;
	db.ಸಂದೇಶ_store_ಎಣಿಕೆ++;
	db.ಸಂದೇಶ_store_ಬೈಟುಗಳು += ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen;

	ಆದರೆ(!ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.store_id){
		ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.store_id = db_new_ಸಂದೇಶ_id();
	}

	rc = db_ಸಂದೇಶ_store_ಕೂಡಿಸು(ತಳ_ಸಂದೇಶ);
	ಆದರೆ(rc){
		db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_store_ಕಂಡುಹಿಡಿ(ರಚನೆ mosquitto *context, uint16_t mid, ರಚನೆ mosquitto_client_ಸಂದೇಶ **client_ಸಂದೇಶ)
{
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *cmsg;

	*client_ಸಂದೇಶ = ಖಾಲಿ;

	ಆದರೆ(!context){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_ಒಳ.inflight, cmsg){
		ಆದರೆ(cmsg->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid == mid){
			*client_ಸಂದೇಶ = cmsg;
			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_ಒಳ.queued, cmsg){
		ಆದರೆ(cmsg->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid == mid){
			*client_ಸಂದೇಶ = cmsg;
			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
		}
	}

	ಹಿಂತಿರುಗಿ 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_reconnect_reset_outgoing(ರಚನೆ mosquitto *context)
{
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;

	context->msgs_ಹೊರಹೋಗು.inflight_ಬೈಟುಗಳು = 0;
	context->msgs_ಹೊರಹೋಗು.inflight_bytes12 = 0;
	context->msgs_ಹೊರಹೋಗು.inflight_ಎಣಿಕೆ = 0;
	context->msgs_ಹೊರಹೋಗು.inflight_count12 = 0;
	context->msgs_ಹೊರಹೋಗು.queued_ಬೈಟುಗಳು = 0;
	context->msgs_ಹೊರಹೋಗು.queued_bytes12 = 0;
	context->msgs_ಹೊರಹೋಗು.queued_ಎಣಿಕೆ = 0;
	context->msgs_ಹೊರಹೋಗು.queued_count12 = 0;
	context->msgs_ಹೊರಹೋಗು.inflight_quota = context->msgs_ಹೊರಹೋಗು.inflight_ಗರಿಷ್ಠ;

	DL_FOREACH_ಸುರಕ್ಷಿತ(context->msgs_ಹೊರಹೋಗು.inflight, client_ಸಂದೇಶ, tmp){
		db_ಸಂದೇಶ_ಕೂಡಿಸು_ಗಮ್ಯ_inflight_stats(&context->msgs_ಹೊರಹೋಗು, client_ಸಂದೇಶ);
		ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos > 0){
			util__decrement_send_quota(context);
		}

		ಆಯ್ಕೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos){
			ಸ್ಥಿತಿ 0:
				client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state = mosq_ms_publish_qos0;
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 1:
				client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state = mosq_ms_publish_qos1;
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 2:
				ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state == mosq_ms_wait_for_pubcomp){
					client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state = mosq_ms_resend_pubrel;
				}ಇಲ್ಲದಿದ್ದರೆ{
					client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state = mosq_ms_publish_qos2;
				}
				ನಿಲ್ಲಿಸು;
		}
		plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_ಸುರಕ್ಷಿತ(context->msgs_ಹೊರಹೋಗು.queued, client_ಸಂದೇಶ, tmp){
		db_ಸಂದೇಶ_ಕೂಡಿಸು_ಗಮ್ಯ_queued_stats(&context->msgs_ಹೊರಹೋಗು, client_ಸಂದೇಶ);
	}
	db_fill_inflight_ಹೊರಹೋಗು_from_ಸರತಿ(context);

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_reconnect_reset_incoming(ರಚನೆ mosquitto *context)
{
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;

	context->msgs_ಒಳ.inflight_ಬೈಟುಗಳು = 0;
	context->msgs_ಒಳ.inflight_bytes12 = 0;
	context->msgs_ಒಳ.inflight_ಎಣಿಕೆ = 0;
	context->msgs_ಒಳ.inflight_count12 = 0;
	context->msgs_ಒಳ.queued_ಬೈಟುಗಳು = 0;
	context->msgs_ಒಳ.queued_bytes12 = 0;
	context->msgs_ಒಳ.queued_ಎಣಿಕೆ = 0;
	context->msgs_ಒಳ.queued_count12 = 0;
	context->msgs_ಒಳ.inflight_quota = context->msgs_ಒಳ.inflight_ಗರಿಷ್ಠ;

	DL_FOREACH_ಸುರಕ್ಷಿತ(context->msgs_ಒಳ.inflight, client_ಸಂದೇಶ, tmp){
		db_ಸಂದೇಶ_ಕೂಡಿಸು_ಗಮ್ಯ_inflight_stats(&context->msgs_ಒಳ, client_ಸಂದೇಶ);
		ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos > 0){
			util__decrement_receive_quota(context);
		}

		ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db_ಸಂದೇಶ_remove_inflight(context, &context->msgs_ಒಳ, client_ಸಂದೇಶ);
		}ಇಲ್ಲದಿದ್ದರೆ{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_ಸಂದೇಶ->ದತ್ತಾಂಶ.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_ಸುರಕ್ಷಿತ(context->msgs_ಒಳ.queued, client_ಸಂದೇಶ, tmp){
		client_ಸಂದೇಶ->ದತ್ತಾಂಶ.dup = 0;
		db_ಸಂದೇಶ_ಕೂಡಿಸು_ಗಮ್ಯ_queued_stats(&context->msgs_ಒಳ, client_ಸಂದೇಶ);
		ಆದರೆ(db__ready_for_flight(context, mosq_md_ಒಳ, client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos)){
			ಆಯ್ಕೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos){
				ಸ್ಥಿತಿ 0:
					client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state = mosq_ms_publish_qos0;
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ 1:
					client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state = mosq_ms_publish_qos1;
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ 2:
					client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state = mosq_ms_publish_qos2;
					ನಿಲ್ಲಿಸು;
			}
			db_ಸಂದೇಶ_ಸರತಿಯಿಂದ_ತೆಗೆ_ಮೊದಲ(context, &context->msgs_ಒಳ);
			plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_reconnect_reset(ರಚನೆ mosquitto *context)
{
	ಪೂರ್ಣಾಂಕ rc;

	rc = db_ಸಂದೇಶ_reconnect_reset_outgoing(context);
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}
	ಹಿಂತಿರುಗಿ db_ಸಂದೇಶ_reconnect_reset_incoming(context);
}


ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_remove_incoming(ರಚನೆ mosquitto *context, uint16_t mid)
{
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;

	ಆದರೆ(!context){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	DL_FOREACH_ಸುರಕ್ಷಿತ(context->msgs_ಒಳ.inflight, client_ಸಂದೇಶ, tmp){
		ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.mid == mid){
			ಆದರೆ(client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos != 2){
				log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
			}
			db_ಸಂದೇಶ_remove_inflight(context, &context->msgs_ಒಳ, client_ಸಂದೇಶ);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_ಸಿಕ್ಕಿತು;
}


ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_release_incoming(ರಚನೆ mosquitto *context, uint16_t mid)
{
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;
	bool deleted = false;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(!context){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	DL_FOREACH_ಸುರಕ್ಷಿತ(context->msgs_ಒಳ.inflight, client_ಸಂದೇಶ, tmp){
		ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.mid == mid){
			ಆದರೆ(client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos != 2){
				log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
			}
			ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic = client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic;
			ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ_id = client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			ಆದರೆ(topic == ಖಾಲಿ){
				db_ಸಂದೇಶ_remove_inflight(context, &context->msgs_ಒಳ, client_ಸಂದೇಶ);
				deleted = true;
			}ಇಲ್ಲದಿದ್ದರೆ{
				rc = sub_messages_ಸರತಿ(ಮೂಲ_id, 2, &client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ);
				ಆದರೆ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db_ಸಂದೇಶ_remove_inflight(context, &context->msgs_ಒಳ, client_ಸಂದೇಶ);
					deleted = true;
				}ಇಲ್ಲದಿದ್ದರೆ{
					ಹಿಂತಿರುಗಿ 1;
				}
			}
		}
	}

	DL_FOREACH_ಸುರಕ್ಷಿತ(context->msgs_ಒಳ.queued, client_ಸಂದೇಶ, tmp){
		ಆದರೆ(db__ready_for_flight(context, mosq_md_ಒಳ, client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos)){
			ನಿಲ್ಲಿಸು;
		}

		ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos == 2){
			send__pubrec(context, client_ಸಂದೇಶ->ದತ್ತಾಂಶ.mid, 0, ಖಾಲಿ);
			client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state = mosq_ms_wait_for_pubrel;
			db_ಸಂದೇಶ_ಸರತಿಯಿಂದ_ತೆಗೆ_ಮೊದಲ(context, &context->msgs_ಒಳ);
			plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
		}
	}
	ಆದರೆ(deleted){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_ಸಿಕ್ಕಿತು;
	}
}


ಶೂನ್ಯ db__expire_all_messages(ರಚನೆ mosquitto *context)
{
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;

	DL_FOREACH_ಸುರಕ್ಷಿತ(context->msgs_ಹೊರಹೋಗು.inflight, client_ಸಂದೇಶ, tmp){
		ಆದರೆ(client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.expiry_ಸಮಯ && db.now_real_s > client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.expiry_ಸಮಯ){
			ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos > 0){
				util__increment_send_quota(context);
			}
			db_ಸಂದೇಶ_remove_inflight(context, &context->msgs_ಹೊರಹೋಗು, client_ಸಂದೇಶ);
		}
	}
	db_fill_inflight_ಹೊರಹೋಗು_from_ಸರತಿ(context);
	DL_FOREACH_ಸುರಕ್ಷಿತ(context->msgs_ಹೊರಹೋಗು.queued, client_ಸಂದೇಶ, tmp){
		ಆದರೆ(client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.expiry_ಸಮಯ && db.now_real_s > client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.expiry_ಸಮಯ){
			db_ಸಂದೇಶ_remove_queued(context, &context->msgs_ಹೊರಹೋಗು, client_ಸಂದೇಶ);
		}
	}
	DL_FOREACH_ಸುರಕ್ಷಿತ(context->msgs_ಒಳ.inflight, client_ಸಂದೇಶ, tmp){
		ಆದರೆ(client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.expiry_ಸಮಯ && db.now_real_s > client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.expiry_ಸಮಯ){
			ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos > 0){
				util__increment_receive_quota(context);
			}
			db_ಸಂದೇಶ_remove_inflight(context, &context->msgs_ಒಳ, client_ಸಂದೇಶ);
		}
	}
	DL_FOREACH_ಸುರಕ್ಷಿತ(context->msgs_ಒಳ.queued, client_ಸಂದೇಶ, tmp){
		ಆದರೆ(client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.expiry_ಸಮಯ && db.now_real_s > client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.expiry_ಸಮಯ){
			db_ಸಂದೇಶ_remove_queued(context, &context->msgs_ಒಳ, client_ಸಂದೇಶ);
		}
	}
}


ಸ್ಥಿರ ಶೂನ್ಯ db__client_messages_check_acl(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ *ಸಂದೇಶ_ದತ್ತಾಂಶ, ರಚನೆ mosquitto_client_ಸಂದೇಶ **ತಲೆ,
		ಶೂನ್ಯ (*decrement_stats_fn)(ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ *ಸಂದೇಶ_ದತ್ತಾಂಶ, ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ))
{
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ;
	ಪೂರ್ಣಾಂಕ access;

	DL_FOREACH_ಸುರಕ್ಷಿತ((*ತಲೆ), client_ಸಂದೇಶ, tmp){
		ತಳ_ಸಂದೇಶ = client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ;
		ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.direction == mosq_md_ಹೊರಹೋಗು){
			access = MOSQ_ACL_READ;
		}ಇಲ್ಲದಿದ್ದರೆ{
			access = MOSQ_ACL_WRITE;
		}
		ಆದರೆ(mosquitto_acl_check(context, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic,
				ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload,
				ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain,
				ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*ತಲೆ), client_ಸಂದೇಶ);
			decrement_stats_fn(ಸಂದೇಶ_ದತ್ತಾಂಶ, client_ಸಂದೇಶ);
			plugin_persist_handle_client_ಸಂದೇಶ_delete(context, client_ಸಂದೇಶ);
			db_ಸಂದೇಶ_store_ref_dec(&client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ);
			mosquitto_FREE(client_ಸಂದೇಶ);
		}
	}
}


ಶೂನ್ಯ db_check_acl_ದ_all_messages(ರಚನೆ mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_ಒಳ, &context->msgs_ಒಳ.inflight, &db_ಸಂದೇಶ_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_ಒಳ, &context->msgs_ಒಳ.queued, &db_ಸಂದೇಶ_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_ಹೊರಹೋಗು, &context->msgs_ಹೊರಹೋಗು.inflight, &db_ಸಂದೇಶ_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_ಹೊರಹೋಗು, &context->msgs_ಹೊರಹೋಗು.queued, &db_ಸಂದೇಶ_remove_from_queued_stats);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_write_inflight_ಹೊರಹೋಗು_single(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ)
{
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ;
	mosquitto_property *ತಳ_ಸಂದೇಶ_props = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ rc;
	uint16_t mid;
	ಪೂರ್ಣಾಂಕ retries;
	ಪೂರ್ಣಾಂಕ retain;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic;
	uint8_t qos;
	uint32_t payloadlen;
	ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *payload;
	uint32_t expiry_interval;
	uint32_t subscription_id;

	ತಳ_ಸಂದೇಶ = client_ಸಂದೇಶ->ತಳ_ಸಂದೇಶ;

	expiry_interval = 0;
	ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.expiry_ಸಮಯ){
		ಆದರೆ(db.now_real_s > ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.expiry_ಸಮಯ){
			/* Message is expired, must not send. */
			ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.direction == mosq_md_ಹೊರಹೋಗು && client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos > 0){
				util__increment_send_quota(context);
			}
			db_ಸಂದೇಶ_remove_inflight(context, &context->msgs_ಹೊರಹೋಗು, client_ಸಂದೇಶ);
			db_fill_inflight_ಹೊರಹೋಗು_from_ಸರತಿ(context);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
		}ಇಲ್ಲದಿದ್ದರೆ{
			expiry_interval = (uint32_t)(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.expiry_ಸಮಯ - db.now_real_s);
		}
	}
	mid = client_ಸಂದೇಶ->ದತ್ತಾಂಶ.mid;
	retries = client_ಸಂದೇಶ->ದತ್ತಾಂಶ.dup;
	retain = client_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain;
	topic = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic;
	qos = (uint8_t)client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos;
	payloadlen = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen;
	payload = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload;
	subscription_id = client_ಸಂದೇಶ->ದತ್ತಾಂಶ.subscription_identifier;
	ತಳ_ಸಂದೇಶ_props = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.properties;

	ಆಯ್ಕೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state){
		ಸ್ಥಿತಿ mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, ತಳ_ಸಂದೇಶ_props, expiry_interval);
			ಆದರೆ(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_ಸಂದೇಶ_remove_inflight(context, &context->msgs_ಹೊರಹೋಗು, client_ಸಂದೇಶ);
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಹಿಂತಿರುಗಿ rc;
			}
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, ತಳ_ಸಂದೇಶ_props, expiry_interval);
			ಆದರೆ(rc == MOSQ_ERR_SUCCESS){
				client_ಸಂದೇಶ->ದತ್ತಾಂಶ.dup = 1; /* Any retry attempts are a duplicate. */
				client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
			}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_ಸಂದೇಶ_remove_inflight(context, &context->msgs_ಹೊರಹೋಗು, client_ಸಂದೇಶ);
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಹಿಂತಿರುಗಿ rc;
			}
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, ತಳ_ಸಂದೇಶ_props, expiry_interval);
			ಆದರೆ(rc == MOSQ_ERR_SUCCESS){
				client_ಸಂದೇಶ->ದತ್ತಾಂಶ.dup = 1; /* Any retry attempts are a duplicate. */
				client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
			}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_ಸಂದೇಶ_remove_inflight(context, &context->msgs_ಹೊರಹೋಗು, client_ಸಂದೇಶ);
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಹಿಂತಿರುಗಿ rc;
			}
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, ಖಾಲಿ);
			ಆದರೆ(!rc){
				client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಹಿಂತಿರುಗಿ rc;
			}
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ mosq_ms_invalid:
		ಸ್ಥಿತಿ mosq_ms_send_pubrec:
		ಸ್ಥಿತಿ mosq_ms_resend_pubcomp:
		ಸ್ಥಿತಿ mosq_ms_wait_for_puback:
		ಸ್ಥಿತಿ mosq_ms_wait_for_pubrec:
		ಸ್ಥಿತಿ mosq_ms_wait_for_pubrel:
		ಸ್ಥಿತಿ mosq_ms_wait_for_pubcomp:
		ಸ್ಥಿತಿ mosq_ms_queued:
			ನಿಲ್ಲಿಸು;
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_write_inflight_ಹೊರಹೋಗು_all(ರಚನೆ mosquitto *context)
{
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(context->state != mosq_cs_ಸಕ್ರಿಯ || !net__is_connected(context)){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_ಸುರಕ್ಷಿತ(context->msgs_ಹೊರಹೋಗು.inflight, client_ಸಂದೇಶ, tmp){
		rc = db_ಸಂದೇಶ_write_inflight_ಹೊರಹೋಗು_single(context, client_ಸಂದೇಶ);
		ಆದರೆ(rc){
			ಹಿಂತಿರುಗಿ rc;
		}
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_write_inflight_ಹೊರಹೋಗು_latest(ರಚನೆ mosquitto *context)
{
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *ಮುಂದಿನ;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(context->state != mosq_cs_ಸಕ್ರಿಯ
			|| !net__is_connected(context)
			|| context->msgs_ಹೊರಹೋಗು.inflight == ಖಾಲಿ){

		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	ಆದರೆ(context->msgs_ಹೊರಹೋಗು.inflight->prev == context->msgs_ಹೊರಹೋಗು.inflight){
		/* Only one message */
		ಹಿಂತಿರುಗಿ db_ಸಂದೇಶ_write_inflight_ಹೊರಹೋಗು_single(context, context->msgs_ಹೊರಹೋಗು.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_ಸಂದೇಶ = context->msgs_ಹೊರಹೋಗು.inflight->prev;
	ಆಗ(client_ಸಂದೇಶ != context->msgs_ಹೊರಹೋಗು.inflight &&
			(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state == mosq_ms_publish_qos0
			|| client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state == mosq_ms_publish_qos1
			|| client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state == mosq_ms_publish_qos2)){

		client_ಸಂದೇಶ = client_ಸಂದೇಶ->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	ಆದರೆ(client_ಸಂದೇಶ != context->msgs_ಹೊರಹೋಗು.inflight){
		client_ಸಂದೇಶ = client_ಸಂದೇಶ->ಮುಂದಿನ;
	}

	ಆಗ(client_ಸಂದೇಶ){
		ಮುಂದಿನ = client_ಸಂದೇಶ->ಮುಂದಿನ;
		rc = db_ಸಂದೇಶ_write_inflight_ಹೊರಹೋಗು_single(context, client_ಸಂದೇಶ);
		ಆದರೆ(rc){
			ಹಿಂತಿರುಗಿ rc;
		}
		client_ಸಂದೇಶ = ಮುಂದಿನ;
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_write_queued_ಒಳ(ರಚನೆ mosquitto *context)
{
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *client_ಸಂದೇಶ, *tmp;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(context->state != mosq_cs_ಸಕ್ರಿಯ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_ಸುರಕ್ಷಿತ(context->msgs_ಒಳ.queued, client_ಸಂದೇಶ, tmp){
		ಆದರೆ(context->msgs_ಒಳ.inflight_ಗರಿಷ್ಠ != 0 && context->msgs_ಒಳ.inflight_quota == 0){
			ನಿಲ್ಲಿಸು;
		}

		ಆದರೆ(client_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos == 2){
			client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state = mosq_ms_send_pubrec;
			db_ಸಂದೇಶ_ಸರತಿಯಿಂದ_ತೆಗೆ_ಮೊದಲ(context, &context->msgs_ಒಳ);
			rc = send__pubrec(context, client_ಸಂದೇಶ->ದತ್ತಾಂಶ.mid, 0, ಖಾಲಿ);
			ಆದರೆ(!rc){
				client_ಸಂದೇಶ->ದತ್ತಾಂಶ.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
			}ಇಲ್ಲದಿದ್ದರೆ{
				plugin_persist_handle_client_ಸಂದೇಶ_update(context, client_ಸಂದೇಶ);
				ಹಿಂತಿರುಗಿ rc;
			}
		}
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ db_ಸಂದೇಶ_write_queued_ಹೊರಹೋಗು(ರಚನೆ mosquitto *context)
{
	ಆದರೆ(context->state != mosq_cs_ಸಕ್ರಿಯ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	db_fill_inflight_ಹೊರಹೋಗು_from_ಸರತಿ(context);

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}
