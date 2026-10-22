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


صحيح send__connack(بنية mosquitto *context, uint8_t ack, uint8_t reason_کد, ثابت mosquitto_property *properties)
{
	بنية mosquitto__packet *packet = عدم;
	صحيح rc;
	mosquitto_property *connack_props = عدم;
	uint32_t مانده_درازی;

	rc = mosquitto_property_copy_all(&connack_props, properties);
	إذا(rc){
		أرجع rc;
	}

	إذا(context->id){
		log__printf(عدم, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->id, ack, reason_کد);
	}وإلا{
		log__printf(عدم, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->آدرس, ack, reason_کد);
	}

	مانده_درازی = 2;

	إذا(context->protocol == mosq_p_mqtt5){
		إذا(reason_کد < 128 && db.config->retain_موجود == false){
			rc = mosquitto_property_جمع_کن_byte(&connack_props, MQTT_PROP_RETAIN_موجود, 0);
			إذا(rc){
				mosquitto_property_free_all(&connack_props);
				أرجع rc;
			}
		}
		إذا(reason_کد < 128 && db.config->max_packet_اندازه > 0){
			rc = mosquitto_property_جمع_کن_int32(&connack_props, MQTT_PROP_بیشینه_PACKET_اندازه, db.config->max_packet_اندازه);
			إذا(rc){
				mosquitto_property_free_all(&connack_props);
				أرجع rc;
			}
		}
		إذا(reason_کد < 128 && db.config->max_inflight_messages > 0){
			rc = mosquitto_property_جمع_کن_int16(&connack_props, MQTT_PROP_RECEIVE_بیشینه, db.config->max_inflight_messages);
			إذا(rc){
				mosquitto_property_free_all(&connack_props);
				أرجع rc;
			}
		}
		إذا(context->listener->max_qos != 2){
			rc = mosquitto_property_جمع_کن_byte(&connack_props, MQTT_PROP_بیشینه_QOS, context->listener->max_qos);
			إذا(rc){
				mosquitto_property_free_all(&connack_props);
				أرجع rc;
			}
		}

		مانده_درازی += mosquitto_property_get_مانده_درازی(connack_props);
	}

	إذا(packet__check_oversize(context, مانده_درازی)){
		mosquitto_property_free_all(&connack_props);
		أرجع MOSQ_ERR_OVERSIZE_PACKET;
	}

	rc = packet__alloc(&packet, CMD_CONNACK, مانده_درازی);
	إذا(rc){
		mosquitto_property_free_all(&connack_props);
		أرجع rc;
	}
	packet__write_byte(packet, ack);
	packet__write_byte(packet, reason_کد);
	إذا(context->protocol == mosq_p_mqtt5){
		property__write_all(packet, connack_props, true);
	}
	mosquitto_property_free_all(&connack_props);

	metrics__int_inc(mosq_counter_mqtt_connack_فرستاده, 1);
	أرجع packet_صف(context, packet);
}

