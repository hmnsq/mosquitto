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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "alias_mosq.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "property_mosq.h"
#ಅನ್ತರ್ಭಾವ "read_handle.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ process_bad_message(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ, uint8_t reason_code)
{
	ಪೂರ್ಣಾಙ್ಕ rc = MOSQ_ERR_UNKNOWN;
	ಯದಿ(base_ಸಂದೇಶ){
		ವಿಕಲ್ಪ(base_ಸಂದೇಶ->data.qos){
			ಪಕ್ಷ 0:
				rc = MOSQ_ERR_SUCCESS;
				ಭಙ್ಗ;
			ಪಕ್ಷ 1:
				ಯದಿ(context){
					rc = send__puback(context, base_ಸಂದೇಶ->data.source_mid, reason_code, ರಿಕ್ತ);
				}ಅನ್ಯಥಾ{
					rc = MOSQ_ERR_SUCCESS;
				}
				ಭಙ್ಗ;
			ಪಕ್ಷ 2:
				ಯದಿ(context){
					rc = send__pubrec(context, base_ಸಂದೇಶ->data.source_mid, reason_code, ರಿಕ್ತ);
				}ಅನ್ಯಥಾ{
					rc = MOSQ_ERR_SUCCESS;
				}
				ಭಙ್ಗ;
		}
		db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
	}
	ಯದಿ(context && db.config->max_queued_messages > 0 && context->out_packet_ಎಣಿಕೆ >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	ಪ್ರತ್ಯಾಗಮ rc;
}


ಪೂರ್ಣಾಙ್ಕ handle__accepted_publish(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ, uint16_t mid, ಪೂರ್ಣಾಙ್ಕ dup, uint32_t *message_expiry_interval)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಪೂರ್ಣಾಙ್ಕ rc2;
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *stored = ರಿಕ್ತ;
	ಸಂರಚನಾ mosquitto_client_ಸಂದೇಶ *cmsg_stored = ರಿಕ್ತ;

	{
		rc = plugin__handle_message_in(context, &base_ಸಂದೇಶ->data);
		ಯದಿ(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, base_ಸಂದೇಶ->data.qos, base_ಸಂದೇಶ->data.retain, base_ಸಂದೇಶ->data.source_mid, base_ಸಂದೇಶ->data.topic,
					(long)base_ಸಂದೇಶ->data.payloadlen);

			ಪ್ರತ್ಯಾಗಮ process_bad_message(context, base_ಸಂದೇಶ, MQTT_RC_NOT_AUTHORIZED);
		}ಅನ್ಯಥಾ ಯದಿ(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			ಪ್ರತ್ಯಾಗಮ process_bad_message(context, base_ಸಂದೇಶ, MQTT_RC_QUOTA_EXCEEDED);
		}ಅನ್ಯಥಾ ಯದಿ(rc != MOSQ_ERR_SUCCESS){
			db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	ಯದಿ(base_ಸಂದೇಶ->data.qos > 0){
		db__message_store_find(context, base_ಸಂದೇಶ->data.source_mid, &cmsg_stored);
	}

	ಯದಿ(cmsg_stored && base_ಸಂದೇಶ->data.source_mid != 0 &&
			(cmsg_stored->base_ಸಂದೇಶ->data.qos != base_ಸಂದೇಶ->data.qos
			|| cmsg_stored->base_ಸಂದೇಶ->data.payloadlen != base_ಸಂದೇಶ->data.payloadlen
			|| strcmp(cmsg_stored->base_ಸಂದೇಶ->data.topic, base_ಸಂದೇಶ->data.topic)
			|| (base_ಸಂದೇಶ->data.payloadlen > 0 && memcmp(cmsg_stored->base_ಸಂದೇಶ->data.payload, base_ಸಂದೇಶ->data.payload, base_ಸಂದೇಶ->data.payloadlen)))){

		log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", base_ಸಂದೇಶ->data.source_mid, context->id);
		db__message_remove_incoming(context, base_ಸಂದೇಶ->data.source_mid);
		cmsg_stored = ರಿಕ್ತ;
	}

	ಯದಿ(!cmsg_stored){
		ಯದಿ(base_ಸಂದೇಶ->data.qos > 0 && context->msgs_in.inflight_quota == 0){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#ಯದಿ 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED;
#endif
		}

		ಯದಿ(base_ಸಂದೇಶ->data.qos == 0
				|| db__ready_for_flight(context, mosq_md_in, base_ಸಂದೇಶ->data.qos)
				){

			dup = 0;
			rc = db__message_store(context, base_ಸಂದೇಶ, message_expiry_interval, mosq_mo_client);
			ಯದಿ(rc){
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}ಅನ್ಯಥಾ{
			/* Client isn't allowed any more incoming messages, so fail early */
			ಪ್ರತ್ಯಾಗಮ process_bad_message(context, base_ಸಂದೇಶ, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = base_ಸಂದೇಶ;
		base_ಸಂದೇಶ = ರಿಕ್ತ;
		dup = 0;
	}ಅನ್ಯಥಾ{
		db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
		base_ಸಂದೇಶ = ರಿಕ್ತ;
		stored = cmsg_stored->base_ಸಂದೇಶ;
		cmsg_stored->data.dup++;
		dup = cmsg_stored->data.dup;
	}

	ವಿಕಲ್ಪ(stored->data.qos){
		ಪಕ್ಷ 0:
			rc2 = sub__messages_queue(context->id, stored->data.qos, &stored);
			ಯದಿ(rc2 > 0){
				rc = rc2;
			}
			ಭಙ್ಗ;
		ಪಕ್ಷ 1:
			util__decrement_receive_quota(context);
			rc2 = sub__messages_queue(context->id, stored->data.qos, &stored);
			/* stored may now be free, so don't refer to it */
			ಯದಿ(rc2 == MOSQ_ERR_SUCCESS || context->protocol != mosq_p_mqtt5){
				rc2 = send__puback(context, mid, 0, ರಿಕ್ತ);
				ಯದಿ(rc2){
					rc = rc2;
				}
			}ಅನ್ಯಥಾ ಯದಿ(rc2 == MOSQ_ERR_NO_SUBSCRIBERS){
				rc2 = send__puback(context, mid, MQTT_RC_NO_MATCHING_SUBSCRIBERS, ರಿಕ್ತ);
				ಯದಿ(rc2){
					rc = rc2;
				}
			}ಅನ್ಯಥಾ{
				rc = rc2;
			}
			ಭಙ್ಗ;
		ಪಕ್ಷ 2:
			{
				ಪೂರ್ಣಾಙ್ಕ res;
				ಯದಿ(dup == 0){
					res = db_message_ಸೇರಿಸು_incoming(context, 0, stored, true);
				}ಅನ್ಯಥಾ{
					res = 0;
				}

				/* db__message_insert() returns 2 to indicate dropped message
				 * due to queue. This isn't an error so don't disconnect them. */
				/* FIXME - this is no longer necessary due to failing early above */
				ಯದಿ(!res){
					ಯದಿ(dup == 0 || dup == 1){
						rc2 = send__pubrec(context, stored->data.source_mid, 0, ರಿಕ್ತ);
						ಯದಿ(rc2){
							rc = rc2;
						}
					}ಅನ್ಯಥಾ{
						log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH with dup = %d.", context->id, dup);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
					}
				}ಅನ್ಯಥಾ{
					rc = res;
				}
				ಭಙ್ಗ;
			}
	}

	db__message_write_queued_in(context);
	ಪ್ರತ್ಯಾಗಮ rc;
}


ಪೂರ್ಣಾಙ್ಕ handle__publish(ಸಂರಚನಾ mosquitto *context)
{
	uint8_t dup;
	ಪೂರ್ಣಾಙ್ಕ rc = 0;
	uint8_t header = context->in_packet.command;
	ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ;
	size_t len;
	uint16_t slen;
	ವರ್ಣ *topic_mount;
	mosquitto_property *properties = ರಿಕ್ತ;
	uint32_t message_expiry_interval = ಸಂದೇಶ_EXPIRY_INFINITE;
	ಪೂರ್ಣಾಙ್ಕ topic_alias = -1;
	uint16_t mid = 0;

	ಯದಿ(context->state != mosq_cs_active){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	base_ಸಂದೇಶ = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ));
	ಯದಿ(base_ಸಂದೇಶ == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	dup = (header & 0x08)>>3;
	base_ಸಂದೇಶ->data.qos = (header & 0x06)>>1;
	ಯದಿ(dup == 1 && base_ಸಂದೇಶ->data.qos == 0){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
	}
	ಯದಿ(base_ಸಂದೇಶ->data.qos == 3){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
	}
	ಯದಿ(base_ಸಂದೇಶ->data.qos > context->max_qos){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	base_ಸಂದೇಶ->data.retain = (header & 0x01);

	ಯದಿ(base_ಸಂದೇಶ->data.retain && db.config->retain_available == false){
		db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	ಯದಿ(packet__read_string(&context->in_packet, &base_ಸಂದೇಶ->data.topic, &slen)){
		db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
	}
	ಯದಿ(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
	}

	ಯದಿ(base_ಸಂದೇಶ->data.qos > 0){
		ಯದಿ(packet__read_uint16(&context->in_packet, &mid)){
			db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
		}
		ಯದಿ(mid == 0){
			db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
			log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		base_ಸಂದೇಶ->data.source_mid = mid;
	}

	/* Handle properties */
	ಯದಿ(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->in_packet, &properties);
		ಯದಿ(rc){
			db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
			ಪ್ರತ್ಯಾಗಮ rc;
		}

		rc = property__process_publish(base_ಸಂದೇಶ, &properties, &topic_alias, &message_expiry_interval, context->bridge);
		ಯದಿ(rc){
			mosquitto_property_free_all(&properties);
			db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	ಯದಿ(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}ಅನ್ಯಥಾ ಯದಿ(topic_alias > 0){
		ಯದಿ(base_ಸಂದೇಶ->data.topic){
			rc = alias__add_r2l(context, base_ಸಂದೇಶ->data.topic, (uint16_t)topic_alias);
			ಯದಿ(rc){
				db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}ಅನ್ಯಥಾ{
			rc = alias__find_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &base_ಸಂದೇಶ->data.topic);
			ಯದಿ(rc){
				db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
				log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
			}
		}
	}

#ifdef WITH_BRIDGE
	rc = bridge__remap_topic_in(context, &base_ಸಂದೇಶ->data.topic);
	ಯದಿ(rc){
		db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

#endif
	ಯದಿ(mosquitto_pub_topic_check(base_ಸಂದೇಶ->data.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
	}

	base_ಸಂದೇಶ->data.payloadlen = context->in_packet.remaining_length - context->in_packet.pos;
	metrics__int_inc(mosq_counter_pub_bytes_received, base_ಸಂದೇಶ->data.payloadlen);
	ಯದಿ(context->listener && context->listener->mount_point){
		len = strlen(context->listener->mount_point) + strlen(base_ಸಂದೇಶ->data.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		ಯದಿ(!topic_mount){
			db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_point, base_ಸಂದೇಶ->data.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(base_ಸಂದೇಶ->data.topic);
		base_ಸಂದೇಶ->data.topic = topic_mount;
	}

	ಯದಿ(base_ಸಂದೇಶ->data.payloadlen){
		ಯದಿ(db.config->message_size_limit && base_ಸಂದೇಶ->data.payloadlen > db.config->message_size_limit){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_ಸಂದೇಶ->data.qos, base_ಸಂದೇಶ->data.retain, base_ಸಂದೇಶ->data.source_mid, base_ಸಂದೇಶ->data.topic, (long)base_ಸಂದೇಶ->data.payloadlen);
			ಪ್ರತ್ಯಾಗಮ process_bad_message(context, base_ಸಂದೇಶ, MQTT_RC_PACKET_TOO_LARGE);
		}
		base_ಸಂದೇಶ->data.payload = mosquitto_malloc(base_ಸಂದೇಶ->data.payloadlen+1);
		ಯದಿ(base_ಸಂದೇಶ->data.payload == ರಿಕ್ತ){
			db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_ಸಂದೇಶ->data.payload)[base_ಸಂದೇಶ->data.payloadlen] = 0;

		ಯದಿ(packet__read_bytes(&context->in_packet, base_ಸಂದೇಶ->data.payload, base_ಸಂದೇಶ->data.payloadlen)){
			db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			base_ಸಂದೇಶ->data.topic, base_ಸಂದೇಶ->data.payloadlen, base_ಸಂದೇಶ->data.payload,
			base_ಸಂದೇಶ->data.qos, base_ಸಂದೇಶ->data.retain, base_ಸಂದೇಶ->data.properties,
			MOSQ_ACL_WRITE);
	ಯದಿ(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, base_ಸಂದೇಶ->data.qos, base_ಸಂದೇಶ->data.retain, base_ಸಂದೇಶ->data.source_mid, base_ಸಂದೇಶ->data.topic,
				(long)base_ಸಂದೇಶ->data.payloadlen);
		ಪ್ರತ್ಯಾಗಮ process_bad_message(context, base_ಸಂದೇಶ, MQTT_RC_NOT_AUTHORIZED);
	}ಅನ್ಯಥಾ ಯದಿ(rc != MOSQ_ERR_SUCCESS){
		db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	log__printf(ರಿಕ್ತ, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_ಸಂದೇಶ->data.qos, base_ಸಂದೇಶ->data.retain, base_ಸಂದೇಶ->data.source_mid, base_ಸಂದೇಶ->data.topic, (long)base_ಸಂದೇಶ->data.payloadlen);

	ಯದಿ(!strncmp(base_ಸಂದೇಶ->data.topic, "$CONTROL/", 9)){
#ifdef WITH_CONTROL
		rc = control__process(context, base_ಸಂದೇಶ);
		db_ಸಂದೇಶ_store_free(base_ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ rc;
#ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ process_bad_message(context, base_ಸಂದೇಶ, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#endif
	}

	ಪ್ರತ್ಯಾಗಮ handle__accepted_publish(context, base_ಸಂದೇಶ, mid, dup, &message_expiry_interval);
}
