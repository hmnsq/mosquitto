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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "property_mosq.h"
#અન્તર્ભાવ "property_common.h"


/* Process the incoming properties, we should be able to assume that only valid
 * properties for CONNECT are present here. */
પૂર્ણાંક property__process_connect(સંરચના mosquitto *context, mosquitto_property **props)
{
	mosquitto_property *p;

	p = *props;

	જ્યારે(p){
		પસંદગી(mosquitto_property_identifier(p)){
			સ્થિતિ MQTT_PROP_SESSION_EXPIRY_INTERVAL:
				context->session_expiry_interval = mosquitto_property_int32_કિંમત(p);
				તોડો;

			સ્થિતિ MQTT_PROP_RECEIVE_MAXIMUM:
				context->msgs_out.inflight_maximum = mosquitto_property_int16_કિંમત(p);
				જો(context->msgs_out.inflight_maximum == 0){
					log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with receive-maximum = 0.", context->id);
					પાછા MOSQ_ERR_PROTOCOL;
				}
				context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;
				તોડો;

			સ્થિતિ MQTT_PROP_MAXIMUM_PACKET_SIZE:
				context->maximum_packet_size = mosquitto_property_int32_કિંમત(p);
				જો(context->maximum_packet_size == 0){
					log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with maximum-packet-size = 0.", context->id);
					પાછા MOSQ_ERR_PROTOCOL;
				}
				તોડો;

			સ્થિતિ MQTT_PROP_TOPIC_ALIAS_MAXIMUM:
				context->alias_max_l2r = mosquitto_property_int16_કિંમત(p);
				જો(context->alias_max_l2r > context->listener->max_topic_alias_broker){
					context->alias_max_l2r = context->listener->max_topic_alias_broker;
				}
				તોડો;

			મૂળભૂત:
				તોડો;
		}
		p = mosquitto_property_next(p);
	}

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક property__process_will(સંરચના mosquitto *context, સંરચના mosquitto_message_all *સંદેશ, mosquitto_property **props)
{
	mosquitto_property *p, *p_prev;
	mosquitto_property *સંદેશ_properties, *સંદેશ_properties_last;

	p = *props;
	p_prev = ખાલી;
	સંદેશ_properties = ખાલી;
	સંદેશ_properties_last = ખાલી;
	સંદેશ->expiry_interval = સંદેશ_EXPIRY_INFINITE;
	જ્યારે(p){
		પસંદગી(mosquitto_property_identifier(p)){
			સ્થિતિ MQTT_PROP_CONTENT_TYPE:
			સ્થિતિ MQTT_PROP_CORRELATION_DATA:
			સ્થિતિ MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			સ્થિતિ MQTT_PROP_RESPONSE_TOPIC:
			સ્થિતિ MQTT_PROP_USER_PROPERTY:
				/* We save these properties for transmission with the PUBLISH */

				/* Add this property to the end of the list */
				જો(સંદેશ_properties){
					સંદેશ_properties_last->next = p;
					સંદેશ_properties_last = p;
				}નહીંતર{
					સંદેશ_properties = p;
					સંદેશ_properties_last = p;
				}

				/* And remove it from *props */
				જો(p_prev){
					p_prev->next = mosquitto_property_next(p);
					p = mosquitto_property_next(p_prev);
				}નહીંતર{
					*props = mosquitto_property_next(p);
					p = *props;
				}
				સંદેશ_properties_last->next = ખાલી;
				તોડો;

			સ્થિતિ MQTT_PROP_WILL_DELAY_INTERVAL:
				/* Leave this in *props, to be freed */
				context->will_delay_interval = mosquitto_property_int32_કિંમત(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				તોડો;

			સ્થિતિ MQTT_PROP_MESSAGE_EXPIRY_INTERVAL:
				/* Leave this in *props, to be freed */
				સંદેશ->expiry_interval = mosquitto_property_int32_કિંમત(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				તોડો;

			મૂળભૂત:
				સંદેશ->properties = સંદેશ_properties;
				log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet invalid property (%d).", context->id, p->identifier);
				પાછા MOSQ_ERR_PROTOCOL;
				તોડો;
		}
	}

	સંદેશ->properties = સંદેશ_properties;
	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક property__process_publish(સંરચના mosquitto_base_સંદેશ *base_સંદેશ, mosquitto_property **props, પૂર્ણાંક *topic_alias, uint32_t *message_expiry_interval, bool is_bridge)
{
	mosquitto_property *p, *p_prev;
	mosquitto_property *સંદેશ_properties_last;

	p = *props;
	p_prev = ખાલી;
	base_સંદેશ->data.properties = ખાલી;
	સંદેશ_properties_last = ખાલી;
	જ્યારે(p){
		પસંદગી(mosquitto_property_identifier(p)){
			સ્થિતિ MQTT_PROP_CONTENT_TYPE:
			સ્થિતિ MQTT_PROP_CORRELATION_DATA:
			સ્થિતિ MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			સ્થિતિ MQTT_PROP_RESPONSE_TOPIC:
			સ્થિતિ MQTT_PROP_USER_PROPERTY:
				જો(base_સંદેશ->data.properties){
					સંદેશ_properties_last->next = p;
					સંદેશ_properties_last = p;
				}નહીંતર{
					base_સંદેશ->data.properties = p;
					સંદેશ_properties_last = p;
				}
				જો(p_prev){
					p_prev->next = mosquitto_property_next(p);
					p = mosquitto_property_next(p_prev);
				}નહીંતર{
					*props = mosquitto_property_next(p);
					p = *props;
				}
				સંદેશ_properties_last->next = ખાલી;
				તોડો;

			સ્થિતિ MQTT_PROP_TOPIC_ALIAS:
				*topic_alias = mosquitto_property_int16_કિંમત(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				તોડો;

			સ્થિતિ MQTT_PROP_MESSAGE_EXPIRY_INTERVAL:
				*message_expiry_interval = mosquitto_property_int32_કિંમત(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				તોડો;

			સ્થિતિ MQTT_PROP_SUBSCRIPTION_IDENTIFIER:
				જો(!is_bridge || mosquitto_property_varint_કિંમત(p) == 0){
					પાછા MOSQ_ERR_PROTOCOL;
				}
				p_prev = p;
				p = mosquitto_property_next(p);
				તોડો;

			મૂળભૂત:
				p = mosquitto_property_next(p);
				તોડો;
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}


/* Process the incoming properties, we should be able to assume that only valid
 * properties for DISCONNECT are present here. */
પૂર્ણાંક property__process_disconnect(સંરચના mosquitto *context, mosquitto_property **props)
{
	mosquitto_property *p;

	p = *props;

	જ્યારે(p){
		જો(mosquitto_property_identifier(p) == MQTT_PROP_SESSION_EXPIRY_INTERVAL){
			uint32_t session_expiry_interval = mosquitto_property_int32_કિંમત(p);
			જો(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE
					&& session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){

				log__printf(ખાલી, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with mismatched session-expiry-interval (%d:%d).",
						context->id, context->session_expiry_interval, p->કિંમત.i32);
				પાછા MOSQ_ERR_PROTOCOL;
			}
			context->session_expiry_interval = session_expiry_interval;
		}
		p = mosquitto_property_next(p);
	}
	પાછા MOSQ_ERR_SUCCESS;
}
