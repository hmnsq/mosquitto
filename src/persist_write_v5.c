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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ifdef WITH_PERSISTENCE

#ifndef WIN32
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ଅନ୍ତର୍ଭାଵ <errno.h>
#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <time.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "persist.h"
#ଅନ୍ତର୍ଭାଵ "packet_mosq.h"
#ଅନ୍ତର୍ଭାଵ "property_common.h"
#ଅନ୍ତର୍ଭାଵ "property_mosq.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__chunk_cfg_write_v6(FILE *db_fptr, ସଂରଚନା PF_cfg *chunk)
{
	ସଂରଚନା PF_header header;

	header.chunk = htonl(DB_CHUNK_CFG);
	header.length = htonl(ଆକାର(ସଂରଚନା PF_cfg));
	write_e(db_fptr, &header, ଆକାର(ସଂରଚନା PF_header));
	write_e(db_fptr, chunk, ଆକାର(ସଂରଚନା PF_cfg));

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
error:
	log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ଫେରନ୍ତୁ 1;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__chunk_client_write_v6(FILE *db_fptr, ସଂରଚନା P_client *chunk)
{
	ସଂରଚନା PF_header header;
	uint16_t id_len = chunk->F.id_len;
	uint16_t username_len = chunk->F.username_len;

	chunk->F.session_expiry_interval = htonl(chunk->F.session_expiry_interval);
	chunk->F.last_mid = htons(chunk->F.last_mid);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.username_len = htons(chunk->F.username_len);
	chunk->F.listener_port = htons(chunk->F.listener_port);

	header.chunk = htonl(DB_CHUNK_CLIENT);
	header.length = htonl((uint32_t)ଆକାର(ସଂରଚନା PF_client)+id_len+username_len);

	write_e(db_fptr, &header, ଆକାର(ସଂରଚନା PF_header));
	write_e(db_fptr, &chunk->F, ଆକାର(ସଂରଚନା PF_client));

	write_e(db_fptr, chunk->clientid, id_len);
	ଯଦି(username_len > 0){
		write_e(db_fptr, chunk->username, username_len);
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
error:
	log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ଫେରନ୍ତୁ 1;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist_chunk_client_ସନ୍ଦେଶ_write_v6(FILE *db_fptr, ସଂରଚନା P_client_ସନ୍ଦେଶ *chunk)
{
	ସଂରଚନା PF_header header;
	ସଂରଚନା mosquitto__packet *prop_packet = ଖାଲି;
	uint16_t id_len = chunk->F.id_len;
	uint32_t proplen = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	mosquitto_property subscription_id_prop = {
		.next = ଖାଲି, .identifier = MQTT_PROP_SUBSCRIPTION_IDENTIFIER, .client_generated = true, .property_type = MQTT_PROP_TYPE_VARINT
	};

	ଯଦି(chunk->subscription_identifier){
		subscription_id_prop.ମୂଲ୍ୟ.varint = chunk->subscription_identifier;
		proplen += mosquitto_property_get_remaining_length(&subscription_id_prop);
	}

	chunk->F.mid = htons(chunk->F.mid);
	chunk->F.id_len = htons(chunk->F.id_len);

	header.chunk = htonl(DB_CHUNK_CLIENT_ସନ୍ଦେଶ);
	header.length = htonl((uint32_t)ଆକାର(ସଂରଚନା PF_client_ସନ୍ଦେଶ) + id_len + proplen);

	write_e(db_fptr, &header, ଆକାର(ସଂରଚନା PF_header));
	write_e(db_fptr, &chunk->F, ଆକାର(ସଂରଚନା PF_client_ସନ୍ଦେଶ));
	write_e(db_fptr, chunk->clientid, id_len);
	ଯଦି(chunk->subscription_identifier){
		ଯଦି(proplen > 0){
			prop_packet = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto__packet)+proplen);
			ଯଦି(prop_packet == ଖାଲି){
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_length = proplen;
			prop_packet->packet_length = proplen;
			rc = property__write_all(prop_packet, &subscription_id_prop, true);
			ଯଦି(rc){
				mosquitto_FREE(prop_packet);
				ଫେରନ୍ତୁ rc;
			}

			write_e(db_fptr, prop_packet->payload, proplen);
			mosquitto_FREE(prop_packet);
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
error:
	log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(prop_packet);
	ଫେରନ୍ତୁ 1;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__chunk_message_store_write_v6(FILE *db_fptr, ସଂରଚନା P_base_ସନ୍ଦେଶ *chunk)
{
	ସଂରଚନା PF_header header;
	uint32_t payloadlen = chunk->F.payloadlen;
	uint16_t source_id_len = chunk->F.source_id_len;
	uint16_t source_username_len = chunk->F.source_username_len;
	uint16_t topic_len = chunk->F.topic_len;
	uint32_t proplen = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(chunk->properties){
		proplen += mosquitto_property_get_remaining_length(chunk->properties);
	}

	chunk->F.payloadlen = htonl(chunk->F.payloadlen);
	chunk->F.source_mid = htons(chunk->F.source_mid);
	chunk->F.source_id_len = htons(chunk->F.source_id_len);
	chunk->F.source_username_len = htons(chunk->F.source_username_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);
	chunk->F.source_port = htons(chunk->F.source_port);

	header.chunk = htonl(DB_CHUNK_BASE_ସନ୍ଦେଶ);
	header.length = htonl((uint32_t)ଆକାର(ସଂରଚନା PF_base_ସନ୍ଦେଶ) +
			topic_len + payloadlen +
			source_id_len + source_username_len + proplen);

	write_e(db_fptr, &header, ଆକାର(ସଂରଚନା PF_header));
	write_e(db_fptr, &chunk->F, ଆକାର(ସଂରଚନା PF_base_ସନ୍ଦେଶ));
	ଯଦି(source_id_len){
		write_e(db_fptr, chunk->source.id, source_id_len);
	}
	ଯଦି(source_username_len){
		write_e(db_fptr, chunk->source.username, source_username_len);
	}
	write_e(db_fptr, chunk->topic, topic_len);
	ଯଦି(payloadlen){
		write_e(db_fptr, chunk->payload, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)payloadlen);
	}
	ଯଦି(chunk->properties){
		ଯଦି(proplen > 0){
			ସଂରଚନା mosquitto__packet *prop_packet = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto__packet)+proplen);
			ଯଦି(prop_packet == ଖାଲି){
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
			prop_packet->remaining_length = proplen;
			prop_packet->packet_length = proplen;
			rc = property__write_all(prop_packet, chunk->properties, true);
			ଯଦି(rc){
				mosquitto_FREE(prop_packet);
				ଫେରନ୍ତୁ rc;
			}

			ଯଦି(fwrite(prop_packet->payload, 1, proplen, db_fptr) != proplen){
				mosquitto_FREE(prop_packet);
				ଯାଅ error;
			}
			mosquitto_FREE(prop_packet);
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
error:
	log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ଫେରନ୍ତୁ 1;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__chunk_retain_write_v6(FILE *db_fptr, ସଂରଚନା P_retain *chunk)
{
	ସଂରଚନା PF_header header;

	header.chunk = htonl(DB_CHUNK_RETAIN);
	header.length = htonl((uint32_t)ଆକାର(ସଂରଚନା PF_retain));

	write_e(db_fptr, &header, ଆକାର(ସଂରଚନା PF_header));
	write_e(db_fptr, &chunk->F, ଆକାର(ସଂରଚନା PF_retain));

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
error:
	log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ଫେରନ୍ତୁ 1;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__chunk_sub_write_v6(FILE *db_fptr, ସଂରଚନା P_sub *chunk)
{
	ସଂରଚନା PF_header header;
	uint16_t id_len = chunk->F.id_len;
	uint16_t topic_len = chunk->F.topic_len;

	chunk->F.identifier = htonl(chunk->F.identifier);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);

	header.chunk = htonl(DB_CHUNK_SUB);
	header.length = htonl((uint32_t)ଆକାର(ସଂରଚନା PF_sub) +
			id_len + topic_len);

	write_e(db_fptr, &header, ଆକାର(ସଂରଚନା PF_header));
	write_e(db_fptr, &chunk->F, ଆକାର(ସଂରଚନା PF_sub));
	write_e(db_fptr, chunk->clientid, id_len);
	write_e(db_fptr, chunk->topic, topic_len);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
error:
	log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ଫେରନ୍ତୁ 1;
}
#endif
