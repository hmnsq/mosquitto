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

#که_تعریف_شوی WITH_CONTROL


ساكن فراغ control_negative_ځواب(ثابت حرف *clientid, ثابت حرف *request_topic)
{
	size_t response_topic_len;
	حرف *response_topic;
	ثابت حرف payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = مزي_اوږدوالی(request_topic) + حجم("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	إذا(!response_topic){
		أرجع;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (صحيح)مزي_اوږدوالی(payload), payload, 0, false, عدم);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
صحيح control__process(بنية mosquitto *context, بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام)
{
	بنية mosquitto__callback *cb_وموندل;
	بنية mosquitto_evt_control event_معلومات;
	بنية mosquitto__security_options *opts;
	mosquitto_property *properties = عدم;
	صحيح rc = MOSQ_ERR_SUCCESS;
	صحيح rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_پیدا_کړه(hh, opts->plugin_callbacks.control, بنسټ_پیغام->معلومات.topic, مزي_اوږدوالی(بنسټ_پیغام->معلومات.topic), cb_وموندل);

	/* If not found, check for per-listener plugins. */
	إذا(cb_وموندل == عدم && db.config->فی_listener_settings){
		إذا(!context->listener){
			log__printf(عدم, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(عدم, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			أرجع MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_پیدا_کړه(hh, opts->plugin_callbacks.control, بنسټ_پیغام->معلومات.topic, مزي_اوږدوالی(بنسټ_پیغام->معلومات.topic), cb_وموندل);
	}
	إذا(cb_وموندل){
		memset(&event_معلومات, 0, حجم(event_معلومات));
		event_معلومات.client = context;
		event_معلومات.topic = بنسټ_پیغام->معلومات.topic;
		event_معلومات.payload = بنسټ_پیغام->معلومات.payload;
		event_معلومات.payloadlen = بنسټ_پیغام->معلومات.payloadlen;
		event_معلومات.qos = بنسټ_پیغام->معلومات.qos;
		event_معلومات.retain = بنسټ_پیغام->معلومات.retain;
		event_معلومات.properties = بنسټ_پیغام->معلومات.properties;
		event_معلومات.reason_کوډ = MQTT_RC_SUCCESS;
		event_معلومات.reason_string = عدم;

		rc = cb_وموندل->cb(MOSQ_EVT_CONTROL, &event_معلومات, cb_وموندل->userdata);
		إذا(rc){
			إذا(context->protocol == mosq_p_mqtt5 && event_معلومات.reason_string){
				/* Not a critical error if this fails */
				(فراغ)mosquitto_property_جمع_کړه_string(&properties, MQTT_PROP_REASON_STRING, event_معلومات.reason_string);
			}
		}
		خوندي_FREE(event_معلومات.reason_string);
	}وإلا{
		control_negative_ځواب(context->id, بنسټ_پیغام->معلومات.topic);
	}

	إذا(بنسټ_پیغام->معلومات.qos == 1){
		rc2 = send__puback(context, بنسټ_پیغام->معلومات.سرچینه_mid, MQTT_RC_SUCCESS, properties);
		إذا(rc2){
			rc = rc2;
		}
	}وإلا إذا(بنسټ_پیغام->معلومات.qos == 2){
		rc2 = send__pubrec(context, بنسټ_پیغام->معلومات.سرچینه_mid, MQTT_RC_SUCCESS, properties);
		إذا(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	أرجع rc;
}
#د_که_پای


صحيح control_ثبتوونکی_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic, فراغ *userdata)
{
#که_تعریف_شوی WITH_CONTROL
	بنية mosquitto__security_options *opts;
	بنية mosquitto__callback *cb_وموندل, *cb_new;
	size_t topic_len;

	إذا(topic == عدم || cb_func == عدم){
		أرجع MOSQ_ERR_INVAL;
	}
	topic_len = مزي_اوږدوالی(topic);
	إذا(topic_len == 0 || topic_len > 65535){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(مزي_پرتله(topic, "$CONTROL/", مزي_اوږدوالی("$CONTROL/")) || مزي_اوږدوالی(topic) < مزي_اوږدوالی("$CONTROL/A/v1")){
		أرجع MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_پیدا_کړه(hh, opts->plugin_callbacks.control, topic, topic_len, cb_وموندل);
	إذا(cb_وموندل){
		أرجع MOSQ_ERR_ALREADY_EXISTS;
	}

	cb_new = mosquitto_calloc(1, حجم(بنية mosquitto__callback));
	إذا(cb_new == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}
	cb_new->معلومات.topic = mosquitto_strdup(topic);
	إذا(cb_new->معلومات.topic == عدم){
		mosquitto_FREE(cb_new);
		أرجع MOSQ_ERR_NOMEM;
	}
	cb_new->identifier = pid;
	cb_new->cb = cb_func;
	cb_new->userdata = userdata;
	HASH_جمع_کړه_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->معلومات.topic, مزي_اوږدوالی(cb_new->معلومات.topic), cb_new);

	إذا(pid->plugin_name){
		بنية control_endpoint *ep;
		ep = mosquitto_malloc(حجم(بنية control_endpoint) + topic_len + 2);
		إذا(ep){
			ep->بل = عدم;
			ep->prev = عدم;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(عدم, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	أرجع MOSQ_ERR_SUCCESS;
#وإلا
	ناکارول_شوی(pid);
	ناکارول_شوی(cb_func);
	ناکارول_شوی(topic);
	ناکارول_شوی(userdata);
	أرجع MOSQ_ERR_NOT_SUPPORTED;
#د_که_پای
}


صحيح control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic)
{
#که_تعریف_شوی WITH_CONTROL
	بنية mosquitto__security_options *opts;

	بنية mosquitto__callback *cb_وموندل;
	size_t topic_len;
	بنية control_endpoint *ep;

	إذا(topic == عدم){
		أرجع MOSQ_ERR_INVAL;
	}
	topic_len = مزي_اوږدوالی(topic);
	إذا(topic_len == 0 || topic_len > 65535){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(مزي_پرتله(topic, "$CONTROL/", مزي_اوږدوالی("$CONTROL/"))){
		أرجع MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_پیدا_کړه(hh, opts->plugin_callbacks.control, topic, topic_len, cb_وموندل);
	إذا(cb_وموندل && cb_وموندل->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_وموندل);
		mosquitto_FREE(cb_وموندل->معلومات.topic);
		mosquitto_FREE(cb_وموندل);

		DL_FOREACH(identifier->control_endpoints, ep){
			إذا(!strcmp(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				اكسر;
			}
		}
		أرجع MOSQ_ERR_SUCCESS;
	}
	أرجع MOSQ_ERR_NOT_وموندل;
#وإلا
	ناکارول_شوی(identifier);
	ناکارول_شوی(cb_func);
	ناکارول_شوی(topic);
	أرجع MOSQ_ERR_NOT_SUPPORTED;
#د_که_پای
}


/* Unregister all control callbacks for a single plugin */
فراغ control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	بنية mosquitto__security_options *opts;

	بنية mosquitto__callback *cb_وموندل;
	بنية control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_خوندي(identifier->control_endpoints, ep, ep_tmp){
		HASH_پیدا_کړه(hh, opts->plugin_callbacks.control, ep->topic, مزي_اوږدوالی(ep->topic), cb_وموندل);
		إذا(cb_وموندل){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_وموندل);
			mosquitto_FREE(cb_وموندل->معلومات.topic);
			mosquitto_FREE(cb_وموندل);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
