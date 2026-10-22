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

#جيڪڏهن_وصف_ٿيل WITH_CONTROL


ساكن فراغ control_negative_جواب(ثابت حرف *clientid, ثابت حرف *request_topic)
{
	size_t response_topic_len;
	حرف *response_topic;
	ثابت حرف payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = لڙي_ڊيگهه(request_topic) + حجم("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	إذا(!response_topic){
		أرجع;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (صحيح)لڙي_ڊيگهه(payload), payload, 0, false, عدم);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
صحيح control__process(بنية mosquitto *context, بنية mosquitto_بنياد_نياپو *بنياد_نياپو)
{
	بنية mosquitto__callback *cb_مليو;
	بنية mosquitto_evt_control event_معلومات;
	بنية mosquitto__security_options *opts;
	mosquitto_property *properties = عدم;
	صحيح rc = MOSQ_ERR_SUCCESS;
	صحيح rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_ڳوليو(hh, opts->plugin_callbacks.control, بنياد_نياپو->معلومات.topic, لڙي_ڊيگهه(بنياد_نياپو->معلومات.topic), cb_مليو);

	/* If not found, check for per-listener plugins. */
	إذا(cb_مليو == عدم && db.config->في_listener_settings){
		إذا(!context->listener){
			log__printf(عدم, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(عدم, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			أرجع MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_ڳوليو(hh, opts->plugin_callbacks.control, بنياد_نياپو->معلومات.topic, لڙي_ڊيگهه(بنياد_نياپو->معلومات.topic), cb_مليو);
	}
	إذا(cb_مليو){
		memset(&event_معلومات, 0, حجم(event_معلومات));
		event_معلومات.client = context;
		event_معلومات.topic = بنياد_نياپو->معلومات.topic;
		event_معلومات.payload = بنياد_نياپو->معلومات.payload;
		event_معلومات.payloadlen = بنياد_نياپو->معلومات.payloadlen;
		event_معلومات.qos = بنياد_نياپو->معلومات.qos;
		event_معلومات.retain = بنياد_نياپو->معلومات.retain;
		event_معلومات.properties = بنياد_نياپو->معلومات.properties;
		event_معلومات.reason_ڪوڊ = MQTT_RC_SUCCESS;
		event_معلومات.reason_string = عدم;

		rc = cb_مليو->cb(MOSQ_EVT_CONTROL, &event_معلومات, cb_مليو->userdata);
		إذا(rc){
			إذا(context->protocol == mosq_p_mqtt5 && event_معلومات.reason_string){
				/* Not a critical error if this fails */
				(فراغ)mosquitto_property_جوڙيو_string(&properties, MQTT_PROP_REASON_STRING, event_معلومات.reason_string);
			}
		}
		محفوظ_FREE(event_معلومات.reason_string);
	}وإلا{
		control_negative_جواب(context->id, بنياد_نياپو->معلومات.topic);
	}

	إذا(بنياد_نياپو->معلومات.qos == 1){
		rc2 = send__puback(context, بنياد_نياپو->معلومات.ذريعو_mid, MQTT_RC_SUCCESS, properties);
		إذا(rc2){
			rc = rc2;
		}
	}وإلا إذا(بنياد_نياپو->معلومات.qos == 2){
		rc2 = send__pubrec(context, بنياد_نياپو->معلومات.ذريعو_mid, MQTT_RC_SUCCESS, properties);
		إذا(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	أرجع rc;
}
#جيڪڏهن_پورو


صحيح control_رجسٽر_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic, فراغ *userdata)
{
#جيڪڏهن_وصف_ٿيل WITH_CONTROL
	بنية mosquitto__security_options *opts;
	بنية mosquitto__callback *cb_مليو, *cb_new;
	size_t topic_len;

	إذا(topic == عدم || cb_func == عدم){
		أرجع MOSQ_ERR_INVAL;
	}
	topic_len = لڙي_ڊيگهه(topic);
	إذا(topic_len == 0 || topic_len > 65535){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(لڙي_ڀيٽ(topic, "$CONTROL/", لڙي_ڊيگهه("$CONTROL/")) || لڙي_ڊيگهه(topic) < لڙي_ڊيگهه("$CONTROL/A/v1")){
		أرجع MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_ڳوليو(hh, opts->plugin_callbacks.control, topic, topic_len, cb_مليو);
	إذا(cb_مليو){
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
	HASH_جوڙيو_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->معلومات.topic, لڙي_ڊيگهه(cb_new->معلومات.topic), cb_new);

	إذا(pid->plugin_name){
		بنية control_endpoint *ep;
		ep = mosquitto_malloc(حجم(بنية control_endpoint) + topic_len + 2);
		إذا(ep){
			ep->ايندڙ = عدم;
			ep->prev = عدم;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(عدم, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	أرجع MOSQ_ERR_SUCCESS;
#وإلا
	اڻ_استعمال_ٿيل(pid);
	اڻ_استعمال_ٿيل(cb_func);
	اڻ_استعمال_ٿيل(topic);
	اڻ_استعمال_ٿيل(userdata);
	أرجع MOSQ_ERR_NOT_SUPPORTED;
#جيڪڏهن_پورو
}


صحيح control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, ثابت حرف *topic)
{
#جيڪڏهن_وصف_ٿيل WITH_CONTROL
	بنية mosquitto__security_options *opts;

	بنية mosquitto__callback *cb_مليو;
	size_t topic_len;
	بنية control_endpoint *ep;

	إذا(topic == عدم){
		أرجع MOSQ_ERR_INVAL;
	}
	topic_len = لڙي_ڊيگهه(topic);
	إذا(topic_len == 0 || topic_len > 65535){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(لڙي_ڀيٽ(topic, "$CONTROL/", لڙي_ڊيگهه("$CONTROL/"))){
		أرجع MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_ڳوليو(hh, opts->plugin_callbacks.control, topic, topic_len, cb_مليو);
	إذا(cb_مليو && cb_مليو->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_مليو);
		mosquitto_FREE(cb_مليو->معلومات.topic);
		mosquitto_FREE(cb_مليو);

		DL_FOREACH(identifier->control_endpoints, ep){
			إذا(!strcmp(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				اكسر;
			}
		}
		أرجع MOSQ_ERR_SUCCESS;
	}
	أرجع MOSQ_ERR_NOT_مليو;
#وإلا
	اڻ_استعمال_ٿيل(identifier);
	اڻ_استعمال_ٿيل(cb_func);
	اڻ_استعمال_ٿيل(topic);
	أرجع MOSQ_ERR_NOT_SUPPORTED;
#جيڪڏهن_پورو
}


/* Unregister all control callbacks for a single plugin */
فراغ control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	بنية mosquitto__security_options *opts;

	بنية mosquitto__callback *cb_مليو;
	بنية control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_محفوظ(identifier->control_endpoints, ep, ep_tmp){
		HASH_ڳوليو(hh, opts->plugin_callbacks.control, ep->topic, لڙي_ڊيگهه(ep->topic), cb_مليو);
		إذا(cb_مليو){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_مليو);
			mosquitto_FREE(cb_مليو->معلومات.topic);
			mosquitto_FREE(cb_مليو);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
