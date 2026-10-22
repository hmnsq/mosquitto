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

#ضمن "config.h"

#اگر_تعریف_شده WITH_PERSISTENCE

#اگر_تعریف_نشده WIN32
#ضمن <arpa/inet.h>
#پایان_اگر
#ضمن <errno.h>
#ضمن <fcntl.h>
#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <sys/stat.h>
#ضمن <time.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "persist.h"
#ضمن "property_mosq.h"
#ضمن "util_mosq.h"


صحيح persist_chunk_سرآیند_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *درازی)
{
	size_t rlen;
	بنية PF_سرآیند سرآیند;

	rlen = fread(&سرآیند, حجم(بنية PF_سرآیند), 1, db_fptr);
	إذا(rlen != 1){
		أرجع 1;
	}

	*chunk = ntohl(سرآیند.chunk);
	*درازی = ntohl(سرآیند.درازی);

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح persist__chunk_cfg_read_v56(FILE *db_fptr, بنية PF_cfg *chunk)
{
	إذا(fread(chunk, حجم(بنية PF_cfg), 1, db_fptr) != 1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		أرجع 1;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح persist__chunk_client_read_v56(FILE *db_fptr, بنية P_client *chunk, uint32_t db_نسخه)
{
	صحيح rc;

	إذا(db_نسخه == 6){
		read_e(db_fptr, &chunk->F, حجم(بنية PF_client));
		chunk->F.username_len = ntohs(chunk->F.username_len);
		chunk->F.listener_پورت = ntohs(chunk->F.listener_پورت);
	}وإلا إذا(db_نسخه == 5){
		read_e(db_fptr, &chunk->F, حجم(بنية PF_client_v5));
	}وإلا{
		أرجع 1;
	}

	chunk->F.session_expiry_interval = ntohl(chunk->F.session_expiry_interval);
	chunk->F.آخرین_mid = ntohs(chunk->F.آخرین_mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);


	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	إذا(rc){
		أرجع 1;
	}وإلا إذا(chunk->clientid == عدم){
		أرجع -1;
	}

	إذا(chunk->F.username_len > 0){
		rc = persist__read_string_len(db_fptr, &chunk->username, chunk->F.username_len);
		إذا(rc || !chunk->username){
			mosquitto_FREE(chunk->clientid);
			أرجع 1;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
error:
	log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	أرجع 1;
}


صحيح persist_chunk_client_پیام_read_v56(FILE *db_fptr, بنية P_client_پیام *chunk, uint32_t درازی)
{
	mosquitto_property *properties = عدم, *p;
	بنية mosquitto_packet_درون prop_packet;
	صحيح rc;

	memset(&prop_packet, 0, حجم(بنية mosquitto_packet_درون));

	read_e(db_fptr, &chunk->F, حجم(بنية PF_client_پیام));
	chunk->F.mid = ntohs(chunk->F.mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);

	درازی -= (uint32_t)(حجم(بنية PF_client_پیام) + chunk->F.id_len);
	إذا(درازی > MQTT_MAX_PAYLOAD){
		اذهب error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	إذا(rc){
		أرجع rc;
	}

	إذا(درازی > 0){
		prop_packet.مانده_درازی = درازی;
		prop_packet.payload = mosquitto_malloc(درازی);
		إذا(!prop_packet.payload){
			errno = ENOMEM;
			اذهب error;
		}

		read_e(db_fptr, prop_packet.payload, درازی);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		إذا(rc){
			mosquitto_FREE(chunk->clientid);
			أرجع rc;
		}

		إذا(properties){
			p = properties;
			طالما(p){
				إذا(mosquitto_property_identifier(p) == MQTT_PROP_SUBSCRIPTION_IDENTIFIER){
					chunk->subscription_identifier = mosquitto_property_varint_مقدار(p);
				}
				p = mosquitto_property_بعدی(p);
			}
			mosquitto_property_free_all(&properties);
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(prop_packet.payload);
	log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	أرجع 1;
}


صحيح persist_chunk_پایه_پیام_read_v56(FILE *db_fptr, بنية P_پایه_پیام *chunk, uint32_t درازی)
{
	صحيح rc = 0;
	mosquitto_property *properties = عدم;
	بنية mosquitto_packet_درون prop_packet;

	memset(&prop_packet, 0, حجم(بنية mosquitto_packet_درون));

	read_e(db_fptr, &chunk->F, حجم(بنية PF_پایه_پیام));
	chunk->F.payloadlen = ntohl(chunk->F.payloadlen);
	إذا(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
		أرجع MOSQ_ERR_INVAL;
	}
	chunk->F.منبع_mid = ntohs(chunk->F.منبع_mid);
	chunk->F.منبع_id_len = ntohs(chunk->F.منبع_id_len);
	chunk->F.منبع_username_len = ntohs(chunk->F.منبع_username_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);
	chunk->F.منبع_پورت = ntohs(chunk->F.منبع_پورت);

	درازی -= (uint32_t)(حجم(بنية PF_پایه_پیام) + chunk->F.payloadlen + chunk->F.منبع_id_len + chunk->F.منبع_username_len + chunk->F.topic_len);
	إذا(درازی > MQTT_MAX_PAYLOAD){
		اذهب error;
	}

	إذا(chunk->F.منبع_id_len){
		rc = persist__read_string_len(db_fptr, &chunk->منبع.id, chunk->F.منبع_id_len);
		إذا(rc){
			اذهب error;
		}
	}
	إذا(chunk->F.منبع_username_len){
		rc = persist__read_string_len(db_fptr, &chunk->منبع.username, chunk->F.منبع_username_len);
		إذا(rc){
			اذهب error;
		}
	}
	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	إذا(rc){
		اذهب error;
	}

	إذا(chunk->F.payloadlen > 0){
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		إذا(chunk->payload == عدم){
			rc = MOSQ_ERR_NOMEM;
			اذهب error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	إذا(درازی > 0){
		prop_packet.مانده_درازی = درازی;
		prop_packet.payload = mosquitto_malloc(درازی);
		إذا(!prop_packet.payload){
			rc = MOSQ_ERR_NOMEM;
			اذهب error;
		}
		read_e(db_fptr, prop_packet.payload, درازی);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		إذا(rc){
			rc = MOSQ_ERR_NOMEM;
			اذهب error;
		}
	}
	chunk->properties = properties;

	أرجع MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->منبع.id);
	mosquitto_FREE(chunk->منبع.username);
	mosquitto_FREE(chunk->topic);
	mosquitto_FREE(prop_packet.payload);
	أرجع rc;
}


صحيح persist__chunk_retain_read_v56(FILE *db_fptr, بنية P_retain *chunk)
{
	إذا(fread(&chunk->F, حجم(بنية P_retain), 1, db_fptr) != 1){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		أرجع 1;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح persist__chunk_sub_read_v56(FILE *db_fptr, بنية P_sub *chunk)
{
	صحيح rc = MOSQ_ERR_SUCCESS;

	read_e(db_fptr, &chunk->F, حجم(بنية PF_sub));
	chunk->F.identifier = ntohl(chunk->F.identifier);
	chunk->F.id_len = ntohs(chunk->F.id_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	إذا(rc){
		اذهب error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	إذا(rc){
		اذهب error;
	}

	أرجع MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	أرجع rc;
}

#پایان_اگر
