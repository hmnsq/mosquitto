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
	uint8_t reason_संहिता = 0;
	mosquitto_property *properties = रिक्त;

	यदि(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	यदि(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		यदि((context->अन्तर्_packet.command&0x0F) != 0x00){
			log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with incorrect flags %02X.",
					context->id, context->अन्तर्_packet.command);
			प्रत्यागम MOSQ_ERR_PROTOCOL;
		}
	}

	यदि(context->protocol == mosq_p_mqtt5 && context->अन्तर्_packet.remaining_दीर्घता > 0){
		/* FIXME - must handle reason code */
		rc = packet__read_byte(&context->अन्तर्_packet, &reason_संहिता);
		यदि(rc){
			प्रत्यागम rc;
		}

		यदि(context->अन्तर्_packet.remaining_दीर्घता > 1){
			rc = property__read_all(CMD_DISCONNECT, &context->अन्तर्_packet, &properties);
			यदि(rc){
				प्रत्यागम rc;
			}
		}
	}
	rc = property__process_disconnect(context, &properties);
	यदि(rc){
		mosquitto_property_free_all(&properties);
		प्रत्यागम rc;
	}
	mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */

	यदि(context->अन्तर्_packet.pos != context->अन्तर्_packet.remaining_दीर्घता){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->अन्तर्_packet.pos, context->अन्तर्_packet.remaining_दीर्घता);
		प्रत्यागम MOSQ_ERR_PROTOCOL;
	}
	log__printf(रिक्त, MOSQ_LOG_DEBUG, "Received DISCONNECT from %s", context->id);
	यदि(reason_संहिता == MQTT_RC_DISCONNECT_WITH_WILL_सन्देशः){
		mosquitto_समुच्चयः_state(context, mosq_cs_disconnect_with_will);
	}अन्यथा{
		will__clear(context);
		mosquitto_समुच्चयः_state(context, mosq_cs_disconnecting);
	}
	do_disconnect(context, MOSQ_ERR_SUCCESS);
	प्रत्यागम MOSQ_ERR_SUCCESS;
}
