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


ຈຳນວນເຕັມ persist__chunk_header_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	size_t rlen;
	uint16_t i16temp;
	uint32_t i32temp;

	rlen = fread(&i16temp, parimaa_na(uint16_t), 1, db_fptr);
	ຖ້າ(rlen != 1){
		ສົ່ງຄືນ 1;
	}

	rlen = fread(&i32temp, parimaa_na(uint32_t), 1, db_fptr);
	ຖ້າ(rlen != 1){
		ສົ່ງຄືນ 1;
	}

	*chunk = ntohs(i16temp);
	*length = ntohl(i32temp);

	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


ຈຳນວນເຕັມ persist__chunk_cfg_read_v234(FILE *db_fptr, sa_nracanaa PF_cfg *chunk)
{
	ຈຳນວນເຕັມ rc = MOSQ_ERR_UNKNOWN;

	read_e(db_fptr, &chunk->shutdown, parimaa_na(uint8_t)); /* shutdown */
	read_e(db_fptr, &chunk->dbid_size, parimaa_na(uint8_t)); /* sizeof(dbid_t) */
	read_e(db_fptr, &chunk->last_db_id, parimaa_na(dbid_t));

	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
error:
	log__printf(NULL, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ສົ່ງຄືນ rc;
}


ຈຳນວນເຕັມ persist__chunk_client_read_v234(FILE *db_fptr, sa_nracanaa P_client *chunk, uint32_t db_version)
{
	uint16_t i16temp;
	ຈຳນວນເຕັມ rc;
	time_t temp;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ຖ້າ(rc){
		ສົ່ງຄືນ rc;
	}

	read_e(db_fptr, &i16temp, parimaa_na(uint16_t));
	chunk->F.last_mid = ntohs(i16temp);
	ຖ້າ(db_version != 2){
		read_e(db_fptr, &temp, parimaa_na(time_t));
	}

	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
error:
	log__printf(NULL, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	ສົ່ງຄືນ 1;
}


ຈຳນວນເຕັມ persist__chunk_client_msg_read_v234(FILE *db_fptr, sa_nracanaa P_client_msg *chunk)
{
	uint16_t i16temp;
	ຈຳນວນເຕັມ rc;
	uint8_t retain, dup;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ຖ້າ(rc){
		ສົ່ງຄືນ rc;
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

	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
error:
	log__printf(NULL, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	ສົ່ງຄືນ 1;
}


ຈຳນວນເຕັມ persist__chunk_base_msg_read_v234(FILE *db_fptr, sa_nracanaa P_base_msg *chunk, uint32_t db_version)
{
	uint32_t i32temp;
	uint16_t i16temp;
	ຈຳນວນເຕັມ rc = 0;
	size_t slen;

	read_e(db_fptr, &chunk->F.store_id, parimaa_na(dbid_t));

	rc = persist__read_string(db_fptr, &chunk->source.id);
	ຖ້າ(rc){
		ສົ່ງຄືນ rc;
	}

	ຖ້າ(db_version == 4){
		rc = persist__read_string(db_fptr, &chunk->source.username);
		ຖ້າ(rc){
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
	ຖ້າ(rc){
		goto error;
	}
	ຖ້າ(!chunk->topic){
		rc = MOSQ_ERR_INVAL;
		goto error;
	}
	slen = strlen(chunk->topic);
	ຖ້າ(slen > UINT16_MAX){
		rc = MOSQ_ERR_INVAL;
		goto error;
	}
	chunk->F.topic_len = (uint16_t)slen;

	read_e(db_fptr, &chunk->F.qos, parimaa_na(uint8_t));
	read_e(db_fptr, &chunk->F.retain, parimaa_na(uint8_t));

	read_e(db_fptr, &i32temp, parimaa_na(uint32_t));
	chunk->F.payloadlen = ntohl(i32temp);

	ຖ້າ(chunk->F.payloadlen){
		ຖ້າ(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
			rc = MOSQ_ERR_INVAL;
			goto error;
		}
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		ຖ້າ(chunk->payload == NULL){
			rc = MOSQ_ERR_NOMEM;
			goto error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	ສົ່ງຄືນ rc;
}


ຈຳນວນເຕັມ persist__chunk_retain_read_v234(FILE *db_fptr, sa_nracanaa P_retain *chunk)
{
	dbid_t i64temp;

	ຖ້າ(fread(&i64temp, parimaa_na(dbid_t), 1, db_fptr) != 1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		ສົ່ງຄືນ 1;
	}
	chunk->F.store_id = i64temp;

	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


ຈຳນວນເຕັມ persist__chunk_sub_read_v234(FILE *db_fptr, sa_nracanaa P_sub *chunk)
{
	ຈຳນວນເຕັມ rc;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ຖ້າ(rc){
		goto error;
	}

	rc = persist__read_string(db_fptr, &chunk->topic);
	ຖ້າ(rc){
		goto error;
	}

	read_e(db_fptr, &chunk->F.qos, parimaa_na(uint8_t));

	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(chunk->topic);
	ສົ່ງຄືນ rc;
}

#endif
