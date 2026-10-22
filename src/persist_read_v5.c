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

#ضمن "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#ضمن <arpa/inet.h>
#endif
#ضمن <errno.h>
#ضمن <fcntl.h>
#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <sys/stat.h>
#ضمن <time.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "persist.h"
#ضمن "property_mosq.h"
#ضمن "util_mosq.h"


صحيح persist__chunk_header_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	size_t rlen;
	بنية PF_header header;

	rlen = fread(&header, حجم(بنية PF_header), 1, db_fptr);
	إذا(rlen != 1){
		أرجع 1;
	}

	*chunk = ntohl(header.chunk);
	*length = ntohl(header.length);

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح persist__chunk_cfg_read_v56(FILE *db_fptr, بنية PF_cfg *chunk)
{
	إذا(fread(chunk, حجم(بنية PF_cfg), 1, db_fptr) != 1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		أرجع 1;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح persist__chunk_client_read_v56(FILE *db_fptr, بنية P_client *chunk, uint32_t db_version)
{
	صحيح rc;

	إذا(db_version == 6){
		read_e(db_fptr, &chunk->F, حجم(بنية PF_client));
		chunk->F.username_len = ntohs(chunk->F.username_len);
		chunk->F.listener_port = ntohs(chunk->F.listener_port);
	}وإلا إذا(db_version == 5){
		read_e(db_fptr, &chunk->F, حجم(بنية PF_client_v5));
	}وإلا{
		أرجع 1;
	}

	chunk->F.session_expiry_interval = ntohl(chunk->F.session_expiry_interval);
	chunk->F.last_mid = ntohs(chunk->F.last_mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);


	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	إذا(rc){
		أرجع 1;
	}وإلا إذا(chunk->clientid == عدم){
		أرجع -1;
	}

	إذا(chunk->F.username_len > 0){
		rc = persist__read_string_len(db_fptr, &chunk->username, chunk->F.username_len);
		إذا(rc || !chunk->username){
			mosquitto_FREE(chunk->clientid);
			أرجع 1;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
error:
	log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	أرجع 1;
}


صحيح persist_chunk_client_ئۇچۇر_read_v56(FILE *db_fptr, بنية P_client_ئۇچۇر *chunk, uint32_t length)
{
	mosquitto_property *properties = عدم, *p;
	بنية mosquitto__packet_in prop_packet;
	صحيح rc;

	memset(&prop_packet, 0, حجم(بنية mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, حجم(بنية PF_client_ئۇچۇر));
	chunk->F.mid = ntohs(chunk->F.mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);

	length -= (uint32_t)(حجم(بنية PF_client_ئۇچۇر) + chunk->F.id_len);
	إذا(length > MQTT_MAX_PAYLOAD){
		اذهب error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	إذا(rc){
		أرجع rc;
	}

	إذا(length > 0){
		prop_packet.remaining_length = length;
		prop_packet.payload = mosquitto_malloc(length);
		إذا(!prop_packet.payload){
			errno = ENOMEM;
			اذهب error;
		}

		read_e(db_fptr, prop_packet.payload, length);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		إذا(rc){
			mosquitto_FREE(chunk->clientid);
			أرجع rc;
		}

		إذا(properties){
			p = properties;
			طالما(p){
				إذا(mosquitto_property_identifier(p) == MQTT_PROP_SUBSCRIPTION_IDENTIFIER){
					chunk->subscription_identifier = mosquitto_property_varint_قىممەت(p);
				}
				p = mosquitto_property_next(p);
			}
			mosquitto_property_free_all(&properties);
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(prop_packet.payload);
	log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	أرجع 1;
}


صحيح persist_chunk_base_ئۇچۇر_read_v56(FILE *db_fptr, بنية P_base_ئۇچۇر *chunk, uint32_t length)
{
	صحيح rc = 0;
	mosquitto_property *properties = عدم;
	بنية mosquitto__packet_in prop_packet;

	memset(&prop_packet, 0, حجم(بنية mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, حجم(بنية PF_base_ئۇچۇر));
	chunk->F.payloadlen = ntohl(chunk->F.payloadlen);
	إذا(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
		أرجع MOSQ_ERR_INVAL;
	}
	chunk->F.source_mid = ntohs(chunk->F.source_mid);
	chunk->F.source_id_len = ntohs(chunk->F.source_id_len);
	chunk->F.source_username_len = ntohs(chunk->F.source_username_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);
	chunk->F.source_port = ntohs(chunk->F.source_port);

	length -= (uint32_t)(حجم(بنية PF_base_ئۇچۇر) + chunk->F.payloadlen + chunk->F.source_id_len + chunk->F.source_username_len + chunk->F.topic_len);
	إذا(length > MQTT_MAX_PAYLOAD){
		اذهب error;
	}

	إذا(chunk->F.source_id_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.id, chunk->F.source_id_len);
		إذا(rc){
			اذهب error;
		}
	}
	إذا(chunk->F.source_username_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.username, chunk->F.source_username_len);
		إذا(rc){
			اذهب error;
		}
	}
	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	إذا(rc){
		اذهب error;
	}

	إذا(chunk->F.payloadlen > 0){
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		إذا(chunk->payload == عدم){
			rc = MOSQ_ERR_NOMEM;
			اذهب error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	إذا(length > 0){
		prop_packet.remaining_length = length;
		prop_packet.payload = mosquitto_malloc(length);
		إذا(!prop_packet.payload){
			rc = MOSQ_ERR_NOMEM;
			اذهب error;
		}
		read_e(db_fptr, prop_packet.payload, length);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		إذا(rc){
			rc = MOSQ_ERR_NOMEM;
			اذهب error;
		}
	}
	chunk->properties = properties;

	أرجع MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	mosquitto_FREE(prop_packet.payload);
	أرجع rc;
}


صحيح persist__chunk_retain_read_v56(FILE *db_fptr, بنية P_retain *chunk)
{
	إذا(fread(&chunk->F, حجم(بنية P_retain), 1, db_fptr) != 1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		أرجع 1;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح persist__chunk_sub_read_v56(FILE *db_fptr, بنية P_sub *chunk)
{
	صحيح rc = MOSQ_ERR_SUCCESS;

	read_e(db_fptr, &chunk->F, حجم(بنية PF_sub));
	chunk->F.identifier = ntohl(chunk->F.identifier);
	chunk->F.id_len = ntohs(chunk->F.id_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	إذا(rc){
		اذهب error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	إذا(rc){
		اذهب error;
	}

	أرجع MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	أرجع rc;
}

#endif
