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

#که_تعریف_شوی WITH_PERSISTENCE

#که_نه_تعریف_شوی WIN32
#ضمن <arpa/inet.h>
#د_که_پای
#ضمن <errno.h>
#ضمن <fcntl.h>
#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <sys/stat.h>
#ضمن <time.h>

#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "persist.h"
#ضمن "packet_mosq.h"
#ضمن "property_common.h"
#ضمن "property_mosq.h"
#ضمن "util_mosq.h"


صحيح persist__chunk_cfg_write_v6(FILE *db_fptr, بنية PF_cfg *chunk)
{
	بنية PF_سرلیک سرلیک;

	سرلیک.chunk = htonl(DB_CHUNK_CFG);
	سرلیک.اوږدتیا = htonl(حجم(بنية PF_cfg));
	write_e(db_fptr, &سرلیک, حجم(بنية PF_سرلیک));
	write_e(db_fptr, chunk, حجم(بنية PF_cfg));

	أرجع MOSQ_ERR_SUCCESS;
error:
	log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	أرجع 1;
}


صحيح persist__chunk_client_write_v6(FILE *db_fptr, بنية P_client *chunk)
{
	بنية PF_سرلیک سرلیک;
	uint16_t id_len = chunk->F.id_len;
	uint16_t username_len = chunk->F.username_len;

	chunk->F.session_expiry_interval = htonl(chunk->F.session_expiry_interval);
	chunk->F.وروستی_mid = htons(chunk->F.وروستی_mid);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.username_len = htons(chunk->F.username_len);
	chunk->F.listener_پورټ = htons(chunk->F.listener_پورټ);

	سرلیک.chunk = htonl(DB_CHUNK_CLIENT);
	سرلیک.اوږدتیا = htonl((uint32_t)حجم(بنية PF_client)+id_len+username_len);

	write_e(db_fptr, &سرلیک, حجم(بنية PF_سرلیک));
	write_e(db_fptr, &chunk->F, حجم(بنية PF_client));

	write_e(db_fptr, chunk->clientid, id_len);
	إذا(username_len > 0){
		write_e(db_fptr, chunk->username, username_len);
	}

	أرجع MOSQ_ERR_SUCCESS;
error:
	log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	أرجع 1;
}


