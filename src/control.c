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

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <utlist.h>

#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "send_mosq.h"

#ifdef WITH_CONTROL


ساكن فراغ control__negative_reply(ثابت حرف *clientid, ثابت حرف *request_topic)
{
	size_t response_topic_len;
	حرف *response_topic;
	ثابت حرف payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = strlen(request_topic) + حجم("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	إذا(!response_topic){
		إرجاع;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (صحيح)strlen(payload), payload, 0, false, عدم);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
صحيح control__process(بنية mosquitto *context, بنية mosquitto_base_رسالة *base_رسالة)
{
	بنية mosquitto__callback *cb_found;
	بنية mosquitto_evt_control event_data;
	بنية mosquitto__security_options *opts;
	mosquitto_property *properties = عدم;
	صحيح rc = MOSQ_ERR_SUCCESS;
	صحيح rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_FIND(hh, opts->plugin_callbacks.control, base_رسالة->data.topic, strlen(base_رسالة->data.topic), cb_found);

	/* If not found, check for per-listener plugins. */
	إذا(cb_found == عدم && db.config->per_listener_settings){
		إذا(!context->listener){
			log__printf(عدم, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(عدم, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			إرجاع MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_FIND(hh, opts->plugin_callbacks.control, base_رسالة->data.topic, strlen(base_رسالة->data.topic), cb_found);
	}
	إذا(cb_found){
		memset(&event_data, 0, حجم(event_data));
		event_data.client = context;
		event_data.topic = base_رسالة->data.topic;
		event_data.payload = base_رسالة->data.payload;
		event_data.payloadlen = base_رسالة->data.payloadlen;
		event_data.qos = base_رسالة->data.qos;
		event_data.retain = base_رسالة->data.retain;
		event_data.properties = base_رسالة->data.properties;
		event_data.reason_code = MQTT_RC_SUCCESS;
		event_data.reason_string = عدم;

		rc = cb_found->cb(MOSQ_EVT_CONTROL, &event_data, cb_found->userdata);
		إذا(rc){
			إذا(context->protocol == mosq_p_mqtt5 && event_data.reason_string){
				/* Not a critical error if this fails */
				(فراغ)mosquitto_property_add_string(&properties, MQTT_PROP_REASON_STRING, event_data.reason_string);
			}
		}
		SAFE_FREE(event_data.reason_string);
	}وإلا{
		control__negative_reply(context->id, base_رسالة->data.topic);
	}

	إذا(base_رسالة->data.qos == 1){
		rc2 = send__puback(context, base_رسالة->data.source_mid, MQTT_RC_SUCCESS, properties);
		إذا(rc2){
			rc = rc2;
		}
	}وإلا إذا(base_رسالة->data.qos == 2){
		rc2 = send__pubrec(context, base_رسالة->data.source_mid, MQTT_RC_SUCCESS, properties);
		إذا(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	إرجاع rc;
}
#endif


صحيح control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic, فراغ *userdata)
{
#ifdef WITH_CONTROL
	بنية mosquitto__security_options *opts;
	بنية mosquitto__callback *cb_found, *cb_new;
	size_t topic_len;

	إذا(topic == عدم || cb_func == عدم){
		إرجاع MOSQ_ERR_INVAL;
	}
	topic_len = strlen(topic);
	إذا(topic_len == 0 || topic_len > 65535){
		إرجاع MOSQ_ERR_INVAL;
	}
	إذا(strncmp(topic, "$CONTROL/", strlen("$CONTROL/")) || strlen(topic) < strlen("$CONTROL/A/v1")){
		إرجاع MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	إذا(cb_found){
		إرجاع MOSQ_ERR_ALREADY_EXISTS;
	}

	cb_new = mosquitto_calloc(1, حجم(بنية mosquitto__callback));
	إذا(cb_new == عدم){
		إرجاع MOSQ_ERR_NOMEM;
	}
	cb_new->data.topic = mosquitto_strdup(topic);
	إذا(cb_new->data.topic == عدم){
		mosquitto_FREE(cb_new);
		إرجاع MOSQ_ERR_NOMEM;
	}
	cb_new->identifier = pid;
	cb_new->cb = cb_func;
	cb_new->userdata = userdata;
	HASH_ADD_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->data.topic, strlen(cb_new->data.topic), cb_new);

	إذا(pid->plugin_name){
		بنية control_endpoint *ep;
		ep = mosquitto_malloc(حجم(بنية control_endpoint) + topic_len + 2);
		إذا(ep){
			ep->next = عدم;
			ep->prev = عدم;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(عدم, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	إرجاع MOSQ_ERR_SUCCESS;
#وإلا
	UNUSED(pid);
	UNUSED(cb_func);
	UNUSED(topic);
	UNUSED(userdata);
	إرجاع MOSQ_ERR_NOT_SUPPORTED;
#endif
}


صحيح control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic)
{
#ifdef WITH_CONTROL
	بنية mosquitto__security_options *opts;

	بنية mosquitto__callback *cb_found;
	size_t topic_len;
	بنية control_endpoint *ep;

	إذا(topic == عدم){
		إرجاع MOSQ_ERR_INVAL;
	}
	topic_len = strlen(topic);
	إذا(topic_len == 0 || topic_len > 65535){
		إرجاع MOSQ_ERR_INVAL;
	}
	إذا(strncmp(topic, "$CONTROL/", strlen("$CONTROL/"))){
		إرجاع MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	إذا(cb_found && cb_found->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
		mosquitto_FREE(cb_found->data.topic);
		mosquitto_FREE(cb_found);

		DL_FOREACH(identifier->control_endpoints, ep){
			إذا(!strcmp(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				اكسر;
			}
		}
		إرجاع MOSQ_ERR_SUCCESS;
	}
	إرجاع MOSQ_ERR_NOT_FOUND;
#وإلا
	UNUSED(identifier);
	UNUSED(cb_func);
	UNUSED(topic);
	إرجاع MOSQ_ERR_NOT_SUPPORTED;
#endif
}


/* Unregister all control callbacks for a single plugin */
فراغ control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	بنية mosquitto__security_options *opts;

	بنية mosquitto__callback *cb_found;
	بنية control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_SAFE(identifier->control_endpoints, ep, ep_tmp){
		HASH_FIND(hh, opts->plugin_callbacks.control, ep->topic, strlen(ep->topic), cb_found);
		إذا(cb_found){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
			mosquitto_FREE(cb_found->data.topic);
			mosquitto_FREE(cb_found);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
