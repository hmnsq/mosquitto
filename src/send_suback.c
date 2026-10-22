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


ಪೂರ್ಣಾಂಕ send__suback(ರಚನೆ mosquitto *context, uint16_t mid, uint32_t payloadlen, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *payload)
{
	ರಚನೆ mosquitto__packet *packet = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ rc;
	mosquitto_property *properties = ಖಾಲಿ;
	uint32_t ಉಳಿದ_ಉದ್ದಳತೆ;

	log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "Sending SUBACK to %s", context->id);

	ಉಳಿದ_ಉದ್ದಳತೆ = 2+payloadlen;
	ಆದರೆ(context->protocol == mosq_p_mqtt5){
		ಉಳಿದ_ಉದ್ದಳತೆ += mosquitto_property_get_ಉಳಿದ_ಉದ್ದಳತೆ(properties);
	}
	rc = packet__alloc(&packet, CMD_SUBACK, ಉಳಿದ_ಉದ್ದಳತೆ);
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}
	packet__write_uint16(packet, mid);

	ಆದರೆ(context->protocol == mosq_p_mqtt5){
		/* We don't use Reason String or User Property yet. */
		property__write_all(packet, properties, true);
	}

	ಆದರೆ(payloadlen){
		packet_write_ಬೈಟುಗಳು(packet, payload, payloadlen);
	}

	metrics__int_inc(mosq_counter_mqtt_suback_ಕಳುಹಿಸಿದ, 1);
	ಹಿಂತಿರುಗಿ packet_ಸರತಿ(context, packet);
}
