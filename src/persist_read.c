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
#ಅನ್ತರ್ಭಾವ <utlist.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "persist.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

uint32_t db_version;

ಧ್ರುವ unsigned ವರ್ಣ magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
ಸ್ಥಿರ long base_ಸಂದೇಶ_ಎಣಿಕೆ = 0;
ಸ್ಥಿರ long retained_ಎಣಿಕೆ = 0;
ಸ್ಥಿರ long client_ಎಣಿಕೆ = 0;
ಸ್ಥಿರ long subscription_ಎಣಿಕೆ = 0;
ಸ್ಥಿರ long client_ಸಂದೇಶ_ಎಣಿಕೆ = 0;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ persist__restore_sub(ಧ್ರುವ ಸಂರಚನಾ mosquitto_subscription *sub);

ಸ್ಥಿರ ಸಂರಚನಾ mosquitto *persist__find_or_add_context(ಧ್ರುವ ವರ್ಣ *clientid, uint16_t last_mid)
{
	ಸಂರಚನಾ mosquitto *context;

	ಯದಿ(!clientid){
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	context = ರಿಕ್ತ;
	HASH_FIND(hh_id, db.contexts_by_id, clientid, strlen(clientid), context);
	ಯದಿ(!context){
		context = context__init();
		ಯದಿ(!context){
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		context->id = mosquitto_strdup(clientid);
		ಯದಿ(!context->id){
			mosquitto_FREE(context);
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}

		context->clean_start = false;

		context__add_to_by_id(context);
	}
	ಯದಿ(last_mid){
		context->last_mid = last_mid;
	}
	ಪ್ರತ್ಯಾಗಮ context;
}


ಪೂರ್ಣಾಙ್ಕ persist__read_string_len(FILE *db_fptr, ವರ್ಣ **str, uint16_t len)
{
	ವರ್ಣ *s = ರಿಕ್ತ;

	ಯದಿ(len){
		s = mosquitto_malloc(len+1U);
		ಯದಿ(!s){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
		ಯದಿ(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ persist__read_string(FILE *db_fptr, ವರ್ಣ **str)
{
	uint16_t i16temp;
	uint16_t slen;

	ಯದಿ(fread(&i16temp, 1, ಪರಿಮಾಣ(uint16_t), db_fptr) != ಪರಿಮಾಣ(uint16_t)){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	ಪ್ರತ್ಯಾಗಮ persist__read_string_len(db_fptr, str, slen);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ persist_client_ಸಂದೇಶ_restore(ಸಂರಚನಾ P_client_ಸಂದೇಶ *chunk)
{
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *cmsg;
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *ಸಂದೇಶ;
	ಸಂರಚನಾ mosquitto *context;
	ಸಂರಚನಾ mosquitto_ಸಂದೇಶ_data *ಸಂದೇಶ_data;

	HASH_FIND(hh, db.ಸಂದೇಶ_store, &chunk->F.store_id, ಪರಿಮಾಣ(chunk->F.store_id), ಸಂದೇಶ);
	ಯದಿ(!ಸಂದೇಶ){
		/* Can't find message - probably expired */
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk->clientid, 0);
	ಯದಿ(!context){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	cmsg = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ));
	ಯದಿ(!cmsg){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	cmsg->next = ರಿಕ್ತ;
	cmsg->base_ಸಂದೇಶ = ರಿಕ್ತ;
	cmsg->data.cmsg_id = ++context->last_cmsg_id;
	cmsg->data.mid = chunk->F.mid;
	cmsg->data.qos = chunk->F.qos;
	cmsg->data.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->data.direction = chunk->F.direction;
	cmsg->data.state = chunk->F.state;
	cmsg->data.dup = chunk->F.retain_dup&0x0F;
	cmsg->data.subscription_identifier = chunk->subscription_identifier;

	cmsg->base_ಸಂದೇಶ = ಸಂದೇಶ;
	db_ಸಂದೇಶ_store_ref_inc(cmsg->base_ಸಂದೇಶ);

	ಯದಿ(cmsg->data.direction == mosq_md_out){
		ಸಂದೇಶ_data = &context->msgs_out;
	}ಅನ್ಯಥಾ{
		ಸಂದೇಶ_data = &context->msgs_in;
	}

	ಯದಿ(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && ಸಂದೇಶ_data->inflight_quota == 0)){
		DL_APPEND(ಸಂದೇಶ_data->queued, cmsg);
		db_ಸಂದೇಶ_add_to_queued_stats(ಸಂದೇಶ_data, cmsg);
	}ಅನ್ಯಥಾ{
		DL_APPEND(ಸಂದೇಶ_data->inflight, cmsg);
		ಯದಿ(chunk->F.qos > 0 && ಸಂದೇಶ_data->inflight_quota > 0){
			ಸಂದೇಶ_data->inflight_quota--;
		}
		db_ಸಂದೇಶ_add_to_inflight_stats(ಸಂದೇಶ_data, cmsg);
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ persist__client_chunk_restore(FILE *db_fptr)
{
	ಪೂರ್ಣಾಙ್ಕ rc = 0;
	ಸಂರಚನಾ mosquitto *context;
	ಸಂರಚನಾ P_client chunk;

	memset(&chunk, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ P_client));

	ಯದಿ(db_version == 6 || db_version == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_version);
	}ಅನ್ಯಥಾ{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_version);
	}
	ಯದಿ(rc > 0){
		ಪ್ರತ್ಯಾಗಮ rc;
	}ಅನ್ಯಥಾ ಯದಿ(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(chunk.clientid, chunk.F.last_mid);
	ಯದಿ(context){
		context->session_expiry_time = chunk.F.session_expiry_time;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		ಯದಿ(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = ರಿಕ್ತ;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		ಯದಿ(db.config->per_listener_settings && !context->listener && chunk.F.listener_port > 0){
			ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i < db.config->listener_ಎಣಿಕೆ; i++){
				ಯದಿ(db.config->listeners[i].port == chunk.F.listener_port){
					context->listener = &db.config->listeners[i];
					ಭಙ್ಗ;
				}
			}
		}
		session_expiry__add_from_persistence(context, chunk.F.session_expiry_time);
	}ಅನ್ಯಥಾ{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	ಯದಿ(rc == 0){
		client_ಎಣಿಕೆ++;
	}
	ಪ್ರತ್ಯಾಗಮ rc;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ persist_client_ಸಂದೇಶ_chunk_restore(FILE *db_fptr, uint32_t length)
{
	ಸಂರಚನಾ P_client_ಸಂದೇಶ chunk;
	ಪೂರ್ಣಾಙ್ಕ rc;

	memset(&chunk, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ P_client_ಸಂದೇಶ));

	ಯದಿ(db_version == 6 || db_version == 5){
		rc = persist_chunk_client_ಸಂದೇಶ_read_v56(db_fptr, &chunk, length);
	}ಅನ್ಯಥಾ{
		rc = persist_chunk_client_ಸಂದೇಶ_read_v234(db_fptr, &chunk);
	}
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	rc = persist_client_ಸಂದೇಶ_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	ಯದಿ(rc == 0){
		client_ಸಂದೇಶ_ಎಣಿಕೆ++;
	}
	ಪ್ರತ್ಯಾಗಮ rc;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ persist_base_ಸಂದೇಶ_chunk_restore(FILE *db_fptr, uint32_t length)
{
	ಸಂರಚನಾ P_base_ಸಂದೇಶ chunk;
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ = ರಿಕ್ತ;
	int64_t message_expiry_interval64;
	uint32_t message_expiry_interval;
	uint32_t *p_message_expiry_interval = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ rc = 0;

	memset(&chunk, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ P_base_ಸಂದೇಶ));

	ಯದಿ(db_version == 6 || db_version == 5){
		rc = persist_chunk_base_ಸಂದೇಶ_read_v56(db_fptr, &chunk, length);
	}ಅನ್ಯಥಾ{
		rc = persist_chunk_base_ಸಂದೇಶ_read_v234(db_fptr, &chunk, db_version);
	}
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}
	ಯದಿ(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		ಗಚ್ಛ cleanup;
	}

	ಯದಿ(chunk.F.source_port){
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
			ಯದಿ(db.config->listeners[i].port == chunk.F.source_port){
				chunk.source.listener = &db.config->listeners[i];
				ಭಙ್ಗ;
			}
		}
	}

	ಯದಿ(chunk.F.expiry_time > 0){
		message_expiry_interval64 = chunk.F.expiry_time - time(ರಿಕ್ತ);
		ಯದಿ(message_expiry_interval64 < 0 || message_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			ಗಚ್ಛ cleanup;
		}ಅನ್ಯಥಾ{
			message_expiry_interval = (uint32_t)message_expiry_interval64;
		}
		p_message_expiry_interval = &message_expiry_interval;
	}

	base_ಸಂದೇಶ = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ));
	ಯದಿ(base_ಸಂದೇಶ == ರಿಕ್ತ){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		ಗಚ್ಛ cleanup;
	}

	base_ಸಂದೇಶ->data.store_id = chunk.F.store_id;
	base_ಸಂದೇಶ->data.source_mid = chunk.F.source_mid;
	base_ಸಂದೇಶ->data.topic = chunk.topic;
	base_ಸಂದೇಶ->data.qos = chunk.F.qos;
	base_ಸಂದೇಶ->data.payloadlen = chunk.F.payloadlen;
	base_ಸಂದೇಶ->data.retain = chunk.F.retain;
	base_ಸಂದೇಶ->data.properties = chunk.properties;
	base_ಸಂದೇಶ->data.payload = chunk.payload;
	base_ಸಂದೇಶ->source_listener = chunk.source.listener;

	rc = db__message_store(&chunk.source, base_ಸಂದೇಶ, p_message_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);

	ಯದಿ(rc == MOSQ_ERR_SUCCESS){
		base_ಸಂದೇಶ_ಎಣಿಕೆ++;
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ rc;
	}
cleanup:
	mosquitto_FREE(chunk.source.id);
	mosquitto_FREE(chunk.source.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	ಪ್ರತ್ಯಾಗಮ rc;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ persist__retain_chunk_restore(FILE *db_fptr)
{
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ;
	ಸಂರಚನಾ P_retain chunk;
	ಪೂರ್ಣಾಙ್ಕ rc;
	ವರ್ಣ **split_topics;
	ವರ್ಣ *local_topic;

	memset(&chunk, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ P_retain));

	ಯದಿ(db_version == 6 || db_version == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}ಅನ್ಯಥಾ{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	HASH_FIND(hh, db.ಸಂದೇಶ_store, &chunk.F.store_id, ಪರಿಮಾಣ(chunk.F.store_id), base_ಸಂದೇಶ);
	ಯದಿ(base_ಸಂದೇಶ){
		rc = sub__topic_tokenise(base_ಸಂದೇಶ->data.topic, &local_topic, &split_topics, ರಿಕ್ತ);
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
		retain__store(base_ಸಂದೇಶ->data.topic, base_ಸಂದೇಶ, split_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(split_topics);
		retained_ಎಣಿಕೆ++;
	}ಅನ್ಯಥಾ{
		/* Can't find the message - probably expired */
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ persist__sub_chunk_restore(FILE *db_fptr)
{
	ಸಂರಚನಾ P_sub chunk;
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ mosquitto_subscription sub;

	memset(&chunk, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ P_sub));

	ಯದಿ(db_version == 6 || db_version == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}ಅನ್ಯಥಾ{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	ಯದಿ(rc == 0){
		subscription_ಎಣಿಕೆ++;
	}

	ಪ್ರತ್ಯಾಗಮ rc;
}


ಪೂರ್ಣಾಙ್ಕ persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	ಯದಿ(db_version == 6 || db_version == 5){
		ಪ್ರತ್ಯಾಗಮ persist__chunk_header_read_v56(db_fptr, chunk, length);
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ persist__chunk_header_read_v234(db_fptr, chunk, length);
	}
}


ಪೂರ್ಣಾಙ್ಕ persist__restore(ಶೂನ್ಯ)
{
	FILE *fptr;
	ವರ್ಣ header[15];
	ಪೂರ್ಣಾಙ್ಕ rc = 0;
	uint32_t crc;
	uint32_t i32temp;
	uint32_t chunk, length;
	size_t rlen;
	ವರ್ಣ *err;
	ಸಂರಚನಾ PF_cfg cfg_chunk;

	ಪ್ರತಿಜ್ಞಾ(db.config);

	ಯದಿ(!db.config->persistence || db.config->persistence_filepath == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	db.ಸಂದೇಶ_store = ರಿಕ್ತ;
	base_ಸಂದೇಶ_ಎಣಿಕೆ = 0;
	retained_ಎಣಿಕೆ = 0;
	client_ಎಣಿಕೆ = 0;
	subscription_ಎಣಿಕೆ = 0;
	client_ಸಂದೇಶ_ಎಣಿಕೆ = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	ಯದಿ(fptr == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&header, 1, 15, fptr);
	ಯದಿ(rlen == 0){
		fclose(fptr);
		log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		ಪ್ರತ್ಯಾಗಮ 0;
	}ಅನ್ಯಥಾ ಯದಿ(rlen != 15){
		ಗಚ್ಛ error;
	}
	ಯದಿ(!memcmp(header, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &crc, ಪರಿಮಾಣ(uint32_t));
		read_e(fptr, &i32temp, ಪರಿಮಾಣ(uint32_t));
		db_version = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		ಯದಿ(db_version != MOSQ_DB_VERSION){
			ಯದಿ(db_version == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}ಅನ್ಯಥಾ ಯದಿ(db_version == 4){
			}ಅನ್ಯಥಾ ಯದಿ(db_version == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}ಅನ್ಯಥಾ ಯದಿ(db_version == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}ಅನ್ಯಥಾ{
				fclose(fptr);
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_version, MOSQ_DB_VERSION);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
			}
		}

		ಯಾವತ್(persist__chunk_header_read(fptr, &chunk, &length) == MOSQ_ERR_SUCCESS){
			ವಿಕಲ್ಪ(chunk){
				ಪಕ್ಷ DB_CHUNK_CFG:
					ಯದಿ(db_version == 6 || db_version == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						ಯದಿ(rc){
							fclose(fptr);
							ಪ್ರತ್ಯಾಗಮ rc;
						}
					}ಅನ್ಯಥಾ{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						ಯದಿ(rc){
							fclose(fptr);
							ಪ್ರತ್ಯಾಗಮ rc;
						}
					}
					ಯದಿ(cfg_chunk.dbid_size != ಪರಿಮಾಣ(dbid_t)){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_size, (unsigned long)ಪರಿಮಾಣ(dbid_t));
						fclose(fptr);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					db.last_db_id = cfg_chunk.last_db_id;
					ಭಙ್ಗ;

				ಪಕ್ಷ DB_CHUNK_BASE_ಸಂದೇಶ:
					rc = persist_base_ಸಂದೇಶ_chunk_restore(fptr, length);
					ಯದಿ(rc){
						fclose(fptr);
						ಪ್ರತ್ಯಾಗಮ rc;
					}
					ಭಙ್ಗ;

				ಪಕ್ಷ DB_CHUNK_CLIENT_ಸಂದೇಶ:
					rc = persist_client_ಸಂದೇಶ_chunk_restore(fptr, length);
					ಯದಿ(rc){
						fclose(fptr);
						ಪ್ರತ್ಯಾಗಮ rc;
					}
					ಭಙ್ಗ;

				ಪಕ್ಷ DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					ಯದಿ(rc){
						fclose(fptr);
						ಪ್ರತ್ಯಾಗಮ rc;
					}
					ಭಙ್ಗ;

				ಪಕ್ಷ DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					ಯದಿ(rc){
						fclose(fptr);
						ಪ್ರತ್ಯಾಗಮ rc;
					}
					ಭಙ್ಗ;

				ಪಕ್ಷ DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					ಯದಿ(rc){
						fclose(fptr);
						ಪ್ರತ್ಯಾಗಮ rc;
					}
					ಭಙ್ಗ;

				ಸ್ವಭಾವ:
					log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					ಯದಿ(fseek(fptr, length, SEEK_CUR) < 0){
						fclose(fptr);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
					}
					ಭಙ್ಗ;
			}
		}
	}ಅನ್ಯಥಾ{
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Restored %ld base messages", base_ಸಂದೇಶ_ಎಣಿಕೆ);
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_ಎಣಿಕೆ);
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Restored %ld clients", client_ಎಣಿಕೆ);
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_ಎಣಿಕೆ);
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Restored %ld client messages", client_ಸಂದೇಶ_ಎಣಿಕೆ);

	ಪ್ರತ್ಯಾಗಮ rc;
error:
	err = strerror(errno);
	log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: %s.", err);
	ಯದಿ(fptr){
		fclose(fptr);
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ERRNO;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ persist__restore_sub(ಧ್ರುವ ಸಂರಚನಾ mosquitto_subscription *sub)
{
	ಸಂರಚನಾ mosquitto *context;

	ಯದಿ(!sub->clientid){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}
	ಯದಿ(!sub->topic_filter){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	context = persist__find_or_add_context(sub->clientid, 0);
	ಯದಿ(!context){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಪ್ರತ್ಯಾಗಮ sub__add(context, sub);
}

#endif
