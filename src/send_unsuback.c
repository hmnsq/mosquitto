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

#ضمن "config.h"

#ضمن <assert.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "packet_mosq.h"
#ضمن "property_mosq.h"
#ضمن "sys_tree.h"


صحيح send__unsuback(بنية mosquitto *mosq, uint16_t mid, صحيح reason_کد_شمار, uint8_t *reason_codes, ثابت mosquitto_property *properties)
{
	بنية mosquitto__packet *packet = عدم;
	صحيح rc;
	uint32_t مانده_درازی;

	أكد(mosq);

	مانده_درازی = 2;
	إذا(mosq->protocol == mosq_p_mqtt5){
		مانده_درازی += mosquitto_property_get_مانده_درازی(properties);
		مانده_درازی += (uint32_t)reason_کد_شمار;
	}

	rc = packet__alloc(&packet, CMD_UNSUBACK, مانده_درازی);
	إذا(rc){
		أرجع rc;
	}

	packet__write_uint16(packet, mid);

	إذا(mosq->protocol == mosq_p_mqtt5){
		property__write_all(packet, properties, true);
		packet_write_بایتها(packet, reason_codes, (uint32_t)reason_کد_شمار);
	}

	metrics__int_inc(mosq_counter_mqtt_unsuback_فرستاده, 1);
	أرجع packet_صف(mosq, packet);
}
