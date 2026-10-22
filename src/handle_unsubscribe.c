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

#समावेश "config.h"

#समावेश <stdio.h>
#समावेश <string.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "packet_mosq.h"
#समावेश "send_mosq.h"


पूर्णांक handle__unsubscribe(काष्ठा mosquitto *context)
{
	uint16_t mid;
	uint16_t slen;
	पूर्णांक rc;
	uint8_t reason = 0;
	पूर्णांक reason_code_गिनती = 0;
	पूर्णांक reason_code_max;
	uint8_t *reason_codes = शून्य, *reason_tmp;
	mosquitto_property *properties = शून्य;
	bool allowed;
	काष्ठा mosquitto_subscription sub;

	अगर(!context){
		वापस MOSQ_ERR_INVAL;
	}

	अगर(context->state != mosq_cs_active){
		log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE before session is active.", context->id);
		वापस MOSQ_ERR_PROTOCOL;
	}
	अगर(context->in_packet.command != (CMD_UNSUBSCRIBE|2)){
		वापस MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(शून्य, MOSQ_LOG_DEBUG, "Received UNSUBSCRIBE from %s", context->id);

	अगर(context->protocol != mosq_p_mqtt31){
		अगर((context->in_packet.command&0x0F) != 0x02){
			वापस MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	अगर(packet__read_uint16(&context->in_packet, &mid)){
		वापस MOSQ_ERR_MALFORMED_PACKET;
	}
	अगर(mid == 0){
		वापस MOSQ_ERR_MALFORMED_PACKET;
	}

	अगर(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_UNSUBSCRIBE, &context->in_packet, &properties);
		अगर(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			अगर(rc == MOSQ_ERR_PROTOCOL){
				log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE packet with invalid properties.", context->id);
				वापस MOSQ_ERR_MALFORMED_PACKET;
			}अन्यथा{
				वापस rc;
			}
		}
		/* Immediately free, we don't do anything with User Property at the moment */
		mosquitto_property_free_all(&properties);
	}

	अगर(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		अगर(context->in_packet.pos == context->in_packet.remaining_लंबाई){
			/* No topic specified, protocol error. */
			वापस MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	reason_code_max = 10;
	reason_codes = mosquitto_malloc((माप_प्रकार)reason_code_max);
	अगर(!reason_codes){
		वापस MOSQ_ERR_NOMEM;
	}

	जबतक(context->in_packet.pos < context->in_packet.remaining_लंबाई){
		स_रखो(&sub, 0, माप(sub));
		sub.properties = properties;
		अगर(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(reason_codes);
			वापस MOSQ_ERR_MALFORMED_PACKET;
		}

		अगर(!slen){
			log__printf(शून्य, MOSQ_LOG_INFO,
					"Empty unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			वापस MOSQ_ERR_MALFORMED_PACKET;
		}
		अगर(mosquitto_sub_topic_check(sub.topic_filter)){
			log__printf(शून्य, MOSQ_LOG_INFO,
					"Invalid unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			वापस MOSQ_ERR_MALFORMED_PACKET;
		}

		/* ACL check */
		allowed = true;
		rc = mosquitto_acl_check(context, sub.topic_filter, 0, शून्य, 0, false, properties, MOSQ_ACL_UNSUBSCRIBE);
		चयन(rc){
			हाल MOSQ_ERR_SUCCESS:
				अवरोध;
			हाल MOSQ_ERR_ACL_DENIED:
				allowed = false;
				reason = MQTT_RC_NOT_AUTHORIZED;
				अवरोध;
			शेष:
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				वापस rc;
		}

		log__printf(शून्य, MOSQ_LOG_DEBUG, "\t%s", sub.topic_filter);
		अगर(allowed){
			rc = plugin__handle_unsubscribe(context, &sub);
			अगर(rc){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				वापस rc;
			}
			rc = sub__remove(context, sub.topic_filter, &reason);
			plugin_persist__handle_subscription_delete(context, sub.topic_filter);
		}अन्यथा{
			rc = MOSQ_ERR_SUCCESS;
		}
		log__printf(शून्य, MOSQ_LOG_UNSUBSCRIBE, "%s %s", context->id, sub.topic_filter);
		mosquitto_FREE(sub.topic_filter);
		अगर(rc){
			mosquitto_FREE(reason_codes);
			वापस rc;
		}

		reason_codes[reason_code_गिनती] = reason;
		reason_code_गिनती++;
		अगर(reason_code_गिनती == reason_code_max){
			reason_tmp = mosquitto_realloc(reason_codes, (माप_प्रकार)(reason_code_max*2));
			अगर(!reason_tmp){
				mosquitto_FREE(reason_codes);
				वापस MOSQ_ERR_NOMEM;
			}
			reason_codes = reason_tmp;
			reason_code_max *= 2;
		}
	}
#अगर_घोषित WITH_PERSISTENCE
	db.persistence_changes++;
#पूर्ण_अगर

	log__printf(शून्य, MOSQ_LOG_DEBUG, "Sending UNSUBACK to %s", context->id);

	/* We don't use Reason String or User Property yet. */
	rc = send__unsuback(context, mid, reason_code_गिनती, reason_codes, शून्य);
	mosquitto_FREE(reason_codes);
	वापस rc;
}
