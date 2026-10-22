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


صحیح send__connack(ساخت mosquitto *context, uint8_t ack, uint8_t reason_رمز, ثابت mosquitto_property *properties)
{
	ساخت mosquitto__packet *packet = تہی;
	صحیح rc;
	mosquitto_property *connack_props = تہی;
	uint32_t بقایا_لمبائی;

	rc = mosquitto_property_copy_all(&connack_props, properties);
	اگر(rc){
		واپس rc;
	}

	اگر(context->id){
		log__printf(تہی, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->id, ack, reason_رمز);
	}ورنہ{
		log__printf(تہی, MOSQ_LOG_DEBUG, "Sending CONNACK to %s (%d, %d)", context->پتہ, ack, reason_رمز);
	}

	بقایا_لمبائی = 2;

	اگر(context->protocol == mosq_p_mqtt5){
		اگر(reason_رمز < 128 && db.config->retain_دستیاب == false){
			rc = mosquitto_property_جوڑو_byte(&connack_props, MQTT_PROP_RETAIN_دستیاب, 0);
			اگر(rc){
				mosquitto_property_free_all(&connack_props);
				واپس rc;
			}
		}
		اگر(reason_رمز < 128 && db.config->max_packet_سائز > 0){
			rc = mosquitto_property_جوڑو_int32(&connack_props, MQTT_PROP_زیادہ_سے_زیادہ_PACKET_سائز, db.config->max_packet_سائز);
			اگر(rc){
				mosquitto_property_free_all(&connack_props);
				واپس rc;
			}
		}
		اگر(reason_رمز < 128 && db.config->max_inflight_messages > 0){
			rc = mosquitto_property_جوڑو_int16(&connack_props, MQTT_PROP_RECEIVE_زیادہ_سے_زیادہ, db.config->max_inflight_messages);
			اگر(rc){
				mosquitto_property_free_all(&connack_props);
				واپس rc;
			}
		}
		اگر(context->listener->max_qos != 2){
			rc = mosquitto_property_جوڑو_byte(&connack_props, MQTT_PROP_زیادہ_سے_زیادہ_QOS, context->listener->max_qos);
			اگر(rc){
				mosquitto_property_free_all(&connack_props);
				واپس rc;
			}
		}

		بقایا_لمبائی += mosquitto_property_get_بقایا_لمبائی(connack_props);
	}

	اگر(packet__check_oversize(context, بقایا_لمبائی)){
		mosquitto_property_free_all(&connack_props);
		واپس MOSQ_ERR_OVERSIZE_PACKET;
	}

	rc = packet__alloc(&packet, CMD_CONNACK, بقایا_لمبائی);
	اگر(rc){
		mosquitto_property_free_all(&connack_props);
		واپس rc;
	}
	packet__write_byte(packet, ack);
	packet__write_byte(packet, reason_رمز);
	اگر(context->protocol == mosq_p_mqtt5){
		property__write_all(packet, connack_props, true);
	}
	mosquitto_property_free_all(&connack_props);

	metrics__int_inc(mosq_شمار_mqtt_connack_بھیجا, 1);
	واپس packet_قطار(context, packet);
}

