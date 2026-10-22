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

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "property_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sys_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"


ਪੂਰਨ send__suback(ਬਣਤਰ mosquitto *context, uint16_t mid, uint32_t payloadlen, ਅਚਲ ਖਾਲੀ *payload)
{
	ਬਣਤਰ mosquitto__packet *packet = ਨਿਰਾ;
	ਪੂਰਨ rc;
	mosquitto_property *properties = ਨਿਰਾ;
	uint32_t ਬਚਿਆ_ਲੰਮਾਈ;

	log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Sending SUBACK to %s", context->id);

	ਬਚਿਆ_ਲੰਮਾਈ = 2+payloadlen;
	ਜੇ(context->protocol == mosq_p_mqtt5){
		ਬਚਿਆ_ਲੰਮਾਈ += mosquitto_property_get_ਬਚਿਆ_ਲੰਮਾਈ(properties);
	}
	rc = packet__alloc(&packet, CMD_SUBACK, ਬਚਿਆ_ਲੰਮਾਈ);
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}
	packet__write_uint16(packet, mid);

	ਜੇ(context->protocol == mosq_p_mqtt5){
		/* We don't use Reason String or User Property yet. */
		property__write_all(packet, properties, true);
	}

	ਜੇ(payloadlen){
		packet_write_ਬਾਈਟਾਂ(packet, payload, payloadlen);
	}

	metrics__int_inc(mosq_counter_mqtt_suback_ਭੇਜਿਆ, 1);
	ਵਾਪਸ packet_ਕਤਾਰ(context, packet);
}
