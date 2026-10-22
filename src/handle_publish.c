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


ساكن صحيح process_bad_پيغام(بنية mosquitto *context, بنية mosquitto_بنياد_نياپو *بنياد_نياپو, uint8_t reason_ڪوڊ)
{
	صحيح rc = MOSQ_ERR_UNKNOWN;
	إذا(بنياد_نياپو){
		بدل(بنياد_نياپو->معلومات.qos){
			حالة 0:
				rc = MOSQ_ERR_SUCCESS;
				اكسر;
			حالة 1:
				إذا(context){
					rc = send__puback(context, بنياد_نياپو->معلومات.ذريعو_mid, reason_ڪوڊ, عدم);
				}وإلا{
					rc = MOSQ_ERR_SUCCESS;
				}
				اكسر;
			حالة 2:
				إذا(context){
					rc = send__pubrec(context, بنياد_نياپو->معلومات.ذريعو_mid, reason_ڪوڊ, عدم);
				}وإلا{
					rc = MOSQ_ERR_SUCCESS;
				}
				اكسر;
		}
		db_نياپو_store_free(بنياد_نياپو);
	}
	إذا(context && db.config->max_queued_messages > 0 && context->ٻاهريون_packet_ڳڻپ >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	أرجع rc;
}


صحيح handle__accepted_publish(بنية mosquitto *context, بنية mosquitto_بنياد_نياپو *بنياد_نياپو, uint16_t mid, صحيح dup, uint32_t *پيغام_expiry_interval)
{
	صحيح rc;
	صحيح rc2;
	بنية mosquitto_بنياد_نياپو *stored = عدم;
	بنية mosquitto_client_نياپو *cmsg_stored = عدم;

	{
		rc = plugin_handle_پيغام_اندريون(context, &بنياد_نياپو->معلومات);
		إذا(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(عدم, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, بنياد_نياپو->معلومات.qos, بنياد_نياپو->معلومات.retain, بنياد_نياپو->معلومات.ذريعو_mid, بنياد_نياپو->معلومات.topic,
					(ڊگهو)بنياد_نياپو->معلومات.payloadlen);

			أرجع process_bad_پيغام(context, بنياد_نياپو, MQTT_RC_NOT_AUTHORIZED);
		}وإلا إذا(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(عدم, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			أرجع process_bad_پيغام(context, بنياد_نياپو, MQTT_RC_QUOTA_EXCEEDED);
		}وإلا إذا(rc != MOSQ_ERR_SUCCESS){
			db_نياپو_store_free(بنياد_نياپو);
			أرجع rc;
		}
	}

	إذا(بنياد_نياپو->معلومات.qos > 0){
		db_پيغام_store_ڳوليو(context, بنياد_نياپو->معلومات.ذريعو_mid, &cmsg_stored);
	}

	إذا(cmsg_stored && بنياد_نياپو->معلومات.ذريعو_mid != 0 &&
			(cmsg_stored->بنياد_نياپو->معلومات.qos != بنياد_نياپو->معلومات.qos
			|| cmsg_stored->بنياد_نياپو->معلومات.payloadlen != بنياد_نياپو->معلومات.payloadlen
			|| strcmp(cmsg_stored->بنياد_نياپو->معلومات.topic, بنياد_نياپو->معلومات.topic)
			|| (بنياد_نياپو->معلومات.payloadlen > 0 && memcmp(cmsg_stored->بنياد_نياپو->معلومات.payload, بنياد_نياپو->معلومات.payload, بنياد_نياپو->معلومات.payloadlen)))){

		log__printf(عدم, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", بنياد_نياپو->معلومات.ذريعو_mid, context->id);
		db_پيغام_remove_incoming(context, بنياد_نياپو->معلومات.ذريعو_mid);
		cmsg_stored = عدم;
	}

	إذا(!cmsg_stored){
		إذا(بنياد_نياپو->معلومات.qos > 0 && context->msgs_اندريون.inflight_quota == 0){
			log__printf(عدم, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#إذا 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_نياپو_store_free(بنياد_نياپو);
			أرجع MOSQ_ERR_RECEIVE_وڌ_کان_وڌ_EXCEEDED;
#جيڪڏهن_پورو
		}

		إذا(بنياد_نياپو->معلومات.qos == 0
				|| db__ready_for_flight(context, mosq_md_اندريون, بنياد_نياپو->معلومات.qos)
				){

			dup = 0;
			rc = db_پيغام_store(context, بنياد_نياپو, پيغام_expiry_interval, mosq_mo_client);
			إذا(rc){
				أرجع rc;
			}
		}وإلا{
			/* Client isn't allowed any more incoming messages, so fail early */
			أرجع process_bad_پيغام(context, بنياد_نياپو, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = بنياد_نياپو;
		بنياد_نياپو = عدم;
		dup = 0;
	}وإلا{
		db_نياپو_store_free(بنياد_نياپو);
		بنياد_نياپو = عدم;
		stored = cmsg_stored->بنياد_نياپو;
		cmsg_stored->معلومات.dup++;
		dup = cmsg_stored->معلومات.dup;
	}

	بدل(stored->معلومات.qos){
		حالة 0:
			rc2 = sub_messages_قطار(context->id, stored->معلومات.qos, &stored);
			إذا(rc2 > 0){
				rc = rc2;
			}
			اكسر;
		حالة 1:
			util__decrement_receive_quota(context);
			rc2 = sub_messages_قطار(context->id, stored->معلومات.qos, &stored);
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
					res = db_پيغام_وجهو_incoming(context, 0, stored, true);
				}وإلا{
					res = 0;
				}

				/* db__message_insert() returns 2 to indicate dropped message
				 * due to queue. This isn't an error so don't disconnect them. */
				/* FIXME - this is no longer necessary due to failing early above */
				إذا(!res){
					إذا(dup == 0 || dup == 1){
						rc2 = send__pubrec(context, stored->معلومات.ذريعو_mid, 0, عدم);
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

	db_پيغام_write_queued_اندريون(context);
	أرجع rc;
}


صحيح handle__publish(بنية mosquitto *context)
{
	uint8_t dup;
	صحيح rc = 0;
	uint8_t سرو = context->اندريون_packet.command;
	بنية mosquitto_بنياد_نياپو *بنياد_نياپو;
	size_t len;
	uint16_t slen;
	حرف *topic_mount;
	mosquitto_property *properties = عدم;
	uint32_t پيغام_expiry_interval = نياپو_EXPIRY_INFINITE;
	صحيح topic_alias = -1;
	uint16_t mid = 0;

	إذا(context->state != mosq_cs_سرگرم){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		أرجع MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	بنياد_نياپو = mosquitto_calloc(1, حجم(بنية mosquitto_بنياد_نياپو));
	إذا(بنياد_نياپو == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	dup = (سرو & 0x08)>>3;
	بنياد_نياپو->معلومات.qos = (سرو & 0x06)>>1;
	إذا(dup == 1 && بنياد_نياپو->معلومات.qos == 0){
		log__printf(عدم, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_نياپو_store_free(بنياد_نياپو);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}
	إذا(بنياد_نياپو->معلومات.qos == 3){
		log__printf(عدم, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_نياپو_store_free(بنياد_نياپو);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}
	إذا(بنياد_نياپو->معلومات.qos > context->max_qos){
		log__printf(عدم, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_نياپو_store_free(بنياد_نياپو);
		أرجع MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	بنياد_نياپو->معلومات.retain = (سرو & 0x01);

	إذا(بنياد_نياپو->معلومات.retain && db.config->retain_موجود == false){
		db_نياپو_store_free(بنياد_نياپو);
		أرجع MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	إذا(packet__read_string(&context->اندريون_packet, &بنياد_نياپو->معلومات.topic, &slen)){
		db_نياپو_store_free(بنياد_نياپو);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}
	إذا(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_نياپو_store_free(بنياد_نياپو);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	إذا(بنياد_نياپو->معلومات.qos > 0){
		إذا(packet__read_uint16(&context->اندريون_packet, &mid)){
			db_نياپو_store_free(بنياد_نياپو);
			أرجع MOSQ_ERR_MALFORMED_PACKET;
		}
		إذا(mid == 0){
			db_نياپو_store_free(بنياد_نياپو);
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			أرجع MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		بنياد_نياپو->معلومات.ذريعو_mid = mid;
	}

	/* Handle properties */
	إذا(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->اندريون_packet, &properties);
		إذا(rc){
			db_نياپو_store_free(بنياد_نياپو);
			أرجع rc;
		}

		rc = property__process_publish(بنياد_نياپو, &properties, &topic_alias, &پيغام_expiry_interval, context->bridge);
		إذا(rc){
			mosquitto_property_free_all(&properties);
			db_نياپو_store_free(بنياد_نياپو);
			أرجع MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	إذا(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_نياپو_store_free(بنياد_نياپو);
		أرجع MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}وإلا إذا(topic_alias > 0){
		إذا(بنياد_نياپو->معلومات.topic){
			rc = alias_جوڙيو_r2l(context, بنياد_نياپو->معلومات.topic, (uint16_t)topic_alias);
			إذا(rc){
				db_نياپو_store_free(بنياد_نياپو);
				أرجع rc;
			}
		}وإلا{
			rc = alias_ڳوليو_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &بنياد_نياپو->معلومات.topic);
			إذا(rc){
				db_نياپو_store_free(بنياد_نياپو);
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				أرجع MOSQ_ERR_PROTOCOL;
			}
		}
	}

#جيڪڏهن_وصف_ٿيل WITH_BRIDGE
	rc = bridge_remap_topic_اندريون(context, &بنياد_نياپو->معلومات.topic);
	إذا(rc){
		db_نياپو_store_free(بنياد_نياپو);
		أرجع rc;
	}

#جيڪڏهن_پورو
	إذا(mosquitto_pub_topic_check(بنياد_نياپو->معلومات.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_نياپو_store_free(بنياد_نياپو);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	بنياد_نياپو->معلومات.payloadlen = context->اندريون_packet.بچيل_ڊيگهائي - context->اندريون_packet.pos;
	metrics__int_inc(mosq_counter_pub_بائيٽون_received, بنياد_نياپو->معلومات.payloadlen);
	إذا(context->listener && context->listener->mount_نقطو){
		len = لڙي_ڊيگهه(context->listener->mount_نقطو) + لڙي_ڊيگهه(بنياد_نياپو->معلومات.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		إذا(!topic_mount){
			db_نياپو_store_free(بنياد_نياپو);
			أرجع MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_نقطو, بنياد_نياپو->معلومات.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(بنياد_نياپو->معلومات.topic);
		بنياد_نياپو->معلومات.topic = topic_mount;
	}

	إذا(بنياد_نياپو->معلومات.payloadlen){
		إذا(db.config->پيغام_سائيز_حد_بندي && بنياد_نياپو->معلومات.payloadlen > db.config->پيغام_سائيز_حد_بندي){
			log__printf(عدم, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, بنياد_نياپو->معلومات.qos, بنياد_نياپو->معلومات.retain, بنياد_نياپو->معلومات.ذريعو_mid, بنياد_نياپو->معلومات.topic, (ڊگهو)بنياد_نياپو->معلومات.payloadlen);
			أرجع process_bad_پيغام(context, بنياد_نياپو, MQTT_RC_PACKET_TOO_LARGE);
		}
		بنياد_نياپو->معلومات.payload = mosquitto_malloc(بنياد_نياپو->معلومات.payloadlen+1);
		إذا(بنياد_نياپو->معلومات.payload == عدم){
			db_نياپو_store_free(بنياد_نياپو);
			أرجع MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)بنياد_نياپو->معلومات.payload)[بنياد_نياپو->معلومات.payloadlen] = 0;

		إذا(packet_read_بائيٽون(&context->اندريون_packet, بنياد_نياپو->معلومات.payload, بنياد_نياپو->معلومات.payloadlen)){
			db_نياپو_store_free(بنياد_نياپو);
			أرجع MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			بنياد_نياپو->معلومات.topic, بنياد_نياپو->معلومات.payloadlen, بنياد_نياپو->معلومات.payload,
			بنياد_نياپو->معلومات.qos, بنياد_نياپو->معلومات.retain, بنياد_نياپو->معلومات.properties,
			MOSQ_ACL_WRITE);
	إذا(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(عدم, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, بنياد_نياپو->معلومات.qos, بنياد_نياپو->معلومات.retain, بنياد_نياپو->معلومات.ذريعو_mid, بنياد_نياپو->معلومات.topic,
				(ڊگهو)بنياد_نياپو->معلومات.payloadlen);
		أرجع process_bad_پيغام(context, بنياد_نياپو, MQTT_RC_NOT_AUTHORIZED);
	}وإلا إذا(rc != MOSQ_ERR_SUCCESS){
		db_نياپو_store_free(بنياد_نياپو);
		أرجع rc;
	}

	log__printf(عدم, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, بنياد_نياپو->معلومات.qos, بنياد_نياپو->معلومات.retain, بنياد_نياپو->معلومات.ذريعو_mid, بنياد_نياپو->معلومات.topic, (ڊگهو)بنياد_نياپو->معلومات.payloadlen);

	إذا(!لڙي_ڀيٽ(بنياد_نياپو->معلومات.topic, "$CONTROL/", 9)){
#جيڪڏهن_وصف_ٿيل WITH_CONTROL
		rc = control__process(context, بنياد_نياپو);
		db_نياپو_store_free(بنياد_نياپو);
		أرجع rc;
#وإلا
		أرجع process_bad_پيغام(context, بنياد_نياپو, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#جيڪڏهن_پورو
	}

	أرجع handle__accepted_publish(context, بنياد_نياپو, mid, dup, &پيغام_expiry_interval);
}
