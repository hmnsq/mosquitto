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

#اگر_تعریف_شده WITH_CONTROL


ساكن فراغ control_negative_پاسخ(ثابت حرف *clientid, ثابت حرف *request_topic)
{
	size_t response_topic_len;
	حرف *response_topic;
	ثابت حرف payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = طول_رشته(request_topic) + حجم("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	إذا(!response_topic){
		أرجع;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (صحيح)طول_رشته(payload), payload, 0, false, عدم);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
صحيح control__process(بنية mosquitto *context, بنية mosquitto_پایه_پیام *پایه_پیام)
{
	بنية mosquitto__callback *cb_یافت;
	بنية mosquitto_evt_control event_داده;
	بنية mosquitto__security_options *opts;
	mosquitto_property *properties = عدم;
	صحيح rc = MOSQ_ERR_SUCCESS;
	صحيح rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_پیدا_کن(hh, opts->plugin_callbacks.control, پایه_پیام->داده.topic, طول_رشته(پایه_پیام->داده.topic), cb_یافت);

	/* If not found, check for per-listener plugins. */
	إذا(cb_یافت == عدم && db.config->در_هر_listener_settings){
		إذا(!context->listener){
			log__printf(عدم, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(عدم, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			أرجع MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_پیدا_کن(hh, opts->plugin_callbacks.control, پایه_پیام->داده.topic, طول_رشته(پایه_پیام->داده.topic), cb_یافت);
	}
	إذا(cb_یافت){
		memset(&event_داده, 0, حجم(event_داده));
		event_داده.client = context;
		event_داده.topic = پایه_پیام->داده.topic;
		event_داده.payload = پایه_پیام->داده.payload;
		event_داده.payloadlen = پایه_پیام->داده.payloadlen;
		event_داده.qos = پایه_پیام->داده.qos;
		event_داده.retain = پایه_پیام->داده.retain;
		event_داده.properties = پایه_پیام->داده.properties;
		event_داده.reason_کد = MQTT_RC_SUCCESS;
		event_داده.reason_string = عدم;

		rc = cb_یافت->cb(MOSQ_EVT_CONTROL, &event_داده, cb_یافت->userdata);
		إذا(rc){
			إذا(context->protocol == mosq_p_mqtt5 && event_داده.reason_string){
				/* Not a critical error if this fails */
				(فراغ)mosquitto_property_جمع_کن_string(&properties, MQTT_PROP_REASON_STRING, event_داده.reason_string);
			}
		}
		امن_FREE(event_داده.reason_string);
	}وإلا{
		control_negative_پاسخ(context->id, پایه_پیام->داده.topic);
	}

	إذا(پایه_پیام->داده.qos == 1){
		rc2 = send__puback(context, پایه_پیام->داده.منبع_mid, MQTT_RC_SUCCESS, properties);
		إذا(rc2){
			rc = rc2;
		}
	}وإلا إذا(پایه_پیام->داده.qos == 2){
		rc2 = send__pubrec(context, پایه_پیام->داده.منبع_mid, MQTT_RC_SUCCESS, properties);
		إذا(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	أرجع rc;
}
#پایان_اگر


صحيح control_ثبات_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic, فراغ *userdata)
{
#اگر_تعریف_شده WITH_CONTROL
	بنية mosquitto__security_options *opts;
	بنية mosquitto__callback *cb_یافت, *cb_new;
	size_t topic_len;

	إذا(topic == عدم || cb_func == عدم){
		أرجع MOSQ_ERR_INVAL;
	}
	topic_len = طول_رشته(topic);
	إذا(topic_len == 0 || topic_len > 65535){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(مقایسه_رشته(topic, "$CONTROL/", طول_رشته("$CONTROL/")) || طول_رشته(topic) < طول_رشته("$CONTROL/A/v1")){
		أرجع MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_پیدا_کن(hh, opts->plugin_callbacks.control, topic, topic_len, cb_یافت);
	إذا(cb_یافت){
		أرجع MOSQ_ERR_ALREADY_EXISTS;
	}

	cb_new = mosquitto_calloc(1, حجم(بنية mosquitto__callback));
	إذا(cb_new == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}
	cb_new->داده.topic = mosquitto_strdup(topic);
	إذا(cb_new->داده.topic == عدم){
		mosquitto_FREE(cb_new);
		أرجع MOSQ_ERR_NOMEM;
	}
	cb_new->identifier = pid;
	cb_new->cb = cb_func;
	cb_new->userdata = userdata;
	HASH_جمع_کن_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->داده.topic, طول_رشته(cb_new->داده.topic), cb_new);

	إذا(pid->plugin_name){
		بنية control_endpoint *ep;
		ep = mosquitto_malloc(حجم(بنية control_endpoint) + topic_len + 2);
		إذا(ep){
			ep->بعدی = عدم;
			ep->prev = عدم;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(عدم, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	أرجع MOSQ_ERR_SUCCESS;
#وإلا
	استفادهنشده(pid);
	استفادهنشده(cb_func);
	استفادهنشده(topic);
	استفادهنشده(userdata);
	أرجع MOSQ_ERR_NOT_SUPPORTED;
#پایان_اگر
}


صحيح control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic)
{
#اگر_تعریف_شده WITH_CONTROL
	بنية mosquitto__security_options *opts;

	بنية mosquitto__callback *cb_یافت;
	size_t topic_len;
	بنية control_endpoint *ep;

	إذا(topic == عدم){
		أرجع MOSQ_ERR_INVAL;
	}
	topic_len = طول_رشته(topic);
	إذا(topic_len == 0 || topic_len > 65535){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(مقایسه_رشته(topic, "$CONTROL/", طول_رشته("$CONTROL/"))){
		أرجع MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_پیدا_کن(hh, opts->plugin_callbacks.control, topic, topic_len, cb_یافت);
	إذا(cb_یافت && cb_یافت->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_یافت);
		mosquitto_FREE(cb_یافت->داده.topic);
		mosquitto_FREE(cb_یافت);

		DL_FOREACH(identifier->control_endpoints, ep){
			إذا(!strcmp(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				اكسر;
			}
		}
		أرجع MOSQ_ERR_SUCCESS;
	}
	أرجع MOSQ_ERR_NOT_یافت;
#وإلا
	استفادهنشده(identifier);
	استفادهنشده(cb_func);
	استفادهنشده(topic);
	أرجع MOSQ_ERR_NOT_SUPPORTED;
#پایان_اگر
}


/* Unregister all control callbacks for a single plugin */
فراغ control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	بنية mosquitto__security_options *opts;

	بنية mosquitto__callback *cb_یافت;
	بنية control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_امن(identifier->control_endpoints, ep, ep_tmp){
		HASH_پیدا_کن(hh, opts->plugin_callbacks.control, ep->topic, طول_رشته(ep->topic), cb_یافت);
		إذا(cb_یافت){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_یافت);
			mosquitto_FREE(cb_یافت->داده.topic);
			mosquitto_FREE(cb_یافت);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
