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

#شامل "config.h"

#شامل <assert.h>

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "packet_mosq.h"
#شامل "property_mosq.h"
#شامل "sys_tree.h"


صحیح send__unsuback(ساختار mosquitto *mosq, uint16_t mid, صحیح reason_code_شمار, uint8_t *reason_codes, ثابت mosquitto_property *properties)
{
	ساختار mosquitto__packet *packet = پوچ;
	صحیح rc;
	uint32_t remaining_length;

	أكد(mosq);

	remaining_length = 2;
	اگر(mosq->protocol == mosq_p_mqtt5){
		remaining_length += mosquitto_property_get_remaining_length(properties);
		remaining_length += (uint32_t)reason_code_شمار;
	}

	rc = packet__alloc(&packet, CMD_UNSUBACK, remaining_length);
	اگر(rc){
		بازگشت rc;
	}

	packet__write_uint16(packet, mid);

	اگر(mosq->protocol == mosq_p_mqtt5){
		property__write_all(packet, properties, true);
		packet__write_bytes(packet, reason_codes, (uint32_t)reason_code_شمار);
	}

	metrics__int_inc(mosq_counter_mqtt_unsuback_sent, 1);
	بازگشت packet__queue(mosq, packet);
}
