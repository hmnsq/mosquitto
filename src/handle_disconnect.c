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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "packet_mosq.h"
#અન્તર્ભાવ "property_mosq.h"
#અન્તર્ભાવ "send_mosq.h"
#અન્તર્ભાવ "util_mosq.h"
#અન્તર્ભાવ "will_mosq.h"


પૂર્ણાંક handle__disconnect(સંરચના mosquitto *context)
{
	પૂર્ણાંક rc;
	uint8_t reason_code = 0;
	mosquitto_property *properties = ખાલી;

	જો(!context){
		પાછા MOSQ_ERR_INVAL;
	}

	જો(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		જો((context->in_packet.command&0x0F) != 0x00){
			log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with incorrect flags %02X.",
					context->id, context->in_packet.command);
			પાછા MOSQ_ERR_PROTOCOL;
		}
	}

	જો(context->protocol == mosq_p_mqtt5 && context->in_packet.remaining_length > 0){
		/* FIXME - must handle reason code */
		rc = packet__read_byte(&context->in_packet, &reason_code);
		જો(rc){
			પાછા rc;
		}

		જો(context->in_packet.remaining_length > 1){
			rc = property__read_all(CMD_DISCONNECT, &context->in_packet, &properties);
			જો(rc){
				પાછા rc;
			}
		}
	}
	rc = property__process_disconnect(context, &properties);
	જો(rc){
		mosquitto_property_free_all(&properties);
		પાછા rc;
	}
	mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */

	જો(context->in_packet.pos != context->in_packet.remaining_length){
		log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->in_packet.pos, context->in_packet.remaining_length);
		પાછા MOSQ_ERR_PROTOCOL;
	}
	log__printf(ખાલી, MOSQ_LOG_DEBUG, "Received DISCONNECT from %s", context->id);
	જો(reason_code == MQTT_RC_DISCONNECT_WITH_WILL_સંદેશ){
		mosquitto__set_state(context, mosq_cs_disconnect_with_will);
	}નહીંતર{
		will__clear(context);
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	do_disconnect(context, MOSQ_ERR_SUCCESS);
	પાછા MOSQ_ERR_SUCCESS;
}
