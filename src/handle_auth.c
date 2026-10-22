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

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "packet_mosq.h"
#include "property_mosq.h"
#include "send_mosq.h"
#include "util_mosq.h"
#include "will_mosq.h"


puur_naa_nka handle__auth(sa_nracanaa mosquitto *context)
{
	puur_naa_nka rc = 0;
	uint8_t reason_code = 0;
	mosquitto_property *properties = NULL;
	var_na *auth_method = NULL;
	void *auth_data = NULL;
	uint16_t auth_data_len = 0;
	void *auth_data_out = NULL;
	uint16_t auth_data_out_len = 0;

	ᯀᯮᯝ(!context){
		ᯀᯮᯞ MOSQ_ERR_INVAL;
	}

	ᯀᯮᯝ(context->protocol != mosq_p_mqtt5){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet when session not MQTT v5.0.", context->id);
		ᯀᯮᯞ MOSQ_ERR_PROTOCOL;
	}
	ᯀᯮᯝ(context->auth_method == NULL){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without existing auth-method.", context->id);
		ᯀᯮᯞ MOSQ_ERR_PROTOCOL;
	}
	ᯀᯮᯝ(context->in_packet.command != CMD_AUTH){
		ᯀᯮᯞ MOSQ_ERR_MALFORMED_PACKET;
	}

	ᯀᯮᯝ(context->in_packet.remaining_length > 0){
		ᯀᯮᯝ(packet__read_byte(&context->in_packet, &reason_code)){
			ᯀᯮᯞ MOSQ_ERR_MALFORMED_PACKET;
		}
		ᯀᯮᯝ(reason_code != MQTT_RC_CONTINUE_AUTHENTICATION
				&& reason_code != MQTT_RC_REAUTHENTICATE){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, NULL);
			log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with reason-code = %d.",
					context->id, reason_code);
			ᯀᯮᯞ MOSQ_ERR_PROTOCOL;
		}

		ᯀᯮᯝ((reason_code == MQTT_RC_REAUTHENTICATE && context->state != mosq_cs_active)
				|| (reason_code == MQTT_RC_CONTINUE_AUTHENTICATION
				&& context->state != mosq_cs_authenticating && context->state != mosq_cs_reauthenticating)){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, NULL);
			log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: AUTH reauthentication packet before session is active,",
					context->id);
			log__printf(NULL, MOSQ_LOG_INFO, "     or attempted to continue authentication when no authentication in progress.");
			ᯀᯮᯞ MOSQ_ERR_PROTOCOL;
		}

		rc = property__read_all(CMD_AUTH, &context->in_packet, &properties);
		ᯀᯮᯝ(rc){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, NULL);
			ᯀᯮᯞ rc;
		}


		ᯀᯮᯝ(mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &auth_method, false) == NULL){
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_UNSPECIFIED, NULL);
			log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without auth-method property.",
					context->id);
			ᯀᯮᯞ MOSQ_ERR_PROTOCOL;
		}

		ᯀᯮᯝ(!auth_method || strcmp(auth_method, context->auth_method)){
			/* No method, or non-matching method */
			mosquitto_FREE(auth_method);
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, NULL);
			log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with non-matching auth-method property (%s:%s).",
					context->id, context->auth_method, auth_method);
			ᯀᯮᯞ MOSQ_ERR_PROTOCOL;
		}
		mosquitto_FREE(auth_method);

		mosquitto_property_read_binary(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);

		mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */
	}

	log__printf(NULL, MOSQ_LOG_DEBUG, "Received AUTH from %s (rc%d, %s)", context->id, reason_code, context->auth_method);


	ᯀᯮᯝ(reason_code == MQTT_RC_REAUTHENTICATE){
		/* This is a re-authentication attempt */
		mosquitto__set_state(context, mosq_cs_reauthenticating);
		rc = mosquitto_security_auth_start(context, true, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
	}ᯀᯮᯝᯂᯪ{
		ᯀᯮᯝ(context->state != mosq_cs_reauthenticating){
			mosquitto__set_state(context, mosq_cs_authenticating);
		}
		rc = mosquitto_security_auth_continue(context, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
	}
	mosquitto_FREE(auth_data);
	ᯀᯮᯝ(rc == MOSQ_ERR_SUCCESS){
		ᯀᯮᯝ(context->state == mosq_cs_authenticating){
			ᯀᯮᯞ connect__on_authorised(context, auth_data_out, auth_data_out_len);
		}ᯀᯮᯝᯂᯪ{
			mosquitto__set_state(context, mosq_cs_active);
			rc = send__auth(context, MQTT_RC_SUCCESS, auth_data_out, auth_data_out_len);
			SAFE_FREE(auth_data_out);
			ᯀᯮᯞ rc;
		}
	}ᯀᯮᯝᯂᯪ ᯀᯮᯝ(rc == MOSQ_ERR_AUTH_CONTINUE){
		rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_data_out, auth_data_out_len);
		SAFE_FREE(auth_data_out);
		ᯀᯮᯞ rc;
	}ᯀᯮᯝᯂᯪ{
		SAFE_FREE(auth_data_out);
		ᯀᯮᯝ(context->state == mosq_cs_authenticating && context->will){
			/* Free will without sending if this is our first authentication attempt */
			will__clear(context);
		}
		ᯀᯮᯝ(rc == MOSQ_ERR_AUTH){
			ᯀᯮᯝ(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, NULL);
				mosquitto_FREE(context->id);
			}ᯀᯮᯝᯂᯪ{
				send__disconnect(context, MQTT_RC_NOT_AUTHORIZED, NULL);
			}
			ᯀᯮᯞ MOSQ_ERR_PROTOCOL;
		}ᯀᯮᯝᯂᯪ ᯀᯮᯝ(rc == MOSQ_ERR_NOT_SUPPORTED){
			/* Client has requested extended authentication, but we don't support it. */
			ᯀᯮᯝ(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, NULL);
				mosquitto_FREE(context->id);
			}ᯀᯮᯝᯂᯪ{
				send__disconnect(context, MQTT_RC_BAD_AUTHENTICATION_METHOD, NULL);
			}
			ᯀᯮᯞ MOSQ_ERR_PROTOCOL;
		}ᯀᯮᯝᯂᯪ{
			ᯀᯮᯝ(context->state == mosq_cs_authenticating){
				mosquitto_FREE(context->id);
			}
			ᯀᯮᯞ rc;
		}
	}
}
