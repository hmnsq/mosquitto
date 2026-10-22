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

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "packet_mosq.h"
#અન્તર્ભાવ "send_mosq.h"


પૂર્ણાંક handle__unsubscribe(સંરચના mosquitto *context)
{
	uint16_t mid;
	uint16_t slen;
	પૂર્ણાંક rc;
	uint8_t reason = 0;
	પૂર્ણાંક reason_code_ગણતરી = 0;
	પૂર્ણાંક reason_code_max;
	uint8_t *reason_codes = ખાલી, *reason_tmp;
	mosquitto_property *properties = ખાલી;
	bool allowed;
	સંરચના mosquitto_subscription sub;

	જો(!context){
		પાછા MOSQ_ERR_INVAL;
	}

	જો(context->state != mosq_cs_active){
		log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE before session is active.", context->id);
		પાછા MOSQ_ERR_PROTOCOL;
	}
	જો(context->in_packet.command != (CMD_UNSUBSCRIBE|2)){
		પાછા MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(ખાલી, MOSQ_LOG_DEBUG, "Received UNSUBSCRIBE from %s", context->id);

	જો(context->protocol != mosq_p_mqtt31){
		જો((context->in_packet.command&0x0F) != 0x02){
			પાછા MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	જો(packet__read_uint16(&context->in_packet, &mid)){
		પાછા MOSQ_ERR_MALFORMED_PACKET;
	}
	જો(mid == 0){
		પાછા MOSQ_ERR_MALFORMED_PACKET;
	}

	જો(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_UNSUBSCRIBE, &context->in_packet, &properties);
		જો(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			જો(rc == MOSQ_ERR_PROTOCOL){
				log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE packet with invalid properties.", context->id);
				પાછા MOSQ_ERR_MALFORMED_PACKET;
			}નહીંતર{
				પાછા rc;
			}
		}
		/* Immediately free, we don't do anything with User Property at the moment */
		mosquitto_property_free_all(&properties);
	}

	જો(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		જો(context->in_packet.pos == context->in_packet.remaining_length){
			/* No topic specified, protocol error. */
			પાછા MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	reason_code_max = 10;
	reason_codes = mosquitto_malloc((size_t)reason_code_max);
	જો(!reason_codes){
		પાછા MOSQ_ERR_NOMEM;
	}

	જ્યારે(context->in_packet.pos < context->in_packet.remaining_length){
		memset(&sub, 0, કદ(sub));
		sub.properties = properties;
		જો(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(reason_codes);
			પાછા MOSQ_ERR_MALFORMED_PACKET;
		}

		જો(!slen){
			log__printf(ખાલી, MOSQ_LOG_INFO,
					"Empty unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			પાછા MOSQ_ERR_MALFORMED_PACKET;
		}
		જો(mosquitto_sub_topic_check(sub.topic_filter)){
			log__printf(ખાલી, MOSQ_LOG_INFO,
					"Invalid unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			પાછા MOSQ_ERR_MALFORMED_PACKET;
		}

		/* ACL check */
		allowed = true;
		rc = mosquitto_acl_check(context, sub.topic_filter, 0, ખાલી, 0, false, properties, MOSQ_ACL_UNSUBSCRIBE);
		પસંદગી(rc){
			સ્થિતિ MOSQ_ERR_SUCCESS:
				તોડો;
			સ્થિતિ MOSQ_ERR_ACL_DENIED:
				allowed = false;
				reason = MQTT_RC_NOT_AUTHORIZED;
				તોડો;
			મૂળભૂત:
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				પાછા rc;
		}

		log__printf(ખાલી, MOSQ_LOG_DEBUG, "\t%s", sub.topic_filter);
		જો(allowed){
			rc = plugin__handle_unsubscribe(context, &sub);
			જો(rc){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				પાછા rc;
			}
			rc = sub__remove(context, sub.topic_filter, &reason);
			plugin_persist__handle_subscription_delete(context, sub.topic_filter);
		}નહીંતર{
			rc = MOSQ_ERR_SUCCESS;
		}
		log__printf(ખાલી, MOSQ_LOG_UNSUBSCRIBE, "%s %s", context->id, sub.topic_filter);
		mosquitto_FREE(sub.topic_filter);
		જો(rc){
			mosquitto_FREE(reason_codes);
			પાછા rc;
		}

		reason_codes[reason_code_ગણતરી] = reason;
		reason_code_ગણતરી++;
		જો(reason_code_ગણતરી == reason_code_max){
			reason_tmp = mosquitto_realloc(reason_codes, (size_t)(reason_code_max*2));
			જો(!reason_tmp){
				mosquitto_FREE(reason_codes);
				પાછા MOSQ_ERR_NOMEM;
			}
			reason_codes = reason_tmp;
			reason_code_max *= 2;
		}
	}
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	log__printf(ખાલી, MOSQ_LOG_DEBUG, "Sending UNSUBACK to %s", context->id);

	/* We don't use Reason String or User Property yet. */
	rc = send__unsuback(context, mid, reason_code_ગણતરી, reason_codes, ખાલી);
	mosquitto_FREE(reason_codes);
	પાછા rc;
}
