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

#समावेश "config.h"

#समावेश <stdio.h>
#समावेश <string.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "packet_mosq.h"
#समावेश "read_handle.h"
#समावेश "send_mosq.h"
#समावेश "sys_tree.h"
#समावेश "util_mosq.h"


पूर्णांक handle__packet(काष्ठा mosquitto *context)
{
	पूर्णांक rc = MOSQ_ERR_INVAL;

	अगर(!context){
		वापस MOSQ_ERR_INVAL;
	}

	चयन((context->in_packet.command)&0xF0){
		हाल CMD_PINGREQ:
			metrics__int_inc(mosq_गणक_mqtt_pingreq_received, 1);
			rc = handle__pingreq(context);
			अवरोध;
		हाल CMD_PINGRESP:
			metrics__int_inc(mosq_गणक_mqtt_pingresp_received, 1);
			rc = handle__pingresp(context);
			अवरोध;
		हाल CMD_PUBACK:
			metrics__int_inc(mosq_गणक_mqtt_puback_received, 1);
			rc = handle__pubackcomp(context, "PUBACK");
			अवरोध;
		हाल CMD_PUBCOMP:
			metrics__int_inc(mosq_गणक_mqtt_pubcomp_received, 1);
			rc = handle__pubackcomp(context, "PUBCOMP");
			अवरोध;
		हाल CMD_PUBLISH:
			metrics__int_inc(mosq_गणक_mqtt_publish_received, 1);
			rc = handle__publish(context);
			अवरोध;
		हाल CMD_PUBREC:
			metrics__int_inc(mosq_गणक_mqtt_pubrec_received, 1);
			rc = handle__pubrec(context);
			अवरोध;
		हाल CMD_PUBREL:
			metrics__int_inc(mosq_गणक_mqtt_pubrel_received, 1);
			rc = handle__pubrel(context);
			अवरोध;
		हाल CMD_CONNECT:
			metrics__int_inc(mosq_गणक_mqtt_connect_received, 1);
			वापस handle__connect(context);
		हाल CMD_DISCONNECT:
			metrics__int_inc(mosq_गणक_mqtt_disconnect_received, 1);
			rc = handle__disconnect(context);
			अवरोध;
		हाल CMD_SUBSCRIBE:
			metrics__int_inc(mosq_गणक_mqtt_subscribe_received, 1);
			rc = handle__subscribe(context);
			अवरोध;
		हाल CMD_UNSUBSCRIBE:
			metrics__int_inc(mosq_गणक_mqtt_unsubscribe_received, 1);
			rc = handle__unsubscribe(context);
			अवरोध;
#अगर_घोषित WITH_BRIDGE
		हाल CMD_CONNACK:
			metrics__int_inc(mosq_गणक_mqtt_connack_received, 1);
			rc = handle__connack(context);
			अवरोध;
		हाल CMD_SUBACK:
			metrics__int_inc(mosq_गणक_mqtt_suback_received, 1);
			rc = handle__suback(context);
			अवरोध;
		हाल CMD_UNSUBACK:
			metrics__int_inc(mosq_गणक_mqtt_unsuback_received, 1);
			rc = handle__unsuback(context);
			अवरोध;
#पूर्ण_अगर
		हाल CMD_AUTH:
			metrics__int_inc(mosq_गणक_mqtt_auth_received, 1);
			rc = handle__auth(context);
			अवरोध;
		शेष:
			rc = MOSQ_ERR_PROTOCOL;
	}

	अगर(context->protocol == mosq_p_mqtt5){
		अगर(rc == MOSQ_ERR_PROTOCOL || rc == MOSQ_ERR_DUPLICATE_PROPERTY){
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, शून्य);
		}अन्यथा अगर(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__disconnect(context, MQTT_RC_MALFORMED_PACKET, शून्य);
		}अन्यथा अगर(rc == MOSQ_ERR_QOS_NOT_SUPPORTED){
			send__disconnect(context, MQTT_RC_QOS_NOT_SUPPORTED, शून्य);
		}अन्यथा अगर(rc == MOSQ_ERR_RETAIN_NOT_SUPPORTED){
			send__disconnect(context, MQTT_RC_RETAIN_NOT_SUPPORTED, शून्य);
		}अन्यथा अगर(rc == MOSQ_ERR_TOPIC_ALIAS_INVALID){
			send__disconnect(context, MQTT_RC_TOPIC_ALIAS_INVALID, शून्य);
		}अन्यथा अगर(rc == MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED){
			send__disconnect(context, MQTT_RC_RECEIVE_MAXIMUM_EXCEEDED, शून्य);
		}अन्यथा अगर(rc == MOSQ_ERR_UNKNOWN || rc == MOSQ_ERR_NOMEM){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, शून्य);
		}
	}
	वापस rc;
}
