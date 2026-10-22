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

#شامل "config.h"

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "packet_mosq.h"
#شامل "property_mosq.h"
#شامل "sys_tree.h"
#شامل "util_mosq.h"


صحیح send__connack(ساختار mosquitto *context, uint8_t ack, uint8_t reason_code, ثابت mosquitto_property *properties)
{
	ساختار mosquitto__packet *packet = پوچ;
	صحیح rc;
	mosquitto_property *connack_props = پوچ;
	uint32_t remaining_length;

	rc = mosquitto_property_copy_all(&connack_props, properties);
	اگر(rc){
		بازگشت rc;
	}

	اگر(context->id){
		log__printf(پوچ, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->id, ack, reason_code);
	}وگرنه{
		log__printf(پوچ, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->address, ack, reason_code);
	}

	remaining_length = 2;

	اگر(context->protocol == mosq_p_mqtt5){
		اگر(reason_code < 128 && db.config->retain_available == false){
			rc = mosquitto_property_add_byte(&connack_props, MQTT_PROP_RETAIN_AVAILABLE, 0);
			اگر(rc){
				mosquitto_property_free_all(&connack_props);
				بازگشت rc;
			}
		}
		اگر(reason_code < 128 && db.config->max_packet_size > 0){
			rc = mosquitto_property_add_int32(&connack_props, MQTT_PROP_MAXIMUM_PACKET_SIZE, db.config->max_packet_size);
			اگر(rc){
				mosquitto_property_free_all(&connack_props);
				بازگشت rc;
			}
		}
		اگر(reason_code < 128 && db.config->max_inflight_messages > 0){
			rc = mosquitto_property_add_int16(&connack_props, MQTT_PROP_RECEIVE_MAXIMUM, db.config->max_inflight_messages);
			اگر(rc){
				mosquitto_property_free_all(&connack_props);
				بازگشت rc;
			}
		}
		اگر(context->listener->max_qos != 2){
			rc = mosquitto_property_add_byte(&connack_props, MQTT_PROP_MAXIMUM_QOS, context->listener->max_qos);
			اگر(rc){
				mosquitto_property_free_all(&connack_props);
				بازگشت rc;
			}
		}

		remaining_length += mosquitto_property_get_remaining_length(connack_props);
	}

	اگر(packet__check_oversize(context, remaining_length)){
		mosquitto_property_free_all(&connack_props);
		بازگشت MOSQ_ERR_OVERSIZE_PACKET;
	}

	rc = packet__alloc(&packet, CMD_CONNACK, remaining_length);
	اگر(rc){
		mosquitto_property_free_all(&connack_props);
		بازگشت rc;
	}
	packet__write_byte(packet, ack);
	packet__write_byte(packet, reason_code);
	اگر(context->protocol == mosq_p_mqtt5){
		property__write_all(packet, connack_props, true);
	}
	mosquitto_property_free_all(&connack_props);

	metrics__int_inc(mosq_counter_mqtt_connack_sent, 1);
	بازگشت packet__queue(context, packet);
}

