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

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(context->state != mosq_cs_active){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE before session is active.", context->id);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا(context->in_packet.command != (CMD_SUBSCRIBE|2)){
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	log__printf(عدم, MOSQ_LOG_DEBUG, "Received SUBSCRIBE from %s", context->id);

	إذا(context->protocol != mosq_p_mqtt31){
		إذا((context->in_packet.command&0x0F) != 0x02){
			أرجع MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	إذا(packet__read_uint16(&context->in_packet, &mid)){
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}
	إذا(mid == 0){
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	إذا(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_SUBSCRIBE, &context->in_packet, &properties);
		إذا(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			إذا(rc == MOSQ_ERR_PROTOCOL){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE packet with invalid properties.", context->id);
				أرجع MOSQ_ERR_MALFORMED_PACKET;
			}وإلا{
				أرجع rc;
			}
		}

		إذا(mosquitto_property_read_varint(properties, MQTT_PROP_SUBSCRIPTION_IDENTIFIER,
				&subscription_identifier, false)){

			/* If the identifier was force set to 0, this is an error */
			إذا(subscription_identifier == 0){
				mosquitto_property_free_all(&properties);
				أرجع MOSQ_ERR_PROTOCOL;
			}
		}

		mosquitto_property_free_all(&properties);
		/* Note - User Property not handled */
	}

	طالما(context->in_packet.pos < context->in_packet.remaining_length){
		memset(&sub, 0, حجم(sub));
		sub.identifier = subscription_identifier;
		sub.properties = properties;
		إذا(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(payload);
			أرجع MOSQ_ERR_MALFORMED_PACKET;
		}

		إذا(sub.topic_filter){
			إذا(!slen){
				log__printf(عدم, MOSQ_LOG_INFO,
						"Empty subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				أرجع MOSQ_ERR_MALFORMED_PACKET;
			}
			إذا(mosquitto_sub_topic_check(sub.topic_filter)){
				log__printf(عدم, MOSQ_LOG_INFO,
						"Invalid subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				أرجع MOSQ_ERR_MALFORMED_PACKET;
			}

			إذا(packet__read_byte(&context->in_packet, &sub.options)){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				أرجع MOSQ_ERR_MALFORMED_PACKET;
			}
			إذا(sub.options & MQTT_SUB_OPT_NO_LOCAL && !strncmp(sub.topic_filter, "$share/", strlen("$share/"))){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: $share subscription with no-local set.", context->id);
				أرجع MOSQ_ERR_PROTOCOL;
			}

			إذا(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt311){
				qos = sub.options;
				sub.options = 0;
				إذا(context->is_bridge){
					sub.options |= MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | MQTT_SUB_OPT_NO_LOCAL;
				}
			}وإلا{
				qos = sub.options & 0x03;
				sub.options &= 0xFC;

				إذا(MQTT_SUB_OPT_GET_NO_LOCAL(sub.options) && !strncmp(sub.topic_filter, "$share/", 7)){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					أرجع MOSQ_ERR_PROTOCOL;
				}
				retain_handling = MQTT_SUB_OPT_GET_RETAIN_HANDLING(sub.options);
				إذا(retain_handling == 0x30 || (sub.options & 0xC0) != 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					أرجع MOSQ_ERR_MALFORMED_PACKET;
				}
			}
			إذا(qos > 2){
				log__printf(عدم, MOSQ_LOG_INFO,
						"Invalid QoS in subscription command from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				أرجع MOSQ_ERR_MALFORMED_PACKET;
			}
			إذا(qos > context->max_qos){
				qos = context->max_qos;
			}
			sub.options |= qos;


			إذا(context->listener && context->listener->mount_point){
				len = strlen(context->listener->mount_point) + slen + 1;
				sub_mount = mosquitto_malloc(len+1);
				إذا(!sub_mount){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					أرجع MOSQ_ERR_NOMEM;
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
					إذا(context->protocol == mosq_p_mqtt5){
						qos = MQTT_RC_NOT_AUTHORIZED;
					}وإلا إذا(context->protocol == mosq_p_mqtt311){
						qos = 0x80;
					}
					اكسر;
				افتراضي:
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					أرجع rc2;
			}
			إذا(qos > 127){
				log__printf(عدم, MOSQ_LOG_DEBUG, "\t%s (denied)", sub.topic_filter);
			}وإلا{
				log__printf(عدم, MOSQ_LOG_DEBUG, "\t%s (QoS %d)", sub.topic_filter, qos);
			}

			إذا(allowed){
				rc2 = plugin__handle_subscribe(context, &sub);
				إذا(rc2){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					أرجع rc2;
				}

				rc2 = sub__add(context, &sub);
				إذا(rc2 > 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					أرجع rc2;
				}
				إذا(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
					إذا(rc2 == MOSQ_ERR_SUCCESS || rc2 == MOSQ_ERR_SUB_EXISTS){
						إذا(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							أرجع rc;
						}
					}
				}وإلا{
					إذا((retain_handling == MQTT_SUB_OPT_SEND_RETAIN_ALWAYS)
							|| (rc2 == MOSQ_ERR_SUCCESS && retain_handling == MQTT_SUB_OPT_SEND_RETAIN_NEW)){

						إذا(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							أرجع rc;
						}
					}
				}
				log__printf(عدم, MOSQ_LOG_SUBSCRIBE, "%s %d %s", context->id, qos, sub.topic_filter);

				plugin_persist__handle_subscription_add(context, &sub);
			}
			mosquitto_FREE(sub.topic_filter);

			tmp_payload = mosquitto_realloc(payload, payloadlen + 1);
			إذا(tmp_payload){
				payload = tmp_payload;
				payload[payloadlen] = qos;
				payloadlen++;
			}وإلا{
				mosquitto_FREE(payload);

				أرجع MOSQ_ERR_NOMEM;
			}
		}
	}

	إذا(context->protocol != mosq_p_mqtt31){
		إذا(payloadlen == 0){
			/* No subscriptions specified, protocol error. */
			أرجع MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	rc = send__suback(context, mid, payloadlen, payload);
	mosquitto_FREE(payload);

#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	إذا(context->out_packet == عدم){
		rc = db__message_write_queued_out(context);
		إذا(rc){
			أرجع rc;
		}
		rc = db__message_write_inflight_out_latest(context);
		إذا(rc){
			أرجع rc;
		}
	}

	أرجع rc;
}
