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
#include "property_mosq.h"


puur_naa_nka handle__subscribe(sa_nracanaa mosquitto *context)
{
	puur_naa_nka rc = 0;
	puur_naa_nka rc2;
	uint16_t mid;
	uint8_t qos;
	uint8_t retain_handling = 0;
	uint8_t *payload = NULL, *tmp_payload;
	uint32_t payloadlen = 0;
	size_t len;
	uint16_t slen;
	var_na *sub_mount;
	mosquitto_property *properties = NULL;
	bool allowed;
	sa_nracanaa mosquitto_subscription sub;
	uint32_t subscription_identifier = 0;

	ᤀᤡ᤺(!context){
		ᤀᤡᤴ MOSQ_ERR_INVAL;
	}

	ᤀᤡ᤺(context->state != mosq_cs_active){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE before session is active.", context->id);
		ᤀᤡᤴ MOSQ_ERR_PROTOCOL;
	}
	ᤀᤡ᤺(context->in_packet.command != (CMD_SUBSCRIBE|2)){
		ᤀᤡᤴ MOSQ_ERR_MALFORMED_PACKET;
	}

	log__printf(NULL, MOSQ_LOG_DEBUG, "Received SUBSCRIBE from %s", context->id);

	ᤀᤡ᤺(context->protocol != mosq_p_mqtt31){
		ᤀᤡ᤺((context->in_packet.command&0x0F) != 0x02){
			ᤀᤡᤴ MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	ᤀᤡ᤺(packet__read_uint16(&context->in_packet, &mid)){
		ᤀᤡᤴ MOSQ_ERR_MALFORMED_PACKET;
	}
	ᤀᤡ᤺(mid == 0){
		ᤀᤡᤴ MOSQ_ERR_MALFORMED_PACKET;
	}

	ᤀᤡ᤺(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_SUBSCRIBE, &context->in_packet, &properties);
		ᤀᤡ᤺(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			ᤀᤡ᤺(rc == MOSQ_ERR_PROTOCOL){
				log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE packet with invalid properties.", context->id);
				ᤀᤡᤴ MOSQ_ERR_MALFORMED_PACKET;
			}ᤀᤡ᤺ᤆᤡ{
				ᤀᤡᤴ rc;
			}
		}

		ᤀᤡ᤺(mosquitto_property_read_varint(properties, MQTT_PROP_SUBSCRIPTION_IDENTIFIER,
				&subscription_identifier, false)){

			/* If the identifier was force set to 0, this is an error */
			ᤀᤡ᤺(subscription_identifier == 0){
				mosquitto_property_free_all(&properties);
				ᤀᤡᤴ MOSQ_ERR_PROTOCOL;
			}
		}

		mosquitto_property_free_all(&properties);
		/* Note - User Property not handled */
	}

	ᤀᤠ᤺(context->in_packet.pos < context->in_packet.remaining_length){
		memset(&sub, 0, parimaa_na(sub));
		sub.identifier = subscription_identifier;
		sub.properties = properties;
		ᤀᤡ᤺(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(payload);
			ᤀᤡᤴ MOSQ_ERR_MALFORMED_PACKET;
		}

		ᤀᤡ᤺(sub.topic_filter){
			ᤀᤡ᤺(!slen){
				log__printf(NULL, MOSQ_LOG_INFO,
						"Empty subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ᤀᤡᤴ MOSQ_ERR_MALFORMED_PACKET;
			}
			ᤀᤡ᤺(mosquitto_sub_topic_check(sub.topic_filter)){
				log__printf(NULL, MOSQ_LOG_INFO,
						"Invalid subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ᤀᤡᤴ MOSQ_ERR_MALFORMED_PACKET;
			}

			ᤀᤡ᤺(packet__read_byte(&context->in_packet, &sub.options)){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ᤀᤡᤴ MOSQ_ERR_MALFORMED_PACKET;
			}
			ᤀᤡ᤺(sub.options & MQTT_SUB_OPT_NO_LOCAL && !strncmp(sub.topic_filter, "$share/", strlen("$share/"))){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: $share subscription with no-local set.", context->id);
				ᤀᤡᤴ MOSQ_ERR_PROTOCOL;
			}

			ᤀᤡ᤺(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt311){
				qos = sub.options;
				sub.options = 0;
				ᤀᤡ᤺(context->is_bridge){
					sub.options |= MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | MQTT_SUB_OPT_NO_LOCAL;
				}
			}ᤀᤡ᤺ᤆᤡ{
				qos = sub.options & 0x03;
				sub.options &= 0xFC;

				ᤀᤡ᤺(MQTT_SUB_OPT_GET_NO_LOCAL(sub.options) && !strncmp(sub.topic_filter, "$share/", 7)){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ᤀᤡᤴ MOSQ_ERR_PROTOCOL;
				}
				retain_handling = MQTT_SUB_OPT_GET_RETAIN_HANDLING(sub.options);
				ᤀᤡ᤺(retain_handling == 0x30 || (sub.options & 0xC0) != 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ᤀᤡᤴ MOSQ_ERR_MALFORMED_PACKET;
				}
			}
			ᤀᤡ᤺(qos > 2){
				log__printf(NULL, MOSQ_LOG_INFO,
						"Invalid QoS in subscription command from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ᤀᤡᤴ MOSQ_ERR_MALFORMED_PACKET;
			}
			ᤀᤡ᤺(qos > context->max_qos){
				qos = context->max_qos;
			}
			sub.options |= qos;


			ᤀᤡ᤺(context->listener && context->listener->mount_point){
				len = strlen(context->listener->mount_point) + slen + 1;
				sub_mount = mosquitto_malloc(len+1);
				ᤀᤡ᤺(!sub_mount){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ᤀᤡᤴ MOSQ_ERR_NOMEM;
				}
				snprintf(sub_mount, len, "%s%s", context->listener->mount_point, sub.topic_filter);
				sub_mount[len] = '\0';

				mosquitto_FREE(sub.topic_filter);
				sub.topic_filter = sub_mount;

			}

			allowed = true;
			rc2 = mosquitto_acl_check(context, sub.topic_filter, 0, NULL, qos, false, properties, MOSQ_ACL_SUBSCRIBE);
			switch(rc2){
				pak_sa MOSQ_ERR_SUCCESS:
					bha_nga;
				pak_sa MOSQ_ERR_ACL_DENIED:
					allowed = false;
					ᤀᤡ᤺(context->protocol == mosq_p_mqtt5){
						qos = MQTT_RC_NOT_AUTHORIZED;
					}ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(context->protocol == mosq_p_mqtt311){
						qos = 0x80;
					}
					bha_nga;
				default:
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ᤀᤡᤴ rc2;
			}
			ᤀᤡ᤺(qos > 127){
				log__printf(NULL, MOSQ_LOG_DEBUG, "\t%s (denied)", sub.topic_filter);
			}ᤀᤡ᤺ᤆᤡ{
				log__printf(NULL, MOSQ_LOG_DEBUG, "\t%s (QoS %d)", sub.topic_filter, qos);
			}

			ᤀᤡ᤺(allowed){
				rc2 = plugin__handle_subscribe(context, &sub);
				ᤀᤡ᤺(rc2){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ᤀᤡᤴ rc2;
				}

				rc2 = sub__add(context, &sub);
				ᤀᤡ᤺(rc2 > 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ᤀᤡᤴ rc2;
				}
				ᤀᤡ᤺(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
					ᤀᤡ᤺(rc2 == MOSQ_ERR_SUCCESS || rc2 == MOSQ_ERR_SUB_EXISTS){
						ᤀᤡ᤺(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							ᤀᤡᤴ rc;
						}
					}
				}ᤀᤡ᤺ᤆᤡ{
					ᤀᤡ᤺((retain_handling == MQTT_SUB_OPT_SEND_RETAIN_ALWAYS)
							|| (rc2 == MOSQ_ERR_SUCCESS && retain_handling == MQTT_SUB_OPT_SEND_RETAIN_NEW)){

						ᤀᤡ᤺(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							ᤀᤡᤴ rc;
						}
					}
				}
				log__printf(NULL, MOSQ_LOG_SUBSCRIBE, "%s %d %s", context->id, qos, sub.topic_filter);

				plugin_persist__handle_subscription_add(context, &sub);
			}
			mosquitto_FREE(sub.topic_filter);

			tmp_payload = mosquitto_realloc(payload, payloadlen + 1);
			ᤀᤡ᤺(tmp_payload){
				payload = tmp_payload;
				payload[payloadlen] = qos;
				payloadlen++;
			}ᤀᤡ᤺ᤆᤡ{
				mosquitto_FREE(payload);

				ᤀᤡᤴ MOSQ_ERR_NOMEM;
			}
		}
	}

	ᤀᤡ᤺(context->protocol != mosq_p_mqtt31){
		ᤀᤡ᤺(payloadlen == 0){
			/* No subscriptions specified, protocol error. */
			ᤀᤡᤴ MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	rc = send__suback(context, mid, payloadlen, payload);
	mosquitto_FREE(payload);

#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	ᤀᤡ᤺(context->out_packet == NULL){
		rc = db__message_write_queued_out(context);
		ᤀᤡ᤺(rc){
			ᤀᤡᤴ rc;
		}
		rc = db__message_write_inflight_out_latest(context);
		ᤀᤡ᤺(rc){
			ᤀᤡᤴ rc;
		}
	}

	ᤀᤡᤴ rc;
}
