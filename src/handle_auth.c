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

#شامل "config.h"

#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "packet_mosq.h"
#شامل "property_mosq.h"
#شامل "send_mosq.h"
#شامل "util_mosq.h"
#شامل "will_mosq.h"


صحیح handle__auth(ساخت mosquitto *context)
{
	صحیح rc = 0;
	uint8_t reason_رمز = 0;
	mosquitto_property *properties = تہی;
	حرف *auth_طریقہ = تہی;
	خالی *auth_ڈیٹا = تہی;
	uint16_t auth_ڈیٹا_len = 0;
	خالی *auth_ڈیٹا_برآمد = تہی;
	uint16_t auth_ڈیٹا_برآمد_len = 0;

	اگر(!context){
		واپس MOSQ_ERR_INVAL;
	}

	اگر(context->protocol != mosq_p_mqtt5){
		log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet when session not MQTT v5.0.", context->id);
		واپس MOSQ_ERR_PROTOCOL;
	}
	اگر(context->auth_طریقہ == تہی){
		log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without existing auth-method.", context->id);
		واپس MOSQ_ERR_PROTOCOL;
	}
	اگر(context->اندر_packet.command != CMD_AUTH){
		واپس MOSQ_ERR_MALFORMED_PACKET;
	}

	اگر(context->اندر_packet.بقایا_لمبائی > 0){
		اگر(packet__read_byte(&context->اندر_packet, &reason_رمز)){
			واپس MOSQ_ERR_MALFORMED_PACKET;
		}
		اگر(reason_رمز != MQTT_RC_CONTINUE_AUTHENTICATION
				&& reason_رمز != MQTT_RC_REAUTHENTICATE){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, تہی);
			log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with reason-code = %d.",
					context->id, reason_رمز);
			واپس MOSQ_ERR_PROTOCOL;
		}

		اگر((reason_رمز == MQTT_RC_REAUTHENTICATE && context->state != mosq_cs_فعال)
				|| (reason_رمز == MQTT_RC_CONTINUE_AUTHENTICATION
				&& context->state != mosq_cs_authenticating && context->state != mosq_cs_reauthenticating)){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, تہی);
			log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: AUTH reauthentication packet before session is active,",
					context->id);
			log__printf(تہی, MOSQ_LOG_INFO, "     or attempted to continue authentication when no authentication in progress.");
			واپس MOSQ_ERR_PROTOCOL;
		}

		rc = property__read_all(CMD_AUTH, &context->اندر_packet, &properties);
		اگر(rc){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, تہی);
			واپس rc;
		}


		اگر(mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_طریقہ, &auth_طریقہ, false) == تہی){
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_UNSPECIFIED, تہی);
			log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without auth-method property.",
					context->id);
			واپس MOSQ_ERR_PROTOCOL;
		}

		اگر(!auth_طریقہ || strcmp(auth_طریقہ, context->auth_طریقہ)){
			/* No method, or non-matching method */
			mosquitto_FREE(auth_طریقہ);
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, تہی);
			log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with non-matching auth-method property (%s:%s).",
					context->id, context->auth_طریقہ, auth_طریقہ);
			واپس MOSQ_ERR_PROTOCOL;
		}
		mosquitto_FREE(auth_طریقہ);

		mosquitto_property_read_ثنائی(properties, MQTT_PROP_AUTHENTICATION_ڈیٹا, &auth_ڈیٹا, &auth_ڈیٹا_len, false);

		mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */
	}

	log__printf(تہی, MOSQ_LOG_DEBUG, "Received AUTH from %s (rc%d, %s)", context->id, reason_رمز, context->auth_طریقہ);


	اگر(reason_رمز == MQTT_RC_REAUTHENTICATE){
		/* This is a re-authentication attempt */
		mosquitto_سیٹ_state(context, mosq_cs_reauthenticating);
		rc = mosquitto_security_auth_آغاز(context, true, auth_ڈیٹا, auth_ڈیٹا_len, &auth_ڈیٹا_برآمد, &auth_ڈیٹا_برآمد_len);
	}ورنہ{
		اگر(context->state != mosq_cs_reauthenticating){
			mosquitto_سیٹ_state(context, mosq_cs_authenticating);
		}
		rc = mosquitto_security_auth_continue(context, auth_ڈیٹا, auth_ڈیٹا_len, &auth_ڈیٹا_برآمد, &auth_ڈیٹا_برآمد_len);
	}
	mosquitto_FREE(auth_ڈیٹا);
	اگر(rc == MOSQ_ERR_SUCCESS){
		اگر(context->state == mosq_cs_authenticating){
			واپس connect__on_authorised(context, auth_ڈیٹا_برآمد, auth_ڈیٹا_برآمد_len);
		}ورنہ{
			mosquitto_سیٹ_state(context, mosq_cs_فعال);
			rc = send__auth(context, MQTT_RC_SUCCESS, auth_ڈیٹا_برآمد, auth_ڈیٹا_برآمد_len);
			محفوظ_FREE(auth_ڈیٹا_برآمد);
			واپس rc;
		}
	}ورنہ اگر(rc == MOSQ_ERR_AUTH_CONTINUE){
		rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_ڈیٹا_برآمد, auth_ڈیٹا_برآمد_len);
		محفوظ_FREE(auth_ڈیٹا_برآمد);
		واپس rc;
	}ورنہ{
		محفوظ_FREE(auth_ڈیٹا_برآمد);
		اگر(context->state == mosq_cs_authenticating && context->will){
			/* Free will without sending if this is our first authentication attempt */
			will__clear(context);
		}
		اگر(rc == MOSQ_ERR_AUTH){
			اگر(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, تہی);
				mosquitto_FREE(context->id);
			}ورنہ{
				send__disconnect(context, MQTT_RC_NOT_AUTHORIZED, تہی);
			}
			واپس MOSQ_ERR_PROTOCOL;
		}ورنہ اگر(rc == MOSQ_ERR_NOT_SUPPORTED){
			/* Client has requested extended authentication, but we don't support it. */
			اگر(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_طریقہ, تہی);
				mosquitto_FREE(context->id);
			}ورنہ{
				send__disconnect(context, MQTT_RC_BAD_AUTHENTICATION_طریقہ, تہی);
			}
			واپس MOSQ_ERR_PROTOCOL;
		}ورنہ{
			اگر(context->state == mosq_cs_authenticating){
				mosquitto_FREE(context->id);
			}
			واپس rc;
		}
	}
}
