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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "packet_mosq.h"
#அந்தர்பாவ "property_mosq.h"
#அந்தர்பாவ "sys_tree.h"
#அந்தர்பாவ "util_mosq.h"


முழு send__connack(கட்டமைப்பு mosquitto *context, uint8_t ack, uint8_t reason_code, மாறிலி mosquitto_property *properties)
{
	கட்டமைப்பு mosquitto__packet *packet = வெற்று;
	முழு rc;
	mosquitto_property *connack_props = வெற்று;
	uint32_t remaining_length;

	rc = mosquitto_property_copy_all(&connack_props, properties);
	எனில்(rc){
		திரும்பு rc;
	}

	எனில்(context->id){
		log__printf(வெற்று, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->id, ack, reason_code);
	}இல்லையெனில்{
		log__printf(வெற்று, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->address, ack, reason_code);
	}

	remaining_length = 2;

	எனில்(context->protocol == mosq_p_mqtt5){
		எனில்(reason_code < 128 && db.config->retain_available == false){
			rc = mosquitto_property_add_byte(&connack_props, MQTT_PROP_RETAIN_AVAILABLE, 0);
			எனில்(rc){
				mosquitto_property_free_all(&connack_props);
				திரும்பு rc;
			}
		}
		எனில்(reason_code < 128 && db.config->max_packet_size > 0){
			rc = mosquitto_property_add_int32(&connack_props, MQTT_PROP_MAXIMUM_PACKET_SIZE, db.config->max_packet_size);
			எனில்(rc){
				mosquitto_property_free_all(&connack_props);
				திரும்பு rc;
			}
		}
		எனில்(reason_code < 128 && db.config->max_inflight_messages > 0){
			rc = mosquitto_property_add_int16(&connack_props, MQTT_PROP_RECEIVE_MAXIMUM, db.config->max_inflight_messages);
			எனில்(rc){
				mosquitto_property_free_all(&connack_props);
				திரும்பு rc;
			}
		}
		எனில்(context->listener->max_qos != 2){
			rc = mosquitto_property_add_byte(&connack_props, MQTT_PROP_MAXIMUM_QOS, context->listener->max_qos);
			எனில்(rc){
				mosquitto_property_free_all(&connack_props);
				திரும்பு rc;
			}
		}

		remaining_length += mosquitto_property_get_remaining_length(connack_props);
	}

	எனில்(packet__check_oversize(context, remaining_length)){
		mosquitto_property_free_all(&connack_props);
		திரும்பு MOSQ_ERR_OVERSIZE_PACKET;
	}

	rc = packet__alloc(&packet, CMD_CONNACK, remaining_length);
	எனில்(rc){
		mosquitto_property_free_all(&connack_props);
		திரும்பு rc;
	}
	packet__write_byte(packet, ack);
	packet__write_byte(packet, reason_code);
	எனில்(context->protocol == mosq_p_mqtt5){
		property__write_all(packet, connack_props, true);
	}
	mosquitto_property_free_all(&connack_props);

	metrics__int_inc(mosq_counter_mqtt_connack_sent, 1);
	திரும்பு packet__queue(context, packet);
}

