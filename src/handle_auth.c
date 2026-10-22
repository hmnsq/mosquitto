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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "property_mosq.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"
#ಅನ್ತರ್ಭಾವ "will_mosq.h"


ಪೂರ್ಣಾಂಕ handle__auth(ರಚನೆ mosquitto *context)
{
	ಪೂರ್ಣಾಂಕ rc = 0;
	uint8_t reason_ಸಂಕೇತ = 0;
	mosquitto_property *properties = ಖಾಲಿ;
	ಅಕ್ಷರ *auth_ವಿಧಾನ = ಖಾಲಿ;
	ಶೂನ್ಯ *auth_ದತ್ತಾಂಶ = ಖಾಲಿ;
	uint16_t auth_ದತ್ತಾಂಶ_len = 0;
	ಶೂನ್ಯ *auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು = ಖಾಲಿ;
	uint16_t auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len = 0;

	ಆದರೆ(!context){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	ಆದರೆ(context->protocol != mosq_p_mqtt5){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet when session not MQTT v5.0.", context->id);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
	}
	ಆದರೆ(context->auth_ವಿಧಾನ == ಖಾಲಿ){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without existing auth-method.", context->id);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
	}
	ಆದರೆ(context->ಒಳ_packet.command != CMD_AUTH){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
	}

	ಆದರೆ(context->ಒಳ_packet.ಉಳಿದ_ಉದ್ದಳತೆ > 0){
		ಆದರೆ(packet__read_byte(&context->ಒಳ_packet, &reason_ಸಂಕೇತ)){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
		}
		ಆದರೆ(reason_ಸಂಕೇತ != MQTT_RC_CONTINUE_AUTHENTICATION
				&& reason_ಸಂಕೇತ != MQTT_RC_REAUTHENTICATE){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ಖಾಲಿ);
			log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with reason-code = %d.",
					context->id, reason_ಸಂಕೇತ);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
		}

		ಆದರೆ((reason_ಸಂಕೇತ == MQTT_RC_REAUTHENTICATE && context->state != mosq_cs_ಸಕ್ರಿಯ)
				|| (reason_ಸಂಕೇತ == MQTT_RC_CONTINUE_AUTHENTICATION
				&& context->state != mosq_cs_authenticating && context->state != mosq_cs_reauthenticating)){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ಖಾಲಿ);
			log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH reauthentication packet before session is active,",
					context->id);
			log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "     or attempted to continue authentication when no authentication in progress.");
			ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
		}

		rc = property__read_all(CMD_AUTH, &context->ಒಳ_packet, &properties);
		ಆದರೆ(rc){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, ಖಾಲಿ);
			ಹಿಂತಿರುಗಿ rc;
		}


		ಆದರೆ(mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_ವಿಧಾನ, &auth_ವಿಧಾನ, false) == ಖಾಲಿ){
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_UNSPECIFIED, ಖಾಲಿ);
			log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without auth-method property.",
					context->id);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
		}

		ಆದರೆ(!auth_ವಿಧಾನ || strcmp(auth_ವಿಧಾನ, context->auth_ವಿಧಾನ)){
			/* No method, or non-matching method */
			mosquitto_FREE(auth_ವಿಧಾನ);
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ಖಾಲಿ);
			log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with non-matching auth-method property (%s:%s).",
					context->id, context->auth_ವಿಧಾನ, auth_ವಿಧಾನ);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
		}
		mosquitto_FREE(auth_ವಿಧಾನ);

		mosquitto_property_read_ದ್ವಿಮಾನ(properties, MQTT_PROP_AUTHENTICATION_ದತ್ತಾಂಶ, &auth_ದತ್ತಾಂಶ, &auth_ದತ್ತಾಂಶ_len, false);

		mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */
	}

	log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "Received AUTH from %s (rc%d, %s)", context->id, reason_ಸಂಕೇತ, context->auth_ವಿಧಾನ);


	ಆದರೆ(reason_ಸಂಕೇತ == MQTT_RC_REAUTHENTICATE){
		/* This is a re-authentication attempt */
		mosquitto_ಗಣ_state(context, mosq_cs_reauthenticating);
		rc = mosquitto_security_auth_ಆರಂಭ(context, true, auth_ದತ್ತಾಂಶ, auth_ದತ್ತಾಂಶ_len, &auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು, &auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len);
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಆದರೆ(context->state != mosq_cs_reauthenticating){
			mosquitto_ಗಣ_state(context, mosq_cs_authenticating);
		}
		rc = mosquitto_security_auth_continue(context, auth_ದತ್ತಾಂಶ, auth_ದತ್ತಾಂಶ_len, &auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು, &auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len);
	}
	mosquitto_FREE(auth_ದತ್ತಾಂಶ);
	ಆದರೆ(rc == MOSQ_ERR_SUCCESS){
		ಆದರೆ(context->state == mosq_cs_authenticating){
			ಹಿಂತಿರುಗಿ connect__on_authorised(context, auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು, auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len);
		}ಇಲ್ಲದಿದ್ದರೆ{
			mosquitto_ಗಣ_state(context, mosq_cs_ಸಕ್ರಿಯ);
			rc = send__auth(context, MQTT_RC_SUCCESS, auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು, auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len);
			ಸುರಕ್ಷಿತ_FREE(auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು);
			ಹಿಂತಿರುಗಿ rc;
		}
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_AUTH_CONTINUE){
		rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು, auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು_len);
		ಸುರಕ್ಷಿತ_FREE(auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು);
		ಹಿಂತಿರುಗಿ rc;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಸುರಕ್ಷಿತ_FREE(auth_ದತ್ತಾಂಶ_ಹೊರಹೋಗು);
		ಆದರೆ(context->state == mosq_cs_authenticating && context->will){
			/* Free will without sending if this is our first authentication attempt */
			will__clear(context);
		}
		ಆದರೆ(rc == MOSQ_ERR_AUTH){
			ಆದರೆ(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ಖಾಲಿ);
				mosquitto_FREE(context->id);
			}ಇಲ್ಲದಿದ್ದರೆ{
				send__disconnect(context, MQTT_RC_NOT_AUTHORIZED, ಖಾಲಿ);
			}
			ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_NOT_SUPPORTED){
			/* Client has requested extended authentication, but we don't support it. */
			ಆದರೆ(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_ವಿಧಾನ, ಖಾಲಿ);
				mosquitto_FREE(context->id);
			}ಇಲ್ಲದಿದ್ದರೆ{
				send__disconnect(context, MQTT_RC_BAD_AUTHENTICATION_ವಿಧಾನ, ಖಾಲಿ);
			}
			ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಆದರೆ(context->state == mosq_cs_authenticating){
				mosquitto_FREE(context->id);
			}
			ಹಿಂತಿರುಗಿ rc;
		}
	}
}
