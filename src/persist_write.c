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

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif
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


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ persist__client_messages_save(FILE *db_fptr, ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *queue)
{
	ಸಂರಚನಾ P_client_ಸಂದೇಶ chunk;
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *cmsg;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಪ್ರತಿಜ್ಞಾ(db_fptr);
	ಪ್ರತಿಜ್ಞಾ(context);

	cmsg = queue;
	ಯಾವತ್(cmsg){
		ಯದಿ(!strncmp(cmsg->base_ಸಂದೇಶ->data.topic, "$SYS", 4)
				&& cmsg->base_ಸಂದೇಶ->ref_ಎಣಿಕೆ <= 1
				&& cmsg->base_ಸಂದೇಶ->dest_id_ಎಣಿಕೆ == 0){

			/* This $SYS message won't have been persisted, so we can't persist
			 * this client message. */
			cmsg = cmsg->next;
			ಅನುವರ್ತ;
		}

		memset(&chunk, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ P_client_ಸಂದೇಶ));

		chunk.F.store_id = cmsg->base_ಸಂದೇಶ->data.store_id;
		chunk.F.mid = cmsg->data.mid;
		chunk.F.id_len = (uint16_t)strlen(context->id);
		chunk.F.qos = cmsg->data.qos;
		chunk.F.retain_dup = (uint8_t)((cmsg->data.retain&0x0F)<<4 | (cmsg->data.dup&0x0F));
		chunk.F.direction = (uint8_t)cmsg->data.direction;
		chunk.F.state = (uint8_t)cmsg->data.state;
		chunk.clientid = context->id;
		chunk.subscription_identifier = cmsg->data.subscription_identifier;

		rc = persist_chunk_client_ಸಂದೇಶ_write_v6(db_fptr, &chunk);
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}

		cmsg = cmsg->next;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ persist__message_store_save(FILE *db_fptr)
{
	ಸಂರಚನಾ P_base_ಸಂದೇಶ chunk;
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ, *base_ಸಂದೇಶ_tmp;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಪ್ರತಿಜ್ಞಾ(db_fptr);

	base_ಸಂದೇಶ = db.ಸಂದೇಶ_store;
	HASH_ITER(hh, db.ಸಂದೇಶ_store, base_ಸಂದೇಶ, base_ಸಂದೇಶ_tmp){
		ಯದಿ(base_ಸಂದೇಶ->ref_ಎಣಿಕೆ < 1 || base_ಸಂದೇಶ->data.topic == ರಿಕ್ತ){
			ಅನುವರ್ತ;
		}

		memset(&chunk, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ P_base_ಸಂದೇಶ));

		ಯದಿ(!strncmp(base_ಸಂದೇಶ->data.topic, "$SYS", 4)){
			ಯದಿ(base_ಸಂದೇಶ->ref_ಎಣಿಕೆ <= 1 && base_ಸಂದೇಶ->dest_id_ಎಣಿಕೆ == 0){
				/* $SYS messages that are only retained shouldn't be persisted. */
				ಅನುವರ್ತ;
			}
			/* Don't save $SYS messages as retained otherwise they can give
			 * misleading information when reloaded. They should still be saved
			 * because a disconnected durable client may have them in their
			 * queue. */
			chunk.F.retain = 0;
		}ಅನ್ಯಥಾ{
			chunk.F.retain = (uint8_t)base_ಸಂದೇಶ->data.retain;
		}

		chunk.F.store_id = base_ಸಂದೇಶ->data.store_id;
		chunk.F.expiry_time = base_ಸಂದೇಶ->data.expiry_time;
		chunk.F.payloadlen = base_ಸಂದೇಶ->data.payloadlen;
		chunk.F.source_mid = base_ಸಂದೇಶ->data.source_mid;
		ಯದಿ(base_ಸಂದೇಶ->data.source_id){
			chunk.F.source_id_len = (uint16_t)strlen(base_ಸಂದೇಶ->data.source_id);
			chunk.source.id = base_ಸಂದೇಶ->data.source_id;
		}ಅನ್ಯಥಾ{
			chunk.F.source_id_len = 0;
			chunk.source.id = ರಿಕ್ತ;
		}
		ಯದಿ(base_ಸಂದೇಶ->data.source_username){
			chunk.F.source_username_len = (uint16_t)strlen(base_ಸಂದೇಶ->data.source_username);
			chunk.source.username = base_ಸಂದೇಶ->data.source_username;
		}ಅನ್ಯಥಾ{
			chunk.F.source_username_len = 0;
			chunk.source.username = ರಿಕ್ತ;
		}

		chunk.F.topic_len = (uint16_t)strlen(base_ಸಂದೇಶ->data.topic);
		chunk.topic = base_ಸಂದೇಶ->data.topic;

		ಯದಿ(base_ಸಂದೇಶ->source_listener){
			chunk.F.source_port = base_ಸಂದೇಶ->source_listener->port;
		}ಅನ್ಯಥಾ{
			chunk.F.source_port = 0;
		}
		chunk.F.qos = base_ಸಂದೇಶ->data.qos;
		chunk.payload = base_ಸಂದೇಶ->data.payload;
		chunk.properties = base_ಸಂದೇಶ->data.properties;

		rc = persist__chunk_message_store_write_v6(db_fptr, &chunk);
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ persist__client_save(FILE *db_fptr)
{
	ಸಂರಚನಾ mosquitto *context, *ctxt_tmp;
	ಸಂರಚನಾ P_client chunk;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಪ್ರತಿಜ್ಞಾ(db_fptr);

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		memset(&chunk, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ P_client));

		ಯದಿ(context &&
				context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE &&
#ifdef WITH_BRIDGE
				((!context->bridge && context->clean_start == false)
				|| (context->bridge && context->bridge->clean_start_local == false))
#ಅನ್ಯಥಾ
				context->clean_start == false
#endif
				){
			chunk.F.session_expiry_time = context->session_expiry_time;
			ಯದಿ(context->session_expiry_interval != MQTT_SESSION_EXPIRY_NEVER
					&& context->session_expiry_time == 0){

				chunk.F.session_expiry_time = context->session_expiry_interval + db.now_real_s;
			}ಅನ್ಯಥಾ{
				chunk.F.session_expiry_time = context->session_expiry_time;
			}
			chunk.F.session_expiry_interval = context->session_expiry_interval;
			chunk.F.last_mid = context->last_mid;
			chunk.F.id_len = (uint16_t)strlen(context->id);
			chunk.clientid = context->id;
			ಯದಿ(context->username){
				chunk.F.username_len = (uint16_t)strlen(context->username);
				chunk.username = context->username;
			}
			ಯದಿ(context->listener){
				chunk.F.listener_port = context->listener->port;
			}

			ಯದಿ(chunk.F.id_len == 0){
				/* This should never happen, but in case we have a client with
				 * zero length ID, don't persist them. */
				ಅನುವರ್ತ;
			}

			rc = persist__chunk_client_write_v6(db_fptr, &chunk);
			ಯದಿ(rc){
				ಪ್ರತ್ಯಾಗಮ rc;
			}

			ಯದಿ(persist__client_messages_save(db_fptr, context, context->msgs_in.inflight)){
				ಪ್ರತ್ಯಾಗಮ 1;
			}
			ಯದಿ(persist__client_messages_save(db_fptr, context, context->msgs_in.queued)){
				ಪ್ರತ್ಯಾಗಮ 1;
			}
			ಯದಿ(persist__client_messages_save(db_fptr, context, context->msgs_out.inflight)){
				ಪ್ರತ್ಯಾಗಮ 1;
			}
			ಯದಿ(persist__client_messages_save(db_fptr, context, context->msgs_out.queued)){
				ಪ್ರತ್ಯಾಗಮ 1;
			}
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ persist__subs_save(FILE *db_fptr, ಸಂರಚನಾ mosquitto__subhier *ಗಂಟು, ಧ್ರುವ ವರ್ಣ *topic, ಪೂರ್ಣಾಙ್ಕ level)
{
	ಸಂರಚನಾ mosquitto__subhier *subhier, *subhier_tmp;
	ಸಂರಚನಾ mosquitto__subleaf *sub;
	ಸಂರಚನಾ P_sub sub_chunk;
	ವರ್ಣ *thistopic;
	size_t slen;
	ಪೂರ್ಣಾಙ್ಕ rc;

	slen = strlen(topic) + ಗಂಟು->topic_len + 2;
	thistopic = mosquitto_malloc(ಪರಿಮಾಣ(ವರ್ಣ)*slen);
	ಯದಿ(!thistopic){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	ಯದಿ(level > 1 || strlen(topic)){
		snprintf(thistopic, slen, "%s/%s", topic, ಗಂಟು->topic);
	}ಅನ್ಯಥಾ{
		snprintf(thistopic, slen, "%s", ಗಂಟು->topic);
	}

	sub = ಗಂಟು->subs;
	ಯಾವತ್(sub){
		ಯದಿ(sub->context->session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE
				&& sub->context->clean_start == false
				&& sub->context->id){

			memset(&sub_chunk, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ P_sub));

			sub_chunk.F.identifier = sub->identifier;
			sub_chunk.F.id_len = (uint16_t)strlen(sub->context->id);
			sub_chunk.F.topic_len = (uint16_t)strlen(thistopic);
			sub_chunk.F.qos = MQTT_SUB_OPT_GET_QOS(sub->subscription_options);
			sub_chunk.F.options = sub->subscription_options & 0xFC;
			sub_chunk.clientid = sub->context->id;
			sub_chunk.topic = thistopic;

			rc = persist__chunk_sub_write_v6(db_fptr, &sub_chunk);
			ಯದಿ(rc){
				mosquitto_FREE(thistopic);
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}
		sub = sub->next;
	}

	HASH_ITER(hh, ಗಂಟು->children, subhier, subhier_tmp){
		persist__subs_save(db_fptr, subhier, thistopic, level+1);
	}
	mosquitto_FREE(thistopic);
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ persist__subs_save_all(FILE *db_fptr)
{
	ಸಂರಚನಾ mosquitto__subhier *subhier, *subhier_tmp;

	HASH_ITER(hh, db.normal_subs, subhier, subhier_tmp){
		ಯದಿ(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	HASH_ITER(hh, db.shared_subs, subhier, subhier_tmp){
		ಯದಿ(subhier->children){
			persist__subs_save(db_fptr, subhier->children, "", 0);
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ persist__retain_save(FILE *db_fptr, ಸಂರಚನಾ mosquitto__retainhier *ಗಂಟು, ಪೂರ್ಣಾಙ್ಕ level)
{
	ಸಂರಚನಾ mosquitto__retainhier *retainhier, *retainhier_tmp;
	ಸಂರಚನಾ P_retain retain_chunk;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(ಗಂಟು->retained && strncmp(ಗಂಟು->retained->data.topic, "$SYS", 4)){
		memset(&retain_chunk, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ P_retain));

		/* Don't save $SYS messages. */
		retain_chunk.F.store_id = ಗಂಟು->retained->data.store_id;
		rc = persist__chunk_retain_write_v6(db_fptr, &retain_chunk);
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	HASH_ITER(hh, ಗಂಟು->children, retainhier, retainhier_tmp){
		persist__retain_save(db_fptr, retainhier, level+1);
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ persist__retain_save_all(FILE *db_fptr)
{
	ಸಂರಚನಾ mosquitto__retainhier *retainhier, *retainhier_tmp;

	HASH_ITER(hh, db.retains, retainhier, retainhier_tmp){
		ಯದಿ(retainhier->children){
			persist__retain_save(db_fptr, retainhier->children, 0);
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ persist__write_data(FILE *db_fptr, ಶೂನ್ಯ *user_data);


ಸ್ಥಿರ ಶೂನ್ಯ persist__log_write_error(ಧ್ರುವ ವರ್ಣ *ಸಂದೇಶ)
{
	log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error saving in-memory database, %s", ಸಂದೇಶ);
}


ಪೂರ್ಣಾಙ್ಕ persist__backup(bool shutdown)
{
	ಯದಿ(db.config == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಯದಿ(db.config->persistence == false){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}
	ಯದಿ(db.config->persistence_filepath == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Saving in-memory database to %s.", db.config->persistence_filepath);

	ಪ್ರತ್ಯಾಗಮ mosquitto_write_file(db.config->persistence_filepath, true, &persist__write_data, &shutdown, &persist__log_write_error);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ persist__write_data(FILE *db_fptr, ಶೂನ್ಯ *user_data)
{
	bool shutdown = *(bool *)(user_data);
	uint32_t db_version_w = htonl(MOSQ_DB_VERSION);
	uint32_t crc = 0;
	ಧ್ರುವ ವರ್ಣ *err;
	ಸಂರಚನಾ PF_cfg cfg_chunk;
	ಪೂರ್ಣಾಙ್ಕ rc = MOSQ_ERR_UNKNOWN;

	/* Header */
	write_e(db_fptr, magic, 15);
	write_e(db_fptr, &crc, ಪರಿಮಾಣ(uint32_t));
	write_e(db_fptr, &db_version_w, ಪರಿಮಾಣ(uint32_t));

	memset(&cfg_chunk, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_cfg));
	cfg_chunk.last_db_id = db.last_db_id;
	cfg_chunk.shutdown = shutdown;
	cfg_chunk.dbid_size = ಪರಿಮಾಣ(dbid_t);
	ಯದಿ(persist__chunk_cfg_write_v6(db_fptr, &cfg_chunk)){
		ಗಚ್ಛ error;
	}

	ಯದಿ(persist__message_store_save(db_fptr)){
		ಗಚ್ಛ error;
	}

	ಯದಿ(persist__client_save(db_fptr)
			|| persist__subs_save_all(db_fptr)
			|| persist__retain_save_all(db_fptr)){
		ಗಚ್ಛ error;
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;

error:
	err = strerror(errno);
	log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	ಯದಿ(db_fptr){
		fclose(db_fptr);
	}
	ಪ್ರತ್ಯಾಗಮ rc;
}


#endif
