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


স্থির পূর্ণসংখ্যা process_bad_message(কাঠামো mosquitto *context, কাঠামো mosquitto_base_বার্তা *base_বার্তা, uint8_t reason_code)
{
	পূর্ণসংখ্যা rc = MOSQ_ERR_UNKNOWN;
	যদি(base_বার্তা){
		নির্বাচন(base_বার্তা->তথ্য.qos){
			ক্ষেত্র 0:
				rc = MOSQ_ERR_SUCCESS;
				বিরতি;
			ক্ষেত্র 1:
				যদি(context){
					rc = send__puback(context, base_বার্তা->তথ্য.source_mid, reason_code, ফাঁকা);
				}অন্যথা{
					rc = MOSQ_ERR_SUCCESS;
				}
				বিরতি;
			ক্ষেত্র 2:
				যদি(context){
					rc = send__pubrec(context, base_বার্তা->তথ্য.source_mid, reason_code, ফাঁকা);
				}অন্যথা{
					rc = MOSQ_ERR_SUCCESS;
				}
				বিরতি;
		}
		db_বার্তা_store_free(base_বার্তা);
	}
	যদি(context && db.config->max_queued_messages > 0 && context->out_packet_গুনতি >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	ফেরত rc;
}


পূর্ণসংখ্যা handle__accepted_publish(কাঠামো mosquitto *context, কাঠামো mosquitto_base_বার্তা *base_বার্তা, uint16_t mid, পূর্ণসংখ্যা dup, uint32_t *message_expiry_interval)
{
	পূর্ণসংখ্যা rc;
	পূর্ণসংখ্যা rc2;
	কাঠামো mosquitto_base_বার্তা *stored = ফাঁকা;
	কাঠামো mosquitto_client_বার্তা *cmsg_stored = ফাঁকা;

	{
		rc = plugin__handle_message_in(context, &base_বার্তা->তথ্য);
		যদি(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(ফাঁকা, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, base_বার্তা->তথ্য.qos, base_বার্তা->তথ্য.retain, base_বার্তা->তথ্য.source_mid, base_বার্তা->তথ্য.topic,
					(long)base_বার্তা->তথ্য.payloadlen);

			ফেরত process_bad_message(context, base_বার্তা, MQTT_RC_NOT_AUTHORIZED);
		}অন্যথা যদি(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(ফাঁকা, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			ফেরত process_bad_message(context, base_বার্তা, MQTT_RC_QUOTA_EXCEEDED);
		}অন্যথা যদি(rc != MOSQ_ERR_SUCCESS){
			db_বার্তা_store_free(base_বার্তা);
			ফেরত rc;
		}
	}

	যদি(base_বার্তা->তথ্য.qos > 0){
		db__message_store_find(context, base_বার্তা->তথ্য.source_mid, &cmsg_stored);
	}

	যদি(cmsg_stored && base_বার্তা->তথ্য.source_mid != 0 &&
			(cmsg_stored->base_বার্তা->তথ্য.qos != base_বার্তা->তথ্য.qos
			|| cmsg_stored->base_বার্তা->তথ্য.payloadlen != base_বার্তা->তথ্য.payloadlen
			|| strcmp(cmsg_stored->base_বার্তা->তথ্য.topic, base_বার্তা->তথ্য.topic)
			|| (base_বার্তা->তথ্য.payloadlen > 0 && memcmp(cmsg_stored->base_বার্তা->তথ্য.payload, base_বার্তা->তথ্য.payload, base_বার্তা->তথ্য.payloadlen)))){

		log__printf(ফাঁকা, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", base_বার্তা->তথ্য.source_mid, context->id);
		db__message_remove_incoming(context, base_বার্তা->তথ্য.source_mid);
		cmsg_stored = ফাঁকা;
	}

	যদি(!cmsg_stored){
		যদি(base_বার্তা->তথ্য.qos > 0 && context->msgs_in.inflight_quota == 0){
			log__printf(ফাঁকা, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#যদি 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_বার্তা_store_free(base_বার্তা);
			ফেরত MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED;
#endif
		}

		যদি(base_বার্তা->তথ্য.qos == 0
				|| db__ready_for_flight(context, mosq_md_in, base_বার্তা->তথ্য.qos)
				){

			dup = 0;
			rc = db__message_store(context, base_বার্তা, message_expiry_interval, mosq_mo_client);
			যদি(rc){
				ফেরত rc;
			}
		}অন্যথা{
			/* Client isn't allowed any more incoming messages, so fail early */
			ফেরত process_bad_message(context, base_বার্তা, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = base_বার্তা;
		base_বার্তা = ফাঁকা;
		dup = 0;
	}অন্যথা{
		db_বার্তা_store_free(base_বার্তা);
		base_বার্তা = ফাঁকা;
		stored = cmsg_stored->base_বার্তা;
		cmsg_stored->তথ্য.dup++;
		dup = cmsg_stored->তথ্য.dup;
	}

	নির্বাচন(stored->তথ্য.qos){
		ক্ষেত্র 0:
			rc2 = sub__messages_queue(context->id, stored->তথ্য.qos, &stored);
			যদি(rc2 > 0){
				rc = rc2;
			}
			বিরতি;
		ক্ষেত্র 1:
			util__decrement_receive_quota(context);
			rc2 = sub__messages_queue(context->id, stored->তথ্য.qos, &stored);
			/* stored may now be free, so don't refer to it */
			যদি(rc2 == MOSQ_ERR_SUCCESS || context->protocol != mosq_p_mqtt5){
				rc2 = send__puback(context, mid, 0, ফাঁকা);
				যদি(rc2){
					rc = rc2;
				}
			}অন্যথা যদি(rc2 == MOSQ_ERR_NO_SUBSCRIBERS){
				rc2 = send__puback(context, mid, MQTT_RC_NO_MATCHING_SUBSCRIBERS, ফাঁকা);
				যদি(rc2){
					rc = rc2;
				}
			}অন্যথা{
				rc = rc2;
			}
			বিরতি;
		ক্ষেত্র 2:
			{
				পূর্ণসংখ্যা res;
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
						rc2 = send__pubrec(context, stored->তথ্য.source_mid, 0, ফাঁকা);
						যদি(rc2){
							rc = rc2;
						}
					}অন্যথা{
						log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH with dup = %d.", context->id, dup);
						ফেরত MOSQ_ERR_PROTOCOL;
					}
				}অন্যথা{
					rc = res;
				}
				বিরতি;
			}
	}

	db__message_write_queued_in(context);
	ফেরত rc;
}


পূর্ণসংখ্যা handle__publish(কাঠামো mosquitto *context)
{
	uint8_t dup;
	পূর্ণসংখ্যা rc = 0;
	uint8_t header = context->in_packet.command;
	কাঠামো mosquitto_base_বার্তা *base_বার্তা;
	size_t len;
	uint16_t slen;
	অক্ষর *topic_mount;
	mosquitto_property *properties = ফাঁকা;
	uint32_t message_expiry_interval = বার্তা_EXPIRY_INFINITE;
	পূর্ণসংখ্যা topic_alias = -1;
	uint16_t mid = 0;

	যদি(context->state != mosq_cs_active){
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		ফেরত MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	base_বার্তা = mosquitto_calloc(1, আকার(কাঠামো mosquitto_base_বার্তা));
	যদি(base_বার্তা == ফাঁকা){
		ফেরত MOSQ_ERR_NOMEM;
	}

	dup = (header & 0x08)>>3;
	base_বার্তা->তথ্য.qos = (header & 0x06)>>1;
	যদি(dup == 1 && base_বার্তা->তথ্য.qos == 0){
		log__printf(ফাঁকা, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_বার্তা_store_free(base_বার্তা);
		ফেরত MOSQ_ERR_MALFORMED_PACKET;
	}
	যদি(base_বার্তা->তথ্য.qos == 3){
		log__printf(ফাঁকা, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_বার্তা_store_free(base_বার্তা);
		ফেরত MOSQ_ERR_MALFORMED_PACKET;
	}
	যদি(base_বার্তা->তথ্য.qos > context->max_qos){
		log__printf(ফাঁকা, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_বার্তা_store_free(base_বার্তা);
		ফেরত MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	base_বার্তা->তথ্য.retain = (header & 0x01);

	যদি(base_বার্তা->তথ্য.retain && db.config->retain_available == false){
		db_বার্তা_store_free(base_বার্তা);
		ফেরত MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	যদি(packet__read_string(&context->in_packet, &base_বার্তা->তথ্য.topic, &slen)){
		db_বার্তা_store_free(base_বার্তা);
		ফেরত MOSQ_ERR_MALFORMED_PACKET;
	}
	যদি(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_বার্তা_store_free(base_বার্তা);
		ফেরত MOSQ_ERR_MALFORMED_PACKET;
	}

	যদি(base_বার্তা->তথ্য.qos > 0){
		যদি(packet__read_uint16(&context->in_packet, &mid)){
			db_বার্তা_store_free(base_বার্তা);
			ফেরত MOSQ_ERR_MALFORMED_PACKET;
		}
		যদি(mid == 0){
			db_বার্তা_store_free(base_বার্তা);
			log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			ফেরত MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		base_বার্তা->তথ্য.source_mid = mid;
	}

	/* Handle properties */
	যদি(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->in_packet, &properties);
		যদি(rc){
			db_বার্তা_store_free(base_বার্তা);
			ফেরত rc;
		}

		rc = property__process_publish(base_বার্তা, &properties, &topic_alias, &message_expiry_interval, context->bridge);
		যদি(rc){
			mosquitto_property_free_all(&properties);
			db_বার্তা_store_free(base_বার্তা);
			ফেরত MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	যদি(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_বার্তা_store_free(base_বার্তা);
		ফেরত MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}অন্যথা যদি(topic_alias > 0){
		যদি(base_বার্তা->তথ্য.topic){
			rc = alias__add_r2l(context, base_বার্তা->তথ্য.topic, (uint16_t)topic_alias);
			যদি(rc){
				db_বার্তা_store_free(base_বার্তা);
				ফেরত rc;
			}
		}অন্যথা{
			rc = alias__find_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &base_বার্তা->তথ্য.topic);
			যদি(rc){
				db_বার্তা_store_free(base_বার্তা);
				log__printf(ফাঁকা, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				ফেরত MOSQ_ERR_PROTOCOL;
			}
		}
	}

#ifdef WITH_BRIDGE
	rc = bridge__remap_topic_in(context, &base_বার্তা->তথ্য.topic);
	যদি(rc){
		db_বার্তা_store_free(base_বার্তা);
		ফেরত rc;
	}

#endif
	যদি(mosquitto_pub_topic_check(base_বার্তা->তথ্য.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_বার্তা_store_free(base_বার্তা);
		ফেরত MOSQ_ERR_MALFORMED_PACKET;
	}

	base_বার্তা->তথ্য.payloadlen = context->in_packet.remaining_দৈর্ঘ্য - context->in_packet.pos;
	metrics__int_inc(mosq_গণক_pub_bytes_received, base_বার্তা->তথ্য.payloadlen);
	যদি(context->listener && context->listener->mount_point){
		len = strlen(context->listener->mount_point) + strlen(base_বার্তা->তথ্য.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		যদি(!topic_mount){
			db_বার্তা_store_free(base_বার্তা);
			ফেরত MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_point, base_বার্তা->তথ্য.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(base_বার্তা->তথ্য.topic);
		base_বার্তা->তথ্য.topic = topic_mount;
	}

	যদি(base_বার্তা->তথ্য.payloadlen){
		যদি(db.config->message_আকার_limit && base_বার্তা->তথ্য.payloadlen > db.config->message_আকার_limit){
			log__printf(ফাঁকা, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_বার্তা->তথ্য.qos, base_বার্তা->তথ্য.retain, base_বার্তা->তথ্য.source_mid, base_বার্তা->তথ্য.topic, (long)base_বার্তা->তথ্য.payloadlen);
			ফেরত process_bad_message(context, base_বার্তা, MQTT_RC_PACKET_TOO_LARGE);
		}
		base_বার্তা->তথ্য.payload = mosquitto_malloc(base_বার্তা->তথ্য.payloadlen+1);
		যদি(base_বার্তা->তথ্য.payload == ফাঁকা){
			db_বার্তা_store_free(base_বার্তা);
			ফেরত MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_বার্তা->তথ্য.payload)[base_বার্তা->তথ্য.payloadlen] = 0;

		যদি(packet__read_bytes(&context->in_packet, base_বার্তা->তথ্য.payload, base_বার্তা->তথ্য.payloadlen)){
			db_বার্তা_store_free(base_বার্তা);
			ফেরত MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			base_বার্তা->তথ্য.topic, base_বার্তা->তথ্য.payloadlen, base_বার্তা->তথ্য.payload,
			base_বার্তা->তথ্য.qos, base_বার্তা->তথ্য.retain, base_বার্তা->তথ্য.properties,
			MOSQ_ACL_WRITE);
	যদি(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(ফাঁকা, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, base_বার্তা->তথ্য.qos, base_বার্তা->তথ্য.retain, base_বার্তা->তথ্য.source_mid, base_বার্তা->তথ্য.topic,
				(long)base_বার্তা->তথ্য.payloadlen);
		ফেরত process_bad_message(context, base_বার্তা, MQTT_RC_NOT_AUTHORIZED);
	}অন্যথা যদি(rc != MOSQ_ERR_SUCCESS){
		db_বার্তা_store_free(base_বার্তা);
		ফেরত rc;
	}

	log__printf(ফাঁকা, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_বার্তা->তথ্য.qos, base_বার্তা->তথ্য.retain, base_বার্তা->তথ্য.source_mid, base_বার্তা->তথ্য.topic, (long)base_বার্তা->তথ্য.payloadlen);

	যদি(!strncmp(base_বার্তা->তথ্য.topic, "$CONTROL/", 9)){
#ifdef WITH_CONTROL
		rc = control__process(context, base_বার্তা);
		db_বার্তা_store_free(base_বার্তা);
		ফেরত rc;
#অন্যথা
		ফেরত process_bad_message(context, base_বার্তা, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#endif
	}

	ফেরত handle__accepted_publish(context, base_বার্তা, mid, dup, &message_expiry_interval);
}