صحيح persist_chunk_client_پیغام_write_v6(FILE *db_fptr, بنية P_client_پیغام *chunk)
{
	بنية PF_سرلیک سرلیک;
	بنية mosquitto__packet *prop_packet = عدم;
	uint16_t id_len = chunk->F.id_len;
	uint32_t proplen = 0;
	صحيح rc;
	mosquitto_property subscription_id_prop = {
		.بل = عدم, .identifier = MQTT_PROP_SUBSCRIPTION_IDENTIFIER, .client_generated = true, .property_type = MQTT_PROP_TYPE_VARINT
	};

	إذا(chunk->subscription_identifier){
		subscription_id_prop.ارزښت.varint = chunk->subscription_identifier;
		proplen += mosquitto_property_get_پاتې_شوی_اوږدتیا(&subscription_id_prop);
	}

	chunk->F.mid = htons(chunk->F.mid);
	chunk->F.id_len = htons(chunk->F.id_len);

	سرلیک.chunk = htonl(DB_CHUNK_CLIENT_پیغام);
	سرلیک.اوږدتیا = htonl((uint32_t)حجم(بنية PF_client_پیغام) + id_len + proplen);

	write_e(db_fptr, &سرلیک, حجم(بنية PF_سرلیک));
	write_e(db_fptr, &chunk->F, حجم(بنية PF_client_پیغام));
	write_e(db_fptr, chunk->clientid, id_len);
	إذا(chunk->subscription_identifier){
		إذا(proplen > 0){
			prop_packet = mosquitto_calloc(1, حجم(بنية mosquitto__packet)+proplen);
			إذا(prop_packet == عدم){
				أرجع MOSQ_ERR_NOMEM;
			}
			prop_packet->پاتې_شوی_اوږدتیا = proplen;
			prop_packet->packet_اوږدتیا = proplen;
			rc = property__write_all(prop_packet, &subscription_id_prop, true);
			إذا(rc){
				mosquitto_FREE(prop_packet);
				أرجع rc;
			}

			write_e(db_fptr, prop_packet->payload, proplen);
			mosquitto_FREE(prop_packet);
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
error:
	log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(prop_packet);
	أرجع 1;
}


صحيح persist_chunk_پیغام_store_write_v6(FILE *db_fptr, بنية P_بنسټ_پیغام *chunk)
{
	بنية PF_سرلیک سرلیک;
	uint32_t payloadlen = chunk->F.payloadlen;
	uint16_t سرچینه_id_len = chunk->F.سرچینه_id_len;
	uint16_t سرچینه_username_len = chunk->F.سرچینه_username_len;
	uint16_t topic_len = chunk->F.topic_len;
	uint32_t proplen = 0;
	صحيح rc;

	إذا(chunk->properties){
		proplen += mosquitto_property_get_پاتې_شوی_اوږدتیا(chunk->properties);
	}

	chunk->F.payloadlen = htonl(chunk->F.payloadlen);
	chunk->F.سرچینه_mid = htons(chunk->F.سرچینه_mid);
	chunk->F.سرچینه_id_len = htons(chunk->F.سرچینه_id_len);
	chunk->F.سرچینه_username_len = htons(chunk->F.سرچینه_username_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);
	chunk->F.سرچینه_پورټ = htons(chunk->F.سرچینه_پورټ);

	سرلیک.chunk = htonl(DB_CHUNK_بنسټ_پیغام);
	سرلیک.اوږدتیا = htonl((uint32_t)حجم(بنية PF_بنسټ_پیغام) +
			topic_len + payloadlen +
			سرچینه_id_len + سرچینه_username_len + proplen);

	write_e(db_fptr, &سرلیک, حجم(بنية PF_سرلیک));
	write_e(db_fptr, &chunk->F, حجم(بنية PF_بنسټ_پیغام));
	إذا(سرچینه_id_len){
		write_e(db_fptr, chunk->سرچینه.id, سرچینه_id_len);
	}
	إذا(سرچینه_username_len){
		write_e(db_fptr, chunk->سرچینه.username, سرچینه_username_len);
	}
	write_e(db_fptr, chunk->topic, topic_len);
	إذا(payloadlen){
		write_e(db_fptr, chunk->payload, (بینښه صحيح)payloadlen);
	}
	إذا(chunk->properties){
		إذا(proplen > 0){
			بنية mosquitto__packet *prop_packet = mosquitto_calloc(1, حجم(بنية mosquitto__packet)+proplen);
			إذا(prop_packet == عدم){
				أرجع MOSQ_ERR_NOMEM;
			}
			prop_packet->پاتې_شوی_اوږدتیا = proplen;
			prop_packet->packet_اوږدتیا = proplen;
			rc = property__write_all(prop_packet, chunk->properties, true);
			إذا(rc){
				mosquitto_FREE(prop_packet);
				أرجع rc;
			}

			إذا(fwrite(prop_packet->payload, 1, proplen, db_fptr) != proplen){
				mosquitto_FREE(prop_packet);
				اذهب error;
			}
			mosquitto_FREE(prop_packet);
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
error:
	log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	أرجع 1;
}


صحيح persist__chunk_retain_write_v6(FILE *db_fptr, بنية P_retain *chunk)
{
	بنية PF_سرلیک سرلیک;

	سرلیک.chunk = htonl(DB_CHUNK_RETAIN);
	سرلیک.اوږدتیا = htonl((uint32_t)حجم(بنية PF_retain));

	write_e(db_fptr, &سرلیک, حجم(بنية PF_سرلیک));
	write_e(db_fptr, &chunk->F, حجم(بنية PF_retain));

	أرجع MOSQ_ERR_SUCCESS;
error:
	log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	أرجع 1;
}


صحيح persist__chunk_sub_write_v6(FILE *db_fptr, بنية P_sub *chunk)
{
	بنية PF_سرلیک سرلیک;
	uint16_t id_len = chunk->F.id_len;
	uint16_t topic_len = chunk->F.topic_len;

	chunk->F.identifier = htonl(chunk->F.identifier);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);

	سرلیک.chunk = htonl(DB_CHUNK_SUB);
	سرلیک.اوږدتیا = htonl((uint32_t)حجم(بنية PF_sub) +
			id_len + topic_len);

	write_e(db_fptr, &سرلیک, حجم(بنية PF_سرلیک));
	write_e(db_fptr, &chunk->F, حجم(بنية PF_sub));
	write_e(db_fptr, chunk->clientid, id_len);
	write_e(db_fptr, chunk->topic, topic_len);

	أرجع MOSQ_ERR_SUCCESS;
error:
	log__printf(عدم, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	أرجع 1;
}
#د_که_پای
