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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <utlist.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "send_mosq.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_CONTROL


ਸਥਿਰ ਖਾਲੀ control_negative_ਜਵਾਬ(ਅਚਲ ਅੱਖਰ *clientid, ਅਚਲ ਅੱਖਰ *request_topic)
{
	size_t response_topic_len;
	ਅੱਖਰ *response_topic;
	ਅਚਲ ਅੱਖਰ payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = ਲੜੀ_ਲੰਬਾਈ(request_topic) + ਆਕਾਰ("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	ਜੇ(!response_topic){
		ਵਾਪਸ;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (ਪੂਰਨ)ਲੜੀ_ਲੰਬਾਈ(payload), payload, 0, false, ਨਿਰਾ);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
ਪੂਰਨ control__process(ਬਣਤਰ mosquitto *context, ਬਣਤਰ mosquitto_ਅਧਾਰ_ਸੁਨੇਹਾ *ਅਧਾਰ_ਸੁਨੇਹਾ)
{
	ਬਣਤਰ mosquitto__callback *cb_ਮਿਲਿਆ;
	ਬਣਤਰ mosquitto_evt_control event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto__security_options *opts;
	mosquitto_property *properties = ਨਿਰਾ;
	ਪੂਰਨ rc = MOSQ_ERR_SUCCESS;
	ਪੂਰਨ rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_ਲੱਭੋ(hh, opts->plugin_callbacks.control, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic, ਲੜੀ_ਲੰਬਾਈ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic), cb_ਮਿਲਿਆ);

	/* If not found, check for per-listener plugins. */
	ਜੇ(cb_ਮਿਲਿਆ == ਨਿਰਾ && db.config->ਪ੍ਰਤੀ_listener_settings){
		ਜੇ(!context->listener){
			log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_ਲੱਭੋ(hh, opts->plugin_callbacks.control, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic, ਲੜੀ_ਲੰਬਾਈ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic), cb_ਮਿਲਿਆ);
	}
	ਜੇ(cb_ਮਿਲਿਆ){
		memset(&event_ਅੰਕੜੇ, 0, ਆਕਾਰ(event_ਅੰਕੜੇ));
		event_ਅੰਕੜੇ.client = context;
		event_ਅੰਕੜੇ.topic = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic;
		event_ਅੰਕੜੇ.payload = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payload;
		event_ਅੰਕੜੇ.payloadlen = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.payloadlen;
		event_ਅੰਕੜੇ.qos = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos;
		event_ਅੰਕੜੇ.retain = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.retain;
		event_ਅੰਕੜੇ.properties = ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.properties;
		event_ਅੰਕੜੇ.reason_ਕੋਡ = MQTT_RC_SUCCESS;
		event_ਅੰਕੜੇ.reason_string = ਨਿਰਾ;

		rc = cb_ਮਿਲਿਆ->cb(MOSQ_EVT_CONTROL, &event_ਅੰਕੜੇ, cb_ਮਿਲਿਆ->userdata);
		ਜੇ(rc){
			ਜੇ(context->protocol == mosq_p_mqtt5 && event_ਅੰਕੜੇ.reason_string){
				/* Not a critical error if this fails */
				(ਖਾਲੀ)mosquitto_property_ਜੋੜੋ_string(&properties, MQTT_PROP_REASON_STRING, event_ਅੰਕੜੇ.reason_string);
			}
		}
		ਸੁਰੱਖਿਅਤ_FREE(event_ਅੰਕੜੇ.reason_string);
	}ਨਹੀਂਤਾਂ{
		control_negative_ਜਵਾਬ(context->id, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.topic);
	}

	ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos == 1){
		rc2 = send__puback(context, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid, MQTT_RC_SUCCESS, properties);
		ਜੇ(rc2){
			rc = rc2;
		}
	}ਨਹੀਂਤਾਂ ਜੇ(ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.qos == 2){
		rc2 = send__pubrec(context, ਅਧਾਰ_ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਸਰੋਤ_mid, MQTT_RC_SUCCESS, properties);
		ਜੇ(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	ਵਾਪਸ rc;
}
#ਜੇ_ਅੰਤ


ਪੂਰਨ control_ਰਜਿਸਟਰ_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ਅਚਲ ਅੱਖਰ *topic, ਖਾਲੀ *userdata)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_CONTROL
	ਬਣਤਰ mosquitto__security_options *opts;
	ਬਣਤਰ mosquitto__callback *cb_ਮਿਲਿਆ, *cb_new;
	size_t topic_len;

	ਜੇ(topic == ਨਿਰਾ || cb_func == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	topic_len = ਲੜੀ_ਲੰਬਾਈ(topic);
	ਜੇ(topic_len == 0 || topic_len > 65535){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਜੇ(ਲੜੀ_ਤੁਲਨਾ(topic, "$CONTROL/", ਲੜੀ_ਲੰਬਾਈ("$CONTROL/")) || ਲੜੀ_ਲੰਬਾਈ(topic) < ਲੜੀ_ਲੰਬਾਈ("$CONTROL/A/v1")){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_ਲੱਭੋ(hh, opts->plugin_callbacks.control, topic, topic_len, cb_ਮਿਲਿਆ);
	ਜੇ(cb_ਮਿਲਿਆ){
		ਵਾਪਸ MOSQ_ERR_ALREADY_EXISTS;
	}

	cb_new = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto__callback));
	ਜੇ(cb_new == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	cb_new->ਅੰਕੜੇ.topic = mosquitto_strdup(topic);
	ਜੇ(cb_new->ਅੰਕੜੇ.topic == ਨਿਰਾ){
		mosquitto_FREE(cb_new);
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	cb_new->identifier = pid;
	cb_new->cb = cb_func;
	cb_new->userdata = userdata;
	HASH_ਜੋੜੋ_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->ਅੰਕੜੇ.topic, ਲੜੀ_ਲੰਬਾਈ(cb_new->ਅੰਕੜੇ.topic), cb_new);

	ਜੇ(pid->plugin_name){
		ਬਣਤਰ control_endpoint *ep;
		ep = mosquitto_malloc(ਆਕਾਰ(ਬਣਤਰ control_endpoint) + topic_len + 2);
		ਜੇ(ep){
			ep->ਅਗਲਾ = ਨਿਰਾ;
			ep->prev = ਨਿਰਾ;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
#ਨਹੀਂਤਾਂ
	ਅਣਵਰਤਿਆ(pid);
	ਅਣਵਰਤਿਆ(cb_func);
	ਅਣਵਰਤਿਆ(topic);
	ਅਣਵਰਤਿਆ(userdata);
	ਵਾਪਸ MOSQ_ERR_NOT_SUPPORTED;
#ਜੇ_ਅੰਤ
}


ਪੂਰਨ control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, ਅਚਲ ਅੱਖਰ *topic)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_CONTROL
	ਬਣਤਰ mosquitto__security_options *opts;

	ਬਣਤਰ mosquitto__callback *cb_ਮਿਲਿਆ;
	size_t topic_len;
	ਬਣਤਰ control_endpoint *ep;

	ਜੇ(topic == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	topic_len = ਲੜੀ_ਲੰਬਾਈ(topic);
	ਜੇ(topic_len == 0 || topic_len > 65535){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}
	ਜੇ(ਲੜੀ_ਤੁਲਨਾ(topic, "$CONTROL/", ਲੜੀ_ਲੰਬਾਈ("$CONTROL/"))){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_ਲੱਭੋ(hh, opts->plugin_callbacks.control, topic, topic_len, cb_ਮਿਲਿਆ);
	ਜੇ(cb_ਮਿਲਿਆ && cb_ਮਿਲਿਆ->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_ਮਿਲਿਆ);
		mosquitto_FREE(cb_ਮਿਲਿਆ->ਅੰਕੜੇ.topic);
		mosquitto_FREE(cb_ਮਿਲਿਆ);

		DL_FOREACH(identifier->control_endpoints, ep){
			ਜੇ(!strcmp(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				ਰੋਕੋ;
			}
		}
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}
	ਵਾਪਸ MOSQ_ERR_NOT_ਮਿਲਿਆ;
#ਨਹੀਂਤਾਂ
	ਅਣਵਰਤਿਆ(identifier);
	ਅਣਵਰਤਿਆ(cb_func);
	ਅਣਵਰਤਿਆ(topic);
	ਵਾਪਸ MOSQ_ERR_NOT_SUPPORTED;
#ਜੇ_ਅੰਤ
}


/* Unregister all control callbacks for a single plugin */
ਖਾਲੀ control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	ਬਣਤਰ mosquitto__security_options *opts;

	ਬਣਤਰ mosquitto__callback *cb_ਮਿਲਿਆ;
	ਬਣਤਰ control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(identifier->control_endpoints, ep, ep_tmp){
		HASH_ਲੱਭੋ(hh, opts->plugin_callbacks.control, ep->topic, ਲੜੀ_ਲੰਬਾਈ(ep->topic), cb_ਮਿਲਿਆ);
		ਜੇ(cb_ਮਿਲਿਆ){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_ਮਿਲਿਆ);
			mosquitto_FREE(cb_ਮਿਲਿਆ->ਅੰਕੜੇ.topic);
			mosquitto_FREE(cb_ਮਿਲਿਆ);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
