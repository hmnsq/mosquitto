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

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "packet_mosq.h"
#include "send_mosq.h"


整数 handle__unsubscribe(struct mosquitto *context)
{
	uint16_t mid;
	uint16_t slen;
	整数 rc;
	uint8_t reason = 0;
	整数 reason_code_count = 0;
	整数 reason_code_max;
	uint8_t *reason_codes = NULL, *reason_tmp;
	mosquitto_property *properties = NULL;
	bool allowed;
	struct mosquitto_subscription sub;

	如果(!context){
		返回 MOSQ_ERR_INVAL;
	}

	如果(context->state != mosq_cs_active){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE before session is active.", context->id);
		返回 MOSQ_ERR_PROTOCOL;
	}
	如果(context->in_packet.command != (CMD_UNSUBSCRIBE|2)){
		返回 MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(NULL, MOSQ_LOG_DEBUG, "Received UNSUBSCRIBE from %s", context->id);

	如果(context->protocol != mosq_p_mqtt31){
		如果((context->in_packet.command&0x0F) != 0x02){
			返回 MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	如果(packet__read_uint16(&context->in_packet, &mid)){
		返回 MOSQ_ERR_MALFORMED_PACKET;
	}
	如果(mid == 0){
		返回 MOSQ_ERR_MALFORMED_PACKET;
	}

	如果(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_UNSUBSCRIBE, &context->in_packet, &properties);
		如果(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			如果(rc == MOSQ_ERR_PROTOCOL){
				log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: UNSUBSCRIBE packet with invalid properties.", context->id);
				返回 MOSQ_ERR_MALFORMED_PACKET;
			}否则{
				返回 rc;
			}
		}
		/* Immediately free, we don't do anything with User Property at the moment */
		mosquitto_property_free_all(&properties);
	}

	如果(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt5){
		如果(context->in_packet.pos == context->in_packet.remaining_length){
			/* No topic specified, protocol error. */
			返回 MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	reason_code_max = 10;
	reason_codes = mosquitto_malloc((size_t)reason_code_max);
	如果(!reason_codes){
		返回 MOSQ_ERR_NOMEM;
	}

	当(context->in_packet.pos < context->in_packet.remaining_length){
		memset(&sub, 0, sizeof(sub));
		sub.properties = properties;
		如果(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(reason_codes);
			返回 MOSQ_ERR_MALFORMED_PACKET;
		}

		如果(!slen){
			log__printf(NULL, MOSQ_LOG_INFO,
					"Empty unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			返回 MOSQ_ERR_MALFORMED_PACKET;
		}
		如果(mosquitto_sub_topic_check(sub.topic_filter)){
			log__printf(NULL, MOSQ_LOG_INFO,
					"Invalid unsubscription string from %s, disconnecting.",
					context->id);
			mosquitto_FREE(sub.topic_filter);
			mosquitto_FREE(reason_codes);
			返回 MOSQ_ERR_MALFORMED_PACKET;
		}

		/* ACL check */
		allowed = true;
		rc = mosquitto_acl_check(context, sub.topic_filter, 0, NULL, 0, false, properties, MOSQ_ACL_UNSUBSCRIBE);
		switch(rc){
			case MOSQ_ERR_SUCCESS:
				break;
			case MOSQ_ERR_ACL_DENIED:
				allowed = false;
				reason = MQTT_RC_NOT_AUTHORIZED;
				break;
			default:
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				返回 rc;
		}

		log__printf(NULL, MOSQ_LOG_DEBUG, "\t%s", sub.topic_filter);
		如果(allowed){
			rc = plugin__handle_unsubscribe(context, &sub);
			如果(rc){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(reason_codes);
				返回 rc;
			}
			rc = sub__remove(context, sub.topic_filter, &reason);
			plugin_persist__handle_subscription_delete(context, sub.topic_filter);
		}否则{
			rc = MOSQ_ERR_SUCCESS;
		}
		log__printf(NULL, MOSQ_LOG_UNSUBSCRIBE, "%s %s", context->id, sub.topic_filter);
		mosquitto_FREE(sub.topic_filter);
		如果(rc){
			mosquitto_FREE(reason_codes);
			返回 rc;
		}

		reason_codes[reason_code_count] = reason;
		reason_code_count++;
		如果(reason_code_count == reason_code_max){
			reason_tmp = mosquitto_realloc(reason_codes, (size_t)(reason_code_max*2));
			如果(!reason_tmp){
				mosquitto_FREE(reason_codes);
				返回 MOSQ_ERR_NOMEM;
			}
			reason_codes = reason_tmp;
			reason_code_max *= 2;
		}
	}
#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	log__printf(NULL, MOSQ_LOG_DEBUG, "Sending UNSUBACK to %s", context->id);

	/* We don't use Reason String or User Property yet. */
	rc = send__unsuback(context, mid, reason_code_count, reason_codes, NULL);
	mosquitto_FREE(reason_codes);
	返回 rc;
}
