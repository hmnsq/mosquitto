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


ساكن صحيح process_bad_پیغام(بنية mosquitto *context, بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام, uint8_t reason_کوډ)
{
	صحيح rc = MOSQ_ERR_UNKNOWN;
	إذا(بنسټ_پیغام){
		بدل(بنسټ_پیغام->معلومات.qos){
			حالة 0:
				rc = MOSQ_ERR_SUCCESS;
				اكسر;
			حالة 1:
				إذا(context){
					rc = send__puback(context, بنسټ_پیغام->معلومات.سرچینه_mid, reason_کوډ, عدم);
				}وإلا{
					rc = MOSQ_ERR_SUCCESS;
				}
				اكسر;
			حالة 2:
				إذا(context){
					rc = send__pubrec(context, بنسټ_پیغام->معلومات.سرچینه_mid, reason_کوډ, عدم);
				}وإلا{
					rc = MOSQ_ERR_SUCCESS;
				}
				اكسر;
		}
		db_پیغام_store_free(بنسټ_پیغام);
	}
	إذا(context && db.config->max_queued_messages > 0 && context->بهر_packet_شمېر >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	أرجع rc;
}


صحيح handle__accepted_publish(بنية mosquitto *context, بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام, uint16_t mid, صحيح dup, uint32_t *پیغام_expiry_interval)
{
	صحيح rc;
	صحيح rc2;
	بنية mosquitto_بنسټ_پیغام *stored = عدم;
	بنية mosquitto_client_پیغام *cmsg_stored = عدم;

	{
		rc = plugin_handle_پیغام_دننه(context, &بنسټ_پیغام->معلومات);
		إذا(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(عدم, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, بنسټ_پیغام->معلومات.qos, بنسټ_پیغام->معلومات.retain, بنسټ_پیغام->معلومات.سرچینه_mid, بنسټ_پیغام->معلومات.topic,
					(اوږد)بنسټ_پیغام->معلومات.payloadlen);

			أرجع process_bad_پیغام(context, بنسټ_پیغام, MQTT_RC_NOT_AUTHORIZED);
		}وإلا إذا(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(عدم, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			أرجع process_bad_پیغام(context, بنسټ_پیغام, MQTT_RC_QUOTA_EXCEEDED);
		}وإلا إذا(rc != MOSQ_ERR_SUCCESS){
			db_پیغام_store_free(بنسټ_پیغام);
			أرجع rc;
		}
	}

	إذا(بنسټ_پیغام->معلومات.qos > 0){
		db_پیغام_store_پیدا_کړه(context, بنسټ_پیغام->معلومات.سرچینه_mid, &cmsg_stored);
	}

	إذا(cmsg_stored && بنسټ_پیغام->معلومات.سرچینه_mid != 0 &&
			(cmsg_stored->بنسټ_پیغام->معلومات.qos != بنسټ_پیغام->معلومات.qos
			|| cmsg_stored->بنسټ_پیغام->معلومات.payloadlen != بنسټ_پیغام->معلومات.payloadlen
			|| strcmp(cmsg_stored->بنسټ_پیغام->معلومات.topic, بنسټ_پیغام->معلومات.topic)
			|| (بنسټ_پیغام->معلومات.payloadlen > 0 && memcmp(cmsg_stored->بنسټ_پیغام->معلومات.payload, بنسټ_پیغام->معلومات.payload, بنسټ_پیغام->معلومات.payloadlen)))){

		log__printf(عدم, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", بنسټ_پیغام->معلومات.سرچینه_mid, context->id);
		db_پیغام_remove_incoming(context, بنسټ_پیغام->معلومات.سرچینه_mid);
		cmsg_stored = عدم;
	}

	إذا(!cmsg_stored){
		إذا(بنسټ_پیغام->معلومات.qos > 0 && context->msgs_دننه.inflight_quota == 0){
			log__printf(عدم, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#إذا 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_پیغام_store_free(بنسټ_پیغام);
			أرجع MOSQ_ERR_RECEIVE_اعظمي_EXCEEDED;
#د_که_پای
		}

		إذا(بنسټ_پیغام->معلومات.qos == 0
				|| db__ready_for_flight(context, mosq_md_دننه, بنسټ_پیغام->معلومات.qos)
				){

			dup = 0;
			rc = db_پیغام_store(context, بنسټ_پیغام, پیغام_expiry_interval, mosq_mo_client);
			إذا(rc){
				أرجع rc;
			}
		}وإلا{
			/* Client isn't allowed any more incoming messages, so fail early */
			أرجع process_bad_پیغام(context, بنسټ_پیغام, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = بنسټ_پیغام;
		بنسټ_پیغام = عدم;
		dup = 0;
	}وإلا{
		db_پیغام_store_free(بنسټ_پیغام);
		بنسټ_پیغام = عدم;
		stored = cmsg_stored->بنسټ_پیغام;
		cmsg_stored->معلومات.dup++;
		dup = cmsg_stored->معلومات.dup;
	}

	بدل(stored->معلومات.qos){
		حالة 0:
			rc2 = sub_messages_کتار(context->id, stored->معلومات.qos, &stored);
			إذا(rc2 > 0){
				rc = rc2;
			}
			اكسر;
		حالة 1:
			util__decrement_receive_quota(context);
			rc2 = sub_messages_کتار(context->id, stored->معلومات.qos, &stored);
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
					res = db_پیغام_ورننباسه_incoming(context, 0, stored, true);
				}وإلا{
					res = 0;
				}

				/* db__message_insert() returns 2 to indicate dropped message
				 * due to queue. This isn't an error so don't disconnect them. */
				/* FIXME - this is no longer necessary due to failing early above */
				إذا(!res){
					إذا(dup == 0 || dup == 1){
						rc2 = send__pubrec(context, stored->معلومات.سرچینه_mid, 0, عدم);
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

	db_پیغام_write_queued_دننه(context);
	أرجع rc;
}


صحيح handle__publish(بنية mosquitto *context)
{
	uint8_t dup;
	صحيح rc = 0;
	uint8_t سرلیک = context->دننه_packet.command;
	بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام;
	size_t len;
	uint16_t slen;
	حرف *topic_mount;
	mosquitto_property *properties = عدم;
	uint32_t پیغام_expiry_interval = پیغام_EXPIRY_INFINITE;
	صحيح topic_alias = -1;
	uint16_t mid = 0;

	إذا(context->state != mosq_cs_فعال){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		أرجع MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	بنسټ_پیغام = mosquitto_calloc(1, حجم(بنية mosquitto_بنسټ_پیغام));
	إذا(بنسټ_پیغام == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	dup = (سرلیک & 0x08)>>3;
	بنسټ_پیغام->معلومات.qos = (سرلیک & 0x06)>>1;
	إذا(dup == 1 && بنسټ_پیغام->معلومات.qos == 0){
		log__printf(عدم, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_پیغام_store_free(بنسټ_پیغام);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}
	إذا(بنسټ_پیغام->معلومات.qos == 3){
		log__printf(عدم, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_پیغام_store_free(بنسټ_پیغام);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}
	إذا(بنسټ_پیغام->معلومات.qos > context->max_qos){
		log__printf(عدم, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_پیغام_store_free(بنسټ_پیغام);
		أرجع MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	بنسټ_پیغام->معلومات.retain = (سرلیک & 0x01);

	إذا(بنسټ_پیغام->معلومات.retain && db.config->retain_شته == false){
		db_پیغام_store_free(بنسټ_پیغام);
		أرجع MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	إذا(packet__read_string(&context->دننه_packet, &بنسټ_پیغام->معلومات.topic, &slen)){
		db_پیغام_store_free(بنسټ_پیغام);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}
	إذا(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_پیغام_store_free(بنسټ_پیغام);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	إذا(بنسټ_پیغام->معلومات.qos > 0){
		إذا(packet__read_uint16(&context->دننه_packet, &mid)){
			db_پیغام_store_free(بنسټ_پیغام);
			أرجع MOSQ_ERR_MALFORMED_PACKET;
		}
		إذا(mid == 0){
			db_پیغام_store_free(بنسټ_پیغام);
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			أرجع MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		بنسټ_پیغام->معلومات.سرچینه_mid = mid;
	}

	/* Handle properties */
	إذا(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->دننه_packet, &properties);
		إذا(rc){
			db_پیغام_store_free(بنسټ_پیغام);
			أرجع rc;
		}

		rc = property__process_publish(بنسټ_پیغام, &properties, &topic_alias, &پیغام_expiry_interval, context->bridge);
		إذا(rc){
			mosquitto_property_free_all(&properties);
			db_پیغام_store_free(بنسټ_پیغام);
			أرجع MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	إذا(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_پیغام_store_free(بنسټ_پیغام);
		أرجع MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}وإلا إذا(topic_alias > 0){
		إذا(بنسټ_پیغام->معلومات.topic){
			rc = alias_جمع_کړه_r2l(context, بنسټ_پیغام->معلومات.topic, (uint16_t)topic_alias);
			إذا(rc){
				db_پیغام_store_free(بنسټ_پیغام);
				أرجع rc;
			}
		}وإلا{
			rc = alias_پیدا_کړه_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &بنسټ_پیغام->معلومات.topic);
			إذا(rc){
				db_پیغام_store_free(بنسټ_پیغام);
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				أرجع MOSQ_ERR_PROTOCOL;
			}
		}
	}

#که_تعریف_شوی WITH_BRIDGE
	rc = bridge_remap_topic_دننه(context, &بنسټ_پیغام->معلومات.topic);
	إذا(rc){
		db_پیغام_store_free(بنسټ_پیغام);
		أرجع rc;
	}

#د_که_پای
	إذا(mosquitto_pub_topic_check(بنسټ_پیغام->معلومات.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_پیغام_store_free(بنسټ_پیغام);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	بنسټ_پیغام->معلومات.payloadlen = context->دننه_packet.پاتې_شوی_اوږدتیا - context->دننه_packet.pos;
	metrics__int_inc(mosq_counter_pub_بایټونه_received, بنسټ_پیغام->معلومات.payloadlen);
	إذا(context->listener && context->listener->mount_ټکی){
		len = مزي_اوږدوالی(context->listener->mount_ټکی) + مزي_اوږدوالی(بنسټ_پیغام->معلومات.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		إذا(!topic_mount){
			db_پیغام_store_free(بنسټ_پیغام);
			أرجع MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_ټکی, بنسټ_پیغام->معلومات.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(بنسټ_پیغام->معلومات.topic);
		بنسټ_پیغام->معلومات.topic = topic_mount;
	}

	إذا(بنسټ_پیغام->معلومات.payloadlen){
		إذا(db.config->پیغام_اندازه_حد && بنسټ_پیغام->معلومات.payloadlen > db.config->پیغام_اندازه_حد){
			log__printf(عدم, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, بنسټ_پیغام->معلومات.qos, بنسټ_پیغام->معلومات.retain, بنسټ_پیغام->معلومات.سرچینه_mid, بنسټ_پیغام->معلومات.topic, (اوږد)بنسټ_پیغام->معلومات.payloadlen);
			أرجع process_bad_پیغام(context, بنسټ_پیغام, MQTT_RC_PACKET_TOO_LARGE);
		}
		بنسټ_پیغام->معلومات.payload = mosquitto_malloc(بنسټ_پیغام->معلومات.payloadlen+1);
		إذا(بنسټ_پیغام->معلومات.payload == عدم){
			db_پیغام_store_free(بنسټ_پیغام);
			أرجع MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)بنسټ_پیغام->معلومات.payload)[بنسټ_پیغام->معلومات.payloadlen] = 0;

		إذا(packet_read_بایټونه(&context->دننه_packet, بنسټ_پیغام->معلومات.payload, بنسټ_پیغام->معلومات.payloadlen)){
			db_پیغام_store_free(بنسټ_پیغام);
			أرجع MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			بنسټ_پیغام->معلومات.topic, بنسټ_پیغام->معلومات.payloadlen, بنسټ_پیغام->معلومات.payload,
			بنسټ_پیغام->معلومات.qos, بنسټ_پیغام->معلومات.retain, بنسټ_پیغام->معلومات.properties,
			MOSQ_ACL_WRITE);
	إذا(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(عدم, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, بنسټ_پیغام->معلومات.qos, بنسټ_پیغام->معلومات.retain, بنسټ_پیغام->معلومات.سرچینه_mid, بنسټ_پیغام->معلومات.topic,
				(اوږد)بنسټ_پیغام->معلومات.payloadlen);
		أرجع process_bad_پیغام(context, بنسټ_پیغام, MQTT_RC_NOT_AUTHORIZED);
	}وإلا إذا(rc != MOSQ_ERR_SUCCESS){
		db_پیغام_store_free(بنسټ_پیغام);
		أرجع rc;
	}

	log__printf(عدم, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, بنسټ_پیغام->معلومات.qos, بنسټ_پیغام->معلومات.retain, بنسټ_پیغام->معلومات.سرچینه_mid, بنسټ_پیغام->معلومات.topic, (اوږد)بنسټ_پیغام->معلومات.payloadlen);

	إذا(!مزي_پرتله(بنسټ_پیغام->معلومات.topic, "$CONTROL/", 9)){
#که_تعریف_شوی WITH_CONTROL
		rc = control__process(context, بنسټ_پیغام);
		db_پیغام_store_free(بنسټ_پیغام);
		أرجع rc;
#وإلا
		أرجع process_bad_پیغام(context, بنسټ_پیغام, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#د_که_پای
	}

	أرجع handle__accepted_publish(context, بنسټ_پیغام, mid, dup, &پیغام_expiry_interval);
}
