/*
Copyright (c) 2020-2021 Roger Light <roger@atchoo.org>

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
#అన్తర్భావ <utlist.h>

#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "send_mosq.h"

#నిర్వచితమైతే WITH_CONTROL


స్థిర శూన్య control_negative_జవాబు(స్థిరరాశి అక్షరం *clientid, స్థిరరాశి అక్షరం *request_topic)
{
	size_t response_topic_len;
	అక్షరం *response_topic;
	స్థిరరాశి అక్షరం payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = అక్షరశ్రేణి_పొడవు(request_topic) + పరిమాణం("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	ఒకవేళ(!response_topic){
		తిరిగి;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (పూర్ణాంకం)అక్షరశ్రేణి_పొడవు(payload), payload, 0, false, ఖాళీ);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
పూర్ణాంకం control__process(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం)
{
	నిర్మాణం mosquitto__callback *cb_దొరికింది;
	నిర్మాణం mosquitto_evt_control event_దత్తాంశం;
	నిర్మాణం mosquitto__security_options *opts;
	mosquitto_property *properties = ఖాళీ;
	పూర్ణాంకం rc = MOSQ_ERR_SUCCESS;
	పూర్ణాంకం rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_కనుగొను(hh, opts->plugin_callbacks.control, పునాది_సందేశం->దత్తాంశం.topic, అక్షరశ్రేణి_పొడవు(పునాది_సందేశం->దత్తాంశం.topic), cb_దొరికింది);

	/* If not found, check for per-listener plugins. */
	ఒకవేళ(cb_దొరికింది == ఖాళీ && db.config->చొప్పున_listener_settings){
		ఒకవేళ(!context->listener){
			log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(ఖాళీ, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			తిరిగి MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_కనుగొను(hh, opts->plugin_callbacks.control, పునాది_సందేశం->దత్తాంశం.topic, అక్షరశ్రేణి_పొడవు(పునాది_సందేశం->దత్తాంశం.topic), cb_దొరికింది);
	}
	ఒకవేళ(cb_దొరికింది){
		memset(&event_దత్తాంశం, 0, పరిమాణం(event_దత్తాంశం));
		event_దత్తాంశం.client = context;
		event_దత్తాంశం.topic = పునాది_సందేశం->దత్తాంశం.topic;
		event_దత్తాంశం.payload = పునాది_సందేశం->దత్తాంశం.payload;
		event_దత్తాంశం.payloadlen = పునాది_సందేశం->దత్తాంశం.payloadlen;
		event_దత్తాంశం.qos = పునాది_సందేశం->దత్తాంశం.qos;
		event_దత్తాంశం.retain = పునాది_సందేశం->దత్తాంశం.retain;
		event_దత్తాంశం.properties = పునాది_సందేశం->దత్తాంశం.properties;
		event_దత్తాంశం.reason_సంకేతం = MQTT_RC_SUCCESS;
		event_దత్తాంశం.reason_string = ఖాళీ;

		rc = cb_దొరికింది->cb(MOSQ_EVT_CONTROL, &event_దత్తాంశం, cb_దొరికింది->userdata);
		ఒకవేళ(rc){
			ఒకవేళ(context->protocol == mosq_p_mqtt5 && event_దత్తాంశం.reason_string){
				/* Not a critical error if this fails */
				(శూన్య)mosquitto_property_కూడు_string(&properties, MQTT_PROP_REASON_STRING, event_దత్తాంశం.reason_string);
			}
		}
		సురక్షిత_FREE(event_దత్తాంశం.reason_string);
	}లేకపోతే{
		control_negative_జవాబు(context->id, పునాది_సందేశం->దత్తాంశం.topic);
	}

	ఒకవేళ(పునాది_సందేశం->దత్తాంశం.qos == 1){
		rc2 = send__puback(context, పునాది_సందేశం->దత్తాంశం.ఆధారం_mid, MQTT_RC_SUCCESS, properties);
		ఒకవేళ(rc2){
			rc = rc2;
		}
	}లేకపోతే ఒకవేళ(పునాది_సందేశం->దత్తాంశం.qos == 2){
		rc2 = send__pubrec(context, పునాది_సందేశం->దత్తాంశం.ఆధారం_mid, MQTT_RC_SUCCESS, properties);
		ఒకవేళ(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	తిరిగి rc;
}
#అయితే_ముగింపు


పూర్ణాంకం control_రిజిస్టరు_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, స్థిరరాశి అక్షరం *topic, శూన్య *userdata)
{
#నిర్వచితమైతే WITH_CONTROL
	నిర్మాణం mosquitto__security_options *opts;
	నిర్మాణం mosquitto__callback *cb_దొరికింది, *cb_new;
	size_t topic_len;

	ఒకవేళ(topic == ఖాళీ || cb_func == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}
	topic_len = అక్షరశ్రేణి_పొడవు(topic);
	ఒకవేళ(topic_len == 0 || topic_len > 65535){
		తిరిగి MOSQ_ERR_INVAL;
	}
	ఒకవేళ(అక్షరశ్రేణి_పోలిక(topic, "$CONTROL/", అక్షరశ్రేణి_పొడవు("$CONTROL/")) || అక్షరశ్రేణి_పొడవు(topic) < అక్షరశ్రేణి_పొడవు("$CONTROL/A/v1")){
		తిరిగి MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_కనుగొను(hh, opts->plugin_callbacks.control, topic, topic_len, cb_దొరికింది);
	ఒకవేళ(cb_దొరికింది){
		తిరిగి MOSQ_ERR_ALREADY_EXISTS;
	}

	cb_new = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto__callback));
	ఒకవేళ(cb_new == ఖాళీ){
		తిరిగి MOSQ_ERR_NOMEM;
	}
	cb_new->దత్తాంశం.topic = mosquitto_strdup(topic);
	ఒకవేళ(cb_new->దత్తాంశం.topic == ఖాళీ){
		mosquitto_FREE(cb_new);
		తిరిగి MOSQ_ERR_NOMEM;
	}
	cb_new->identifier = pid;
	cb_new->cb = cb_func;
	cb_new->userdata = userdata;
	HASH_కూడు_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->దత్తాంశం.topic, అక్షరశ్రేణి_పొడవు(cb_new->దత్తాంశం.topic), cb_new);

	ఒకవేళ(pid->plugin_name){
		నిర్మాణం control_endpoint *ep;
		ep = mosquitto_malloc(పరిమాణం(నిర్మాణం control_endpoint) + topic_len + 2);
		ఒకవేళ(ep){
			ep->తదుపరి = ఖాళీ;
			ep->prev = ఖాళీ;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	తిరిగి MOSQ_ERR_SUCCESS;
#లేకపోతే
	వాడని(pid);
	వాడని(cb_func);
	వాడని(topic);
	వాడని(userdata);
	తిరిగి MOSQ_ERR_NOT_SUPPORTED;
#అయితే_ముగింపు
}


పూర్ణాంకం control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, స్థిరరాశి అక్షరం *topic)
{
#నిర్వచితమైతే WITH_CONTROL
	నిర్మాణం mosquitto__security_options *opts;

	నిర్మాణం mosquitto__callback *cb_దొరికింది;
	size_t topic_len;
	నిర్మాణం control_endpoint *ep;

	ఒకవేళ(topic == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}
	topic_len = అక్షరశ్రేణి_పొడవు(topic);
	ఒకవేళ(topic_len == 0 || topic_len > 65535){
		తిరిగి MOSQ_ERR_INVAL;
	}
	ఒకవేళ(అక్షరశ్రేణి_పోలిక(topic, "$CONTROL/", అక్షరశ్రేణి_పొడవు("$CONTROL/"))){
		తిరిగి MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_కనుగొను(hh, opts->plugin_callbacks.control, topic, topic_len, cb_దొరికింది);
	ఒకవేళ(cb_దొరికింది && cb_దొరికింది->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_దొరికింది);
		mosquitto_FREE(cb_దొరికింది->దత్తాంశం.topic);
		mosquitto_FREE(cb_దొరికింది);

		DL_FOREACH(identifier->control_endpoints, ep){
			ఒకవేళ(!strcmp(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				ఆపు;
			}
		}
		తిరిగి MOSQ_ERR_SUCCESS;
	}
	తిరిగి MOSQ_ERR_NOT_దొరికింది;
#లేకపోతే
	వాడని(identifier);
	వాడని(cb_func);
	వాడని(topic);
	తిరిగి MOSQ_ERR_NOT_SUPPORTED;
#అయితే_ముగింపు
}


/* Unregister all control callbacks for a single plugin */
శూన్య control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	నిర్మాణం mosquitto__security_options *opts;

	నిర్మాణం mosquitto__callback *cb_దొరికింది;
	నిర్మాణం control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_సురక్షిత(identifier->control_endpoints, ep, ep_tmp){
		HASH_కనుగొను(hh, opts->plugin_callbacks.control, ep->topic, అక్షరశ్రేణి_పొడవు(ep->topic), cb_దొరికింది);
		ఒకవేళ(cb_దొరికింది){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_దొరికింది);
			mosquitto_FREE(cb_దొరికింది->దత్తాంశం.topic);
			mosquitto_FREE(cb_దొరికింది);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
