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

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "property_mosq.h"
#include "property_common.h"


/* Process the incoming properties, we should be able to assume that only valid
 * properties for CONNECT are present here. */
ကိန်းပြည့် property__process_connect(sa_nracanaa mosquitto *context, mosquitto_property **props)
{
	mosquitto_property *p;

	p = *props;

	နေစဉ်(p){
		switch(mosquitto_property_identifier(p)){
			pak_sa MQTT_PROP_SESSION_EXPIRY_INTERVAL:
				context->session_expiry_interval = mosquitto_property_int32_value(p);
				bha_nga;

			pak_sa MQTT_PROP_RECEIVE_MAXIMUM:
				context->msgs_out.inflight_maximum = mosquitto_property_int16_value(p);
				အကယ်၍(context->msgs_out.inflight_maximum == 0){
					log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with receive-maximum = 0.", context->id);
					ပြန်ပေး MOSQ_ERR_PROTOCOL;
				}
				context->msgs_out.inflight_quota = context->msgs_out.inflight_maximum;
				bha_nga;

			pak_sa MQTT_PROP_MAXIMUM_PACKET_SIZE:
				context->maximum_packet_size = mosquitto_property_int32_value(p);
				အကယ်၍(context->maximum_packet_size == 0){
					log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with maximum-packet-size = 0.", context->id);
					ပြန်ပေး MOSQ_ERR_PROTOCOL;
				}
				bha_nga;

			pak_sa MQTT_PROP_TOPIC_ALIAS_MAXIMUM:
				context->alias_max_l2r = mosquitto_property_int16_value(p);
				အကယ်၍(context->alias_max_l2r > context->listener->max_topic_alias_broker){
					context->alias_max_l2r = context->listener->max_topic_alias_broker;
				}
				bha_nga;

			default:
				bha_nga;
		}
		p = mosquitto_property_next(p);
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


ကိန်းပြည့် property__process_will(sa_nracanaa mosquitto *context, sa_nracanaa mosquitto_message_all *msg, mosquitto_property **props)
{
	mosquitto_property *p, *p_prev;
	mosquitto_property *msg_properties, *msg_properties_last;

	p = *props;
	p_prev = NULL;
	msg_properties = NULL;
	msg_properties_last = NULL;
	msg->expiry_interval = MSG_EXPIRY_INFINITE;
	နေစဉ်(p){
		switch(mosquitto_property_identifier(p)){
			pak_sa MQTT_PROP_CONTENT_TYPE:
			pak_sa MQTT_PROP_CORRELATION_DATA:
			pak_sa MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			pak_sa MQTT_PROP_RESPONSE_TOPIC:
			pak_sa MQTT_PROP_USER_PROPERTY:
				/* We save these properties for transmission with the PUBLISH */

				/* Add this property to the end of the list */
				အကယ်၍(msg_properties){
					msg_properties_last->next = p;
					msg_properties_last = p;
				}မဟုတ်ပါက{
					msg_properties = p;
					msg_properties_last = p;
				}

				/* And remove it from *props */
				အကယ်၍(p_prev){
					p_prev->next = mosquitto_property_next(p);
					p = mosquitto_property_next(p_prev);
				}မဟုတ်ပါက{
					*props = mosquitto_property_next(p);
					p = *props;
				}
				msg_properties_last->next = NULL;
				bha_nga;

			pak_sa MQTT_PROP_WILL_DELAY_INTERVAL:
				/* Leave this in *props, to be freed */
				context->will_delay_interval = mosquitto_property_int32_value(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				bha_nga;

			pak_sa MQTT_PROP_MESSAGE_EXPIRY_INTERVAL:
				/* Leave this in *props, to be freed */
				msg->expiry_interval = mosquitto_property_int32_value(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				bha_nga;

			default:
				msg->properties = msg_properties;
				log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet invalid property (%d).", context->id, p->identifier);
				ပြန်ပေး MOSQ_ERR_PROTOCOL;
				bha_nga;
		}
	}

	msg->properties = msg_properties;
	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


ကိန်းပြည့် property__process_publish(sa_nracanaa mosquitto__base_msg *base_msg, mosquitto_property **props, ကိန်းပြည့် *topic_alias, uint32_t *message_expiry_interval, bool is_bridge)
{
	mosquitto_property *p, *p_prev;
	mosquitto_property *msg_properties_last;

	p = *props;
	p_prev = NULL;
	base_msg->data.properties = NULL;
	msg_properties_last = NULL;
	နေစဉ်(p){
		switch(mosquitto_property_identifier(p)){
			pak_sa MQTT_PROP_CONTENT_TYPE:
			pak_sa MQTT_PROP_CORRELATION_DATA:
			pak_sa MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			pak_sa MQTT_PROP_RESPONSE_TOPIC:
			pak_sa MQTT_PROP_USER_PROPERTY:
				အကယ်၍(base_msg->data.properties){
					msg_properties_last->next = p;
					msg_properties_last = p;
				}မဟုတ်ပါက{
					base_msg->data.properties = p;
					msg_properties_last = p;
				}
				အကယ်၍(p_prev){
					p_prev->next = mosquitto_property_next(p);
					p = mosquitto_property_next(p_prev);
				}မဟုတ်ပါက{
					*props = mosquitto_property_next(p);
					p = *props;
				}
				msg_properties_last->next = NULL;
				bha_nga;

			pak_sa MQTT_PROP_TOPIC_ALIAS:
				*topic_alias = mosquitto_property_int16_value(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				bha_nga;

			pak_sa MQTT_PROP_MESSAGE_EXPIRY_INTERVAL:
				*message_expiry_interval = mosquitto_property_int32_value(p);
				p_prev = p;
				p = mosquitto_property_next(p);
				bha_nga;

			pak_sa MQTT_PROP_SUBSCRIPTION_IDENTIFIER:
				အကယ်၍(!is_bridge || mosquitto_property_varint_value(p) == 0){
					ပြန်ပေး MOSQ_ERR_PROTOCOL;
				}
				p_prev = p;
				p = mosquitto_property_next(p);
				bha_nga;

			default:
				p = mosquitto_property_next(p);
				bha_nga;
		}
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


/* Process the incoming properties, we should be able to assume that only valid
 * properties for DISCONNECT are present here. */
ကိန်းပြည့် property__process_disconnect(sa_nracanaa mosquitto *context, mosquitto_property **props)
{
	mosquitto_property *p;

	p = *props;

	နေစဉ်(p){
		အကယ်၍(mosquitto_property_identifier(p) == MQTT_PROP_SESSION_EXPIRY_INTERVAL){
			uint32_t session_expiry_interval = mosquitto_property_int32_value(p);
			အကယ်၍(context->session_expiry_interval == MQTT_SESSION_EXPIRY_IMMEDIATE
					&& session_expiry_interval != MQTT_SESSION_EXPIRY_IMMEDIATE){

				log__printf(NULL, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with mismatched session-expiry-interval (%d:%d).",
						context->id, context->session_expiry_interval, p->value.i32);
				ပြန်ပေး MOSQ_ERR_PROTOCOL;
			}
			context->session_expiry_interval = session_expiry_interval;
		}
		p = mosquitto_property_next(p);
	}
	ပြန်ပေး MOSQ_ERR_SUCCESS;
}
