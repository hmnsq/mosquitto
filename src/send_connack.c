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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "packet_mosq.h"
#অন্তর্ভাব "property_mosq.h"
#অন্তর্ভাব "sys_tree.h"
#অন্তর্ভাব "util_mosq.h"


পূর্ণাঙ্ক send__connack(সংরচনা mosquitto *context, uint8_t ack, uint8_t reason_code, ধ্রুব mosquitto_property *properties)
{
	সংরচনা mosquitto__packet *packet = রিক্ত;
	পূর্ণাঙ্ক rc;
	mosquitto_property *connack_props = রিক্ত;
	uint32_t remaining_length;

	rc = mosquitto_property_copy_all(&connack_props, properties);
	যদি(rc){
		প্রত্যাগম rc;
	}

	যদি(context->id){
		log__printf(রিক্ত, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->id, ack, reason_code);
	}অন্যথা{
		log__printf(রিক্ত, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->address, ack, reason_code);
	}

	remaining_length = 2;

	যদি(context->protocol == mosq_p_mqtt5){
		যদি(reason_code < 128 && db.config->retain_available == false){
			rc = mosquitto_property_add_byte(&connack_props, MQTT_PROP_RETAIN_AVAILABLE, 0);
			যদি(rc){
				mosquitto_property_free_all(&connack_props);
				প্রত্যাগম rc;
			}
		}
		যদি(reason_code < 128 && db.config->max_packet_size > 0){
			rc = mosquitto_property_add_int32(&connack_props, MQTT_PROP_MAXIMUM_PACKET_SIZE, db.config->max_packet_size);
			যদি(rc){
				mosquitto_property_free_all(&connack_props);
				প্রত্যাগম rc;
			}
		}
		যদি(reason_code < 128 && db.config->max_inflight_messages > 0){
			rc = mosquitto_property_add_int16(&connack_props, MQTT_PROP_RECEIVE_MAXIMUM, db.config->max_inflight_messages);
			যদি(rc){
				mosquitto_property_free_all(&connack_props);
				প্রত্যাগম rc;
			}
		}
		যদি(context->listener->max_qos != 2){
			rc = mosquitto_property_add_byte(&connack_props, MQTT_PROP_MAXIMUM_QOS, context->listener->max_qos);
			যদি(rc){
				mosquitto_property_free_all(&connack_props);
				প্রত্যাগম rc;
			}
		}

		remaining_length += mosquitto_property_get_remaining_length(connack_props);
	}

	যদি(packet__check_oversize(context, remaining_length)){
		mosquitto_property_free_all(&connack_props);
		প্রত্যাগম MOSQ_ERR_OVERSIZE_PACKET;
	}

	rc = packet__alloc(&packet, CMD_CONNACK, remaining_length);
	যদি(rc){
		mosquitto_property_free_all(&connack_props);
		প্রত্যাগম rc;
	}
	packet__write_byte(packet, ack);
	packet__write_byte(packet, reason_code);
	যদি(context->protocol == mosq_p_mqtt5){
		property__write_all(packet, connack_props, true);
	}
	mosquitto_property_free_all(&connack_props);

	metrics__int_inc(mosq_counter_mqtt_connack_sent, 1);
	প্রত্যাগম packet__queue(context, packet);
}

