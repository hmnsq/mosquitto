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

#समावेश "config.h"

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "packet_mosq.h"
#समावेश "property_mosq.h"
#समावेश "sys_tree.h"
#समावेश "util_mosq.h"


पूर्णांक send__auth(काष्ठा mosquitto *context, uint8_t reason_code, स्थिर व्योम *auth_तथ्य, uint16_t auth_तथ्य_len)
{
	काष्ठा mosquitto__packet *packet = शून्य;
	पूर्णांक rc;
	mosquitto_property *properties = शून्य;
	uint32_t remaining_लंबाई;

	अगर(context->auth_method == शून्य){
		वापस MOSQ_ERR_INVAL;
	}
	अगर(context->protocol != mosq_p_mqtt5){
		log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: Sending AUTH packet when session not MQTT v5.0.", context->id);
		वापस MOSQ_ERR_PROTOCOL;
	}

	log__printf(शून्य, MOSQ_LOG_DEBUG, "Sending AUTH to %s (rc%d, %s)", context->id, reason_code, context->auth_method);

	remaining_लंबाई = 1;

	rc = mosquitto_property_add_string(&properties, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method);
	अगर(rc){
		जाओ error;
	}

	अगर(auth_तथ्य != शून्य && auth_तथ्य_len > 0){
		rc = mosquitto_property_add_द्विआधारी(&properties, MQTT_PROP_AUTHENTICATION_तथ्य, auth_तथ्य, auth_तथ्य_len);
		अगर(rc){
			जाओ error;
		}
	}

	remaining_लंबाई += mosquitto_property_get_remaining_लंबाई(properties);

	rc = packet__check_oversize(context, remaining_लंबाई);
	अगर(rc){
		जाओ error;
	}

	rc = packet__alloc(&packet, CMD_AUTH, remaining_लंबाई);
	अगर(rc){
		जाओ error;
	}

	packet__write_byte(packet, reason_code);
	property__write_all(packet, properties, true);
	mosquitto_property_free_all(&properties);

	metrics__int_inc(mosq_गणक_mqtt_auth_sent, 1);
	वापस packet__queue(context, packet);
error:
	mosquitto_property_free_all(&properties);
	वापस rc;
}
