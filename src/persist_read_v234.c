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

#அந்தர்பாவ "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#அந்தர்பாவ <arpa/inet.h>
#endif
#அந்தர்பாவ <errno.h>
#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <time.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "persist.h"
#அந்தர்பாவ "util_mosq.h"


முழு persist__chunk_header_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	size_t rlen;
	uint16_t i16temp;
	uint32_t i32temp;

	rlen = fread(&i16temp, அளவு(uint16_t), 1, db_fptr);
	எனில்(rlen != 1){
		திரும்பு 1;
	}

	rlen = fread(&i32temp, அளவு(uint32_t), 1, db_fptr);
	எனில்(rlen != 1){
		திரும்பு 1;
	}

	*chunk = ntohs(i16temp);
	*length = ntohl(i32temp);

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு persist__chunk_cfg_read_v234(FILE *db_fptr, கட்டமைப்பு PF_cfg *chunk)
{
	முழு rc = MOSQ_ERR_UNKNOWN;

	read_e(db_fptr, &chunk->shutdown, அளவு(uint8_t)); /* shutdown */
	read_e(db_fptr, &chunk->dbid_size, அளவு(uint8_t)); /* sizeof(dbid_t) */
	read_e(db_fptr, &chunk->last_db_id, அளவு(dbid_t));

	திரும்பு MOSQ_ERR_SUCCESS;
error:
	log__printf(வெற்று, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	திரும்பு rc;
}


முழு persist__chunk_client_read_v234(FILE *db_fptr, கட்டமைப்பு P_client *chunk, uint32_t db_version)
{
	uint16_t i16temp;
	முழு rc;
	time_t temp;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	எனில்(rc){
		திரும்பு rc;
	}

	read_e(db_fptr, &i16temp, அளவு(uint16_t));
	chunk->F.last_mid = ntohs(i16temp);
	எனில்(db_version != 2){
		read_e(db_fptr, &temp, அளவு(time_t));
	}

	திரும்பு MOSQ_ERR_SUCCESS;
error:
	log__printf(வெற்று, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	திரும்பு 1;
}


முழு persist_chunk_client_செய்தி_read_v234(FILE *db_fptr, கட்டமைப்பு P_client_செய்தி *chunk)
{
	uint16_t i16temp;
	முழு rc;
	uint8_t retain, dup;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	எனில்(rc){
		திரும்பு rc;
	}

	read_e(db_fptr, &chunk->F.store_id, அளவு(dbid_t));

	read_e(db_fptr, &i16temp, அளவு(uint16_t));
	chunk->F.mid = ntohs(i16temp);

	read_e(db_fptr, &chunk->F.qos, அளவு(uint8_t));
	read_e(db_fptr, &retain, அளவு(uint8_t));
	read_e(db_fptr, &chunk->F.direction, அளவு(uint8_t));
	read_e(db_fptr, &chunk->F.state, அளவு(uint8_t));
	read_e(db_fptr, &dup, அளவு(uint8_t));

	chunk->F.retain_dup = (uint8_t)((retain&0x0F)<<4 | (dup&0x0F));

	திரும்பு MOSQ_ERR_SUCCESS;
error:
	log__printf(வெற்று, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	திரும்பு 1;
}


முழு persist_chunk_base_செய்தி_read_v234(FILE *db_fptr, கட்டமைப்பு P_base_செய்தி *chunk, uint32_t db_version)
{
	uint32_t i32temp;
	uint16_t i16temp;
	முழு rc = 0;
	size_t slen;

	read_e(db_fptr, &chunk->F.store_id, அளவு(dbid_t));

	rc = persist__read_string(db_fptr, &chunk->source.id);
	எனில்(rc){
		திரும்பு rc;
	}

	எனில்(db_version == 4){
		rc = persist__read_string(db_fptr, &chunk->source.username);
		எனில்(rc){
			செல் error;
		}
		read_e(db_fptr, &i16temp, அளவு(uint16_t));
		chunk->F.source_port = ntohs(i16temp);
	}

	read_e(db_fptr, &i16temp, அளவு(uint16_t));
	chunk->F.source_mid = ntohs(i16temp);

	/* This is the mid - don't need it */
	read_e(db_fptr, &i16temp, அளவு(uint16_t));

	rc = persist__read_string(db_fptr, &chunk->topic);
	எனில்(rc){
		செல் error;
	}
	எனில்(!chunk->topic){
		rc = MOSQ_ERR_INVAL;
		செல் error;
	}
	slen = strlen(chunk->topic);
	எனில்(slen > UINT16_MAX){
		rc = MOSQ_ERR_INVAL;
		செல் error;
	}
	chunk->F.topic_len = (uint16_t)slen;

	read_e(db_fptr, &chunk->F.qos, அளவு(uint8_t));
	read_e(db_fptr, &chunk->F.retain, அளவு(uint8_t));

	read_e(db_fptr, &i32temp, அளவு(uint32_t));
	chunk->F.payloadlen = ntohl(i32temp);

	எனில்(chunk->F.payloadlen){
		எனில்(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
			rc = MOSQ_ERR_INVAL;
			செல் error;
		}
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		எனில்(chunk->payload == வெற்று){
			rc = MOSQ_ERR_NOMEM;
			செல் error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	திரும்பு rc;
}


முழு persist__chunk_retain_read_v234(FILE *db_fptr, கட்டமைப்பு P_retain *chunk)
{
	dbid_t i64temp;

	எனில்(fread(&i64temp, அளவு(dbid_t), 1, db_fptr) != 1){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		திரும்பு 1;
	}
	chunk->F.store_id = i64temp;

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு persist__chunk_sub_read_v234(FILE *db_fptr, கட்டமைப்பு P_sub *chunk)
{
	முழு rc;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	எனில்(rc){
		செல் error;
	}

	rc = persist__read_string(db_fptr, &chunk->topic);
	எனில்(rc){
		செல் error;
	}

	read_e(db_fptr, &chunk->F.qos, அளவு(uint8_t));

	திரும்பு MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(chunk->topic);
	திரும்பு rc;
}

#endif
