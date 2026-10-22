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

#شامل "config.h"

#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "property_mosq.h"
#شامل "property_common.h"


/* Process the incoming properties, we should be able to assume that only valid
 * properties for CONNECT are present here. */
صحیح property__process_connect(ساختار mosquitto *context, mosquitto_property **props)
{
	mosquitto_property *p;

	p = *props;

	وقتی(p){
		انتخاب(mosquitto_property_identifier(p)){
			حالت MQTT_PROP_SESSION_EXPIRY_INTERVAL:
				context->session_expiry_interval = mosquitto_property_int32_مقدار(p);
				بشکن;

			حالت MQTT_PROP_RECEIVE_MAXIMUM:
				context->msgs_out.inflight_maximum = mosquitto_property_int16_مقدار(p);
				اگر(context->msgs_out.inflight_maximum == 0){
					log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with receive-maximum = 0.", context->id);
					بازگشت MOSQ_ERR_PROTOCOL;
				}
				context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;
				بشکن;

			حالت MQTT_PROP_MAXIMUM_PACKET_SIZE:
				context->maximum_packet_size = mosquitto_property_int32_مقدار(p);
				اگر(context->maximum_packet_size == 0){
					log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with maximum-packet-size = 0.", context->id);
					بازگشت MOSQ_ERR_PROTOCOL;
				}
				بشکن;

			حالت MQTT_PROP_TOPIC_ALIAS_MAXIMUM:
				context->alias_max_l2r = mosquitto_property_int16_مقدار(p);
				اگر(context->alias_max_l2r > context->listener->max_topic_alias_broker){
					context->alias_max_l2r = context->listener->max_topic_alias_broker;
				}
				بشکن;

			پیشفرض:
				بشکن;
		}
		p = mosquitto_property_next(p);
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح property__process_will(ساختار mosquitto *context, ساختار mosquitto_message_all *پیام, mosquitto_property **props)
{
	mosquitto_property *p, *p_prev;
	mosquitto_property *پیام_properties, *پیام_properties_last;

	p = *props;
	p_prev = پوچ;
	پیام_properties = پوچ;
	پیام_properties_last = پوچ;
	پیام->expiry_interval = پیام_EXPIRY_INFINITE;
	وقتی(p){
		انتخاب(mosquitto_property_identifier(p)){
			حالت MQTT_PROP_CONTENT_TYPE:
			حالت MQTT_PROP_CORRELATION_DATA:
			حالت MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			حالت MQTT_PROP_RESPONSE_TOPIC:
			حالت MQTT_PROP_USER_PROPERTY:
				/* We save these properties for transmission with the PUBLISH */

				/* Add this property to the end of the list */
				اگر(پیام_properties){
					پیام_properties_last->next = p;
					پیام_properties_last = p;
				}وگرنه{
					پیام_properties = p;
					پیام_properties_last = p;
				}

				/* And remove it from *props */
				اگر(p_prev){
					p_prev->next = mosquitto_property_next(p);
					p = mosquitto_property_next(p_prev);
				}وگرنه{
					*props = mosquitto_property_next(p);
					p = *props;
				}
				پیام_properties_last->next = پوچ;
				بشکن;

			حالت MQTT_PROP_WILL_DELAY_INTERVAL:
				/* Leave this in *props, to be freed */
				context->will_delay_interval = mosquitto_property_int32_مقدار(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				بشکن;

			حالت MQTT_PROP_MESSAGE_EXPIRY_INTERVAL:
				/* Leave this in *props, to be freed */
				پیام->expiry_interval = mosquitto_property_int32_مقدار(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				بشکن;

			پیشفرض:
				پیام->properties = پیام_properties;
				log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet invalid property (%d).", context->id, p->identifier);
				بازگشت MOSQ_ERR_PROTOCOL;
				بشکن;
		}
	}

	پیام->properties = پیام_properties;
	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح property__process_publish(ساختار mosquitto_base_پیام *base_پیام, mosquitto_property **props, صحیح *topic_alias, uint32_t *message_expiry_interval, bool is_bridge)
{
	mosquitto_property *p, *p_prev;
	mosquitto_property *پیام_properties_last;

	p = *props;
	p_prev = پوچ;
	base_پیام->data.properties = پوچ;
	پیام_properties_last = پوچ;
	وقتی(p){
		انتخاب(mosquitto_property_identifier(p)){
			حالت MQTT_PROP_CONTENT_TYPE:
			حالت MQTT_PROP_CORRELATION_DATA:
			حالت MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			حالت MQTT_PROP_RESPONSE_TOPIC:
			حالت MQTT_PROP_USER_PROPERTY:
				اگر(base_پیام->data.properties){
					پیام_properties_last->next = p;
					پیام_properties_last = p;
				}وگرنه{
					base_پیام->data.properties = p;
					پیام_properties_last = p;
				}
				اگر(p_prev){
					p_prev->next = mosquitto_property_next(p);
					p = mosquitto_property_next(p_prev);
				}وگرنه{
					*props = mosquitto_property_next(p);
					p = *props;
				}
				پیام_properties_last->next = پوچ;
				بشکن;

			حالت MQTT_PROP_TOPIC_ALIAS:
				*topic_alias = mosquitto_property_int16_مقدار(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				بشکن;

			حالت MQTT_PROP_MESSAGE_EXPIRY_INTERVAL:
				*message_expiry_interval = mosquitto_property_int32_مقدار(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				بشکن;

			حالت MQTT_PROP_SUBSCRIPTION_IDENTIFIER:
				اگر(!is_bridge || mosquitto_property_varint_مقدار(p) == 0){
					بازگشت MOSQ_ERR_PROTOCOL;
				}
				p_prev = p;
				p = mosquitto_property_next(p);
				بشکن;

			پیشفرض:
				p = mosquitto_property_next(p);
				بشکن;
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


/* Process the incoming properties, we should be able to assume that only valid
 * properties for DISCONNECT are present here. */
صحیح property__process_disconnect(ساختار mosquitto *context, mosquitto_property **props)
{
	mosquitto_property *p;

	p = *props;

	وقتی(p){
		اگر(mosquitto_property_identifier(p) == MQTT_PROP_SESSION_EXPIRY_INTERVAL){
			uint32_t session_expiry_interval = mosquitto_property_int32_مقدار(p);
			اگر(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE
					&& session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){

				log__printf(پوچ, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with mismatched session-expiry-interval (%d:%d).",
						context->id, context->session_expiry_interval, p->مقدار.i32);
				بازگشت MOSQ_ERR_PROTOCOL;
			}
			context->session_expiry_interval = session_expiry_interval;
		}
		p = mosquitto_property_next(p);
	}
	بازگشت MOSQ_ERR_SUCCESS;
}
