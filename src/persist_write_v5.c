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

#شامل "mosquitto/mqtt_protocol.h"
#شامل "mosquitto_broker_internal.h"
#شامل "persist.h"
#شامل "packet_mosq.h"
#شامل "property_common.h"
#شامل "property_mosq.h"
#شامل "util_mosq.h"


صحیح persist__chunk_cfg_write_v6(FILE *db_fptr, ساخت PF_cfg *chunk)
{
	ساخت PF_سرخی سرخی;

	سرخی.chunk = htonl(DB_CHUNK_CFG);
	سرخی.لمبائی = htonl(حجم(ساخت PF_cfg));
	write_e(db_fptr, &سرخی, حجم(ساخت PF_سرخی));
	write_e(db_fptr, chunk, حجم(ساخت PF_cfg));

	واپس MOSQ_ERR_SUCCESS;
error:
	log__printf(تہی, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	واپس 1;
}


صحیح persist__chunk_client_write_v6(FILE *db_fptr, ساخت P_client *chunk)
{
	ساخت PF_سرخی سرخی;
	uint16_t id_len = chunk->F.id_len;
	uint16_t username_len = chunk->F.username_len;

	chunk->F.session_expiry_interval = htonl(chunk->F.session_expiry_interval);
	chunk->F.آخری_mid = htons(chunk->F.آخری_mid);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.username_len = htons(chunk->F.username_len);
	chunk->F.listener_پورٹ = htons(chunk->F.listener_پورٹ);

	سرخی.chunk = htonl(DB_CHUNK_CLIENT);
	سرخی.لمبائی = htonl((uint32_t)حجم(ساخت PF_client)+id_len+username_len);

	write_e(db_fptr, &سرخی, حجم(ساخت PF_سرخی));
	write_e(db_fptr, &chunk->F, حجم(ساخت PF_client));

	write_e(db_fptr, chunk->clientid, id_len);
	اگر(username_len > 0){
		write_e(db_fptr, chunk->username, username_len);
	}

	واپس MOSQ_ERR_SUCCESS;
error:
	log__printf(تہی, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	واپس 1;
}


صحیح persist_chunk_client_پیغام_write_v6(FILE *db_fptr, ساخت P_client_پیغام *chunk)
{
	ساخت PF_سرخی سرخی;
	ساخت mosquitto__packet *prop_packet = تہی;
	uint16_t id_len = chunk->F.id_len;
	uint32_t proplen = 0;
	صحیح rc;
	mosquitto_property subscription_id_prop = {
		.اگلا = تہی, .identifier = MQTT_PROP_SUBSCRIPTION_IDENTIFIER, .client_generated = true, .property_type = MQTT_PROP_TYPE_VARINT
	};

	اگر(chunk->subscription_identifier){
		subscription_id_prop.قدر.varint = chunk->subscription_identifier;
		proplen += mosquitto_property_get_بقایا_لمبائی(&subscription_id_prop);
	}

	chunk->F.mid = htons(chunk->F.mid);
	chunk->F.id_len = htons(chunk->F.id_len);

	سرخی.chunk = htonl(DB_CHUNK_CLIENT_پیغام);
	سرخی.لمبائی = htonl((uint32_t)حجم(ساخت PF_client_پیغام) + id_len + proplen);

	write_e(db_fptr, &سرخی, حجم(ساخت PF_سرخی));
	write_e(db_fptr, &chunk->F, حجم(ساخت PF_client_پیغام));
	write_e(db_fptr, chunk->clientid, id_len);
	اگر(chunk->subscription_identifier){
		اگر(proplen > 0){
			prop_packet = mosquitto_calloc(1, حجم(ساخت mosquitto__packet)+proplen);
			اگر(prop_packet == تہی){
				واپس MOSQ_ERR_NOMEM;
			}
			prop_packet->بقایا_لمبائی = proplen;
			prop_packet->packet_لمبائی = proplen;
			rc = property__write_all(prop_packet, &subscription_id_prop, true);
			اگر(rc){
				mosquitto_FREE(prop_packet);
				واپس rc;
			}

			write_e(db_fptr, prop_packet->payload, proplen);
			mosquitto_FREE(prop_packet);
		}
	}

	واپس MOSQ_ERR_SUCCESS;
error:
	log__printf(تہی, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(prop_packet);
	واپس 1;
}


صحیح persist_chunk_پیغام_store_write_v6(FILE *db_fptr, ساخت P_بنیاد_پیغام *chunk)
{
	ساخت PF_سرخی سرخی;
	uint32_t payloadlen = chunk->F.payloadlen;
	uint16_t ماخذ_id_len = chunk->F.ماخذ_id_len;
	uint16_t ماخذ_username_len = chunk->F.ماخذ_username_len;
	uint16_t topic_len = chunk->F.topic_len;
	uint32_t proplen = 0;
	صحیح rc;

	اگر(chunk->properties){
		proplen += mosquitto_property_get_بقایا_لمبائی(chunk->properties);
	}

	chunk->F.payloadlen = htonl(chunk->F.payloadlen);
	chunk->F.ماخذ_mid = htons(chunk->F.ماخذ_mid);
	chunk->F.ماخذ_id_len = htons(chunk->F.ماخذ_id_len);
	chunk->F.ماخذ_username_len = htons(chunk->F.ماخذ_username_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);
	chunk->F.ماخذ_پورٹ = htons(chunk->F.ماخذ_پورٹ);

	سرخی.chunk = htonl(DB_CHUNK_بنیاد_پیغام);
	سرخی.لمبائی = htonl((uint32_t)حجم(ساخت PF_بنیاد_پیغام) +
			topic_len + payloadlen +
			ماخذ_id_len + ماخذ_username_len + proplen);

	write_e(db_fptr, &سرخی, حجم(ساخت PF_سرخی));
	write_e(db_fptr, &chunk->F, حجم(ساخت PF_بنیاد_پیغام));
	اگر(ماخذ_id_len){
		write_e(db_fptr, chunk->ماخذ.id, ماخذ_id_len);
	}
	اگر(ماخذ_username_len){
		write_e(db_fptr, chunk->ماخذ.username, ماخذ_username_len);
	}
	write_e(db_fptr, chunk->topic, topic_len);
	اگر(payloadlen){
		write_e(db_fptr, chunk->payload, (غیرعلامتی صحیح)payloadlen);
	}
	اگر(chunk->properties){
		اگر(proplen > 0){
			ساخت mosquitto__packet *prop_packet = mosquitto_calloc(1, حجم(ساخت mosquitto__packet)+proplen);
			اگر(prop_packet == تہی){
				واپس MOSQ_ERR_NOMEM;
			}
			prop_packet->بقایا_لمبائی = proplen;
			prop_packet->packet_لمبائی = proplen;
			rc = property__write_all(prop_packet, chunk->properties, true);
			اگر(rc){
				mosquitto_FREE(prop_packet);
				واپس rc;
			}

			اگر(fwrite(prop_packet->payload, 1, proplen, db_fptr) != proplen){
				mosquitto_FREE(prop_packet);
				جاؤ error;
			}
			mosquitto_FREE(prop_packet);
		}
	}

	واپس MOSQ_ERR_SUCCESS;
error:
	log__printf(تہی, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	واپس 1;
}


صحیح persist__chunk_retain_write_v6(FILE *db_fptr, ساخت P_retain *chunk)
{
	ساخت PF_سرخی سرخی;

	سرخی.chunk = htonl(DB_CHUNK_RETAIN);
	سرخی.لمبائی = htonl((uint32_t)حجم(ساخت PF_retain));

	write_e(db_fptr, &سرخی, حجم(ساخت PF_سرخی));
	write_e(db_fptr, &chunk->F, حجم(ساخت PF_retain));

	واپس MOSQ_ERR_SUCCESS;
error:
	log__printf(تہی, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	واپس 1;
}


صحیح persist__chunk_sub_write_v6(FILE *db_fptr, ساخت P_sub *chunk)
{
	ساخت PF_سرخی سرخی;
	uint16_t id_len = chunk->F.id_len;
	uint16_t topic_len = chunk->F.topic_len;

	chunk->F.identifier = htonl(chunk->F.identifier);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);

	سرخی.chunk = htonl(DB_CHUNK_SUB);
	سرخی.لمبائی = htonl((uint32_t)حجم(ساخت PF_sub) +
			id_len + topic_len);

	write_e(db_fptr, &سرخی, حجم(ساخت PF_سرخی));
	write_e(db_fptr, &chunk->F, حجم(ساخت PF_sub));
	write_e(db_fptr, chunk->clientid, id_len);
	write_e(db_fptr, chunk->topic, topic_len);

	واپس MOSQ_ERR_SUCCESS;
error:
	log__printf(تہی, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	واپس 1;
}
#اگر_ختم
