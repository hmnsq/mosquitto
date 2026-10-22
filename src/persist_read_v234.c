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

#అన్తర్భావ "config.h"

#నిర్వచితమైతే WITH_PERSISTENCE

#నిర్వచితం_కాకపోతే WIN32
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#అన్తర్భావ <errno.h>
#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <time.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "persist.h"
#అన్తర్భావ "util_mosq.h"


పూర్ణాంకం persist_chunk_శీర్షిక_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *నిడివి)
{
	size_t rlen;
	uint16_t i16temp;
	uint32_t i32temp;

	rlen = fread(&i16temp, పరిమాణం(uint16_t), 1, db_fptr);
	ఒకవేళ(rlen != 1){
		తిరిగి 1;
	}

	rlen = fread(&i32temp, పరిమాణం(uint32_t), 1, db_fptr);
	ఒకవేళ(rlen != 1){
		తిరిగి 1;
	}

	*chunk = ntohs(i16temp);
	*నిడివి = ntohl(i32temp);

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం persist__chunk_cfg_read_v234(FILE *db_fptr, నిర్మాణం PF_cfg *chunk)
{
	పూర్ణాంకం rc = MOSQ_ERR_UNKNOWN;

	read_e(db_fptr, &chunk->shutdown, పరిమాణం(uint8_t)); /* shutdown */
	read_e(db_fptr, &chunk->dbid_కొలత, పరిమాణం(uint8_t)); /* sizeof(dbid_t) */
	read_e(db_fptr, &chunk->చివరి_db_id, పరిమాణం(dbid_t));

	తిరిగి MOSQ_ERR_SUCCESS;
error:
	log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	తిరిగి rc;
}


పూర్ణాంకం persist__chunk_client_read_v234(FILE *db_fptr, నిర్మాణం P_client *chunk, uint32_t db_సంస్కరణ)
{
	uint16_t i16temp;
	పూర్ణాంకం rc;
	time_t తాత్కాలిక;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	read_e(db_fptr, &i16temp, పరిమాణం(uint16_t));
	chunk->F.చివరి_mid = ntohs(i16temp);
	ఒకవేళ(db_సంస్కరణ != 2){
		read_e(db_fptr, &తాత్కాలిక, పరిమాణం(time_t));
	}

	తిరిగి MOSQ_ERR_SUCCESS;
error:
	log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	తిరిగి 1;
}


పూర్ణాంకం persist_chunk_client_సందేశం_read_v234(FILE *db_fptr, నిర్మాణం P_client_సందేశం *chunk)
{
	uint16_t i16temp;
	పూర్ణాంకం rc;
	uint8_t retain, dup;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	read_e(db_fptr, &chunk->F.store_id, పరిమాణం(dbid_t));

	read_e(db_fptr, &i16temp, పరిమాణం(uint16_t));
	chunk->F.mid = ntohs(i16temp);

	read_e(db_fptr, &chunk->F.qos, పరిమాణం(uint8_t));
	read_e(db_fptr, &retain, పరిమాణం(uint8_t));
	read_e(db_fptr, &chunk->F.direction, పరిమాణం(uint8_t));
	read_e(db_fptr, &chunk->F.state, పరిమాణం(uint8_t));
	read_e(db_fptr, &dup, పరిమాణం(uint8_t));

	chunk->F.retain_dup = (uint8_t)((retain&0x0F)<<4 | (dup&0x0F));

	తిరిగి MOSQ_ERR_SUCCESS;
error:
	log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(chunk->clientid);
	తిరిగి 1;
}


పూర్ణాంకం persist_chunk_పునాది_సందేశం_read_v234(FILE *db_fptr, నిర్మాణం P_పునాది_సందేశం *chunk, uint32_t db_సంస్కరణ)
{
	uint32_t i32temp;
	uint16_t i16temp;
	పూర్ణాంకం rc = 0;
	size_t slen;

	read_e(db_fptr, &chunk->F.store_id, పరిమాణం(dbid_t));

	rc = persist__read_string(db_fptr, &chunk->ఆధారం.id);
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	ఒకవేళ(db_సంస్కరణ == 4){
		rc = persist__read_string(db_fptr, &chunk->ఆధారం.username);
		ఒకవేళ(rc){
			వెళ్ళు error;
		}
		read_e(db_fptr, &i16temp, పరిమాణం(uint16_t));
		chunk->F.ఆధారం_పోర్టు = ntohs(i16temp);
	}

	read_e(db_fptr, &i16temp, పరిమాణం(uint16_t));
	chunk->F.ఆధారం_mid = ntohs(i16temp);

	/* This is the mid - don't need it */
	read_e(db_fptr, &i16temp, పరిమాణం(uint16_t));

	rc = persist__read_string(db_fptr, &chunk->topic);
	ఒకవేళ(rc){
		వెళ్ళు error;
	}
	ఒకవేళ(!chunk->topic){
		rc = MOSQ_ERR_INVAL;
		వెళ్ళు error;
	}
	slen = అక్షరశ్రేణి_పొడవు(chunk->topic);
	ఒకవేళ(slen > UINT16_MAX){
		rc = MOSQ_ERR_INVAL;
		వెళ్ళు error;
	}
	chunk->F.topic_len = (uint16_t)slen;

	read_e(db_fptr, &chunk->F.qos, పరిమాణం(uint8_t));
	read_e(db_fptr, &chunk->F.retain, పరిమాణం(uint8_t));

	read_e(db_fptr, &i32temp, పరిమాణం(uint32_t));
	chunk->F.payloadlen = ntohl(i32temp);

	ఒకవేళ(chunk->F.payloadlen){
		ఒకవేళ(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
			rc = MOSQ_ERR_INVAL;
			వెళ్ళు error;
		}
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		ఒకవేళ(chunk->payload == ఖాళీ){
			rc = MOSQ_ERR_NOMEM;
			వెళ్ళు error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->ఆధారం.id);
	mosquitto_FREE(chunk->ఆధారం.username);
	mosquitto_FREE(chunk->topic);
	తిరిగి rc;
}


పూర్ణాంకం persist__chunk_retain_read_v234(FILE *db_fptr, నిర్మాణం P_retain *chunk)
{
	dbid_t i64temp;

	ఒకవేళ(fread(&i64temp, పరిమాణం(dbid_t), 1, db_fptr) != 1){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		తిరిగి 1;
	}
	chunk->F.store_id = i64temp;

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం persist__chunk_sub_read_v234(FILE *db_fptr, నిర్మాణం P_sub *chunk)
{
	పూర్ణాంకం rc;

	rc = persist__read_string(db_fptr, &chunk->clientid);
	ఒకవేళ(rc){
		వెళ్ళు error;
	}

	rc = persist__read_string(db_fptr, &chunk->topic);
	ఒకవేళ(rc){
		వెళ్ళు error;
	}

	read_e(db_fptr, &chunk->F.qos, పరిమాణం(uint8_t));

	తిరిగి MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(chunk->topic);
	తిరిగి rc;
}

#అయితే_ముగింపు
