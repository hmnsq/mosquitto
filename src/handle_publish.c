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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "alias_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "property_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "read_handle.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "send_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sys_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"


ਸਥਿਰ ਪੂਰਨ process_bad_ਸੁਨੇਹਾ(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ, uint8_t reason_ਕੋਡ)
{
	ਪੂਰਨ rc = MOSQ_ERR_UNKNOWN;
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ){
		ਚੋਣ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos){
			ਹਾਲਤ 0:
				rc = MOSQ_ERR_SUCCESS;
				ਰੋਕੋ;
			ਹਾਲਤ 1:
				ਜੇ(context){
					rc = send__puback(context, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid, reason_ਕੋਡ, ਨਿਰਾ);
				}ਨਹੀਂਤਾਂ{
					rc = MOSQ_ERR_SUCCESS;
				}
				ਰੋਕੋ;
			ਹਾਲਤ 2:
				ਜੇ(context){
					rc = send__pubrec(context, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid, reason_ਕੋਡ, ਨਿਰਾ);
				}ਨਹੀਂਤਾਂ{
					rc = MOSQ_ERR_SUCCESS;
				}
				ਰੋਕੋ;
		}
		db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
	}
	ਜੇ(context && db.config->max_queued_messages > 0 && context->ਬਾਹਰਲਾ_packet_ਗਿਣਤੀ >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	ਵਾਪਸ rc;
}


ਪੂਰਨ handle__accepted_publish(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ, uint16_t mid, ਪੂਰਨ dup, uint32_t *ਸੁਨੇਹਾ_expiry_interval)
{
	ਪੂਰਨ rc;
	ਪੂਰਨ rc2;
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *stored = ਨਿਰਾ;
	ਬਣਤਰ mosquitto_client_ਸੁਨੇਹਾ *cmsg_stored = ਨਿਰਾ;

	{
		rc = plugin_handle_ਸੁਨੇਹਾ_ਵਿੱਚ(context, &ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ);
		ਜੇ(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic,
					(ਲੰਮਾ)ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen);

			ਵਾਪਸ process_bad_ਸੁਨੇਹਾ(context, ਅਧਾਰ_ਸੁਨੇਹਾ, MQTT_RC_NOT_AUTHORIZED);
		}ਨਹੀਂਤਾਂ ਜੇ(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			ਵਾਪਸ process_bad_ਸੁਨੇਹਾ(context, ਅਧਾਰ_ਸੁਨੇਹਾ, MQTT_RC_QUOTA_EXCEEDED);
		}ਨਹੀਂਤਾਂ ਜੇ(rc != MOSQ_ERR_SUCCESS){
			db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
			ਵਾਪਸ rc;
		}
	}

	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos > 0){
		db_ਸੁਨੇਹਾ_store_ਲੱਭੋ(context, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid, &cmsg_stored);
	}

	ਜੇ(cmsg_stored && ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid != 0 &&
			(cmsg_stored->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos != ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos
			|| cmsg_stored->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen != ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen
			|| strcmp(cmsg_stored->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic)
			|| (ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen > 0 && memcmp(cmsg_stored->ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen)))){

		log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid, context->id);
		db_ਸੁਨੇਹਾ_remove_incoming(context, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid);
		cmsg_stored = ਨਿਰਾ;
	}

	ਜੇ(!cmsg_stored){
		ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos > 0 && context->msgs_ਵਿੱਚ.inflight_quota == 0){
			log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#ਜੇ 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
			ਵਾਪਸ MOSQ_ERR_RECEIVE_ਵੱਧ_ਤੋਂ_ਵੱਧ_EXCEEDED;
#ਜੇ_ਅੰਤ
		}

		ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos == 0
				|| db__ready_for_flight(context, mosq_md_ਵਿੱਚ, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos)
				){

			dup = 0;
			rc = db_ਸੁਨੇਹਾ_store(context, ਅਧਾਰ_ਸੁਨੇਹਾ, ਸੁਨੇਹਾ_expiry_interval, mosq_mo_client);
			ਜੇ(rc){
				ਵਾਪਸ rc;
			}
		}ਨਹੀਂਤਾਂ{
			/* Client isn't allowed any more incoming messages, so fail early */
			ਵਾਪਸ process_bad_ਸੁਨੇਹਾ(context, ਅਧਾਰ_ਸੁਨੇਹਾ, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = ਅਧਾਰ_ਸੁਨੇਹਾ;
		ਅਧਾਰ_ਸੁਨੇਹਾ = ਨਿਰਾ;
		dup = 0;
	}ਨਹੀਂਤਾਂ{
		db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
		ਅਧਾਰ_ਸੁਨੇਹਾ = ਨਿਰਾ;
		stored = cmsg_stored->ਅਧਾਰ_ਸੁਨੇਹਾ;
		cmsg_stored->ਅੰਕੜੇ.dup++;
		dup = cmsg_stored->ਅੰਕੜੇ.dup;
	}

	ਚੋਣ(stored->ਅੰਕੜੇ.qos){
		ਹਾਲਤ 0:
			rc2 = sub_messages_ਕਤਾਰ(context->id, stored->ਅੰਕੜੇ.qos, &stored);
			ਜੇ(rc2 > 0){
				rc = rc2;
			}
			ਰੋਕੋ;
		ਹਾਲਤ 1:
			util__decrement_receive_quota(context);
			rc2 = sub_messages_ਕਤਾਰ(context->id, stored->ਅੰਕੜੇ.qos, &stored);
			/* stored may now be free, so don't refer to it */
			ਜੇ(rc2 == MOSQ_ERR_SUCCESS || context->protocol != mosq_p_mqtt5){
				rc2 = send__puback(context, mid, 0, ਨਿਰਾ);
				ਜੇ(rc2){
					rc = rc2;
				}
			}ਨਹੀਂਤਾਂ ਜੇ(rc2 == MOSQ_ERR_NO_SUBSCRIBERS){
				rc2 = send__puback(context, mid, MQTT_RC_NO_MATCHING_SUBSCRIBERS, ਨਿਰਾ);
				ਜੇ(rc2){
					rc = rc2;
				}
			}ਨਹੀਂਤਾਂ{
				rc = rc2;
			}
			ਰੋਕੋ;
		ਹਾਲਤ 2:
			{
				ਪੂਰਨ res;
				ਜੇ(dup == 0){
					res = db_ਸੁਨੇਹਾ_ਪਾਓ_incoming(context, 0, stored, true);
				}ਨਹੀਂਤਾਂ{
					res = 0;
				}

				/* db__message_insert() returns 2 to indicate dropped message
				 * due to queue. This isn't an error so don't disconnect them. */
				/* FIXME - this is no longer necessary due to failing early above */
				ਜੇ(!res){
					ਜੇ(dup == 0 || dup == 1){
						rc2 = send__pubrec(context, stored->ਅੰਕੜੇ.ਸਰੋਤ_mid, 0, ਨਿਰਾ);
						ਜੇ(rc2){
							rc = rc2;
						}
					}ਨਹੀਂਤਾਂ{
						log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH with dup = %d.", context->id, dup);
						ਵਾਪਸ MOSQ_ERR_PROTOCOL;
					}
				}ਨਹੀਂਤਾਂ{
					rc = res;
				}
				ਰੋਕੋ;
			}
	}

	db_ਸੁਨੇਹਾ_write_queued_ਵਿੱਚ(context);
	ਵਾਪਸ rc;
}


