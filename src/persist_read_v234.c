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
#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <time.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "persist.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"


ಪೂರ್ಣಾಙ್ಕ persist__chunk_header_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	size_t rlen;
	uint16_t i16temp;
	uint32_t i32temp;

	rlen = fread(&i16temp, ಪರಿಮಾಣ(uint16_t), 1, db_fptr);
	ಯದಿ(rlen != 1){
		ಪ್ರತ್ಯಾಗಮ 1;
	}

	rlen = fread(&i32temp, ಪರಿಮಾಣ(uint32_t), 1, db_fptr);
	ಯದಿ(rlen != 1){
		ಪ್ರತ್ಯಾಗಮ 1;
	}

	*chunk = ntohs(i16temp);
	*length = ntohl(i32temp);

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ persist__chunk_cfg_read_v234(FILE *db_fptr, ಸಂರಚನಾ PF_cfg *chunk)
{
	ಪೂರ್ಣಾಙ್ಕ rc = MOSQ_ERR_UNKNOWN;

	read_e(db_fptr, &chunk->shutdown, ಪರಿಮಾಣ(uint8_t)); /* shutdown */
	read_e(db_fptr, &chunk->dbid_size, ಪರಿಮಾಣ(uint8_t)); /* sizeof(dbid_t) */
	read_e(db_fptr, &chunk->last_db_id, ಪರಿಮಾಣ(dbid_t));

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
error:
	log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ಪ್ರತ್ಯಾಗಮ rc;
}


ಪೂರ್ಣಾಙ್ಕ persist__chunk_client_read_v234(FILE *db_fptr, ಸಂರಚನಾ P_client *chunk, uint32_t db_version)
{
	uint16_t i16temp;
	ಪೂರ್ಣಾಙ್ಕ rc;
	time_t temp;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	read_e(db_fptr, &i16temp, ಪರಿಮಾಣ(uint16_t));
	chunk->F.last_mid = ntohs(i16temp);
	ಯದಿ(db_version != 2){
		read_e(db_fptr, &temp, ಪರಿಮಾಣ(time_t));
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
error:
	log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	ಪ್ರತ್ಯಾಗಮ 1;
}


ಪೂರ್ಣಾಙ್ಕ persist_chunk_client_ಸಂದೇಶ_read_v234(FILE *db_fptr, ಸಂರಚನಾ P_client_ಸಂದೇಶ *chunk)
{
	uint16_t i16temp;
	ಪೂರ್ಣಾಙ್ಕ rc;
	uint8_t retain, dup;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	read_e(db_fptr, &chunk->F.store_id, ಪರಿಮಾಣ(dbid_t));

	read_e(db_fptr, &i16temp, ಪರಿಮಾಣ(uint16_t));
	chunk->F.mid = ntohs(i16temp);

	read_e(db_fptr, &chunk->F.qos, ಪರಿಮಾಣ(uint8_t));
	read_e(db_fptr, &retain, ಪರಿಮಾಣ(uint8_t));
	read_e(db_fptr, &chunk->F.direction, ಪರಿಮಾಣ(uint8_t));
	read_e(db_fptr, &chunk->F.state, ಪರಿಮಾಣ(uint8_t));
	read_e(db_fptr, &dup, ಪರಿಮಾಣ(uint8_t));

	chunk->F.retain_dup = (uint8_t)((retain&0x0F)<<4 | (dup&0x0F));

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
error:
	log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	ಪ್ರತ್ಯಾಗಮ 1;
}


ಪೂರ್ಣಾಙ್ಕ persist_chunk_base_ಸಂದೇಶ_read_v234(FILE *db_fptr, ಸಂರಚನಾ P_base_ಸಂದೇಶ *chunk, uint32_t db_version)
{
	uint32_t i32temp;
	uint16_t i16temp;
	ಪೂರ್ಣಾಙ್ಕ rc = 0;
	size_t slen;

	read_e(db_fptr, &chunk->F.store_id, ಪರಿಮಾಣ(dbid_t));

	rc = persist__read_string(db_fptr, &chunk->source.id);
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಯದಿ(db_version == 4){
		rc = persist__read_string(db_fptr, &chunk->source.username);
		ಯದಿ(rc){
			ಗಚ್ಛ error;
		}
		read_e(db_fptr, &i16temp, ಪರಿಮಾಣ(uint16_t));
		chunk->F.source_port = ntohs(i16temp);
	}

	read_e(db_fptr, &i16temp, ಪರಿಮಾಣ(uint16_t));
	chunk->F.source_mid = ntohs(i16temp);

	/* This is the mid - don't need it */
	read_e(db_fptr, &i16temp, ಪರಿಮಾಣ(uint16_t));

	rc = persist__read_string(db_fptr, &chunk->topic);
	ಯದಿ(rc){
		ಗಚ್ಛ error;
	}
	ಯದಿ(!chunk->topic){
		rc = MOSQ_ERR_INVAL;
		ಗಚ್ಛ error;
	}
	slen = strlen(chunk->topic);
	ಯದಿ(slen > UINT16_MAX){
		rc = MOSQ_ERR_INVAL;
		ಗಚ್ಛ error;
	}
	chunk->F.topic_len = (uint16_t)slen;

	read_e(db_fptr, &chunk->F.qos, ಪರಿಮಾಣ(uint8_t));
	read_e(db_fptr, &chunk->F.retain, ಪರಿಮಾಣ(uint8_t));

	read_e(db_fptr, &i32temp, ಪರಿಮಾಣ(uint32_t));
	chunk->F.payloadlen = ntohl(i32temp);

	ಯದಿ(chunk->F.payloadlen){
		ಯದಿ(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
			rc = MOSQ_ERR_INVAL;
			ಗಚ್ಛ error;
		}
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		ಯದಿ(chunk->payload == ರಿಕ್ತ){
			rc = MOSQ_ERR_NOMEM;
			ಗಚ್ಛ error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	ಪ್ರತ್ಯಾಗಮ rc;
}


ಪೂರ್ಣಾಙ್ಕ persist__chunk_retain_read_v234(FILE *db_fptr, ಸಂರಚನಾ P_retain *chunk)
{
	dbid_t i64temp;

	ಯದಿ(fread(&i64temp, ಪರಿಮಾಣ(dbid_t), 1, db_fptr) != 1){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		ಪ್ರತ್ಯಾಗಮ 1;
	}
	chunk->F.store_id = i64temp;

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ persist__chunk_sub_read_v234(FILE *db_fptr, ಸಂರಚನಾ P_sub *chunk)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ಯದಿ(rc){
		ಗಚ್ಛ error;
	}

	rc = persist__read_string(db_fptr, &chunk->topic);
	ಯದಿ(rc){
		ಗಚ್ಛ error;
	}

	read_e(db_fptr, &chunk->F.qos, ಪರಿಮಾಣ(uint8_t));

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(chunk->topic);
	ಪ್ರತ್ಯಾಗಮ rc;
}

#endif
