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

#અન્તર્ભાવ "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#અન્તર્ભાવ <arpa/inet.h>
#endif
#અન્તર્ભાવ <errno.h>
#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <time.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "persist.h"
#અન્તર્ભાવ "util_mosq.h"


પૂર્ણાંક persist__chunk_header_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	size_t rlen;
	uint16_t i16temp;
	uint32_t i32temp;

	rlen = fread(&i16temp, કદ(uint16_t), 1, db_fptr);
	જો(rlen != 1){
		પાછા 1;
	}

	rlen = fread(&i32temp, કદ(uint32_t), 1, db_fptr);
	જો(rlen != 1){
		પાછા 1;
	}

	*chunk = ntohs(i16temp);
	*length = ntohl(i32temp);

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક persist__chunk_cfg_read_v234(FILE *db_fptr, સંરચના PF_cfg *chunk)
{
	પૂર્ણાંક rc = MOSQ_ERR_UNKNOWN;

	read_e(db_fptr, &chunk->shutdown, કદ(uint8_t)); /* shutdown */
	read_e(db_fptr, &chunk->dbid_size, કદ(uint8_t)); /* sizeof(dbid_t) */
	read_e(db_fptr, &chunk->last_db_id, કદ(dbid_t));

	પાછા MOSQ_ERR_SUCCESS;
error:
	log__printf(ખાલી, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	પાછા rc;
}


પૂર્ણાંક persist__chunk_client_read_v234(FILE *db_fptr, સંરચના P_client *chunk, uint32_t db_version)
{
	uint16_t i16temp;
	પૂર્ણાંક rc;
	time_t temp;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	જો(rc){
		પાછા rc;
	}

	read_e(db_fptr, &i16temp, કદ(uint16_t));
	chunk->F.last_mid = ntohs(i16temp);
	જો(db_version != 2){
		read_e(db_fptr, &temp, કદ(time_t));
	}

	પાછા MOSQ_ERR_SUCCESS;
error:
	log__printf(ખાલી, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	પાછા 1;
}


પૂર્ણાંક persist_chunk_client_સંદેશ_read_v234(FILE *db_fptr, સંરચના P_client_સંદેશ *chunk)
{
	uint16_t i16temp;
	પૂર્ણાંક rc;
	uint8_t retain, dup;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	જો(rc){
		પાછા rc;
	}

	read_e(db_fptr, &chunk->F.store_id, કદ(dbid_t));

	read_e(db_fptr, &i16temp, કદ(uint16_t));
	chunk->F.mid = ntohs(i16temp);

	read_e(db_fptr, &chunk->F.qos, કદ(uint8_t));
	read_e(db_fptr, &retain, કદ(uint8_t));
	read_e(db_fptr, &chunk->F.direction, કદ(uint8_t));
	read_e(db_fptr, &chunk->F.state, કદ(uint8_t));
	read_e(db_fptr, &dup, કદ(uint8_t));

	chunk->F.retain_dup = (uint8_t)((retain&0x0F)<<4 | (dup&0x0F));

	પાછા MOSQ_ERR_SUCCESS;
error:
	log__printf(ખાલી, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	પાછા 1;
}


પૂર્ણાંક persist_chunk_base_સંદેશ_read_v234(FILE *db_fptr, સંરચના P_base_સંદેશ *chunk, uint32_t db_version)
{
	uint32_t i32temp;
	uint16_t i16temp;
	પૂર્ણાંક rc = 0;
	size_t slen;

	read_e(db_fptr, &chunk->F.store_id, કદ(dbid_t));

	rc = persist__read_string(db_fptr, &chunk->source.id);
	જો(rc){
		પાછા rc;
	}

	જો(db_version == 4){
		rc = persist__read_string(db_fptr, &chunk->source.username);
		જો(rc){
			જાઓ error;
		}
		read_e(db_fptr, &i16temp, કદ(uint16_t));
		chunk->F.source_port = ntohs(i16temp);
	}

	read_e(db_fptr, &i16temp, કદ(uint16_t));
	chunk->F.source_mid = ntohs(i16temp);

	/* This is the mid - don't need it */
	read_e(db_fptr, &i16temp, કદ(uint16_t));

	rc = persist__read_string(db_fptr, &chunk->topic);
	જો(rc){
		જાઓ error;
	}
	જો(!chunk->topic){
		rc = MOSQ_ERR_INVAL;
		જાઓ error;
	}
	slen = strlen(chunk->topic);
	જો(slen > UINT16_MAX){
		rc = MOSQ_ERR_INVAL;
		જાઓ error;
	}
	chunk->F.topic_len = (uint16_t)slen;

	read_e(db_fptr, &chunk->F.qos, કદ(uint8_t));
	read_e(db_fptr, &chunk->F.retain, કદ(uint8_t));

	read_e(db_fptr, &i32temp, કદ(uint32_t));
	chunk->F.payloadlen = ntohl(i32temp);

	જો(chunk->F.payloadlen){
		જો(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
			rc = MOSQ_ERR_INVAL;
			જાઓ error;
		}
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		જો(chunk->payload == ખાલી){
			rc = MOSQ_ERR_NOMEM;
			જાઓ error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	પાછા MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	પાછા rc;
}


પૂર્ણાંક persist__chunk_retain_read_v234(FILE *db_fptr, સંરચના P_retain *chunk)
{
	dbid_t i64temp;

	જો(fread(&i64temp, કદ(dbid_t), 1, db_fptr) != 1){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		પાછા 1;
	}
	chunk->F.store_id = i64temp;

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક persist__chunk_sub_read_v234(FILE *db_fptr, સંરચના P_sub *chunk)
{
	પૂર્ણાંક rc;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	જો(rc){
		જાઓ error;
	}

	rc = persist__read_string(db_fptr, &chunk->topic);
	જો(rc){
		જાઓ error;
	}

	read_e(db_fptr, &chunk->F.qos, કદ(uint8_t));

	પાછા MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(chunk->topic);
	પાછા rc;
}

#endif
