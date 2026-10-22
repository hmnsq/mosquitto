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
	uint8_t reason_کد = 0;
	mosquitto_property *properties = عدم;
	حرف *auth_روش = عدم;
	فراغ *auth_داده = عدم;
	uint16_t auth_داده_len = 0;
	فراغ *auth_داده_بیرون = عدم;
	uint16_t auth_داده_بیرون_len = 0;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(context->protocol != mosq_p_mqtt5){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet when session not MQTT v5.0.", context->id);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا(context->auth_روش == عدم){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without existing auth-method.", context->id);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا(context->درون_packet.command != CMD_AUTH){
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	إذا(context->درون_packet.مانده_درازی > 0){
		إذا(packet__read_byte(&context->درون_packet, &reason_کد)){
			أرجع MOSQ_ERR_MALFORMED_PACKET;
		}
		إذا(reason_کد != MQTT_RC_CONTINUE_AUTHENTICATION
				&& reason_کد != MQTT_RC_REAUTHENTICATE){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, عدم);
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with reason-code = %d.",
					context->id, reason_کد);
			أرجع MOSQ_ERR_PROTOCOL;
		}

		إذا((reason_کد == MQTT_RC_REAUTHENTICATE && context->state != mosq_cs_فعال)
				|| (reason_کد == MQTT_RC_CONTINUE_AUTHENTICATION
				&& context->state != mosq_cs_authenticating && context->state != mosq_cs_reauthenticating)){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, عدم);
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: AUTH reauthentication packet before session is active,",
					context->id);
			log__printf(عدم, MOSQ_LOG_INFO, "     or attempted to continue authentication when no authentication in progress.");
			أرجع MOSQ_ERR_PROTOCOL;
		}

		rc = property__read_all(CMD_AUTH, &context->درون_packet, &properties);
		إذا(rc){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, عدم);
			أرجع rc;
		}


		إذا(mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_روش, &auth_روش, false) == عدم){
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_UNSPECIFIED, عدم);
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without auth-method property.",
					context->id);
			أرجع MOSQ_ERR_PROTOCOL;
		}

		إذا(!auth_روش || strcmp(auth_روش, context->auth_روش)){
			/* No method, or non-matching method */
			mosquitto_FREE(auth_روش);
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, عدم);
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with non-matching auth-method property (%s:%s).",
					context->id, context->auth_روش, auth_روش);
			أرجع MOSQ_ERR_PROTOCOL;
		}
		mosquitto_FREE(auth_روش);

		mosquitto_property_read_دودویی(properties, MQTT_PROP_AUTHENTICATION_داده, &auth_داده, &auth_داده_len, false);

		mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */
	}

	log__printf(عدم, MOSQ_LOG_DEBUG, "Received AUTH from %s (rc%d, %s)", context->id, reason_کد, context->auth_روش);


	إذا(reason_کد == MQTT_RC_REAUTHENTICATE){
		/* This is a re-authentication attempt */
		mosquitto_مجموعه_state(context, mosq_cs_reauthenticating);
		rc = mosquitto_security_auth_شروع(context, true, auth_داده, auth_داده_len, &auth_داده_بیرون, &auth_داده_بیرون_len);
	}وإلا{
		إذا(context->state != mosq_cs_reauthenticating){
			mosquitto_مجموعه_state(context, mosq_cs_authenticating);
		}
		rc = mosquitto_security_auth_continue(context, auth_داده, auth_داده_len, &auth_داده_بیرون, &auth_داده_بیرون_len);
	}
	mosquitto_FREE(auth_داده);
	إذا(rc == MOSQ_ERR_SUCCESS){
		إذا(context->state == mosq_cs_authenticating){
			أرجع connect__on_authorised(context, auth_داده_بیرون, auth_داده_بیرون_len);
		}وإلا{
			mosquitto_مجموعه_state(context, mosq_cs_فعال);
			rc = send__auth(context, MQTT_RC_SUCCESS, auth_داده_بیرون, auth_داده_بیرون_len);
			امن_FREE(auth_داده_بیرون);
			أرجع rc;
		}
	}وإلا إذا(rc == MOSQ_ERR_AUTH_CONTINUE){
		rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_داده_بیرون, auth_داده_بیرون_len);
		امن_FREE(auth_داده_بیرون);
		أرجع rc;
	}وإلا{
		امن_FREE(auth_داده_بیرون);
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
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_روش, عدم);
				mosquitto_FREE(context->id);
			}وإلا{
				send__disconnect(context, MQTT_RC_BAD_AUTHENTICATION_روش, عدم);
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
