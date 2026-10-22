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
पूर्णांक property__process_connect(संरचना mosquitto *context, mosquitto_property **props)
{
	mosquitto_property *p;

	p = *props;

	पर्यंत(p){
		निवड(mosquitto_property_identifier(p)){
			स्थिती MQTT_PROP_SESSION_EXPIRY_INTERVAL:
				context->session_expiry_interval = mosquitto_property_int32_मूल्य(p);
				थांबा;

			स्थिती MQTT_PROP_RECEIVE_MAXIMUM:
				context->msgs_out.inflight_maximum = mosquitto_property_int16_मूल्य(p);
				जर(context->msgs_out.inflight_maximum == 0){
					log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with receive-maximum = 0.", context->id);
					परत MOSQ_ERR_PROTOCOL;
				}
				context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;
				थांबा;

			स्थिती MQTT_PROP_MAXIMUM_PACKET_SIZE:
				context->maximum_packet_size = mosquitto_property_int32_मूल्य(p);
				जर(context->maximum_packet_size == 0){
					log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with maximum-packet-size = 0.", context->id);
					परत MOSQ_ERR_PROTOCOL;
				}
				थांबा;

			स्थिती MQTT_PROP_TOPIC_ALIAS_MAXIMUM:
				context->alias_max_l2r = mosquitto_property_int16_मूल्य(p);
				जर(context->alias_max_l2r > context->listener->max_topic_alias_broker){
					context->alias_max_l2r = context->listener->max_topic_alias_broker;
				}
				थांबा;

			पूर्वनिर्धारित:
				थांबा;
		}
		p = mosquitto_property_next(p);
	}

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक property__process_will(संरचना mosquitto *context, संरचना mosquitto_message_all *संदेश, mosquitto_property **props)
{
	mosquitto_property *p, *p_prev;
	mosquitto_property *संदेश_properties, *संदेश_properties_last;

	p = *props;
	p_prev = रिक्त;
	संदेश_properties = रिक्त;
	संदेश_properties_last = रिक्त;
	संदेश->expiry_interval = संदेश_EXPIRY_INFINITE;
	पर्यंत(p){
		निवड(mosquitto_property_identifier(p)){
			स्थिती MQTT_PROP_CONTENT_TYPE:
			स्थिती MQTT_PROP_CORRELATION_DATA:
			स्थिती MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			स्थिती MQTT_PROP_RESPONSE_TOPIC:
			स्थिती MQTT_PROP_USER_PROPERTY:
				/* We save these properties for transmission with the PUBLISH */

				/* Add this property to the end of the list */
				जर(संदेश_properties){
					संदेश_properties_last->next = p;
					संदेश_properties_last = p;
				}अन्यथा{
					संदेश_properties = p;
					संदेश_properties_last = p;
				}

				/* And remove it from *props */
				जर(p_prev){
					p_prev->next = mosquitto_property_next(p);
					p = mosquitto_property_next(p_prev);
				}अन्यथा{
					*props = mosquitto_property_next(p);
					p = *props;
				}
				संदेश_properties_last->next = रिक्त;
				थांबा;

			स्थिती MQTT_PROP_WILL_DELAY_INTERVAL:
				/* Leave this in *props, to be freed */
				context->will_delay_interval = mosquitto_property_int32_मूल्य(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				थांबा;

			स्थिती MQTT_PROP_MESSAGE_EXPIRY_INTERVAL:
				/* Leave this in *props, to be freed */
				संदेश->expiry_interval = mosquitto_property_int32_मूल्य(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				थांबा;

			पूर्वनिर्धारित:
				संदेश->properties = संदेश_properties;
				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet invalid property (%d).", context->id, p->identifier);
				परत MOSQ_ERR_PROTOCOL;
				थांबा;
		}
	}

	संदेश->properties = संदेश_properties;
	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक property__process_publish(संरचना mosquitto_base_संदेश *base_संदेश, mosquitto_property **props, पूर्णांक *topic_alias, uint32_t *message_expiry_interval, bool is_bridge)
{
	mosquitto_property *p, *p_prev;
	mosquitto_property *संदेश_properties_last;

	p = *props;
	p_prev = रिक्त;
	base_संदेश->data.properties = रिक्त;
	संदेश_properties_last = रिक्त;
	पर्यंत(p){
		निवड(mosquitto_property_identifier(p)){
			स्थिती MQTT_PROP_CONTENT_TYPE:
			स्थिती MQTT_PROP_CORRELATION_DATA:
			स्थिती MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			स्थिती MQTT_PROP_RESPONSE_TOPIC:
			स्थिती MQTT_PROP_USER_PROPERTY:
				जर(base_संदेश->data.properties){
					संदेश_properties_last->next = p;
					संदेश_properties_last = p;
				}अन्यथा{
					base_संदेश->data.properties = p;
					संदेश_properties_last = p;
				}
				जर(p_prev){
					p_prev->next = mosquitto_property_next(p);
					p = mosquitto_property_next(p_prev);
				}अन्यथा{
					*props = mosquitto_property_next(p);
					p = *props;
				}
				संदेश_properties_last->next = रिक्त;
				थांबा;

			स्थिती MQTT_PROP_TOPIC_ALIAS:
				*topic_alias = mosquitto_property_int16_मूल्य(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				थांबा;

			स्थिती MQTT_PROP_MESSAGE_EXPIRY_INTERVAL:
				*message_expiry_interval = mosquitto_property_int32_मूल्य(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				थांबा;

			स्थिती MQTT_PROP_SUBSCRIPTION_IDENTIFIER:
				जर(!is_bridge || mosquitto_property_varint_मूल्य(p) == 0){
					परत MOSQ_ERR_PROTOCOL;
				}
				p_prev = p;
				p = mosquitto_property_next(p);
				थांबा;

			पूर्वनिर्धारित:
				p = mosquitto_property_next(p);
				थांबा;
		}
	}

	परत MOSQ_ERR_SUCCESS;
}


/* Process the incoming properties, we should be able to assume that only valid
 * properties for DISCONNECT are present here. */
पूर्णांक property__process_disconnect(संरचना mosquitto *context, mosquitto_property **props)
{
	mosquitto_property *p;

	p = *props;

	पर्यंत(p){
		जर(mosquitto_property_identifier(p) == MQTT_PROP_SESSION_EXPIRY_INTERVAL){
			uint32_t session_expiry_interval = mosquitto_property_int32_मूल्य(p);
			जर(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE
					&& session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){

				log__printf(रिक्त, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with mismatched session-expiry-interval (%d:%d).",
						context->id, context->session_expiry_interval, p->मूल्य.i32);
				परत MOSQ_ERR_PROTOCOL;
			}
			context->session_expiry_interval = session_expiry_interval;
		}
		p = mosquitto_property_next(p);
	}
	परत MOSQ_ERR_SUCCESS;
}
