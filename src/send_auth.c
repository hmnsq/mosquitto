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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "property_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sys_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"


ਪੂਰਨ send__auth(ਬਣਤਰ mosquitto *context, uint8_t reason_ਕੋਡ, ਅਚਲ ਖਾਲੀ *auth_ਅੰਕੜੇ, uint16_t auth_ਅੰਕੜੇ_len)
{
	ਬਣਤਰ mosquitto__packet *packet = ਨਿਰਾ;
	ਪੂਰਨ rc;
	mosquitto_property *properties = ਨਿਰਾ;
	uint32_t ਬਚਿਆ_ਲੰਮਾਈ;

	ਜੇ(context->auth_ਵਿਧੀ == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਜੇ(context->protocol != mosq_p_mqtt5){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: Sending AUTH packet when session not MQTT v5.0.", context->id);
		ਵਾਪਸ MOSQ_ERR_PROTOCOL;
	}

	log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Sending AUTH to %s (rc%d, %s)", context->id, reason_ਕੋਡ, context->auth_ਵਿਧੀ);

	ਬਚਿਆ_ਲੰਮਾਈ = 1;

	rc = mosquitto_property_ਜੋੜੋ_string(&properties, MQTT_PROP_AUTHENTICATION_ਵਿਧੀ, context->auth_ਵਿਧੀ);
	ਜੇ(rc){
		ਜਾਓ error;
	}

	ਜੇ(auth_ਅੰਕੜੇ != ਨਿਰਾ && auth_ਅੰਕੜੇ_len > 0){
		rc = mosquitto_property_ਜੋੜੋ_ਦੁਆਧਾਰੀ(&properties, MQTT_PROP_AUTHENTICATION_ਅੰਕੜੇ, auth_ਅੰਕੜੇ, auth_ਅੰਕੜੇ_len);
		ਜੇ(rc){
			ਜਾਓ error;
		}
	}

	ਬਚਿਆ_ਲੰਮਾਈ += mosquitto_property_get_ਬਚਿਆ_ਲੰਮਾਈ(properties);

	rc = packet__check_oversize(context, ਬਚਿਆ_ਲੰਮਾਈ);
	ਜੇ(rc){
		ਜਾਓ error;
	}

	rc = packet__alloc(&packet, CMD_AUTH, ਬਚਿਆ_ਲੰਮਾਈ);
	ਜੇ(rc){
		ਜਾਓ error;
	}

	packet__write_byte(packet, reason_ਕੋਡ);
	property__write_all(packet, properties, true);
	mosquitto_property_free_all(&properties);

	metrics__int_inc(mosq_counter_mqtt_auth_ਭੇਜਿਆ, 1);
	ਵਾਪਸ packet_ਕਤਾਰ(context, packet);
error:
	mosquitto_property_free_all(&properties);
	ਵਾਪਸ rc;
}
