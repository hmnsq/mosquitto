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

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "packet_mosq.h"
#അന്തര്ഭാവ "property_mosq.h"
#അന്തര്ഭാവ "sys_tree.h"
#അന്തര്ഭാവ "util_mosq.h"


പൂർണ്ണസംഖ്യ send__suback(ഘടന mosquitto *context, uint16_t mid, uint32_t payloadlen, സ്ഥിരം ശൂന്യം *payload)
{
	ഘടന mosquitto__packet *packet = ശൂന്യ;
	പൂർണ്ണസംഖ്യ rc;
	mosquitto_property *properties = ശൂന്യ;
	uint32_t remaining_length;

	log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "Sending SUBACK to %s", context->id);

	remaining_length = 2+payloadlen;
	എങ്കിൽ(context->protocol == mosq_p_mqtt5){
		remaining_length += mosquitto_property_get_remaining_length(properties);
	}
	rc = packet__alloc(&packet, CMD_SUBACK, remaining_length);
	എങ്കിൽ(rc){
		തിരികെ rc;
	}
	packet__write_uint16(packet, mid);

	എങ്കിൽ(context->protocol == mosq_p_mqtt5){
		/* We don't use Reason String or User Property yet. */
		property__write_all(packet, properties, true);
	}

	എങ്കിൽ(payloadlen){
		packet__write_bytes(packet, payload, payloadlen);
	}

	metrics__int_inc(mosq_counter_mqtt_suback_sent, 1);
	തിരികെ packet__queue(context, packet);
}
