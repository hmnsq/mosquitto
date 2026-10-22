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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "property_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "send_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "will_mosq.h"


ਪੂਰਨ handle__disconnect(ਬਣਤਰ mosquitto *context)
{
	ਪੂਰਨ rc;
	uint8_t reason_ਕੋਡ = 0;
	mosquitto_property *properties = ਨਿਰਾ;

	ਜੇ(!context){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਜੇ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		ਜੇ((context->ਵਿੱਚ_packet.command&0x0F) != 0x00){
			log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with incorrect flags %02X.",
					context->id, context->ਵਿੱਚ_packet.command);
			ਵਾਪਸ MOSQ_ERR_PROTOCOL;
		}
	}

	ਜੇ(context->protocol == mosq_p_mqtt5 && context->ਵਿੱਚ_packet.ਬਚਿਆ_ਲੰਮਾਈ > 0){
		/* FIXME - must handle reason code */
		rc = packet__read_byte(&context->ਵਿੱਚ_packet, &reason_ਕੋਡ);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}

		ਜੇ(context->ਵਿੱਚ_packet.ਬਚਿਆ_ਲੰਮਾਈ > 1){
			rc = property__read_all(CMD_DISCONNECT, &context->ਵਿੱਚ_packet, &properties);
			ਜੇ(rc){
				ਵਾਪਸ rc;
			}
		}
	}
	rc = property__process_disconnect(context, &properties);
	ਜੇ(rc){
		mosquitto_property_free_all(&properties);
		ਵਾਪਸ rc;
	}
	mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */

	ਜੇ(context->ਵਿੱਚ_packet.pos != context->ਵਿੱਚ_packet.ਬਚਿਆ_ਲੰਮਾਈ){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->ਵਿੱਚ_packet.pos, context->ਵਿੱਚ_packet.ਬਚਿਆ_ਲੰਮਾਈ);
		ਵਾਪਸ MOSQ_ERR_PROTOCOL;
	}
	log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Received DISCONNECT from %s", context->id);
	ਜੇ(reason_ਕੋਡ == MQTT_RC_DISCONNECT_WITH_WILL_ਸੁਨੇਹਾ){
		mosquitto_ਸੈੱਟ_state(context, mosq_cs_disconnect_with_will);
	}ਨਹੀਂਤਾਂ{
		will__clear(context);
		mosquitto_ਸੈੱਟ_state(context, mosq_cs_disconnecting);
	}
	do_disconnect(context, MOSQ_ERR_SUCCESS);
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}
