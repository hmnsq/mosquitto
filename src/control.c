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

#شامل "config.h"

#شامل <stdio.h>
#شامل <utlist.h>

#شامل "mosquitto/mqtt_protocol.h"
#شامل "mosquitto_broker_internal.h"
#شامل "send_mosq.h"

#اگر_تعریف_شدہ WITH_CONTROL


ساکن خالی control_negative_جواب(ثابت حرف *clientid, ثابت حرف *request_topic)
{
	size_t response_topic_len;
	حرف *response_topic;
	ثابت حرف payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = لڑی_لمبائی(request_topic) + حجم("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	اگر(!response_topic){
		واپس;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (صحیح)لڑی_لمبائی(payload), payload, 0, false, تہی);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
صحیح control__process(ساخت mosquitto *context, ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام)
{
	ساخت mosquitto__callback *cb_ملا;
	ساخت mosquitto_evt_control event_ڈیٹا;
	ساخت mosquitto__security_options *opts;
	mosquitto_property *properties = تہی;
	صحیح rc = MOSQ_ERR_SUCCESS;
	صحیح rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_ڈھونڈو(hh, opts->plugin_callbacks.control, بنیاد_پیغام->ڈیٹا.topic, لڑی_لمبائی(بنیاد_پیغام->ڈیٹا.topic), cb_ملا);

	/* If not found, check for per-listener plugins. */
	اگر(cb_ملا == تہی && db.config->فی_listener_settings){
		اگر(!context->listener){
			log__printf(تہی, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(تہی, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			واپس MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_ڈھونڈو(hh, opts->plugin_callbacks.control, بنیاد_پیغام->ڈیٹا.topic, لڑی_لمبائی(بنیاد_پیغام->ڈیٹا.topic), cb_ملا);
	}
	اگر(cb_ملا){
		memset(&event_ڈیٹا, 0, حجم(event_ڈیٹا));
		event_ڈیٹا.client = context;
		event_ڈیٹا.topic = بنیاد_پیغام->ڈیٹا.topic;
		event_ڈیٹا.payload = بنیاد_پیغام->ڈیٹا.payload;
		event_ڈیٹا.payloadlen = بنیاد_پیغام->ڈیٹا.payloadlen;
		event_ڈیٹا.qos = بنیاد_پیغام->ڈیٹا.qos;
		event_ڈیٹا.retain = بنیاد_پیغام->ڈیٹا.retain;
		event_ڈیٹا.properties = بنیاد_پیغام->ڈیٹا.properties;
		event_ڈیٹا.reason_رمز = MQTT_RC_SUCCESS;
		event_ڈیٹا.reason_string = تہی;

		rc = cb_ملا->cb(MOSQ_EVT_CONTROL, &event_ڈیٹا, cb_ملا->userdata);
		اگر(rc){
			اگر(context->protocol == mosq_p_mqtt5 && event_ڈیٹا.reason_string){
				/* Not a critical error if this fails */
				(خالی)mosquitto_property_جوڑو_string(&properties, MQTT_PROP_REASON_STRING, event_ڈیٹا.reason_string);
			}
		}
		محفوظ_FREE(event_ڈیٹا.reason_string);
	}ورنہ{
		control_negative_جواب(context->id, بنیاد_پیغام->ڈیٹا.topic);
	}

	اگر(بنیاد_پیغام->ڈیٹا.qos == 1){
		rc2 = send__puback(context, بنیاد_پیغام->ڈیٹا.ماخذ_mid, MQTT_RC_SUCCESS, properties);
		اگر(rc2){
			rc = rc2;
		}
	}ورنہ اگر(بنیاد_پیغام->ڈیٹا.qos == 2){
		rc2 = send__pubrec(context, بنیاد_پیغام->ڈیٹا.ماخذ_mid, MQTT_RC_SUCCESS, properties);
		اگر(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	واپس rc;
}
#اگر_ختم


صحیح control_رجسٹر_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic, خالی *userdata)
{
#اگر_تعریف_شدہ WITH_CONTROL
	ساخت mosquitto__security_options *opts;
	ساخت mosquitto__callback *cb_ملا, *cb_new;
	size_t topic_len;

	اگر(topic == تہی || cb_func == تہی){
		واپس MOSQ_ERR_INVAL;
	}
	topic_len = لڑی_لمبائی(topic);
	اگر(topic_len == 0 || topic_len > 65535){
		واپس MOSQ_ERR_INVAL;
	}
	اگر(لڑی_موازنہ(topic, "$CONTROL/", لڑی_لمبائی("$CONTROL/")) || لڑی_لمبائی(topic) < لڑی_لمبائی("$CONTROL/A/v1")){
		واپس MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_ڈھونڈو(hh, opts->plugin_callbacks.control, topic, topic_len, cb_ملا);
	اگر(cb_ملا){
		واپس MOSQ_ERR_ALREADY_EXISTS;
	}

	cb_new = mosquitto_calloc(1, حجم(ساخت mosquitto__callback));
	اگر(cb_new == تہی){
		واپس MOSQ_ERR_NOMEM;
	}
	cb_new->ڈیٹا.topic = mosquitto_strdup(topic);
	اگر(cb_new->ڈیٹا.topic == تہی){
		mosquitto_FREE(cb_new);
		واپس MOSQ_ERR_NOMEM;
	}
	cb_new->identifier = pid;
	cb_new->cb = cb_func;
	cb_new->userdata = userdata;
	HASH_جوڑو_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->ڈیٹا.topic, لڑی_لمبائی(cb_new->ڈیٹا.topic), cb_new);

	اگر(pid->plugin_name){
		ساخت control_endpoint *ep;
		ep = mosquitto_malloc(حجم(ساخت control_endpoint) + topic_len + 2);
		اگر(ep){
			ep->اگلا = تہی;
			ep->پچھلا = تہی;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(تہی, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	واپس MOSQ_ERR_SUCCESS;
#ورنہ
	غیر_مستعمل(pid);
	غیر_مستعمل(cb_func);
	غیر_مستعمل(topic);
	غیر_مستعمل(userdata);
	واپس MOSQ_ERR_NOT_SUPPORTED;
#اگر_ختم
}


صحیح control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic)
{
#اگر_تعریف_شدہ WITH_CONTROL
	ساخت mosquitto__security_options *opts;

	ساخت mosquitto__callback *cb_ملا;
	size_t topic_len;
	ساخت control_endpoint *ep;

	اگر(topic == تہی){
		واپس MOSQ_ERR_INVAL;
	}
	topic_len = لڑی_لمبائی(topic);
	اگر(topic_len == 0 || topic_len > 65535){
		واپس MOSQ_ERR_INVAL;
	}
	اگر(لڑی_موازنہ(topic, "$CONTROL/", لڑی_لمبائی("$CONTROL/"))){
		واپس MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_ڈھونڈو(hh, opts->plugin_callbacks.control, topic, topic_len, cb_ملا);
	اگر(cb_ملا && cb_ملا->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_ملا);
		mosquitto_FREE(cb_ملا->ڈیٹا.topic);
		mosquitto_FREE(cb_ملا);

		DL_FOREACH(identifier->control_endpoints, ep){
			اگر(!strcmp(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				توڑو;
			}
		}
		واپس MOSQ_ERR_SUCCESS;
	}
	واپس MOSQ_ERR_NOT_ملا;
#ورنہ
	غیر_مستعمل(identifier);
	غیر_مستعمل(cb_func);
	غیر_مستعمل(topic);
	واپس MOSQ_ERR_NOT_SUPPORTED;
#اگر_ختم
}


/* Unregister all control callbacks for a single plugin */
خالی control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	ساخت mosquitto__security_options *opts;

	ساخت mosquitto__callback *cb_ملا;
	ساخت control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_محفوظ(identifier->control_endpoints, ep, ep_tmp){
		HASH_ڈھونڈو(hh, opts->plugin_callbacks.control, ep->topic, لڑی_لمبائی(ep->topic), cb_ملا);
		اگر(cb_ملا){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_ملا);
			mosquitto_FREE(cb_ملا->ڈیٹا.topic);
			mosquitto_FREE(cb_ملا);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
