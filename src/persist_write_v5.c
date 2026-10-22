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

#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "persist.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "property_common.h"
#ಅನ್ತರ್ಭಾವ "property_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"


ಪೂರ್ಣಾಂಕ persist__chunk_cfg_write_v6(FILE *db_fptr, ರಚನೆ PF_cfg *chunk)
{
	ರಚನೆ PF_ಶೀರ್ಷಿಕೆ ಶೀರ್ಷಿಕೆ;

	ಶೀರ್ಷಿಕೆ.chunk = htonl(DB_CHUNK_CFG);
	ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ = htonl(ಗಾತ್ರ(ರಚನೆ PF_cfg));
	write_e(db_fptr, &ಶೀರ್ಷಿಕೆ, ಗಾತ್ರ(ರಚನೆ PF_ಶೀರ್ಷಿಕೆ));
	write_e(db_fptr, chunk, ಗಾತ್ರ(ರಚನೆ PF_cfg));

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
error:
	log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ಹಿಂತಿರುಗಿ 1;
}


ಪೂರ್ಣಾಂಕ persist__chunk_client_write_v6(FILE *db_fptr, ರಚನೆ P_client *chunk)
{
	ರಚನೆ PF_ಶೀರ್ಷಿಕೆ ಶೀರ್ಷಿಕೆ;
	uint16_t id_len = chunk->F.id_len;
	uint16_t username_len = chunk->F.username_len;

	chunk->F.session_expiry_interval = htonl(chunk->F.session_expiry_interval);
	chunk->F.ಕೊನೆಯ_mid = htons(chunk->F.ಕೊನೆಯ_mid);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.username_len = htons(chunk->F.username_len);
	chunk->F.listener_ಪೋರ್ಟ್ = htons(chunk->F.listener_ಪೋರ್ಟ್);

	ಶೀರ್ಷಿಕೆ.chunk = htonl(DB_CHUNK_CLIENT);
	ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ = htonl((uint32_t)ಗಾತ್ರ(ರಚನೆ PF_client)+id_len+username_len);

	write_e(db_fptr, &ಶೀರ್ಷಿಕೆ, ಗಾತ್ರ(ರಚನೆ PF_ಶೀರ್ಷಿಕೆ));
	write_e(db_fptr, &chunk->F, ಗಾತ್ರ(ರಚನೆ PF_client));

	write_e(db_fptr, chunk->clientid, id_len);
	ಆದರೆ(username_len > 0){
		write_e(db_fptr, chunk->username, username_len);
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
error:
	log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ಹಿಂತಿರುಗಿ 1;
}


ಪೂರ್ಣಾಂಕ persist_chunk_client_ಸಂದೇಶ_write_v6(FILE *db_fptr, ರಚನೆ P_client_ಸಂದೇಶ *chunk)
{
	ರಚನೆ PF_ಶೀರ್ಷಿಕೆ ಶೀರ್ಷಿಕೆ;
	ರಚನೆ mosquitto__packet *prop_packet = ಖಾಲಿ;
	uint16_t id_len = chunk->F.id_len;
	uint32_t proplen = 0;
	ಪೂರ್ಣಾಂಕ rc;
	mosquitto_property subscription_id_prop = {
		.ಮುಂದಿನ = ಖಾಲಿ, .identifier = MQTT_PROP_SUBSCRIPTION_IDENTIFIER, .client_generated = true, .property_type = MQTT_PROP_TYPE_VARINT
	};

	ಆದರೆ(chunk->subscription_identifier){
		subscription_id_prop.ಮೌಲ್ಯ.varint = chunk->subscription_identifier;
		proplen += mosquitto_property_get_ಉಳಿದ_ಉದ್ದಳತೆ(&subscription_id_prop);
	}

	chunk->F.mid = htons(chunk->F.mid);
	chunk->F.id_len = htons(chunk->F.id_len);

	ಶೀರ್ಷಿಕೆ.chunk = htonl(DB_CHUNK_CLIENT_ಸಂದೇಶ);
	ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ = htonl((uint32_t)ಗಾತ್ರ(ರಚನೆ PF_client_ಸಂದೇಶ) + id_len + proplen);

	write_e(db_fptr, &ಶೀರ್ಷಿಕೆ, ಗಾತ್ರ(ರಚನೆ PF_ಶೀರ್ಷಿಕೆ));
	write_e(db_fptr, &chunk->F, ಗಾತ್ರ(ರಚನೆ PF_client_ಸಂದೇಶ));
	write_e(db_fptr, chunk->clientid, id_len);
	ಆದರೆ(chunk->subscription_identifier){
		ಆದರೆ(proplen > 0){
			prop_packet = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto__packet)+proplen);
			ಆದರೆ(prop_packet == ಖಾಲಿ){
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
			prop_packet->ಉಳಿದ_ಉದ್ದಳತೆ = proplen;
			prop_packet->packet_ಉದ್ದಳತೆ = proplen;
			rc = property__write_all(prop_packet, &subscription_id_prop, true);
			ಆದರೆ(rc){
				mosquitto_FREE(prop_packet);
				ಹಿಂತಿರುಗಿ rc;
			}

			write_e(db_fptr, prop_packet->payload, proplen);
			mosquitto_FREE(prop_packet);
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
error:
	log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(prop_packet);
	ಹಿಂತಿರುಗಿ 1;
}


ಪೂರ್ಣಾಂಕ persist_chunk_ಸಂದೇಶ_store_write_v6(FILE *db_fptr, ರಚನೆ P_ತಳ_ಸಂದೇಶ *chunk)
{
	ರಚನೆ PF_ಶೀರ್ಷಿಕೆ ಶೀರ್ಷಿಕೆ;
	uint32_t payloadlen = chunk->F.payloadlen;
	uint16_t ಮೂಲ_id_len = chunk->F.ಮೂಲ_id_len;
	uint16_t ಮೂಲ_username_len = chunk->F.ಮೂಲ_username_len;
	uint16_t topic_len = chunk->F.topic_len;
	uint32_t proplen = 0;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ(chunk->properties){
		proplen += mosquitto_property_get_ಉಳಿದ_ಉದ್ದಳತೆ(chunk->properties);
	}

	chunk->F.payloadlen = htonl(chunk->F.payloadlen);
	chunk->F.ಮೂಲ_mid = htons(chunk->F.ಮೂಲ_mid);
	chunk->F.ಮೂಲ_id_len = htons(chunk->F.ಮೂಲ_id_len);
	chunk->F.ಮೂಲ_username_len = htons(chunk->F.ಮೂಲ_username_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);
	chunk->F.ಮೂಲ_ಪೋರ್ಟ್ = htons(chunk->F.ಮೂಲ_ಪೋರ್ಟ್);

	ಶೀರ್ಷಿಕೆ.chunk = htonl(DB_CHUNK_ತಳ_ಸಂದೇಶ);
	ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ = htonl((uint32_t)ಗಾತ್ರ(ರಚನೆ PF_ತಳ_ಸಂದೇಶ) +
			topic_len + payloadlen +
			ಮೂಲ_id_len + ಮೂಲ_username_len + proplen);

	write_e(db_fptr, &ಶೀರ್ಷಿಕೆ, ಗಾತ್ರ(ರಚನೆ PF_ಶೀರ್ಷಿಕೆ));
	write_e(db_fptr, &chunk->F, ಗಾತ್ರ(ರಚನೆ PF_ತಳ_ಸಂದೇಶ));
	ಆದರೆ(ಮೂಲ_id_len){
		write_e(db_fptr, chunk->ಮೂಲ.id, ಮೂಲ_id_len);
	}
	ಆದರೆ(ಮೂಲ_username_len){
		write_e(db_fptr, chunk->ಮೂಲ.username, ಮೂಲ_username_len);
	}
	write_e(db_fptr, chunk->topic, topic_len);
	ಆದರೆ(payloadlen){
		write_e(db_fptr, chunk->payload, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)payloadlen);
	}
	ಆದರೆ(chunk->properties){
		ಆದರೆ(proplen > 0){
			ರಚನೆ mosquitto__packet *prop_packet = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto__packet)+proplen);
			ಆದರೆ(prop_packet == ಖಾಲಿ){
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
			prop_packet->ಉಳಿದ_ಉದ್ದಳತೆ = proplen;
			prop_packet->packet_ಉದ್ದಳತೆ = proplen;
			rc = property__write_all(prop_packet, chunk->properties, true);
			ಆದರೆ(rc){
				mosquitto_FREE(prop_packet);
				ಹಿಂತಿರುಗಿ rc;
			}

			ಆದರೆ(fwrite(prop_packet->payload, 1, proplen, db_fptr) != proplen){
				mosquitto_FREE(prop_packet);
				ಹೋಗು error;
			}
			mosquitto_FREE(prop_packet);
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
error:
	log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ಹಿಂತಿರುಗಿ 1;
}


ಪೂರ್ಣಾಂಕ persist__chunk_retain_write_v6(FILE *db_fptr, ರಚನೆ P_retain *chunk)
{
	ರಚನೆ PF_ಶೀರ್ಷಿಕೆ ಶೀರ್ಷಿಕೆ;

	ಶೀರ್ಷಿಕೆ.chunk = htonl(DB_CHUNK_RETAIN);
	ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ = htonl((uint32_t)ಗಾತ್ರ(ರಚನೆ PF_retain));

	write_e(db_fptr, &ಶೀರ್ಷಿಕೆ, ಗಾತ್ರ(ರಚನೆ PF_ಶೀರ್ಷಿಕೆ));
	write_e(db_fptr, &chunk->F, ಗಾತ್ರ(ರಚನೆ PF_retain));

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
error:
	log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ಹಿಂತಿರುಗಿ 1;
}


ಪೂರ್ಣಾಂಕ persist__chunk_sub_write_v6(FILE *db_fptr, ರಚನೆ P_sub *chunk)
{
	ರಚನೆ PF_ಶೀರ್ಷಿಕೆ ಶೀರ್ಷಿಕೆ;
	uint16_t id_len = chunk->F.id_len;
	uint16_t topic_len = chunk->F.topic_len;

	chunk->F.identifier = htonl(chunk->F.identifier);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);

	ಶೀರ್ಷಿಕೆ.chunk = htonl(DB_CHUNK_SUB);
	ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ = htonl((uint32_t)ಗಾತ್ರ(ರಚನೆ PF_sub) +
			id_len + topic_len);

	write_e(db_fptr, &ಶೀರ್ಷಿಕೆ, ಗಾತ್ರ(ರಚನೆ PF_ಶೀರ್ಷಿಕೆ));
	write_e(db_fptr, &chunk->F, ಗಾತ್ರ(ರಚನೆ PF_sub));
	write_e(db_fptr, chunk->clientid, id_len);
	write_e(db_fptr, chunk->topic, topic_len);

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
error:
	log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ಹಿಂತಿರುಗಿ 1;
}
#ಆದರೆ_ಅಂತ್ಯ
