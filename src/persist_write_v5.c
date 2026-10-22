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

#समावेश "config.h"

#अगर_घोषित WITH_PERSISTENCE

#अगर_अघोषित WIN32
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#समावेश <errno.h>
#समावेश <fcntl.h>
#समावेश <stdio.h>
#समावेश <string.h>
#समावेश <sys/stat.h>
#समावेश <time.h>

#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "mosquitto_broker_internal.h"
#समावेश "persist.h"
#समावेश "packet_mosq.h"
#समावेश "property_common.h"
#समावेश "property_mosq.h"
#समावेश "util_mosq.h"


पूर्णांक persist__chunk_cfg_write_v6(खाता *db_fptr, काष्ठा PF_cfg *chunk)
{
	काष्ठा PF_header header;

	header.chunk = htonl(DB_CHUNK_CFG);
	header.लंबाई = htonl(माप(काष्ठा PF_cfg));
	write_e(db_fptr, &header, माप(काष्ठा PF_header));
	write_e(db_fptr, chunk, माप(काष्ठा PF_cfg));

	वापस MOSQ_ERR_SUCCESS;
error:
	log__printf(शून्य, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	वापस 1;
}


पूर्णांक persist__chunk_client_write_v6(खाता *db_fptr, काष्ठा P_client *chunk)
{
	काष्ठा PF_header header;
	uint16_t id_len = chunk->F.id_len;
	uint16_t username_len = chunk->F.username_len;

	chunk->F.session_expiry_interval = htonl(chunk->F.session_expiry_interval);
	chunk->F.last_mid = htons(chunk->F.last_mid);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.username_len = htons(chunk->F.username_len);
	chunk->F.listener_port = htons(chunk->F.listener_port);

	header.chunk = htonl(DB_CHUNK_CLIENT);
	header.लंबाई = htonl((uint32_t)माप(काष्ठा PF_client)+id_len+username_len);

	write_e(db_fptr, &header, माप(काष्ठा PF_header));
	write_e(db_fptr, &chunk->F, माप(काष्ठा PF_client));

	write_e(db_fptr, chunk->clientid, id_len);
	अगर(username_len > 0){
		write_e(db_fptr, chunk->username, username_len);
	}

	वापस MOSQ_ERR_SUCCESS;
error:
	log__printf(शून्य, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	वापस 1;
}


पूर्णांक persist_chunk_client_संदेश_write_v6(खाता *db_fptr, काष्ठा P_client_संदेश *chunk)
{
	काष्ठा PF_header header;
	काष्ठा mosquitto__packet *prop_packet = शून्य;
	uint16_t id_len = chunk->F.id_len;
	uint32_t proplen = 0;
	पूर्णांक rc;
	mosquitto_property subscription_id_prop = {
		.अग्र = शून्य, .identifier = MQTT_PROP_SUBSCRIPTION_IDENTIFIER, .client_generated = true, .property_type = MQTT_PROP_TYPE_VARINT
	};

	अगर(chunk->subscription_identifier){
		subscription_id_prop.मूल्य.varint = chunk->subscription_identifier;
		proplen += mosquitto_property_get_remaining_लंबाई(&subscription_id_prop);
	}

	chunk->F.mid = htons(chunk->F.mid);
	chunk->F.id_len = htons(chunk->F.id_len);

	header.chunk = htonl(DB_CHUNK_CLIENT_संदेश);
	header.लंबाई = htonl((uint32_t)माप(काष्ठा PF_client_संदेश) + id_len + proplen);

	write_e(db_fptr, &header, माप(काष्ठा PF_header));
	write_e(db_fptr, &chunk->F, माप(काष्ठा PF_client_संदेश));
	write_e(db_fptr, chunk->clientid, id_len);
	अगर(chunk->subscription_identifier){
		अगर(proplen > 0){
			prop_packet = mosquitto_calloc(1, माप(काष्ठा mosquitto__packet)+proplen);
			अगर(prop_packet == शून्य){
				वापस MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_लंबाई = proplen;
			prop_packet->packet_लंबाई = proplen;
			rc = property__write_all(prop_packet, &subscription_id_prop, true);
			अगर(rc){
				mosquitto_FREE(prop_packet);
				वापस rc;
			}

			write_e(db_fptr, prop_packet->payload, proplen);
			mosquitto_FREE(prop_packet);
		}
	}

	वापस MOSQ_ERR_SUCCESS;
error:
	log__printf(शून्य, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	mosquitto_FREE(prop_packet);
	वापस 1;
}


पूर्णांक persist__chunk_message_store_write_v6(खाता *db_fptr, काष्ठा P_base_संदेश *chunk)
{
	काष्ठा PF_header header;
	uint32_t payloadlen = chunk->F.payloadlen;
	uint16_t source_id_len = chunk->F.source_id_len;
	uint16_t source_username_len = chunk->F.source_username_len;
	uint16_t topic_len = chunk->F.topic_len;
	uint32_t proplen = 0;
	पूर्णांक rc;

	अगर(chunk->properties){
		proplen += mosquitto_property_get_remaining_लंबाई(chunk->properties);
	}

	chunk->F.payloadlen = htonl(chunk->F.payloadlen);
	chunk->F.source_mid = htons(chunk->F.source_mid);
	chunk->F.source_id_len = htons(chunk->F.source_id_len);
	chunk->F.source_username_len = htons(chunk->F.source_username_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);
	chunk->F.source_port = htons(chunk->F.source_port);

	header.chunk = htonl(DB_CHUNK_BASE_संदेश);
	header.लंबाई = htonl((uint32_t)माप(काष्ठा PF_base_संदेश) +
			topic_len + payloadlen +
			source_id_len + source_username_len + proplen);

	write_e(db_fptr, &header, माप(काष्ठा PF_header));
	write_e(db_fptr, &chunk->F, माप(काष्ठा PF_base_संदेश));
	अगर(source_id_len){
		write_e(db_fptr, chunk->source.id, source_id_len);
	}
	अगर(source_username_len){
		write_e(db_fptr, chunk->source.username, source_username_len);
	}
	write_e(db_fptr, chunk->topic, topic_len);
	अगर(payloadlen){
		write_e(db_fptr, chunk->payload, (अचिन्हित पूर्णांक)payloadlen);
	}
	अगर(chunk->properties){
		अगर(proplen > 0){
			काष्ठा mosquitto__packet *prop_packet = mosquitto_calloc(1, माप(काष्ठा mosquitto__packet)+proplen);
			अगर(prop_packet == शून्य){
				वापस MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_लंबाई = proplen;
			prop_packet->packet_लंबाई = proplen;
			rc = property__write_all(prop_packet, chunk->properties, true);
			अगर(rc){
				mosquitto_FREE(prop_packet);
				वापस rc;
			}

			अगर(ख_डालो(prop_packet->payload, 1, proplen, db_fptr) != proplen){
				mosquitto_FREE(prop_packet);
				जाओ error;
			}
			mosquitto_FREE(prop_packet);
		}
	}

	वापस MOSQ_ERR_SUCCESS;
error:
	log__printf(शून्य, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	वापस 1;
}


पूर्णांक persist__chunk_retain_write_v6(खाता *db_fptr, काष्ठा P_retain *chunk)
{
	काष्ठा PF_header header;

	header.chunk = htonl(DB_CHUNK_RETAIN);
	header.लंबाई = htonl((uint32_t)माप(काष्ठा PF_retain));

	write_e(db_fptr, &header, माप(काष्ठा PF_header));
	write_e(db_fptr, &chunk->F, माप(काष्ठा PF_retain));

	वापस MOSQ_ERR_SUCCESS;
error:
	log__printf(शून्य, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	वापस 1;
}


पूर्णांक persist__chunk_sub_write_v6(खाता *db_fptr, काष्ठा P_sub *chunk)
{
	काष्ठा PF_header header;
	uint16_t id_len = chunk->F.id_len;
	uint16_t topic_len = chunk->F.topic_len;

	chunk->F.identifier = htonl(chunk->F.identifier);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);

	header.chunk = htonl(DB_CHUNK_SUB);
	header.लंबाई = htonl((uint32_t)माप(काष्ठा PF_sub) +
			id_len + topic_len);

	write_e(db_fptr, &header, माप(काष्ठा PF_header));
	write_e(db_fptr, &chunk->F, माप(काष्ठा PF_sub));
	write_e(db_fptr, chunk->clientid, id_len);
	write_e(db_fptr, chunk->topic, topic_len);

	वापस MOSQ_ERR_SUCCESS;
error:
	log__printf(शून्य, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	वापस 1;
}
#पूर्ण_अगर
