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

#समावेश "config.h"

#समावेश <stdio.h>
#समावेश <string.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "alias_mosq.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "packet_mosq.h"
#समावेश "property_mosq.h"
#समावेश "read_handle.h"
#समावेश "send_mosq.h"
#समावेश "sys_tree.h"
#समावेश "util_mosq.h"


अटल पूर्णांक process_bad_message(काष्ठा mosquitto *context, काष्ठा mosquitto_base_संदेश *base_संदेश, uint8_t reason_code)
{
	पूर्णांक rc = MOSQ_ERR_UNKNOWN;
	अगर(base_संदेश){
		चयन(base_संदेश->तथ्य.qos){
			हाल 0:
				rc = MOSQ_ERR_SUCCESS;
				अवरोध;
			हाल 1:
				अगर(context){
					rc = send__puback(context, base_संदेश->तथ्य.source_mid, reason_code, शून्य);
				}अन्यथा{
					rc = MOSQ_ERR_SUCCESS;
				}
				अवरोध;
			हाल 2:
				अगर(context){
					rc = send__pubrec(context, base_संदेश->तथ्य.source_mid, reason_code, शून्य);
				}अन्यथा{
					rc = MOSQ_ERR_SUCCESS;
				}
				अवरोध;
		}
		db_संदेश_store_free(base_संदेश);
	}
	अगर(context && db.config->max_queued_messages > 0 && context->out_packet_गिनती >= db.config->max_queued_messages){
		rc = MQTT_RC_QUOTA_EXCEEDED;
	}
	वापस rc;
}


