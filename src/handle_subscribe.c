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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "packet_mosq.h"
#അന്തര്ഭാവ "property_mosq.h"


പൂർണ്ണസംഖ്യ handle__subscribe(ഘടന mosquitto *context)
{
	പൂർണ്ണസംഖ്യ rc = 0;
	പൂർണ്ണസംഖ്യ rc2;
	uint16_t mid;
	uint8_t qos;
	uint8_t retain_handling = 0;
	uint8_t *payload = ശൂന്യ, *tmp_payload;
	uint32_t payloadlen = 0;
	size_t len;
	uint16_t slen;
	അക്ഷരം *sub_mount;
	mosquitto_property *properties = ശൂന്യ;
	bool allowed;
	ഘടന mosquitto_subscription sub;
	uint32_t subscription_identifier = 0;

	എങ്കിൽ(!context){
		തിരികെ MOSQ_ERR_INVAL;
	}

	എങ്കിൽ(context->state != mosq_cs_active){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE before session is active.", context->id);
		തിരികെ MOSQ_ERR_PROTOCOL;
	}
	എങ്കിൽ(context->in_packet.command != (CMD_SUBSCRIBE|2)){
		തിരികെ MOSQ_ERR_MALFORMED_PACKET;
	}

	log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "Received SUBSCRIBE from %s", context->id);

	എങ്കിൽ(context->protocol != mosq_p_mqtt31){
		എങ്കിൽ((context->in_packet.command&0x0F) != 0x02){
			തിരികെ MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	എങ്കിൽ(packet__read_uint16(&context->in_packet, &mid)){
		തിരികെ MOSQ_ERR_MALFORMED_PACKET;
	}
	എങ്കിൽ(mid == 0){
		തിരികെ MOSQ_ERR_MALFORMED_PACKET;
	}

	എങ്കിൽ(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_SUBSCRIBE, &context->in_packet, &properties);
		എങ്കിൽ(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			എങ്കിൽ(rc == MOSQ_ERR_PROTOCOL){
				log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE packet with invalid properties.", context->id);
				തിരികെ MOSQ_ERR_MALFORMED_PACKET;
			}അല്ലെങ്കിൽ{
				തിരികെ rc;
			}
		}

		എങ്കിൽ(mosquitto_property_read_varint(properties, MQTT_PROP_SUBSCRIPTION_IDENTIFIER,
				&subscription_identifier, false)){

			/* If the identifier was force set to 0, this is an error */
			എങ്കിൽ(subscription_identifier == 0){
				mosquitto_property_free_all(&properties);
				തിരികെ MOSQ_ERR_PROTOCOL;
			}
		}

		mosquitto_property_free_all(&properties);
		/* Note - User Property not handled */
	}

	ആകുമ്പോൾ(context->in_packet.pos < context->in_packet.remaining_length){
		memset(&sub, 0, വലുപ്പം(sub));
		sub.identifier = subscription_identifier;
		sub.properties = properties;
		എങ്കിൽ(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(payload);
			തിരികെ MOSQ_ERR_MALFORMED_PACKET;
		}

		എങ്കിൽ(sub.topic_filter){
			എങ്കിൽ(!slen){
				log__printf(ശൂന്യ, MOSQ_LOG_INFO,
						"Empty subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				തിരികെ MOSQ_ERR_MALFORMED_PACKET;
			}
			എങ്കിൽ(mosquitto_sub_topic_check(sub.topic_filter)){
				log__printf(ശൂന്യ, MOSQ_LOG_INFO,
						"Invalid subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				തിരികെ MOSQ_ERR_MALFORMED_PACKET;
			}

			എങ്കിൽ(packet__read_byte(&context->in_packet, &sub.options)){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				തിരികെ MOSQ_ERR_MALFORMED_PACKET;
			}
			എങ്കിൽ(sub.options & MQTT_SUB_OPT_NO_LOCAL && !strncmp(sub.topic_filter, "$share/", strlen("$share/"))){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: $share subscription with no-local set.", context->id);
				തിരികെ MOSQ_ERR_PROTOCOL;
			}

			എങ്കിൽ(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt311){
				qos = sub.options;
				sub.options = 0;
				എങ്കിൽ(context->is_bridge){
					sub.options |= MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | MQTT_SUB_OPT_NO_LOCAL;
				}
			}അല്ലെങ്കിൽ{
				qos = sub.options & 0x03;
				sub.options &= 0xFC;

				എങ്കിൽ(MQTT_SUB_OPT_GET_NO_LOCAL(sub.options) && !strncmp(sub.topic_filter, "$share/", 7)){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					തിരികെ MOSQ_ERR_PROTOCOL;
				}
				retain_handling = MQTT_SUB_OPT_GET_RETAIN_HANDLING(sub.options);
				എങ്കിൽ(retain_handling == 0x30 || (sub.options & 0xC0) != 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					തിരികെ MOSQ_ERR_MALFORMED_PACKET;
				}
			}
			എങ്കിൽ(qos > 2){
				log__printf(ശൂന്യ, MOSQ_LOG_INFO,
						"Invalid QoS in subscription command from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				തിരികെ MOSQ_ERR_MALFORMED_PACKET;
			}
			എങ്കിൽ(qos > context->max_qos){
				qos = context->max_qos;
			}
			sub.options |= qos;


			എങ്കിൽ(context->listener && context->listener->mount_point){
				len = strlen(context->listener->mount_point) + slen + 1;
				sub_mount = mosquitto_malloc(len+1);
				എങ്കിൽ(!sub_mount){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					തിരികെ MOSQ_ERR_NOMEM;
				}
				snprintf(sub_mount, len, "%s%s", context->listener->mount_point, sub.topic_filter);
				sub_mount[len] = '\0';

				mosquitto_FREE(sub.topic_filter);
				sub.topic_filter = sub_mount;

			}

			allowed = true;
			rc2 = mosquitto_acl_check(context, sub.topic_filter, 0, ശൂന്യ, qos, false, properties, MOSQ_ACL_SUBSCRIBE);
			തിരഞ്ഞെടുപ്പ്(rc2){
				അവസ്ഥ MOSQ_ERR_SUCCESS:
					നിർത്തുക;
				അവസ്ഥ MOSQ_ERR_ACL_DENIED:
					allowed = false;
					എങ്കിൽ(context->protocol == mosq_p_mqtt5){
						qos = MQTT_RC_NOT_AUTHORIZED;
					}അല്ലെങ്കിൽ എങ്കിൽ(context->protocol == mosq_p_mqtt311){
						qos = 0x80;
					}
					നിർത്തുക;
				സ്ഥിരസ്ഥിതി:
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					തിരികെ rc2;
			}
			എങ്കിൽ(qos > 127){
				log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "\t%s (denied)", sub.topic_filter);
			}അല്ലെങ്കിൽ{
				log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "\t%s (QoS %d)", sub.topic_filter, qos);
			}

			എങ്കിൽ(allowed){
				rc2 = plugin__handle_subscribe(context, &sub);
				എങ്കിൽ(rc2){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					തിരികെ rc2;
				}

				rc2 = sub__add(context, &sub);
				എങ്കിൽ(rc2 > 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					തിരികെ rc2;
				}
				എങ്കിൽ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
					എങ്കിൽ(rc2 == MOSQ_ERR_SUCCESS || rc2 == MOSQ_ERR_SUB_EXISTS){
						എങ്കിൽ(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							തിരികെ rc;
						}
					}
				}അല്ലെങ്കിൽ{
					എങ്കിൽ((retain_handling == MQTT_SUB_OPT_SEND_RETAIN_ALWAYS)
							|| (rc2 == MOSQ_ERR_SUCCESS && retain_handling == MQTT_SUB_OPT_SEND_RETAIN_NEW)){

						എങ്കിൽ(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							തിരികെ rc;
						}
					}
				}
				log__printf(ശൂന്യ, MOSQ_LOG_SUBSCRIBE, "%s %d %s", context->id, qos, sub.topic_filter);

				plugin_persist__handle_subscription_add(context, &sub);
			}
			mosquitto_FREE(sub.topic_filter);

			tmp_payload = mosquitto_realloc(payload, payloadlen + 1);
			എങ്കിൽ(tmp_payload){
				payload = tmp_payload;
				payload[payloadlen] = qos;
				payloadlen++;
			}അല്ലെങ്കിൽ{
				mosquitto_FREE(payload);

				തിരികെ MOSQ_ERR_NOMEM;
			}
		}
	}

	എങ്കിൽ(context->protocol != mosq_p_mqtt31){
		എങ്കിൽ(payloadlen == 0){
			/* No subscriptions specified, protocol error. */
			തിരികെ MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	rc = send__suback(context, mid, payloadlen, payload);
	mosquitto_FREE(payload);

#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	എങ്കിൽ(context->out_packet == ശൂന്യ){
		rc = db__message_write_queued_out(context);
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
		rc = db__message_write_inflight_out_latest(context);
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
	}

	തിരികെ rc;
}
