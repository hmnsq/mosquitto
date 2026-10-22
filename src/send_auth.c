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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "packet_mosq.h"
#অন্তর্ভাব "property_mosq.h"
#অন্তর্ভাব "sys_tree.h"
#অন্তর্ভাব "util_mosq.h"


পূর্ণাঙ্ক send__auth(সংরচনা mosquitto *context, uint8_t reason_code, ধ্রুব শূন্য *auth_data, uint16_t auth_data_len)
{
	সংরচনা mosquitto__packet *packet = রিক্ত;
	পূর্ণাঙ্ক rc;
	mosquitto_property *properties = রিক্ত;
	uint32_t remaining_length;

	যদি(context->auth_method == রিক্ত){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}
	যদি(context->protocol != mosq_p_mqtt5){
		log__printf(রিক্ত, MOSQ_LOG_INFO, "Protocol error from %s: Sending AUTH packet when session not MQTT v5.0.", context->id);
		প্রত্যাগম MOSQ_ERR_PROTOCOL;
	}

	log__printf(রিক্ত, MOSQ_LOG_DEBUG, "Sending AUTH to %s (rc%d, %s)", context->id, reason_code, context->auth_method);

	remaining_length = 1;

	rc = mosquitto_property_add_string(&properties, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method);
	যদি(rc){
		গচ্ছ error;
	}

	যদি(auth_data != রিক্ত && auth_data_len > 0){
		rc = mosquitto_property_add_দ্বিমিক(&properties, MQTT_PROP_AUTHENTICATION_DATA, auth_data, auth_data_len);
		যদি(rc){
			গচ্ছ error;
		}
	}

	remaining_length += mosquitto_property_get_remaining_length(properties);

	rc = packet__check_oversize(context, remaining_length);
	যদি(rc){
		গচ্ছ error;
	}

	rc = packet__alloc(&packet, CMD_AUTH, remaining_length);
	যদি(rc){
		গচ্ছ error;
	}

	packet__write_byte(packet, reason_code);
	property__write_all(packet, properties, true);
	mosquitto_property_free_all(&properties);

	metrics__int_inc(mosq_counter_mqtt_auth_sent, 1);
	প্রত্যাগম packet__queue(context, packet);
error:
	mosquitto_property_free_all(&properties);
	প্রত্যাগম rc;
}
