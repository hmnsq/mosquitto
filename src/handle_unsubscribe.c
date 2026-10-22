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


ಪೂರ್ಣಾಂಕ handle__unsubscribe(ರಚನೆ mosquitto *context)
{
	uint16_t mid;
	uint16_t slen;
	ಪೂರ್ಣಾಂಕ rc;
	uint8_t reason = 0;
	ಪೂರ್ಣಾಂಕ reason_ಸಂಕೇತ_ಎಣಿಕೆ = 0;
	ಪೂರ್ಣಾಂಕ reason_ಸಂಕೇತ_max;
	uint8_t *reason_codes = ಖಾಲಿ, *reason_tmp;
	mosquitto_property *properties = ಖಾಲಿ;
	bool allowed;
	ರಚನೆ mosquitto_subscription sub;

	ಆದರೆ(!context){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	ಆದರೆ(context->state != mosq_cs_ಸಕ್ರಿಯ){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE before session is active.", context->id);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
	}
	ಆದರೆ(context->ಒಳ_packet.command != (CMD_UNSUBSCRIBE|2)){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "Received UNSUBSCRIBE from %s", context->id);

	ಆದರೆ(context->protocol != mosq_p_mqtt31){
		ಆದರೆ((context->ಒಳ_packet.command&0x0F) != 0x02){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	ಆದರೆ(packet__read_uint16(&context->ಒಳ_packet, &mid)){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
	}
	ಆದರೆ(mid == 0){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
	}

	ಆದರೆ(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_UNSUBSCRIBE, &context->ಒಳ_packet, &properties);
		ಆದರೆ(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			ಆದರೆ(rc == MOSQ_ERR_PROTOCOL){
				log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE packet with invalid properties.", context->id);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಹಿಂತಿರುಗಿ rc;
			}
		}
		/* Immediately free, we don't do anything with User Property at the moment */
		mosquitto_property_free_all(&properties);
	}

	ಆದರೆ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		ಆದರೆ(context->ಒಳ_packet.pos == context->ಒಳ_packet.ಉಳಿದ_ಉದ್ದಳತೆ){
			/* No topic specified, protocol error. */
			ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	reason_ಸಂಕೇತ_max = 10;
	reason_codes = mosquitto_malloc((size_t)reason_ಸಂಕೇತ_max);
	ಆದರೆ(!reason_codes){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	ಆಗ(context->ಒಳ_packet.pos < context->ಒಳ_packet.ಉಳಿದ_ಉದ್ದಳತೆ){
		memset(&sub, 0, ಗಾತ್ರ(sub));
		sub.properties = properties;
		ಆದರೆ(packet__read_string(&context->ಒಳ_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(reason_codes);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
		}

		ಆದರೆ(!slen){
			log__printf(ಖಾಲಿ, MOSQ_LOG_INFO,
					"Empty unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
		}
		ಆದರೆ(mosquitto_sub_topic_check(sub.topic_filter)){
			log__printf(ಖಾಲಿ, MOSQ_LOG_INFO,
					"Invalid unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
		}

		/* ACL check */
		allowed = true;
		rc = mosquitto_acl_check(context, sub.topic_filter, 0, ಖಾಲಿ, 0, false, properties, MOSQ_ACL_UNSUBSCRIBE);
		ಆಯ್ಕೆ(rc){
			ಸ್ಥಿತಿ MOSQ_ERR_SUCCESS:
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ MOSQ_ERR_ACL_DENIED:
				allowed = false;
				reason = MQTT_RC_NOT_AUTHORIZED;
				ನಿಲ್ಲಿಸು;
			ಪೂರ್ವನಿಯೋಜಿತ:
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				ಹಿಂತಿರುಗಿ rc;
		}

		log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "\t%s", sub.topic_filter);
		ಆದರೆ(allowed){
			rc = plugin__handle_unsubscribe(context, &sub);
			ಆದರೆ(rc){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				ಹಿಂತಿರುಗಿ rc;
			}
			rc = sub__remove(context, sub.topic_filter, &reason);
			plugin_persist__handle_subscription_delete(context, sub.topic_filter);
		}ಇಲ್ಲದಿದ್ದರೆ{
			rc = MOSQ_ERR_SUCCESS;
		}
		log__printf(ಖಾಲಿ, MOSQ_LOG_UNSUBSCRIBE, "%s %s", context->id, sub.topic_filter);
		mosquitto_FREE(sub.topic_filter);
		ಆದರೆ(rc){
			mosquitto_FREE(reason_codes);
			ಹಿಂತಿರುಗಿ rc;
		}

		reason_codes[reason_ಸಂಕೇತ_ಎಣಿಕೆ] = reason;
		reason_ಸಂಕೇತ_ಎಣಿಕೆ++;
		ಆದರೆ(reason_ಸಂಕೇತ_ಎಣಿಕೆ == reason_ಸಂಕೇತ_max){
			reason_tmp = mosquitto_realloc(reason_codes, (size_t)(reason_ಸಂಕೇತ_max*2));
			ಆದರೆ(!reason_tmp){
				mosquitto_FREE(reason_codes);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
			reason_codes = reason_tmp;
			reason_ಸಂಕೇತ_max *= 2;
		}
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_PERSISTENCE
	db.persistence_changes++;
#ಆದರೆ_ಅಂತ್ಯ

	log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "Sending UNSUBACK to %s", context->id);

	/* We don't use Reason String or User Property yet. */
	rc = send__unsuback(context, mid, reason_ಸಂಕೇತ_ಎಣಿಕೆ, reason_codes, ಖಾಲಿ);
	mosquitto_FREE(reason_codes);
	ಹಿಂತಿರುಗಿ rc;
}
