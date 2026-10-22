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

#اگر_تعریف_شدہ WITH_PERSISTENCE

#اگر_غیر_تعریف_شدہ WIN32
#شامل <arpa/inet.h>
#اگر_ختم
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


صحیح persist_chunk_سرخی_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *لمبائی)
{
	size_t rlen;
	ساخت PF_سرخی سرخی;

	rlen = fread(&سرخی, حجم(ساخت PF_سرخی), 1, db_fptr);
	اگر(rlen != 1){
		واپس 1;
	}

	*chunk = ntohl(سرخی.chunk);
	*لمبائی = ntohl(سرخی.لمبائی);

	واپس MOSQ_ERR_SUCCESS;
}


صحیح persist__chunk_cfg_read_v56(FILE *db_fptr, ساخت PF_cfg *chunk)
{
	اگر(fread(chunk, حجم(ساخت PF_cfg), 1, db_fptr) != 1){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		واپس 1;
	}

	واپس MOSQ_ERR_SUCCESS;
}


صحیح persist__chunk_client_read_v56(FILE *db_fptr, ساخت P_client *chunk, uint32_t db_نسخہ)
{
	صحیح rc;

	اگر(db_نسخہ == 6){
		read_e(db_fptr, &chunk->F, حجم(ساخت PF_client));
		chunk->F.username_len = ntohs(chunk->F.username_len);
		chunk->F.listener_پورٹ = ntohs(chunk->F.listener_پورٹ);
	}ورنہ اگر(db_نسخہ == 5){
		read_e(db_fptr, &chunk->F, حجم(ساخت PF_client_v5));
	}ورنہ{
		واپس 1;
	}

	chunk->F.session_expiry_interval = ntohl(chunk->F.session_expiry_interval);
	chunk->F.آخری_mid = ntohs(chunk->F.آخری_mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);


	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	اگر(rc){
		واپس 1;
	}ورنہ اگر(chunk->clientid == تہی){
		واپس -1;
	}

	اگر(chunk->F.username_len > 0){
		rc = persist__read_string_len(db_fptr, &chunk->username, chunk->F.username_len);
		اگر(rc || !chunk->username){
			mosquitto_FREE(chunk->clientid);
			واپس 1;
		}
	}

	واپس MOSQ_ERR_SUCCESS;
error:
	log__printf(تہی, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	واپس 1;
}


صحیح persist_chunk_client_پیغام_read_v56(FILE *db_fptr, ساخت P_client_پیغام *chunk, uint32_t لمبائی)
{
	mosquitto_property *properties = تہی, *p;
	ساخت mosquitto_packet_اندر prop_packet;
	صحیح rc;

	memset(&prop_packet, 0, حجم(ساخت mosquitto_packet_اندر));

	read_e(db_fptr, &chunk->F, حجم(ساخت PF_client_پیغام));
	chunk->F.mid = ntohs(chunk->F.mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);

	لمبائی -= (uint32_t)(حجم(ساخت PF_client_پیغام) + chunk->F.id_len);
	اگر(لمبائی > MQTT_MAX_PAYLOAD){
		جاؤ error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	اگر(rc){
		واپس rc;
	}

	اگر(لمبائی > 0){
		prop_packet.بقایا_لمبائی = لمبائی;
		prop_packet.payload = mosquitto_malloc(لمبائی);
		اگر(!prop_packet.payload){
			errno = ENOMEM;
			جاؤ error;
		}

		read_e(db_fptr, prop_packet.payload, لمبائی);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		اگر(rc){
			mosquitto_FREE(chunk->clientid);
			واپس rc;
		}

		اگر(properties){
			p = properties;
			جب(p){
				اگر(mosquitto_property_identifier(p) == MQTT_PROP_SUBSCRIPTION_IDENTIFIER){
					chunk->subscription_identifier = mosquitto_property_varint_قدر(p);
				}
				p = mosquitto_property_اگلا(p);
			}
			mosquitto_property_free_all(&properties);
		}
	}

	واپس MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(prop_packet.payload);
	log__printf(تہی, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	واپس 1;
}


صحیح persist_chunk_بنیاد_پیغام_read_v56(FILE *db_fptr, ساخت P_بنیاد_پیغام *chunk, uint32_t لمبائی)
{
	صحیح rc = 0;
	mosquitto_property *properties = تہی;
	ساخت mosquitto_packet_اندر prop_packet;

	memset(&prop_packet, 0, حجم(ساخت mosquitto_packet_اندر));

	read_e(db_fptr, &chunk->F, حجم(ساخت PF_بنیاد_پیغام));
	chunk->F.payloadlen = ntohl(chunk->F.payloadlen);
	اگر(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
		واپس MOSQ_ERR_INVAL;
	}
	chunk->F.ماخذ_mid = ntohs(chunk->F.ماخذ_mid);
	chunk->F.ماخذ_id_len = ntohs(chunk->F.ماخذ_id_len);
	chunk->F.ماخذ_username_len = ntohs(chunk->F.ماخذ_username_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);
	chunk->F.ماخذ_پورٹ = ntohs(chunk->F.ماخذ_پورٹ);

	لمبائی -= (uint32_t)(حجم(ساخت PF_بنیاد_پیغام) + chunk->F.payloadlen + chunk->F.ماخذ_id_len + chunk->F.ماخذ_username_len + chunk->F.topic_len);
	اگر(لمبائی > MQTT_MAX_PAYLOAD){
		جاؤ error;
	}

	اگر(chunk->F.ماخذ_id_len){
		rc = persist__read_string_len(db_fptr, &chunk->ماخذ.id, chunk->F.ماخذ_id_len);
		اگر(rc){
			جاؤ error;
		}
	}
	اگر(chunk->F.ماخذ_username_len){
		rc = persist__read_string_len(db_fptr, &chunk->ماخذ.username, chunk->F.ماخذ_username_len);
		اگر(rc){
			جاؤ error;
		}
	}
	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	اگر(rc){
		جاؤ error;
	}

	اگر(chunk->F.payloadlen > 0){
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		اگر(chunk->payload == تہی){
			rc = MOSQ_ERR_NOMEM;
			جاؤ error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	اگر(لمبائی > 0){
		prop_packet.بقایا_لمبائی = لمبائی;
		prop_packet.payload = mosquitto_malloc(لمبائی);
		اگر(!prop_packet.payload){
			rc = MOSQ_ERR_NOMEM;
			جاؤ error;
		}
		read_e(db_fptr, prop_packet.payload, لمبائی);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		اگر(rc){
			rc = MOSQ_ERR_NOMEM;
			جاؤ error;
		}
	}
	chunk->properties = properties;

	واپس MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->ماخذ.id);
	mosquitto_FREE(chunk->ماخذ.username);
	mosquitto_FREE(chunk->topic);
	mosquitto_FREE(prop_packet.payload);
	واپس rc;
}


صحیح persist__chunk_retain_read_v56(FILE *db_fptr, ساخت P_retain *chunk)
{
	اگر(fread(&chunk->F, حجم(ساخت P_retain), 1, db_fptr) != 1){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		واپس 1;
	}
	واپس MOSQ_ERR_SUCCESS;
}


صحیح persist__chunk_sub_read_v56(FILE *db_fptr, ساخت P_sub *chunk)
{
	صحیح rc = MOSQ_ERR_SUCCESS;

	read_e(db_fptr, &chunk->F, حجم(ساخت PF_sub));
	chunk->F.identifier = ntohl(chunk->F.identifier);
	chunk->F.id_len = ntohs(chunk->F.id_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	اگر(rc){
		جاؤ error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	اگر(rc){
		جاؤ error;
	}

	واپس MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	واپس rc;
}

#اگر_ختم
