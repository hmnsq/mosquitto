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

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "persist.h"
#அந்தர்பாவ "property_mosq.h"
#அந்தர்பாவ "util_mosq.h"


முழு persist__chunk_header_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	size_t rlen;
	கட்டமைப்பு PF_header header;

	rlen = fread(&header, அளவு(கட்டமைப்பு PF_header), 1, db_fptr);
	எனில்(rlen != 1){
		திரும்பு 1;
	}

	*chunk = ntohl(header.chunk);
	*length = ntohl(header.length);

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு persist__chunk_cfg_read_v56(FILE *db_fptr, கட்டமைப்பு PF_cfg *chunk)
{
	எனில்(fread(chunk, அளவு(கட்டமைப்பு PF_cfg), 1, db_fptr) != 1){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		திரும்பு 1;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு persist__chunk_client_read_v56(FILE *db_fptr, கட்டமைப்பு P_client *chunk, uint32_t db_version)
{
	முழு rc;

	எனில்(db_version == 6){
		read_e(db_fptr, &chunk->F, அளவு(கட்டமைப்பு PF_client));
		chunk->F.username_len = ntohs(chunk->F.username_len);
		chunk->F.listener_port = ntohs(chunk->F.listener_port);
	}இல்லையெனில் எனில்(db_version == 5){
		read_e(db_fptr, &chunk->F, அளவு(கட்டமைப்பு PF_client_v5));
	}இல்லையெனில்{
		திரும்பு 1;
	}

	chunk->F.session_expiry_interval = ntohl(chunk->F.session_expiry_interval);
	chunk->F.last_mid = ntohs(chunk->F.last_mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);


	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	எனில்(rc){
		திரும்பு 1;
	}இல்லையெனில் எனில்(chunk->clientid == வெற்று){
		திரும்பு -1;
	}

	எனில்(chunk->F.username_len > 0){
		rc = persist__read_string_len(db_fptr, &chunk->username, chunk->F.username_len);
		எனில்(rc || !chunk->username){
			mosquitto_FREE(chunk->clientid);
			திரும்பு 1;
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
error:
	log__printf(வெற்று, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	திரும்பு 1;
}


முழு persist_chunk_client_செய்தி_read_v56(FILE *db_fptr, கட்டமைப்பு P_client_செய்தி *chunk, uint32_t length)
{
	mosquitto_property *properties = வெற்று, *p;
	கட்டமைப்பு mosquitto__packet_in prop_packet;
	முழு rc;

	memset(&prop_packet, 0, அளவு(கட்டமைப்பு mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, அளவு(கட்டமைப்பு PF_client_செய்தி));
	chunk->F.mid = ntohs(chunk->F.mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);

	length -= (uint32_t)(அளவு(கட்டமைப்பு PF_client_செய்தி) + chunk->F.id_len);
	எனில்(length > MQTT_MAX_PAYLOAD){
		செல் error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	எனில்(rc){
		திரும்பு rc;
	}

	எனில்(length > 0){
		prop_packet.remaining_length = length;
		prop_packet.payload = mosquitto_malloc(length);
		எனில்(!prop_packet.payload){
			errno = ENOMEM;
			செல் error;
		}

		read_e(db_fptr, prop_packet.payload, length);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		எனில்(rc){
			mosquitto_FREE(chunk->clientid);
			திரும்பு rc;
		}

		எனில்(properties){
			p = properties;
			போது(p){
				எனில்(mosquitto_property_identifier(p) == MQTT_PROP_SUBSCRIPTION_IDENTIFIER){
					chunk->subscription_identifier = mosquitto_property_varint_மதிப்பு(p);
				}
				p = mosquitto_property_next(p);
			}
			mosquitto_property_free_all(&properties);
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(prop_packet.payload);
	log__printf(வெற்று, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	திரும்பு 1;
}


முழு persist_chunk_base_செய்தி_read_v56(FILE *db_fptr, கட்டமைப்பு P_base_செய்தி *chunk, uint32_t length)
{
	முழு rc = 0;
	mosquitto_property *properties = வெற்று;
	கட்டமைப்பு mosquitto__packet_in prop_packet;

	memset(&prop_packet, 0, அளவு(கட்டமைப்பு mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, அளவு(கட்டமைப்பு PF_base_செய்தி));
	chunk->F.payloadlen = ntohl(chunk->F.payloadlen);
	எனில்(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
		திரும்பு MOSQ_ERR_INVAL;
	}
	chunk->F.source_mid = ntohs(chunk->F.source_mid);
	chunk->F.source_id_len = ntohs(chunk->F.source_id_len);
	chunk->F.source_username_len = ntohs(chunk->F.source_username_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);
	chunk->F.source_port = ntohs(chunk->F.source_port);

	length -= (uint32_t)(அளவு(கட்டமைப்பு PF_base_செய்தி) + chunk->F.payloadlen + chunk->F.source_id_len + chunk->F.source_username_len + chunk->F.topic_len);
	எனில்(length > MQTT_MAX_PAYLOAD){
		செல் error;
	}

	எனில்(chunk->F.source_id_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.id, chunk->F.source_id_len);
		எனில்(rc){
			செல் error;
		}
	}
	எனில்(chunk->F.source_username_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.username, chunk->F.source_username_len);
		எனில்(rc){
			செல் error;
		}
	}
	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	எனில்(rc){
		செல் error;
	}

	எனில்(chunk->F.payloadlen > 0){
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		எனில்(chunk->payload == வெற்று){
			rc = MOSQ_ERR_NOMEM;
			செல் error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	எனில்(length > 0){
		prop_packet.remaining_length = length;
		prop_packet.payload = mosquitto_malloc(length);
		எனில்(!prop_packet.payload){
			rc = MOSQ_ERR_NOMEM;
			செல் error;
		}
		read_e(db_fptr, prop_packet.payload, length);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		எனில்(rc){
			rc = MOSQ_ERR_NOMEM;
			செல் error;
		}
	}
	chunk->properties = properties;

	திரும்பு MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	mosquitto_FREE(prop_packet.payload);
	திரும்பு rc;
}


முழு persist__chunk_retain_read_v56(FILE *db_fptr, கட்டமைப்பு P_retain *chunk)
{
	எனில்(fread(&chunk->F, அளவு(கட்டமைப்பு P_retain), 1, db_fptr) != 1){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		திரும்பு 1;
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு persist__chunk_sub_read_v56(FILE *db_fptr, கட்டமைப்பு P_sub *chunk)
{
	முழு rc = MOSQ_ERR_SUCCESS;

	read_e(db_fptr, &chunk->F, அளவு(கட்டமைப்பு PF_sub));
	chunk->F.identifier = ntohl(chunk->F.identifier);
	chunk->F.id_len = ntohs(chunk->F.id_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	எனில்(rc){
		செல் error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	எனில்(rc){
		செல் error;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	திரும்பு rc;
}

#endif
