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


ساكن صحيح process_bad_سنیہا(بنية mosquitto *context, بنية mosquitto_ادھار_پیغام *ادھار_پیغام, uint8_t reason_کوڈ)
{
	صحيح rc = MOSQ_ERR_UNKNOWN;
	إذا(ادھار_پیغام){
		بدل(ادھار_پیغام->انکڑے.qos){
			حالة 0:
				rc = MOSQ_ERR_SUCCESS;
				اكسر;
			حالة 1:
				إذا(context){
					rc = send__puback(context, ادھار_پیغام->انکڑے.سروت_mid, reason_کوڈ, عدم);
				}وإلا{
					rc = MOSQ_ERR_SUCCESS;
				}
				اكسر;
			حالة 2:
				إذا(context){
					rc = send__pubrec(context, ادھار_پیغام->انکڑے.سروت_mid, reason_کوڈ, عدم);
				}وإلا{
					rc = MOSQ_ERR_SUCCESS;
				}
				اكسر;
		}
		db_پیغام_store_free(ادھار_پیغام);
	}
	إذا(context && db.config->max_queued_messages > 0 && context->باہرلا_packet_گنتی >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	أرجع rc;
}


صحيح handle__accepted_publish(بنية mosquitto *context, بنية mosquitto_ادھار_پیغام *ادھار_پیغام, uint16_t mid, صحيح dup, uint32_t *سنیہا_expiry_interval)
{
	صحيح rc;
	صحيح rc2;
	بنية mosquitto_ادھار_پیغام *stored = عدم;
	بنية mosquitto_client_پیغام *cmsg_stored = عدم;

	{
		rc = plugin_handle_سنیہا_وچ(context, &ادھار_پیغام->انکڑے);
		إذا(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(عدم, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, ادھار_پیغام->انکڑے.qos, ادھار_پیغام->انکڑے.retain, ادھار_پیغام->انکڑے.سروت_mid, ادھار_پیغام->انکڑے.topic,
					(لما)ادھار_پیغام->انکڑے.payloadlen);

			أرجع process_bad_سنیہا(context, ادھار_پیغام, MQTT_RC_NOT_AUTHORIZED);
		}وإلا إذا(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(عدم, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			أرجع process_bad_سنیہا(context, ادھار_پیغام, MQTT_RC_QUOTA_EXCEEDED);
		}وإلا إذا(rc != MOSQ_ERR_SUCCESS){
			db_پیغام_store_free(ادھار_پیغام);
			أرجع rc;
		}
	}

	إذا(ادھار_پیغام->انکڑے.qos > 0){
		db_سنیہا_store_لبھو(context, ادھار_پیغام->انکڑے.سروت_mid, &cmsg_stored);
	}

	إذا(cmsg_stored && ادھار_پیغام->انکڑے.سروت_mid != 0 &&
			(cmsg_stored->ادھار_پیغام->انکڑے.qos != ادھار_پیغام->انکڑے.qos
			|| cmsg_stored->ادھار_پیغام->انکڑے.payloadlen != ادھار_پیغام->انکڑے.payloadlen
			|| strcmp(cmsg_stored->ادھار_پیغام->انکڑے.topic, ادھار_پیغام->انکڑے.topic)
			|| (ادھار_پیغام->انکڑے.payloadlen > 0 && memcmp(cmsg_stored->ادھار_پیغام->انکڑے.payload, ادھار_پیغام->انکڑے.payload, ادھار_پیغام->انکڑے.payloadlen)))){

		log__printf(عدم, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", ادھار_پیغام->انکڑے.سروت_mid, context->id);
		db_سنیہا_remove_incoming(context, ادھار_پیغام->انکڑے.سروت_mid);
		cmsg_stored = عدم;
	}

	إذا(!cmsg_stored){
		إذا(ادھار_پیغام->انکڑے.qos > 0 && context->msgs_وچ.inflight_quota == 0){
			log__printf(عدم, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#إذا 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_پیغام_store_free(ادھار_پیغام);
			أرجع MOSQ_ERR_RECEIVE_ودھ_توں_ودھ_EXCEEDED;
#جے_انت
		}

		إذا(ادھار_پیغام->انکڑے.qos == 0
				|| db__ready_for_flight(context, mosq_md_وچ, ادھار_پیغام->انکڑے.qos)
				){

			dup = 0;
			rc = db_سنیہا_store(context, ادھار_پیغام, سنیہا_expiry_interval, mosq_mo_client);
			إذا(rc){
				أرجع rc;
			}
		}وإلا{
			/* Client isn't allowed any more incoming messages, so fail early */
			أرجع process_bad_سنیہا(context, ادھار_پیغام, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = ادھار_پیغام;
		ادھار_پیغام = عدم;
		dup = 0;
	}وإلا{
		db_پیغام_store_free(ادھار_پیغام);
		ادھار_پیغام = عدم;
		stored = cmsg_stored->ادھار_پیغام;
		cmsg_stored->انکڑے.dup++;
		dup = cmsg_stored->انکڑے.dup;
	}

	بدل(stored->انکڑے.qos){
		حالة 0:
			rc2 = sub_messages_قطار(context->id, stored->انکڑے.qos, &stored);
			إذا(rc2 > 0){
				rc = rc2;
			}
			اكسر;
		حالة 1:
			util__decrement_receive_quota(context);
			rc2 = sub_messages_قطار(context->id, stored->انکڑے.qos, &stored);
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
					res = db_سنیہا_داخل_کرو_incoming(context, 0, stored, true);
				}وإلا{
					res = 0;
				}

				/* db__message_insert() returns 2 to indicate dropped message
				 * due to queue. This isn't an error so don't disconnect them. */
				/* FIXME - this is no longer necessary due to failing early above */
				إذا(!res){
					إذا(dup == 0 || dup == 1){
						rc2 = send__pubrec(context, stored->انکڑے.سروت_mid, 0, عدم);
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

	db_سنیہا_write_queued_وچ(context);
	أرجع rc;
}


صحيح handle__publish(بنية mosquitto *context)
{
	uint8_t dup;
	صحيح rc = 0;
	uint8_t سرخی = context->وچ_packet.command;
	بنية mosquitto_ادھار_پیغام *ادھار_پیغام;
	size_t len;
	uint16_t slen;
	حرف *topic_mount;
	mosquitto_property *properties = عدم;
	uint32_t سنیہا_expiry_interval = پیغام_EXPIRY_INFINITE;
	صحيح topic_alias = -1;
	uint16_t mid = 0;

	إذا(context->state != mosq_cs_سرگرم){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		أرجع MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	ادھار_پیغام = mosquitto_calloc(1, حجم(بنية mosquitto_ادھار_پیغام));
	إذا(ادھار_پیغام == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	dup = (سرخی & 0x08)>>3;
	ادھار_پیغام->انکڑے.qos = (سرخی & 0x06)>>1;
	إذا(dup == 1 && ادھار_پیغام->انکڑے.qos == 0){
		log__printf(عدم, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_پیغام_store_free(ادھار_پیغام);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}
	إذا(ادھار_پیغام->انکڑے.qos == 3){
		log__printf(عدم, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_پیغام_store_free(ادھار_پیغام);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}
	إذا(ادھار_پیغام->انکڑے.qos > context->max_qos){
		log__printf(عدم, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_پیغام_store_free(ادھار_پیغام);
		أرجع MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	ادھار_پیغام->انکڑے.retain = (سرخی & 0x01);

	إذا(ادھار_پیغام->انکڑے.retain && db.config->retain_دستیاب == false){
		db_پیغام_store_free(ادھار_پیغام);
		أرجع MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	إذا(packet__read_string(&context->وچ_packet, &ادھار_پیغام->انکڑے.topic, &slen)){
		db_پیغام_store_free(ادھار_پیغام);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}
	إذا(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_پیغام_store_free(ادھار_پیغام);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	إذا(ادھار_پیغام->انکڑے.qos > 0){
		إذا(packet__read_uint16(&context->وچ_packet, &mid)){
			db_پیغام_store_free(ادھار_پیغام);
			أرجع MOSQ_ERR_MALFORMED_PACKET;
		}
		إذا(mid == 0){
			db_پیغام_store_free(ادھار_پیغام);
			log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			أرجع MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		ادھار_پیغام->انکڑے.سروت_mid = mid;
	}

	/* Handle properties */
	إذا(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->وچ_packet, &properties);
		إذا(rc){
			db_پیغام_store_free(ادھار_پیغام);
			أرجع rc;
		}

		rc = property__process_publish(ادھار_پیغام, &properties, &topic_alias, &سنیہا_expiry_interval, context->bridge);
		إذا(rc){
			mosquitto_property_free_all(&properties);
			db_پیغام_store_free(ادھار_پیغام);
			أرجع MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	إذا(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_پیغام_store_free(ادھار_پیغام);
		أرجع MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}وإلا إذا(topic_alias > 0){
		إذا(ادھار_پیغام->انکڑے.topic){
			rc = alias_جوڑو_r2l(context, ادھار_پیغام->انکڑے.topic, (uint16_t)topic_alias);
			إذا(rc){
				db_پیغام_store_free(ادھار_پیغام);
				أرجع rc;
			}
		}وإلا{
			rc = alias_لبھو_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &ادھار_پیغام->انکڑے.topic);
			إذا(rc){
				db_پیغام_store_free(ادھار_پیغام);
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				أرجع MOSQ_ERR_PROTOCOL;
			}
		}
	}

#جے_تعریف_شدہ WITH_BRIDGE
	rc = bridge_remap_topic_وچ(context, &ادھار_پیغام->انکڑے.topic);
	إذا(rc){
		db_پیغام_store_free(ادھار_پیغام);
		أرجع rc;
	}

#جے_انت
	إذا(mosquitto_pub_topic_check(ادھار_پیغام->انکڑے.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_پیغام_store_free(ادھار_پیغام);
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	ادھار_پیغام->انکڑے.payloadlen = context->وچ_packet.بچیا_لمائی - context->وچ_packet.pos;
	metrics__int_inc(mosq_counter_pub_بائٹاں_received, ادھار_پیغام->انکڑے.payloadlen);
	إذا(context->listener && context->listener->mount_نقطہ){
		len = لڑی_لمبائی(context->listener->mount_نقطہ) + لڑی_لمبائی(ادھار_پیغام->انکڑے.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		إذا(!topic_mount){
			db_پیغام_store_free(ادھار_پیغام);
			أرجع MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_نقطہ, ادھار_پیغام->انکڑے.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(ادھار_پیغام->انکڑے.topic);
		ادھار_پیغام->انکڑے.topic = topic_mount;
	}

	إذا(ادھار_پیغام->انکڑے.payloadlen){
		إذا(db.config->سنیہا_ناپ_حد_بندی && ادھار_پیغام->انکڑے.payloadlen > db.config->سنیہا_ناپ_حد_بندی){
			log__printf(عدم, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, ادھار_پیغام->انکڑے.qos, ادھار_پیغام->انکڑے.retain, ادھار_پیغام->انکڑے.سروت_mid, ادھار_پیغام->انکڑے.topic, (لما)ادھار_پیغام->انکڑے.payloadlen);
			أرجع process_bad_سنیہا(context, ادھار_پیغام, MQTT_RC_PACKET_TOO_LARGE);
		}
		ادھار_پیغام->انکڑے.payload = mosquitto_malloc(ادھار_پیغام->انکڑے.payloadlen+1);
		إذا(ادھار_پیغام->انکڑے.payload == عدم){
			db_پیغام_store_free(ادھار_پیغام);
			أرجع MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)ادھار_پیغام->انکڑے.payload)[ادھار_پیغام->انکڑے.payloadlen] = 0;

		إذا(packet_read_بائٹاں(&context->وچ_packet, ادھار_پیغام->انکڑے.payload, ادھار_پیغام->انکڑے.payloadlen)){
			db_پیغام_store_free(ادھار_پیغام);
			أرجع MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			ادھار_پیغام->انکڑے.topic, ادھار_پیغام->انکڑے.payloadlen, ادھار_پیغام->انکڑے.payload,
			ادھار_پیغام->انکڑے.qos, ادھار_پیغام->انکڑے.retain, ادھار_پیغام->انکڑے.properties,
			MOSQ_ACL_WRITE);
	إذا(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(عدم, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, ادھار_پیغام->انکڑے.qos, ادھار_پیغام->انکڑے.retain, ادھار_پیغام->انکڑے.سروت_mid, ادھار_پیغام->انکڑے.topic,
				(لما)ادھار_پیغام->انکڑے.payloadlen);
		أرجع process_bad_سنیہا(context, ادھار_پیغام, MQTT_RC_NOT_AUTHORIZED);
	}وإلا إذا(rc != MOSQ_ERR_SUCCESS){
		db_پیغام_store_free(ادھار_پیغام);
		أرجع rc;
	}

	log__printf(عدم, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, ادھار_پیغام->انکڑے.qos, ادھار_پیغام->انکڑے.retain, ادھار_پیغام->انکڑے.سروت_mid, ادھار_پیغام->انکڑے.topic, (لما)ادھار_پیغام->انکڑے.payloadlen);

	إذا(!لڑی_مقابلہ(ادھار_پیغام->انکڑے.topic, "$CONTROL/", 9)){
#جے_تعریف_شدہ WITH_CONTROL
		rc = control__process(context, ادھار_پیغام);
		db_پیغام_store_free(ادھار_پیغام);
		أرجع rc;
#وإلا
		أرجع process_bad_سنیہا(context, ادھار_پیغام, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#جے_انت
	}

	أرجع handle__accepted_publish(context, ادھار_پیغام, mid, dup, &سنیہا_expiry_interval);
}
