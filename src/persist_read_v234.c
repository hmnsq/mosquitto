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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ଅନ୍ତର୍ଭାଵ <errno.h>
#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <time.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "persist.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__chunk_header_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	size_t rlen;
	uint16_t i16temp;
	uint32_t i32temp;

	rlen = fread(&i16temp, ଆକାର(uint16_t), 1, db_fptr);
	ଯଦି(rlen != 1){
		ଫେରନ୍ତୁ 1;
	}

	rlen = fread(&i32temp, ଆକାର(uint32_t), 1, db_fptr);
	ଯଦି(rlen != 1){
		ଫେରନ୍ତୁ 1;
	}

	*chunk = ntohs(i16temp);
	*length = ntohl(i32temp);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__chunk_cfg_read_v234(FILE *db_fptr, ସଂରଚନା PF_cfg *chunk)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_UNKNOWN;

	read_e(db_fptr, &chunk->shutdown, ଆକାର(uint8_t)); /* shutdown */
	read_e(db_fptr, &chunk->dbid_size, ଆକାର(uint8_t)); /* sizeof(dbid_t) */
	read_e(db_fptr, &chunk->last_db_id, ଆକାର(dbid_t));

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
error:
	log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ଫେରନ୍ତୁ rc;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__chunk_client_read_v234(FILE *db_fptr, ସଂରଚନା P_client *chunk, uint32_t db_version)
{
	uint16_t i16temp;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	time_t temp;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	read_e(db_fptr, &i16temp, ଆକାର(uint16_t));
	chunk->F.last_mid = ntohs(i16temp);
	ଯଦି(db_version != 2){
		read_e(db_fptr, &temp, ଆକାର(time_t));
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
error:
	log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	ଫେରନ୍ତୁ 1;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist_chunk_client_ସନ୍ଦେଶ_read_v234(FILE *db_fptr, ସଂରଚନା P_client_ସନ୍ଦେଶ *chunk)
{
	uint16_t i16temp;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	uint8_t retain, dup;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	read_e(db_fptr, &chunk->F.store_id, ଆକାର(dbid_t));

	read_e(db_fptr, &i16temp, ଆକାର(uint16_t));
	chunk->F.mid = ntohs(i16temp);

	read_e(db_fptr, &chunk->F.qos, ଆକାର(uint8_t));
	read_e(db_fptr, &retain, ଆକାର(uint8_t));
	read_e(db_fptr, &chunk->F.direction, ଆକାର(uint8_t));
	read_e(db_fptr, &chunk->F.state, ଆକାର(uint8_t));
	read_e(db_fptr, &dup, ଆକାର(uint8_t));

	chunk->F.retain_dup = (uint8_t)((retain&0x0F)<<4 | (dup&0x0F));

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
error:
	log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	ଫେରନ୍ତୁ 1;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist_chunk_base_ସନ୍ଦେଶ_read_v234(FILE *db_fptr, ସଂରଚନା P_base_ସନ୍ଦେଶ *chunk, uint32_t db_version)
{
	uint32_t i32temp;
	uint16_t i16temp;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
	size_t slen;

	read_e(db_fptr, &chunk->F.store_id, ଆକାର(dbid_t));

	rc = persist__read_string(db_fptr, &chunk->source.id);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	ଯଦି(db_version == 4){
		rc = persist__read_string(db_fptr, &chunk->source.username);
		ଯଦି(rc){
			ଯାଅ error;
		}
		read_e(db_fptr, &i16temp, ଆକାର(uint16_t));
		chunk->F.source_port = ntohs(i16temp);
	}

	read_e(db_fptr, &i16temp, ଆକାର(uint16_t));
	chunk->F.source_mid = ntohs(i16temp);

	/* This is the mid - don't need it */
	read_e(db_fptr, &i16temp, ଆକାର(uint16_t));

	rc = persist__read_string(db_fptr, &chunk->topic);
	ଯଦି(rc){
		ଯାଅ error;
	}
	ଯଦି(!chunk->topic){
		rc = MOSQ_ERR_INVAL;
		ଯାଅ error;
	}
	slen = strlen(chunk->topic);
	ଯଦି(slen > UINT16_MAX){
		rc = MOSQ_ERR_INVAL;
		ଯାଅ error;
	}
	chunk->F.topic_len = (uint16_t)slen;

	read_e(db_fptr, &chunk->F.qos, ଆକାର(uint8_t));
	read_e(db_fptr, &chunk->F.retain, ଆକାର(uint8_t));

	read_e(db_fptr, &i32temp, ଆକାର(uint32_t));
	chunk->F.payloadlen = ntohl(i32temp);

	ଯଦି(chunk->F.payloadlen){
		ଯଦି(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
			rc = MOSQ_ERR_INVAL;
			ଯାଅ error;
		}
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		ଯଦି(chunk->payload == ଖାଲି){
			rc = MOSQ_ERR_NOMEM;
			ଯାଅ error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	ଫେରନ୍ତୁ rc;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__chunk_retain_read_v234(FILE *db_fptr, ସଂରଚନା P_retain *chunk)
{
	dbid_t i64temp;

	ଯଦି(fread(&i64temp, ଆକାର(dbid_t), 1, db_fptr) != 1){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		ଫେରନ୍ତୁ 1;
	}
	chunk->F.store_id = i64temp;

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__chunk_sub_read_v234(FILE *db_fptr, ସଂରଚନା P_sub *chunk)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ଯଦି(rc){
		ଯାଅ error;
	}

	rc = persist__read_string(db_fptr, &chunk->topic);
	ଯଦି(rc){
		ଯାଅ error;
	}

	read_e(db_fptr, &chunk->F.qos, ଆକାର(uint8_t));

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(chunk->topic);
	ଫେରନ୍ତୁ rc;
}

#endif
