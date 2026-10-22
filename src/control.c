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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <utlist.h>

#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "send_mosq.h"

#ifdef WITH_CONTROL


സ്ഥിതം ശൂന്യം control__negative_reply(സ്ഥിരം അക്ഷരം *clientid, സ്ഥിരം അക്ഷരം *request_topic)
{
	size_t response_topic_len;
	അക്ഷരം *response_topic;
	സ്ഥിരം അക്ഷരം payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = strlen(request_topic) + വലുപ്പം("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	എങ്കിൽ(!response_topic){
		തിരികെ;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (പൂർണ്ണസംഖ്യ)strlen(payload), payload, 0, false, ശൂന്യ);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
പൂർണ്ണസംഖ്യ control__process(ഘടന mosquitto *context, ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം)
{
	ഘടന mosquitto__callback *cb_found;
	ഘടന mosquitto_evt_control event_data;
	ഘടന mosquitto__security_options *opts;
	mosquitto_property *properties = ശൂന്യ;
	പൂർണ്ണസംഖ്യ rc = MOSQ_ERR_SUCCESS;
	പൂർണ്ണസംഖ്യ rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_FIND(hh, opts->plugin_callbacks.control, base_സന്ദേശം->data.topic, strlen(base_സന്ദേശം->data.topic), cb_found);

	/* If not found, check for per-listener plugins. */
	എങ്കിൽ(cb_found == ശൂന്യ && db.config->per_listener_settings){
		എങ്കിൽ(!context->listener){
			log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			തിരികെ MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_FIND(hh, opts->plugin_callbacks.control, base_സന്ദേശം->data.topic, strlen(base_സന്ദേശം->data.topic), cb_found);
	}
	എങ്കിൽ(cb_found){
		memset(&event_data, 0, വലുപ്പം(event_data));
		event_data.client = context;
		event_data.topic = base_സന്ദേശം->data.topic;
		event_data.payload = base_സന്ദേശം->data.payload;
		event_data.payloadlen = base_സന്ദേശം->data.payloadlen;
		event_data.qos = base_സന്ദേശം->data.qos;
		event_data.retain = base_സന്ദേശം->data.retain;
		event_data.properties = base_സന്ദേശം->data.properties;
		event_data.reason_code = MQTT_RC_SUCCESS;
		event_data.reason_string = ശൂന്യ;

		rc = cb_found->cb(MOSQ_EVT_CONTROL, &event_data, cb_found->userdata);
		എങ്കിൽ(rc){
			എങ്കിൽ(context->protocol == mosq_p_mqtt5 && event_data.reason_string){
				/* Not a critical error if this fails */
				(ശൂന്യം)mosquitto_property_add_string(&properties, MQTT_PROP_REASON_STRING, event_data.reason_string);
			}
		}
		SAFE_FREE(event_data.reason_string);
	}അല്ലെങ്കിൽ{
		control__negative_reply(context->id, base_സന്ദേശം->data.topic);
	}

	എങ്കിൽ(base_സന്ദേശം->data.qos == 1){
		rc2 = send__puback(context, base_സന്ദേശം->data.source_mid, MQTT_RC_SUCCESS, properties);
		എങ്കിൽ(rc2){
			rc = rc2;
		}
	}അല്ലെങ്കിൽ എങ്കിൽ(base_സന്ദേശം->data.qos == 2){
		rc2 = send__pubrec(context, base_സന്ദേശം->data.source_mid, MQTT_RC_SUCCESS, properties);
		എങ്കിൽ(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	തിരികെ rc;
}
#endif


പൂർണ്ണസംഖ്യ control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, സ്ഥിരം അക്ഷരം *topic, ശൂന്യം *userdata)
{
#ifdef WITH_CONTROL
	ഘടന mosquitto__security_options *opts;
	ഘടന mosquitto__callback *cb_found, *cb_new;
	size_t topic_len;

	എങ്കിൽ(topic == ശൂന്യ || cb_func == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}
	topic_len = strlen(topic);
	എങ്കിൽ(topic_len == 0 || topic_len > 65535){
		തിരികെ MOSQ_ERR_INVAL;
	}
	എങ്കിൽ(strncmp(topic, "$CONTROL/", strlen("$CONTROL/")) || strlen(topic) < strlen("$CONTROL/A/v1")){
		തിരികെ MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	എങ്കിൽ(cb_found){
		തിരികെ MOSQ_ERR_ALREADY_EXISTS;
	}

	cb_new = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto__callback));
	എങ്കിൽ(cb_new == ശൂന്യ){
		തിരികെ MOSQ_ERR_NOMEM;
	}
	cb_new->data.topic = mosquitto_strdup(topic);
	എങ്കിൽ(cb_new->data.topic == ശൂന്യ){
		mosquitto_FREE(cb_new);
		തിരികെ MOSQ_ERR_NOMEM;
	}
	cb_new->identifier = pid;
	cb_new->cb = cb_func;
	cb_new->userdata = userdata;
	HASH_ADD_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->data.topic, strlen(cb_new->data.topic), cb_new);

	എങ്കിൽ(pid->plugin_name){
		ഘടന control_endpoint *ep;
		ep = mosquitto_malloc(വലുപ്പം(ഘടന control_endpoint) + topic_len + 2);
		എങ്കിൽ(ep){
			ep->next = ശൂന്യ;
			ep->prev = ശൂന്യ;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	തിരികെ MOSQ_ERR_SUCCESS;
#അല്ലെങ്കിൽ
	UNUSED(pid);
	UNUSED(cb_func);
	UNUSED(topic);
	UNUSED(userdata);
	തിരികെ MOSQ_ERR_NOT_SUPPORTED;
#endif
}


പൂർണ്ണസംഖ്യ control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, സ്ഥിരം അക്ഷരം *topic)
{
#ifdef WITH_CONTROL
	ഘടന mosquitto__security_options *opts;

	ഘടന mosquitto__callback *cb_found;
	size_t topic_len;
	ഘടന control_endpoint *ep;

	എങ്കിൽ(topic == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}
	topic_len = strlen(topic);
	എങ്കിൽ(topic_len == 0 || topic_len > 65535){
		തിരികെ MOSQ_ERR_INVAL;
	}
	എങ്കിൽ(strncmp(topic, "$CONTROL/", strlen("$CONTROL/"))){
		തിരികെ MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	എങ്കിൽ(cb_found && cb_found->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
		mosquitto_FREE(cb_found->data.topic);
		mosquitto_FREE(cb_found);

		DL_FOREACH(identifier->control_endpoints, ep){
			എങ്കിൽ(!strcmp(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				നിർത്തുക;
			}
		}
		തിരികെ MOSQ_ERR_SUCCESS;
	}
	തിരികെ MOSQ_ERR_NOT_FOUND;
#അല്ലെങ്കിൽ
	UNUSED(identifier);
	UNUSED(cb_func);
	UNUSED(topic);
	തിരികെ MOSQ_ERR_NOT_SUPPORTED;
#endif
}


/* Unregister all control callbacks for a single plugin */
ശൂന്യം control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	ഘടന mosquitto__security_options *opts;

	ഘടന mosquitto__callback *cb_found;
	ഘടന control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_SAFE(identifier->control_endpoints, ep, ep_tmp){
		HASH_FIND(hh, opts->plugin_callbacks.control, ep->topic, strlen(ep->topic), cb_found);
		എങ്കിൽ(cb_found){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
			mosquitto_FREE(cb_found->data.topic);
			mosquitto_FREE(cb_found);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
