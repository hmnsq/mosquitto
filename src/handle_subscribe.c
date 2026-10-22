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

#అన్తర్భావ "config.h"

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "packet_mosq.h"
#అన్తర్భావ "property_mosq.h"


పూర్ణాంకం handle__subscribe(నిర్మాణం mosquitto *context)
{
	పూర్ణాంకం rc = 0;
	పూర్ణాంకం rc2;
	uint16_t mid;
	uint8_t qos;
	uint8_t retain_handling = 0;
	uint8_t *payload = ఖాళీ, *tmp_payload;
	uint32_t payloadlen = 0;
	size_t len;
	uint16_t slen;
	అక్షరం *sub_mount;
	mosquitto_property *properties = ఖాళీ;
	bool allowed;
	నిర్మాణం mosquitto_subscription sub;
	uint32_t subscription_identifier = 0;

	ఒకవేళ(!context){
		తిరిగి MOSQ_ERR_INVAL;
	}

	ఒకవేళ(context->state != mosq_cs_చురుకైన){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE before session is active.", context->id);
		తిరిగి MOSQ_ERR_PROTOCOL;
	}
	ఒకవేళ(context->లోపల_packet.command != (CMD_SUBSCRIBE|2)){
		తిరిగి MOSQ_ERR_MALFORMED_PACKET;
	}

	log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "Received SUBSCRIBE from %s", context->id);

	ఒకవేళ(context->protocol != mosq_p_mqtt31){
		ఒకవేళ((context->లోపల_packet.command&0x0F) != 0x02){
			తిరిగి MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	ఒకవేళ(packet__read_uint16(&context->లోపల_packet, &mid)){
		తిరిగి MOSQ_ERR_MALFORMED_PACKET;
	}
	ఒకవేళ(mid == 0){
		తిరిగి MOSQ_ERR_MALFORMED_PACKET;
	}

	ఒకవేళ(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_SUBSCRIBE, &context->లోపల_packet, &properties);
		ఒకవేళ(rc){
			/* FIXME - it would be better if property__read_all() returned
			 * MOSQ_ERR_MALFORMED_PACKET, but this is would change the library
			 * return codes so needs doc changes as well. */
			ఒకవేళ(rc == MOSQ_ERR_PROTOCOL){
				log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: SUBSCRIBE packet with invalid properties.", context->id);
				తిరిగి MOSQ_ERR_MALFORMED_PACKET;
			}లేకపోతే{
				తిరిగి rc;
			}
		}

		ఒకవేళ(mosquitto_property_read_varint(properties, MQTT_PROP_SUBSCRIPTION_IDENTIFIER,
				&subscription_identifier, false)){

			/* If the identifier was force set to 0, this is an error */
			ఒకవేళ(subscription_identifier == 0){
				mosquitto_property_free_all(&properties);
				తిరిగి MOSQ_ERR_PROTOCOL;
			}
		}

		mosquitto_property_free_all(&properties);
		/* Note - User Property not handled */
	}

	అయితే(context->లోపల_packet.pos < context->లోపల_packet.మిగిలిన_నిడివి){
		memset(&sub, 0, పరిమాణం(sub));
		sub.identifier = subscription_identifier;
		sub.properties = properties;
		ఒకవేళ(packet__read_string(&context->లోపల_packet, &sub.topic_filter, &slen)){
			mosquitto_FREE(payload);
			తిరిగి MOSQ_ERR_MALFORMED_PACKET;
		}

		ఒకవేళ(sub.topic_filter){
			ఒకవేళ(!slen){
				log__printf(ఖాళీ, MOSQ_LOG_INFO,
						"Empty subscription string from %s, disconnecting.",
						context->చిరునామా);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				తిరిగి MOSQ_ERR_MALFORMED_PACKET;
			}
			ఒకవేళ(mosquitto_sub_topic_check(sub.topic_filter)){
				log__printf(ఖాళీ, MOSQ_LOG_INFO,
						"Invalid subscription string from %s, disconnecting.",
						context->చిరునామా);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				తిరిగి MOSQ_ERR_MALFORMED_PACKET;
			}

			ఒకవేళ(packet__read_byte(&context->లోపల_packet, &sub.options)){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				తిరిగి MOSQ_ERR_MALFORMED_PACKET;
			}
			ఒకవేళ(sub.options & MQTT_SUB_OPT_NO_LOCAL && !అక్షరశ్రేణి_పోలిక(sub.topic_filter, "$share/", అక్షరశ్రేణి_పొడవు("$share/"))){
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: $share subscription with no-local set.", context->id);
				తిరిగి MOSQ_ERR_PROTOCOL;
			}

			ఒకవేళ(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt311){
				qos = sub.options;
				sub.options = 0;
				ఒకవేళ(context->is_bridge){
					sub.options |= MQTT_SUB_OPT_RETAIN_AS_PUBLISHED | MQTT_SUB_OPT_NO_LOCAL;
				}
			}లేకపోతే{
				qos = sub.options & 0x03;
				sub.options &= 0xFC;

				ఒకవేళ(MQTT_SUB_OPT_GET_NO_LOCAL(sub.options) && !అక్షరశ్రేణి_పోలిక(sub.topic_filter, "$share/", 7)){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					తిరిగి MOSQ_ERR_PROTOCOL;
				}
				retain_handling = MQTT_SUB_OPT_GET_RETAIN_HANDLING(sub.options);
				ఒకవేళ(retain_handling == 0x30 || (sub.options & 0xC0) != 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					తిరిగి MOSQ_ERR_MALFORMED_PACKET;
				}
			}
			ఒకవేళ(qos > 2){
				log__printf(ఖాళీ, MOSQ_LOG_INFO,
						"Invalid QoS in subscription command from %s, disconnecting.",
						context->చిరునామా);
				mosquitto_FREE(sub.topic_filter);
				mosquitto_FREE(payload);
				తిరిగి MOSQ_ERR_MALFORMED_PACKET;
			}
			ఒకవేళ(qos > context->max_qos){
				qos = context->max_qos;
			}
			sub.options |= qos;


			ఒకవేళ(context->listener && context->listener->mount_బిందువు){
				len = అక్షరశ్రేణి_పొడవు(context->listener->mount_బిందువు) + slen + 1;
				sub_mount = mosquitto_malloc(len+1);
				ఒకవేళ(!sub_mount){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					తిరిగి MOSQ_ERR_NOMEM;
				}
				snprintf(sub_mount, len, "%s%s", context->listener->mount_బిందువు, sub.topic_filter);
				sub_mount[len] = '\0';

				mosquitto_FREE(sub.topic_filter);
				sub.topic_filter = sub_mount;

			}

			allowed = true;
			rc2 = mosquitto_acl_check(context, sub.topic_filter, 0, ఖాళీ, qos, false, properties, MOSQ_ACL_SUBSCRIBE);
			ఎంపిక(rc2){
				స్థితి MOSQ_ERR_SUCCESS:
					ఆపు;
				స్థితి MOSQ_ERR_ACL_DENIED:
					allowed = false;
					ఒకవేళ(context->protocol == mosq_p_mqtt5){
						qos = MQTT_RC_NOT_AUTHORIZED;
					}లేకపోతే ఒకవేళ(context->protocol == mosq_p_mqtt311){
						qos = 0x80;
					}
					ఆపు;
				అప్రమేయ:
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					తిరిగి rc2;
			}
			ఒకవేళ(qos > 127){
				log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "\t%s (denied)", sub.topic_filter);
			}లేకపోతే{
				log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "\t%s (QoS %d)", sub.topic_filter, qos);
			}

			ఒకవేళ(allowed){
				rc2 = plugin__handle_subscribe(context, &sub);
				ఒకవేళ(rc2){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					తిరిగి rc2;
				}

				rc2 = sub_కూడు(context, &sub);
				ఒకవేళ(rc2 > 0){
					mosquitto_FREE(sub.topic_filter);
					mosquitto_FREE(payload);
					తిరిగి rc2;
				}
				ఒకవేళ(context->protocol == mosq_p_mqtt311 || context->protocol == mosq_p_mqtt31){
					ఒకవేళ(rc2 == MOSQ_ERR_SUCCESS || rc2 == MOSQ_ERR_SUB_EXISTS){
						ఒకవేళ(retain_వరుస(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							తిరిగి rc;
						}
					}
				}లేకపోతే{
					ఒకవేళ((retain_handling == MQTT_SUB_OPT_SEND_RETAIN_ALWAYS)
							|| (rc2 == MOSQ_ERR_SUCCESS && retain_handling == MQTT_SUB_OPT_SEND_RETAIN_NEW)){

						ఒకవేళ(retain_వరుస(context, &sub)){
							mosquitto_FREE(sub.topic_filter);
							mosquitto_FREE(payload);
							తిరిగి rc;
						}
					}
				}
				log__printf(ఖాళీ, MOSQ_LOG_SUBSCRIBE, "%s %d %s", context->id, qos, sub.topic_filter);

				plugin_persist_handle_subscription_కూడు(context, &sub);
			}
			mosquitto_FREE(sub.topic_filter);

			tmp_payload = mosquitto_realloc(payload, payloadlen + 1);
			ఒకవేళ(tmp_payload){
				payload = tmp_payload;
				payload[payloadlen] = qos;
				payloadlen++;
			}లేకపోతే{
				mosquitto_FREE(payload);

				తిరిగి MOSQ_ERR_NOMEM;
			}
		}
	}

	ఒకవేళ(context->protocol != mosq_p_mqtt31){
		ఒకవేళ(payloadlen == 0){
			/* No subscriptions specified, protocol error. */
			తిరిగి MOSQ_ERR_MALFORMED_PACKET;
		}
	}
	rc = send__suback(context, mid, payloadlen, payload);
	mosquitto_FREE(payload);

#నిర్వచితమైతే WITH_PERSISTENCE
	db.persistence_changes++;
#అయితే_ముగింపు

	ఒకవేళ(context->బహిర్గతం_packet == ఖాళీ){
		rc = db_సందేశం_write_queued_బహిర్గతం(context);
		ఒకవేళ(rc){
			తిరిగి rc;
		}
		rc = db_సందేశం_write_inflight_బహిర్గతం_latest(context);
		ఒకవేళ(rc){
			తిరిగి rc;
		}
	}

	తిరిగి rc;
}
