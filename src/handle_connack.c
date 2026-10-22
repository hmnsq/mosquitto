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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "packet_mosq.h"
#ଅନ୍ତର୍ଭାଵ "send_mosq.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା handle__connack_properties(ସଂରଚନା mosquitto *context)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	mosquitto_property *properties = ଖାଲି;
	uint16_t inflight_maximum;
	uint8_t max_qos = 255;
	uint32_t maximum_packet_size;
	uint16_t server_keepalive;
	uint16_t max_topic_alias;
	uint8_t retain_available;

	rc = property__read_all(CMD_CONNACK, &context->in_packet, &properties);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	/* maximum-qos */
	mosquitto_property_read_byte(properties, MQTT_PROP_MAXIMUM_QOS,
			&max_qos, false);
	ଯଦି(max_qos != 255){
		context->max_qos = max_qos;
	}

	/* maximum-packet-size */
	ଯଦି(mosquitto_property_read_int32(properties, MQTT_PROP_MAXIMUM_PACKET_SIZE,
			&maximum_packet_size, false)){

		ଯଦି(context->maximum_packet_size == 0 || context->maximum_packet_size > maximum_packet_size){
			context->maximum_packet_size = maximum_packet_size;
		}
	}

	/* receive-maximum */
	inflight_maximum = context->msgs_out.inflight_maximum;
	mosquitto_property_read_int16(properties, MQTT_PROP_RECEIVE_MAXIMUM, &inflight_maximum, false);
	ଯଦି(context->msgs_out.inflight_maximum != inflight_maximum){
		context->msgs_out.inflight_maximum = inflight_maximum;
		db__message_reconnect_reset(context);
	}

	/* retain-available */
	ଯଦି(mosquitto_property_read_byte(properties, MQTT_PROP_RETAIN_AVAILABLE,
			&retain_available, false)){

		/* Only use broker provided value if the local config is set to available==true */
		ଯଦି(context->retain_available){
			context->retain_available = retain_available;
		}
	}

	/* server-keepalive */
	ଯଦି(mosquitto_property_read_int16(properties, MQTT_PROP_SERVER_KEEP_ALIVE,
			&server_keepalive, false)){

		context->keepalive = server_keepalive;
	}

	/* topic-alias-maximum */
	ଯଦି(mosquitto_property_read_int16(properties, MQTT_PROP_TOPIC_ALIAS_MAXIMUM,
			&max_topic_alias, false)){

		ଯଦି(max_topic_alias < context->bridge->max_topic_alias){
			context->alias_max_l2r = max_topic_alias;
		}ଅନ୍ୟଥା{
			context->alias_max_l2r = context->bridge->max_topic_alias;
		}
	}

	mosquitto_property_free_all(&properties);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା handle__connack(ସଂରଚନା mosquitto *context)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	uint8_t connect_acknowledge;
	uint8_t reason_code;

	ଯଦି(context == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଯଦି(context->bridge == ଖାଲି){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: CONNACK when not a bridge.", context->id);
		ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
	}
	ଯଦି(context->in_packet.command != CMD_CONNACK){
		ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(ଖାଲି, MOSQ_LOG_DEBUG, "Received CONNACK on connection %s.", context->id);
	ଯଦି(packet__read_byte(&context->in_packet, &connect_acknowledge)){
		ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
	}
	ଯଦି(packet__read_byte(&context->in_packet, &reason_code)){
		ଫେରନ୍ତୁ MOSQ_ERR_MALFORMED_PACKET;
	}

	ଯଦି(context->protocol == mosq_p_mqtt5){
		ଯଦି(context->in_packet.remaining_length == 2 && reason_code == CONNACK_REFUSED_PROTOCOL_VERSION){
			/* We have connected to a MQTT v3.x broker that doesn't support MQTT v5.0
			 * It has correctly replied with a CONNACK code of a bad protocol version.
			 */
			log__printf(ଖାଲି, MOSQ_LOG_NOTICE,
					"Warning: Remote bridge %s does not support MQTT v5.0, reconnecting using MQTT v3.1.1.",
					context->bridge->name);

			context->protocol = mosq_p_mqtt311;
			context->bridge->protocol_version = mosq_p_mqtt311;
			ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
		}

		rc = handle__connack_properties(context);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
	}

	ଯଦି(reason_code == MQTT_RC_SUCCESS){
#ifdef WITH_BRIDGE
		ଯଦି(context->bridge){
			rc = bridge__on_connect(context);
			ଯଦି(rc){
				ଫେରନ୍ତୁ rc;
			}
		}
#endif
		mosquitto__set_state(context, mosq_cs_active);
		rc = db__message_write_queued_out(context);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
		rc = db__message_write_inflight_out_all(context);
		ଫେରନ୍ତୁ rc;
	}ଅନ୍ୟଥା{
		ଯଦି(context->protocol == mosq_p_mqtt5){
			ବାଛ(reason_code){
				ସ୍ଥିତି MQTT_RC_RETAIN_NOT_SUPPORTED:
					context->retain_available = 0;
					log__printf(ଖାଲି, MOSQ_LOG_ERR, "Connection Refused: retain not available (will retry)");
					ଫେରନ୍ତୁ MOSQ_ERR_CONN_LOST;
				ସ୍ଥିତି MQTT_RC_QOS_NOT_SUPPORTED:
					ଯଦି(context->max_qos != 0){
						context->max_qos--;
					}
					log__printf(ଖାଲି, MOSQ_LOG_ERR, "Connection Refused: QoS not supported (will retry)");
					ଫେରନ୍ତୁ MOSQ_ERR_CONN_LOST;
				ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
					log__printf(ଖାଲି, MOSQ_LOG_ERR, "Connection Refused: %s", mosquitto_reason_string(reason_code));
					ଫେରନ୍ତୁ MOSQ_ERR_CONN_LOST;
			}
		}ଅନ୍ୟଥା{
			ବାଛ(reason_code){
				ସ୍ଥିତି CONNACK_REFUSED_PROTOCOL_VERSION:
					ଯଦି(context->bridge){
						context->bridge->try_private_accepted = false;
					}
					log__printf(ଖାଲି, MOSQ_LOG_ERR, "Connection Refused: unacceptable protocol version");
					ଫେରନ୍ତୁ MOSQ_ERR_CONN_LOST;
				ସ୍ଥିତି CONNACK_REFUSED_IDENTIFIER_REJECTED:
					log__printf(ଖାଲି, MOSQ_LOG_ERR, "Connection Refused: identifier rejected");
					ଫେରନ୍ତୁ MOSQ_ERR_CONN_LOST;
				ସ୍ଥିତି CONNACK_REFUSED_SERVER_UNAVAILABLE:
					log__printf(ଖାଲି, MOSQ_LOG_ERR, "Connection Refused: broker unavailable");
					ଫେରନ୍ତୁ MOSQ_ERR_CONN_LOST;
				ସ୍ଥିତି CONNACK_REFUSED_BAD_USERNAME_PASSWORD:
					log__printf(ଖାଲି, MOSQ_LOG_ERR, "Connection Refused: bad user name or password");
					ଫେରନ୍ତୁ MOSQ_ERR_CONN_LOST;
				ସ୍ଥିତି CONNACK_REFUSED_NOT_AUTHORIZED:
					log__printf(ଖାଲି, MOSQ_LOG_ERR, "Connection Refused: not authorised");
					ଫେରନ୍ତୁ MOSQ_ERR_CONN_LOST;
				ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
					log__printf(ଖାଲି, MOSQ_LOG_ERR, "Connection Refused: unknown reason");
					ଫେରନ୍ତୁ MOSQ_ERR_CONN_LOST;
			}
		}
	}
	ଫେରନ୍ତୁ MOSQ_ERR_CONN_LOST;
}

