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
#અન્તર્ભાવ "alias_mosq.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "packet_mosq.h"
#અન્તર્ભાવ "property_mosq.h"
#અન્તર્ભાવ "read_handle.h"
#અન્તર્ભાવ "send_mosq.h"
#અન્તર્ભાવ "sys_tree.h"
#અન્તર્ભાવ "util_mosq.h"


સ્થિર પૂર્ણાંક process_bad_message(સંરચના mosquitto *context, સંરચના mosquitto_base_સંદેશ *base_સંદેશ, uint8_t reason_code)
{
	પૂર્ણાંક rc = MOSQ_ERR_UNKNOWN;
	જો(base_સંદેશ){
		પસંદગી(base_સંદેશ->data.qos){
			સ્થિતિ 0:
				rc = MOSQ_ERR_SUCCESS;
				તોડો;
			સ્થિતિ 1:
				જો(context){
					rc = send__puback(context, base_સંદેશ->data.source_mid, reason_code, ખાલી);
				}નહીંતર{
					rc = MOSQ_ERR_SUCCESS;
				}
				તોડો;
			સ્થિતિ 2:
				જો(context){
					rc = send__pubrec(context, base_સંદેશ->data.source_mid, reason_code, ખાલી);
				}નહીંતર{
					rc = MOSQ_ERR_SUCCESS;
				}
				તોડો;
		}
		db_સંદેશ_store_free(base_સંદેશ);
	}
	જો(context && db.config->max_queued_messages > 0 && context->out_packet_ગણતરી >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	પાછા rc;
}


પૂર્ણાંક handle__accepted_publish(સંરચના mosquitto *context, સંરચના mosquitto_base_સંદેશ *base_સંદેશ, uint16_t mid, પૂર્ણાંક dup, uint32_t *message_expiry_interval)
{
	પૂર્ણાંક rc;
	પૂર્ણાંક rc2;
	સંરચના mosquitto_base_સંદેશ *stored = ખાલી;
	સંરચના mosquitto_client_સંદેશ *cmsg_stored = ખાલી;

	{
		rc = plugin__handle_message_in(context, &base_સંદેશ->data);
		જો(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(ખાલી, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, base_સંદેશ->data.qos, base_સંદેશ->data.retain, base_સંદેશ->data.source_mid, base_સંદેશ->data.topic,
					(long)base_સંદેશ->data.payloadlen);

			પાછા process_bad_message(context, base_સંદેશ, MQTT_RC_NOT_AUTHORIZED);
		}નહીંતર જો(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(ખાલી, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			પાછા process_bad_message(context, base_સંદેશ, MQTT_RC_QUOTA_EXCEEDED);
		}નહીંતર જો(rc != MOSQ_ERR_SUCCESS){
			db_સંદેશ_store_free(base_સંદેશ);
			પાછા rc;
		}
	}

	જો(base_સંદેશ->data.qos > 0){
		db__message_store_find(context, base_સંદેશ->data.source_mid, &cmsg_stored);
	}

	જો(cmsg_stored && base_સંદેશ->data.source_mid != 0 &&
			(cmsg_stored->base_સંદેશ->data.qos != base_સંદેશ->data.qos
			|| cmsg_stored->base_સંદેશ->data.payloadlen != base_સંદેશ->data.payloadlen
			|| strcmp(cmsg_stored->base_સંદેશ->data.topic, base_સંદેશ->data.topic)
			|| (base_સંદેશ->data.payloadlen > 0 && memcmp(cmsg_stored->base_સંદેશ->data.payload, base_સંદેશ->data.payload, base_સંદેશ->data.payloadlen)))){

		log__printf(ખાલી, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", base_સંદેશ->data.source_mid, context->id);
		db__message_remove_incoming(context, base_સંદેશ->data.source_mid);
		cmsg_stored = ખાલી;
	}

	જો(!cmsg_stored){
		જો(base_સંદેશ->data.qos > 0 && context->msgs_in.inflight_quota == 0){
			log__printf(ખાલી, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#જો 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_સંદેશ_store_free(base_સંદેશ);
			પાછા MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED;
#endif
		}

		જો(base_સંદેશ->data.qos == 0
				|| db__ready_for_flight(context, mosq_md_in, base_સંદેશ->data.qos)
				){

			dup = 0;
			rc = db__message_store(context, base_સંદેશ, message_expiry_interval, mosq_mo_client);
			જો(rc){
				પાછા rc;
			}
		}નહીંતર{
			/* Client isn't allowed any more incoming messages, so fail early */
			પાછા process_bad_message(context, base_સંદેશ, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = base_સંદેશ;
		base_સંદેશ = ખાલી;
		dup = 0;
	}નહીંતર{
		db_સંદેશ_store_free(base_સંદેશ);
		base_સંદેશ = ખાલી;
		stored = cmsg_stored->base_સંદેશ;
		cmsg_stored->data.dup++;
		dup = cmsg_stored->data.dup;
	}

	પસંદગી(stored->data.qos){
		સ્થિતિ 0:
			rc2 = sub__messages_queue(context->id, stored->data.qos, &stored);
			જો(rc2 > 0){
				rc = rc2;
			}
			તોડો;
		સ્થિતિ 1:
			util__decrement_receive_quota(context);
			rc2 = sub__messages_queue(context->id, stored->data.qos, &stored);
			/* stored may now be free, so don't refer to it */
			જો(rc2 == MOSQ_ERR_SUCCESS || context->protocol != mosq_p_mqtt5){
				rc2 = send__puback(context, mid, 0, ખાલી);
				જો(rc2){
					rc = rc2;
				}
			}નહીંતર જો(rc2 == MOSQ_ERR_NO_SUBSCRIBERS){
				rc2 = send__puback(context, mid, MQTT_RC_NO_MATCHING_SUBSCRIBERS, ખાલી);
				જો(rc2){
					rc = rc2;
				}
			}નહીંતર{
				rc = rc2;
			}
			તોડો;
		સ્થિતિ 2:
			{
				પૂર્ણાંક res;
				જો(dup == 0){
					res = db_message_ઉમેરો_incoming(context, 0, stored, true);
				}નહીંતર{
					res = 0;
				}

				/* db__message_insert() returns 2 to indicate dropped message
				 * due to queue. This isn't an error so don't disconnect them. */
				/* FIXME - this is no longer necessary due to failing early above */
				જો(!res){
					જો(dup == 0 || dup == 1){
						rc2 = send__pubrec(context, stored->data.source_mid, 0, ખાલી);
						જો(rc2){
							rc = rc2;
						}
					}નહીંતર{
						log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH with dup = %d.", context->id, dup);
						પાછા MOSQ_ERR_PROTOCOL;
					}
				}નહીંતર{
					rc = res;
				}
				તોડો;
			}
	}

	db__message_write_queued_in(context);
	પાછા rc;
}


પૂર્ણાંક handle__publish(સંરચના mosquitto *context)
{
	uint8_t dup;
	પૂર્ણાંક rc = 0;
	uint8_t header = context->in_packet.command;
	સંરચના mosquitto_base_સંદેશ *base_સંદેશ;
	size_t len;
	uint16_t slen;
	અક્ષર *topic_mount;
	mosquitto_property *properties = ખાલી;
	uint32_t message_expiry_interval = સંદેશ_EXPIRY_INFINITE;
	પૂર્ણાંક topic_alias = -1;
	uint16_t mid = 0;

	જો(context->state != mosq_cs_active){
		log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		પાછા MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	base_સંદેશ = mosquitto_calloc(1, કદ(સંરચના mosquitto_base_સંદેશ));
	જો(base_સંદેશ == ખાલી){
		પાછા MOSQ_ERR_NOMEM;
	}

	dup = (header & 0x08)>>3;
	base_સંદેશ->data.qos = (header & 0x06)>>1;
	જો(dup == 1 && base_સંદેશ->data.qos == 0){
		log__printf(ખાલી, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_સંદેશ_store_free(base_સંદેશ);
		પાછા MOSQ_ERR_MALFORMED_PACKET;
	}
	જો(base_સંદેશ->data.qos == 3){
		log__printf(ખાલી, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_સંદેશ_store_free(base_સંદેશ);
		પાછા MOSQ_ERR_MALFORMED_PACKET;
	}
	જો(base_સંદેશ->data.qos > context->max_qos){
		log__printf(ખાલી, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_સંદેશ_store_free(base_સંદેશ);
		પાછા MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	base_સંદેશ->data.retain = (header & 0x01);

	જો(base_સંદેશ->data.retain && db.config->retain_available == false){
		db_સંદેશ_store_free(base_સંદેશ);
		પાછા MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	જો(packet__read_string(&context->in_packet, &base_સંદેશ->data.topic, &slen)){
		db_સંદેશ_store_free(base_સંદેશ);
		પાછા MOSQ_ERR_MALFORMED_PACKET;
	}
	જો(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_સંદેશ_store_free(base_સંદેશ);
		પાછા MOSQ_ERR_MALFORMED_PACKET;
	}

	જો(base_સંદેશ->data.qos > 0){
		જો(packet__read_uint16(&context->in_packet, &mid)){
			db_સંદેશ_store_free(base_સંદેશ);
			પાછા MOSQ_ERR_MALFORMED_PACKET;
		}
		જો(mid == 0){
			db_સંદેશ_store_free(base_સંદેશ);
			log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			પાછા MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		base_સંદેશ->data.source_mid = mid;
	}

	/* Handle properties */
	જો(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->in_packet, &properties);
		જો(rc){
			db_સંદેશ_store_free(base_સંદેશ);
			પાછા rc;
		}

		rc = property__process_publish(base_સંદેશ, &properties, &topic_alias, &message_expiry_interval, context->bridge);
		જો(rc){
			mosquitto_property_free_all(&properties);
			db_સંદેશ_store_free(base_સંદેશ);
			પાછા MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	જો(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_સંદેશ_store_free(base_સંદેશ);
		પાછા MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}નહીંતર જો(topic_alias > 0){
		જો(base_સંદેશ->data.topic){
			rc = alias__add_r2l(context, base_સંદેશ->data.topic, (uint16_t)topic_alias);
			જો(rc){
				db_સંદેશ_store_free(base_સંદેશ);
				પાછા rc;
			}
		}નહીંતર{
			rc = alias__find_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &base_સંદેશ->data.topic);
			જો(rc){
				db_સંદેશ_store_free(base_સંદેશ);
				log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				પાછા MOSQ_ERR_PROTOCOL;
			}
		}
	}

#ifdef WITH_BRIDGE
	rc = bridge__remap_topic_in(context, &base_સંદેશ->data.topic);
	જો(rc){
		db_સંદેશ_store_free(base_સંદેશ);
		પાછા rc;
	}

#endif
	જો(mosquitto_pub_topic_check(base_સંદેશ->data.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_સંદેશ_store_free(base_સંદેશ);
		પાછા MOSQ_ERR_MALFORMED_PACKET;
	}

	base_સંદેશ->data.payloadlen = context->in_packet.remaining_length - context->in_packet.pos;
	metrics__int_inc(mosq_counter_pub_bytes_received, base_સંદેશ->data.payloadlen);
	જો(context->listener && context->listener->mount_point){
		len = strlen(context->listener->mount_point) + strlen(base_સંદેશ->data.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		જો(!topic_mount){
			db_સંદેશ_store_free(base_સંદેશ);
			પાછા MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_point, base_સંદેશ->data.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(base_સંદેશ->data.topic);
		base_સંદેશ->data.topic = topic_mount;
	}

	જો(base_સંદેશ->data.payloadlen){
		જો(db.config->message_size_limit && base_સંદેશ->data.payloadlen > db.config->message_size_limit){
			log__printf(ખાલી, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_સંદેશ->data.qos, base_સંદેશ->data.retain, base_સંદેશ->data.source_mid, base_સંદેશ->data.topic, (long)base_સંદેશ->data.payloadlen);
			પાછા process_bad_message(context, base_સંદેશ, MQTT_RC_PACKET_TOO_LARGE);
		}
		base_સંદેશ->data.payload = mosquitto_malloc(base_સંદેશ->data.payloadlen+1);
		જો(base_સંદેશ->data.payload == ખાલી){
			db_સંદેશ_store_free(base_સંદેશ);
			પાછા MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_સંદેશ->data.payload)[base_સંદેશ->data.payloadlen] = 0;

		જો(packet__read_bytes(&context->in_packet, base_સંદેશ->data.payload, base_સંદેશ->data.payloadlen)){
			db_સંદેશ_store_free(base_સંદેશ);
			પાછા MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			base_સંદેશ->data.topic, base_સંદેશ->data.payloadlen, base_સંદેશ->data.payload,
			base_સંદેશ->data.qos, base_સંદેશ->data.retain, base_સંદેશ->data.properties,
			MOSQ_ACL_WRITE);
	જો(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(ખાલી, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, base_સંદેશ->data.qos, base_સંદેશ->data.retain, base_સંદેશ->data.source_mid, base_સંદેશ->data.topic,
				(long)base_સંદેશ->data.payloadlen);
		પાછા process_bad_message(context, base_સંદેશ, MQTT_RC_NOT_AUTHORIZED);
	}નહીંતર જો(rc != MOSQ_ERR_SUCCESS){
		db_સંદેશ_store_free(base_સંદેશ);
		પાછા rc;
	}

	log__printf(ખાલી, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_સંદેશ->data.qos, base_સંદેશ->data.retain, base_સંદેશ->data.source_mid, base_સંદેશ->data.topic, (long)base_સંદેશ->data.payloadlen);

	જો(!strncmp(base_સંદેશ->data.topic, "$CONTROL/", 9)){
#ifdef WITH_CONTROL
		rc = control__process(context, base_સંદેશ);
		db_સંદેશ_store_free(base_સંદેશ);
		પાછા rc;
#નહીંતર
		પાછા process_bad_message(context, base_સંદેશ, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#endif
	}

	પાછા handle__accepted_publish(context, base_સંદેશ, mid, dup, &message_expiry_interval);
}
