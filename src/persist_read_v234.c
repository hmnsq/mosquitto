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

#ضمن "config.h"

#اگر_تعریف_شده WITH_PERSISTENCE

#اگر_تعریف_نشده WIN32
#ضمن <arpa/inet.h>
#پایان_اگر
#ضمن <errno.h>
#ضمن <fcntl.h>
#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <sys/stat.h>
#ضمن <time.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "persist.h"
#ضمن "util_mosq.h"


صحيح persist_chunk_سرآیند_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *درازی)
{
	size_t rlen;
	uint16_t i16temp;
	uint32_t i32temp;

	rlen = fread(&i16temp, حجم(uint16_t), 1, db_fptr);
	إذا(rlen != 1){
		أرجع 1;
	}

	rlen = fread(&i32temp, حجم(uint32_t), 1, db_fptr);
	إذا(rlen != 1){
		أرجع 1;
	}

	*chunk = ntohs(i16temp);
	*درازی = ntohl(i32temp);

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح persist__chunk_cfg_read_v234(FILE *db_fptr, بنية PF_cfg *chunk)
{
	صحيح rc = MOSQ_ERR_UNKNOWN;

	read_e(db_fptr, &chunk->shutdown, حجم(uint8_t)); /* shutdown */
	read_e(db_fptr, &chunk->dbid_اندازه, حجم(uint8_t)); /* sizeof(dbid_t) */
	read_e(db_fptr, &chunk->آخرین_db_id, حجم(dbid_t));

	أرجع MOSQ_ERR_SUCCESS;
error:
	log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	أرجع rc;
}


صحيح persist__chunk_client_read_v234(FILE *db_fptr, بنية P_client *chunk, uint32_t db_نسخه)
{
	uint16_t i16temp;
	صحيح rc;
	time_t موقت;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	إذا(rc){
		أرجع rc;
	}

	read_e(db_fptr, &i16temp, حجم(uint16_t));
	chunk->F.آخرین_mid = ntohs(i16temp);
	إذا(db_نسخه != 2){
		read_e(db_fptr, &موقت, حجم(time_t));
	}

	أرجع MOSQ_ERR_SUCCESS;
error:
	log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	أرجع 1;
}


صحيح persist_chunk_client_پیام_read_v234(FILE *db_fptr, بنية P_client_پیام *chunk)
{
	uint16_t i16temp;
	صحيح rc;
	uint8_t retain, dup;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	إذا(rc){
		أرجع rc;
	}

	read_e(db_fptr, &chunk->F.store_id, حجم(dbid_t));

	read_e(db_fptr, &i16temp, حجم(uint16_t));
	chunk->F.mid = ntohs(i16temp);

	read_e(db_fptr, &chunk->F.qos, حجم(uint8_t));
	read_e(db_fptr, &retain, حجم(uint8_t));
	read_e(db_fptr, &chunk->F.direction, حجم(uint8_t));
	read_e(db_fptr, &chunk->F.state, حجم(uint8_t));
	read_e(db_fptr, &dup, حجم(uint8_t));

	chunk->F.retain_dup = (uint8_t)((retain&0x0F)<<4 | (dup&0x0F));

	أرجع MOSQ_ERR_SUCCESS;
error:
	log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	أرجع 1;
}


صحيح persist_chunk_پایه_پیام_read_v234(FILE *db_fptr, بنية P_پایه_پیام *chunk, uint32_t db_نسخه)
{
	uint32_t i32temp;
	uint16_t i16temp;
	صحيح rc = 0;
	size_t slen;

	read_e(db_fptr, &chunk->F.store_id, حجم(dbid_t));

	rc = persist__read_string(db_fptr, &chunk->منبع.id);
	إذا(rc){
		أرجع rc;
	}

	إذا(db_نسخه == 4){
		rc = persist__read_string(db_fptr, &chunk->منبع.username);
		إذا(rc){
			اذهب error;
		}
		read_e(db_fptr, &i16temp, حجم(uint16_t));
		chunk->F.منبع_پورت = ntohs(i16temp);
	}

	read_e(db_fptr, &i16temp, حجم(uint16_t));
	chunk->F.منبع_mid = ntohs(i16temp);

	/* This is the mid - don't need it */
	read_e(db_fptr, &i16temp, حجم(uint16_t));

	rc = persist__read_string(db_fptr, &chunk->topic);
	إذا(rc){
		اذهب error;
	}
	إذا(!chunk->topic){
		rc = MOSQ_ERR_INVAL;
		اذهب error;
	}
	slen = طول_رشته(chunk->topic);
	إذا(slen > UINT16_MAX){
		rc = MOSQ_ERR_INVAL;
		اذهب error;
	}
	chunk->F.topic_len = (uint16_t)slen;

	read_e(db_fptr, &chunk->F.qos, حجم(uint8_t));
	read_e(db_fptr, &chunk->F.retain, حجم(uint8_t));

	read_e(db_fptr, &i32temp, حجم(uint32_t));
	chunk->F.payloadlen = ntohl(i32temp);

	إذا(chunk->F.payloadlen){
		إذا(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
			rc = MOSQ_ERR_INVAL;
			اذهب error;
		}
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		إذا(chunk->payload == عدم){
			rc = MOSQ_ERR_NOMEM;
			اذهب error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	أرجع MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->منبع.id);
	mosquitto_FREE(chunk->منبع.username);
	mosquitto_FREE(chunk->topic);
	أرجع rc;
}


صحيح persist__chunk_retain_read_v234(FILE *db_fptr, بنية P_retain *chunk)
{
	dbid_t i64temp;

	إذا(fread(&i64temp, حجم(dbid_t), 1, db_fptr) != 1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		أرجع 1;
	}
	chunk->F.store_id = i64temp;

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح persist__chunk_sub_read_v234(FILE *db_fptr, بنية P_sub *chunk)
{
	صحيح rc;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	إذا(rc){
		اذهب error;
	}

	rc = persist__read_string(db_fptr, &chunk->topic);
	إذا(rc){
		اذهب error;
	}

	read_e(db_fptr, &chunk->F.qos, حجم(uint8_t));

	أرجع MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(chunk->topic);
	أرجع rc;
}

#پایان_اگر
