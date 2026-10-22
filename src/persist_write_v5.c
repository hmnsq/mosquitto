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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "persist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "property_common.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "property_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"


ਪੂਰਨ persist__chunk_cfg_write_v6(FILE *db_fptr, ਬਣਤਰ PF_cfg *chunk)
{
	ਬਣਤਰ PF_ਸਿਰਲੇਖ ਸਿਰਲੇਖ;

	ਸਿਰਲੇਖ.chunk = htonl(DB_CHUNK_CFG);
	ਸਿਰਲੇਖ.ਲੰਮਾਈ = htonl(ਆਕਾਰ(ਬਣਤਰ PF_cfg));
	write_e(db_fptr, &ਸਿਰਲੇਖ, ਆਕਾਰ(ਬਣਤਰ PF_ਸਿਰਲੇਖ));
	write_e(db_fptr, chunk, ਆਕਾਰ(ਬਣਤਰ PF_cfg));

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
error:
	log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ਵਾਪਸ 1;
}


ਪੂਰਨ persist__chunk_client_write_v6(FILE *db_fptr, ਬਣਤਰ P_client *chunk)
{
	ਬਣਤਰ PF_ਸਿਰਲੇਖ ਸਿਰਲੇਖ;
	uint16_t id_len = chunk->F.id_len;
	uint16_t username_len = chunk->F.username_len;

	chunk->F.session_expiry_interval = htonl(chunk->F.session_expiry_interval);
	chunk->F.ਆਖਰੀ_mid = htons(chunk->F.ਆਖਰੀ_mid);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.username_len = htons(chunk->F.username_len);
	chunk->F.listener_ਪੋਰਟ = htons(chunk->F.listener_ਪੋਰਟ);

	ਸਿਰਲੇਖ.chunk = htonl(DB_CHUNK_CLIENT);
	ਸਿਰਲੇਖ.ਲੰਮਾਈ = htonl((uint32_t)ਆਕਾਰ(ਬਣਤਰ PF_client)+id_len+username_len);

	write_e(db_fptr, &ਸਿਰਲੇਖ, ਆਕਾਰ(ਬਣਤਰ PF_ਸਿਰਲੇਖ));
	write_e(db_fptr, &chunk->F, ਆਕਾਰ(ਬਣਤਰ PF_client));

	write_e(db_fptr, chunk->clientid, id_len);
	ਜੇ(username_len > 0){
		write_e(db_fptr, chunk->username, username_len);
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
error:
	log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ਵਾਪਸ 1;
}


ਪੂਰਨ persist_chunk_client_ਸੁਨੇਹਾ_write_v6(FILE *db_fptr, ਬਣਤਰ P_client_ਸੁਨੇਹਾ *chunk)
{
	ਬਣਤਰ PF_ਸਿਰਲੇਖ ਸਿਰਲੇਖ;
	ਬਣਤਰ mosquitto__packet *prop_packet = ਨਿਰਾ;
	uint16_t id_len = chunk->F.id_len;
	uint32_t proplen = 0;
	ਪੂਰਨ rc;
	mosquitto_property subscription_id_prop = {
		.ਅਗਲਾ = ਨਿਰਾ, .identifier = MQTT_PROP_SUBSCRIPTION_IDENTIFIER, .client_generated = true, .property_type = MQTT_PROP_TYPE_VARINT
	};

	ਜੇ(chunk->subscription_identifier){
		subscription_id_prop.ਮੁੱਲ.varint = chunk->subscription_identifier;
		proplen += mosquitto_property_get_ਬਚਿਆ_ਲੰਮਾਈ(&subscription_id_prop);
	}

	chunk->F.mid = htons(chunk->F.mid);
	chunk->F.id_len = htons(chunk->F.id_len);

	ਸਿਰਲੇਖ.chunk = htonl(DB_CHUNK_CLIENT_ਸੁਨੇਹਾ);
	ਸਿਰਲੇਖ.ਲੰਮਾਈ = htonl((uint32_t)ਆਕਾਰ(ਬਣਤਰ PF_client_ਸੁਨੇਹਾ) + id_len + proplen);

	write_e(db_fptr, &ਸਿਰਲੇਖ, ਆਕਾਰ(ਬਣਤਰ PF_ਸਿਰਲੇਖ));
	write_e(db_fptr, &chunk->F, ਆਕਾਰ(ਬਣਤਰ PF_client_ਸੁਨੇਹਾ));
	write_e(db_fptr, chunk->clientid, id_len);
	ਜੇ(chunk->subscription_identifier){
		ਜੇ(proplen > 0){
			prop_packet = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto__packet)+proplen);
			ਜੇ(prop_packet == ਨਿਰਾ){
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
			prop_packet->ਬਚਿਆ_ਲੰਮਾਈ = proplen;
			prop_packet->packet_ਲੰਮਾਈ = proplen;
			rc = property__write_all(prop_packet, &subscription_id_prop, true);
			ਜੇ(rc){
				mosquitto_FREE(prop_packet);
				ਵਾਪਸ rc;
			}

			write_e(db_fptr, prop_packet->payload, proplen);
			mosquitto_FREE(prop_packet);
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
error:
	log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(prop_packet);
	ਵਾਪਸ 1;
}


ਪੂਰਨ persist_chunk_ਸੁਨੇਹਾ_store_write_v6(FILE *db_fptr, ਬਣਤਰ P_ਅਧਾਰ_ਸੁਨੇਹਾ *chunk)
{
	ਬਣਤਰ PF_ਸਿਰਲੇਖ ਸਿਰਲੇਖ;
	uint32_t payloadlen = chunk->F.payloadlen;
	uint16_t ਸਰੋਤ_id_len = chunk->F.ਸਰੋਤ_id_len;
	uint16_t ਸਰੋਤ_username_len = chunk->F.ਸਰੋਤ_username_len;
	uint16_t topic_len = chunk->F.topic_len;
	uint32_t proplen = 0;
	ਪੂਰਨ rc;

	ਜੇ(chunk->properties){
		proplen += mosquitto_property_get_ਬਚਿਆ_ਲੰਮਾਈ(chunk->properties);
	}

	chunk->F.payloadlen = htonl(chunk->F.payloadlen);
	chunk->F.ਸਰੋਤ_mid = htons(chunk->F.ਸਰੋਤ_mid);
	chunk->F.ਸਰੋਤ_id_len = htons(chunk->F.ਸਰੋਤ_id_len);
	chunk->F.ਸਰੋਤ_username_len = htons(chunk->F.ਸਰੋਤ_username_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);
	chunk->F.ਸਰੋਤ_ਪੋਰਟ = htons(chunk->F.ਸਰੋਤ_ਪੋਰਟ);

	ਸਿਰਲੇਖ.chunk = htonl(DB_CHUNK_ਅਧਾਰ_ਸੁਨੇਹਾ);
	ਸਿਰਲੇਖ.ਲੰਮਾਈ = htonl((uint32_t)ਆਕਾਰ(ਬਣਤਰ PF_ਅਧਾਰ_ਸੁਨੇਹਾ) +
			topic_len + payloadlen +
			ਸਰੋਤ_id_len + ਸਰੋਤ_username_len + proplen);

	write_e(db_fptr, &ਸਿਰਲੇਖ, ਆਕਾਰ(ਬਣਤਰ PF_ਸਿਰਲੇਖ));
	write_e(db_fptr, &chunk->F, ਆਕਾਰ(ਬਣਤਰ PF_ਅਧਾਰ_ਸੁਨੇਹਾ));
	ਜੇ(ਸਰੋਤ_id_len){
		write_e(db_fptr, chunk->ਸਰੋਤ.id, ਸਰੋਤ_id_len);
	}
	ਜੇ(ਸਰੋਤ_username_len){
		write_e(db_fptr, chunk->ਸਰੋਤ.username, ਸਰੋਤ_username_len);
	}
	write_e(db_fptr, chunk->topic, topic_len);
	ਜੇ(payloadlen){
		write_e(db_fptr, chunk->payload, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)payloadlen);
	}
	ਜੇ(chunk->properties){
		ਜੇ(proplen > 0){
			ਬਣਤਰ mosquitto__packet *prop_packet = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto__packet)+proplen);
			ਜੇ(prop_packet == ਨਿਰਾ){
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
			prop_packet->ਬਚਿਆ_ਲੰਮਾਈ = proplen;
			prop_packet->packet_ਲੰਮਾਈ = proplen;
			rc = property__write_all(prop_packet, chunk->properties, true);
			ਜੇ(rc){
				mosquitto_FREE(prop_packet);
				ਵਾਪਸ rc;
			}

			ਜੇ(fwrite(prop_packet->payload, 1, proplen, db_fptr) != proplen){
				mosquitto_FREE(prop_packet);
				ਜਾਓ error;
			}
			mosquitto_FREE(prop_packet);
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
error:
	log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ਵਾਪਸ 1;
}


ਪੂਰਨ persist__chunk_retain_write_v6(FILE *db_fptr, ਬਣਤਰ P_retain *chunk)
{
	ਬਣਤਰ PF_ਸਿਰਲੇਖ ਸਿਰਲੇਖ;

	ਸਿਰਲੇਖ.chunk = htonl(DB_CHUNK_RETAIN);
	ਸਿਰਲੇਖ.ਲੰਮਾਈ = htonl((uint32_t)ਆਕਾਰ(ਬਣਤਰ PF_retain));

	write_e(db_fptr, &ਸਿਰਲੇਖ, ਆਕਾਰ(ਬਣਤਰ PF_ਸਿਰਲੇਖ));
	write_e(db_fptr, &chunk->F, ਆਕਾਰ(ਬਣਤਰ PF_retain));

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
error:
	log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ਵਾਪਸ 1;
}


ਪੂਰਨ persist__chunk_sub_write_v6(FILE *db_fptr, ਬਣਤਰ P_sub *chunk)
{
	ਬਣਤਰ PF_ਸਿਰਲੇਖ ਸਿਰਲੇਖ;
	uint16_t id_len = chunk->F.id_len;
	uint16_t topic_len = chunk->F.topic_len;

	chunk->F.identifier = htonl(chunk->F.identifier);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);

	ਸਿਰਲੇਖ.chunk = htonl(DB_CHUNK_SUB);
	ਸਿਰਲੇਖ.ਲੰਮਾਈ = htonl((uint32_t)ਆਕਾਰ(ਬਣਤਰ PF_sub) +
			id_len + topic_len);

	write_e(db_fptr, &ਸਿਰਲੇਖ, ਆਕਾਰ(ਬਣਤਰ PF_ਸਿਰਲੇਖ));
	write_e(db_fptr, &chunk->F, ਆਕਾਰ(ਬਣਤਰ PF_sub));
	write_e(db_fptr, chunk->clientid, id_len);
	write_e(db_fptr, chunk->topic, topic_len);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
error:
	log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ਵਾਪਸ 1;
}
#ਜੇ_ਅੰਤ
