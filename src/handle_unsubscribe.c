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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "packet_mosq.h"
#ଅନ୍ତର୍ଭାଵ "send_mosq.h"


ପୂର୍ଣ୍ଣସଂଖ୍ୟା handle__unsubscribe(ସଂରଚନା mosquitto *context)
{
	uint16_t mid;
	uint16_t slen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	uint8_t reason = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା reason_code_ଗଣନା = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା reason_code_max;
	uint8_t *reason_codes = ଖାଲି, *reason_tmp;
	mosquitto_property *properties = ଖାଲି;
	bool allowed;
	ସଂରଚନା mosquitto_subscription sub;

	ଯଦି(!context){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	ଯଦି(context->state != mosq_cs_active){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE before session is active.", context->id);
		ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
	}
	ଯଦି(context->in_packet.command != (CMD_UNSUBSCRIBE|2)){
		ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "Received UNSUBSCRIBE from %s", context->id);

	ଯଦି(context->protocol != mosq_p_mqtt31){
		ଯଦି((context->in_packet.command&0x0F) != 0x02){
			ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	ଯଦି(packet__read_uint16(&context->in_packet, &mid)){
		ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
	}
	ଯଦି(mid == 0){
		ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
	}

	ଯଦି(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_UNSUBSCRIBE, &context->in_packet, &properties);
		ଯଦି(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			ଯଦି(rc == MOSQ_ERR_PROTOCOL){
				log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE packet with invalid properties.", context->id);
				ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
			}ଅନ୍ୟଥା{
				ଫେରନ୍ତୁ rc;
			}
		}
		/* Immediately free, we don't do anything with User Property at the moment */
		mosquitto_property_free_all(&properties);
	}

	ଯଦି(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		ଯଦି(context->in_packet.pos == context->in_packet.remaining_length){
			/* No topic specified, protocol error. */
			ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	reason_code_max = 10;
	reason_codes = mosquitto_malloc((size_t)reason_code_max);
	ଯଦି(!reason_codes){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	ଯେତେବେଳେ(context->in_packet.pos < context->in_packet.remaining_length){
		memset(&sub, 0, ଆକାର(sub));
		sub.properties = properties;
		ଯଦି(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(reason_codes);
			ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
		}

		ଯଦି(!slen){
			log__printf(ଖାଲି, MOSQ_LOG_INFO,
					"Empty unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
		}
		ଯଦି(mosquitto_sub_topic_check(sub.topic_filter)){
			log__printf(ଖାଲି, MOSQ_LOG_INFO,
					"Invalid unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
		}

		/* ACL check */
		allowed = true;
		rc = mosquitto_acl_check(context, sub.topic_filter, 0, ଖାଲି, 0, false, properties, MOSQ_ACL_UNSUBSCRIBE);
		ବାଛ(rc){
			ସ୍ଥିତି MOSQ_ERR_SUCCESS:
				ଭାଙ୍ଗ;
			ସ୍ଥିତି MOSQ_ERR_ACL_DENIED:
				allowed = false;
				reason = MQTT_RC_NOT_AUTHORIZED;
				ଭାଙ୍ଗ;
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				ଫେରନ୍ତୁ rc;
		}

		log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "\t%s", sub.topic_filter);
		ଯଦି(allowed){
			rc = plugin__handle_unsubscribe(context, &sub);
			ଯଦି(rc){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				ଫେରନ୍ତୁ rc;
			}
			rc = sub__remove(context, sub.topic_filter, &reason);
			plugin_persist__handle_subscription_delete(context, sub.topic_filter);
		}ଅନ୍ୟଥା{
			rc = MOSQ_ERR_SUCCESS;
		}
		log__printf(ଖାଲି, MOSQ_LOG_UNSUBSCRIBE, "%s %s", context->id, sub.topic_filter);
		mosquitto_FREE(sub.topic_filter);
		ଯଦି(rc){
			mosquitto_FREE(reason_codes);
			ଫେରନ୍ତୁ rc;
		}

		reason_codes[reason_code_ଗଣନା] = reason;
		reason_code_ଗଣନା++;
		ଯଦି(reason_code_ଗଣନା == reason_code_max){
			reason_tmp = mosquitto_realloc(reason_codes, (size_t)(reason_code_max*2));
			ଯଦି(!reason_tmp){
				mosquitto_FREE(reason_codes);
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
			reason_codes = reason_tmp;
			reason_code_max *= 2;
		}
	}
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "Sending UNSUBACK to %s", context->id);

	/* We don't use Reason String or User Property yet. */
	rc = send__unsuback(context, mid, reason_code_ଗଣନା, reason_codes, ଖାଲି);
	mosquitto_FREE(reason_codes);
	ଫେରନ୍ତୁ rc;
}
