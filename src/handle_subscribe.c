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
	माप_प्रकार len;
	uint16_t slen;
	वर्ण *sub_mount;
	mosquitto_property *properties = रिक्त;
	bool allowed;
	संरचना mosquitto_subscription sub;
	uint32_t subscription_identifier = 0;

	यदि(!context){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	यदि(context->state != mosq_cs_active){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE before session is active.", context->id);
		फर्काऊ MOSQ_ERR_PROTOCOL;
	}
	यदि(context->in_packet.command != (CMD_SUBSCRIBE|2)){
		फर्काऊ MOSQ_ERR_MALFORMED_PACKET;
	}

	log__printf(रिक्त, MOSQ_LOG_DEBUG, "Received SUBSCRIBE from %s", context->id);

	यदि(context->protocol != mosq_p_mqtt31){
		यदि((context->in_packet.command&0x0F) != 0x02){
			फर्काऊ MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	यदि(packet__read_uint16(&context->in_packet, &mid)){
		फर्काऊ MOSQ_ERR_MALFORMED_PACKET;
	}
	यदि(mid == 0){
		फर्काऊ MOSQ_ERR_MALFORMED_PACKET;
	}

	यदि(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_SUBSCRIBE, &context->in_packet, &properties);
		यदि(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			यदि(rc == MOSQ_ERR_PROTOCOL){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE packet with invalid properties.", context->id);
				फर्काऊ MOSQ_ERR_MALFORMED_PACKET;
			}अन्यथा{
				फर्काऊ rc;
			}
		}

		यदि(mosquitto_property_read_varint(properties, MQTT_PROP_SUBSCRIPTION_IDENTIFIER,
				&subscription_identifier, false)){

			/* If the identifier was force set to 0, this is an error */
			यदि(subscription_identifier == 0){
				mosquitto_property_free_all(&properties);
				फर्काऊ MOSQ_ERR_PROTOCOL;
			}
		}

		mosquitto_property_free_all(&properties);
		/* Note - User Property not handled */
	}

	जबसम्म(context->in_packet.pos < context->in_packet.remaining_length){
		स_रखो(&sub, 0, परिमाण(sub));
		sub.identifier = subscription_identifier;
		sub.properties = properties;
		यदि(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(payload);
			फर्काऊ MOSQ_ERR_MALFORMED_PACKET;
		}

		यदि(sub.topic_filter){
			यदि(!slen){
				log__printf(रिक्त, MOSQ_LOG_INFO,
						"Empty subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				फर्काऊ MOSQ_ERR_MALFORMED_PACKET;
			}
			यदि(mosquitto_sub_topic_check(sub.topic_filter)){
				log__printf(रिक्त, MOSQ_LOG_INFO,
						"Invalid subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				फर्काऊ MOSQ_ERR_MALFORMED_PACKET;
			}

			यदि(packet__read_byte(&context->in_packet, &sub.options)){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				फर्काऊ MOSQ_ERR_MALFORMED_PACKET;
			}
			यदि(sub.options & MQTT_SUB_OPT_NO_LOCAL && !म_भेदन(sub.topic_filter, "$share/", म_माप("$share/"))){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: $share subscription with no-local set.", context->id);
				फर्काऊ MOSQ_ERR_PROTOCOL;
			}

			यदि(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt311){
				qos = sub.options;
				sub.options = 0;
				यदि(context->is_bridge){
					sub.options |= MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | MQTT_SUB_OPT_NO_LOCAL;
				}
			}अन्यथा{
				qos = sub.options & 0x03;
				sub.options &= 0xFC;

				यदि(MQTT_SUB_OPT_GET_NO_LOCAL(sub.options) && !म_भेदन(sub.topic_filter, "$share/", 7)){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					फर्काऊ MOSQ_ERR_PROTOCOL;
				}
				retain_handling = MQTT_SUB_OPT_GET_RETAIN_HANDLING(sub.options);
				यदि(retain_handling == 0x30 || (sub.options & 0xC0) != 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					फर्काऊ MOSQ_ERR_MALFORMED_PACKET;
				}
			}
			यदि(qos > 2){
				log__printf(रिक्त, MOSQ_LOG_INFO,
						"Invalid QoS in subscription command from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				फर्काऊ MOSQ_ERR_MALFORMED_PACKET;
			}
			यदि(qos > context->max_qos){
				qos = context->max_qos;
			}
			sub.options |= qos;


			यदि(context->listener && context->listener->mount_point){
				len = म_माप(context->listener->mount_point) + slen + 1;
				sub_mount = mosquitto_malloc(len+1);
				यदि(!sub_mount){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					फर्काऊ MOSQ_ERR_NOMEM;
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
					रोक;
				पक्ष MOSQ_ERR_ACL_DENIED:
					allowed = false;
					यदि(context->protocol == mosq_p_mqtt5){
						qos = MQTT_RC_NOT_AUTHORIZED;
					}अन्यथा यदि(context->protocol == mosq_p_mqtt311){
						qos = 0x80;
					}
					रोक;
				स्वभाव:
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					फर्काऊ rc2;
			}
			यदि(qos > 127){
				log__printf(रिक्त, MOSQ_LOG_DEBUG, "\t%s (denied)", sub.topic_filter);
			}अन्यथा{
				log__printf(रिक्त, MOSQ_LOG_DEBUG, "\t%s (QoS %d)", sub.topic_filter, qos);
			}

			यदि(allowed){
				rc2 = plugin__handle_subscribe(context, &sub);
				यदि(rc2){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					फर्काऊ rc2;
				}

				rc2 = sub__add(context, &sub);
				यदि(rc2 > 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					फर्काऊ rc2;
				}
				यदि(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
					यदि(rc2 == MOSQ_ERR_SUCCESS || rc2 == MOSQ_ERR_SUB_EXISTS){
						यदि(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							फर्काऊ rc;
						}
					}
				}अन्यथा{
					यदि((retain_handling == MQTT_SUB_OPT_SEND_RETAIN_ALWAYS)
							|| (rc2 == MOSQ_ERR_SUCCESS && retain_handling == MQTT_SUB_OPT_SEND_RETAIN_NEW)){

						यदि(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							फर्काऊ rc;
						}
					}
				}
				log__printf(रिक्त, MOSQ_LOG_SUBSCRIBE, "%s %d %s", context->id, qos, sub.topic_filter);

				plugin_persist__handle_subscription_add(context, &sub);
			}
			mosquitto_FREE(sub.topic_filter);

			tmp_payload = mosquitto_realloc(payload, payloadlen + 1);
			यदि(tmp_payload){
				payload = tmp_payload;
				payload[payloadlen] = qos;
				payloadlen++;
			}अन्यथा{
				mosquitto_FREE(payload);

				फर्काऊ MOSQ_ERR_NOMEM;
			}
		}
	}

	यदि(context->protocol != mosq_p_mqtt31){
		यदि(payloadlen == 0){
			/* No subscriptions specified, protocol error. */
			फर्काऊ MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	rc = send__suback(context, mid, payloadlen, payload);
	mosquitto_FREE(payload);

#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	यदि(context->out_packet == रिक्त){
		rc = db__message_write_queued_out(context);
		यदि(rc){
			फर्काऊ rc;
		}
		rc = db__message_write_inflight_out_latest(context);
		यदि(rc){
			फर्काऊ rc;
		}
	}

	फर्काऊ rc;
}
