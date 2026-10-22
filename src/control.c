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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_CONTROL


ಸ್ಥಿರ ಶೂನ್ಯ control_negative_ಉತ್ತರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clientid, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *request_topic)
{
	size_t response_topic_len;
	ಅಕ್ಷರ *response_topic;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(request_topic) + ಗಾತ್ರ("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	ಆದರೆ(!response_topic){
		ಹಿಂತಿರುಗಿ;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (ಪೂರ್ಣಾಂಕ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(payload), payload, 0, false, ಖಾಲಿ);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
ಪೂರ್ಣಾಂಕ control__process(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ)
{
	ರಚನೆ mosquitto__callback *cb_ಸಿಕ್ಕಿತು;
	ರಚನೆ mosquitto_evt_control event_ದತ್ತಾಂಶ;
	ರಚನೆ mosquitto__security_options *opts;
	mosquitto_property *properties = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ rc = MOSQ_ERR_SUCCESS;
	ಪೂರ್ಣಾಂಕ rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_ಕಂಡುಹಿಡಿ(hh, opts->plugin_callbacks.control, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic), cb_ಸಿಕ್ಕಿತು);

	/* If not found, check for per-listener plugins. */
	ಆದರೆ(cb_ಸಿಕ್ಕಿತು == ಖಾಲಿ && db.config->ಪ್ರತಿ_listener_settings){
		ಆದರೆ(!context->listener){
			log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_ಕಂಡುಹಿಡಿ(hh, opts->plugin_callbacks.control, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic), cb_ಸಿಕ್ಕಿತು);
	}
	ಆದರೆ(cb_ಸಿಕ್ಕಿತು){
		memset(&event_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(event_ದತ್ತಾಂಶ));
		event_ದತ್ತಾಂಶ.client = context;
		event_ದತ್ತಾಂಶ.topic = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic;
		event_ದತ್ತಾಂಶ.payload = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payload;
		event_ದತ್ತಾಂಶ.payloadlen = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen;
		event_ದತ್ತಾಂಶ.qos = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos;
		event_ದತ್ತಾಂಶ.retain = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.retain;
		event_ದತ್ತಾಂಶ.properties = ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.properties;
		event_ದತ್ತಾಂಶ.reason_ಸಂಕೇತ = MQTT_RC_SUCCESS;
		event_ದತ್ತಾಂಶ.reason_string = ಖಾಲಿ;

		rc = cb_ಸಿಕ್ಕಿತು->cb(MOSQ_EVT_CONTROL, &event_ದತ್ತಾಂಶ, cb_ಸಿಕ್ಕಿತು->userdata);
		ಆದರೆ(rc){
			ಆದರೆ(context->protocol == mosq_p_mqtt5 && event_ದತ್ತಾಂಶ.reason_string){
				/* Not a critical error if this fails */
				(ಶೂನ್ಯ)mosquitto_property_ಕೂಡಿಸು_string(&properties, MQTT_PROP_REASON_STRING, event_ದತ್ತಾಂಶ.reason_string);
			}
		}
		ಸುರಕ್ಷಿತ_FREE(event_ದತ್ತಾಂಶ.reason_string);
	}ಇಲ್ಲದಿದ್ದರೆ{
		control_negative_ಉತ್ತರ(context->id, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.topic);
	}

	ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos == 1){
		rc2 = send__puback(context, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid, MQTT_RC_SUCCESS, properties);
		ಆದರೆ(rc2){
			rc = rc2;
		}
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.qos == 2){
		rc2 = send__pubrec(context, ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.ಮೂಲ_mid, MQTT_RC_SUCCESS, properties);
		ಆದರೆ(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	ಹಿಂತಿರುಗಿ rc;
}
#ಆದರೆ_ಅಂತ್ಯ


ಪೂರ್ಣಾಂಕ control_ರಿಜಿಸ್ಟರ್_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, ಶೂನ್ಯ *userdata)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_CONTROL
	ರಚನೆ mosquitto__security_options *opts;
	ರಚನೆ mosquitto__callback *cb_ಸಿಕ್ಕಿತು, *cb_new;
	size_t topic_len;

	ಆದರೆ(topic == ಖಾಲಿ || cb_func == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	topic_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(topic);
	ಆದರೆ(topic_len == 0 || topic_len > 65535){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(topic, "$CONTROL/", ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("$CONTROL/")) || ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(topic) < ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("$CONTROL/A/v1")){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_ಕಂಡುಹಿಡಿ(hh, opts->plugin_callbacks.control, topic, topic_len, cb_ಸಿಕ್ಕಿತು);
	ಆದರೆ(cb_ಸಿಕ್ಕಿತು){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_ALREADY_EXISTS;
	}

	cb_new = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto__callback));
	ಆದರೆ(cb_new == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	cb_new->ದತ್ತಾಂಶ.topic = mosquitto_strdup(topic);
	ಆದರೆ(cb_new->ದತ್ತಾಂಶ.topic == ಖಾಲಿ){
		mosquitto_FREE(cb_new);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	cb_new->identifier = pid;
	cb_new->cb = cb_func;
	cb_new->userdata = userdata;
	HASH_ಕೂಡಿಸು_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->ದತ್ತಾಂಶ.topic, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cb_new->ದತ್ತಾಂಶ.topic), cb_new);

	ಆದರೆ(pid->plugin_name){
		ರಚನೆ control_endpoint *ep;
		ep = mosquitto_malloc(ಗಾತ್ರ(ರಚನೆ control_endpoint) + topic_len + 2);
		ಆದರೆ(ep){
			ep->ಮುಂದಿನ = ಖಾಲಿ;
			ep->prev = ಖಾಲಿ;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
#ಇಲ್ಲದಿದ್ದರೆ
	ಬಳಸದ(pid);
	ಬಳಸದ(cb_func);
	ಬಳಸದ(topic);
	ಬಳಸದ(userdata);
	ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_SUPPORTED;
#ಆದರೆ_ಅಂತ್ಯ
}


ಪೂರ್ಣಾಂಕ control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_CONTROL
	ರಚನೆ mosquitto__security_options *opts;

	ರಚನೆ mosquitto__callback *cb_ಸಿಕ್ಕಿತು;
	size_t topic_len;
	ರಚನೆ control_endpoint *ep;

	ಆದರೆ(topic == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	topic_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(topic);
	ಆದರೆ(topic_len == 0 || topic_len > 65535){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}
	ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(topic, "$CONTROL/", ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("$CONTROL/"))){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_ಕಂಡುಹಿಡಿ(hh, opts->plugin_callbacks.control, topic, topic_len, cb_ಸಿಕ್ಕಿತು);
	ಆದರೆ(cb_ಸಿಕ್ಕಿತು && cb_ಸಿಕ್ಕಿತು->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_ಸಿಕ್ಕಿತು);
		mosquitto_FREE(cb_ಸಿಕ್ಕಿತು->ದತ್ತಾಂಶ.topic);
		mosquitto_FREE(cb_ಸಿಕ್ಕಿತು);

		DL_FOREACH(identifier->control_endpoints, ep){
			ಆದರೆ(!strcmp(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				ನಿಲ್ಲಿಸು;
			}
		}
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_ಸಿಕ್ಕಿತು;
#ಇಲ್ಲದಿದ್ದರೆ
	ಬಳಸದ(identifier);
	ಬಳಸದ(cb_func);
	ಬಳಸದ(topic);
	ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_SUPPORTED;
#ಆದರೆ_ಅಂತ್ಯ
}


/* Unregister all control callbacks for a single plugin */
ಶೂನ್ಯ control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	ರಚನೆ mosquitto__security_options *opts;

	ರಚನೆ mosquitto__callback *cb_ಸಿಕ್ಕಿತು;
	ರಚನೆ control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_ಸುರಕ್ಷಿತ(identifier->control_endpoints, ep, ep_tmp){
		HASH_ಕಂಡುಹಿಡಿ(hh, opts->plugin_callbacks.control, ep->topic, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ep->topic), cb_ಸಿಕ್ಕಿತು);
		ಆದರೆ(cb_ಸಿಕ್ಕಿತು){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_ಸಿಕ್ಕಿತು);
			mosquitto_FREE(cb_ಸಿಕ್ಕಿತು->ದತ್ತಾಂಶ.topic);
			mosquitto_FREE(cb_ಸಿಕ್ಕಿತು);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
