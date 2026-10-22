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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "property_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sys_tree.h"


ਪੂਰਨ send__unsuback(ਬਣਤਰ mosquitto *mosq, uint16_t mid, ਪੂਰਨ reason_ਕੋਡ_ਗਿਣਤੀ, uint8_t *reason_codes, ਅਚਲ mosquitto_property *properties)
{
	ਬਣਤਰ mosquitto__packet *packet = ਨਿਰਾ;
	ਪੂਰਨ rc;
	uint32_t ਬਚਿਆ_ਲੰਮਾਈ;

	ਪ੍ਰਤਿਜ੍ਞਾ(mosq);

	ਬਚਿਆ_ਲੰਮਾਈ = 2;
	ਜੇ(mosq->protocol == mosq_p_mqtt5){
		ਬਚਿਆ_ਲੰਮਾਈ += mosquitto_property_get_ਬਚਿਆ_ਲੰਮਾਈ(properties);
		ਬਚਿਆ_ਲੰਮਾਈ += (uint32_t)reason_ਕੋਡ_ਗਿਣਤੀ;
	}

	rc = packet__alloc(&packet, CMD_UNSUBACK, ਬਚਿਆ_ਲੰਮਾਈ);
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	packet__write_uint16(packet, mid);

	ਜੇ(mosq->protocol == mosq_p_mqtt5){
		property__write_all(packet, properties, true);
		packet_write_ਬਾਈਟਾਂ(packet, reason_codes, (uint32_t)reason_ਕੋਡ_ਗਿਣਤੀ);
	}

	metrics__int_inc(mosq_counter_mqtt_unsuback_ਭੇਜਿਆ, 1);
	ਵਾਪਸ packet_ਕਤਾਰ(mosq, packet);
}
