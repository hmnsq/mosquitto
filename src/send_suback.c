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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "packet_mosq.h"
#অন্তর্ভাব "property_mosq.h"
#অন্তর্ভাব "sys_tree.h"
#অন্তর্ভাব "util_mosq.h"


পূর্ণসংখ্যা send__suback(কাঠামো mosquitto *context, uint16_t mid, uint32_t payloadlen, ধ্রুবক শূন্য *payload)
{
	কাঠামো mosquitto__packet *packet = ফাঁকা;
	পূর্ণসংখ্যা rc;
	mosquitto_property *properties = ফাঁকা;
	uint32_t remaining_দৈর্ঘ্য;

	log__printf(ফাঁকা, MOSQ_LOG_DEBUG, "Sending SUBACK to %s", context->id);

	remaining_দৈর্ঘ্য = 2+payloadlen;
	যদি(context->protocol == mosq_p_mqtt5){
		remaining_দৈর্ঘ্য += mosquitto_property_get_remaining_দৈর্ঘ্য(properties);
	}
	rc = packet__alloc(&packet, CMD_SUBACK, remaining_দৈর্ঘ্য);
	যদি(rc){
		ফেরত rc;
	}
	packet__write_uint16(packet, mid);

	যদি(context->protocol == mosq_p_mqtt5){
		/* We don't use Reason String or User Property yet. */
		property__write_all(packet, properties, true);
	}

	যদি(payloadlen){
		packet__write_bytes(packet, payload, payloadlen);
	}

	metrics__int_inc(mosq_গণক_mqtt_suback_sent, 1);
	ফেরত packet__queue(context, packet);
}
