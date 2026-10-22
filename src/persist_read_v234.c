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

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#شامل <arpa/inet.h>
#endif
#شامل <errno.h>
#شامل <fcntl.h>
#شامل <stdio.h>
#شامل <string.h>
#شامل <sys/stat.h>
#شامل <time.h>

#شامل "mosquitto_broker_internal.h"
#شامل "persist.h"
#شامل "util_mosq.h"


صحیح persist__chunk_header_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	size_t rlen;
	uint16_t i16temp;
	uint32_t i32temp;

	rlen = fread(&i16temp, اندازه(uint16_t), 1, db_fptr);
	اگر(rlen != 1){
		بازگشت 1;
	}

	rlen = fread(&i32temp, اندازه(uint32_t), 1, db_fptr);
	اگر(rlen != 1){
		بازگشت 1;
	}

	*chunk = ntohs(i16temp);
	*length = ntohl(i32temp);

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح persist__chunk_cfg_read_v234(FILE *db_fptr, ساختار PF_cfg *chunk)
{
	صحیح rc = MOSQ_ERR_UNKNOWN;

	read_e(db_fptr, &chunk->shutdown, اندازه(uint8_t)); /* shutdown */
	read_e(db_fptr, &chunk->dbid_size, اندازه(uint8_t)); /* sizeof(dbid_t) */
	read_e(db_fptr, &chunk->last_db_id, اندازه(dbid_t));

	بازگشت MOSQ_ERR_SUCCESS;
error:
	log__printf(پوچ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	بازگشت rc;
}


صحیح persist__chunk_client_read_v234(FILE *db_fptr, ساختار P_client *chunk, uint32_t db_version)
{
	uint16_t i16temp;
	صحیح rc;
	time_t temp;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	اگر(rc){
		بازگشت rc;
	}

	read_e(db_fptr, &i16temp, اندازه(uint16_t));
	chunk->F.last_mid = ntohs(i16temp);
	اگر(db_version != 2){
		read_e(db_fptr, &temp, اندازه(time_t));
	}

	بازگشت MOSQ_ERR_SUCCESS;
error:
	log__printf(پوچ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	بازگشت 1;
}


صحیح persist_chunk_client_پیام_read_v234(FILE *db_fptr, ساختار P_client_پیام *chunk)
{
	uint16_t i16temp;
	صحیح rc;
	uint8_t retain, dup;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	اگر(rc){
		بازگشت rc;
	}

	read_e(db_fptr, &chunk->F.store_id, اندازه(dbid_t));

	read_e(db_fptr, &i16temp, اندازه(uint16_t));
	chunk->F.mid = ntohs(i16temp);

	read_e(db_fptr, &chunk->F.qos, اندازه(uint8_t));
	read_e(db_fptr, &retain, اندازه(uint8_t));
	read_e(db_fptr, &chunk->F.direction, اندازه(uint8_t));
	read_e(db_fptr, &chunk->F.state, اندازه(uint8_t));
	read_e(db_fptr, &dup, اندازه(uint8_t));

	chunk->F.retain_dup = (uint8_t)((retain&0x0F)<<4 | (dup&0x0F));

	بازگشت MOSQ_ERR_SUCCESS;
error:
	log__printf(پوچ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	بازگشت 1;
}


صحیح persist_chunk_base_پیام_read_v234(FILE *db_fptr, ساختار P_base_پیام *chunk, uint32_t db_version)
{
	uint32_t i32temp;
	uint16_t i16temp;
	صحیح rc = 0;
	size_t slen;

	read_e(db_fptr, &chunk->F.store_id, اندازه(dbid_t));

	rc = persist__read_string(db_fptr, &chunk->source.id);
	اگر(rc){
		بازگشت rc;
	}

	اگر(db_version == 4){
		rc = persist__read_string(db_fptr, &chunk->source.username);
		اگر(rc){
			برو error;
		}
		read_e(db_fptr, &i16temp, اندازه(uint16_t));
		chunk->F.source_port = ntohs(i16temp);
	}

	read_e(db_fptr, &i16temp, اندازه(uint16_t));
	chunk->F.source_mid = ntohs(i16temp);

	/* This is the mid - don't need it */
	read_e(db_fptr, &i16temp, اندازه(uint16_t));

	rc = persist__read_string(db_fptr, &chunk->topic);
	اگر(rc){
		برو error;
	}
	اگر(!chunk->topic){
		rc = MOSQ_ERR_INVAL;
		برو error;
	}
	slen = strlen(chunk->topic);
	اگر(slen > UINT16_MAX){
		rc = MOSQ_ERR_INVAL;
		برو error;
	}
	chunk->F.topic_len = (uint16_t)slen;

	read_e(db_fptr, &chunk->F.qos, اندازه(uint8_t));
	read_e(db_fptr, &chunk->F.retain, اندازه(uint8_t));

	read_e(db_fptr, &i32temp, اندازه(uint32_t));
	chunk->F.payloadlen = ntohl(i32temp);

	اگر(chunk->F.payloadlen){
		اگر(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
			rc = MOSQ_ERR_INVAL;
			برو error;
		}
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		اگر(chunk->payload == پوچ){
			rc = MOSQ_ERR_NOMEM;
			برو error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	بازگشت MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	بازگشت rc;
}


صحیح persist__chunk_retain_read_v234(FILE *db_fptr, ساختار P_retain *chunk)
{
	dbid_t i64temp;

	اگر(fread(&i64temp, اندازه(dbid_t), 1, db_fptr) != 1){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		بازگشت 1;
	}
	chunk->F.store_id = i64temp;

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح persist__chunk_sub_read_v234(FILE *db_fptr, ساختار P_sub *chunk)
{
	صحیح rc;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	اگر(rc){
		برو error;
	}

	rc = persist__read_string(db_fptr, &chunk->topic);
	اگر(rc){
		برو error;
	}

	read_e(db_fptr, &chunk->F.qos, اندازه(uint8_t));

	بازگشت MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(chunk->topic);
	بازگشت rc;
}

#endif
