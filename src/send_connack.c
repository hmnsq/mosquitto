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

#समावेश "config.h"

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "packet_mosq.h"
#समावेश "property_mosq.h"
#समावेश "sys_tree.h"
#समावेश "util_mosq.h"


पूर्णांक send__connack(काष्ठा mosquitto *context, uint8_t ack, uint8_t reason_code, स्थिर mosquitto_property *properties)
{
	काष्ठा mosquitto__packet *packet = शून्य;
	पूर्णांक rc;
	mosquitto_property *connack_props = शून्य;
	uint32_t remaining_लंबाई;

	rc = mosquitto_property_copy_all(&connack_props, properties);
	अगर(rc){
		वापस rc;
	}

	अगर(context->id){
		log__printf(शून्य, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->id, ack, reason_code);
	}अन्यथा{
		log__printf(शून्य, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->address, ack, reason_code);
	}

	remaining_लंबाई = 2;

	अगर(context->protocol == mosq_p_mqtt5){
		अगर(reason_code < 128 && db.config->retain_available == false){
			rc = mosquitto_property_add_byte(&connack_props, MQTT_PROP_RETAIN_AVAILABLE, 0);
			अगर(rc){
				mosquitto_property_free_all(&connack_props);
				वापस rc;
			}
		}
		अगर(reason_code < 128 && db.config->max_packet_आकार > 0){
			rc = mosquitto_property_add_int32(&connack_props, MQTT_PROP_MAXIMUM_PACKET_आकार, db.config->max_packet_आकार);
			अगर(rc){
				mosquitto_property_free_all(&connack_props);
				वापस rc;
			}
		}
		अगर(reason_code < 128 && db.config->max_inflight_messages > 0){
			rc = mosquitto_property_add_int16(&connack_props, MQTT_PROP_RECEIVE_MAXIMUM, db.config->max_inflight_messages);
			अगर(rc){
				mosquitto_property_free_all(&connack_props);
				वापस rc;
			}
		}
		अगर(context->listener->max_qos != 2){
			rc = mosquitto_property_add_byte(&connack_props, MQTT_PROP_MAXIMUM_QOS, context->listener->max_qos);
			अगर(rc){
				mosquitto_property_free_all(&connack_props);
				वापस rc;
			}
		}

		remaining_लंबाई += mosquitto_property_get_remaining_लंबाई(connack_props);
	}

	अगर(packet__check_oversize(context, remaining_लंबाई)){
		mosquitto_property_free_all(&connack_props);
		वापस MOSQ_ERR_OVERSIZE_PACKET;
	}

	rc = packet__alloc(&packet, CMD_CONNACK, remaining_लंबाई);
	अगर(rc){
		mosquitto_property_free_all(&connack_props);
		वापस rc;
	}
	packet__write_byte(packet, ack);
	packet__write_byte(packet, reason_code);
	अगर(context->protocol == mosq_p_mqtt5){
		property__write_all(packet, connack_props, true);
	}
	mosquitto_property_free_all(&connack_props);

	metrics__int_inc(mosq_गणक_mqtt_connack_sent, 1);
	वापस packet__queue(context, packet);
}

