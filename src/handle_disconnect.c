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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "packet_mosq.h"
#അന്തര്ഭാവ "property_mosq.h"
#അന്തര്ഭാവ "send_mosq.h"
#അന്തര്ഭാവ "util_mosq.h"
#അന്തര്ഭാവ "will_mosq.h"


പൂർണ്ണസംഖ്യ handle__disconnect(ഘടന mosquitto *context)
{
	പൂർണ്ണസംഖ്യ rc;
	uint8_t reason_code = 0;
	mosquitto_property *properties = ശൂന്യ;

	എങ്കിൽ(!context){
		തിരികെ MOSQ_ERR_INVAL;
	}

	എങ്കിൽ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		എങ്കിൽ((context->in_packet.command&0x0F) != 0x00){
			log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with incorrect flags %02X.",
					context->id, context->in_packet.command);
			തിരികെ MOSQ_ERR_PROTOCOL;
		}
	}

	എങ്കിൽ(context->protocol == mosq_p_mqtt5 && context->in_packet.remaining_length > 0){
		/* FIXME - must handle reason code */
		rc = packet__read_byte(&context->in_packet, &reason_code);
		എങ്കിൽ(rc){
			തിരികെ rc;
		}

		എങ്കിൽ(context->in_packet.remaining_length > 1){
			rc = property__read_all(CMD_DISCONNECT, &context->in_packet, &properties);
			എങ്കിൽ(rc){
				തിരികെ rc;
			}
		}
	}
	rc = property__process_disconnect(context, &properties);
	എങ്കിൽ(rc){
		mosquitto_property_free_all(&properties);
		തിരികെ rc;
	}
	mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */

	എങ്കിൽ(context->in_packet.pos != context->in_packet.remaining_length){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->in_packet.pos, context->in_packet.remaining_length);
		തിരികെ MOSQ_ERR_PROTOCOL;
	}
	log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "Received DISCONNECT from %s", context->id);
	എങ്കിൽ(reason_code == MQTT_RC_DISCONNECT_WITH_WILL_സന്ദേശം){
		mosquitto__set_state(context, mosq_cs_disconnect_with_will);
	}അല്ലെങ്കിൽ{
		will__clear(context);
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	do_disconnect(context, MOSQ_ERR_SUCCESS);
	തിരികെ MOSQ_ERR_SUCCESS;
}
