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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "packet_mosq.h"
#அந்தர்பாவ "read_handle.h"
#அந்தர்பாவ "send_mosq.h"
#அந்தர்பாவ "sys_tree.h"
#அந்தர்பாவ "util_mosq.h"


முழு handle__packet(கட்டமைப்பு mosquitto *context)
{
	முழு rc = MOSQ_ERR_INVAL;

	எனில்(!context){
		திரும்பு MOSQ_ERR_INVAL;
	}

	தேர்வு((context->in_packet.command)&0xF0){
		நிலை CMD_PINGREQ:
			metrics__int_inc(mosq_counter_mqtt_pingreq_received, 1);
			rc = handle__pingreq(context);
			நிறுத்து;
		நிலை CMD_PINGRESP:
			metrics__int_inc(mosq_counter_mqtt_pingresp_received, 1);
			rc = handle__pingresp(context);
			நிறுத்து;
		நிலை CMD_PUBACK:
			metrics__int_inc(mosq_counter_mqtt_puback_received, 1);
			rc = handle__pubackcomp(context, "PUBACK");
			நிறுத்து;
		நிலை CMD_PUBCOMP:
			metrics__int_inc(mosq_counter_mqtt_pubcomp_received, 1);
			rc = handle__pubackcomp(context, "PUBCOMP");
			நிறுத்து;
		நிலை CMD_PUBLISH:
			metrics__int_inc(mosq_counter_mqtt_publish_received, 1);
			rc = handle__publish(context);
			நிறுத்து;
		நிலை CMD_PUBREC:
			metrics__int_inc(mosq_counter_mqtt_pubrec_received, 1);
			rc = handle__pubrec(context);
			நிறுத்து;
		நிலை CMD_PUBREL:
			metrics__int_inc(mosq_counter_mqtt_pubrel_received, 1);
			rc = handle__pubrel(context);
			நிறுத்து;
		நிலை CMD_CONNECT:
			metrics__int_inc(mosq_counter_mqtt_connect_received, 1);
			திரும்பு handle__connect(context);
		நிலை CMD_DISCONNECT:
			metrics__int_inc(mosq_counter_mqtt_disconnect_received, 1);
			rc = handle__disconnect(context);
			நிறுத்து;
		நிலை CMD_SUBSCRIBE:
			metrics__int_inc(mosq_counter_mqtt_subscribe_received, 1);
			rc = handle__subscribe(context);
			நிறுத்து;
		நிலை CMD_UNSUBSCRIBE:
			metrics__int_inc(mosq_counter_mqtt_unsubscribe_received, 1);
			rc = handle__unsubscribe(context);
			நிறுத்து;
#ifdef WITH_BRIDGE
		நிலை CMD_CONNACK:
			metrics__int_inc(mosq_counter_mqtt_connack_received, 1);
			rc = handle__connack(context);
			நிறுத்து;
		நிலை CMD_SUBACK:
			metrics__int_inc(mosq_counter_mqtt_suback_received, 1);
			rc = handle__suback(context);
			நிறுத்து;
		நிலை CMD_UNSUBACK:
			metrics__int_inc(mosq_counter_mqtt_unsuback_received, 1);
			rc = handle__unsuback(context);
			நிறுத்து;
#endif
		நிலை CMD_AUTH:
			metrics__int_inc(mosq_counter_mqtt_auth_received, 1);
			rc = handle__auth(context);
			நிறுத்து;
		இயல்பு:
			rc = MOSQ_ERR_PROTOCOL;
	}

	எனில்(context->protocol == mosq_p_mqtt5){
		எனில்(rc == MOSQ_ERR_PROTOCOL || rc == MOSQ_ERR_DUPLICATE_PROPERTY){
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, வெற்று);
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_MALFORMED_PACKET){
			send__disconnect(context, MQTT_RC_MALFORMED_PACKET, வெற்று);
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_QOS_NOT_SUPPORTED){
			send__disconnect(context, MQTT_RC_QOS_NOT_SUPPORTED, வெற்று);
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_RETAIN_NOT_SUPPORTED){
			send__disconnect(context, MQTT_RC_RETAIN_NOT_SUPPORTED, வெற்று);
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_TOPIC_ALIAS_INVALID){
			send__disconnect(context, MQTT_RC_TOPIC_ALIAS_INVALID, வெற்று);
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED){
			send__disconnect(context, MQTT_RC_RECEIVE_MAXIMUM_EXCEEDED, வெற்று);
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_UNKNOWN || rc == MOSQ_ERR_NOMEM){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, வெற்று);
		}
	}
	திரும்பு rc;
}
