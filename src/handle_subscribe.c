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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "packet_mosq.h"
#અન્તર્ભાવ "property_mosq.h"


પૂર્ણાંક handle__subscribe(સંરચના mosquitto *context)
{
	પૂર્ણાંક rc = 0;
	પૂર્ણાંક rc2;
	uint16_t mid;
	uint8_t qos;
	uint8_t retain_handling = 0;
	uint8_t *payload = ખાલી, *tmp_payload;
	uint32_t payloadlen = 0;
	size_t len;
	uint16_t slen;
	અક્ષર *sub_mount;
	mosquitto_property *properties = ખાલી;
	bool allowed;
	સંરચના mosquitto_subscription sub;
	uint32_t subscription_identifier = 0;

	જો(!context){
		પાછા MOSQ_ERR_INVAL;
	}

	જો(context->state != mosq_cs_active){
		log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE before session is active.", context->id);
		પાછા MOSQ_ERR_PROTOCOL;
	}
	જો(context->in_packet.command != (CMD_SUBSCRIBE|2)){
		પાછા MOSQ_ERR_MALFORMED_PACKET;
	}

	log__printf(ખાલી, MOSQ_LOG_DEBUG, "Received SUBSCRIBE from %s", context->id);

	જો(context->protocol != mosq_p_mqtt31){
		જો((context->in_packet.command&0x0F) != 0x02){
			પાછા MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	જો(packet__read_uint16(&context->in_packet, &mid)){
		પાછા MOSQ_ERR_MALFORMED_PACKET;
	}
	જો(mid == 0){
		પાછા MOSQ_ERR_MALFORMED_PACKET;
	}

	જો(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_SUBSCRIBE, &context->in_packet, &properties);
		જો(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			જો(rc == MOSQ_ERR_PROTOCOL){
				log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE packet with invalid properties.", context->id);
				પાછા MOSQ_ERR_MALFORMED_PACKET;
			}નહીંતર{
				પાછા rc;
			}
		}

		જો(mosquitto_property_read_varint(properties, MQTT_PROP_SUBSCRIPTION_IDENTIFIER,
				&subscription_identifier, false)){

			/* If the identifier was force set to 0, this is an error */
			જો(subscription_identifier == 0){
				mosquitto_property_free_all(&properties);
				પાછા MOSQ_ERR_PROTOCOL;
			}
		}

		mosquitto_property_free_all(&properties);
		/* Note - User Property not handled */
	}

	જ્યારે(context->in_packet.pos < context->in_packet.remaining_length){
		memset(&sub, 0, કદ(sub));
		sub.identifier = subscription_identifier;
		sub.properties = properties;
		જો(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(payload);
			પાછા MOSQ_ERR_MALFORMED_PACKET;
		}

		જો(sub.topic_filter){
			જો(!slen){
				log__printf(ખાલી, MOSQ_LOG_INFO,
						"Empty subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				પાછા MOSQ_ERR_MALFORMED_PACKET;
			}
			જો(mosquitto_sub_topic_check(sub.topic_filter)){
				log__printf(ખાલી, MOSQ_LOG_INFO,
						"Invalid subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				પાછા MOSQ_ERR_MALFORMED_PACKET;
			}

			જો(packet__read_byte(&context->in_packet, &sub.options)){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				પાછા MOSQ_ERR_MALFORMED_PACKET;
			}
			જો(sub.options & MQTT_SUB_OPT_NO_LOCAL && !strncmp(sub.topic_filter, "$share/", strlen("$share/"))){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: $share subscription with no-local set.", context->id);
				પાછા MOSQ_ERR_PROTOCOL;
			}

			જો(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt311){
				qos = sub.options;
				sub.options = 0;
				જો(context->is_bridge){
					sub.options |= MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | MQTT_SUB_OPT_NO_LOCAL;
				}
			}નહીંતર{
				qos = sub.options & 0x03;
				sub.options &= 0xFC;

				જો(MQTT_SUB_OPT_GET_NO_LOCAL(sub.options) && !strncmp(sub.topic_filter, "$share/", 7)){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					પાછા MOSQ_ERR_PROTOCOL;
				}
				retain_handling = MQTT_SUB_OPT_GET_RETAIN_HANDLING(sub.options);
				જો(retain_handling == 0x30 || (sub.options & 0xC0) != 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					પાછા MOSQ_ERR_MALFORMED_PACKET;
				}
			}
			જો(qos > 2){
				log__printf(ખાલી, MOSQ_LOG_INFO,
						"Invalid QoS in subscription command from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				પાછા MOSQ_ERR_MALFORMED_PACKET;
			}
			જો(qos > context->max_qos){
				qos = context->max_qos;
			}
			sub.options |= qos;


			જો(context->listener && context->listener->mount_point){
				len = strlen(context->listener->mount_point) + slen + 1;
				sub_mount = mosquitto_malloc(len+1);
				જો(!sub_mount){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					પાછા MOSQ_ERR_NOMEM;
				}
				snprintf(sub_mount, len, "%s%s", context->listener->mount_point, sub.topic_filter);
				sub_mount[len] = '\0';

				mosquitto_FREE(sub.topic_filter);
				sub.topic_filter = sub_mount;

			}

			allowed = true;
			rc2 = mosquitto_acl_check(context, sub.topic_filter, 0, ખાલી, qos, false, properties, MOSQ_ACL_SUBSCRIBE);
			પસંદગી(rc2){
				સ્થિતિ MOSQ_ERR_SUCCESS:
					તોડો;
				સ્થિતિ MOSQ_ERR_ACL_DENIED:
					allowed = false;
					જો(context->protocol == mosq_p_mqtt5){
						qos = MQTT_RC_NOT_AUTHORIZED;
					}નહીંતર જો(context->protocol == mosq_p_mqtt311){
						qos = 0x80;
					}
					તોડો;
				મૂળભૂત:
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					પાછા rc2;
			}
			જો(qos > 127){
				log__printf(ખાલી, MOSQ_LOG_DEBUG, "\t%s (denied)", sub.topic_filter);
			}નહીંતર{
				log__printf(ખાલી, MOSQ_LOG_DEBUG, "\t%s (QoS %d)", sub.topic_filter, qos);
			}

			જો(allowed){
				rc2 = plugin__handle_subscribe(context, &sub);
				જો(rc2){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					પાછા rc2;
				}

				rc2 = sub__add(context, &sub);
				જો(rc2 > 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					પાછા rc2;
				}
				જો(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
					જો(rc2 == MOSQ_ERR_SUCCESS || rc2 == MOSQ_ERR_SUB_EXISTS){
						જો(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							પાછા rc;
						}
					}
				}નહીંતર{
					જો((retain_handling == MQTT_SUB_OPT_SEND_RETAIN_ALWAYS)
							|| (rc2 == MOSQ_ERR_SUCCESS && retain_handling == MQTT_SUB_OPT_SEND_RETAIN_NEW)){

						જો(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							પાછા rc;
						}
					}
				}
				log__printf(ખાલી, MOSQ_LOG_SUBSCRIBE, "%s %d %s", context->id, qos, sub.topic_filter);

				plugin_persist__handle_subscription_add(context, &sub);
			}
			mosquitto_FREE(sub.topic_filter);

			tmp_payload = mosquitto_realloc(payload, payloadlen + 1);
			જો(tmp_payload){
				payload = tmp_payload;
				payload[payloadlen] = qos;
				payloadlen++;
			}નહીંતર{
				mosquitto_FREE(payload);

				પાછા MOSQ_ERR_NOMEM;
			}
		}
	}

	જો(context->protocol != mosq_p_mqtt31){
		જો(payloadlen == 0){
			/* No subscriptions specified, protocol error. */
			પાછા MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	rc = send__suback(context, mid, payloadlen, payload);
	mosquitto_FREE(payload);

#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	જો(context->out_packet == ખાલી){
		rc = db__message_write_queued_out(context);
		જો(rc){
			પાછા rc;
		}
		rc = db__message_write_inflight_out_latest(context);
		જો(rc){
			પાછા rc;
		}
	}

	પાછા rc;
}
