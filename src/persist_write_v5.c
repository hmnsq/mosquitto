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

#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "persist.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "property_common.h"
#ಅನ್ತರ್ಭಾವ "property_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"


ಪೂರ್ಣಾಙ್ಕ persist__chunk_cfg_write_v6(FILE *db_fptr, ಸಂರಚನಾ PF_cfg *chunk)
{
	ಸಂರಚನಾ PF_header header;

	header.chunk = htonl(DB_CHUNK_CFG);
	header.length = htonl(ಪರಿಮಾಣ(ಸಂರಚನಾ PF_cfg));
	write_e(db_fptr, &header, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_header));
	write_e(db_fptr, chunk, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_cfg));

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
error:
	log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ಪ್ರತ್ಯಾಗಮ 1;
}


ಪೂರ್ಣಾಙ್ಕ persist__chunk_client_write_v6(FILE *db_fptr, ಸಂರಚನಾ P_client *chunk)
{
	ಸಂರಚನಾ PF_header header;
	uint16_t id_len = chunk->F.id_len;
	uint16_t username_len = chunk->F.username_len;

	chunk->F.session_expiry_interval = htonl(chunk->F.session_expiry_interval);
	chunk->F.last_mid = htons(chunk->F.last_mid);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.username_len = htons(chunk->F.username_len);
	chunk->F.listener_port = htons(chunk->F.listener_port);

	header.chunk = htonl(DB_CHUNK_CLIENT);
	header.length = htonl((uint32_t)ಪರಿಮಾಣ(ಸಂರಚನಾ PF_client)+id_len+username_len);

	write_e(db_fptr, &header, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_header));
	write_e(db_fptr, &chunk->F, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_client));

	write_e(db_fptr, chunk->clientid, id_len);
	ಯದಿ(username_len > 0){
		write_e(db_fptr, chunk->username, username_len);
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
error:
	log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ಪ್ರತ್ಯಾಗಮ 1;
}


ಪೂರ್ಣಾಙ್ಕ persist_chunk_client_ಸಂದೇಶ_write_v6(FILE *db_fptr, ಸಂರಚನಾ P_client_ಸಂದೇಶ *chunk)
{
	ಸಂರಚನಾ PF_header header;
	ಸಂರಚನಾ mosquitto__packet *prop_packet = ರಿಕ್ತ;
	uint16_t id_len = chunk->F.id_len;
	uint32_t proplen = 0;
	ಪೂರ್ಣಾಙ್ಕ rc;
	mosquitto_property subscription_id_prop = {
		.next = ರಿಕ್ತ, .identifier = MQTT_PROP_SUBSCRIPTION_IDENTIFIER, .client_generated = true, .property_type = MQTT_PROP_TYPE_VARINT
	};

	ಯದಿ(chunk->subscription_identifier){
		subscription_id_prop.ಮೌಲ್ಯ.varint = chunk->subscription_identifier;
		proplen += mosquitto_property_get_remaining_length(&subscription_id_prop);
	}

	chunk->F.mid = htons(chunk->F.mid);
	chunk->F.id_len = htons(chunk->F.id_len);

	header.chunk = htonl(DB_CHUNK_CLIENT_ಸಂದೇಶ);
	header.length = htonl((uint32_t)ಪರಿಮಾಣ(ಸಂರಚನಾ PF_client_ಸಂದೇಶ) + id_len + proplen);

	write_e(db_fptr, &header, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_header));
	write_e(db_fptr, &chunk->F, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_client_ಸಂದೇಶ));
	write_e(db_fptr, chunk->clientid, id_len);
	ಯದಿ(chunk->subscription_identifier){
		ಯದಿ(proplen > 0){
			prop_packet = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__packet)+proplen);
			ಯದಿ(prop_packet == ರಿಕ್ತ){
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_length = proplen;
			prop_packet->packet_length = proplen;
			rc = property__write_all(prop_packet, &subscription_id_prop, true);
			ಯದಿ(rc){
				mosquitto_FREE(prop_packet);
				ಪ್ರತ್ಯಾಗಮ rc;
			}

			write_e(db_fptr, prop_packet->payload, proplen);
			mosquitto_FREE(prop_packet);
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
error:
	log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(prop_packet);
	ಪ್ರತ್ಯಾಗಮ 1;
}


