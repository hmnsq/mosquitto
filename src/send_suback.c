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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "packet_mosq.h"
#ଅନ୍ତର୍ଭାଵ "property_mosq.h"
#ଅନ୍ତର୍ଭାଵ "sys_tree.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"


ପୂର୍ଣ୍ଣସଂଖ୍ୟା send__suback(ସଂରଚନା mosquitto *context, uint16_t mid, uint32_t payloadlen, ଅଚଳ ଶୂନ୍ୟ *payload)
{
	ସଂରଚନା mosquitto__packet *packet = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	mosquitto_property *properties = ଖାଲି;
	uint32_t remaining_length;

	log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "Sending SUBACK to %s", context->id);

	remaining_length = 2+payloadlen;
	ଯଦି(context->protocol == mosq_p_mqtt5){
		remaining_length += mosquitto_property_get_remaining_length(properties);
	}
	rc = packet__alloc(&packet, CMD_SUBACK, remaining_length);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}
	packet__write_uint16(packet, mid);

	ଯଦି(context->protocol == mosq_p_mqtt5){
		/* We don't use Reason String or User Property yet. */
		property__write_all(packet, properties, true);
	}

	ଯଦି(payloadlen){
		packet__write_bytes(packet, payload, payloadlen);
	}

	metrics__int_inc(mosq_counter_mqtt_suback_sent, 1);
	ଫେରନ୍ତୁ packet__queue(context, packet);
}
