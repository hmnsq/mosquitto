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
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "packet_mosq.h"
#समावेश "send_mosq.h"
#समावेश "util_mosq.h"


अटल पूर्णांक handle__connack_properties(काष्ठा mosquitto *context)
{
	पूर्णांक rc;
	mosquitto_property *properties = शून्य;
	uint16_t inflight_maximum;
	uint8_t max_qos = 255;
	uint32_t maximum_packet_आकार;
	uint16_t server_keepalive;
	uint16_t max_topic_alias;
	uint8_t retain_available;

	rc = property__read_all(CMD_CONNACK, &context->in_packet, &properties);
	अगर(rc){
		वापस rc;
	}

	/* maximum-qos */
	mosquitto_property_read_byte(properties, MQTT_PROP_MAXIMUM_QOS,
			&max_qos, false);
	अगर(max_qos != 255){
		context->max_qos = max_qos;
	}

	/* maximum-packet-size */
	अगर(mosquitto_property_read_int32(properties, MQTT_PROP_MAXIMUM_PACKET_आकार,
			&maximum_packet_आकार, false)){

		अगर(context->maximum_packet_आकार == 0 || context->maximum_packet_आकार > maximum_packet_आकार){
			context->maximum_packet_आकार = maximum_packet_आकार;
		}
	}

	/* receive-maximum */
	inflight_maximum = context->msgs_out.inflight_maximum;
	mosquitto_property_read_int16(properties, MQTT_PROP_RECEIVE_MAXIMUM, &inflight_maximum, false);
	अगर(context->msgs_out.inflight_maximum != inflight_maximum){
		context->msgs_out.inflight_maximum = inflight_maximum;
		db__message_reconnect_reset(context);
	}

	/* retain-available */
	अगर(mosquitto_property_read_byte(properties, MQTT_PROP_RETAIN_AVAILABLE,
			&retain_available, false)){

		/* Only use broker provided value if the local config is set to available==true */
		अगर(context->retain_available){
			context->retain_available = retain_available;
		}
	}

	/* server-keepalive */
	अगर(mosquitto_property_read_int16(properties, MQTT_PROP_SERVER_KEEP_ALIVE,
			&server_keepalive, false)){

		context->keepalive = server_keepalive;
	}

	/* topic-alias-maximum */
	अगर(mosquitto_property_read_int16(properties, MQTT_PROP_TOPIC_ALIAS_MAXIMUM,
			&max_topic_alias, false)){

		अगर(max_topic_alias < context->bridge->max_topic_alias){
			context->alias_max_l2r = max_topic_alias;
		}अन्यथा{
			context->alias_max_l2r = context->bridge->max_topic_alias;
		}
	}

	mosquitto_property_free_all(&properties);

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक handle__connack(काष्ठा mosquitto *context)
{
	पूर्णांक rc;
	uint8_t connect_acknowledge;
	uint8_t reason_code;

	अगर(context == शून्य){
		वापस MOSQ_ERR_INVAL;
	}
	अगर(context->bridge == शून्य){
		log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: CONNACK when not a bridge.", context->id);
		वापस MOSQ_ERR_PROTOCOL;
	}
	अगर(context->in_packet.command != CMD_CONNACK){
		वापस MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(शून्य, MOSQ_LOG_DEBUG, "Received CONNACK on connection %s.", context->id);
	अगर(packet__read_byte(&context->in_packet, &connect_acknowledge)){
		वापस MOSQ_ERR_MALFORMED_PACKET;
	}
	अगर(packet__read_byte(&context->in_packet, &reason_code)){
		वापस MOSQ_ERR_MALFORMED_PACKET;
	}

	अगर(context->protocol == mosq_p_mqtt5){
		अगर(context->in_packet.remaining_लंबाई == 2 && reason_code == CONNACK_REFUSED_PROTOCOL_VERSION){
			/* We have connected to a MQTT v3.x broker that doesn't support MQTT v5.0
			 * It has correctly replied with a CONNACK code of a bad protocol version.
			 */
			log__printf(शून्य, MOSQ_LOG_NOTICE,
					"Warning: Remote bridge %s does not support MQTT v5.0, reconnecting using MQTT v3.1.1.",
					context->bridge->name);

			context->protocol = mosq_p_mqtt311;
			context->bridge->protocol_version = mosq_p_mqtt311;
			वापस MOSQ_ERR_PROTOCOL;
		}

		rc = handle__connack_properties(context);
		अगर(rc){
			वापस rc;
		}
	}

	अगर(reason_code == MQTT_RC_SUCCESS){
#अगर_घोषित WITH_BRIDGE
		अगर(context->bridge){
			rc = bridge__on_connect(context);
			अगर(rc){
				वापस rc;
			}
		}
#पूर्ण_अगर
		mosquitto__set_state(context, mosq_cs_active);
		rc = db__message_write_queued_out(context);
		अगर(rc){
			वापस rc;
		}
		rc = db__message_write_inflight_out_all(context);
		वापस rc;
	}अन्यथा{
		अगर(context->protocol == mosq_p_mqtt5){
			चयन(reason_code){
				हाल MQTT_RC_RETAIN_NOT_SUPPORTED:
					context->retain_available = 0;
					log__printf(शून्य, MOSQ_LOG_ERR, "Connection Refused: retain not available (will retry)");
					वापस MOSQ_ERR_CONN_LOST;
				हाल MQTT_RC_QOS_NOT_SUPPORTED:
					अगर(context->max_qos != 0){
						context->max_qos--;
					}
					log__printf(शून्य, MOSQ_LOG_ERR, "Connection Refused: QoS not supported (will retry)");
					वापस MOSQ_ERR_CONN_LOST;
				शेष:
					log__printf(शून्य, MOSQ_LOG_ERR, "Connection Refused: %s", mosquitto_reason_string(reason_code));
					वापस MOSQ_ERR_CONN_LOST;
			}
		}अन्यथा{
			चयन(reason_code){
				हाल CONNACK_REFUSED_PROTOCOL_VERSION:
					अगर(context->bridge){
						context->bridge->try_private_accepted = false;
					}
					log__printf(शून्य, MOSQ_LOG_ERR, "Connection Refused: unacceptable protocol version");
					वापस MOSQ_ERR_CONN_LOST;
				हाल CONNACK_REFUSED_IDENTIFIER_REJECTED:
					log__printf(शून्य, MOSQ_LOG_ERR, "Connection Refused: identifier rejected");
					वापस MOSQ_ERR_CONN_LOST;
				हाल CONNACK_REFUSED_SERVER_UNAVAILABLE:
					log__printf(शून्य, MOSQ_LOG_ERR, "Connection Refused: broker unavailable");
					वापस MOSQ_ERR_CONN_LOST;
				हाल CONNACK_REFUSED_BAD_USERNAME_PASSWORD:
					log__printf(शून्य, MOSQ_LOG_ERR, "Connection Refused: bad user name or password");
					वापस MOSQ_ERR_CONN_LOST;
				हाल CONNACK_REFUSED_NOT_AUTHORIZED:
					log__printf(शून्य, MOSQ_LOG_ERR, "Connection Refused: not authorised");
					वापस MOSQ_ERR_CONN_LOST;
				शेष:
					log__printf(शून्य, MOSQ_LOG_ERR, "Connection Refused: unknown reason");
					वापस MOSQ_ERR_CONN_LOST;
			}
		}
	}
	वापस MOSQ_ERR_CONN_LOST;
}

