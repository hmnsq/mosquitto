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


স্থির শূন্য control__negative_reply(ধ্রুবক অক্ষর *clientid, ধ্রুবক অক্ষর *request_topic)
{
	size_t response_topic_len;
	অক্ষর *response_topic;
	ধ্রুবক অক্ষর payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = strlen(request_topic) + আকার("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	যদি(!response_topic){
		ফেরত;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (পূর্ণসংখ্যা)strlen(payload), payload, 0, false, ফাঁকা);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
পূর্ণসংখ্যা control__process(কাঠামো mosquitto *context, কাঠামো mosquitto_base_বার্তা *base_বার্তা)
{
	কাঠামো mosquitto__callback *cb_found;
	কাঠামো mosquitto_evt_control event_তথ্য;
	কাঠামো mosquitto__security_options *opts;
	mosquitto_property *properties = ফাঁকা;
	পূর্ণসংখ্যা rc = MOSQ_ERR_SUCCESS;
	পূর্ণসংখ্যা rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_FIND(hh, opts->plugin_callbacks.control, base_বার্তা->তথ্য.topic, strlen(base_বার্তা->তথ্য.topic), cb_found);

	/* If not found, check for per-listener plugins. */
	যদি(cb_found == ফাঁকা && db.config->per_listener_settings){
		যদি(!context->listener){
			log__printf(ফাঁকা, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(ফাঁকা, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			ফেরত MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_FIND(hh, opts->plugin_callbacks.control, base_বার্তা->তথ্য.topic, strlen(base_বার্তা->তথ্য.topic), cb_found);
	}
	যদি(cb_found){
		memset(&event_তথ্য, 0, আকার(event_তথ্য));
		event_তথ্য.client = context;
		event_তথ্য.topic = base_বার্তা->তথ্য.topic;
		event_তথ্য.payload = base_বার্তা->তথ্য.payload;
		event_তথ্য.payloadlen = base_বার্তা->তথ্য.payloadlen;
		event_তথ্য.qos = base_বার্তা->তথ্য.qos;
		event_তথ্য.retain = base_বার্তা->তথ্য.retain;
		event_তথ্য.properties = base_বার্তা->তথ্য.properties;
		event_তথ্য.reason_code = MQTT_RC_SUCCESS;
		event_তথ্য.reason_string = ফাঁকা;

		rc = cb_found->cb(MOSQ_EVT_CONTROL, &event_তথ্য, cb_found->userdata);
		যদি(rc){
			যদি(context->protocol == mosq_p_mqtt5 && event_তথ্য.reason_string){
				/* Not a critical error if this fails */
				(শূন্য)mosquitto_property_add_string(&properties, MQTT_PROP_REASON_STRING, event_তথ্য.reason_string);
			}
		}
		SAFE_FREE(event_তথ্য.reason_string);
	}অন্যথা{
		control__negative_reply(context->id, base_বার্তা->তথ্য.topic);
	}

	যদি(base_বার্তা->তথ্য.qos == 1){
		rc2 = send__puback(context, base_বার্তা->তথ্য.source_mid, MQTT_RC_SUCCESS, properties);
		যদি(rc2){
			rc = rc2;
		}
	}অন্যথা যদি(base_বার্তা->তথ্য.qos == 2){
		rc2 = send__pubrec(context, base_বার্তা->তথ্য.source_mid, MQTT_RC_SUCCESS, properties);
		যদি(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	ফেরত rc;
}
#endif


পূর্ণসংখ্যা control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, ধ্রুবক অক্ষর *topic, শূন্য *userdata)
{
#ifdef WITH_CONTROL
	কাঠামো mosquitto__security_options *opts;
	কাঠামো mosquitto__callback *cb_found, *cb_new;
	size_t topic_len;

	যদি(topic == ফাঁকা || cb_func == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}
	topic_len = strlen(topic);
	যদি(topic_len == 0 || topic_len > 65535){
		ফেরত MOSQ_ERR_INVAL;
	}
	যদি(strncmp(topic, "$CONTROL/", strlen("$CONTROL/")) || strlen(topic) < strlen("$CONTROL/A/v1")){
		ফেরত MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	যদি(cb_found){
		ফেরত MOSQ_ERR_ALREADY_EXISTS;
	}

	cb_new = mosquitto_calloc(1, আকার(কাঠামো mosquitto__callback));
	যদি(cb_new == ফাঁকা){
		ফেরত MOSQ_ERR_NOMEM;
	}
	cb_new->তথ্য.topic = mosquitto_strdup(topic);
	যদি(cb_new->তথ্য.topic == ফাঁকা){
		mosquitto_FREE(cb_new);
		ফেরত MOSQ_ERR_NOMEM;
	}
	cb_new->identifier = pid;
	cb_new->cb = cb_func;
	cb_new->userdata = userdata;
	HASH_ADD_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->তথ্য.topic, strlen(cb_new->তথ্য.topic), cb_new);

	যদি(pid->plugin_name){
		কাঠামো control_endpoint *ep;
		ep = mosquitto_malloc(আকার(কাঠামো control_endpoint) + topic_len + 2);
		যদি(ep){
			ep->পরবর্তী = ফাঁকা;
			ep->পূর্ব = ফাঁকা;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	ফেরত MOSQ_ERR_SUCCESS;
#অন্যথা
	UNUSED(pid);
	UNUSED(cb_func);
	UNUSED(topic);
	UNUSED(userdata);
	ফেরত MOSQ_ERR_NOT_SUPPORTED;
#endif
}


পূর্ণসংখ্যা control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, ধ্রুবক অক্ষর *topic)
{
#ifdef WITH_CONTROL
	কাঠামো mosquitto__security_options *opts;

	কাঠামো mosquitto__callback *cb_found;
	size_t topic_len;
	কাঠামো control_endpoint *ep;

	যদি(topic == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}
	topic_len = strlen(topic);
	যদি(topic_len == 0 || topic_len > 65535){
		ফেরত MOSQ_ERR_INVAL;
	}
	যদি(strncmp(topic, "$CONTROL/", strlen("$CONTROL/"))){
		ফেরত MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	যদি(cb_found && cb_found->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
		mosquitto_FREE(cb_found->তথ্য.topic);
		mosquitto_FREE(cb_found);

		DL_FOREACH(identifier->control_endpoints, ep){
			যদি(!strcmp(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				বিরতি;
			}
		}
		ফেরত MOSQ_ERR_SUCCESS;
	}
	ফেরত MOSQ_ERR_NOT_FOUND;
#অন্যথা
	UNUSED(identifier);
	UNUSED(cb_func);
	UNUSED(topic);
	ফেরত MOSQ_ERR_NOT_SUPPORTED;
#endif
}


/* Unregister all control callbacks for a single plugin */
শূন্য control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	কাঠামো mosquitto__security_options *opts;

	কাঠামো mosquitto__callback *cb_found;
	কাঠামো control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_SAFE(identifier->control_endpoints, ep, ep_tmp){
		HASH_FIND(hh, opts->plugin_callbacks.control, ep->topic, strlen(ep->topic), cb_found);
		যদি(cb_found){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
			mosquitto_FREE(cb_found->তথ্য.topic);
			mosquitto_FREE(cb_found);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
