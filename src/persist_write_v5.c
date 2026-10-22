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

#यदि_परिभाषितम् WITH_PERSISTENCE

#यदि_अपरिभाषितम् WIN32
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
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


पूर्णाङ्क persist__chunk_cfg_write_v6(FILE *db_fptr, संरचना PF_cfg *chunk)
{
	संरचना PF_शीर्षकम् शीर्षकम्;

	शीर्षकम्.chunk = htonl(DB_CHUNK_CFG);
	शीर्षकम्.दीर्घता = htonl(परिमाण(संरचना PF_cfg));
	write_e(db_fptr, &शीर्षकम्, परिमाण(संरचना PF_शीर्षकम्));
	write_e(db_fptr, chunk, परिमाण(संरचना PF_cfg));

	प्रत्यागम MOSQ_ERR_SUCCESS;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	प्रत्यागम 1;
}


पूर्णाङ्क persist__chunk_client_write_v6(FILE *db_fptr, संरचना P_client *chunk)
{
	संरचना PF_शीर्षकम् शीर्षकम्;
	uint16_t id_len = chunk->F.id_len;
	uint16_t username_len = chunk->F.username_len;

	chunk->F.session_expiry_interval = htonl(chunk->F.session_expiry_interval);
	chunk->F.चरमः_mid = htons(chunk->F.चरमः_mid);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.username_len = htons(chunk->F.username_len);
	chunk->F.listener_पत्तनम् = htons(chunk->F.listener_पत्तनम्);

	शीर्षकम्.chunk = htonl(DB_CHUNK_CLIENT);
	शीर्षकम्.दीर्घता = htonl((uint32_t)परिमाण(संरचना PF_client)+id_len+username_len);

	write_e(db_fptr, &शीर्षकम्, परिमाण(संरचना PF_शीर्षकम्));
	write_e(db_fptr, &chunk->F, परिमाण(संरचना PF_client));

	write_e(db_fptr, chunk->clientid, id_len);
	यदि(username_len > 0){
		write_e(db_fptr, chunk->username, username_len);
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	प्रत्यागम 1;
}


पूर्णाङ्क persist_chunk_client_सन्देशः_write_v6(FILE *db_fptr, संरचना P_client_सन्देशः *chunk)
{
	संरचना PF_शीर्षकम् शीर्षकम्;
	संरचना mosquitto__packet *prop_packet = रिक्त;
	uint16_t id_len = chunk->F.id_len;
	uint32_t proplen = 0;
	पूर्णाङ्क rc;
	mosquitto_property subscription_id_prop = {
		.अग्र = रिक्त, .identifier = MQTT_PROP_SUBSCRIPTION_IDENTIFIER, .client_generated = true, .property_type = MQTT_PROP_TYPE_VARINT
	};

	यदि(chunk->subscription_identifier){
		subscription_id_prop.मानम्.varint = chunk->subscription_identifier;
		proplen += mosquitto_property_get_remaining_दीर्घता(&subscription_id_prop);
	}

	chunk->F.mid = htons(chunk->F.mid);
	chunk->F.id_len = htons(chunk->F.id_len);

	शीर्षकम्.chunk = htonl(DB_CHUNK_CLIENT_सन्देशः);
	शीर्षकम्.दीर्घता = htonl((uint32_t)परिमाण(संरचना PF_client_सन्देशः) + id_len + proplen);

	write_e(db_fptr, &शीर्षकम्, परिमाण(संरचना PF_शीर्षकम्));
	write_e(db_fptr, &chunk->F, परिमाण(संरचना PF_client_सन्देशः));
	write_e(db_fptr, chunk->clientid, id_len);
	यदि(chunk->subscription_identifier){
		यदि(proplen > 0){
			prop_packet = mosquitto_calloc(1, परिमाण(संरचना mosquitto__packet)+proplen);
			यदि(prop_packet == रिक्त){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_दीर्घता = proplen;
			prop_packet->packet_दीर्घता = proplen;
			rc = property__write_all(prop_packet, &subscription_id_prop, true);
			यदि(rc){
				mosquitto_FREE(prop_packet);
				प्रत्यागम rc;
			}

			write_e(db_fptr, prop_packet->payload, proplen);
			mosquitto_FREE(prop_packet);
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(prop_packet);
	प्रत्यागम 1;
}


पूर्णाङ्क persist_chunk_सन्देशः_store_write_v6(FILE *db_fptr, संरचना P_आधारः_सन्देशः *chunk)
{
	संरचना PF_शीर्षकम् शीर्षकम्;
	uint32_t payloadlen = chunk->F.payloadlen;
	uint16_t source_id_len = chunk->F.source_id_len;
	uint16_t source_username_len = chunk->F.source_username_len;
	uint16_t topic_len = chunk->F.topic_len;
	uint32_t proplen = 0;
	पूर्णाङ्क rc;

	यदि(chunk->properties){
		proplen += mosquitto_property_get_remaining_दीर्घता(chunk->properties);
	}

	chunk->F.payloadlen = htonl(chunk->F.payloadlen);
	chunk->F.source_mid = htons(chunk->F.source_mid);
	chunk->F.source_id_len = htons(chunk->F.source_id_len);
	chunk->F.source_username_len = htons(chunk->F.source_username_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);
	chunk->F.source_पत्तनम् = htons(chunk->F.source_पत्तनम्);

	शीर्षकम्.chunk = htonl(DB_CHUNK_आधारः_सन्देशः);
	शीर्षकम्.दीर्घता = htonl((uint32_t)परिमाण(संरचना PF_आधारः_सन्देशः) +
			topic_len + payloadlen +
			source_id_len + source_username_len + proplen);

	write_e(db_fptr, &शीर्षकम्, परिमाण(संरचना PF_शीर्षकम्));
	write_e(db_fptr, &chunk->F, परिमाण(संरचना PF_आधारः_सन्देशः));
	यदि(source_id_len){
		write_e(db_fptr, chunk->source.id, source_id_len);
	}
	यदि(source_username_len){
		write_e(db_fptr, chunk->source.username, source_username_len);
	}
	write_e(db_fptr, chunk->topic, topic_len);
	यदि(payloadlen){
		write_e(db_fptr, chunk->payload, (अचिह्नित पूर्णाङ्क)payloadlen);
	}
	यदि(chunk->properties){
		यदि(proplen > 0){
			संरचना mosquitto__packet *prop_packet = mosquitto_calloc(1, परिमाण(संरचना mosquitto__packet)+proplen);
			यदि(prop_packet == रिक्त){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_दीर्घता = proplen;
			prop_packet->packet_दीर्घता = proplen;
			rc = property__write_all(prop_packet, chunk->properties, true);
			यदि(rc){
				mosquitto_FREE(prop_packet);
				प्रत्यागम rc;
			}

			यदि(fwrite(prop_packet->payload, 1, proplen, db_fptr) != proplen){
				mosquitto_FREE(prop_packet);
				गच्छ error;
			}
			mosquitto_FREE(prop_packet);
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	प्रत्यागम 1;
}


पूर्णाङ्क persist__chunk_retain_write_v6(FILE *db_fptr, संरचना P_retain *chunk)
{
	संरचना PF_शीर्षकम् शीर्षकम्;

	शीर्षकम्.chunk = htonl(DB_CHUNK_RETAIN);
	शीर्षकम्.दीर्घता = htonl((uint32_t)परिमाण(संरचना PF_retain));

	write_e(db_fptr, &शीर्षकम्, परिमाण(संरचना PF_शीर्षकम्));
	write_e(db_fptr, &chunk->F, परिमाण(संरचना PF_retain));

	प्रत्यागम MOSQ_ERR_SUCCESS;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	प्रत्यागम 1;
}


पूर्णाङ्क persist__chunk_sub_write_v6(FILE *db_fptr, संरचना P_sub *chunk)
{
	संरचना PF_शीर्षकम् शीर्षकम्;
	uint16_t id_len = chunk->F.id_len;
	uint16_t topic_len = chunk->F.topic_len;

	chunk->F.identifier = htonl(chunk->F.identifier);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);

	शीर्षकम्.chunk = htonl(DB_CHUNK_SUB);
	शीर्षकम्.दीर्घता = htonl((uint32_t)परिमाण(संरचना PF_sub) +
			id_len + topic_len);

	write_e(db_fptr, &शीर्षकम्, परिमाण(संरचना PF_शीर्षकम्));
	write_e(db_fptr, &chunk->F, परिमाण(संरचना PF_sub));
	write_e(db_fptr, chunk->clientid, id_len);
	write_e(db_fptr, chunk->topic, topic_len);

	प्रत्यागम MOSQ_ERR_SUCCESS;
error:
	log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	प्रत्यागम 1;
}
#यदि_अन्तः
