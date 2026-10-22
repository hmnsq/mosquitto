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


ساکن صحیح process_bad_پیغام(ساخت mosquitto *context, ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام, uint8_t reason_رمز)
{
	صحیح rc = MOSQ_ERR_UNKNOWN;
	اگر(بنیاد_پیغام){
		انتخاب(بنیاد_پیغام->ڈیٹا.qos){
			حالت 0:
				rc = MOSQ_ERR_SUCCESS;
				توڑو;
			حالت 1:
				اگر(context){
					rc = send__puback(context, بنیاد_پیغام->ڈیٹا.ماخذ_mid, reason_رمز, تہی);
				}ورنہ{
					rc = MOSQ_ERR_SUCCESS;
				}
				توڑو;
			حالت 2:
				اگر(context){
					rc = send__pubrec(context, بنیاد_پیغام->ڈیٹا.ماخذ_mid, reason_رمز, تہی);
				}ورنہ{
					rc = MOSQ_ERR_SUCCESS;
				}
				توڑو;
		}
		db_پیغام_store_free(بنیاد_پیغام);
	}
	اگر(context && db.config->max_queued_messages > 0 && context->برآمد_packet_گنتی >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	واپس rc;
}


صحیح handle__accepted_publish(ساخت mosquitto *context, ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام, uint16_t mid, صحیح dup, uint32_t *پیغام_expiry_interval)
{
	صحیح rc;
	صحیح rc2;
	ساخت mosquitto_بنیاد_پیغام *stored = تہی;
	ساخت mosquitto_client_پیغام *cmsg_stored = تہی;

	{
		rc = plugin_handle_پیغام_اندر(context, &بنیاد_پیغام->ڈیٹا);
		اگر(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(تہی, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, بنیاد_پیغام->ڈیٹا.qos, بنیاد_پیغام->ڈیٹا.retain, بنیاد_پیغام->ڈیٹا.ماخذ_mid, بنیاد_پیغام->ڈیٹا.topic,
					(طویل)بنیاد_پیغام->ڈیٹا.payloadlen);

			واپس process_bad_پیغام(context, بنیاد_پیغام, MQTT_RC_NOT_AUTHORIZED);
		}ورنہ اگر(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(تہی, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			واپس process_bad_پیغام(context, بنیاد_پیغام, MQTT_RC_QUOTA_EXCEEDED);
		}ورنہ اگر(rc != MOSQ_ERR_SUCCESS){
			db_پیغام_store_free(بنیاد_پیغام);
			واپس rc;
		}
	}

	اگر(بنیاد_پیغام->ڈیٹا.qos > 0){
		db_پیغام_store_ڈھونڈو(context, بنیاد_پیغام->ڈیٹا.ماخذ_mid, &cmsg_stored);
	}

	اگر(cmsg_stored && بنیاد_پیغام->ڈیٹا.ماخذ_mid != 0 &&
			(cmsg_stored->بنیاد_پیغام->ڈیٹا.qos != بنیاد_پیغام->ڈیٹا.qos
			|| cmsg_stored->بنیاد_پیغام->ڈیٹا.payloadlen != بنیاد_پیغام->ڈیٹا.payloadlen
			|| strcmp(cmsg_stored->بنیاد_پیغام->ڈیٹا.topic, بنیاد_پیغام->ڈیٹا.topic)
			|| (بنیاد_پیغام->ڈیٹا.payloadlen > 0 && memcmp(cmsg_stored->بنیاد_پیغام->ڈیٹا.payload, بنیاد_پیغام->ڈیٹا.payload, بنیاد_پیغام->ڈیٹا.payloadlen)))){

		log__printf(تہی, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", بنیاد_پیغام->ڈیٹا.ماخذ_mid, context->id);
		db_پیغام_remove_incoming(context, بنیاد_پیغام->ڈیٹا.ماخذ_mid);
		cmsg_stored = تہی;
	}

	اگر(!cmsg_stored){
		اگر(بنیاد_پیغام->ڈیٹا.qos > 0 && context->msgs_اندر.inflight_quota == 0){
			log__printf(تہی, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#اگر 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_پیغام_store_free(بنیاد_پیغام);
			واپس MOSQ_ERR_RECEIVE_زیادہ_سے_زیادہ_EXCEEDED;
#اگر_ختم
		}

		اگر(بنیاد_پیغام->ڈیٹا.qos == 0
				|| db__ready_for_flight(context, mosq_md_اندر, بنیاد_پیغام->ڈیٹا.qos)
				){

			dup = 0;
			rc = db_پیغام_store(context, بنیاد_پیغام, پیغام_expiry_interval, mosq_mo_client);
			اگر(rc){
				واپس rc;
			}
		}ورنہ{
			/* Client isn't allowed any more incoming messages, so fail early */
			واپس process_bad_پیغام(context, بنیاد_پیغام, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = بنیاد_پیغام;
		بنیاد_پیغام = تہی;
		dup = 0;
	}ورنہ{
		db_پیغام_store_free(بنیاد_پیغام);
		بنیاد_پیغام = تہی;
		stored = cmsg_stored->بنیاد_پیغام;
		cmsg_stored->ڈیٹا.dup++;
		dup = cmsg_stored->ڈیٹا.dup;
	}

	انتخاب(stored->ڈیٹا.qos){
		حالت 0:
			rc2 = sub_messages_قطار(context->id, stored->ڈیٹا.qos, &stored);
			اگر(rc2 > 0){
				rc = rc2;
			}
			توڑو;
		حالت 1:
			util__decrement_receive_quota(context);
			rc2 = sub_messages_قطار(context->id, stored->ڈیٹا.qos, &stored);
			/* stored may now be free, so don't refer to it */
			اگر(rc2 == MOSQ_ERR_SUCCESS || context->protocol != mosq_p_mqtt5){
				rc2 = send__puback(context, mid, 0, تہی);
				اگر(rc2){
					rc = rc2;
				}
			}ورنہ اگر(rc2 == MOSQ_ERR_NO_SUBSCRIBERS){
				rc2 = send__puback(context, mid, MQTT_RC_NO_MATCHING_SUBSCRIBERS, تہی);
				اگر(rc2){
					rc = rc2;
				}
			}ورنہ{
				rc = rc2;
			}
			توڑو;
		حالت 2:
			{
				صحیح res;
				اگر(dup == 0){
					res = db_پیغام_داخل_کرو_incoming(context, 0, stored, true);
				}ورنہ{
					res = 0;
				}

				/* db__message_insert() returns 2 to indicate dropped message
				 * due to queue. This isn't an error so don't disconnect them. */
				/* FIXME - this is no longer necessary due to failing early above */
				اگر(!res){
					اگر(dup == 0 || dup == 1){
						rc2 = send__pubrec(context, stored->ڈیٹا.ماخذ_mid, 0, تہی);
						اگر(rc2){
							rc = rc2;
						}
					}ورنہ{
						log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH with dup = %d.", context->id, dup);
						واپس MOSQ_ERR_PROTOCOL;
					}
				}ورنہ{
					rc = res;
				}
				توڑو;
			}
	}

	db_پیغام_write_queued_اندر(context);
	واپس rc;
}


صحیح handle__publish(ساخت mosquitto *context)
{
	uint8_t dup;
	صحیح rc = 0;
	uint8_t سرخی = context->اندر_packet.command;
	ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام;
	size_t len;
	uint16_t slen;
	حرف *topic_mount;
	mosquitto_property *properties = تہی;
	uint32_t پیغام_expiry_interval = پیغام_EXPIRY_INFINITE;
	صحیح topic_alias = -1;
	uint16_t mid = 0;

	اگر(context->state != mosq_cs_فعال){
		log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		واپس MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	بنیاد_پیغام = mosquitto_calloc(1, حجم(ساخت mosquitto_بنیاد_پیغام));
	اگر(بنیاد_پیغام == تہی){
		واپس MOSQ_ERR_NOMEM;
	}

	dup = (سرخی & 0x08)>>3;
	بنیاد_پیغام->ڈیٹا.qos = (سرخی & 0x06)>>1;
	اگر(dup == 1 && بنیاد_پیغام->ڈیٹا.qos == 0){
		log__printf(تہی, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_پیغام_store_free(بنیاد_پیغام);
		واپس MOSQ_ERR_MALFORMED_PACKET;
	}
	اگر(بنیاد_پیغام->ڈیٹا.qos == 3){
		log__printf(تہی, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_پیغام_store_free(بنیاد_پیغام);
		واپس MOSQ_ERR_MALFORMED_PACKET;
	}
	اگر(بنیاد_پیغام->ڈیٹا.qos > context->max_qos){
		log__printf(تہی, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_پیغام_store_free(بنیاد_پیغام);
		واپس MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	بنیاد_پیغام->ڈیٹا.retain = (سرخی & 0x01);

	اگر(بنیاد_پیغام->ڈیٹا.retain && db.config->retain_دستیاب == false){
		db_پیغام_store_free(بنیاد_پیغام);
		واپس MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	اگر(packet__read_string(&context->اندر_packet, &بنیاد_پیغام->ڈیٹا.topic, &slen)){
		db_پیغام_store_free(بنیاد_پیغام);
		واپس MOSQ_ERR_MALFORMED_PACKET;
	}
	اگر(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_پیغام_store_free(بنیاد_پیغام);
		واپس MOSQ_ERR_MALFORMED_PACKET;
	}

	اگر(بنیاد_پیغام->ڈیٹا.qos > 0){
		اگر(packet__read_uint16(&context->اندر_packet, &mid)){
			db_پیغام_store_free(بنیاد_پیغام);
			واپس MOSQ_ERR_MALFORMED_PACKET;
		}
		اگر(mid == 0){
			db_پیغام_store_free(بنیاد_پیغام);
			log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			واپس MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		بنیاد_پیغام->ڈیٹا.ماخذ_mid = mid;
	}

	/* Handle properties */
	اگر(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->اندر_packet, &properties);
		اگر(rc){
			db_پیغام_store_free(بنیاد_پیغام);
			واپس rc;
		}

		rc = property__process_publish(بنیاد_پیغام, &properties, &topic_alias, &پیغام_expiry_interval, context->bridge);
		اگر(rc){
			mosquitto_property_free_all(&properties);
			db_پیغام_store_free(بنیاد_پیغام);
			واپس MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	اگر(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_پیغام_store_free(بنیاد_پیغام);
		واپس MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}ورنہ اگر(topic_alias > 0){
		اگر(بنیاد_پیغام->ڈیٹا.topic){
			rc = alias_جوڑو_r2l(context, بنیاد_پیغام->ڈیٹا.topic, (uint16_t)topic_alias);
			اگر(rc){
				db_پیغام_store_free(بنیاد_پیغام);
				واپس rc;
			}
		}ورنہ{
			rc = alias_ڈھونڈو_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &بنیاد_پیغام->ڈیٹا.topic);
			اگر(rc){
				db_پیغام_store_free(بنیاد_پیغام);
				log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				واپس MOSQ_ERR_PROTOCOL;
			}
		}
	}

#اگر_تعریف_شدہ WITH_BRIDGE
	rc = bridge_remap_topic_اندر(context, &بنیاد_پیغام->ڈیٹا.topic);
	اگر(rc){
		db_پیغام_store_free(بنیاد_پیغام);
		واپس rc;
	}

#اگر_ختم
	اگر(mosquitto_pub_topic_check(بنیاد_پیغام->ڈیٹا.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_پیغام_store_free(بنیاد_پیغام);
		واپس MOSQ_ERR_MALFORMED_PACKET;
	}

	بنیاد_پیغام->ڈیٹا.payloadlen = context->اندر_packet.بقایا_لمبائی - context->اندر_packet.pos;
	metrics__int_inc(mosq_شمار_pub_بائٹیں_received, بنیاد_پیغام->ڈیٹا.payloadlen);
	اگر(context->listener && context->listener->mount_نقطہ){
		len = لڑی_لمبائی(context->listener->mount_نقطہ) + لڑی_لمبائی(بنیاد_پیغام->ڈیٹا.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		اگر(!topic_mount){
			db_پیغام_store_free(بنیاد_پیغام);
			واپس MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_نقطہ, بنیاد_پیغام->ڈیٹا.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(بنیاد_پیغام->ڈیٹا.topic);
		بنیاد_پیغام->ڈیٹا.topic = topic_mount;
	}

	اگر(بنیاد_پیغام->ڈیٹا.payloadlen){
		اگر(db.config->پیغام_سائز_حد_بندی && بنیاد_پیغام->ڈیٹا.payloadlen > db.config->پیغام_سائز_حد_بندی){
			log__printf(تہی, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, بنیاد_پیغام->ڈیٹا.qos, بنیاد_پیغام->ڈیٹا.retain, بنیاد_پیغام->ڈیٹا.ماخذ_mid, بنیاد_پیغام->ڈیٹا.topic, (طویل)بنیاد_پیغام->ڈیٹا.payloadlen);
			واپس process_bad_پیغام(context, بنیاد_پیغام, MQTT_RC_PACKET_TOO_LARGE);
		}
		بنیاد_پیغام->ڈیٹا.payload = mosquitto_malloc(بنیاد_پیغام->ڈیٹا.payloadlen+1);
		اگر(بنیاد_پیغام->ڈیٹا.payload == تہی){
			db_پیغام_store_free(بنیاد_پیغام);
			واپس MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)بنیاد_پیغام->ڈیٹا.payload)[بنیاد_پیغام->ڈیٹا.payloadlen] = 0;

		اگر(packet_read_بائٹیں(&context->اندر_packet, بنیاد_پیغام->ڈیٹا.payload, بنیاد_پیغام->ڈیٹا.payloadlen)){
			db_پیغام_store_free(بنیاد_پیغام);
			واپس MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			بنیاد_پیغام->ڈیٹا.topic, بنیاد_پیغام->ڈیٹا.payloadlen, بنیاد_پیغام->ڈیٹا.payload,
			بنیاد_پیغام->ڈیٹا.qos, بنیاد_پیغام->ڈیٹا.retain, بنیاد_پیغام->ڈیٹا.properties,
			MOSQ_ACL_WRITE);
	اگر(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(تہی, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, بنیاد_پیغام->ڈیٹا.qos, بنیاد_پیغام->ڈیٹا.retain, بنیاد_پیغام->ڈیٹا.ماخذ_mid, بنیاد_پیغام->ڈیٹا.topic,
				(طویل)بنیاد_پیغام->ڈیٹا.payloadlen);
		واپس process_bad_پیغام(context, بنیاد_پیغام, MQTT_RC_NOT_AUTHORIZED);
	}ورنہ اگر(rc != MOSQ_ERR_SUCCESS){
		db_پیغام_store_free(بنیاد_پیغام);
		واپس rc;
	}

	log__printf(تہی, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, بنیاد_پیغام->ڈیٹا.qos, بنیاد_پیغام->ڈیٹا.retain, بنیاد_پیغام->ڈیٹا.ماخذ_mid, بنیاد_پیغام->ڈیٹا.topic, (طویل)بنیاد_پیغام->ڈیٹا.payloadlen);

	اگر(!لڑی_موازنہ(بنیاد_پیغام->ڈیٹا.topic, "$CONTROL/", 9)){
#اگر_تعریف_شدہ WITH_CONTROL
		rc = control__process(context, بنیاد_پیغام);
		db_پیغام_store_free(بنیاد_پیغام);
		واپس rc;
#ورنہ
		واپس process_bad_پیغام(context, بنیاد_پیغام, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#اگر_ختم
	}

	واپس handle__accepted_publish(context, بنیاد_پیغام, mid, dup, &پیغام_expiry_interval);
}
