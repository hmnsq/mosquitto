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
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "packet_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "send_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"


ਸਥਿਰ ਪੂਰਨ handle__connack_properties(ਬਣਤਰ mosquitto *context)
{
	ਪੂਰਨ rc;
	mosquitto_property *properties = ਨਿਰਾ;
	uint16_t inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ;
	uint8_t max_qos = 255;
	uint32_t ਵੱਧ_ਤੋਂ_ਵੱਧ_packet_ਮਾਪ;
	uint16_t server_keepalive;
	uint16_t max_topic_alias;
	uint8_t retain_ਉਪਲਬਧ;

	rc = property__read_all(CMD_CONNACK, &context->ਵਿੱਚ_packet, &properties);
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	/* maximum-qos */
	mosquitto_property_read_byte(properties, MQTT_PROP_ਵੱਧ_ਤੋਂ_ਵੱਧ_QOS,
			&max_qos, false);
	ਜੇ(max_qos != 255){
		context->max_qos = max_qos;
	}

	/* maximum-packet-size */
	ਜੇ(mosquitto_property_read_int32(properties, MQTT_PROP_ਵੱਧ_ਤੋਂ_ਵੱਧ_PACKET_ਮਾਪ,
			&ਵੱਧ_ਤੋਂ_ਵੱਧ_packet_ਮਾਪ, false)){

		ਜੇ(context->ਵੱਧ_ਤੋਂ_ਵੱਧ_packet_ਮਾਪ == 0 || context->ਵੱਧ_ਤੋਂ_ਵੱਧ_packet_ਮਾਪ > ਵੱਧ_ਤੋਂ_ਵੱਧ_packet_ਮਾਪ){
			context->ਵੱਧ_ਤੋਂ_ਵੱਧ_packet_ਮਾਪ = ਵੱਧ_ਤੋਂ_ਵੱਧ_packet_ਮਾਪ;
		}
	}

	/* receive-maximum */
	inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ = context->msgs_ਬਾਹਰਲਾ.inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ;
	mosquitto_property_read_int16(properties, MQTT_PROP_RECEIVE_ਵੱਧ_ਤੋਂ_ਵੱਧ, &inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ, false);
	ਜੇ(context->msgs_ਬਾਹਰਲਾ.inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ != inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ){
		context->msgs_ਬਾਹਰਲਾ.inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ = inflight_ਵੱਧ_ਤੋਂ_ਵੱਧ;
		db_ਸੁਨੇਹਾ_reconnect_reset(context);
	}

	/* retain-available */
	ਜੇ(mosquitto_property_read_byte(properties, MQTT_PROP_RETAIN_ਉਪਲਬਧ,
			&retain_ਉਪਲਬਧ, false)){

		/* Only use broker provided value if the local config is set to available==true */
		ਜੇ(context->retain_ਉਪਲਬਧ){
			context->retain_ਉਪਲਬਧ = retain_ਉਪਲਬਧ;
		}
	}

	/* server-keepalive */
	ਜੇ(mosquitto_property_read_int16(properties, MQTT_PROP_SERVER_KEEP_ALIVE,
			&server_keepalive, false)){

		context->keepalive = server_keepalive;
	}

	/* topic-alias-maximum */
	ਜੇ(mosquitto_property_read_int16(properties, MQTT_PROP_TOPIC_ALIAS_ਵੱਧ_ਤੋਂ_ਵੱਧ,
			&max_topic_alias, false)){

		ਜੇ(max_topic_alias < context->bridge->max_topic_alias){
			context->alias_max_l2r = max_topic_alias;
		}ਨਹੀਂਤਾਂ{
			context->alias_max_l2r = context->bridge->max_topic_alias;
		}
	}

	mosquitto_property_free_all(&properties);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ handle__connack(ਬਣਤਰ mosquitto *context)
{
	ਪੂਰਨ rc;
	uint8_t connect_acknowledge;
	uint8_t reason_ਕੋਡ;

	ਜੇ(context == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਜੇ(context->bridge == ਨਿਰਾ){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Protocol error from %s: CONNACK when not a bridge.", context->id);
		ਵਾਪਸ MOSQ_ERR_PROTOCOL;
	}
	ਜੇ(context->ਵਿੱਚ_packet.command != CMD_CONNACK){
		ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(ਨਿਰਾ, MOSQ_LOG_DEBUG, "Received CONNACK on connection %s.", context->id);
	ਜੇ(packet__read_byte(&context->ਵਿੱਚ_packet, &connect_acknowledge)){
		ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
	}
	ਜੇ(packet__read_byte(&context->ਵਿੱਚ_packet, &reason_ਕੋਡ)){
		ਵਾਪਸ MOSQ_ERR_MALFORMED_PACKET;
	}

	ਜੇ(context->protocol == mosq_p_mqtt5){
		ਜੇ(context->ਵਿੱਚ_packet.ਬਚਿਆ_ਲੰਮਾਈ == 2 && reason_ਕੋਡ == CONNACK_REFUSED_PROTOCOL_ਸੰਸਕਰਣ){
			/* We have connected to a MQTT v3.x broker that doesn't support MQTT v5.0
			 * It has correctly replied with a CONNACK code of a bad protocol version.
			 */
			log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE,
					"Warning: Remote bridge %s does not support MQTT v5.0, reconnecting using MQTT v3.1.1.",
					context->bridge->name);

			context->protocol = mosq_p_mqtt311;
			context->bridge->protocol_ਸੰਸਕਰਣ = mosq_p_mqtt311;
			ਵਾਪਸ MOSQ_ERR_PROTOCOL;
		}

		rc = handle__connack_properties(context);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
	}

	ਜੇ(reason_ਕੋਡ == MQTT_RC_SUCCESS){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
		ਜੇ(context->bridge){
			rc = bridge__on_connect(context);
			ਜੇ(rc){
				ਵਾਪਸ rc;
			}
		}
#ਜੇ_ਅੰਤ
		mosquitto_ਸੈੱਟ_state(context, mosq_cs_ਸਰਗਰਮ);
		rc = db_ਸੁਨੇਹਾ_write_queued_ਬਾਹਰਲਾ(context);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
		rc = db_ਸੁਨੇਹਾ_write_inflight_ਬਾਹਰਲਾ_all(context);
		ਵਾਪਸ rc;
	}ਨਹੀਂਤਾਂ{
		ਜੇ(context->protocol == mosq_p_mqtt5){
			ਚੋਣ(reason_ਕੋਡ){
				ਹਾਲਤ MQTT_RC_RETAIN_NOT_SUPPORTED:
					context->retain_ਉਪਲਬਧ = 0;
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Connection Refused: retain not available (will retry)");
					ਵਾਪਸ MOSQ_ERR_CONN_ਗੁਆਚਾ;
				ਹਾਲਤ MQTT_RC_QOS_NOT_SUPPORTED:
					ਜੇ(context->max_qos != 0){
						context->max_qos--;
					}
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Connection Refused: QoS not supported (will retry)");
					ਵਾਪਸ MOSQ_ERR_CONN_ਗੁਆਚਾ;
				ਮੂਲ:
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Connection Refused: %s", mosquitto_reason_string(reason_ਕੋਡ));
					ਵਾਪਸ MOSQ_ERR_CONN_ਗੁਆਚਾ;
			}
		}ਨਹੀਂਤਾਂ{
			ਚੋਣ(reason_ਕੋਡ){
				ਹਾਲਤ CONNACK_REFUSED_PROTOCOL_ਸੰਸਕਰਣ:
					ਜੇ(context->bridge){
						context->bridge->try_ਨਿੱਜੀ_accepted = false;
					}
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Connection Refused: unacceptable protocol version");
					ਵਾਪਸ MOSQ_ERR_CONN_ਗੁਆਚਾ;
				ਹਾਲਤ CONNACK_REFUSED_IDENTIFIER_REJECTED:
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Connection Refused: identifier rejected");
					ਵਾਪਸ MOSQ_ERR_CONN_ਗੁਆਚਾ;
				ਹਾਲਤ CONNACK_REFUSED_SERVER_UNAVAILABLE:
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Connection Refused: broker unavailable");
					ਵਾਪਸ MOSQ_ERR_CONN_ਗੁਆਚਾ;
				ਹਾਲਤ CONNACK_REFUSED_BAD_USERNAME_PASSWORD:
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Connection Refused: bad user name or password");
					ਵਾਪਸ MOSQ_ERR_CONN_ਗੁਆਚਾ;
				ਹਾਲਤ CONNACK_REFUSED_NOT_AUTHORIZED:
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Connection Refused: not authorised");
					ਵਾਪਸ MOSQ_ERR_CONN_ਗੁਆਚਾ;
				ਮੂਲ:
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Connection Refused: unknown reason");
					ਵਾਪਸ MOSQ_ERR_CONN_ਗੁਆਚਾ;
			}
		}
	}
	ਵਾਪਸ MOSQ_ERR_CONN_ਗੁਆਚਾ;
}

