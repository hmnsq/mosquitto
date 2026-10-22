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
#अन्तर्भाव "property_mosq.h"


पूर्णाङ्क handle__subscribe(संरचना mosquitto *context)
{
	पूर्णाङ्क rc = 0;
	पूर्णाङ्क rc2;
	uint16_t mid;
	uint8_t qos;
	uint8_t retain_handling = 0;
	uint8_t *payload = रिक्त, *tmp_payload;
	uint32_t payloadlen = 0;
	size_t len;
	uint16_t slen;
	वर्ण *sub_mount;
	mosquitto_property *properties = रिक्त;
	bool allowed;
	संरचना mosquitto_subscription sub;
	uint32_t subscription_identifier = 0;

	जइ(!context){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	जइ(context->state != mosq_cs_active){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE before session is active.", context->id);
		प्रत्यागम MOSQ_ERR_PROTOCOL;
	}
	जइ(context->in_packet.command != (CMD_SUBSCRIBE|2)){
		प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
	}

	log__printf(रिक्त, MOSQ_LOG_DEBUG, "Received SUBSCRIBE from %s", context->id);

	जइ(context->protocol != mosq_p_mqtt31){
		जइ((context->in_packet.command&0x0F) != 0x02){
			प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	जइ(packet__read_uint16(&context->in_packet, &mid)){
		प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
	}
	जइ(mid == 0){
		प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
	}

	जइ(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_SUBSCRIBE, &context->in_packet, &properties);
		जइ(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			जइ(rc == MOSQ_ERR_PROTOCOL){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE packet with invalid properties.", context->id);
				प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
			}अण्णहा{
				प्रत्यागम rc;
			}
		}

		जइ(mosquitto_property_read_varint(properties, MQTT_PROP_SUBSCRIPTION_IDENTIFIER,
				&subscription_identifier, false)){

			/* If the identifier was force set to 0, this is an error */
			जइ(subscription_identifier == 0){
				mosquitto_property_free_all(&properties);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}
		}

		mosquitto_property_free_all(&properties);
		/* Note - User Property not handled */
	}

	जाव(context->in_packet.pos < context->in_packet.remaining_length){
		memset(&sub, 0, परिमाण(sub));
		sub.identifier = subscription_identifier;
		sub.properties = properties;
		जइ(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(payload);
			प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
		}

		जइ(sub.topic_filter){
			जइ(!slen){
				log__printf(रिक्त, MOSQ_LOG_INFO,
						"Empty subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
			}
			जइ(mosquitto_sub_topic_check(sub.topic_filter)){
				log__printf(रिक्त, MOSQ_LOG_INFO,
						"Invalid subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
			}

			जइ(packet__read_byte(&context->in_packet, &sub.options)){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
			}
			जइ(sub.options & MQTT_SUB_OPT_NO_LOCAL && !strncmp(sub.topic_filter, "$share/", strlen("$share/"))){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: $share subscription with no-local set.", context->id);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}

			जइ(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt311){
				qos = sub.options;
				sub.options = 0;
				जइ(context->is_bridge){
					sub.options |= MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | MQTT_SUB_OPT_NO_LOCAL;
				}
			}अण्णहा{
				qos = sub.options & 0x03;
				sub.options &= 0xFC;

				जइ(MQTT_SUB_OPT_GET_NO_LOCAL(sub.options) && !strncmp(sub.topic_filter, "$share/", 7)){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					प्रत्यागम MOSQ_ERR_PROTOCOL;
				}
				retain_handling = MQTT_SUB_OPT_GET_RETAIN_HANDLING(sub.options);
				जइ(retain_handling == 0x30 || (sub.options & 0xC0) != 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
				}
			}
			जइ(qos > 2){
				log__printf(रिक्त, MOSQ_LOG_INFO,
						"Invalid QoS in subscription command from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
			}
			जइ(qos > context->max_qos){
				qos = context->max_qos;
			}
			sub.options |= qos;


			जइ(context->listener && context->listener->mount_point){
				len = strlen(context->listener->mount_point) + slen + 1;
				sub_mount = mosquitto_malloc(len+1);
				जइ(!sub_mount){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					प्रत्यागम MOSQ_ERR_NOMEM;
				}
				snprintf(sub_mount, len, "%s%s", context->listener->mount_point, sub.topic_filter);
				sub_mount[len] = '\0';

				mosquitto_FREE(sub.topic_filter);
				sub.topic_filter = sub_mount;

			}

			allowed = true;
			rc2 = mosquitto_acl_check(context, sub.topic_filter, 0, रिक्त, qos, false, properties, MOSQ_ACL_SUBSCRIBE);
			विकल्प(rc2){
				पक्ष MOSQ_ERR_SUCCESS:
					भङ्ग;
				पक्ष MOSQ_ERR_ACL_DENIED:
					allowed = false;
					जइ(context->protocol == mosq_p_mqtt5){
						qos = MQTT_RC_NOT_AUTHORIZED;
					}अण्णहा जइ(context->protocol == mosq_p_mqtt311){
						qos = 0x80;
					}
					भङ्ग;
				स्वभाव:
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					प्रत्यागम rc2;
			}
			जइ(qos > 127){
				log__printf(रिक्त, MOSQ_LOG_DEBUG, "\t%s (denied)", sub.topic_filter);
			}अण्णहा{
				log__printf(रिक्त, MOSQ_LOG_DEBUG, "\t%s (QoS %d)", sub.topic_filter, qos);
			}

			जइ(allowed){
				rc2 = plugin__handle_subscribe(context, &sub);
				जइ(rc2){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					प्रत्यागम rc2;
				}

				rc2 = sub__add(context, &sub);
				जइ(rc2 > 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					प्रत्यागम rc2;
				}
				जइ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
					जइ(rc2 == MOSQ_ERR_SUCCESS || rc2 == MOSQ_ERR_SUB_EXISTS){
						जइ(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							प्रत्यागम rc;
						}
					}
				}अण्णहा{
					जइ((retain_handling == MQTT_SUB_OPT_SEND_RETAIN_ALWAYS)
							|| (rc2 == MOSQ_ERR_SUCCESS && retain_handling == MQTT_SUB_OPT_SEND_RETAIN_NEW)){

						जइ(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							प्रत्यागम rc;
						}
					}
				}
				log__printf(रिक्त, MOSQ_LOG_SUBSCRIBE, "%s %d %s", context->id, qos, sub.topic_filter);

				plugin_persist__handle_subscription_add(context, &sub);
			}
			mosquitto_FREE(sub.topic_filter);

			tmp_payload = mosquitto_realloc(payload, payloadlen + 1);
			जइ(tmp_payload){
				payload = tmp_payload;
				payload[payloadlen] = qos;
				payloadlen++;
			}अण्णहा{
				mosquitto_FREE(payload);

				प्रत्यागम MOSQ_ERR_NOMEM;
			}
		}
	}

	जइ(context->protocol != mosq_p_mqtt31){
		जइ(payloadlen == 0){
			/* No subscriptions specified, protocol error. */
			प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	rc = send__suback(context, mid, payloadlen, payload);
	mosquitto_FREE(payload);

#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	जइ(context->out_packet == रिक्त){
		rc = db__message_write_queued_out(context);
		जइ(rc){
			प्रत्यागम rc;
		}
		rc = db__message_write_inflight_out_latest(context);
		जइ(rc){
			प्रत्यागम rc;
		}
	}

	प्रत्यागम rc;
}
