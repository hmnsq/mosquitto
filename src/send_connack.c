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


పూర్ణాంకం send__connack(నిర్మాణం mosquitto *context, uint8_t ack, uint8_t reason_సంకేతం, స్థిరరాశి mosquitto_property *properties)
{
	నిర్మాణం mosquitto__packet *packet = ఖాళీ;
	పూర్ణాంకం rc;
	mosquitto_property *connack_props = ఖాళీ;
	uint32_t మిగిలిన_నిడివి;

	rc = mosquitto_property_copy_all(&connack_props, properties);
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	ఒకవేళ(context->id){
		log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->id, ack, reason_సంకేతం);
	}లేకపోతే{
		log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->చిరునామా, ack, reason_సంకేతం);
	}

	మిగిలిన_నిడివి = 2;

	ఒకవేళ(context->protocol == mosq_p_mqtt5){
		ఒకవేళ(reason_సంకేతం < 128 && db.config->retain_అందుబాటు == false){
			rc = mosquitto_property_కూడు_byte(&connack_props, MQTT_PROP_RETAIN_అందుబాటు, 0);
			ఒకవేళ(rc){
				mosquitto_property_free_all(&connack_props);
				తిరిగి rc;
			}
		}
		ఒకవేళ(reason_సంకేతం < 128 && db.config->max_packet_కొలత > 0){
			rc = mosquitto_property_కూడు_int32(&connack_props, MQTT_PROP_గరిష్ఠ_PACKET_కొలత, db.config->max_packet_కొలత);
			ఒకవేళ(rc){
				mosquitto_property_free_all(&connack_props);
				తిరిగి rc;
			}
		}
		ఒకవేళ(reason_సంకేతం < 128 && db.config->max_inflight_messages > 0){
			rc = mosquitto_property_కూడు_int16(&connack_props, MQTT_PROP_RECEIVE_గరిష్ఠ, db.config->max_inflight_messages);
			ఒకవేళ(rc){
				mosquitto_property_free_all(&connack_props);
				తిరిగి rc;
			}
		}
		ఒకవేళ(context->listener->max_qos != 2){
			rc = mosquitto_property_కూడు_byte(&connack_props, MQTT_PROP_గరిష్ఠ_QOS, context->listener->max_qos);
			ఒకవేళ(rc){
				mosquitto_property_free_all(&connack_props);
				తిరిగి rc;
			}
		}

		మిగిలిన_నిడివి += mosquitto_property_get_మిగిలిన_నిడివి(connack_props);
	}

	ఒకవేళ(packet__check_oversize(context, మిగిలిన_నిడివి)){
		mosquitto_property_free_all(&connack_props);
		తిరిగి MOSQ_ERR_OVERSIZE_PACKET;
	}

	rc = packet__alloc(&packet, CMD_CONNACK, మిగిలిన_నిడివి);
	ఒకవేళ(rc){
		mosquitto_property_free_all(&connack_props);
		తిరిగి rc;
	}
	packet__write_byte(packet, ack);
	packet__write_byte(packet, reason_సంకేతం);
	ఒకవేళ(context->protocol == mosq_p_mqtt5){
		property__write_all(packet, connack_props, true);
	}
	mosquitto_property_free_all(&connack_props);

	metrics__int_inc(mosq_counter_mqtt_connack_పంపిన, 1);
	తిరిగి packet_వరుస(context, packet);
}

