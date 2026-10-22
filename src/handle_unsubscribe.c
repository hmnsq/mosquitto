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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"


ಪೂರ್ಣಾಙ್ಕ handle__unsubscribe(ಸಂರಚನಾ mosquitto *context)
{
	uint16_t mid;
	uint16_t slen;
	ಪೂರ್ಣಾಙ್ಕ rc;
	uint8_t reason = 0;
	ಪೂರ್ಣಾಙ್ಕ reason_code_ಎಣಿಕೆ = 0;
	ಪೂರ್ಣಾಙ್ಕ reason_code_max;
	uint8_t *reason_codes = ರಿಕ್ತ, *reason_tmp;
	mosquitto_property *properties = ರಿಕ್ತ;
	bool allowed;
	ಸಂರಚನಾ mosquitto_subscription sub;

	ಯದಿ(!context){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	ಯದಿ(context->state != mosq_cs_active){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE before session is active.", context->id);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
	}
	ಯದಿ(context->in_packet.command != (CMD_UNSUBSCRIBE|2)){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "Received UNSUBSCRIBE from %s", context->id);

	ಯದಿ(context->protocol != mosq_p_mqtt31){
		ಯದಿ((context->in_packet.command&0x0F) != 0x02){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	ಯದಿ(packet__read_uint16(&context->in_packet, &mid)){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
	}
	ಯದಿ(mid == 0){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
	}

	ಯದಿ(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_UNSUBSCRIBE, &context->in_packet, &properties);
		ಯದಿ(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			ಯದಿ(rc == MOSQ_ERR_PROTOCOL){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE packet with invalid properties.", context->id);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
			}ಅನ್ಯಥಾ{
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}
		/* Immediately free, we don't do anything with User Property at the moment */
		mosquitto_property_free_all(&properties);
	}

	ಯದಿ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		ಯದಿ(context->in_packet.pos == context->in_packet.remaining_length){
			/* No topic specified, protocol error. */
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	reason_code_max = 10;
	reason_codes = mosquitto_malloc((size_t)reason_code_max);
	ಯದಿ(!reason_codes){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	ಯಾವತ್(context->in_packet.pos < context->in_packet.remaining_length){
		memset(&sub, 0, ಪರಿಮಾಣ(sub));
		sub.properties = properties;
		ಯದಿ(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(reason_codes);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
		}

		ಯದಿ(!slen){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO,
					"Empty unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
		}
		ಯದಿ(mosquitto_sub_topic_check(sub.topic_filter)){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO,
					"Invalid unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
		}

		/* ACL check */
		allowed = true;
		rc = mosquitto_acl_check(context, sub.topic_filter, 0, ರಿಕ್ತ, 0, false, properties, MOSQ_ACL_UNSUBSCRIBE);
		ವಿಕಲ್ಪ(rc){
			ಪಕ್ಷ MOSQ_ERR_SUCCESS:
				ಭಙ್ಗ;
			ಪಕ್ಷ MOSQ_ERR_ACL_DENIED:
				allowed = false;
				reason = MQTT_RC_NOT_AUTHORIZED;
				ಭಙ್ಗ;
			ಸ್ವಭಾವ:
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				ಪ್ರತ್ಯಾಗಮ rc;
		}

		log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "\t%s", sub.topic_filter);
		ಯದಿ(allowed){
			rc = plugin__handle_unsubscribe(context, &sub);
			ಯದಿ(rc){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				ಪ್ರತ್ಯಾಗಮ rc;
			}
			rc = sub__remove(context, sub.topic_filter, &reason);
			plugin_persist__handle_subscription_delete(context, sub.topic_filter);
		}ಅನ್ಯಥಾ{
			rc = MOSQ_ERR_SUCCESS;
		}
		log__printf(ರಿಕ್ತ, MOSQ_LOG_UNSUBSCRIBE, "%s %s", context->id, sub.topic_filter);
		mosquitto_FREE(sub.topic_filter);
		ಯದಿ(rc){
			mosquitto_FREE(reason_codes);
			ಪ್ರತ್ಯಾಗಮ rc;
		}

		reason_codes[reason_code_ಎಣಿಕೆ] = reason;
		reason_code_ಎಣಿಕೆ++;
		ಯದಿ(reason_code_ಎಣಿಕೆ == reason_code_max){
			reason_tmp = mosquitto_realloc(reason_codes, (size_t)(reason_code_max*2));
			ಯದಿ(!reason_tmp){
				mosquitto_FREE(reason_codes);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
			reason_codes = reason_tmp;
			reason_code_max *= 2;
		}
	}
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "Sending UNSUBACK to %s", context->id);

	/* We don't use Reason String or User Property yet. */
	rc = send__unsuback(context, mid, reason_code_ಎಣಿಕೆ, reason_codes, ರಿಕ್ತ);
	mosquitto_FREE(reason_codes);
	ಪ್ರತ್ಯಾಗಮ rc;
}
