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

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "persist.h"
#അന്തര്ഭാവ "property_mosq.h"
#അന്തര്ഭാവ "util_mosq.h"


പൂർണ്ണസംഖ്യ persist__chunk_header_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	size_t rlen;
	ഘടന PF_header header;

	rlen = fread(&header, വലുപ്പം(ഘടന PF_header), 1, db_fptr);
	എങ്കിൽ(rlen != 1){
		തിരികെ 1;
	}

	*chunk = ntohl(header.chunk);
	*length = ntohl(header.length);

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ persist__chunk_cfg_read_v56(FILE *db_fptr, ഘടന PF_cfg *chunk)
{
	എങ്കിൽ(fread(chunk, വലുപ്പം(ഘടന PF_cfg), 1, db_fptr) != 1){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		തിരികെ 1;
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ persist__chunk_client_read_v56(FILE *db_fptr, ഘടന P_client *chunk, uint32_t db_version)
{
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(db_version == 6){
		read_e(db_fptr, &chunk->F, വലുപ്പം(ഘടന PF_client));
		chunk->F.username_len = ntohs(chunk->F.username_len);
		chunk->F.listener_port = ntohs(chunk->F.listener_port);
	}അല്ലെങ്കിൽ എങ്കിൽ(db_version == 5){
		read_e(db_fptr, &chunk->F, വലുപ്പം(ഘടന PF_client_v5));
	}അല്ലെങ്കിൽ{
		തിരികെ 1;
	}

	chunk->F.session_expiry_interval = ntohl(chunk->F.session_expiry_interval);
	chunk->F.last_mid = ntohs(chunk->F.last_mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);


	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	എങ്കിൽ(rc){
		തിരികെ 1;
	}അല്ലെങ്കിൽ എങ്കിൽ(chunk->clientid == ശൂന്യ){
		തിരികെ -1;
	}

	എങ്കിൽ(chunk->F.username_len > 0){
		rc = persist__read_string_len(db_fptr, &chunk->username, chunk->F.username_len);
		എങ്കിൽ(rc || !chunk->username){
			mosquitto_FREE(chunk->clientid);
			തിരികെ 1;
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
error:
	log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	തിരികെ 1;
}


പൂർണ്ണസംഖ്യ persist_chunk_client_സന്ദേശം_read_v56(FILE *db_fptr, ഘടന P_client_സന്ദേശം *chunk, uint32_t length)
{
	mosquitto_property *properties = ശൂന്യ, *p;
	ഘടന mosquitto__packet_in prop_packet;
	പൂർണ്ണസംഖ്യ rc;

	memset(&prop_packet, 0, വലുപ്പം(ഘടന mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, വലുപ്പം(ഘടന PF_client_സന്ദേശം));
	chunk->F.mid = ntohs(chunk->F.mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);

	length -= (uint32_t)(വലുപ്പം(ഘടന PF_client_സന്ദേശം) + chunk->F.id_len);
	എങ്കിൽ(length > MQTT_MAX_PAYLOAD){
		പോകുക error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	എങ്കിൽ(length > 0){
		prop_packet.remaining_length = length;
		prop_packet.payload = mosquitto_malloc(length);
		എങ്കിൽ(!prop_packet.payload){
			errno = ENOMEM;
			പോകുക error;
		}

		read_e(db_fptr, prop_packet.payload, length);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		എങ്കിൽ(rc){
			mosquitto_FREE(chunk->clientid);
			തിരികെ rc;
		}

		എങ്കിൽ(properties){
			p = properties;
			ആകുമ്പോൾ(p){
				എങ്കിൽ(mosquitto_property_identifier(p) == MQTT_PROP_SUBSCRIPTION_IDENTIFIER){
					chunk->subscription_identifier = mosquitto_property_varint_മൂല്യം(p);
				}
				p = mosquitto_property_next(p);
			}
			mosquitto_property_free_all(&properties);
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(prop_packet.payload);
	log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	തിരികെ 1;
}


പൂർണ്ണസംഖ്യ persist_chunk_base_സന്ദേശം_read_v56(FILE *db_fptr, ഘടന P_base_സന്ദേശം *chunk, uint32_t length)
{
	പൂർണ്ണസംഖ്യ rc = 0;
	mosquitto_property *properties = ശൂന്യ;
	ഘടന mosquitto__packet_in prop_packet;

	memset(&prop_packet, 0, വലുപ്പം(ഘടന mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, വലുപ്പം(ഘടന PF_base_സന്ദേശം));
	chunk->F.payloadlen = ntohl(chunk->F.payloadlen);
	എങ്കിൽ(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
		തിരികെ MOSQ_ERR_INVAL;
	}
	chunk->F.source_mid = ntohs(chunk->F.source_mid);
	chunk->F.source_id_len = ntohs(chunk->F.source_id_len);
	chunk->F.source_username_len = ntohs(chunk->F.source_username_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);
	chunk->F.source_port = ntohs(chunk->F.source_port);

	length -= (uint32_t)(വലുപ്പം(ഘടന PF_base_സന്ദേശം) + chunk->F.payloadlen + chunk->F.source_id_len + chunk->F.source_username_len + chunk->F.topic_len);
	എങ്കിൽ(length > MQTT_MAX_PAYLOAD){
		പോകുക error;
	}

	എങ്കിൽ(chunk->F.source_id_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.id, chunk->F.source_id_len);
		എങ്കിൽ(rc){
			പോകുക error;
		}
	}
	എങ്കിൽ(chunk->F.source_username_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.username, chunk->F.source_username_len);
		എങ്കിൽ(rc){
			പോകുക error;
		}
	}
	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	എങ്കിൽ(rc){
		പോകുക error;
	}

	എങ്കിൽ(chunk->F.payloadlen > 0){
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		എങ്കിൽ(chunk->payload == ശൂന്യ){
			rc = MOSQ_ERR_NOMEM;
			പോകുക error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	എങ്കിൽ(length > 0){
		prop_packet.remaining_length = length;
		prop_packet.payload = mosquitto_malloc(length);
		എങ്കിൽ(!prop_packet.payload){
			rc = MOSQ_ERR_NOMEM;
			പോകുക error;
		}
		read_e(db_fptr, prop_packet.payload, length);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		എങ്കിൽ(rc){
			rc = MOSQ_ERR_NOMEM;
			പോകുക error;
		}
	}
	chunk->properties = properties;

	തിരികെ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	mosquitto_FREE(prop_packet.payload);
	തിരികെ rc;
}


പൂർണ്ണസംഖ്യ persist__chunk_retain_read_v56(FILE *db_fptr, ഘടന P_retain *chunk)
{
	എങ്കിൽ(fread(&chunk->F, വലുപ്പം(ഘടന P_retain), 1, db_fptr) != 1){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		തിരികെ 1;
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ persist__chunk_sub_read_v56(FILE *db_fptr, ഘടന P_sub *chunk)
{
	പൂർണ്ണസംഖ്യ rc = MOSQ_ERR_SUCCESS;

	read_e(db_fptr, &chunk->F, വലുപ്പം(ഘടന PF_sub));
	chunk->F.identifier = ntohl(chunk->F.identifier);
	chunk->F.id_len = ntohs(chunk->F.id_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	എങ്കിൽ(rc){
		പോകുക error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	എങ്കിൽ(rc){
		പോകുക error;
	}

	തിരികെ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	തിരികെ rc;
}

#endif