पूर्णांक handle__accepted_publish(काष्ठा mosquitto *context, काष्ठा mosquitto_base_संदेश *base_संदेश, uint16_t mid, पूर्णांक dup, uint32_t *message_expiry_interval)
{
	पूर्णांक rc;
	पूर्णांक rc2;
	काष्ठा mosquitto_base_संदेश *stored = शून्य;
	काष्ठा mosquitto_client_संदेश *cmsg_stored = शून्य;

	{
		rc = plugin__handle_message_in(context, &base_संदेश->तथ्य);
		अगर(rc == MOSQ_ERR_ACL_DENIED){
			log__printf(शून्य, MOSQ_LOG_DEBUG,
					"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
					context->id, dup, base_संदेश->तथ्य.qos, base_संदेश->तथ्य.retain, base_संदेश->तथ्य.source_mid, base_संदेश->तथ्य.topic,
					(दीर्घ)base_संदेश->तथ्य.payloadlen);

			वापस process_bad_message(context, base_संदेश, MQTT_RC_NOT_AUTHORIZED);
		}अन्यथा अगर(rc == MOSQ_ERR_QUOTA_EXCEEDED){
			log__printf(शून्य, MOSQ_LOG_DEBUG,
					"Rejected PUBLISH from %s, quota exceeded.", context->id);

			वापस process_bad_message(context, base_संदेश, MQTT_RC_QUOTA_EXCEEDED);
		}अन्यथा अगर(rc != MOSQ_ERR_SUCCESS){
			db_संदेश_store_free(base_संदेश);
			वापस rc;
		}
	}

	अगर(base_संदेश->तथ्य.qos > 0){
		db__message_store_find(context, base_संदेश->तथ्य.source_mid, &cmsg_stored);
	}

	अगर(cmsg_stored && base_संदेश->तथ्य.source_mid != 0 &&
			(cmsg_stored->base_संदेश->तथ्य.qos != base_संदेश->तथ्य.qos
			|| cmsg_stored->base_संदेश->तथ्य.payloadlen != base_संदेश->तथ्य.payloadlen
			|| म_भेद(cmsg_stored->base_संदेश->तथ्य.topic, base_संदेश->तथ्य.topic)
			|| (base_संदेश->तथ्य.payloadlen > 0 && स_भेद(cmsg_stored->base_संदेश->तथ्य.payload, base_संदेश->तथ्य.payload, base_संदेश->तथ्य.payloadlen)))){

		log__printf(शून्य, MOSQ_LOG_WARNING, "Reused message ID %u from %s detected. Clearing from storage.", base_संदेश->तथ्य.source_mid, context->id);
		db__message_remove_incoming(context, base_संदेश->तथ्य.source_mid);
		cmsg_stored = शून्य;
	}

	अगर(!cmsg_stored){
		अगर(base_संदेश->तथ्य.qos > 0 && context->msgs_in.inflight_quota == 0){
			log__printf(शून्य, MOSQ_LOG_WARNING, "Client %s has exceeded its receive-maximum quota. This behaviour must be fixed on the client.", context->id);
#अगर 0
			/* Badly behaving clients like on the esp32 fall foul of this
			 * check, so report it for now but don't disconnect, to give chance
			 * for the bad behaviour to be fixed. */
			/* Client isn't allowed any more incoming messages, so fail early */
			db_संदेश_store_free(base_संदेश);
			वापस MOSQ_ERR_RECEIVE_MAXIMUM_EXCEEDED;
#पूर्ण_अगर
		}

		अगर(base_संदेश->तथ्य.qos == 0
				|| db__ready_for_flight(context, mosq_md_in, base_संदेश->तथ्य.qos)
				){

			dup = 0;
			rc = db__message_store(context, base_संदेश, message_expiry_interval, mosq_mo_client);
			अगर(rc){
				वापस rc;
			}
		}अन्यथा{
			/* Client isn't allowed any more incoming messages, so fail early */
			वापस process_bad_message(context, base_संदेश, MQTT_RC_QUOTA_EXCEEDED);
		}
		stored = base_संदेश;
		base_संदेश = शून्य;
		dup = 0;
	}अन्यथा{
		db_संदेश_store_free(base_संदेश);
		base_संदेश = शून्य;
		stored = cmsg_stored->base_संदेश;
		cmsg_stored->तथ्य.dup++;
		dup = cmsg_stored->तथ्य.dup;
	}

	चयन(stored->तथ्य.qos){
		हाल 0:
			rc2 = sub__messages_queue(context->id, stored->तथ्य.qos, &stored);
			अगर(rc2 > 0){
				rc = rc2;
			}
			अवरोध;
		हाल 1:
			util__decrement_receive_quota(context);
			rc2 = sub__messages_queue(context->id, stored->तथ्य.qos, &stored);
			/* stored may now be free, so don't refer to it */
			अगर(rc2 == MOSQ_ERR_SUCCESS || context->protocol != mosq_p_mqtt5){
				rc2 = send__puback(context, mid, 0, शून्य);
				अगर(rc2){
					rc = rc2;
				}
			}अन्यथा अगर(rc2 == MOSQ_ERR_NO_SUBSCRIBERS){
				rc2 = send__puback(context, mid, MQTT_RC_NO_MATCHING_SUBSCRIBERS, शून्य);
				अगर(rc2){
					rc = rc2;
				}
			}अन्यथा{
				rc = rc2;
			}
			अवरोध;
		हाल 2:
			{
				पूर्णांक res;
				अगर(dup == 0){
					res = db_message_जोड़ो_incoming(context, 0, stored, true);
				}अन्यथा{
					res = 0;
				}

				/* db__message_insert() returns 2 to indicate dropped message
				 * due to queue. This isn't an error so don't disconnect them. */
				/* FIXME - this is no longer necessary due to failing early above */
				अगर(!res){
					अगर(dup == 0 || dup == 1){
						rc2 = send__pubrec(context, stored->तथ्य.source_mid, 0, शून्य);
						अगर(rc2){
							rc = rc2;
						}
					}अन्यथा{
						log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH with dup = %d.", context->id, dup);
						वापस MOSQ_ERR_PROTOCOL;
					}
				}अन्यथा{
					rc = res;
				}
				अवरोध;
			}
	}

	db__message_write_queued_in(context);
	वापस rc;
}


