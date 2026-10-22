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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "packet_mosq.h"
#ଅନ୍ତର୍ଭାଵ "property_mosq.h"
#ଅନ୍ତର୍ଭାଵ "sys_tree.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"


ପୂର୍ଣ୍ଣସଂଖ୍ୟା send__connack(ସଂରଚନା mosquitto *context, uint8_t ack, uint8_t reason_code, ଅଚଳ mosquitto_property *properties)
{
	ସଂରଚନା mosquitto__packet *packet = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	mosquitto_property *connack_props = ଖାଲି;
	uint32_t remaining_length;

	rc = mosquitto_property_copy_all(&connack_props, properties);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	ଯଦି(context->id){
		log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->id, ack, reason_code);
	}ଅନ୍ୟଥା{
		log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->address, ack, reason_code);
	}

	remaining_length = 2;

	ଯଦି(context->protocol == mosq_p_mqtt5){
		ଯଦି(reason_code < 128 && db.config->retain_available == false){
			rc = mosquitto_property_add_byte(&connack_props, MQTT_PROP_RETAIN_AVAILABLE, 0);
			ଯଦି(rc){
				mosquitto_property_free_all(&connack_props);
				ଫେରନ୍ତୁ rc;
			}
		}
		ଯଦି(reason_code < 128 && db.config->max_packet_size > 0){
			rc = mosquitto_property_add_int32(&connack_props, MQTT_PROP_MAXIMUM_PACKET_SIZE, db.config->max_packet_size);
			ଯଦି(rc){
				mosquitto_property_free_all(&connack_props);
				ଫେରନ୍ତୁ rc;
			}
		}
		ଯଦି(reason_code < 128 && db.config->max_inflight_messages > 0){
			rc = mosquitto_property_add_int16(&connack_props, MQTT_PROP_RECEIVE_MAXIMUM, db.config->max_inflight_messages);
			ଯଦି(rc){
				mosquitto_property_free_all(&connack_props);
				ଫେରନ୍ତୁ rc;
			}
		}
		ଯଦି(context->listener->max_qos != 2){
			rc = mosquitto_property_add_byte(&connack_props, MQTT_PROP_MAXIMUM_QOS, context->listener->max_qos);
			ଯଦି(rc){
				mosquitto_property_free_all(&connack_props);
				ଫେରନ୍ତୁ rc;
			}
		}

		remaining_length += mosquitto_property_get_remaining_length(connack_props);
	}

	ଯଦି(packet__check_oversize(context, remaining_length)){
		mosquitto_property_free_all(&connack_props);
		ଫେରନ୍ତୁ MOSQ_ERR_OVERSIZE_PACKET;
	}

	rc = packet__alloc(&packet, CMD_CONNACK, remaining_length);
	ଯଦି(rc){
		mosquitto_property_free_all(&connack_props);
		ଫେରନ୍ତୁ rc;
	}
	packet__write_byte(packet, ack);
	packet__write_byte(packet, reason_code);
	ଯଦି(context->protocol == mosq_p_mqtt5){
		property__write_all(packet, connack_props, true);
	}
	mosquitto_property_free_all(&connack_props);

	metrics__int_inc(mosq_counter_mqtt_connack_sent, 1);
	ଫେରନ୍ତୁ packet__queue(context, packet);
}

