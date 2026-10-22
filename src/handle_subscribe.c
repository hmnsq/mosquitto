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


ಪೂರ್ಣಾಙ್ಕ handle__subscribe(ಸಂರಚನಾ mosquitto *context)
{
	ಪೂರ್ಣಾಙ್ಕ rc = 0;
	ಪೂರ್ಣಾಙ್ಕ rc2;
	uint16_t mid;
	uint8_t qos;
	uint8_t retain_handling = 0;
	uint8_t *payload = ರಿಕ್ತ, *tmp_payload;
	uint32_t payloadlen = 0;
	size_t len;
	uint16_t slen;
	ವರ್ಣ *sub_mount;
	mosquitto_property *properties = ರಿಕ್ತ;
	bool allowed;
	ಸಂರಚನಾ mosquitto_subscription sub;
	uint32_t subscription_identifier = 0;

	ಯದಿ(!context){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	ಯದಿ(context->state != mosq_cs_active){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE before session is active.", context->id);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
	}
	ಯದಿ(context->in_packet.command != (CMD_SUBSCRIBE|2)){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
	}

	log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "Received SUBSCRIBE from %s", context->id);

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
		rc = property__read_all(CMD_SUBSCRIBE, &context->in_packet, &properties);
		ಯದಿ(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			ಯದಿ(rc == MOSQ_ERR_PROTOCOL){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE packet with invalid properties.", context->id);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
			}ಅನ್ಯಥಾ{
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}

		ಯದಿ(mosquitto_property_read_varint(properties, MQTT_PROP_SUBSCRIPTION_IDENTIFIER,
				&subscription_identifier, false)){

			/* If the identifier was force set to 0, this is an error */
			ಯದಿ(subscription_identifier == 0){
				mosquitto_property_free_all(&properties);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
			}
		}

		mosquitto_property_free_all(&properties);
		/* Note - User Property not handled */
	}

	ಯಾವತ್(context->in_packet.pos < context->in_packet.remaining_length){
		memset(&sub, 0, ಪರಿಮಾಣ(sub));
		sub.identifier = subscription_identifier;
		sub.properties = properties;
		ಯದಿ(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(payload);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
		}

		ಯದಿ(sub.topic_filter){
			ಯದಿ(!slen){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO,
						"Empty subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
			}
			ಯದಿ(mosquitto_sub_topic_check(sub.topic_filter)){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO,
						"Invalid subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
			}

			ಯದಿ(packet__read_byte(&context->in_packet, &sub.options)){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
			}
			ಯದಿ(sub.options & MQTT_SUB_OPT_NO_LOCAL && !strncmp(sub.topic_filter, "$share/", strlen("$share/"))){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: $share subscription with no-local set.", context->id);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
			}

			ಯದಿ(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt311){
				qos = sub.options;
				sub.options = 0;
				ಯದಿ(context->is_bridge){
					sub.options |= MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | MQTT_SUB_OPT_NO_LOCAL;
				}
			}ಅನ್ಯಥಾ{
				qos = sub.options & 0x03;
				sub.options &= 0xFC;

				ಯದಿ(MQTT_SUB_OPT_GET_NO_LOCAL(sub.options) && !strncmp(sub.topic_filter, "$share/", 7)){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
				}
				retain_handling = MQTT_SUB_OPT_GET_RETAIN_HANDLING(sub.options);
				ಯದಿ(retain_handling == 0x30 || (sub.options & 0xC0) != 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
				}
			}
			ಯದಿ(qos > 2){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO,
						"Invalid QoS in subscription command from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
			}
			ಯದಿ(qos > context->max_qos){
				qos = context->max_qos;
			}
			sub.options |= qos;


			ಯದಿ(context->listener && context->listener->mount_point){
				len = strlen(context->listener->mount_point) + slen + 1;
				sub_mount = mosquitto_malloc(len+1);
				ಯದಿ(!sub_mount){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
				}
				snprintf(sub_mount, len, "%s%s", context->listener->mount_point, sub.topic_filter);
				sub_mount[len] = '\0';

				mosquitto_FREE(sub.topic_filter);
				sub.topic_filter = sub_mount;

			}

			allowed = true;
			rc2 = mosquitto_acl_check(context, sub.topic_filter, 0, ರಿಕ್ತ, qos, false, properties, MOSQ_ACL_SUBSCRIBE);
			ವಿಕಲ್ಪ(rc2){
				ಪಕ್ಷ MOSQ_ERR_SUCCESS:
					ಭಙ್ಗ;
				ಪಕ್ಷ MOSQ_ERR_ACL_DENIED:
					allowed = false;
					ಯದಿ(context->protocol == mosq_p_mqtt5){
						qos = MQTT_RC_NOT_AUTHORIZED;
					}ಅನ್ಯಥಾ ಯದಿ(context->protocol == mosq_p_mqtt311){
						qos = 0x80;
					}
					ಭಙ್ಗ;
				ಸ್ವಭಾವ:
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ಪ್ರತ್ಯಾಗಮ rc2;
			}
			ಯದಿ(qos > 127){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "\t%s (denied)", sub.topic_filter);
			}ಅನ್ಯಥಾ{
				log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "\t%s (QoS %d)", sub.topic_filter, qos);
			}

			ಯದಿ(allowed){
				rc2 = plugin__handle_subscribe(context, &sub);
				ಯದಿ(rc2){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ಪ್ರತ್ಯಾಗಮ rc2;
				}

				rc2 = sub__add(context, &sub);
				ಯದಿ(rc2 > 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ಪ್ರತ್ಯಾಗಮ rc2;
				}
				ಯದಿ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
					ಯದಿ(rc2 == MOSQ_ERR_SUCCESS || rc2 == MOSQ_ERR_SUB_EXISTS){
						ಯದಿ(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							ಪ್ರತ್ಯಾಗಮ rc;
						}
					}
				}ಅನ್ಯಥಾ{
					ಯದಿ((retain_handling == MQTT_SUB_OPT_SEND_RETAIN_ALWAYS)
							|| (rc2 == MOSQ_ERR_SUCCESS && retain_handling == MQTT_SUB_OPT_SEND_RETAIN_NEW)){

						ಯದಿ(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							ಪ್ರತ್ಯಾಗಮ rc;
						}
					}
				}
				log__printf(ರಿಕ್ತ, MOSQ_LOG_SUBSCRIBE, "%s %d %s", context->id, qos, sub.topic_filter);

				plugin_persist__handle_subscription_add(context, &sub);
			}
			mosquitto_FREE(sub.topic_filter);

			tmp_payload = mosquitto_realloc(payload, payloadlen + 1);
			ಯದಿ(tmp_payload){
				payload = tmp_payload;
				payload[payloadlen] = qos;
				payloadlen++;
			}ಅನ್ಯಥಾ{
				mosquitto_FREE(payload);

				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
		}
	}

	ಯದಿ(context->protocol != mosq_p_mqtt31){
		ಯದಿ(payloadlen == 0){
			/* No subscriptions specified, protocol error. */
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	rc = send__suback(context, mid, payloadlen, payload);
	mosquitto_FREE(payload);

#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	ಯದಿ(context->out_packet == ರಿಕ್ತ){
		rc = db__message_write_queued_out(context);
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
		rc = db__message_write_inflight_out_latest(context);
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	ಪ್ರತ್ಯಾಗಮ rc;
}
