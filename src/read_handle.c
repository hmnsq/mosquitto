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

#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "packet_mosq.h"
#شامل "read_handle.h"
#شامل "send_mosq.h"
#شامل "sys_tree.h"
#شامل "util_mosq.h"


صحیح handle__packet(ساخت mosquitto *context)
{
	صحیح rc = MOSQ_ERR_INVAL;

	اگر(!context){
		واپس MOSQ_ERR_INVAL;
	}

	انتخاب((context->اندر_packet.command)&0xF0){
		حالت CMD_PINGREQ:
			metrics__int_inc(mosq_شمار_mqtt_pingreq_received, 1);
			rc = handle__pingreq(context);
			توڑو;
		حالت CMD_PINGRESP:
			metrics__int_inc(mosq_شمار_mqtt_pingresp_received, 1);
			rc = handle__pingresp(context);
			توڑو;
		حالت CMD_PUBACK:
			metrics__int_inc(mosq_شمار_mqtt_puback_received, 1);
			rc = handle__pubackcomp(context, "PUBACK");
			توڑو;
		حالت CMD_PUBCOMP:
			metrics__int_inc(mosq_شمار_mqtt_pubcomp_received, 1);
			rc = handle__pubackcomp(context, "PUBCOMP");
			توڑو;
		حالت CMD_PUBLISH:
			metrics__int_inc(mosq_شمار_mqtt_publish_received, 1);
			rc = handle__publish(context);
			توڑو;
		حالت CMD_PUBREC:
			metrics__int_inc(mosq_شمار_mqtt_pubrec_received, 1);
			rc = handle__pubrec(context);
			توڑو;
		حالت CMD_PUBREL:
			metrics__int_inc(mosq_شمار_mqtt_pubrel_received, 1);
			rc = handle__pubrel(context);
			توڑو;
		حالت CMD_CONNECT:
			metrics__int_inc(mosq_شمار_mqtt_connect_received, 1);
			واپس handle__connect(context);
		حالت CMD_DISCONNECT:
			metrics__int_inc(mosq_شمار_mqtt_disconnect_received, 1);
			rc = handle__disconnect(context);
			توڑو;
		حالت CMD_SUBSCRIBE:
			metrics__int_inc(mosq_شمار_mqtt_subscribe_received, 1);
			rc = handle__subscribe(context);
			توڑو;
		حالت CMD_UNSUBSCRIBE:
			metrics__int_inc(mosq_شمار_mqtt_unsubscribe_received, 1);
			rc = handle__unsubscribe(context);
			توڑو;
#اگر_تعریف_شدہ WITH_BRIDGE
		حالت CMD_CONNACK:
			metrics__int_inc(mosq_شمار_mqtt_connack_received, 1);
			rc = handle__connack(context);
			توڑو;
		حالت CMD_SUBACK:
			metrics__int_inc(mosq_شمار_mqtt_suback_received, 1);
			rc = handle__suback(context);
			توڑو;
		حالت CMD_UNSUBACK:
			metrics__int_inc(mosq_شمار_mqtt_unsuback_received, 1);
			rc = handle__unsuback(context);
			توڑو;
#اگر_ختم
		حالت CMD_AUTH:
			metrics__int_inc(mosq_شمار_mqtt_auth_received, 1);
			rc = handle__auth(context);
			توڑو;
		طےشدہ:
			rc = MOSQ_ERR_PROTOCOL;
	}

	اگر(context->protocol == mosq_p_mqtt5){
		اگر(rc == MOSQ_ERR_PROTOCOL || rc == MOSQ_ERR_DUPLICATE_PROPERTY){
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, تہی);
		}ورنہ اگر(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__disconnect(context, MQTT_RC_MALFORMED_PACKET, تہی);
		}ورنہ اگر(rc == MOSQ_ERR_QOS_NOT_SUPPORTED){
			send__disconnect(context, MQTT_RC_QOS_NOT_SUPPORTED, تہی);
		}ورنہ اگر(rc == MOSQ_ERR_RETAIN_NOT_SUPPORTED){
			send__disconnect(context, MQTT_RC_RETAIN_NOT_SUPPORTED, تہی);
		}ورنہ اگر(rc == MOSQ_ERR_TOPIC_ALIAS_INVALID){
			send__disconnect(context, MQTT_RC_TOPIC_ALIAS_INVALID, تہی);
		}ورنہ اگر(rc == MOSQ_ERR_RECEIVE_زیادہ_سے_زیادہ_EXCEEDED){
			send__disconnect(context, MQTT_RC_RECEIVE_زیادہ_سے_زیادہ_EXCEEDED, تہی);
		}ورنہ اگر(rc == MOSQ_ERR_UNKNOWN || rc == MOSQ_ERR_NOMEM){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, تہی);
		}
	}
	واپس rc;
}
