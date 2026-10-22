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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "property_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "send_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "will_mosq.h"


ਪੂਰਨ handle__auth(ਬਣਤਰ mosquitto *context)
{
	ਪੂਰਨ rc = 0;
	uint8_t reason_ਕੋਡ = 0;
	mosquitto_property *properties = ਨਿਰਾ;
	ਅੱਖਰ *auth_ਵਿਧੀ = ਨਿਰਾ;
	ਖਾਲੀ *auth_ਅੰਕੜੇ = ਨਿਰਾ;
	uint16_t auth_ਅੰਕੜੇ_len = 0;
	ਖਾਲੀ *auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ = ਨਿਰਾ;
	uint16_t auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ_len = 0;

	ਜੇ(!context){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਜੇ(context->protocol != mosq_p_mqtt5){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet when session not MQTT v5.0.", context->id);
		ਵਾਪਸ MOSQ_ERR_PROTOCOL;
	}
	ਜੇ(context->auth_ਵਿਧੀ == ਨਿਰਾ){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without existing auth-method.", context->id);
		ਵਾਪਸ MOSQ_ERR_PROTOCOL;
	}
	ਜੇ(context->ਵਿੱਚ_packet.command != CMD_AUTH){
		ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
	}

	ਜੇ(context->ਵਿੱਚ_packet.ਬਚਿਆ_ਲੰਮਾਈ > 0){
		ਜੇ(packet__read_byte(&context->ਵਿੱਚ_packet, &reason_ਕੋਡ)){
			ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
		}
		ਜੇ(reason_ਕੋਡ != MQTT_RC_CONTINUE_AUTHENTICATION
				&& reason_ਕੋਡ != MQTT_RC_REAUTHENTICATE){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ਨਿਰਾ);
			log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with reason-code = %d.",
					context->id, reason_ਕੋਡ);
			ਵਾਪਸ MOSQ_ERR_PROTOCOL;
		}

		ਜੇ((reason_ਕੋਡ == MQTT_RC_REAUTHENTICATE && context->state != mosq_cs_ਸਰਗਰਮ)
				|| (reason_ਕੋਡ == MQTT_RC_CONTINUE_AUTHENTICATION
				&& context->state != mosq_cs_authenticating && context->state != mosq_cs_reauthenticating)){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ਨਿਰਾ);
			log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH reauthentication packet before session is active,",
					context->id);
			log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "     or attempted to continue authentication when no authentication in progress.");
			ਵਾਪਸ MOSQ_ERR_PROTOCOL;
		}

		rc = property__read_all(CMD_AUTH, &context->ਵਿੱਚ_packet, &properties);
		ਜੇ(rc){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, ਨਿਰਾ);
			ਵਾਪਸ rc;
		}


		ਜੇ(mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_ਵਿਧੀ, &auth_ਵਿਧੀ, false) == ਨਿਰਾ){
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_UNSPECIFIED, ਨਿਰਾ);
			log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without auth-method property.",
					context->id);
			ਵਾਪਸ MOSQ_ERR_PROTOCOL;
		}

		ਜੇ(!auth_ਵਿਧੀ || strcmp(auth_ਵਿਧੀ, context->auth_ਵਿਧੀ)){
			/* No method, or non-matching method */
			mosquitto_FREE(auth_ਵਿਧੀ);
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ਨਿਰਾ);
			log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with non-matching auth-method property (%s:%s).",
					context->id, context->auth_ਵਿਧੀ, auth_ਵਿਧੀ);
			ਵਾਪਸ MOSQ_ERR_PROTOCOL;
		}
		mosquitto_FREE(auth_ਵਿਧੀ);

		mosquitto_property_read_ਦੁਆਧਾਰੀ(properties, MQTT_PROP_AUTHENTICATION_ਅੰਕੜੇ, &auth_ਅੰਕੜੇ, &auth_ਅੰਕੜੇ_len, false);

		mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */
	}

	log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Received AUTH from %s (rc%d, %s)", context->id, reason_ਕੋਡ, context->auth_ਵਿਧੀ);


	ਜੇ(reason_ਕੋਡ == MQTT_RC_REAUTHENTICATE){
		/* This is a re-authentication attempt */
		mosquitto_ਸੈੱਟ_state(context, mosq_cs_reauthenticating);
		rc = mosquitto_security_auth_ਸ਼ੁਰੂ(context, true, auth_ਅੰਕੜੇ, auth_ਅੰਕੜੇ_len, &auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ, &auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ_len);
	}ਨਹੀਂਤਾਂ{
		ਜੇ(context->state != mosq_cs_reauthenticating){
			mosquitto_ਸੈੱਟ_state(context, mosq_cs_authenticating);
		}
		rc = mosquitto_security_auth_continue(context, auth_ਅੰਕੜੇ, auth_ਅੰਕੜੇ_len, &auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ, &auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ_len);
	}
	mosquitto_FREE(auth_ਅੰਕੜੇ);
	ਜੇ(rc == MOSQ_ERR_SUCCESS){
		ਜੇ(context->state == mosq_cs_authenticating){
			ਵਾਪਸ connect__on_authorised(context, auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ, auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ_len);
		}ਨਹੀਂਤਾਂ{
			mosquitto_ਸੈੱਟ_state(context, mosq_cs_ਸਰਗਰਮ);
			rc = send__auth(context, MQTT_RC_SUCCESS, auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ, auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ_len);
			ਸੁਰੱਖਿਅਤ_FREE(auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ);
			ਵਾਪਸ rc;
		}
	}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_AUTH_CONTINUE){
		rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ, auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ_len);
		ਸੁਰੱਖਿਅਤ_FREE(auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ);
		ਵਾਪਸ rc;
	}ਨਹੀਂਤਾਂ{
		ਸੁਰੱਖਿਅਤ_FREE(auth_ਅੰਕੜੇ_ਬਾਹਰਲਾ);
		ਜੇ(context->state == mosq_cs_authenticating && context->will){
			/* Free will without sending if this is our first authentication attempt */
			will__clear(context);
		}
		ਜੇ(rc == MOSQ_ERR_AUTH){
			ਜੇ(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ਨਿਰਾ);
				mosquitto_FREE(context->id);
			}ਨਹੀਂਤਾਂ{
				send__disconnect(context, MQTT_RC_NOT_AUTHORIZED, ਨਿਰਾ);
			}
			ਵਾਪਸ MOSQ_ERR_PROTOCOL;
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_NOT_SUPPORTED){
			/* Client has requested extended authentication, but we don't support it. */
			ਜੇ(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_ਵਿਧੀ, ਨਿਰਾ);
				mosquitto_FREE(context->id);
			}ਨਹੀਂਤਾਂ{
				send__disconnect(context, MQTT_RC_BAD_AUTHENTICATION_ਵਿਧੀ, ਨਿਰਾ);
			}
			ਵਾਪਸ MOSQ_ERR_PROTOCOL;
		}ਨਹੀਂਤਾਂ{
			ਜੇ(context->state == mosq_cs_authenticating){
				mosquitto_FREE(context->id);
			}
			ਵਾਪਸ rc;
		}
	}
}
