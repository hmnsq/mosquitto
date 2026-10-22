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

#অন্তর্ভাৱ "mosquitto_broker_internal.h"
#অন্তর্ভাৱ "mosquitto/mqtt_protocol.h"
#অন্তর্ভাৱ "packet_mosq.h"
#অন্তর্ভাৱ "property_mosq.h"
#অন্তর্ভাৱ "sys_tree.h"
#অন্তর্ভাৱ "util_mosq.h"


পূর্ণাঙ্ক send__suback(সংরচনা mosquitto *context, uint16_t mid, uint32_t payloadlen, ধ্রুৱ শূন্য *payload)
{
	সংরচনা mosquitto__packet *packet = রিক্ত;
	পূর্ণাঙ্ক rc;
	mosquitto_property *properties = রিক্ত;
	uint32_t remaining_length;

	log__printf(রিক্ত, MOSQ_LOG_DEBUG, "Sending SUBACK to %s", context->id);

	remaining_length = 2+payloadlen;
	যদি(context->protocol == mosq_p_mqtt5){
		remaining_length += mosquitto_property_get_remaining_length(properties);
	}
	rc = packet__alloc(&packet, CMD_SUBACK, remaining_length);
	যদি(rc){
		ঘূৰাই rc;
	}
	packet__write_uint16(packet, mid);

	যদি(context->protocol == mosq_p_mqtt5){
		/* We don't use Reason String or User Property yet. */
		property__write_all(packet, properties, true);
	}

	যদি(payloadlen){
		packet__write_bytes(packet, payload, payloadlen);
	}

	metrics__int_inc(mosq_counter_mqtt_suback_sent, 1);
	ঘূৰাই packet__queue(context, packet);
}
