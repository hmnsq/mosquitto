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

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "packet_mosq.h"
#ضمن "property_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"


صحيح send__suback(بنية mosquitto *context, uint16_t mid, uint32_t payloadlen, ثابت فراغ *payload)
{
	بنية mosquitto__packet *packet = عدم;
	صحيح rc;
	mosquitto_property *properties = عدم;
	uint32_t مانده_درازی;

	log__printf(عدم, MOSQ_LOG_DEBUG, "Sending SUBACK to %s", context->id);

	مانده_درازی = 2+payloadlen;
	إذا(context->protocol == mosq_p_mqtt5){
		مانده_درازی += mosquitto_property_get_مانده_درازی(properties);
	}
	rc = packet__alloc(&packet, CMD_SUBACK, مانده_درازی);
	إذا(rc){
		أرجع rc;
	}
	packet__write_uint16(packet, mid);

	إذا(context->protocol == mosq_p_mqtt5){
		/* We don't use Reason String or User Property yet. */
		property__write_all(packet, properties, true);
	}

	إذا(payloadlen){
		packet_write_بایتها(packet, payload, payloadlen);
	}

	metrics__int_inc(mosq_counter_mqtt_suback_فرستاده, 1);
	أرجع packet_صف(context, packet);
}
