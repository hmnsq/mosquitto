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

#شامل "config.h"

#اگر_تعریف_شدہ WITH_PERSISTENCE

#اگر_غیر_تعریف_شدہ WIN32
#شامل <arpa/inet.h>
#اگر_ختم
#شامل <errno.h>
#شامل <fcntl.h>
#شامل <stdio.h>
#شامل <string.h>
#شامل <sys/stat.h>
#شامل <time.h>

#شامل "mosquitto_broker_internal.h"
#شامل "persist.h"
#شامل "util_mosq.h"


صحیح persist_chunk_سرخی_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *لمبائی)
{
	size_t rlen;
	uint16_t i16temp;
	uint32_t i32temp;

	rlen = fread(&i16temp, حجم(uint16_t), 1, db_fptr);
	اگر(rlen != 1){
		واپس 1;
	}

	rlen = fread(&i32temp, حجم(uint32_t), 1, db_fptr);
	اگر(rlen != 1){
		واپس 1;
	}

	*chunk = ntohs(i16temp);
	*لمبائی = ntohl(i32temp);

	واپس MOSQ_ERR_SUCCESS;
}


صحیح persist__chunk_cfg_read_v234(FILE *db_fptr, ساخت PF_cfg *chunk)
{
	صحیح rc = MOSQ_ERR_UNKNOWN;

	read_e(db_fptr, &chunk->shutdown, حجم(uint8_t)); /* shutdown */
	read_e(db_fptr, &chunk->dbid_سائز, حجم(uint8_t)); /* sizeof(dbid_t) */
	read_e(db_fptr, &chunk->آخری_db_id, حجم(dbid_t));

	واپس MOSQ_ERR_SUCCESS;
error:
	log__printf(تہی, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	واپس rc;
}


صحیح persist__chunk_client_read_v234(FILE *db_fptr, ساخت P_client *chunk, uint32_t db_نسخہ)
{
	uint16_t i16temp;
	صحیح rc;
	time_t عارضی;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	اگر(rc){
		واپس rc;
	}

	read_e(db_fptr, &i16temp, حجم(uint16_t));
	chunk->F.آخری_mid = ntohs(i16temp);
	اگر(db_نسخہ != 2){
		read_e(db_fptr, &عارضی, حجم(time_t));
	}

	واپس MOSQ_ERR_SUCCESS;
error:
	log__printf(تہی, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	واپس 1;
}


صحیح persist_chunk_client_پیغام_read_v234(FILE *db_fptr, ساخت P_client_پیغام *chunk)
{
	uint16_t i16temp;
	صحیح rc;
	uint8_t retain, dup;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	اگر(rc){
		واپس rc;
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

	واپس MOSQ_ERR_SUCCESS;
error:
	log__printf(تہی, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	واپس 1;
}


صحیح persist_chunk_بنیاد_پیغام_read_v234(FILE *db_fptr, ساخت P_بنیاد_پیغام *chunk, uint32_t db_نسخہ)
{
	uint32_t i32temp;
	uint16_t i16temp;
	صحیح rc = 0;
	size_t slen;

	read_e(db_fptr, &chunk->F.store_id, حجم(dbid_t));

	rc = persist__read_string(db_fptr, &chunk->ماخذ.id);
	اگر(rc){
		واپس rc;
	}

	اگر(db_نسخہ == 4){
		rc = persist__read_string(db_fptr, &chunk->ماخذ.username);
		اگر(rc){
			جاؤ error;
		}
		read_e(db_fptr, &i16temp, حجم(uint16_t));
		chunk->F.ماخذ_پورٹ = ntohs(i16temp);
	}

	read_e(db_fptr, &i16temp, حجم(uint16_t));
	chunk->F.ماخذ_mid = ntohs(i16temp);

	/* This is the mid - don't need it */
	read_e(db_fptr, &i16temp, حجم(uint16_t));

	rc = persist__read_string(db_fptr, &chunk->topic);
	اگر(rc){
		جاؤ error;
	}
	اگر(!chunk->topic){
		rc = MOSQ_ERR_INVAL;
		جاؤ error;
	}
	slen = لڑی_لمبائی(chunk->topic);
	اگر(slen > UINT16_MAX){
		rc = MOSQ_ERR_INVAL;
		جاؤ error;
	}
	chunk->F.topic_len = (uint16_t)slen;

	read_e(db_fptr, &chunk->F.qos, حجم(uint8_t));
	read_e(db_fptr, &chunk->F.retain, حجم(uint8_t));

	read_e(db_fptr, &i32temp, حجم(uint32_t));
	chunk->F.payloadlen = ntohl(i32temp);

	اگر(chunk->F.payloadlen){
		اگر(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
			rc = MOSQ_ERR_INVAL;
			جاؤ error;
		}
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		اگر(chunk->payload == تہی){
			rc = MOSQ_ERR_NOMEM;
			جاؤ error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	واپس MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->ماخذ.id);
	mosquitto_FREE(chunk->ماخذ.username);
	mosquitto_FREE(chunk->topic);
	واپس rc;
}


صحیح persist__chunk_retain_read_v234(FILE *db_fptr, ساخت P_retain *chunk)
{
	dbid_t i64temp;

	اگر(fread(&i64temp, حجم(dbid_t), 1, db_fptr) != 1){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		واپس 1;
	}
	chunk->F.store_id = i64temp;

	واپس MOSQ_ERR_SUCCESS;
}


صحیح persist__chunk_sub_read_v234(FILE *db_fptr, ساخت P_sub *chunk)
{
	صحیح rc;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	اگر(rc){
		جاؤ error;
	}

	rc = persist__read_string(db_fptr, &chunk->topic);
	اگر(rc){
		جاؤ error;
	}

	read_e(db_fptr, &chunk->F.qos, حجم(uint8_t));

	واپس MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(chunk->topic);
	واپس rc;
}

#اگر_ختم
