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

#ಅನ್ತರ್ಭಾವ "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif
#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <time.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "persist.h"
#ಅನ್ತರ್ಭಾವ "property_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"


ಪೂರ್ಣಾಙ್ಕ persist__chunk_header_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	size_t rlen;
	ಸಂರಚನಾ PF_header header;

	rlen = fread(&header, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_header), 1, db_fptr);
	ಯದಿ(rlen != 1){
		ಪ್ರತ್ಯಾಗಮ 1;
	}

	*chunk = ntohl(header.chunk);
	*length = ntohl(header.length);

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ persist__chunk_cfg_read_v56(FILE *db_fptr, ಸಂರಚನಾ PF_cfg *chunk)
{
	ಯದಿ(fread(chunk, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_cfg), 1, db_fptr) != 1){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		ಪ್ರತ್ಯಾಗಮ 1;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ persist__chunk_client_read_v56(FILE *db_fptr, ಸಂರಚನಾ P_client *chunk, uint32_t db_version)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(db_version == 6){
		read_e(db_fptr, &chunk->F, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_client));
		chunk->F.username_len = ntohs(chunk->F.username_len);
		chunk->F.listener_port = ntohs(chunk->F.listener_port);
	}ಅನ್ಯಥಾ ಯದಿ(db_version == 5){
		read_e(db_fptr, &chunk->F, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_client_v5));
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ 1;
	}

	chunk->F.session_expiry_interval = ntohl(chunk->F.session_expiry_interval);
	chunk->F.last_mid = ntohs(chunk->F.last_mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);


	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ 1;
	}ಅನ್ಯಥಾ ಯದಿ(chunk->clientid == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಯದಿ(chunk->F.username_len > 0){
		rc = persist__read_string_len(db_fptr, &chunk->username, chunk->F.username_len);
		ಯದಿ(rc || !chunk->username){
			mosquitto_FREE(chunk->clientid);
			ಪ್ರತ್ಯಾಗಮ 1;
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
error:
	log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ಪ್ರತ್ಯಾಗಮ 1;
}


ಪೂರ್ಣಾಙ್ಕ persist_chunk_client_ಸಂದೇಶ_read_v56(FILE *db_fptr, ಸಂರಚನಾ P_client_ಸಂದೇಶ *chunk, uint32_t length)
{
	mosquitto_property *properties = ರಿಕ್ತ, *p;
	ಸಂರಚನಾ mosquitto__packet_in prop_packet;
	ಪೂರ್ಣಾಙ್ಕ rc;

	memset(&prop_packet, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_client_ಸಂದೇಶ));
	chunk->F.mid = ntohs(chunk->F.mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);

	length -= (uint32_t)(ಪರಿಮಾಣ(ಸಂರಚನಾ PF_client_ಸಂದೇಶ) + chunk->F.id_len);
	ಯದಿ(length > MQTT_MAX_PAYLOAD){
		ಗಚ್ಛ error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಯದಿ(length > 0){
		prop_packet.remaining_length = length;
		prop_packet.payload = mosquitto_malloc(length);
		ಯದಿ(!prop_packet.payload){
			errno = ENOMEM;
			ಗಚ್ಛ error;
		}

		read_e(db_fptr, prop_packet.payload, length);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		ಯದಿ(rc){
			mosquitto_FREE(chunk->clientid);
			ಪ್ರತ್ಯಾಗಮ rc;
		}

		ಯದಿ(properties){
			p = properties;
			ಯಾವತ್(p){
				ಯದಿ(mosquitto_property_identifier(p) == MQTT_PROP_SUBSCRIPTION_IDENTIFIER){
					chunk->subscription_identifier = mosquitto_property_varint_ಮೌಲ್ಯ(p);
				}
				p = mosquitto_property_next(p);
			}
			mosquitto_property_free_all(&properties);
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(prop_packet.payload);
	log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ಪ್ರತ್ಯಾಗಮ 1;
}


ಪೂರ್ಣಾಙ್ಕ persist_chunk_base_ಸಂದೇಶ_read_v56(FILE *db_fptr, ಸಂರಚನಾ P_base_ಸಂದೇಶ *chunk, uint32_t length)
{
	ಪೂರ್ಣಾಙ್ಕ rc = 0;
	mosquitto_property *properties = ರಿಕ್ತ;
	ಸಂರಚನಾ mosquitto__packet_in prop_packet;

	memset(&prop_packet, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_base_ಸಂದೇಶ));
	chunk->F.payloadlen = ntohl(chunk->F.payloadlen);
	ಯದಿ(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	chunk->F.source_mid = ntohs(chunk->F.source_mid);
	chunk->F.source_id_len = ntohs(chunk->F.source_id_len);
	chunk->F.source_username_len = ntohs(chunk->F.source_username_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);
	chunk->F.source_port = ntohs(chunk->F.source_port);

	length -= (uint32_t)(ಪರಿಮಾಣ(ಸಂರಚನಾ PF_base_ಸಂದೇಶ) + chunk->F.payloadlen + chunk->F.source_id_len + chunk->F.source_username_len + chunk->F.topic_len);
	ಯದಿ(length > MQTT_MAX_PAYLOAD){
		ಗಚ್ಛ error;
	}

	ಯದಿ(chunk->F.source_id_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.id, chunk->F.source_id_len);
		ಯದಿ(rc){
			ಗಚ್ಛ error;
		}
	}
	ಯದಿ(chunk->F.source_username_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.username, chunk->F.source_username_len);
		ಯದಿ(rc){
			ಗಚ್ಛ error;
		}
	}
	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	ಯದಿ(rc){
		ಗಚ್ಛ error;
	}

	ಯದಿ(chunk->F.payloadlen > 0){
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		ಯದಿ(chunk->payload == ರಿಕ್ತ){
			rc = MOSQ_ERR_NOMEM;
			ಗಚ್ಛ error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	ಯದಿ(length > 0){
		prop_packet.remaining_length = length;
		prop_packet.payload = mosquitto_malloc(length);
		ಯದಿ(!prop_packet.payload){
			rc = MOSQ_ERR_NOMEM;
			ಗಚ್ಛ error;
		}
		read_e(db_fptr, prop_packet.payload, length);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		ಯದಿ(rc){
			rc = MOSQ_ERR_NOMEM;
			ಗಚ್ಛ error;
		}
	}
	chunk->properties = properties;

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	mosquitto_FREE(prop_packet.payload);
	ಪ್ರತ್ಯಾಗಮ rc;
}


ಪೂರ್ಣಾಙ್ಕ persist__chunk_retain_read_v56(FILE *db_fptr, ಸಂರಚನಾ P_retain *chunk)
{
	ಯದಿ(fread(&chunk->F, ಪರಿಮಾಣ(ಸಂರಚನಾ P_retain), 1, db_fptr) != 1){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		ಪ್ರತ್ಯಾಗಮ 1;
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ persist__chunk_sub_read_v56(FILE *db_fptr, ಸಂರಚನಾ P_sub *chunk)
{
	ಪೂರ್ಣಾಙ್ಕ rc = MOSQ_ERR_SUCCESS;

	read_e(db_fptr, &chunk->F, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_sub));
	chunk->F.identifier = ntohl(chunk->F.identifier);
	chunk->F.id_len = ntohs(chunk->F.id_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	ಯದಿ(rc){
		ಗಚ್ಛ error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	ಯದಿ(rc){
		ಗಚ್ಛ error;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	ಪ್ರತ್ಯಾಗಮ rc;
}

#endif
