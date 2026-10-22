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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "property_mosq.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"


ಪೂರ್ಣಾಙ್ಕ send__suback(ಸಂರಚನಾ mosquitto *context, uint16_t mid, uint32_t payloadlen, ಧ್ರುವ ಶೂನ್ಯ *payload)
{
	ಸಂರಚನಾ mosquitto__packet *packet = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ rc;
	mosquitto_property *properties = ರಿಕ್ತ;
	uint32_t remaining_length;

	log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "Sending SUBACK to %s", context->id);

	remaining_length = 2+payloadlen;
	ಯದಿ(context->protocol == mosq_p_mqtt5){
		remaining_length += mosquitto_property_get_remaining_length(properties);
	}
	rc = packet__alloc(&packet, CMD_SUBACK, remaining_length);
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}
	packet__write_uint16(packet, mid);

	ಯದಿ(context->protocol == mosq_p_mqtt5){
		/* We don't use Reason String or User Property yet. */
		property__write_all(packet, properties, true);
	}

	ಯದಿ(payloadlen){
		packet__write_bytes(packet, payload, payloadlen);
	}

	metrics__int_inc(mosq_counter_mqtt_suback_sent, 1);
	ಪ್ರತ್ಯಾಗಮ packet__queue(context, packet);
}
