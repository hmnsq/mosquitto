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


পূর্ণসংখ্যা send__auth(কাঠামো mosquitto *context, uint8_t reason_code, ধ্রুবক শূন্য *auth_তথ্য, uint16_t auth_তথ্য_len)
{
	কাঠামো mosquitto__packet *packet = ফাঁকা;
	পূর্ণসংখ্যা rc;
	mosquitto_property *properties = ফাঁকা;
	uint32_t remaining_দৈর্ঘ্য;

	যদি(context->auth_method == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}
	যদি(context->protocol != mosq_p_mqtt5){
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s: Sending AUTH packet when session not MQTT v5.0.", context->id);
		ফেরত MOSQ_ERR_PROTOCOL;
	}

	log__printf(ফাঁকা, MOSQ_LOG_DEBUG, "Sending AUTH to %s (rc%d, %s)", context->id, reason_code, context->auth_method);

	remaining_দৈর্ঘ্য = 1;

	rc = mosquitto_property_add_string(&properties, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method);
	যদি(rc){
		যাও error;
	}

	যদি(auth_তথ্য != ফাঁকা && auth_তথ্য_len > 0){
		rc = mosquitto_property_add_দ্বিমিক(&properties, MQTT_PROP_AUTHENTICATION_তথ্য, auth_তথ্য, auth_তথ্য_len);
		যদি(rc){
			যাও error;
		}
	}

	remaining_দৈর্ঘ্য += mosquitto_property_get_remaining_দৈর্ঘ্য(properties);

	rc = packet__check_oversize(context, remaining_দৈর্ঘ্য);
	যদি(rc){
		যাও error;
	}

	rc = packet__alloc(&packet, CMD_AUTH, remaining_দৈর্ঘ্য);
	যদি(rc){
		যাও error;
	}

	packet__write_byte(packet, reason_code);
	property__write_all(packet, properties, true);
	mosquitto_property_free_all(&properties);

	metrics__int_inc(mosq_গণক_mqtt_auth_sent, 1);
	ফেরত packet__queue(context, packet);
error:
	mosquitto_property_free_all(&properties);
	ফেরত rc;
}
