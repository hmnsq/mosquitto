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
#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <time.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "persist.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"


ಪೂರ್ಣಾಂಕ persist_chunk_ಶೀರ್ಷಿಕೆ_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *ಉದ್ದಳತೆ)
{
	size_t rlen;
	uint16_t i16temp;
	uint32_t i32temp;

	rlen = fread(&i16temp, ಗಾತ್ರ(uint16_t), 1, db_fptr);
	ಆದರೆ(rlen != 1){
		ಹಿಂತಿರುಗಿ 1;
	}

	rlen = fread(&i32temp, ಗಾತ್ರ(uint32_t), 1, db_fptr);
	ಆದರೆ(rlen != 1){
		ಹಿಂತಿರುಗಿ 1;
	}

	*chunk = ntohs(i16temp);
	*ಉದ್ದಳತೆ = ntohl(i32temp);

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ persist__chunk_cfg_read_v234(FILE *db_fptr, ರಚನೆ PF_cfg *chunk)
{
	ಪೂರ್ಣಾಂಕ rc = MOSQ_ERR_UNKNOWN;

	read_e(db_fptr, &chunk->shutdown, ಗಾತ್ರ(uint8_t)); /* shutdown */
	read_e(db_fptr, &chunk->dbid_ಗಾತ್ರ, ಗಾತ್ರ(uint8_t)); /* sizeof(dbid_t) */
	read_e(db_fptr, &chunk->ಕೊನೆಯ_db_id, ಗಾತ್ರ(dbid_t));

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
error:
	log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ಹಿಂತಿರುಗಿ rc;
}


ಪೂರ್ಣಾಂಕ persist__chunk_client_read_v234(FILE *db_fptr, ರಚನೆ P_client *chunk, uint32_t db_ಆವೃತ್ತಿ)
{
	uint16_t i16temp;
	ಪೂರ್ಣಾಂಕ rc;
	time_t ತಾತ್ಕಾಲಿಕ;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}

	read_e(db_fptr, &i16temp, ಗಾತ್ರ(uint16_t));
	chunk->F.ಕೊನೆಯ_mid = ntohs(i16temp);
	ಆದರೆ(db_ಆವೃತ್ತಿ != 2){
		read_e(db_fptr, &ತಾತ್ಕಾಲಿಕ, ಗಾತ್ರ(time_t));
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
error:
	log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	ಹಿಂತಿರುಗಿ 1;
}


ಪೂರ್ಣಾಂಕ persist_chunk_client_ಸಂದೇಶ_read_v234(FILE *db_fptr, ರಚನೆ P_client_ಸಂದೇಶ *chunk)
{
	uint16_t i16temp;
	ಪೂರ್ಣಾಂಕ rc;
	uint8_t retain, dup;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}

	read_e(db_fptr, &chunk->F.store_id, ಗಾತ್ರ(dbid_t));

	read_e(db_fptr, &i16temp, ಗಾತ್ರ(uint16_t));
	chunk->F.mid = ntohs(i16temp);

	read_e(db_fptr, &chunk->F.qos, ಗಾತ್ರ(uint8_t));
	read_e(db_fptr, &retain, ಗಾತ್ರ(uint8_t));
	read_e(db_fptr, &chunk->F.direction, ಗಾತ್ರ(uint8_t));
	read_e(db_fptr, &chunk->F.state, ಗಾತ್ರ(uint8_t));
	read_e(db_fptr, &dup, ಗಾತ್ರ(uint8_t));

	chunk->F.retain_dup = (uint8_t)((retain&0x0F)<<4 | (dup&0x0F));

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
error:
	log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	ಹಿಂತಿರುಗಿ 1;
}


ಪೂರ್ಣಾಂಕ persist_chunk_ತಳ_ಸಂದೇಶ_read_v234(FILE *db_fptr, ರಚನೆ P_ತಳ_ಸಂದೇಶ *chunk, uint32_t db_ಆವೃತ್ತಿ)
{
	uint32_t i32temp;
	uint16_t i16temp;
	ಪೂರ್ಣಾಂಕ rc = 0;
	size_t slen;

	read_e(db_fptr, &chunk->F.store_id, ಗಾತ್ರ(dbid_t));

	rc = persist__read_string(db_fptr, &chunk->ಮೂಲ.id);
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}

	ಆದರೆ(db_ಆವೃತ್ತಿ == 4){
		rc = persist__read_string(db_fptr, &chunk->ಮೂಲ.username);
		ಆದರೆ(rc){
			ಹೋಗು error;
		}
		read_e(db_fptr, &i16temp, ಗಾತ್ರ(uint16_t));
		chunk->F.ಮೂಲ_ಪೋರ್ಟ್ = ntohs(i16temp);
	}

	read_e(db_fptr, &i16temp, ಗಾತ್ರ(uint16_t));
	chunk->F.ಮೂಲ_mid = ntohs(i16temp);

	/* This is the mid - don't need it */
	read_e(db_fptr, &i16temp, ಗಾತ್ರ(uint16_t));

	rc = persist__read_string(db_fptr, &chunk->topic);
	ಆದರೆ(rc){
		ಹೋಗು error;
	}
	ಆದರೆ(!chunk->topic){
		rc = MOSQ_ERR_INVAL;
		ಹೋಗು error;
	}
	slen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(chunk->topic);
	ಆದರೆ(slen > UINT16_MAX){
		rc = MOSQ_ERR_INVAL;
		ಹೋಗು error;
	}
	chunk->F.topic_len = (uint16_t)slen;

	read_e(db_fptr, &chunk->F.qos, ಗಾತ್ರ(uint8_t));
	read_e(db_fptr, &chunk->F.retain, ಗಾತ್ರ(uint8_t));

	read_e(db_fptr, &i32temp, ಗಾತ್ರ(uint32_t));
	chunk->F.payloadlen = ntohl(i32temp);

	ಆದರೆ(chunk->F.payloadlen){
		ಆದರೆ(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
			rc = MOSQ_ERR_INVAL;
			ಹೋಗು error;
		}
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		ಆದರೆ(chunk->payload == ಖಾಲಿ){
			rc = MOSQ_ERR_NOMEM;
			ಹೋಗು error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->ಮೂಲ.id);
	mosquitto_FREE(chunk->ಮೂಲ.username);
	mosquitto_FREE(chunk->topic);
	ಹಿಂತಿರುಗಿ rc;
}


ಪೂರ್ಣಾಂಕ persist__chunk_retain_read_v234(FILE *db_fptr, ರಚನೆ P_retain *chunk)
{
	dbid_t i64temp;

	ಆದರೆ(fread(&i64temp, ಗಾತ್ರ(dbid_t), 1, db_fptr) != 1){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		ಹಿಂತಿರುಗಿ 1;
	}
	chunk->F.store_id = i64temp;

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ persist__chunk_sub_read_v234(FILE *db_fptr, ರಚನೆ P_sub *chunk)
{
	ಪೂರ್ಣಾಂಕ rc;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ಆದರೆ(rc){
		ಹೋಗು error;
	}

	rc = persist__read_string(db_fptr, &chunk->topic);
	ಆದರೆ(rc){
		ಹೋಗು error;
	}

	read_e(db_fptr, &chunk->F.qos, ಗಾತ್ರ(uint8_t));

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(chunk->topic);
	ಹಿಂತಿರುಗಿ rc;
}

#ಆದರೆ_ಅಂತ್ಯ
