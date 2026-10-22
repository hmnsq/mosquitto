/*
Copyright (c) 2019-2021 Roger Light <roger@atchoo.org>

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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "property_mosq.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"


ಪೂರ್ಣಾಙ್ಕ send__auth(ಸಂರಚನಾ mosquitto *context, uint8_t reason_code, ಧ್ರುವ ಶೂನ್ಯ *auth_data, uint16_t auth_data_len)
{
	ಸಂರಚನಾ mosquitto__packet *packet = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ rc;
	mosquitto_property *properties = ರಿಕ್ತ;
	uint32_t remaining_length;

	ಯದಿ(context->auth_method == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಯದಿ(context->protocol != mosq_p_mqtt5){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: Sending AUTH packet when session not MQTT v5.0.", context->id);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
	}

	log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "Sending AUTH to %s (rc%d, %s)", context->id, reason_code, context->auth_method);

	remaining_length = 1;

	rc = mosquitto_property_add_string(&properties, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method);
	ಯದಿ(rc){
		ಗಚ್ಛ error;
	}

	ಯದಿ(auth_data != ರಿಕ್ತ && auth_data_len > 0){
		rc = mosquitto_property_add_ದ್ವಿಮಾನ(&properties, MQTT_PROP_AUTHENTICATION_DATA, auth_data, auth_data_len);
		ಯದಿ(rc){
			ಗಚ್ಛ error;
		}
	}

	remaining_length += mosquitto_property_get_remaining_length(properties);

	rc = packet__check_oversize(context, remaining_length);
	ಯದಿ(rc){
		ಗಚ್ಛ error;
	}

	rc = packet__alloc(&packet, CMD_AUTH, remaining_length);
	ಯದಿ(rc){
		ಗಚ್ಛ error;
	}

	packet__write_byte(packet, reason_code);
	property__write_all(packet, properties, true);
	mosquitto_property_free_all(&properties);

	metrics__int_inc(mosq_counter_mqtt_auth_sent, 1);
	ಪ್ರತ್ಯಾಗಮ packet__queue(context, packet);
error:
	mosquitto_property_free_all(&properties);
	ಪ್ರತ್ಯಾಗಮ rc;
}
