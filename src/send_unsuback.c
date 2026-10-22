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

#అన్తర్భావ <assert.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "packet_mosq.h"
#అన్తర్భావ "property_mosq.h"
#అన్తర్భావ "sys_tree.h"


పూర్ణాంకం send__unsuback(నిర్మాణం mosquitto *mosq, uint16_t mid, పూర్ణాంకం reason_సంకేతం_లెక్క, uint8_t *reason_codes, స్థిరరాశి mosquitto_property *properties)
{
	నిర్మాణం mosquitto__packet *packet = ఖాళీ;
	పూర్ణాంకం rc;
	uint32_t మిగిలిన_నిడివి;

	ప్రతిజ్ఞా(mosq);

	మిగిలిన_నిడివి = 2;
	ఒకవేళ(mosq->protocol == mosq_p_mqtt5){
		మిగిలిన_నిడివి += mosquitto_property_get_మిగిలిన_నిడివి(properties);
		మిగిలిన_నిడివి += (uint32_t)reason_సంకేతం_లెక్క;
	}

	rc = packet__alloc(&packet, CMD_UNSUBACK, మిగిలిన_నిడివి);
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	packet__write_uint16(packet, mid);

	ఒకవేళ(mosq->protocol == mosq_p_mqtt5){
		property__write_all(packet, properties, true);
		packet_write_బైట్లు(packet, reason_codes, (uint32_t)reason_సంకేతం_లెక్క);
	}

	metrics__int_inc(mosq_counter_mqtt_unsuback_పంపిన, 1);
	తిరిగి packet_వరుస(mosq, packet);
}
