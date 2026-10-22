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


ಪೂರ್ಣಾಂಕ send__auth(ರಚನೆ mosquitto *context, uint8_t reason_ಸಂಕೇತ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *auth_ದತ್ತಾಂಶ, uint16_t auth_ದತ್ತಾಂಶ_len)
{
	ರಚನೆ mosquitto__packet *packet = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ rc;
	mosquitto_property *properties = ಖಾಲಿ;
	uint32_t ಉಳಿದ_ಉದ್ದಳತೆ;

	ಆದರೆ(context->auth_ವಿಧಾನ == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	ಆದರೆ(context->protocol != mosq_p_mqtt5){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: Sending AUTH packet when session not MQTT v5.0.", context->id);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
	}

	log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "Sending AUTH to %s (rc%d, %s)", context->id, reason_ಸಂಕೇತ, context->auth_ವಿಧಾನ);

	ಉಳಿದ_ಉದ್ದಳತೆ = 1;

	rc = mosquitto_property_ಕೂಡಿಸು_string(&properties, MQTT_PROP_AUTHENTICATION_ವಿಧಾನ, context->auth_ವಿಧಾನ);
	ಆದರೆ(rc){
		ಹೋಗು error;
	}

	ಆದರೆ(auth_ದತ್ತಾಂಶ != ಖಾಲಿ && auth_ದತ್ತಾಂಶ_len > 0){
		rc = mosquitto_property_ಕೂಡಿಸು_ದ್ವಿಮಾನ(&properties, MQTT_PROP_AUTHENTICATION_ದತ್ತಾಂಶ, auth_ದತ್ತಾಂಶ, auth_ದತ್ತಾಂಶ_len);
		ಆದರೆ(rc){
			ಹೋಗು error;
		}
	}

	ಉಳಿದ_ಉದ್ದಳತೆ += mosquitto_property_get_ಉಳಿದ_ಉದ್ದಳತೆ(properties);

	rc = packet__check_oversize(context, ಉಳಿದ_ಉದ್ದಳತೆ);
	ಆದರೆ(rc){
		ಹೋಗು error;
	}

	rc = packet__alloc(&packet, CMD_AUTH, ಉಳಿದ_ಉದ್ದಳತೆ);
	ಆದರೆ(rc){
		ಹೋಗು error;
	}

	packet__write_byte(packet, reason_ಸಂಕೇತ);
	property__write_all(packet, properties, true);
	mosquitto_property_free_all(&properties);

	metrics__int_inc(mosq_counter_mqtt_auth_ಕಳುಹಿಸಿದ, 1);
	ಹಿಂತಿರುಗಿ packet_ಸರತಿ(context, packet);
error:
	mosquitto_property_free_all(&properties);
	ಹಿಂತಿರುಗಿ rc;
}
