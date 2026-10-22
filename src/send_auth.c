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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "packet_mosq.h"
#அந்தர்பாவ "property_mosq.h"
#அந்தர்பாவ "sys_tree.h"
#அந்தர்பாவ "util_mosq.h"


முழு send__auth(கட்டமைப்பு mosquitto *context, uint8_t reason_code, மாறிலி வெறுமை *auth_data, uint16_t auth_data_len)
{
	கட்டமைப்பு mosquitto__packet *packet = வெற்று;
	முழு rc;
	mosquitto_property *properties = வெற்று;
	uint32_t remaining_length;

	எனில்(context->auth_method == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}
	எனில்(context->protocol != mosq_p_mqtt5){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: Sending AUTH packet when session not MQTT v5.0.", context->id);
		திரும்பு MOSQ_ERR_PROTOCOL;
	}

	log__printf(வெற்று, MOSQ_LOG_DEBUG, "Sending AUTH to %s (rc%d, %s)", context->id, reason_code, context->auth_method);

	remaining_length = 1;

	rc = mosquitto_property_add_string(&properties, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method);
	எனில்(rc){
		செல் error;
	}

	எனில்(auth_data != வெற்று && auth_data_len > 0){
		rc = mosquitto_property_add_இருமம்(&properties, MQTT_PROP_AUTHENTICATION_DATA, auth_data, auth_data_len);
		எனில்(rc){
			செல் error;
		}
	}

	remaining_length += mosquitto_property_get_remaining_length(properties);

	rc = packet__check_oversize(context, remaining_length);
	எனில்(rc){
		செல் error;
	}

	rc = packet__alloc(&packet, CMD_AUTH, remaining_length);
	எனில்(rc){
		செல் error;
	}

	packet__write_byte(packet, reason_code);
	property__write_all(packet, properties, true);
	mosquitto_property_free_all(&properties);

	metrics__int_inc(mosq_counter_mqtt_auth_sent, 1);
	திரும்பு packet__queue(context, packet);
error:
	mosquitto_property_free_all(&properties);
	திரும்பு rc;
}
