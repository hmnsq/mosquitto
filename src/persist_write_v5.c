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

#అన్తర్భావ "config.h"

#నిర్వచితమైతే WITH_PERSISTENCE

#నిర్వచితం_కాకపోతే WIN32
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#అన్తర్భావ <errno.h>
#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <time.h>

#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "persist.h"
#అన్తర్భావ "packet_mosq.h"
#అన్తర్భావ "property_common.h"
#అన్తర్భావ "property_mosq.h"
#అన్తర్భావ "util_mosq.h"


పూర్ణాంకం persist__chunk_cfg_write_v6(FILE *db_fptr, నిర్మాణం PF_cfg *chunk)
{
	నిర్మాణం PF_శీర్షిక శీర్షిక;

	శీర్షిక.chunk = htonl(DB_CHUNK_CFG);
	శీర్షిక.నిడివి = htonl(పరిమాణం(నిర్మాణం PF_cfg));
	write_e(db_fptr, &శీర్షిక, పరిమాణం(నిర్మాణం PF_శీర్షిక));
	write_e(db_fptr, chunk, పరిమాణం(నిర్మాణం PF_cfg));

	తిరిగి MOSQ_ERR_SUCCESS;
error:
	log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	తిరిగి 1;
}


పూర్ణాంకం persist__chunk_client_write_v6(FILE *db_fptr, నిర్మాణం P_client *chunk)
{
	నిర్మాణం PF_శీర్షిక శీర్షిక;
	uint16_t id_len = chunk->F.id_len;
	uint16_t username_len = chunk->F.username_len;

	chunk->F.session_expiry_interval = htonl(chunk->F.session_expiry_interval);
	chunk->F.చివరి_mid = htons(chunk->F.చివరి_mid);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.username_len = htons(chunk->F.username_len);
	chunk->F.listener_పోర్టు = htons(chunk->F.listener_పోర్టు);

	శీర్షిక.chunk = htonl(DB_CHUNK_CLIENT);
	శీర్షిక.నిడివి = htonl((uint32_t)పరిమాణం(నిర్మాణం PF_client)+id_len+username_len);

	write_e(db_fptr, &శీర్షిక, పరిమాణం(నిర్మాణం PF_శీర్షిక));
	write_e(db_fptr, &chunk->F, పరిమాణం(నిర్మాణం PF_client));

	write_e(db_fptr, chunk->clientid, id_len);
	ఒకవేళ(username_len > 0){
		write_e(db_fptr, chunk->username, username_len);
	}

	తిరిగి MOSQ_ERR_SUCCESS;
error:
	log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	తిరిగి 1;
}


పూర్ణాంకం persist_chunk_client_సందేశం_write_v6(FILE *db_fptr, నిర్మాణం P_client_సందేశం *chunk)
{
	నిర్మాణం PF_శీర్షిక శీర్షిక;
	నిర్మాణం mosquitto__packet *prop_packet = ఖాళీ;
	uint16_t id_len = chunk->F.id_len;
	uint32_t proplen = 0;
	పూర్ణాంకం rc;
	mosquitto_property subscription_id_prop = {
		.తదుపరి = ఖాళీ, .identifier = MQTT_PROP_SUBSCRIPTION_IDENTIFIER, .client_generated = true, .property_type = MQTT_PROP_TYPE_VARINT
	};

	ఒకవేళ(chunk->subscription_identifier){
		subscription_id_prop.విలువ.varint = chunk->subscription_identifier;
		proplen += mosquitto_property_get_మిగిలిన_నిడివి(&subscription_id_prop);
	}

	chunk->F.mid = htons(chunk->F.mid);
	chunk->F.id_len = htons(chunk->F.id_len);

	శీర్షిక.chunk = htonl(DB_CHUNK_CLIENT_సందేశం);
	శీర్షిక.నిడివి = htonl((uint32_t)పరిమాణం(నిర్మాణం PF_client_సందేశం) + id_len + proplen);

	write_e(db_fptr, &శీర్షిక, పరిమాణం(నిర్మాణం PF_శీర్షిక));
	write_e(db_fptr, &chunk->F, పరిమాణం(నిర్మాణం PF_client_సందేశం));
	write_e(db_fptr, chunk->clientid, id_len);
	ఒకవేళ(chunk->subscription_identifier){
		ఒకవేళ(proplen > 0){
			prop_packet = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto__packet)+proplen);
			ఒకవేళ(prop_packet == ఖాళీ){
				తిరిగి MOSQ_ERR_NOMEM;
			}
			prop_packet->మిగిలిన_నిడివి = proplen;
			prop_packet->packet_నిడివి = proplen;
			rc = property__write_all(prop_packet, &subscription_id_prop, true);
			ఒకవేళ(rc){
				mosquitto_FREE(prop_packet);
				తిరిగి rc;
			}

			write_e(db_fptr, prop_packet->payload, proplen);
			mosquitto_FREE(prop_packet);
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
error:
	log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	mosquitto_FREE(prop_packet);
	తిరిగి 1;
}


