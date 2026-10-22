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

#અન્તર્ભાવ <assert.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "packet_mosq.h"
#અન્તર્ભાવ "property_mosq.h"
#અન્તર્ભાવ "sys_tree.h"


પૂર્ણાંક send__unsuback(સંરચના mosquitto *mosq, uint16_t mid, પૂર્ણાંક reason_code_ગણતરી, uint8_t *reason_codes, અચલ mosquitto_property *properties)
{
	સંરચના mosquitto__packet *packet = ખાલી;
	પૂર્ણાંક rc;
	uint32_t remaining_length;

	પ્રતિજ્ઞા(mosq);

	remaining_length = 2;
	જો(mosq->protocol == mosq_p_mqtt5){
		remaining_length += mosquitto_property_get_remaining_length(properties);
		remaining_length += (uint32_t)reason_code_ગણતરી;
	}

	rc = packet__alloc(&packet, CMD_UNSUBACK, remaining_length);
	જો(rc){
		પાછા rc;
	}

	packet__write_uint16(packet, mid);

	જો(mosq->protocol == mosq_p_mqtt5){
		property__write_all(packet, properties, true);
		packet__write_bytes(packet, reason_codes, (uint32_t)reason_code_ગણતરી);
	}

	metrics__int_inc(mosq_counter_mqtt_unsuback_sent, 1);
	પાછા packet__queue(mosq, packet);
}
