/*
Copyright (c) 2009-2021 Roger Light <roger@atchoo.org>

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

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "packet_mosq.h"
#समावेश "property_mosq.h"
#समावेश "send_mosq.h"
#समावेश "util_mosq.h"
#समावेश "will_mosq.h"


पूर्णांक handle__disconnect(काष्ठा mosquitto *context)
{
	पूर्णांक rc;
	uint8_t reason_code = 0;
	mosquitto_property *properties = शून्य;

	अगर(!context){
		वापस MOSQ_ERR_INVAL;
	}

	अगर(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		अगर((context->in_packet.command&0x0F) != 0x00){
			log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with incorrect flags %02X.",
					context->id, context->in_packet.command);
			वापस MOSQ_ERR_PROTOCOL;
		}
	}

	अगर(context->protocol == mosq_p_mqtt5 && context->in_packet.remaining_लंबाई > 0){
		/* FIXME - must handle reason code */
		rc = packet__read_byte(&context->in_packet, &reason_code);
		अगर(rc){
			वापस rc;
		}

		अगर(context->in_packet.remaining_लंबाई > 1){
			rc = property__read_all(CMD_DISCONNECT, &context->in_packet, &properties);
			अगर(rc){
				वापस rc;
			}
		}
	}
	rc = property__process_disconnect(context, &properties);
	अगर(rc){
		mosquitto_property_free_all(&properties);
		वापस rc;
	}
	mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */

	अगर(context->in_packet.pos != context->in_packet.remaining_लंबाई){
		log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->in_packet.pos, context->in_packet.remaining_लंबाई);
		वापस MOSQ_ERR_PROTOCOL;
	}
	log__printf(शून्य, MOSQ_LOG_DEBUG, "Received DISCONNECT from %s", context->id);
	अगर(reason_code == MQTT_RC_DISCONNECT_WITH_WILL_संदेश){
		mosquitto__set_state(context, mosq_cs_disconnect_with_will);
	}अन्यथा{
		will__clear(context);
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	do_disconnect(context, MOSQ_ERR_SUCCESS);
	वापस MOSQ_ERR_SUCCESS;
}
