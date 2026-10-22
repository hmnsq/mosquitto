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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "packet_mosq.h"
#অন্তর্ভাব "property_mosq.h"


পূর্ণাঙ্ক handle__subscribe(সংরচনা mosquitto *context)
{
	পূর্ণাঙ্ক rc = 0;
	পূর্ণাঙ্ক rc2;
	uint16_t mid;
	uint8_t qos;
	uint8_t retain_handling = 0;
	uint8_t *payload = রিক্ত, *tmp_payload;
	uint32_t payloadlen = 0;
	size_t len;
	uint16_t slen;
	বর্ণ *sub_mount;
	mosquitto_property *properties = রিক্ত;
	bool allowed;
	সংরচনা mosquitto_subscription sub;
	uint32_t subscription_identifier = 0;

	যদি(!context){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	যদি(context->state != mosq_cs_active){
		log__printf(রিক্ত, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE before session is active.", context->id);
		প্রত্যাগম MOSQ_ERR_PROTOCOL;
	}
	যদি(context->in_packet.command != (CMD_SUBSCRIBE|2)){
		প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
	}

	log__printf(রিক্ত, MOSQ_LOG_DEBUG, "Received SUBSCRIBE from %s", context->id);

	যদি(context->protocol != mosq_p_mqtt31){
		যদি((context->in_packet.command&0x0F) != 0x02){
			প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	যদি(packet__read_uint16(&context->in_packet, &mid)){
		প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
	}
	যদি(mid == 0){
		প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
	}

	যদি(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_SUBSCRIBE, &context->in_packet, &properties);
		যদি(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			যদি(rc == MOSQ_ERR_PROTOCOL){
				log__printf(রিক্ত, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE packet with invalid properties.", context->id);
				প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
			}অন্যথা{
				প্রত্যাগম rc;
			}
		}

		যদি(mosquitto_property_read_varint(properties, MQTT_PROP_SUBSCRIPTION_IDENTIFIER,
				&subscription_identifier, false)){

			/* If the identifier was force set to 0, this is an error */
			যদি(subscription_identifier == 0){
				mosquitto_property_free_all(&properties);
				প্রত্যাগম MOSQ_ERR_PROTOCOL;
			}
		}

		mosquitto_property_free_all(&properties);
		/* Note - User Property not handled */
	}

	যাবত্(context->in_packet.pos < context->in_packet.remaining_length){
		memset(&sub, 0, পরিমাণ(sub));
		sub.identifier = subscription_identifier;
		sub.properties = properties;
		যদি(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(payload);
			প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
		}

		যদি(sub.topic_filter){
			যদি(!slen){
				log__printf(রিক্ত, MOSQ_LOG_INFO,
						"Empty subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
			}
			যদি(mosquitto_sub_topic_check(sub.topic_filter)){
				log__printf(রিক্ত, MOSQ_LOG_INFO,
						"Invalid subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
			}

			যদি(packet__read_byte(&context->in_packet, &sub.options)){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
			}
			যদি(sub.options & MQTT_SUB_OPT_NO_LOCAL && !strncmp(sub.topic_filter, "$share/", strlen("$share/"))){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				log__printf(রিক্ত, MOSQ_LOG_INFO, "Protocol error from %s: $share subscription with no-local set.", context->id);
				প্রত্যাগম MOSQ_ERR_PROTOCOL;
			}

			যদি(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt311){
				qos = sub.options;
				sub.options = 0;
				যদি(context->is_bridge){
					sub.options |= MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | MQTT_SUB_OPT_NO_LOCAL;
				}
			}অন্যথা{
				qos = sub.options & 0x03;
				sub.options &= 0xFC;

				যদি(MQTT_SUB_OPT_GET_NO_LOCAL(sub.options) && !strncmp(sub.topic_filter, "$share/", 7)){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					প্রত্যাগম MOSQ_ERR_PROTOCOL;
				}
				retain_handling = MQTT_SUB_OPT_GET_RETAIN_HANDLING(sub.options);
				যদি(retain_handling == 0x30 || (sub.options & 0xC0) != 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
				}
			}
			যদি(qos > 2){
				log__printf(রিক্ত, MOSQ_LOG_INFO,
						"Invalid QoS in subscription command from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
			}
			যদি(qos > context->max_qos){
				qos = context->max_qos;
			}
			sub.options |= qos;


			যদি(context->listener && context->listener->mount_point){
				len = strlen(context->listener->mount_point) + slen + 1;
				sub_mount = mosquitto_malloc(len+1);
				যদি(!sub_mount){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					প্রত্যাগম MOSQ_ERR_NOMEM;
				}
				snprintf(sub_mount, len, "%s%s", context->listener->mount_point, sub.topic_filter);
				sub_mount[len] = '\0';

				mosquitto_FREE(sub.topic_filter);
				sub.topic_filter = sub_mount;

			}

			allowed = true;
			rc2 = mosquitto_acl_check(context, sub.topic_filter, 0, রিক্ত, qos, false, properties, MOSQ_ACL_SUBSCRIBE);
			বিকল্প(rc2){
				পক্ষ MOSQ_ERR_SUCCESS:
					ভঙ্গ;
				পক্ষ MOSQ_ERR_ACL_DENIED:
					allowed = false;
					যদি(context->protocol == mosq_p_mqtt5){
						qos = MQTT_RC_NOT_AUTHORIZED;
					}অন্যথা যদি(context->protocol == mosq_p_mqtt311){
						qos = 0x80;
					}
					ভঙ্গ;
				স্বভাব:
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					প্রত্যাগম rc2;
			}
			যদি(qos > 127){
				log__printf(রিক্ত, MOSQ_LOG_DEBUG, "\t%s (denied)", sub.topic_filter);
			}অন্যথা{
				log__printf(রিক্ত, MOSQ_LOG_DEBUG, "\t%s (QoS %d)", sub.topic_filter, qos);
			}

			যদি(allowed){
				rc2 = plugin__handle_subscribe(context, &sub);
				যদি(rc2){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					প্রত্যাগম rc2;
				}

				rc2 = sub__add(context, &sub);
				যদি(rc2 > 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					প্রত্যাগম rc2;
				}
				যদি(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
					যদি(rc2 == MOSQ_ERR_SUCCESS || rc2 == MOSQ_ERR_SUB_EXISTS){
						যদি(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							প্রত্যাগম rc;
						}
					}
				}অন্যথা{
					যদি((retain_handling == MQTT_SUB_OPT_SEND_RETAIN_ALWAYS)
							|| (rc2 == MOSQ_ERR_SUCCESS && retain_handling == MQTT_SUB_OPT_SEND_RETAIN_NEW)){

						যদি(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							প্রত্যাগম rc;
						}
					}
				}
				log__printf(রিক্ত, MOSQ_LOG_SUBSCRIBE, "%s %d %s", context->id, qos, sub.topic_filter);

				plugin_persist__handle_subscription_add(context, &sub);
			}
			mosquitto_FREE(sub.topic_filter);

			tmp_payload = mosquitto_realloc(payload, payloadlen + 1);
			যদি(tmp_payload){
				payload = tmp_payload;
				payload[payloadlen] = qos;
				payloadlen++;
			}অন্যথা{
				mosquitto_FREE(payload);

				প্রত্যাগম MOSQ_ERR_NOMEM;
			}
		}
	}

	যদি(context->protocol != mosq_p_mqtt31){
		যদি(payloadlen == 0){
			/* No subscriptions specified, protocol error. */
			প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	rc = send__suback(context, mid, payloadlen, payload);
	mosquitto_FREE(payload);

#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	যদি(context->out_packet == রিক্ত){
		rc = db__message_write_queued_out(context);
		যদি(rc){
			প্রত্যাগম rc;
		}
		rc = db__message_write_inflight_out_latest(context);
		যদি(rc){
			প্রত্যাগম rc;
		}
	}

	প্রত্যাগম rc;
}
