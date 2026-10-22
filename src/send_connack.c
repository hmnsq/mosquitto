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


ಪೂರ್ಣಾಙ್ಕ send__connack(ಸಂರಚನಾ mosquitto *context, uint8_t ack, uint8_t reason_code, ಧ್ರುವ mosquitto_property *properties)
{
	ಸಂರಚನಾ mosquitto__packet *packet = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ rc;
	mosquitto_property *connack_props = ರಿಕ್ತ;
	uint32_t remaining_length;

	rc = mosquitto_property_copy_all(&connack_props, properties);
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಯದಿ(context->id){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->id, ack, reason_code);
	}ಅನ್ಯಥಾ{
		log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->address, ack, reason_code);
	}

	remaining_length = 2;

	ಯದಿ(context->protocol == mosq_p_mqtt5){
		ಯದಿ(reason_code < 128 && db.config->retain_available == false){
			rc = mosquitto_property_add_byte(&connack_props, MQTT_PROP_RETAIN_AVAILABLE, 0);
			ಯದಿ(rc){
				mosquitto_property_free_all(&connack_props);
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}
		ಯದಿ(reason_code < 128 && db.config->max_packet_size > 0){
			rc = mosquitto_property_add_int32(&connack_props, MQTT_PROP_MAXIMUM_PACKET_SIZE, db.config->max_packet_size);
			ಯದಿ(rc){
				mosquitto_property_free_all(&connack_props);
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}
		ಯದಿ(reason_code < 128 && db.config->max_inflight_messages > 0){
			rc = mosquitto_property_add_int16(&connack_props, MQTT_PROP_RECEIVE_MAXIMUM, db.config->max_inflight_messages);
			ಯದಿ(rc){
				mosquitto_property_free_all(&connack_props);
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}
		ಯದಿ(context->listener->max_qos != 2){
			rc = mosquitto_property_add_byte(&connack_props, MQTT_PROP_MAXIMUM_QOS, context->listener->max_qos);
			ಯದಿ(rc){
				mosquitto_property_free_all(&connack_props);
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}

		remaining_length += mosquitto_property_get_remaining_length(connack_props);
	}

	ಯದಿ(packet__check_oversize(context, remaining_length)){
		mosquitto_property_free_all(&connack_props);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_OVERSIZE_PACKET;
	}

	rc = packet__alloc(&packet, CMD_CONNACK, remaining_length);
	ಯದಿ(rc){
		mosquitto_property_free_all(&connack_props);
		ಪ್ರತ್ಯಾಗಮ rc;
	}
	packet__write_byte(packet, ack);
	packet__write_byte(packet, reason_code);
	ಯದಿ(context->protocol == mosq_p_mqtt5){
		property__write_all(packet, connack_props, true);
	}
	mosquitto_property_free_all(&connack_props);

	metrics__int_inc(mosq_counter_mqtt_connack_sent, 1);
	ಪ್ರತ್ಯಾಗಮ packet__queue(context, packet);
}

