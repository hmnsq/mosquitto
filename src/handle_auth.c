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

#समावेश "config.h"

#समावेश <stdio.h>
#समावेश <string.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "packet_mosq.h"
#समावेश "property_mosq.h"
#समावेश "send_mosq.h"
#समावेश "util_mosq.h"
#समावेश "will_mosq.h"


पूर्णांक handle__auth(काष्ठा mosquitto *context)
{
	पूर्णांक rc = 0;
	uint8_t reason_code = 0;
	mosquitto_property *properties = शून्य;
	अक्षर *auth_method = शून्य;
	व्योम *auth_तथ्य = शून्य;
	uint16_t auth_तथ्य_len = 0;
	व्योम *auth_तथ्य_out = शून्य;
	uint16_t auth_तथ्य_out_len = 0;

	अगर(!context){
		वापस MOSQ_ERR_INVAL;
	}

	अगर(context->protocol != mosq_p_mqtt5){
		log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet when session not MQTT v5.0.", context->id);
		वापस MOSQ_ERR_PROTOCOL;
	}
	अगर(context->auth_method == शून्य){
		log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without existing auth-method.", context->id);
		वापस MOSQ_ERR_PROTOCOL;
	}
	अगर(context->in_packet.command != CMD_AUTH){
		वापस MOSQ_ERR_MALFORMED_PACKET;
	}

	अगर(context->in_packet.remaining_लंबाई > 0){
		अगर(packet__read_byte(&context->in_packet, &reason_code)){
			वापस MOSQ_ERR_MALFORMED_PACKET;
		}
		अगर(reason_code != MQTT_RC_CONTINUE_AUTHENTICATION
				&& reason_code != MQTT_RC_REAUTHENTICATE){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, शून्य);
			log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with reason-code = %d.",
					context->id, reason_code);
			वापस MOSQ_ERR_PROTOCOL;
		}

		अगर((reason_code == MQTT_RC_REAUTHENTICATE && context->state != mosq_cs_active)
				|| (reason_code == MQTT_RC_CONTINUE_AUTHENTICATION
				&& context->state != mosq_cs_authenticating && context->state != mosq_cs_reauthenticating)){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, शून्य);
			log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: AUTH reauthentication packet before session is active,",
					context->id);
			log__printf(शून्य, MOSQ_LOG_INFO, "     or attempted to continue authentication when no authentication in progress.");
			वापस MOSQ_ERR_PROTOCOL;
		}

		rc = property__read_all(CMD_AUTH, &context->in_packet, &properties);
		अगर(rc){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, शून्य);
			वापस rc;
		}


		अगर(mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &auth_method, false) == शून्य){
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_UNSPECIFIED, शून्य);
			log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without auth-method property.",
					context->id);
			वापस MOSQ_ERR_PROTOCOL;
		}

		अगर(!auth_method || म_भेद(auth_method, context->auth_method)){
			/* No method, or non-matching method */
			mosquitto_FREE(auth_method);
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, शून्य);
			log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with non-matching auth-method property (%s:%s).",
					context->id, context->auth_method, auth_method);
			वापस MOSQ_ERR_PROTOCOL;
		}
		mosquitto_FREE(auth_method);

		mosquitto_property_read_द्विआधारी(properties, MQTT_PROP_AUTHENTICATION_तथ्य, &auth_तथ्य, &auth_तथ्य_len, false);

		mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */
	}

	log__printf(शून्य, MOSQ_LOG_DEBUG, "Received AUTH from %s (rc%d, %s)", context->id, reason_code, context->auth_method);


	अगर(reason_code == MQTT_RC_REAUTHENTICATE){
		/* This is a re-authentication attempt */
		mosquitto__set_state(context, mosq_cs_reauthenticating);
		rc = mosquitto_security_auth_start(context, true, auth_तथ्य, auth_तथ्य_len, &auth_तथ्य_out, &auth_तथ्य_out_len);
	}अन्यथा{
		अगर(context->state != mosq_cs_reauthenticating){
			mosquitto__set_state(context, mosq_cs_authenticating);
		}
		rc = mosquitto_security_auth_continue(context, auth_तथ्य, auth_तथ्य_len, &auth_तथ्य_out, &auth_तथ्य_out_len);
	}
	mosquitto_FREE(auth_तथ्य);
	अगर(rc == MOSQ_ERR_SUCCESS){
		अगर(context->state == mosq_cs_authenticating){
			वापस connect__on_authorised(context, auth_तथ्य_out, auth_तथ्य_out_len);
		}अन्यथा{
			mosquitto__set_state(context, mosq_cs_active);
			rc = send__auth(context, MQTT_RC_SUCCESS, auth_तथ्य_out, auth_तथ्य_out_len);
			SAFE_FREE(auth_तथ्य_out);
			वापस rc;
		}
	}अन्यथा अगर(rc == MOSQ_ERR_AUTH_CONTINUE){
		rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_तथ्य_out, auth_तथ्य_out_len);
		SAFE_FREE(auth_तथ्य_out);
		वापस rc;
	}अन्यथा{
		SAFE_FREE(auth_तथ्य_out);
		अगर(context->state == mosq_cs_authenticating && context->will){
			/* Free will without sending if this is our first authentication attempt */
			will__clear(context);
		}
		अगर(rc == MOSQ_ERR_AUTH){
			अगर(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, शून्य);
				mosquitto_FREE(context->id);
			}अन्यथा{
				send__disconnect(context, MQTT_RC_NOT_AUTHORIZED, शून्य);
			}
			वापस MOSQ_ERR_PROTOCOL;
		}अन्यथा अगर(rc == MOSQ_ERR_NOT_SUPPORTED){
			/* Client has requested extended authentication, but we don't support it. */
			अगर(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, शून्य);
				mosquitto_FREE(context->id);
			}अन्यथा{
				send__disconnect(context, MQTT_RC_BAD_AUTHENTICATION_METHOD, शून्य);
			}
			वापस MOSQ_ERR_PROTOCOL;
		}अन्यथा{
			अगर(context->state == mosq_cs_authenticating){
				mosquitto_FREE(context->id);
			}
			वापस rc;
		}
	}
}
