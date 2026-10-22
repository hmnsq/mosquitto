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

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "persist.h"
#समावेश "property_mosq.h"
#समावेश "util_mosq.h"


पूर्णांक persist__chunk_header_read_v56(खाता *db_fptr, uint32_t *chunk, uint32_t *लंबाई)
{
	माप_प्रकार rlen;
	काष्ठा PF_header header;

	rlen = ख_पढ़ो(&header, माप(काष्ठा PF_header), 1, db_fptr);
	अगर(rlen != 1){
		वापस 1;
	}

	*chunk = ntohl(header.chunk);
	*लंबाई = ntohl(header.लंबाई);

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक persist__chunk_cfg_read_v56(खाता *db_fptr, काष्ठा PF_cfg *chunk)
{
	अगर(ख_पढ़ो(chunk, माप(काष्ठा PF_cfg), 1, db_fptr) != 1){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
		वापस 1;
	}

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक persist__chunk_client_read_v56(खाता *db_fptr, काष्ठा P_client *chunk, uint32_t db_version)
{
	पूर्णांक rc;

	अगर(db_version == 6){
		read_e(db_fptr, &chunk->F, माप(काष्ठा PF_client));
		chunk->F.username_len = ntohs(chunk->F.username_len);
		chunk->F.listener_port = ntohs(chunk->F.listener_port);
	}अन्यथा अगर(db_version == 5){
		read_e(db_fptr, &chunk->F, माप(काष्ठा PF_client_v5));
	}अन्यथा{
		वापस 1;
	}

	chunk->F.session_expiry_interval = ntohl(chunk->F.session_expiry_interval);
	chunk->F.last_mid = ntohs(chunk->F.last_mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);


	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	अगर(rc){
		वापस 1;
	}अन्यथा अगर(chunk->clientid == शून्य){
		वापस -1;
	}

	अगर(chunk->F.username_len > 0){
		rc = persist__read_string_len(db_fptr, &chunk->username, chunk->F.username_len);
		अगर(rc || !chunk->username){
			mosquitto_FREE(chunk->clientid);
			वापस 1;
		}
	}

	वापस MOSQ_ERR_SUCCESS;
error:
	log__printf(शून्य, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	वापस 1;
}


पूर्णांक persist_chunk_client_संदेश_read_v56(खाता *db_fptr, काष्ठा P_client_संदेश *chunk, uint32_t लंबाई)
{
	mosquitto_property *properties = शून्य, *p;
	काष्ठा mosquitto__packet_in prop_packet;
	पूर्णांक rc;

	स_रखो(&prop_packet, 0, माप(काष्ठा mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, माप(काष्ठा PF_client_संदेश));
	chunk->F.mid = ntohs(chunk->F.mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);

	लंबाई -= (uint32_t)(माप(काष्ठा PF_client_संदेश) + chunk->F.id_len);
	अगर(लंबाई > MQTT_MAX_PAYLOAD){
		जाओ error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	अगर(rc){
		वापस rc;
	}

	अगर(लंबाई > 0){
		prop_packet.remaining_लंबाई = लंबाई;
		prop_packet.payload = mosquitto_malloc(लंबाई);
		अगर(!prop_packet.payload){
			त्रुटि_सं = ENOMEM;
			जाओ error;
		}

		read_e(db_fptr, prop_packet.payload, लंबाई);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		अगर(rc){
			mosquitto_FREE(chunk->clientid);
			वापस rc;
		}

		अगर(properties){
			p = properties;
			जबतक(p){
				अगर(mosquitto_property_identifier(p) == MQTT_PROP_SUBSCRIPTION_IDENTIFIER){
					chunk->subscription_identifier = mosquitto_property_varint_मूल्य(p);
				}
				p = mosquitto_property_अग्र(p);
			}
			mosquitto_property_free_all(&properties);
		}
	}

	वापस MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(prop_packet.payload);
	log__printf(शून्य, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	वापस 1;
}


पूर्णांक persist_chunk_base_संदेश_read_v56(खाता *db_fptr, काष्ठा P_base_संदेश *chunk, uint32_t लंबाई)
{
	पूर्णांक rc = 0;
	mosquitto_property *properties = शून्य;
	काष्ठा mosquitto__packet_in prop_packet;

	स_रखो(&prop_packet, 0, माप(काष्ठा mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, माप(काष्ठा PF_base_संदेश));
	chunk->F.payloadlen = ntohl(chunk->F.payloadlen);
	अगर(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
		वापस MOSQ_ERR_INVAL;
	}
	chunk->F.source_mid = ntohs(chunk->F.source_mid);
	chunk->F.source_id_len = ntohs(chunk->F.source_id_len);
	chunk->F.source_username_len = ntohs(chunk->F.source_username_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);
	chunk->F.source_port = ntohs(chunk->F.source_port);

	लंबाई -= (uint32_t)(माप(काष्ठा PF_base_संदेश) + chunk->F.payloadlen + chunk->F.source_id_len + chunk->F.source_username_len + chunk->F.topic_len);
	अगर(लंबाई > MQTT_MAX_PAYLOAD){
		जाओ error;
	}

	अगर(chunk->F.source_id_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.id, chunk->F.source_id_len);
		अगर(rc){
			जाओ error;
		}
	}
	अगर(chunk->F.source_username_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.username, chunk->F.source_username_len);
		अगर(rc){
			जाओ error;
		}
	}
	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	अगर(rc){
		जाओ error;
	}

	अगर(chunk->F.payloadlen > 0){
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		अगर(chunk->payload == शून्य){
			rc = MOSQ_ERR_NOMEM;
			जाओ error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	अगर(लंबाई > 0){
		prop_packet.remaining_लंबाई = लंबाई;
		prop_packet.payload = mosquitto_malloc(लंबाई);
		अगर(!prop_packet.payload){
			rc = MOSQ_ERR_NOMEM;
			जाओ error;
		}
		read_e(db_fptr, prop_packet.payload, लंबाई);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		अगर(rc){
			rc = MOSQ_ERR_NOMEM;
			जाओ error;
		}
	}
	chunk->properties = properties;

	वापस MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	mosquitto_FREE(prop_packet.payload);
	वापस rc;
}


पूर्णांक persist__chunk_retain_read_v56(खाता *db_fptr, काष्ठा P_retain *chunk)
{
	अगर(ख_पढ़ो(&chunk->F, माप(काष्ठा P_retain), 1, db_fptr) != 1){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
		वापस 1;
	}
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक persist__chunk_sub_read_v56(खाता *db_fptr, काष्ठा P_sub *chunk)
{
	पूर्णांक rc = MOSQ_ERR_SUCCESS;

	read_e(db_fptr, &chunk->F, माप(काष्ठा PF_sub));
	chunk->F.identifier = ntohl(chunk->F.identifier);
	chunk->F.id_len = ntohs(chunk->F.id_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	अगर(rc){
		जाओ error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	अगर(rc){
		जाओ error;
	}

	वापस MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	वापस rc;
}

#पूर्ण_अगर
