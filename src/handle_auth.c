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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "packet_mosq.h"
#அந்தர்பாவ "property_mosq.h"
#அந்தர்பாவ "send_mosq.h"
#அந்தர்பாவ "util_mosq.h"
#அந்தர்பாவ "will_mosq.h"


முழு handle__auth(கட்டமைப்பு mosquitto *context)
{
	முழு rc = 0;
	uint8_t reason_code = 0;
	mosquitto_property *properties = வெற்று;
	எழுத்து *auth_method = வெற்று;
	வெறுமை *auth_data = வெற்று;
	uint16_t auth_data_len = 0;
	வெறுமை *auth_data_out = வெற்று;
	uint16_t auth_data_out_len = 0;

	எனில்(!context){
		திரும்பு MOSQ_ERR_INVAL;
	}

	எனில்(context->protocol != mosq_p_mqtt5){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet when session not MQTT v5.0.", context->id);
		திரும்பு MOSQ_ERR_PROTOCOL;
	}
	எனில்(context->auth_method == வெற்று){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without existing auth-method.", context->id);
		திரும்பு MOSQ_ERR_PROTOCOL;
	}
	எனில்(context->in_packet.command != CMD_AUTH){
		திரும்பு MOSQ_ERR_MALFORMED_PACKET;
	}

	எனில்(context->in_packet.remaining_length > 0){
		எனில்(packet__read_byte(&context->in_packet, &reason_code)){
			திரும்பு MOSQ_ERR_MALFORMED_PACKET;
		}
		எனில்(reason_code != MQTT_RC_CONTINUE_AUTHENTICATION
				&& reason_code != MQTT_RC_REAUTHENTICATE){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, வெற்று);
			log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with reason-code = %d.",
					context->id, reason_code);
			திரும்பு MOSQ_ERR_PROTOCOL;
		}

		எனில்((reason_code == MQTT_RC_REAUTHENTICATE && context->state != mosq_cs_active)
				|| (reason_code == MQTT_RC_CONTINUE_AUTHENTICATION
				&& context->state != mosq_cs_authenticating && context->state != mosq_cs_reauthenticating)){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, வெற்று);
			log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: AUTH reauthentication packet before session is active,",
					context->id);
			log__printf(வெற்று, MOSQ_LOG_INFO, "     or attempted to continue authentication when no authentication in progress.");
			திரும்பு MOSQ_ERR_PROTOCOL;
		}

		rc = property__read_all(CMD_AUTH, &context->in_packet, &properties);
		எனில்(rc){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, வெற்று);
			திரும்பு rc;
		}


		எனில்(mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &auth_method, false) == வெற்று){
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_UNSPECIFIED, வெற்று);
			log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without auth-method property.",
					context->id);
			திரும்பு MOSQ_ERR_PROTOCOL;
		}

		எனில்(!auth_method || strcmp(auth_method, context->auth_method)){
			/* No method, or non-matching method */
			mosquitto_FREE(auth_method);
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, வெற்று);
			log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with non-matching auth-method property (%s:%s).",
					context->id, context->auth_method, auth_method);
			திரும்பு MOSQ_ERR_PROTOCOL;
		}
		mosquitto_FREE(auth_method);

		mosquitto_property_read_இருமம்(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);

		mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */
	}

	log__printf(வெற்று, MOSQ_LOG_DEBUG, "Received AUTH from %s (rc%d, %s)", context->id, reason_code, context->auth_method);


	எனில்(reason_code == MQTT_RC_REAUTHENTICATE){
		/* This is a re-authentication attempt */
		mosquitto__set_state(context, mosq_cs_reauthenticating);
		rc = mosquitto_security_auth_start(context, true, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
	}இல்லையெனில்{
		எனில்(context->state != mosq_cs_reauthenticating){
			mosquitto__set_state(context, mosq_cs_authenticating);
		}
		rc = mosquitto_security_auth_continue(context, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
	}
	mosquitto_FREE(auth_data);
	எனில்(rc == MOSQ_ERR_SUCCESS){
		எனில்(context->state == mosq_cs_authenticating){
			திரும்பு connect__on_authorised(context, auth_data_out, auth_data_out_len);
		}இல்லையெனில்{
			mosquitto__set_state(context, mosq_cs_active);
			rc = send__auth(context, MQTT_RC_SUCCESS, auth_data_out, auth_data_out_len);
			SAFE_FREE(auth_data_out);
			திரும்பு rc;
		}
	}இல்லையெனில் எனில்(rc == MOSQ_ERR_AUTH_CONTINUE){
		rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_data_out, auth_data_out_len);
		SAFE_FREE(auth_data_out);
		திரும்பு rc;
	}இல்லையெனில்{
		SAFE_FREE(auth_data_out);
		எனில்(context->state == mosq_cs_authenticating && context->will){
			/* Free will without sending if this is our first authentication attempt */
			will__clear(context);
		}
		எனில்(rc == MOSQ_ERR_AUTH){
			எனில்(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, வெற்று);
				mosquitto_FREE(context->id);
			}இல்லையெனில்{
				send__disconnect(context, MQTT_RC_NOT_AUTHORIZED, வெற்று);
			}
			திரும்பு MOSQ_ERR_PROTOCOL;
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_NOT_SUPPORTED){
			/* Client has requested extended authentication, but we don't support it. */
			எனில்(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, வெற்று);
				mosquitto_FREE(context->id);
			}இல்லையெனில்{
				send__disconnect(context, MQTT_RC_BAD_AUTHENTICATION_METHOD, வெற்று);
			}
			திரும்பு MOSQ_ERR_PROTOCOL;
		}இல்லையெனில்{
			எனில்(context->state == mosq_cs_authenticating){
				mosquitto_FREE(context->id);
			}
			திரும்பு rc;
		}
	}
}
