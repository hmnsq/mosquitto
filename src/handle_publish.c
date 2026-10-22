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
#அந்தர்பாவ "alias_mosq.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "packet_mosq.h"
#அந்தர்பாவ "property_mosq.h"
#அந்தர்பாவ "read_handle.h"
#அந்தர்பாவ "send_mosq.h"
#அந்தர்பாவ "sys_tree.h"
#அந்தர்பாவ "util_mosq.h"


நிலையான முழு process_bad_message(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி, uint8_t reason_code)
{
	முழு rc = MOSQ_ERR_UNKNOWN;
	எனில்(base_செய்தி){
		தேர்வு(base_செய்தி->data.qos){
			நிலை 0:
				rc = MOSQ_ERR_SUCCESS;
				நிறுத்து;
			நிலை 1:
				எனில்(context){
					rc = send__puback(context, base_செய்தி->data.source_mid, reason_code, வெற்று);
				}இல்லையெனில்{
					rc = MOSQ_ERR_SUCCESS;
				}
				நிறுத்து;
			நிலை 2:
				எனில்(context){
					rc = send__pubrec(context, base_செய்தி->data.source_mid, reason_code, வெற்று);
				}இல்லையெனில்{
					rc = MOSQ_ERR_SUCCESS;
				}
				நிறுத்து;
		}
		db_செய்தி_store_free(base_செய்தி);
	}
	எனில்(context && db.config->max_queued_messages > 0 && context->out_packet_மொத்தம் >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	திரும்பு rc;
}


முழு handle__accepted_publish(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி, uint16_t mid, முழு dup, uint32_t *message_expiry_interval)
{
	முழு rc;
	முழு rc2;
	கட்டமைப்பு mosquitto_base_செய்தி *stored = வெற்று;
	கட்டமைப்பு mosquitto_client_செய்தி *cmsg_stored = வெற்று;

	{
		rc = plugin__handle_message_in(context, &base_செய்தி->data);
		எனில்(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(வெற்று, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, base_செய்தி->data.qos, base_செய்தி->data.retain, base_செய்தி->data.source_mid, base_செய்தி->data.topic,
					(long)base_செய்தி->data.payloadlen);

			திரும்பு process_bad_message(context, base_செய்தி, MQTT_RC_NOT_AUTHORIZED);
		}இல்லையெனில் எனில்(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(வெற்று, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			திரும்பு process_bad_message(context, base_செய்தி, MQTT_RC_QUOTA_EXCEEDED);
		}இல்லையெனில் எனில்(rc != MOSQ_ERR_SUCCESS){
			db_செய்தி_store_free(base_செய்தி);
			திரும்பு rc;
		}
	}

	எனில்(base_செய்தி->data.qos > 0){
		db__message_store_find(context, base_செய்தி->data.source_mid, &cmsg_stored);
	}

	எனில்(cmsg_stored && base_செய்தி->data.source_mid != 0 &&
			(cmsg_stored->base_செய்தி->data.qos != base_செய்தி->data.qos
			|| cmsg_stored->base_செய்தி->data.payloadlen != base_செய்தி->data.payloadlen
			|| strcmp(cmsg_stored->base_செய்தி->data.topic, base_செய்தி->data.topic)
			|| (base_செய்தி->data.payloadlen > 0 && memcmp(cmsg_stored->base_செய்தி->data.payload, base_செய்தி->data.payload, base_செய்தி->data.payloadlen)))){

		log__printf(வெற்று, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", base_செய்தி->data.source_mid, context->id);
		db__message_remove_incoming(context, base_செய்தி->data.source_mid);
		cmsg_stored = வெற்று;
	}

	எனில்(!cmsg_stored){
		எனில்(base_செய்தி->data.qos > 0 && context->msgs_in.inflight_quota == 0){
			log__printf(வெற்று, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#எனில் 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_செய்தி_store_free(base_செய்தி);
			திரும்பு MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED;
#endif
		}

		எனில்(base_செய்தி->data.qos == 0
				|| db__ready_for_flight(context, mosq_md_in, base_செய்தி->data.qos)
				){

			dup = 0;
			rc = db__message_store(context, base_செய்தி, message_expiry_interval, mosq_mo_client);
			எனில்(rc){
				திரும்பு rc;
			}
		}இல்லையெனில்{
			/* Client isn't allowed any more incoming messages, so fail early */
			திரும்பு process_bad_message(context, base_செய்தி, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = base_செய்தி;
		base_செய்தி = வெற்று;
		dup = 0;
	}இல்லையெனில்{
		db_செய்தி_store_free(base_செய்தி);
		base_செய்தி = வெற்று;
		stored = cmsg_stored->base_செய்தி;
		cmsg_stored->data.dup++;
		dup = cmsg_stored->data.dup;
	}

	தேர்வு(stored->data.qos){
		நிலை 0:
			rc2 = sub__messages_queue(context->id, stored->data.qos, &stored);
			எனில்(rc2 > 0){
				rc = rc2;
			}
			நிறுத்து;
		நிலை 1:
			util__decrement_receive_quota(context);
			rc2 = sub__messages_queue(context->id, stored->data.qos, &stored);
			/* stored may now be free, so don't refer to it */
			எனில்(rc2 == MOSQ_ERR_SUCCESS || context->protocol != mosq_p_mqtt5){
				rc2 = send__puback(context, mid, 0, வெற்று);
				எனில்(rc2){
					rc = rc2;
				}
			}இல்லையெனில் எனில்(rc2 == MOSQ_ERR_NO_SUBSCRIBERS){
				rc2 = send__puback(context, mid, MQTT_RC_NO_MATCHING_SUBSCRIBERS, வெற்று);
				எனில்(rc2){
					rc = rc2;
				}
			}இல்லையெனில்{
				rc = rc2;
			}
			நிறுத்து;
		நிலை 2:
			{
				முழு res;
				எனில்(dup == 0){
					res = db_message_செருகு_incoming(context, 0, stored, true);
				}இல்லையெனில்{
					res = 0;
				}

				/* db__message_insert() returns 2 to indicate dropped message
				 * due to queue. This isn't an error so don't disconnect them. */
				/* FIXME - this is no longer necessary due to failing early above */
				எனில்(!res){
					எனில்(dup == 0 || dup == 1){
						rc2 = send__pubrec(context, stored->data.source_mid, 0, வெற்று);
						எனில்(rc2){
							rc = rc2;
						}
					}இல்லையெனில்{
						log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH with dup = %d.", context->id, dup);
						திரும்பு MOSQ_ERR_PROTOCOL;
					}
				}இல்லையெனில்{
					rc = res;
				}
				நிறுத்து;
			}
	}

	db__message_write_queued_in(context);
	திரும்பு rc;
}


முழு handle__publish(கட்டமைப்பு mosquitto *context)
{
	uint8_t dup;
	முழு rc = 0;
	uint8_t header = context->in_packet.command;
	கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி;
	size_t len;
	uint16_t slen;
	எழுத்து *topic_mount;
	mosquitto_property *properties = வெற்று;
	uint32_t message_expiry_interval = செய்தி_EXPIRY_INFINITE;
	முழு topic_alias = -1;
	uint16_t mid = 0;

	எனில்(context->state != mosq_cs_active){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		திரும்பு MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	base_செய்தி = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto_base_செய்தி));
	எனில்(base_செய்தி == வெற்று){
		திரும்பு MOSQ_ERR_NOMEM;
	}

	dup = (header & 0x08)>>3;
	base_செய்தி->data.qos = (header & 0x06)>>1;
	எனில்(dup == 1 && base_செய்தி->data.qos == 0){
		log__printf(வெற்று, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_செய்தி_store_free(base_செய்தி);
		திரும்பு MOSQ_ERR_MALFORMED_PACKET;
	}
	எனில்(base_செய்தி->data.qos == 3){
		log__printf(வெற்று, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_செய்தி_store_free(base_செய்தி);
		திரும்பு MOSQ_ERR_MALFORMED_PACKET;
	}
	எனில்(base_செய்தி->data.qos > context->max_qos){
		log__printf(வெற்று, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_செய்தி_store_free(base_செய்தி);
		திரும்பு MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	base_செய்தி->data.retain = (header & 0x01);

	எனில்(base_செய்தி->data.retain && db.config->retain_available == false){
		db_செய்தி_store_free(base_செய்தி);
		திரும்பு MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	எனில்(packet__read_string(&context->in_packet, &base_செய்தி->data.topic, &slen)){
		db_செய்தி_store_free(base_செய்தி);
		திரும்பு MOSQ_ERR_MALFORMED_PACKET;
	}
	எனில்(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_செய்தி_store_free(base_செய்தி);
		திரும்பு MOSQ_ERR_MALFORMED_PACKET;
	}

	எனில்(base_செய்தி->data.qos > 0){
		எனில்(packet__read_uint16(&context->in_packet, &mid)){
			db_செய்தி_store_free(base_செய்தி);
			திரும்பு MOSQ_ERR_MALFORMED_PACKET;
		}
		எனில்(mid == 0){
			db_செய்தி_store_free(base_செய்தி);
			log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			திரும்பு MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		base_செய்தி->data.source_mid = mid;
	}

	/* Handle properties */
	எனில்(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->in_packet, &properties);
		எனில்(rc){
			db_செய்தி_store_free(base_செய்தி);
			திரும்பு rc;
		}

		rc = property__process_publish(base_செய்தி, &properties, &topic_alias, &message_expiry_interval, context->bridge);
		எனில்(rc){
			mosquitto_property_free_all(&properties);
			db_செய்தி_store_free(base_செய்தி);
			திரும்பு MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	எனில்(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_செய்தி_store_free(base_செய்தி);
		திரும்பு MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}இல்லையெனில் எனில்(topic_alias > 0){
		எனில்(base_செய்தி->data.topic){
			rc = alias__add_r2l(context, base_செய்தி->data.topic, (uint16_t)topic_alias);
			எனில்(rc){
				db_செய்தி_store_free(base_செய்தி);
				திரும்பு rc;
			}
		}இல்லையெனில்{
			rc = alias__find_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &base_செய்தி->data.topic);
			எனில்(rc){
				db_செய்தி_store_free(base_செய்தி);
				log__printf(வெற்று, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				திரும்பு MOSQ_ERR_PROTOCOL;
			}
		}
	}

#ifdef WITH_BRIDGE
	rc = bridge__remap_topic_in(context, &base_செய்தி->data.topic);
	எனில்(rc){
		db_செய்தி_store_free(base_செய்தி);
		திரும்பு rc;
	}

#endif
	எனில்(mosquitto_pub_topic_check(base_செய்தி->data.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_செய்தி_store_free(base_செய்தி);
		திரும்பு MOSQ_ERR_MALFORMED_PACKET;
	}

	base_செய்தி->data.payloadlen = context->in_packet.remaining_length - context->in_packet.pos;
	metrics__int_inc(mosq_counter_pub_bytes_received, base_செய்தி->data.payloadlen);
	எனில்(context->listener && context->listener->mount_point){
		len = strlen(context->listener->mount_point) + strlen(base_செய்தி->data.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		எனில்(!topic_mount){
			db_செய்தி_store_free(base_செய்தி);
			திரும்பு MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_point, base_செய்தி->data.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(base_செய்தி->data.topic);
		base_செய்தி->data.topic = topic_mount;
	}

	எனில்(base_செய்தி->data.payloadlen){
		எனில்(db.config->message_size_limit && base_செய்தி->data.payloadlen > db.config->message_size_limit){
			log__printf(வெற்று, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_செய்தி->data.qos, base_செய்தி->data.retain, base_செய்தி->data.source_mid, base_செய்தி->data.topic, (long)base_செய்தி->data.payloadlen);
			திரும்பு process_bad_message(context, base_செய்தி, MQTT_RC_PACKET_TOO_LARGE);
		}
		base_செய்தி->data.payload = mosquitto_malloc(base_செய்தி->data.payloadlen+1);
		எனில்(base_செய்தி->data.payload == வெற்று){
			db_செய்தி_store_free(base_செய்தி);
			திரும்பு MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_செய்தி->data.payload)[base_செய்தி->data.payloadlen] = 0;

		எனில்(packet__read_bytes(&context->in_packet, base_செய்தி->data.payload, base_செய்தி->data.payloadlen)){
			db_செய்தி_store_free(base_செய்தி);
			திரும்பு MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			base_செய்தி->data.topic, base_செய்தி->data.payloadlen, base_செய்தி->data.payload,
			base_செய்தி->data.qos, base_செய்தி->data.retain, base_செய்தி->data.properties,
			MOSQ_ACL_WRITE);
	எனில்(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(வெற்று, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, base_செய்தி->data.qos, base_செய்தி->data.retain, base_செய்தி->data.source_mid, base_செய்தி->data.topic,
				(long)base_செய்தி->data.payloadlen);
		திரும்பு process_bad_message(context, base_செய்தி, MQTT_RC_NOT_AUTHORIZED);
	}இல்லையெனில் எனில்(rc != MOSQ_ERR_SUCCESS){
		db_செய்தி_store_free(base_செய்தி);
		திரும்பு rc;
	}

	log__printf(வெற்று, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_செய்தி->data.qos, base_செய்தி->data.retain, base_செய்தி->data.source_mid, base_செய்தி->data.topic, (long)base_செய்தி->data.payloadlen);

	எனில்(!strncmp(base_செய்தி->data.topic, "$CONTROL/", 9)){
#ifdef WITH_CONTROL
		rc = control__process(context, base_செய்தி);
		db_செய்தி_store_free(base_செய்தி);
		திரும்பு rc;
#இல்லையெனில்
		திரும்பு process_bad_message(context, base_செய்தி, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#endif
	}

	திரும்பு handle__accepted_publish(context, base_செய்தி, mid, dup, &message_expiry_interval);
}
