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

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "read_handle.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "send_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sys_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"


ਪੂਰਨ handle__packet(ਬਣਤਰ mosquitto *context)
{
	ਪੂਰਨ rc = MOSQ_ERR_INVAL;

	ਜੇ(!context){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਚੋਣ((context->ਵਿੱਚ_packet.command)&0xF0){
		ਹਾਲਤ CMD_PINGREQ:
			metrics__int_inc(mosq_counter_mqtt_pingreq_received, 1);
			rc = handle__pingreq(context);
			ਰੋਕੋ;
		ਹਾਲਤ CMD_PINGRESP:
			metrics__int_inc(mosq_counter_mqtt_pingresp_received, 1);
			rc = handle__pingresp(context);
			ਰੋਕੋ;
		ਹਾਲਤ CMD_PUBACK:
			metrics__int_inc(mosq_counter_mqtt_puback_received, 1);
			rc = handle__pubackcomp(context, "PUBACK");
			ਰੋਕੋ;
		ਹਾਲਤ CMD_PUBCOMP:
			metrics__int_inc(mosq_counter_mqtt_pubcomp_received, 1);
			rc = handle__pubackcomp(context, "PUBCOMP");
			ਰੋਕੋ;
		ਹਾਲਤ CMD_PUBLISH:
			metrics__int_inc(mosq_counter_mqtt_publish_received, 1);
			rc = handle__publish(context);
			ਰੋਕੋ;
		ਹਾਲਤ CMD_PUBREC:
			metrics__int_inc(mosq_counter_mqtt_pubrec_received, 1);
			rc = handle__pubrec(context);
			ਰੋਕੋ;
		ਹਾਲਤ CMD_PUBREL:
			metrics__int_inc(mosq_counter_mqtt_pubrel_received, 1);
			rc = handle__pubrel(context);
			ਰੋਕੋ;
		ਹਾਲਤ CMD_CONNECT:
			metrics__int_inc(mosq_counter_mqtt_connect_received, 1);
			ਵਾਪਸ handle__connect(context);
		ਹਾਲਤ CMD_DISCONNECT:
			metrics__int_inc(mosq_counter_mqtt_disconnect_received, 1);
			rc = handle__disconnect(context);
			ਰੋਕੋ;
		ਹਾਲਤ CMD_SUBSCRIBE:
			metrics__int_inc(mosq_counter_mqtt_subscribe_received, 1);
			rc = handle__subscribe(context);
			ਰੋਕੋ;
		ਹਾਲਤ CMD_UNSUBSCRIBE:
			metrics__int_inc(mosq_counter_mqtt_unsubscribe_received, 1);
			rc = handle__unsubscribe(context);
			ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
		ਹਾਲਤ CMD_CONNACK:
			metrics__int_inc(mosq_counter_mqtt_connack_received, 1);
			rc = handle__connack(context);
			ਰੋਕੋ;
		ਹਾਲਤ CMD_SUBACK:
			metrics__int_inc(mosq_counter_mqtt_suback_received, 1);
			rc = handle__suback(context);
			ਰੋਕੋ;
		ਹਾਲਤ CMD_UNSUBACK:
			metrics__int_inc(mosq_counter_mqtt_unsuback_received, 1);
			rc = handle__unsuback(context);
			ਰੋਕੋ;
#ਜੇ_ਅੰਤ
		ਹਾਲਤ CMD_AUTH:
			metrics__int_inc(mosq_counter_mqtt_auth_received, 1);
			rc = handle__auth(context);
			ਰੋਕੋ;
		ਮੂਲ:
			rc = MOSQ_ERR_PROTOCOL;
	}

	ਜੇ(context->protocol == mosq_p_mqtt5){
		ਜੇ(rc == MOSQ_ERR_PROTOCOL || rc == MOSQ_ERR_DUPLICATE_PROPERTY){
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ਨਿਰਾ);
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__disconnect(context, MQTT_RC_MALFORMED_PACKET, ਨਿਰਾ);
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_QOS_NOT_SUPPORTED){
			send__disconnect(context, MQTT_RC_QOS_NOT_SUPPORTED, ਨਿਰਾ);
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_RETAIN_NOT_SUPPORTED){
			send__disconnect(context, MQTT_RC_RETAIN_NOT_SUPPORTED, ਨਿਰਾ);
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_TOPIC_ALIAS_INVALID){
			send__disconnect(context, MQTT_RC_TOPIC_ALIAS_INVALID, ਨਿਰਾ);
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_RECEIVE_ਵੱਧ_ਤੋਂ_ਵੱਧ_EXCEEDED){
			send__disconnect(context, MQTT_RC_RECEIVE_ਵੱਧ_ਤੋਂ_ਵੱਧ_EXCEEDED, ਨਿਰਾ);
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_UNKNOWN || rc == MOSQ_ERR_NOMEM){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, ਨਿਰਾ);
		}
	}
	ਵਾਪਸ rc;
}
