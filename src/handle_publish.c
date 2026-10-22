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

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "alias_mosq.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "packet_mosq.h"
#ضمن "property_mosq.h"
#ضمن "read_handle.h"
#ضمن "send_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"


ساكن صحيح process_bad_message(بنية mosquitto *context, بنية mosquitto_base_سكو *base_سكو, uint8_t reason_code)
{
	صحيح rc = MOSQ_ERR_UNKNOWN;
	إذا(base_سكو){
		بدل(base_سكو->data.qos){
			حالة 0:
				rc = MOSQ_ERR_SUCCESS;
				اكسر;
			حالة 1:
				إذا(context){
					rc = send__puback(context, base_سكو->data.source_mid, reason_code, عدم);
				}وإلا{
					rc = MOSQ_ERR_SUCCESS;
				}
				اكسر;
			حالة 2:
				إذا(context){
					rc = send__pubrec(context, base_سكو->data.source_mid, reason_code, عدم);
				}وإلا{
					rc = MOSQ_ERR_SUCCESS;
				}
				اكسر;
		}
		db_سكو_store_free(base_سكو);
	}
	إذا(context && db.config->max_queued_messages > 0 && context->out_packet_كدي >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	أرجع rc;
}


صحيح handle__accepted_publish(بنية mosquitto *context, بنية mosquitto_base_سكو *base_سكو, uint16_t mid, صحيح dup, uint32_t *message_expiry_interval)
{
	صحيح rc;
	صحيح rc2;
	بنية mosquitto_base_سكو *stored = عدم;
	بنية mosquitto_client_سكو *cmsg_stored = عدم;

	{
		rc = plugin__handle_message_in(context, &base_سكو->data);
		إذا(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(عدم, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, base_سكو->data.qos, base_سكو->data.retain, base_سكو->data.source_mid, base_سكو->data.topic,
					(long)base_سكو->data.payloadlen);

			أرجع process_bad_message(context, base_سكو, MQTT_RC_NOT_AUTHORIZED);
		}وإلا إذا(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(عدم, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			أرجع process_bad_message(context, base_سكو, MQTT_RC_QUOTA_EXCEEDED);
		}وإلا إذا(rc != MOSQ_ERR_SUCCESS){
			db_سكو_store_free(base_سكو);
			أرجع rc;
		}
	}

	إذا(base_سكو->data.qos > 0){
		db__message_store_find(context, base_سكو->data.source_mid, &cmsg_stored);
	}

	إذا(cmsg_stored && base_سكو->data.source_mid != 0 &&
			(cmsg_stored->base_سكو->data.qos != base_سكو->data.qos
			|| cmsg_stored->base_سكو->data.payloadlen != base_سكو->data.payloadlen
			|| strcmp(cmsg_stored->base_سكو->data.topic, base_سكو->data.topic)
			|| (base_سكو->data.payloadlen > 0 && memcmp(cmsg_stored->base_سكو->data.payload, base_سكو->data.payload, base_سكو->data.payloadlen)))){

		log__printf(عدم, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", base_سكو->data.source_mid, context->id);
		db__message_remove_incoming(context, base_سكو->data.source_mid);
		cmsg_stored = عدم;
	}

	إذا(!cmsg_stored){
		إذا(base_سكو->data.qos > 0 && context->msgs_in.inflight_quota == 0){
			log__printf(عدم, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#إذا 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_سكو_store_free(base_سكو);
			أرجع MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED;
#endif
		}

		إذا(base_سكو->data.qos == 0
				|| db__ready_for_flight(context, mosq_md_in, base_سكو->data.qos)
				){

			dup = 0;
			rc = db__message_store(context, base_سكو, message_expiry_interval, mosq_mo_client);
			إذا(rc){
				أرجع rc;
			}
		}وإلا{
			/* Client isn't allowed any more incoming messages, so fail early */
			أرجع process_bad_message(context, base_سكو, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = base_سكو;
		base_سكو = عدم;
		dup = 0;
	}وإلا{
		db_سكو_store_free(base_سكو);
		base_سكو = عدم;
		stored = cmsg_stored->base_سكو;
		cmsg_stored->data.dup++;
		dup = cmsg_stored->data.dup;
	}

	بدل(stored->data.qos){
		حالة 0:
			rc2 = sub__messages_queue(context->id, stored->data.qos, &stored);
			إذا(rc2 > 0){
				rc = rc2;
			}
			اكسر;
		حالة 1:
			util__decrement_receive_quota(context);
			rc2 = sub__messages_queue(context->id, stored->data.qos, &stored);
			/* stored may now be free, so don't refer to it */
			إذا(rc2 == MOSQ_ERR_SUCCESS || context->protocol != mosq_p_mqtt5){
				rc2 = send__puback(context, mid, 0, عدم);
				إذا(rc2){
					rc = rc2;
				}
			}وإلا إذا(rc2 == MOSQ_ERR_NO_SUBSCRIBERS){
				rc2 = send__puback(context, mid, MQTT_RC_NO_MATCHING_SUBSCRIBERS, عدم);
				إذا(rc2){
					rc = rc2;
				}
			}وإلا{
				rc = rc2;
			}
			اكسر;
		حالة 2:
			{
				صحيح res;
				إذا(dup == 0){
					res = db_message_سك_incoming(context, 0, stored, true);
				}وإلا{
					res = 0;
				}

				/* db__message_insert() returns 2 to indicate dropped message
				 * due to queue. This isn't an error so don't disconnect them. */
				/* FIXME - this is no longer necessary due to failing early above */
				إذا(!res){
					إذا(dup == 0 || dup == 1){
						rc2 = send__pubrec(context, stored->data.source_mid, 0, عدم);
						إذا(rc2){
							rc = rc2;
						}
					}وإلا{
						log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH with dup = %d.", context->id, dup);
						أرجع MOSQ_ERR_PROTOCOL;
					}
				}وإلا{
					rc = res;
				}
				اكسر;
			}
	}

	db__message_write_queued_in(context);
	أرجع rc;
}


صحيح handle__publish(بنية mosquitto *context)
{
	uint8_t dup;
	صحيح rc = 0;
	uint8_t header = context->in_packet.command;
	بنية mosquitto_base_سكو *base_سكو;
	size_t len;
	uint16_t slen;
	حرف *topic_mount;
	mosquitto_property *properties = عدم;
	uint32_t message_expiry_interval = سكو_EXPIRY_INFINITE;
	صحيح topic_alias = -1;
	uint16_t mid = 0;

	إذا(context->state != mosq_cs_active){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		أرجع MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	base_سكو = mosquitto_calloc(1, حجم(بنية mosquitto_base_سكو));
	إذا(base_سكو == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	dup = (header & 0x08)>>3;
	base_سكو->data.qos = (header & 0x06)>>1;
	إذا(dup == 1 && base_سكو->data.qos == 0){
		log__printf(عدم, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_سكو_store_free(base_سكو);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}
	إذا(base_سكو->data.qos == 3){
		log__printf(عدم, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_سكو_store_free(base_سكو);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}
	إذا(base_سكو->data.qos > context->max_qos){
		log__printf(عدم, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_سكو_store_free(base_سكو);
		أرجع MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	base_سكو->data.retain = (header & 0x01);

	إذا(base_سكو->data.retain && db.config->retain_available == false){
		db_سكو_store_free(base_سكو);
		أرجع MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	إذا(packet__read_string(&context->in_packet, &base_سكو->data.topic, &slen)){
		db_سكو_store_free(base_سكو);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}
	إذا(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_سكو_store_free(base_سكو);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	إذا(base_سكو->data.qos > 0){
		إذا(packet__read_uint16(&context->in_packet, &mid)){
			db_سكو_store_free(base_سكو);
			أرجع MOSQ_ERR_MALFORMED_PACKET;
		}
		إذا(mid == 0){
			db_سكو_store_free(base_سكو);
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			أرجع MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		base_سكو->data.source_mid = mid;
	}

	/* Handle properties */
	إذا(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->in_packet, &properties);
		إذا(rc){
			db_سكو_store_free(base_سكو);
			أرجع rc;
		}

		rc = property__process_publish(base_سكو, &properties, &topic_alias, &message_expiry_interval, context->bridge);
		إذا(rc){
			mosquitto_property_free_all(&properties);
			db_سكو_store_free(base_سكو);
			أرجع MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	إذا(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_سكو_store_free(base_سكو);
		أرجع MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}وإلا إذا(topic_alias > 0){
		إذا(base_سكو->data.topic){
			rc = alias__add_r2l(context, base_سكو->data.topic, (uint16_t)topic_alias);
			إذا(rc){
				db_سكو_store_free(base_سكو);
				أرجع rc;
			}
		}وإلا{
			rc = alias__find_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &base_سكو->data.topic);
			إذا(rc){
				db_سكو_store_free(base_سكو);
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				أرجع MOSQ_ERR_PROTOCOL;
			}
		}
	}

#ifdef WITH_BRIDGE
	rc = bridge__remap_topic_in(context, &base_سكو->data.topic);
	إذا(rc){
		db_سكو_store_free(base_سكو);
		أرجع rc;
	}

#endif
	إذا(mosquitto_pub_topic_check(base_سكو->data.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_سكو_store_free(base_سكو);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	base_سكو->data.payloadlen = context->in_packet.remaining_length - context->in_packet.pos;
	metrics__int_inc(mosq_counter_pub_bytes_received, base_سكو->data.payloadlen);
	إذا(context->listener && context->listener->mount_point){
		len = strlen(context->listener->mount_point) + strlen(base_سكو->data.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		إذا(!topic_mount){
			db_سكو_store_free(base_سكو);
			أرجع MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_point, base_سكو->data.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(base_سكو->data.topic);
		base_سكو->data.topic = topic_mount;
	}

	إذا(base_سكو->data.payloadlen){
		إذا(db.config->message_size_limit && base_سكو->data.payloadlen > db.config->message_size_limit){
			log__printf(عدم, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_سكو->data.qos, base_سكو->data.retain, base_سكو->data.source_mid, base_سكو->data.topic, (long)base_سكو->data.payloadlen);
			أرجع process_bad_message(context, base_سكو, MQTT_RC_PACKET_TOO_LARGE);
		}
		base_سكو->data.payload = mosquitto_malloc(base_سكو->data.payloadlen+1);
		إذا(base_سكو->data.payload == عدم){
			db_سكو_store_free(base_سكو);
			أرجع MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_سكو->data.payload)[base_سكو->data.payloadlen] = 0;

		إذا(packet__read_bytes(&context->in_packet, base_سكو->data.payload, base_سكو->data.payloadlen)){
			db_سكو_store_free(base_سكو);
			أرجع MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			base_سكو->data.topic, base_سكو->data.payloadlen, base_سكو->data.payload,
			base_سكو->data.qos, base_سكو->data.retain, base_سكو->data.properties,
			MOSQ_ACL_WRITE);
	إذا(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(عدم, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, base_سكو->data.qos, base_سكو->data.retain, base_سكو->data.source_mid, base_سكو->data.topic,
				(long)base_سكو->data.payloadlen);
		أرجع process_bad_message(context, base_سكو, MQTT_RC_NOT_AUTHORIZED);
	}وإلا إذا(rc != MOSQ_ERR_SUCCESS){
		db_سكو_store_free(base_سكو);
		أرجع rc;
	}

	log__printf(عدم, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_سكو->data.qos, base_سكو->data.retain, base_سكو->data.source_mid, base_سكو->data.topic, (long)base_سكو->data.payloadlen);

	إذا(!strncmp(base_سكو->data.topic, "$CONTROL/", 9)){
#ifdef WITH_CONTROL
		rc = control__process(context, base_سكو);
		db_سكو_store_free(base_سكو);
		أرجع rc;
#وإلا
		أرجع process_bad_message(context, base_سكو, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#endif
	}

	أرجع handle__accepted_publish(context, base_سكو, mid, dup, &message_expiry_interval);
}
