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

#அந்தர்பாவ "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#அந்தர்பாவ <arpa/inet.h>
#endif
#அந்தர்பாவ <errno.h>
#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <time.h>

#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "persist.h"
#அந்தர்பாவ "packet_mosq.h"
#அந்தர்பாவ "property_common.h"
#அந்தர்பாவ "property_mosq.h"
#அந்தர்பாவ "util_mosq.h"


முழு persist__chunk_cfg_write_v6(FILE *db_fptr, கட்டமைப்பு PF_cfg *chunk)
{
	கட்டமைப்பு PF_header header;

	header.chunk = htonl(DB_CHUNK_CFG);
	header.length = htonl(அளவு(கட்டமைப்பு PF_cfg));
	write_e(db_fptr, &header, அளவு(கட்டமைப்பு PF_header));
	write_e(db_fptr, chunk, அளவு(கட்டமைப்பு PF_cfg));

	திரும்பு MOSQ_ERR_SUCCESS;
error:
	log__printf(வெற்று, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	திரும்பு 1;
}


முழு persist__chunk_client_write_v6(FILE *db_fptr, கட்டமைப்பு P_client *chunk)
{
	கட்டமைப்பு PF_header header;
	uint16_t id_len = chunk->F.id_len;
	uint16_t username_len = chunk->F.username_len;

	chunk->F.session_expiry_interval = htonl(chunk->F.session_expiry_interval);
	chunk->F.last_mid = htons(chunk->F.last_mid);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.username_len = htons(chunk->F.username_len);
	chunk->F.listener_port = htons(chunk->F.listener_port);

	header.chunk = htonl(DB_CHUNK_CLIENT);
	header.length = htonl((uint32_t)அளவு(கட்டமைப்பு PF_client)+id_len+username_len);

	write_e(db_fptr, &header, அளவு(கட்டமைப்பு PF_header));
	write_e(db_fptr, &chunk->F, அளவு(கட்டமைப்பு PF_client));

	write_e(db_fptr, chunk->clientid, id_len);
	எனில்(username_len > 0){
		write_e(db_fptr, chunk->username, username_len);
	}

	திரும்பு MOSQ_ERR_SUCCESS;
error:
	log__printf(வெற்று, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	திரும்பு 1;
}


முழு persist_chunk_client_செய்தி_write_v6(FILE *db_fptr, கட்டமைப்பு P_client_செய்தி *chunk)
{
	கட்டமைப்பு PF_header header;
	கட்டமைப்பு mosquitto__packet *prop_packet = வெற்று;
	uint16_t id_len = chunk->F.id_len;
	uint32_t proplen = 0;
	முழு rc;
	mosquitto_property subscription_id_prop = {
		.next = வெற்று, .identifier = MQTT_PROP_SUBSCRIPTION_IDENTIFIER, .client_generated = true, .property_type = MQTT_PROP_TYPE_VARINT
	};

	எனில்(chunk->subscription_identifier){
		subscription_id_prop.மதிப்பு.varint = chunk->subscription_identifier;
		proplen += mosquitto_property_get_remaining_length(&subscription_id_prop);
	}

	chunk->F.mid = htons(chunk->F.mid);
	chunk->F.id_len = htons(chunk->F.id_len);

	header.chunk = htonl(DB_CHUNK_CLIENT_செய்தி);
	header.length = htonl((uint32_t)அளவு(கட்டமைப்பு PF_client_செய்தி) + id_len + proplen);

	write_e(db_fptr, &header, அளவு(கட்டமைப்பு PF_header));
	write_e(db_fptr, &chunk->F, அளவு(கட்டமைப்பு PF_client_செய்தி));
	write_e(db_fptr, chunk->clientid, id_len);
	எனில்(chunk->subscription_identifier){
		எனில்(proplen > 0){
			prop_packet = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto__packet)+proplen);
			எனில்(prop_packet == வெற்று){
				திரும்பு MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_length = proplen;
			prop_packet->packet_length = proplen;
			rc = property__write_all(prop_packet, &subscription_id_prop, true);
			எனில்(rc){
				mosquitto_FREE(prop_packet);
				திரும்பு rc;
			}

			write_e(db_fptr, prop_packet->payload, proplen);
			mosquitto_FREE(prop_packet);
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
error:
	log__printf(வெற்று, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(prop_packet);
	திரும்பு 1;
}


முழு persist__chunk_message_store_write_v6(FILE *db_fptr, கட்டமைப்பு P_base_செய்தி *chunk)
{
	கட்டமைப்பு PF_header header;
	uint32_t payloadlen = chunk->F.payloadlen;
	uint16_t source_id_len = chunk->F.source_id_len;
	uint16_t source_username_len = chunk->F.source_username_len;
	uint16_t topic_len = chunk->F.topic_len;
	uint32_t proplen = 0;
	முழு rc;

	எனில்(chunk->properties){
		proplen += mosquitto_property_get_remaining_length(chunk->properties);
	}

	chunk->F.payloadlen = htonl(chunk->F.payloadlen);
	chunk->F.source_mid = htons(chunk->F.source_mid);
	chunk->F.source_id_len = htons(chunk->F.source_id_len);
	chunk->F.source_username_len = htons(chunk->F.source_username_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);
	chunk->F.source_port = htons(chunk->F.source_port);

	header.chunk = htonl(DB_CHUNK_BASE_செய்தி);
	header.length = htonl((uint32_t)அளவு(கட்டமைப்பு PF_base_செய்தி) +
			topic_len + payloadlen +
			source_id_len + source_username_len + proplen);

	write_e(db_fptr, &header, அளவு(கட்டமைப்பு PF_header));
	write_e(db_fptr, &chunk->F, அளவு(கட்டமைப்பு PF_base_செய்தி));
	எனில்(source_id_len){
		write_e(db_fptr, chunk->source.id, source_id_len);
	}
	எனில்(source_username_len){
		write_e(db_fptr, chunk->source.username, source_username_len);
	}
	write_e(db_fptr, chunk->topic, topic_len);
	எனில்(payloadlen){
		write_e(db_fptr, chunk->payload, (unsigned முழு)payloadlen);
	}
	எனில்(chunk->properties){
		எனில்(proplen > 0){
			கட்டமைப்பு mosquitto__packet *prop_packet = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto__packet)+proplen);
			எனில்(prop_packet == வெற்று){
				திரும்பு MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_length = proplen;
			prop_packet->packet_length = proplen;
			rc = property__write_all(prop_packet, chunk->properties, true);
			எனில்(rc){
				mosquitto_FREE(prop_packet);
				திரும்பு rc;
			}

			எனில்(fwrite(prop_packet->payload, 1, proplen, db_fptr) != proplen){
				mosquitto_FREE(prop_packet);
				செல் error;
			}
			mosquitto_FREE(prop_packet);
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
error:
	log__printf(வெற்று, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	திரும்பு 1;
}


முழு persist__chunk_retain_write_v6(FILE *db_fptr, கட்டமைப்பு P_retain *chunk)
{
	கட்டமைப்பு PF_header header;

	header.chunk = htonl(DB_CHUNK_RETAIN);
	header.length = htonl((uint32_t)அளவு(கட்டமைப்பு PF_retain));

	write_e(db_fptr, &header, அளவு(கட்டமைப்பு PF_header));
	write_e(db_fptr, &chunk->F, அளவு(கட்டமைப்பு PF_retain));

	திரும்பு MOSQ_ERR_SUCCESS;
error:
	log__printf(வெற்று, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	திரும்பு 1;
}


முழு persist__chunk_sub_write_v6(FILE *db_fptr, கட்டமைப்பு P_sub *chunk)
{
	கட்டமைப்பு PF_header header;
	uint16_t id_len = chunk->F.id_len;
	uint16_t topic_len = chunk->F.topic_len;

	chunk->F.identifier = htonl(chunk->F.identifier);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);

	header.chunk = htonl(DB_CHUNK_SUB);
	header.length = htonl((uint32_t)அளவு(கட்டமைப்பு PF_sub) +
			id_len + topic_len);

	write_e(db_fptr, &header, அளவு(கட்டமைப்பு PF_header));
	write_e(db_fptr, &chunk->F, அளவு(கட்டமைப்பு PF_sub));
	write_e(db_fptr, chunk->clientid, id_len);
	write_e(db_fptr, chunk->topic, topic_len);

	திரும்பு MOSQ_ERR_SUCCESS;
error:
	log__printf(வெற்று, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	திரும்பு 1;
}
#endif
