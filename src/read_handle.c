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

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "read_handle.h"
#अन्तर्भाव "send_mosq.h"
#अन्तर्भाव "sys_tree.h"
#अन्तर्भाव "util_mosq.h"


पूर्णांक handle__packet(संरचना mosquitto *context)
{
	पूर्णांक rc = MOSQ_ERR_INVAL;

	जर(!context){
		परत MOSQ_ERR_INVAL;
	}

	निवड((context->in_packet.command)&0xF0){
		स्थिती CMD_PINGREQ:
			metrics__int_inc(mosq_counter_mqtt_pingreq_received, 1);
			rc = handle__pingreq(context);
			थांबा;
		स्थिती CMD_PINGRESP:
			metrics__int_inc(mosq_counter_mqtt_pingresp_received, 1);
			rc = handle__pingresp(context);
			थांबा;
		स्थिती CMD_PUBACK:
			metrics__int_inc(mosq_counter_mqtt_puback_received, 1);
			rc = handle__pubackcomp(context, "PUBACK");
			थांबा;
		स्थिती CMD_PUBCOMP:
			metrics__int_inc(mosq_counter_mqtt_pubcomp_received, 1);
			rc = handle__pubackcomp(context, "PUBCOMP");
			थांबा;
		स्थिती CMD_PUBLISH:
			metrics__int_inc(mosq_counter_mqtt_publish_received, 1);
			rc = handle__publish(context);
			थांबा;
		स्थिती CMD_PUBREC:
			metrics__int_inc(mosq_counter_mqtt_pubrec_received, 1);
			rc = handle__pubrec(context);
			थांबा;
		स्थिती CMD_PUBREL:
			metrics__int_inc(mosq_counter_mqtt_pubrel_received, 1);
			rc = handle__pubrel(context);
			थांबा;
		स्थिती CMD_CONNECT:
			metrics__int_inc(mosq_counter_mqtt_connect_received, 1);
			परत handle__connect(context);
		स्थिती CMD_DISCONNECT:
			metrics__int_inc(mosq_counter_mqtt_disconnect_received, 1);
			rc = handle__disconnect(context);
			थांबा;
		स्थिती CMD_SUBSCRIBE:
			metrics__int_inc(mosq_counter_mqtt_subscribe_received, 1);
			rc = handle__subscribe(context);
			थांबा;
		स्थिती CMD_UNSUBSCRIBE:
			metrics__int_inc(mosq_counter_mqtt_unsubscribe_received, 1);
			rc = handle__unsubscribe(context);
			थांबा;
#ifdef WITH_BRIDGE
		स्थिती CMD_CONNACK:
			metrics__int_inc(mosq_counter_mqtt_connack_received, 1);
			rc = handle__connack(context);
			थांबा;
		स्थिती CMD_SUBACK:
			metrics__int_inc(mosq_counter_mqtt_suback_received, 1);
			rc = handle__suback(context);
			थांबा;
		स्थिती CMD_UNSUBACK:
			metrics__int_inc(mosq_counter_mqtt_unsuback_received, 1);
			rc = handle__unsuback(context);
			थांबा;
#endif
		स्थिती CMD_AUTH:
			metrics__int_inc(mosq_counter_mqtt_auth_received, 1);
			rc = handle__auth(context);
			थांबा;
		पूर्वनिर्धारित:
			rc = MOSQ_ERR_PROTOCOL;
	}

	जर(context->protocol == mosq_p_mqtt5){
		जर(rc == MOSQ_ERR_PROTOCOL || rc == MOSQ_ERR_DUPLICATE_PROPERTY){
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, रिक्त);
		}अन्यथा जर(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__disconnect(context, MQTT_RC_MALFORMED_PACKET, रिक्त);
		}अन्यथा जर(rc == MOSQ_ERR_QOS_NOT_SUPPORTED){
			send__disconnect(context, MQTT_RC_QOS_NOT_SUPPORTED, रिक्त);
		}अन्यथा जर(rc == MOSQ_ERR_RETAIN_NOT_SUPPORTED){
			send__disconnect(context, MQTT_RC_RETAIN_NOT_SUPPORTED, रिक्त);
		}अन्यथा जर(rc == MOSQ_ERR_TOPIC_ALIAS_INVALID){
			send__disconnect(context, MQTT_RC_TOPIC_ALIAS_INVALID, रिक्त);
		}अन्यथा जर(rc == MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED){
			send__disconnect(context, MQTT_RC_RECEIVE_MAXIMUM_EXCEEDED, रिक्त);
		}अन्यथा जर(rc == MOSQ_ERR_UNKNOWN || rc == MOSQ_ERR_NOMEM){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, रिक्त);
		}
	}
	परत rc;
}
