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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "property_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sys_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"


ਪੂਰਨ send__connack(ਬਣਤਰ mosquitto *context, uint8_t ack, uint8_t reason_ਕੋਡ, ਅਚਲ mosquitto_property *properties)
{
	ਬਣਤਰ mosquitto__packet *packet = ਨਿਰਾ;
	ਪੂਰਨ rc;
	mosquitto_property *connack_props = ਨਿਰਾ;
	uint32_t ਬਚਿਆ_ਲੰਮਾਈ;

	rc = mosquitto_property_copy_all(&connack_props, properties);
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	ਜੇ(context->id){
		log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->id, ack, reason_ਕੋਡ);
	}ਨਹੀਂਤਾਂ{
		log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->ਪਤਾ, ack, reason_ਕੋਡ);
	}

	ਬਚਿਆ_ਲੰਮਾਈ = 2;

	ਜੇ(context->protocol == mosq_p_mqtt5){
		ਜੇ(reason_ਕੋਡ < 128 && db.config->retain_ਉਪਲਬਧ == false){
			rc = mosquitto_property_ਜੋੜੋ_byte(&connack_props, MQTT_PROP_RETAIN_ਉਪਲਬਧ, 0);
			ਜੇ(rc){
				mosquitto_property_free_all(&connack_props);
				ਵਾਪਸ rc;
			}
		}
		ਜੇ(reason_ਕੋਡ < 128 && db.config->max_packet_ਮਾਪ > 0){
			rc = mosquitto_property_ਜੋੜੋ_int32(&connack_props, MQTT_PROP_ਵੱਧ_ਤੋਂ_ਵੱਧ_PACKET_ਮਾਪ, db.config->max_packet_ਮਾਪ);
			ਜੇ(rc){
				mosquitto_property_free_all(&connack_props);
				ਵਾਪਸ rc;
			}
		}
		ਜੇ(reason_ਕੋਡ < 128 && db.config->max_inflight_messages > 0){
			rc = mosquitto_property_ਜੋੜੋ_int16(&connack_props, MQTT_PROP_RECEIVE_ਵੱਧ_ਤੋਂ_ਵੱਧ, db.config->max_inflight_messages);
			ਜੇ(rc){
				mosquitto_property_free_all(&connack_props);
				ਵਾਪਸ rc;
			}
		}
		ਜੇ(context->listener->max_qos != 2){
			rc = mosquitto_property_ਜੋੜੋ_byte(&connack_props, MQTT_PROP_ਵੱਧ_ਤੋਂ_ਵੱਧ_QOS, context->listener->max_qos);
			ਜੇ(rc){
				mosquitto_property_free_all(&connack_props);
				ਵਾਪਸ rc;
			}
		}

		ਬਚਿਆ_ਲੰਮਾਈ += mosquitto_property_get_ਬਚਿਆ_ਲੰਮਾਈ(connack_props);
	}

	ਜੇ(packet__check_oversize(context, ਬਚਿਆ_ਲੰਮਾਈ)){
		mosquitto_property_free_all(&connack_props);
		ਵਾਪਸ MOSQ_ERR_OVERSIZE_PACKET;
	}

	rc = packet__alloc(&packet, CMD_CONNACK, ਬਚਿਆ_ਲੰਮਾਈ);
	ਜੇ(rc){
		mosquitto_property_free_all(&connack_props);
		ਵਾਪਸ rc;
	}
	packet__write_byte(packet, ack);
	packet__write_byte(packet, reason_ਕੋਡ);
	ਜੇ(context->protocol == mosq_p_mqtt5){
		property__write_all(packet, connack_props, true);
	}
	mosquitto_property_free_all(&connack_props);

	metrics__int_inc(mosq_counter_mqtt_connack_ਭੇਜਿਆ, 1);
	ਵਾਪਸ packet_ਕਤਾਰ(context, packet);
}

