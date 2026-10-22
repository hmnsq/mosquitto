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

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "packet_mosq.h"
#include "send_mosq.h"
#include "util_mosq.h"


static цяло handle__connack_properties(struct mosquitto *context)
{
	цяло rc;
	mosquitto_property *properties = NULL;
	uint16_t inflight_maximum;
	uint8_t max_qos = 255;
	uint32_t maximum_packet_size;
	uint16_t server_keepalive;
	uint16_t max_topic_alias;
	uint8_t retain_available;

	rc = property__read_all(CMD_CONNACK, &context->in_packet, &properties);
	ако(rc){
		върни rc;
	}

	/* maximum-qos */
	mosquitto_property_read_byte(properties, MQTT_PROP_MAXIMUM_QOS,
			&max_qos, false);
	ако(max_qos != 255){
		context->max_qos = max_qos;
	}

	/* maximum-packet-size */
	ако(mosquitto_property_read_int32(properties, MQTT_PROP_MAXIMUM_PACKET_SIZE,
			&maximum_packet_size, false)){

		ако(context->maximum_packet_size == 0 || context->maximum_packet_size > maximum_packet_size){
			context->maximum_packet_size = maximum_packet_size;
		}
	}

	/* receive-maximum */
	inflight_maximum = context->msgs_out.inflight_maximum;
	mosquitto_property_read_int16(properties, MQTT_PROP_RECEIVE_MAXIMUM, &inflight_maximum, false);
	ако(context->msgs_out.inflight_maximum != inflight_maximum){
		context->msgs_out.inflight_maximum = inflight_maximum;
		db__message_reconnect_reset(context);
	}

	/* retain-available */
	ако(mosquitto_property_read_byte(properties, MQTT_PROP_RETAIN_AVAILABLE,
			&retain_available, false)){

		/* Only use broker provided value if the local config is set to available==true */
		ако(context->retain_available){
			context->retain_available = retain_available;
		}
	}

	/* server-keepalive */
	ако(mosquitto_property_read_int16(properties, MQTT_PROP_SERVER_KEEP_ALIVE,
			&server_keepalive, false)){

		context->keepalive = server_keepalive;
	}

	/* topic-alias-maximum */
	ако(mosquitto_property_read_int16(properties, MQTT_PROP_TOPIC_ALIAS_MAXIMUM,
			&max_topic_alias, false)){

		ако(max_topic_alias < context->bridge->max_topic_alias){
			context->alias_max_l2r = max_topic_alias;
		}иначе{
			context->alias_max_l2r = context->bridge->max_topic_alias;
		}
	}

	mosquitto_property_free_all(&properties);

	върни MOSQ_ERR_SUCCESS;
}


цяло handle__connack(struct mosquitto *context)
{
	цяло rc;
	uint8_t connect_acknowledge;
	uint8_t reason_code;

	ако(context == NULL){
		върни MOSQ_ERR_INVAL;
	}
	ако(context->bridge == NULL){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: CONNACK when not a bridge.", context->id);
		върни MOSQ_ERR_PROTOCOL;
	}
	ако(context->in_packet.command != CMD_CONNACK){
		върни MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(NULL, MOSQ_LOG_DEBUG, "Received CONNACK on connection %s.", context->id);
	ако(packet__read_byte(&context->in_packet, &connect_acknowledge)){
		върни MOSQ_ERR_MALFORMED_PACKET;
	}
	ако(packet__read_byte(&context->in_packet, &reason_code)){
		върни MOSQ_ERR_MALFORMED_PACKET;
	}

	ако(context->protocol == mosq_p_mqtt5){
		ако(context->in_packet.remaining_length == 2 && reason_code == CONNACK_REFUSED_PROTOCOL_VERSION){
			/* We have connected to a MQTT v3.x broker that doesn't support MQTT v5.0
			 * It has correctly replied with a CONNACK code of a bad protocol version.
			 */
			log__printf(NULL, MOSQ_LOG_NOTICE,
					"Warning: Remote bridge %s does not support MQTT v5.0, reconnecting using MQTT v3.1.1.",
					context->bridge->name);

			context->protocol = mosq_p_mqtt311;
			context->bridge->protocol_version = mosq_p_mqtt311;
			върни MOSQ_ERR_PROTOCOL;
		}

		rc = handle__connack_properties(context);
		ако(rc){
			върни rc;
		}
	}

	ако(reason_code == MQTT_RC_SUCCESS){
#ifdef WITH_BRIDGE
		ако(context->bridge){
			rc = bridge__on_connect(context);
			ако(rc){
				върни rc;
			}
		}
#endif
		mosquitto__set_state(context, mosq_cs_active);
		rc = db__message_write_queued_out(context);
		ако(rc){
			върни rc;
		}
		rc = db__message_write_inflight_out_all(context);
		върни rc;
	}иначе{
		ако(context->protocol == mosq_p_mqtt5){
			switch(reason_code){
				case MQTT_RC_RETAIN_NOT_SUPPORTED:
					context->retain_available = 0;
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: retain not available (will retry)");
					върни MOSQ_ERR_CONN_LOST;
				case MQTT_RC_QOS_NOT_SUPPORTED:
					ако(context->max_qos != 0){
						context->max_qos--;
					}
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: QoS not supported (will retry)");
					върни MOSQ_ERR_CONN_LOST;
				default:
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: %s", mosquitto_reason_string(reason_code));
					върни MOSQ_ERR_CONN_LOST;
			}
		}иначе{
			switch(reason_code){
				case CONNACK_REFUSED_PROTOCOL_VERSION:
					ако(context->bridge){
						context->bridge->try_private_accepted = false;
					}
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: unacceptable protocol version");
					върни MOSQ_ERR_CONN_LOST;
				case CONNACK_REFUSED_IDENTIFIER_REJECTED:
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: identifier rejected");
					върни MOSQ_ERR_CONN_LOST;
				case CONNACK_REFUSED_SERVER_UNAVAILABLE:
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: broker unavailable");
					върни MOSQ_ERR_CONN_LOST;
				case CONNACK_REFUSED_BAD_USERNAME_PASSWORD:
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: bad user name or password");
					върни MOSQ_ERR_CONN_LOST;
				case CONNACK_REFUSED_NOT_AUTHORIZED:
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: not authorised");
					върни MOSQ_ERR_CONN_LOST;
				default:
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: unknown reason");
					върни MOSQ_ERR_CONN_LOST;
			}
		}
	}
	върни MOSQ_ERR_CONN_LOST;
}

