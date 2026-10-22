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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <assert.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "packet_mosq.h"
#അന്തര്ഭാവ "property_mosq.h"
#അന്തര്ഭാവ "sys_tree.h"


പൂർണ്ണസംഖ്യ send__unsuback(ഘടന mosquitto *mosq, uint16_t mid, പൂർണ്ണസംഖ്യ reason_code_എണ്ണം, uint8_t *reason_codes, സ്ഥിരം mosquitto_property *properties)
{
	ഘടന mosquitto__packet *packet = ശൂന്യ;
	പൂർണ്ണസംഖ്യ rc;
	uint32_t remaining_length;

	പ്രതിജ്ഞാ(mosq);

	remaining_length = 2;
	എങ്കിൽ(mosq->protocol == mosq_p_mqtt5){
		remaining_length += mosquitto_property_get_remaining_length(properties);
		remaining_length += (uint32_t)reason_code_എണ്ണം;
	}

	rc = packet__alloc(&packet, CMD_UNSUBACK, remaining_length);
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	packet__write_uint16(packet, mid);

	എങ്കിൽ(mosq->protocol == mosq_p_mqtt5){
		property__write_all(packet, properties, true);
		packet__write_bytes(packet, reason_codes, (uint32_t)reason_code_എണ്ണം);
	}

	metrics__int_inc(mosq_counter_mqtt_unsuback_sent, 1);
	തിരികെ packet__queue(mosq, packet);
}
