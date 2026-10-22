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

#अन्तर्भाव "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#अन्तर्भाव <arpa/inet.h>
#endif
#अन्तर्भाव <errno.h>
#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <time.h>

#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "persist.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "property_common.h"
#अन्तर्भाव "property_mosq.h"
#अन्तर्भाव "util_mosq.h"


पूर्णांक persist__chunk_cfg_write_v6(खाता *db_fptr, संरचना PF_cfg *chunk)
{
	संरचना PF_header header;

	header.chunk = htonl(DB_CHUNK_CFG);
	header.length = htonl(आकार(संरचना PF_cfg));
	write_e(db_fptr, &header, आकार(संरचना PF_header));
	write_e(db_fptr, chunk, आकार(संरचना PF_cfg));

	परत MOSQ_ERR_SUCCESS;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	परत 1;
}


पूर्णांक persist__chunk_client_write_v6(खाता *db_fptr, संरचना P_client *chunk)
{
	संरचना PF_header header;
	uint16_t id_len = chunk->F.id_len;
	uint16_t username_len = chunk->F.username_len;

	chunk->F.session_expiry_interval = htonl(chunk->F.session_expiry_interval);
	chunk->F.last_mid = htons(chunk->F.last_mid);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.username_len = htons(chunk->F.username_len);
	chunk->F.listener_port = htons(chunk->F.listener_port);

	header.chunk = htonl(DB_CHUNK_CLIENT);
	header.length = htonl((uint32_t)आकार(संरचना PF_client)+id_len+username_len);

	write_e(db_fptr, &header, आकार(संरचना PF_header));
	write_e(db_fptr, &chunk->F, आकार(संरचना PF_client));

	write_e(db_fptr, chunk->clientid, id_len);
	जर(username_len > 0){
		write_e(db_fptr, chunk->username, username_len);
	}

	परत MOSQ_ERR_SUCCESS;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	परत 1;
}


