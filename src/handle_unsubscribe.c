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

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "packet_mosq.h"
#অন্তর্ভাব "send_mosq.h"


পূর্ণসংখ্যা handle__unsubscribe(কাঠামো mosquitto *context)
{
	uint16_t mid;
	uint16_t slen;
	পূর্ণসংখ্যা rc;
	uint8_t reason = 0;
	পূর্ণসংখ্যা reason_code_গুনতি = 0;
	পূর্ণসংখ্যা reason_code_max;
	uint8_t *reason_codes = ফাঁকা, *reason_tmp;
	mosquitto_property *properties = ফাঁকা;
	bool allowed;
	কাঠামো mosquitto_subscription sub;

	যদি(!context){
		ফেরত MOSQ_ERR_INVAL;
	}

	যদি(context->state != mosq_cs_active){
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE before session is active.", context->id);
		ফেরত MOSQ_ERR_PROTOCOL;
	}
	যদি(context->in_packet.command != (CMD_UNSUBSCRIBE|2)){
		ফেরত MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(ফাঁকা, MOSQ_LOG_DEBUG, "Received UNSUBSCRIBE from %s", context->id);

	যদি(context->protocol != mosq_p_mqtt31){
		যদি((context->in_packet.command&0x0F) != 0x02){
			ফেরত MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	যদি(packet__read_uint16(&context->in_packet, &mid)){
		ফেরত MOSQ_ERR_MALFORMED_PACKET;
	}
	যদি(mid == 0){
		ফেরত MOSQ_ERR_MALFORMED_PACKET;
	}

	যদি(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_UNSUBSCRIBE, &context->in_packet, &properties);
		যদি(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			যদি(rc == MOSQ_ERR_PROTOCOL){
				log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE packet with invalid properties.", context->id);
				ফেরত MOSQ_ERR_MALFORMED_PACKET;
			}অন্যথা{
				ফেরত rc;
			}
		}
		/* Immediately free, we don't do anything with User Property at the moment */
		mosquitto_property_free_all(&properties);
	}

	যদি(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		যদি(context->in_packet.pos == context->in_packet.remaining_দৈর্ঘ্য){
			/* No topic specified, protocol error. */
			ফেরত MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	reason_code_max = 10;
	reason_codes = mosquitto_malloc((size_t)reason_code_max);
	যদি(!reason_codes){
		ফেরত MOSQ_ERR_NOMEM;
	}

	যখন(context->in_packet.pos < context->in_packet.remaining_দৈর্ঘ্য){
		memset(&sub, 0, আকার(sub));
		sub.properties = properties;
		যদি(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(reason_codes);
			ফেরত MOSQ_ERR_MALFORMED_PACKET;
		}

		যদি(!slen){
			log__printf(ফাঁকা, MOSQ_LOG_INFO,
					"Empty unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			ফেরত MOSQ_ERR_MALFORMED_PACKET;
		}
		যদি(mosquitto_sub_topic_check(sub.topic_filter)){
			log__printf(ফাঁকা, MOSQ_LOG_INFO,
					"Invalid unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			ফেরত MOSQ_ERR_MALFORMED_PACKET;
		}

		/* ACL check */
		allowed = true;
		rc = mosquitto_acl_check(context, sub.topic_filter, 0, ফাঁকা, 0, false, properties, MOSQ_ACL_UNSUBSCRIBE);
		নির্বাচন(rc){
			ক্ষেত্র MOSQ_ERR_SUCCESS:
				বিরতি;
			ক্ষেত্র MOSQ_ERR_ACL_DENIED:
				allowed = false;
				reason = MQTT_RC_NOT_AUTHORIZED;
				বিরতি;
			পূর্বনির্ধারিত:
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				ফেরত rc;
		}

		log__printf(ফাঁকা, MOSQ_LOG_DEBUG, "\t%s", sub.topic_filter);
		যদি(allowed){
			rc = plugin__handle_unsubscribe(context, &sub);
			যদি(rc){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				ফেরত rc;
			}
			rc = sub__remove(context, sub.topic_filter, &reason);
			plugin_persist__handle_subscription_delete(context, sub.topic_filter);
		}অন্যথা{
			rc = MOSQ_ERR_SUCCESS;
		}
		log__printf(ফাঁকা, MOSQ_LOG_UNSUBSCRIBE, "%s %s", context->id, sub.topic_filter);
		mosquitto_FREE(sub.topic_filter);
		যদি(rc){
			mosquitto_FREE(reason_codes);
			ফেরত rc;
		}

		reason_codes[reason_code_গুনতি] = reason;
		reason_code_গুনতি++;
		যদি(reason_code_গুনতি == reason_code_max){
			reason_tmp = mosquitto_realloc(reason_codes, (size_t)(reason_code_max*2));
			যদি(!reason_tmp){
				mosquitto_FREE(reason_codes);
				ফেরত MOSQ_ERR_NOMEM;
			}
			reason_codes = reason_tmp;
			reason_code_max *= 2;
		}
	}
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	log__printf(ফাঁকা, MOSQ_LOG_DEBUG, "Sending UNSUBACK to %s", context->id);

	/* We don't use Reason String or User Property yet. */
	rc = send__unsuback(context, mid, reason_code_গুনতি, reason_codes, ফাঁকা);
	mosquitto_FREE(reason_codes);
	ফেরত rc;
}