ਪੂਰਨ handle__publish(ਬਣਤਰ mosquitto *context)
{
	uint8_t dup;
	ਪੂਰਨ rc = 0;
	uint8_t ਸਿਰਲੇਖ = context->ਵਿੱਚ_packet.command;
	ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ;
	size_t len;
	uint16_t slen;
	ਅੱਖਰ *topic_mount;
	mosquitto_property *properties = ਨਿਰਾ;
	uint32_t ਸੁਨੇਹਾ_expiry_interval = ਸੁਨੇਹਾ_EXPIRY_INFINITE;
	ਪੂਰਨ topic_alias = -1;
	uint16_t mid = 0;

	ਜੇ(context->state != mosq_cs_ਸਰਗਰਮ){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		ਵਾਪਸ MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	ਅਧਾਰ_ਸੁਨੇਹਾ = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ));
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	dup = (ਸਿਰਲੇਖ & 0x08)>>3;
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos = (ਸਿਰਲੇਖ & 0x06)>>1;
	ਜੇ(dup == 1 && ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos == 0){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
		ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
	}
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos == 3){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
		ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
	}
	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos > context->max_qos){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
		ਵਾਪਸ MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain = (ਸਿਰਲੇਖ & 0x01);

	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain && db.config->retain_ਉਪਲਬਧ == false){
		db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
		ਵਾਪਸ MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	ਜੇ(packet__read_string(&context->ਵਿੱਚ_packet, &ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic, &slen)){
		db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
		ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
	}
	ਜੇ(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
		ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
	}

	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos > 0){
		ਜੇ(packet__read_uint16(&context->ਵਿੱਚ_packet, &mid)){
			db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
			ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
		}
		ਜੇ(mid == 0){
			db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
			log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			ਵਾਪਸ MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid = mid;
	}

	/* Handle properties */
	ਜੇ(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->ਵਿੱਚ_packet, &properties);
		ਜੇ(rc){
			db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
			ਵਾਪਸ rc;
		}

		rc = property__process_publish(ਅਧਾਰ_ਸੁਨੇਹਾ, &properties, &topic_alias, &ਸੁਨੇਹਾ_expiry_interval, context->bridge);
		ਜੇ(rc){
			mosquitto_property_free_all(&properties);
			db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
			ਵਾਪਸ MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	ਜੇ(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
		ਵਾਪਸ MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}ਨਹੀਂਤਾਂ ਜੇ(topic_alias > 0){
		ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic){
			rc = alias_ਜੋੜੋ_r2l(context, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic, (uint16_t)topic_alias);
			ਜੇ(rc){
				db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
				ਵਾਪਸ rc;
			}
		}ਨਹੀਂਤਾਂ{
			rc = alias_ਲੱਭੋ_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic);
			ਜੇ(rc){
				db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
				log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				ਵਾਪਸ MOSQ_ERR_PROTOCOL;
			}
		}
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
	rc = bridge_remap_topic_ਵਿੱਚ(context, &ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic);
	ਜੇ(rc){
		db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
		ਵਾਪਸ rc;
	}

#ਜੇ_ਅੰਤ
	ਜੇ(mosquitto_pub_topic_check(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
		ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
	}

	ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen = context->ਵਿੱਚ_packet.ਬਚਿਆ_ਲੰਮਾਈ - context->ਵਿੱਚ_packet.pos;
	metrics__int_inc(mosq_counter_pub_ਬਾਈਟਾਂ_received, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen);
	ਜੇ(context->listener && context->listener->mount_ਬਿੰਦੂ){
		len = ਲੜੀ_ਲੰਬਾਈ(context->listener->mount_ਬਿੰਦੂ) + ਲੜੀ_ਲੰਬਾਈ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		ਜੇ(!topic_mount){
			db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_ਬਿੰਦੂ, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic);
		ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic = topic_mount;
	}

	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen){
		ਜੇ(db.config->ਸੁਨੇਹਾ_ਮਾਪ_ਹੱਦਬੰਦੀ && ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen > db.config->ਸੁਨੇਹਾ_ਮਾਪ_ਹੱਦਬੰਦੀ){
			log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic, (ਲੰਮਾ)ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen);
			ਵਾਪਸ process_bad_ਸੁਨੇਹਾ(context, ਅਧਾਰ_ਸੁਨੇਹਾ, MQTT_RC_PACKET_TOO_LARGE);
		}
		ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload = mosquitto_malloc(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen+1);
		ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload == ਨਿਰਾ){
			db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload)[ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen] = 0;

		ਜੇ(packet_read_ਬਾਈਟਾਂ(&context->ਵਿੱਚ_packet, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen)){
			db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
			ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload,
			ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.properties,
			MOSQ_ACL_WRITE);
	ਜੇ(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic,
				(ਲੰਮਾ)ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen);
		ਵਾਪਸ process_bad_ਸੁਨੇਹਾ(context, ਅਧਾਰ_ਸੁਨੇਹਾ, MQTT_RC_NOT_AUTHORIZED);
	}ਨਹੀਂਤਾਂ ਜੇ(rc != MOSQ_ERR_SUCCESS){
		db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
		ਵਾਪਸ rc;
	}

	log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic, (ਲੰਮਾ)ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen);

	ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic, "$CONTROL/", 9)){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_CONTROL
		rc = control__process(context, ਅਧਾਰ_ਸੁਨੇਹਾ);
		db_ਸੁਨੇਹਾ_store_free(ਅਧਾਰ_ਸੁਨੇਹਾ);
		ਵਾਪਸ rc;
#ਨਹੀਂਤਾਂ
		ਵਾਪਸ process_bad_ਸੁਨੇਹਾ(context, ਅਧਾਰ_ਸੁਨੇਹਾ, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#ਜੇ_ਅੰਤ
	}

	ਵਾਪਸ handle__accepted_publish(context, ਅਧਾਰ_ਸੁਨੇਹਾ, mid, dup, &ਸੁਨੇਹਾ_expiry_interval);
}
