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


পূর্ণসংখ্যা send__connack(কাঠামো mosquitto *context, uint8_t ack, uint8_t reason_code, ধ্রুবক mosquitto_property *properties)
{
	কাঠামো mosquitto__packet *packet = ফাঁকা;
	পূর্ণসংখ্যা rc;
	mosquitto_property *connack_props = ফাঁকা;
	uint32_t remaining_দৈর্ঘ্য;

	rc = mosquitto_property_copy_all(&connack_props, properties);
	যদি(rc){
		ফেরত rc;
	}

	যদি(context->id){
		log__printf(ফাঁকা, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->id, ack, reason_code);
	}অন্যথা{
		log__printf(ফাঁকা, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->address, ack, reason_code);
	}

	remaining_দৈর্ঘ্য = 2;

	যদি(context->protocol == mosq_p_mqtt5){
		যদি(reason_code < 128 && db.config->retain_available == false){
			rc = mosquitto_property_add_byte(&connack_props, MQTT_PROP_RETAIN_AVAILABLE, 0);
			যদি(rc){
				mosquitto_property_free_all(&connack_props);
				ফেরত rc;
			}
		}
		যদি(reason_code < 128 && db.config->max_packet_আকার > 0){
			rc = mosquitto_property_add_int32(&connack_props, MQTT_PROP_MAXIMUM_PACKET_আকার, db.config->max_packet_আকার);
			যদি(rc){
				mosquitto_property_free_all(&connack_props);
				ফেরত rc;
			}
		}
		যদি(reason_code < 128 && db.config->max_inflight_messages > 0){
			rc = mosquitto_property_add_int16(&connack_props, MQTT_PROP_RECEIVE_MAXIMUM, db.config->max_inflight_messages);
			যদি(rc){
				mosquitto_property_free_all(&connack_props);
				ফেরত rc;
			}
		}
		যদি(context->listener->max_qos != 2){
			rc = mosquitto_property_add_byte(&connack_props, MQTT_PROP_MAXIMUM_QOS, context->listener->max_qos);
			যদি(rc){
				mosquitto_property_free_all(&connack_props);
				ফেরত rc;
			}
		}

		remaining_দৈর্ঘ্য += mosquitto_property_get_remaining_দৈর্ঘ্য(connack_props);
	}

	যদি(packet__check_oversize(context, remaining_দৈর্ঘ্য)){
		mosquitto_property_free_all(&connack_props);
		ফেরত MOSQ_ERR_OVERSIZE_PACKET;
	}

	rc = packet__alloc(&packet, CMD_CONNACK, remaining_দৈর্ঘ্য);
	যদি(rc){
		mosquitto_property_free_all(&connack_props);
		ফেরত rc;
	}
	packet__write_byte(packet, ack);
	packet__write_byte(packet, reason_code);
	যদি(context->protocol == mosq_p_mqtt5){
		property__write_all(packet, connack_props, true);
	}
	mosquitto_property_free_all(&connack_props);

	metrics__int_inc(mosq_গণক_mqtt_connack_sent, 1);
	ফেরত packet__queue(context, packet);
}

