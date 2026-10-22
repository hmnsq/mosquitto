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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "alias_mosq.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "packet_mosq.h"
#ಅನ್ತರ್ಭಾವ "property_mosq.h"
#ಅನ್ತರ್ಭಾವ "read_handle.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ process_bad_ಸಂದೇಶ(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ, uint8_t reason_ಸಂಕೇತ)
{
	ಪೂರ್ಣಾಂಕ rc = MOSQ_ERR_UNKNOWN;
	ಆದರೆ(ತಳ_ಸಂದೇಶ){
		ಆಯ್ಕೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos){
			ಸ್ಥಿತಿ 0:
				rc = MOSQ_ERR_SUCCESS;
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 1:
				ಆದರೆ(context){
					rc = send__puback(context, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid, reason_ಸಂಕೇತ, ಖಾಲಿ);
				}ಇಲ್ಲದಿದ್ದರೆ{
					rc = MOSQ_ERR_SUCCESS;
				}
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 2:
				ಆದರೆ(context){
					rc = send__pubrec(context, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid, reason_ಸಂಕೇತ, ಖಾಲಿ);
				}ಇಲ್ಲದಿದ್ದರೆ{
					rc = MOSQ_ERR_SUCCESS;
				}
				ನಿಲ್ಲಿಸು;
		}
		db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
	}
	ಆದರೆ(context && db.config->max_queued_messages > 0 && context->ಹೊರಹೋಗು_packet_ಎಣಿಕೆ >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	ಹಿಂತಿರುಗಿ rc;
}


