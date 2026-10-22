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


पूर्णांक handle__subscribe(संरचना mosquitto *context)
{
	पूर्णांक rc = 0;
	पूर्णांक rc2;
	uint16_t mid;
	uint8_t qos;
	uint8_t retain_handling = 0;
	uint8_t *payload = रिक्त, *tmp_payload;
	uint32_t payloadlen = 0;
	माप_प्रकार len;
	uint16_t slen;
	अक्षर *sub_mount;
	mosquitto_property *properties = रिक्त;
	bool allowed;
	संरचना mosquitto_subscription sub;
	uint32_t subscription_identifier = 0;

	जर(!context){
		परत MOSQ_ERR_INVAL;
	}

	जर(context->state != mosq_cs_active){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE before session is active.", context->id);
		परत MOSQ_ERR_PROTOCOL;
	}
	जर(context->in_packet.command != (CMD_SUBSCRIBE|2)){
		परत MOSQ_ERR_MALFORMED_PACKET;
	}

	log__printf(रिक्त, MOSQ_LOG_DEBUG, "Received SUBSCRIBE from %s", context->id);

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
		rc = property__read_all(CMD_SUBSCRIBE, &context->in_packet, &properties);
		जर(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			जर(rc == MOSQ_ERR_PROTOCOL){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE packet with invalid properties.", context->id);
				परत MOSQ_ERR_MALFORMED_PACKET;
			}अन्यथा{
				परत rc;
			}
		}

		जर(mosquitto_property_read_varint(properties, MQTT_PROP_SUBSCRIPTION_IDENTIFIER,
				&subscription_identifier, false)){

			/* If the identifier was force set to 0, this is an error */
			जर(subscription_identifier == 0){
				mosquitto_property_free_all(&properties);
				परत MOSQ_ERR_PROTOCOL;
			}
		}

		mosquitto_property_free_all(&properties);
		/* Note - User Property not handled */
	}

	पर्यंत(context->in_packet.pos < context->in_packet.remaining_length){
		स_रखो(&sub, 0, आकार(sub));
		sub.identifier = subscription_identifier;
		sub.properties = properties;
		जर(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(payload);
			परत MOSQ_ERR_MALFORMED_PACKET;
		}

		जर(sub.topic_filter){
			जर(!slen){
				log__printf(रिक्त, MOSQ_LOG_INFO,
						"Empty subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				परत MOSQ_ERR_MALFORMED_PACKET;
			}
			जर(mosquitto_sub_topic_check(sub.topic_filter)){
				log__printf(रिक्त, MOSQ_LOG_INFO,
						"Invalid subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				परत MOSQ_ERR_MALFORMED_PACKET;
			}

			जर(packet__read_byte(&context->in_packet, &sub.options)){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				परत MOSQ_ERR_MALFORMED_PACKET;
			}
			जर(sub.options & MQTT_SUB_OPT_NO_LOCAL && !म_भेदन(sub.topic_filter, "$share/", म_माप("$share/"))){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: $share subscription with no-local set.", context->id);
				परत MOSQ_ERR_PROTOCOL;
			}

			जर(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt311){
				qos = sub.options;
				sub.options = 0;
				जर(context->is_bridge){
					sub.options |= MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | MQTT_SUB_OPT_NO_LOCAL;
				}
			}अन्यथा{
				qos = sub.options & 0x03;
				sub.options &= 0xFC;

				जर(MQTT_SUB_OPT_GET_NO_LOCAL(sub.options) && !म_भेदन(sub.topic_filter, "$share/", 7)){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					परत MOSQ_ERR_PROTOCOL;
				}
				retain_handling = MQTT_SUB_OPT_GET_RETAIN_HANDLING(sub.options);
				जर(retain_handling == 0x30 || (sub.options & 0xC0) != 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					परत MOSQ_ERR_MALFORMED_PACKET;
				}
			}
			जर(qos > 2){
				log__printf(रिक्त, MOSQ_LOG_INFO,
						"Invalid QoS in subscription command from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				परत MOSQ_ERR_MALFORMED_PACKET;
			}
			जर(qos > context->max_qos){
				qos = context->max_qos;
			}
			sub.options |= qos;


			जर(context->listener && context->listener->mount_point){
				len = म_माप(context->listener->mount_point) + slen + 1;
				sub_mount = mosquitto_malloc(len+1);
				जर(!sub_mount){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					परत MOSQ_ERR_NOMEM;
				}
				snprintf(sub_mount, len, "%s%s", context->listener->mount_point, sub.topic_filter);
				sub_mount[len] = '\0';

				mosquitto_FREE(sub.topic_filter);
				sub.topic_filter = sub_mount;

			}

			allowed = true;
			rc2 = mosquitto_acl_check(context, sub.topic_filter, 0, रिक्त, qos, false, properties, MOSQ_ACL_SUBSCRIBE);
			निवड(rc2){
				स्थिती MOSQ_ERR_SUCCESS:
					थांबा;
				स्थिती MOSQ_ERR_ACL_DENIED:
					allowed = false;
					जर(context->protocol == mosq_p_mqtt5){
						qos = MQTT_RC_NOT_AUTHORIZED;
					}अन्यथा जर(context->protocol == mosq_p_mqtt311){
						qos = 0x80;
					}
					थांबा;
				पूर्वनिर्धारित:
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					परत rc2;
			}
			जर(qos > 127){
				log__printf(रिक्त, MOSQ_LOG_DEBUG, "\t%s (denied)", sub.topic_filter);
			}अन्यथा{
				log__printf(रिक्त, MOSQ_LOG_DEBUG, "\t%s (QoS %d)", sub.topic_filter, qos);
			}

			जर(allowed){
				rc2 = plugin__handle_subscribe(context, &sub);
				जर(rc2){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					परत rc2;
				}

				rc2 = sub__add(context, &sub);
				जर(rc2 > 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					परत rc2;
				}
				जर(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
					जर(rc2 == MOSQ_ERR_SUCCESS || rc2 == MOSQ_ERR_SUB_EXISTS){
						जर(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							परत rc;
						}
					}
				}अन्यथा{
					जर((retain_handling == MQTT_SUB_OPT_SEND_RETAIN_ALWAYS)
							|| (rc2 == MOSQ_ERR_SUCCESS && retain_handling == MQTT_SUB_OPT_SEND_RETAIN_NEW)){

						जर(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							परत rc;
						}
					}
				}
				log__printf(रिक्त, MOSQ_LOG_SUBSCRIBE, "%s %d %s", context->id, qos, sub.topic_filter);

				plugin_persist__handle_subscription_add(context, &sub);
			}
			mosquitto_FREE(sub.topic_filter);

			tmp_payload = mosquitto_realloc(payload, payloadlen + 1);
			जर(tmp_payload){
				payload = tmp_payload;
				payload[payloadlen] = qos;
				payloadlen++;
			}अन्यथा{
				mosquitto_FREE(payload);

				परत MOSQ_ERR_NOMEM;
			}
		}
	}

	जर(context->protocol != mosq_p_mqtt31){
		जर(payloadlen == 0){
			/* No subscriptions specified, protocol error. */
			परत MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	rc = send__suback(context, mid, payloadlen, payload);
	mosquitto_FREE(payload);

#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	जर(context->out_packet == रिक्त){
		rc = db__message_write_queued_out(context);
		जर(rc){
			परत rc;
		}
		rc = db__message_write_inflight_out_latest(context);
		जर(rc){
			परत rc;
		}
	}

	परत rc;
}
