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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "packet_mosq.h"
#અન્તર્ભાવ "property_mosq.h"
#અન્તર્ભાવ "send_mosq.h"
#અન્તર્ભાવ "util_mosq.h"
#અન્તર્ભાવ "will_mosq.h"


પૂર્ણાંક handle__auth(સંરચના mosquitto *context)
{
	પૂર્ણાંક rc = 0;
	uint8_t reason_code = 0;
	mosquitto_property *properties = ખાલી;
	અક્ષર *auth_method = ખાલી;
	શૂન્ય *auth_data = ખાલી;
	uint16_t auth_data_len = 0;
	શૂન્ય *auth_data_out = ખાલી;
	uint16_t auth_data_out_len = 0;

	જો(!context){
		પાછા MOSQ_ERR_INVAL;
	}

	જો(context->protocol != mosq_p_mqtt5){
		log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet when session not MQTT v5.0.", context->id);
		પાછા MOSQ_ERR_PROTOCOL;
	}
	જો(context->auth_method == ખાલી){
		log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without existing auth-method.", context->id);
		પાછા MOSQ_ERR_PROTOCOL;
	}
	જો(context->in_packet.command != CMD_AUTH){
		પાછા MOSQ_ERR_MALFORMED_PACKET;
	}

	જો(context->in_packet.remaining_length > 0){
		જો(packet__read_byte(&context->in_packet, &reason_code)){
			પાછા MOSQ_ERR_MALFORMED_PACKET;
		}
		જો(reason_code != MQTT_RC_CONTINUE_AUTHENTICATION
				&& reason_code != MQTT_RC_REAUTHENTICATE){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ખાલી);
			log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with reason-code = %d.",
					context->id, reason_code);
			પાછા MOSQ_ERR_PROTOCOL;
		}

		જો((reason_code == MQTT_RC_REAUTHENTICATE && context->state != mosq_cs_active)
				|| (reason_code == MQTT_RC_CONTINUE_AUTHENTICATION
				&& context->state != mosq_cs_authenticating && context->state != mosq_cs_reauthenticating)){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ખાલી);
			log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: AUTH reauthentication packet before session is active,",
					context->id);
			log__printf(ખાલી, MOSQ_LOG_INFO, "     or attempted to continue authentication when no authentication in progress.");
			પાછા MOSQ_ERR_PROTOCOL;
		}

		rc = property__read_all(CMD_AUTH, &context->in_packet, &properties);
		જો(rc){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, ખાલી);
			પાછા rc;
		}


		જો(mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &auth_method, false) == ખાલી){
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_UNSPECIFIED, ખાલી);
			log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without auth-method property.",
					context->id);
			પાછા MOSQ_ERR_PROTOCOL;
		}

		જો(!auth_method || strcmp(auth_method, context->auth_method)){
			/* No method, or non-matching method */
			mosquitto_FREE(auth_method);
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, ખાલી);
			log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with non-matching auth-method property (%s:%s).",
					context->id, context->auth_method, auth_method);
			પાછા MOSQ_ERR_PROTOCOL;
		}
		mosquitto_FREE(auth_method);

		mosquitto_property_read_દ્વિઅંકી(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);

		mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */
	}

	log__printf(ખાલી, MOSQ_LOG_DEBUG, "Received AUTH from %s (rc%d, %s)", context->id, reason_code, context->auth_method);


	જો(reason_code == MQTT_RC_REAUTHENTICATE){
		/* This is a re-authentication attempt */
		mosquitto__set_state(context, mosq_cs_reauthenticating);
		rc = mosquitto_security_auth_start(context, true, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
	}નહીંતર{
		જો(context->state != mosq_cs_reauthenticating){
			mosquitto__set_state(context, mosq_cs_authenticating);
		}
		rc = mosquitto_security_auth_continue(context, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
	}
	mosquitto_FREE(auth_data);
	જો(rc == MOSQ_ERR_SUCCESS){
		જો(context->state == mosq_cs_authenticating){
			પાછા connect__on_authorised(context, auth_data_out, auth_data_out_len);
		}નહીંતર{
			mosquitto__set_state(context, mosq_cs_active);
			rc = send__auth(context, MQTT_RC_SUCCESS, auth_data_out, auth_data_out_len);
			SAFE_FREE(auth_data_out);
			પાછા rc;
		}
	}નહીંતર જો(rc == MOSQ_ERR_AUTH_CONTINUE){
		rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_data_out, auth_data_out_len);
		SAFE_FREE(auth_data_out);
		પાછા rc;
	}નહીંતર{
		SAFE_FREE(auth_data_out);
		જો(context->state == mosq_cs_authenticating && context->will){
			/* Free will without sending if this is our first authentication attempt */
			will__clear(context);
		}
		જો(rc == MOSQ_ERR_AUTH){
			જો(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, ખાલી);
				mosquitto_FREE(context->id);
			}નહીંતર{
				send__disconnect(context, MQTT_RC_NOT_AUTHORIZED, ખાલી);
			}
			પાછા MOSQ_ERR_PROTOCOL;
		}નહીંતર જો(rc == MOSQ_ERR_NOT_SUPPORTED){
			/* Client has requested extended authentication, but we don't support it. */
			જો(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, ખાલી);
				mosquitto_FREE(context->id);
			}નહીંતર{
				send__disconnect(context, MQTT_RC_BAD_AUTHENTICATION_METHOD, ખાલી);
			}
			પાછા MOSQ_ERR_PROTOCOL;
		}નહીંતર{
			જો(context->state == mosq_cs_authenticating){
				mosquitto_FREE(context->id);
			}
			પાછા rc;
		}
	}
}