पूर्णांक persist_chunk_client_संदेश_write_v6(खाता *db_fptr, संरचना P_client_संदेश *chunk)
{
	संरचना PF_header header;
	संरचना mosquitto__packet *prop_packet = रिक्त;
	uint16_t id_len = chunk->F.id_len;
	uint32_t proplen = 0;
	पूर्णांक rc;
	mosquitto_property subscription_id_prop = {
		.next = रिक्त, .identifier = MQTT_PROP_SUBSCRIPTION_IDENTIFIER, .client_generated = true, .property_type = MQTT_PROP_TYPE_VARINT
	};

	जर(chunk->subscription_identifier){
		subscription_id_prop.मूल्य.varint = chunk->subscription_identifier;
		proplen += mosquitto_property_get_remaining_length(&subscription_id_prop);
	}

	chunk->F.mid = htons(chunk->F.mid);
	chunk->F.id_len = htons(chunk->F.id_len);

	header.chunk = htonl(DB_CHUNK_CLIENT_संदेश);
	header.length = htonl((uint32_t)आकार(संरचना PF_client_संदेश) + id_len + proplen);

	write_e(db_fptr, &header, आकार(संरचना PF_header));
	write_e(db_fptr, &chunk->F, आकार(संरचना PF_client_संदेश));
	write_e(db_fptr, chunk->clientid, id_len);
	जर(chunk->subscription_identifier){
		जर(proplen > 0){
			prop_packet = mosquitto_calloc(1, आकार(संरचना mosquitto__packet)+proplen);
			जर(prop_packet == रिक्त){
				परत MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_length = proplen;
			prop_packet->packet_length = proplen;
			rc = property__write_all(prop_packet, &subscription_id_prop, true);
			जर(rc){
				mosquitto_FREE(prop_packet);
				परत rc;
			}

			write_e(db_fptr, prop_packet->payload, proplen);
			mosquitto_FREE(prop_packet);
		}
	}

	परत MOSQ_ERR_SUCCESS;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	mosquitto_FREE(prop_packet);
	परत 1;
}


पूर्णांक persist__chunk_message_store_write_v6(खाता *db_fptr, संरचना P_base_संदेश *chunk)
{
	संरचना PF_header header;
	uint32_t payloadlen = chunk->F.payloadlen;
	uint16_t source_id_len = chunk->F.source_id_len;
	uint16_t source_username_len = chunk->F.source_username_len;
	uint16_t topic_len = chunk->F.topic_len;
	uint32_t proplen = 0;
	पूर्णांक rc;

	जर(chunk->properties){
		proplen += mosquitto_property_get_remaining_length(chunk->properties);
	}

	chunk->F.payloadlen = htonl(chunk->F.payloadlen);
	chunk->F.source_mid = htons(chunk->F.source_mid);
	chunk->F.source_id_len = htons(chunk->F.source_id_len);
	chunk->F.source_username_len = htons(chunk->F.source_username_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);
	chunk->F.source_port = htons(chunk->F.source_port);

	header.chunk = htonl(DB_CHUNK_BASE_संदेश);
	header.length = htonl((uint32_t)आकार(संरचना PF_base_संदेश) +
			topic_len + payloadlen +
			source_id_len + source_username_len + proplen);

	write_e(db_fptr, &header, आकार(संरचना PF_header));
	write_e(db_fptr, &chunk->F, आकार(संरचना PF_base_संदेश));
	जर(source_id_len){
		write_e(db_fptr, chunk->source.id, source_id_len);
	}
	जर(source_username_len){
		write_e(db_fptr, chunk->source.username, source_username_len);
	}
	write_e(db_fptr, chunk->topic, topic_len);
	जर(payloadlen){
		write_e(db_fptr, chunk->payload, (अचिन्हित पूर्णांक)payloadlen);
	}
	जर(chunk->properties){
		जर(proplen > 0){
			संरचना mosquitto__packet *prop_packet = mosquitto_calloc(1, आकार(संरचना mosquitto__packet)+proplen);
			जर(prop_packet == रिक्त){
				परत MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_length = proplen;
			prop_packet->packet_length = proplen;
			rc = property__write_all(prop_packet, chunk->properties, true);
			जर(rc){
				mosquitto_FREE(prop_packet);
				परत rc;
			}

			जर(ख_डालो(prop_packet->payload, 1, proplen, db_fptr) != proplen){
				mosquitto_FREE(prop_packet);
				जा error;
			}
			mosquitto_FREE(prop_packet);
		}
	}

	परत MOSQ_ERR_SUCCESS;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	परत 1;
}


पूर्णांक persist__chunk_retain_write_v6(खाता *db_fptr, संरचना P_retain *chunk)
{
	संरचना PF_header header;

	header.chunk = htonl(DB_CHUNK_RETAIN);
	header.length = htonl((uint32_t)आकार(संरचना PF_retain));

	write_e(db_fptr, &header, आकार(संरचना PF_header));
	write_e(db_fptr, &chunk->F, आकार(संरचना PF_retain));

	परत MOSQ_ERR_SUCCESS;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	परत 1;
}


पूर्णांक persist__chunk_sub_write_v6(खाता *db_fptr, संरचना P_sub *chunk)
{
	संरचना PF_header header;
	uint16_t id_len = chunk->F.id_len;
	uint16_t topic_len = chunk->F.topic_len;

	chunk->F.identifier = htonl(chunk->F.identifier);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);

	header.chunk = htonl(DB_CHUNK_SUB);
	header.length = htonl((uint32_t)आकार(संरचना PF_sub) +
			id_len + topic_len);

	write_e(db_fptr, &header, आकार(संरचना PF_header));
	write_e(db_fptr, &chunk->F, आकार(संरचना PF_sub));
	write_e(db_fptr, chunk->clientid, id_len);
	write_e(db_fptr, chunk->topic, topic_len);

	परत MOSQ_ERR_SUCCESS;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", म_त्रुटि(त्रुटि_सं));
	परत 1;
}
#endif
