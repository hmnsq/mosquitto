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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "alias_mosq.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "packet_mosq.h"
#ଅନ୍ତର୍ଭାଵ "property_mosq.h"
#ଅନ୍ତର୍ଭାଵ "read_handle.h"
#ଅନ୍ତର୍ଭାଵ "send_mosq.h"
#ଅନ୍ତର୍ଭାଵ "sys_tree.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା process_bad_message(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ, uint8_t reason_code)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_UNKNOWN;
	ଯଦି(base_ସନ୍ଦେଶ){
		ବାଛ(base_ସନ୍ଦେଶ->data.qos){
			ସ୍ଥିତି 0:
				rc = MOSQ_ERR_SUCCESS;
				ଭାଙ୍ଗ;
			ସ୍ଥିତି 1:
				ଯଦି(context){
					rc = send__puback(context, base_ସନ୍ଦେଶ->data.source_mid, reason_code, ଖାଲି);
				}ଅନ୍ୟଥା{
					rc = MOSQ_ERR_SUCCESS;
				}
				ଭାଙ୍ଗ;
			ସ୍ଥିତି 2:
				ଯଦି(context){
					rc = send__pubrec(context, base_ସନ୍ଦେଶ->data.source_mid, reason_code, ଖାଲି);
				}ଅନ୍ୟଥା{
					rc = MOSQ_ERR_SUCCESS;
				}
				ଭାଙ୍ଗ;
		}
		db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
	}
	ଯଦି(context && db.config->max_queued_messages > 0 && context->out_packet_ଗଣନା >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	ଫେରନ୍ତୁ rc;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା handle__accepted_publish(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ, uint16_t mid, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dup, uint32_t *message_expiry_interval)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc2;
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *stored = ଖାଲି;
	ସଂରଚନା mosquitto_client_ସନ୍ଦେଶ *cmsg_stored = ଖାଲି;

	{
		rc = plugin__handle_message_in(context, &base_ସନ୍ଦେଶ->data);
		ଯଦି(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(ଖାଲି, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, base_ସନ୍ଦେଶ->data.qos, base_ସନ୍ଦେଶ->data.retain, base_ସନ୍ଦେଶ->data.source_mid, base_ସନ୍ଦେଶ->data.topic,
					(long)base_ସନ୍ଦେଶ->data.payloadlen);

			ଫେରନ୍ତୁ process_bad_message(context, base_ସନ୍ଦେଶ, MQTT_RC_NOT_AUTHORIZED);
		}ଅନ୍ୟଥା ଯଦି(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(ଖାଲି, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			ଫେରନ୍ତୁ process_bad_message(context, base_ସନ୍ଦେଶ, MQTT_RC_QUOTA_EXCEEDED);
		}ଅନ୍ୟଥା ଯଦି(rc != MOSQ_ERR_SUCCESS){
			db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
			ଫେରନ୍ତୁ rc;
		}
	}

	ଯଦି(base_ସନ୍ଦେଶ->data.qos > 0){
		db__message_store_find(context, base_ସନ୍ଦେଶ->data.source_mid, &cmsg_stored);
	}

	ଯଦି(cmsg_stored && base_ସନ୍ଦେଶ->data.source_mid != 0 &&
			(cmsg_stored->base_ସନ୍ଦେଶ->data.qos != base_ସନ୍ଦେଶ->data.qos
			|| cmsg_stored->base_ସନ୍ଦେଶ->data.payloadlen != base_ସନ୍ଦେଶ->data.payloadlen
			|| strcmp(cmsg_stored->base_ସନ୍ଦେଶ->data.topic, base_ସନ୍ଦେଶ->data.topic)
			|| (base_ସନ୍ଦେଶ->data.payloadlen > 0 && memcmp(cmsg_stored->base_ସନ୍ଦେଶ->data.payload, base_ସନ୍ଦେଶ->data.payload, base_ସନ୍ଦେଶ->data.payloadlen)))){

		log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", base_ସନ୍ଦେଶ->data.source_mid, context->id);
		db__message_remove_incoming(context, base_ସନ୍ଦେଶ->data.source_mid);
		cmsg_stored = ଖାଲି;
	}

	ଯଦି(!cmsg_stored){
		ଯଦି(base_ସନ୍ଦେଶ->data.qos > 0 && context->msgs_in.inflight_quota == 0){
			log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#ଯଦି 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
			ଫେରନ୍ତୁ MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED;
#endif
		}

		ଯଦି(base_ସନ୍ଦେଶ->data.qos == 0
				|| db__ready_for_flight(context, mosq_md_in, base_ସନ୍ଦେଶ->data.qos)
				){

			dup = 0;
			rc = db__message_store(context, base_ସନ୍ଦେଶ, message_expiry_interval, mosq_mo_client);
			ଯଦି(rc){
				ଫେରନ୍ତୁ rc;
			}
		}ଅନ୍ୟଥା{
			/* Client isn't allowed any more incoming messages, so fail early */
			ଫେରନ୍ତୁ process_bad_message(context, base_ସନ୍ଦେଶ, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = base_ସନ୍ଦେଶ;
		base_ସନ୍ଦେଶ = ଖାଲି;
		dup = 0;
	}ଅନ୍ୟଥା{
		db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
		base_ସନ୍ଦେଶ = ଖାଲି;
		stored = cmsg_stored->base_ସନ୍ଦେଶ;
		cmsg_stored->data.dup++;
		dup = cmsg_stored->data.dup;
	}

	ବାଛ(stored->data.qos){
		ସ୍ଥିତି 0:
			rc2 = sub__messages_queue(context->id, stored->data.qos, &stored);
			ଯଦି(rc2 > 0){
				rc = rc2;
			}
			ଭାଙ୍ଗ;
		ସ୍ଥିତି 1:
			util__decrement_receive_quota(context);
			rc2 = sub__messages_queue(context->id, stored->data.qos, &stored);
			/* stored may now be free, so don't refer to it */
			ଯଦି(rc2 == MOSQ_ERR_SUCCESS || context->protocol != mosq_p_mqtt5){
				rc2 = send__puback(context, mid, 0, ଖାଲି);
				ଯଦି(rc2){
					rc = rc2;
				}
			}ଅନ୍ୟଥା ଯଦି(rc2 == MOSQ_ERR_NO_SUBSCRIBERS){
				rc2 = send__puback(context, mid, MQTT_RC_NO_MATCHING_SUBSCRIBERS, ଖାଲି);
				ଯଦି(rc2){
					rc = rc2;
				}
			}ଅନ୍ୟଥା{
				rc = rc2;
			}
			ଭାଙ୍ଗ;
		ସ୍ଥିତି 2:
			{
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା res;
				ଯଦି(dup == 0){
					res = db_message_ଭର୍ତ୍ତି_incoming(context, 0, stored, true);
				}ଅନ୍ୟଥା{
					res = 0;
				}

				/* db__message_insert() returns 2 to indicate dropped message
				 * due to queue. This isn't an error so don't disconnect them. */
				/* FIXME - this is no longer necessary due to failing early above */
				ଯଦି(!res){
					ଯଦି(dup == 0 || dup == 1){
						rc2 = send__pubrec(context, stored->data.source_mid, 0, ଖାଲି);
						ଯଦି(rc2){
							rc = rc2;
						}
					}ଅନ୍ୟଥା{
						log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH with dup = %d.", context->id, dup);
						ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
					}
				}ଅନ୍ୟଥା{
					rc = res;
				}
				ଭାଙ୍ଗ;
			}
	}

	db__message_write_queued_in(context);
	ଫେରନ୍ତୁ rc;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା handle__publish(ସଂରଚନା mosquitto *context)
{
	uint8_t dup;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
	uint8_t header = context->in_packet.command;
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ;
	size_t len;
	uint16_t slen;
	ଅକ୍ଷର *topic_mount;
	mosquitto_property *properties = ଖାଲି;
	uint32_t message_expiry_interval = ସନ୍ଦେଶ_EXPIRY_INFINITE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା topic_alias = -1;
	uint16_t mid = 0;

	ଯଦି(context->state != mosq_cs_active){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	base_ସନ୍ଦେଶ = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ));
	ଯଦି(base_ସନ୍ଦେଶ == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	dup = (header & 0x08)>>3;
	base_ସନ୍ଦେଶ->data.qos = (header & 0x06)>>1;
	ଯଦି(dup == 1 && base_ସନ୍ଦେଶ->data.qos == 0){
		log__printf(ଖାଲି, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
	}
	ଯଦି(base_ସନ୍ଦେଶ->data.qos == 3){
		log__printf(ଖାଲି, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
	}
	ଯଦି(base_ସନ୍ଦେଶ->data.qos > context->max_qos){
		log__printf(ଖାଲି, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	base_ସନ୍ଦେଶ->data.retain = (header & 0x01);

	ଯଦି(base_ସନ୍ଦେଶ->data.retain && db.config->retain_available == false){
		db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	ଯଦି(packet__read_string(&context->in_packet, &base_ସନ୍ଦେଶ->data.topic, &slen)){
		db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
	}
	ଯଦି(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
	}

	ଯଦି(base_ସନ୍ଦେଶ->data.qos > 0){
		ଯଦି(packet__read_uint16(&context->in_packet, &mid)){
			db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
			ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
		}
		ଯଦି(mid == 0){
			db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
			log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		base_ସନ୍ଦେଶ->data.source_mid = mid;
	}

	/* Handle properties */
	ଯଦି(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->in_packet, &properties);
		ଯଦି(rc){
			db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
			ଫେରନ୍ତୁ rc;
		}

		rc = property__process_publish(base_ସନ୍ଦେଶ, &properties, &topic_alias, &message_expiry_interval, context->bridge);
		ଯଦି(rc){
			mosquitto_property_free_all(&properties);
			db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
			ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	ଯଦି(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}ଅନ୍ୟଥା ଯଦି(topic_alias > 0){
		ଯଦି(base_ସନ୍ଦେଶ->data.topic){
			rc = alias__add_r2l(context, base_ସନ୍ଦେଶ->data.topic, (uint16_t)topic_alias);
			ଯଦି(rc){
				db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
				ଫେରନ୍ତୁ rc;
			}
		}ଅନ୍ୟଥା{
			rc = alias__find_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &base_ସନ୍ଦେଶ->data.topic);
			ଯଦି(rc){
				db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
				log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
			}
		}
	}

#ifdef WITH_BRIDGE
	rc = bridge__remap_topic_in(context, &base_ସନ୍ଦେଶ->data.topic);
	ଯଦି(rc){
		db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ rc;
	}

#endif
	ଯଦି(mosquitto_pub_topic_check(base_ସନ୍ଦେଶ->data.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
	}

	base_ସନ୍ଦେଶ->data.payloadlen = context->in_packet.remaining_length - context->in_packet.pos;
	metrics__int_inc(mosq_counter_pub_bytes_received, base_ସନ୍ଦେଶ->data.payloadlen);
	ଯଦି(context->listener && context->listener->mount_point){
		len = strlen(context->listener->mount_point) + strlen(base_ସନ୍ଦେଶ->data.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		ଯଦି(!topic_mount){
			db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_point, base_ସନ୍ଦେଶ->data.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(base_ସନ୍ଦେଶ->data.topic);
		base_ସନ୍ଦେଶ->data.topic = topic_mount;
	}

	ଯଦି(base_ସନ୍ଦେଶ->data.payloadlen){
		ଯଦି(db.config->message_size_limit && base_ସନ୍ଦେଶ->data.payloadlen > db.config->message_size_limit){
			log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_ସନ୍ଦେଶ->data.qos, base_ସନ୍ଦେଶ->data.retain, base_ସନ୍ଦେଶ->data.source_mid, base_ସନ୍ଦେଶ->data.topic, (long)base_ସନ୍ଦେଶ->data.payloadlen);
			ଫେରନ୍ତୁ process_bad_message(context, base_ସନ୍ଦେଶ, MQTT_RC_PACKET_TOO_LARGE);
		}
		base_ସନ୍ଦେଶ->data.payload = mosquitto_malloc(base_ସନ୍ଦେଶ->data.payloadlen+1);
		ଯଦି(base_ସନ୍ଦେଶ->data.payload == ଖାଲି){
			db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_ସନ୍ଦେଶ->data.payload)[base_ସନ୍ଦେଶ->data.payloadlen] = 0;

		ଯଦି(packet__read_bytes(&context->in_packet, base_ସନ୍ଦେଶ->data.payload, base_ସନ୍ଦେଶ->data.payloadlen)){
			db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
			ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			base_ସନ୍ଦେଶ->data.topic, base_ସନ୍ଦେଶ->data.payloadlen, base_ସନ୍ଦେଶ->data.payload,
			base_ସନ୍ଦେଶ->data.qos, base_ସନ୍ଦେଶ->data.retain, base_ସନ୍ଦେଶ->data.properties,
			MOSQ_ACL_WRITE);
	ଯଦି(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(ଖାଲି, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, base_ସନ୍ଦେଶ->data.qos, base_ସନ୍ଦେଶ->data.retain, base_ସନ୍ଦେଶ->data.source_mid, base_ସନ୍ଦେଶ->data.topic,
				(long)base_ସନ୍ଦେଶ->data.payloadlen);
		ଫେରନ୍ତୁ process_bad_message(context, base_ସନ୍ଦେଶ, MQTT_RC_NOT_AUTHORIZED);
	}ଅନ୍ୟଥା ଯଦି(rc != MOSQ_ERR_SUCCESS){
		db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ rc;
	}

	log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_ସନ୍ଦେଶ->data.qos, base_ସନ୍ଦେଶ->data.retain, base_ସନ୍ଦେଶ->data.source_mid, base_ସନ୍ଦେଶ->data.topic, (long)base_ସନ୍ଦେଶ->data.payloadlen);

	ଯଦି(!strncmp(base_ସନ୍ଦେଶ->data.topic, "$CONTROL/", 9)){
#ifdef WITH_CONTROL
		rc = control__process(context, base_ସନ୍ଦେଶ);
		db_ସନ୍ଦେଶ_store_free(base_ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ rc;
#ଅନ୍ୟଥା
		ଫେରନ୍ତୁ process_bad_message(context, base_ସନ୍ଦେଶ, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#endif
	}

	ଫେରନ୍ତୁ handle__accepted_publish(context, base_ସନ୍ଦେଶ, mid, dup, &message_expiry_interval);
}
