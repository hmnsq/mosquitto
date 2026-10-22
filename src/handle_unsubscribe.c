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

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "packet_mosq.h"
#అన్తర్భావ "send_mosq.h"


పూర్ణాంకం handle__unsubscribe(నిర్మాణం mosquitto *context)
{
	uint16_t mid;
	uint16_t slen;
	పూర్ణాంకం rc;
	uint8_t reason = 0;
	పూర్ణాంకం reason_సంకేతం_లెక్క = 0;
	పూర్ణాంకం reason_సంకేతం_max;
	uint8_t *reason_codes = ఖాళీ, *reason_tmp;
	mosquitto_property *properties = ఖాళీ;
	bool allowed;
	నిర్మాణం mosquitto_subscription sub;

	ఒకవేళ(!context){
		తిరిగి MOSQ_ERR_INVAL;
	}

	ఒకవేళ(context->state != mosq_cs_చురుకైన){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE before session is active.", context->id);
		తిరిగి MOSQ_ERR_PROTOCOL;
	}
	ఒకవేళ(context->లోపల_packet.command != (CMD_UNSUBSCRIBE|2)){
		తిరిగి MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "Received UNSUBSCRIBE from %s", context->id);

	ఒకవేళ(context->protocol != mosq_p_mqtt31){
		ఒకవేళ((context->లోపల_packet.command&0x0F) != 0x02){
			తిరిగి MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	ఒకవేళ(packet__read_uint16(&context->లోపల_packet, &mid)){
		తిరిగి MOSQ_ERR_MALFORMED_PACKET;
	}
	ఒకవేళ(mid == 0){
		తిరిగి MOSQ_ERR_MALFORMED_PACKET;
	}

	ఒకవేళ(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_UNSUBSCRIBE, &context->లోపల_packet, &properties);
		ఒకవేళ(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			ఒకవేళ(rc == MOSQ_ERR_PROTOCOL){
				log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE packet with invalid properties.", context->id);
				తిరిగి MOSQ_ERR_MALFORMED_PACKET;
			}లేకపోతే{
				తిరిగి rc;
			}
		}
		/* Immediately free, we don't do anything with User Property at the moment */
		mosquitto_property_free_all(&properties);
	}

	ఒకవేళ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		ఒకవేళ(context->లోపల_packet.pos == context->లోపల_packet.మిగిలిన_నిడివి){
			/* No topic specified, protocol error. */
			తిరిగి MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	reason_సంకేతం_max = 10;
	reason_codes = mosquitto_malloc((size_t)reason_సంకేతం_max);
	ఒకవేళ(!reason_codes){
		తిరిగి MOSQ_ERR_NOMEM;
	}

	అయితే(context->లోపల_packet.pos < context->లోపల_packet.మిగిలిన_నిడివి){
		memset(&sub, 0, పరిమాణం(sub));
		sub.properties = properties;
		ఒకవేళ(packet__read_string(&context->లోపల_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(reason_codes);
			తిరిగి MOSQ_ERR_MALFORMED_PACKET;
		}

		ఒకవేళ(!slen){
			log__printf(ఖాళీ, MOSQ_LOG_INFO,
					"Empty unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			తిరిగి MOSQ_ERR_MALFORMED_PACKET;
		}
		ఒకవేళ(mosquitto_sub_topic_check(sub.topic_filter)){
			log__printf(ఖాళీ, MOSQ_LOG_INFO,
					"Invalid unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			తిరిగి MOSQ_ERR_MALFORMED_PACKET;
		}

		/* ACL check */
		allowed = true;
		rc = mosquitto_acl_check(context, sub.topic_filter, 0, ఖాళీ, 0, false, properties, MOSQ_ACL_UNSUBSCRIBE);
		ఎంపిక(rc){
			స్థితి MOSQ_ERR_SUCCESS:
				ఆపు;
			స్థితి MOSQ_ERR_ACL_DENIED:
				allowed = false;
				reason = MQTT_RC_NOT_AUTHORIZED;
				ఆపు;
			అప్రమేయ:
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				తిరిగి rc;
		}

		log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "\t%s", sub.topic_filter);
		ఒకవేళ(allowed){
			rc = plugin__handle_unsubscribe(context, &sub);
			ఒకవేళ(rc){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				తిరిగి rc;
			}
			rc = sub__remove(context, sub.topic_filter, &reason);
			plugin_persist__handle_subscription_delete(context, sub.topic_filter);
		}లేకపోతే{
			rc = MOSQ_ERR_SUCCESS;
		}
		log__printf(ఖాళీ, MOSQ_LOG_UNSUBSCRIBE, "%s %s", context->id, sub.topic_filter);
		mosquitto_FREE(sub.topic_filter);
		ఒకవేళ(rc){
			mosquitto_FREE(reason_codes);
			తిరిగి rc;
		}

		reason_codes[reason_సంకేతం_లెక్క] = reason;
		reason_సంకేతం_లెక్క++;
		ఒకవేళ(reason_సంకేతం_లెక్క == reason_సంకేతం_max){
			reason_tmp = mosquitto_realloc(reason_codes, (size_t)(reason_సంకేతం_max*2));
			ఒకవేళ(!reason_tmp){
				mosquitto_FREE(reason_codes);
				తిరిగి MOSQ_ERR_NOMEM;
			}
			reason_codes = reason_tmp;
			reason_సంకేతం_max *= 2;
		}
	}
#నిర్వచితమైతే WITH_PERSISTENCE
	db.persistence_changes++;
#అయితే_ముగింపు

	log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "Sending UNSUBACK to %s", context->id);

	/* We don't use Reason String or User Property yet. */
	rc = send__unsuback(context, mid, reason_సంకేతం_లెక్క, reason_codes, ఖాళీ);
	mosquitto_FREE(reason_codes);
	తిరిగి rc;
}
