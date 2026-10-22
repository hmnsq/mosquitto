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

#यदि_परिभाषितम् WITH_CONTROL


स्थिर शून्य control_negative_प्रत्युत्तरम्(ध्रुव वर्ण *clientid, ध्रुव वर्ण *request_topic)
{
	size_t response_topic_len;
	वर्ण *response_topic;
	ध्रुव वर्ण payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = वाक्यदैर्घ्यम्(request_topic) + परिमाण("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	यदि(!response_topic){
		प्रत्यागम;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (पूर्णाङ्क)वाक्यदैर्घ्यम्(payload), payload, 0, false, रिक्त);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
पूर्णाङ्क control__process(संरचना mosquitto *context, संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः)
{
	संरचना mosquitto__callback *cb_प्राप्तम्;
	संरचना mosquitto_evt_control event_दत्त;
	संरचना mosquitto__security_options *opts;
	mosquitto_property *properties = रिक्त;
	पूर्णाङ्क rc = MOSQ_ERR_SUCCESS;
	पूर्णाङ्क rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_FIND(hh, opts->plugin_callbacks.control, आधारः_सन्देशः->दत्त.topic, वाक्यदैर्घ्यम्(आधारः_सन्देशः->दत्त.topic), cb_प्राप्तम्);

	/* If not found, check for per-listener plugins. */
	यदि(cb_प्राप्तम् == रिक्त && db.config->प्रत्येकम्_listener_settings){
		यदि(!context->listener){
			log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(रिक्त, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_FIND(hh, opts->plugin_callbacks.control, आधारः_सन्देशः->दत्त.topic, वाक्यदैर्घ्यम्(आधारः_सन्देशः->दत्त.topic), cb_प्राप्तम्);
	}
	यदि(cb_प्राप्तम्){
		स्मृतिं_पूरय(&event_दत्त, 0, परिमाण(event_दत्त));
		event_दत्त.client = context;
		event_दत्त.topic = आधारः_सन्देशः->दत्त.topic;
		event_दत्त.payload = आधारः_सन्देशः->दत्त.payload;
		event_दत्त.payloadlen = आधारः_सन्देशः->दत्त.payloadlen;
		event_दत्त.qos = आधारः_सन्देशः->दत्त.qos;
		event_दत्त.retain = आधारः_सन्देशः->दत्त.retain;
		event_दत्त.properties = आधारः_सन्देशः->दत्त.properties;
		event_दत्त.reason_संहिता = MQTT_RC_SUCCESS;
		event_दत्त.reason_string = रिक्त;

		rc = cb_प्राप्तम्->cb(MOSQ_EVT_CONTROL, &event_दत्त, cb_प्राप्तम्->userdata);
		यदि(rc){
			यदि(context->protocol == mosq_p_mqtt5 && event_दत्त.reason_string){
				/* Not a critical error if this fails */
				(शून्य)mosquitto_property_सङ्कलय_string(&properties, MQTT_PROP_REASON_STRING, event_दत्त.reason_string);
			}
		}
		SAFE_FREE(event_दत्त.reason_string);
	}अन्यथा{
		control_negative_प्रत्युत्तरम्(context->id, आधारः_सन्देशः->दत्त.topic);
	}

	यदि(आधारः_सन्देशः->दत्त.qos == 1){
		rc2 = send__puback(context, आधारः_सन्देशः->दत्त.source_mid, MQTT_RC_SUCCESS, properties);
		यदि(rc2){
			rc = rc2;
		}
	}अन्यथा यदि(आधारः_सन्देशः->दत्त.qos == 2){
		rc2 = send__pubrec(context, आधारः_सन्देशः->दत्त.source_mid, MQTT_RC_SUCCESS, properties);
		यदि(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	प्रत्यागम rc;
}
#यदि_अन्तः


पूर्णाङ्क control_पञ्जिकाकोशः_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ध्रुव वर्ण *topic, शून्य *userdata)
{
#यदि_परिभाषितम् WITH_CONTROL
	संरचना mosquitto__security_options *opts;
	संरचना mosquitto__callback *cb_प्राप्तम्, *cb_new;
	size_t topic_len;

	यदि(topic == रिक्त || cb_func == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	topic_len = वाक्यदैर्घ्यम्(topic);
	यदि(topic_len == 0 || topic_len > 65535){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(वाक्ये_तुलय(topic, "$CONTROL/", वाक्यदैर्घ्यम्("$CONTROL/")) || वाक्यदैर्घ्यम्(topic) < वाक्यदैर्घ्यम्("$CONTROL/A/v1")){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_प्राप्तम्);
	यदि(cb_प्राप्तम्){
		प्रत्यागम MOSQ_ERR_ALREADY_EXISTS;
	}

	cb_new = mosquitto_calloc(1, परिमाण(संरचना mosquitto__callback));
	यदि(cb_new == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	cb_new->दत्त.topic = mosquitto_strdup(topic);
	यदि(cb_new->दत्त.topic == रिक्त){
		mosquitto_FREE(cb_new);
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	cb_new->identifier = pid;
	cb_new->cb = cb_func;
	cb_new->userdata = userdata;
	HASH_सङ्कलय_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->दत्त.topic, वाक्यदैर्घ्यम्(cb_new->दत्त.topic), cb_new);

	यदि(pid->plugin_name){
		संरचना control_endpoint *ep;
		ep = mosquitto_malloc(परिमाण(संरचना control_endpoint) + topic_len + 2);
		यदि(ep){
			ep->अग्र = रिक्त;
			ep->पूर्व = रिक्त;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(रिक्त, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
#अन्यथा
	अप्रयुक्तम्(pid);
	अप्रयुक्तम्(cb_func);
	अप्रयुक्तम्(topic);
	अप्रयुक्तम्(userdata);
	प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
#यदि_अन्तः
}


पूर्णाङ्क control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, ध्रुव वर्ण *topic)
{
#यदि_परिभाषितम् WITH_CONTROL
	संरचना mosquitto__security_options *opts;

	संरचना mosquitto__callback *cb_प्राप्तम्;
	size_t topic_len;
	संरचना control_endpoint *ep;

	यदि(topic == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	topic_len = वाक्यदैर्घ्यम्(topic);
	यदि(topic_len == 0 || topic_len > 65535){
		प्रत्यागम MOSQ_ERR_INVAL;
	}
	यदि(वाक्ये_तुलय(topic, "$CONTROL/", वाक्यदैर्घ्यम्("$CONTROL/"))){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_प्राप्तम्);
	यदि(cb_प्राप्तम् && cb_प्राप्तम्->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_प्राप्तम्);
		mosquitto_FREE(cb_प्राप्तम्->दत्त.topic);
		mosquitto_FREE(cb_प्राप्तम्);

		DL_FOREACH(identifier->control_endpoints, ep){
			यदि(!वाक्यं_तुलय(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				भङ्ग;
			}
		}
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}
	प्रत्यागम MOSQ_ERR_NOT_प्राप्तम्;
#अन्यथा
	अप्रयुक्तम्(identifier);
	अप्रयुक्तम्(cb_func);
	अप्रयुक्तम्(topic);
	प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
#यदि_अन्तः
}


/* Unregister all control callbacks for a single plugin */
शून्य control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	संरचना mosquitto__security_options *opts;

	संरचना mosquitto__callback *cb_प्राप्तम्;
	संरचना control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_SAFE(identifier->control_endpoints, ep, ep_tmp){
		HASH_FIND(hh, opts->plugin_callbacks.control, ep->topic, वाक्यदैर्घ्यम्(ep->topic), cb_प्राप्तम्);
		यदि(cb_प्राप्तम्){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_प्राप्तम्);
			mosquitto_FREE(cb_प्राप्तम्->दत्त.topic);
			mosquitto_FREE(cb_प्राप्तम्);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
