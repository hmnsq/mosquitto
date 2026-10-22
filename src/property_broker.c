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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "property_mosq.h"
#ଅନ୍ତର୍ଭାଵ "property_common.h"


/* Process the incoming properties, we should be able to assume that only valid
 * properties for CONNECT are present here. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା property__process_connect(ସଂରଚନା mosquitto *context, mosquitto_property **props)
{
	mosquitto_property *p;

	p = *props;

	ଯେତେବେଳେ(p){
		ବାଛ(mosquitto_property_identifier(p)){
			ସ୍ଥିତି MQTT_PROP_SESSION_EXPIRY_INTERVAL:
				context->session_expiry_interval = mosquitto_property_int32_ମୂଲ୍ୟ(p);
				ଭାଙ୍ଗ;

			ସ୍ଥିତି MQTT_PROP_RECEIVE_MAXIMUM:
				context->msgs_out.inflight_maximum = mosquitto_property_int16_ମୂଲ୍ୟ(p);
				ଯଦି(context->msgs_out.inflight_maximum == 0){
					log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with receive-maximum = 0.", context->id);
					ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
				}
				context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;
				ଭାଙ୍ଗ;

			ସ୍ଥିତି MQTT_PROP_MAXIMUM_PACKET_SIZE:
				context->maximum_packet_size = mosquitto_property_int32_ମୂଲ୍ୟ(p);
				ଯଦି(context->maximum_packet_size == 0){
					log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with maximum-packet-size = 0.", context->id);
					ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
				}
				ଭାଙ୍ଗ;

			ସ୍ଥିତି MQTT_PROP_TOPIC_ALIAS_MAXIMUM:
				context->alias_max_l2r = mosquitto_property_int16_ମୂଲ୍ୟ(p);
				ଯଦି(context->alias_max_l2r > context->listener->max_topic_alias_broker){
					context->alias_max_l2r = context->listener->max_topic_alias_broker;
				}
				ଭାଙ୍ଗ;

			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
				ଭାଙ୍ଗ;
		}
		p = mosquitto_property_next(p);
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା property__process_will(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_message_all *ସନ୍ଦେଶ, mosquitto_property **props)
{
	mosquitto_property *p, *p_prev;
	mosquitto_property *ସନ୍ଦେଶ_properties, *ସନ୍ଦେଶ_properties_last;

	p = *props;
	p_prev = ଖାଲି;
	ସନ୍ଦେଶ_properties = ଖାଲି;
	ସନ୍ଦେଶ_properties_last = ଖାଲି;
	ସନ୍ଦେଶ->expiry_interval = ସନ୍ଦେଶ_EXPIRY_INFINITE;
	ଯେତେବେଳେ(p){
		ବାଛ(mosquitto_property_identifier(p)){
			ସ୍ଥିତି MQTT_PROP_CONTENT_TYPE:
			ସ୍ଥିତି MQTT_PROP_CORRELATION_DATA:
			ସ୍ଥିତି MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			ସ୍ଥିତି MQTT_PROP_RESPONSE_TOPIC:
			ସ୍ଥିତି MQTT_PROP_USER_PROPERTY:
				/* We save these properties for transmission with the PUBLISH */

				/* Add this property to the end of the list */
				ଯଦି(ସନ୍ଦେଶ_properties){
					ସନ୍ଦେଶ_properties_last->next = p;
					ସନ୍ଦେଶ_properties_last = p;
				}ଅନ୍ୟଥା{
					ସନ୍ଦେଶ_properties = p;
					ସନ୍ଦେଶ_properties_last = p;
				}

				/* And remove it from *props */
				ଯଦି(p_prev){
					p_prev->next = mosquitto_property_next(p);
					p = mosquitto_property_next(p_prev);
				}ଅନ୍ୟଥା{
					*props = mosquitto_property_next(p);
					p = *props;
				}
				ସନ୍ଦେଶ_properties_last->next = ଖାଲି;
				ଭାଙ୍ଗ;

			ସ୍ଥିତି MQTT_PROP_WILL_DELAY_INTERVAL:
				/* Leave this in *props, to be freed */
				context->will_delay_interval = mosquitto_property_int32_ମୂଲ୍ୟ(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				ଭାଙ୍ଗ;

			ସ୍ଥିତି MQTT_PROP_MESSAGE_EXPIRY_INTERVAL:
				/* Leave this in *props, to be freed */
				ସନ୍ଦେଶ->expiry_interval = mosquitto_property_int32_ମୂଲ୍ୟ(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				ଭାଙ୍ଗ;

			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
				ସନ୍ଦେଶ->properties = ସନ୍ଦେଶ_properties;
				log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet invalid property (%d).", context->id, p->identifier);
				ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
				ଭାଙ୍ଗ;
		}
	}

	ସନ୍ଦେଶ->properties = ସନ୍ଦେଶ_properties;
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା property__process_publish(ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ, mosquitto_property **props, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *topic_alias, uint32_t *message_expiry_interval, bool is_bridge)
{
	mosquitto_property *p, *p_prev;
	mosquitto_property *ସନ୍ଦେଶ_properties_last;

	p = *props;
	p_prev = ଖାଲି;
	base_ସନ୍ଦେଶ->data.properties = ଖାଲି;
	ସନ୍ଦେଶ_properties_last = ଖାଲି;
	ଯେତେବେଳେ(p){
		ବାଛ(mosquitto_property_identifier(p)){
			ସ୍ଥିତି MQTT_PROP_CONTENT_TYPE:
			ସ୍ଥିତି MQTT_PROP_CORRELATION_DATA:
			ସ୍ଥିତି MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			ସ୍ଥିତି MQTT_PROP_RESPONSE_TOPIC:
			ସ୍ଥିତି MQTT_PROP_USER_PROPERTY:
				ଯଦି(base_ସନ୍ଦେଶ->data.properties){
					ସନ୍ଦେଶ_properties_last->next = p;
					ସନ୍ଦେଶ_properties_last = p;
				}ଅନ୍ୟଥା{
					base_ସନ୍ଦେଶ->data.properties = p;
					ସନ୍ଦେଶ_properties_last = p;
				}
				ଯଦି(p_prev){
					p_prev->next = mosquitto_property_next(p);
					p = mosquitto_property_next(p_prev);
				}ଅନ୍ୟଥା{
					*props = mosquitto_property_next(p);
					p = *props;
				}
				ସନ୍ଦେଶ_properties_last->next = ଖାଲି;
				ଭାଙ୍ଗ;

			ସ୍ଥିତି MQTT_PROP_TOPIC_ALIAS:
				*topic_alias = mosquitto_property_int16_ମୂଲ୍ୟ(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				ଭାଙ୍ଗ;

			ସ୍ଥିତି MQTT_PROP_MESSAGE_EXPIRY_INTERVAL:
				*message_expiry_interval = mosquitto_property_int32_ମୂଲ୍ୟ(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				ଭାଙ୍ଗ;

			ସ୍ଥିତି MQTT_PROP_SUBSCRIPTION_IDENTIFIER:
				ଯଦି(!is_bridge || mosquitto_property_varint_ମୂଲ୍ୟ(p) == 0){
					ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
				}
				p_prev = p;
				p = mosquitto_property_next(p);
				ଭାଙ୍ଗ;

			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
				p = mosquitto_property_next(p);
				ଭାଙ୍ଗ;
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


/* Process the incoming properties, we should be able to assume that only valid
 * properties for DISCONNECT are present here. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା property__process_disconnect(ସଂରଚନା mosquitto *context, mosquitto_property **props)
{
	mosquitto_property *p;

	p = *props;

	ଯେତେବେଳେ(p){
		ଯଦି(mosquitto_property_identifier(p) == MQTT_PROP_SESSION_EXPIRY_INTERVAL){
			uint32_t session_expiry_interval = mosquitto_property_int32_ମୂଲ୍ୟ(p);
			ଯଦି(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE
					&& session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){

				log__printf(ଖାଲି, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with mismatched session-expiry-interval (%d:%d).",
						context->id, context->session_expiry_interval, p->ମୂଲ୍ୟ.i32);
				ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
			}
			context->session_expiry_interval = session_expiry_interval;
		}
		p = mosquitto_property_next(p);
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}
