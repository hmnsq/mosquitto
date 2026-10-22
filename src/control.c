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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <utlist.h>

#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"

#ifdef WITH_CONTROL


ಸ್ಥಿರ ಶೂನ್ಯ control__negative_reply(ಧ್ರುವ ವರ್ಣ *clientid, ಧ್ರುವ ವರ್ಣ *request_topic)
{
	size_t response_topic_len;
	ವರ್ಣ *response_topic;
	ಧ್ರುವ ವರ್ಣ payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = strlen(request_topic) + ಪರಿಮಾಣ("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	ಯದಿ(!response_topic){
		ಪ್ರತ್ಯಾಗಮ;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (ಪೂರ್ಣಾಙ್ಕ)strlen(payload), payload, 0, false, ರಿಕ್ತ);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
ಪೂರ್ಣಾಙ್ಕ control__process(ಸಂರಚನಾ mosquitto *context, ಸಂರಚನಾ mosquitto_base_ಸಂದೇಶ *base_ಸಂದೇಶ)
{
	ಸಂರಚನಾ mosquitto__callback *cb_found;
	ಸಂರಚನಾ mosquitto_evt_control event_data;
	ಸಂರಚನಾ mosquitto__security_options *opts;
	mosquitto_property *properties = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ rc = MOSQ_ERR_SUCCESS;
	ಪೂರ್ಣಾಙ್ಕ rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_FIND(hh, opts->plugin_callbacks.control, base_ಸಂದೇಶ->data.topic, strlen(base_ಸಂದೇಶ->data.topic), cb_found);

	/* If not found, check for per-listener plugins. */
	ಯದಿ(cb_found == ರಿಕ್ತ && db.config->per_listener_settings){
		ಯದಿ(!context->listener){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_FIND(hh, opts->plugin_callbacks.control, base_ಸಂದೇಶ->data.topic, strlen(base_ಸಂದೇಶ->data.topic), cb_found);
	}
	ಯದಿ(cb_found){
		memset(&event_data, 0, ಪರಿಮಾಣ(event_data));
		event_data.client = context;
		event_data.topic = base_ಸಂದೇಶ->data.topic;
		event_data.payload = base_ಸಂದೇಶ->data.payload;
		event_data.payloadlen = base_ಸಂದೇಶ->data.payloadlen;
		event_data.qos = base_ಸಂದೇಶ->data.qos;
		event_data.retain = base_ಸಂದೇಶ->data.retain;
		event_data.properties = base_ಸಂದೇಶ->data.properties;
		event_data.reason_code = MQTT_RC_SUCCESS;
		event_data.reason_string = ರಿಕ್ತ;

		rc = cb_found->cb(MOSQ_EVT_CONTROL, &event_data, cb_found->userdata);
		ಯದಿ(rc){
			ಯದಿ(context->protocol == mosq_p_mqtt5 && event_data.reason_string){
				/* Not a critical error if this fails */
				(ಶೂನ್ಯ)mosquitto_property_add_string(&properties, MQTT_PROP_REASON_STRING, event_data.reason_string);
			}
		}
		SAFE_FREE(event_data.reason_string);
	}ಅನ್ಯಥಾ{
		control__negative_reply(context->id, base_ಸಂದೇಶ->data.topic);
	}

	ಯದಿ(base_ಸಂದೇಶ->data.qos == 1){
		rc2 = send__puback(context, base_ಸಂದೇಶ->data.source_mid, MQTT_RC_SUCCESS, properties);
		ಯದಿ(rc2){
			rc = rc2;
		}
	}ಅನ್ಯಥಾ ಯದಿ(base_ಸಂದೇಶ->data.qos == 2){
		rc2 = send__pubrec(context, base_ಸಂದೇಶ->data.source_mid, MQTT_RC_SUCCESS, properties);
		ಯದಿ(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	ಪ್ರತ್ಯಾಗಮ rc;
}
#endif


ಪೂರ್ಣಾಙ್ಕ control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ಧ್ರುವ ವರ್ಣ *topic, ಶೂನ್ಯ *userdata)
{
#ifdef WITH_CONTROL
	ಸಂರಚನಾ mosquitto__security_options *opts;
	ಸಂರಚನಾ mosquitto__callback *cb_found, *cb_new;
	size_t topic_len;

	ಯದಿ(topic == ರಿಕ್ತ || cb_func == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	topic_len = strlen(topic);
	ಯದಿ(topic_len == 0 || topic_len > 65535){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಯದಿ(strncmp(topic, "$CONTROL/", strlen("$CONTROL/")) || strlen(topic) < strlen("$CONTROL/A/v1")){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	ಯದಿ(cb_found){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ALREADY_EXISTS;
	}

	cb_new = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__callback));
	ಯದಿ(cb_new == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	cb_new->data.topic = mosquitto_strdup(topic);
	ಯದಿ(cb_new->data.topic == ರಿಕ್ತ){
		mosquitto_FREE(cb_new);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	cb_new->identifier = pid;
	cb_new->cb = cb_func;
	cb_new->userdata = userdata;
	HASH_ADD_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->data.topic, strlen(cb_new->data.topic), cb_new);

	ಯದಿ(pid->plugin_name){
		ಸಂರಚನಾ control_endpoint *ep;
		ep = mosquitto_malloc(ಪರಿಮಾಣ(ಸಂರಚನಾ control_endpoint) + topic_len + 2);
		ಯದಿ(ep){
			ep->next = ರಿಕ್ತ;
			ep->prev = ರಿಕ್ತ;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
#ಅನ್ಯಥಾ
	UNUSED(pid);
	UNUSED(cb_func);
	UNUSED(topic);
	UNUSED(userdata);
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_SUPPORTED;
#endif
}


ಪೂರ್ಣಾಙ್ಕ control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, ಧ್ರುವ ವರ್ಣ *topic)
{
#ifdef WITH_CONTROL
	ಸಂರಚನಾ mosquitto__security_options *opts;

	ಸಂರಚನಾ mosquitto__callback *cb_found;
	size_t topic_len;
	ಸಂರಚನಾ control_endpoint *ep;

	ಯದಿ(topic == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	topic_len = strlen(topic);
	ಯದಿ(topic_len == 0 || topic_len > 65535){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}
	ಯದಿ(strncmp(topic, "$CONTROL/", strlen("$CONTROL/"))){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	ಯದಿ(cb_found && cb_found->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
		mosquitto_FREE(cb_found->data.topic);
		mosquitto_FREE(cb_found);

		DL_FOREACH(identifier->control_endpoints, ep){
			ಯದಿ(!strcmp(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				ಭಙ್ಗ;
			}
		}
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_FOUND;
#ಅನ್ಯಥಾ
	UNUSED(identifier);
	UNUSED(cb_func);
	UNUSED(topic);
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_SUPPORTED;
#endif
}


/* Unregister all control callbacks for a single plugin */
ಶೂನ್ಯ control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	ಸಂರಚನಾ mosquitto__security_options *opts;

	ಸಂರಚನಾ mosquitto__callback *cb_found;
	ಸಂರಚನಾ control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_SAFE(identifier->control_endpoints, ep, ep_tmp){
		HASH_FIND(hh, opts->plugin_callbacks.control, ep->topic, strlen(ep->topic), cb_found);
		ಯದಿ(cb_found){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
			mosquitto_FREE(cb_found->data.topic);
			mosquitto_FREE(cb_found);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
