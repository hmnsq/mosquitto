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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "packet_mosq.h"
#અન્તર્ભાવ "property_mosq.h"
#અન્તર્ભાવ "sys_tree.h"
#અન્તર્ભાવ "util_mosq.h"


પૂર્ણાંક send__suback(સંરચના mosquitto *context, uint16_t mid, uint32_t payloadlen, અચલ શૂન્ય *payload)
{
	સંરચના mosquitto__packet *packet = ખાલી;
	પૂર્ણાંક rc;
	mosquitto_property *properties = ખાલી;
	uint32_t remaining_length;

	log__printf(ખાલી, MOSQ_LOG_DEBUG, "Sending SUBACK to %s", context->id);

	remaining_length = 2+payloadlen;
	જો(context->protocol == mosq_p_mqtt5){
		remaining_length += mosquitto_property_get_remaining_length(properties);
	}
	rc = packet__alloc(&packet, CMD_SUBACK, remaining_length);
	જો(rc){
		પાછા rc;
	}
	packet__write_uint16(packet, mid);

	જો(context->protocol == mosq_p_mqtt5){
		/* We don't use Reason String or User Property yet. */
		property__write_all(packet, properties, true);
	}

	જો(payloadlen){
		packet__write_bytes(packet, payload, payloadlen);
	}

	metrics__int_inc(mosq_counter_mqtt_suback_sent, 1);
	પાછા packet__queue(context, packet);
}
