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


static 整数 handle__connack_properties(struct mosquitto *context)
{
	整数 rc;
	mosquitto_property *properties = NULL;
	uint16_t inflight_maximum;
	uint8_t max_qos = 255;
	uint32_t maximum_packet_size;
	uint16_t server_keepalive;
	uint16_t max_topic_alias;
	uint8_t retain_available;

	rc = property__read_all(CMD_CONNACK, &context->in_packet, &properties);
	如果(rc){
		返回 rc;
	}

	/* maximum-qos */
	mosquitto_property_read_byte(properties, MQTT_PROP_MAXIMUM_QOS,
			&max_qos, false);
	如果(max_qos != 255){
		context->max_qos = max_qos;
	}

	/* maximum-packet-size */
	如果(mosquitto_property_read_int32(properties, MQTT_PROP_MAXIMUM_PACKET_SIZE,
			&maximum_packet_size, false)){

		如果(context->maximum_packet_size == 0 || context->maximum_packet_size > maximum_packet_size){
			context->maximum_packet_size = maximum_packet_size;
		}
	}

	/* receive-maximum */
	inflight_maximum = context->msgs_out.inflight_maximum;
	mosquitto_property_read_int16(properties, MQTT_PROP_RECEIVE_MAXIMUM, &inflight_maximum, false);
	如果(context->msgs_out.inflight_maximum != inflight_maximum){
		context->msgs_out.inflight_maximum = inflight_maximum;
		db__message_reconnect_reset(context);
	}

	/* retain-available */
	如果(mosquitto_property_read_byte(properties, MQTT_PROP_RETAIN_AVAILABLE,
			&retain_available, false)){

		/* Only use broker provided value if the local config is set to available==true */
		如果(context->retain_available){
			context->retain_available = retain_available;
		}
	}

	/* server-keepalive */
	如果(mosquitto_property_read_int16(properties, MQTT_PROP_SERVER_KEEP_ALIVE,
			&server_keepalive, false)){

		context->keepalive = server_keepalive;
	}

	/* topic-alias-maximum */
	如果(mosquitto_property_read_int16(properties, MQTT_PROP_TOPIC_ALIAS_MAXIMUM,
			&max_topic_alias, false)){

		如果(max_topic_alias < context->bridge->max_topic_alias){
			context->alias_max_l2r = max_topic_alias;
		}否则{
			context->alias_max_l2r = context->bridge->max_topic_alias;
		}
	}

	mosquitto_property_free_all(&properties);

	返回 MOSQ_ERR_SUCCESS;
}


整数 handle__connack(struct mosquitto *context)
{
	整数 rc;
	uint8_t connect_acknowledge;
	uint8_t reason_code;

	如果(context == NULL){
		返回 MOSQ_ERR_INVAL;
	}
	如果(context->bridge == NULL){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: CONNACK when not a bridge.", context->id);
		返回 MOSQ_ERR_PROTOCOL;
	}
	如果(context->in_packet.command != CMD_CONNACK){
		返回 MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(NULL, MOSQ_LOG_DEBUG, "Received CONNACK on connection %s.", context->id);
	如果(packet__read_byte(&context->in_packet, &connect_acknowledge)){
		返回 MOSQ_ERR_MALFORMED_PACKET;
	}
	如果(packet__read_byte(&context->in_packet, &reason_code)){
		返回 MOSQ_ERR_MALFORMED_PACKET;
	}

	如果(context->protocol == mosq_p_mqtt5){
		如果(context->in_packet.remaining_length == 2 && reason_code == CONNACK_REFUSED_PROTOCOL_VERSION){
			/* We have connected to a MQTT v3.x broker that doesn't support MQTT v5.0
			 * It has correctly replied with a CONNACK code of a bad protocol version.
			 */
			log__printf(NULL, MOSQ_LOG_NOTICE,
					"Warning: Remote bridge %s does not support MQTT v5.0, reconnecting using MQTT v3.1.1.",
					context->bridge->name);

			context->protocol = mosq_p_mqtt311;
			context->bridge->protocol_version = mosq_p_mqtt311;
			返回 MOSQ_ERR_PROTOCOL;
		}

		rc = handle__connack_properties(context);
		如果(rc){
			返回 rc;
		}
	}

	如果(reason_code == MQTT_RC_SUCCESS){
#ifdef WITH_BRIDGE
		如果(context->bridge){
			rc = bridge__on_connect(context);
			如果(rc){
				返回 rc;
			}
		}
#endif
		mosquitto__set_state(context, mosq_cs_active);
		rc = db__message_write_queued_out(context);
		如果(rc){
			返回 rc;
		}
		rc = db__message_write_inflight_out_all(context);
		返回 rc;
	}否则{
		如果(context->protocol == mosq_p_mqtt5){
			switch(reason_code){
				case MQTT_RC_RETAIN_NOT_SUPPORTED:
					context->retain_available = 0;
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: retain not available (will retry)");
					返回 MOSQ_ERR_CONN_LOST;
				case MQTT_RC_QOS_NOT_SUPPORTED:
					如果(context->max_qos != 0){
						context->max_qos--;
					}
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: QoS not supported (will retry)");
					返回 MOSQ_ERR_CONN_LOST;
				default:
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: %s", mosquitto_reason_string(reason_code));
					返回 MOSQ_ERR_CONN_LOST;
			}
		}否则{
			switch(reason_code){
				case CONNACK_REFUSED_PROTOCOL_VERSION:
					如果(context->bridge){
						context->bridge->try_private_accepted = false;
					}
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: unacceptable protocol version");
					返回 MOSQ_ERR_CONN_LOST;
				case CONNACK_REFUSED_IDENTIFIER_REJECTED:
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: identifier rejected");
					返回 MOSQ_ERR_CONN_LOST;
				case CONNACK_REFUSED_SERVER_UNAVAILABLE:
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: broker unavailable");
					返回 MOSQ_ERR_CONN_LOST;
				case CONNACK_REFUSED_BAD_USERNAME_PASSWORD:
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: bad user name or password");
					返回 MOSQ_ERR_CONN_LOST;
				case CONNACK_REFUSED_NOT_AUTHORIZED:
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: not authorised");
					返回 MOSQ_ERR_CONN_LOST;
				default:
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: unknown reason");
					返回 MOSQ_ERR_CONN_LOST;
			}
		}
	}
	返回 MOSQ_ERR_CONN_LOST;
}