ಪೂರ್ಣಾಂಕ handle__accepted_publish(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ, uint16_t mid, ಪೂರ್ಣಾಂಕ dup, uint32_t *ಸಂದೇಶ_expiry_interval)
{
	ಪೂರ್ಣಾಂಕ rc;
	ಪೂರ್ಣಾಂಕ rc2;
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *stored = ಖಾಲಿ;
	ರಚನೆ mosquitto_client_ಸಂದೇಶ *cmsg_stored = ಖಾಲಿ;

	{
		rc = plugin_handle_ಸಂದೇಶ_ಒಳ(context, &ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ);
		ಆದರೆ(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic,
					(ದೀರ್ಘ)ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen);

			ಹಿಂತಿರುಗಿ process_bad_ಸಂದೇಶ(context, ತಳ_ಸಂದೇಶ, MQTT_RC_NOT_AUTHORIZED);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			ಹಿಂತಿರುಗಿ process_bad_ಸಂದೇಶ(context, ತಳ_ಸಂದೇಶ, MQTT_RC_QUOTA_EXCEEDED);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
			db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos > 0){
		db_ಸಂದೇಶ_store_ಕಂಡುಹಿಡಿ(context, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid, &cmsg_stored);
	}

	ಆದರೆ(cmsg_stored && ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid != 0 &&
			(cmsg_stored->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos != ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos
			|| cmsg_stored->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen != ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen
			|| strcmp(cmsg_stored->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic)
			|| (ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen > 0 && memcmp(cmsg_stored->ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen)))){

		log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid, context->id);
		db_ಸಂದೇಶ_remove_incoming(context, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid);
		cmsg_stored = ಖಾಲಿ;
	}

	ಆದರೆ(!cmsg_stored){
		ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos > 0 && context->msgs_ಒಳ.inflight_quota == 0){
			log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#ಆದರೆ 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_RECEIVE_ಗರಿಷ್ಠ_EXCEEDED;
#ಆದರೆ_ಅಂತ್ಯ
		}

		ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos == 0
				|| db__ready_for_flight(context, mosq_md_ಒಳ, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos)
				){

			dup = 0;
			rc = db_ಸಂದೇಶ_store(context, ತಳ_ಸಂದೇಶ, ಸಂದೇಶ_expiry_interval, mosq_mo_client);
			ಆದರೆ(rc){
				ಹಿಂತಿರುಗಿ rc;
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			/* Client isn't allowed any more incoming messages, so fail early */
			ಹಿಂತಿರುಗಿ process_bad_ಸಂದೇಶ(context, ತಳ_ಸಂದೇಶ, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = ತಳ_ಸಂದೇಶ;
		ತಳ_ಸಂದೇಶ = ಖಾಲಿ;
		dup = 0;
	}ಇಲ್ಲದಿದ್ದರೆ{
		db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
		ತಳ_ಸಂದೇಶ = ಖಾಲಿ;
		stored = cmsg_stored->ತಳ_ಸಂದೇಶ;
		cmsg_stored->ದತ್ತಾಂಶ.dup++;
		dup = cmsg_stored->ದತ್ತಾಂಶ.dup;
	}

	ಆಯ್ಕೆ(stored->ದತ್ತಾಂಶ.qos){
		ಸ್ಥಿತಿ 0:
			rc2 = sub_messages_ಸರತಿ(context->id, stored->ದತ್ತಾಂಶ.qos, &stored);
			ಆದರೆ(rc2 > 0){
				rc = rc2;
			}
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 1:
			util__decrement_receive_quota(context);
			rc2 = sub_messages_ಸರತಿ(context->id, stored->ದತ್ತಾಂಶ.qos, &stored);
			/* stored may now be free, so don't refer to it */
			ಆದರೆ(rc2 == MOSQ_ERR_SUCCESS || context->protocol != mosq_p_mqtt5){
				rc2 = send__puback(context, mid, 0, ಖಾಲಿ);
				ಆದರೆ(rc2){
					rc = rc2;
				}
			}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc2 == MOSQ_ERR_NO_SUBSCRIBERS){
				rc2 = send__puback(context, mid, MQTT_RC_NO_MATCHING_SUBSCRIBERS, ಖಾಲಿ);
				ಆದರೆ(rc2){
					rc = rc2;
				}
			}ಇಲ್ಲದಿದ್ದರೆ{
				rc = rc2;
			}
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 2:
			{
				ಪೂರ್ಣಾಂಕ res;
				ಆದರೆ(dup == 0){
					res = db_ಸಂದೇಶ_ಸೇರಿಸು_incoming(context, 0, stored, true);
				}ಇಲ್ಲದಿದ್ದರೆ{
					res = 0;
				}

				/* db__message_insert() returns 2 to indicate dropped message
				 * due to queue. This isn't an error so don't disconnect them. */
				/* FIXME - this is no longer necessary due to failing early above */
				ಆದರೆ(!res){
					ಆದರೆ(dup == 0 || dup == 1){
						rc2 = send__pubrec(context, stored->ದತ್ತಾಂಶ.ಮೂಲ_mid, 0, ಖಾಲಿ);
						ಆದರೆ(rc2){
							rc = rc2;
						}
					}ಇಲ್ಲದಿದ್ದರೆ{
						log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH with dup = %d.", context->id, dup);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
					}
				}ಇಲ್ಲದಿದ್ದರೆ{
					rc = res;
				}
				ನಿಲ್ಲಿಸು;
			}
	}

	db_ಸಂದೇಶ_write_queued_ಒಳ(context);
	ಹಿಂತಿರುಗಿ rc;
}


