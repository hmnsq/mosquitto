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

#അന്തര്ഭാവ "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#അന്തര്ഭാവ <errno.h>
#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <time.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "persist.h"
#അന്തര്ഭാവ "util_mosq.h"


പൂർണ്ണസംഖ്യ persist__chunk_header_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	size_t rlen;
	uint16_t i16temp;
	uint32_t i32temp;

	rlen = fread(&i16temp, വലുപ്പം(uint16_t), 1, db_fptr);
	എങ്കിൽ(rlen != 1){
		തിരികെ 1;
	}

	rlen = fread(&i32temp, വലുപ്പം(uint32_t), 1, db_fptr);
	എങ്കിൽ(rlen != 1){
		തിരികെ 1;
	}

	*chunk = ntohs(i16temp);
	*length = ntohl(i32temp);

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ persist__chunk_cfg_read_v234(FILE *db_fptr, ഘടന PF_cfg *chunk)
{
	പൂർണ്ണസംഖ്യ rc = MOSQ_ERR_UNKNOWN;

	read_e(db_fptr, &chunk->shutdown, വലുപ്പം(uint8_t)); /* shutdown */
	read_e(db_fptr, &chunk->dbid_size, വലുപ്പം(uint8_t)); /* sizeof(dbid_t) */
	read_e(db_fptr, &chunk->last_db_id, വലുപ്പം(dbid_t));

	തിരികെ MOSQ_ERR_SUCCESS;
error:
	log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	തിരികെ rc;
}


പൂർണ്ണസംഖ്യ persist__chunk_client_read_v234(FILE *db_fptr, ഘടന P_client *chunk, uint32_t db_version)
{
	uint16_t i16temp;
	പൂർണ്ണസംഖ്യ rc;
	time_t temp;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	read_e(db_fptr, &i16temp, വലുപ്പം(uint16_t));
	chunk->F.last_mid = ntohs(i16temp);
	എങ്കിൽ(db_version != 2){
		read_e(db_fptr, &temp, വലുപ്പം(time_t));
	}

	തിരികെ MOSQ_ERR_SUCCESS;
error:
	log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	തിരികെ 1;
}


പൂർണ്ണസംഖ്യ persist_chunk_client_സന്ദേശം_read_v234(FILE *db_fptr, ഘടന P_client_സന്ദേശം *chunk)
{
	uint16_t i16temp;
	പൂർണ്ണസംഖ്യ rc;
	uint8_t retain, dup;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	read_e(db_fptr, &chunk->F.store_id, വലുപ്പം(dbid_t));

	read_e(db_fptr, &i16temp, വലുപ്പം(uint16_t));
	chunk->F.mid = ntohs(i16temp);

	read_e(db_fptr, &chunk->F.qos, വലുപ്പം(uint8_t));
	read_e(db_fptr, &retain, വലുപ്പം(uint8_t));
	read_e(db_fptr, &chunk->F.direction, വലുപ്പം(uint8_t));
	read_e(db_fptr, &chunk->F.state, വലുപ്പം(uint8_t));
	read_e(db_fptr, &dup, വലുപ്പം(uint8_t));

	chunk->F.retain_dup = (uint8_t)((retain&0x0F)<<4 | (dup&0x0F));

	തിരികെ MOSQ_ERR_SUCCESS;
error:
	log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	തിരികെ 1;
}


പൂർണ്ണസംഖ്യ persist_chunk_base_സന്ദേശം_read_v234(FILE *db_fptr, ഘടന P_base_സന്ദേശം *chunk, uint32_t db_version)
{
	uint32_t i32temp;
	uint16_t i16temp;
	പൂർണ്ണസംഖ്യ rc = 0;
	size_t slen;

	read_e(db_fptr, &chunk->F.store_id, വലുപ്പം(dbid_t));

	rc = persist__read_string(db_fptr, &chunk->source.id);
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	എങ്കിൽ(db_version == 4){
		rc = persist__read_string(db_fptr, &chunk->source.username);
		എങ്കിൽ(rc){
			പോകുക error;
		}
		read_e(db_fptr, &i16temp, വലുപ്പം(uint16_t));
		chunk->F.source_port = ntohs(i16temp);
	}

	read_e(db_fptr, &i16temp, വലുപ്പം(uint16_t));
	chunk->F.source_mid = ntohs(i16temp);

	/* This is the mid - don't need it */
	read_e(db_fptr, &i16temp, വലുപ്പം(uint16_t));

	rc = persist__read_string(db_fptr, &chunk->topic);
	എങ്കിൽ(rc){
		പോകുക error;
	}
	എങ്കിൽ(!chunk->topic){
		rc = MOSQ_ERR_INVAL;
		പോകുക error;
	}
	slen = strlen(chunk->topic);
	എങ്കിൽ(slen > UINT16_MAX){
		rc = MOSQ_ERR_INVAL;
		പോകുക error;
	}
	chunk->F.topic_len = (uint16_t)slen;

	read_e(db_fptr, &chunk->F.qos, വലുപ്പം(uint8_t));
	read_e(db_fptr, &chunk->F.retain, വലുപ്പം(uint8_t));

	read_e(db_fptr, &i32temp, വലുപ്പം(uint32_t));
	chunk->F.payloadlen = ntohl(i32temp);

	എങ്കിൽ(chunk->F.payloadlen){
		എങ്കിൽ(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
			rc = MOSQ_ERR_INVAL;
			പോകുക error;
		}
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		എങ്കിൽ(chunk->payload == ശൂന്യ){
			rc = MOSQ_ERR_NOMEM;
			പോകുക error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	തിരികെ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	തിരികെ rc;
}


പൂർണ്ണസംഖ്യ persist__chunk_retain_read_v234(FILE *db_fptr, ഘടന P_retain *chunk)
{
	dbid_t i64temp;

	എങ്കിൽ(fread(&i64temp, വലുപ്പം(dbid_t), 1, db_fptr) != 1){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		തിരികെ 1;
	}
	chunk->F.store_id = i64temp;

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ persist__chunk_sub_read_v234(FILE *db_fptr, ഘടന P_sub *chunk)
{
	പൂർണ്ണസംഖ്യ rc;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	എങ്കിൽ(rc){
		പോകുക error;
	}

	rc = persist__read_string(db_fptr, &chunk->topic);
	എങ്കിൽ(rc){
		പോകുക error;
	}

	read_e(db_fptr, &chunk->F.qos, വലുപ്പം(uint8_t));

	തിരികെ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(chunk->topic);
	തിരികെ rc;
}

#endif
