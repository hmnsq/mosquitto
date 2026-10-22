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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "alias_mosq.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "packet_mosq.h"
#അന്തര്ഭാവ "property_mosq.h"
#അന്തര്ഭാവ "read_handle.h"
#അന്തര്ഭാവ "send_mosq.h"
#അന്തര്ഭാവ "sys_tree.h"
#അന്തര്ഭാവ "util_mosq.h"


സ്ഥിതം പൂർണ്ണസംഖ്യ process_bad_message(ഘടന mosquitto *context, ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം, uint8_t reason_code)
{
	പൂർണ്ണസംഖ്യ rc = MOSQ_ERR_UNKNOWN;
	എങ്കിൽ(base_സന്ദേശം){
		തിരഞ്ഞെടുപ്പ്(base_സന്ദേശം->data.qos){
			അവസ്ഥ 0:
				rc = MOSQ_ERR_SUCCESS;
				നിർത്തുക;
			അവസ്ഥ 1:
				എങ്കിൽ(context){
					rc = send__puback(context, base_സന്ദേശം->data.source_mid, reason_code, ശൂന്യ);
				}അല്ലെങ്കിൽ{
					rc = MOSQ_ERR_SUCCESS;
				}
				നിർത്തുക;
			അവസ്ഥ 2:
				എങ്കിൽ(context){
					rc = send__pubrec(context, base_സന്ദേശം->data.source_mid, reason_code, ശൂന്യ);
				}അല്ലെങ്കിൽ{
					rc = MOSQ_ERR_SUCCESS;
				}
				നിർത്തുക;
		}
		db_സന്ദേശം_store_free(base_സന്ദേശം);
	}
	എങ്കിൽ(context && db.config->max_queued_messages > 0 && context->out_packet_എണ്ണം >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	തിരികെ rc;
}


പൂർണ്ണസംഖ്യ handle__accepted_publish(ഘടന mosquitto *context, ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം, uint16_t mid, പൂർണ്ണസംഖ്യ dup, uint32_t *message_expiry_interval)
{
	പൂർണ്ണസംഖ്യ rc;
	പൂർണ്ണസംഖ്യ rc2;
	ഘടന mosquitto_base_സന്ദേശം *stored = ശൂന്യ;
	ഘടന mosquitto_client_സന്ദേശം *cmsg_stored = ശൂന്യ;

	{
		rc = plugin__handle_message_in(context, &base_സന്ദേശം->data);
		എങ്കിൽ(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(ശൂന്യ, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, base_സന്ദേശം->data.qos, base_സന്ദേശം->data.retain, base_സന്ദേശം->data.source_mid, base_സന്ദേശം->data.topic,
					(long)base_സന്ദേശം->data.payloadlen);

			തിരികെ process_bad_message(context, base_സന്ദേശം, MQTT_RC_NOT_AUTHORIZED);
		}അല്ലെങ്കിൽ എങ്കിൽ(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(ശൂന്യ, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			തിരികെ process_bad_message(context, base_സന്ദേശം, MQTT_RC_QUOTA_EXCEEDED);
		}അല്ലെങ്കിൽ എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
			db_സന്ദേശം_store_free(base_സന്ദേശം);
			തിരികെ rc;
		}
	}

	എങ്കിൽ(base_സന്ദേശം->data.qos > 0){
		db__message_store_find(context, base_സന്ദേശം->data.source_mid, &cmsg_stored);
	}

	എങ്കിൽ(cmsg_stored && base_സന്ദേശം->data.source_mid != 0 &&
			(cmsg_stored->base_സന്ദേശം->data.qos != base_സന്ദേശം->data.qos
			|| cmsg_stored->base_സന്ദേശം->data.payloadlen != base_സന്ദേശം->data.payloadlen
			|| strcmp(cmsg_stored->base_സന്ദേശം->data.topic, base_സന്ദേശം->data.topic)
			|| (base_സന്ദേശം->data.payloadlen > 0 && memcmp(cmsg_stored->base_സന്ദേശം->data.payload, base_സന്ദേശം->data.payload, base_സന്ദേശം->data.payloadlen)))){

		log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", base_സന്ദേശം->data.source_mid, context->id);
		db__message_remove_incoming(context, base_സന്ദേശം->data.source_mid);
		cmsg_stored = ശൂന്യ;
	}

	എങ്കിൽ(!cmsg_stored){
		എങ്കിൽ(base_സന്ദേശം->data.qos > 0 && context->msgs_in.inflight_quota == 0){
			log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#എങ്കിൽ 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_സന്ദേശം_store_free(base_സന്ദേശം);
			തിരികെ MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED;
#endif
		}

		എങ്കിൽ(base_സന്ദേശം->data.qos == 0
				|| db__ready_for_flight(context, mosq_md_in, base_സന്ദേശം->data.qos)
				){

			dup = 0;
			rc = db__message_store(context, base_സന്ദേശം, message_expiry_interval, mosq_mo_client);
			എങ്കിൽ(rc){
				തിരികെ rc;
			}
		}അല്ലെങ്കിൽ{
			/* Client isn't allowed any more incoming messages, so fail early */
			തിരികെ process_bad_message(context, base_സന്ദേശം, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = base_സന്ദേശം;
		base_സന്ദേശം = ശൂന്യ;
		dup = 0;
	}അല്ലെങ്കിൽ{
		db_സന്ദേശം_store_free(base_സന്ദേശം);
		base_സന്ദേശം = ശൂന്യ;
		stored = cmsg_stored->base_സന്ദേശം;
		cmsg_stored->data.dup++;
		dup = cmsg_stored->data.dup;
	}

	തിരഞ്ഞെടുപ്പ്(stored->data.qos){
		അവസ്ഥ 0:
			rc2 = sub__messages_queue(context->id, stored->data.qos, &stored);
			എങ്കിൽ(rc2 > 0){
				rc = rc2;
			}
			നിർത്തുക;
		അവസ്ഥ 1:
			util__decrement_receive_quota(context);
			rc2 = sub__messages_queue(context->id, stored->data.qos, &stored);
			/* stored may now be free, so don't refer to it */
			എങ്കിൽ(rc2 == MOSQ_ERR_SUCCESS || context->protocol != mosq_p_mqtt5){
				rc2 = send__puback(context, mid, 0, ശൂന്യ);
				എങ്കിൽ(rc2){
					rc = rc2;
				}
			}അല്ലെങ്കിൽ എങ്കിൽ(rc2 == MOSQ_ERR_NO_SUBSCRIBERS){
				rc2 = send__puback(context, mid, MQTT_RC_NO_MATCHING_SUBSCRIBERS, ശൂന്യ);
				എങ്കിൽ(rc2){
					rc = rc2;
				}
			}അല്ലെങ്കിൽ{
				rc = rc2;
			}
			നിർത്തുക;
		അവസ്ഥ 2:
			{
				പൂർണ്ണസംഖ്യ res;
				എങ്കിൽ(dup == 0){
					res = db_message_തിരുകുക_incoming(context, 0, stored, true);
				}അല്ലെങ്കിൽ{
					res = 0;
				}

				/* db__message_insert() returns 2 to indicate dropped message
				 * due to queue. This isn't an error so don't disconnect them. */
				/* FIXME - this is no longer necessary due to failing early above */
				എങ്കിൽ(!res){
					എങ്കിൽ(dup == 0 || dup == 1){
						rc2 = send__pubrec(context, stored->data.source_mid, 0, ശൂന്യ);
						എങ്കിൽ(rc2){
							rc = rc2;
						}
					}അല്ലെങ്കിൽ{
						log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH with dup = %d.", context->id, dup);
						തിരികെ MOSQ_ERR_PROTOCOL;
					}
				}അല്ലെങ്കിൽ{
					rc = res;
				}
				നിർത്തുക;
			}
	}

	db__message_write_queued_in(context);
	തിരികെ rc;
}


പൂർണ്ണസംഖ്യ handle__publish(ഘടന mosquitto *context)
{
	uint8_t dup;
	പൂർണ്ണസംഖ്യ rc = 0;
	uint8_t header = context->in_packet.command;
	ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം;
	size_t len;
	uint16_t slen;
	അക്ഷരം *topic_mount;
	mosquitto_property *properties = ശൂന്യ;
	uint32_t message_expiry_interval = സന്ദേശം_EXPIRY_INFINITE;
	പൂർണ്ണസംഖ്യ topic_alias = -1;
	uint16_t mid = 0;

	എങ്കിൽ(context->state != mosq_cs_active){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		തിരികെ MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	base_സന്ദേശം = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto_base_സന്ദേശം));
	എങ്കിൽ(base_സന്ദേശം == ശൂന്യ){
		തിരികെ MOSQ_ERR_NOMEM;
	}

	dup = (header & 0x08)>>3;
	base_സന്ദേശം->data.qos = (header & 0x06)>>1;
	എങ്കിൽ(dup == 1 && base_സന്ദേശം->data.qos == 0){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_സന്ദേശം_store_free(base_സന്ദേശം);
		തിരികെ MOSQ_ERR_MALFORMED_PACKET;
	}
	എങ്കിൽ(base_സന്ദേശം->data.qos == 3){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_സന്ദേശം_store_free(base_സന്ദേശം);
		തിരികെ MOSQ_ERR_MALFORMED_PACKET;
	}
	എങ്കിൽ(base_സന്ദേശം->data.qos > context->max_qos){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_സന്ദേശം_store_free(base_സന്ദേശം);
		തിരികെ MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	base_സന്ദേശം->data.retain = (header & 0x01);

	എങ്കിൽ(base_സന്ദേശം->data.retain && db.config->retain_available == false){
		db_സന്ദേശം_store_free(base_സന്ദേശം);
		തിരികെ MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	എങ്കിൽ(packet__read_string(&context->in_packet, &base_സന്ദേശം->data.topic, &slen)){
		db_സന്ദേശം_store_free(base_സന്ദേശം);
		തിരികെ MOSQ_ERR_MALFORMED_PACKET;
	}
	എങ്കിൽ(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_സന്ദേശം_store_free(base_സന്ദേശം);
		തിരികെ MOSQ_ERR_MALFORMED_PACKET;
	}

	എങ്കിൽ(base_സന്ദേശം->data.qos > 0){
		എങ്കിൽ(packet__read_uint16(&context->in_packet, &mid)){
			db_സന്ദേശം_store_free(base_സന്ദേശം);
			തിരികെ MOSQ_ERR_MALFORMED_PACKET;
		}
		എങ്കിൽ(mid == 0){
			db_സന്ദേശം_store_free(base_സന്ദേശം);
			log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			തിരികെ MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		base_സന്ദേശം->data.source_mid = mid;
	}

	/* Handle properties */
	എങ്കിൽ(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->in_packet, &properties);
		എങ്കിൽ(rc){
			db_സന്ദേശം_store_free(base_സന്ദേശം);
			തിരികെ rc;
		}

		rc = property__process_publish(base_സന്ദേശം, &properties, &topic_alias, &message_expiry_interval, context->bridge);
		എങ്കിൽ(rc){
			mosquitto_property_free_all(&properties);
			db_സന്ദേശം_store_free(base_സന്ദേശം);
			തിരികെ MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	എങ്കിൽ(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_സന്ദേശം_store_free(base_സന്ദേശം);
		തിരികെ MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}അല്ലെങ്കിൽ എങ്കിൽ(topic_alias > 0){
		എങ്കിൽ(base_സന്ദേശം->data.topic){
			rc = alias__add_r2l(context, base_സന്ദേശം->data.topic, (uint16_t)topic_alias);
			എങ്കിൽ(rc){
				db_സന്ദേശം_store_free(base_സന്ദേശം);
				തിരികെ rc;
			}
		}അല്ലെങ്കിൽ{
			rc = alias__find_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &base_സന്ദേശം->data.topic);
			എങ്കിൽ(rc){
				db_സന്ദേശം_store_free(base_സന്ദേശം);
				log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				തിരികെ MOSQ_ERR_PROTOCOL;
			}
		}
	}

#ifdef WITH_BRIDGE
	rc = bridge__remap_topic_in(context, &base_സന്ദേശം->data.topic);
	എങ്കിൽ(rc){
		db_സന്ദേശം_store_free(base_സന്ദേശം);
		തിരികെ rc;
	}

#endif
	എങ്കിൽ(mosquitto_pub_topic_check(base_സന്ദേശം->data.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_സന്ദേശം_store_free(base_സന്ദേശം);
		തിരികെ MOSQ_ERR_MALFORMED_PACKET;
	}

	base_സന്ദേശം->data.payloadlen = context->in_packet.remaining_length - context->in_packet.pos;
	metrics__int_inc(mosq_counter_pub_bytes_received, base_സന്ദേശം->data.payloadlen);
	എങ്കിൽ(context->listener && context->listener->mount_point){
		len = strlen(context->listener->mount_point) + strlen(base_സന്ദേശം->data.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		എങ്കിൽ(!topic_mount){
			db_സന്ദേശം_store_free(base_സന്ദേശം);
			തിരികെ MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_point, base_സന്ദേശം->data.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(base_സന്ദേശം->data.topic);
		base_സന്ദേശം->data.topic = topic_mount;
	}

	എങ്കിൽ(base_സന്ദേശം->data.payloadlen){
		എങ്കിൽ(db.config->message_size_limit && base_സന്ദേശം->data.payloadlen > db.config->message_size_limit){
			log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_സന്ദേശം->data.qos, base_സന്ദേശം->data.retain, base_സന്ദേശം->data.source_mid, base_സന്ദേശം->data.topic, (long)base_സന്ദേശം->data.payloadlen);
			തിരികെ process_bad_message(context, base_സന്ദേശം, MQTT_RC_PACKET_TOO_LARGE);
		}
		base_സന്ദേശം->data.payload = mosquitto_malloc(base_സന്ദേശം->data.payloadlen+1);
		എങ്കിൽ(base_സന്ദേശം->data.payload == ശൂന്യ){
			db_സന്ദേശം_store_free(base_സന്ദേശം);
			തിരികെ MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_സന്ദേശം->data.payload)[base_സന്ദേശം->data.payloadlen] = 0;

		എങ്കിൽ(packet__read_bytes(&context->in_packet, base_സന്ദേശം->data.payload, base_സന്ദേശം->data.payloadlen)){
			db_സന്ദേശം_store_free(base_സന്ദേശം);
			തിരികെ MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			base_സന്ദേശം->data.topic, base_സന്ദേശം->data.payloadlen, base_സന്ദേശം->data.payload,
			base_സന്ദേശം->data.qos, base_സന്ദേശം->data.retain, base_സന്ദേശം->data.properties,
			MOSQ_ACL_WRITE);
	എങ്കിൽ(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(ശൂന്യ, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, base_സന്ദേശം->data.qos, base_സന്ദേശം->data.retain, base_സന്ദേശം->data.source_mid, base_സന്ദേശം->data.topic,
				(long)base_സന്ദേശം->data.payloadlen);
		തിരികെ process_bad_message(context, base_സന്ദേശം, MQTT_RC_NOT_AUTHORIZED);
	}അല്ലെങ്കിൽ എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
		db_സന്ദേശം_store_free(base_സന്ദേശം);
		തിരികെ rc;
	}

	log__printf(ശൂന്യ, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_സന്ദേശം->data.qos, base_സന്ദേശം->data.retain, base_സന്ദേശം->data.source_mid, base_സന്ദേശം->data.topic, (long)base_സന്ദേശം->data.payloadlen);

	എങ്കിൽ(!strncmp(base_സന്ദേശം->data.topic, "$CONTROL/", 9)){
#ifdef WITH_CONTROL
		rc = control__process(context, base_സന്ദേശം);
		db_സന്ദേശം_store_free(base_സന്ദേശം);
		തിരികെ rc;
#അല്ലെങ്കിൽ
		തിരികെ process_bad_message(context, base_സന്ദേശം, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#endif
	}

	തിരികെ handle__accepted_publish(context, base_സന്ദേശം, mid, dup, &message_expiry_interval);
}
