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

#समावेश "config.h"

#समावेश <assert.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "packet_mosq.h"
#समावेश "property_mosq.h"
#समावेश "sys_tree.h"


पूर्णांक send__unsuback(काष्ठा mosquitto *mosq, uint16_t mid, पूर्णांक reason_code_गिनती, uint8_t *reason_codes, स्थिर mosquitto_property *properties)
{
	काष्ठा mosquitto__packet *packet = शून्य;
	पूर्णांक rc;
	uint32_t remaining_लंबाई;

	निश्चित(mosq);

	remaining_लंबाई = 2;
	अगर(mosq->protocol == mosq_p_mqtt5){
		remaining_लंबाई += mosquitto_property_get_remaining_लंबाई(properties);
		remaining_लंबाई += (uint32_t)reason_code_गिनती;
	}

	rc = packet__alloc(&packet, CMD_UNSUBACK, remaining_लंबाई);
	अगर(rc){
		वापस rc;
	}

	packet__write_uint16(packet, mid);

	अगर(mosq->protocol == mosq_p_mqtt5){
		property__write_all(packet, properties, true);
		packet__write_bytes(packet, reason_codes, (uint32_t)reason_code_गिनती);
	}

	metrics__int_inc(mosq_गणक_mqtt_unsuback_sent, 1);
	वापस packet__queue(mosq, packet);
}