ಪೂರ್ಣಾಙ್ಕ persist__chunk_message_store_write_v6(FILE *db_fptr, ಸಂರಚನಾ P_base_ಸಂದೇಶ *chunk)
{
	ಸಂರಚನಾ PF_header header;
	uint32_t payloadlen = chunk->F.payloadlen;
	uint16_t source_id_len = chunk->F.source_id_len;
	uint16_t source_username_len = chunk->F.source_username_len;
	uint16_t topic_len = chunk->F.topic_len;
	uint32_t proplen = 0;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(chunk->properties){
		proplen += mosquitto_property_get_remaining_length(chunk->properties);
	}

	chunk->F.payloadlen = htonl(chunk->F.payloadlen);
	chunk->F.source_mid = htons(chunk->F.source_mid);
	chunk->F.source_id_len = htons(chunk->F.source_id_len);
	chunk->F.source_username_len = htons(chunk->F.source_username_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);
	chunk->F.source_port = htons(chunk->F.source_port);

	header.chunk = htonl(DB_CHUNK_BASE_ಸಂದೇಶ);
	header.length = htonl((uint32_t)ಪರಿಮಾಣ(ಸಂರಚನಾ PF_base_ಸಂದೇಶ) +
			topic_len + payloadlen +
			source_id_len + source_username_len + proplen);

	write_e(db_fptr, &header, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_header));
	write_e(db_fptr, &chunk->F, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_base_ಸಂದೇಶ));
	ಯದಿ(source_id_len){
		write_e(db_fptr, chunk->source.id, source_id_len);
	}
	ಯದಿ(source_username_len){
		write_e(db_fptr, chunk->source.username, source_username_len);
	}
	write_e(db_fptr, chunk->topic, topic_len);
	ಯದಿ(payloadlen){
		write_e(db_fptr, chunk->payload, (unsigned ಪೂರ್ಣಾಙ್ಕ)payloadlen);
	}
	ಯದಿ(chunk->properties){
		ಯದಿ(proplen > 0){
			ಸಂರಚನಾ mosquitto__packet *prop_packet = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__packet)+proplen);
			ಯದಿ(prop_packet == ರಿಕ್ತ){
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_length = proplen;
			prop_packet->packet_length = proplen;
			rc = property__write_all(prop_packet, chunk->properties, true);
			ಯದಿ(rc){
				mosquitto_FREE(prop_packet);
				ಪ್ರತ್ಯಾಗಮ rc;
			}

			ಯದಿ(fwrite(prop_packet->payload, 1, proplen, db_fptr) != proplen){
				mosquitto_FREE(prop_packet);
				ಗಚ್ಛ error;
			}
			mosquitto_FREE(prop_packet);
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
error:
	log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ಪ್ರತ್ಯಾಗಮ 1;
}


ಪೂರ್ಣಾಙ್ಕ persist__chunk_retain_write_v6(FILE *db_fptr, ಸಂರಚನಾ P_retain *chunk)
{
	ಸಂರಚನಾ PF_header header;

	header.chunk = htonl(DB_CHUNK_RETAIN);
	header.length = htonl((uint32_t)ಪರಿಮಾಣ(ಸಂರಚನಾ PF_retain));

	write_e(db_fptr, &header, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_header));
	write_e(db_fptr, &chunk->F, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_retain));

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
error:
	log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ಪ್ರತ್ಯಾಗಮ 1;
}


ಪೂರ್ಣಾಙ್ಕ persist__chunk_sub_write_v6(FILE *db_fptr, ಸಂರಚನಾ P_sub *chunk)
{
	ಸಂರಚನಾ PF_header header;
	uint16_t id_len = chunk->F.id_len;
	uint16_t topic_len = chunk->F.topic_len;

	chunk->F.identifier = htonl(chunk->F.identifier);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);

	header.chunk = htonl(DB_CHUNK_SUB);
	header.length = htonl((uint32_t)ಪರಿಮಾಣ(ಸಂರಚನಾ PF_sub) +
			id_len + topic_len);

	write_e(db_fptr, &header, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_header));
	write_e(db_fptr, &chunk->F, ಪರಿಮಾಣ(ಸಂರಚನಾ PF_sub));
	write_e(db_fptr, chunk->clientid, id_len);
	write_e(db_fptr, chunk->topic, topic_len);

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
error:
	log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ಪ್ರತ್ಯಾಗಮ 1;
}
#endif
