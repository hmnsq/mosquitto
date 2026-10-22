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

#అన్తర్భావ "config.h"

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "packet_mosq.h"
#అన్తర్భావ "property_mosq.h"
#అన్తర్భావ "send_mosq.h"
#అన్తర్భావ "util_mosq.h"
#అన్తర్భావ "will_mosq.h"


పూర్ణాంకం handle__disconnect(నిర్మాణం mosquitto *context)
{
	పూర్ణాంకం rc;
	uint8_t reason_సంకేతం = 0;
	mosquitto_property *properties = ఖాళీ;

	ఒకవేళ(!context){
		తిరిగి MOSQ_ERR_INVAL;
	}

	ఒకవేళ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		ఒకవేళ((context->లోపల_packet.command&0x0F) != 0x00){
			log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with incorrect flags %02X.",
					context->id, context->లోపల_packet.command);
			తిరిగి MOSQ_ERR_PROTOCOL;
		}
	}

	ఒకవేళ(context->protocol == mosq_p_mqtt5 && context->లోపల_packet.మిగిలిన_నిడివి > 0){
		/* FIXME - must handle reason code */
		rc = packet__read_byte(&context->లోపల_packet, &reason_సంకేతం);
		ఒకవేళ(rc){
			తిరిగి rc;
		}

		ఒకవేళ(context->లోపల_packet.మిగిలిన_నిడివి > 1){
			rc = property__read_all(CMD_DISCONNECT, &context->లోపల_packet, &properties);
			ఒకవేళ(rc){
				తిరిగి rc;
			}
		}
	}
	rc = property__process_disconnect(context, &properties);
	ఒకవేళ(rc){
		mosquitto_property_free_all(&properties);
		తిరిగి rc;
	}
	mosquitto_property_free_all(&properties); /* FIXME - TEMPORARY UNTIL PROPERTIES PROCESSED */

	ఒకవేళ(context->లోపల_packet.pos != context->లోపల_packet.మిగిలిన_నిడివి){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with overlong remaining length (%d:%d).",
				context->id, context->లోపల_packet.pos, context->లోపల_packet.మిగిలిన_నిడివి);
		తిరిగి MOSQ_ERR_PROTOCOL;
	}
	log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "Received DISCONNECT from %s", context->id);
	ఒకవేళ(reason_సంకేతం == MQTT_RC_DISCONNECT_WITH_WILL_సందేశం){
		mosquitto_సమితి_state(context, mosq_cs_disconnect_with_will);
	}లేకపోతే{
		will__clear(context);
		mosquitto_సమితి_state(context, mosq_cs_disconnecting);
	}
	do_disconnect(context, MOSQ_ERR_SUCCESS);
	తిరిగి MOSQ_ERR_SUCCESS;
}
