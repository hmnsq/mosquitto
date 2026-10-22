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

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "packet_mosq.h"
#ضمن "property_mosq.h"


صحيح handle__subscribe(بنية mosquitto *context)
{
	صحيح rc = 0;
	صحيح rc2;
	uint16_t mid;
	uint8_t qos;
	uint8_t retain_handling = 0;
	uint8_t *payload = عدم, *tmp_payload;
	uint32_t payloadlen = 0;
	size_t len;
	uint16_t slen;
	حرف *sub_mount;
	mosquitto_property *properties = عدم;
	bool allowed;
	بنية mosquitto_subscription sub;
	uint32_t subscription_identifier = 0;

	אם(!context){
		החזר MOSQ_ERR_INVAL;
	}

	אם(context->state != mosq_cs_active){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE before session is active.", context->id);
		החזר MOSQ_ERR_PROTOCOL;
	}
	אם(context->in_packet.command != (CMD_SUBSCRIBE|2)){
		החזר MOSQ_ERR_MALFORMED_PACKET;
	}

	log__printf(عدم, MOSQ_LOG_DEBUG, "Received SUBSCRIBE from %s", context->id);

	אם(context->protocol != mosq_p_mqtt31){
		אם((context->in_packet.command&0x0F) != 0x02){
			החזר MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	אם(packet__read_uint16(&context->in_packet, &mid)){
		החזר MOSQ_ERR_MALFORMED_PACKET;
	}
	אם(mid == 0){
		החזר MOSQ_ERR_MALFORMED_PACKET;
	}

	אם(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_SUBSCRIBE, &context->in_packet, &properties);
		אם(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			אם(rc == MOSQ_ERR_PROTOCOL){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE packet with invalid properties.", context->id);
				החזר MOSQ_ERR_MALFORMED_PACKET;
			}אחרת{
				החזר rc;
			}
		}

		אם(mosquitto_property_read_varint(properties, MQTT_PROP_SUBSCRIPTION_IDENTIFIER,
				&subscription_identifier, false)){

			/* If the identifier was force set to 0, this is an error */
			אם(subscription_identifier == 0){
				mosquitto_property_free_all(&properties);
				החזר MOSQ_ERR_PROTOCOL;
			}
		}

		mosquitto_property_free_all(&properties);
		/* Note - User Property not handled */
	}

	כלעוד(context->in_packet.pos < context->in_packet.remaining_length){
		memset(&sub, 0, حجم(sub));
		sub.identifier = subscription_identifier;
		sub.properties = properties;
		אם(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(payload);
			החזר MOSQ_ERR_MALFORMED_PACKET;
		}

		אם(sub.topic_filter){
			אם(!slen){
				log__printf(عدم, MOSQ_LOG_INFO,
						"Empty subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				החזר MOSQ_ERR_MALFORMED_PACKET;
			}
			אם(mosquitto_sub_topic_check(sub.topic_filter)){
				log__printf(عدم, MOSQ_LOG_INFO,
						"Invalid subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				החזר MOSQ_ERR_MALFORMED_PACKET;
			}

			אם(packet__read_byte(&context->in_packet, &sub.options)){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				החזר MOSQ_ERR_MALFORMED_PACKET;
			}
			אם(sub.options & MQTT_SUB_OPT_NO_LOCAL && !strncmp(sub.topic_filter, "$share/", strlen("$share/"))){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: $share subscription with no-local set.", context->id);
				החזר MOSQ_ERR_PROTOCOL;
			}

			אם(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt311){
				qos = sub.options;
				sub.options = 0;
				אם(context->is_bridge){
					sub.options |= MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | MQTT_SUB_OPT_NO_LOCAL;
				}
			}אחרת{
				qos = sub.options & 0x03;
				sub.options &= 0xFC;

				אם(MQTT_SUB_OPT_GET_NO_LOCAL(sub.options) && !strncmp(sub.topic_filter, "$share/", 7)){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					החזר MOSQ_ERR_PROTOCOL;
				}
				retain_handling = MQTT_SUB_OPT_GET_RETAIN_HANDLING(sub.options);
				אם(retain_handling == 0x30 || (sub.options & 0xC0) != 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					החזר MOSQ_ERR_MALFORMED_PACKET;
				}
			}
			אם(qos > 2){
				log__printf(عدم, MOSQ_LOG_INFO,
						"Invalid QoS in subscription command from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				החזר MOSQ_ERR_MALFORMED_PACKET;
			}
			אם(qos > context->max_qos){
				qos = context->max_qos;
			}
			sub.options |= qos;


			אם(context->listener && context->listener->mount_point){
				len = strlen(context->listener->mount_point) + slen + 1;
				sub_mount = mosquitto_malloc(len+1);
				אם(!sub_mount){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					החזר MOSQ_ERR_NOMEM;
				}
				snprintf(sub_mount, len, "%s%s", context->listener->mount_point, sub.topic_filter);
				sub_mount[len] = '\0';

				mosquitto_FREE(sub.topic_filter);
				sub.topic_filter = sub_mount;

			}

			allowed = true;
			rc2 = mosquitto_acl_check(context, sub.topic_filter, 0, عدم, qos, false, properties, MOSQ_ACL_SUBSCRIBE);
			بدل(rc2){
				حالة MOSQ_ERR_SUCCESS:
					اكسر;
				حالة MOSQ_ERR_ACL_DENIED:
					allowed = false;
					אם(context->protocol == mosq_p_mqtt5){
						qos = MQTT_RC_NOT_AUTHORIZED;
					}אחרת אם(context->protocol == mosq_p_mqtt311){
						qos = 0x80;
					}
					اكسر;
				افتراضي:
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					החזר rc2;
			}
			אם(qos > 127){
				log__printf(عدم, MOSQ_LOG_DEBUG, "\t%s (denied)", sub.topic_filter);
			}אחרת{
				log__printf(عدم, MOSQ_LOG_DEBUG, "\t%s (QoS %d)", sub.topic_filter, qos);
			}

			אם(allowed){
				rc2 = plugin__handle_subscribe(context, &sub);
				אם(rc2){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					החזר rc2;
				}

				rc2 = sub__add(context, &sub);
				אם(rc2 > 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					החזר rc2;
				}
				אם(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
					אם(rc2 == MOSQ_ERR_SUCCESS || rc2 == MOSQ_ERR_SUB_EXISTS){
						אם(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							החזר rc;
						}
					}
				}אחרת{
					אם((retain_handling == MQTT_SUB_OPT_SEND_RETAIN_ALWAYS)
							|| (rc2 == MOSQ_ERR_SUCCESS && retain_handling == MQTT_SUB_OPT_SEND_RETAIN_NEW)){

						אם(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							החזר rc;
						}
					}
				}
				log__printf(عدم, MOSQ_LOG_SUBSCRIBE, "%s %d %s", context->id, qos, sub.topic_filter);

				plugin_persist__handle_subscription_add(context, &sub);
			}
			mosquitto_FREE(sub.topic_filter);

			tmp_payload = mosquitto_realloc(payload, payloadlen + 1);
			אם(tmp_payload){
				payload = tmp_payload;
				payload[payloadlen] = qos;
				payloadlen++;
			}אחרת{
				mosquitto_FREE(payload);

				החזר MOSQ_ERR_NOMEM;
			}
		}
	}

	אם(context->protocol != mosq_p_mqtt31){
		אם(payloadlen == 0){
			/* No subscriptions specified, protocol error. */
			החזר MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	rc = send__suback(context, mid, payloadlen, payload);
	mosquitto_FREE(payload);

#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	אם(context->out_packet == عدم){
		rc = db__message_write_queued_out(context);
		אם(rc){
			החזר rc;
		}
		rc = db__message_write_inflight_out_latest(context);
		אם(rc){
			החזר rc;
		}
	}

	החזר rc;
}