पूर्णांक handle__publish(काष्ठा mosquitto *context)
{
	uint8_t dup;
	पूर्णांक rc = 0;
	uint8_t header = context->in_packet.command;
	काष्ठा mosquitto_base_संदेश *base_संदेश;
	माप_प्रकार len;
	uint16_t slen;
	अक्षर *topic_mount;
	mosquitto_property *properties = शून्य;
	uint32_t message_expiry_interval = संदेश_EXPIRY_INFINITE;
	पूर्णांक topic_alias = -1;
	uint16_t mid = 0;

	अगर(context->state != mosq_cs_active){
		log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH before session is active.", context->id);
		वापस MOSQ_ERR_PROTOCOL;
	}

	context->stats.messages_received++;

	base_संदेश = mosquitto_calloc(1, माप(काष्ठा mosquitto_base_संदेश));
	अगर(base_संदेश == शून्य){
		वापस MOSQ_ERR_NOMEM;
	}

	dup = (header & 0x08)>>3;
	base_संदेश->तथ्य.qos = (header & 0x06)>>1;
	अगर(dup == 1 && base_संदेश->तथ्य.qos == 0){
		log__printf(शून्य, MOSQ_LOG_INFO,
				"Invalid PUBLISH (QoS=0 and DUP=1) from %s, disconnecting.", context->id);
		db_संदेश_store_free(base_संदेश);
		वापस MOSQ_ERR_MALFORMED_PACKET;
	}
	अगर(base_संदेश->तथ्य.qos == 3){
		log__printf(शून्य, MOSQ_LOG_INFO,
				"Invalid QoS in PUBLISH from %s, disconnecting.", context->id);
		db_संदेश_store_free(base_संदेश);
		वापस MOSQ_ERR_MALFORMED_PACKET;
	}
	अगर(base_संदेश->तथ्य.qos > context->max_qos){
		log__printf(शून्य, MOSQ_LOG_INFO,
				"Too high QoS in PUBLISH from %s, disconnecting.", context->id);
		db_संदेश_store_free(base_संदेश);
		वापस MOSQ_ERR_QOS_NOT_SUPPORTED;
	}
	base_संदेश->तथ्य.retain = (header & 0x01);

	अगर(base_संदेश->तथ्य.retain && db.config->retain_available == false){
		db_संदेश_store_free(base_संदेश);
		वापस MOSQ_ERR_RETAIN_NOT_SUPPORTED;
	}

	अगर(packet__read_string(&context->in_packet, &base_संदेश->तथ्य.topic, &slen)){
		db_संदेश_store_free(base_संदेश);
		वापस MOSQ_ERR_MALFORMED_PACKET;
	}
	अगर(!slen && context->protocol != mosq_p_mqtt5){
		/* Invalid publish topic, disconnect client. */
		db_संदेश_store_free(base_संदेश);
		वापस MOSQ_ERR_MALFORMED_PACKET;
	}

	अगर(base_संदेश->तथ्य.qos > 0){
		अगर(packet__read_uint16(&context->in_packet, &mid)){
			db_संदेश_store_free(base_संदेश);
			वापस MOSQ_ERR_MALFORMED_PACKET;
		}
		अगर(mid == 0){
			db_संदेश_store_free(base_संदेश);
			log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH packet with mid = 0.", context->id);
			वापस MOSQ_ERR_PROTOCOL;
		}
		/* It is important to have a separate copy of mid, because msg may be
		 * freed before we want to send a PUBACK/PUBREC. */
		base_संदेश->तथ्य.source_mid = mid;
	}

	/* Handle properties */
	अगर(context->protocol == mosq_p_mqtt5){
		rc = property__read_all(CMD_PUBLISH, &context->in_packet, &properties);
		अगर(rc){
			db_संदेश_store_free(base_संदेश);
			वापस rc;
		}

		rc = property__process_publish(base_संदेश, &properties, &topic_alias, &message_expiry_interval, context->bridge);
		अगर(rc){
			mosquitto_property_free_all(&properties);
			db_संदेश_store_free(base_संदेश);
			वापस MOSQ_ERR_PROTOCOL;
		}
	}
	mosquitto_property_free_all(&properties);

	अगर(topic_alias == 0 || (context->listener && topic_alias > context->listener->max_topic_alias)){
		db_संदेश_store_free(base_संदेश);
		वापस MOSQ_ERR_TOPIC_ALIAS_INVALID;
	}अन्यथा अगर(topic_alias > 0){
		अगर(base_संदेश->तथ्य.topic){
			rc = alias__add_r2l(context, base_संदेश->तथ्य.topic, (uint16_t)topic_alias);
			अगर(rc){
				db_संदेश_store_free(base_संदेश);
				वापस rc;
			}
		}अन्यथा{
			rc = alias__find_by_alias(context, ALIAS_DIR_R2L, (uint16_t)topic_alias, &base_संदेश->तथ्य.topic);
			अगर(rc){
				db_संदेश_store_free(base_संदेश);
				log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: PUBLISH invalid topic alias (%d).",
						context->id, topic_alias);
				वापस MOSQ_ERR_PROTOCOL;
			}
		}
	}

#अगर_घोषित WITH_BRIDGE
	rc = bridge__remap_topic_in(context, &base_संदेश->तथ्य.topic);
	अगर(rc){
		db_संदेश_store_free(base_संदेश);
		वापस rc;
	}

