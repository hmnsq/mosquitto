/*
Copyright (c) 2010-2021 Roger Light <roger@atchoo.org>

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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_PERSISTENCE

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <time.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "persist.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ persist__client_messages_save(FILE *db_fptr, ರಚನೆ mosquitto *context, ರಚನೆ mosquitto_client_ಸಂದೇಶ *ಸರತಿ)
{
	ರಚನೆ P_client_ಸಂದೇಶ chunk;
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *cmsg;
	ಪೂರ್ಣಾಂಕ rc;

	ಪ್ರತಿಜ್ಞಾ(db_fptr);
	ಪ್ರತಿಜ್ಞಾ(context);

	cmsg = ಸರತಿ;
	ಆಗ(cmsg){
		ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmsg->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic, "$SYS", 4)
				&& cmsg->ತಳ_ಸಂದೇಶ->ref_ಎಣಿಕೆ <= 1
				&& cmsg->ತಳ_ಸಂದೇಶ->dest_id_ಎಣಿಕೆ == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->ಮುಂದಿನ;
			ಮುಂದುವರಿ;
		}

		memset(&chunk, 0, ಗಾತ್ರ(ರಚನೆ P_client_ಸಂದೇಶ));

		chunk.F.store_id = cmsg->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.store_id;
		chunk.F.mid = cmsg->ದತ್ತಾಂಶ.mid;
		chunk.F.id_len = (uint16_t)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->id);
		chunk.F.qos = cmsg->ದತ್ತಾಂಶ.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->ದತ್ತಾಂಶ.retain&0x0F)<<4 | (cmsg->ದತ್ತಾಂಶ.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->ದತ್ತಾಂಶ.direction;
		chunk.F.state = (uint8_t)cmsg->ದತ್ತಾಂಶ.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->ದತ್ತಾಂಶ.subscription_identifier;

		rc = persist_chunk_client_ಸಂದೇಶ_write_v6(db_fptr, &chunk);
		ಆದರೆ(rc){
			ಹಿಂತಿರುಗಿ rc;
		}

		cmsg = cmsg->ಮುಂದಿನ;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ persist_ಸಂದೇಶ_store_save(FILE *db_fptr)
{
	ರಚನೆ P_ತಳ_ಸಂದೇಶ chunk;
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ, *ತಳ_ಸಂದೇಶ_tmp;
	ಪೂರ್ಣಾಂಕ rc;

	ಪ್ರತಿಜ್ಞಾ(db_fptr);

	ತಳ_ಸಂದೇಶ = db.ಸಂದೇಶ_store;
	HASH_ITER(hh, db.ಸಂದೇಶ_store, ತಳ_ಸಂದೇಶ, ತಳ_ಸಂದೇಶ_tmp){
		ಆದರೆ(ತಳ_ಸಂದೇಶ->ref_ಎಣಿಕೆ < 1 || ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic == ಖಾಲಿ){
			ಮುಂದುವರಿ;
		}

		memset(&chunk, 0, ಗಾತ್ರ(ರಚನೆ P_ತಳ_ಸಂದೇಶ));

		ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic, "$SYS", 4)){
			ಆದರೆ(ತಳ_ಸಂದೇಶ->ref_ಎಣಿಕೆ <= 1 && ತಳ_ಸಂದೇಶ->dest_id_ಎಣಿಕೆ == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				ಮುಂದುವರಿ;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}ಇಲ್ಲದಿದ್ದರೆ{
			chunk.F.retain = (uint8_t)ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain;
		}

		chunk.F.store_id = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.store_id;
		chunk.F.expiry_ಸಮಯ = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.expiry_ಸಮಯ;
		chunk.F.payloadlen = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen;
		chunk.F.ಮೂಲ_mid = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid;
		ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_id){
			chunk.F.ಮೂಲ_id_len = (uint16_t)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_id);
			chunk.ಮೂಲ.id = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_id;
		}ಇಲ್ಲದಿದ್ದರೆ{
			chunk.F.ಮೂಲ_id_len = 0;
			chunk.ಮೂಲ.id = ಖಾಲಿ;
		}
		ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_username){
			chunk.F.ಮೂಲ_username_len = (uint16_t)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_username);
			chunk.ಮೂಲ.username = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_username;
		}ಇಲ್ಲದಿದ್ದರೆ{
			chunk.F.ಮೂಲ_username_len = 0;
			chunk.ಮೂಲ.username = ಖಾಲಿ;
		}

		chunk.F.topic_len = (uint16_t)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic);
		chunk.topic = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic;

		ಆದರೆ(ತಳ_ಸಂದೇಶ->ಮೂಲ_listener){
			chunk.F.ಮೂಲ_ಪೋರ್ಟ್ = ತಳ_ಸಂದೇಶ->ಮೂಲ_listener->ಪೋರ್ಟ್;
		}ಇಲ್ಲದಿದ್ದರೆ{
			chunk.F.ಮೂಲ_ಪೋರ್ಟ್ = 0;
		}
		chunk.F.qos = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos;
		chunk.payload = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload;
		chunk.properties = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.properties;

		rc = persist_chunk_ಸಂದೇಶ_store_write_v6(db_fptr, &chunk);
		ಆದರೆ(rc){
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ persist__client_save(FILE *db_fptr)
{
	ರಚನೆ mosquitto *context, *ctxt_tmp;
	ರಚನೆ P_client chunk;
	ಪೂರ್ಣಾಂಕ rc;

	ಪ್ರತಿಜ್ಞಾ(db_fptr);

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		memset(&chunk, 0, ಗಾತ್ರ(ರಚನೆ P_client));

		ಆದರೆ(context &&
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_ತಕ್ಷಣ &&
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
				((!context->bridge && context->clean_ಆರಂಭ == false)
				|| (context->bridge && context->bridge->clean_ಆರಂಭ_local == false))
#ಇಲ್ಲದಿದ್ದರೆ
				context->clean_ಆರಂಭ == false
#ಆದರೆ_ಅಂತ್ಯ
				){
			chunk.F.session_expiry_ಸಮಯ = context->session_expiry_ಸಮಯ;
			ಆದರೆ(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_ಸಮಯ == 0){

				chunk.F.session_expiry_ಸಮಯ = context->session_expiry_interval + db.now_real_s;
			}ಇಲ್ಲದಿದ್ದರೆ{
				chunk.F.session_expiry_ಸಮಯ = context->session_expiry_ಸಮಯ;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.ಕೊನೆಯ_mid = context->ಕೊನೆಯ_mid;
			chunk.F.id_len = (uint16_t)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->id);
			chunk.clientid = context->id;
			ಆದರೆ(context->username){
				chunk.F.username_len = (uint16_t)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->username);
				chunk.username = context->username;
			}
			ಆದರೆ(context->listener){
				chunk.F.listener_ಪೋರ್ಟ್ = context->listener->ಪೋರ್ಟ್;
			}

			ಆದರೆ(chunk.F.id_len == 0){
				/* This should never happen, but in case we have a client with
				 * zero length ID, don't persist them. */
				ಮುಂದುವರಿ;
			}

			rc = persist__chunk_client_write_v6(db_fptr, &chunk);
			ಆದರೆ(rc){
				ಹಿಂತಿರುಗಿ rc;
			}

			ಆದರೆ(persist__client_messages_save(db_fptr, context, context->msgs_ಒಳ.inflight)){
				ಹಿಂತಿರುಗಿ 1;
			}
			ಆದರೆ(persist__client_messages_save(db_fptr, context, context->msgs_ಒಳ.queued)){
				ಹಿಂತಿರುಗಿ 1;
			}
			ಆದರೆ(persist__client_messages_save(db_fptr, context, context->msgs_ಹೊರಹೋಗು.inflight)){
				ಹಿಂತಿರುಗಿ 1;
			}
			ಆದರೆ(persist__client_messages_save(db_fptr, context, context->msgs_ಹೊರಹೋಗು.queued)){
				ಹಿಂತಿರುಗಿ 1;
			}
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ persist__subs_save(FILE *db_fptr, ರಚನೆ mosquitto__subhier *ಗಂಟು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, ಪೂರ್ಣಾಂಕ level)
{
	ರಚನೆ mosquitto__subhier *subhier, *subhier_tmp;
	ರಚನೆ mosquitto__subleaf *sub;
	ರಚನೆ P_sub sub_chunk;
	ಅಕ್ಷರ *thistopic;
	size_t slen;
	ಪೂರ್ಣಾಂಕ rc;

	slen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(topic) + ಗಂಟು->topic_len + 2;
	thistopic = mosquitto_malloc(ಗಾತ್ರ(ಅಕ್ಷರ)*slen);
	ಆದರೆ(!thistopic){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	ಆದರೆ(level > 1 || ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, ಗಂಟು->topic);
	}ಇಲ್ಲದಿದ್ದರೆ{
		snprintf(thistopic, slen, "%s", ಗಂಟು->topic);
	}

	sub = ಗಂಟು->subs;
	ಆಗ(sub){
		ಆದರೆ(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_ತಕ್ಷಣ
				&& sub->context->clean_ಆರಂಭ == false
				&& sub->context->id){

			memset(&sub_chunk, 0, ಗಾತ್ರ(ರಚನೆ P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(thistopic);
			sub_chunk.F.qos = MQTT_SUB_OPT_GET_QOS(sub->subscription_options);
			sub_chunk.F.options = sub->subscription_options & 0xFC;
			sub_chunk.clientid = sub->context->id;
			sub_chunk.topic = thistopic;

			rc = persist__chunk_sub_write_v6(db_fptr, &sub_chunk);
			ಆದರೆ(rc){
				mosquitto_FREE(thistopic);
				ಹಿಂತಿರುಗಿ rc;
			}
		}
		sub = sub->ಮುಂದಿನ;
	}

	HASH_ITER(hh, ಗಂಟು->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ persist__subs_save_all(FILE *db_fptr)
{
	ರಚನೆ mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.ಸಾಮಾನ್ಯ_subs, subhier, subhier_tmp){
		ಆದರೆ(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	HASH_ITER(hh, db.shared_subs, subhier, subhier_tmp){
		ಆದರೆ(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ persist__retain_save(FILE *db_fptr, ರಚನೆ mosquitto__retainhier *ಗಂಟು, ಪೂರ್ಣಾಂಕ level)
{
	ರಚನೆ mosquitto__retainhier *retainhier, *retainhier_tmp;
	ರಚನೆ P_retain retain_chunk;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(ಗಂಟು->retained && ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(ಗಂಟು->retained->ದತ್ತಾಂಶ.topic, "$SYS", 4)){
		memset(&retain_chunk, 0, ಗಾತ್ರ(ರಚನೆ P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = ಗಂಟು->retained->ದತ್ತಾಂಶ.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		ಆದರೆ(rc){
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	HASH_ITER(hh, ಗಂಟು->children, retainhier, retainhier_tmp){
		persist__retain_save(db_fptr, retainhier, level+1);
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ persist__retain_save_all(FILE *db_fptr)
{
	ರಚನೆ mosquitto__retainhier *retainhier, *retainhier_tmp;

	HASH_ITER(hh, db.retains, retainhier, retainhier_tmp){
		ಆದರೆ(retainhier->children){
			persist__retain_save(db_fptr, retainhier->children, 0);
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ persist_write_ದತ್ತಾಂಶ(FILE *db_fptr, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ);


ಸ್ಥಿರ ಶೂನ್ಯ persist__log_write_error(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ)
{
	log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error saving in-memory database, %s", ಸಂದೇಶ);
}


ಪೂರ್ಣಾಂಕ persist__backup(bool shutdown)
{
	ಆದರೆ(db.config == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	ಆದರೆ(db.config->persistence == false){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}
	ಆದರೆ(db.config->persistence_filepath == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Saving in-memory database to %s.", db.config->persistence_filepath);

	ಹಿಂತಿರುಗಿ mosquitto_write_file(db.config->persistence_filepath, true, &persist_write_ದತ್ತಾಂಶ, &shutdown, &persist__log_write_error);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ persist_write_ದತ್ತಾಂಶ(FILE *db_fptr, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
	bool shutdown = *(bool *)(user_ದತ್ತಾಂಶ);
	uint32_t db_ಆವೃತ್ತಿ_w = htonl(MOSQ_DB_ಆವೃತ್ತಿ);
	uint32_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = 0;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *err;
	ರಚನೆ PF_cfg cfg_chunk;
	ಪೂರ್ಣಾಂಕ rc = MOSQ_ERR_UNKNOWN;

	/*ಶೀರ್ಷಿಕೆ*/
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ, ಗಾತ್ರ(uint32_t));
	write_e(db_fptr, &db_ಆವೃತ್ತಿ_w, ಗಾತ್ರ(uint32_t));

	memset(&cfg_chunk, 0, ಗಾತ್ರ(ರಚನೆ PF_cfg));
	cfg_chunk.ಕೊನೆಯ_db_id = db.ಕೊನೆಯ_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_ಗಾತ್ರ = ಗಾತ್ರ(dbid_t);
	ಆದರೆ(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		ಹೋಗು error;
	}

	ಆದರೆ(persist_ಸಂದೇಶ_store_save(db_fptr)){
		ಹೋಗು error;
	}

	ಆದರೆ(persist__client_save(db_fptr)
			|| persist__subs_save_all(db_fptr)
			|| persist__retain_save_all(db_fptr)){
		ಹೋಗು error;
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;

error:
	err = strerror(errno);
	log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	ಆದರೆ(db_fptr){
		fclose(db_fptr);
	}
	ಹಿಂತಿರುಗಿ rc;
}


#ಆದರೆ_ಅಂತ್ಯ
