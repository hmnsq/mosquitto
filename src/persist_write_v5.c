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

#অন্তর্ভাব "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#অন্তর্ভাব <arpa/inet.h>
#endif
#অন্তর্ভাব <errno.h>
#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <time.h>

#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "persist.h"
#অন্তর্ভাব "packet_mosq.h"
#অন্তর্ভাব "property_common.h"
#অন্তর্ভাব "property_mosq.h"
#অন্তর্ভাব "util_mosq.h"


পূর্ণসংখ্যা persist__chunk_cfg_write_v6(FILE *db_fptr, কাঠামো PF_cfg *chunk)
{
	কাঠামো PF_header header;

	header.chunk = htonl(DB_CHUNK_CFG);
	header.দৈর্ঘ্য = htonl(আকার(কাঠামো PF_cfg));
	write_e(db_fptr, &header, আকার(কাঠামো PF_header));
	write_e(db_fptr, chunk, আকার(কাঠামো PF_cfg));

	ফেরত MOSQ_ERR_SUCCESS;
error:
	log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ফেরত 1;
}


পূর্ণসংখ্যা persist__chunk_client_write_v6(FILE *db_fptr, কাঠামো P_client *chunk)
{
	কাঠামো PF_header header;
	uint16_t id_len = chunk->F.id_len;
	uint16_t username_len = chunk->F.username_len;

	chunk->F.session_expiry_interval = htonl(chunk->F.session_expiry_interval);
	chunk->F.last_mid = htons(chunk->F.last_mid);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.username_len = htons(chunk->F.username_len);
	chunk->F.listener_port = htons(chunk->F.listener_port);

	header.chunk = htonl(DB_CHUNK_CLIENT);
	header.দৈর্ঘ্য = htonl((uint32_t)আকার(কাঠামো PF_client)+id_len+username_len);

	write_e(db_fptr, &header, আকার(কাঠামো PF_header));
	write_e(db_fptr, &chunk->F, আকার(কাঠামো PF_client));

	write_e(db_fptr, chunk->clientid, id_len);
	যদি(username_len > 0){
		write_e(db_fptr, chunk->username, username_len);
	}

	ফেরত MOSQ_ERR_SUCCESS;
error:
	log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ফেরত 1;
}


