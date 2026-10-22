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


पूर्णाङ्क send__connack(संरचना mosquitto *context, uint8_t ack, uint8_t reason_संहिता, ध्रुव mosquitto_property *properties)
{
	संरचना mosquitto__packet *packet = रिक्त;
	पूर्णाङ्क rc;
	mosquitto_property *connack_props = रिक्त;
	uint32_t remaining_दीर्घता;

	rc = mosquitto_property_copy_all(&connack_props, properties);
	यदि(rc){
		प्रत्यागम rc;
	}

	यदि(context->id){
		log__printf(रिक्त, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->id, ack, reason_संहिता);
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->स्थानाङ्कः, ack, reason_संहिता);
	}

	remaining_दीर्घता = 2;

	यदि(context->protocol == mosq_p_mqtt5){
		यदि(reason_संहिता < 128 && db.config->retain_उपलब्धम् == false){
			rc = mosquitto_property_सङ्कलय_byte(&connack_props, MQTT_PROP_RETAIN_उपलब्धम्, 0);
			यदि(rc){
				mosquitto_property_free_all(&connack_props);
				प्रत्यागम rc;
			}
		}
		यदि(reason_संहिता < 128 && db.config->max_packet_आकार > 0){
			rc = mosquitto_property_सङ्कलय_int32(&connack_props, MQTT_PROP_अधिकतमम्_PACKET_आकार, db.config->max_packet_आकार);
			यदि(rc){
				mosquitto_property_free_all(&connack_props);
				प्रत्यागम rc;
			}
		}
		यदि(reason_संहिता < 128 && db.config->max_inflight_messages > 0){
			rc = mosquitto_property_सङ्कलय_int16(&connack_props, MQTT_PROP_RECEIVE_अधिकतमम्, db.config->max_inflight_messages);
			यदि(rc){
				mosquitto_property_free_all(&connack_props);
				प्रत्यागम rc;
			}
		}
		यदि(context->listener->max_qos != 2){
			rc = mosquitto_property_सङ्कलय_byte(&connack_props, MQTT_PROP_अधिकतमम्_QOS, context->listener->max_qos);
			यदि(rc){
				mosquitto_property_free_all(&connack_props);
				प्रत्यागम rc;
			}
		}

		remaining_दीर्घता += mosquitto_property_get_remaining_दीर्घता(connack_props);
	}

	यदि(packet__check_oversize(context, remaining_दीर्घता)){
		mosquitto_property_free_all(&connack_props);
		प्रत्यागम MOSQ_ERR_OVERSIZE_PACKET;
	}

	rc = packet__alloc(&packet, CMD_CONNACK, remaining_दीर्घता);
	यदि(rc){
		mosquitto_property_free_all(&connack_props);
		प्रत्यागम rc;
	}
	packet__write_byte(packet, ack);
	packet__write_byte(packet, reason_संहिता);
	यदि(context->protocol == mosq_p_mqtt5){
		property__write_all(packet, connack_props, true);
	}
	mosquitto_property_free_all(&connack_props);

	metrics__int_inc(mosq_गणक_mqtt_connack_प्रेषितम्, 1);
	प्रत्यागम packet__queue(context, packet);
}

