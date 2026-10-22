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


पूर्णांक persist__chunk_header_read_v234(खाता *db_fptr, uint32_t *chunk, uint32_t *length)
{
	माप_प्रकार rlen;
	uint16_t i16temp;
	uint32_t i32temp;

	rlen = ख_पढ़ो(&i16temp, आकार(uint16_t), 1, db_fptr);
	जर(rlen != 1){
		परत 1;
	}

	rlen = ख_पढ़ो(&i32temp, आकार(uint32_t), 1, db_fptr);
	जर(rlen != 1){
		परत 1;
	}

	*chunk = ntohs(i16temp);
	*length = ntohl(i32temp);

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक persist__chunk_cfg_read_v234(खाता *db_fptr, संरचना PF_cfg *chunk)
{
	पूर्णांक rc = MOSQ_ERR_UNKNOWN;

	read_e(db_fptr, &chunk->shutdown, आकार(uint8_t)); /* shutdown */
	read_e(db_fptr, &chunk->dbid_size, आकार(uint8_t)); /* sizeof(dbid_t) */
	read_e(db_fptr, &chunk->last_db_id, आकार(dbid_t));

	परत MOSQ_ERR_SUCCESS;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	परत rc;
}


पूर्णांक persist__chunk_client_read_v234(खाता *db_fptr, संरचना P_client *chunk, uint32_t db_version)
{
	uint16_t i16temp;
	पूर्णांक rc;
	समय_प्रकार temp;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	जर(rc){
		परत rc;
	}

	read_e(db_fptr, &i16temp, आकार(uint16_t));
	chunk->F.last_mid = ntohs(i16temp);
	जर(db_version != 2){
		read_e(db_fptr, &temp, आकार(समय_प्रकार));
	}

	परत MOSQ_ERR_SUCCESS;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	mosquitto_FREE(chunk->clientid);
	परत 1;
}


पूर्णांक persist_chunk_client_संदेश_read_v234(खाता *db_fptr, संरचना P_client_संदेश *chunk)
{
	uint16_t i16temp;
	पूर्णांक rc;
	uint8_t retain, dup;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	जर(rc){
		परत rc;
	}

	read_e(db_fptr, &chunk->F.store_id, आकार(dbid_t));

	read_e(db_fptr, &i16temp, आकार(uint16_t));
	chunk->F.mid = ntohs(i16temp);

	read_e(db_fptr, &chunk->F.qos, आकार(uint8_t));
	read_e(db_fptr, &retain, आकार(uint8_t));
	read_e(db_fptr, &chunk->F.direction, आकार(uint8_t));
	read_e(db_fptr, &chunk->F.state, आकार(uint8_t));
	read_e(db_fptr, &dup, आकार(uint8_t));

	chunk->F.retain_dup = (uint8_t)((retain&0x0F)<<4 | (dup&0x0F));

	परत MOSQ_ERR_SUCCESS;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	mosquitto_FREE(chunk->clientid);
	परत 1;
}


पूर्णांक persist_chunk_base_संदेश_read_v234(खाता *db_fptr, संरचना P_base_संदेश *chunk, uint32_t db_version)
{
	uint32_t i32temp;
	uint16_t i16temp;
	पूर्णांक rc = 0;
	माप_प्रकार slen;

	read_e(db_fptr, &chunk->F.store_id, आकार(dbid_t));

	rc = persist__read_string(db_fptr, &chunk->source.id);
	जर(rc){
		परत rc;
	}

	जर(db_version == 4){
		rc = persist__read_string(db_fptr, &chunk->source.username);
		जर(rc){
			जा error;
		}
		read_e(db_fptr, &i16temp, आकार(uint16_t));
		chunk->F.source_port = ntohs(i16temp);
	}

	read_e(db_fptr, &i16temp, आकार(uint16_t));
	chunk->F.source_mid = ntohs(i16temp);

	/* This is the mid - don't need it */
	read_e(db_fptr, &i16temp, आकार(uint16_t));

	rc = persist__read_string(db_fptr, &chunk->topic);
	जर(rc){
		जा error;
	}
	जर(!chunk->topic){
		rc = MOSQ_ERR_INVAL;
		जा error;
	}
	slen = म_माप(chunk->topic);
	जर(slen > UINT16_MAX){
		rc = MOSQ_ERR_INVAL;
		जा error;
	}
	chunk->F.topic_len = (uint16_t)slen;

	read_e(db_fptr, &chunk->F.qos, आकार(uint8_t));
	read_e(db_fptr, &chunk->F.retain, आकार(uint8_t));

	read_e(db_fptr, &i32temp, आकार(uint32_t));
	chunk->F.payloadlen = ntohl(i32temp);

	जर(chunk->F.payloadlen){
		जर(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
			rc = MOSQ_ERR_INVAL;
			जा error;
		}
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		जर(chunk->payload == रिक्त){
			rc = MOSQ_ERR_NOMEM;
			जा error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	परत MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	परत rc;
}


पूर्णांक persist__chunk_retain_read_v234(खाता *db_fptr, संरचना P_retain *chunk)
{
	dbid_t i64temp;

	जर(ख_पढ़ो(&i64temp, आकार(dbid_t), 1, db_fptr) != 1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
		परत 1;
	}
	chunk->F.store_id = i64temp;

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक persist__chunk_sub_read_v234(खाता *db_fptr, संरचना P_sub *chunk)
{
	पूर्णांक rc;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	जर(rc){
		जा error;
	}

	rc = persist__read_string(db_fptr, &chunk->topic);
	जर(rc){
		जा error;
	}

	read_e(db_fptr, &chunk->F.qos, आकार(uint8_t));

	परत MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(chunk->topic);
	परत rc;
}

#endif
