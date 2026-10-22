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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "packet_mosq.h"
#അന്തര്ഭാവ "property_mosq.h"
#അന്തര്ഭാവ "sys_tree.h"
#അന്തര്ഭാവ "util_mosq.h"


പൂർണ്ണസംഖ്യ send__auth(ഘടന mosquitto *context, uint8_t reason_code, സ്ഥിരം ശൂന്യം *auth_data, uint16_t auth_data_len)
{
	ഘടന mosquitto__packet *packet = ശൂന്യ;
	പൂർണ്ണസംഖ്യ rc;
	mosquitto_property *properties = ശൂന്യ;
	uint32_t remaining_length;

	എങ്കിൽ(context->auth_method == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}
	എങ്കിൽ(context->protocol != mosq_p_mqtt5){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: Sending AUTH packet when session not MQTT v5.0.", context->id);
		തിരികെ MOSQ_ERR_PROTOCOL;
	}

	log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "Sending AUTH to %s (rc%d, %s)", context->id, reason_code, context->auth_method);

	remaining_length = 1;

	rc = mosquitto_property_add_string(&properties, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method);
	എങ്കിൽ(rc){
		പോകുക error;
	}

	എങ്കിൽ(auth_data != ശൂന്യ && auth_data_len > 0){
		rc = mosquitto_property_add_ദ്വിമാന(&properties, MQTT_PROP_AUTHENTICATION_DATA, auth_data, auth_data_len);
		എങ്കിൽ(rc){
			പോകുക error;
		}
	}

	remaining_length += mosquitto_property_get_remaining_length(properties);

	rc = packet__check_oversize(context, remaining_length);
	എങ്കിൽ(rc){
		പോകുക error;
	}

	rc = packet__alloc(&packet, CMD_AUTH, remaining_length);
	എങ്കിൽ(rc){
		പോകുക error;
	}

	packet__write_byte(packet, reason_code);
	property__write_all(packet, properties, true);
	mosquitto_property_free_all(&properties);

	metrics__int_inc(mosq_counter_mqtt_auth_sent, 1);
	തിരികെ packet__queue(context, packet);
error:
	mosquitto_property_free_all(&properties);
	തിരികെ rc;
}
