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

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "persist.h"
#ଅନ୍ତର୍ଭାଵ "property_mosq.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__chunk_header_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	size_t rlen;
	ସଂରଚନା PF_header header;

	rlen = fread(&header, ଆକାର(ସଂରଚନା PF_header), 1, db_fptr);
	ଯଦି(rlen != 1){
		ଫେରନ୍ତୁ 1;
	}

	*chunk = ntohl(header.chunk);
	*length = ntohl(header.length);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__chunk_cfg_read_v56(FILE *db_fptr, ସଂରଚନା PF_cfg *chunk)
{
	ଯଦି(fread(chunk, ଆକାର(ସଂରଚନା PF_cfg), 1, db_fptr) != 1){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		ଫେରନ୍ତୁ 1;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__chunk_client_read_v56(FILE *db_fptr, ସଂରଚନା P_client *chunk, uint32_t db_version)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(db_version == 6){
		read_e(db_fptr, &chunk->F, ଆକାର(ସଂରଚନା PF_client));
		chunk->F.username_len = ntohs(chunk->F.username_len);
		chunk->F.listener_port = ntohs(chunk->F.listener_port);
	}ଅନ୍ୟଥା ଯଦି(db_version == 5){
		read_e(db_fptr, &chunk->F, ଆକାର(ସଂରଚନା PF_client_v5));
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ 1;
	}

	chunk->F.session_expiry_interval = ntohl(chunk->F.session_expiry_interval);
	chunk->F.last_mid = ntohs(chunk->F.last_mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);


	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	ଯଦି(rc){
		ଫେରନ୍ତୁ 1;
	}ଅନ୍ୟଥା ଯଦି(chunk->clientid == ଖାଲି){
		ଫେରନ୍ତୁ -1;
	}

	ଯଦି(chunk->F.username_len > 0){
		rc = persist__read_string_len(db_fptr, &chunk->username, chunk->F.username_len);
		ଯଦି(rc || !chunk->username){
			mosquitto_FREE(chunk->clientid);
			ଫେରନ୍ତୁ 1;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
error:
	log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ଫେରନ୍ତୁ 1;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist_chunk_client_ସନ୍ଦେଶ_read_v56(FILE *db_fptr, ସଂରଚନା P_client_ସନ୍ଦେଶ *chunk, uint32_t length)
{
	mosquitto_property *properties = ଖାଲି, *p;
	ସଂରଚନା mosquitto__packet_in prop_packet;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	memset(&prop_packet, 0, ଆକାର(ସଂରଚନା mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, ଆକାର(ସଂରଚନା PF_client_ସନ୍ଦେଶ));
	chunk->F.mid = ntohs(chunk->F.mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);

	length -= (uint32_t)(ଆକାର(ସଂରଚନା PF_client_ସନ୍ଦେଶ) + chunk->F.id_len);
	ଯଦି(length > MQTT_MAX_PAYLOAD){
		ଯାଅ error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	ଯଦି(length > 0){
		prop_packet.remaining_length = length;
		prop_packet.payload = mosquitto_malloc(length);
		ଯଦି(!prop_packet.payload){
			errno = ENOMEM;
			ଯାଅ error;
		}

		read_e(db_fptr, prop_packet.payload, length);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		ଯଦି(rc){
			mosquitto_FREE(chunk->clientid);
			ଫେରନ୍ତୁ rc;
		}

		ଯଦି(properties){
			p = properties;
			ଯେତେବେଳେ(p){
				ଯଦି(mosquitto_property_identifier(p) == MQTT_PROP_SUBSCRIPTION_IDENTIFIER){
					chunk->subscription_identifier = mosquitto_property_varint_ମୂଲ୍ୟ(p);
				}
				p = mosquitto_property_next(p);
			}
			mosquitto_property_free_all(&properties);
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(prop_packet.payload);
	log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ଫେରନ୍ତୁ 1;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist_chunk_base_ସନ୍ଦେଶ_read_v56(FILE *db_fptr, ସଂରଚନା P_base_ସନ୍ଦେଶ *chunk, uint32_t length)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
	mosquitto_property *properties = ଖାଲି;
	ସଂରଚନା mosquitto__packet_in prop_packet;

	memset(&prop_packet, 0, ଆକାର(ସଂରଚନା mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, ଆକାର(ସଂରଚନା PF_base_ସନ୍ଦେଶ));
	chunk->F.payloadlen = ntohl(chunk->F.payloadlen);
	ଯଦି(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	chunk->F.source_mid = ntohs(chunk->F.source_mid);
	chunk->F.source_id_len = ntohs(chunk->F.source_id_len);
	chunk->F.source_username_len = ntohs(chunk->F.source_username_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);
	chunk->F.source_port = ntohs(chunk->F.source_port);

	length -= (uint32_t)(ଆକାର(ସଂରଚନା PF_base_ସନ୍ଦେଶ) + chunk->F.payloadlen + chunk->F.source_id_len + chunk->F.source_username_len + chunk->F.topic_len);
	ଯଦି(length > MQTT_MAX_PAYLOAD){
		ଯାଅ error;
	}

	ଯଦି(chunk->F.source_id_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.id, chunk->F.source_id_len);
		ଯଦି(rc){
			ଯାଅ error;
		}
	}
	ଯଦି(chunk->F.source_username_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.username, chunk->F.source_username_len);
		ଯଦି(rc){
			ଯାଅ error;
		}
	}
	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	ଯଦି(rc){
		ଯାଅ error;
	}

	ଯଦି(chunk->F.payloadlen > 0){
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		ଯଦି(chunk->payload == ଖାଲି){
			rc = MOSQ_ERR_NOMEM;
			ଯାଅ error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	ଯଦି(length > 0){
		prop_packet.remaining_length = length;
		prop_packet.payload = mosquitto_malloc(length);
		ଯଦି(!prop_packet.payload){
			rc = MOSQ_ERR_NOMEM;
			ଯାଅ error;
		}
		read_e(db_fptr, prop_packet.payload, length);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		ଯଦି(rc){
			rc = MOSQ_ERR_NOMEM;
			ଯାଅ error;
		}
	}
	chunk->properties = properties;

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	mosquitto_FREE(prop_packet.payload);
	ଫେରନ୍ତୁ rc;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__chunk_retain_read_v56(FILE *db_fptr, ସଂରଚନା P_retain *chunk)
{
	ଯଦି(fread(&chunk->F, ଆକାର(ସଂରଚନା P_retain), 1, db_fptr) != 1){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		ଫେରନ୍ତୁ 1;
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା persist__chunk_sub_read_v56(FILE *db_fptr, ସଂରଚନା P_sub *chunk)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_SUCCESS;

	read_e(db_fptr, &chunk->F, ଆକାର(ସଂରଚନା PF_sub));
	chunk->F.identifier = ntohl(chunk->F.identifier);
	chunk->F.id_len = ntohs(chunk->F.id_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	ଯଦି(rc){
		ଯାଅ error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	ଯଦି(rc){
		ଯାଅ error;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	ଫେରନ୍ତୁ rc;
}

#endif
