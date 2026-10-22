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


ಪೂರ್ಣಾಂಕ send__unsuback(ರಚನೆ mosquitto *mosq, uint16_t mid, ಪೂರ್ಣಾಂಕ reason_ಸಂಕೇತ_ಎಣಿಕೆ, uint8_t *reason_codes, ಸ್ಥಿರಾಂಕ mosquitto_property *properties)
{
	ರಚನೆ mosquitto__packet *packet = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ rc;
	uint32_t ಉಳಿದ_ಉದ್ದಳತೆ;

	ಪ್ರತಿಜ್ಞಾ(mosq);

	ಉಳಿದ_ಉದ್ದಳತೆ = 2;
	ಆದರೆ(mosq->protocol == mosq_p_mqtt5){
		ಉಳಿದ_ಉದ್ದಳತೆ += mosquitto_property_get_ಉಳಿದ_ಉದ್ದಳತೆ(properties);
		ಉಳಿದ_ಉದ್ದಳತೆ += (uint32_t)reason_ಸಂಕೇತ_ಎಣಿಕೆ;
	}

	rc = packet__alloc(&packet, CMD_UNSUBACK, ಉಳಿದ_ಉದ್ದಳತೆ);
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}

	packet__write_uint16(packet, mid);

	ಆದರೆ(mosq->protocol == mosq_p_mqtt5){
		property__write_all(packet, properties, true);
		packet_write_ಬೈಟುಗಳು(packet, reason_codes, (uint32_t)reason_ಸಂಕೇತ_ಎಣಿಕೆ);
	}

	metrics__int_inc(mosq_counter_mqtt_unsuback_ಕಳುಹಿಸಿದ, 1);
	ಹಿಂತಿರುಗಿ packet_ಸರತಿ(mosq, packet);
}
