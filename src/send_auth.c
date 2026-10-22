/*
Copyright (c) 2019-2021 Roger Light <roger@atchoo.org>

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

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "property_mosq.h"
#अन्तर्भाव "sys_tree.h"
#अन्तर्भाव "util_mosq.h"


पूर्णांक send__auth(संरचना mosquitto *context, uint8_t reason_code, स्थिरांक शून्य *auth_data, uint16_t auth_data_len)
{
	संरचना mosquitto__packet *packet = रिक्त;
	पूर्णांक rc;
	mosquitto_property *properties = रिक्त;
	uint32_t remaining_length;

	जर(context->auth_method == रिक्त){
		परत MOSQ_ERR_INVAL;
	}
	जर(context->protocol != mosq_p_mqtt5){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: Sending AUTH packet when session not MQTT v5.0.", context->id);
		परत MOSQ_ERR_PROTOCOL;
	}

	log__printf(रिक्त, MOSQ_LOG_DEBUG, "Sending AUTH to %s (rc%d, %s)", context->id, reason_code, context->auth_method);

	remaining_length = 1;

	rc = mosquitto_property_add_string(&properties, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method);
	जर(rc){
		जा error;
	}

	जर(auth_data != रिक्त && auth_data_len > 0){
		rc = mosquitto_property_add_द्विमान(&properties, MQTT_PROP_AUTHENTICATION_DATA, auth_data, auth_data_len);
		जर(rc){
			जा error;
		}
	}

	remaining_length += mosquitto_property_get_remaining_length(properties);

	rc = packet__check_oversize(context, remaining_length);
	जर(rc){
		जा error;
	}

	rc = packet__alloc(&packet, CMD_AUTH, remaining_length);
	जर(rc){
		जा error;
	}

	packet__write_byte(packet, reason_code);
	property__write_all(packet, properties, true);
	mosquitto_property_free_all(&properties);

	metrics__int_inc(mosq_counter_mqtt_auth_sent, 1);
	परत packet__queue(context, packet);
error:
	mosquitto_property_free_all(&properties);
	परत rc;
}
