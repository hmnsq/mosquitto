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


पूर्णांक handle__unsubscribe(संरचना mosquitto *context)
{
	uint16_t mid;
	uint16_t slen;
	पूर्णांक rc;
	uint8_t reason = 0;
	पूर्णांक reason_code_मोजणी = 0;
	पूर्णांक reason_code_max;
	uint8_t *reason_codes = रिक्त, *reason_tmp;
	mosquitto_property *properties = रिक्त;
	bool allowed;
	संरचना mosquitto_subscription sub;

	जर(!context){
		परत MOSQ_ERR_INVAL;
	}

	जर(context->state != mosq_cs_active){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE before session is active.", context->id);
		परत MOSQ_ERR_PROTOCOL;
	}
	जर(context->in_packet.command != (CMD_UNSUBSCRIBE|2)){
		परत MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(रिक्त, MOSQ_LOG_DEBUG, "Received UNSUBSCRIBE from %s", context->id);

	जर(context->protocol != mosq_p_mqtt31){
		जर((context->in_packet.command&0x0F) != 0x02){
			परत MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	जर(packet__read_uint16(&context->in_packet, &mid)){
		परत MOSQ_ERR_MALFORMED_PACKET;
	}
	जर(mid == 0){
		परत MOSQ_ERR_MALFORMED_PACKET;
	}

	जर(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_UNSUBSCRIBE, &context->in_packet, &properties);
		जर(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			जर(rc == MOSQ_ERR_PROTOCOL){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE packet with invalid properties.", context->id);
				परत MOSQ_ERR_MALFORMED_PACKET;
			}अन्यथा{
				परत rc;
			}
		}
		/* Immediately free, we don't do anything with User Property at the moment */
		mosquitto_property_free_all(&properties);
	}

	जर(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		जर(context->in_packet.pos == context->in_packet.remaining_length){
			/* No topic specified, protocol error. */
			परत MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	reason_code_max = 10;
	reason_codes = mosquitto_malloc((माप_प्रकार)reason_code_max);
	जर(!reason_codes){
		परत MOSQ_ERR_NOMEM;
	}

	पर्यंत(context->in_packet.pos < context->in_packet.remaining_length){
		स_रखो(&sub, 0, आकार(sub));
		sub.properties = properties;
		जर(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(reason_codes);
			परत MOSQ_ERR_MALFORMED_PACKET;
		}

		जर(!slen){
			log__printf(रिक्त, MOSQ_LOG_INFO,
					"Empty unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			परत MOSQ_ERR_MALFORMED_PACKET;
		}
		जर(mosquitto_sub_topic_check(sub.topic_filter)){
			log__printf(रिक्त, MOSQ_LOG_INFO,
					"Invalid unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			परत MOSQ_ERR_MALFORMED_PACKET;
		}

		/* ACL check */
		allowed = true;
		rc = mosquitto_acl_check(context, sub.topic_filter, 0, रिक्त, 0, false, properties, MOSQ_ACL_UNSUBSCRIBE);
		निवड(rc){
			स्थिती MOSQ_ERR_SUCCESS:
				थांबा;
			स्थिती MOSQ_ERR_ACL_DENIED:
				allowed = false;
				reason = MQTT_RC_NOT_AUTHORIZED;
				थांबा;
			पूर्वनिर्धारित:
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				परत rc;
		}

		log__printf(रिक्त, MOSQ_LOG_DEBUG, "\t%s", sub.topic_filter);
		जर(allowed){
			rc = plugin__handle_unsubscribe(context, &sub);
			जर(rc){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				परत rc;
			}
			rc = sub__remove(context, sub.topic_filter, &reason);
			plugin_persist__handle_subscription_delete(context, sub.topic_filter);
		}अन्यथा{
			rc = MOSQ_ERR_SUCCESS;
		}
		log__printf(रिक्त, MOSQ_LOG_UNSUBSCRIBE, "%s %s", context->id, sub.topic_filter);
		mosquitto_FREE(sub.topic_filter);
		जर(rc){
			mosquitto_FREE(reason_codes);
			परत rc;
		}

		reason_codes[reason_code_मोजणी] = reason;
		reason_code_मोजणी++;
		जर(reason_code_मोजणी == reason_code_max){
			reason_tmp = mosquitto_realloc(reason_codes, (माप_प्रकार)(reason_code_max*2));
			जर(!reason_tmp){
				mosquitto_FREE(reason_codes);
				परत MOSQ_ERR_NOMEM;
			}
			reason_codes = reason_tmp;
			reason_code_max *= 2;
		}
	}
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	log__printf(रिक्त, MOSQ_LOG_DEBUG, "Sending UNSUBACK to %s", context->id);

	/* We don't use Reason String or User Property yet. */
	rc = send__unsuback(context, mid, reason_code_मोजणी, reason_codes, रिक्त);
	mosquitto_FREE(reason_codes);
	परत rc;
}
