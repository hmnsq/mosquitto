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

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "packet_mosq.h"
#ضمن "send_mosq.h"
#ضمن "util_mosq.h"


ساكن صحيح handle__connack_properties(بنية mosquitto *context)
{
	صحيح rc;
	mosquitto_property *properties = عدم;
	uint16_t inflight_بیشینه;
	uint8_t max_qos = 255;
	uint32_t بیشینه_packet_اندازه;
	uint16_t server_keepalive;
	uint16_t max_topic_alias;
	uint8_t retain_موجود;

	rc = property__read_all(CMD_CONNACK, &context->درون_packet, &properties);
	إذا(rc){
		أرجع rc;
	}

	/* maximum-qos */
	mosquitto_property_read_byte(properties, MQTT_PROP_بیشینه_QOS,
			&max_qos, false);
	إذا(max_qos != 255){
		context->max_qos = max_qos;
	}

	/* maximum-packet-size */
	إذا(mosquitto_property_read_int32(properties, MQTT_PROP_بیشینه_PACKET_اندازه,
			&بیشینه_packet_اندازه, false)){

		إذا(context->بیشینه_packet_اندازه == 0 || context->بیشینه_packet_اندازه > بیشینه_packet_اندازه){
			context->بیشینه_packet_اندازه = بیشینه_packet_اندازه;
		}
	}

	/* receive-maximum */
	inflight_بیشینه = context->msgs_بیرون.inflight_بیشینه;
	mosquitto_property_read_int16(properties, MQTT_PROP_RECEIVE_بیشینه, &inflight_بیشینه, false);
	إذا(context->msgs_بیرون.inflight_بیشینه != inflight_بیشینه){
		context->msgs_بیرون.inflight_بیشینه = inflight_بیشینه;
		db_پیام_reconnect_reset(context);
	}

	/* retain-available */
	إذا(mosquitto_property_read_byte(properties, MQTT_PROP_RETAIN_موجود,
			&retain_موجود, false)){

		/* Only use broker provided value if the local config is set to available==true */
		إذا(context->retain_موجود){
			context->retain_موجود = retain_موجود;
		}
	}

	/* server-keepalive */
	إذا(mosquitto_property_read_int16(properties, MQTT_PROP_SERVER_KEEP_ALIVE,
			&server_keepalive, false)){

		context->keepalive = server_keepalive;
	}

	/* topic-alias-maximum */
	إذا(mosquitto_property_read_int16(properties, MQTT_PROP_TOPIC_ALIAS_بیشینه,
			&max_topic_alias, false)){

		إذا(max_topic_alias < context->bridge->max_topic_alias){
			context->alias_max_l2r = max_topic_alias;
		}وإلا{
			context->alias_max_l2r = context->bridge->max_topic_alias;
		}
	}

	mosquitto_property_free_all(&properties);

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح handle__connack(بنية mosquitto *context)
{
	صحيح rc;
	uint8_t connect_acknowledge;
	uint8_t reason_کد;

	إذا(context == عدم){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(context->bridge == عدم){
		log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: CONNACK when not a bridge.", context->id);
		أرجع MOSQ_ERR_PROTOCOL;
	}
	إذا(context->درون_packet.command != CMD_CONNACK){
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}
	log__printf(عدم, MOSQ_LOG_DEBUG, "Received CONNACK on connection %s.", context->id);
	إذا(packet__read_byte(&context->درون_packet, &connect_acknowledge)){
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}
	إذا(packet__read_byte(&context->درون_packet, &reason_کد)){
		أرجع MOSQ_ERR_MALFORMED_PACKET;
	}

	إذا(context->protocol == mosq_p_mqtt5){
		إذا(context->درون_packet.مانده_درازی == 2 && reason_کد == CONNACK_REFUSED_PROTOCOL_نسخه){
			/* We have connected to a MQTT v3.x broker that doesn't support MQTT v5.0
			 * It has correctly replied with a CONNACK code of a bad protocol version.
			 */
			log__printf(عدم, MOSQ_LOG_NOTICE,
					"Warning: Remote bridge %s does not support MQTT v5.0, reconnecting using MQTT v3.1.1.",
					context->bridge->name);

			context->protocol = mosq_p_mqtt311;
			context->bridge->protocol_نسخه = mosq_p_mqtt311;
			أرجع MOSQ_ERR_PROTOCOL;
		}

		rc = handle__connack_properties(context);
		إذا(rc){
			أرجع rc;
		}
	}

	إذا(reason_کد == MQTT_RC_SUCCESS){
#اگر_تعریف_شده WITH_BRIDGE
		إذا(context->bridge){
			rc = bridge__on_connect(context);
			إذا(rc){
				أرجع rc;
			}
		}
#پایان_اگر
		mosquitto_مجموعه_state(context, mosq_cs_فعال);
		rc = db_پیام_write_queued_بیرون(context);
		إذا(rc){
			أرجع rc;
		}
		rc = db_پیام_write_inflight_بیرون_all(context);
		أرجع rc;
	}وإلا{
		إذا(context->protocol == mosq_p_mqtt5){
			بدل(reason_کد){
				حالة MQTT_RC_RETAIN_NOT_SUPPORTED:
					context->retain_موجود = 0;
					log__printf(عدم, MOSQ_LOG_ERR, "Connection Refused: retain not available (will retry)");
					أرجع MOSQ_ERR_CONN_گمشده;
				حالة MQTT_RC_QOS_NOT_SUPPORTED:
					إذا(context->max_qos != 0){
						context->max_qos--;
					}
					log__printf(عدم, MOSQ_LOG_ERR, "Connection Refused: QoS not supported (will retry)");
					أرجع MOSQ_ERR_CONN_گمشده;
				افتراضي:
					log__printf(عدم, MOSQ_LOG_ERR, "Connection Refused: %s", mosquitto_reason_string(reason_کد));
					أرجع MOSQ_ERR_CONN_گمشده;
			}
		}وإلا{
			بدل(reason_کد){
				حالة CONNACK_REFUSED_PROTOCOL_نسخه:
					إذا(context->bridge){
						context->bridge->try_خصوصی_accepted = false;
					}
					log__printf(عدم, MOSQ_LOG_ERR, "Connection Refused: unacceptable protocol version");
					أرجع MOSQ_ERR_CONN_گمشده;
				حالة CONNACK_REFUSED_IDENTIFIER_REJECTED:
					log__printf(عدم, MOSQ_LOG_ERR, "Connection Refused: identifier rejected");
					أرجع MOSQ_ERR_CONN_گمشده;
				حالة CONNACK_REFUSED_SERVER_UNAVAILABLE:
					log__printf(عدم, MOSQ_LOG_ERR, "Connection Refused: broker unavailable");
					أرجع MOSQ_ERR_CONN_گمشده;
				حالة CONNACK_REFUSED_BAD_USERNAME_PASSWORD:
					log__printf(عدم, MOSQ_LOG_ERR, "Connection Refused: bad user name or password");
					أرجع MOSQ_ERR_CONN_گمشده;
				حالة CONNACK_REFUSED_NOT_AUTHORIZED:
					log__printf(عدم, MOSQ_LOG_ERR, "Connection Refused: not authorised");
					أرجع MOSQ_ERR_CONN_گمشده;
				افتراضي:
					log__printf(عدم, MOSQ_LOG_ERR, "Connection Refused: unknown reason");
					أرجع MOSQ_ERR_CONN_گمشده;
			}
		}
	}
	أرجع MOSQ_ERR_CONN_گمشده;
}

