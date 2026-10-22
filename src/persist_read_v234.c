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

#अन्तर्भाव "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#अन्तर्भाव <arpa/inet.h>
#endif
#अन्तर्भाव <errno.h>
#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <time.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "persist.h"
#अन्तर्भाव "util_mosq.h"


पूर्णाङ्क persist__chunk_header_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	size_t rlen;
	uint16_t i16temp;
	uint32_t i32temp;

	rlen = fread(&i16temp, परिमाण(uint16_t), 1, db_fptr);
	जइ(rlen != 1){
		प्रत्यागम 1;
	}

	rlen = fread(&i32temp, परिमाण(uint32_t), 1, db_fptr);
	जइ(rlen != 1){
		प्रत्यागम 1;
	}

	*chunk = ntohs(i16temp);
	*length = ntohl(i32temp);

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क persist__chunk_cfg_read_v234(FILE *db_fptr, संरचना PF_cfg *chunk)
{
	पूर्णाङ्क rc = MOSQ_ERR_UNKNOWN;

	read_e(db_fptr, &chunk->shutdown, परिमाण(uint8_t)); /* shutdown */
	read_e(db_fptr, &chunk->dbid_size, परिमाण(uint8_t)); /* sizeof(dbid_t) */
	read_e(db_fptr, &chunk->last_db_id, परिमाण(dbid_t));

	प्रत्यागम MOSQ_ERR_SUCCESS;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	प्रत्यागम rc;
}


पूर्णाङ्क persist__chunk_client_read_v234(FILE *db_fptr, संरचना P_client *chunk, uint32_t db_version)
{
	uint16_t i16temp;
	पूर्णाङ्क rc;
	time_t temp;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	जइ(rc){
		प्रत्यागम rc;
	}

	read_e(db_fptr, &i16temp, परिमाण(uint16_t));
	chunk->F.last_mid = ntohs(i16temp);
	जइ(db_version != 2){
		read_e(db_fptr, &temp, परिमाण(time_t));
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	प्रत्यागम 1;
}


पूर्णाङ्क persist_chunk_client_सन्देशः_read_v234(FILE *db_fptr, संरचना P_client_सन्देशः *chunk)
{
	uint16_t i16temp;
	पूर्णाङ्क rc;
	uint8_t retain, dup;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	जइ(rc){
		प्रत्यागम rc;
	}

	read_e(db_fptr, &chunk->F.store_id, परिमाण(dbid_t));

	read_e(db_fptr, &i16temp, परिमाण(uint16_t));
	chunk->F.mid = ntohs(i16temp);

	read_e(db_fptr, &chunk->F.qos, परिमाण(uint8_t));
	read_e(db_fptr, &retain, परिमाण(uint8_t));
	read_e(db_fptr, &chunk->F.direction, परिमाण(uint8_t));
	read_e(db_fptr, &chunk->F.state, परिमाण(uint8_t));
	read_e(db_fptr, &dup, परिमाण(uint8_t));

	chunk->F.retain_dup = (uint8_t)((retain&0x0F)<<4 | (dup&0x0F));

	प्रत्यागम MOSQ_ERR_SUCCESS;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	प्रत्यागम 1;
}


पूर्णाङ्क persist_chunk_base_सन्देशः_read_v234(FILE *db_fptr, संरचना P_base_सन्देशः *chunk, uint32_t db_version)
{
	uint32_t i32temp;
	uint16_t i16temp;
	पूर्णाङ्क rc = 0;
	size_t slen;

	read_e(db_fptr, &chunk->F.store_id, परिमाण(dbid_t));

	rc = persist__read_string(db_fptr, &chunk->source.id);
	जइ(rc){
		प्रत्यागम rc;
	}

	जइ(db_version == 4){
		rc = persist__read_string(db_fptr, &chunk->source.username);
		जइ(rc){
			गच्छ error;
		}
		read_e(db_fptr, &i16temp, परिमाण(uint16_t));
		chunk->F.source_port = ntohs(i16temp);
	}

	read_e(db_fptr, &i16temp, परिमाण(uint16_t));
	chunk->F.source_mid = ntohs(i16temp);

	/* This is the mid - don't need it */
	read_e(db_fptr, &i16temp, परिमाण(uint16_t));

	rc = persist__read_string(db_fptr, &chunk->topic);
	जइ(rc){
		गच्छ error;
	}
	जइ(!chunk->topic){
		rc = MOSQ_ERR_INVAL;
		गच्छ error;
	}
	slen = strlen(chunk->topic);
	जइ(slen > UINT16_MAX){
		rc = MOSQ_ERR_INVAL;
		गच्छ error;
	}
	chunk->F.topic_len = (uint16_t)slen;

	read_e(db_fptr, &chunk->F.qos, परिमाण(uint8_t));
	read_e(db_fptr, &chunk->F.retain, परिमाण(uint8_t));

	read_e(db_fptr, &i32temp, परिमाण(uint32_t));
	chunk->F.payloadlen = ntohl(i32temp);

	जइ(chunk->F.payloadlen){
		जइ(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
			rc = MOSQ_ERR_INVAL;
			गच्छ error;
		}
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		जइ(chunk->payload == रिक्त){
			rc = MOSQ_ERR_NOMEM;
			गच्छ error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	प्रत्यागम rc;
}


पूर्णाङ्क persist__chunk_retain_read_v234(FILE *db_fptr, संरचना P_retain *chunk)
{
	dbid_t i64temp;

	जइ(fread(&i64temp, परिमाण(dbid_t), 1, db_fptr) != 1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		प्रत्यागम 1;
	}
	chunk->F.store_id = i64temp;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क persist__chunk_sub_read_v234(FILE *db_fptr, संरचना P_sub *chunk)
{
	पूर्णाङ्क rc;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	जइ(rc){
		गच्छ error;
	}

	rc = persist__read_string(db_fptr, &chunk->topic);
	जइ(rc){
		गच्छ error;
	}

	read_e(db_fptr, &chunk->F.qos, परिमाण(uint8_t));

	प्रत्यागम MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(chunk->topic);
	प्रत्यागम rc;
}

#endif
