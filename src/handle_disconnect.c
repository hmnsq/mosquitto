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

#include "config.h"

#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "packet_mosq.h"
#include "property_mosq.h"
#include "send_mosq.h"
#include "util_mosq.h"
#include "will_mosq.h"


정수 handle__disconnect(struct mosquitto *context)
{
	정수 rc;
	uint8_t reason_code = 0;
	mosquitto_property *properties = NULL;

	만약(!context){
		반환 MOSQ_ERR_INVAL;
	}

	만약(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		만약((context->in_packet.command&0x0F) != 0x00){
			log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with incorrect flags %02X.",
					context->id, context->in_packet.command);
			반환 MOSQ_ERR_PROTOCOL;
		}
	}

	만약(context->protocol == mosq_p_mqtt5 && context->in_packet.remaining_length > 0){
		/* FIXME - must handle reason code */
		rc = packet__read_byte(&context->in_packet, &reason_code);
		만약(rc){
			반환 rc;
		}

		만약(context->in_packet.remaining_length > 1){
			rc = property__read_all(CMD_DISCONNECT, &context->in_packet, &properties);
			만약(rc){
				반환 rc;
			}
		}
	}
	rc = property__process_disconnect(context, &properties);
	만약(rc){
		mosquitto_property_free_all(&properties);
		반환 rc;
	}
	mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */

	만약(context->in_packet.pos != context->in_packet.remaining_length){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->in_packet.pos, context->in_packet.remaining_length);
		반환 MOSQ_ERR_PROTOCOL;
	}
	log__printf(NULL, MOSQ_LOG_DEBUG, "Received DISCONNECT from %s", context->id);
	만약(reason_code == MQTT_RC_DISCONNECT_WITH_WILL_MSG){
		mosquitto__set_state(context, mosq_cs_disconnect_with_will);
	}아니면{
		will__clear(context);
		mosquitto__set_state(context, mosq_cs_disconnecting);
	}
	do_disconnect(context, MOSQ_ERR_SUCCESS);
	반환 MOSQ_ERR_SUCCESS;
}
