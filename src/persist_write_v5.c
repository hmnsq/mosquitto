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

#അന്തര്ഭാവ "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#അന്തര്ഭാവ <errno.h>
#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <time.h>

#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "persist.h"
#അന്തര്ഭാവ "packet_mosq.h"
#അന്തര്ഭാവ "property_common.h"
#അന്തര്ഭാവ "property_mosq.h"
#അന്തര്ഭാവ "util_mosq.h"


പൂർണ്ണസംഖ്യ persist__chunk_cfg_write_v6(FILE *db_fptr, ഘടന PF_cfg *chunk)
{
	ഘടന PF_header header;

	header.chunk = htonl(DB_CHUNK_CFG);
	header.length = htonl(വലുപ്പം(ഘടന PF_cfg));
	write_e(db_fptr, &header, വലുപ്പം(ഘടന PF_header));
	write_e(db_fptr, chunk, വലുപ്പം(ഘടന PF_cfg));

	തിരികെ MOSQ_ERR_SUCCESS;
error:
	log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	തിരികെ 1;
}


പൂർണ്ണസംഖ്യ persist__chunk_client_write_v6(FILE *db_fptr, ഘടന P_client *chunk)
{
	ഘടന PF_header header;
	uint16_t id_len = chunk->F.id_len;
	uint16_t username_len = chunk->F.username_len;

	chunk->F.session_expiry_interval = htonl(chunk->F.session_expiry_interval);
	chunk->F.last_mid = htons(chunk->F.last_mid);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.username_len = htons(chunk->F.username_len);
	chunk->F.listener_port = htons(chunk->F.listener_port);

	header.chunk = htonl(DB_CHUNK_CLIENT);
	header.length = htonl((uint32_t)വലുപ്പം(ഘടന PF_client)+id_len+username_len);

	write_e(db_fptr, &header, വലുപ്പം(ഘടന PF_header));
	write_e(db_fptr, &chunk->F, വലുപ്പം(ഘടന PF_client));

	write_e(db_fptr, chunk->clientid, id_len);
	എങ്കിൽ(username_len > 0){
		write_e(db_fptr, chunk->username, username_len);
	}

	തിരികെ MOSQ_ERR_SUCCESS;
error:
	log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	തിരികെ 1;
}