పూర్ణాంకం persist_chunk_సందేశం_store_write_v6(FILE *db_fptr, నిర్మాణం P_పునాది_సందేశం *chunk)
{
	నిర్మాణం PF_శీర్షిక శీర్షిక;
	uint32_t payloadlen = chunk->F.payloadlen;
	uint16_t ఆధారం_id_len = chunk->F.ఆధారం_id_len;
	uint16_t ఆధారం_username_len = chunk->F.ఆధారం_username_len;
	uint16_t topic_len = chunk->F.topic_len;
	uint32_t proplen = 0;
	పూర్ణాంకం rc;

	ఒకవేళ(chunk->properties){
		proplen += mosquitto_property_get_మిగిలిన_నిడివి(chunk->properties);
	}

	chunk->F.payloadlen = htonl(chunk->F.payloadlen);
	chunk->F.ఆధారం_mid = htons(chunk->F.ఆధారం_mid);
	chunk->F.ఆధారం_id_len = htons(chunk->F.ఆధారం_id_len);
	chunk->F.ఆధారం_username_len = htons(chunk->F.ఆధారం_username_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);
	chunk->F.ఆధారం_పోర్టు = htons(chunk->F.ఆధారం_పోర్టు);

	శీర్షిక.chunk = htonl(DB_CHUNK_పునాది_సందేశం);
	శీర్షిక.నిడివి = htonl((uint32_t)పరిమాణం(నిర్మాణం PF_పునాది_సందేశం) +
			topic_len + payloadlen +
			ఆధారం_id_len + ఆధారం_username_len + proplen);

	write_e(db_fptr, &శీర్షిక, పరిమాణం(నిర్మాణం PF_శీర్షిక));
	write_e(db_fptr, &chunk->F, పరిమాణం(నిర్మాణం PF_పునాది_సందేశం));
	ఒకవేళ(ఆధారం_id_len){
		write_e(db_fptr, chunk->ఆధారం.id, ఆధారం_id_len);
	}
	ఒకవేళ(ఆధారం_username_len){
		write_e(db_fptr, chunk->ఆధారం.username, ఆధారం_username_len);
	}
	write_e(db_fptr, chunk->topic, topic_len);
	ఒకవేళ(payloadlen){
		write_e(db_fptr, chunk->payload, (గుర్తులేని పూర్ణాంకం)payloadlen);
	}
	ఒకవేళ(chunk->properties){
		ఒకవేళ(proplen > 0){
			నిర్మాణం mosquitto__packet *prop_packet = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto__packet)+proplen);
			ఒకవేళ(prop_packet == ఖాళీ){
				తిరిగి MOSQ_ERR_NOMEM;
			}
			prop_packet->మిగిలిన_నిడివి = proplen;
			prop_packet->packet_నిడివి = proplen;
			rc = property__write_all(prop_packet, chunk->properties, true);
			ఒకవేళ(rc){
				mosquitto_FREE(prop_packet);
				తిరిగి rc;
			}

			ఒకవేళ(fwrite(prop_packet->payload, 1, proplen, db_fptr) != proplen){
				mosquitto_FREE(prop_packet);
				వెళ్ళు error;
			}
			mosquitto_FREE(prop_packet);
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
error:
	log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	తిరిగి 1;
}


పూర్ణాంకం persist__chunk_retain_write_v6(FILE *db_fptr, నిర్మాణం P_retain *chunk)
{
	నిర్మాణం PF_శీర్షిక శీర్షిక;

	శీర్షిక.chunk = htonl(DB_CHUNK_RETAIN);
	శీర్షిక.నిడివి = htonl((uint32_t)పరిమాణం(నిర్మాణం PF_retain));

	write_e(db_fptr, &శీర్షిక, పరిమాణం(నిర్మాణం PF_శీర్షిక));
	write_e(db_fptr, &chunk->F, పరిమాణం(నిర్మాణం PF_retain));

	తిరిగి MOSQ_ERR_SUCCESS;
error:
	log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	తిరిగి 1;
}


పూర్ణాంకం persist__chunk_sub_write_v6(FILE *db_fptr, నిర్మాణం P_sub *chunk)
{
	నిర్మాణం PF_శీర్షిక శీర్షిక;
	uint16_t id_len = chunk->F.id_len;
	uint16_t topic_len = chunk->F.topic_len;

	chunk->F.identifier = htonl(chunk->F.identifier);
	chunk->F.id_len = htons(chunk->F.id_len);
	chunk->F.topic_len = htons(chunk->F.topic_len);

	శీర్షిక.chunk = htonl(DB_CHUNK_SUB);
	శీర్షిక.నిడివి = htonl((uint32_t)పరిమాణం(నిర్మాణం PF_sub) +
			id_len + topic_len);

	write_e(db_fptr, &శీర్షిక, పరిమాణం(నిర్మాణం PF_శీర్షిక));
	write_e(db_fptr, &chunk->F, పరిమాణం(నిర్మాణం PF_sub));
	write_e(db_fptr, chunk->clientid, id_len);
	write_e(db_fptr, chunk->topic, topic_len);

	తిరిగి MOSQ_ERR_SUCCESS;
error:
	log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: %s.", strerror(errno));
	తిరిగి 1;
}
#అయితే_ముగింపు
