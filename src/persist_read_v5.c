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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_PERSISTENCE

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
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


ಪೂರ್ಣಾಂಕ persist_chunk_ಶೀರ್ಷಿಕೆ_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *ಉದ್ದಳತೆ)
{
	size_t rlen;
	ರಚನೆ PF_ಶೀರ್ಷಿಕೆ ಶೀರ್ಷಿಕೆ;

	rlen = fread(&ಶೀರ್ಷಿಕೆ, ಗಾತ್ರ(ರಚನೆ PF_ಶೀರ್ಷಿಕೆ), 1, db_fptr);
	ಆದರೆ(rlen != 1){
		ಹಿಂತಿರುಗಿ 1;
	}

	*chunk = ntohl(ಶೀರ್ಷಿಕೆ.chunk);
	*ಉದ್ದಳತೆ = ntohl(ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ);

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ persist__chunk_cfg_read_v56(FILE *db_fptr, ರಚನೆ PF_cfg *chunk)
{
	ಆದರೆ(fread(chunk, ಗಾತ್ರ(ರಚನೆ PF_cfg), 1, db_fptr) != 1){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		ಹಿಂತಿರುಗಿ 1;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ persist__chunk_client_read_v56(FILE *db_fptr, ರಚನೆ P_client *chunk, uint32_t db_ಆವೃತ್ತಿ)
{
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(db_ಆವೃತ್ತಿ == 6){
		read_e(db_fptr, &chunk->F, ಗಾತ್ರ(ರಚನೆ PF_client));
		chunk->F.username_len = ntohs(chunk->F.username_len);
		chunk->F.listener_ಪೋರ್ಟ್ = ntohs(chunk->F.listener_ಪೋರ್ಟ್);
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(db_ಆವೃತ್ತಿ == 5){
		read_e(db_fptr, &chunk->F, ಗಾತ್ರ(ರಚನೆ PF_client_v5));
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ 1;
	}

	chunk->F.session_expiry_interval = ntohl(chunk->F.session_expiry_interval);
	chunk->F.ಕೊನೆಯ_mid = ntohs(chunk->F.ಕೊನೆಯ_mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);


	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ 1;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(chunk->clientid == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ(chunk->F.username_len > 0){
		rc = persist__read_string_len(db_fptr, &chunk->username, chunk->F.username_len);
		ಆದರೆ(rc || !chunk->username){
			mosquitto_FREE(chunk->clientid);
			ಹಿಂತಿರುಗಿ 1;
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
error:
	log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ಹಿಂತಿರುಗಿ 1;
}


ಪೂರ್ಣಾಂಕ persist_chunk_client_ಸಂದೇಶ_read_v56(FILE *db_fptr, ರಚನೆ P_client_ಸಂದೇಶ *chunk, uint32_t ಉದ್ದಳತೆ)
{
	mosquitto_property *properties = ಖಾಲಿ, *p;
	ರಚನೆ mosquitto_packet_ಒಳ prop_packet;
	ಪೂರ್ಣಾಂಕ rc;

	memset(&prop_packet, 0, ಗಾತ್ರ(ರಚನೆ mosquitto_packet_ಒಳ));

	read_e(db_fptr, &chunk->F, ಗಾತ್ರ(ರಚನೆ PF_client_ಸಂದೇಶ));
	chunk->F.mid = ntohs(chunk->F.mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);

	ಉದ್ದಳತೆ -= (uint32_t)(ಗಾತ್ರ(ರಚನೆ PF_client_ಸಂದೇಶ) + chunk->F.id_len);
	ಆದರೆ(ಉದ್ದಳತೆ > MQTT_MAX_PAYLOAD){
		ಹೋಗು error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}

	ಆದರೆ(ಉದ್ದಳತೆ > 0){
		prop_packet.ಉಳಿದ_ಉದ್ದಳತೆ = ಉದ್ದಳತೆ;
		prop_packet.payload = mosquitto_malloc(ಉದ್ದಳತೆ);
		ಆದರೆ(!prop_packet.payload){
			errno = ENOMEM;
			ಹೋಗು error;
		}

		read_e(db_fptr, prop_packet.payload, ಉದ್ದಳತೆ);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		ಆದರೆ(rc){
			mosquitto_FREE(chunk->clientid);
			ಹಿಂತಿರುಗಿ rc;
		}

		ಆದರೆ(properties){
			p = properties;
			ಆಗ(p){
				ಆದರೆ(mosquitto_property_identifier(p) == MQTT_PROP_SUBSCRIPTION_IDENTIFIER){
					chunk->subscription_identifier = mosquitto_property_varint_ಮೌಲ್ಯ(p);
				}
				p = mosquitto_property_ಮುಂದಿನ(p);
			}
			mosquitto_property_free_all(&properties);
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(prop_packet.payload);
	log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ಹಿಂತಿರುಗಿ 1;
}


ಪೂರ್ಣಾಂಕ persist_chunk_ತಳ_ಸಂದೇಶ_read_v56(FILE *db_fptr, ರಚನೆ P_ತಳ_ಸಂದೇಶ *chunk, uint32_t ಉದ್ದಳತೆ)
{
	ಪೂರ್ಣಾಂಕ rc = 0;
	mosquitto_property *properties = ಖಾಲಿ;
	ರಚನೆ mosquitto_packet_ಒಳ prop_packet;

	memset(&prop_packet, 0, ಗಾತ್ರ(ರಚನೆ mosquitto_packet_ಒಳ));

	read_e(db_fptr, &chunk->F, ಗಾತ್ರ(ರಚನೆ PF_ತಳ_ಸಂದೇಶ));
	chunk->F.payloadlen = ntohl(chunk->F.payloadlen);
	ಆದರೆ(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	chunk->F.ಮೂಲ_mid = ntohs(chunk->F.ಮೂಲ_mid);
	chunk->F.ಮೂಲ_id_len = ntohs(chunk->F.ಮೂಲ_id_len);
	chunk->F.ಮೂಲ_username_len = ntohs(chunk->F.ಮೂಲ_username_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);
	chunk->F.ಮೂಲ_ಪೋರ್ಟ್ = ntohs(chunk->F.ಮೂಲ_ಪೋರ್ಟ್);

	ಉದ್ದಳತೆ -= (uint32_t)(ಗಾತ್ರ(ರಚನೆ PF_ತಳ_ಸಂದೇಶ) + chunk->F.payloadlen + chunk->F.ಮೂಲ_id_len + chunk->F.ಮೂಲ_username_len + chunk->F.topic_len);
	ಆದರೆ(ಉದ್ದಳತೆ > MQTT_MAX_PAYLOAD){
		ಹೋಗು error;
	}

	ಆದರೆ(chunk->F.ಮೂಲ_id_len){
		rc = persist__read_string_len(db_fptr, &chunk->ಮೂಲ.id, chunk->F.ಮೂಲ_id_len);
		ಆದರೆ(rc){
			ಹೋಗು error;
		}
	}
	ಆದರೆ(chunk->F.ಮೂಲ_username_len){
		rc = persist__read_string_len(db_fptr, &chunk->ಮೂಲ.username, chunk->F.ಮೂಲ_username_len);
		ಆದರೆ(rc){
			ಹೋಗು error;
		}
	}
	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	ಆದರೆ(rc){
		ಹೋಗು error;
	}

	ಆದರೆ(chunk->F.payloadlen > 0){
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		ಆದರೆ(chunk->payload == ಖಾಲಿ){
			rc = MOSQ_ERR_NOMEM;
			ಹೋಗು error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	ಆದರೆ(ಉದ್ದಳತೆ > 0){
		prop_packet.ಉಳಿದ_ಉದ್ದಳತೆ = ಉದ್ದಳತೆ;
		prop_packet.payload = mosquitto_malloc(ಉದ್ದಳತೆ);
		ಆದರೆ(!prop_packet.payload){
			rc = MOSQ_ERR_NOMEM;
			ಹೋಗು error;
		}
		read_e(db_fptr, prop_packet.payload, ಉದ್ದಳತೆ);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		ಆದರೆ(rc){
			rc = MOSQ_ERR_NOMEM;
			ಹೋಗು error;
		}
	}
	chunk->properties = properties;

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->ಮೂಲ.id);
	mosquitto_FREE(chunk->ಮೂಲ.username);
	mosquitto_FREE(chunk->topic);
	mosquitto_FREE(prop_packet.payload);
	ಹಿಂತಿರುಗಿ rc;
}


ಪೂರ್ಣಾಂಕ persist__chunk_retain_read_v56(FILE *db_fptr, ರಚನೆ P_retain *chunk)
{
	ಆದರೆ(fread(&chunk->F, ಗಾತ್ರ(ರಚನೆ P_retain), 1, db_fptr) != 1){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		ಹಿಂತಿರುಗಿ 1;
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ persist__chunk_sub_read_v56(FILE *db_fptr, ರಚನೆ P_sub *chunk)
{
	ಪೂರ್ಣಾಂಕ rc = MOSQ_ERR_SUCCESS;

	read_e(db_fptr, &chunk->F, ಗಾತ್ರ(ರಚನೆ PF_sub));
	chunk->F.identifier = ntohl(chunk->F.identifier);
	chunk->F.id_len = ntohs(chunk->F.id_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	ಆದರೆ(rc){
		ಹೋಗು error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	ಆದರೆ(rc){
		ಹೋಗು error;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	ಹಿಂತಿರುಗಿ rc;
}

#ಆದರೆ_ಅಂತ್ಯ
