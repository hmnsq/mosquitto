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

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "send_mosq.h"


पूर्णाङ्क handle__unsubscribe(संरचना mosquitto *context)
{
	uint16_t mid;
	uint16_t slen;
	पूर्णाङ्क rc;
	uint8_t reason = 0;
	पूर्णाङ्क reason_संहिता_गणनम् = 0;
	पूर्णाङ्क reason_संहिता_max;
	uint8_t *reason_codes = रिक्त, *reason_tmp;
	mosquitto_property *properties = रिक्त;
	bool allowed;
	संरचना mosquitto_subscription sub;

	यदि(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	यदि(context->state != mosq_cs_सक्रिय){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE before session is active.", context->id);
		प्रत्यागम MOSQ_ERR_PROTOCOL;
	}
	यदि(context->अन्तर्_packet.command != (CMD_UNSUBSCRIBE|2)){
		प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(रिक्त, MOSQ_LOG_DEBUG, "Received UNSUBSCRIBE from %s", context->id);

	यदि(context->protocol != mosq_p_mqtt31){
		यदि((context->अन्तर्_packet.command&0x0F) != 0x02){
			प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	यदि(packet__read_uint16(&context->अन्तर्_packet, &mid)){
		प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
	}
	यदि(mid == 0){
		प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
	}

	यदि(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_UNSUBSCRIBE, &context->अन्तर्_packet, &properties);
		यदि(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			यदि(rc == MOSQ_ERR_PROTOCOL){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE packet with invalid properties.", context->id);
				प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
			}अन्यथा{
				प्रत्यागम rc;
			}
		}
		/* Immediately free, we don't do anything with User Property at the moment */
		mosquitto_property_free_all(&properties);
	}

	यदि(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		यदि(context->अन्तर्_packet.pos == context->अन्तर्_packet.remaining_दीर्घता){
			/* No topic specified, protocol error. */
			प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	reason_संहिता_max = 10;
	reason_codes = mosquitto_malloc((size_t)reason_संहिता_max);
	यदि(!reason_codes){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	यावत्(context->अन्तर्_packet.pos < context->अन्तर्_packet.remaining_दीर्घता){
		स्मृतिं_पूरय(&sub, 0, परिमाण(sub));
		sub.properties = properties;
		यदि(packet__read_string(&context->अन्तर्_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(reason_codes);
			प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
		}

		यदि(!slen){
			log__printf(रिक्त, MOSQ_LOG_INFO,
					"Empty unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
		}
		यदि(mosquitto_sub_topic_check(sub.topic_filter)){
			log__printf(रिक्त, MOSQ_LOG_INFO,
					"Invalid unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
		}

		/* ACL check */
		allowed = true;
		rc = mosquitto_acl_check(context, sub.topic_filter, 0, रिक्त, 0, false, properties, MOSQ_ACL_UNSUBSCRIBE);
		विकल्प(rc){
			पक्ष MOSQ_ERR_SUCCESS:
				भङ्ग;
			पक्ष MOSQ_ERR_ACL_DENIED:
				allowed = false;
				reason = MQTT_RC_NOT_AUTHORIZED;
				भङ्ग;
			स्वभाव:
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				प्रत्यागम rc;
		}

		log__printf(रिक्त, MOSQ_LOG_DEBUG, "\t%s", sub.topic_filter);
		यदि(allowed){
			rc = plugin__handle_unsubscribe(context, &sub);
			यदि(rc){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				प्रत्यागम rc;
			}
			rc = sub__remove(context, sub.topic_filter, &reason);
			plugin_persist__handle_subscription_delete(context, sub.topic_filter);
		}अन्यथा{
			rc = MOSQ_ERR_SUCCESS;
		}
		log__printf(रिक्त, MOSQ_LOG_UNSUBSCRIBE, "%s %s", context->id, sub.topic_filter);
		mosquitto_FREE(sub.topic_filter);
		यदि(rc){
			mosquitto_FREE(reason_codes);
			प्रत्यागम rc;
		}

		reason_codes[reason_संहिता_गणनम्] = reason;
		reason_संहिता_गणनम्++;
		यदि(reason_संहिता_गणनम् == reason_संहिता_max){
			reason_tmp = mosquitto_realloc(reason_codes, (size_t)(reason_संहिता_max*2));
			यदि(!reason_tmp){
				mosquitto_FREE(reason_codes);
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
			reason_codes = reason_tmp;
			reason_संहिता_max *= 2;
		}
	}
#यदि_परिभाषितम् WITH_PERSISTENCE
	db.persistence_changes++;
#यदि_अन्तः

	log__printf(रिक्त, MOSQ_LOG_DEBUG, "Sending UNSUBACK to %s", context->id);

	/* We don't use Reason String or User Property yet. */
	rc = send__unsuback(context, mid, reason_संहिता_गणनम्, reason_codes, रिक्त);
	mosquitto_FREE(reason_codes);
	प्रत्यागम rc;
}
