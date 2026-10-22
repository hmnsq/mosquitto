/*
Copyright (c) 2018-2021 Roger Light <roger@atchoo.org>

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

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "packet_mosq.h"
#అన్తర్భావ "property_mosq.h"
#అన్తర్భావ "send_mosq.h"
#అన్తర్భావ "util_mosq.h"
#అన్తర్భావ "will_mosq.h"


పూర్ణాంకం handle__auth(నిర్మాణం mosquitto *context)
{
	పూర్ణాంకం rc = 0;
	uint8_t reason_సంకేతం = 0;
	mosquitto_property *properties = ఖాళీ;
	అక్షరం *auth_పద్ధతి = ఖాళీ;
	శూన్య *auth_దత్తాంశం = ఖాళీ;
	uint16_t auth_దత్తాంశం_len = 0;
	శూన్య *auth_దత్తాంశం_బహిర్గతం = ఖాళీ;
	uint16_t auth_దత్తాంశం_బహిర్గతం_len = 0;

	ఒకవేళ(!context){
		తిరిగి MOSQ_ERR_INVAL;
	}

	ఒకవేళ(context->protocol != mosq_p_mqtt5){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet when session not MQTT v5.0.", context->id);
		తిరిగి MOSQ_ERR_PROTOCOL;
	}
	ఒకవేళ(context->auth_పద్ధతి == ఖాళీ){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without existing auth-method.", context->id);
		తిరిగి MOSQ_ERR_PROTOCOL;
	}
	ఒకవేళ(context->లోపల_packet.command != CMD_AUTH){
		తిరిగి MOSQ_ERR_MALFORMED_PACKET;
	}

	ఒకవేళ(context->లోపల_packet.మిగిలిన_నిడివి > 0){
		ఒకవేళ(packet__read_byte(&context->లోపల_packet, &reason_సంకేతం)){
			తిరిగి MOSQ_ERR_MALFORMED_PACKET;
		}
		ఒకవేళ(reason_సంకేతం != MQTT_RC_CONTINUE_AUTHENTICATION
				&& reason_సంకేతం != MQTT_RC_REAUTHENTICATE){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ఖాళీ);
			log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with reason-code = %d.",
					context->id, reason_సంకేతం);
			తిరిగి MOSQ_ERR_PROTOCOL;
		}

		ఒకవేళ((reason_సంకేతం == MQTT_RC_REAUTHENTICATE && context->state != mosq_cs_చురుకైన)
				|| (reason_సంకేతం == MQTT_RC_CONTINUE_AUTHENTICATION
				&& context->state != mosq_cs_authenticating && context->state != mosq_cs_reauthenticating)){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ఖాళీ);
			log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH reauthentication packet before session is active,",
					context->id);
			log__printf(ఖాళీ, MOSQ_LOG_INFO, "     or attempted to continue authentication when no authentication in progress.");
			తిరిగి MOSQ_ERR_PROTOCOL;
		}

		rc = property__read_all(CMD_AUTH, &context->లోపల_packet, &properties);
		ఒకవేళ(rc){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, ఖాళీ);
			తిరిగి rc;
		}


		ఒకవేళ(mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_పద్ధతి, &auth_పద్ధతి, false) == ఖాళీ){
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_UNSPECIFIED, ఖాళీ);
			log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without auth-method property.",
					context->id);
			తిరిగి MOSQ_ERR_PROTOCOL;
		}

		ఒకవేళ(!auth_పద్ధతి || strcmp(auth_పద్ధతి, context->auth_పద్ధతి)){
			/* No method, or non-matching method */
			mosquitto_FREE(auth_పద్ధతి);
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ఖాళీ);
			log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with non-matching auth-method property (%s:%s).",
					context->id, context->auth_పద్ధతి, auth_పద్ధతి);
			తిరిగి MOSQ_ERR_PROTOCOL;
		}
		mosquitto_FREE(auth_పద్ధతి);

		mosquitto_property_read_ద్విమాన(properties, MQTT_PROP_AUTHENTICATION_దత్తాంశం, &auth_దత్తాంశం, &auth_దత్తాంశం_len, false);

		mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */
	}

	log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "Received AUTH from %s (rc%d, %s)", context->id, reason_సంకేతం, context->auth_పద్ధతి);


	ఒకవేళ(reason_సంకేతం == MQTT_RC_REAUTHENTICATE){
		/* This is a re-authentication attempt */
		mosquitto_సమితి_state(context, mosq_cs_reauthenticating);
		rc = mosquitto_security_auth_ప్రారంభం(context, true, auth_దత్తాంశం, auth_దత్తాంశం_len, &auth_దత్తాంశం_బహిర్గతం, &auth_దత్తాంశం_బహిర్గతం_len);
	}లేకపోతే{
		ఒకవేళ(context->state != mosq_cs_reauthenticating){
			mosquitto_సమితి_state(context, mosq_cs_authenticating);
		}
		rc = mosquitto_security_auth_continue(context, auth_దత్తాంశం, auth_దత్తాంశం_len, &auth_దత్తాంశం_బహిర్గతం, &auth_దత్తాంశం_బహిర్గతం_len);
	}
	mosquitto_FREE(auth_దత్తాంశం);
	ఒకవేళ(rc == MOSQ_ERR_SUCCESS){
		ఒకవేళ(context->state == mosq_cs_authenticating){
			తిరిగి connect__on_authorised(context, auth_దత్తాంశం_బహిర్గతం, auth_దత్తాంశం_బహిర్గతం_len);
		}లేకపోతే{
			mosquitto_సమితి_state(context, mosq_cs_చురుకైన);
			rc = send__auth(context, MQTT_RC_SUCCESS, auth_దత్తాంశం_బహిర్గతం, auth_దత్తాంశం_బహిర్గతం_len);
			సురక్షిత_FREE(auth_దత్తాంశం_బహిర్గతం);
			తిరిగి rc;
		}
	}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_AUTH_CONTINUE){
		rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_దత్తాంశం_బహిర్గతం, auth_దత్తాంశం_బహిర్గతం_len);
		సురక్షిత_FREE(auth_దత్తాంశం_బహిర్గతం);
		తిరిగి rc;
	}లేకపోతే{
		సురక్షిత_FREE(auth_దత్తాంశం_బహిర్గతం);
		ఒకవేళ(context->state == mosq_cs_authenticating && context->will){
			/* Free will without sending if this is our first authentication attempt */
			will__clear(context);
		}
		ఒకవేళ(rc == MOSQ_ERR_AUTH){
			ఒకవేళ(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ఖాళీ);
				mosquitto_FREE(context->id);
			}లేకపోతే{
				send__disconnect(context, MQTT_RC_NOT_AUTHORIZED, ఖాళీ);
			}
			తిరిగి MOSQ_ERR_PROTOCOL;
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_NOT_SUPPORTED){
			/* Client has requested extended authentication, but we don't support it. */
			ఒకవేళ(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_పద్ధతి, ఖాళీ);
				mosquitto_FREE(context->id);
			}లేకపోతే{
				send__disconnect(context, MQTT_RC_BAD_AUTHENTICATION_పద్ధతి, ఖాళీ);
			}
			తిరిగి MOSQ_ERR_PROTOCOL;
		}లేకపోతే{
			ఒకవేళ(context->state == mosq_cs_authenticating){
				mosquitto_FREE(context->id);
			}
			తిరిగి rc;
		}
	}
}
