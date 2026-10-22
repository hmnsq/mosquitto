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

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "property_mosq.h"
#अन्तर्भाव "sys_tree.h"
#अन्तर्भाव "util_mosq.h"


पूर्णाङ्क send__suback(संरचना mosquitto *context, uint16_t mid, uint32_t payloadlen, ध्रुव शून्य *payload)
{
	संरचना mosquitto__packet *packet = रिक्त;
	पूर्णाङ्क rc;
	mosquitto_property *properties = रिक्त;
	uint32_t remaining_दीर्घता;

	log__printf(रिक्त, MOSQ_LOG_DEBUG, "Sending SUBACK to %s", context->id);

	remaining_दीर्घता = 2+payloadlen;
	यदि(context->protocol == mosq_p_mqtt5){
		remaining_दीर्घता += mosquitto_property_get_remaining_दीर्घता(properties);
	}
	rc = packet__alloc(&packet, CMD_SUBACK, remaining_दीर्घता);
	यदि(rc){
		प्रत्यागम rc;
	}
	packet__write_uint16(packet, mid);

	यदि(context->protocol == mosq_p_mqtt5){
		/* We don't use Reason String or User Property yet. */
		property__write_all(packet, properties, true);
	}

	यदि(payloadlen){
		packet_write_अष्टकानि(packet, payload, payloadlen);
	}

	metrics__int_inc(mosq_गणक_mqtt_suback_प्रेषितम्, 1);
	प्रत्यागम packet__queue(context, packet);
}
