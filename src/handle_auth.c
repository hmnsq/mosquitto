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

#অন্তর্ভাৱ "config.h"

#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <string.h>

#অন্তর্ভাৱ "mosquitto_broker_internal.h"
#অন্তর্ভাৱ "mosquitto/mqtt_protocol.h"
#অন্তর্ভাৱ "packet_mosq.h"
#অন্তর্ভাৱ "property_mosq.h"
#অন্তর্ভাৱ "send_mosq.h"
#অন্তর্ভাৱ "util_mosq.h"
#অন্তর্ভাৱ "will_mosq.h"


পূর্ণাঙ্ক handle__auth(সংরচনা mosquitto *context)
{
	পূর্ণাঙ্ক rc = 0;
	uint8_t reason_code = 0;
	mosquitto_property *properties = রিক্ত;
	ৱর্ণ *auth_method = রিক্ত;
	শূন্য *auth_data = রিক্ত;
	uint16_t auth_data_len = 0;
	শূন্য *auth_data_out = রিক্ত;
	uint16_t auth_data_out_len = 0;

	যদি(!context){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	যদি(context->protocol != mosq_p_mqtt5){
		log__printf(রিক্ত, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet when session not MQTT v5.0.", context->id);
		ঘূৰাই MOSQ_ERR_PROTOCOL;
	}
	যদি(context->auth_method == রিক্ত){
		log__printf(রিক্ত, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without existing auth-method.", context->id);
		ঘূৰাই MOSQ_ERR_PROTOCOL;
	}
	যদি(context->in_packet.command != CMD_AUTH){
		ঘূৰাই MOSQ_ERR_MALFORMED_PACKET;
	}

	যদি(context->in_packet.remaining_length > 0){
		যদি(packet__read_byte(&context->in_packet, &reason_code)){
			ঘূৰাই MOSQ_ERR_MALFORMED_PACKET;
		}
		যদি(reason_code != MQTT_RC_CONTINUE_AUTHENTICATION
				&& reason_code != MQTT_RC_REAUTHENTICATE){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, রিক্ত);
			log__printf(রিক্ত, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with reason-code = %d.",
					context->id, reason_code);
			ঘূৰাই MOSQ_ERR_PROTOCOL;
		}

		যদি((reason_code == MQTT_RC_REAUTHENTICATE && context->state != mosq_cs_active)
				|| (reason_code == MQTT_RC_CONTINUE_AUTHENTICATION
				&& context->state != mosq_cs_authenticating && context->state != mosq_cs_reauthenticating)){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, রিক্ত);
			log__printf(রিক্ত, MOSQ_LOG_INFO, "Protocol error from %s: AUTH reauthentication packet before session is active,",
					context->id);
			log__printf(রিক্ত, MOSQ_LOG_INFO, "     or attempted to continue authentication when no authentication in progress.");
			ঘূৰাই MOSQ_ERR_PROTOCOL;
		}

		rc = property__read_all(CMD_AUTH, &context->in_packet, &properties);
		যদি(rc){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, রিক্ত);
			ঘূৰাই rc;
		}


		যদি(mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &auth_method, false) == রিক্ত){
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_UNSPECIFIED, রিক্ত);
			log__printf(রিক্ত, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without auth-method property.",
					context->id);
			ঘূৰাই MOSQ_ERR_PROTOCOL;
		}

		যদি(!auth_method || strcmp(auth_method, context->auth_method)){
			/* No method, or non-matching method */
			mosquitto_FREE(auth_method);
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, রিক্ত);
			log__printf(রিক্ত, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with non-matching auth-method property (%s:%s).",
					context->id, context->auth_method, auth_method);
			ঘূৰাই MOSQ_ERR_PROTOCOL;
		}
		mosquitto_FREE(auth_method);

		mosquitto_property_read_দ্বিমিক(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);

		mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */
	}

	log__printf(রিক্ত, MOSQ_LOG_DEBUG, "Received AUTH from %s (rc%d, %s)", context->id, reason_code, context->auth_method);


	যদি(reason_code == MQTT_RC_REAUTHENTICATE){
		/* This is a re-authentication attempt */
		mosquitto__set_state(context, mosq_cs_reauthenticating);
		rc = mosquitto_security_auth_start(context, true, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
	}অন্যথা{
		যদি(context->state != mosq_cs_reauthenticating){
			mosquitto__set_state(context, mosq_cs_authenticating);
		}
		rc = mosquitto_security_auth_continue(context, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
	}
	mosquitto_FREE(auth_data);
	যদি(rc == MOSQ_ERR_SUCCESS){
		যদি(context->state == mosq_cs_authenticating){
			ঘূৰাই connect__on_authorised(context, auth_data_out, auth_data_out_len);
		}অন্যথা{
			mosquitto__set_state(context, mosq_cs_active);
			rc = send__auth(context, MQTT_RC_SUCCESS, auth_data_out, auth_data_out_len);
			SAFE_FREE(auth_data_out);
			ঘূৰাই rc;
		}
	}অন্যথা যদি(rc == MOSQ_ERR_AUTH_CONTINUE){
		rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_data_out, auth_data_out_len);
		SAFE_FREE(auth_data_out);
		ঘূৰাই rc;
	}অন্যথা{
		SAFE_FREE(auth_data_out);
		যদি(context->state == mosq_cs_authenticating && context->will){
			/* Free will without sending if this is our first authentication attempt */
			will__clear(context);
		}
		যদি(rc == MOSQ_ERR_AUTH){
			যদি(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, রিক্ত);
				mosquitto_FREE(context->id);
			}অন্যথা{
				send__disconnect(context, MQTT_RC_NOT_AUTHORIZED, রিক্ত);
			}
			ঘূৰাই MOSQ_ERR_PROTOCOL;
		}অন্যথা যদি(rc == MOSQ_ERR_NOT_SUPPORTED){
			/* Client has requested extended authentication, but we don't support it. */
			যদি(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, রিক্ত);
				mosquitto_FREE(context->id);
			}অন্যথা{
				send__disconnect(context, MQTT_RC_BAD_AUTHENTICATION_METHOD, রিক্ত);
			}
			ঘূৰাই MOSQ_ERR_PROTOCOL;
		}অন্যথা{
			যদি(context->state == mosq_cs_authenticating){
				mosquitto_FREE(context->id);
			}
			ঘূৰাই rc;
		}
	}
}
