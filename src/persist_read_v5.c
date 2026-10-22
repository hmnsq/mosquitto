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

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "persist.h"
#شامل "property_mosq.h"
#شامل "util_mosq.h"


صحیح persist__chunk_header_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *length)
{
	size_t rlen;
	ساختار PF_header header;

	rlen = fread(&header, اندازه(ساختار PF_header), 1, db_fptr);
	اگر(rlen != 1){
		بازگشت 1;
	}

	*chunk = ntohl(header.chunk);
	*length = ntohl(header.length);

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح persist__chunk_cfg_read_v56(FILE *db_fptr, ساختار PF_cfg *chunk)
{
	اگر(fread(chunk, اندازه(ساختار PF_cfg), 1, db_fptr) != 1){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		بازگشت 1;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح persist__chunk_client_read_v56(FILE *db_fptr, ساختار P_client *chunk, uint32_t db_version)
{
	صحیح rc;

	اگر(db_version == 6){
		read_e(db_fptr, &chunk->F, اندازه(ساختار PF_client));
		chunk->F.username_len = ntohs(chunk->F.username_len);
		chunk->F.listener_port = ntohs(chunk->F.listener_port);
	}وگرنه اگر(db_version == 5){
		read_e(db_fptr, &chunk->F, اندازه(ساختار PF_client_v5));
	}وگرنه{
		بازگشت 1;
	}

	chunk->F.session_expiry_interval = ntohl(chunk->F.session_expiry_interval);
	chunk->F.last_mid = ntohs(chunk->F.last_mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);


	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	اگر(rc){
		بازگشت 1;
	}وگرنه اگر(chunk->clientid == پوچ){
		بازگشت -1;
	}

	اگر(chunk->F.username_len > 0){
		rc = persist__read_string_len(db_fptr, &chunk->username, chunk->F.username_len);
		اگر(rc || !chunk->username){
			mosquitto_FREE(chunk->clientid);
			بازگشت 1;
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
error:
	log__printf(پوچ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	بازگشت 1;
}


صحیح persist_chunk_client_پیام_read_v56(FILE *db_fptr, ساختار P_client_پیام *chunk, uint32_t length)
{
	mosquitto_property *properties = پوچ, *p;
	ساختار mosquitto__packet_in prop_packet;
	صحیح rc;

	memset(&prop_packet, 0, اندازه(ساختار mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, اندازه(ساختار PF_client_پیام));
	chunk->F.mid = ntohs(chunk->F.mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);

	length -= (uint32_t)(اندازه(ساختار PF_client_پیام) + chunk->F.id_len);
	اگر(length > MQTT_MAX_PAYLOAD){
		برو error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	اگر(rc){
		بازگشت rc;
	}

	اگر(length > 0){
		prop_packet.remaining_length = length;
		prop_packet.payload = mosquitto_malloc(length);
		اگر(!prop_packet.payload){
			errno = ENOMEM;
			برو error;
		}

		read_e(db_fptr, prop_packet.payload, length);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		اگر(rc){
			mosquitto_FREE(chunk->clientid);
			بازگشت rc;
		}

		اگر(properties){
			p = properties;
			وقتی(p){
				اگر(mosquitto_property_identifier(p) == MQTT_PROP_SUBSCRIPTION_IDENTIFIER){
					chunk->subscription_identifier = mosquitto_property_varint_مقدار(p);
				}
				p = mosquitto_property_next(p);
			}
			mosquitto_property_free_all(&properties);
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(prop_packet.payload);
	log__printf(پوچ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	بازگشت 1;
}


صحیح persist_chunk_base_پیام_read_v56(FILE *db_fptr, ساختار P_base_پیام *chunk, uint32_t length)
{
	صحیح rc = 0;
	mosquitto_property *properties = پوچ;
	ساختار mosquitto__packet_in prop_packet;

	memset(&prop_packet, 0, اندازه(ساختار mosquitto__packet_in));

	read_e(db_fptr, &chunk->F, اندازه(ساختار PF_base_پیام));
	chunk->F.payloadlen = ntohl(chunk->F.payloadlen);
	اگر(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
		بازگشت MOSQ_ERR_INVAL;
	}
	chunk->F.source_mid = ntohs(chunk->F.source_mid);
	chunk->F.source_id_len = ntohs(chunk->F.source_id_len);
	chunk->F.source_username_len = ntohs(chunk->F.source_username_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);
	chunk->F.source_port = ntohs(chunk->F.source_port);

	length -= (uint32_t)(اندازه(ساختار PF_base_پیام) + chunk->F.payloadlen + chunk->F.source_id_len + chunk->F.source_username_len + chunk->F.topic_len);
	اگر(length > MQTT_MAX_PAYLOAD){
		برو error;
	}

	اگر(chunk->F.source_id_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.id, chunk->F.source_id_len);
		اگر(rc){
			برو error;
		}
	}
	اگر(chunk->F.source_username_len){
		rc = persist__read_string_len(db_fptr, &chunk->source.username, chunk->F.source_username_len);
		اگر(rc){
			برو error;
		}
	}
	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	اگر(rc){
		برو error;
	}

	اگر(chunk->F.payloadlen > 0){
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		اگر(chunk->payload == پوچ){
			rc = MOSQ_ERR_NOMEM;
			برو error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	اگر(length > 0){
		prop_packet.remaining_length = length;
		prop_packet.payload = mosquitto_malloc(length);
		اگر(!prop_packet.payload){
			rc = MOSQ_ERR_NOMEM;
			برو error;
		}
		read_e(db_fptr, prop_packet.payload, length);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		اگر(rc){
			rc = MOSQ_ERR_NOMEM;
			برو error;
		}
	}
	chunk->properties = properties;

	بازگشت MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->source.id);
	mosquitto_FREE(chunk->source.username);
	mosquitto_FREE(chunk->topic);
	mosquitto_FREE(prop_packet.payload);
	بازگشت rc;
}


صحیح persist__chunk_retain_read_v56(FILE *db_fptr, ساختار P_retain *chunk)
{
	اگر(fread(&chunk->F, اندازه(ساختار P_retain), 1, db_fptr) != 1){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		بازگشت 1;
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح persist__chunk_sub_read_v56(FILE *db_fptr, ساختار P_sub *chunk)
{
	صحیح rc = MOSQ_ERR_SUCCESS;

	read_e(db_fptr, &chunk->F, اندازه(ساختار PF_sub));
	chunk->F.identifier = ntohl(chunk->F.identifier);
	chunk->F.id_len = ntohs(chunk->F.id_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	اگر(rc){
		برو error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	اگر(rc){
		برو error;
	}

	بازگشت MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	بازگشت rc;
}

#endif