#पूर्ण_अगर
	अगर(mosquitto_pub_topic_check(base_संदेश->तथ्य.topic) != MOSQ_ERR_SUCCESS){
		/* Invalid publish topic, just swallow it. */
		db_संदेश_store_free(base_संदेश);
		वापस MOSQ_ERR_MALFORMED_PACKET;
	}

	base_संदेश->तथ्य.payloadlen = context->in_packet.remaining_लंबाई - context->in_packet.pos;
	metrics__int_inc(mosq_गणक_pub_bytes_received, base_संदेश->तथ्य.payloadlen);
	अगर(context->listener && context->listener->mount_point){
		len = म_माप(context->listener->mount_point) + म_माप(base_संदेश->तथ्य.topic) + 1;
		topic_mount = mosquitto_malloc(len+1);
		अगर(!topic_mount){
			db_संदेश_store_free(base_संदेश);
			वापस MOSQ_ERR_NOMEM;
		}
		snprintf(topic_mount, len, "%s%s", context->listener->mount_point, base_संदेश->तथ्य.topic);
		topic_mount[len] = '\0';

		mosquitto_FREE(base_संदेश->तथ्य.topic);
		base_संदेश->तथ्य.topic = topic_mount;
	}

	अगर(base_संदेश->तथ्य.payloadlen){
		अगर(db.config->message_आकार_limit && base_संदेश->तथ्य.payloadlen > db.config->message_आकार_limit){
			log__printf(शून्य, MOSQ_LOG_DEBUG, "Dropped too large PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_संदेश->तथ्य.qos, base_संदेश->तथ्य.retain, base_संदेश->तथ्य.source_mid, base_संदेश->तथ्य.topic, (दीर्घ)base_संदेश->तथ्य.payloadlen);
			वापस process_bad_message(context, base_संदेश, MQTT_RC_PACKET_TOO_LARGE);
		}
		base_संदेश->तथ्य.payload = mosquitto_malloc(base_संदेश->तथ्य.payloadlen+1);
		अगर(base_संदेश->तथ्य.payload == शून्य){
			db_संदेश_store_free(base_संदेश);
			वापस MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)base_संदेश->तथ्य.payload)[base_संदेश->तथ्य.payloadlen] = 0;

		अगर(packet__read_bytes(&context->in_packet, base_संदेश->तथ्य.payload, base_संदेश->तथ्य.payloadlen)){
			db_संदेश_store_free(base_संदेश);
			वापस MOSQ_ERR_MALFORMED_PACKET;
		}
	}

	/* Check for topic access */
	rc = mosquitto_acl_check(context,
			base_संदेश->तथ्य.topic, base_संदेश->तथ्य.payloadlen, base_संदेश->तथ्य.payload,
			base_संदेश->तथ्य.qos, base_संदेश->तथ्य.retain, base_संदेश->तथ्य.properties,
			MOSQ_ACL_WRITE);
	अगर(rc == MOSQ_ERR_ACL_DENIED){
		log__printf(शून्य, MOSQ_LOG_DEBUG,
				"Denied PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))",
				context->id, dup, base_संदेश->तथ्य.qos, base_संदेश->तथ्य.retain, base_संदेश->तथ्य.source_mid, base_संदेश->तथ्य.topic,
				(दीर्घ)base_संदेश->तथ्य.payloadlen);
		वापस process_bad_message(context, base_संदेश, MQTT_RC_NOT_AUTHORIZED);
	}अन्यथा अगर(rc != MOSQ_ERR_SUCCESS){
		db_संदेश_store_free(base_संदेश);
		वापस rc;
	}

	log__printf(शून्य, MOSQ_LOG_DEBUG, "Received PUBLISH from %s (d%d, q%d, r%d, m%d, '%s', ... (%ld bytes))", context->id, dup, base_संदेश->तथ्य.qos, base_संदेश->तथ्य.retain, base_संदेश->तथ्य.source_mid, base_संदेश->तथ्य.topic, (दीर्घ)base_संदेश->तथ्य.payloadlen);

	अगर(!म_भेदन(base_संदेश->तथ्य.topic, "$CONTROL/", 9)){
#अगर_घोषित WITH_CONTROL
		rc = control__process(context, base_संदेश);
		db_संदेश_store_free(base_संदेश);
		वापस rc;
#अन्यथा
		वापस process_bad_message(context, base_संदेश, MQTT_RC_IMPLEMENTATION_SPECIFIC);
#पूर्ण_अगर
	}

	वापस handle__accepted_publish(context, base_संदेश, mid, dup, &message_expiry_interval);
}
