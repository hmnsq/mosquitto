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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "packet_mosq.h"
#அந்தர்பாவ "property_mosq.h"
#அந்தர்பாவ "send_mosq.h"
#அந்தர்பாவ "util_mosq.h"
#அந்தர்பாவ "will_mosq.h"


முழு handle__disconnect(கட்டமைப்பு mosquitto *context)
{
	முழு rc;
	uint8_t reason_code = 0;
	mosquitto_property *properties = வெற்று;

	எனில்(!context){
		திரும்பு MOSQ_ERR_INVAL;
	}

	எனில்(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		எனில்((context->in_packet.command&0x0F) != 0x00){
			log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with incorrect flags %02X.",
					context->id, context->in_packet.command);
			திரும்பு MOSQ_ERR_PROTOCOL;
		}
	}

	எனில்(context->protocol == mosq_p_mqtt5 && context->in_packet.remaining_length > 0){
		/* FIXME - must handle reason code */
		rc = packet__read_byte(&context->in_packet, &reason_code);
		எனில்(rc){
			திரும்பு rc;
		}

		எனில்(context->in_packet.remaining_length > 1){
			rc = property__read_all(CMD_DISCONNECT, &context->in_packet, &properties);
			எனில்(rc){
				திரும்பு rc;
			}
		}
	}
	rc = property__process_disconnect(context, &properties);
	எனில்(rc){
		mosquitto_property_free_all(&properties);
		திரும்பு rc;
	}
	mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */

	எனில்(context->in_packet.pos != context->in_packet.remaining_length){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->in_packet.pos, context->in_packet.remaining_length);
		திரும்பு MOSQ_ERR_PROTOCOL;
	}
	log__printf(வெற்று, MOSQ_LOG_DEBUG, "Received DISCONNECT from %s", context->id);
	எனில்(reason_code == MQTT_RC_DISCONNECT_WITH_WILL_செய்தி){
		mosquitto__set_state(context, mosq_cs_disconnect_with_will);
	}இல்லையெனில்{
		will__clear(context);
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	do_disconnect(context, MOSQ_ERR_SUCCESS);
	திரும்பு MOSQ_ERR_SUCCESS;
}
