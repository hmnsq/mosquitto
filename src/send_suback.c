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

#అన్తర్భావ "config.h"

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "packet_mosq.h"
#అన్తర్భావ "property_mosq.h"
#అన్తర్భావ "sys_tree.h"
#అన్తర్భావ "util_mosq.h"


పూర్ణాంకం send__suback(నిర్మాణం mosquitto *context, uint16_t mid, uint32_t payloadlen, స్థిరరాశి శూన్య *payload)
{
	నిర్మాణం mosquitto__packet *packet = ఖాళీ;
	పూర్ణాంకం rc;
	mosquitto_property *properties = ఖాళీ;
	uint32_t మిగిలిన_నిడివి;

	log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "Sending SUBACK to %s", context->id);

	మిగిలిన_నిడివి = 2+payloadlen;
	ఒకవేళ(context->protocol == mosq_p_mqtt5){
		మిగిలిన_నిడివి += mosquitto_property_get_మిగిలిన_నిడివి(properties);
	}
	rc = packet__alloc(&packet, CMD_SUBACK, మిగిలిన_నిడివి);
	ఒకవేళ(rc){
		తిరిగి rc;
	}
	packet__write_uint16(packet, mid);

	ఒకవేళ(context->protocol == mosq_p_mqtt5){
		/* We don't use Reason String or User Property yet. */
		property__write_all(packet, properties, true);
	}

	ఒకవేళ(payloadlen){
		packet_write_బైట్లు(packet, payload, payloadlen);
	}

	metrics__int_inc(mosq_counter_mqtt_suback_పంపిన, 1);
	తిరిగి packet_వరుస(context, packet);
}
