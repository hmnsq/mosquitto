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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "packet_mosq.h"
#অন্তর্ভাব "property_mosq.h"
#অন্তর্ভাব "send_mosq.h"
#অন্তর্ভাব "util_mosq.h"
#অন্তর্ভাব "will_mosq.h"


পূর্ণসংখ্যা handle__disconnect(কাঠামো mosquitto *context)
{
	পূর্ণসংখ্যা rc;
	uint8_t reason_code = 0;
	mosquitto_property *properties = ফাঁকা;

	যদি(!context){
		ফেরত MOSQ_ERR_INVAL;
	}

	যদি(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		যদি((context->in_packet.command&0x0F) != 0x00){
			log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with incorrect flags %02X.",
					context->id, context->in_packet.command);
			ফেরত MOSQ_ERR_PROTOCOL;
		}
	}

	যদি(context->protocol == mosq_p_mqtt5 && context->in_packet.remaining_দৈর্ঘ্য > 0){
		/* FIXME - must handle reason code */
		rc = packet__read_byte(&context->in_packet, &reason_code);
		যদি(rc){
			ফেরত rc;
		}

		যদি(context->in_packet.remaining_দৈর্ঘ্য > 1){
			rc = property__read_all(CMD_DISCONNECT, &context->in_packet, &properties);
			যদি(rc){
				ফেরত rc;
			}
		}
	}
	rc = property__process_disconnect(context, &properties);
	যদি(rc){
		mosquitto_property_free_all(&properties);
		ফেরত rc;
	}
	mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */

	যদি(context->in_packet.pos != context->in_packet.remaining_দৈর্ঘ্য){
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->in_packet.pos, context->in_packet.remaining_দৈর্ঘ্য);
		ফেরত MOSQ_ERR_PROTOCOL;
	}
	log__printf(ফাঁকা, MOSQ_LOG_DEBUG, "Received DISCONNECT from %s", context->id);
	যদি(reason_code == MQTT_RC_DISCONNECT_WITH_WILL_বার্তা){
		mosquitto__set_state(context, mosq_cs_disconnect_with_will);
	}অন্যথা{
		will__clear(context);
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	do_disconnect(context, MOSQ_ERR_SUCCESS);
	ফেরত MOSQ_ERR_SUCCESS;
}
