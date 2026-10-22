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

#ಅನ್ತರ್ಭಾವ <assert.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "property_mosq.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"


ಪೂರ್ಣಾಙ್ಕ send__unsuback(ಸಂರಚನಾ mosquitto *mosq, uint16_t mid, ಪೂರ್ಣಾಙ್ಕ reason_code_ಎಣಿಕೆ, uint8_t *reason_codes, ಧ್ರುವ mosquitto_property *properties)
{
	ಸಂರಚನಾ mosquitto__packet *packet = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ rc;
	uint32_t remaining_length;

	ಪ್ರತಿಜ್ಞಾ(mosq);

	remaining_length = 2;
	ಯದಿ(mosq->protocol == mosq_p_mqtt5){
		remaining_length += mosquitto_property_get_remaining_length(properties);
		remaining_length += (uint32_t)reason_code_ಎಣಿಕೆ;
	}

	rc = packet__alloc(&packet, CMD_UNSUBACK, remaining_length);
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	packet__write_uint16(packet, mid);

	ಯದಿ(mosq->protocol == mosq_p_mqtt5){
		property__write_all(packet, properties, true);
		packet__write_bytes(packet, reason_codes, (uint32_t)reason_code_ಎಣಿಕೆ);
	}

	metrics__int_inc(mosq_counter_mqtt_unsuback_sent, 1);
	ಪ್ರತ್ಯಾಗಮ packet__queue(mosq, packet);
}
