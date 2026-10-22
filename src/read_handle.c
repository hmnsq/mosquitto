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

#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "packet_mosq.h"
#ضمن "read_handle.h"
#ضمن "send_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"


صحيح handle__packet(بنية mosquitto *context)
{
	صحيح rc = MOSQ_ERR_INVAL;

	אם(!context){
		החזר MOSQ_ERR_INVAL;
	}

	بدل((context->in_packet.command)&0xF0){
		حالة CMD_PINGREQ:
			metrics__int_inc(mosq_counter_mqtt_pingreq_received, 1);
			rc = handle__pingreq(context);
			اكسر;
		حالة CMD_PINGRESP:
			metrics__int_inc(mosq_counter_mqtt_pingresp_received, 1);
			rc = handle__pingresp(context);
			اكسر;
		حالة CMD_PUBACK:
			metrics__int_inc(mosq_counter_mqtt_puback_received, 1);
			rc = handle__pubackcomp(context, "PUBACK");
			اكسر;
		حالة CMD_PUBCOMP:
			metrics__int_inc(mosq_counter_mqtt_pubcomp_received, 1);
			rc = handle__pubackcomp(context, "PUBCOMP");
			اكسر;
		حالة CMD_PUBLISH:
			metrics__int_inc(mosq_counter_mqtt_publish_received, 1);
			rc = handle__publish(context);
			اكسر;
		حالة CMD_PUBREC:
			metrics__int_inc(mosq_counter_mqtt_pubrec_received, 1);
			rc = handle__pubrec(context);
			اكسر;
		حالة CMD_PUBREL:
			metrics__int_inc(mosq_counter_mqtt_pubrel_received, 1);
			rc = handle__pubrel(context);
			اكسر;
		حالة CMD_CONNECT:
			metrics__int_inc(mosq_counter_mqtt_connect_received, 1);
			החזר handle__connect(context);
		حالة CMD_DISCONNECT:
			metrics__int_inc(mosq_counter_mqtt_disconnect_received, 1);
			rc = handle__disconnect(context);
			اكسر;
		حالة CMD_SUBSCRIBE:
			metrics__int_inc(mosq_counter_mqtt_subscribe_received, 1);
			rc = handle__subscribe(context);
			اكسر;
		حالة CMD_UNSUBSCRIBE:
			metrics__int_inc(mosq_counter_mqtt_unsubscribe_received, 1);
			rc = handle__unsubscribe(context);
			اكسر;
#ifdef WITH_BRIDGE
		حالة CMD_CONNACK:
			metrics__int_inc(mosq_counter_mqtt_connack_received, 1);
			rc = handle__connack(context);
			اكسر;
		حالة CMD_SUBACK:
			metrics__int_inc(mosq_counter_mqtt_suback_received, 1);
			rc = handle__suback(context);
			اكسر;
		حالة CMD_UNSUBACK:
			metrics__int_inc(mosq_counter_mqtt_unsuback_received, 1);
			rc = handle__unsuback(context);
			اكسر;
#endif
		حالة CMD_AUTH:
			metrics__int_inc(mosq_counter_mqtt_auth_received, 1);
			rc = handle__auth(context);
			اكسر;
		افتراضي:
			rc = MOSQ_ERR_PROTOCOL;
	}

	אם(context->protocol == mosq_p_mqtt5){
		אם(rc == MOSQ_ERR_PROTOCOL || rc == MOSQ_ERR_DUPLICATE_PROPERTY){
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, عدم);
		}אחרת אם(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__disconnect(context, MQTT_RC_MALFORMED_PACKET, عدم);
		}אחרת אם(rc == MOSQ_ERR_QOS_NOT_SUPPORTED){
			send__disconnect(context, MQTT_RC_QOS_NOT_SUPPORTED, عدم);
		}אחרת אם(rc == MOSQ_ERR_RETAIN_NOT_SUPPORTED){
			send__disconnect(context, MQTT_RC_RETAIN_NOT_SUPPORTED, عدم);
		}אחרת אם(rc == MOSQ_ERR_TOPIC_ALIAS_INVALID){
			send__disconnect(context, MQTT_RC_TOPIC_ALIAS_INVALID, عدم);
		}אחרת אם(rc == MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED){
			send__disconnect(context, MQTT_RC_RECEIVE_MAXIMUM_EXCEEDED, عدم);
		}אחרת אם(rc == MOSQ_ERR_UNKNOWN || rc == MOSQ_ERR_NOMEM){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, عدم);
		}
	}
	החזר rc;
}
