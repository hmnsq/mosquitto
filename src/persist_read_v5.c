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

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "persist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "property_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"


ਪੂਰਨ persist_chunk_ਸਿਰਲੇਖ_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *ਲੰਮਾਈ)
{
	size_t rlen;
	ਬਣਤਰ PF_ਸਿਰਲੇਖ ਸਿਰਲੇਖ;

	rlen = fread(&ਸਿਰਲੇਖ, ਆਕਾਰ(ਬਣਤਰ PF_ਸਿਰਲੇਖ), 1, db_fptr);
	ਜੇ(rlen != 1){
		ਵਾਪਸ 1;
	}

	*chunk = ntohl(ਸਿਰਲੇਖ.chunk);
	*ਲੰਮਾਈ = ntohl(ਸਿਰਲੇਖ.ਲੰਮਾਈ);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ persist__chunk_cfg_read_v56(FILE *db_fptr, ਬਣਤਰ PF_cfg *chunk)
{
	ਜੇ(fread(chunk, ਆਕਾਰ(ਬਣਤਰ PF_cfg), 1, db_fptr) != 1){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		ਵਾਪਸ 1;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ persist__chunk_client_read_v56(FILE *db_fptr, ਬਣਤਰ P_client *chunk, uint32_t db_ਸੰਸਕਰਣ)
{
	ਪੂਰਨ rc;

	ਜੇ(db_ਸੰਸਕਰਣ == 6){
		read_e(db_fptr, &chunk->F, ਆਕਾਰ(ਬਣਤਰ PF_client));
		chunk->F.username_len = ntohs(chunk->F.username_len);
		chunk->F.listener_ਪੋਰਟ = ntohs(chunk->F.listener_ਪੋਰਟ);
	}ਨਹੀਂਤਾਂ ਜੇ(db_ਸੰਸਕਰਣ == 5){
		read_e(db_fptr, &chunk->F, ਆਕਾਰ(ਬਣਤਰ PF_client_v5));
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ 1;
	}

	chunk->F.session_expiry_interval = ntohl(chunk->F.session_expiry_interval);
	chunk->F.ਆਖਰੀ_mid = ntohs(chunk->F.ਆਖਰੀ_mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);


	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	ਜੇ(rc){
		ਵਾਪਸ 1;
	}ਨਹੀਂਤਾਂ ਜੇ(chunk->clientid == ਨਿਰਾ){
		ਵਾਪਸ -1;
	}

	ਜੇ(chunk->F.username_len > 0){
		rc = persist__read_string_len(db_fptr, &chunk->username, chunk->F.username_len);
		ਜੇ(rc || !chunk->username){
			mosquitto_FREE(chunk->clientid);
			ਵਾਪਸ 1;
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
error:
	log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ਵਾਪਸ 1;
}


ਪੂਰਨ persist_chunk_client_ਸੁਨੇਹਾ_read_v56(FILE *db_fptr, ਬਣਤਰ P_client_ਸੁਨੇਹਾ *chunk, uint32_t ਲੰਮਾਈ)
{
	mosquitto_property *properties = ਨਿਰਾ, *p;
	ਬਣਤਰ mosquitto_packet_ਵਿੱਚ prop_packet;
	ਪੂਰਨ rc;

	memset(&prop_packet, 0, ਆਕਾਰ(ਬਣਤਰ mosquitto_packet_ਵਿੱਚ));

	read_e(db_fptr, &chunk->F, ਆਕਾਰ(ਬਣਤਰ PF_client_ਸੁਨੇਹਾ));
	chunk->F.mid = ntohs(chunk->F.mid);
	chunk->F.id_len = ntohs(chunk->F.id_len);

	ਲੰਮਾਈ -= (uint32_t)(ਆਕਾਰ(ਬਣਤਰ PF_client_ਸੁਨੇਹਾ) + chunk->F.id_len);
	ਜੇ(ਲੰਮਾਈ > MQTT_MAX_PAYLOAD){
		ਜਾਓ error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	ਜੇ(ਲੰਮਾਈ > 0){
		prop_packet.ਬਚਿਆ_ਲੰਮਾਈ = ਲੰਮਾਈ;
		prop_packet.payload = mosquitto_malloc(ਲੰਮਾਈ);
		ਜੇ(!prop_packet.payload){
			errno = ENOMEM;
			ਜਾਓ error;
		}

		read_e(db_fptr, prop_packet.payload, ਲੰਮਾਈ);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		ਜੇ(rc){
			mosquitto_FREE(chunk->clientid);
			ਵਾਪਸ rc;
		}

		ਜੇ(properties){
			p = properties;
			ਜਦੋਂ(p){
				ਜੇ(mosquitto_property_identifier(p) == MQTT_PROP_SUBSCRIPTION_IDENTIFIER){
					chunk->subscription_identifier = mosquitto_property_varint_ਮੁੱਲ(p);
				}
				p = mosquitto_property_ਅਗਲਾ(p);
			}
			mosquitto_property_free_all(&properties);
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	mosquitto_FREE(prop_packet.payload);
	log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	ਵਾਪਸ 1;
}


ਪੂਰਨ persist_chunk_ਅਧਾਰ_ਸੁਨੇਹਾ_read_v56(FILE *db_fptr, ਬਣਤਰ P_ਅਧਾਰ_ਸੁਨੇਹਾ *chunk, uint32_t ਲੰਮਾਈ)
{
	ਪੂਰਨ rc = 0;
	mosquitto_property *properties = ਨਿਰਾ;
	ਬਣਤਰ mosquitto_packet_ਵਿੱਚ prop_packet;

	memset(&prop_packet, 0, ਆਕਾਰ(ਬਣਤਰ mosquitto_packet_ਵਿੱਚ));

	read_e(db_fptr, &chunk->F, ਆਕਾਰ(ਬਣਤਰ PF_ਅਧਾਰ_ਸੁਨੇਹਾ));
	chunk->F.payloadlen = ntohl(chunk->F.payloadlen);
	ਜੇ(chunk->F.payloadlen > MQTT_MAX_PAYLOAD){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	chunk->F.ਸਰੋਤ_mid = ntohs(chunk->F.ਸਰੋਤ_mid);
	chunk->F.ਸਰੋਤ_id_len = ntohs(chunk->F.ਸਰੋਤ_id_len);
	chunk->F.ਸਰੋਤ_username_len = ntohs(chunk->F.ਸਰੋਤ_username_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);
	chunk->F.ਸਰੋਤ_ਪੋਰਟ = ntohs(chunk->F.ਸਰੋਤ_ਪੋਰਟ);

	ਲੰਮਾਈ -= (uint32_t)(ਆਕਾਰ(ਬਣਤਰ PF_ਅਧਾਰ_ਸੁਨੇਹਾ) + chunk->F.payloadlen + chunk->F.ਸਰੋਤ_id_len + chunk->F.ਸਰੋਤ_username_len + chunk->F.topic_len);
	ਜੇ(ਲੰਮਾਈ > MQTT_MAX_PAYLOAD){
		ਜਾਓ error;
	}

	ਜੇ(chunk->F.ਸਰੋਤ_id_len){
		rc = persist__read_string_len(db_fptr, &chunk->ਸਰੋਤ.id, chunk->F.ਸਰੋਤ_id_len);
		ਜੇ(rc){
			ਜਾਓ error;
		}
	}
	ਜੇ(chunk->F.ਸਰੋਤ_username_len){
		rc = persist__read_string_len(db_fptr, &chunk->ਸਰੋਤ.username, chunk->F.ਸਰੋਤ_username_len);
		ਜੇ(rc){
			ਜਾਓ error;
		}
	}
	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	ਜੇ(rc){
		ਜਾਓ error;
	}

	ਜੇ(chunk->F.payloadlen > 0){
		chunk->payload = mosquitto_malloc(chunk->F.payloadlen+1);
		ਜੇ(chunk->payload == ਨਿਰਾ){
			rc = MOSQ_ERR_NOMEM;
			ਜਾਓ error;
		}
		read_e(db_fptr, chunk->payload, chunk->F.payloadlen);
		/* Ensure zero terminated regardless of contents */
		((uint8_t *)chunk->payload)[chunk->F.payloadlen] = 0;
	}

	ਜੇ(ਲੰਮਾਈ > 0){
		prop_packet.ਬਚਿਆ_ਲੰਮਾਈ = ਲੰਮਾਈ;
		prop_packet.payload = mosquitto_malloc(ਲੰਮਾਈ);
		ਜੇ(!prop_packet.payload){
			rc = MOSQ_ERR_NOMEM;
			ਜਾਓ error;
		}
		read_e(db_fptr, prop_packet.payload, ਲੰਮਾਈ);
		rc = property__read_all(CMD_PUBLISH, &prop_packet, &properties);
		mosquitto_FREE(prop_packet.payload);
		ਜੇ(rc){
			rc = MOSQ_ERR_NOMEM;
			ਜਾਓ error;
		}
	}
	chunk->properties = properties;

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->payload);
	mosquitto_FREE(chunk->ਸਰੋਤ.id);
	mosquitto_FREE(chunk->ਸਰੋਤ.username);
	mosquitto_FREE(chunk->topic);
	mosquitto_FREE(prop_packet.payload);
	ਵਾਪਸ rc;
}


ਪੂਰਨ persist__chunk_retain_read_v56(FILE *db_fptr, ਬਣਤਰ P_retain *chunk)
{
	ਜੇ(fread(&chunk->F, ਆਕਾਰ(ਬਣਤਰ P_retain), 1, db_fptr) != 1){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
		ਵਾਪਸ 1;
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ persist__chunk_sub_read_v56(FILE *db_fptr, ਬਣਤਰ P_sub *chunk)
{
	ਪੂਰਨ rc = MOSQ_ERR_SUCCESS;

	read_e(db_fptr, &chunk->F, ਆਕਾਰ(ਬਣਤਰ PF_sub));
	chunk->F.identifier = ntohl(chunk->F.identifier);
	chunk->F.id_len = ntohs(chunk->F.id_len);
	chunk->F.topic_len = ntohs(chunk->F.topic_len);

	rc = persist__read_string_len(db_fptr, &chunk->clientid, chunk->F.id_len);
	ਜੇ(rc){
		ਜਾਓ error;
	}

	rc = persist__read_string_len(db_fptr, &chunk->topic, chunk->F.topic_len);
	ਜੇ(rc){
		ਜਾਓ error;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
error:
	mosquitto_FREE(chunk->clientid);
	ਵਾਪਸ rc;
}

#ਜੇ_ਅੰਤ
