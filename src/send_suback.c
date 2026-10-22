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

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "packet_mosq.h"
#شامل "property_mosq.h"
#شامل "sys_tree.h"
#شامل "util_mosq.h"


صحیح send__suback(ساخت mosquitto *context, uint16_t mid, uint32_t payloadlen, ثابت خالی *payload)
{
	ساخت mosquitto__packet *packet = تہی;
	صحیح rc;
	mosquitto_property *properties = تہی;
	uint32_t بقایا_لمبائی;

	log__printf(تہی, MOSQ_LOG_DEBUG, "Sending SUBACK to %s", context->id);

	بقایا_لمبائی = 2+payloadlen;
	اگر(context->protocol == mosq_p_mqtt5){
		بقایا_لمبائی += mosquitto_property_get_بقایا_لمبائی(properties);
	}
	rc = packet__alloc(&packet, CMD_SUBACK, بقایا_لمبائی);
	اگر(rc){
		واپس rc;
	}
	packet__write_uint16(packet, mid);

	اگر(context->protocol == mosq_p_mqtt5){
		/* We don't use Reason String or User Property yet. */
		property__write_all(packet, properties, true);
	}

	اگر(payloadlen){
		packet_write_بائٹیں(packet, payload, payloadlen);
	}

	metrics__int_inc(mosq_شمار_mqtt_suback_بھیجا, 1);
	واپس packet_قطار(context, packet);
}
