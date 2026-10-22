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

#అన్తర్భావ "config.h"

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "alias_mosq.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "packet_mosq.h"
#అన్తర్భావ "property_mosq.h"
#అన్తర్భావ "read_handle.h"
#అన్తర్భావ "send_mosq.h"
#అన్తర్భావ "sys_tree.h"
#అన్తర్భావ "util_mosq.h"


స్థిర పూర్ణాంకం process_bad_సందేశం(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం, uint8_t reason_సంకేతం)
{
	పూర్ణాంకం rc = MOSQ_ERR_UNKNOWN;
	ఒకవేళ(పునాది_సందేశం){
		ఎంపిక(పునాది_సందేశం->దత్తాంశం.qos){
			స్థితి 0:
				rc = MOSQ_ERR_SUCCESS;
				ఆపు;
			స్థితి 1:
				ఒకవేళ(context){
					rc = send__puback(context, పునాది_సందేశం->దత్తాంశం.ఆధారం_mid, reason_సంకేతం, ఖాళీ);
				}లేకపోతే{
					rc = MOSQ_ERR_SUCCESS;
				}
				ఆపు;
			స్థితి 2:
				ఒకవేళ(context){
					rc = send__pubrec(context, పునాది_సందేశం->దత్తాంశం.ఆధారం_mid, reason_సంకేతం, ఖాళీ);
				}లేకపోతే{
					rc = MOSQ_ERR_SUCCESS;
				}
				ఆపు;
		}
		db_సందేశం_store_free(పునాది_సందేశం);
	}
	ఒకవేళ(context && db.config->max_queued_messages > 0 && context->బహిర్గతం_packet_లెక్క >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	తిరిగి rc;
}


పూర్ణాంకం handle__accepted_publish(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం, uint16_t mid, పూర్ణాంకం dup, uint32_t *సందేశం_expiry_interval)
{
	పూర్ణాంకం rc;
	పూర్ణాంకం rc2;
	నిర్మాణం mosquitto_పునాది_సందేశం *stored = ఖాళీ;
	నిర్మాణం mosquitto_client_సందేశం *cmsg_stored = ఖాళీ;

	{
		rc = plugin_handle_సందేశం_లోపల(context, &పునాది_సందేశం->దత్తాంశం);
		ఒకవేళ(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(ఖాళీ, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, పునాది_సందేశం->దత్తాంశం.qos, పునాది_సందేశం->దత్తాంశం.retain, పునాది_సందేశం->దత్తాంశం.ఆధారం_mid, పునాది_సందేశం->దత్తాంశం.topic,
					(దీర్ఘ)పునాది_సందేశం->దత్తాంశం.payloadlen);

			తిరిగి process_bad_సందేశం(context, పునాది_సందేశం, MQTT_RC_NOT_AUTHORIZED);
		}లేకపోతే ఒకవేళ(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(ఖాళీ, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			తిరిగి process_bad_సందేశం(context, పునాది_సందేశం, MQTT_RC_QUOTA_EXCEEDED);
		}లేకపోతే ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
			db_సందేశం_store_free(పునాది_సందేశం);
			తిరిగి rc;
		}
	}

	ఒకవేళ(పునాది_సందేశం->దత్తాంశం.qos > 0){
		db_సందేశం_store_కనుగొను(context, పునాది_సందేశం->దత్తాంశం.ఆధారం_mid, &cmsg_stored);
	}

	ఒకవేళ(cmsg_stored && పునాది_సందేశం->దత్తాంశం.ఆధారం_mid != 0 &&
			(cmsg_stored->పునాది_సందేశం->దత్తాంశం.qos != పునాది_సందేశం->దత్తాంశం.qos
			|| cmsg_stored->పునాది_సందేశం->దత్తాంశం.payloadlen != పునాది_సందేశం->దత్తాంశం.payloadlen
			|| strcmp(cmsg_stored->పునాది_సందేశం->దత్తాంశం.topic, పునాది_సందేశం->దత్తాంశం.topic)
			|| (పునాది_సందేశం->దత్తాంశం.payloadlen > 0 && memcmp(cmsg_stored->పునాది_సందేశం->దత్తాంశం.payload, పునాది_సందేశం->దత్తాంశం.payload, పునాది_సందేశం->దత్తాంశం.payloadlen)))){

		log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", పునాది_సందేశం->దత్తాంశం.ఆధారం_mid, context->id);
		db_సందేశం_remove_incoming(context, పునాది_సందేశం->దత్తాంశం.ఆధారం_mid);
		cmsg_stored = ఖాళీ;
	}

	ఒకవేళ(!cmsg_stored){
		ఒకవేళ(పునాది_సందేశం->దత్తాంశం.qos > 0 && context->msgs_లోపల.inflight_quota == 0){
			log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#ఒకవేళ 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_సందేశం_store_free(పునాది_సందేశం);
			తిరిగి MOSQ_ERR_RECEIVE_గరిష్ఠ_EXCEEDED;
#అయితే_ముగింపు
		}

		ఒకవేళ(పునాది_సందేశం->దత్తాంశం.qos == 0
				|| db__ready_for_flight(context, mosq_md_లోపల, పునాది_సందేశం->దత్తాంశం.qos)
				){

			dup = 0;
			rc = db_సందేశం_store(context, పునాది_సందేశం, సందేశం_expiry_interval, mosq_mo_client);
			ఒకవేళ(rc){
				తిరిగి rc;
			}
		}లేకపోతే{
			/* Client isn't allowed any more incoming messages, so fail early */
			తిరిగి process_bad_సందేశం(context, పునాది_సందేశం, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = పునాది_సందేశం;
		పునాది_సందేశం = ఖాళీ;
		dup = 0;
	}లేకపోతే{
		db_సందేశం_store_free(పునాది_సందేశం);
		పునాది_సందేశం = ఖాళీ;
		stored = cmsg_stored->పునాది_సందేశం;
		cmsg_stored->దత్తాంశం.dup++;
		dup = cmsg_stored->దత్తాంశం.dup;
	}

	ఎంపిక(stored->దత్తాంశం.qos){
		స్థితి 0:
			rc2 = sub_messages_వరుస(context->id, stored->దత్తాంశం.qos, &stored);
			ఒకవేళ(rc2 > 0){
				rc = rc2;
			}
			ఆపు;
		స్థితి 1:
			util__decrement_receive_quota(context);
			rc2 = sub_messages_వరుస(context->id, stored->దత్తాంశం.qos, &stored);
			/* stored may now be free, so don't refer to it */
			ఒకవేళ(rc2 == MOSQ_ERR_SUCCESS || context->protocol != mosq_p_mqtt5){
				rc2 = send__puback(context, mid, 0, ఖాళీ);
				ఒకవేళ(rc2){
					rc = rc2;
				}
			}లేకపోతే ఒకవేళ(rc2 == MOSQ_ERR_NO_SUBSCRIBERS){
				rc2 = send__puback(context, mid, MQTT_RC_NO_MATCHING_SUBSCRIBERS, ఖాళీ);
				ఒకవేళ(rc2){
					rc = rc2;
				}
			}లేకపోతే{
				rc = rc2;
			}
			ఆపు;
		స్థితి 2:
			{
				పూర్ణాంకం res;
				ఒకవేళ(dup == 0){
					res = db_సందేశం_చొప్పించు_incoming(context, 0, stored, true);
				}లేకపోతే{
					res = 0;
				}

				/* db__message_insert() returns 2 to indicate dropped message
				 * due to queue. This isn't an error so don't disconnect them. */
				/* FIXME - this is no longer necessary due to failing early above */
				ఒకవేళ(!res){
					ఒకవేళ(dup == 0 || dup == 1){
						rc2 = send__pubrec(context, stored->దత్తాంశం.ఆధారం_mid, 0, ఖాళీ);
						ఒకవేళ(rc2){
							rc = rc2;
						}
					}లేకపోతే{
						log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH with dup = %d.", context->id, dup);
						తిరిగి MOSQ_ERR_PROTOCOL;
					}
				}లేకపోతే{
					rc = res;
				}
				ఆపు;
			}
	}

	db_సందేశం_write_queued_లోపల(context);
	తిరిగి rc;
}


పూర్ణాంకం handle__publish(నిర్మాణం mosquitto *context)
{
	uint8_t dup;
	పూర్ణాంకం rc = 0;
	uint8_t శీర్షిక = context->లోపల_packet.command;
	నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం;
	size_t len;
	uint16_t slen;
	అక్షరం *topic_mount;
	mosquitto_property *properties = ఖాళీ;
	uint32_t సందేశం_expiry_interval = సందేశం_EXPIRY_INFINITE;
	పూర్ణాంకం topic_alias = -1;
	uint16_t mid = 0;

	ఒకవేళ(context->state != mosq_cs_చురుకైన){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		తిరిగి MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	పునాది_సందేశం = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto_పునాది_సందేశం));
	ఒకవేళ(పునాది_సందేశం == ఖాళీ){
		తిరిగి MOSQ_ERR_NOMEM;
	}

	dup = (శీర్షిక & 0x08)>>3;
	పునాది_సందేశం->దత్తాంశం.qos = (శీర్షిక & 0x06)>>1;
	ఒకవేళ(dup == 1 && పునాది_సందేశం->దత్తాంశం.qos == 0){
		log__printf(ఖాళీ, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_సందేశం_store_free(పునాది_సందేశం);
		తిరిగి MOSQ_ERR_MALFORMED_PACKET;
	}
	ఒకవేళ(పునాది_సందేశం->దత్తాంశం.qos == 3){
		log__printf(ఖాళీ, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_సందేశం_store_free(పునాది_సందేశం);
		తిరిగి MOSQ_ERR_MALFORMED_PACKET;
	}
	ఒకవేళ(పునాది_సందేశం->దత్తాంశం.qos > context->max_qos){
		log__printf(ఖాళీ, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_సందేశం_store_free(పునాది_సందేశం);
		తిరిగి MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	పునాది_సందేశం->దత్తాంశం.retain = (శీర్షిక & 0x01);

	ఒకవేళ(పునాది_సందేశం->దత్తాంశం.retain && db.config->retain_అందుబాటు == false){
		db_సందేశం_store_free(పునాది_సందేశం);
		తిరిగి MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	ఒకవేళ(packet__read_string(&context->లోపల_packet, &పునాది_సందేశం->దత్తాంశం.topic, &slen)){
		db_సందేశం_store_free(పునాది_సందేశం);
		తిరిగి MOSQ_ERR_MALFORMED_PACKET;
	}
	ఒకవేళ(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_సందేశం_store_free(పునాది_సందేశం);
		తిరిగి MOSQ_ERR_MALFORMED_PACKET;
	}

	ఒకవేళ(పునాది_సందేశం->దత్తాంశం.qos > 0){
		ఒకవేళ(packet__read_uint16(&context->లోపల_packet, &mid)){
			db_సందేశం_store_free(పునాది_సందేశం);
			తిరిగి MOSQ_ERR_MALFORMED_PACKET;
		}
		ఒకవేళ(mid == 0){
			db_సందేశం_store_free(పునాది_సందేశం);
			log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			తిరిగి MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		పునాది_సందేశం->దత్తాంశం.ఆధారం_mid = mid;
	}

	/* Handle properties */
	ఒకవేళ(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->లోపల_packet, &properties);
		ఒకవేళ(rc){
			db_సందేశం_store_free(పునాది_సందేశం);
			తిరిగి rc;
		}

		rc = property__process_publish(పునాది_సందేశం, &properties, &topic_alias, &సందేశం_expiry_interval, context->bridge);
		ఒకవేళ(rc){
			mosquitto_property_free_all(&properties);
			db_సందేశం_store_free(పునాది_సందేశం);
			తిరిగి MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	ఒకవేళ(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_సందేశం_store_free(పునాది_సందేశం);
		తిరిగి MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}లేకపోతే ఒకవేళ(topic_alias > 0){
		ఒకవేళ(పునాది_సందేశం->దత్తాంశం.topic){
			rc = alias_కూడు_r2l(context, పునాది_సందేశం->దత్తాంశం.topic, (uint16_t)topic_alias);
			ఒకవేళ(rc){
				db_సందేశం_store_free(పునాది_సందేశం);
				తిరిగి rc;
			}
		}లేకపోతే{
			rc = alias_కనుగొను_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &పునాది_సందేశం->దత్తాంశం.topic);
			ఒకవేళ(rc){
				db_సందేశం_store_free(పునాది_సందేశం);
				log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				తిరిగి MOSQ_ERR_PROTOCOL;
			}
		}
	}

#నిర్వచితమైతే WITH_BRIDGE
	rc = bridge_remap_topic_లోపల(context, &పునాది_సందేశం->దత్తాంశం.topic);
	ఒకవేళ(rc){
		db_సందేశం_store_free(పునాది_సందేశం);
		తిరిగి rc;
	}

#అయితే_ముగింపు
	ఒకవేళ(mosquitto_pub_topic_check(పునాది_సందేశం->దత్తాంశం.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_సందేశం_store_free(పునాది_సందేశం);
		తిరిగి MOSQ_ERR_MALFORMED_PACKET;
	}

	పునాది_సందేశం->దత్తాంశం.payloadlen = context->లోపల_packet.మిగిలిన_నిడివి - context->లోపల_packet.pos;
	metrics__int_inc(mosq_counter_pub_బైట్లు_received, పునాది_సందేశం->దత్తాంశం.payloadlen);
	ఒకవేళ(context->listener && context->listener->mount_బిందువు){
		len = అక్షరశ్రేణి_పొడవు(context->listener->mount_బిందువు) + అక్షరశ్రేణి_పొడవు(పునాది_సందేశం->దత్తాంశం.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		ఒకవేళ(!topic_mount){
			db_సందేశం_store_free(పునాది_సందేశం);
			తిరిగి MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_బిందువు, పునాది_సందేశం->దత్తాంశం.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(పునాది_సందేశం->దత్తాంశం.topic);
		పునాది_సందేశం->దత్తాంశం.topic = topic_mount;
	}

	ఒకవేళ(పునాది_సందేశం->దత్తాంశం.payloadlen){
		ఒకవేళ(db.config->సందేశం_కొలత_హద్దు && పునాది_సందేశం->దత్తాంశం.payloadlen > db.config->సందేశం_కొలత_హద్దు){
			log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, పునాది_సందేశం->దత్తాంశం.qos, పునాది_సందేశం->దత్తాంశం.retain, పునాది_సందేశం->దత్తాంశం.ఆధారం_mid, పునాది_సందేశం->దత్తాంశం.topic, (దీర్ఘ)పునాది_సందేశం->దత్తాంశం.payloadlen);
			తిరిగి process_bad_సందేశం(context, పునాది_సందేశం, MQTT_RC_PACKET_TOO_LARGE);
		}
		పునాది_సందేశం->దత్తాంశం.payload = mosquitto_malloc(పునాది_సందేశం->దత్తాంశం.payloadlen+1);
		ఒకవేళ(పునాది_సందేశం->దత్తాంశం.payload == ఖాళీ){
			db_సందేశం_store_free(పునాది_సందేశం);
			తిరిగి MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)పునాది_సందేశం->దత్తాంశం.payload)[పునాది_సందేశం->దత్తాంశం.payloadlen] = 0;

		ఒకవేళ(packet_read_బైట్లు(&context->లోపల_packet, పునాది_సందేశం->దత్తాంశం.payload, పునాది_సందేశం->దత్తాంశం.payloadlen)){
			db_సందేశం_store_free(పునాది_సందేశం);
			తిరిగి MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			పునాది_సందేశం->దత్తాంశం.topic, పునాది_సందేశం->దత్తాంశం.payloadlen, పునాది_సందేశం->దత్తాంశం.payload,
			పునాది_సందేశం->దత్తాంశం.qos, పునాది_సందేశం->దత్తాంశం.retain, పునాది_సందేశం->దత్తాంశం.properties,
			MOSQ_ACL_WRITE);
	ఒకవేళ(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(ఖాళీ, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, పునాది_సందేశం->దత్తాంశం.qos, పునాది_సందేశం->దత్తాంశం.retain, పునాది_సందేశం->దత్తాంశం.ఆధారం_mid, పునాది_సందేశం->దత్తాంశం.topic,
				(దీర్ఘ)పునాది_సందేశం->దత్తాంశం.payloadlen);
		తిరిగి process_bad_సందేశం(context, పునాది_సందేశం, MQTT_RC_NOT_AUTHORIZED);
	}లేకపోతే ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
		db_సందేశం_store_free(పునాది_సందేశం);
		తిరిగి rc;
	}

	log__printf(ఖాళీ, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, పునాది_సందేశం->దత్తాంశం.qos, పునాది_సందేశం->దత్తాంశం.retain, పునాది_సందేశం->దత్తాంశం.ఆధారం_mid, పునాది_సందేశం->దత్తాంశం.topic, (దీర్ఘ)పునాది_సందేశం->దత్తాంశం.payloadlen);

	ఒకవేళ(!అక్షరశ్రేణి_పోలిక(పునాది_సందేశం->దత్తాంశం.topic, "$CONTROL/", 9)){
#నిర్వచితమైతే WITH_CONTROL
		rc = control__process(context, పునాది_సందేశం);
		db_సందేశం_store_free(పునాది_సందేశం);
		తిరిగి rc;
#లేకపోతే
		తిరిగి process_bad_సందేశం(context, పునాది_సందేశం, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#అయితే_ముగింపు
	}

	తిరిగి handle__accepted_publish(context, పునాది_సందేశం, mid, dup, &సందేశం_expiry_interval);
}
