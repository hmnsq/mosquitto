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

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "packet_mosq.h"
#ضمن "property_mosq.h"
#ضمن "send_mosq.h"
#ضمن "util_mosq.h"
#ضمن "will_mosq.h"


صحيح handle__auth(بنية mosquitto *context)
{
	صحيح rc = 0;
	uint8_t reason_code = 0;
	mosquitto_property *properties = عدم;
	حرف *auth_method = عدم;
	فراغ *auth_data = عدم;
	uint16_t auth_data_len = 0;
	فراغ *auth_data_out = عدم;
	uint16_t auth_data_out_len = 0;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(context->protocol != mosq_p_mqtt5){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet when session not MQTT v5.0.", context->id);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا(context->auth_method == عدم){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without existing auth-method.", context->id);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا(context->in_packet.command != CMD_AUTH){
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	إذا(context->in_packet.remaining_length > 0){
		إذا(packet__read_byte(&context->in_packet, &reason_code)){
			أرجع MOSQ_ERR_MALFORMED_PACKET;
		}
		إذا(reason_code != MQTT_RC_CONTINUE_AUTHENTICATION
				&& reason_code != MQTT_RC_REAUTHENTICATE){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, عدم);
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with reason-code = %d.",
					context->id, reason_code);
			أرجع MOSQ_ERR_PROTOCOL;
		}

		إذا((reason_code == MQTT_RC_REAUTHENTICATE && context->state != mosq_cs_active)
				|| (reason_code == MQTT_RC_CONTINUE_AUTHENTICATION
				&& context->state != mosq_cs_authenticating && context->state != mosq_cs_reauthenticating)){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, عدم);
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: AUTH reauthentication packet before session is active,",
					context->id);
			log__printf(عدم, MOSQ_LOG_INFO, "     or attempted to continue authentication when no authentication in progress.");
			أرجع MOSQ_ERR_PROTOCOL;
		}

		rc = property__read_all(CMD_AUTH, &context->in_packet, &properties);
		إذا(rc){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, عدم);
			أرجع rc;
		}


		إذا(mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &auth_method, false) == عدم){
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_UNSPECIFIED, عدم);
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without auth-method property.",
					context->id);
			أرجع MOSQ_ERR_PROTOCOL;
		}

		إذا(!auth_method || strcmp(auth_method, context->auth_method)){
			/* No method, or non-matching method */
			mosquitto_FREE(auth_method);
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, عدم);
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with non-matching auth-method property (%s:%s).",
					context->id, context->auth_method, auth_method);
			أرجع MOSQ_ERR_PROTOCOL;
		}
		mosquitto_FREE(auth_method);

		mosquitto_property_read_نبيو(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);

		mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */
	}

	log__printf(عدم, MOSQ_LOG_DEBUG, "Received AUTH from %s (rc%d, %s)", context->id, reason_code, context->auth_method);


	إذا(reason_code == MQTT_RC_REAUTHENTICATE){
		/* This is a re-authentication attempt */
		mosquitto__set_state(context, mosq_cs_reauthenticating);
		rc = mosquitto_security_auth_start(context, true, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
	}وإلا{
		إذا(context->state != mosq_cs_reauthenticating){
			mosquitto__set_state(context, mosq_cs_authenticating);
		}
		rc = mosquitto_security_auth_continue(context, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
	}
	mosquitto_FREE(auth_data);
	إذا(rc == MOSQ_ERR_SUCCESS){
		إذا(context->state == mosq_cs_authenticating){
			أرجع connect__on_authorised(context, auth_data_out, auth_data_out_len);
		}وإلا{
			mosquitto__set_state(context, mosq_cs_active);
			rc = send__auth(context, MQTT_RC_SUCCESS, auth_data_out, auth_data_out_len);
			SAFE_FREE(auth_data_out);
			أرجع rc;
		}
	}وإلا إذا(rc == MOSQ_ERR_AUTH_CONTINUE){
		rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_data_out, auth_data_out_len);
		SAFE_FREE(auth_data_out);
		أرجع rc;
	}وإلا{
		SAFE_FREE(auth_data_out);
		إذا(context->state == mosq_cs_authenticating && context->will){
			/* Free will without sending if this is our first authentication attempt */
			will__clear(context);
		}
		إذا(rc == MOSQ_ERR_AUTH){
			إذا(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, عدم);
				mosquitto_FREE(context->id);
			}وإلا{
				send__disconnect(context, MQTT_RC_NOT_AUTHORIZED, عدم);
			}
			أرجع MOSQ_ERR_PROTOCOL;
		}وإلا إذا(rc == MOSQ_ERR_NOT_SUPPORTED){
			/* Client has requested extended authentication, but we don't support it. */
			إذا(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, عدم);
				mosquitto_FREE(context->id);
			}وإلا{
				send__disconnect(context, MQTT_RC_BAD_AUTHENTICATION_METHOD, عدم);
			}
			أرجع MOSQ_ERR_PROTOCOL;
		}وإلا{
			إذا(context->state == mosq_cs_authenticating){
				mosquitto_FREE(context->id);
			}
			أرجع rc;
		}
	}
}
