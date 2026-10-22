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
#समावेश "property_mosq.h"


पूर्णांक handle__subscribe(काष्ठा mosquitto *context)
{
	पूर्णांक rc = 0;
	पूर्णांक rc2;
	uint16_t mid;
	uint8_t qos;
	uint8_t retain_handling = 0;
	uint8_t *payload = शून्य, *tmp_payload;
	uint32_t payloadlen = 0;
	माप_प्रकार len;
	uint16_t slen;
	अक्षर *sub_mount;
	mosquitto_property *properties = शून्य;
	bool allowed;
	काष्ठा mosquitto_subscription sub;
	uint32_t subscription_identifier = 0;

	अगर(!context){
		वापस MOSQ_ERR_INVAL;
	}

	अगर(context->state != mosq_cs_active){
		log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE before session is active.", context->id);
		वापस MOSQ_ERR_PROTOCOL;
	}
	अगर(context->in_packet.command != (CMD_SUBSCRIBE|2)){
		वापस MOSQ_ERR_MALFORMED_PACKET;
	}

	log__printf(शून्य, MOSQ_LOG_DEBUG, "Received SUBSCRIBE from %s", context->id);

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
		rc = property__read_all(CMD_SUBSCRIBE, &context->in_packet, &properties);
		अगर(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			अगर(rc == MOSQ_ERR_PROTOCOL){
				log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE packet with invalid properties.", context->id);
				वापस MOSQ_ERR_MALFORMED_PACKET;
			}अन्यथा{
				वापस rc;
			}
		}

		अगर(mosquitto_property_read_varint(properties, MQTT_PROP_SUBSCRIPTION_IDENTIFIER,
				&subscription_identifier, false)){

			/* If the identifier was force set to 0, this is an error */
			अगर(subscription_identifier == 0){
				mosquitto_property_free_all(&properties);
				वापस MOSQ_ERR_PROTOCOL;
			}
		}

		mosquitto_property_free_all(&properties);
		/* Note - User Property not handled */
	}

	जबतक(context->in_packet.pos < context->in_packet.remaining_लंबाई){
		स_रखो(&sub, 0, माप(sub));
		sub.identifier = subscription_identifier;
		sub.properties = properties;
		अगर(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(payload);
			वापस MOSQ_ERR_MALFORMED_PACKET;
		}

		अगर(sub.topic_filter){
			अगर(!slen){
				log__printf(शून्य, MOSQ_LOG_INFO,
						"Empty subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				वापस MOSQ_ERR_MALFORMED_PACKET;
			}
			अगर(mosquitto_sub_topic_check(sub.topic_filter)){
				log__printf(शून्य, MOSQ_LOG_INFO,
						"Invalid subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				वापस MOSQ_ERR_MALFORMED_PACKET;
			}

			अगर(packet__read_byte(&context->in_packet, &sub.options)){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				वापस MOSQ_ERR_MALFORMED_PACKET;
			}
			अगर(sub.options & MQTT_SUB_OPT_NO_LOCAL && !म_भेदन(sub.topic_filter, "$share/", म_माप("$share/"))){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: $share subscription with no-local set.", context->id);
				वापस MOSQ_ERR_PROTOCOL;
			}

			अगर(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt311){
				qos = sub.options;
				sub.options = 0;
				अगर(context->is_bridge){
					sub.options |= MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | MQTT_SUB_OPT_NO_LOCAL;
				}
			}अन्यथा{
				qos = sub.options & 0x03;
				sub.options &= 0xFC;

				अगर(MQTT_SUB_OPT_GET_NO_LOCAL(sub.options) && !म_भेदन(sub.topic_filter, "$share/", 7)){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					वापस MOSQ_ERR_PROTOCOL;
				}
				retain_handling = MQTT_SUB_OPT_GET_RETAIN_HANDLING(sub.options);
				अगर(retain_handling == 0x30 || (sub.options & 0xC0) != 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					वापस MOSQ_ERR_MALFORMED_PACKET;
				}
			}
			अगर(qos > 2){
				log__printf(शून्य, MOSQ_LOG_INFO,
						"Invalid QoS in subscription command from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				वापस MOSQ_ERR_MALFORMED_PACKET;
			}
			अगर(qos > context->max_qos){
				qos = context->max_qos;
			}
			sub.options |= qos;


			अगर(context->listener && context->listener->mount_point){
				len = म_माप(context->listener->mount_point) + slen + 1;
				sub_mount = mosquitto_malloc(len+1);
				अगर(!sub_mount){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					वापस MOSQ_ERR_NOMEM;
				}
				snprintf(sub_mount, len, "%s%s", context->listener->mount_point, sub.topic_filter);
				sub_mount[len] = '\0';

				mosquitto_FREE(sub.topic_filter);
				sub.topic_filter = sub_mount;

			}

			allowed = true;
			rc2 = mosquitto_acl_check(context, sub.topic_filter, 0, शून्य, qos, false, properties, MOSQ_ACL_SUBSCRIBE);
			चयन(rc2){
				हाल MOSQ_ERR_SUCCESS:
					अवरोध;
				हाल MOSQ_ERR_ACL_DENIED:
					allowed = false;
					अगर(context->protocol == mosq_p_mqtt5){
						qos = MQTT_RC_NOT_AUTHORIZED;
					}अन्यथा अगर(context->protocol == mosq_p_mqtt311){
						qos = 0x80;
					}
					अवरोध;
				शेष:
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					वापस rc2;
			}
			अगर(qos > 127){
				log__printf(शून्य, MOSQ_LOG_DEBUG, "\t%s (denied)", sub.topic_filter);
			}अन्यथा{
				log__printf(शून्य, MOSQ_LOG_DEBUG, "\t%s (QoS %d)", sub.topic_filter, qos);
			}

			अगर(allowed){
				rc2 = plugin__handle_subscribe(context, &sub);
				अगर(rc2){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					वापस rc2;
				}

				rc2 = sub__add(context, &sub);
				अगर(rc2 > 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					वापस rc2;
				}
				अगर(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
					अगर(rc2 == MOSQ_ERR_SUCCESS || rc2 == MOSQ_ERR_SUB_EXISTS){
						अगर(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							वापस rc;
						}
					}
				}अन्यथा{
					अगर((retain_handling == MQTT_SUB_OPT_SEND_RETAIN_ALWAYS)
							|| (rc2 == MOSQ_ERR_SUCCESS && retain_handling == MQTT_SUB_OPT_SEND_RETAIN_NEW)){

						अगर(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							वापस rc;
						}
					}
				}
				log__printf(शून्य, MOSQ_LOG_SUBSCRIBE, "%s %d %s", context->id, qos, sub.topic_filter);

				plugin_persist__handle_subscription_add(context, &sub);
			}
			mosquitto_FREE(sub.topic_filter);

			tmp_payload = mosquitto_realloc(payload, payloadlen + 1);
			अगर(tmp_payload){
				payload = tmp_payload;
				payload[payloadlen] = qos;
				payloadlen++;
			}अन्यथा{
				mosquitto_FREE(payload);

				वापस MOSQ_ERR_NOMEM;
			}
		}
	}

	अगर(context->protocol != mosq_p_mqtt31){
		अगर(payloadlen == 0){
			/* No subscriptions specified, protocol error. */
			वापस MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	rc = send__suback(context, mid, payloadlen, payload);
	mosquitto_FREE(payload);

#अगर_घोषित WITH_PERSISTENCE
	db.persistence_changes++;
#पूर्ण_अगर

	अगर(context->out_packet == शून्य){
		rc = db__message_write_queued_out(context);
		अगर(rc){
			वापस rc;
		}
		rc = db__message_write_inflight_out_latest(context);
		अगर(rc){
			वापस rc;
		}
	}

	वापस rc;
}