പൂർണ്ണസംഖ്യ persist_chunk_client_സന്ദേശം_write_v6(FILE *db_fptr, ഘടന P_client_സന്ദേശം *chunk)
{
	ഘടന PF_header header;
	ഘടന mosquitto__packet *prop_packet = ശൂന്യ;
	uint16_t id_len = chunk->F.id_len;
	uint32_t proplen = 0;
	പൂർണ്ണസംഖ്യ rc;
	mosquitto_property subscription_id_prop = {
		.next = ശൂന്യ, .identifier = MQTT_PROP_SUBSCRIPTION_IDENTIFIER, .client_generated = true, .property_type = MQTT_PROP_TYPE_VARINT
	};

	എങ്കിൽ(chunk->subscription_identifier){
		subscription_id_prop.മൂല്യം.varint = chunk->subscription_identifier;
		proplen += mosquitto_property_get_remaining_length(&subscription_id_prop);
	}

	chunk->F.mid = htons(chunk->F.mid);
	chunk->F.id_len = htons(chunk->F.id_len);

	header.chunk = htonl(DB_CHUNK_CLIENT_സന്ദേശം);
	header.length = htonl((uint32_t)വലുപ്പം(ഘടന PF_client_സന്ദേശം) + id_len + proplen);

	write_e(db_fptr, &header, വലുപ്പം(ഘടന PF_header));
	write_e(db_fptr, &chunk->F, വലുപ്പം(ഘടന PF_client_സന്ദേശം));
	write_e(db_fptr, chunk->clientid, id_len);
	എങ്കിൽ(chunk->subscription_identifier){
		എങ്കിൽ(proplen > 0){
			prop_packet = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto__packet)+proplen);
			എങ്കിൽ(prop_packet == ശൂന്യ){
				തിരികെ MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_length = proplen;
			prop_packet->packet_length = proplen;
			rc = property__write_all(prop_packet, &subscription_id_prop, true);
			എങ്കിൽ(rc){
				mosquitto_FREE(prop_packet);
				തിരികെ rc;
			}

			write_e(db_fptr, prop_packet->payload, proplen);
			mosquitto_FREE(prop_packet);
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
error:
	log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(prop_packet);
	തിരികെ 1;
}


പൂർണ്ണസംഖ്യ persist__chunk_message_store_write_v6(FILE *db_fptr, ഘടന P_base_സന്ദേശം *chunk)
{
	ഘടന PF_header header;
	uint32_t payloadlen = chunk->F.payloadlen;
	uint16_t source_id_len = chunk->F.source_id_len;
	uint16_t source_username_len = chunk->F.source_username_len;
	uint16_t topic_len = chunk->F.topic_len;
	uint32_t proplen = 0;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(chunk->properties){
		proplen += mosquitto_property_get_remaining_length(chunk->properties);
	}

	chunk->F.payloadlen = htonl(chunk->F.payloadlen);
	chunk->F.source_mid = htons(chunk->F.source_mid);
	chunk->F.source_id_len = htons(chunk->F.source_id_len);
	chunk->F.source_username_len = htons(chunk->F.source_username_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);
	chunk->F.source_port = htons(chunk->F.source_port);

	header.chunk = htonl(DB_CHUNK_BASE_സന്ദേശം);
	header.length = htonl((uint32_t)വലുപ്പം(ഘടന PF_base_സന്ദേശം) +
			topic_len + payloadlen +
			source_id_len + source_username_len + proplen);

	write_e(db_fptr, &header, വലുപ്പം(ഘടന PF_header));
	write_e(db_fptr, &chunk->F, വലുപ്പം(ഘടന PF_base_സന്ദേശം));
	എങ്കിൽ(source_id_len){
		write_e(db_fptr, chunk->source.id, source_id_len);
	}
	എങ്കിൽ(source_username_len){
		write_e(db_fptr, chunk->source.username, source_username_len);
	}
	write_e(db_fptr, chunk->topic, topic_len);
	എങ്കിൽ(payloadlen){
		write_e(db_fptr, chunk->payload, (unsigned പൂർണ്ണസംഖ്യ)payloadlen);
	}
	എങ്കിൽ(chunk->properties){
		എങ്കിൽ(proplen > 0){
			ഘടന mosquitto__packet *prop_packet = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto__packet)+proplen);
			എങ്കിൽ(prop_packet == ശൂന്യ){
				തിരികെ MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_length = proplen;
			prop_packet->packet_length = proplen;
			rc = property__write_all(prop_packet, chunk->properties, true);
			എങ്കിൽ(rc){
				mosquitto_FREE(prop_packet);
				തിരികെ rc;
			}

			എങ്കിൽ(fwrite(prop_packet->payload, 1, proplen, db_fptr) != proplen){
				mosquitto_FREE(prop_packet);
				പോകുക error;
			}
			mosquitto_FREE(prop_packet);
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
error:
	log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	തിരികെ 1;
}


പൂർണ്ണസംഖ്യ persist__chunk_retain_write_v6(FILE *db_fptr, ഘടന P_retain *chunk)
{
	ഘടന PF_header header;

	header.chunk = htonl(DB_CHUNK_RETAIN);
	header.length = htonl((uint32_t)വലുപ്പം(ഘടന PF_retain));

	write_e(db_fptr, &header, വലുപ്പം(ഘടന PF_header));
	write_e(db_fptr, &chunk->F, വലുപ്പം(ഘടന PF_retain));

	തിരികെ MOSQ_ERR_SUCCESS;
error:
	log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	തിരികെ 1;
}


പൂർണ്ണസംഖ്യ persist__chunk_sub_write_v6(FILE *db_fptr, ഘടന P_sub *chunk)
{
	ഘടന PF_header header;
	uint16_t id_len = chunk->F.id_len;
	uint16_t topic_len = chunk->F.topic_len;

	chunk->F.identifier = htonl(chunk->F.identifier);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);

	header.chunk = htonl(DB_CHUNK_SUB);
	header.length = htonl((uint32_t)വലുപ്പം(ഘടന PF_sub) +
			id_len + topic_len);

	write_e(db_fptr, &header, വലുപ്പം(ഘടന PF_header));
	write_e(db_fptr, &chunk->F, വലുപ്പം(ഘടന PF_sub));
	write_e(db_fptr, chunk->clientid, id_len);
	write_e(db_fptr, chunk->topic, topic_len);

	തിരികെ MOSQ_ERR_SUCCESS;
error:
	log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	തിരികെ 1;
}
#endif
