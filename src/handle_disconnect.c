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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "property_mosq.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"
#ಅನ್ತರ್ಭಾವ "will_mosq.h"


ಪೂರ್ಣಾಙ್ಕ handle__disconnect(ಸಂರಚನಾ mosquitto *context)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	uint8_t reason_code = 0;
	mosquitto_property *properties = ರಿಕ್ತ;

	ಯದಿ(!context){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	ಯದಿ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		ಯದಿ((context->in_packet.command&0x0F) != 0x00){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with incorrect flags %02X.",
					context->id, context->in_packet.command);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
		}
	}

	ಯದಿ(context->protocol == mosq_p_mqtt5 && context->in_packet.remaining_length > 0){
		/* FIXME - must handle reason code */
		rc = packet__read_byte(&context->in_packet, &reason_code);
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}

		ಯದಿ(context->in_packet.remaining_length > 1){
			rc = property__read_all(CMD_DISCONNECT, &context->in_packet, &properties);
			ಯದಿ(rc){
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}
	}
	rc = property__process_disconnect(context, &properties);
	ಯದಿ(rc){
		mosquitto_property_free_all(&properties);
		ಪ್ರತ್ಯಾಗಮ rc;
	}
	mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */

	ಯದಿ(context->in_packet.pos != context->in_packet.remaining_length){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->in_packet.pos, context->in_packet.remaining_length);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
	}
	log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "Received DISCONNECT from %s", context->id);
	ಯದಿ(reason_code == MQTT_RC_DISCONNECT_WITH_WILL_ಸಂದೇಶ){
		mosquitto__set_state(context, mosq_cs_disconnect_with_will);
	}ಅನ್ಯಥಾ{
		will__clear(context);
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	do_disconnect(context, MOSQ_ERR_SUCCESS);
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}
