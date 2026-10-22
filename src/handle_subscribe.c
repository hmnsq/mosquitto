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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "packet_mosq.h"
#அந்தர்பாவ "property_mosq.h"


முழு handle__subscribe(கட்டமைப்பு mosquitto *context)
{
	முழு rc = 0;
	முழு rc2;
	uint16_t mid;
	uint8_t qos;
	uint8_t retain_handling = 0;
	uint8_t *payload = வெற்று, *tmp_payload;
	uint32_t payloadlen = 0;
	size_t len;
	uint16_t slen;
	எழுத்து *sub_mount;
	mosquitto_property *properties = வெற்று;
	bool allowed;
	கட்டமைப்பு mosquitto_subscription sub;
	uint32_t subscription_identifier = 0;

	எனில்(!context){
		திரும்பு MOSQ_ERR_INVAL;
	}

	எனில்(context->state != mosq_cs_active){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE before session is active.", context->id);
		திரும்பு MOSQ_ERR_PROTOCOL;
	}
	எனில்(context->in_packet.command != (CMD_SUBSCRIBE|2)){
		திரும்பு MOSQ_ERR_MALFORMED_PACKET;
	}

	log__printf(வெற்று, MOSQ_LOG_DEBUG, "Received SUBSCRIBE from %s", context->id);

	எனில்(context->protocol != mosq_p_mqtt31){
		எனில்((context->in_packet.command&0x0F) != 0x02){
			திரும்பு MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	எனில்(packet__read_uint16(&context->in_packet, &mid)){
		திரும்பு MOSQ_ERR_MALFORMED_PACKET;
	}
	எனில்(mid == 0){
		திரும்பு MOSQ_ERR_MALFORMED_PACKET;
	}

	எனில்(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_SUBSCRIBE, &context->in_packet, &properties);
		எனில்(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			எனில்(rc == MOSQ_ERR_PROTOCOL){
				log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE packet with invalid properties.", context->id);
				திரும்பு MOSQ_ERR_MALFORMED_PACKET;
			}இல்லையெனில்{
				திரும்பு rc;
			}
		}

		எனில்(mosquitto_property_read_varint(properties, MQTT_PROP_SUBSCRIPTION_IDENTIFIER,
				&subscription_identifier, false)){

			/* If the identifier was force set to 0, this is an error */
			எனில்(subscription_identifier == 0){
				mosquitto_property_free_all(&properties);
				திரும்பு MOSQ_ERR_PROTOCOL;
			}
		}

		mosquitto_property_free_all(&properties);
		/* Note - User Property not handled */
	}

	போது(context->in_packet.pos < context->in_packet.remaining_length){
		memset(&sub, 0, அளவு(sub));
		sub.identifier = subscription_identifier;
		sub.properties = properties;
		எனில்(packet__read_string(&context->in_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(payload);
			திரும்பு MOSQ_ERR_MALFORMED_PACKET;
		}

		எனில்(sub.topic_filter){
			எனில்(!slen){
				log__printf(வெற்று, MOSQ_LOG_INFO,
						"Empty subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				திரும்பு MOSQ_ERR_MALFORMED_PACKET;
			}
			எனில்(mosquitto_sub_topic_check(sub.topic_filter)){
				log__printf(வெற்று, MOSQ_LOG_INFO,
						"Invalid subscription string from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				திரும்பு MOSQ_ERR_MALFORMED_PACKET;
			}

			எனில்(packet__read_byte(&context->in_packet, &sub.options)){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				திரும்பு MOSQ_ERR_MALFORMED_PACKET;
			}
			எனில்(sub.options & MQTT_SUB_OPT_NO_LOCAL && !strncmp(sub.topic_filter, "$share/", strlen("$share/"))){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: $share subscription with no-local set.", context->id);
				திரும்பு MOSQ_ERR_PROTOCOL;
			}

			எனில்(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt311){
				qos = sub.options;
				sub.options = 0;
				எனில்(context->is_bridge){
					sub.options |= MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | MQTT_SUB_OPT_NO_LOCAL;
				}
			}இல்லையெனில்{
				qos = sub.options & 0x03;
				sub.options &= 0xFC;

				எனில்(MQTT_SUB_OPT_GET_NO_LOCAL(sub.options) && !strncmp(sub.topic_filter, "$share/", 7)){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					திரும்பு MOSQ_ERR_PROTOCOL;
				}
				retain_handling = MQTT_SUB_OPT_GET_RETAIN_HANDLING(sub.options);
				எனில்(retain_handling == 0x30 || (sub.options & 0xC0) != 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					திரும்பு MOSQ_ERR_MALFORMED_PACKET;
				}
			}
			எனில்(qos > 2){
				log__printf(வெற்று, MOSQ_LOG_INFO,
						"Invalid QoS in subscription command from %s, disconnecting.",
						context->address);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				திரும்பு MOSQ_ERR_MALFORMED_PACKET;
			}
			எனில்(qos > context->max_qos){
				qos = context->max_qos;
			}
			sub.options |= qos;


			எனில்(context->listener && context->listener->mount_point){
				len = strlen(context->listener->mount_point) + slen + 1;
				sub_mount = mosquitto_malloc(len+1);
				எனில்(!sub_mount){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					திரும்பு MOSQ_ERR_NOMEM;
				}
				snprintf(sub_mount, len, "%s%s", context->listener->mount_point, sub.topic_filter);
				sub_mount[len] = '\0';

				mosquitto_FREE(sub.topic_filter);
				sub.topic_filter = sub_mount;

			}

			allowed = true;
			rc2 = mosquitto_acl_check(context, sub.topic_filter, 0, வெற்று, qos, false, properties, MOSQ_ACL_SUBSCRIBE);
			தேர்வு(rc2){
				நிலை MOSQ_ERR_SUCCESS:
					நிறுத்து;
				நிலை MOSQ_ERR_ACL_DENIED:
					allowed = false;
					எனில்(context->protocol == mosq_p_mqtt5){
						qos = MQTT_RC_NOT_AUTHORIZED;
					}இல்லையெனில் எனில்(context->protocol == mosq_p_mqtt311){
						qos = 0x80;
					}
					நிறுத்து;
				இயல்பு:
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					திரும்பு rc2;
			}
			எனில்(qos > 127){
				log__printf(வெற்று, MOSQ_LOG_DEBUG, "\t%s (denied)", sub.topic_filter);
			}இல்லையெனில்{
				log__printf(வெற்று, MOSQ_LOG_DEBUG, "\t%s (QoS %d)", sub.topic_filter, qos);
			}

			எனில்(allowed){
				rc2 = plugin__handle_subscribe(context, &sub);
				எனில்(rc2){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					திரும்பு rc2;
				}

				rc2 = sub__add(context, &sub);
				எனில்(rc2 > 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					திரும்பு rc2;
				}
				எனில்(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
					எனில்(rc2 == MOSQ_ERR_SUCCESS || rc2 == MOSQ_ERR_SUB_EXISTS){
						எனில்(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							திரும்பு rc;
						}
					}
				}இல்லையெனில்{
					எனில்((retain_handling == MQTT_SUB_OPT_SEND_RETAIN_ALWAYS)
							|| (rc2 == MOSQ_ERR_SUCCESS && retain_handling == MQTT_SUB_OPT_SEND_RETAIN_NEW)){

						எனில்(retain__queue(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							திரும்பு rc;
						}
					}
				}
				log__printf(வெற்று, MOSQ_LOG_SUBSCRIBE, "%s %d %s", context->id, qos, sub.topic_filter);

				plugin_persist__handle_subscription_add(context, &sub);
			}
			mosquitto_FREE(sub.topic_filter);

			tmp_payload = mosquitto_realloc(payload, payloadlen + 1);
			எனில்(tmp_payload){
				payload = tmp_payload;
				payload[payloadlen] = qos;
				payloadlen++;
			}இல்லையெனில்{
				mosquitto_FREE(payload);

				திரும்பு MOSQ_ERR_NOMEM;
			}
		}
	}

	எனில்(context->protocol != mosq_p_mqtt31){
		எனில்(payloadlen == 0){
			/* No subscriptions specified, protocol error. */
			திரும்பு MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	rc = send__suback(context, mid, payloadlen, payload);
	mosquitto_FREE(payload);

#ifdef WITH_PERSISTENCE
	db.persistence_changes++;
#endif

	எனில்(context->out_packet == வெற்று){
		rc = db__message_write_queued_out(context);
		எனில்(rc){
			திரும்பு rc;
		}
		rc = db__message_write_inflight_out_latest(context);
		எனில்(rc){
			திரும்பு rc;
		}
	}

	திரும்பு rc;
}
