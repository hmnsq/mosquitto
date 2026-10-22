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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <utlist.h>

#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "send_mosq.h"

#ifdef WITH_CONTROL


স্থির শূন্য control__negative_reply(ধ্রুব বর্ণ *clientid, ধ্রুব বর্ণ *request_topic)
{
	size_t response_topic_len;
	বর্ণ *response_topic;
	ধ্রুব বর্ণ payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = strlen(request_topic) + পরিমাণ("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	যদি(!response_topic){
		প্রত্যাগম;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (পূর্ণাঙ্ক)strlen(payload), payload, 0, false, রিক্ত);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
পূর্ণাঙ্ক control__process(সংরচনা mosquitto *context, সংরচনা mosquitto_base_বার্তা *base_বার্তা)
{
	সংরচনা mosquitto__callback *cb_found;
	সংরচনা mosquitto_evt_control event_data;
	সংরচনা mosquitto__security_options *opts;
	mosquitto_property *properties = রিক্ত;
	পূর্ণাঙ্ক rc = MOSQ_ERR_SUCCESS;
	পূর্ণাঙ্ক rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_FIND(hh, opts->plugin_callbacks.control, base_বার্তা->data.topic, strlen(base_বার্তা->data.topic), cb_found);

	/* If not found, check for per-listener plugins. */
	যদি(cb_found == রিক্ত && db.config->per_listener_settings){
		যদি(!context->listener){
			log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(রিক্ত, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			প্রত্যাগম MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_FIND(hh, opts->plugin_callbacks.control, base_বার্তা->data.topic, strlen(base_বার্তা->data.topic), cb_found);
	}
	যদি(cb_found){
		memset(&event_data, 0, পরিমাণ(event_data));
		event_data.client = context;
		event_data.topic = base_বার্তা->data.topic;
		event_data.payload = base_বার্তা->data.payload;
		event_data.payloadlen = base_বার্তা->data.payloadlen;
		event_data.qos = base_বার্তা->data.qos;
		event_data.retain = base_বার্তা->data.retain;
		event_data.properties = base_বার্তা->data.properties;
		event_data.reason_code = MQTT_RC_SUCCESS;
		event_data.reason_string = রিক্ত;

		rc = cb_found->cb(MOSQ_EVT_CONTROL, &event_data, cb_found->userdata);
		যদি(rc){
			যদি(context->protocol == mosq_p_mqtt5 && event_data.reason_string){
				/* Not a critical error if this fails */
				(শূন্য)mosquitto_property_add_string(&properties, MQTT_PROP_REASON_STRING, event_data.reason_string);
			}
		}
		SAFE_FREE(event_data.reason_string);
	}অন্যথা{
		control__negative_reply(context->id, base_বার্তা->data.topic);
	}

	যদি(base_বার্তা->data.qos == 1){
		rc2 = send__puback(context, base_বার্তা->data.source_mid, MQTT_RC_SUCCESS, properties);
		যদি(rc2){
			rc = rc2;
		}
	}অন্যথা যদি(base_বার্তা->data.qos == 2){
		rc2 = send__pubrec(context, base_বার্তা->data.source_mid, MQTT_RC_SUCCESS, properties);
		যদি(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	প্রত্যাগম rc;
}
#endif


পূর্ণাঙ্ক control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ধ্রুব বর্ণ *topic, শূন্য *userdata)
{
#ifdef WITH_CONTROL
	সংরচনা mosquitto__security_options *opts;
	সংরচনা mosquitto__callback *cb_found, *cb_new;
	size_t topic_len;

	যদি(topic == রিক্ত || cb_func == রিক্ত){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}
	topic_len = strlen(topic);
	যদি(topic_len == 0 || topic_len > 65535){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}
	যদি(strncmp(topic, "$CONTROL/", strlen("$CONTROL/")) || strlen(topic) < strlen("$CONTROL/A/v1")){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	যদি(cb_found){
		প্রত্যাগম MOSQ_ERR_ALREADY_EXISTS;
	}

	cb_new = mosquitto_calloc(1, পরিমাণ(সংরচনা mosquitto__callback));
	যদি(cb_new == রিক্ত){
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}
	cb_new->data.topic = mosquitto_strdup(topic);
	যদি(cb_new->data.topic == রিক্ত){
		mosquitto_FREE(cb_new);
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}
	cb_new->identifier = pid;
	cb_new->cb = cb_func;
	cb_new->userdata = userdata;
	HASH_ADD_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->data.topic, strlen(cb_new->data.topic), cb_new);

	যদি(pid->plugin_name){
		সংরচনা control_endpoint *ep;
		ep = mosquitto_malloc(পরিমাণ(সংরচনা control_endpoint) + topic_len + 2);
		যদি(ep){
			ep->next = রিক্ত;
			ep->prev = রিক্ত;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(রিক্ত, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
#অন্যথা
	UNUSED(pid);
	UNUSED(cb_func);
	UNUSED(topic);
	UNUSED(userdata);
	প্রত্যাগম MOSQ_ERR_NOT_SUPPORTED;
#endif
}


পূর্ণাঙ্ক control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, ধ্রুব বর্ণ *topic)
{
#ifdef WITH_CONTROL
	সংরচনা mosquitto__security_options *opts;

	সংরচনা mosquitto__callback *cb_found;
	size_t topic_len;
	সংরচনা control_endpoint *ep;

	যদি(topic == রিক্ত){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}
	topic_len = strlen(topic);
	যদি(topic_len == 0 || topic_len > 65535){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}
	যদি(strncmp(topic, "$CONTROL/", strlen("$CONTROL/"))){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	যদি(cb_found && cb_found->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
		mosquitto_FREE(cb_found->data.topic);
		mosquitto_FREE(cb_found);

		DL_FOREACH(identifier->control_endpoints, ep){
			যদি(!strcmp(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				ভঙ্গ;
			}
		}
		প্রত্যাগম MOSQ_ERR_SUCCESS;
	}
	প্রত্যাগম MOSQ_ERR_NOT_FOUND;
#অন্যথা
	UNUSED(identifier);
	UNUSED(cb_func);
	UNUSED(topic);
	প্রত্যাগম MOSQ_ERR_NOT_SUPPORTED;
#endif
}


/* Unregister all control callbacks for a single plugin */
শূন্য control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	সংরচনা mosquitto__security_options *opts;

	সংরচনা mosquitto__callback *cb_found;
	সংরচনা control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_SAFE(identifier->control_endpoints, ep, ep_tmp){
		HASH_FIND(hh, opts->plugin_callbacks.control, ep->topic, strlen(ep->topic), cb_found);
		যদি(cb_found){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
			mosquitto_FREE(cb_found->data.topic);
			mosquitto_FREE(cb_found);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
