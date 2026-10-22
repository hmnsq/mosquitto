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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <assert.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "packet_mosq.h"
#அந்தர்பாவ "property_mosq.h"
#அந்தர்பாவ "sys_tree.h"


முழு send__unsuback(கட்டமைப்பு mosquitto *mosq, uint16_t mid, முழு reason_code_மொத்தம், uint8_t *reason_codes, மாறிலி mosquitto_property *properties)
{
	கட்டமைப்பு mosquitto__packet *packet = வெற்று;
	முழு rc;
	uint32_t remaining_length;

	ப்ரதிஜ்ஞா(mosq);

	remaining_length = 2;
	எனில்(mosq->protocol == mosq_p_mqtt5){
		remaining_length += mosquitto_property_get_remaining_length(properties);
		remaining_length += (uint32_t)reason_code_மொத்தம்;
	}

	rc = packet__alloc(&packet, CMD_UNSUBACK, remaining_length);
	எனில்(rc){
		திரும்பு rc;
	}

	packet__write_uint16(packet, mid);

	எனில்(mosq->protocol == mosq_p_mqtt5){
		property__write_all(packet, properties, true);
		packet__write_bytes(packet, reason_codes, (uint32_t)reason_code_மொத்தம்);
	}

	metrics__int_inc(mosq_counter_mqtt_unsuback_sent, 1);
	திரும்பு packet__queue(mosq, packet);
}