পূর্ণসংখ্যা persist_chunk_client_বার্তা_write_v6(FILE *db_fptr, কাঠামো P_client_বার্তা *chunk)
{
	কাঠামো PF_header header;
	কাঠামো mosquitto__packet *prop_packet = ফাঁকা;
	uint16_t id_len = chunk->F.id_len;
	uint32_t proplen = 0;
	পূর্ণসংখ্যা rc;
	mosquitto_property subscription_id_prop = {
		.পরবর্তী = ফাঁকা, .identifier = MQTT_PROP_SUBSCRIPTION_IDENTIFIER, .client_generated = true, .property_type = MQTT_PROP_TYPE_VARINT
	};

	যদি(chunk->subscription_identifier){
		subscription_id_prop.মান.varint = chunk->subscription_identifier;
		proplen += mosquitto_property_get_remaining_দৈর্ঘ্য(&subscription_id_prop);
	}

	chunk->F.mid = htons(chunk->F.mid);
	chunk->F.id_len = htons(chunk->F.id_len);

	header.chunk = htonl(DB_CHUNK_CLIENT_বার্তা);
	header.দৈর্ঘ্য = htonl((uint32_t)আকার(কাঠামো PF_client_বার্তা) + id_len + proplen);

	write_e(db_fptr, &header, আকার(কাঠামো PF_header));
	write_e(db_fptr, &chunk->F, আকার(কাঠামো PF_client_বার্তা));
	write_e(db_fptr, chunk->clientid, id_len);
	যদি(chunk->subscription_identifier){
		যদি(proplen > 0){
			prop_packet = mosquitto_calloc(1, আকার(কাঠামো mosquitto__packet)+proplen);
			যদি(prop_packet == ফাঁকা){
				ফেরত MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_দৈর্ঘ্য = proplen;
			prop_packet->packet_দৈর্ঘ্য = proplen;
			rc = property__write_all(prop_packet, &subscription_id_prop, true);
			যদি(rc){
				mosquitto_FREE(prop_packet);
				ফেরত rc;
			}

			write_e(db_fptr, prop_packet->payload, proplen);
			mosquitto_FREE(prop_packet);
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
error:
	log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(prop_packet);
	ফেরত 1;
}


পূর্ণসংখ্যা persist__chunk_message_store_write_v6(FILE *db_fptr, কাঠামো P_base_বার্তা *chunk)
{
	কাঠামো PF_header header;
	uint32_t payloadlen = chunk->F.payloadlen;
	uint16_t source_id_len = chunk->F.source_id_len;
	uint16_t source_username_len = chunk->F.source_username_len;
	uint16_t topic_len = chunk->F.topic_len;
	uint32_t proplen = 0;
	পূর্ণসংখ্যা rc;

	যদি(chunk->properties){
		proplen += mosquitto_property_get_remaining_দৈর্ঘ্য(chunk->properties);
	}

	chunk->F.payloadlen = htonl(chunk->F.payloadlen);
	chunk->F.source_mid = htons(chunk->F.source_mid);
	chunk->F.source_id_len = htons(chunk->F.source_id_len);
	chunk->F.source_username_len = htons(chunk->F.source_username_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);
	chunk->F.source_port = htons(chunk->F.source_port);

	header.chunk = htonl(DB_CHUNK_BASE_বার্তা);
	header.দৈর্ঘ্য = htonl((uint32_t)আকার(কাঠামো PF_base_বার্তা) +
			topic_len + payloadlen +
			source_id_len + source_username_len + proplen);

	write_e(db_fptr, &header, আকার(কাঠামো PF_header));
	write_e(db_fptr, &chunk->F, আকার(কাঠামো PF_base_বার্তা));
	যদি(source_id_len){
		write_e(db_fptr, chunk->source.id, source_id_len);
	}
	যদি(source_username_len){
		write_e(db_fptr, chunk->source.username, source_username_len);
	}
	write_e(db_fptr, chunk->topic, topic_len);
	যদি(payloadlen){
		write_e(db_fptr, chunk->payload, (unsigned পূর্ণসংখ্যা)payloadlen);
	}
	যদি(chunk->properties){
		যদি(proplen > 0){
			কাঠামো mosquitto__packet *prop_packet = mosquitto_calloc(1, আকার(কাঠামো mosquitto__packet)+proplen);
			যদি(prop_packet == ফাঁকা){
				ফেরত MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_দৈর্ঘ্য = proplen;
			prop_packet->packet_দৈর্ঘ্য = proplen;
			rc = property__write_all(prop_packet, chunk->properties, true);
			যদি(rc){
				mosquitto_FREE(prop_packet);
				ফেরত rc;
			}

			যদি(fwrite(prop_packet->payload, 1, proplen, db_fptr) != proplen){
				mosquitto_FREE(prop_packet);
				যাও error;
			}
			mosquitto_FREE(prop_packet);
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
error:
	log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ফেরত 1;
}


পূর্ণসংখ্যা persist__chunk_retain_write_v6(FILE *db_fptr, কাঠামো P_retain *chunk)
{
	কাঠামো PF_header header;

	header.chunk = htonl(DB_CHUNK_RETAIN);
	header.দৈর্ঘ্য = htonl((uint32_t)আকার(কাঠামো PF_retain));

	write_e(db_fptr, &header, আকার(কাঠামো PF_header));
	write_e(db_fptr, &chunk->F, আকার(কাঠামো PF_retain));

	ফেরত MOSQ_ERR_SUCCESS;
error:
	log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ফেরত 1;
}


পূর্ণসংখ্যা persist__chunk_sub_write_v6(FILE *db_fptr, কাঠামো P_sub *chunk)
{
	কাঠামো PF_header header;
	uint16_t id_len = chunk->F.id_len;
	uint16_t topic_len = chunk->F.topic_len;

	chunk->F.identifier = htonl(chunk->F.identifier);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);

	header.chunk = htonl(DB_CHUNK_SUB);
	header.দৈর্ঘ্য = htonl((uint32_t)আকার(কাঠামো PF_sub) +
			id_len + topic_len);

	write_e(db_fptr, &header, আকার(কাঠামো PF_header));
	write_e(db_fptr, &chunk->F, আকার(কাঠামো PF_sub));
	write_e(db_fptr, chunk->clientid, id_len);
	write_e(db_fptr, chunk->topic, topic_len);

	ফেরত MOSQ_ERR_SUCCESS;
error:
	log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ফেরত 1;
}
#endif
