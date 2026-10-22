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

#અન્તર્ભાવ "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#અન્તર્ભાવ <arpa/inet.h>
#endif
#અન્તર્ભાવ <errno.h>
#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <time.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "persist.h"
#અન્તર્ભાવ "property_mosq.h"
#અન્તર્ભાવ "util_mosq.h"


પૂર્ણાંક persist__chunk_header_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	size_t rlen;
	સંરચના PF_header header;

	rlen = fread(&header, કદ(સંરચના PF_header), 1, db_fptr);
	જો(rlen != 1){
		પાછા 1;
	}

	*chunk = ntohl(header.chunk);
	*length = ntohl(header.length);

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક persist__chunk_cfg_read_v56(FILE *db_fptr, સંરચના PF_cfg *chunk)
{
	જો(fread(chunk, કદ(સંરચના PF_cfg), 1, db_fptr) != 1){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		પાછા 1;
	}

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક persist__chunk_client_read_v56(FILE *db_fptr, સંરચના P_client *chunk, uint32_t db_version)
{
	પૂર્ણાંક rc;

	જો(db_version == 6){
		read_e(db_fptr, &chunk->F, કદ(સંરચના PF_client));
		chunk->F.username_len = ntohs(chunk->F.username_len);
		chunk->F.listener_port = ntohs(chunk->F.listener_port);
	}નહીંતર જો(db_version == 5){
		read_e(db_fptr, &chunk->F, કદ(સંરચના PF_client_v5));
	}નહીંતર{
		પાછા 1;
	}

	chunk->F.session_expiry_interval = ntohl(chunk->F.session_expiry_interval);
	chunk->F.last_mid = ntohs(chunk->F.last_mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);


	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	જો(rc){
		પાછા 1;
	}નહીંતર જો(chunk->clientid == ખાલી){
		પાછા -1;
	}

	જો(chunk->F.username_len > 0){
		rc = persist__read_string_len(db_fptr, &chunk->username, chunk->F.username_len);
		જો(rc || !chunk->username){
			mosquitto_FREE(chunk->clientid);
			પાછા 1;
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
error:
	log__printf(ખાલી, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	પાછા 1;
}


પૂર્ણાંક persist_chunk_client_સંદેશ_read_v56(FILE *db_fptr, સંરચના P_client_સંદેશ *chunk, uint32_t length)
{
	mosquitto_property *properties = ખાલી, *p;
	સંરચના mosquitto__packet_in prop_packet;
	પૂર્ણાંક rc;

	memset(&prop_packet, 0, કદ(સંરચના mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, કદ(સંરચના PF_client_સંદેશ));
	chunk->F.mid = ntohs(chunk->F.mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);

	length -= (uint32_t)(કદ(સંરચના PF_client_સંદેશ) + chunk->F.id_len);
	જો(length > MQTT_MAX_PAYLOAD){
		જાઓ error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	જો(rc){
		પાછા rc;
	}

	જો(length > 0){
		prop_packet.remaining_length = length;
		prop_packet.payload = mosquitto_malloc(length);
		જો(!prop_packet.payload){
			errno = ENOMEM;
			જાઓ error;
		}

		read_e(db_fptr, prop_packet.payload, length);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		જો(rc){
			mosquitto_FREE(chunk->clientid);
			પાછા rc;
		}

		જો(properties){
			p = properties;
			જ્યારે(p){
				જો(mosquitto_property_identifier(p) == MQTT_PROP_SUBSCRIPTION_IDENTIFIER){
					chunk->subscription_identifier = mosquitto_property_varint_કિંમત(p);
				}
				p = mosquitto_property_next(p);
			}
			mosquitto_property_free_all(&properties);
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(prop_packet.payload);
	log__printf(ખાલી, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	પાછા 1;
}


પૂર્ણાંક persist_chunk_base_સંદેશ_read_v56(FILE *db_fptr, સંરચના P_base_સંદેશ *chunk, uint32_t length)
{
	પૂર્ણાંક rc = 0;
	mosquitto_property *properties = ખાલી;
	સંરચના mosquitto__packet_in prop_packet;

	memset(&prop_packet, 0, કદ(સંરચના mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, કદ(સંરચના PF_base_સંદેશ));
	chunk->F.payloadlen = ntohl(chunk->F.payloadlen);
	જો(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
		પાછા MOSQ_ERR_INVAL;
	}
	chunk->F.source_mid = ntohs(chunk->F.source_mid);
	chunk->F.source_id_len = ntohs(chunk->F.source_id_len);
	chunk->F.source_username_len = ntohs(chunk->F.source_username_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);
	chunk->F.source_port = ntohs(chunk->F.source_port);

	length -= (uint32_t)(કદ(સંરચના PF_base_સંદેશ) + chunk->F.payloadlen + chunk->F.source_id_len + chunk->F.source_username_len + chunk->F.topic_len);
	જો(length > MQTT_MAX_PAYLOAD){
		જાઓ error;
	}

	જો(chunk->F.source_id_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.id, chunk->F.source_id_len);
		જો(rc){
			જાઓ error;
		}
	}
	જો(chunk->F.source_username_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.username, chunk->F.source_username_len);
		જો(rc){
			જાઓ error;
		}
	}
	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	જો(rc){
		જાઓ error;
	}

	જો(chunk->F.payloadlen > 0){
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		જો(chunk->payload == ખાલી){
			rc = MOSQ_ERR_NOMEM;
			જાઓ error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	જો(length > 0){
		prop_packet.remaining_length = length;
		prop_packet.payload = mosquitto_malloc(length);
		જો(!prop_packet.payload){
			rc = MOSQ_ERR_NOMEM;
			જાઓ error;
		}
		read_e(db_fptr, prop_packet.payload, length);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		જો(rc){
			rc = MOSQ_ERR_NOMEM;
			જાઓ error;
		}
	}
	chunk->properties = properties;

	પાછા MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	mosquitto_FREE(prop_packet.payload);
	પાછા rc;
}


પૂર્ણાંક persist__chunk_retain_read_v56(FILE *db_fptr, સંરચના P_retain *chunk)
{
	જો(fread(&chunk->F, કદ(સંરચના P_retain), 1, db_fptr) != 1){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		પાછા 1;
	}
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક persist__chunk_sub_read_v56(FILE *db_fptr, સંરચના P_sub *chunk)
{
	પૂર્ણાંક rc = MOSQ_ERR_SUCCESS;

	read_e(db_fptr, &chunk->F, કદ(સંરચના PF_sub));
	chunk->F.identifier = ntohl(chunk->F.identifier);
	chunk->F.id_len = ntohs(chunk->F.id_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	જો(rc){
		જાઓ error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	જો(rc){
		જાઓ error;
	}

	પાછા MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	પાછા rc;
}

#endif
