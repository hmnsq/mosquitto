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

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "property_mosq.h"
#अन्तर्भाव "send_mosq.h"
#अन्तर्भाव "util_mosq.h"
#अन्तर्भाव "will_mosq.h"


पूर्णाङ्क handle__auth(संरचना mosquitto *context)
{
	पूर्णाङ्क rc = 0;
	uint8_t reason_code = 0;
	mosquitto_property *properties = रिक्त;
	वर्ण *auth_method = रिक्त;
	शून्य *auth_data = रिक्त;
	uint16_t auth_data_len = 0;
	शून्य *auth_data_out = रिक्त;
	uint16_t auth_data_out_len = 0;

	यदि(!context){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	यदि(context->protocol != mosq_p_mqtt5){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet when session not MQTT v5.0.", context->id);
		फर्काऊ MOSQ_ERR_PROTOCOL;
	}
	यदि(context->auth_method == रिक्त){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without existing auth-method.", context->id);
		फर्काऊ MOSQ_ERR_PROTOCOL;
	}
	यदि(context->in_packet.command != CMD_AUTH){
		फर्काऊ MOSQ_ERR_MALFORMED_PACKET;
	}

	यदि(context->in_packet.remaining_length > 0){
		यदि(packet__read_byte(&context->in_packet, &reason_code)){
			फर्काऊ MOSQ_ERR_MALFORMED_PACKET;
		}
		यदि(reason_code != MQTT_RC_CONTINUE_AUTHENTICATION
				&& reason_code != MQTT_RC_REAUTHENTICATE){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, रिक्त);
			log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with reason-code = %d.",
					context->id, reason_code);
			फर्काऊ MOSQ_ERR_PROTOCOL;
		}

		यदि((reason_code == MQTT_RC_REAUTHENTICATE && context->state != mosq_cs_active)
				|| (reason_code == MQTT_RC_CONTINUE_AUTHENTICATION
				&& context->state != mosq_cs_authenticating && context->state != mosq_cs_reauthenticating)){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, रिक्त);
			log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: AUTH reauthentication packet before session is active,",
					context->id);
			log__printf(रिक्त, MOSQ_LOG_INFO, "     or attempted to continue authentication when no authentication in progress.");
			फर्काऊ MOSQ_ERR_PROTOCOL;
		}

		rc = property__read_all(CMD_AUTH, &context->in_packet, &properties);
		यदि(rc){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, रिक्त);
			फर्काऊ rc;
		}


		यदि(mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &auth_method, false) == रिक्त){
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_UNSPECIFIED, रिक्त);
			log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without auth-method property.",
					context->id);
			फर्काऊ MOSQ_ERR_PROTOCOL;
		}

		यदि(!auth_method || म_भेद(auth_method, context->auth_method)){
			/* No method, or non-matching method */
			mosquitto_FREE(auth_method);
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, रिक्त);
			log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with non-matching auth-method property (%s:%s).",
					context->id, context->auth_method, auth_method);
			फर्काऊ MOSQ_ERR_PROTOCOL;
		}
		mosquitto_FREE(auth_method);

		mosquitto_property_read_द्विआधारी(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);

		mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */
	}

	log__printf(रिक्त, MOSQ_LOG_DEBUG, "Received AUTH from %s (rc%d, %s)", context->id, reason_code, context->auth_method);


	यदि(reason_code == MQTT_RC_REAUTHENTICATE){
		/* This is a re-authentication attempt */
		mosquitto__set_state(context, mosq_cs_reauthenticating);
		rc = mosquitto_security_auth_start(context, true, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
	}अन्यथा{
		यदि(context->state != mosq_cs_reauthenticating){
			mosquitto__set_state(context, mosq_cs_authenticating);
		}
		rc = mosquitto_security_auth_continue(context, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
	}
	mosquitto_FREE(auth_data);
	यदि(rc == MOSQ_ERR_SUCCESS){
		यदि(context->state == mosq_cs_authenticating){
			फर्काऊ connect__on_authorised(context, auth_data_out, auth_data_out_len);
		}अन्यथा{
			mosquitto__set_state(context, mosq_cs_active);
			rc = send__auth(context, MQTT_RC_SUCCESS, auth_data_out, auth_data_out_len);
			SAFE_FREE(auth_data_out);
			फर्काऊ rc;
		}
	}अन्यथा यदि(rc == MOSQ_ERR_AUTH_CONTINUE){
		rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_data_out, auth_data_out_len);
		SAFE_FREE(auth_data_out);
		फर्काऊ rc;
	}अन्यथा{
		SAFE_FREE(auth_data_out);
		यदि(context->state == mosq_cs_authenticating && context->will){
			/* Free will without sending if this is our first authentication attempt */
			will__clear(context);
		}
		यदि(rc == MOSQ_ERR_AUTH){
			यदि(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, रिक्त);
				mosquitto_FREE(context->id);
			}अन्यथा{
				send__disconnect(context, MQTT_RC_NOT_AUTHORIZED, रिक्त);
			}
			फर्काऊ MOSQ_ERR_PROTOCOL;
		}अन्यथा यदि(rc == MOSQ_ERR_NOT_SUPPORTED){
			/* Client has requested extended authentication, but we don't support it. */
			यदि(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, रिक्त);
				mosquitto_FREE(context->id);
			}अन्यथा{
				send__disconnect(context, MQTT_RC_BAD_AUTHENTICATION_METHOD, रिक्त);
			}
			फर्काऊ MOSQ_ERR_PROTOCOL;
		}अन्यथा{
			यदि(context->state == mosq_cs_authenticating){
				mosquitto_FREE(context->id);
			}
			फर्काऊ rc;
		}
	}
}
