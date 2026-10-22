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

#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "persist.h"
#અન્તર્ભાવ "packet_mosq.h"
#અન્તર્ભાવ "property_common.h"
#અન્તર્ભાવ "property_mosq.h"
#અન્તર્ભાવ "util_mosq.h"


પૂર્ણાંક persist__chunk_cfg_write_v6(FILE *db_fptr, સંરચના PF_cfg *chunk)
{
	સંરચના PF_header header;

	header.chunk = htonl(DB_CHUNK_CFG);
	header.length = htonl(કદ(સંરચના PF_cfg));
	write_e(db_fptr, &header, કદ(સંરચના PF_header));
	write_e(db_fptr, chunk, કદ(સંરચના PF_cfg));

	પાછા MOSQ_ERR_SUCCESS;
error:
	log__printf(ખાલી, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	પાછા 1;
}


પૂર્ણાંક persist__chunk_client_write_v6(FILE *db_fptr, સંરચના P_client *chunk)
{
	સંરચના PF_header header;
	uint16_t id_len = chunk->F.id_len;
	uint16_t username_len = chunk->F.username_len;

	chunk->F.session_expiry_interval = htonl(chunk->F.session_expiry_interval);
	chunk->F.last_mid = htons(chunk->F.last_mid);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.username_len = htons(chunk->F.username_len);
	chunk->F.listener_port = htons(chunk->F.listener_port);

	header.chunk = htonl(DB_CHUNK_CLIENT);
	header.length = htonl((uint32_t)કદ(સંરચના PF_client)+id_len+username_len);

	write_e(db_fptr, &header, કદ(સંરચના PF_header));
	write_e(db_fptr, &chunk->F, કદ(સંરચના PF_client));

	write_e(db_fptr, chunk->clientid, id_len);
	જો(username_len > 0){
		write_e(db_fptr, chunk->username, username_len);
	}

	પાછા MOSQ_ERR_SUCCESS;
error:
	log__printf(ખાલી, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	પાછા 1;
}


પૂર્ણાંક persist_chunk_client_સંદેશ_write_v6(FILE *db_fptr, સંરચના P_client_સંદેશ *chunk)
{
	સંરચના PF_header header;
	સંરચના mosquitto__packet *prop_packet = ખાલી;
	uint16_t id_len = chunk->F.id_len;
	uint32_t proplen = 0;
	પૂર્ણાંક rc;
	mosquitto_property subscription_id_prop = {
		.next = ખાલી, .identifier = MQTT_PROP_SUBSCRIPTION_IDENTIFIER, .client_generated = true, .property_type = MQTT_PROP_TYPE_VARINT
	};

	જો(chunk->subscription_identifier){
		subscription_id_prop.કિંમત.varint = chunk->subscription_identifier;
		proplen += mosquitto_property_get_remaining_length(&subscription_id_prop);
	}

	chunk->F.mid = htons(chunk->F.mid);
	chunk->F.id_len = htons(chunk->F.id_len);

	header.chunk = htonl(DB_CHUNK_CLIENT_સંદેશ);
	header.length = htonl((uint32_t)કદ(સંરચના PF_client_સંદેશ) + id_len + proplen);

	write_e(db_fptr, &header, કદ(સંરચના PF_header));
	write_e(db_fptr, &chunk->F, કદ(સંરચના PF_client_સંદેશ));
	write_e(db_fptr, chunk->clientid, id_len);
	જો(chunk->subscription_identifier){
		જો(proplen > 0){
			prop_packet = mosquitto_calloc(1, કદ(સંરચના mosquitto__packet)+proplen);
			જો(prop_packet == ખાલી){
				પાછા MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_length = proplen;
			prop_packet->packet_length = proplen;
			rc = property__write_all(prop_packet, &subscription_id_prop, true);
			જો(rc){
				mosquitto_FREE(prop_packet);
				પાછા rc;
			}

			write_e(db_fptr, prop_packet->payload, proplen);
			mosquitto_FREE(prop_packet);
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
error:
	log__printf(ખાલી, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(prop_packet);
	પાછા 1;
}


પૂર્ણાંક persist__chunk_message_store_write_v6(FILE *db_fptr, સંરચના P_base_સંદેશ *chunk)
{
	સંરચના PF_header header;
	uint32_t payloadlen = chunk->F.payloadlen;
	uint16_t source_id_len = chunk->F.source_id_len;
	uint16_t source_username_len = chunk->F.source_username_len;
	uint16_t topic_len = chunk->F.topic_len;
	uint32_t proplen = 0;
	પૂર્ણાંક rc;

	જો(chunk->properties){
		proplen += mosquitto_property_get_remaining_length(chunk->properties);
	}

	chunk->F.payloadlen = htonl(chunk->F.payloadlen);
	chunk->F.source_mid = htons(chunk->F.source_mid);
	chunk->F.source_id_len = htons(chunk->F.source_id_len);
	chunk->F.source_username_len = htons(chunk->F.source_username_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);
	chunk->F.source_port = htons(chunk->F.source_port);

	header.chunk = htonl(DB_CHUNK_BASE_સંદેશ);
	header.length = htonl((uint32_t)કદ(સંરચના PF_base_સંદેશ) +
			topic_len + payloadlen +
			source_id_len + source_username_len + proplen);

	write_e(db_fptr, &header, કદ(સંરચના PF_header));
	write_e(db_fptr, &chunk->F, કદ(સંરચના PF_base_સંદેશ));
	જો(source_id_len){
		write_e(db_fptr, chunk->source.id, source_id_len);
	}
	જો(source_username_len){
		write_e(db_fptr, chunk->source.username, source_username_len);
	}
	write_e(db_fptr, chunk->topic, topic_len);
	જો(payloadlen){
		write_e(db_fptr, chunk->payload, (unsigned પૂર્ણાંક)payloadlen);
	}
	જો(chunk->properties){
		જો(proplen > 0){
			સંરચના mosquitto__packet *prop_packet = mosquitto_calloc(1, કદ(સંરચના mosquitto__packet)+proplen);
			જો(prop_packet == ખાલી){
				પાછા MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_length = proplen;
			prop_packet->packet_length = proplen;
			rc = property__write_all(prop_packet, chunk->properties, true);
			જો(rc){
				mosquitto_FREE(prop_packet);
				પાછા rc;
			}

			જો(fwrite(prop_packet->payload, 1, proplen, db_fptr) != proplen){
				mosquitto_FREE(prop_packet);
				જાઓ error;
			}
			mosquitto_FREE(prop_packet);
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
error:
	log__printf(ખાલી, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	પાછા 1;
}


પૂર્ણાંક persist__chunk_retain_write_v6(FILE *db_fptr, સંરચના P_retain *chunk)
{
	સંરચના PF_header header;

	header.chunk = htonl(DB_CHUNK_RETAIN);
	header.length = htonl((uint32_t)કદ(સંરચના PF_retain));

	write_e(db_fptr, &header, કદ(સંરચના PF_header));
	write_e(db_fptr, &chunk->F, કદ(સંરચના PF_retain));

	પાછા MOSQ_ERR_SUCCESS;
error:
	log__printf(ખાલી, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	પાછા 1;
}


પૂર્ણાંક persist__chunk_sub_write_v6(FILE *db_fptr, સંરચના P_sub *chunk)
{
	સંરચના PF_header header;
	uint16_t id_len = chunk->F.id_len;
	uint16_t topic_len = chunk->F.topic_len;

	chunk->F.identifier = htonl(chunk->F.identifier);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);

	header.chunk = htonl(DB_CHUNK_SUB);
	header.length = htonl((uint32_t)કદ(સંરચના PF_sub) +
			id_len + topic_len);

	write_e(db_fptr, &header, કદ(સંરચના PF_header));
	write_e(db_fptr, &chunk->F, કદ(સંરચના PF_sub));
	write_e(db_fptr, chunk->clientid, id_len);
	write_e(db_fptr, chunk->topic, topic_len);

	પાછા MOSQ_ERR_SUCCESS;
error:
	log__printf(ખાલી, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	પાછા 1;
}
#endif
