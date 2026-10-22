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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "persist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"


ਪੂਰਨ persist_chunk_ਸਿਰਲੇਖ_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *ਲੰਮਾਈ)
{
	size_t rlen;
	uint16_t i16temp;
	uint32_t i32temp;

	rlen = fread(&i16temp, ਆਕਾਰ(uint16_t), 1, db_fptr);
	ਜੇ(rlen != 1){
		ਵਾਪਸ 1;
	}

	rlen = fread(&i32temp, ਆਕਾਰ(uint32_t), 1, db_fptr);
	ਜੇ(rlen != 1){
		ਵਾਪਸ 1;
	}

	*chunk = ntohs(i16temp);
	*ਲੰਮਾਈ = ntohl(i32temp);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ persist__chunk_cfg_read_v234(FILE *db_fptr, ਬਣਤਰ PF_cfg *chunk)
{
	ਪੂਰਨ rc = MOSQ_ERR_UNKNOWN;

	read_e(db_fptr, &chunk->shutdown, ਆਕਾਰ(uint8_t)); /* shutdown */
	read_e(db_fptr, &chunk->dbid_ਮਾਪ, ਆਕਾਰ(uint8_t)); /* sizeof(dbid_t) */
	read_e(db_fptr, &chunk->ਆਖਰੀ_db_id, ਆਕਾਰ(dbid_t));

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
error:
	log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ਵਾਪਸ rc;
}


ਪੂਰਨ persist__chunk_client_read_v234(FILE *db_fptr, ਬਣਤਰ P_client *chunk, uint32_t db_ਸੰਸਕਰਣ)
{
	uint16_t i16temp;
	ਪੂਰਨ rc;
	time_t ਆਰਜ਼ੀ;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	read_e(db_fptr, &i16temp, ਆਕਾਰ(uint16_t));
	chunk->F.ਆਖਰੀ_mid = ntohs(i16temp);
	ਜੇ(db_ਸੰਸਕਰਣ != 2){
		read_e(db_fptr, &ਆਰਜ਼ੀ, ਆਕਾਰ(time_t));
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
error:
	log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	ਵਾਪਸ 1;
}


ਪੂਰਨ persist_chunk_client_ਸੁਨੇਹਾ_read_v234(FILE *db_fptr, ਬਣਤਰ P_client_ਸੁਨੇਹਾ *chunk)
{
	uint16_t i16temp;
	ਪੂਰਨ rc;
	uint8_t retain, dup;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	read_e(db_fptr, &chunk->F.store_id, ਆਕਾਰ(dbid_t));

	read_e(db_fptr, &i16temp, ਆਕਾਰ(uint16_t));
	chunk->F.mid = ntohs(i16temp);

	read_e(db_fptr, &chunk->F.qos, ਆਕਾਰ(uint8_t));
	read_e(db_fptr, &retain, ਆਕਾਰ(uint8_t));
	read_e(db_fptr, &chunk->F.direction, ਆਕਾਰ(uint8_t));
	read_e(db_fptr, &chunk->F.state, ਆਕਾਰ(uint8_t));
	read_e(db_fptr, &dup, ਆਕਾਰ(uint8_t));

	chunk->F.retain_dup = (uint8_t)((retain&0x0F)<<4 | (dup&0x0F));

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
error:
	log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	ਵਾਪਸ 1;
}


ਪੂਰਨ persist_chunk_ਅਧਾਰ_ਸੁਨੇਹਾ_read_v234(FILE *db_fptr, ਬਣਤਰ P_ਅਧਾਰ_ਸੁਨੇਹਾ *chunk, uint32_t db_ਸੰਸਕਰਣ)
{
	uint32_t i32temp;
	uint16_t i16temp;
	ਪੂਰਨ rc = 0;
	size_t slen;

	read_e(db_fptr, &chunk->F.store_id, ਆਕਾਰ(dbid_t));

	rc = persist__read_string(db_fptr, &chunk->ਸਰੋਤ.id);
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	ਜੇ(db_ਸੰਸਕਰਣ == 4){
		rc = persist__read_string(db_fptr, &chunk->ਸਰੋਤ.username);
		ਜੇ(rc){
			ਜਾਓ error;
		}
		read_e(db_fptr, &i16temp, ਆਕਾਰ(uint16_t));
		chunk->F.ਸਰੋਤ_ਪੋਰਟ = ntohs(i16temp);
	}

	read_e(db_fptr, &i16temp, ਆਕਾਰ(uint16_t));
	chunk->F.ਸਰੋਤ_mid = ntohs(i16temp);

	/* This is the mid - don't need it */
	read_e(db_fptr, &i16temp, ਆਕਾਰ(uint16_t));

	rc = persist__read_string(db_fptr, &chunk->topic);
	ਜੇ(rc){
		ਜਾਓ error;
	}
	ਜੇ(!chunk->topic){
		rc = MOSQ_ERR_INVAL;
		ਜਾਓ error;
	}
	slen = ਲੜੀ_ਲੰਬਾਈ(chunk->topic);
	ਜੇ(slen > UINT16_MAX){
		rc = MOSQ_ERR_INVAL;
		ਜਾਓ error;
	}
	chunk->F.topic_len = (uint16_t)slen;

	read_e(db_fptr, &chunk->F.qos, ਆਕਾਰ(uint8_t));
	read_e(db_fptr, &chunk->F.retain, ਆਕਾਰ(uint8_t));

	read_e(db_fptr, &i32temp, ਆਕਾਰ(uint32_t));
	chunk->F.payloadlen = ntohl(i32temp);

	ਜੇ(chunk->F.payloadlen){
		ਜੇ(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
			rc = MOSQ_ERR_INVAL;
			ਜਾਓ error;
		}
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		ਜੇ(chunk->payload == ਨਿਰਾ){
			rc = MOSQ_ERR_NOMEM;
			ਜਾਓ error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->ਸਰੋਤ.id);
	mosquitto_FREE(chunk->ਸਰੋਤ.username);
	mosquitto_FREE(chunk->topic);
	ਵਾਪਸ rc;
}


ਪੂਰਨ persist__chunk_retain_read_v234(FILE *db_fptr, ਬਣਤਰ P_retain *chunk)
{
	dbid_t i64temp;

	ਜੇ(fread(&i64temp, ਆਕਾਰ(dbid_t), 1, db_fptr) != 1){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		ਵਾਪਸ 1;
	}
	chunk->F.store_id = i64temp;

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ persist__chunk_sub_read_v234(FILE *db_fptr, ਬਣਤਰ P_sub *chunk)
{
	ਪੂਰਨ rc;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ਜੇ(rc){
		ਜਾਓ error;
	}

	rc = persist__read_string(db_fptr, &chunk->topic);
	ਜੇ(rc){
		ਜਾਓ error;
	}

	read_e(db_fptr, &chunk->F.qos, ਆਕਾਰ(uint8_t));

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(chunk->topic);
	ਵਾਪਸ rc;
}

#ਜੇ_ਅੰਤ
