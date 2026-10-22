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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "property_mosq.h"


ਪੂਰਨ handle__subscribe(ਬਣਤਰ mosquitto *context)
{
	ਪੂਰਨ rc = 0;
	ਪੂਰਨ rc2;
	uint16_t mid;
	uint8_t qos;
	uint8_t retain_handling = 0;
	uint8_t *payload = ਨਿਰਾ, *tmp_payload;
	uint32_t payloadlen = 0;
	size_t len;
	uint16_t slen;
	ਅੱਖਰ *sub_mount;
	mosquitto_property *properties = ਨਿਰਾ;
	bool allowed;
	ਬਣਤਰ mosquitto_subscription sub;
	uint32_t subscription_identifier = 0;

	ਜੇ(!context){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਜੇ(context->state != mosq_cs_ਸਰਗਰਮ){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE before session is active.", context->id);
		ਵਾਪਸ MOSQ_ERR_PROTOCOL;
	}
	ਜੇ(context->ਵਿੱਚ_packet.command != (CMD_SUBSCRIBE|2)){
		ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
	}

	log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Received SUBSCRIBE from %s", context->id);

	ਜੇ(context->protocol != mosq_p_mqtt31){
		ਜੇ((context->ਵਿੱਚ_packet.command&0x0F) != 0x02){
			ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	ਜੇ(packet__read_uint16(&context->ਵਿੱਚ_packet, &mid)){
		ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
	}
	ਜੇ(mid == 0){
		ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
	}

	ਜੇ(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_SUBSCRIBE, &context->ਵਿੱਚ_packet, &properties);
		ਜੇ(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			ਜੇ(rc == MOSQ_ERR_PROTOCOL){
				log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE packet with invalid properties.", context->id);
				ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
			}ਨਹੀਂਤਾਂ{
				ਵਾਪਸ rc;
			}
		}

		ਜੇ(mosquitto_property_read_varint(properties, MQTT_PROP_SUBSCRIPTION_IDENTIFIER,
				&subscription_identifier, false)){

			/* If the identifier was force set to 0, this is an error */
			ਜੇ(subscription_identifier == 0){
				mosquitto_property_free_all(&properties);
				ਵਾਪਸ MOSQ_ERR_PROTOCOL;
			}
		}

		mosquitto_property_free_all(&properties);
		/* Note - User Property not handled */
	}

	ਜਦੋਂ(context->ਵਿੱਚ_packet.pos < context->ਵਿੱਚ_packet.ਬਚਿਆ_ਲੰਮਾਈ){
		memset(&sub, 0, ਆਕਾਰ(sub));
		sub.identifier = subscription_identifier;
		sub.properties = properties;
		ਜੇ(packet__read_string(&context->ਵਿੱਚ_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(payload);
			ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
		}

		ਜੇ(sub.topic_filter){
			ਜੇ(!slen){
				log__printf(ਨਿਰਾ, MOSQ_LOG_INFO,
						"Empty subscription string from %s, disconnecting.",
						context->ਪਤਾ);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
			}
			ਜੇ(mosquitto_sub_topic_check(sub.topic_filter)){
				log__printf(ਨਿਰਾ, MOSQ_LOG_INFO,
						"Invalid subscription string from %s, disconnecting.",
						context->ਪਤਾ);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
			}

			ਜੇ(packet__read_byte(&context->ਵਿੱਚ_packet, &sub.options)){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
			}
			ਜੇ(sub.options & MQTT_SUB_OPT_NO_LOCAL && !ਲੜੀ_ਤੁਲਨਾ(sub.topic_filter, "$share/", ਲੜੀ_ਲੰਬਾਈ("$share/"))){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: $share subscription with no-local set.", context->id);
				ਵਾਪਸ MOSQ_ERR_PROTOCOL;
			}

			ਜੇ(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt311){
				qos = sub.options;
				sub.options = 0;
				ਜੇ(context->is_bridge){
					sub.options |= MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | MQTT_SUB_OPT_NO_LOCAL;
				}
			}ਨਹੀਂਤਾਂ{
				qos = sub.options & 0x03;
				sub.options &= 0xFC;

				ਜੇ(MQTT_SUB_OPT_GET_NO_LOCAL(sub.options) && !ਲੜੀ_ਤੁਲਨਾ(sub.topic_filter, "$share/", 7)){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ਵਾਪਸ MOSQ_ERR_PROTOCOL;
				}
				retain_handling = MQTT_SUB_OPT_GET_RETAIN_HANDLING(sub.options);
				ਜੇ(retain_handling == 0x30 || (sub.options & 0xC0) != 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
				}
			}
			ਜੇ(qos > 2){
				log__printf(ਨਿਰਾ, MOSQ_LOG_INFO,
						"Invalid QoS in subscription command from %s, disconnecting.",
						context->ਪਤਾ);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
			}
			ਜੇ(qos > context->max_qos){
				qos = context->max_qos;
			}
			sub.options |= qos;


			ਜੇ(context->listener && context->listener->mount_ਬਿੰਦੂ){
				len = ਲੜੀ_ਲੰਬਾਈ(context->listener->mount_ਬਿੰਦੂ) + slen + 1;
				sub_mount = mosquitto_malloc(len+1);
				ਜੇ(!sub_mount){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ਵਾਪਸ MOSQ_ERR_NOMEM;
				}
				snprintf(sub_mount, len, "%s%s", context->listener->mount_ਬਿੰਦੂ, sub.topic_filter);
				sub_mount[len] = '\0';

				mosquitto_FREE(sub.topic_filter);
				sub.topic_filter = sub_mount;

			}

			allowed = true;
			rc2 = mosquitto_acl_check(context, sub.topic_filter, 0, ਨਿਰਾ, qos, false, properties, MOSQ_ACL_SUBSCRIBE);
			ਚੋਣ(rc2){
				ਹਾਲਤ MOSQ_ERR_SUCCESS:
					ਰੋਕੋ;
				ਹਾਲਤ MOSQ_ERR_ACL_DENIED:
					allowed = false;
					ਜੇ(context->protocol == mosq_p_mqtt5){
						qos = MQTT_RC_NOT_AUTHORIZED;
					}ਨਹੀਂਤਾਂ ਜੇ(context->protocol == mosq_p_mqtt311){
						qos = 0x80;
					}
					ਰੋਕੋ;
				ਮੂਲ:
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ਵਾਪਸ rc2;
			}
			ਜੇ(qos > 127){
				log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "\t%s (denied)", sub.topic_filter);
			}ਨਹੀਂਤਾਂ{
				log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "\t%s (QoS %d)", sub.topic_filter, qos);
			}

			ਜੇ(allowed){
				rc2 = plugin__handle_subscribe(context, &sub);
				ਜੇ(rc2){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ਵਾਪਸ rc2;
				}

				rc2 = sub_ਜੋੜੋ(context, &sub);
				ਜੇ(rc2 > 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ਵਾਪਸ rc2;
				}
				ਜੇ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
					ਜੇ(rc2 == MOSQ_ERR_SUCCESS || rc2 == MOSQ_ERR_SUB_EXISTS){
						ਜੇ(retain_ਕਤਾਰ(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							ਵਾਪਸ rc;
						}
					}
				}ਨਹੀਂਤਾਂ{
					ਜੇ((retain_handling == MQTT_SUB_OPT_SEND_RETAIN_ALWAYS)
							|| (rc2 == MOSQ_ERR_SUCCESS && retain_handling == MQTT_SUB_OPT_SEND_RETAIN_NEW)){

						ਜੇ(retain_ਕਤਾਰ(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							ਵਾਪਸ rc;
						}
					}
				}
				log__printf(ਨਿਰਾ, MOSQ_LOG_SUBSCRIBE, "%s %d %s", context->id, qos, sub.topic_filter);

				plugin_persist_handle_subscription_ਜੋੜੋ(context, &sub);
			}
			mosquitto_FREE(sub.topic_filter);

			tmp_payload = mosquitto_realloc(payload, payloadlen + 1);
			ਜੇ(tmp_payload){
				payload = tmp_payload;
				payload[payloadlen] = qos;
				payloadlen++;
			}ਨਹੀਂਤਾਂ{
				mosquitto_FREE(payload);

				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
		}
	}

	ਜੇ(context->protocol != mosq_p_mqtt31){
		ਜੇ(payloadlen == 0){
			/* No subscriptions specified, protocol error. */
			ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	rc = send__suback(context, mid, payloadlen, payload);
	mosquitto_FREE(payload);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE
	db.persistence_changes++;
#ਜੇ_ਅੰਤ

	ਜੇ(context->ਬਾਹਰਲਾ_packet == ਨਿਰਾ){
		rc = db_ਸੁਨੇਹਾ_write_queued_ਬਾਹਰਲਾ(context);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
		rc = db_ਸੁਨੇਹਾ_write_inflight_ਬਾਹਰਲਾ_latest(context);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
	}

	ਵਾਪਸ rc;
}
