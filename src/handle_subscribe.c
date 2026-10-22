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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "packet_mosq.h"
#ଅନ୍ତର୍ଭାଵ "property_mosq.h"


ପୂର୍ଣ୍ଣସଂଖ୍ୟା handle__subscribe(ସଂରଚନା mosquitto *context)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc2;
	uint16_t mid;
	uint8_t qos;
	uint8_t retain_handling = 0;
	uint8_t *payload = ଖାଲି, *tmp_payload;
	uint32_t payloadlen = 0;
	size_t len;
	uint16_t slen;
	ଅକ୍ଷର *sub_mount;
	mosquitto_property *properties = ଖାଲି;
	bool allowed;
	ସଂରଚନା mosquitto_subscription sub;
	uint32_t subscription_identifier = 0;

	ଯଦି(!context){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	ଯଦି(context->state != mosq_cs_active){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE before session is active.", context->id);
		ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
	}
	ଯଦି(context->in_packet.command != (CMD_SUBSCRIBE|2)){
		ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
	}

	log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "Received SUBSCRIBE from %s", context->id);

	ଯଦି(context->protocol != mosq_p_mqtt31){
		ଯଦି((context->in_packet.command&0x0F) != 0x02){
			ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	ଯଦି(packet__read_uint16(&context->in_packet, &mid)){
		ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
	}
	ଯଦି(mid == 0){
		ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
	}

	ଯଦି(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_SUBSCRIBE, &context->in_packet, &properties);
		ଯଦି(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			ଯଦି(rc == MOSQ_ERR_PROTOCOL){
				log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE packet with invalid properties.", context->id);
				ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
			}ଅନ୍ୟଥା{
				ଫେରନ୍ତୁ rc;
			}
		}

		ଯଦି(mosquitto_property_read_varint(properties, MQTT_PROP_SUBSCRIPTION_IDENTIFIER,
				&subscription_identifier, false)){

			/* If the identifier was force set to 0, this is an error */
			ଯଦି(subscription_identifier == 0){
				mosquitto_property_free_all(&properties);
				ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
			}
		}

		mosquitto_property_free_all(&properties);
		/* Note - User Property not handled */
	}

	ଯେତେବେଳେ(context->in_packet.pos < context->in_packet.remaining_length){
		memset(&sub, 0, ଆକାର(sub));
		sub.identifier = subscription_identifier;
		sub.properties = properties;
		ଯଦି(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(payload);
			ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
		}

		ଯଦି(sub.topic_filter){
			ଯଦି(!slen){
				log__printf(ଖାଲି, MOSQ_LOG_INFO,
						"Empty subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
			}
			ଯଦି(mosquitto_sub_topic_check(sub.topic_filter)){
				log__printf(ଖାଲି, MOSQ_LOG_INFO,
						"Invalid subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
			}

			ଯଦି(packet__read_byte(&context->in_packet, &sub.options)){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
			}
			ଯଦି(sub.options & MQTT_SUB_OPT_NO_LOCAL && !strncmp(sub.topic_filter, "$share/", strlen("$share/"))){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: $share subscription with no-local set.", context->id);
				ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
			}

			ଯଦି(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt311){
				qos = sub.options;
				sub.options = 0;
				ଯଦି(context->is_bridge){
					sub.options |= MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | MQTT_SUB_OPT_NO_LOCAL;
				}
			}ଅନ୍ୟଥା{
				qos = sub.options & 0x03;
				sub.options &= 0xFC;

				ଯଦି(MQTT_SUB_OPT_GET_NO_LOCAL(sub.options) && !strncmp(sub.topic_filter, "$share/", 7)){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
				}
				retain_handling = MQTT_SUB_OPT_GET_RETAIN_HANDLING(sub.options);
				ଯଦି(retain_handling == 0x30 || (sub.options & 0xC0) != 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
				}
			}
			ଯଦି(qos > 2){
				log__printf(ଖାଲି, MOSQ_LOG_INFO,
						"Invalid QoS in subscription command from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
			}
			ଯଦି(qos > context->max_qos){
				qos = context->max_qos;
			}
			sub.options |= qos;


			ଯଦି(context->listener && context->listener->mount_point){
				len = strlen(context->listener->mount_point) + slen + 1;
				sub_mount = mosquitto_malloc(len+1);
				ଯଦି(!sub_mount){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
				}
				snprintf(sub_mount, len, "%s%s", context->listener->mount_point, sub.topic_filter);
				sub_mount[len] = '\0';

				mosquitto_FREE(sub.topic_filter);
				sub.topic_filter = sub_mount;

			}

			allowed = true;
			rc2 = mosquitto_acl_check(context, sub.topic_filter, 0, ଖାଲି, qos, false, properties, MOSQ_ACL_SUBSCRIBE);
			ବାଛ(rc2){
				ସ୍ଥିତି MOSQ_ERR_SUCCESS:
					ଭାଙ୍ଗ;
				ସ୍ଥିତି MOSQ_ERR_ACL_DENIED:
					allowed = false;
					ଯଦି(context->protocol == mosq_p_mqtt5){
						qos = MQTT_RC_NOT_AUTHORIZED;
					}ଅନ୍ୟଥା ଯଦି(context->protocol == mosq_p_mqtt311){
						qos = 0x80;
					}
					ଭାଙ୍ଗ;
				ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ଫେରନ୍ତୁ rc2;
			}
			ଯଦି(qos > 127){
				log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "\t%s (denied)", sub.topic_filter);
			}ଅନ୍ୟଥା{
				log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "\t%s (QoS %d)", sub.topic_filter, qos);
			}

			ଯଦି(allowed){
				rc2 = plugin__handle_subscribe(context, &sub);
				ଯଦି(rc2){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ଫେରନ୍ତୁ rc2;
				}

				rc2 = sub__add(context, &sub);
				ଯଦି(rc2 > 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					ଫେରନ୍ତୁ rc2;
				}
				ଯଦି(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
					ଯଦି(rc2 == MOSQ_ERR_SUCCESS || rc2 == MOSQ_ERR_SUB_EXISTS){
						ଯଦି(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							ଫେରନ୍ତୁ rc;
						}
					}
				}ଅନ୍ୟଥା{
					ଯଦି((retain_handling == MQTT_SUB_OPT_SEND_RETAIN_ALWAYS)
							|| (rc2 == MOSQ_ERR_SUCCESS && retain_handling == MQTT_SUB_OPT_SEND_RETAIN_NEW)){

						ଯଦି(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							ଫେରନ୍ତୁ rc;
						}
					}
				}
				log__printf(ଖାଲି, MOSQ_LOG_SUBSCRIBE, "%s %d %s", context->id, qos, sub.topic_filter);

				plugin_persist__handle_subscription_add(context, &sub);
			}
			mosquitto_FREE(sub.topic_filter);

			tmp_payload = mosquitto_realloc(payload, payloadlen + 1);
			ଯଦି(tmp_payload){
				payload = tmp_payload;
				payload[payloadlen] = qos;
				payloadlen++;
			}ଅନ୍ୟଥା{
				mosquitto_FREE(payload);

				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
		}
	}

	ଯଦି(context->protocol != mosq_p_mqtt31){
		ଯଦି(payloadlen == 0){
			/* No subscriptions specified, protocol error. */
			ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	rc = send__suback(context, mid, payloadlen, payload);
	mosquitto_FREE(payload);

#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	ଯଦି(context->out_packet == ଖାଲି){
		rc = db__message_write_queued_out(context);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
		rc = db__message_write_inflight_out_latest(context);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
	}

	ଫେରନ୍ତୁ rc;
}
