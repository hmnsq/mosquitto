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

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "packet_mosq.h"
#समावेश "property_mosq.h"
#समावेश "sys_tree.h"
#समावेश "util_mosq.h"


पूर्णांक send__suback(काष्ठा mosquitto *context, uint16_t mid, uint32_t payloadlen, स्थिर व्योम *payload)
{
	काष्ठा mosquitto__packet *packet = शून्य;
	पूर्णांक rc;
	mosquitto_property *properties = शून्य;
	uint32_t remaining_लंबाई;

	log__printf(शून्य, MOSQ_LOG_DEBUG, "Sending SUBACK to %s", context->id);

	remaining_लंबाई = 2+payloadlen;
	अगर(context->protocol == mosq_p_mqtt5){
		remaining_लंबाई += mosquitto_property_get_remaining_लंबाई(properties);
	}
	rc = packet__alloc(&packet, CMD_SUBACK, remaining_लंबाई);
	अगर(rc){
		वापस rc;
	}
	packet__write_uint16(packet, mid);

	अगर(context->protocol == mosq_p_mqtt5){
		/* We don't use Reason String or User Property yet. */
		property__write_all(packet, properties, true);
	}

	अगर(payloadlen){
		packet__write_bytes(packet, payload, payloadlen);
	}

	metrics__int_inc(mosq_गणक_mqtt_suback_sent, 1);
	वापस packet__queue(context, packet);
}
