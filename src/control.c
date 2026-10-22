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

#समावेश "config.h"

#समावेश <stdio.h>
#समावेश <utlist.h>

#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "mosquitto_broker_internal.h"
#समावेश "send_mosq.h"

#अगर_घोषित WITH_CONTROL


अटल व्योम control__negative_reply(स्थिर अक्षर *clientid, स्थिर अक्षर *request_topic)
{
	माप_प्रकार response_topic_len;
	अक्षर *response_topic;
	स्थिर अक्षर payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = म_माप(request_topic) + माप("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	अगर(!response_topic){
		वापस;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (पूर्णांक)म_माप(payload), payload, 0, false, शून्य);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
पूर्णांक control__process(काष्ठा mosquitto *context, काष्ठा mosquitto_base_संदेश *base_संदेश)
{
	काष्ठा mosquitto__callback *cb_found;
	काष्ठा mosquitto_evt_control event_तथ्य;
	काष्ठा mosquitto__security_options *opts;
	mosquitto_property *properties = शून्य;
	पूर्णांक rc = MOSQ_ERR_SUCCESS;
	पूर्णांक rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_FIND(hh, opts->plugin_callbacks.control, base_संदेश->तथ्य.topic, म_माप(base_संदेश->तथ्य.topic), cb_found);

	/* If not found, check for per-listener plugins. */
	अगर(cb_found == शून्य && db.config->per_listener_settings){
		अगर(!context->listener){
			log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(शून्य, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			वापस MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_FIND(hh, opts->plugin_callbacks.control, base_संदेश->तथ्य.topic, म_माप(base_संदेश->तथ्य.topic), cb_found);
	}
	अगर(cb_found){
		स_रखो(&event_तथ्य, 0, माप(event_तथ्य));
		event_तथ्य.client = context;
		event_तथ्य.topic = base_संदेश->तथ्य.topic;
		event_तथ्य.payload = base_संदेश->तथ्य.payload;
		event_तथ्य.payloadlen = base_संदेश->तथ्य.payloadlen;
		event_तथ्य.qos = base_संदेश->तथ्य.qos;
		event_तथ्य.retain = base_संदेश->तथ्य.retain;
		event_तथ्य.properties = base_संदेश->तथ्य.properties;
		event_तथ्य.reason_code = MQTT_RC_SUCCESS;
		event_तथ्य.reason_string = शून्य;

		rc = cb_found->cb(MOSQ_EVT_CONTROL, &event_तथ्य, cb_found->userdata);
		अगर(rc){
			अगर(context->protocol == mosq_p_mqtt5 && event_तथ्य.reason_string){
				/* Not a critical error if this fails */
				(व्योम)mosquitto_property_add_string(&properties, MQTT_PROP_REASON_STRING, event_तथ्य.reason_string);
			}
		}
		SAFE_FREE(event_तथ्य.reason_string);
	}अन्यथा{
		control__negative_reply(context->id, base_संदेश->तथ्य.topic);
	}

	अगर(base_संदेश->तथ्य.qos == 1){
		rc2 = send__puback(context, base_संदेश->तथ्य.source_mid, MQTT_RC_SUCCESS, properties);
		अगर(rc2){
			rc = rc2;
		}
	}अन्यथा अगर(base_संदेश->तथ्य.qos == 2){
		rc2 = send__pubrec(context, base_संदेश->तथ्य.source_mid, MQTT_RC_SUCCESS, properties);
		अगर(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	वापस rc;
}
#पूर्ण_अगर


पूर्णांक control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, स्थिर अक्षर *topic, व्योम *userdata)
{
#अगर_घोषित WITH_CONTROL
	काष्ठा mosquitto__security_options *opts;
	काष्ठा mosquitto__callback *cb_found, *cb_new;
	माप_प्रकार topic_len;

	अगर(topic == शून्य || cb_func == शून्य){
		वापस MOSQ_ERR_INVAL;
	}
	topic_len = म_माप(topic);
	अगर(topic_len == 0 || topic_len > 65535){
		वापस MOSQ_ERR_INVAL;
	}
	अगर(म_भेदन(topic, "$CONTROL/", म_माप("$CONTROL/")) || म_माप(topic) < म_माप("$CONTROL/A/v1")){
		वापस MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	अगर(cb_found){
		वापस MOSQ_ERR_ALREADY_EXISTS;
	}

	cb_new = mosquitto_calloc(1, माप(काष्ठा mosquitto__callback));
	अगर(cb_new == शून्य){
		वापस MOSQ_ERR_NOMEM;
	}
	cb_new->तथ्य.topic = mosquitto_strdup(topic);
	अगर(cb_new->तथ्य.topic == शून्य){
		mosquitto_FREE(cb_new);
		वापस MOSQ_ERR_NOMEM;
	}
	cb_new->identifier = pid;
	cb_new->cb = cb_func;
	cb_new->userdata = userdata;
	HASH_ADD_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->तथ्य.topic, म_माप(cb_new->तथ्य.topic), cb_new);

	अगर(pid->plugin_name){
		काष्ठा control_endpoint *ep;
		ep = mosquitto_malloc(माप(काष्ठा control_endpoint) + topic_len + 2);
		अगर(ep){
			ep->अग्र = शून्य;
			ep->पूर्व = शून्य;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(शून्य, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	वापस MOSQ_ERR_SUCCESS;
#अन्यथा
	UNUSED(pid);
	UNUSED(cb_func);
	UNUSED(topic);
	UNUSED(userdata);
	वापस MOSQ_ERR_NOT_SUPPORTED;
#पूर्ण_अगर
}


पूर्णांक control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, स्थिर अक्षर *topic)
{
#अगर_घोषित WITH_CONTROL
	काष्ठा mosquitto__security_options *opts;

	काष्ठा mosquitto__callback *cb_found;
	माप_प्रकार topic_len;
	काष्ठा control_endpoint *ep;

	अगर(topic == शून्य){
		वापस MOSQ_ERR_INVAL;
	}
	topic_len = म_माप(topic);
	अगर(topic_len == 0 || topic_len > 65535){
		वापस MOSQ_ERR_INVAL;
	}
	अगर(म_भेदन(topic, "$CONTROL/", म_माप("$CONTROL/"))){
		वापस MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	अगर(cb_found && cb_found->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
		mosquitto_FREE(cb_found->तथ्य.topic);
		mosquitto_FREE(cb_found);

		DL_FOREACH(identifier->control_endpoints, ep){
			अगर(!म_भेद(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				अवरोध;
			}
		}
		वापस MOSQ_ERR_SUCCESS;
	}
	वापस MOSQ_ERR_NOT_FOUND;
#अन्यथा
	UNUSED(identifier);
	UNUSED(cb_func);
	UNUSED(topic);
	वापस MOSQ_ERR_NOT_SUPPORTED;
#पूर्ण_अगर
}


/* Unregister all control callbacks for a single plugin */
व्योम control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	काष्ठा mosquitto__security_options *opts;

	काष्ठा mosquitto__callback *cb_found;
	काष्ठा control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_SAFE(identifier->control_endpoints, ep, ep_tmp){
		HASH_FIND(hh, opts->plugin_callbacks.control, ep->topic, म_माप(ep->topic), cb_found);
		अगर(cb_found){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
			mosquitto_FREE(cb_found->तथ्य.topic);
			mosquitto_FREE(cb_found);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
