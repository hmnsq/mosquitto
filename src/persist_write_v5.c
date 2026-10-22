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

#شامل "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#شامل <arpa/inet.h>
#endif
#شامل <errno.h>
#شامل <fcntl.h>
#شامل <stdio.h>
#شامل <string.h>
#شامل <sys/stat.h>
#شامل <time.h>

#شامل "mosquitto/mqtt_protocol.h"
#شامل "mosquitto_broker_internal.h"
#شامل "persist.h"
#شامل "packet_mosq.h"
#شامل "property_common.h"
#شامل "property_mosq.h"
#شامل "util_mosq.h"


صحیح persist__chunk_cfg_write_v6(FILE *db_fptr, ساختار PF_cfg *chunk)
{
	ساختار PF_header header;

	header.chunk = htonl(DB_CHUNK_CFG);
	header.length = htonl(اندازه(ساختار PF_cfg));
	write_e(db_fptr, &header, اندازه(ساختار PF_header));
	write_e(db_fptr, chunk, اندازه(ساختار PF_cfg));

	بازگشت MOSQ_ERR_SUCCESS;
error:
	log__printf(پوچ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	بازگشت 1;
}


صحیح persist__chunk_client_write_v6(FILE *db_fptr, ساختار P_client *chunk)
{
	ساختار PF_header header;
	uint16_t id_len = chunk->F.id_len;
	uint16_t username_len = chunk->F.username_len;

	chunk->F.session_expiry_interval = htonl(chunk->F.session_expiry_interval);
	chunk->F.last_mid = htons(chunk->F.last_mid);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.username_len = htons(chunk->F.username_len);
	chunk->F.listener_port = htons(chunk->F.listener_port);

	header.chunk = htonl(DB_CHUNK_CLIENT);
	header.length = htonl((uint32_t)اندازه(ساختار PF_client)+id_len+username_len);

	write_e(db_fptr, &header, اندازه(ساختار PF_header));
	write_e(db_fptr, &chunk->F, اندازه(ساختار PF_client));

	write_e(db_fptr, chunk->clientid, id_len);
	اگر(username_len > 0){
		write_e(db_fptr, chunk->username, username_len);
	}

	بازگشت MOSQ_ERR_SUCCESS;
error:
	log__printf(پوچ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	بازگشت 1;
}


صحیح persist_chunk_client_پیام_write_v6(FILE *db_fptr, ساختار P_client_پیام *chunk)
{
	ساختار PF_header header;
	ساختار mosquitto__packet *prop_packet = پوچ;
	uint16_t id_len = chunk->F.id_len;
	uint32_t proplen = 0;
	صحیح rc;
	mosquitto_property subscription_id_prop = {
		.next = پوچ, .identifier = MQTT_PROP_SUBSCRIPTION_IDENTIFIER, .client_generated = true, .property_type = MQTT_PROP_TYPE_VARINT
	};

	اگر(chunk->subscription_identifier){
		subscription_id_prop.مقدار.varint = chunk->subscription_identifier;
		proplen += mosquitto_property_get_remaining_length(&subscription_id_prop);
	}

	chunk->F.mid = htons(chunk->F.mid);
	chunk->F.id_len = htons(chunk->F.id_len);

	header.chunk = htonl(DB_CHUNK_CLIENT_پیام);
	header.length = htonl((uint32_t)اندازه(ساختار PF_client_پیام) + id_len + proplen);

	write_e(db_fptr, &header, اندازه(ساختار PF_header));
	write_e(db_fptr, &chunk->F, اندازه(ساختار PF_client_پیام));
	write_e(db_fptr, chunk->clientid, id_len);
	اگر(chunk->subscription_identifier){
		اگر(proplen > 0){
			prop_packet = mosquitto_calloc(1, اندازه(ساختار mosquitto__packet)+proplen);
			اگر(prop_packet == پوچ){
				بازگشت MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_length = proplen;
			prop_packet->packet_length = proplen;
			rc = property__write_all(prop_packet, &subscription_id_prop, true);
			اگر(rc){
				mosquitto_FREE(prop_packet);
				بازگشت rc;
			}

			write_e(db_fptr, prop_packet->payload, proplen);
			mosquitto_FREE(prop_packet);
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
error:
	log__printf(پوچ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(prop_packet);
	بازگشت 1;
}


صحیح persist__chunk_message_store_write_v6(FILE *db_fptr, ساختار P_base_پیام *chunk)
{
	ساختار PF_header header;
	uint32_t payloadlen = chunk->F.payloadlen;
	uint16_t source_id_len = chunk->F.source_id_len;
	uint16_t source_username_len = chunk->F.source_username_len;
	uint16_t topic_len = chunk->F.topic_len;
	uint32_t proplen = 0;
	صحیح rc;

	اگر(chunk->properties){
		proplen += mosquitto_property_get_remaining_length(chunk->properties);
	}

	chunk->F.payloadlen = htonl(chunk->F.payloadlen);
	chunk->F.source_mid = htons(chunk->F.source_mid);
	chunk->F.source_id_len = htons(chunk->F.source_id_len);
	chunk->F.source_username_len = htons(chunk->F.source_username_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);
	chunk->F.source_port = htons(chunk->F.source_port);

	header.chunk = htonl(DB_CHUNK_BASE_پیام);
	header.length = htonl((uint32_t)اندازه(ساختار PF_base_پیام) +
			topic_len + payloadlen +
			source_id_len + source_username_len + proplen);

	write_e(db_fptr, &header, اندازه(ساختار PF_header));
	write_e(db_fptr, &chunk->F, اندازه(ساختار PF_base_پیام));
	اگر(source_id_len){
		write_e(db_fptr, chunk->source.id, source_id_len);
	}
	اگر(source_username_len){
		write_e(db_fptr, chunk->source.username, source_username_len);
	}
	write_e(db_fptr, chunk->topic, topic_len);
	اگر(payloadlen){
		write_e(db_fptr, chunk->payload, (unsigned صحیح)payloadlen);
	}
	اگر(chunk->properties){
		اگر(proplen > 0){
			ساختار mosquitto__packet *prop_packet = mosquitto_calloc(1, اندازه(ساختار mosquitto__packet)+proplen);
			اگر(prop_packet == پوچ){
				بازگشت MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_length = proplen;
			prop_packet->packet_length = proplen;
			rc = property__write_all(prop_packet, chunk->properties, true);
			اگر(rc){
				mosquitto_FREE(prop_packet);
				بازگشت rc;
			}

			اگر(fwrite(prop_packet->payload, 1, proplen, db_fptr) != proplen){
				mosquitto_FREE(prop_packet);
				برو error;
			}
			mosquitto_FREE(prop_packet);
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
error:
	log__printf(پوچ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	بازگشت 1;
}


صحیح persist__chunk_retain_write_v6(FILE *db_fptr, ساختار P_retain *chunk)
{
	ساختار PF_header header;

	header.chunk = htonl(DB_CHUNK_RETAIN);
	header.length = htonl((uint32_t)اندازه(ساختار PF_retain));

	write_e(db_fptr, &header, اندازه(ساختار PF_header));
	write_e(db_fptr, &chunk->F, اندازه(ساختار PF_retain));

	بازگشت MOSQ_ERR_SUCCESS;
error:
	log__printf(پوچ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	بازگشت 1;
}


صحیح persist__chunk_sub_write_v6(FILE *db_fptr, ساختار P_sub *chunk)
{
	ساختار PF_header header;
	uint16_t id_len = chunk->F.id_len;
	uint16_t topic_len = chunk->F.topic_len;

	chunk->F.identifier = htonl(chunk->F.identifier);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);

	header.chunk = htonl(DB_CHUNK_SUB);
	header.length = htonl((uint32_t)اندازه(ساختار PF_sub) +
			id_len + topic_len);

	write_e(db_fptr, &header, اندازه(ساختار PF_header));
	write_e(db_fptr, &chunk->F, اندازه(ساختار PF_sub));
	write_e(db_fptr, chunk->clientid, id_len);
	write_e(db_fptr, chunk->topic, topic_len);

	بازگشت MOSQ_ERR_SUCCESS;
error:
	log__printf(پوچ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	بازگشت 1;
}
#endif
