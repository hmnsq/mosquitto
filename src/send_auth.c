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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "packet_mosq.h"
#અન્તર્ભાવ "property_mosq.h"
#અન્તર્ભાવ "sys_tree.h"
#અન્તર્ભાવ "util_mosq.h"


પૂર્ણાંક send__auth(સંરચના mosquitto *context, uint8_t reason_code, અચલ શૂન્ય *auth_data, uint16_t auth_data_len)
{
	સંરચના mosquitto__packet *packet = ખાલી;
	પૂર્ણાંક rc;
	mosquitto_property *properties = ખાલી;
	uint32_t remaining_length;

	જો(context->auth_method == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}
	જો(context->protocol != mosq_p_mqtt5){
		log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: Sending AUTH packet when session not MQTT v5.0.", context->id);
		પાછા MOSQ_ERR_PROTOCOL;
	}

	log__printf(ખાલી, MOSQ_LOG_DEBUG, "Sending AUTH to %s (rc%d, %s)", context->id, reason_code, context->auth_method);

	remaining_length = 1;

	rc = mosquitto_property_add_string(&properties, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method);
	જો(rc){
		જાઓ error;
	}

	જો(auth_data != ખાલી && auth_data_len > 0){
		rc = mosquitto_property_add_દ્વિઅંકી(&properties, MQTT_PROP_AUTHENTICATION_DATA, auth_data, auth_data_len);
		જો(rc){
			જાઓ error;
		}
	}

	remaining_length += mosquitto_property_get_remaining_length(properties);

	rc = packet__check_oversize(context, remaining_length);
	જો(rc){
		જાઓ error;
	}

	rc = packet__alloc(&packet, CMD_AUTH, remaining_length);
	જો(rc){
		જાઓ error;
	}

	packet__write_byte(packet, reason_code);
	property__write_all(packet, properties, true);
	mosquitto_property_free_all(&properties);

	metrics__int_inc(mosq_counter_mqtt_auth_sent, 1);
	પાછા packet__queue(context, packet);
error:
	mosquitto_property_free_all(&properties);
	પાછા rc;
}
