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

#شامل "config.h"

#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "packet_mosq.h"
#شامل "property_mosq.h"


صحیح handle__subscribe(ساخت mosquitto *context)
{
	صحیح rc = 0;
	صحیح rc2;
	uint16_t mid;
	uint8_t qos;
	uint8_t retain_handling = 0;
	uint8_t *payload = تہی, *tmp_payload;
	uint32_t payloadlen = 0;
	size_t len;
	uint16_t slen;
	حرف *sub_mount;
	mosquitto_property *properties = تہی;
	bool allowed;
	ساخت mosquitto_subscription sub;
	uint32_t subscription_identifier = 0;

	اگر(!context){
		واپس MOSQ_ERR_INVAL;
	}

	اگر(context->state != mosq_cs_فعال){
		log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE before session is active.", context->id);
		واپس MOSQ_ERR_PROTOCOL;
	}
	اگر(context->اندر_packet.command != (CMD_SUBSCRIBE|2)){
		واپس MOSQ_ERR_MALFORMED_PACKET;
	}

	log__printf(تہی, MOSQ_LOG_DEBUG, "Received SUBSCRIBE from %s", context->id);

	اگر(context->protocol != mosq_p_mqtt31){
		اگر((context->اندر_packet.command&0x0F) != 0x02){
			واپس MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	اگر(packet__read_uint16(&context->اندر_packet, &mid)){
		واپس MOSQ_ERR_MALFORMED_PACKET;
	}
	اگر(mid == 0){
		واپس MOSQ_ERR_MALFORMED_PACKET;
	}

	اگر(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_SUBSCRIBE, &context->اندر_packet, &properties);
		اگر(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			اگر(rc == MOSQ_ERR_PROTOCOL){
				log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE packet with invalid properties.", context->id);
				واپس MOSQ_ERR_MALFORMED_PACKET;
			}ورنہ{
				واپس rc;
			}
		}

		اگر(mosquitto_property_read_varint(properties, MQTT_PROP_SUBSCRIPTION_IDENTIFIER,
				&subscription_identifier, false)){

			/* If the identifier was force set to 0, this is an error */
			اگر(subscription_identifier == 0){
				mosquitto_property_free_all(&properties);
				واپس MOSQ_ERR_PROTOCOL;
			}
		}

		mosquitto_property_free_all(&properties);
		/* Note - User Property not handled */
	}

	جب(context->اندر_packet.pos < context->اندر_packet.بقایا_لمبائی){
		memset(&sub, 0, حجم(sub));
		sub.identifier = subscription_identifier;
		sub.properties = properties;
		اگر(packet__read_string(&context->اندر_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(payload);
			واپس MOSQ_ERR_MALFORMED_PACKET;
		}

		اگر(sub.topic_filter){
			اگر(!slen){
				log__printf(تہی, MOSQ_LOG_INFO,
						"Empty subscription string from %s, disconnecting.",
						context->پتہ);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				واپس MOSQ_ERR_MALFORMED_PACKET;
			}
			اگر(mosquitto_sub_topic_check(sub.topic_filter)){
				log__printf(تہی, MOSQ_LOG_INFO,
						"Invalid subscription string from %s, disconnecting.",
						context->پتہ);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				واپس MOSQ_ERR_MALFORMED_PACKET;
			}

			اگر(packet__read_byte(&context->اندر_packet, &sub.options)){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				واپس MOSQ_ERR_MALFORMED_PACKET;
			}
			اگر(sub.options & MQTT_SUB_OPT_NO_LOCAL && !لڑی_موازنہ(sub.topic_filter, "$share/", لڑی_لمبائی("$share/"))){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: $share subscription with no-local set.", context->id);
				واپس MOSQ_ERR_PROTOCOL;
			}

			اگر(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt311){
				qos = sub.options;
				sub.options = 0;
				اگر(context->is_bridge){
					sub.options |= MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | MQTT_SUB_OPT_NO_LOCAL;
				}
			}ورنہ{
				qos = sub.options & 0x03;
				sub.options &= 0xFC;

				اگر(MQTT_SUB_OPT_GET_NO_LOCAL(sub.options) && !لڑی_موازنہ(sub.topic_filter, "$share/", 7)){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					واپس MOSQ_ERR_PROTOCOL;
				}
				retain_handling = MQTT_SUB_OPT_GET_RETAIN_HANDLING(sub.options);
				اگر(retain_handling == 0x30 || (sub.options & 0xC0) != 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					واپس MOSQ_ERR_MALFORMED_PACKET;
				}
			}
			اگر(qos > 2){
				log__printf(تہی, MOSQ_LOG_INFO,
						"Invalid QoS in subscription command from %s, disconnecting.",
						context->پتہ);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				واپس MOSQ_ERR_MALFORMED_PACKET;
			}
			اگر(qos > context->max_qos){
				qos = context->max_qos;
			}
			sub.options |= qos;


			اگر(context->listener && context->listener->mount_نقطہ){
				len = لڑی_لمبائی(context->listener->mount_نقطہ) + slen + 1;
				sub_mount = mosquitto_malloc(len+1);
				اگر(!sub_mount){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					واپس MOSQ_ERR_NOMEM;
				}
				snprintf(sub_mount, len, "%s%s", context->listener->mount_نقطہ, sub.topic_filter);
				sub_mount[len] = '\0';

				mosquitto_FREE(sub.topic_filter);
				sub.topic_filter = sub_mount;

			}

			allowed = true;
			rc2 = mosquitto_acl_check(context, sub.topic_filter, 0, تہی, qos, false, properties, MOSQ_ACL_SUBSCRIBE);
			انتخاب(rc2){
				حالت MOSQ_ERR_SUCCESS:
					توڑو;
				حالت MOSQ_ERR_ACL_DENIED:
					allowed = false;
					اگر(context->protocol == mosq_p_mqtt5){
						qos = MQTT_RC_NOT_AUTHORIZED;
					}ورنہ اگر(context->protocol == mosq_p_mqtt311){
						qos = 0x80;
					}
					توڑو;
				طےشدہ:
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					واپس rc2;
			}
			اگر(qos > 127){
				log__printf(تہی, MOSQ_LOG_DEBUG, "\t%s (denied)", sub.topic_filter);
			}ورنہ{
				log__printf(تہی, MOSQ_LOG_DEBUG, "\t%s (QoS %d)", sub.topic_filter, qos);
			}

			اگر(allowed){
				rc2 = plugin__handle_subscribe(context, &sub);
				اگر(rc2){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					واپس rc2;
				}

				rc2 = sub_جوڑو(context, &sub);
				اگر(rc2 > 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					واپس rc2;
				}
				اگر(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
					اگر(rc2 == MOSQ_ERR_SUCCESS || rc2 == MOSQ_ERR_SUB_EXISTS){
						اگر(retain_قطار(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							واپس rc;
						}
					}
				}ورنہ{
					اگر((retain_handling == MQTT_SUB_OPT_SEND_RETAIN_ALWAYS)
							|| (rc2 == MOSQ_ERR_SUCCESS && retain_handling == MQTT_SUB_OPT_SEND_RETAIN_NEW)){

						اگر(retain_قطار(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							واپس rc;
						}
					}
				}
				log__printf(تہی, MOSQ_LOG_SUBSCRIBE, "%s %d %s", context->id, qos, sub.topic_filter);

				plugin_persist_handle_subscription_جوڑو(context, &sub);
			}
			mosquitto_FREE(sub.topic_filter);

			tmp_payload = mosquitto_realloc(payload, payloadlen + 1);
			اگر(tmp_payload){
				payload = tmp_payload;
				payload[payloadlen] = qos;
				payloadlen++;
			}ورنہ{
				mosquitto_FREE(payload);

				واپس MOSQ_ERR_NOMEM;
			}
		}
	}

	اگر(context->protocol != mosq_p_mqtt31){
		اگر(payloadlen == 0){
			/* No subscriptions specified, protocol error. */
			واپس MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	rc = send__suback(context, mid, payloadlen, payload);
	mosquitto_FREE(payload);

#اگر_تعریف_شدہ WITH_PERSISTENCE
	db.persistence_changes++;
#اگر_ختم

	اگر(context->برآمد_packet == تہی){
		rc = db_پیغام_write_queued_برآمد(context);
		اگر(rc){
			واپس rc;
		}
		rc = db_پیغام_write_inflight_برآمد_latest(context);
		اگر(rc){
			واپس rc;
		}
	}

	واپس rc;
}
