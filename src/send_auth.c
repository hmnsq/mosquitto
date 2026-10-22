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

#ضمن "config.h"

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "packet_mosq.h"
#ضمن "property_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"


صحيح send__auth(بنية mosquitto *context, uint8_t reason_code, ثابت فراغ *auth_data, uint16_t auth_data_len)
{
	بنية mosquitto__packet *packet = عدم;
	صحيح rc;
	mosquitto_property *properties = عدم;
	uint32_t remaining_length;

	אם(context->auth_method == عدم){
		החזר MOSQ_ERR_INVAL;
	}
	אם(context->protocol != mosq_p_mqtt5){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Sending AUTH packet when session not MQTT v5.0.", context->id);
		החזר MOSQ_ERR_PROTOCOL;
	}

	log__printf(عدم, MOSQ_LOG_DEBUG, "Sending AUTH to %s (rc%d, %s)", context->id, reason_code, context->auth_method);

	remaining_length = 1;

	rc = mosquitto_property_add_string(&properties, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method);
	אם(rc){
		اذهب error;
	}

	אם(auth_data != عدم && auth_data_len > 0){
		rc = mosquitto_property_add_binary(&properties, MQTT_PROP_AUTHENTICATION_DATA, auth_data, auth_data_len);
		אם(rc){
			اذهب error;
		}
	}

	remaining_length += mosquitto_property_get_remaining_length(properties);

	rc = packet__check_oversize(context, remaining_length);
	אם(rc){
		اذهب error;
	}

	rc = packet__alloc(&packet, CMD_AUTH, remaining_length);
	אם(rc){
		اذهب error;
	}

	packet__write_byte(packet, reason_code);
	property__write_all(packet, properties, true);
	mosquitto_property_free_all(&properties);

	metrics__int_inc(mosq_counter_mqtt_auth_sent, 1);
	החזר packet__queue(context, packet);
error:
	mosquitto_property_free_all(&properties);
	החזר rc;
}
