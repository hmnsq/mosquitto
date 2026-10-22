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

#ଅନ୍ତର୍ଭାଵ <assert.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "packet_mosq.h"
#ଅନ୍ତର୍ଭାଵ "property_mosq.h"
#ଅନ୍ତର୍ଭାଵ "sys_tree.h"


ପୂର୍ଣ୍ଣସଂଖ୍ୟା send__unsuback(ସଂରଚନା mosquitto *mosq, uint16_t mid, ପୂର୍ଣ୍ଣସଂଖ୍ୟା reason_code_ଗଣନା, uint8_t *reason_codes, ଅଚଳ mosquitto_property *properties)
{
	ସଂରଚନା mosquitto__packet *packet = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	uint32_t remaining_length;

	ପ୍ରତିଜ୍ଞା(mosq);

	remaining_length = 2;
	ଯଦି(mosq->protocol == mosq_p_mqtt5){
		remaining_length += mosquitto_property_get_remaining_length(properties);
		remaining_length += (uint32_t)reason_code_ଗଣନା;
	}

	rc = packet__alloc(&packet, CMD_UNSUBACK, remaining_length);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	packet__write_uint16(packet, mid);

	ଯଦି(mosq->protocol == mosq_p_mqtt5){
		property__write_all(packet, properties, true);
		packet__write_bytes(packet, reason_codes, (uint32_t)reason_code_ଗଣନା);
	}

	metrics__int_inc(mosq_counter_mqtt_unsuback_sent, 1);
	ଫେରନ୍ତୁ packet__queue(mosq, packet);
}
