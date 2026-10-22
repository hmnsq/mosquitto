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


صحیح send__unsuback(ساخت mosquitto *mosq, uint16_t mid, صحیح reason_رمز_گنتی, uint8_t *reason_codes, ثابت mosquitto_property *properties)
{
	ساخت mosquitto__packet *packet = تہی;
	صحیح rc;
	uint32_t بقایا_لمبائی;

	أكد(mosq);

	بقایا_لمبائی = 2;
	اگر(mosq->protocol == mosq_p_mqtt5){
		بقایا_لمبائی += mosquitto_property_get_بقایا_لمبائی(properties);
		بقایا_لمبائی += (uint32_t)reason_رمز_گنتی;
	}

	rc = packet__alloc(&packet, CMD_UNSUBACK, بقایا_لمبائی);
	اگر(rc){
		واپس rc;
	}

	packet__write_uint16(packet, mid);

	اگر(mosq->protocol == mosq_p_mqtt5){
		property__write_all(packet, properties, true);
		packet_write_بائٹیں(packet, reason_codes, (uint32_t)reason_رمز_گنتی);
	}

	metrics__int_inc(mosq_شمار_mqtt_unsuback_بھیجا, 1);
	واپس packet_قطار(mosq, packet);
}