ಪೂರ್ಣಾಂಕ handle__publish(ರಚನೆ mosquitto *context)
{
	uint8_t dup;
	ಪೂರ್ಣಾಂಕ rc = 0;
	uint8_t ಶೀರ್ಷಿಕೆ = context->ಒಳ_packet.command;
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ;
	size_t len;
	uint16_t slen;
	ಅಕ್ಷರ *topic_mount;
	mosquitto_property *properties = ಖಾಲಿ;
	uint32_t ಸಂದೇಶ_expiry_interval = ಸಂದೇಶ_EXPIRY_INFINITE;
	ಪೂರ್ಣಾಂಕ topic_alias = -1;
	uint16_t mid = 0;

	ಆದರೆ(context->state != mosq_cs_ಸಕ್ರಿಯ){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	ತಳ_ಸಂದೇಶ = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ));
	ಆದರೆ(ತಳ_ಸಂದೇಶ == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	dup = (ಶೀರ್ಷಿಕೆ & 0x08)>>3;
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos = (ಶೀರ್ಷಿಕೆ & 0x06)>>1;
	ಆದರೆ(dup == 1 && ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos == 0){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
	}
	ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos == 3){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
	}
	ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos > context->max_qos){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain = (ಶೀರ್ಷಿಕೆ & 0x01);

	ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain && db.config->retain_ಲಭ್ಯ == false){
		db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	ಆದರೆ(packet__read_string(&context->ಒಳ_packet, &ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic, &slen)){
		db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
	}
	ಆದರೆ(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
	}

	ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos > 0){
		ಆದರೆ(packet__read_uint16(&context->ಒಳ_packet, &mid)){
			db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
		}
		ಆದರೆ(mid == 0){
			db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
			log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid = mid;
	}

	/* Handle properties */
	ಆದರೆ(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->ಒಳ_packet, &properties);
		ಆದರೆ(rc){
			db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
			ಹಿಂತಿರುಗಿ rc;
		}

		rc = property__process_publish(ತಳ_ಸಂದೇಶ, &properties, &topic_alias, &ಸಂದೇಶ_expiry_interval, context->bridge);
		ಆದರೆ(rc){
			mosquitto_property_free_all(&properties);
			db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	ಆದರೆ(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(topic_alias > 0){
		ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic){
			rc = alias_ಕೂಡಿಸು_r2l(context, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic, (uint16_t)topic_alias);
			ಆದರೆ(rc){
				db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
				ಹಿಂತಿರುಗಿ rc;
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			rc = alias_ಕಂಡುಹಿಡಿ_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic);
			ಆದರೆ(rc){
				db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
				log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
			}
		}
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
	rc = bridge_remap_topic_ಒಳ(context, &ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic);
	ಆದರೆ(rc){
		db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ rc;
	}

#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ(mosquitto_pub_topic_check(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
	}

	ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen = context->ಒಳ_packet.ಉಳಿದ_ಉದ್ದಳತೆ - context->ಒಳ_packet.pos;
	metrics__int_inc(mosq_counter_pub_ಬೈಟುಗಳು_received, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen);
	ಆದರೆ(context->listener && context->listener->mount_ಬಿಂದು){
		len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->listener->mount_ಬಿಂದು) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		ಆದರೆ(!topic_mount){
			db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_ಬಿಂದು, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic);
		ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic = topic_mount;
	}

	ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen){
		ಆದರೆ(db.config->ಸಂದೇಶ_ಗಾತ್ರ_ಎಲ್ಲೆ && ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen > db.config->ಸಂದೇಶ_ಗಾತ್ರ_ಎಲ್ಲೆ){
			log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic, (ದೀರ್ಘ)ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen);
			ಹಿಂತಿರುಗಿ process_bad_ಸಂದೇಶ(context, ತಳ_ಸಂದೇಶ, MQTT_RC_PACKET_TOO_LARGE);
		}
		ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload = mosquitto_malloc(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen+1);
		ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload == ಖಾಲಿ){
			db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload)[ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen] = 0;

		ಆದರೆ(packet_read_ಬೈಟುಗಳು(&context->ಒಳ_packet, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen)){
			db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload,
			ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.properties,
			MOSQ_ACL_WRITE);
	ಆದರೆ(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic,
				(ದೀರ್ಘ)ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen);
		ಹಿಂತಿರುಗಿ process_bad_ಸಂದೇಶ(context, ತಳ_ಸಂದೇಶ, MQTT_RC_NOT_AUTHORIZED);
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
		db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ rc;
	}

	log__printf(ಖಾಲಿ, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic, (ದೀರ್ಘ)ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen);

	ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic, "$CONTROL/", 9)){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_CONTROL
		rc = control__process(context, ತಳ_ಸಂದೇಶ);
		db_ಸಂದೇಶ_store_free(ತಳ_ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ rc;
#ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ process_bad_ಸಂದೇಶ(context, ತಳ_ಸಂದೇಶ, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#ಆದರೆ_ಅಂತ್ಯ
	}

	ಹಿಂತಿರುಗಿ handle__accepted_publish(context, ತಳ_ಸಂದೇಶ, mid, dup, &ಸಂದೇಶ_expiry_interval);
}
