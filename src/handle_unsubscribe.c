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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "send_mosq.h"


ਪੂਰਨ handle__unsubscribe(ਬਣਤਰ mosquitto *context)
{
	uint16_t mid;
	uint16_t slen;
	ਪੂਰਨ rc;
	uint8_t reason = 0;
	ਪੂਰਨ reason_ਕੋਡ_ਗਿਣਤੀ = 0;
	ਪੂਰਨ reason_ਕੋਡ_max;
	uint8_t *reason_codes = ਨਿਰਾ, *reason_tmp;
	mosquitto_property *properties = ਨਿਰਾ;
	bool allowed;
	ਬਣਤਰ mosquitto_subscription sub;

	ਜੇ(!context){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਜੇ(context->state != mosq_cs_ਸਰਗਰਮ){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE before session is active.", context->id);
		ਵਾਪਸ MOSQ_ERR_PROTOCOL;
	}
	ਜੇ(context->ਵਿੱਚ_packet.command != (CMD_UNSUBSCRIBE|2)){
		ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Received UNSUBSCRIBE from %s", context->id);

	ਜੇ(context->protocol != mosq_p_mqtt31){
		ਜੇ((context->ਵਿੱਚ_packet.command&0x0F) != 0x02){
			ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	ਜੇ(packet__read_uint16(&context->ਵਿੱਚ_packet, &mid)){
		ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
	}
	ਜੇ(mid == 0){
		ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
	}

	ਜੇ(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_UNSUBSCRIBE, &context->ਵਿੱਚ_packet, &properties);
		ਜੇ(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			ਜੇ(rc == MOSQ_ERR_PROTOCOL){
				log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE packet with invalid properties.", context->id);
				ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
			}ਨਹੀਂਤਾਂ{
				ਵਾਪਸ rc;
			}
		}
		/* Immediately free, we don't do anything with User Property at the moment */
		mosquitto_property_free_all(&properties);
	}

	ਜੇ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		ਜੇ(context->ਵਿੱਚ_packet.pos == context->ਵਿੱਚ_packet.ਬਚਿਆ_ਲੰਮਾਈ){
			/* No topic specified, protocol error. */
			ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	reason_ਕੋਡ_max = 10;
	reason_codes = mosquitto_malloc((size_t)reason_ਕੋਡ_max);
	ਜੇ(!reason_codes){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	ਜਦੋਂ(context->ਵਿੱਚ_packet.pos < context->ਵਿੱਚ_packet.ਬਚਿਆ_ਲੰਮਾਈ){
		memset(&sub, 0, ਆਕਾਰ(sub));
		sub.properties = properties;
		ਜੇ(packet__read_string(&context->ਵਿੱਚ_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(reason_codes);
			ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
		}

		ਜੇ(!slen){
			log__printf(ਨਿਰਾ, MOSQ_LOG_INFO,
					"Empty unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
		}
		ਜੇ(mosquitto_sub_topic_check(sub.topic_filter)){
			log__printf(ਨਿਰਾ, MOSQ_LOG_INFO,
					"Invalid unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
		}

		/* ACL check */
		allowed = true;
		rc = mosquitto_acl_check(context, sub.topic_filter, 0, ਨਿਰਾ, 0, false, properties, MOSQ_ACL_UNSUBSCRIBE);
		ਚੋਣ(rc){
			ਹਾਲਤ MOSQ_ERR_SUCCESS:
				ਰੋਕੋ;
			ਹਾਲਤ MOSQ_ERR_ACL_DENIED:
				allowed = false;
				reason = MQTT_RC_NOT_AUTHORIZED;
				ਰੋਕੋ;
			ਮੂਲ:
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				ਵਾਪਸ rc;
		}

		log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "\t%s", sub.topic_filter);
		ਜੇ(allowed){
			rc = plugin__handle_unsubscribe(context, &sub);
			ਜੇ(rc){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				ਵਾਪਸ rc;
			}
			rc = sub__remove(context, sub.topic_filter, &reason);
			plugin_persist__handle_subscription_delete(context, sub.topic_filter);
		}ਨਹੀਂਤਾਂ{
			rc = MOSQ_ERR_SUCCESS;
		}
		log__printf(ਨਿਰਾ, MOSQ_LOG_UNSUBSCRIBE, "%s %s", context->id, sub.topic_filter);
		mosquitto_FREE(sub.topic_filter);
		ਜੇ(rc){
			mosquitto_FREE(reason_codes);
			ਵਾਪਸ rc;
		}

		reason_codes[reason_ਕੋਡ_ਗਿਣਤੀ] = reason;
		reason_ਕੋਡ_ਗਿਣਤੀ++;
		ਜੇ(reason_ਕੋਡ_ਗਿਣਤੀ == reason_ਕੋਡ_max){
			reason_tmp = mosquitto_realloc(reason_codes, (size_t)(reason_ਕੋਡ_max*2));
			ਜੇ(!reason_tmp){
				mosquitto_FREE(reason_codes);
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
			reason_codes = reason_tmp;
			reason_ਕੋਡ_max *= 2;
		}
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE
	db.persistence_changes++;
#ਜੇ_ਅੰਤ

	log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Sending UNSUBACK to %s", context->id);

	/* We don't use Reason String or User Property yet. */
	rc = send__unsuback(context, mid, reason_ਕੋਡ_ਗਿਣਤੀ, reason_codes, ਨਿਰਾ);
	mosquitto_FREE(reason_codes);
	ਵਾਪਸ rc;
}
