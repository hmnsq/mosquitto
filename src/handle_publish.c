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

#شامل "config.h"

#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto_broker_internal.h"
#شامل "alias_mosq.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "packet_mosq.h"
#شامل "property_mosq.h"
#شامل "read_handle.h"
#شامل "send_mosq.h"
#شامل "sys_tree.h"
#شامل "util_mosq.h"


ایستا صحیح process_bad_message(ساختار mosquitto *context, ساختار mosquitto_base_پیام *base_پیام, uint8_t reason_code)
{
	صحیح rc = MOSQ_ERR_UNKNOWN;
	اگر(base_پیام){
		انتخاب(base_پیام->data.qos){
			حالت 0:
				rc = MOSQ_ERR_SUCCESS;
				بشکن;
			حالت 1:
				اگر(context){
					rc = send__puback(context, base_پیام->data.source_mid, reason_code, پوچ);
				}وگرنه{
					rc = MOSQ_ERR_SUCCESS;
				}
				بشکن;
			حالت 2:
				اگر(context){
					rc = send__pubrec(context, base_پیام->data.source_mid, reason_code, پوچ);
				}وگرنه{
					rc = MOSQ_ERR_SUCCESS;
				}
				بشکن;
		}
		db_پیام_store_free(base_پیام);
	}
	اگر(context && db.config->max_queued_messages > 0 && context->out_packet_شمار >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	بازگشت rc;
}


صحیح handle__accepted_publish(ساختار mosquitto *context, ساختار mosquitto_base_پیام *base_پیام, uint16_t mid, صحیح dup, uint32_t *message_expiry_interval)
{
	صحیح rc;
	صحیح rc2;
	ساختار mosquitto_base_پیام *stored = پوچ;
	ساختار mosquitto_client_پیام *cmsg_stored = پوچ;

	{
		rc = plugin__handle_message_in(context, &base_پیام->data);
		اگر(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(پوچ, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, base_پیام->data.qos, base_پیام->data.retain, base_پیام->data.source_mid, base_پیام->data.topic,
					(long)base_پیام->data.payloadlen);

			بازگشت process_bad_message(context, base_پیام, MQTT_RC_NOT_AUTHORIZED);
		}وگرنه اگر(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(پوچ, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			بازگشت process_bad_message(context, base_پیام, MQTT_RC_QUOTA_EXCEEDED);
		}وگرنه اگر(rc != MOSQ_ERR_SUCCESS){
			db_پیام_store_free(base_پیام);
			بازگشت rc;
		}
	}

	اگر(base_پیام->data.qos > 0){
		db__message_store_find(context, base_پیام->data.source_mid, &cmsg_stored);
	}

	اگر(cmsg_stored && base_پیام->data.source_mid != 0 &&
			(cmsg_stored->base_پیام->data.qos != base_پیام->data.qos
			|| cmsg_stored->base_پیام->data.payloadlen != base_پیام->data.payloadlen
			|| strcmp(cmsg_stored->base_پیام->data.topic, base_پیام->data.topic)
			|| (base_پیام->data.payloadlen > 0 && memcmp(cmsg_stored->base_پیام->data.payload, base_پیام->data.payload, base_پیام->data.payloadlen)))){

		log__printf(پوچ, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", base_پیام->data.source_mid, context->id);
		db__message_remove_incoming(context, base_پیام->data.source_mid);
		cmsg_stored = پوچ;
	}

	اگر(!cmsg_stored){
		اگر(base_پیام->data.qos > 0 && context->msgs_in.inflight_quota == 0){
			log__printf(پوچ, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#اگر 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_پیام_store_free(base_پیام);
			بازگشت MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED;
#endif
		}

		اگر(base_پیام->data.qos == 0
				|| db__ready_for_flight(context, mosq_md_in, base_پیام->data.qos)
				){

			dup = 0;
			rc = db__message_store(context, base_پیام, message_expiry_interval, mosq_mo_client);
			اگر(rc){
				بازگشت rc;
			}
		}وگرنه{
			/* Client isn't allowed any more incoming messages, so fail early */
			بازگشت process_bad_message(context, base_پیام, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = base_پیام;
		base_پیام = پوچ;
		dup = 0;
	}وگرنه{
		db_پیام_store_free(base_پیام);
		base_پیام = پوچ;
		stored = cmsg_stored->base_پیام;
		cmsg_stored->data.dup++;
		dup = cmsg_stored->data.dup;
	}

	انتخاب(stored->data.qos){
		حالت 0:
			rc2 = sub__messages_queue(context->id, stored->data.qos, &stored);
			اگر(rc2 > 0){
				rc = rc2;
			}
			بشکن;
		حالت 1:
			util__decrement_receive_quota(context);
			rc2 = sub__messages_queue(context->id, stored->data.qos, &stored);
			/* stored may now be free, so don't refer to it */
			اگر(rc2 == MOSQ_ERR_SUCCESS || context->protocol != mosq_p_mqtt5){
				rc2 = send__puback(context, mid, 0, پوچ);
				اگر(rc2){
					rc = rc2;
				}
			}وگرنه اگر(rc2 == MOSQ_ERR_NO_SUBSCRIBERS){
				rc2 = send__puback(context, mid, MQTT_RC_NO_MATCHING_SUBSCRIBERS, پوچ);
				اگر(rc2){
					rc = rc2;
				}
			}وگرنه{
				rc = rc2;
			}
			بشکن;
		حالت 2:
			{
				صحیح res;
				اگر(dup == 0){
					res = db_message_درج_incoming(context, 0, stored, true);
				}وگرنه{
					res = 0;
				}

				/* db__message_insert() returns 2 to indicate dropped message
				 * due to queue. This isn't an error so don't disconnect them. */
				/* FIXME - this is no longer necessary due to failing early above */
				اگر(!res){
					اگر(dup == 0 || dup == 1){
						rc2 = send__pubrec(context, stored->data.source_mid, 0, پوچ);
						اگر(rc2){
							rc = rc2;
						}
					}وگرنه{
						log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH with dup = %d.", context->id, dup);
						بازگشت MOSQ_ERR_PROTOCOL;
					}
				}وگرنه{
					rc = res;
				}
				بشکن;
			}
	}

	db__message_write_queued_in(context);
	بازگشت rc;
}


صحیح handle__publish(ساختار mosquitto *context)
{
	uint8_t dup;
	صحیح rc = 0;
	uint8_t header = context->in_packet.command;
	ساختار mosquitto_base_پیام *base_پیام;
	size_t len;
	uint16_t slen;
	نویسه *topic_mount;
	mosquitto_property *properties = پوچ;
	uint32_t message_expiry_interval = پیام_EXPIRY_INFINITE;
	صحیح topic_alias = -1;
	uint16_t mid = 0;

	اگر(context->state != mosq_cs_active){
		log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		بازگشت MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	base_پیام = mosquitto_calloc(1, اندازه(ساختار mosquitto_base_پیام));
	اگر(base_پیام == پوچ){
		بازگشت MOSQ_ERR_NOMEM;
	}

	dup = (header & 0x08)>>3;
	base_پیام->data.qos = (header & 0x06)>>1;
	اگر(dup == 1 && base_پیام->data.qos == 0){
		log__printf(پوچ, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_پیام_store_free(base_پیام);
		بازگشت MOSQ_ERR_MALFORMED_PACKET;
	}
	اگر(base_پیام->data.qos == 3){
		log__printf(پوچ, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_پیام_store_free(base_پیام);
		بازگشت MOSQ_ERR_MALFORMED_PACKET;
	}
	اگر(base_پیام->data.qos > context->max_qos){
		log__printf(پوچ, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_پیام_store_free(base_پیام);
		بازگشت MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	base_پیام->data.retain = (header & 0x01);

	اگر(base_پیام->data.retain && db.config->retain_available == false){
		db_پیام_store_free(base_پیام);
		بازگشت MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	اگر(packet__read_string(&context->in_packet, &base_پیام->data.topic, &slen)){
		db_پیام_store_free(base_پیام);
		بازگشت MOSQ_ERR_MALFORMED_PACKET;
	}
	اگر(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_پیام_store_free(base_پیام);
		بازگشت MOSQ_ERR_MALFORMED_PACKET;
	}

	اگر(base_پیام->data.qos > 0){
		اگر(packet__read_uint16(&context->in_packet, &mid)){
			db_پیام_store_free(base_پیام);
			بازگشت MOSQ_ERR_MALFORMED_PACKET;
		}
		اگر(mid == 0){
			db_پیام_store_free(base_پیام);
			log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			بازگشت MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		base_پیام->data.source_mid = mid;
	}

	/* Handle properties */
	اگر(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->in_packet, &properties);
		اگر(rc){
			db_پیام_store_free(base_پیام);
			بازگشت rc;
		}

		rc = property__process_publish(base_پیام, &properties, &topic_alias, &message_expiry_interval, context->bridge);
		اگر(rc){
			mosquitto_property_free_all(&properties);
			db_پیام_store_free(base_پیام);
			بازگشت MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	اگر(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_پیام_store_free(base_پیام);
		بازگشت MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}وگرنه اگر(topic_alias > 0){
		اگر(base_پیام->data.topic){
			rc = alias__add_r2l(context, base_پیام->data.topic, (uint16_t)topic_alias);
			اگر(rc){
				db_پیام_store_free(base_پیام);
				بازگشت rc;
			}
		}وگرنه{
			rc = alias__find_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &base_پیام->data.topic);
			اگر(rc){
				db_پیام_store_free(base_پیام);
				log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				بازگشت MOSQ_ERR_PROTOCOL;
			}
		}
	}

#ifdef WITH_BRIDGE
	rc = bridge__remap_topic_in(context, &base_پیام->data.topic);
	اگر(rc){
		db_پیام_store_free(base_پیام);
		بازگشت rc;
	}

#endif
	اگر(mosquitto_pub_topic_check(base_پیام->data.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_پیام_store_free(base_پیام);
		بازگشت MOSQ_ERR_MALFORMED_PACKET;
	}

	base_پیام->data.payloadlen = context->in_packet.remaining_length - context->in_packet.pos;
	metrics__int_inc(mosq_counter_pub_bytes_received, base_پیام->data.payloadlen);
	اگر(context->listener && context->listener->mount_point){
		len = strlen(context->listener->mount_point) + strlen(base_پیام->data.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		اگر(!topic_mount){
			db_پیام_store_free(base_پیام);
			بازگشت MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_point, base_پیام->data.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(base_پیام->data.topic);
		base_پیام->data.topic = topic_mount;
	}

	اگر(base_پیام->data.payloadlen){
		اگر(db.config->message_size_limit && base_پیام->data.payloadlen > db.config->message_size_limit){
			log__printf(پوچ, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_پیام->data.qos, base_پیام->data.retain, base_پیام->data.source_mid, base_پیام->data.topic, (long)base_پیام->data.payloadlen);
			بازگشت process_bad_message(context, base_پیام, MQTT_RC_PACKET_TOO_LARGE);
		}
		base_پیام->data.payload = mosquitto_malloc(base_پیام->data.payloadlen+1);
		اگر(base_پیام->data.payload == پوچ){
			db_پیام_store_free(base_پیام);
			بازگشت MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_پیام->data.payload)[base_پیام->data.payloadlen] = 0;

		اگر(packet__read_bytes(&context->in_packet, base_پیام->data.payload, base_پیام->data.payloadlen)){
			db_پیام_store_free(base_پیام);
			بازگشت MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			base_پیام->data.topic, base_پیام->data.payloadlen, base_پیام->data.payload,
			base_پیام->data.qos, base_پیام->data.retain, base_پیام->data.properties,
			MOSQ_ACL_WRITE);
	اگر(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(پوچ, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, base_پیام->data.qos, base_پیام->data.retain, base_پیام->data.source_mid, base_پیام->data.topic,
				(long)base_پیام->data.payloadlen);
		بازگشت process_bad_message(context, base_پیام, MQTT_RC_NOT_AUTHORIZED);
	}وگرنه اگر(rc != MOSQ_ERR_SUCCESS){
		db_پیام_store_free(base_پیام);
		بازگشت rc;
	}

	log__printf(پوچ, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_پیام->data.qos, base_پیام->data.retain, base_پیام->data.source_mid, base_پیام->data.topic, (long)base_پیام->data.payloadlen);

	اگر(!strncmp(base_پیام->data.topic, "$CONTROL/", 9)){
#ifdef WITH_CONTROL
		rc = control__process(context, base_پیام);
		db_پیام_store_free(base_پیام);
		بازگشت rc;
#وگرنه
		بازگشت process_bad_message(context, base_پیام, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#endif
	}

	بازگشت handle__accepted_publish(context, base_پیام, mid, dup, &message_expiry_interval);
}
