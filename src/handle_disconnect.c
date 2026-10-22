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

#شامل "config.h"

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "packet_mosq.h"
#شامل "property_mosq.h"
#شامل "send_mosq.h"
#شامل "util_mosq.h"
#شامل "will_mosq.h"


صحیح handle__disconnect(ساخت mosquitto *context)
{
	صحیح rc;
	uint8_t reason_رمز = 0;
	mosquitto_property *properties = تہی;

	اگر(!context){
		واپس MOSQ_ERR_INVAL;
	}

	اگر(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		اگر((context->اندر_packet.command&0x0F) != 0x00){
			log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with incorrect flags %02X.",
					context->id, context->اندر_packet.command);
			واپس MOSQ_ERR_PROTOCOL;
		}
	}

	اگر(context->protocol == mosq_p_mqtt5 && context->اندر_packet.بقایا_لمبائی > 0){
		/* FIXME - must handle reason code */
		rc = packet__read_byte(&context->اندر_packet, &reason_رمز);
		اگر(rc){
			واپس rc;
		}

		اگر(context->اندر_packet.بقایا_لمبائی > 1){
			rc = property__read_all(CMD_DISCONNECT, &context->اندر_packet, &properties);
			اگر(rc){
				واپس rc;
			}
		}
	}
	rc = property__process_disconnect(context, &properties);
	اگر(rc){
		mosquitto_property_free_all(&properties);
		واپس rc;
	}
	mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */

	اگر(context->اندر_packet.pos != context->اندر_packet.بقایا_لمبائی){
		log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->اندر_packet.pos, context->اندر_packet.بقایا_لمبائی);
		واپس MOSQ_ERR_PROTOCOL;
	}
	log__printf(تہی, MOSQ_LOG_DEBUG, "Received DISCONNECT from %s", context->id);
	اگر(reason_رمز == MQTT_RC_DISCONNECT_WITH_WILL_پیغام){
		mosquitto_سیٹ_state(context, mosq_cs_disconnect_with_will);
	}ورنہ{
		will__clear(context);
		mosquitto_سیٹ_state(context, mosq_cs_disconnecting);
	}
	do_disconnect(context, MOSQ_ERR_SUCCESS);
	واپس MOSQ_ERR_SUCCESS;
}
