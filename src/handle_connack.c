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


static ចំនួនគត់ handle__connack_properties(sa_nracanaa mosquitto *context)
{
	ចំនួនគត់ rc;
	mosquitto_property *properties = NULL;
	uint16_t inflight_maximum;
	uint8_t max_qos = 255;
	uint32_t maximum_packet_size;
	uint16_t server_keepalive;
	uint16_t max_topic_alias;
	uint8_t retain_available;

	rc = property__read_all(CMD_CONNACK, &context->in_packet, &properties);
	ប្រសិនបើ(rc){
		ត្រឡប់ rc;
	}

	/* maximum-qos */
	mosquitto_property_read_byte(properties, MQTT_PROP_MAXIMUM_QOS,
			&max_qos, false);
	ប្រសិនបើ(max_qos != 255){
		context->max_qos = max_qos;
	}

	/* maximum-packet-size */
	ប្រសិនបើ(mosquitto_property_read_int32(properties, MQTT_PROP_MAXIMUM_PACKET_SIZE,
			&maximum_packet_size, false)){

		ប្រសិនបើ(context->maximum_packet_size == 0 || context->maximum_packet_size > maximum_packet_size){
			context->maximum_packet_size = maximum_packet_size;
		}
	}

	/* receive-maximum */
	inflight_maximum = context->msgs_out.inflight_maximum;
	mosquitto_property_read_int16(properties, MQTT_PROP_RECEIVE_MAXIMUM, &inflight_maximum, false);
	ប្រសិនបើ(context->msgs_out.inflight_maximum != inflight_maximum){
		context->msgs_out.inflight_maximum = inflight_maximum;
		db__message_reconnect_reset(context);
	}

	/* retain-available */
	ប្រសិនបើ(mosquitto_property_read_byte(properties, MQTT_PROP_RETAIN_AVAILABLE,
			&retain_available, false)){

		/* Only use broker provided value if the local config is set to available==true */
		ប្រសិនបើ(context->retain_available){
			context->retain_available = retain_available;
		}
	}

	/* server-keepalive */
	ប្រសិនបើ(mosquitto_property_read_int16(properties, MQTT_PROP_SERVER_KEEP_ALIVE,
			&server_keepalive, false)){

		context->keepalive = server_keepalive;
	}

	/* topic-alias-maximum */
	ប្រសិនបើ(mosquitto_property_read_int16(properties, MQTT_PROP_TOPIC_ALIAS_MAXIMUM,
			&max_topic_alias, false)){

		ប្រសិនបើ(max_topic_alias < context->bridge->max_topic_alias){
			context->alias_max_l2r = max_topic_alias;
		}បើមិនដូច្នេះ{
			context->alias_max_l2r = context->bridge->max_topic_alias;
		}
	}

	mosquitto_property_free_all(&properties);

	ត្រឡប់ MOSQ_ERR_SUCCESS;
}


ចំនួនគត់ handle__connack(sa_nracanaa mosquitto *context)
{
	ចំនួនគត់ rc;
	uint8_t connect_acknowledge;
	uint8_t reason_code;

	ប្រសិនបើ(context == NULL){
		ត្រឡប់ MOSQ_ERR_INVAL;
	}
	ប្រសិនបើ(context->bridge == NULL){
		log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: CONNACK when not a bridge.", context->id);
		ត្រឡប់ MOSQ_ERR_PROTOCOL;
	}
	ប្រសិនបើ(context->in_packet.command != CMD_CONNACK){
		ត្រឡប់ MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(NULL, MOSQ_LOG_DEBUG, "Received CONNACK on connection %s.", context->id);
	ប្រសិនបើ(packet__read_byte(&context->in_packet, &connect_acknowledge)){
		ត្រឡប់ MOSQ_ERR_MALFORMED_PACKET;
	}
	ប្រសិនបើ(packet__read_byte(&context->in_packet, &reason_code)){
		ត្រឡប់ MOSQ_ERR_MALFORMED_PACKET;
	}

	ប្រសិនបើ(context->protocol == mosq_p_mqtt5){
		ប្រសិនបើ(context->in_packet.remaining_length == 2 && reason_code == CONNACK_REFUSED_PROTOCOL_VERSION){
			/* We have connected to a MQTT v3.x broker that doesn't support MQTT v5.0
			 * It has correctly replied with a CONNACK code of a bad protocol version.
			 */
			log__printf(NULL, MOSQ_LOG_NOTICE,
					"Warning: Remote bridge %s does not support MQTT v5.0, reconnecting using MQTT v3.1.1.",
					context->bridge->name);

			context->protocol = mosq_p_mqtt311;
			context->bridge->protocol_version = mosq_p_mqtt311;
			ត្រឡប់ MOSQ_ERR_PROTOCOL;
		}

		rc = handle__connack_properties(context);
		ប្រសិនបើ(rc){
			ត្រឡប់ rc;
		}
	}

	ប្រសិនបើ(reason_code == MQTT_RC_SUCCESS){
#ifdef WITH_BRIDGE
		ប្រសិនបើ(context->bridge){
			rc = bridge__on_connect(context);
			ប្រសិនបើ(rc){
				ត្រឡប់ rc;
			}
		}
#endif
		mosquitto__set_state(context, mosq_cs_active);
		rc = db__message_write_queued_out(context);
		ប្រសិនបើ(rc){
			ត្រឡប់ rc;
		}
		rc = db__message_write_inflight_out_all(context);
		ត្រឡប់ rc;
	}បើមិនដូច្នេះ{
		ប្រសិនបើ(context->protocol == mosq_p_mqtt5){
			switch(reason_code){
				pak_sa MQTT_RC_RETAIN_NOT_SUPPORTED:
					context->retain_available = 0;
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: retain not available (will retry)");
					ត្រឡប់ MOSQ_ERR_CONN_LOST;
				pak_sa MQTT_RC_QOS_NOT_SUPPORTED:
					ប្រសិនបើ(context->max_qos != 0){
						context->max_qos--;
					}
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: QoS not supported (will retry)");
					ត្រឡប់ MOSQ_ERR_CONN_LOST;
				default:
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: %s", mosquitto_reason_string(reason_code));
					ត្រឡប់ MOSQ_ERR_CONN_LOST;
			}
		}បើមិនដូច្នេះ{
			switch(reason_code){
				pak_sa CONNACK_REFUSED_PROTOCOL_VERSION:
					ប្រសិនបើ(context->bridge){
						context->bridge->try_private_accepted = false;
					}
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: unacceptable protocol version");
					ត្រឡប់ MOSQ_ERR_CONN_LOST;
				pak_sa CONNACK_REFUSED_IDENTIFIER_REJECTED:
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: identifier rejected");
					ត្រឡប់ MOSQ_ERR_CONN_LOST;
				pak_sa CONNACK_REFUSED_SERVER_UNAVAILABLE:
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: broker unavailable");
					ត្រឡប់ MOSQ_ERR_CONN_LOST;
				pak_sa CONNACK_REFUSED_BAD_USERNAME_PASSWORD:
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: bad user name or password");
					ត្រឡប់ MOSQ_ERR_CONN_LOST;
				pak_sa CONNACK_REFUSED_NOT_AUTHORIZED:
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: not authorised");
					ត្រឡប់ MOSQ_ERR_CONN_LOST;
				default:
					log__printf(NULL, MOSQ_LOG_ERR, "Connection Refused: unknown reason");
					ត្រឡប់ MOSQ_ERR_CONN_LOST;
			}
		}
	}
	ត្រឡប់ MOSQ_ERR_CONN_LOST;
}

