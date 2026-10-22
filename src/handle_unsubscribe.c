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

#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "packet_mosq.h"
#ضمن "send_mosq.h"


صحيح handle__unsubscribe(بنية mosquitto *context)
{
	uint16_t mid;
	uint16_t slen;
	صحيح rc;
	uint8_t reason = 0;
	صحيح reason_code_count = 0;
	صحيح reason_code_max;
	uint8_t *reason_codes = عدم, *reason_tmp;
	mosquitto_property *properties = عدم;
	bool allowed;
	بنية mosquitto_subscription sub;

	אם(!context){
		החזר MOSQ_ERR_INVAL;
	}

	אם(context->state != mosq_cs_active){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE before session is active.", context->id);
		החזר MOSQ_ERR_PROTOCOL;
	}
	אם(context->in_packet.command != (CMD_UNSUBSCRIBE|2)){
		החזר MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(عدم, MOSQ_LOG_DEBUG, "Received UNSUBSCRIBE from %s", context->id);

	אם(context->protocol != mosq_p_mqtt31){
		אם((context->in_packet.command&0x0F) != 0x02){
			החזר MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	אם(packet__read_uint16(&context->in_packet, &mid)){
		החזר MOSQ_ERR_MALFORMED_PACKET;
	}
	אם(mid == 0){
		החזר MOSQ_ERR_MALFORMED_PACKET;
	}

	אם(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_UNSUBSCRIBE, &context->in_packet, &properties);
		אם(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			אם(rc == MOSQ_ERR_PROTOCOL){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE packet with invalid properties.", context->id);
				החזר MOSQ_ERR_MALFORMED_PACKET;
			}אחרת{
				החזר rc;
			}
		}
		/* Immediately free, we don't do anything with User Property at the moment */
		mosquitto_property_free_all(&properties);
	}

	אם(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		אם(context->in_packet.pos == context->in_packet.remaining_length){
			/* No topic specified, protocol error. */
			החזר MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	reason_code_max = 10;
	reason_codes = mosquitto_malloc((size_t)reason_code_max);
	אם(!reason_codes){
		החזר MOSQ_ERR_NOMEM;
	}

	כלעוד(context->in_packet.pos < context->in_packet.remaining_length){
		memset(&sub, 0, حجم(sub));
		sub.properties = properties;
		אם(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(reason_codes);
			החזר MOSQ_ERR_MALFORMED_PACKET;
		}

		אם(!slen){
			log__printf(عدم, MOSQ_LOG_INFO,
					"Empty unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			החזר MOSQ_ERR_MALFORMED_PACKET;
		}
		אם(mosquitto_sub_topic_check(sub.topic_filter)){
			log__printf(عدم, MOSQ_LOG_INFO,
					"Invalid unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			החזר MOSQ_ERR_MALFORMED_PACKET;
		}

		/* ACL check */
		allowed = true;
		rc = mosquitto_acl_check(context, sub.topic_filter, 0, عدم, 0, false, properties, MOSQ_ACL_UNSUBSCRIBE);
		بدل(rc){
			حالة MOSQ_ERR_SUCCESS:
				اكسر;
			حالة MOSQ_ERR_ACL_DENIED:
				allowed = false;
				reason = MQTT_RC_NOT_AUTHORIZED;
				اكسر;
			افتراضي:
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				החזר rc;
		}

		log__printf(عدم, MOSQ_LOG_DEBUG, "\t%s", sub.topic_filter);
		אם(allowed){
			rc = plugin__handle_unsubscribe(context, &sub);
			אם(rc){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				החזר rc;
			}
			rc = sub__remove(context, sub.topic_filter, &reason);
			plugin_persist__handle_subscription_delete(context, sub.topic_filter);
		}אחרת{
			rc = MOSQ_ERR_SUCCESS;
		}
		log__printf(عدم, MOSQ_LOG_UNSUBSCRIBE, "%s %s", context->id, sub.topic_filter);
		mosquitto_FREE(sub.topic_filter);
		אם(rc){
			mosquitto_FREE(reason_codes);
			החזר rc;
		}

		reason_codes[reason_code_count] = reason;
		reason_code_count++;
		אם(reason_code_count == reason_code_max){
			reason_tmp = mosquitto_realloc(reason_codes, (size_t)(reason_code_max*2));
			אם(!reason_tmp){
				mosquitto_FREE(reason_codes);
				החזר MOSQ_ERR_NOMEM;
			}
			reason_codes = reason_tmp;
			reason_code_max *= 2;
		}
	}
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	log__printf(عدم, MOSQ_LOG_DEBUG, "Sending UNSUBACK to %s", context->id);

	/* We don't use Reason String or User Property yet. */
	rc = send__unsuback(context, mid, reason_code_count, reason_codes, عدم);
	mosquitto_FREE(reason_codes);
	החזר rc;
}
