/*
Copyright (c) 2009-2021 Roger Light <roger@atchoo.org>

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

#অন্তর্ভাৱ "config.h"

#অন্তর্ভাৱ <assert.h>

#অন্তর্ভাৱ "mosquitto_broker_internal.h"
#অন্তর্ভাৱ "mosquitto/mqtt_protocol.h"
#অন্তর্ভাৱ "packet_mosq.h"
#অন্তর্ভাৱ "property_mosq.h"
#অন্তর্ভাৱ "sys_tree.h"


পূর্ণাঙ্ক send__unsuback(সংরচনা mosquitto *mosq, uint16_t mid, পূর্ণাঙ্ক reason_code_গুনতি, uint8_t *reason_codes, ধ্রুৱ mosquitto_property *properties)
{
	সংরচনা mosquitto__packet *packet = রিক্ত;
	পূর্ণাঙ্ক rc;
	uint32_t remaining_length;

	প্রতিজ্ঞা(mosq);

	remaining_length = 2;
	যদি(mosq->protocol == mosq_p_mqtt5){
		remaining_length += mosquitto_property_get_remaining_length(properties);
		remaining_length += (uint32_t)reason_code_গুনতি;
	}

	rc = packet__alloc(&packet, CMD_UNSUBACK, remaining_length);
	যদি(rc){
		ঘূৰাই rc;
	}

	packet__write_uint16(packet, mid);

	যদি(mosq->protocol == mosq_p_mqtt5){
		property__write_all(packet, properties, true);
		packet__write_bytes(packet, reason_codes, (uint32_t)reason_code_গুনতি);
	}

	metrics__int_inc(mosq_counter_mqtt_unsuback_sent, 1);
	ঘূৰাই packet__queue(mosq, packet);
}
