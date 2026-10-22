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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "packet_mosq.h"
#അന്തര്ഭാവ "property_mosq.h"
#അന്തര്ഭാവ "send_mosq.h"
#അന്തര്ഭാവ "util_mosq.h"
#അന്തര്ഭാവ "will_mosq.h"


പൂർണ്ണസംഖ്യ handle__auth(ഘടന mosquitto *context)
{
	പൂർണ്ണസംഖ്യ rc = 0;
	uint8_t reason_code = 0;
	mosquitto_property *properties = ശൂന്യ;
	അക്ഷരം *auth_method = ശൂന്യ;
	ശൂന്യം *auth_data = ശൂന്യ;
	uint16_t auth_data_len = 0;
	ശൂന്യം *auth_data_out = ശൂന്യ;
	uint16_t auth_data_out_len = 0;

	എങ്കിൽ(!context){
		തിരികെ MOSQ_ERR_INVAL;
	}

	എങ്കിൽ(context->protocol != mosq_p_mqtt5){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet when session not MQTT v5.0.", context->id);
		തിരികെ MOSQ_ERR_PROTOCOL;
	}
	എങ്കിൽ(context->auth_method == ശൂന്യ){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without existing auth-method.", context->id);
		തിരികെ MOSQ_ERR_PROTOCOL;
	}
	എങ്കിൽ(context->in_packet.command != CMD_AUTH){
		തിരികെ MOSQ_ERR_MALFORMED_PACKET;
	}

	എങ്കിൽ(context->in_packet.remaining_length > 0){
		എങ്കിൽ(packet__read_byte(&context->in_packet, &reason_code)){
			തിരികെ MOSQ_ERR_MALFORMED_PACKET;
		}
		എങ്കിൽ(reason_code != MQTT_RC_CONTINUE_AUTHENTICATION
				&& reason_code != MQTT_RC_REAUTHENTICATE){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ശൂന്യ);
			log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with reason-code = %d.",
					context->id, reason_code);
			തിരികെ MOSQ_ERR_PROTOCOL;
		}

		എങ്കിൽ((reason_code == MQTT_RC_REAUTHENTICATE && context->state != mosq_cs_active)
				|| (reason_code == MQTT_RC_CONTINUE_AUTHENTICATION
				&& context->state != mosq_cs_authenticating && context->state != mosq_cs_reauthenticating)){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ശൂന്യ);
			log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH reauthentication packet before session is active,",
					context->id);
			log__printf(ശൂന്യ, MOSQ_LOG_INFO, "     or attempted to continue authentication when no authentication in progress.");
			തിരികെ MOSQ_ERR_PROTOCOL;
		}

		rc = property__read_all(CMD_AUTH, &context->in_packet, &properties);
		എങ്കിൽ(rc){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, ശൂന്യ);
			തിരികെ rc;
		}


		എങ്കിൽ(mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &auth_method, false) == ശൂന്യ){
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_UNSPECIFIED, ശൂന്യ);
			log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without auth-method property.",
					context->id);
			തിരികെ MOSQ_ERR_PROTOCOL;
		}

		എങ്കിൽ(!auth_method || strcmp(auth_method, context->auth_method)){
			/* No method, or non-matching method */
			mosquitto_FREE(auth_method);
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ശൂന്യ);
			log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with non-matching auth-method property (%s:%s).",
					context->id, context->auth_method, auth_method);
			തിരികെ MOSQ_ERR_PROTOCOL;
		}
		mosquitto_FREE(auth_method);

		mosquitto_property_read_ദ്വിമാന(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);

		mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */
	}

	log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "Received AUTH from %s (rc%d, %s)", context->id, reason_code, context->auth_method);


	എങ്കിൽ(reason_code == MQTT_RC_REAUTHENTICATE){
		/* This is a re-authentication attempt */
		mosquitto__set_state(context, mosq_cs_reauthenticating);
		rc = mosquitto_security_auth_start(context, true, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
	}അല്ലെങ്കിൽ{
		എങ്കിൽ(context->state != mosq_cs_reauthenticating){
			mosquitto__set_state(context, mosq_cs_authenticating);
		}
		rc = mosquitto_security_auth_continue(context, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
	}
	mosquitto_FREE(auth_data);
	എങ്കിൽ(rc == MOSQ_ERR_SUCCESS){
		എങ്കിൽ(context->state == mosq_cs_authenticating){
			തിരികെ connect__on_authorised(context, auth_data_out, auth_data_out_len);
		}അല്ലെങ്കിൽ{
			mosquitto__set_state(context, mosq_cs_active);
			rc = send__auth(context, MQTT_RC_SUCCESS, auth_data_out, auth_data_out_len);
			SAFE_FREE(auth_data_out);
			തിരികെ rc;
		}
	}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_AUTH_CONTINUE){
		rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_data_out, auth_data_out_len);
		SAFE_FREE(auth_data_out);
		തിരികെ rc;
	}അല്ലെങ്കിൽ{
		SAFE_FREE(auth_data_out);
		എങ്കിൽ(context->state == mosq_cs_authenticating && context->will){
			/* Free will without sending if this is our first authentication attempt */
			will__clear(context);
		}
		എങ്കിൽ(rc == MOSQ_ERR_AUTH){
			എങ്കിൽ(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ശൂന്യ);
				mosquitto_FREE(context->id);
			}അല്ലെങ്കിൽ{
				send__disconnect(context, MQTT_RC_NOT_AUTHORIZED, ശൂന്യ);
			}
			തിരികെ MOSQ_ERR_PROTOCOL;
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_NOT_SUPPORTED){
			/* Client has requested extended authentication, but we don't support it. */
			എങ്കിൽ(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, ശൂന്യ);
				mosquitto_FREE(context->id);
			}അല്ലെങ്കിൽ{
				send__disconnect(context, MQTT_RC_BAD_AUTHENTICATION_METHOD, ശൂന്യ);
			}
			തിരികെ MOSQ_ERR_PROTOCOL;
		}അല്ലെങ്കിൽ{
			എങ്കിൽ(context->state == mosq_cs_authenticating){
				mosquitto_FREE(context->id);
			}
			തിരികെ rc;
		}
	}
}
