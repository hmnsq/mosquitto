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


ساكن صحيح process_bad_پیام(بنية mosquitto *context, بنية mosquitto_پایه_پیام *پایه_پیام, uint8_t reason_کد)
{
	صحيح rc = MOSQ_ERR_UNKNOWN;
	إذا(پایه_پیام){
		بدل(پایه_پیام->داده.qos){
			حالة 0:
				rc = MOSQ_ERR_SUCCESS;
				اكسر;
			حالة 1:
				إذا(context){
					rc = send__puback(context, پایه_پیام->داده.منبع_mid, reason_کد, عدم);
				}وإلا{
					rc = MOSQ_ERR_SUCCESS;
				}
				اكسر;
			حالة 2:
				إذا(context){
					rc = send__pubrec(context, پایه_پیام->داده.منبع_mid, reason_کد, عدم);
				}وإلا{
					rc = MOSQ_ERR_SUCCESS;
				}
				اكسر;
		}
		db_پیام_store_free(پایه_پیام);
	}
	إذا(context && db.config->max_queued_messages > 0 && context->بیرون_packet_شمار >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	أرجع rc;
}


صحيح handle__accepted_publish(بنية mosquitto *context, بنية mosquitto_پایه_پیام *پایه_پیام, uint16_t mid, صحيح dup, uint32_t *پیام_expiry_interval)
{
	صحيح rc;
	صحيح rc2;
	بنية mosquitto_پایه_پیام *stored = عدم;
	بنية mosquitto_client_پیام *cmsg_stored = عدم;

	{
		rc = plugin_handle_پیام_درون(context, &پایه_پیام->داده);
		إذا(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(عدم, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, پایه_پیام->داده.qos, پایه_پیام->داده.retain, پایه_پیام->داده.منبع_mid, پایه_پیام->داده.topic,
					(دراز)پایه_پیام->داده.payloadlen);

			أرجع process_bad_پیام(context, پایه_پیام, MQTT_RC_NOT_AUTHORIZED);
		}وإلا إذا(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(عدم, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			أرجع process_bad_پیام(context, پایه_پیام, MQTT_RC_QUOTA_EXCEEDED);
		}وإلا إذا(rc != MOSQ_ERR_SUCCESS){
			db_پیام_store_free(پایه_پیام);
			أرجع rc;
		}
	}

	إذا(پایه_پیام->داده.qos > 0){
		db_پیام_store_پیدا_کن(context, پایه_پیام->داده.منبع_mid, &cmsg_stored);
	}

	إذا(cmsg_stored && پایه_پیام->داده.منبع_mid != 0 &&
			(cmsg_stored->پایه_پیام->داده.qos != پایه_پیام->داده.qos
			|| cmsg_stored->پایه_پیام->داده.payloadlen != پایه_پیام->داده.payloadlen
			|| strcmp(cmsg_stored->پایه_پیام->داده.topic, پایه_پیام->داده.topic)
			|| (پایه_پیام->داده.payloadlen > 0 && memcmp(cmsg_stored->پایه_پیام->داده.payload, پایه_پیام->داده.payload, پایه_پیام->داده.payloadlen)))){

		log__printf(عدم, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", پایه_پیام->داده.منبع_mid, context->id);
		db_پیام_remove_incoming(context, پایه_پیام->داده.منبع_mid);
		cmsg_stored = عدم;
	}

	إذا(!cmsg_stored){
		إذا(پایه_پیام->داده.qos > 0 && context->msgs_درون.inflight_quota == 0){
			log__printf(عدم, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#إذا 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_پیام_store_free(پایه_پیام);
			أرجع MOSQ_ERR_RECEIVE_بیشینه_EXCEEDED;
#پایان_اگر
		}

		إذا(پایه_پیام->داده.qos == 0
				|| db__ready_for_flight(context, mosq_md_درون, پایه_پیام->داده.qos)
				){

			dup = 0;
			rc = db_پیام_store(context, پایه_پیام, پیام_expiry_interval, mosq_mo_client);
			إذا(rc){
				أرجع rc;
			}
		}وإلا{
			/* Client isn't allowed any more incoming messages, so fail early */
			أرجع process_bad_پیام(context, پایه_پیام, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = پایه_پیام;
		پایه_پیام = عدم;
		dup = 0;
	}وإلا{
		db_پیام_store_free(پایه_پیام);
		پایه_پیام = عدم;
		stored = cmsg_stored->پایه_پیام;
		cmsg_stored->داده.dup++;
		dup = cmsg_stored->داده.dup;
	}

	بدل(stored->داده.qos){
		حالة 0:
			rc2 = sub_messages_صف(context->id, stored->داده.qos, &stored);
			إذا(rc2 > 0){
				rc = rc2;
			}
			اكسر;
		حالة 1:
			util__decrement_receive_quota(context);
			rc2 = sub_messages_صف(context->id, stored->داده.qos, &stored);
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
					res = db_پیام_درج_incoming(context, 0, stored, true);
				}وإلا{
					res = 0;
				}

				/* db__message_insert() returns 2 to indicate dropped message
				 * due to queue. This isn't an error so don't disconnect them. */
				/* FIXME - this is no longer necessary due to failing early above */
				إذا(!res){
					إذا(dup == 0 || dup == 1){
						rc2 = send__pubrec(context, stored->داده.منبع_mid, 0, عدم);
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

	db_پیام_write_queued_درون(context);
	أرجع rc;
}


صحيح handle__publish(بنية mosquitto *context)
{
	uint8_t dup;
	صحيح rc = 0;
	uint8_t سرآیند = context->درون_packet.command;
	بنية mosquitto_پایه_پیام *پایه_پیام;
	size_t len;
	uint16_t slen;
	حرف *topic_mount;
	mosquitto_property *properties = عدم;
	uint32_t پیام_expiry_interval = پیام_EXPIRY_INFINITE;
	صحيح topic_alias = -1;
	uint16_t mid = 0;

	إذا(context->state != mosq_cs_فعال){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		أرجع MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	پایه_پیام = mosquitto_calloc(1, حجم(بنية mosquitto_پایه_پیام));
	إذا(پایه_پیام == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	dup = (سرآیند & 0x08)>>3;
	پایه_پیام->داده.qos = (سرآیند & 0x06)>>1;
	إذا(dup == 1 && پایه_پیام->داده.qos == 0){
		log__printf(عدم, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_پیام_store_free(پایه_پیام);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}
	إذا(پایه_پیام->داده.qos == 3){
		log__printf(عدم, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_پیام_store_free(پایه_پیام);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}
	إذا(پایه_پیام->داده.qos > context->max_qos){
		log__printf(عدم, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_پیام_store_free(پایه_پیام);
		أرجع MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	پایه_پیام->داده.retain = (سرآیند & 0x01);

	إذا(پایه_پیام->داده.retain && db.config->retain_موجود == false){
		db_پیام_store_free(پایه_پیام);
		أرجع MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	إذا(packet__read_string(&context->درون_packet, &پایه_پیام->داده.topic, &slen)){
		db_پیام_store_free(پایه_پیام);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}
	إذا(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_پیام_store_free(پایه_پیام);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	إذا(پایه_پیام->داده.qos > 0){
		إذا(packet__read_uint16(&context->درون_packet, &mid)){
			db_پیام_store_free(پایه_پیام);
			أرجع MOSQ_ERR_MALFORMED_PACKET;
		}
		إذا(mid == 0){
			db_پیام_store_free(پایه_پیام);
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			أرجع MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		پایه_پیام->داده.منبع_mid = mid;
	}

	/* Handle properties */
	إذا(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->درون_packet, &properties);
		إذا(rc){
			db_پیام_store_free(پایه_پیام);
			أرجع rc;
		}

		rc = property__process_publish(پایه_پیام, &properties, &topic_alias, &پیام_expiry_interval, context->bridge);
		إذا(rc){
			mosquitto_property_free_all(&properties);
			db_پیام_store_free(پایه_پیام);
			أرجع MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	إذا(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_پیام_store_free(پایه_پیام);
		أرجع MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}وإلا إذا(topic_alias > 0){
		إذا(پایه_پیام->داده.topic){
			rc = alias_جمع_کن_r2l(context, پایه_پیام->داده.topic, (uint16_t)topic_alias);
			إذا(rc){
				db_پیام_store_free(پایه_پیام);
				أرجع rc;
			}
		}وإلا{
			rc = alias_پیدا_کن_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &پایه_پیام->داده.topic);
			إذا(rc){
				db_پیام_store_free(پایه_پیام);
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				أرجع MOSQ_ERR_PROTOCOL;
			}
		}
	}

#اگر_تعریف_شده WITH_BRIDGE
	rc = bridge_remap_topic_درون(context, &پایه_پیام->داده.topic);
	إذا(rc){
		db_پیام_store_free(پایه_پیام);
		أرجع rc;
	}

#پایان_اگر
	إذا(mosquitto_pub_topic_check(پایه_پیام->داده.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_پیام_store_free(پایه_پیام);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	پایه_پیام->داده.payloadlen = context->درون_packet.مانده_درازی - context->درون_packet.pos;
	metrics__int_inc(mosq_counter_pub_بایتها_received, پایه_پیام->داده.payloadlen);
	إذا(context->listener && context->listener->mount_نقطه){
		len = طول_رشته(context->listener->mount_نقطه) + طول_رشته(پایه_پیام->داده.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		إذا(!topic_mount){
			db_پیام_store_free(پایه_پیام);
			أرجع MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_نقطه, پایه_پیام->داده.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(پایه_پیام->داده.topic);
		پایه_پیام->داده.topic = topic_mount;
	}

	إذا(پایه_پیام->داده.payloadlen){
		إذا(db.config->پیام_اندازه_حد && پایه_پیام->داده.payloadlen > db.config->پیام_اندازه_حد){
			log__printf(عدم, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, پایه_پیام->داده.qos, پایه_پیام->داده.retain, پایه_پیام->داده.منبع_mid, پایه_پیام->داده.topic, (دراز)پایه_پیام->داده.payloadlen);
			أرجع process_bad_پیام(context, پایه_پیام, MQTT_RC_PACKET_TOO_LARGE);
		}
		پایه_پیام->داده.payload = mosquitto_malloc(پایه_پیام->داده.payloadlen+1);
		إذا(پایه_پیام->داده.payload == عدم){
			db_پیام_store_free(پایه_پیام);
			أرجع MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)پایه_پیام->داده.payload)[پایه_پیام->داده.payloadlen] = 0;

		إذا(packet_read_بایتها(&context->درون_packet, پایه_پیام->داده.payload, پایه_پیام->داده.payloadlen)){
			db_پیام_store_free(پایه_پیام);
			أرجع MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			پایه_پیام->داده.topic, پایه_پیام->داده.payloadlen, پایه_پیام->داده.payload,
			پایه_پیام->داده.qos, پایه_پیام->داده.retain, پایه_پیام->داده.properties,
			MOSQ_ACL_WRITE);
	إذا(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(عدم, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, پایه_پیام->داده.qos, پایه_پیام->داده.retain, پایه_پیام->داده.منبع_mid, پایه_پیام->داده.topic,
				(دراز)پایه_پیام->داده.payloadlen);
		أرجع process_bad_پیام(context, پایه_پیام, MQTT_RC_NOT_AUTHORIZED);
	}وإلا إذا(rc != MOSQ_ERR_SUCCESS){
		db_پیام_store_free(پایه_پیام);
		أرجع rc;
	}

	log__printf(عدم, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, پایه_پیام->داده.qos, پایه_پیام->داده.retain, پایه_پیام->داده.منبع_mid, پایه_پیام->داده.topic, (دراز)پایه_پیام->داده.payloadlen);

	إذا(!مقایسه_رشته(پایه_پیام->داده.topic, "$CONTROL/", 9)){
#اگر_تعریف_شده WITH_CONTROL
		rc = control__process(context, پایه_پیام);
		db_پیام_store_free(پایه_پیام);
		أرجع rc;
#وإلا
		أرجع process_bad_پیام(context, پایه_پیام, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#پایان_اگر
	}

	أرجع handle__accepted_publish(context, پایه_پیام, mid, dup, &پیام_expiry_interval);
}
