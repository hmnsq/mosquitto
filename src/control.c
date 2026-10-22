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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <utlist.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "send_mosq.h"

#ifdef WITH_CONTROL


ସ୍ଥିର ଶୂନ୍ୟ control__negative_reply(ଅଚଳ ଅକ୍ଷର *clientid, ଅଚଳ ଅକ୍ଷର *request_topic)
{
	size_t response_topic_len;
	ଅକ୍ଷର *response_topic;
	ଅଚଳ ଅକ୍ଷର payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = strlen(request_topic) + ଆକାର("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	ଯଦି(!response_topic){
		ଫେରନ୍ତୁ;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strlen(payload), payload, 0, false, ଖାଲି);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା control__process(ସଂରଚନା mosquitto *context, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ)
{
	ସଂରଚନା mosquitto__callback *cb_found;
	ସଂରଚନା mosquitto_evt_control event_data;
	ସଂରଚନା mosquitto__security_options *opts;
	mosquitto_property *properties = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_SUCCESS;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_FIND(hh, opts->plugin_callbacks.control, base_ସନ୍ଦେଶ->data.topic, strlen(base_ସନ୍ଦେଶ->data.topic), cb_found);

	/* If not found, check for per-listener plugins. */
	ଯଦି(cb_found == ଖାଲି && db.config->per_listener_settings){
		ଯଦି(!context->listener){
			log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(ଖାଲି, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_FIND(hh, opts->plugin_callbacks.control, base_ସନ୍ଦେଶ->data.topic, strlen(base_ସନ୍ଦେଶ->data.topic), cb_found);
	}
	ଯଦି(cb_found){
		memset(&event_data, 0, ଆକାର(event_data));
		event_data.client = context;
		event_data.topic = base_ସନ୍ଦେଶ->data.topic;
		event_data.payload = base_ସନ୍ଦେଶ->data.payload;
		event_data.payloadlen = base_ସନ୍ଦେଶ->data.payloadlen;
		event_data.qos = base_ସନ୍ଦେଶ->data.qos;
		event_data.retain = base_ସନ୍ଦେଶ->data.retain;
		event_data.properties = base_ସନ୍ଦେଶ->data.properties;
		event_data.reason_code = MQTT_RC_SUCCESS;
		event_data.reason_string = ଖାଲି;

		rc = cb_found->cb(MOSQ_EVT_CONTROL, &event_data, cb_found->userdata);
		ଯଦି(rc){
			ଯଦି(context->protocol == mosq_p_mqtt5 && event_data.reason_string){
				/* Not a critical error if this fails */
				(ଶୂନ୍ୟ)mosquitto_property_add_string(&properties, MQTT_PROP_REASON_STRING, event_data.reason_string);
			}
		}
		SAFE_FREE(event_data.reason_string);
	}ଅନ୍ୟଥା{
		control__negative_reply(context->id, base_ସନ୍ଦେଶ->data.topic);
	}

	ଯଦି(base_ସନ୍ଦେଶ->data.qos == 1){
		rc2 = send__puback(context, base_ସନ୍ଦେଶ->data.source_mid, MQTT_RC_SUCCESS, properties);
		ଯଦି(rc2){
			rc = rc2;
		}
	}ଅନ୍ୟଥା ଯଦି(base_ସନ୍ଦେଶ->data.qos == 2){
		rc2 = send__pubrec(context, base_ସନ୍ଦେଶ->data.source_mid, MQTT_RC_SUCCESS, properties);
		ଯଦି(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	ଫେରନ୍ତୁ rc;
}
#endif


ପୂର୍ଣ୍ଣସଂଖ୍ୟା control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ଅଚଳ ଅକ୍ଷର *topic, ଶୂନ୍ୟ *userdata)
{
#ifdef WITH_CONTROL
	ସଂରଚନା mosquitto__security_options *opts;
	ସଂରଚନା mosquitto__callback *cb_found, *cb_new;
	size_t topic_len;

	ଯଦି(topic == ଖାଲି || cb_func == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	topic_len = strlen(topic);
	ଯଦି(topic_len == 0 || topic_len > 65535){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଯଦି(strncmp(topic, "$CONTROL/", strlen("$CONTROL/")) || strlen(topic) < strlen("$CONTROL/A/v1")){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	ଯଦି(cb_found){
		ଫେରନ୍ତୁ MOSQ_ERR_ALREADY_EXISTS;
	}

	cb_new = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto__callback));
	ଯଦି(cb_new == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	cb_new->data.topic = mosquitto_strdup(topic);
	ଯଦି(cb_new->data.topic == ଖାଲି){
		mosquitto_FREE(cb_new);
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	cb_new->identifier = pid;
	cb_new->cb = cb_func;
	cb_new->userdata = userdata;
	HASH_ADD_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->data.topic, strlen(cb_new->data.topic), cb_new);

	ଯଦି(pid->plugin_name){
		ସଂରଚନା control_endpoint *ep;
		ep = mosquitto_malloc(ଆକାର(ସଂରଚନା control_endpoint) + topic_len + 2);
		ଯଦି(ep){
			ep->next = ଖାଲି;
			ep->prev = ଖାଲି;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
#ଅନ୍ୟଥା
	UNUSED(pid);
	UNUSED(cb_func);
	UNUSED(topic);
	UNUSED(userdata);
	ଫେରନ୍ତୁ MOSQ_ERR_NOT_SUPPORTED;
#endif
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, ଅଚଳ ଅକ୍ଷର *topic)
{
#ifdef WITH_CONTROL
	ସଂରଚନା mosquitto__security_options *opts;

	ସଂରଚନା mosquitto__callback *cb_found;
	size_t topic_len;
	ସଂରଚନା control_endpoint *ep;

	ଯଦି(topic == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	topic_len = strlen(topic);
	ଯଦି(topic_len == 0 || topic_len > 65535){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}
	ଯଦି(strncmp(topic, "$CONTROL/", strlen("$CONTROL/"))){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	ଯଦି(cb_found && cb_found->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
		mosquitto_FREE(cb_found->data.topic);
		mosquitto_FREE(cb_found);

		DL_FOREACH(identifier->control_endpoints, ep){
			ଯଦି(!strcmp(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				ଭାଙ୍ଗ;
			}
		}
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}
	ଫେରନ୍ତୁ MOSQ_ERR_NOT_FOUND;
#ଅନ୍ୟଥା
	UNUSED(identifier);
	UNUSED(cb_func);
	UNUSED(topic);
	ଫେରନ୍ତୁ MOSQ_ERR_NOT_SUPPORTED;
#endif
}


/* Unregister all control callbacks for a single plugin */
ଶୂନ୍ୟ control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	ସଂରଚନା mosquitto__security_options *opts;

	ସଂରଚନା mosquitto__callback *cb_found;
	ସଂରଚନା control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_SAFE(identifier->control_endpoints, ep, ep_tmp){
		HASH_FIND(hh, opts->plugin_callbacks.control, ep->topic, strlen(ep->topic), cb_found);
		ଯଦି(cb_found){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
			mosquitto_FREE(cb_found->data.topic);
			mosquitto_FREE(cb_found);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
