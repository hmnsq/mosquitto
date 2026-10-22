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

	بينما(p){
		بدل(mosquitto_property_identifier(p)){
			حالة MQTT_PROP_SESSION_EXPIRY_INTERVAL:
				context->session_expiry_interval = mosquitto_property_int32_قيمة(p);
				اكسر;

			حالة MQTT_PROP_RECEIVE_MAXIMUM:
				context->msgs_out.inflight_maximum = mosquitto_property_int16_قيمة(p);
				إذا(context->msgs_out.inflight_maximum == 0){
					log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with receive-maximum = 0.", context->id);
					إرجاع MOSQ_ERR_PROTOCOL;
				}
				context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;
				اكسر;

			حالة MQTT_PROP_MAXIMUM_PACKET_SIZE:
				context->maximum_packet_size = mosquitto_property_int32_قيمة(p);
				إذا(context->maximum_packet_size == 0){
					log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with maximum-packet-size = 0.", context->id);
					إرجاع MOSQ_ERR_PROTOCOL;
				}
				اكسر;

			حالة MQTT_PROP_TOPIC_ALIAS_MAXIMUM:
				context->alias_max_l2r = mosquitto_property_int16_قيمة(p);
				إذا(context->alias_max_l2r > context->listener->max_topic_alias_broker){
					context->alias_max_l2r = context->listener->max_topic_alias_broker;
				}
				اكسر;

			افتراضي:
				اكسر;
		}
		p = mosquitto_property_next(p);
	}

	إرجاع MOSQ_ERR_SUCCESS;
}


صحيح property__process_will(بنية mosquitto *context, بنية mosquitto_message_all *رسالة, mosquitto_property **props)
{
	mosquitto_property *p, *p_prev;
	mosquitto_property *رسالة_properties, *رسالة_properties_last;

	p = *props;
	p_prev = عدم;
	رسالة_properties = عدم;
	رسالة_properties_last = عدم;
	رسالة->expiry_interval = رسالة_EXPIRY_INFINITE;
	بينما(p){
		بدل(mosquitto_property_identifier(p)){
			حالة MQTT_PROP_CONTENT_TYPE:
			حالة MQTT_PROP_CORRELATION_DATA:
			حالة MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			حالة MQTT_PROP_RESPONSE_TOPIC:
			حالة MQTT_PROP_USER_PROPERTY:
				/* We save these properties for transmission with the PUBLISH */

				/* Add this property to the end of the list */
				إذا(رسالة_properties){
					رسالة_properties_last->next = p;
					رسالة_properties_last = p;
				}وإلا{
					رسالة_properties = p;
					رسالة_properties_last = p;
				}

				/* And remove it from *props */
				إذا(p_prev){
					p_prev->next = mosquitto_property_next(p);
					p = mosquitto_property_next(p_prev);
				}وإلا{
					*props = mosquitto_property_next(p);
					p = *props;
				}
				رسالة_properties_last->next = عدم;
				اكسر;

			حالة MQTT_PROP_WILL_DELAY_INTERVAL:
				/* Leave this in *props, to be freed */
				context->will_delay_interval = mosquitto_property_int32_قيمة(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				اكسر;

			حالة MQTT_PROP_MESSAGE_EXPIRY_INTERVAL:
				/* Leave this in *props, to be freed */
				رسالة->expiry_interval = mosquitto_property_int32_قيمة(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				اكسر;

			افتراضي:
				رسالة->properties = رسالة_properties;
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet invalid property (%d).", context->id, p->identifier);
				إرجاع MOSQ_ERR_PROTOCOL;
				اكسر;
		}
	}

	رسالة->properties = رسالة_properties;
	إرجاع MOSQ_ERR_SUCCESS;
}


صحيح property__process_publish(بنية mosquitto_base_رسالة *base_رسالة, mosquitto_property **props, صحيح *topic_alias, uint32_t *message_expiry_interval, bool is_bridge)
{
	mosquitto_property *p, *p_prev;
	mosquitto_property *رسالة_properties_last;

	p = *props;
	p_prev = عدم;
	base_رسالة->data.properties = عدم;
	رسالة_properties_last = عدم;
	بينما(p){
		بدل(mosquitto_property_identifier(p)){
			حالة MQTT_PROP_CONTENT_TYPE:
			حالة MQTT_PROP_CORRELATION_DATA:
			حالة MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			حالة MQTT_PROP_RESPONSE_TOPIC:
			حالة MQTT_PROP_USER_PROPERTY:
				إذا(base_رسالة->data.properties){
					رسالة_properties_last->next = p;
					رسالة_properties_last = p;
				}وإلا{
					base_رسالة->data.properties = p;
					رسالة_properties_last = p;
				}
				إذا(p_prev){
					p_prev->next = mosquitto_property_next(p);
					p = mosquitto_property_next(p_prev);
				}وإلا{
					*props = mosquitto_property_next(p);
					p = *props;
				}
				رسالة_properties_last->next = عدم;
				اكسر;

			حالة MQTT_PROP_TOPIC_ALIAS:
				*topic_alias = mosquitto_property_int16_قيمة(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				اكسر;

			حالة MQTT_PROP_MESSAGE_EXPIRY_INTERVAL:
				*message_expiry_interval = mosquitto_property_int32_قيمة(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				اكسر;

			حالة MQTT_PROP_SUBSCRIPTION_IDENTIFIER:
				إذا(!is_bridge || mosquitto_property_varint_قيمة(p) == 0){
					إرجاع MOSQ_ERR_PROTOCOL;
				}
				p_prev = p;
				p = mosquitto_property_next(p);
				اكسر;

			افتراضي:
				p = mosquitto_property_next(p);
				اكسر;
		}
	}

	إرجاع MOSQ_ERR_SUCCESS;
}


/* Process the incoming properties, we should be able to assume that only valid
 * properties for DISCONNECT are present here. */
صحيح property__process_disconnect(بنية mosquitto *context, mosquitto_property **props)
{
	mosquitto_property *p;

	p = *props;

	بينما(p){
		إذا(mosquitto_property_identifier(p) == MQTT_PROP_SESSION_EXPIRY_INTERVAL){
			uint32_t session_expiry_interval = mosquitto_property_int32_قيمة(p);
			إذا(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE
					&& session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){

				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with mismatched session-expiry-interval (%d:%d).",
						context->id, context->session_expiry_interval, p->قيمة.i32);
				إرجاع MOSQ_ERR_PROTOCOL;
			}
			context->session_expiry_interval = session_expiry_interval;
		}
		p = mosquitto_property_next(p);
	}
	إرجاع MOSQ_ERR_SUCCESS;
}
