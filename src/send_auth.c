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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "packet_mosq.h"
#ଅନ୍ତର୍ଭାଵ "property_mosq.h"
#ଅନ୍ତର୍ଭାଵ "sys_tree.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"


ପୂର୍ଣ୍ଣସଂଖ୍ୟା send__auth(ସଂରଚନା mosquitto *context, uint8_t reason_code, ଅଚଳ ଶୂନ୍ୟ *auth_data, uint16_t auth_data_len)
{
	ସଂରଚନା mosquitto__packet *packet = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	mosquitto_property *properties = ଖାଲି;
	uint32_t remaining_length;

	ଯଦି(context->auth_method == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଯଦି(context->protocol != mosq_p_mqtt5){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: Sending AUTH packet when session not MQTT v5.0.", context->id);
		ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
	}

	log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "Sending AUTH to %s (rc%d, %s)", context->id, reason_code, context->auth_method);

	remaining_length = 1;

	rc = mosquitto_property_add_string(&properties, MQTT_PROP_AUTHENTICATION_METHOD, context->auth_method);
	ଯଦି(rc){
		ଯାଅ error;
	}

	ଯଦି(auth_data != ଖାଲି && auth_data_len > 0){
		rc = mosquitto_property_add_ଦ୍ୱିମିକ(&properties, MQTT_PROP_AUTHENTICATION_DATA, auth_data, auth_data_len);
		ଯଦି(rc){
			ଯାଅ error;
		}
	}

	remaining_length += mosquitto_property_get_remaining_length(properties);

	rc = packet__check_oversize(context, remaining_length);
	ଯଦି(rc){
		ଯାଅ error;
	}

	rc = packet__alloc(&packet, CMD_AUTH, remaining_length);
	ଯଦି(rc){
		ଯାଅ error;
	}

	packet__write_byte(packet, reason_code);
	property__write_all(packet, properties, true);
	mosquitto_property_free_all(&properties);

	metrics__int_inc(mosq_counter_mqtt_auth_sent, 1);
	ଫେରନ୍ତୁ packet__queue(context, packet);
error:
	mosquitto_property_free_all(&properties);
	ଫେରନ୍ତୁ rc;
}
