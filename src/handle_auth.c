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
	uint8_t reason_کوڈ = 0;
	mosquitto_property *properties = عدم;
	حرف *auth_طریقہ = عدم;
	فراغ *auth_انکڑے = عدم;
	uint16_t auth_انکڑے_len = 0;
	فراغ *auth_انکڑے_باہرلا = عدم;
	uint16_t auth_انکڑے_باہرلا_len = 0;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(context->protocol != mosq_p_mqtt5){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet when session not MQTT v5.0.", context->id);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا(context->auth_طریقہ == عدم){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without existing auth-method.", context->id);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا(context->وچ_packet.command != CMD_AUTH){
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	إذا(context->وچ_packet.بچیا_لمائی > 0){
		إذا(packet__read_byte(&context->وچ_packet, &reason_کوڈ)){
			أرجع MOSQ_ERR_MALFORMED_PACKET;
		}
		إذا(reason_کوڈ != MQTT_RC_CONTINUE_AUTHENTICATION
				&& reason_کوڈ != MQTT_RC_REAUTHENTICATE){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, عدم);
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with reason-code = %d.",
					context->id, reason_کوڈ);
			أرجع MOSQ_ERR_PROTOCOL;
		}

		إذا((reason_کوڈ == MQTT_RC_REAUTHENTICATE && context->state != mosq_cs_سرگرم)
				|| (reason_کوڈ == MQTT_RC_CONTINUE_AUTHENTICATION
				&& context->state != mosq_cs_authenticating && context->state != mosq_cs_reauthenticating)){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, عدم);
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: AUTH reauthentication packet before session is active,",
					context->id);
			log__printf(عدم, MOSQ_LOG_INFO, "     or attempted to continue authentication when no authentication in progress.");
			أرجع MOSQ_ERR_PROTOCOL;
		}

		rc = property__read_all(CMD_AUTH, &context->وچ_packet, &properties);
		إذا(rc){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, عدم);
			أرجع rc;
		}


		إذا(mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_طریقہ, &auth_طریقہ, false) == عدم){
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_UNSPECIFIED, عدم);
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without auth-method property.",
					context->id);
			أرجع MOSQ_ERR_PROTOCOL;
		}

		إذا(!auth_طریقہ || strcmp(auth_طریقہ, context->auth_طریقہ)){
			/* No method, or non-matching method */
			mosquitto_FREE(auth_طریقہ);
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, عدم);
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with non-matching auth-method property (%s:%s).",
					context->id, context->auth_طریقہ, auth_طریقہ);
			أرجع MOSQ_ERR_PROTOCOL;
		}
		mosquitto_FREE(auth_طریقہ);

		mosquitto_property_read_ثنائی(properties, MQTT_PROP_AUTHENTICATION_انکڑے, &auth_انکڑے, &auth_انکڑے_len, false);

		mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */
	}

	log__printf(عدم, MOSQ_LOG_DEBUG, "Received AUTH from %s (rc%d, %s)", context->id, reason_کوڈ, context->auth_طریقہ);


	إذا(reason_کوڈ == MQTT_RC_REAUTHENTICATE){
		/* This is a re-authentication attempt */
		mosquitto_سیٹ_state(context, mosq_cs_reauthenticating);
		rc = mosquitto_security_auth_شروع(context, true, auth_انکڑے, auth_انکڑے_len, &auth_انکڑے_باہرلا, &auth_انکڑے_باہرلا_len);
	}وإلا{
		إذا(context->state != mosq_cs_reauthenticating){
			mosquitto_سیٹ_state(context, mosq_cs_authenticating);
		}
		rc = mosquitto_security_auth_continue(context, auth_انکڑے, auth_انکڑے_len, &auth_انکڑے_باہرلا, &auth_انکڑے_باہرلا_len);
	}
	mosquitto_FREE(auth_انکڑے);
	إذا(rc == MOSQ_ERR_SUCCESS){
		إذا(context->state == mosq_cs_authenticating){
			أرجع connect__on_authorised(context, auth_انکڑے_باہرلا, auth_انکڑے_باہرلا_len);
		}وإلا{
			mosquitto_سیٹ_state(context, mosq_cs_سرگرم);
			rc = send__auth(context, MQTT_RC_SUCCESS, auth_انکڑے_باہرلا, auth_انکڑے_باہرلا_len);
			محفوظ_FREE(auth_انکڑے_باہرلا);
			أرجع rc;
		}
	}وإلا إذا(rc == MOSQ_ERR_AUTH_CONTINUE){
		rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_انکڑے_باہرلا, auth_انکڑے_باہرلا_len);
		محفوظ_FREE(auth_انکڑے_باہرلا);
		أرجع rc;
	}وإلا{
		محفوظ_FREE(auth_انکڑے_باہرلا);
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
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_طریقہ, عدم);
				mosquitto_FREE(context->id);
			}وإلا{
				send__disconnect(context, MQTT_RC_BAD_AUTHENTICATION_طریقہ, عدم);
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
