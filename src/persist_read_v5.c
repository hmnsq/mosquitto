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
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "persist.h"
#అన్తర్భావ "property_mosq.h"
#అన్తర్భావ "util_mosq.h"


పూర్ణాంకం persist_chunk_శీర్షిక_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *నిడివి)
{
	size_t rlen;
	నిర్మాణం PF_శీర్షిక శీర్షిక;

	rlen = fread(&శీర్షిక, పరిమాణం(నిర్మాణం PF_శీర్షిక), 1, db_fptr);
	ఒకవేళ(rlen != 1){
		తిరిగి 1;
	}

	*chunk = ntohl(శీర్షిక.chunk);
	*నిడివి = ntohl(శీర్షిక.నిడివి);

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం persist__chunk_cfg_read_v56(FILE *db_fptr, నిర్మాణం PF_cfg *chunk)
{
	ఒకవేళ(fread(chunk, పరిమాణం(నిర్మాణం PF_cfg), 1, db_fptr) != 1){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		తిరిగి 1;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం persist__chunk_client_read_v56(FILE *db_fptr, నిర్మాణం P_client *chunk, uint32_t db_సంస్కరణ)
{
	పూర్ణాంకం rc;

	ఒకవేళ(db_సంస్కరణ == 6){
		read_e(db_fptr, &chunk->F, పరిమాణం(నిర్మాణం PF_client));
		chunk->F.username_len = ntohs(chunk->F.username_len);
		chunk->F.listener_పోర్టు = ntohs(chunk->F.listener_పోర్టు);
	}లేకపోతే ఒకవేళ(db_సంస్కరణ == 5){
		read_e(db_fptr, &chunk->F, పరిమాణం(నిర్మాణం PF_client_v5));
	}లేకపోతే{
		తిరిగి 1;
	}

	chunk->F.session_expiry_interval = ntohl(chunk->F.session_expiry_interval);
	chunk->F.చివరి_mid = ntohs(chunk->F.చివరి_mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);


	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	ఒకవేళ(rc){
		తిరిగి 1;
	}లేకపోతే ఒకవేళ(chunk->clientid == ఖాళీ){
		తిరిగి -1;
	}

	ఒకవేళ(chunk->F.username_len > 0){
		rc = persist__read_string_len(db_fptr, &chunk->username, chunk->F.username_len);
		ఒకవేళ(rc || !chunk->username){
			mosquitto_FREE(chunk->clientid);
			తిరిగి 1;
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
error:
	log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	తిరిగి 1;
}


పూర్ణాంకం persist_chunk_client_సందేశం_read_v56(FILE *db_fptr, నిర్మాణం P_client_సందేశం *chunk, uint32_t నిడివి)
{
	mosquitto_property *properties = ఖాళీ, *p;
	నిర్మాణం mosquitto_packet_లోపల prop_packet;
	పూర్ణాంకం rc;

	memset(&prop_packet, 0, పరిమాణం(నిర్మాణం mosquitto_packet_లోపల));

	read_e(db_fptr, &chunk->F, పరిమాణం(నిర్మాణం PF_client_సందేశం));
	chunk->F.mid = ntohs(chunk->F.mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);

	నిడివి -= (uint32_t)(పరిమాణం(నిర్మాణం PF_client_సందేశం) + chunk->F.id_len);
	ఒకవేళ(నిడివి > MQTT_MAX_PAYLOAD){
		వెళ్ళు error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	ఒకవేళ(నిడివి > 0){
		prop_packet.మిగిలిన_నిడివి = నిడివి;
		prop_packet.payload = mosquitto_malloc(నిడివి);
		ఒకవేళ(!prop_packet.payload){
			errno = ENOMEM;
			వెళ్ళు error;
		}

		read_e(db_fptr, prop_packet.payload, నిడివి);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		ఒకవేళ(rc){
			mosquitto_FREE(chunk->clientid);
			తిరిగి rc;
		}

		ఒకవేళ(properties){
			p = properties;
			అయితే(p){
				ఒకవేళ(mosquitto_property_identifier(p) == MQTT_PROP_SUBSCRIPTION_IDENTIFIER){
					chunk->subscription_identifier = mosquitto_property_varint_విలువ(p);
				}
				p = mosquitto_property_తదుపరి(p);
			}
			mosquitto_property_free_all(&properties);
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(prop_packet.payload);
	log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	తిరిగి 1;
}


పూర్ణాంకం persist_chunk_పునాది_సందేశం_read_v56(FILE *db_fptr, నిర్మాణం P_పునాది_సందేశం *chunk, uint32_t నిడివి)
{
	పూర్ణాంకం rc = 0;
	mosquitto_property *properties = ఖాళీ;
	నిర్మాణం mosquitto_packet_లోపల prop_packet;

	memset(&prop_packet, 0, పరిమాణం(నిర్మాణం mosquitto_packet_లోపల));

	read_e(db_fptr, &chunk->F, పరిమాణం(నిర్మాణం PF_పునాది_సందేశం));
	chunk->F.payloadlen = ntohl(chunk->F.payloadlen);
	ఒకవేళ(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
		తిరిగి MOSQ_ERR_INVAL;
	}
	chunk->F.ఆధారం_mid = ntohs(chunk->F.ఆధారం_mid);
	chunk->F.ఆధారం_id_len = ntohs(chunk->F.ఆధారం_id_len);
	chunk->F.ఆధారం_username_len = ntohs(chunk->F.ఆధారం_username_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);
	chunk->F.ఆధారం_పోర్టు = ntohs(chunk->F.ఆధారం_పోర్టు);

	నిడివి -= (uint32_t)(పరిమాణం(నిర్మాణం PF_పునాది_సందేశం) + chunk->F.payloadlen + chunk->F.ఆధారం_id_len + chunk->F.ఆధారం_username_len + chunk->F.topic_len);
	ఒకవేళ(నిడివి > MQTT_MAX_PAYLOAD){
		వెళ్ళు error;
	}

	ఒకవేళ(chunk->F.ఆధారం_id_len){
		rc = persist__read_string_len(db_fptr, &chunk->ఆధారం.id, chunk->F.ఆధారం_id_len);
		ఒకవేళ(rc){
			వెళ్ళు error;
		}
	}
	ఒకవేళ(chunk->F.ఆధారం_username_len){
		rc = persist__read_string_len(db_fptr, &chunk->ఆధారం.username, chunk->F.ఆధారం_username_len);
		ఒకవేళ(rc){
			వెళ్ళు error;
		}
	}
	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	ఒకవేళ(rc){
		వెళ్ళు error;
	}

	ఒకవేళ(chunk->F.payloadlen > 0){
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		ఒకవేళ(chunk->payload == ఖాళీ){
			rc = MOSQ_ERR_NOMEM;
			వెళ్ళు error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	ఒకవేళ(నిడివి > 0){
		prop_packet.మిగిలిన_నిడివి = నిడివి;
		prop_packet.payload = mosquitto_malloc(నిడివి);
		ఒకవేళ(!prop_packet.payload){
			rc = MOSQ_ERR_NOMEM;
			వెళ్ళు error;
		}
		read_e(db_fptr, prop_packet.payload, నిడివి);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		ఒకవేళ(rc){
			rc = MOSQ_ERR_NOMEM;
			వెళ్ళు error;
		}
	}
	chunk->properties = properties;

	తిరిగి MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->ఆధారం.id);
	mosquitto_FREE(chunk->ఆధారం.username);
	mosquitto_FREE(chunk->topic);
	mosquitto_FREE(prop_packet.payload);
	తిరిగి rc;
}


పూర్ణాంకం persist__chunk_retain_read_v56(FILE *db_fptr, నిర్మాణం P_retain *chunk)
{
	ఒకవేళ(fread(&chunk->F, పరిమాణం(నిర్మాణం P_retain), 1, db_fptr) != 1){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		తిరిగి 1;
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం persist__chunk_sub_read_v56(FILE *db_fptr, నిర్మాణం P_sub *chunk)
{
	పూర్ణాంకం rc = MOSQ_ERR_SUCCESS;

	read_e(db_fptr, &chunk->F, పరిమాణం(నిర్మాణం PF_sub));
	chunk->F.identifier = ntohl(chunk->F.identifier);
	chunk->F.id_len = ntohs(chunk->F.id_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	ఒకవేళ(rc){
		వెళ్ళు error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	ఒకవేళ(rc){
		వెళ్ళు error;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	తిరిగి rc;
}

#అయితే_ముగింపు
