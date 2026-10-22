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

#समावेश "config.h"

#समावेश <stdio.h>
#समावेश <string.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "property_mosq.h"
#समावेश "property_common.h"


/* Process the incoming properties, we should be able to assume that only valid
 * properties for CONNECT are present here. */
पूर्णांक property__process_connect(काष्ठा mosquitto *context, mosquitto_property **props)
{
	mosquitto_property *p;

	p = *props;

	जबतक(p){
		चयन(mosquitto_property_identifier(p)){
			हाल MQTT_PROP_SESSION_EXPIRY_INTERVAL:
				context->session_expiry_interval = mosquitto_property_int32_मूल्य(p);
				अवरोध;

			हाल MQTT_PROP_RECEIVE_MAXIMUM:
				context->msgs_out.inflight_maximum = mosquitto_property_int16_मूल्य(p);
				अगर(context->msgs_out.inflight_maximum == 0){
					log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with receive-maximum = 0.", context->id);
					वापस MOSQ_ERR_PROTOCOL;
				}
				context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;
				अवरोध;

			हाल MQTT_PROP_MAXIMUM_PACKET_आकार:
				context->maximum_packet_आकार = mosquitto_property_int32_मूल्य(p);
				अगर(context->maximum_packet_आकार == 0){
					log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with maximum-packet-size = 0.", context->id);
					वापस MOSQ_ERR_PROTOCOL;
				}
				अवरोध;

			हाल MQTT_PROP_TOPIC_ALIAS_MAXIMUM:
				context->alias_max_l2r = mosquitto_property_int16_मूल्य(p);
				अगर(context->alias_max_l2r > context->listener->max_topic_alias_broker){
					context->alias_max_l2r = context->listener->max_topic_alias_broker;
				}
				अवरोध;

			शेष:
				अवरोध;
		}
		p = mosquitto_property_अग्र(p);
	}

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक property__process_will(काष्ठा mosquitto *context, काष्ठा mosquitto_message_all *संदेश, mosquitto_property **props)
{
	mosquitto_property *p, *p_पूर्व;
	mosquitto_property *संदेश_properties, *संदेश_properties_last;

	p = *props;
	p_पूर्व = शून्य;
	संदेश_properties = शून्य;
	संदेश_properties_last = शून्य;
	संदेश->expiry_interval = संदेश_EXPIRY_INFINITE;
	जबतक(p){
		चयन(mosquitto_property_identifier(p)){
			हाल MQTT_PROP_CONTENT_TYPE:
			हाल MQTT_PROP_CORRELATION_तथ्य:
			हाल MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			हाल MQTT_PROP_RESPONSE_TOPIC:
			हाल MQTT_PROP_USER_PROPERTY:
				/* We save these properties for transmission with the PUBLISH */

				/* Add this property to the end of the list */
				अगर(संदेश_properties){
					संदेश_properties_last->अग्र = p;
					संदेश_properties_last = p;
				}अन्यथा{
					संदेश_properties = p;
					संदेश_properties_last = p;
				}

				/* And remove it from *props */
				अगर(p_पूर्व){
					p_पूर्व->अग्र = mosquitto_property_अग्र(p);
					p = mosquitto_property_अग्र(p_पूर्व);
				}अन्यथा{
					*props = mosquitto_property_अग्र(p);
					p = *props;
				}
				संदेश_properties_last->अग्र = शून्य;
				अवरोध;

			हाल MQTT_PROP_WILL_DELAY_INTERVAL:
				/* Leave this in *props, to be freed */
				context->will_delay_interval = mosquitto_property_int32_मूल्य(p);
				p_पूर्व = p;
				p = mosquitto_property_अग्र(p);
				अवरोध;

			हाल MQTT_PROP_MESSAGE_EXPIRY_INTERVAL:
				/* Leave this in *props, to be freed */
				संदेश->expiry_interval = mosquitto_property_int32_मूल्य(p);
				p_पूर्व = p;
				p = mosquitto_property_अग्र(p);
				अवरोध;

			शेष:
				संदेश->properties = संदेश_properties;
				log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet invalid property (%d).", context->id, p->identifier);
				वापस MOSQ_ERR_PROTOCOL;
				अवरोध;
		}
	}

	संदेश->properties = संदेश_properties;
	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक property__process_publish(काष्ठा mosquitto_base_संदेश *base_संदेश, mosquitto_property **props, पूर्णांक *topic_alias, uint32_t *message_expiry_interval, bool is_bridge)
{
	mosquitto_property *p, *p_पूर्व;
	mosquitto_property *संदेश_properties_last;

	p = *props;
	p_पूर्व = शून्य;
	base_संदेश->तथ्य.properties = शून्य;
	संदेश_properties_last = शून्य;
	जबतक(p){
		चयन(mosquitto_property_identifier(p)){
			हाल MQTT_PROP_CONTENT_TYPE:
			हाल MQTT_PROP_CORRELATION_तथ्य:
			हाल MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			हाल MQTT_PROP_RESPONSE_TOPIC:
			हाल MQTT_PROP_USER_PROPERTY:
				अगर(base_संदेश->तथ्य.properties){
					संदेश_properties_last->अग्र = p;
					संदेश_properties_last = p;
				}अन्यथा{
					base_संदेश->तथ्य.properties = p;
					संदेश_properties_last = p;
				}
				अगर(p_पूर्व){
					p_पूर्व->अग्र = mosquitto_property_अग्र(p);
					p = mosquitto_property_अग्र(p_पूर्व);
				}अन्यथा{
					*props = mosquitto_property_अग्र(p);
					p = *props;
				}
				संदेश_properties_last->अग्र = शून्य;
				अवरोध;

			हाल MQTT_PROP_TOPIC_ALIAS:
				*topic_alias = mosquitto_property_int16_मूल्य(p);
				p_पूर्व = p;
				p = mosquitto_property_अग्र(p);
				अवरोध;

			हाल MQTT_PROP_MESSAGE_EXPIRY_INTERVAL:
				*message_expiry_interval = mosquitto_property_int32_मूल्य(p);
				p_पूर्व = p;
				p = mosquitto_property_अग्र(p);
				अवरोध;

			हाल MQTT_PROP_SUBSCRIPTION_IDENTIFIER:
				अगर(!is_bridge || mosquitto_property_varint_मूल्य(p) == 0){
					वापस MOSQ_ERR_PROTOCOL;
				}
				p_पूर्व = p;
				p = mosquitto_property_अग्र(p);
				अवरोध;

			शेष:
				p = mosquitto_property_अग्र(p);
				अवरोध;
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


/* Process the incoming properties, we should be able to assume that only valid
 * properties for DISCONNECT are present here. */
पूर्णांक property__process_disconnect(काष्ठा mosquitto *context, mosquitto_property **props)
{
	mosquitto_property *p;

	p = *props;

	जबतक(p){
		अगर(mosquitto_property_identifier(p) == MQTT_PROP_SESSION_EXPIRY_INTERVAL){
			uint32_t session_expiry_interval = mosquitto_property_int32_मूल्य(p);
			अगर(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE
					&& session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){

				log__printf(शून्य, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with mismatched session-expiry-interval (%d:%d).",
						context->id, context->session_expiry_interval, p->मूल्य.i32);
				वापस MOSQ_ERR_PROTOCOL;
			}
			context->session_expiry_interval = session_expiry_interval;
		}
		p = mosquitto_property_अग्र(p);
	}
	वापस MOSQ_ERR_SUCCESS;
}
