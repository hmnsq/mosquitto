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

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "property_mosq.h"
#अन्तर्भाव "property_common.h"


/* Process the incoming properties, we should be able to assume that only valid
 * properties for CONNECT are present here. */
पूर्णाङ्क property__process_connect(संरचना mosquitto *context, mosquitto_property **props)
{
	mosquitto_property *p;

	p = *props;

	जबसम्म(p){
		विकल्प(mosquitto_property_identifier(p)){
			पक्ष MQTT_PROP_SESSION_EXPIRY_INTERVAL:
				context->session_expiry_interval = mosquitto_property_int32_मान(p);
				रोक;

			पक्ष MQTT_PROP_RECEIVE_MAXIMUM:
				context->msgs_out.inflight_maximum = mosquitto_property_int16_मान(p);
				यदि(context->msgs_out.inflight_maximum == 0){
					log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with receive-maximum = 0.", context->id);
					फर्काऊ MOSQ_ERR_PROTOCOL;
				}
				context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;
				रोक;

			पक्ष MQTT_PROP_MAXIMUM_PACKET_SIZE:
				context->maximum_packet_size = mosquitto_property_int32_मान(p);
				यदि(context->maximum_packet_size == 0){
					log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with maximum-packet-size = 0.", context->id);
					फर्काऊ MOSQ_ERR_PROTOCOL;
				}
				रोक;

			पक्ष MQTT_PROP_TOPIC_ALIAS_MAXIMUM:
				context->alias_max_l2r = mosquitto_property_int16_मान(p);
				यदि(context->alias_max_l2r > context->listener->max_topic_alias_broker){
					context->alias_max_l2r = context->listener->max_topic_alias_broker;
				}
				रोक;

			स्वभाव:
				रोक;
		}
		p = mosquitto_property_next(p);
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क property__process_will(संरचना mosquitto *context, संरचना mosquitto_message_all *सन्देश, mosquitto_property **props)
{
	mosquitto_property *p, *p_prev;
	mosquitto_property *सन्देश_properties, *सन्देश_properties_last;

	p = *props;
	p_prev = रिक्त;
	सन्देश_properties = रिक्त;
	सन्देश_properties_last = रिक्त;
	सन्देश->expiry_interval = सन्देश_EXPIRY_INFINITE;
	जबसम्म(p){
		विकल्प(mosquitto_property_identifier(p)){
			पक्ष MQTT_PROP_CONTENT_TYPE:
			पक्ष MQTT_PROP_CORRELATION_DATA:
			पक्ष MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			पक्ष MQTT_PROP_RESPONSE_TOPIC:
			पक्ष MQTT_PROP_USER_PROPERTY:
				/* We save these properties for transmission with the PUBLISH */

				/* Add this property to the end of the list */
				यदि(सन्देश_properties){
					सन्देश_properties_last->next = p;
					सन्देश_properties_last = p;
				}अन्यथा{
					सन्देश_properties = p;
					सन्देश_properties_last = p;
				}

				/* And remove it from *props */
				यदि(p_prev){
					p_prev->next = mosquitto_property_next(p);
					p = mosquitto_property_next(p_prev);
				}अन्यथा{
					*props = mosquitto_property_next(p);
					p = *props;
				}
				सन्देश_properties_last->next = रिक्त;
				रोक;

			पक्ष MQTT_PROP_WILL_DELAY_INTERVAL:
				/* Leave this in *props, to be freed */
				context->will_delay_interval = mosquitto_property_int32_मान(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				रोक;

			पक्ष MQTT_PROP_MESSAGE_EXPIRY_INTERVAL:
				/* Leave this in *props, to be freed */
				सन्देश->expiry_interval = mosquitto_property_int32_मान(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				रोक;

			स्वभाव:
				सन्देश->properties = सन्देश_properties;
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet invalid property (%d).", context->id, p->identifier);
				फर्काऊ MOSQ_ERR_PROTOCOL;
				रोक;
		}
	}

	सन्देश->properties = सन्देश_properties;
	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क property__process_publish(संरचना mosquitto_base_सन्देश *base_सन्देश, mosquitto_property **props, पूर्णाङ्क *topic_alias, uint32_t *message_expiry_interval, bool is_bridge)
{
	mosquitto_property *p, *p_prev;
	mosquitto_property *सन्देश_properties_last;

	p = *props;
	p_prev = रिक्त;
	base_सन्देश->data.properties = रिक्त;
	सन्देश_properties_last = रिक्त;
	जबसम्म(p){
		विकल्प(mosquitto_property_identifier(p)){
			पक्ष MQTT_PROP_CONTENT_TYPE:
			पक्ष MQTT_PROP_CORRELATION_DATA:
			पक्ष MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			पक्ष MQTT_PROP_RESPONSE_TOPIC:
			पक्ष MQTT_PROP_USER_PROPERTY:
				यदि(base_सन्देश->data.properties){
					सन्देश_properties_last->next = p;
					सन्देश_properties_last = p;
				}अन्यथा{
					base_सन्देश->data.properties = p;
					सन्देश_properties_last = p;
				}
				यदि(p_prev){
					p_prev->next = mosquitto_property_next(p);
					p = mosquitto_property_next(p_prev);
				}अन्यथा{
					*props = mosquitto_property_next(p);
					p = *props;
				}
				सन्देश_properties_last->next = रिक्त;
				रोक;

			पक्ष MQTT_PROP_TOPIC_ALIAS:
				*topic_alias = mosquitto_property_int16_मान(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				रोक;

			पक्ष MQTT_PROP_MESSAGE_EXPIRY_INTERVAL:
				*message_expiry_interval = mosquitto_property_int32_मान(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				रोक;

			पक्ष MQTT_PROP_SUBSCRIPTION_IDENTIFIER:
				यदि(!is_bridge || mosquitto_property_varint_मान(p) == 0){
					फर्काऊ MOSQ_ERR_PROTOCOL;
				}
				p_prev = p;
				p = mosquitto_property_next(p);
				रोक;

			स्वभाव:
				p = mosquitto_property_next(p);
				रोक;
		}
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


/* Process the incoming properties, we should be able to assume that only valid
 * properties for DISCONNECT are present here. */
पूर्णाङ्क property__process_disconnect(संरचना mosquitto *context, mosquitto_property **props)
{
	mosquitto_property *p;

	p = *props;

	जबसम्म(p){
		यदि(mosquitto_property_identifier(p) == MQTT_PROP_SESSION_EXPIRY_INTERVAL){
			uint32_t session_expiry_interval = mosquitto_property_int32_मान(p);
			यदि(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE
					&& session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){

				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with mismatched session-expiry-interval (%d:%d).",
						context->id, context->session_expiry_interval, p->मान.i32);
				फर्काऊ MOSQ_ERR_PROTOCOL;
			}
			context->session_expiry_interval = session_expiry_interval;
		}
		p = mosquitto_property_next(p);
	}
	फर्काऊ MOSQ_ERR_SUCCESS;
}
