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


صحيح send__unsuback(بنية mosquitto *mosq, uint16_t mid, صحيح reason_کوډ_شمېر, uint8_t *reason_codes, ثابت mosquitto_property *properties)
{
	بنية mosquitto__packet *packet = عدم;
	صحيح rc;
	uint32_t پاتې_شوی_اوږدتیا;

	أكد(mosq);

	پاتې_شوی_اوږدتیا = 2;
	إذا(mosq->protocol == mosq_p_mqtt5){
		پاتې_شوی_اوږدتیا += mosquitto_property_get_پاتې_شوی_اوږدتیا(properties);
		پاتې_شوی_اوږدتیا += (uint32_t)reason_کوډ_شمېر;
	}

	rc = packet__alloc(&packet, CMD_UNSUBACK, پاتې_شوی_اوږدتیا);
	إذا(rc){
		أرجع rc;
	}

	packet__write_uint16(packet, mid);

	إذا(mosq->protocol == mosq_p_mqtt5){
		property__write_all(packet, properties, true);
		packet_write_بایټونه(packet, reason_codes, (uint32_t)reason_کوډ_شمېر);
	}

	metrics__int_inc(mosq_counter_mqtt_unsuback_لېږل_شوی, 1);
	أرجع packet_کتار(mosq, packet);
}
