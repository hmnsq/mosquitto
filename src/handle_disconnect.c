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

#अन्तर्भाव "config.h"

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "property_mosq.h"
#अन्तर्भाव "send_mosq.h"
#अन्तर्भाव "util_mosq.h"
#अन्तर्भाव "will_mosq.h"


पूर्णाङ्क handle__disconnect(संरचना mosquitto *context)
{
	पूर्णाङ्क rc;
	uint8_t reason_code = 0;
	mosquitto_property *properties = रिक्त;

	जइ(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	जइ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		जइ((context->in_packet.command&0x0F) != 0x00){
			log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with incorrect flags %02X.",
					context->id, context->in_packet.command);
			प्रत्यागम MOSQ_ERR_PROTOCOL;
		}
	}

	जइ(context->protocol == mosq_p_mqtt5 && context->in_packet.remaining_length > 0){
		/* FIXME - must handle reason code */
		rc = packet__read_byte(&context->in_packet, &reason_code);
		जइ(rc){
			प्रत्यागम rc;
		}

		जइ(context->in_packet.remaining_length > 1){
			rc = property__read_all(CMD_DISCONNECT, &context->in_packet, &properties);
			जइ(rc){
				प्रत्यागम rc;
			}
		}
	}
	rc = property__process_disconnect(context, &properties);
	जइ(rc){
		mosquitto_property_free_all(&properties);
		प्रत्यागम rc;
	}
	mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */

	जइ(context->in_packet.pos != context->in_packet.remaining_length){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->in_packet.pos, context->in_packet.remaining_length);
		प्रत्यागम MOSQ_ERR_PROTOCOL;
	}
	log__printf(रिक्त, MOSQ_LOG_DEBUG, "Received DISCONNECT from %s", context->id);
	जइ(reason_code == MQTT_RC_DISCONNECT_WITH_WILL_सन्देशः){
		mosquitto__set_state(context, mosq_cs_disconnect_with_will);
	}अण्णहा{
		will__clear(context);
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	do_disconnect(context, MOSQ_ERR_SUCCESS);
	प्रत्यागम MOSQ_ERR_SUCCESS;
}
