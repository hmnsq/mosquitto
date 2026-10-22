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

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "alias_mosq.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "packet_mosq.h"
#अन्तर्भाव "property_mosq.h"
#अन्तर्भाव "read_handle.h"
#अन्तर्भाव "send_mosq.h"
#अन्तर्भाव "sys_tree.h"
#अन्तर्भाव "util_mosq.h"


स्थिर पूर्णाङ्क process_bad_सन्देशः(संरचना mosquitto *context, संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः, uint8_t reason_संहिता)
{
	पूर्णाङ्क rc = MOSQ_ERR_UNKNOWN;
	यदि(आधारः_सन्देशः){
		विकल्प(आधारः_सन्देशः->दत्त.qos){
			पक्ष 0:
				rc = MOSQ_ERR_SUCCESS;
				भङ्ग;
			पक्ष 1:
				यदि(context){
					rc = send__puback(context, आधारः_सन्देशः->दत्त.source_mid, reason_संहिता, रिक्त);
				}अन्यथा{
					rc = MOSQ_ERR_SUCCESS;
				}
				भङ्ग;
			पक्ष 2:
				यदि(context){
					rc = send__pubrec(context, आधारः_सन्देशः->दत्त.source_mid, reason_संहिता, रिक्त);
				}अन्यथा{
					rc = MOSQ_ERR_SUCCESS;
				}
				भङ्ग;
		}
		db_सन्देशः_store_free(आधारः_सन्देशः);
	}
	यदि(context && db.config->max_queued_messages > 0 && context->बहिर्गमनम्_packet_गणनम् >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	प्रत्यागम rc;
}


पूर्णाङ्क handle__accepted_publish(संरचना mosquitto *context, संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः, uint16_t mid, पूर्णाङ्क dup, uint32_t *सन्देशः_expiry_interval)
{
	पूर्णाङ्क rc;
	पूर्णाङ्क rc2;
	संरचना mosquitto_आधारः_सन्देशः *stored = रिक्त;
	संरचना mosquitto_client_सन्देशः *cmsg_stored = रिक्त;

	{
		rc = plugin_handle_सन्देशः_अन्तर्(context, &आधारः_सन्देशः->दत्त);
		यदि(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(रिक्त, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, आधारः_सन्देशः->दत्त.qos, आधारः_सन्देशः->दत्त.retain, आधारः_सन्देशः->दत्त.source_mid, आधारः_सन्देशः->दत्त.topic,
					(दीर्घ)आधारः_सन्देशः->दत्त.payloadlen);

			प्रत्यागम process_bad_सन्देशः(context, आधारः_सन्देशः, MQTT_RC_NOT_AUTHORIZED);
		}अन्यथा यदि(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(रिक्त, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			प्रत्यागम process_bad_सन्देशः(context, आधारः_सन्देशः, MQTT_RC_QUOTA_EXCEEDED);
		}अन्यथा यदि(rc != MOSQ_ERR_SUCCESS){
			db_सन्देशः_store_free(आधारः_सन्देशः);
			प्रत्यागम rc;
		}
	}

	यदि(आधारः_सन्देशः->दत्त.qos > 0){
		db_सन्देशः_store_find(context, आधारः_सन्देशः->दत्त.source_mid, &cmsg_stored);
	}

	यदि(cmsg_stored && आधारः_सन्देशः->दत्त.source_mid != 0 &&
			(cmsg_stored->आधारः_सन्देशः->दत्त.qos != आधारः_सन्देशः->दत्त.qos
			|| cmsg_stored->आधारः_सन्देशः->दत्त.payloadlen != आधारः_सन्देशः->दत्त.payloadlen
			|| वाक्यं_तुलय(cmsg_stored->आधारः_सन्देशः->दत्त.topic, आधारः_सन्देशः->दत्त.topic)
			|| (आधारः_सन्देशः->दत्त.payloadlen > 0 && memcmp(cmsg_stored->आधारः_सन्देशः->दत्त.payload, आधारः_सन्देशः->दत्त.payload, आधारः_सन्देशः->दत्त.payloadlen)))){

		log__printf(रिक्त, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", आधारः_सन्देशः->दत्त.source_mid, context->id);
		db_सन्देशः_remove_incoming(context, आधारः_सन्देशः->दत्त.source_mid);
		cmsg_stored = रिक्त;
	}

	यदि(!cmsg_stored){
		यदि(आधारः_सन्देशः->दत्त.qos > 0 && context->msgs_अन्तर्.inflight_quota == 0){
			log__printf(रिक्त, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#यदि 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_सन्देशः_store_free(आधारः_सन्देशः);
			प्रत्यागम MOSQ_ERR_RECEIVE_अधिकतमम्_EXCEEDED;
#यदि_अन्तः
		}

		यदि(आधारः_सन्देशः->दत्त.qos == 0
				|| db__ready_for_flight(context, mosq_md_अन्तर्, आधारः_सन्देशः->दत्त.qos)
				){

			dup = 0;
			rc = db_सन्देशः_store(context, आधारः_सन्देशः, सन्देशः_expiry_interval, mosq_mo_client);
			यदि(rc){
				प्रत्यागम rc;
			}
		}अन्यथा{
			/* Client isn't allowed any more incoming messages, so fail early */
			प्रत्यागम process_bad_सन्देशः(context, आधारः_सन्देशः, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = आधारः_सन्देशः;
		आधारः_सन्देशः = रिक्त;
		dup = 0;
	}अन्यथा{
		db_सन्देशः_store_free(आधारः_सन्देशः);
		आधारः_सन्देशः = रिक्त;
		stored = cmsg_stored->आधारः_सन्देशः;
		cmsg_stored->दत्त.dup++;
		dup = cmsg_stored->दत्त.dup;
	}

	विकल्प(stored->दत्त.qos){
		पक्ष 0:
			rc2 = sub__messages_queue(context->id, stored->दत्त.qos, &stored);
			यदि(rc2 > 0){
				rc = rc2;
			}
			भङ्ग;
		पक्ष 1:
			util__decrement_receive_quota(context);
			rc2 = sub__messages_queue(context->id, stored->दत्त.qos, &stored);
			/* stored may now be free, so don't refer to it */
			यदि(rc2 == MOSQ_ERR_SUCCESS || context->protocol != mosq_p_mqtt5){
				rc2 = send__puback(context, mid, 0, रिक्त);
				यदि(rc2){
					rc = rc2;
				}
			}अन्यथा यदि(rc2 == MOSQ_ERR_NO_SUBSCRIBERS){
				rc2 = send__puback(context, mid, MQTT_RC_NO_MATCHING_SUBSCRIBERS, रिक्त);
				यदि(rc2){
					rc = rc2;
				}
			}अन्यथा{
				rc = rc2;
			}
			भङ्ग;
		पक्ष 2:
			{
				पूर्णाङ्क res;
				यदि(dup == 0){
					res = db_सन्देशः_निवेशय_incoming(context, 0, stored, true);
				}अन्यथा{
					res = 0;
				}

				/* db__message_insert() returns 2 to indicate dropped message
				 * due to queue. This isn't an error so don't disconnect them. */
				/* FIXME - this is no longer necessary due to failing early above */
				यदि(!res){
					यदि(dup == 0 || dup == 1){
						rc2 = send__pubrec(context, stored->दत्त.source_mid, 0, रिक्त);
						यदि(rc2){
							rc = rc2;
						}
					}अन्यथा{
						log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH with dup = %d.", context->id, dup);
						प्रत्यागम MOSQ_ERR_PROTOCOL;
					}
				}अन्यथा{
					rc = res;
				}
				भङ्ग;
			}
	}

	db_सन्देशः_write_queued_अन्तर्(context);
	प्रत्यागम rc;
}


पूर्णाङ्क handle__publish(संरचना mosquitto *context)
{
	uint8_t dup;
	पूर्णाङ्क rc = 0;
	uint8_t शीर्षकम् = context->अन्तर्_packet.command;
	संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः;
	size_t len;
	uint16_t slen;
	वर्ण *topic_mount;
	mosquitto_property *properties = रिक्त;
	uint32_t सन्देशः_expiry_interval = सन्देशः_EXPIRY_INFINITE;
	पूर्णाङ्क topic_alias = -1;
	uint16_t mid = 0;

	यदि(context->state != mosq_cs_सक्रिय){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		प्रत्यागम MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	आधारः_सन्देशः = mosquitto_calloc(1, परिमाण(संरचना mosquitto_आधारः_सन्देशः));
	यदि(आधारः_सन्देशः == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	dup = (शीर्षकम् & 0x08)>>3;
	आधारः_सन्देशः->दत्त.qos = (शीर्षकम् & 0x06)>>1;
	यदि(dup == 1 && आधारः_सन्देशः->दत्त.qos == 0){
		log__printf(रिक्त, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_सन्देशः_store_free(आधारः_सन्देशः);
		प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
	}
	यदि(आधारः_सन्देशः->दत्त.qos == 3){
		log__printf(रिक्त, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_सन्देशः_store_free(आधारः_सन्देशः);
		प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
	}
	यदि(आधारः_सन्देशः->दत्त.qos > context->max_qos){
		log__printf(रिक्त, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_सन्देशः_store_free(आधारः_सन्देशः);
		प्रत्यागम MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	आधारः_सन्देशः->दत्त.retain = (शीर्षकम् & 0x01);

	यदि(आधारः_सन्देशः->दत्त.retain && db.config->retain_उपलब्धम् == false){
		db_सन्देशः_store_free(आधारः_सन्देशः);
		प्रत्यागम MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	यदि(packet__read_string(&context->अन्तर्_packet, &आधारः_सन्देशः->दत्त.topic, &slen)){
		db_सन्देशः_store_free(आधारः_सन्देशः);
		प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
	}
	यदि(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_सन्देशः_store_free(आधारः_सन्देशः);
		प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
	}

	यदि(आधारः_सन्देशः->दत्त.qos > 0){
		यदि(packet__read_uint16(&context->अन्तर्_packet, &mid)){
			db_सन्देशः_store_free(आधारः_सन्देशः);
			प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
		}
		यदि(mid == 0){
			db_सन्देशः_store_free(आधारः_सन्देशः);
			log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			प्रत्यागम MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		आधारः_सन्देशः->दत्त.source_mid = mid;
	}

	/* Handle properties */
	यदि(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->अन्तर्_packet, &properties);
		यदि(rc){
			db_सन्देशः_store_free(आधारः_सन्देशः);
			प्रत्यागम rc;
		}

		rc = property__process_publish(आधारः_सन्देशः, &properties, &topic_alias, &सन्देशः_expiry_interval, context->bridge);
		यदि(rc){
			mosquitto_property_free_all(&properties);
			db_सन्देशः_store_free(आधारः_सन्देशः);
			प्रत्यागम MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	यदि(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_सन्देशः_store_free(आधारः_सन्देशः);
		प्रत्यागम MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}अन्यथा यदि(topic_alias > 0){
		यदि(आधारः_सन्देशः->दत्त.topic){
			rc = alias_सङ्कलय_r2l(context, आधारः_सन्देशः->दत्त.topic, (uint16_t)topic_alias);
			यदि(rc){
				db_सन्देशः_store_free(आधारः_सन्देशः);
				प्रत्यागम rc;
			}
		}अन्यथा{
			rc = alias__find_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &आधारः_सन्देशः->दत्त.topic);
			यदि(rc){
				db_सन्देशः_store_free(आधारः_सन्देशः);
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}
		}
	}

#यदि_परिभाषितम् WITH_BRIDGE
	rc = bridge_remap_topic_अन्तर्(context, &आधारः_सन्देशः->दत्त.topic);
	यदि(rc){
		db_सन्देशः_store_free(आधारः_सन्देशः);
		प्रत्यागम rc;
	}

#यदि_अन्तः
	यदि(mosquitto_pub_topic_check(आधारः_सन्देशः->दत्त.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_सन्देशः_store_free(आधारः_सन्देशः);
		प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
	}

	आधारः_सन्देशः->दत्त.payloadlen = context->अन्तर्_packet.remaining_दीर्घता - context->अन्तर्_packet.pos;
	metrics__int_inc(mosq_गणक_pub_अष्टकानि_received, आधारः_सन्देशः->दत्त.payloadlen);
	यदि(context->listener && context->listener->mount_point){
		len = वाक्यदैर्घ्यम्(context->listener->mount_point) + वाक्यदैर्घ्यम्(आधारः_सन्देशः->दत्त.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		यदि(!topic_mount){
			db_सन्देशः_store_free(आधारः_सन्देशः);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_point, आधारः_सन्देशः->दत्त.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(आधारः_सन्देशः->दत्त.topic);
		आधारः_सन्देशः->दत्त.topic = topic_mount;
	}

	यदि(आधारः_सन्देशः->दत्त.payloadlen){
		यदि(db.config->सन्देशः_आकार_सीमा && आधारः_सन्देशः->दत्त.payloadlen > db.config->सन्देशः_आकार_सीमा){
			log__printf(रिक्त, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, आधारः_सन्देशः->दत्त.qos, आधारः_सन्देशः->दत्त.retain, आधारः_सन्देशः->दत्त.source_mid, आधारः_सन्देशः->दत्त.topic, (दीर्घ)आधारः_सन्देशः->दत्त.payloadlen);
			प्रत्यागम process_bad_सन्देशः(context, आधारः_सन्देशः, MQTT_RC_PACKET_TOO_LARGE);
		}
		आधारः_सन्देशः->दत्त.payload = mosquitto_malloc(आधारः_सन्देशः->दत्त.payloadlen+1);
		यदि(आधारः_सन्देशः->दत्त.payload == रिक्त){
			db_सन्देशः_store_free(आधारः_सन्देशः);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)आधारः_सन्देशः->दत्त.payload)[आधारः_सन्देशः->दत्त.payloadlen] = 0;

		यदि(packet_read_अष्टकानि(&context->अन्तर्_packet, आधारः_सन्देशः->दत्त.payload, आधारः_सन्देशः->दत्त.payloadlen)){
			db_सन्देशः_store_free(आधारः_सन्देशः);
			प्रत्यागम MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			आधारः_सन्देशः->दत्त.topic, आधारः_सन्देशः->दत्त.payloadlen, आधारः_सन्देशः->दत्त.payload,
			आधारः_सन्देशः->दत्त.qos, आधारः_सन्देशः->दत्त.retain, आधारः_सन्देशः->दत्त.properties,
			MOSQ_ACL_WRITE);
	यदि(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(रिक्त, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, आधारः_सन्देशः->दत्त.qos, आधारः_सन्देशः->दत्त.retain, आधारः_सन्देशः->दत्त.source_mid, आधारः_सन्देशः->दत्त.topic,
				(दीर्घ)आधारः_सन्देशः->दत्त.payloadlen);
		प्रत्यागम process_bad_सन्देशः(context, आधारः_सन्देशः, MQTT_RC_NOT_AUTHORIZED);
	}अन्यथा यदि(rc != MOSQ_ERR_SUCCESS){
		db_सन्देशः_store_free(आधारः_सन्देशः);
		प्रत्यागम rc;
	}

	log__printf(रिक्त, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, आधारः_सन्देशः->दत्त.qos, आधारः_सन्देशः->दत्त.retain, आधारः_सन्देशः->दत्त.source_mid, आधारः_सन्देशः->दत्त.topic, (दीर्घ)आधारः_सन्देशः->दत्त.payloadlen);

	यदि(!वाक्ये_तुलय(आधारः_सन्देशः->दत्त.topic, "$CONTROL/", 9)){
#यदि_परिभाषितम् WITH_CONTROL
		rc = control__process(context, आधारः_सन्देशः);
		db_सन्देशः_store_free(आधारः_सन्देशः);
		प्रत्यागम rc;
#अन्यथा
		प्रत्यागम process_bad_सन्देशः(context, आधारः_सन्देशः, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#यदि_अन्तः
	}

	प्रत्यागम handle__accepted_publish(context, आधारः_सन्देशः, mid, dup, &सन्देशः_expiry_interval);
}
