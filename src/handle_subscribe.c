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
#ಅನ್ತರ್ಭಾವ "property_mosq.h"


ಪೂರ್ಣಾಂಕ handle__subscribe(ರಚನೆ mosquitto *context)
{
	ಪೂರ್ಣಾಂಕ rc = 0;
	ಪೂರ್ಣಾಂಕ rc2;
	uint16_t mid;
	uint8_t qos;
	uint8_t retain_handling = 0;
	uint8_t *payload = ಖಾಲಿ, *tmp_payload;
	uint32_t payloadlen = 0;
	size_t len;
	uint16_t slen;
	ಅಕ್ಷರ *sub_mount;
	mosquitto_property *properties = ಖಾಲಿ;
	bool allowed;
	ರಚನೆ mosquitto_subscription sub;
	uint32_t subscription_identifier = 0;

	ಆದರೆ(!context){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	ಆದರೆ(context->state != mosq_cs_ಸಕ್ರಿಯ){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE before session is active.", context->id);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
	}
	ಆದರೆ(context->ಒಳ_packet.command != (CMD_SUBSCRIBE|2)){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
	}

	log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "Received SUBSCRIBE from %s", context->id);

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
		rc = property__read_all(CMD_SUBSCRIBE, &context->ಒಳ_packet, &properties);
		ಆದರೆ(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			ಆದರೆ(rc == MOSQ_ERR_PROTOCOL){
				log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE packet with invalid properties.", context->id);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಹಿಂತಿರುಗಿ rc;
			}
		}

		ಆದರೆ(mosquitto_property_read_varint(properties, MQTT_PROP_SUBSCRIPTION_IDENTIFIER,
				&subscription_identifier, false)){

			/* If the identifier was force set to 0, this is an error */
			ಆದರೆ(subscription_identifier == 0){
				mosquitto_property_free_all(&properties);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
			}
		}

		mosquitto_property_free_all(&properties);
		/* Note - User Property not handled */
	}

	ಆಗ(context->ಒಳ_packet.pos < context->ಒಳ_packet.ಉಳಿದ_ಉದ್ದಳತೆ){
		memset(&sub, 0, ಗಾತ್ರ(sub));
		sub.identifier = subscription_identifier;
		sub.properties = properties;
		ಆದರೆ(packet__read_string(&context->ಒಳ_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(payload);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
		}

		ಆದರೆ(sub.topic_filter){
			ಆದರೆ(!slen){
				log__printf(ಖಾಲಿ, MOSQ_LOG_INFO,
						"Empty subscription string from %s, disconnecting.",
						context->ವಿಳಾಸ);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
			}
			ಆದರೆ(mosquitto_sub_topic_check(sub.topic_filter)){
				log__printf(ಖಾಲಿ, MOSQ_LOG_INFO,
						"Invalid subscription string from %s, disconnecting.",
						context->ವಿಳಾಸ);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
			}

			ಆದರೆ(packet__read_byte(&context->ಒಳ_packet, &sub.options)){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
			}
			ಆದರೆ(sub.options & MQTT_SUB_OPT_NO_LOCAL && !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(sub.topic_filter, "$share/", ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("$share/"))){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: $share subscription with no-local set.", context->id);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
			}

			ಆದರೆ(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt311){
				qos = sub.options;
				sub.options = 0;
				ಆದರೆ(context->is_bridge){
					sub.options |= MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | MQTT_SUB_OPT_NO_LOCAL;
				}
			}ಇಲ್ಲದಿದ್ದರೆ{
				qos = sub.options & 0x03;
				sub.options &= 0xFC;

				ಆದರೆ(MQTT_SUB_OPT_GET_NO_LOCAL(sub.options) && !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(sub.topic_filter, "$share/", 7)){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
				}
				retain_handling = MQTT_SUB_OPT_GET_RETAIN_HANDLING(sub.options);
				ಆದರೆ(retain_handling == 0x30 || (sub.options & 0xC0) != 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
				}
			}
			ಆದರೆ(qos > 2){
				log__printf(ಖಾಲಿ, MOSQ_LOG_INFO,
						"Invalid QoS in subscription command from %s, disconnecting.",
						context->ವಿಳಾಸ);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
			}
			ಆದರೆ(qos > context->max_qos){
				qos = context->max_qos;
			}
			sub.options |= qos;


			ಆದರೆ(context->listener && context->listener->mount_ಬಿಂದು){
				len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->listener->mount_ಬಿಂದು) + slen + 1;
				sub_mount = mosquitto_malloc(len+1);
				ಆದರೆ(!sub_mount){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
				}
				snprintf(sub_mount, len, "%s%s", context->listener->mount_ಬಿಂದು, sub.topic_filter);
				sub_mount[len] = '\0';

				mosquitto_FREE(sub.topic_filter);
				sub.topic_filter = sub_mount;

			}

			allowed = true;
			rc2 = mosquitto_acl_check(context, sub.topic_filter, 0, ಖಾಲಿ, qos, false, properties, MOSQ_ACL_SUBSCRIBE);
			ಆಯ್ಕೆ(rc2){
				ಸ್ಥಿತಿ MOSQ_ERR_SUCCESS:
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ MOSQ_ERR_ACL_DENIED:
					allowed = false;
					ಆದರೆ(context->protocol == mosq_p_mqtt5){
						qos = MQTT_RC_NOT_AUTHORIZED;
					}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(context->protocol == mosq_p_mqtt311){
						qos = 0x80;
					}
					ನಿಲ್ಲಿಸು;
				ಪೂರ್ವನಿಯೋಜಿತ:
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ಹಿಂತಿರುಗಿ rc2;
			}
			ಆದರೆ(qos > 127){
				log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "\t%s (denied)", sub.topic_filter);
			}ಇಲ್ಲದಿದ್ದರೆ{
				log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "\t%s (QoS %d)", sub.topic_filter, qos);
			}

			ಆದರೆ(allowed){
				rc2 = plugin__handle_subscribe(context, &sub);
				ಆದರೆ(rc2){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ಹಿಂತಿರುಗಿ rc2;
				}

				rc2 = sub_ಕೂಡಿಸು(context, &sub);
				ಆದರೆ(rc2 > 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ಹಿಂತಿರುಗಿ rc2;
				}
				ಆದರೆ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
					ಆದರೆ(rc2 == MOSQ_ERR_SUCCESS || rc2 == MOSQ_ERR_SUB_EXISTS){
						ಆದರೆ(retain_ಸರತಿ(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							ಹಿಂತಿರುಗಿ rc;
						}
					}
				}ಇಲ್ಲದಿದ್ದರೆ{
					ಆದರೆ((retain_handling == MQTT_SUB_OPT_SEND_RETAIN_ALWAYS)
							|| (rc2 == MOSQ_ERR_SUCCESS && retain_handling == MQTT_SUB_OPT_SEND_RETAIN_NEW)){

						ಆದರೆ(retain_ಸರತಿ(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							ಹಿಂತಿರುಗಿ rc;
						}
					}
				}
				log__printf(ಖಾಲಿ, MOSQ_LOG_SUBSCRIBE, "%s %d %s", context->id, qos, sub.topic_filter);

				plugin_persist_handle_subscription_ಕೂಡಿಸು(context, &sub);
			}
			mosquitto_FREE(sub.topic_filter);

			tmp_payload = mosquitto_realloc(payload, payloadlen + 1);
			ಆದರೆ(tmp_payload){
				payload = tmp_payload;
				payload[payloadlen] = qos;
				payloadlen++;
			}ಇಲ್ಲದಿದ್ದರೆ{
				mosquitto_FREE(payload);

				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
		}
	}

	ಆದರೆ(context->protocol != mosq_p_mqtt31){
		ಆದರೆ(payloadlen == 0){
			/* No subscriptions specified, protocol error. */
			ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	rc = send__suback(context, mid, payloadlen, payload);
	mosquitto_FREE(payload);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_PERSISTENCE
	db.persistence_changes++;
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ(context->ಹೊರಹೋಗು_packet == ಖಾಲಿ){
		rc = db_ಸಂದೇಶ_write_queued_ಹೊರಹೋಗು(context);
		ಆದರೆ(rc){
			ಹಿಂತಿರುಗಿ rc;
		}
		rc = db_ಸಂದೇಶ_write_inflight_ಹೊರಹೋಗು_latest(context);
		ಆದರೆ(rc){
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	ಹಿಂತಿರುಗಿ rc;
}
