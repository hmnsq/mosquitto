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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <utlist.h>

#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "send_mosq.h"

#ifdef WITH_CONTROL


நிலையான வெறுமை control__negative_reply(மாறிலி எழுத்து *clientid, மாறிலி எழுத்து *request_topic)
{
	size_t response_topic_len;
	எழுத்து *response_topic;
	மாறிலி எழுத்து payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = strlen(request_topic) + அளவு("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	எனில்(!response_topic){
		திரும்பு;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (முழு)strlen(payload), payload, 0, false, வெற்று);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
முழு control__process(கட்டமைப்பு mosquitto *context, கட்டமைப்பு mosquitto_base_செய்தி *base_செய்தி)
{
	கட்டமைப்பு mosquitto__callback *cb_found;
	கட்டமைப்பு mosquitto_evt_control event_data;
	கட்டமைப்பு mosquitto__security_options *opts;
	mosquitto_property *properties = வெற்று;
	முழு rc = MOSQ_ERR_SUCCESS;
	முழு rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_FIND(hh, opts->plugin_callbacks.control, base_செய்தி->data.topic, strlen(base_செய்தி->data.topic), cb_found);

	/* If not found, check for per-listener plugins. */
	எனில்(cb_found == வெற்று && db.config->per_listener_settings){
		எனில்(!context->listener){
			log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(வெற்று, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			திரும்பு MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_FIND(hh, opts->plugin_callbacks.control, base_செய்தி->data.topic, strlen(base_செய்தி->data.topic), cb_found);
	}
	எனில்(cb_found){
		memset(&event_data, 0, அளவு(event_data));
		event_data.client = context;
		event_data.topic = base_செய்தி->data.topic;
		event_data.payload = base_செய்தி->data.payload;
		event_data.payloadlen = base_செய்தி->data.payloadlen;
		event_data.qos = base_செய்தி->data.qos;
		event_data.retain = base_செய்தி->data.retain;
		event_data.properties = base_செய்தி->data.properties;
		event_data.reason_code = MQTT_RC_SUCCESS;
		event_data.reason_string = வெற்று;

		rc = cb_found->cb(MOSQ_EVT_CONTROL, &event_data, cb_found->userdata);
		எனில்(rc){
			எனில்(context->protocol == mosq_p_mqtt5 && event_data.reason_string){
				/* Not a critical error if this fails */
				(வெறுமை)mosquitto_property_add_string(&properties, MQTT_PROP_REASON_STRING, event_data.reason_string);
			}
		}
		SAFE_FREE(event_data.reason_string);
	}இல்லையெனில்{
		control__negative_reply(context->id, base_செய்தி->data.topic);
	}

	எனில்(base_செய்தி->data.qos == 1){
		rc2 = send__puback(context, base_செய்தி->data.source_mid, MQTT_RC_SUCCESS, properties);
		எனில்(rc2){
			rc = rc2;
		}
	}இல்லையெனில் எனில்(base_செய்தி->data.qos == 2){
		rc2 = send__pubrec(context, base_செய்தி->data.source_mid, MQTT_RC_SUCCESS, properties);
		எனில்(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	திரும்பு rc;
}
#endif


முழு control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, மாறிலி எழுத்து *topic, வெறுமை *userdata)
{
#ifdef WITH_CONTROL
	கட்டமைப்பு mosquitto__security_options *opts;
	கட்டமைப்பு mosquitto__callback *cb_found, *cb_new;
	size_t topic_len;

	எனில்(topic == வெற்று || cb_func == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}
	topic_len = strlen(topic);
	எனில்(topic_len == 0 || topic_len > 65535){
		திரும்பு MOSQ_ERR_INVAL;
	}
	எனில்(strncmp(topic, "$CONTROL/", strlen("$CONTROL/")) || strlen(topic) < strlen("$CONTROL/A/v1")){
		திரும்பு MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	எனில்(cb_found){
		திரும்பு MOSQ_ERR_ALREADY_EXISTS;
	}

	cb_new = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto__callback));
	எனில்(cb_new == வெற்று){
		திரும்பு MOSQ_ERR_NOMEM;
	}
	cb_new->data.topic = mosquitto_strdup(topic);
	எனில்(cb_new->data.topic == வெற்று){
		mosquitto_FREE(cb_new);
		திரும்பு MOSQ_ERR_NOMEM;
	}
	cb_new->identifier = pid;
	cb_new->cb = cb_func;
	cb_new->userdata = userdata;
	HASH_ADD_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->data.topic, strlen(cb_new->data.topic), cb_new);

	எனில்(pid->plugin_name){
		கட்டமைப்பு control_endpoint *ep;
		ep = mosquitto_malloc(அளவு(கட்டமைப்பு control_endpoint) + topic_len + 2);
		எனில்(ep){
			ep->next = வெற்று;
			ep->prev = வெற்று;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(வெற்று, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	திரும்பு MOSQ_ERR_SUCCESS;
#இல்லையெனில்
	UNUSED(pid);
	UNUSED(cb_func);
	UNUSED(topic);
	UNUSED(userdata);
	திரும்பு MOSQ_ERR_NOT_SUPPORTED;
#endif
}


முழு control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, மாறிலி எழுத்து *topic)
{
#ifdef WITH_CONTROL
	கட்டமைப்பு mosquitto__security_options *opts;

	கட்டமைப்பு mosquitto__callback *cb_found;
	size_t topic_len;
	கட்டமைப்பு control_endpoint *ep;

	எனில்(topic == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}
	topic_len = strlen(topic);
	எனில்(topic_len == 0 || topic_len > 65535){
		திரும்பு MOSQ_ERR_INVAL;
	}
	எனில்(strncmp(topic, "$CONTROL/", strlen("$CONTROL/"))){
		திரும்பு MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	எனில்(cb_found && cb_found->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
		mosquitto_FREE(cb_found->data.topic);
		mosquitto_FREE(cb_found);

		DL_FOREACH(identifier->control_endpoints, ep){
			எனில்(!strcmp(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				நிறுத்து;
			}
		}
		திரும்பு MOSQ_ERR_SUCCESS;
	}
	திரும்பு MOSQ_ERR_NOT_FOUND;
#இல்லையெனில்
	UNUSED(identifier);
	UNUSED(cb_func);
	UNUSED(topic);
	திரும்பு MOSQ_ERR_NOT_SUPPORTED;
#endif
}


/* Unregister all control callbacks for a single plugin */
வெறுமை control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	கட்டமைப்பு mosquitto__security_options *opts;

	கட்டமைப்பு mosquitto__callback *cb_found;
	கட்டமைப்பு control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_SAFE(identifier->control_endpoints, ep, ep_tmp){
		HASH_FIND(hh, opts->plugin_callbacks.control, ep->topic, strlen(ep->topic), cb_found);
		எனில்(cb_found){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
			mosquitto_FREE(cb_found->data.topic);
			mosquitto_FREE(cb_found);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
