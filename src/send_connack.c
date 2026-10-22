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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "packet_mosq.h"
#അന്തര്ഭാവ "property_mosq.h"
#അന്തര്ഭാവ "sys_tree.h"
#അന്തര്ഭാവ "util_mosq.h"


പൂർണ്ണസംഖ്യ send__connack(ഘടന mosquitto *context, uint8_t ack, uint8_t reason_code, സ്ഥിരം mosquitto_property *properties)
{
	ഘടന mosquitto__packet *packet = ശൂന്യ;
	പൂർണ്ണസംഖ്യ rc;
	mosquitto_property *connack_props = ശൂന്യ;
	uint32_t remaining_length;

	rc = mosquitto_property_copy_all(&connack_props, properties);
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	എങ്കിൽ(context->id){
		log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->id, ack, reason_code);
	}അല്ലെങ്കിൽ{
		log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->address, ack, reason_code);
	}

	remaining_length = 2;

	എങ്കിൽ(context->protocol == mosq_p_mqtt5){
		എങ്കിൽ(reason_code < 128 && db.config->retain_available == false){
			rc = mosquitto_property_add_byte(&connack_props, MQTT_PROP_RETAIN_AVAILABLE, 0);
			എങ്കിൽ(rc){
				mosquitto_property_free_all(&connack_props);
				തിരികെ rc;
			}
		}
		എങ്കിൽ(reason_code < 128 && db.config->max_packet_size > 0){
			rc = mosquitto_property_add_int32(&connack_props, MQTT_PROP_MAXIMUM_PACKET_SIZE, db.config->max_packet_size);
			എങ്കിൽ(rc){
				mosquitto_property_free_all(&connack_props);
				തിരികെ rc;
			}
		}
		എങ്കിൽ(reason_code < 128 && db.config->max_inflight_messages > 0){
			rc = mosquitto_property_add_int16(&connack_props, MQTT_PROP_RECEIVE_MAXIMUM, db.config->max_inflight_messages);
			എങ്കിൽ(rc){
				mosquitto_property_free_all(&connack_props);
				തിരികെ rc;
			}
		}
		എങ്കിൽ(context->listener->max_qos != 2){
			rc = mosquitto_property_add_byte(&connack_props, MQTT_PROP_MAXIMUM_QOS, context->listener->max_qos);
			എങ്കിൽ(rc){
				mosquitto_property_free_all(&connack_props);
				തിരികെ rc;
			}
		}

		remaining_length += mosquitto_property_get_remaining_length(connack_props);
	}

	എങ്കിൽ(packet__check_oversize(context, remaining_length)){
		mosquitto_property_free_all(&connack_props);
		തിരികെ MOSQ_ERR_OVERSIZE_PACKET;
	}

	rc = packet__alloc(&packet, CMD_CONNACK, remaining_length);
	എങ്കിൽ(rc){
		mosquitto_property_free_all(&connack_props);
		തിരികെ rc;
	}
	packet__write_byte(packet, ack);
	packet__write_byte(packet, reason_code);
	എങ്കിൽ(context->protocol == mosq_p_mqtt5){
		property__write_all(packet, connack_props, true);
	}
	mosquitto_property_free_all(&connack_props);

	metrics__int_inc(mosq_counter_mqtt_connack_sent, 1);
	തിരികെ packet__queue(context, packet);
}

