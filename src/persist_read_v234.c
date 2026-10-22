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

#অন্তর্ভাৱ "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#অন্তর্ভাৱ <arpa/inet.h>
#endif
#অন্তর্ভাৱ <errno.h>
#অন্তর্ভাৱ <fcntl.h>
#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <sys/stat.h>
#অন্তর্ভাৱ <time.h>

#অন্তর্ভাৱ "mosquitto_broker_internal.h"
#অন্তর্ভাৱ "persist.h"
#অন্তর্ভাৱ "util_mosq.h"


পূর্ণাঙ্ক persist__chunk_header_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	size_t rlen;
	uint16_t i16temp;
	uint32_t i32temp;

	rlen = fread(&i16temp, পরিমাণ(uint16_t), 1, db_fptr);
	যদি(rlen != 1){
		ঘূৰাই 1;
	}

	rlen = fread(&i32temp, পরিমাণ(uint32_t), 1, db_fptr);
	যদি(rlen != 1){
		ঘূৰাই 1;
	}

	*chunk = ntohs(i16temp);
	*length = ntohl(i32temp);

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক persist__chunk_cfg_read_v234(FILE *db_fptr, সংরচনা PF_cfg *chunk)
{
	পূর্ণাঙ্ক rc = MOSQ_ERR_UNKNOWN;

	read_e(db_fptr, &chunk->shutdown, পরিমাণ(uint8_t)); /* shutdown */
	read_e(db_fptr, &chunk->dbid_size, পরিমাণ(uint8_t)); /* sizeof(dbid_t) */
	read_e(db_fptr, &chunk->last_db_id, পরিমাণ(dbid_t));

	ঘূৰাই MOSQ_ERR_SUCCESS;
error:
	log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ঘূৰাই rc;
}


পূর্ণাঙ্ক persist__chunk_client_read_v234(FILE *db_fptr, সংরচনা P_client *chunk, uint32_t db_version)
{
	uint16_t i16temp;
	পূর্ণাঙ্ক rc;
	time_t temp;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	যদি(rc){
		ঘূৰাই rc;
	}

	read_e(db_fptr, &i16temp, পরিমাণ(uint16_t));
	chunk->F.last_mid = ntohs(i16temp);
	যদি(db_version != 2){
		read_e(db_fptr, &temp, পরিমাণ(time_t));
	}

	ঘূৰাই MOSQ_ERR_SUCCESS;
error:
	log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	ঘূৰাই 1;
}


পূর্ণাঙ্ক persist_chunk_client_বাৰ্তা_read_v234(FILE *db_fptr, সংরচনা P_client_বাৰ্তা *chunk)
{
	uint16_t i16temp;
	পূর্ণাঙ্ক rc;
	uint8_t retain, dup;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	যদি(rc){
		ঘূৰাই rc;
	}

	read_e(db_fptr, &chunk->F.store_id, পরিমাণ(dbid_t));

	read_e(db_fptr, &i16temp, পরিমাণ(uint16_t));
	chunk->F.mid = ntohs(i16temp);

	read_e(db_fptr, &chunk->F.qos, পরিমাণ(uint8_t));
	read_e(db_fptr, &retain, পরিমাণ(uint8_t));
	read_e(db_fptr, &chunk->F.direction, পরিমাণ(uint8_t));
	read_e(db_fptr, &chunk->F.state, পরিমাণ(uint8_t));
	read_e(db_fptr, &dup, পরিমাণ(uint8_t));

	chunk->F.retain_dup = (uint8_t)((retain&0x0F)<<4 | (dup&0x0F));

	ঘূৰাই MOSQ_ERR_SUCCESS;
error:
	log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	ঘূৰাই 1;
}


পূর্ণাঙ্ক persist_chunk_base_বাৰ্তা_read_v234(FILE *db_fptr, সংরচনা P_base_বাৰ্তা *chunk, uint32_t db_version)
{
	uint32_t i32temp;
	uint16_t i16temp;
	পূর্ণাঙ্ক rc = 0;
	size_t slen;

	read_e(db_fptr, &chunk->F.store_id, পরিমাণ(dbid_t));

	rc = persist__read_string(db_fptr, &chunk->source.id);
	যদি(rc){
		ঘূৰাই rc;
	}

	যদি(db_version == 4){
		rc = persist__read_string(db_fptr, &chunk->source.username);
		যদি(rc){
			গচ্ছ error;
		}
		read_e(db_fptr, &i16temp, পরিমাণ(uint16_t));
		chunk->F.source_port = ntohs(i16temp);
	}

	read_e(db_fptr, &i16temp, পরিমাণ(uint16_t));
	chunk->F.source_mid = ntohs(i16temp);

	/* This is the mid - don't need it */
	read_e(db_fptr, &i16temp, পরিমাণ(uint16_t));

	rc = persist__read_string(db_fptr, &chunk->topic);
	যদি(rc){
		গচ্ছ error;
	}
	যদি(!chunk->topic){
		rc = MOSQ_ERR_INVAL;
		গচ্ছ error;
	}
	slen = strlen(chunk->topic);
	যদি(slen > UINT16_MAX){
		rc = MOSQ_ERR_INVAL;
		গচ্ছ error;
	}
	chunk->F.topic_len = (uint16_t)slen;

	read_e(db_fptr, &chunk->F.qos, পরিমাণ(uint8_t));
	read_e(db_fptr, &chunk->F.retain, পরিমাণ(uint8_t));

	read_e(db_fptr, &i32temp, পরিমাণ(uint32_t));
	chunk->F.payloadlen = ntohl(i32temp);

	যদি(chunk->F.payloadlen){
		যদি(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
			rc = MOSQ_ERR_INVAL;
			গচ্ছ error;
		}
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		যদি(chunk->payload == রিক্ত){
			rc = MOSQ_ERR_NOMEM;
			গচ্ছ error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	ঘূৰাই MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	ঘূৰাই rc;
}


পূর্ণাঙ্ক persist__chunk_retain_read_v234(FILE *db_fptr, সংরচনা P_retain *chunk)
{
	dbid_t i64temp;

	যদি(fread(&i64temp, পরিমাণ(dbid_t), 1, db_fptr) != 1){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		ঘূৰাই 1;
	}
	chunk->F.store_id = i64temp;

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক persist__chunk_sub_read_v234(FILE *db_fptr, সংরচনা P_sub *chunk)
{
	পূর্ণাঙ্ক rc;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	যদি(rc){
		গচ্ছ error;
	}

	rc = persist__read_string(db_fptr, &chunk->topic);
	যদি(rc){
		গচ্ছ error;
	}

	read_e(db_fptr, &chunk->F.qos, পরিমাণ(uint8_t));

	ঘূৰাই MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(chunk->topic);
	ঘূৰাই rc;
}

#endif
