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

#include "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#include <arpa/inet.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "persist.h"
#include "property_mosq.h"
#include "util_mosq.h"


puur_naa_nka persist__chunk_header_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	size_t rlen;
	sa_nracanaa PF_header header;

	rlen = fread(&header, parimaa_na(sa_nracanaa PF_header), 1, db_fptr);
	ᤀᤡ᤺(rlen != 1){
		ᤀᤡᤴ 1;
	}

	*chunk = ntohl(header.chunk);
	*length = ntohl(header.length);

	ᤀᤡᤴ MOSQ_ERR_SUCCESS;
}


puur_naa_nka persist__chunk_cfg_read_v56(FILE *db_fptr, sa_nracanaa PF_cfg *chunk)
{
	ᤀᤡ᤺(fread(chunk, parimaa_na(sa_nracanaa PF_cfg), 1, db_fptr) != 1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		ᤀᤡᤴ 1;
	}

	ᤀᤡᤴ MOSQ_ERR_SUCCESS;
}


puur_naa_nka persist__chunk_client_read_v56(FILE *db_fptr, sa_nracanaa P_client *chunk, uint32_t db_version)
{
	puur_naa_nka rc;

	ᤀᤡ᤺(db_version == 6){
		read_e(db_fptr, &chunk->F, parimaa_na(sa_nracanaa PF_client));
		chunk->F.username_len = ntohs(chunk->F.username_len);
		chunk->F.listener_port = ntohs(chunk->F.listener_port);
	}ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(db_version == 5){
		read_e(db_fptr, &chunk->F, parimaa_na(sa_nracanaa PF_client_v5));
	}ᤀᤡ᤺ᤆᤡ{
		ᤀᤡᤴ 1;
	}

	chunk->F.session_expiry_interval = ntohl(chunk->F.session_expiry_interval);
	chunk->F.last_mid = ntohs(chunk->F.last_mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);


	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	ᤀᤡ᤺(rc){
		ᤀᤡᤴ 1;
	}ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(chunk->clientid == NULL){
		ᤀᤡᤴ -1;
	}

	ᤀᤡ᤺(chunk->F.username_len > 0){
		rc = persist__read_string_len(db_fptr, &chunk->username, chunk->F.username_len);
		ᤀᤡ᤺(rc || !chunk->username){
			mosquitto_FREE(chunk->clientid);
			ᤀᤡᤴ 1;
		}
	}

	ᤀᤡᤴ MOSQ_ERR_SUCCESS;
error:
	log__printf(NULL, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ᤀᤡᤴ 1;
}


puur_naa_nka persist__chunk_client_msg_read_v56(FILE *db_fptr, sa_nracanaa P_client_msg *chunk, uint32_t length)
{
	mosquitto_property *properties = NULL, *p;
	sa_nracanaa mosquitto__packet_in prop_packet;
	puur_naa_nka rc;

	memset(&prop_packet, 0, parimaa_na(sa_nracanaa mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, parimaa_na(sa_nracanaa PF_client_msg));
	chunk->F.mid = ntohs(chunk->F.mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);

	length -= (uint32_t)(parimaa_na(sa_nracanaa PF_client_msg) + chunk->F.id_len);
	ᤀᤡ᤺(length > MQTT_MAX_PAYLOAD){
		goto error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	ᤀᤡ᤺(rc){
		ᤀᤡᤴ rc;
	}

	ᤀᤡ᤺(length > 0){
		prop_packet.remaining_length = length;
		prop_packet.payload = mosquitto_malloc(length);
		ᤀᤡ᤺(!prop_packet.payload){
			errno = ENOMEM;
			goto error;
		}

		read_e(db_fptr, prop_packet.payload, length);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		ᤀᤡ᤺(rc){
			mosquitto_FREE(chunk->clientid);
			ᤀᤡᤴ rc;
		}

		ᤀᤡ᤺(properties){
			p = properties;
			ᤀᤠ᤺(p){
				ᤀᤡ᤺(mosquitto_property_identifier(p) == MQTT_PROP_SUBSCRIPTION_IDENTIFIER){
					chunk->subscription_identifier = mosquitto_property_varint_value(p);
				}
				p = mosquitto_property_next(p);
			}
			mosquitto_property_free_all(&properties);
		}
	}

	ᤀᤡᤴ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(prop_packet.payload);
	log__printf(NULL, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ᤀᤡᤴ 1;
}


puur_naa_nka persist__chunk_base_msg_read_v56(FILE *db_fptr, sa_nracanaa P_base_msg *chunk, uint32_t length)
{
	puur_naa_nka rc = 0;
	mosquitto_property *properties = NULL;
	sa_nracanaa mosquitto__packet_in prop_packet;

	memset(&prop_packet, 0, parimaa_na(sa_nracanaa mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, parimaa_na(sa_nracanaa PF_base_msg));
	chunk->F.payloadlen = ntohl(chunk->F.payloadlen);
	ᤀᤡ᤺(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
		ᤀᤡᤴ MOSQ_ERR_INVAL;
	}
	chunk->F.source_mid = ntohs(chunk->F.source_mid);
	chunk->F.source_id_len = ntohs(chunk->F.source_id_len);
	chunk->F.source_username_len = ntohs(chunk->F.source_username_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);
	chunk->F.source_port = ntohs(chunk->F.source_port);

	length -= (uint32_t)(parimaa_na(sa_nracanaa PF_base_msg) + chunk->F.payloadlen + chunk->F.source_id_len + chunk->F.source_username_len + chunk->F.topic_len);
	ᤀᤡ᤺(length > MQTT_MAX_PAYLOAD){
		goto error;
	}

	ᤀᤡ᤺(chunk->F.source_id_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.id, chunk->F.source_id_len);
		ᤀᤡ᤺(rc){
			goto error;
		}
	}
	ᤀᤡ᤺(chunk->F.source_username_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.username, chunk->F.source_username_len);
		ᤀᤡ᤺(rc){
			goto error;
		}
	}
	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	ᤀᤡ᤺(rc){
		goto error;
	}

	ᤀᤡ᤺(chunk->F.payloadlen > 0){
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		ᤀᤡ᤺(chunk->payload == NULL){
			rc = MOSQ_ERR_NOMEM;
			goto error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	ᤀᤡ᤺(length > 0){
		prop_packet.remaining_length = length;
		prop_packet.payload = mosquitto_malloc(length);
		ᤀᤡ᤺(!prop_packet.payload){
			rc = MOSQ_ERR_NOMEM;
			goto error;
		}
		read_e(db_fptr, prop_packet.payload, length);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		ᤀᤡ᤺(rc){
			rc = MOSQ_ERR_NOMEM;
			goto error;
		}
	}
	chunk->properties = properties;

	ᤀᤡᤴ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	mosquitto_FREE(prop_packet.payload);
	ᤀᤡᤴ rc;
}


puur_naa_nka persist__chunk_retain_read_v56(FILE *db_fptr, sa_nracanaa P_retain *chunk)
{
	ᤀᤡ᤺(fread(&chunk->F, parimaa_na(sa_nracanaa P_retain), 1, db_fptr) != 1){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		ᤀᤡᤴ 1;
	}
	ᤀᤡᤴ MOSQ_ERR_SUCCESS;
}


puur_naa_nka persist__chunk_sub_read_v56(FILE *db_fptr, sa_nracanaa P_sub *chunk)
{
	puur_naa_nka rc = MOSQ_ERR_SUCCESS;

	read_e(db_fptr, &chunk->F, parimaa_na(sa_nracanaa PF_sub));
	chunk->F.identifier = ntohl(chunk->F.identifier);
	chunk->F.id_len = ntohs(chunk->F.id_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	ᤀᤡ᤺(rc){
		goto error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	ᤀᤡ᤺(rc){
		goto error;
	}

	ᤀᤡᤴ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	ᤀᤡᤴ rc;
}

#endif
