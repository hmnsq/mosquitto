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

#అన్తర్భావ "config.h"

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "packet_mosq.h"
#అన్తర్భావ "read_handle.h"
#అన్తర్భావ "send_mosq.h"
#అన్తర్భావ "sys_tree.h"
#అన్తర్భావ "util_mosq.h"


పూర్ణాంకం handle__packet(నిర్మాణం mosquitto *context)
{
	పూర్ణాంకం rc = MOSQ_ERR_INVAL;

	ఒకవేళ(!context){
		తిరిగి MOSQ_ERR_INVAL;
	}

	ఎంపిక((context->లోపల_packet.command)&0xF0){
		స్థితి CMD_PINGREQ:
			metrics__int_inc(mosq_counter_mqtt_pingreq_received, 1);
			rc = handle__pingreq(context);
			ఆపు;
		స్థితి CMD_PINGRESP:
			metrics__int_inc(mosq_counter_mqtt_pingresp_received, 1);
			rc = handle__pingresp(context);
			ఆపు;
		స్థితి CMD_PUBACK:
			metrics__int_inc(mosq_counter_mqtt_puback_received, 1);
			rc = handle__pubackcomp(context, "PUBACK");
			ఆపు;
		స్థితి CMD_PUBCOMP:
			metrics__int_inc(mosq_counter_mqtt_pubcomp_received, 1);
			rc = handle__pubackcomp(context, "PUBCOMP");
			ఆపు;
		స్థితి CMD_PUBLISH:
			metrics__int_inc(mosq_counter_mqtt_publish_received, 1);
			rc = handle__publish(context);
			ఆపు;
		స్థితి CMD_PUBREC:
			metrics__int_inc(mosq_counter_mqtt_pubrec_received, 1);
			rc = handle__pubrec(context);
			ఆపు;
		స్థితి CMD_PUBREL:
			metrics__int_inc(mosq_counter_mqtt_pubrel_received, 1);
			rc = handle__pubrel(context);
			ఆపు;
		స్థితి CMD_CONNECT:
			metrics__int_inc(mosq_counter_mqtt_connect_received, 1);
			తిరిగి handle__connect(context);
		స్థితి CMD_DISCONNECT:
			metrics__int_inc(mosq_counter_mqtt_disconnect_received, 1);
			rc = handle__disconnect(context);
			ఆపు;
		స్థితి CMD_SUBSCRIBE:
			metrics__int_inc(mosq_counter_mqtt_subscribe_received, 1);
			rc = handle__subscribe(context);
			ఆపు;
		స్థితి CMD_UNSUBSCRIBE:
			metrics__int_inc(mosq_counter_mqtt_unsubscribe_received, 1);
			rc = handle__unsubscribe(context);
			ఆపు;
#నిర్వచితమైతే WITH_BRIDGE
		స్థితి CMD_CONNACK:
			metrics__int_inc(mosq_counter_mqtt_connack_received, 1);
			rc = handle__connack(context);
			ఆపు;
		స్థితి CMD_SUBACK:
			metrics__int_inc(mosq_counter_mqtt_suback_received, 1);
			rc = handle__suback(context);
			ఆపు;
		స్థితి CMD_UNSUBACK:
			metrics__int_inc(mosq_counter_mqtt_unsuback_received, 1);
			rc = handle__unsuback(context);
			ఆపు;
#అయితే_ముగింపు
		స్థితి CMD_AUTH:
			metrics__int_inc(mosq_counter_mqtt_auth_received, 1);
			rc = handle__auth(context);
			ఆపు;
		అప్రమేయ:
			rc = MOSQ_ERR_PROTOCOL;
	}

	ఒకవేళ(context->protocol == mosq_p_mqtt5){
		ఒకవేళ(rc == MOSQ_ERR_PROTOCOL || rc == MOSQ_ERR_DUPLICATE_PROPERTY){
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ఖాళీ);
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__disconnect(context, MQTT_RC_MALFORMED_PACKET, ఖాళీ);
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_QOS_NOT_SUPPORTED){
			send__disconnect(context, MQTT_RC_QOS_NOT_SUPPORTED, ఖాళీ);
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_RETAIN_NOT_SUPPORTED){
			send__disconnect(context, MQTT_RC_RETAIN_NOT_SUPPORTED, ఖాళీ);
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_TOPIC_ALIAS_INVALID){
			send__disconnect(context, MQTT_RC_TOPIC_ALIAS_INVALID, ఖాళీ);
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_RECEIVE_గరిష్ఠ_EXCEEDED){
			send__disconnect(context, MQTT_RC_RECEIVE_గరిష్ఠ_EXCEEDED, ఖాళీ);
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_UNKNOWN || rc == MOSQ_ERR_NOMEM){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, ఖాళీ);
		}
	}
	తిరిగి rc;
}
