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

#అన్తర్భావ "config.h"

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "property_mosq.h"
#అన్తర్భావ "property_common.h"


/* Process the incoming properties, we should be able to assume that only valid
 * properties for CONNECT are present here. */
పూర్ణాంకం property__process_connect(నిర్మాణం mosquitto *context, mosquitto_property **props)
{
	mosquitto_property *p;

	p = *props;

	అయితే(p){
		ఎంపిక(mosquitto_property_identifier(p)){
			స్థితి MQTT_PROP_SESSION_EXPIRY_INTERVAL:
				context->session_expiry_interval = mosquitto_property_int32_విలువ(p);
				ఆపు;

			స్థితి MQTT_PROP_RECEIVE_గరిష్ఠ:
				context->msgs_బహిర్గతం.inflight_గరిష్ఠ = mosquitto_property_int16_విలువ(p);
				ఒకవేళ(context->msgs_బహిర్గతం.inflight_గరిష్ఠ == 0){
					log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with receive-maximum = 0.", context->id);
					తిరిగి MOSQ_ERR_PROTOCOL;
				}
				context->msgs_బహిర్గతం.inflight_quota = context->msgs_బహిర్గతం.inflight_గరిష్ఠ;
				ఆపు;

			స్థితి MQTT_PROP_గరిష్ఠ_PACKET_కొలత:
				context->గరిష్ఠ_packet_కొలత = mosquitto_property_int32_విలువ(p);
				ఒకవేళ(context->గరిష్ఠ_packet_కొలత == 0){
					log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet with maximum-packet-size = 0.", context->id);
					తిరిగి MOSQ_ERR_PROTOCOL;
				}
				ఆపు;

			స్థితి MQTT_PROP_TOPIC_ALIAS_గరిష్ఠ:
				context->alias_max_l2r = mosquitto_property_int16_విలువ(p);
				ఒకవేళ(context->alias_max_l2r > context->listener->max_topic_alias_broker){
					context->alias_max_l2r = context->listener->max_topic_alias_broker;
				}
				ఆపు;

			అప్రమేయ:
				ఆపు;
		}
		p = mosquitto_property_తదుపరి(p);
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం property__process_will(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_సందేశం_all *సందేశం, mosquitto_property **props)
{
	mosquitto_property *p, *p_prev;
	mosquitto_property *సందేశం_properties, *సందేశం_properties_చివరి;

	p = *props;
	p_prev = ఖాళీ;
	సందేశం_properties = ఖాళీ;
	సందేశం_properties_చివరి = ఖాళీ;
	సందేశం->expiry_interval = సందేశం_EXPIRY_INFINITE;
	అయితే(p){
		ఎంపిక(mosquitto_property_identifier(p)){
			స్థితి MQTT_PROP_CONTENT_TYPE:
			స్థితి MQTT_PROP_CORRELATION_దత్తాంశం:
			స్థితి MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			స్థితి MQTT_PROP_RESPONSE_TOPIC:
			స్థితి MQTT_PROP_USER_PROPERTY:
				/* We save these properties for transmission with the PUBLISH */

				/* Add this property to the end of the list */
				ఒకవేళ(సందేశం_properties){
					సందేశం_properties_చివరి->తదుపరి = p;
					సందేశం_properties_చివరి = p;
				}లేకపోతే{
					సందేశం_properties = p;
					సందేశం_properties_చివరి = p;
				}

				/* And remove it from *props */
				ఒకవేళ(p_prev){
					p_prev->తదుపరి = mosquitto_property_తదుపరి(p);
					p = mosquitto_property_తదుపరి(p_prev);
				}లేకపోతే{
					*props = mosquitto_property_తదుపరి(p);
					p = *props;
				}
				సందేశం_properties_చివరి->తదుపరి = ఖాళీ;
				ఆపు;

			స్థితి MQTT_PROP_WILL_DELAY_INTERVAL:
				/* Leave this in *props, to be freed */
				context->will_delay_interval = mosquitto_property_int32_విలువ(p);
				p_prev = p;
				p = mosquitto_property_తదుపరి(p);
				ఆపు;

			స్థితి MQTT_PROP_సందేశం_EXPIRY_INTERVAL:
				/* Leave this in *props, to be freed */
				సందేశం->expiry_interval = mosquitto_property_int32_విలువ(p);
				p_prev = p;
				p = mosquitto_property_తదుపరి(p);
				ఆపు;

			అప్రమేయ:
				సందేశం->properties = సందేశం_properties;
				log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: CONNECT packet invalid property (%d).", context->id, p->identifier);
				తిరిగి MOSQ_ERR_PROTOCOL;
				ఆపు;
		}
	}

	సందేశం->properties = సందేశం_properties;
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం property__process_publish(నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం, mosquitto_property **props, పూర్ణాంకం *topic_alias, uint32_t *సందేశం_expiry_interval, bool is_bridge)
{
	mosquitto_property *p, *p_prev;
	mosquitto_property *సందేశం_properties_చివరి;

	p = *props;
	p_prev = ఖాళీ;
	పునాది_సందేశం->దత్తాంశం.properties = ఖాళీ;
	సందేశం_properties_చివరి = ఖాళీ;
	అయితే(p){
		ఎంపిక(mosquitto_property_identifier(p)){
			స్థితి MQTT_PROP_CONTENT_TYPE:
			స్థితి MQTT_PROP_CORRELATION_దత్తాంశం:
			స్థితి MQTT_PROP_PAYLOAD_FORMAT_INDICATOR:
			స్థితి MQTT_PROP_RESPONSE_TOPIC:
			స్థితి MQTT_PROP_USER_PROPERTY:
				ఒకవేళ(పునాది_సందేశం->దత్తాంశం.properties){
					సందేశం_properties_చివరి->తదుపరి = p;
					సందేశం_properties_చివరి = p;
				}లేకపోతే{
					పునాది_సందేశం->దత్తాంశం.properties = p;
					సందేశం_properties_చివరి = p;
				}
				ఒకవేళ(p_prev){
					p_prev->తదుపరి = mosquitto_property_తదుపరి(p);
					p = mosquitto_property_తదుపరి(p_prev);
				}లేకపోతే{
					*props = mosquitto_property_తదుపరి(p);
					p = *props;
				}
				సందేశం_properties_చివరి->తదుపరి = ఖాళీ;
				ఆపు;

			స్థితి MQTT_PROP_TOPIC_ALIAS:
				*topic_alias = mosquitto_property_int16_విలువ(p);
				p_prev = p;
				p = mosquitto_property_తదుపరి(p);
				ఆపు;

			స్థితి MQTT_PROP_సందేశం_EXPIRY_INTERVAL:
				*సందేశం_expiry_interval = mosquitto_property_int32_విలువ(p);
				p_prev = p;
				p = mosquitto_property_తదుపరి(p);
				ఆపు;

			స్థితి MQTT_PROP_SUBSCRIPTION_IDENTIFIER:
				ఒకవేళ(!is_bridge || mosquitto_property_varint_విలువ(p) == 0){
					తిరిగి MOSQ_ERR_PROTOCOL;
				}
				p_prev = p;
				p = mosquitto_property_తదుపరి(p);
				ఆపు;

			అప్రమేయ:
				p = mosquitto_property_తదుపరి(p);
				ఆపు;
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


/* Process the incoming properties, we should be able to assume that only valid
 * properties for DISCONNECT are present here. */
పూర్ణాంకం property__process_disconnect(నిర్మాణం mosquitto *context, mosquitto_property **props)
{
	mosquitto_property *p;

	p = *props;

	అయితే(p){
		ఒకవేళ(mosquitto_property_identifier(p) == MQTT_PROP_SESSION_EXPIRY_INTERVAL){
			uint32_t session_expiry_interval = mosquitto_property_int32_విలువ(p);
			ఒకవేళ(context->session_expiry_interval == MQTT_SESSION_EXPIRY_తక్షణ
					&& session_expiry_interval != MQTT_SESSION_EXPIRY_తక్షణ){

				log__printf(ఖాళీ, MOSQ_LOG_INFO, "Protocol error from %s: DISCONNECT packet with mismatched session-expiry-interval (%d:%d).",
						context->id, context->session_expiry_interval, p->విలువ.i32);
				తిరిగి MOSQ_ERR_PROTOCOL;
			}
			context->session_expiry_interval = session_expiry_interval;
		}
		p = mosquitto_property_తదుపరి(p);
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}
