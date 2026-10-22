/*
Copyright (c) 2019-2021 Roger Light <roger@atchoo.org>

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


पूर्णाङ्क send__auth(संरचना mosquitto *context, uint8_t reason_संहिता, ध्रुव शून्य *auth_दत्त, uint16_t auth_दत्त_len)
{
	संरचना mosquitto__packet *packet = रिक्त;
	पूर्णाङ्क rc;
	mosquitto_property *properties = रिक्त;
	uint32_t remaining_दीर्घता;

	यदि(context->auth_विधानम् == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(context->protocol != mosq_p_mqtt5){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Sending AUTH packet when session not MQTT v5.0.", context->id);
		प्रत्यागम MOSQ_ERR_PROTOCOL;
	}

	log__printf(रिक्त, MOSQ_LOG_DEBUG, "Sending AUTH to %s (rc%d, %s)", context->id, reason_संहिता, context->auth_विधानम्);

	remaining_दीर्घता = 1;

	rc = mosquitto_property_सङ्कलय_string(&properties, MQTT_PROP_AUTHENTICATION_विधानम्, context->auth_विधानम्);
	यदि(rc){
		गच्छ error;
	}

	यदि(auth_दत्त != रिक्त && auth_दत्त_len > 0){
		rc = mosquitto_property_सङ्कलय_द्विमान(&properties, MQTT_PROP_AUTHENTICATION_दत्त, auth_दत्त, auth_दत्त_len);
		यदि(rc){
			गच्छ error;
		}
	}

	remaining_दीर्घता += mosquitto_property_get_remaining_दीर्घता(properties);

	rc = packet__check_oversize(context, remaining_दीर्घता);
	यदि(rc){
		गच्छ error;
	}

	rc = packet__alloc(&packet, CMD_AUTH, remaining_दीर्घता);
	यदि(rc){
		गच्छ error;
	}

	packet__write_byte(packet, reason_संहिता);
	property__write_all(packet, properties, true);
	mosquitto_property_free_all(&properties);

	metrics__int_inc(mosq_गणक_mqtt_auth_प्रेषितम्, 1);
	प्रत्यागम packet__queue(context, packet);
error:
	mosquitto_property_free_all(&properties);
	प्रत्यागम rc;
}
