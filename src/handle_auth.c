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


ಪೂರ್ಣಾಙ್ಕ handle__auth(ಸಂರಚನಾ mosquitto *context)
{
	ಪೂರ್ಣಾಙ್ಕ rc = 0;
	uint8_t reason_code = 0;
	mosquitto_property *properties = ರಿಕ್ತ;
	ವರ್ಣ *auth_method = ರಿಕ್ತ;
	ಶೂನ್ಯ *auth_data = ರಿಕ್ತ;
	uint16_t auth_data_len = 0;
	ಶೂನ್ಯ *auth_data_out = ರಿಕ್ತ;
	uint16_t auth_data_out_len = 0;

	ಯದಿ(!context){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	ಯದಿ(context->protocol != mosq_p_mqtt5){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet when session not MQTT v5.0.", context->id);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
	}
	ಯದಿ(context->auth_method == ರಿಕ್ತ){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without existing auth-method.", context->id);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
	}
	ಯದಿ(context->in_packet.command != CMD_AUTH){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
	}

	ಯದಿ(context->in_packet.remaining_length > 0){
		ಯದಿ(packet__read_byte(&context->in_packet, &reason_code)){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
		}
		ಯದಿ(reason_code != MQTT_RC_CONTINUE_AUTHENTICATION
				&& reason_code != MQTT_RC_REAUTHENTICATE){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ರಿಕ್ತ);
			log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with reason-code = %d.",
					context->id, reason_code);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
		}

		ಯದಿ((reason_code == MQTT_RC_REAUTHENTICATE && context->state != mosq_cs_active)
				|| (reason_code == MQTT_RC_CONTINUE_AUTHENTICATION
				&& context->state != mosq_cs_authenticating && context->state != mosq_cs_reauthenticating)){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ರಿಕ್ತ);
			log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH reauthentication packet before session is active,",
					context->id);
			log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "     or attempted to continue authentication when no authentication in progress.");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
		}

		rc = property__read_all(CMD_AUTH, &context->in_packet, &properties);
		ಯದಿ(rc){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, ರಿಕ್ತ);
			ಪ್ರತ್ಯಾಗಮ rc;
		}


		ಯದಿ(mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &auth_method, false) == ರಿಕ್ತ){
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_UNSPECIFIED, ರಿಕ್ತ);
			log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without auth-method property.",
					context->id);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
		}

		ಯದಿ(!auth_method || strcmp(auth_method, context->auth_method)){
			/* No method, or non-matching method */
			mosquitto_FREE(auth_method);
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ರಿಕ್ತ);
			log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with non-matching auth-method property (%s:%s).",
					context->id, context->auth_method, auth_method);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
		}
		mosquitto_FREE(auth_method);

		mosquitto_property_read_ದ್ವಿಮಾನ(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);

		mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */
	}

	log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "Received AUTH from %s (rc%d, %s)", context->id, reason_code, context->auth_method);


	ಯದಿ(reason_code == MQTT_RC_REAUTHENTICATE){
		/* This is a re-authentication attempt */
		mosquitto__set_state(context, mosq_cs_reauthenticating);
		rc = mosquitto_security_auth_start(context, true, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
	}ಅನ್ಯಥಾ{
		ಯದಿ(context->state != mosq_cs_reauthenticating){
			mosquitto__set_state(context, mosq_cs_authenticating);
		}
		rc = mosquitto_security_auth_continue(context, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
	}
	mosquitto_FREE(auth_data);
	ಯದಿ(rc == MOSQ_ERR_SUCCESS){
		ಯದಿ(context->state == mosq_cs_authenticating){
			ಪ್ರತ್ಯಾಗಮ connect__on_authorised(context, auth_data_out, auth_data_out_len);
		}ಅನ್ಯಥಾ{
			mosquitto__set_state(context, mosq_cs_active);
			rc = send__auth(context, MQTT_RC_SUCCESS, auth_data_out, auth_data_out_len);
			SAFE_FREE(auth_data_out);
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_AUTH_CONTINUE){
		rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_data_out, auth_data_out_len);
		SAFE_FREE(auth_data_out);
		ಪ್ರತ್ಯಾಗಮ rc;
	}ಅನ್ಯಥಾ{
		SAFE_FREE(auth_data_out);
		ಯದಿ(context->state == mosq_cs_authenticating && context->will){
			/* Free will without sending if this is our first authentication attempt */
			will__clear(context);
		}
		ಯದಿ(rc == MOSQ_ERR_AUTH){
			ಯದಿ(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ರಿಕ್ತ);
				mosquitto_FREE(context->id);
			}ಅನ್ಯಥಾ{
				send__disconnect(context, MQTT_RC_NOT_AUTHORIZED, ರಿಕ್ತ);
			}
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
		}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_NOT_SUPPORTED){
			/* Client has requested extended authentication, but we don't support it. */
			ಯದಿ(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, ರಿಕ್ತ);
				mosquitto_FREE(context->id);
			}ಅನ್ಯಥಾ{
				send__disconnect(context, MQTT_RC_BAD_AUTHENTICATION_METHOD, ರಿಕ್ತ);
			}
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
		}ಅನ್ಯಥಾ{
			ಯದಿ(context->state == mosq_cs_authenticating){
				mosquitto_FREE(context->id);
			}
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}
}
