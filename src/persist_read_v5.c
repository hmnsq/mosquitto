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
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "persist.h"
#अन्तर्भाव "property_mosq.h"
#अन्तर्भाव "util_mosq.h"


पूर्णाङ्क persist__chunk_header_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	size_t rlen;
	संरचना PF_header header;

	rlen = fread(&header, परिमाण(संरचना PF_header), 1, db_fptr);
	सचे(rlen != 1){
		पटिनिवत्त 1;
	}

	*chunk = ntohl(header.chunk);
	*length = ntohl(header.length);

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क persist__chunk_cfg_read_v56(FILE *db_fptr, संरचना PF_cfg *chunk)
{
	सचे(fread(chunk, परिमाण(संरचना PF_cfg), 1, db_fptr) != 1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		पटिनिवत्त 1;
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क persist__chunk_client_read_v56(FILE *db_fptr, संरचना P_client *chunk, uint32_t db_version)
{
	पूर्णाङ्क rc;

	सचे(db_version == 6){
		read_e(db_fptr, &chunk->F, परिमाण(संरचना PF_client));
		chunk->F.username_len = ntohs(chunk->F.username_len);
		chunk->F.listener_port = ntohs(chunk->F.listener_port);
	}अञ्ञथा सचे(db_version == 5){
		read_e(db_fptr, &chunk->F, परिमाण(संरचना PF_client_v5));
	}अञ्ञथा{
		पटिनिवत्त 1;
	}

	chunk->F.session_expiry_interval = ntohl(chunk->F.session_expiry_interval);
	chunk->F.last_mid = ntohs(chunk->F.last_mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);


	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	सचे(rc){
		पटिनिवत्त 1;
	}अञ्ञथा सचे(chunk->clientid == रिक्त){
		पटिनिवत्त -1;
	}

	सचे(chunk->F.username_len > 0){
		rc = persist__read_string_len(db_fptr, &chunk->username, chunk->F.username_len);
		सचे(rc || !chunk->username){
			mosquitto_FREE(chunk->clientid);
			पटिनिवत्त 1;
		}
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	पटिनिवत्त 1;
}


पूर्णाङ्क persist_chunk_client_सन्देशः_read_v56(FILE *db_fptr, संरचना P_client_सन्देशः *chunk, uint32_t length)
{
	mosquitto_property *properties = रिक्त, *p;
	संरचना mosquitto__packet_in prop_packet;
	पूर्णाङ्क rc;

	memset(&prop_packet, 0, परिमाण(संरचना mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, परिमाण(संरचना PF_client_सन्देशः));
	chunk->F.mid = ntohs(chunk->F.mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);

	length -= (uint32_t)(परिमाण(संरचना PF_client_सन्देशः) + chunk->F.id_len);
	सचे(length > MQTT_MAX_PAYLOAD){
		गच्छ error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	सचे(rc){
		पटिनिवत्त rc;
	}

	सचे(length > 0){
		prop_packet.remaining_length = length;
		prop_packet.payload = mosquitto_malloc(length);
		सचे(!prop_packet.payload){
			errno = ENOMEM;
			गच्छ error;
		}

		read_e(db_fptr, prop_packet.payload, length);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		सचे(rc){
			mosquitto_FREE(chunk->clientid);
			पटिनिवत्त rc;
		}

		सचे(properties){
			p = properties;
			याव(p){
				सचे(mosquitto_property_identifier(p) == MQTT_PROP_SUBSCRIPTION_IDENTIFIER){
					chunk->subscription_identifier = mosquitto_property_varint_मानम्(p);
				}
				p = mosquitto_property_next(p);
			}
			mosquitto_property_free_all(&properties);
		}
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(prop_packet.payload);
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	पटिनिवत्त 1;
}


पूर्णाङ्क persist_chunk_base_सन्देशः_read_v56(FILE *db_fptr, संरचना P_base_सन्देशः *chunk, uint32_t length)
{
	पूर्णाङ्क rc = 0;
	mosquitto_property *properties = रिक्त;
	संरचना mosquitto__packet_in prop_packet;

	memset(&prop_packet, 0, परिमाण(संरचना mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, परिमाण(संरचना PF_base_सन्देशः));
	chunk->F.payloadlen = ntohl(chunk->F.payloadlen);
	सचे(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}
	chunk->F.source_mid = ntohs(chunk->F.source_mid);
	chunk->F.source_id_len = ntohs(chunk->F.source_id_len);
	chunk->F.source_username_len = ntohs(chunk->F.source_username_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);
	chunk->F.source_port = ntohs(chunk->F.source_port);

	length -= (uint32_t)(परिमाण(संरचना PF_base_सन्देशः) + chunk->F.payloadlen + chunk->F.source_id_len + chunk->F.source_username_len + chunk->F.topic_len);
	सचे(length > MQTT_MAX_PAYLOAD){
		गच्छ error;
	}

	सचे(chunk->F.source_id_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.id, chunk->F.source_id_len);
		सचे(rc){
			गच्छ error;
		}
	}
	सचे(chunk->F.source_username_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.username, chunk->F.source_username_len);
		सचे(rc){
			गच्छ error;
		}
	}
	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	सचे(rc){
		गच्छ error;
	}

	सचे(chunk->F.payloadlen > 0){
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		सचे(chunk->payload == रिक्त){
			rc = MOSQ_ERR_NOMEM;
			गच्छ error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	सचे(length > 0){
		prop_packet.remaining_length = length;
		prop_packet.payload = mosquitto_malloc(length);
		सचे(!prop_packet.payload){
			rc = MOSQ_ERR_NOMEM;
			गच्छ error;
		}
		read_e(db_fptr, prop_packet.payload, length);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		सचे(rc){
			rc = MOSQ_ERR_NOMEM;
			गच्छ error;
		}
	}
	chunk->properties = properties;

	पटिनिवत्त MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	mosquitto_FREE(prop_packet.payload);
	पटिनिवत्त rc;
}


पूर्णाङ्क persist__chunk_retain_read_v56(FILE *db_fptr, संरचना P_retain *chunk)
{
	सचे(fread(&chunk->F, परिमाण(संरचना P_retain), 1, db_fptr) != 1){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		पटिनिवत्त 1;
	}
	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क persist__chunk_sub_read_v56(FILE *db_fptr, संरचना P_sub *chunk)
{
	पूर्णाङ्क rc = MOSQ_ERR_SUCCESS;

	read_e(db_fptr, &chunk->F, परिमाण(संरचना PF_sub));
	chunk->F.identifier = ntohl(chunk->F.identifier);
	chunk->F.id_len = ntohs(chunk->F.id_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	सचे(rc){
		गच्छ error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	सचे(rc){
		गच्छ error;
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	पटिनिवत्त rc;
}

#endif
