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

#समावेश "config.h"

#अगर_घोषित WITH_PERSISTENCE

#अगर_अघोषित WIN32
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#समावेश <errno.h>
#समावेश <fcntl.h>
#समावेश <stdio.h>
#समावेश <string.h>
#समावेश <sys/stat.h>
#समावेश <time.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "persist.h"
#समावेश "util_mosq.h"


पूर्णांक persist__chunk_header_read_v234(खाता *db_fptr, uint32_t *chunk, uint32_t *लंबाई)
{
	माप_प्रकार rlen;
	uint16_t i16temp;
	uint32_t i32temp;

	rlen = ख_पढ़ो(&i16temp, माप(uint16_t), 1, db_fptr);
	अगर(rlen != 1){
		वापस 1;
	}

	rlen = ख_पढ़ो(&i32temp, माप(uint32_t), 1, db_fptr);
	अगर(rlen != 1){
		वापस 1;
	}

	*chunk = ntohs(i16temp);
	*लंबाई = ntohl(i32temp);

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक persist__chunk_cfg_read_v234(खाता *db_fptr, काष्ठा PF_cfg *chunk)
{
	पूर्णांक rc = MOSQ_ERR_UNKNOWN;

	read_e(db_fptr, &chunk->shutdown, माप(uint8_t)); /* shutdown */
	read_e(db_fptr, &chunk->dbid_आकार, माप(uint8_t)); /* sizeof(dbid_t) */
	read_e(db_fptr, &chunk->last_db_id, माप(dbid_t));

	वापस MOSQ_ERR_SUCCESS;
error:
	log__printf(शून्य, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	वापस rc;
}


पूर्णांक persist__chunk_client_read_v234(खाता *db_fptr, काष्ठा P_client *chunk, uint32_t db_version)
{
	uint16_t i16temp;
	पूर्णांक rc;
	समय_प्रकार अस्थायी;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	अगर(rc){
		वापस rc;
	}

	read_e(db_fptr, &i16temp, माप(uint16_t));
	chunk->F.last_mid = ntohs(i16temp);
	अगर(db_version != 2){
		read_e(db_fptr, &अस्थायी, माप(समय_प्रकार));
	}

	वापस MOSQ_ERR_SUCCESS;
error:
	log__printf(शून्य, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	mosquitto_FREE(chunk->clientid);
	वापस 1;
}


पूर्णांक persist_chunk_client_संदेश_read_v234(खाता *db_fptr, काष्ठा P_client_संदेश *chunk)
{
	uint16_t i16temp;
	पूर्णांक rc;
	uint8_t retain, dup;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	अगर(rc){
		वापस rc;
	}

	read_e(db_fptr, &chunk->F.store_id, माप(dbid_t));

	read_e(db_fptr, &i16temp, माप(uint16_t));
	chunk->F.mid = ntohs(i16temp);

	read_e(db_fptr, &chunk->F.qos, माप(uint8_t));
	read_e(db_fptr, &retain, माप(uint8_t));
	read_e(db_fptr, &chunk->F.direction, माप(uint8_t));
	read_e(db_fptr, &chunk->F.state, माप(uint8_t));
	read_e(db_fptr, &dup, माप(uint8_t));

	chunk->F.retain_dup = (uint8_t)((retain&0x0F)<<4 | (dup&0x0F));

	वापस MOSQ_ERR_SUCCESS;
error:
	log__printf(शून्य, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	mosquitto_FREE(chunk->clientid);
	वापस 1;
}


पूर्णांक persist_chunk_base_संदेश_read_v234(खाता *db_fptr, काष्ठा P_base_संदेश *chunk, uint32_t db_version)
{
	uint32_t i32temp;
	uint16_t i16temp;
	पूर्णांक rc = 0;
	माप_प्रकार slen;

	read_e(db_fptr, &chunk->F.store_id, माप(dbid_t));

	rc = persist__read_string(db_fptr, &chunk->source.id);
	अगर(rc){
		वापस rc;
	}

	अगर(db_version == 4){
		rc = persist__read_string(db_fptr, &chunk->source.username);
		अगर(rc){
			जाओ error;
		}
		read_e(db_fptr, &i16temp, माप(uint16_t));
		chunk->F.source_port = ntohs(i16temp);
	}

	read_e(db_fptr, &i16temp, माप(uint16_t));
	chunk->F.source_mid = ntohs(i16temp);

	/* This is the mid - don't need it */
	read_e(db_fptr, &i16temp, माप(uint16_t));

	rc = persist__read_string(db_fptr, &chunk->topic);
	अगर(rc){
		जाओ error;
	}
	अगर(!chunk->topic){
		rc = MOSQ_ERR_INVAL;
		जाओ error;
	}
	slen = म_माप(chunk->topic);
	अगर(slen > UINT16_MAX){
		rc = MOSQ_ERR_INVAL;
		जाओ error;
	}
	chunk->F.topic_len = (uint16_t)slen;

	read_e(db_fptr, &chunk->F.qos, माप(uint8_t));
	read_e(db_fptr, &chunk->F.retain, माप(uint8_t));

	read_e(db_fptr, &i32temp, माप(uint32_t));
	chunk->F.payloadlen = ntohl(i32temp);

	अगर(chunk->F.payloadlen){
		अगर(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
			rc = MOSQ_ERR_INVAL;
			जाओ error;
		}
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		अगर(chunk->payload == शून्य){
			rc = MOSQ_ERR_NOMEM;
			जाओ error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	वापस MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	वापस rc;
}


पूर्णांक persist__chunk_retain_read_v234(खाता *db_fptr, काष्ठा P_retain *chunk)
{
	dbid_t i64temp;

	अगर(ख_पढ़ो(&i64temp, माप(dbid_t), 1, db_fptr) != 1){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
		वापस 1;
	}
	chunk->F.store_id = i64temp;

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक persist__chunk_sub_read_v234(खाता *db_fptr, काष्ठा P_sub *chunk)
{
	पूर्णांक rc;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	अगर(rc){
		जाओ error;
	}

	rc = persist__read_string(db_fptr, &chunk->topic);
	अगर(rc){
		जाओ error;
	}

	read_e(db_fptr, &chunk->F.qos, माप(uint8_t));

	वापस MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(chunk->topic);
	वापस rc;
}

#पूर्ण_अगर
