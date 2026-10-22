/*
Copyright (c) 2018-2021 Roger Light <roger@atchoo.org>

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
#ضمن "property_mosq.h"
#ضمن "property_common.h"


/* Process the incoming properties, we should be able to assume that only valid
 * properties for CONNECT are present here. */
صحيح property__process_connect(بنية mosquitto *context, mosquitto_property **props)
{
	mosquitto_property *p;

	p = *props;

	طالما(p){
		بدل(mosquitto_property_identifier(p)){
			حالة MQTT_PROP_SESSION_EXPIRY_INTERVAL:
				context->session_expiry_interval = mosquitto_property_int32_مقدار(p);
				اكسر;

			حالة MQTT_PROP_RECEIVE_بیشینه:
				context->msgs_بیرون.inflight_بیشینه = mosquitto_property_int16_مقدار(p);
				إذا(context->msgs_بیرون.inflight_بیشینه == 0){
					log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with receive-maximum = 0.", context->id);
					أرجع MOSQ_ERR_PROTOCOL;
				}
				context->msgs_بیرون.inflight_quota = context->msgs_بیرون.inflight_بیشینه;
				اكسر;

			حالة MQTT_PROP_بیشینه_PACKET_اندازه:
				context->بیشینه_packet_اندازه = mosquitto_property_int32_مقدار(p);
				إذا(context->بیشینه_packet_اندازه == 0){
					log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with maximum-packet-size = 0.", context->id);
					أرجع MOSQ_ERR_PROTOCOL;
				}
				اكسر;

			حالة MQTT_PROP_TOPIC_ALIAS_بیشینه:
				context->alias_max_l2r = mosquitto_property_int16_مقدار(p);
				إذا(context->alias_max_l2r > context->listener->max_topic_alias_broker){
					context->alias_max_l2r = context->listener->max_topic_alias_broker;
				}
				اكسر;

			افتراضي:
				اكسر;
		}
		p = mosquitto_property_بعدی(p);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح property__process_will(بنية mosquitto *context, بنية mosquitto_پیام_all *پیام, mosquitto_property **props)
{
	mosquitto_property *p, *p_prev;
	mosquitto_property *پیام_properties, *پیام_properties_آخرین;

	p = *props;
	p_prev = عدم;
	پیام_properties = عدم;
	پیام_properties_آخرین = عدم;
	پیام->expiry_interval = پیام_EXPIRY_INFINITE;
	طالما(p){
		بدل(mosquitto_property_identifier(p)){
			حالة MQTT_PROP_CONTENT_TYPE:
			حالة MQTT_PROP_CORRELATION_داده:
			حالة MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			حالة MQTT_PROP_RESPONSE_TOPIC:
			حالة MQTT_PROP_USER_PROPERTY:
				/* We save these properties for transmission with the PUBLISH */

				/* Add this property to the end of the list */
				إذا(پیام_properties){
					پیام_properties_آخرین->بعدی = p;
					پیام_properties_آخرین = p;
				}وإلا{
					پیام_properties = p;
					پیام_properties_آخرین = p;
				}

				/* And remove it from *props */
				إذا(p_prev){
					p_prev->بعدی = mosquitto_property_بعدی(p);
					p = mosquitto_property_بعدی(p_prev);
				}وإلا{
					*props = mosquitto_property_بعدی(p);
					p = *props;
				}
				پیام_properties_آخرین->بعدی = عدم;
				اكسر;

			حالة MQTT_PROP_WILL_DELAY_INTERVAL:
				/* Leave this in *props, to be freed */
				context->will_delay_interval = mosquitto_property_int32_مقدار(p);
				p_prev = p;
				p = mosquitto_property_بعدی(p);
				اكسر;

			حالة MQTT_PROP_پیام_EXPIRY_INTERVAL:
				/* Leave this in *props, to be freed */
				پیام->expiry_interval = mosquitto_property_int32_مقدار(p);
				p_prev = p;
				p = mosquitto_property_بعدی(p);
				اكسر;

			افتراضي:
				پیام->properties = پیام_properties;
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet invalid property (%d).", context->id, p->identifier);
				أرجع MOSQ_ERR_PROTOCOL;
				اكسر;
		}
	}

	پیام->properties = پیام_properties;
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح property__process_publish(بنية mosquitto_پایه_پیام *پایه_پیام, mosquitto_property **props, صحيح *topic_alias, uint32_t *پیام_expiry_interval, bool is_bridge)
{
	mosquitto_property *p, *p_prev;
	mosquitto_property *پیام_properties_آخرین;

	p = *props;
	p_prev = عدم;
	پایه_پیام->داده.properties = عدم;
	پیام_properties_آخرین = عدم;
	طالما(p){
		بدل(mosquitto_property_identifier(p)){
			حالة MQTT_PROP_CONTENT_TYPE:
			حالة MQTT_PROP_CORRELATION_داده:
			حالة MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			حالة MQTT_PROP_RESPONSE_TOPIC:
			حالة MQTT_PROP_USER_PROPERTY:
				إذا(پایه_پیام->داده.properties){
					پیام_properties_آخرین->بعدی = p;
					پیام_properties_آخرین = p;
				}وإلا{
					پایه_پیام->داده.properties = p;
					پیام_properties_آخرین = p;
				}
				إذا(p_prev){
					p_prev->بعدی = mosquitto_property_بعدی(p);
					p = mosquitto_property_بعدی(p_prev);
				}وإلا{
					*props = mosquitto_property_بعدی(p);
					p = *props;
				}
				پیام_properties_آخرین->بعدی = عدم;
				اكسر;

			حالة MQTT_PROP_TOPIC_ALIAS:
				*topic_alias = mosquitto_property_int16_مقدار(p);
				p_prev = p;
				p = mosquitto_property_بعدی(p);
				اكسر;

			حالة MQTT_PROP_پیام_EXPIRY_INTERVAL:
				*پیام_expiry_interval = mosquitto_property_int32_مقدار(p);
				p_prev = p;
				p = mosquitto_property_بعدی(p);
				اكسر;

			حالة MQTT_PROP_SUBSCRIPTION_IDENTIFIER:
				إذا(!is_bridge || mosquitto_property_varint_مقدار(p) == 0){
					أرجع MOSQ_ERR_PROTOCOL;
				}
				p_prev = p;
				p = mosquitto_property_بعدی(p);
				اكسر;

			افتراضي:
				p = mosquitto_property_بعدی(p);
				اكسر;
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


/* Process the incoming properties, we should be able to assume that only valid
 * properties for DISCONNECT are present here. */
صحيح property__process_disconnect(بنية mosquitto *context, mosquitto_property **props)
{
	mosquitto_property *p;

	p = *props;

	طالما(p){
		إذا(mosquitto_property_identifier(p) == MQTT_PROP_SESSION_EXPIRY_INTERVAL){
			uint32_t session_expiry_interval = mosquitto_property_int32_مقدار(p);
			إذا(context->session_expiry_interval == MQTT_SESSION_EXPIRY_فوری
					&& session_expiry_interval != MQTT_SESSION_EXPIRY_فوری){

				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with mismatched session-expiry-interval (%d:%d).",
						context->id, context->session_expiry_interval, p->مقدار.i32);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			context->session_expiry_interval = session_expiry_interval;
		}
		p = mosquitto_property_بعدی(p);
	}
	أرجع MOSQ_ERR_SUCCESS;
}
