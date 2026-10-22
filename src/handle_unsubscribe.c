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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "packet_mosq.h"
#അന്തര്ഭാവ "send_mosq.h"


പൂർണ്ണസംഖ്യ handle__unsubscribe(ഘടന mosquitto *context)
{
	uint16_t mid;
	uint16_t slen;
	പൂർണ്ണസംഖ്യ rc;
	uint8_t reason = 0;
	പൂർണ്ണസംഖ്യ reason_code_എണ്ണം = 0;
	പൂർണ്ണസംഖ്യ reason_code_max;
	uint8_t *reason_codes = ശൂന്യ, *reason_tmp;
	mosquitto_property *properties = ശൂന്യ;
	bool allowed;
	ഘടന mosquitto_subscription sub;

	എങ്കിൽ(!context){
		തിരികെ MOSQ_ERR_INVAL;
	}

	എങ്കിൽ(context->state != mosq_cs_active){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE before session is active.", context->id);
		തിരികെ MOSQ_ERR_PROTOCOL;
	}
	എങ്കിൽ(context->in_packet.command != (CMD_UNSUBSCRIBE|2)){
		തിരികെ MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "Received UNSUBSCRIBE from %s", context->id);

	എങ്കിൽ(context->protocol != mosq_p_mqtt31){
		എങ്കിൽ((context->in_packet.command&0x0F) != 0x02){
			തിരികെ MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	എങ്കിൽ(packet__read_uint16(&context->in_packet, &mid)){
		തിരികെ MOSQ_ERR_MALFORMED_PACKET;
	}
	എങ്കിൽ(mid == 0){
		തിരികെ MOSQ_ERR_MALFORMED_PACKET;
	}

	എങ്കിൽ(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_UNSUBSCRIBE, &context->in_packet, &properties);
		എങ്കിൽ(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			എങ്കിൽ(rc == MOSQ_ERR_PROTOCOL){
				log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE packet with invalid properties.", context->id);
				തിരികെ MOSQ_ERR_MALFORMED_PACKET;
			}അല്ലെങ്കിൽ{
				തിരികെ rc;
			}
		}
		/* Immediately free, we don't do anything with User Property at the moment */
		mosquitto_property_free_all(&properties);
	}

	എങ്കിൽ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		എങ്കിൽ(context->in_packet.pos == context->in_packet.remaining_length){
			/* No topic specified, protocol error. */
			തിരികെ MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	reason_code_max = 10;
	reason_codes = mosquitto_malloc((size_t)reason_code_max);
	എങ്കിൽ(!reason_codes){
		തിരികെ MOSQ_ERR_NOMEM;
	}

	ആകുമ്പോൾ(context->in_packet.pos < context->in_packet.remaining_length){
		memset(&sub, 0, വലുപ്പം(sub));
		sub.properties = properties;
		എങ്കിൽ(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(reason_codes);
			തിരികെ MOSQ_ERR_MALFORMED_PACKET;
		}

		എങ്കിൽ(!slen){
			log__printf(ശൂന്യ, MOSQ_LOG_INFO,
					"Empty unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			തിരികെ MOSQ_ERR_MALFORMED_PACKET;
		}
		എങ്കിൽ(mosquitto_sub_topic_check(sub.topic_filter)){
			log__printf(ശൂന്യ, MOSQ_LOG_INFO,
					"Invalid unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			തിരികെ MOSQ_ERR_MALFORMED_PACKET;
		}

		/* ACL check */
		allowed = true;
		rc = mosquitto_acl_check(context, sub.topic_filter, 0, ശൂന്യ, 0, false, properties, MOSQ_ACL_UNSUBSCRIBE);
		തിരഞ്ഞെടുപ്പ്(rc){
			അവസ്ഥ MOSQ_ERR_SUCCESS:
				നിർത്തുക;
			അവസ്ഥ MOSQ_ERR_ACL_DENIED:
				allowed = false;
				reason = MQTT_RC_NOT_AUTHORIZED;
				നിർത്തുക;
			സ്ഥിരസ്ഥിതി:
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				തിരികെ rc;
		}

		log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "\t%s", sub.topic_filter);
		എങ്കിൽ(allowed){
			rc = plugin__handle_unsubscribe(context, &sub);
			എങ്കിൽ(rc){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				തിരികെ rc;
			}
			rc = sub__remove(context, sub.topic_filter, &reason);
			plugin_persist__handle_subscription_delete(context, sub.topic_filter);
		}അല്ലെങ്കിൽ{
			rc = MOSQ_ERR_SUCCESS;
		}
		log__printf(ശൂന്യ, MOSQ_LOG_UNSUBSCRIBE, "%s %s", context->id, sub.topic_filter);
		mosquitto_FREE(sub.topic_filter);
		എങ്കിൽ(rc){
			mosquitto_FREE(reason_codes);
			തിരികെ rc;
		}

		reason_codes[reason_code_എണ്ണം] = reason;
		reason_code_എണ്ണം++;
		എങ്കിൽ(reason_code_എണ്ണം == reason_code_max){
			reason_tmp = mosquitto_realloc(reason_codes, (size_t)(reason_code_max*2));
			എങ്കിൽ(!reason_tmp){
				mosquitto_FREE(reason_codes);
				തിരികെ MOSQ_ERR_NOMEM;
			}
			reason_codes = reason_tmp;
			reason_code_max *= 2;
		}
	}
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "Sending UNSUBACK to %s", context->id);

	/* We don't use Reason String or User Property yet. */
	rc = send__unsuback(context, mid, reason_code_എണ്ണം, reason_codes, ശൂന്യ);
	mosquitto_FREE(reason_codes);
	തിരികെ rc;
}
