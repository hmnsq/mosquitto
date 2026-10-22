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

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <utlist.h>

#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "send_mosq.h"

#ifdef WITH_CONTROL


स्थिर शून्य control__negative_reply(स्थिरांक अक्षर *clientid, स्थिरांक अक्षर *request_topic)
{
	माप_प्रकार response_topic_len;
	अक्षर *response_topic;
	स्थिरांक अक्षर payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = म_माप(request_topic) + आकार("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	जर(!response_topic){
		परत;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (पूर्णांक)म_माप(payload), payload, 0, false, रिक्त);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
पूर्णांक control__process(संरचना mosquitto *context, संरचना mosquitto_base_संदेश *base_संदेश)
{
	संरचना mosquitto__callback *cb_found;
	संरचना mosquitto_evt_control event_data;
	संरचना mosquitto__security_options *opts;
	mosquitto_property *properties = रिक्त;
	पूर्णांक rc = MOSQ_ERR_SUCCESS;
	पूर्णांक rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_FIND(hh, opts->plugin_callbacks.control, base_संदेश->data.topic, म_माप(base_संदेश->data.topic), cb_found);

	/* If not found, check for per-listener plugins. */
	जर(cb_found == रिक्त && db.config->per_listener_settings){
		जर(!context->listener){
			log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(रिक्त, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			परत MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_FIND(hh, opts->plugin_callbacks.control, base_संदेश->data.topic, म_माप(base_संदेश->data.topic), cb_found);
	}
	जर(cb_found){
		स_रखो(&event_data, 0, आकार(event_data));
		event_data.client = context;
		event_data.topic = base_संदेश->data.topic;
		event_data.payload = base_संदेश->data.payload;
		event_data.payloadlen = base_संदेश->data.payloadlen;
		event_data.qos = base_संदेश->data.qos;
		event_data.retain = base_संदेश->data.retain;
		event_data.properties = base_संदेश->data.properties;
		event_data.reason_code = MQTT_RC_SUCCESS;
		event_data.reason_string = रिक्त;

		rc = cb_found->cb(MOSQ_EVT_CONTROL, &event_data, cb_found->userdata);
		जर(rc){
			जर(context->protocol == mosq_p_mqtt5 && event_data.reason_string){
				/* Not a critical error if this fails */
				(शून्य)mosquitto_property_add_string(&properties, MQTT_PROP_REASON_STRING, event_data.reason_string);
			}
		}
		SAFE_FREE(event_data.reason_string);
	}अन्यथा{
		control__negative_reply(context->id, base_संदेश->data.topic);
	}

	जर(base_संदेश->data.qos == 1){
		rc2 = send__puback(context, base_संदेश->data.source_mid, MQTT_RC_SUCCESS, properties);
		जर(rc2){
			rc = rc2;
		}
	}अन्यथा जर(base_संदेश->data.qos == 2){
		rc2 = send__pubrec(context, base_संदेश->data.source_mid, MQTT_RC_SUCCESS, properties);
		जर(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	परत rc;
}
#endif


पूर्णांक control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, स्थिरांक अक्षर *topic, शून्य *userdata)
{
#ifdef WITH_CONTROL
	संरचना mosquitto__security_options *opts;
	संरचना mosquitto__callback *cb_found, *cb_new;
	माप_प्रकार topic_len;

	जर(topic == रिक्त || cb_func == रिक्त){
		परत MOSQ_ERR_INVAL;
	}
	topic_len = म_माप(topic);
	जर(topic_len == 0 || topic_len > 65535){
		परत MOSQ_ERR_INVAL;
	}
	जर(म_भेदन(topic, "$CONTROL/", म_माप("$CONTROL/")) || म_माप(topic) < म_माप("$CONTROL/A/v1")){
		परत MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	जर(cb_found){
		परत MOSQ_ERR_ALREADY_EXISTS;
	}

	cb_new = mosquitto_calloc(1, आकार(संरचना mosquitto__callback));
	जर(cb_new == रिक्त){
		परत MOSQ_ERR_NOMEM;
	}
	cb_new->data.topic = mosquitto_strdup(topic);
	जर(cb_new->data.topic == रिक्त){
		mosquitto_FREE(cb_new);
		परत MOSQ_ERR_NOMEM;
	}
	cb_new->identifier = pid;
	cb_new->cb = cb_func;
	cb_new->userdata = userdata;
	HASH_ADD_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->data.topic, म_माप(cb_new->data.topic), cb_new);

	जर(pid->plugin_name){
		संरचना control_endpoint *ep;
		ep = mosquitto_malloc(आकार(संरचना control_endpoint) + topic_len + 2);
		जर(ep){
			ep->next = रिक्त;
			ep->prev = रिक्त;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(रिक्त, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	परत MOSQ_ERR_SUCCESS;
#अन्यथा
	UNUSED(pid);
	UNUSED(cb_func);
	UNUSED(topic);
	UNUSED(userdata);
	परत MOSQ_ERR_NOT_SUPPORTED;
#endif
}


पूर्णांक control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, स्थिरांक अक्षर *topic)
{
#ifdef WITH_CONTROL
	संरचना mosquitto__security_options *opts;

	संरचना mosquitto__callback *cb_found;
	माप_प्रकार topic_len;
	संरचना control_endpoint *ep;

	जर(topic == रिक्त){
		परत MOSQ_ERR_INVAL;
	}
	topic_len = म_माप(topic);
	जर(topic_len == 0 || topic_len > 65535){
		परत MOSQ_ERR_INVAL;
	}
	जर(म_भेदन(topic, "$CONTROL/", म_माप("$CONTROL/"))){
		परत MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	जर(cb_found && cb_found->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
		mosquitto_FREE(cb_found->data.topic);
		mosquitto_FREE(cb_found);

		DL_FOREACH(identifier->control_endpoints, ep){
			जर(!म_भेद(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				थांबा;
			}
		}
		परत MOSQ_ERR_SUCCESS;
	}
	परत MOSQ_ERR_NOT_FOUND;
#अन्यथा
	UNUSED(identifier);
	UNUSED(cb_func);
	UNUSED(topic);
	परत MOSQ_ERR_NOT_SUPPORTED;
#endif
}


/* Unregister all control callbacks for a single plugin */
शून्य control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	संरचना mosquitto__security_options *opts;

	संरचना mosquitto__callback *cb_found;
	संरचना control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_SAFE(identifier->control_endpoints, ep, ep_tmp){
		HASH_FIND(hh, opts->plugin_callbacks.control, ep->topic, म_माप(ep->topic), cb_found);
		जर(cb_found){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
			mosquitto_FREE(cb_found->data.topic);
			mosquitto_FREE(cb_found);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
