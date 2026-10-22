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
#ಅನ್ತರ್ಭಾವ <utlist.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "persist.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

uint32_t db_ಆವೃತ್ತಿ;

ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ magic[15] = {0x00, 0xB5, 0x00, 'm', 'o', 's', 'q', 'u', 'i', 't', 't', 'o', ' ', 'd', 'b'};
ಸ್ಥಿರ ದೀರ್ಘ ತಳ_ಸಂದೇಶ_ಎಣಿಕೆ = 0;
ಸ್ಥಿರ ದೀರ್ಘ retained_ಎಣಿಕೆ = 0;
ಸ್ಥಿರ ದೀರ್ಘ client_ಎಣಿಕೆ = 0;
ಸ್ಥಿರ ದೀರ್ಘ subscription_ಎಣಿಕೆ = 0;
ಸ್ಥಿರ ದೀರ್ಘ client_ಸಂದೇಶ_ಎಣಿಕೆ = 0;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ persist__restore_sub(ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_subscription *sub);

ಸ್ಥಿರ ರಚನೆ mosquitto *persist_ಕಂಡುಹಿಡಿ_or_ಕೂಡಿಸು_context(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clientid, uint16_t ಕೊನೆಯ_mid)
{
	ರಚನೆ mosquitto *context;

	ಆದರೆ(!clientid){
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	context = ಖಾಲಿ;
	HASH_ಕಂಡುಹಿಡಿ(hh_id, db.contexts_by_id, clientid, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(clientid), context);
	ಆದರೆ(!context){
		context = context__init();
		ಆದರೆ(!context){
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		context->id = mosquitto_strdup(clientid);
		ಆದರೆ(!context->id){
			mosquitto_FREE(context);
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}

		context->clean_ಆರಂಭ = false;

		context_ಕೂಡಿಸು_ಗಮ್ಯ_by_id(context);
	}
	ಆದರೆ(ಕೊನೆಯ_mid){
		context->ಕೊನೆಯ_mid = ಕೊನೆಯ_mid;
	}
	ಹಿಂತಿರುಗಿ context;
}


ಪೂರ್ಣಾಂಕ persist__read_string_len(FILE *db_fptr, ಅಕ್ಷರ **str, uint16_t len)
{
	ಅಕ್ಷರ *s = ಖಾಲಿ;

	ಆದರೆ(len){
		s = mosquitto_malloc(len+1U);
		ಆದರೆ(!s){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
		ಆದರೆ(fread(s, 1, len, db_fptr) != len){
			mosquitto_FREE(s);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
		s[len] = '\0';
	}

	*str = s;
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ persist__read_string(FILE *db_fptr, ಅಕ್ಷರ **str)
{
	uint16_t i16temp;
	uint16_t slen;

	ಆದರೆ(fread(&i16temp, 1, ಗಾತ್ರ(uint16_t), db_fptr) != ಗಾತ್ರ(uint16_t)){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	slen = ntohs(i16temp);
	ಹಿಂತಿರುಗಿ persist__read_string_len(db_fptr, str, slen);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ persist_client_ಸಂದೇಶ_restore(ರಚನೆ P_client_ಸಂದೇಶ *chunk)
{
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *cmsg;
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ಸಂದೇಶ;
	ರಚನೆ mosquitto *context;
	ರಚನೆ mosquitto_ಸಂದೇಶ_ದತ್ತಾಂಶ *ಸಂದೇಶ_ದತ್ತಾಂಶ;

	HASH_ಕಂಡುಹಿಡಿ(hh, db.ಸಂದೇಶ_store, &chunk->F.store_id, ಗಾತ್ರ(chunk->F.store_id), ಸಂದೇಶ);
	ಆದರೆ(!ಸಂದೇಶ){
		/* Can't find message - probably expired */
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	context = persist_ಕಂಡುಹಿಡಿ_or_ಕೂಡಿಸು_context(chunk->clientid, 0);
	ಆದರೆ(!context){
		log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Persistence file contains client message with no matching client. File may be corrupt.");
		ಹಿಂತಿರುಗಿ 0;
	}

	cmsg = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto_client_ಸಂದೇಶ));
	ಆದರೆ(!cmsg){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	cmsg->ಮುಂದಿನ = ಖಾಲಿ;
	cmsg->ತಳ_ಸಂದೇಶ = ಖಾಲಿ;
	cmsg->ದತ್ತಾಂಶ.cmsg_id = ++context->ಕೊನೆಯ_cmsg_id;
	cmsg->ದತ್ತಾಂಶ.mid = chunk->F.mid;
	cmsg->ದತ್ತಾಂಶ.qos = chunk->F.qos;
	cmsg->ದತ್ತಾಂಶ.retain = (chunk->F.retain_dup&0xF0)>>4;
	cmsg->ದತ್ತಾಂಶ.direction = chunk->F.direction;
	cmsg->ದತ್ತಾಂಶ.state = chunk->F.state;
	cmsg->ದತ್ತಾಂಶ.dup = chunk->F.retain_dup&0x0F;
	cmsg->ದತ್ತಾಂಶ.subscription_identifier = chunk->subscription_identifier;

	cmsg->ತಳ_ಸಂದೇಶ = ಸಂದೇಶ;
	db_ಸಂದೇಶ_store_ref_inc(cmsg->ತಳ_ಸಂದೇಶ);

	ಆದರೆ(cmsg->ದತ್ತಾಂಶ.direction == mosq_md_ಹೊರಹೋಗು){
		ಸಂದೇಶ_ದತ್ತಾಂಶ = &context->msgs_ಹೊರಹೋಗು;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಸಂದೇಶ_ದತ್ತಾಂಶ = &context->msgs_ಒಳ;
	}

	ಆದರೆ(chunk->F.state == mosq_ms_queued || (chunk->F.qos > 0 && ಸಂದೇಶ_ದತ್ತಾಂಶ->inflight_quota == 0)){
		DL_APPEND(ಸಂದೇಶ_ದತ್ತಾಂಶ->queued, cmsg);
		db_ಸಂದೇಶ_ಕೂಡಿಸು_ಗಮ್ಯ_queued_stats(ಸಂದೇಶ_ದತ್ತಾಂಶ, cmsg);
	}ಇಲ್ಲದಿದ್ದರೆ{
		DL_APPEND(ಸಂದೇಶ_ದತ್ತಾಂಶ->inflight, cmsg);
		ಆದರೆ(chunk->F.qos > 0 && ಸಂದೇಶ_ದತ್ತಾಂಶ->inflight_quota > 0){
			ಸಂದೇಶ_ದತ್ತಾಂಶ->inflight_quota--;
		}
		db_ಸಂದೇಶ_ಕೂಡಿಸು_ಗಮ್ಯ_inflight_stats(ಸಂದೇಶ_ದತ್ತಾಂಶ, cmsg);
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ persist__client_chunk_restore(FILE *db_fptr)
{
	ಪೂರ್ಣಾಂಕ rc = 0;
	ರಚನೆ mosquitto *context;
	ರಚನೆ P_client chunk;

	memset(&chunk, 0, ಗಾತ್ರ(ರಚನೆ P_client));

	ಆದರೆ(db_ಆವೃತ್ತಿ == 6 || db_ಆವೃತ್ತಿ == 5){
		rc = persist__chunk_client_read_v56(db_fptr, &chunk, db_ಆವೃತ್ತಿ);
	}ಇಲ್ಲದಿದ್ದರೆ{
		rc = persist__chunk_client_read_v234(db_fptr, &chunk, db_ಆವೃತ್ತಿ);
	}
	ಆದರೆ(rc > 0){
		ಹಿಂತಿರುಗಿ rc;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc < 0){
		/* Client not loaded, but otherwise not an error */
		log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Empty client entry found in persistence database, it may be corrupt.");
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	context = persist_ಕಂಡುಹಿಡಿ_or_ಕೂಡಿಸು_context(chunk.clientid, chunk.F.ಕೊನೆಯ_mid);
	ಆದರೆ(context){
		context->session_expiry_ಸಮಯ = chunk.F.session_expiry_ಸಮಯ;
		context->session_expiry_interval = chunk.F.session_expiry_interval;
		ಆದರೆ(chunk.username && !context->username){
			/* username is not freed here, it is now owned by context */
			context->username = chunk.username;
			chunk.username = ಖಾಲಿ;
		}
		/* in per_listener_settings mode, try to find the listener by persisted port */
		ಆದರೆ(db.config->ಪ್ರತಿ_listener_settings && !context->listener && chunk.F.listener_ಪೋರ್ಟ್ > 0){
			ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i < db.config->listener_ಎಣಿಕೆ; i++){
				ಆದರೆ(db.config->listeners[i].ಪೋರ್ಟ್ == chunk.F.listener_ಪೋರ್ಟ್){
					context->listener = &db.config->listeners[i];
					ನಿಲ್ಲಿಸು;
				}
			}
		}
		session_expiry_ಕೂಡಿಸು_from_persistence(context, chunk.F.session_expiry_ಸಮಯ);
	}ಇಲ್ಲದಿದ್ದರೆ{
		rc = 1;
	}

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.username);
	ಆದರೆ(rc == 0){
		client_ಎಣಿಕೆ++;
	}
	ಹಿಂತಿರುಗಿ rc;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ persist_client_ಸಂದೇಶ_chunk_restore(FILE *db_fptr, uint32_t ಉದ್ದಳತೆ)
{
	ರಚನೆ P_client_ಸಂದೇಶ chunk;
	ಪೂರ್ಣಾಂಕ rc;

	memset(&chunk, 0, ಗಾತ್ರ(ರಚನೆ P_client_ಸಂದೇಶ));

	ಆದರೆ(db_ಆವೃತ್ತಿ == 6 || db_ಆವೃತ್ತಿ == 5){
		rc = persist_chunk_client_ಸಂದೇಶ_read_v56(db_fptr, &chunk, ಉದ್ದಳತೆ);
	}ಇಲ್ಲದಿದ್ದರೆ{
		rc = persist_chunk_client_ಸಂದೇಶ_read_v234(db_fptr, &chunk);
	}
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}

	rc = persist_client_ಸಂದೇಶ_restore(&chunk);
	mosquitto_FREE(chunk.clientid);

	ಆದರೆ(rc == 0){
		client_ಸಂದೇಶ_ಎಣಿಕೆ++;
	}
	ಹಿಂತಿರುಗಿ rc;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ persist_ತಳ_ಸಂದೇಶ_chunk_restore(FILE *db_fptr, uint32_t ಉದ್ದಳತೆ)
{
	ರಚನೆ P_ತಳ_ಸಂದೇಶ chunk;
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ = ಖಾಲಿ;
	int64_t ಸಂದೇಶ_expiry_interval64;
	uint32_t ಸಂದೇಶ_expiry_interval;
	uint32_t *p_ಸಂದೇಶ_expiry_interval = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ rc = 0;

	memset(&chunk, 0, ಗಾತ್ರ(ರಚನೆ P_ತಳ_ಸಂದೇಶ));

	ಆದರೆ(db_ಆವೃತ್ತಿ == 6 || db_ಆವೃತ್ತಿ == 5){
		rc = persist_chunk_ತಳ_ಸಂದೇಶ_read_v56(db_fptr, &chunk, ಉದ್ದಳತೆ);
	}ಇಲ್ಲದಿದ್ದರೆ{
		rc = persist_chunk_ತಳ_ಸಂದೇಶ_read_v234(db_fptr, &chunk, db_ಆವೃತ್ತಿ);
	}
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}
	ಆದರೆ(chunk.F.topic_len == 0){
		rc = MOSQ_ERR_INVAL;
		ಹೋಗು cleanup;
	}

	ಆದರೆ(chunk.F.ಮೂಲ_ಪೋರ್ಟ್){
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
			ಆದರೆ(db.config->listeners[i].ಪೋರ್ಟ್ == chunk.F.ಮೂಲ_ಪೋರ್ಟ್){
				chunk.ಮೂಲ.listener = &db.config->listeners[i];
				ನಿಲ್ಲಿಸು;
			}
		}
	}

	ಆದರೆ(chunk.F.expiry_ಸಮಯ > 0){
		ಸಂದೇಶ_expiry_interval64 = chunk.F.expiry_ಸಮಯ - ಸಮಯ(ಖಾಲಿ);
		ಆದರೆ(ಸಂದೇಶ_expiry_interval64 < 0 || ಸಂದೇಶ_expiry_interval64 > UINT32_MAX){
			/* Expired message */
			rc = MOSQ_ERR_SUCCESS;
			ಹೋಗು cleanup;
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಸಂದೇಶ_expiry_interval = (uint32_t)ಸಂದೇಶ_expiry_interval64;
		}
		p_ಸಂದೇಶ_expiry_interval = &ಸಂದೇಶ_expiry_interval;
	}

	ತಳ_ಸಂದೇಶ = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ));
	ಆದರೆ(ತಳ_ಸಂದೇಶ == ಖಾಲಿ){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
		rc = MOSQ_ERR_NOMEM;
		ಹೋಗು cleanup;
	}

	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.store_id = chunk.F.store_id;
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid = chunk.F.ಮೂಲ_mid;
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic = chunk.topic;
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos = chunk.F.qos;
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen = chunk.F.payloadlen;
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain = chunk.F.retain;
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.properties = chunk.properties;
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload = chunk.payload;
	ತಳ_ಸಂದೇಶ->ಮೂಲ_listener = chunk.ಮೂಲ.listener;

	rc = db_ಸಂದೇಶ_store(&chunk.ಮೂಲ, ತಳ_ಸಂದೇಶ, p_ಸಂದೇಶ_expiry_interval,
			mosq_mo_client);

	mosquitto_FREE(chunk.ಮೂಲ.id);
	mosquitto_FREE(chunk.ಮೂಲ.username);

	ಆದರೆ(rc == MOSQ_ERR_SUCCESS){
		ತಳ_ಸಂದೇಶ_ಎಣಿಕೆ++;
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ rc;
	}
cleanup:
	mosquitto_FREE(chunk.ಮೂಲ.id);
	mosquitto_FREE(chunk.ಮೂಲ.username);
	mosquitto_FREE(chunk.topic);
	mosquitto_FREE(chunk.payload);
	ಹಿಂತಿರುಗಿ rc;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ persist__retain_chunk_restore(FILE *db_fptr)
{
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ;
	ರಚನೆ P_retain chunk;
	ಪೂರ್ಣಾಂಕ rc;
	ಅಕ್ಷರ **ವಿಭಜಿಸು_topics;
	ಅಕ್ಷರ *local_topic;

	memset(&chunk, 0, ಗಾತ್ರ(ರಚನೆ P_retain));

	ಆದರೆ(db_ಆವೃತ್ತಿ == 6 || db_ಆವೃತ್ತಿ == 5){
		rc = persist__chunk_retain_read_v56(db_fptr, &chunk);
	}ಇಲ್ಲದಿದ್ದರೆ{
		rc = persist__chunk_retain_read_v234(db_fptr, &chunk);
	}
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}

	HASH_ಕಂಡುಹಿಡಿ(hh, db.ಸಂದೇಶ_store, &chunk.F.store_id, ಗಾತ್ರ(chunk.F.store_id), ತಳ_ಸಂದೇಶ);
	ಆದರೆ(ತಳ_ಸಂದೇಶ){
		rc = sub__topic_tokenise(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic, &local_topic, &ವಿಭಜಿಸು_topics, ಖಾಲಿ);
		ಆದರೆ(rc){
			ಹಿಂತಿರುಗಿ rc;
		}
		retain__store(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic, ತಳ_ಸಂದೇಶ, ವಿಭಜಿಸು_topics, true);
		mosquitto_FREE(local_topic);
		mosquitto_FREE(ವಿಭಜಿಸು_topics);
		retained_ಎಣಿಕೆ++;
	}ಇಲ್ಲದಿದ್ದರೆ{
		/* Can't find the message - probably expired */
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ persist__sub_chunk_restore(FILE *db_fptr)
{
	ರಚನೆ P_sub chunk;
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ mosquitto_subscription sub;

	memset(&chunk, 0, ಗಾತ್ರ(ರಚನೆ P_sub));

	ಆದರೆ(db_ಆವೃತ್ತಿ == 6 || db_ಆವೃತ್ತಿ == 5){
		rc = persist__chunk_sub_read_v56(db_fptr, &chunk);
	}ಇಲ್ಲದಿದ್ದರೆ{
		rc = persist__chunk_sub_read_v234(db_fptr, &chunk);
	}
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}

	sub.clientid = chunk.clientid;
	sub.topic_filter = chunk.topic;
	sub.options = chunk.F.qos | chunk.F.options;
	sub.identifier = chunk.F.identifier;
	rc = persist__restore_sub(&sub);

	mosquitto_FREE(chunk.clientid);
	mosquitto_FREE(chunk.topic);
	ಆದರೆ(rc == 0){
		subscription_ಎಣಿಕೆ++;
	}

	ಹಿಂತಿರುಗಿ rc;
}


ಪೂರ್ಣಾಂಕ persist_chunk_ಶೀರ್ಷಿಕೆ_read(FILE *db_fptr, uint32_t *chunk, uint32_t *ಉದ್ದಳತೆ)
{
	ಆದರೆ(db_ಆವೃತ್ತಿ == 6 || db_ಆವೃತ್ತಿ == 5){
		ಹಿಂತಿರುಗಿ persist_chunk_ಶೀರ್ಷಿಕೆ_read_v56(db_fptr, chunk, ಉದ್ದಳತೆ);
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ persist_chunk_ಶೀರ್ಷಿಕೆ_read_v234(db_fptr, chunk, ಉದ್ದಳತೆ);
	}
}


ಪೂರ್ಣಾಂಕ persist__restore(ಶೂನ್ಯ)
{
	FILE *fptr;
	ಅಕ್ಷರ ಶೀರ್ಷಿಕೆ[15];
	ಪೂರ್ಣಾಂಕ rc = 0;
	uint32_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ;
	uint32_t i32temp;
	uint32_t chunk, ಉದ್ದಳತೆ;
	size_t rlen;
	ಅಕ್ಷರ *err;
	ರಚನೆ PF_cfg cfg_chunk;

	ಪ್ರತಿಜ್ಞಾ(db.config);

	ಆದರೆ(!db.config->persistence || db.config->persistence_filepath == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	db.ಸಂದೇಶ_store = ಖಾಲಿ;
	ತಳ_ಸಂದೇಶ_ಎಣಿಕೆ = 0;
	retained_ಎಣಿಕೆ = 0;
	client_ಎಣಿಕೆ = 0;
	subscription_ಎಣಿಕೆ = 0;
	client_ಸಂದೇಶ_ಎಣಿಕೆ = 0;

	fptr = mosquitto_fopen(db.config->persistence_filepath, "rb", true);
	ಆದರೆ(fptr == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}
	rlen = fread(&ಶೀರ್ಷಿಕೆ, 1, 15, fptr);
	ಆದರೆ(rlen == 0){
		fclose(fptr);
		log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Persistence file is empty.");
		ಹಿಂತಿರುಗಿ 0;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rlen != 15){
		ಹೋಗು error;
	}
	ಆದರೆ(!memcmp(ಶೀರ್ಷಿಕೆ, magic, 15)){
		/* Restore DB as normal */
		read_e(fptr, &ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ, ಗಾತ್ರ(uint32_t));
		read_e(fptr, &i32temp, ಗಾತ್ರ(uint32_t));
		db_ಆವೃತ್ತಿ = ntohl(i32temp);
		/* IMPORTANT - this is where compatibility checks are made.
		 * Is your DB change still compatible with previous versions?
		 */
		ಆದರೆ(db_ಆವೃತ್ತಿ != MOSQ_DB_ಆವೃತ್ತಿ){
			ಆದರೆ(db_ಆವೃತ್ತಿ == 5){
				/* Addition of username and listener_port to client chunk in v6 */
			}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(db_ಆವೃತ್ತಿ == 4){
			}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(db_ಆವೃತ್ತಿ == 3){
				/* Addition of source_username and source_port to msg_store chunk in v4, v1.5.6 */
			}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(db_ಆವೃತ್ತಿ == 2){
				/* Addition of disconnect_t to client chunk in v3. */
			}ಇಲ್ಲದಿದ್ದರೆ{
				fclose(fptr);
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unsupported persistent database format version %d (need version %d).", db_ಆವೃತ್ತಿ, MOSQ_DB_ಆವೃತ್ತಿ);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
			}
		}

		ಆಗ(persist_chunk_ಶೀರ್ಷಿಕೆ_read(fptr, &chunk, &ಉದ್ದಳತೆ) == MOSQ_ERR_SUCCESS){
			ಆಯ್ಕೆ(chunk){
				ಸ್ಥಿತಿ DB_CHUNK_CFG:
					ಆದರೆ(db_ಆವೃತ್ತಿ == 6 || db_ಆವೃತ್ತಿ == 5){
						rc = persist__chunk_cfg_read_v56(fptr, &cfg_chunk);
						ಆದರೆ(rc){
							fclose(fptr);
							ಹಿಂತಿರುಗಿ rc;
						}
					}ಇಲ್ಲದಿದ್ದರೆ{
						rc = persist__chunk_cfg_read_v234(fptr, &cfg_chunk);
						ಆದರೆ(rc){
							fclose(fptr);
							ಹಿಂತಿರುಗಿ rc;
						}
					}
					ಆದರೆ(cfg_chunk.dbid_ಗಾತ್ರ != ಗಾತ್ರ(dbid_t)){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Incompatible database configuration (dbid size is %d bytes, expected %lu)",
								cfg_chunk.dbid_ಗಾತ್ರ, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ಗಾತ್ರ(dbid_t));
						fclose(fptr);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					db.ಕೊನೆಯ_db_id = cfg_chunk.ಕೊನೆಯ_db_id;
					ನಿಲ್ಲಿಸು;

				ಸ್ಥಿತಿ DB_CHUNK_ತಳ_ಸಂದೇಶ:
					rc = persist_ತಳ_ಸಂದೇಶ_chunk_restore(fptr, ಉದ್ದಳತೆ);
					ಆದರೆ(rc){
						fclose(fptr);
						ಹಿಂತಿರುಗಿ rc;
					}
					ನಿಲ್ಲಿಸು;

				ಸ್ಥಿತಿ DB_CHUNK_CLIENT_ಸಂದೇಶ:
					rc = persist_client_ಸಂದೇಶ_chunk_restore(fptr, ಉದ್ದಳತೆ);
					ಆದರೆ(rc){
						fclose(fptr);
						ಹಿಂತಿರುಗಿ rc;
					}
					ನಿಲ್ಲಿಸು;

				ಸ್ಥಿತಿ DB_CHUNK_RETAIN:
					rc = persist__retain_chunk_restore(fptr);
					ಆದರೆ(rc){
						fclose(fptr);
						ಹಿಂತಿರುಗಿ rc;
					}
					ನಿಲ್ಲಿಸು;

				ಸ್ಥಿತಿ DB_CHUNK_SUB:
					rc = persist__sub_chunk_restore(fptr);
					ಆದರೆ(rc){
						fclose(fptr);
						ಹಿಂತಿರುಗಿ rc;
					}
					ನಿಲ್ಲಿಸು;

				ಸ್ಥಿತಿ DB_CHUNK_CLIENT:
					rc = persist__client_chunk_restore(fptr);
					ಆದರೆ(rc){
						fclose(fptr);
						ಹಿಂತಿರುಗಿ rc;
					}
					ನಿಲ್ಲಿಸು;

				ಪೂರ್ವನಿಯೋಜಿತ:
					log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Unsupported chunk \"%d\" in persistent database file. Ignoring.", chunk);
					ಆದರೆ(fseek(fptr, ಉದ್ದಳತೆ, SEEK_CUR) < 0){
						fclose(fptr);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
					}
					ನಿಲ್ಲಿಸು;
			}
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to restore persistent database. Unrecognised file format.");
		rc = 1;
	}

	fclose(fptr);

	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Restored %ld base messages", ತಳ_ಸಂದೇಶ_ಎಣಿಕೆ);
	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Restored %ld retained messages", retained_ಎಣಿಕೆ);
	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Restored %ld clients", client_ಎಣಿಕೆ);
	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Restored %ld subscriptions", subscription_ಎಣಿಕೆ);
	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Restored %ld client messages", client_ಸಂದೇಶ_ಎಣಿಕೆ);

	ಹಿಂತಿರುಗಿ rc;
error:
	err = strerror(errno);
	log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: %s.", err);
	ಆದರೆ(fptr){
		fclose(fptr);
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_ERRNO;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ persist__restore_sub(ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_subscription *sub)
{
	ರಚನೆ mosquitto *context;

	ಆದರೆ(!sub->clientid){
		log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no client id, ignoring.");
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}
	ಆದರೆ(!sub->topic_filter){
		log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Persistence found a subscription with no topic filter, ignoring.");
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	context = persist_ಕಂಡುಹಿಡಿ_or_ಕೂಡಿಸು_context(sub->clientid, 0);
	ಆದರೆ(!context){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	ಹಿಂತಿರುಗಿ sub_ಕೂಡಿಸು(context, sub);
}

#ಆದರೆ_ಅಂತ್ಯ
