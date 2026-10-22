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

#شامل "config.h"

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "packet_mosq.h"
#شامل "property_mosq.h"
#شامل "sys_tree.h"
#شامل "util_mosq.h"


صحیح send__auth(ساختار mosquitto *context, uint8_t reason_code, ثابت تهی *auth_data, uint16_t auth_data_len)
{
	ساختار mosquitto__packet *packet = پوچ;
	صحیح rc;
	mosquitto_property *properties = پوچ;
	uint32_t remaining_length;

	اگر(context->auth_method == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}
	اگر(context->protocol != mosq_p_mqtt5){
		log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s: Sending AUTH packet when session not MQTT v5.0.", context->id);
		بازگشت MOSQ_ERR_PROTOCOL;
	}

	log__printf(پوچ, MOSQ_LOG_DEBUG, "Sending AUTH to %s (rc%d, %s)", context->id, reason_code, context->auth_method);

	remaining_length = 1;

	rc = mosquitto_property_add_string(&properties, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method);
	اگر(rc){
		برو error;
	}

	اگر(auth_data != پوچ && auth_data_len > 0){
		rc = mosquitto_property_add_دودویی(&properties, MQTT_PROP_AUTHENTICATION_DATA, auth_data, auth_data_len);
		اگر(rc){
			برو error;
		}
	}

	remaining_length += mosquitto_property_get_remaining_length(properties);

	rc = packet__check_oversize(context, remaining_length);
	اگر(rc){
		برو error;
	}

	rc = packet__alloc(&packet, CMD_AUTH, remaining_length);
	اگر(rc){
		برو error;
	}

	packet__write_byte(packet, reason_code);
	property__write_all(packet, properties, true);
	mosquitto_property_free_all(&properties);

	metrics__int_inc(mosq_counter_mqtt_auth_sent, 1);
	بازگشت packet__queue(context, packet);
error:
	mosquitto_property_free_all(&properties);
	بازگشت rc;
}
