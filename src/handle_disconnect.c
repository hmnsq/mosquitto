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

#ضمن "config.h"

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "packet_mosq.h"
#ضمن "property_mosq.h"
#ضمن "send_mosq.h"
#ضمن "util_mosq.h"
#ضمن "will_mosq.h"


صحيح handle__disconnect(بنية mosquitto *context)
{
	صحيح rc;
	uint8_t reason_کوډ = 0;
	mosquitto_property *properties = عدم;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		إذا((context->دننه_packet.command&0x0F) != 0x00){
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with incorrect flags %02X.",
					context->id, context->دننه_packet.command);
			أرجع MOSQ_ERR_PROTOCOL;
		}
	}

	إذا(context->protocol == mosq_p_mqtt5 && context->دننه_packet.پاتې_شوی_اوږدتیا > 0){
		/* FIXME - must handle reason code */
		rc = packet__read_byte(&context->دننه_packet, &reason_کوډ);
		إذا(rc){
			أرجع rc;
		}

		إذا(context->دننه_packet.پاتې_شوی_اوږدتیا > 1){
			rc = property__read_all(CMD_DISCONNECT, &context->دننه_packet, &properties);
			إذا(rc){
				أرجع rc;
			}
		}
	}
	rc = property__process_disconnect(context, &properties);
	إذا(rc){
		mosquitto_property_free_all(&properties);
		أرجع rc;
	}
	mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */

	إذا(context->دننه_packet.pos != context->دننه_packet.پاتې_شوی_اوږدتیا){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->دننه_packet.pos, context->دننه_packet.پاتې_شوی_اوږدتیا);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	log__printf(عدم, MOSQ_LOG_DEBUG, "Received DISCONNECT from %s", context->id);
	إذا(reason_کوډ == MQTT_RC_DISCONNECT_WITH_WILL_پیغام){
		mosquitto_سېټ_state(context, mosq_cs_disconnect_with_will);
	}وإلا{
		will__clear(context);
		mosquitto_سېټ_state(context, mosq_cs_disconnecting);
	}
	do_disconnect(context, MOSQ_ERR_SUCCESS);
	أرجع MOSQ_ERR_SUCCESS;
}
