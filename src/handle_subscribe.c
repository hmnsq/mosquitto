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

	सचे(!context){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	सचे(context->state != mosq_cs_active){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE before session is active.", context->id);
		पटिनिवत्त MOSQ_ERR_PROTOCOL;
	}
	सचे(context->in_packet.command != (CMD_SUBSCRIBE|2)){
		पटिनिवत्त MOSQ_ERR_MALFORMED_PACKET;
	}

	log__printf(रिक्त, MOSQ_LOG_DEBUG, "Received SUBSCRIBE from %s", context->id);

	सचे(context->protocol != mosq_p_mqtt31){
		सचे((context->in_packet.command&0x0F) != 0x02){
			पटिनिवत्त MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	सचे(packet__read_uint16(&context->in_packet, &mid)){
		पटिनिवत्त MOSQ_ERR_MALFORMED_PACKET;
	}
	सचे(mid == 0){
		पटिनिवत्त MOSQ_ERR_MALFORMED_PACKET;
	}

	सचे(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_SUBSCRIBE, &context->in_packet, &properties);
		सचे(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			सचे(rc == MOSQ_ERR_PROTOCOL){
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE packet with invalid properties.", context->id);
				पटिनिवत्त MOSQ_ERR_MALFORMED_PACKET;
			}अञ्ञथा{
				पटिनिवत्त rc;
			}
		}

		सचे(mosquitto_property_read_varint(properties, MQTT_PROP_SUBSCRIPTION_IDENTIFIER,
				&subscription_identifier, false)){

			/* If the identifier was force set to 0, this is an error */
			सचे(subscription_identifier == 0){
				mosquitto_property_free_all(&properties);
				पटिनिवत्त MOSQ_ERR_PROTOCOL;
			}
		}

		mosquitto_property_free_all(&properties);
		/* Note - User Property not handled */
	}

	याव(context->in_packet.pos < context->in_packet.remaining_length){
		memset(&sub, 0, परिमाण(sub));
		sub.identifier = subscription_identifier;
		sub.properties = properties;
		सचे(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(payload);
			पटिनिवत्त MOSQ_ERR_MALFORMED_PACKET;
		}

		सचे(sub.topic_filter){
			सचे(!slen){
				log__printf(रिक्त, MOSQ_LOG_INFO,
						"Empty subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				पटिनिवत्त MOSQ_ERR_MALFORMED_PACKET;
			}
			सचे(mosquitto_sub_topic_check(sub.topic_filter)){
				log__printf(रिक्त, MOSQ_LOG_INFO,
						"Invalid subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				पटिनिवत्त MOSQ_ERR_MALFORMED_PACKET;
			}

			सचे(packet__read_byte(&context->in_packet, &sub.options)){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				पटिनिवत्त MOSQ_ERR_MALFORMED_PACKET;
			}
			सचे(sub.options & MQTT_SUB_OPT_NO_LOCAL && !strncmp(sub.topic_filter, "$share/", strlen("$share/"))){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: $share subscription with no-local set.", context->id);
				पटिनिवत्त MOSQ_ERR_PROTOCOL;
			}

			सचे(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt311){
				qos = sub.options;
				sub.options = 0;
				सचे(context->is_bridge){
					sub.options |= MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | MQTT_SUB_OPT_NO_LOCAL;
				}
			}अञ्ञथा{
				qos = sub.options & 0x03;
				sub.options &= 0xFC;

				सचे(MQTT_SUB_OPT_GET_NO_LOCAL(sub.options) && !strncmp(sub.topic_filter, "$share/", 7)){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					पटिनिवत्त MOSQ_ERR_PROTOCOL;
				}
				retain_handling = MQTT_SUB_OPT_GET_RETAIN_HANDLING(sub.options);
				सचे(retain_handling == 0x30 || (sub.options & 0xC0) != 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					पटिनिवत्त MOSQ_ERR_MALFORMED_PACKET;
				}
			}
			सचे(qos > 2){
				log__printf(रिक्त, MOSQ_LOG_INFO,
						"Invalid QoS in subscription command from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				पटिनिवत्त MOSQ_ERR_MALFORMED_PACKET;
			}
			सचे(qos > context->max_qos){
				qos = context->max_qos;
			}
			sub.options |= qos;


			सचे(context->listener && context->listener->mount_point){
				len = strlen(context->listener->mount_point) + slen + 1;
				sub_mount = mosquitto_malloc(len+1);
				सचे(!sub_mount){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					पटिनिवत्त MOSQ_ERR_NOMEM;
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
					सचे(context->protocol == mosq_p_mqtt5){
						qos = MQTT_RC_NOT_AUTHORIZED;
					}अञ्ञथा सचे(context->protocol == mosq_p_mqtt311){
						qos = 0x80;
					}
					भङ्ग;
				स्वभाव:
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					पटिनिवत्त rc2;
			}
			सचे(qos > 127){
				log__printf(रिक्त, MOSQ_LOG_DEBUG, "\t%s (denied)", sub.topic_filter);
			}अञ्ञथा{
				log__printf(रिक्त, MOSQ_LOG_DEBUG, "\t%s (QoS %d)", sub.topic_filter, qos);
			}

			सचे(allowed){
				rc2 = plugin__handle_subscribe(context, &sub);
				सचे(rc2){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					पटिनिवत्त rc2;
				}

				rc2 = sub__add(context, &sub);
				सचे(rc2 > 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					पटिनिवत्त rc2;
				}
				सचे(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
					सचे(rc2 == MOSQ_ERR_SUCCESS || rc2 == MOSQ_ERR_SUB_EXISTS){
						सचे(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							पटिनिवत्त rc;
						}
					}
				}अञ्ञथा{
					सचे((retain_handling == MQTT_SUB_OPT_SEND_RETAIN_ALWAYS)
							|| (rc2 == MOSQ_ERR_SUCCESS && retain_handling == MQTT_SUB_OPT_SEND_RETAIN_NEW)){

						सचे(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							पटिनिवत्त rc;
						}
					}
				}
				log__printf(रिक्त, MOSQ_LOG_SUBSCRIBE, "%s %d %s", context->id, qos, sub.topic_filter);

				plugin_persist__handle_subscription_add(context, &sub);
			}
			mosquitto_FREE(sub.topic_filter);

			tmp_payload = mosquitto_realloc(payload, payloadlen + 1);
			सचे(tmp_payload){
				payload = tmp_payload;
				payload[payloadlen] = qos;
				payloadlen++;
			}अञ्ञथा{
				mosquitto_FREE(payload);

				पटिनिवत्त MOSQ_ERR_NOMEM;
			}
		}
	}

	सचे(context->protocol != mosq_p_mqtt31){
		सचे(payloadlen == 0){
			/* No subscriptions specified, protocol error. */
			पटिनिवत्त MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	rc = send__suback(context, mid, payloadlen, payload);
	mosquitto_FREE(payload);

#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	सचे(context->out_packet == रिक्त){
		rc = db__message_write_queued_out(context);
		सचे(rc){
			पटिनिवत्त rc;
		}
		rc = db__message_write_inflight_out_latest(context);
		सचे(rc){
			पटिनिवत्त rc;
		}
	}

	पटिनिवत्त rc;
}
