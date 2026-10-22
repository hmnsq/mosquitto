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

#अन्तर्भाव "config.h"

#अन्तर्भाव <assert.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "property_mosq.h"
#अन्तर्भाव "sys_tree.h"


पूर्णाङ्क send__unsuback(संरचना mosquitto *mosq, uint16_t mid, पूर्णाङ्क reason_code_गणनम्, uint8_t *reason_codes, ध्रुव mosquitto_property *properties)
{
	संरचना mosquitto__packet *packet = रिक्त;
	पूर्णाङ्क rc;
	uint32_t remaining_length;

	प्रतिज्ञा(mosq);

	remaining_length = 2;
	सचे(mosq->protocol == mosq_p_mqtt5){
		remaining_length += mosquitto_property_get_remaining_length(properties);
		remaining_length += (uint32_t)reason_code_गणनम्;
	}

	rc = packet__alloc(&packet, CMD_UNSUBACK, remaining_length);
	सचे(rc){
		पटिनिवत्त rc;
	}

	packet__write_uint16(packet, mid);

	सचे(mosq->protocol == mosq_p_mqtt5){
		property__write_all(packet, properties, true);
		packet__write_bytes(packet, reason_codes, (uint32_t)reason_code_गणनम्);
	}

	metrics__int_inc(mosq_counter_mqtt_unsuback_sent, 1);
	पटिनिवत्त packet__queue(mosq, packet);
}
