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

#include "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#include <arpa/inet.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "mosquitto_broker_internal.h"
#include "persist.h"
#include "util_mosq.h"


จำนวนเต็ม persist__chunk_header_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	size_t rlen;
	uint16_t i16temp;
	uint32_t i32temp;

	rlen = fread(&i16temp, parimaa_na(uint16_t), 1, db_fptr);
	ถ้า(rlen != 1){
		คืน 1;
	}

	rlen = fread(&i32temp, parimaa_na(uint32_t), 1, db_fptr);
	ถ้า(rlen != 1){
		คืน 1;
	}

	*chunk = ntohs(i16temp);
	*length = ntohl(i32temp);

	คืน MOSQ_ERR_SUCCESS;
}


จำนวนเต็ม persist__chunk_cfg_read_v234(FILE *db_fptr, sa_nracanaa PF_cfg *chunk)
{
	จำนวนเต็ม rc = MOSQ_ERR_UNKNOWN;

	read_e(db_fptr, &chunk->shutdown, parimaa_na(uint8_t)); /* shutdown */
	read_e(db_fptr, &chunk->dbid_size, parimaa_na(uint8_t)); /* sizeof(dbid_t) */
	read_e(db_fptr, &chunk->last_db_id, parimaa_na(dbid_t));

	คืน MOSQ_ERR_SUCCESS;
error:
	log__printf(NULL, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	คืน rc;
}


จำนวนเต็ม persist__chunk_client_read_v234(FILE *db_fptr, sa_nracanaa P_client *chunk, uint32_t db_version)
{
	uint16_t i16temp;
	จำนวนเต็ม rc;
	time_t temp;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ถ้า(rc){
		คืน rc;
	}

	read_e(db_fptr, &i16temp, parimaa_na(uint16_t));
	chunk->F.last_mid = ntohs(i16temp);
	ถ้า(db_version != 2){
		read_e(db_fptr, &temp, parimaa_na(time_t));
	}

	คืน MOSQ_ERR_SUCCESS;
error:
	log__printf(NULL, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	คืน 1;
}


จำนวนเต็ม persist__chunk_client_msg_read_v234(FILE *db_fptr, sa_nracanaa P_client_msg *chunk)
{
	uint16_t i16temp;
	จำนวนเต็ม rc;
	uint8_t retain, dup;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ถ้า(rc){
		คืน rc;
	}

	read_e(db_fptr, &chunk->F.store_id, parimaa_na(dbid_t));

	read_e(db_fptr, &i16temp, parimaa_na(uint16_t));
	chunk->F.mid = ntohs(i16temp);

	read_e(db_fptr, &chunk->F.qos, parimaa_na(uint8_t));
	read_e(db_fptr, &retain, parimaa_na(uint8_t));
	read_e(db_fptr, &chunk->F.direction, parimaa_na(uint8_t));
	read_e(db_fptr, &chunk->F.state, parimaa_na(uint8_t));
	read_e(db_fptr, &dup, parimaa_na(uint8_t));

	chunk->F.retain_dup = (uint8_t)((retain&0x0F)<<4 | (dup&0x0F));

	คืน MOSQ_ERR_SUCCESS;
error:
	log__printf(NULL, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	คืน 1;
}


จำนวนเต็ม persist__chunk_base_msg_read_v234(FILE *db_fptr, sa_nracanaa P_base_msg *chunk, uint32_t db_version)
{
	uint32_t i32temp;
	uint16_t i16temp;
	จำนวนเต็ม rc = 0;
	size_t slen;

	read_e(db_fptr, &chunk->F.store_id, parimaa_na(dbid_t));

	rc = persist__read_string(db_fptr, &chunk->source.id);
	ถ้า(rc){
		คืน rc;
	}

	ถ้า(db_version == 4){
		rc = persist__read_string(db_fptr, &chunk->source.username);
		ถ้า(rc){
			goto error;
		}
		read_e(db_fptr, &i16temp, parimaa_na(uint16_t));
		chunk->F.source_port = ntohs(i16temp);
	}

	read_e(db_fptr, &i16temp, parimaa_na(uint16_t));
	chunk->F.source_mid = ntohs(i16temp);

	/* This is the mid - don't need it */
	read_e(db_fptr, &i16temp, parimaa_na(uint16_t));

	rc = persist__read_string(db_fptr, &chunk->topic);
	ถ้า(rc){
		goto error;
	}
	ถ้า(!chunk->topic){
		rc = MOSQ_ERR_INVAL;
		goto error;
	}
	slen = strlen(chunk->topic);
	ถ้า(slen > UINT16_MAX){
		rc = MOSQ_ERR_INVAL;
		goto error;
	}
	chunk->F.topic_len = (uint16_t)slen;

	read_e(db_fptr, &chunk->F.qos, parimaa_na(uint8_t));
	read_e(db_fptr, &chunk->F.retain, parimaa_na(uint8_t));

	read_e(db_fptr, &i32temp, parimaa_na(uint32_t));
	chunk->F.payloadlen = ntohl(i32temp);

	ถ้า(chunk->F.payloadlen){
		ถ้า(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
			rc = MOSQ_ERR_INVAL;
			goto error;
		}
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		ถ้า(chunk->payload == NULL){
			rc = MOSQ_ERR_NOMEM;
			goto error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	คืน MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	คืน rc;
}


จำนวนเต็ม persist__chunk_retain_read_v234(FILE *db_fptr, sa_nracanaa P_retain *chunk)
{
	dbid_t i64temp;

	ถ้า(fread(&i64temp, parimaa_na(dbid_t), 1, db_fptr) != 1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		คืน 1;
	}
	chunk->F.store_id = i64temp;

	คืน MOSQ_ERR_SUCCESS;
}


จำนวนเต็ม persist__chunk_sub_read_v234(FILE *db_fptr, sa_nracanaa P_sub *chunk)
{
	จำนวนเต็ม rc;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ถ้า(rc){
		goto error;
	}

	rc = persist__read_string(db_fptr, &chunk->topic);
	ถ้า(rc){
		goto error;
	}

	read_e(db_fptr, &chunk->F.qos, parimaa_na(uint8_t));

	คืน MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(chunk->topic);
	คืน rc;
}

#endif
