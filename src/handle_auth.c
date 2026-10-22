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


ကိန်းပြည့် handle__auth(sa_nracanaa mosquitto *context)
{
	ကိန်းပြည့် rc = 0;
	uint8_t reason_code = 0;
	mosquitto_property *properties = NULL;
	var_na *auth_method = NULL;
	void *auth_data = NULL;
	uint16_t auth_data_len = 0;
	void *auth_data_out = NULL;
	uint16_t auth_data_out_len = 0;

	အကယ်၍(!context){
		ပြန်ပေး MOSQ_ERR_INVAL;
	}

	အကယ်၍(context->protocol != mosq_p_mqtt5){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet when session not MQTT v5.0.", context->id);
		ပြန်ပေး MOSQ_ERR_PROTOCOL;
	}
	အကယ်၍(context->auth_method == NULL){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without existing auth-method.", context->id);
		ပြန်ပေး MOSQ_ERR_PROTOCOL;
	}
	အကယ်၍(context->in_packet.command != CMD_AUTH){
		ပြန်ပေး MOSQ_ERR_MALFORMED_PACKET;
	}

	အကယ်၍(context->in_packet.remaining_length > 0){
		အကယ်၍(packet__read_byte(&context->in_packet, &reason_code)){
			ပြန်ပေး MOSQ_ERR_MALFORMED_PACKET;
		}
		အကယ်၍(reason_code != MQTT_RC_CONTINUE_AUTHENTICATION
				&& reason_code != MQTT_RC_REAUTHENTICATE){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, NULL);
			log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with reason-code = %d.",
					context->id, reason_code);
			ပြန်ပေး MOSQ_ERR_PROTOCOL;
		}

		အကယ်၍((reason_code == MQTT_RC_REAUTHENTICATE && context->state != mosq_cs_active)
				|| (reason_code == MQTT_RC_CONTINUE_AUTHENTICATION
				&& context->state != mosq_cs_authenticating && context->state != mosq_cs_reauthenticating)){

			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, NULL);
			log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: AUTH reauthentication packet before session is active,",
					context->id);
			log__printf(NULL, MOSQ_LOG_INFO, "     or attempted to continue authentication when no authentication in progress.");
			ပြန်ပေး MOSQ_ERR_PROTOCOL;
		}

		rc = property__read_all(CMD_AUTH, &context->in_packet, &properties);
		အကယ်၍(rc){
			send__disconnect(context, MQTT_RC_UNSPECIFIED, NULL);
			ပြန်ပေး rc;
		}


		အကယ်၍(mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &auth_method, false) == NULL){
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_UNSPECIFIED, NULL);
			log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet without auth-method property.",
					context->id);
			ပြန်ပေး MOSQ_ERR_PROTOCOL;
		}

		အကယ်၍(!auth_method || strcmp(auth_method, context->auth_method)){
			/* No method, or non-matching method */
			mosquitto_FREE(auth_method);
			mosquitto_property_free_all(&properties);
			send__disconnect(context, MQTT_RC_PROTOCOL_ERROR, NULL);
			log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: AUTH packet with non-matching auth-method property (%s:%s).",
					context->id, context->auth_method, auth_method);
			ပြန်ပေး MOSQ_ERR_PROTOCOL;
		}
		mosquitto_FREE(auth_method);

		mosquitto_property_read_binary(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);

		mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */
	}

	log__printf(NULL, MOSQ_LOG_DEBUG, "Received AUTH from %s (rc%d, %s)", context->id, reason_code, context->auth_method);


	အကယ်၍(reason_code == MQTT_RC_REAUTHENTICATE){
		/* This is a re-authentication attempt */
		mosquitto__set_state(context, mosq_cs_reauthenticating);
		rc = mosquitto_security_auth_start(context, true, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
	}မဟုတ်ပါက{
		အကယ်၍(context->state != mosq_cs_reauthenticating){
			mosquitto__set_state(context, mosq_cs_authenticating);
		}
		rc = mosquitto_security_auth_continue(context, auth_data, auth_data_len, &auth_data_out, &auth_data_out_len);
	}
	mosquitto_FREE(auth_data);
	အကယ်၍(rc == MOSQ_ERR_SUCCESS){
		အကယ်၍(context->state == mosq_cs_authenticating){
			ပြန်ပေး connect__on_authorised(context, auth_data_out, auth_data_out_len);
		}မဟုတ်ပါက{
			mosquitto__set_state(context, mosq_cs_active);
			rc = send__auth(context, MQTT_RC_SUCCESS, auth_data_out, auth_data_out_len);
			SAFE_FREE(auth_data_out);
			ပြန်ပေး rc;
		}
	}မဟုတ်ပါက အကယ်၍(rc == MOSQ_ERR_AUTH_CONTINUE){
		rc = send__auth(context, MQTT_RC_CONTINUE_AUTHENTICATION, auth_data_out, auth_data_out_len);
		SAFE_FREE(auth_data_out);
		ပြန်ပေး rc;
	}မဟုတ်ပါက{
		SAFE_FREE(auth_data_out);
		အကယ်၍(context->state == mosq_cs_authenticating && context->will){
			/* Free will without sending if this is our first authentication attempt */
			will__clear(context);
		}
		အကယ်၍(rc == MOSQ_ERR_AUTH){
			အကယ်၍(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_NOT_AUTHORIZED, NULL);
				mosquitto_FREE(context->id);
			}မဟုတ်ပါက{
				send__disconnect(context, MQTT_RC_NOT_AUTHORIZED, NULL);
			}
			ပြန်ပေး MOSQ_ERR_PROTOCOL;
		}မဟုတ်ပါက အကယ်၍(rc == MOSQ_ERR_NOT_SUPPORTED){
			/* Client has requested extended authentication, but we don't support it. */
			အကယ်၍(context->state == mosq_cs_authenticating){
				send__connack(context, 0, MQTT_RC_BAD_AUTHENTICATION_METHOD, NULL);
				mosquitto_FREE(context->id);
			}မဟုတ်ပါက{
				send__disconnect(context, MQTT_RC_BAD_AUTHENTICATION_METHOD, NULL);
			}
			ပြန်ပေး MOSQ_ERR_PROTOCOL;
		}မဟုတ်ပါက{
			အကယ်၍(context->state == mosq_cs_authenticating){
				mosquitto_FREE(context->id);
			}
			ပြန်ပေး rc;
		}
	}
}
