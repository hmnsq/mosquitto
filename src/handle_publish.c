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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "alias_mosq.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "packet_mosq.h"
#অন্তর্ভাব "property_mosq.h"
#অন্তর্ভাব "read_handle.h"
#অন্তর্ভাব "send_mosq.h"
#অন্তর্ভাব "sys_tree.h"
#অন্তর্ভাব "util_mosq.h"


স্থির পূর্ণাঙ্ক process_bad_message(সংরচনা mosquitto *context, সংরচনা mosquitto_base_বার্তা *base_বার্তা, uint8_t reason_code)
{
	পূর্ণাঙ্ক rc = MOSQ_ERR_UNKNOWN;
	যদি(base_বার্তা){
		বিকল্প(base_বার্তা->data.qos){
			পক্ষ 0:
				rc = MOSQ_ERR_SUCCESS;
				ভঙ্গ;
			পক্ষ 1:
				যদি(context){
					rc = send__puback(context, base_বার্তা->data.source_mid, reason_code, রিক্ত);
				}অন্যথা{
					rc = MOSQ_ERR_SUCCESS;
				}
				ভঙ্গ;
			পক্ষ 2:
				যদি(context){
					rc = send__pubrec(context, base_বার্তা->data.source_mid, reason_code, রিক্ত);
				}অন্যথা{
					rc = MOSQ_ERR_SUCCESS;
				}
				ভঙ্গ;
		}
		db_বার্তা_store_free(base_বার্তা);
	}
	যদি(context && db.config->max_queued_messages > 0 && context->out_packet_গুনতি >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	প্রত্যাগম rc;
}


পূর্ণাঙ্ক handle__accepted_publish(সংরচনা mosquitto *context, সংরচনা mosquitto_base_বার্তা *base_বার্তা, uint16_t mid, পূর্ণাঙ্ক dup, uint32_t *message_expiry_interval)
{
	পূর্ণাঙ্ক rc;
	পূর্ণাঙ্ক rc2;
	সংরচনা mosquitto_base_বার্তা *stored = রিক্ত;
	সংরচনা mosquitto_client_বার্তা *cmsg_stored = রিক্ত;

	{
		rc = plugin__handle_message_in(context, &base_বার্তা->data);
		যদি(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(রিক্ত, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, base_বার্তা->data.qos, base_বার্তা->data.retain, base_বার্তা->data.source_mid, base_বার্তা->data.topic,
					(long)base_বার্তা->data.payloadlen);

			প্রত্যাগম process_bad_message(context, base_বার্তা, MQTT_RC_NOT_AUTHORIZED);
		}অন্যথা যদি(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(রিক্ত, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			প্রত্যাগম process_bad_message(context, base_বার্তা, MQTT_RC_QUOTA_EXCEEDED);
		}অন্যথা যদি(rc != MOSQ_ERR_SUCCESS){
			db_বার্তা_store_free(base_বার্তা);
			প্রত্যাগম rc;
		}
	}

	যদি(base_বার্তা->data.qos > 0){
		db__message_store_find(context, base_বার্তা->data.source_mid, &cmsg_stored);
	}

	যদি(cmsg_stored && base_বার্তা->data.source_mid != 0 &&
			(cmsg_stored->base_বার্তা->data.qos != base_বার্তা->data.qos
			|| cmsg_stored->base_বার্তা->data.payloadlen != base_বার্তা->data.payloadlen
			|| strcmp(cmsg_stored->base_বার্তা->data.topic, base_বার্তা->data.topic)
			|| (base_বার্তা->data.payloadlen > 0 && memcmp(cmsg_stored->base_বার্তা->data.payload, base_বার্তা->data.payload, base_বার্তা->data.payloadlen)))){

		log__printf(রিক্ত, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", base_বার্তা->data.source_mid, context->id);
		db__message_remove_incoming(context, base_বার্তা->data.source_mid);
		cmsg_stored = রিক্ত;
	}

	যদি(!cmsg_stored){
		যদি(base_বার্তা->data.qos > 0 && context->msgs_in.inflight_quota == 0){
			log__printf(রিক্ত, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#যদি 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_বার্তা_store_free(base_বার্তা);
			প্রত্যাগম MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED;
#endif
		}

		যদি(base_বার্তা->data.qos == 0
				|| db__ready_for_flight(context, mosq_md_in, base_বার্তা->data.qos)
				){

			dup = 0;
			rc = db__message_store(context, base_বার্তা, message_expiry_interval, mosq_mo_client);
			যদি(rc){
				প্রত্যাগম rc;
			}
		}অন্যথা{
			/* Client isn't allowed any more incoming messages, so fail early */
			প্রত্যাগম process_bad_message(context, base_বার্তা, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = base_বার্তা;
		base_বার্তা = রিক্ত;
		dup = 0;
	}অন্যথা{
		db_বার্তা_store_free(base_বার্তা);
		base_বার্তা = রিক্ত;
		stored = cmsg_stored->base_বার্তা;
		cmsg_stored->data.dup++;
		dup = cmsg_stored->data.dup;
	}

	বিকল্প(stored->data.qos){
		পক্ষ 0:
			rc2 = sub__messages_queue(context->id, stored->data.qos, &stored);
			যদি(rc2 > 0){
				rc = rc2;
			}
			ভঙ্গ;
		পক্ষ 1:
			util__decrement_receive_quota(context);
			rc2 = sub__messages_queue(context->id, stored->data.qos, &stored);
			/* stored may now be free, so don't refer to it */
			যদি(rc2 == MOSQ_ERR_SUCCESS || context->protocol != mosq_p_mqtt5){
				rc2 = send__puback(context, mid, 0, রিক্ত);
				যদি(rc2){
					rc = rc2;
				}
			}অন্যথা যদি(rc2 == MOSQ_ERR_NO_SUBSCRIBERS){
				rc2 = send__puback(context, mid, MQTT_RC_NO_MATCHING_SUBSCRIBERS, রিক্ত);
				যদি(rc2){
					rc = rc2;
				}
			}অন্যথা{
				rc = rc2;
			}
			ভঙ্গ;
		পক্ষ 2:
			{
				পূর্ণাঙ্ক res;
				যদি(dup == 0){
					res = db_message_ঢোকাও_incoming(context, 0, stored, true);
				}অন্যথা{
					res = 0;
				}

				/* db__message_insert() returns 2 to indicate dropped message
				 * due to queue. This isn't an error so don't disconnect them. */
				/* FIXME - this is no longer necessary due to failing early above */
				যদি(!res){
					যদি(dup == 0 || dup == 1){
						rc2 = send__pubrec(context, stored->data.source_mid, 0, রিক্ত);
						যদি(rc2){
							rc = rc2;
						}
					}অন্যথা{
						log__printf(রিক্ত, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH with dup = %d.", context->id, dup);
						প্রত্যাগম MOSQ_ERR_PROTOCOL;
					}
				}অন্যথা{
					rc = res;
				}
				ভঙ্গ;
			}
	}

	db__message_write_queued_in(context);
	প্রত্যাগম rc;
}


পূর্ণাঙ্ক handle__publish(সংরচনা mosquitto *context)
{
	uint8_t dup;
	পূর্ণাঙ্ক rc = 0;
	uint8_t header = context->in_packet.command;
	সংরচনা mosquitto_base_বার্তা *base_বার্তা;
	size_t len;
	uint16_t slen;
	বর্ণ *topic_mount;
	mosquitto_property *properties = রিক্ত;
	uint32_t message_expiry_interval = বার্তা_EXPIRY_INFINITE;
	পূর্ণাঙ্ক topic_alias = -1;
	uint16_t mid = 0;

	যদি(context->state != mosq_cs_active){
		log__printf(রিক্ত, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		প্রত্যাগম MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	base_বার্তা = mosquitto_calloc(1, পরিমাণ(সংরচনা mosquitto_base_বার্তা));
	যদি(base_বার্তা == রিক্ত){
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}

	dup = (header & 0x08)>>3;
	base_বার্তা->data.qos = (header & 0x06)>>1;
	যদি(dup == 1 && base_বার্তা->data.qos == 0){
		log__printf(রিক্ত, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_বার্তা_store_free(base_বার্তা);
		প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
	}
	যদি(base_বার্তা->data.qos == 3){
		log__printf(রিক্ত, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_বার্তা_store_free(base_বার্তা);
		প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
	}
	যদি(base_বার্তা->data.qos > context->max_qos){
		log__printf(রিক্ত, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_বার্তা_store_free(base_বার্তা);
		প্রত্যাগম MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	base_বার্তা->data.retain = (header & 0x01);

	যদি(base_বার্তা->data.retain && db.config->retain_available == false){
		db_বার্তা_store_free(base_বার্তা);
		প্রত্যাগম MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	যদি(packet__read_string(&context->in_packet, &base_বার্তা->data.topic, &slen)){
		db_বার্তা_store_free(base_বার্তা);
		প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
	}
	যদি(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_বার্তা_store_free(base_বার্তা);
		প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
	}

	যদি(base_বার্তা->data.qos > 0){
		যদি(packet__read_uint16(&context->in_packet, &mid)){
			db_বার্তা_store_free(base_বার্তা);
			প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
		}
		যদি(mid == 0){
			db_বার্তা_store_free(base_বার্তা);
			log__printf(রিক্ত, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			প্রত্যাগম MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		base_বার্তা->data.source_mid = mid;
	}

	/* Handle properties */
	যদি(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->in_packet, &properties);
		যদি(rc){
			db_বার্তা_store_free(base_বার্তা);
			প্রত্যাগম rc;
		}

		rc = property__process_publish(base_বার্তা, &properties, &topic_alias, &message_expiry_interval, context->bridge);
		যদি(rc){
			mosquitto_property_free_all(&properties);
			db_বার্তা_store_free(base_বার্তা);
			প্রত্যাগম MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	যদি(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_বার্তা_store_free(base_বার্তা);
		প্রত্যাগম MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}অন্যথা যদি(topic_alias > 0){
		যদি(base_বার্তা->data.topic){
			rc = alias__add_r2l(context, base_বার্তা->data.topic, (uint16_t)topic_alias);
			যদি(rc){
				db_বার্তা_store_free(base_বার্তা);
				প্রত্যাগম rc;
			}
		}অন্যথা{
			rc = alias__find_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &base_বার্তা->data.topic);
			যদি(rc){
				db_বার্তা_store_free(base_বার্তা);
				log__printf(রিক্ত, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				প্রত্যাগম MOSQ_ERR_PROTOCOL;
			}
		}
	}

#ifdef WITH_BRIDGE
	rc = bridge__remap_topic_in(context, &base_বার্তা->data.topic);
	যদি(rc){
		db_বার্তা_store_free(base_বার্তা);
		প্রত্যাগম rc;
	}

#endif
	যদি(mosquitto_pub_topic_check(base_বার্তা->data.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_বার্তা_store_free(base_বার্তা);
		প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
	}

	base_বার্তা->data.payloadlen = context->in_packet.remaining_length - context->in_packet.pos;
	metrics__int_inc(mosq_counter_pub_bytes_received, base_বার্তা->data.payloadlen);
	যদি(context->listener && context->listener->mount_point){
		len = strlen(context->listener->mount_point) + strlen(base_বার্তা->data.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		যদি(!topic_mount){
			db_বার্তা_store_free(base_বার্তা);
			প্রত্যাগম MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_point, base_বার্তা->data.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(base_বার্তা->data.topic);
		base_বার্তা->data.topic = topic_mount;
	}

	যদি(base_বার্তা->data.payloadlen){
		যদি(db.config->message_size_limit && base_বার্তা->data.payloadlen > db.config->message_size_limit){
			log__printf(রিক্ত, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_বার্তা->data.qos, base_বার্তা->data.retain, base_বার্তা->data.source_mid, base_বার্তা->data.topic, (long)base_বার্তা->data.payloadlen);
			প্রত্যাগম process_bad_message(context, base_বার্তা, MQTT_RC_PACKET_TOO_LARGE);
		}
		base_বার্তা->data.payload = mosquitto_malloc(base_বার্তা->data.payloadlen+1);
		যদি(base_বার্তা->data.payload == রিক্ত){
			db_বার্তা_store_free(base_বার্তা);
			প্রত্যাগম MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_বার্তা->data.payload)[base_বার্তা->data.payloadlen] = 0;

		যদি(packet__read_bytes(&context->in_packet, base_বার্তা->data.payload, base_বার্তা->data.payloadlen)){
			db_বার্তা_store_free(base_বার্তা);
			প্রত্যাগম MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			base_বার্তা->data.topic, base_বার্তা->data.payloadlen, base_বার্তা->data.payload,
			base_বার্তা->data.qos, base_বার্তা->data.retain, base_বার্তা->data.properties,
			MOSQ_ACL_WRITE);
	যদি(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(রিক্ত, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, base_বার্তা->data.qos, base_বার্তা->data.retain, base_বার্তা->data.source_mid, base_বার্তা->data.topic,
				(long)base_বার্তা->data.payloadlen);
		প্রত্যাগম process_bad_message(context, base_বার্তা, MQTT_RC_NOT_AUTHORIZED);
	}অন্যথা যদি(rc != MOSQ_ERR_SUCCESS){
		db_বার্তা_store_free(base_বার্তা);
		প্রত্যাগম rc;
	}

	log__printf(রিক্ত, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_বার্তা->data.qos, base_বার্তা->data.retain, base_বার্তা->data.source_mid, base_বার্তা->data.topic, (long)base_বার্তা->data.payloadlen);

	যদি(!strncmp(base_বার্তা->data.topic, "$CONTROL/", 9)){
#ifdef WITH_CONTROL
		rc = control__process(context, base_বার্তা);
		db_বার্তা_store_free(base_বার্তা);
		প্রত্যাগম rc;
#অন্যথা
		প্রত্যাগম process_bad_message(context, base_বার্তা, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#endif
	}

	প্রত্যাগম handle__accepted_publish(context, base_বার্তা, mid, dup, &message_expiry_interval);
}
