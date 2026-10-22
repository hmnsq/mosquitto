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

#అన్తర్భావ "config.h"

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "packet_mosq.h"
#అన్తర్భావ "property_mosq.h"
#అన్తర్భావ "sys_tree.h"
#అన్తర్భావ "util_mosq.h"


పూర్ణాంకం send__auth(నిర్మాణం mosquitto *context, uint8_t reason_సంకేతం, స్థిరరాశి శూన్య *auth_దత్తాంశం, uint16_t auth_దత్తాంశం_len)
{
	నిర్మాణం mosquitto__packet *packet = ఖాళీ;
	పూర్ణాంకం rc;
	mosquitto_property *properties = ఖాళీ;
	uint32_t మిగిలిన_నిడివి;

	ఒకవేళ(context->auth_పద్ధతి == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}
	ఒకవేళ(context->protocol != mosq_p_mqtt5){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: Sending AUTH packet when session not MQTT v5.0.", context->id);
		తిరిగి MOSQ_ERR_PROTOCOL;
	}

	log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "Sending AUTH to %s (rc%d, %s)", context->id, reason_సంకేతం, context->auth_పద్ధతి);

	మిగిలిన_నిడివి = 1;

	rc = mosquitto_property_కూడు_string(&properties, MQTT_PROP_AUTHENTICATION_పద్ధతి, context->auth_పద్ధతి);
	ఒకవేళ(rc){
		వెళ్ళు error;
	}

	ఒకవేళ(auth_దత్తాంశం != ఖాళీ && auth_దత్తాంశం_len > 0){
		rc = mosquitto_property_కూడు_ద్విమాన(&properties, MQTT_PROP_AUTHENTICATION_దత్తాంశం, auth_దత్తాంశం, auth_దత్తాంశం_len);
		ఒకవేళ(rc){
			వెళ్ళు error;
		}
	}

	మిగిలిన_నిడివి += mosquitto_property_get_మిగిలిన_నిడివి(properties);

	rc = packet__check_oversize(context, మిగిలిన_నిడివి);
	ఒకవేళ(rc){
		వెళ్ళు error;
	}

	rc = packet__alloc(&packet, CMD_AUTH, మిగిలిన_నిడివి);
	ఒకవేళ(rc){
		వెళ్ళు error;
	}

	packet__write_byte(packet, reason_సంకేతం);
	property__write_all(packet, properties, true);
	mosquitto_property_free_all(&properties);

	metrics__int_inc(mosq_counter_mqtt_auth_పంపిన, 1);
	తిరిగి packet_వరుస(context, packet);
error:
	mosquitto_property_free_all(&properties);
	తిరిగి rc;
}
