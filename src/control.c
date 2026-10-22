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

#جے_تعریف_شدہ WITH_CONTROL


ساكن فراغ control_negative_جواب(ثابت حرف *clientid, ثابت حرف *request_topic)
{
	size_t response_topic_len;
	حرف *response_topic;
	ثابت حرف payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = لڑی_لمبائی(request_topic) + حجم("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	إذا(!response_topic){
		أرجع;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (صحيح)لڑی_لمبائی(payload), payload, 0, false, عدم);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
صحيح control__process(بنية mosquitto *context, بنية mosquitto_ادھار_پیغام *ادھار_پیغام)
{
	بنية mosquitto__callback *cb_لبھیا;
	بنية mosquitto_evt_control event_انکڑے;
	بنية mosquitto__security_options *opts;
	mosquitto_property *properties = عدم;
	صحيح rc = MOSQ_ERR_SUCCESS;
	صحيح rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_لبھو(hh, opts->plugin_callbacks.control, ادھار_پیغام->انکڑے.topic, لڑی_لمبائی(ادھار_پیغام->انکڑے.topic), cb_لبھیا);

	/* If not found, check for per-listener plugins. */
	إذا(cb_لبھیا == عدم && db.config->فی_listener_settings){
		إذا(!context->listener){
			log__printf(عدم, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(عدم, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			أرجع MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_لبھو(hh, opts->plugin_callbacks.control, ادھار_پیغام->انکڑے.topic, لڑی_لمبائی(ادھار_پیغام->انکڑے.topic), cb_لبھیا);
	}
	إذا(cb_لبھیا){
		memset(&event_انکڑے, 0, حجم(event_انکڑے));
		event_انکڑے.client = context;
		event_انکڑے.topic = ادھار_پیغام->انکڑے.topic;
		event_انکڑے.payload = ادھار_پیغام->انکڑے.payload;
		event_انکڑے.payloadlen = ادھار_پیغام->انکڑے.payloadlen;
		event_انکڑے.qos = ادھار_پیغام->انکڑے.qos;
		event_انکڑے.retain = ادھار_پیغام->انکڑے.retain;
		event_انکڑے.properties = ادھار_پیغام->انکڑے.properties;
		event_انکڑے.reason_کوڈ = MQTT_RC_SUCCESS;
		event_انکڑے.reason_string = عدم;

		rc = cb_لبھیا->cb(MOSQ_EVT_CONTROL, &event_انکڑے, cb_لبھیا->userdata);
		إذا(rc){
			إذا(context->protocol == mosq_p_mqtt5 && event_انکڑے.reason_string){
				/* Not a critical error if this fails */
				(فراغ)mosquitto_property_جوڑو_string(&properties, MQTT_PROP_REASON_STRING, event_انکڑے.reason_string);
			}
		}
		محفوظ_FREE(event_انکڑے.reason_string);
	}وإلا{
		control_negative_جواب(context->id, ادھار_پیغام->انکڑے.topic);
	}

	إذا(ادھار_پیغام->انکڑے.qos == 1){
		rc2 = send__puback(context, ادھار_پیغام->انکڑے.سروت_mid, MQTT_RC_SUCCESS, properties);
		إذا(rc2){
			rc = rc2;
		}
	}وإلا إذا(ادھار_پیغام->انکڑے.qos == 2){
		rc2 = send__pubrec(context, ادھار_پیغام->انکڑے.سروت_mid, MQTT_RC_SUCCESS, properties);
		إذا(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	أرجع rc;
}
#جے_انت


صحيح control_رجسٹر_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic, فراغ *userdata)
{
#جے_تعریف_شدہ WITH_CONTROL
	بنية mosquitto__security_options *opts;
	بنية mosquitto__callback *cb_لبھیا, *cb_new;
	size_t topic_len;

	إذا(topic == عدم || cb_func == عدم){
		أرجع MOSQ_ERR_INVAL;
	}
	topic_len = لڑی_لمبائی(topic);
	إذا(topic_len == 0 || topic_len > 65535){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(لڑی_مقابلہ(topic, "$CONTROL/", لڑی_لمبائی("$CONTROL/")) || لڑی_لمبائی(topic) < لڑی_لمبائی("$CONTROL/A/v1")){
		أرجع MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_لبھو(hh, opts->plugin_callbacks.control, topic, topic_len, cb_لبھیا);
	إذا(cb_لبھیا){
		أرجع MOSQ_ERR_ALREADY_EXISTS;
	}

	cb_new = mosquitto_calloc(1, حجم(بنية mosquitto__callback));
	إذا(cb_new == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}
	cb_new->انکڑے.topic = mosquitto_strdup(topic);
	إذا(cb_new->انکڑے.topic == عدم){
		mosquitto_FREE(cb_new);
		أرجع MOSQ_ERR_NOMEM;
	}
	cb_new->identifier = pid;
	cb_new->cb = cb_func;
	cb_new->userdata = userdata;
	HASH_جوڑو_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->انکڑے.topic, لڑی_لمبائی(cb_new->انکڑے.topic), cb_new);

	إذا(pid->plugin_name){
		بنية control_endpoint *ep;
		ep = mosquitto_malloc(حجم(بنية control_endpoint) + topic_len + 2);
		إذا(ep){
			ep->اگلا = عدم;
			ep->prev = عدم;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(عدم, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	أرجع MOSQ_ERR_SUCCESS;
#وإلا
	بن_ورتیا(pid);
	بن_ورتیا(cb_func);
	بن_ورتیا(topic);
	بن_ورتیا(userdata);
	أرجع MOSQ_ERR_NOT_SUPPORTED;
#جے_انت
}


صحيح control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic)
{
#جے_تعریف_شدہ WITH_CONTROL
	بنية mosquitto__security_options *opts;

	بنية mosquitto__callback *cb_لبھیا;
	size_t topic_len;
	بنية control_endpoint *ep;

	إذا(topic == عدم){
		أرجع MOSQ_ERR_INVAL;
	}
	topic_len = لڑی_لمبائی(topic);
	إذا(topic_len == 0 || topic_len > 65535){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(لڑی_مقابلہ(topic, "$CONTROL/", لڑی_لمبائی("$CONTROL/"))){
		أرجع MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_لبھو(hh, opts->plugin_callbacks.control, topic, topic_len, cb_لبھیا);
	إذا(cb_لبھیا && cb_لبھیا->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_لبھیا);
		mosquitto_FREE(cb_لبھیا->انکڑے.topic);
		mosquitto_FREE(cb_لبھیا);

		DL_FOREACH(identifier->control_endpoints, ep){
			إذا(!strcmp(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				اكسر;
			}
		}
		أرجع MOSQ_ERR_SUCCESS;
	}
	أرجع MOSQ_ERR_NOT_لبھیا;
#وإلا
	بن_ورتیا(identifier);
	بن_ورتیا(cb_func);
	بن_ورتیا(topic);
	أرجع MOSQ_ERR_NOT_SUPPORTED;
#جے_انت
}


/* Unregister all control callbacks for a single plugin */
فراغ control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	بنية mosquitto__security_options *opts;

	بنية mosquitto__callback *cb_لبھیا;
	بنية control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_محفوظ(identifier->control_endpoints, ep, ep_tmp){
		HASH_لبھو(hh, opts->plugin_callbacks.control, ep->topic, لڑی_لمبائی(ep->topic), cb_لبھیا);
		إذا(cb_لبھیا){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_لبھیا);
			mosquitto_FREE(cb_لبھیا->انکڑے.topic);
			mosquitto_FREE(cb_لبھیا);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
