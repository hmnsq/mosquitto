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

#ضمن "config.h"

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "packet_mosq.h"
#ضمن "property_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"


صحيح send__connack(بنية mosquitto *context, uint8_t ack, uint8_t reason_code, ثابت mosquitto_property *properties)
{
	بنية mosquitto__packet *packet = عدم;
	صحيح rc;
	mosquitto_property *connack_props = عدم;
	uint32_t remaining_length;

	rc = mosquitto_property_copy_all(&connack_props, properties);
	ܐܢ(rc){
		ܦܢܐ rc;
	}

	ܐܢ(context->id){
		log__printf(عدم, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->id, ack, reason_code);
	}ܐܚܪܢܐ{
		log__printf(عدم, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->address, ack, reason_code);
	}

	remaining_length = 2;

	ܐܢ(context->protocol == mosq_p_mqtt5){
		ܐܢ(reason_code < 128 && db.config->retain_available == false){
			rc = mosquitto_property_add_byte(&connack_props, MQTT_PROP_RETAIN_AVAILABLE, 0);
			ܐܢ(rc){
				mosquitto_property_free_all(&connack_props);
				ܦܢܐ rc;
			}
		}
		ܐܢ(reason_code < 128 && db.config->max_packet_size > 0){
			rc = mosquitto_property_add_int32(&connack_props, MQTT_PROP_MAXIMUM_PACKET_SIZE, db.config->max_packet_size);
			ܐܢ(rc){
				mosquitto_property_free_all(&connack_props);
				ܦܢܐ rc;
			}
		}
		ܐܢ(reason_code < 128 && db.config->max_inflight_messages > 0){
			rc = mosquitto_property_add_int16(&connack_props, MQTT_PROP_RECEIVE_MAXIMUM, db.config->max_inflight_messages);
			ܐܢ(rc){
				mosquitto_property_free_all(&connack_props);
				ܦܢܐ rc;
			}
		}
		ܐܢ(context->listener->max_qos != 2){
			rc = mosquitto_property_add_byte(&connack_props, MQTT_PROP_MAXIMUM_QOS, context->listener->max_qos);
			ܐܢ(rc){
				mosquitto_property_free_all(&connack_props);
				ܦܢܐ rc;
			}
		}

		remaining_length += mosquitto_property_get_remaining_length(connack_props);
	}

	ܐܢ(packet__check_oversize(context, remaining_length)){
		mosquitto_property_free_all(&connack_props);
		ܦܢܐ MOSQ_ERR_OVERSIZE_PACKET;
	}

	rc = packet__alloc(&packet, CMD_CONNACK, remaining_length);
	ܐܢ(rc){
		mosquitto_property_free_all(&connack_props);
		ܦܢܐ rc;
	}
	packet__write_byte(packet, ack);
	packet__write_byte(packet, reason_code);
	ܐܢ(context->protocol == mosq_p_mqtt5){
		property__write_all(packet, connack_props, true);
	}
	mosquitto_property_free_all(&connack_props);

	metrics__int_inc(mosq_counter_mqtt_connack_sent, 1);
	ܦܢܐ packet__queue(context, packet);
}

