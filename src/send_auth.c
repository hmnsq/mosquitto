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

#ضمن "config.h"

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "packet_mosq.h"
#ضمن "property_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"


صحيح send__auth(بنية mosquitto *context, uint8_t reason_کوډ, ثابت فراغ *auth_معلومات, uint16_t auth_معلومات_len)
{
	بنية mosquitto__packet *packet = عدم;
	صحيح rc;
	mosquitto_property *properties = عدم;
	uint32_t پاتې_شوی_اوږدتیا;

	إذا(context->auth_لاره == عدم){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(context->protocol != mosq_p_mqtt5){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Sending AUTH packet when session not MQTT v5.0.", context->id);
		أرجع MOSQ_ERR_PROTOCOL;
	}

	log__printf(عدم, MOSQ_LOG_DEBUG, "Sending AUTH to %s (rc%d, %s)", context->id, reason_کوډ, context->auth_لاره);

	پاتې_شوی_اوږدتیا = 1;

	rc = mosquitto_property_جمع_کړه_string(&properties, MQTT_PROP_AUTHENTICATION_لاره, context->auth_لاره);
	إذا(rc){
		اذهب error;
	}

	إذا(auth_معلومات != عدم && auth_معلومات_len > 0){
		rc = mosquitto_property_جمع_کړه_دوګونی(&properties, MQTT_PROP_AUTHENTICATION_معلومات, auth_معلومات, auth_معلومات_len);
		إذا(rc){
			اذهب error;
		}
	}

	پاتې_شوی_اوږدتیا += mosquitto_property_get_پاتې_شوی_اوږدتیا(properties);

	rc = packet__check_oversize(context, پاتې_شوی_اوږدتیا);
	إذا(rc){
		اذهب error;
	}

	rc = packet__alloc(&packet, CMD_AUTH, پاتې_شوی_اوږدتیا);
	إذا(rc){
		اذهب error;
	}

	packet__write_byte(packet, reason_کوډ);
	property__write_all(packet, properties, true);
	mosquitto_property_free_all(&properties);

	metrics__int_inc(mosq_counter_mqtt_auth_لېږل_شوی, 1);
	أرجع packet_کتار(context, packet);
error:
	mosquitto_property_free_all(&properties);
	أرجع rc;
}
