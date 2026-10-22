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
#अन्तर्भाव "alias_mosq.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "property_mosq.h"
#अन्तर्भाव "read_handle.h"
#अन्तर्भाव "send_mosq.h"
#अन्तर्भाव "sys_tree.h"
#अन्तर्भाव "util_mosq.h"


स्थिर पूर्णाङ्क process_bad_message(संरचना mosquitto *context, संरचना mosquitto_base_संदेश *base_संदेश, uint8_t reason_code)
{
	पूर्णाङ्क rc = MOSQ_ERR_UNKNOWN;
	यदि(base_संदेश){
		विकल्प(base_संदेश->data.qos){
			पक्ष 0:
				rc = MOSQ_ERR_SUCCESS;
				भङ्ग;
			पक्ष 1:
				यदि(context){
					rc = send__puback(context, base_संदेश->data.source_mid, reason_code, रिक्त);
				}अन्यथा{
					rc = MOSQ_ERR_SUCCESS;
				}
				भङ्ग;
			पक्ष 2:
				यदि(context){
					rc = send__pubrec(context, base_संदेश->data.source_mid, reason_code, रिक्त);
				}अन्यथा{
					rc = MOSQ_ERR_SUCCESS;
				}
				भङ्ग;
		}
		db_संदेश_store_free(base_संदेश);
	}
	यदि(context && db.config->max_queued_messages > 0 && context->out_packet_मोजणी >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	प्रत्यागम rc;
}


पूर्णाङ्क handle__accepted_publish(संरचना mosquitto *context, संरचना mosquitto_base_संदेश *base_संदेश, uint16_t mid, पूर्णाङ्क dup, uint32_t *message_expiry_interval)
{
	पूर्णाङ्क rc;
	पूर्णाङ्क rc2;
	संरचना mosquitto_base_संदेश *stored = रिक्त;
	संरचना mosquitto_client_संदेश *cmsg_stored = रिक्त;

	{
		rc = plugin__handle_message_in(context, &base_संदेश->data);
		यदि(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(रिक्त, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, base_संदेश->data.qos, base_संदेश->data.retain, base_संदेश->data.source_mid, base_संदेश->data.topic,
					(long)base_संदेश->data.payloadlen);

			प्रत्यागम process_bad_message(context, base_संदेश, MQTT_RC_NOT_AUTHORIZED);
		}अन्यथा यदि(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(रिक्त, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			प्रत्यागम process_bad_message(context, base_संदेश, MQTT_RC_QUOTA_EXCEEDED);
		}अन्यथा यदि(rc != MOSQ_ERR_SUCCESS){
			db_संदेश_store_free(base_संदेश);
			प्रत्यागम rc;
		}
	}

	यदि(base_संदेश->data.qos > 0){
		db__message_store_find(context, base_संदेश->data.source_mid, &cmsg_stored);
	}

	यदि(cmsg_stored && base_संदेश->data.source_mid != 0 &&
			(cmsg_stored->base_संदेश->data.qos != base_संदेश->data.qos
			|| cmsg_stored->base_संदेश->data.payloadlen != base_संदेश->data.payloadlen
			|| strcmp(cmsg_stored->base_संदेश->data.topic, base_संदेश->data.topic)
			|| (base_संदेश->data.payloadlen > 0 && memcmp(cmsg_stored->base_संदेश->data.payload, base_संदेश->data.payload, base_संदेश->data.payloadlen)))){

		log__printf(रिक्त, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", base_संदेश->data.source_mid, context->id);
		db__message_remove_incoming(context, base_संदेश->data.source_mid);
		cmsg_stored = रिक्त;
	}

	यदि(!cmsg_stored){
		यदि(base_संदेश->data.qos > 0 && context->msgs_in.inflight_quota == 0){
			log__printf(रिक्त, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#यदि 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_संदेश_store_free(base_संदेश);
			प्रत्यागम MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED;
#endif
		}

		यदि(base_संदेश->data.qos == 0
				|| db__ready_for_flight(context, mosq_md_in, base_संदेश->data.qos)
				){

			dup = 0;
			rc = db__message_store(context, base_संदेश, message_expiry_interval, mosq_mo_client);
			यदि(rc){
				प्रत्यागम rc;
			}
		}अन्यथा{
			/* Client isn't allowed any more incoming messages, so fail early */
			प्रत्यागम process_bad_message(context, base_संदेश, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = base_संदेश;
		base_संदेश = रिक्त;
		dup = 0;
	}अन्यथा{
		db_संदेश_store_free(base_संदेश);
		base_संदेश = रिक्त;
		stored = cmsg_stored->base_संदेश;
		cmsg_stored->data.dup++;
		dup = cmsg_stored->data.dup;
	}

	विकल्प(stored->data.qos){
		पक्ष 0:
			rc2 = sub__messages_queue(context->id, stored->data.qos, &stored);
			यदि(rc2 > 0){
				rc = rc2;
			}
			भङ्ग;
		पक्ष 1:
			util__decrement_receive_quota(context);
			rc2 = sub__messages_queue(context->id, stored->data.qos, &stored);
			/* stored may now be free, so don't refer to it */
			यदि(rc2 == MOSQ_ERR_SUCCESS || context->protocol != mosq_p_mqtt5){
				rc2 = send__puback(context, mid, 0, रिक्त);
				यदि(rc2){
					rc = rc2;
				}
			}अन्यथा यदि(rc2 == MOSQ_ERR_NO_SUBSCRIBERS){
				rc2 = send__puback(context, mid, MQTT_RC_NO_MATCHING_SUBSCRIBERS, रिक्त);
				यदि(rc2){
					rc = rc2;
				}
			}अन्यथा{
				rc = rc2;
			}
			भङ्ग;
		पक्ष 2:
			{
				पूर्णाङ्क res;
				यदि(dup == 0){
					res = db_message_घाला_incoming(context, 0, stored, true);
				}अन्यथा{
					res = 0;
				}

				/* db__message_insert() returns 2 to indicate dropped message
				 * due to queue. This isn't an error so don't disconnect them. */
				/* FIXME - this is no longer necessary due to failing early above */
				यदि(!res){
					यदि(dup == 0 || dup == 1){
						rc2 = send__pubrec(context, stored->data.source_mid, 0, रिक्त);
						यदि(rc2){
							rc = rc2;
						}
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH with dup = %d.", context->id, dup);
						प्रत्यागम MOSQ_ERR_PROTOCOL;
					}
				}अन्यथा{
					rc = res;
				}
				भङ्ग;
			}
	}

	db__message_write_queued_in(context);
	प्रत्यागम rc;
}


पूर्णाङ्क handle__publish(संरचना mosquitto *context)
{
	uint8_t dup;
	पूर्णाङ्क rc = 0;
	uint8_t header = context->in_packet.command;
	संरचना mosquitto_base_संदेश *base_संदेश;
	size_t len;
	uint16_t slen;
	वर्ण *topic_mount;
	mosquitto_property *properties = रिक्त;
	uint32_t message_expiry_interval = संदेश_EXPIRY_INFINITE;
	पूर्णाङ्क topic_alias = -1;
	uint16_t mid = 0;

	यदि(context->state != mosq_cs_active){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		प्रत्यागम MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	base_संदेश = mosquitto_calloc(1, परिमाण(संरचना mosquitto_base_संदेश));
	यदि(base_संदेश == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	dup = (header & 0x08)>>3;
	base_संदेश->data.qos = (header & 0x06)>>1;
	यदि(dup == 1 && base_संदेश->data.qos == 0){
		log__printf(रिक्त, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_संदेश_store_free(base_संदेश);
		प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
	}
	यदि(base_संदेश->data.qos == 3){
		log__printf(रिक्त, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_संदेश_store_free(base_संदेश);
		प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
	}
	यदि(base_संदेश->data.qos > context->max_qos){
		log__printf(रिक्त, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_संदेश_store_free(base_संदेश);
		प्रत्यागम MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	base_संदेश->data.retain = (header & 0x01);

	यदि(base_संदेश->data.retain && db.config->retain_available == false){
		db_संदेश_store_free(base_संदेश);
		प्रत्यागम MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	यदि(packet__read_string(&context->in_packet, &base_संदेश->data.topic, &slen)){
		db_संदेश_store_free(base_संदेश);
		प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
	}
	यदि(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_संदेश_store_free(base_संदेश);
		प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
	}

	यदि(base_संदेश->data.qos > 0){
		यदि(packet__read_uint16(&context->in_packet, &mid)){
			db_संदेश_store_free(base_संदेश);
			प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
		}
		यदि(mid == 0){
			db_संदेश_store_free(base_संदेश);
			log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			प्रत्यागम MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		base_संदेश->data.source_mid = mid;
	}

	/* Handle properties */
	यदि(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->in_packet, &properties);
		यदि(rc){
			db_संदेश_store_free(base_संदेश);
			प्रत्यागम rc;
		}

		rc = property__process_publish(base_संदेश, &properties, &topic_alias, &message_expiry_interval, context->bridge);
		यदि(rc){
			mosquitto_property_free_all(&properties);
			db_संदेश_store_free(base_संदेश);
			प्रत्यागम MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	यदि(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_संदेश_store_free(base_संदेश);
		प्रत्यागम MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}अन्यथा यदि(topic_alias > 0){
		यदि(base_संदेश->data.topic){
			rc = alias__add_r2l(context, base_संदेश->data.topic, (uint16_t)topic_alias);
			यदि(rc){
				db_संदेश_store_free(base_संदेश);
				प्रत्यागम rc;
			}
		}अन्यथा{
			rc = alias__find_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &base_संदेश->data.topic);
			यदि(rc){
				db_संदेश_store_free(base_संदेश);
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}
		}
	}

#ifdef WITH_BRIDGE
	rc = bridge__remap_topic_in(context, &base_संदेश->data.topic);
	यदि(rc){
		db_संदेश_store_free(base_संदेश);
		प्रत्यागम rc;
	}

#endif
	यदि(mosquitto_pub_topic_check(base_संदेश->data.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_संदेश_store_free(base_संदेश);
		प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
	}

	base_संदेश->data.payloadlen = context->in_packet.remaining_length - context->in_packet.pos;
	metrics__int_inc(mosq_counter_pub_bytes_received, base_संदेश->data.payloadlen);
	यदि(context->listener && context->listener->mount_point){
		len = strlen(context->listener->mount_point) + strlen(base_संदेश->data.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		यदि(!topic_mount){
			db_संदेश_store_free(base_संदेश);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_point, base_संदेश->data.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(base_संदेश->data.topic);
		base_संदेश->data.topic = topic_mount;
	}

	यदि(base_संदेश->data.payloadlen){
		यदि(db.config->message_size_limit && base_संदेश->data.payloadlen > db.config->message_size_limit){
			log__printf(रिक्त, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_संदेश->data.qos, base_संदेश->data.retain, base_संदेश->data.source_mid, base_संदेश->data.topic, (long)base_संदेश->data.payloadlen);
			प्रत्यागम process_bad_message(context, base_संदेश, MQTT_RC_PACKET_TOO_LARGE);
		}
		base_संदेश->data.payload = mosquitto_malloc(base_संदेश->data.payloadlen+1);
		यदि(base_संदेश->data.payload == रिक्त){
			db_संदेश_store_free(base_संदेश);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_संदेश->data.payload)[base_संदेश->data.payloadlen] = 0;

		यदि(packet__read_bytes(&context->in_packet, base_संदेश->data.payload, base_संदेश->data.payloadlen)){
			db_संदेश_store_free(base_संदेश);
			प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			base_संदेश->data.topic, base_संदेश->data.payloadlen, base_संदेश->data.payload,
			base_संदेश->data.qos, base_संदेश->data.retain, base_संदेश->data.properties,
			MOSQ_ACL_WRITE);
	यदि(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(रिक्त, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, base_संदेश->data.qos, base_संदेश->data.retain, base_संदेश->data.source_mid, base_संदेश->data.topic,
				(long)base_संदेश->data.payloadlen);
		प्रत्यागम process_bad_message(context, base_संदेश, MQTT_RC_NOT_AUTHORIZED);
	}अन्यथा यदि(rc != MOSQ_ERR_SUCCESS){
		db_संदेश_store_free(base_संदेश);
		प्रत्यागम rc;
	}

	log__printf(रिक्त, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_संदेश->data.qos, base_संदेश->data.retain, base_संदेश->data.source_mid, base_संदेश->data.topic, (long)base_संदेश->data.payloadlen);

	यदि(!strncmp(base_संदेश->data.topic, "$CONTROL/", 9)){
#ifdef WITH_CONTROL
		rc = control__process(context, base_संदेश);
		db_संदेश_store_free(base_संदेश);
		प्रत्यागम rc;
#अन्यथा
		प्रत्यागम process_bad_message(context, base_संदेश, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#endif
	}

	प्रत्यागम handle__accepted_publish(context, base_संदेश, mid, dup, &message_expiry_interval);
}
