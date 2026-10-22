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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <utlist.h>

#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "send_mosq.h"

#ifdef WITH_CONTROL


સ્થિર શૂન્ય control__negative_reply(અચલ અક્ષર *clientid, અચલ અક્ષર *request_topic)
{
	size_t response_topic_len;
	અક્ષર *response_topic;
	અચલ અક્ષર payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = strlen(request_topic) + કદ("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	જો(!response_topic){
		પાછા;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (પૂર્ણાંક)strlen(payload), payload, 0, false, ખાલી);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
પૂર્ણાંક control__process(સંરચના mosquitto *context, સંરચના mosquitto_base_સંદેશ *base_સંદેશ)
{
	સંરચના mosquitto__callback *cb_found;
	સંરચના mosquitto_evt_control event_data;
	સંરચના mosquitto__security_options *opts;
	mosquitto_property *properties = ખાલી;
	પૂર્ણાંક rc = MOSQ_ERR_SUCCESS;
	પૂર્ણાંક rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_FIND(hh, opts->plugin_callbacks.control, base_સંદેશ->data.topic, strlen(base_સંદેશ->data.topic), cb_found);

	/* If not found, check for per-listener plugins. */
	જો(cb_found == ખાલી && db.config->per_listener_settings){
		જો(!context->listener){
			log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(ખાલી, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			પાછા MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_FIND(hh, opts->plugin_callbacks.control, base_સંદેશ->data.topic, strlen(base_સંદેશ->data.topic), cb_found);
	}
	જો(cb_found){
		memset(&event_data, 0, કદ(event_data));
		event_data.client = context;
		event_data.topic = base_સંદેશ->data.topic;
		event_data.payload = base_સંદેશ->data.payload;
		event_data.payloadlen = base_સંદેશ->data.payloadlen;
		event_data.qos = base_સંદેશ->data.qos;
		event_data.retain = base_સંદેશ->data.retain;
		event_data.properties = base_સંદેશ->data.properties;
		event_data.reason_code = MQTT_RC_SUCCESS;
		event_data.reason_string = ખાલી;

		rc = cb_found->cb(MOSQ_EVT_CONTROL, &event_data, cb_found->userdata);
		જો(rc){
			જો(context->protocol == mosq_p_mqtt5 && event_data.reason_string){
				/* Not a critical error if this fails */
				(શૂન્ય)mosquitto_property_add_string(&properties, MQTT_PROP_REASON_STRING, event_data.reason_string);
			}
		}
		SAFE_FREE(event_data.reason_string);
	}નહીંતર{
		control__negative_reply(context->id, base_સંદેશ->data.topic);
	}

	જો(base_સંદેશ->data.qos == 1){
		rc2 = send__puback(context, base_સંદેશ->data.source_mid, MQTT_RC_SUCCESS, properties);
		જો(rc2){
			rc = rc2;
		}
	}નહીંતર જો(base_સંદેશ->data.qos == 2){
		rc2 = send__pubrec(context, base_સંદેશ->data.source_mid, MQTT_RC_SUCCESS, properties);
		જો(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	પાછા rc;
}
#endif


પૂર્ણાંક control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, અચલ અક્ષર *topic, શૂન્ય *userdata)
{
#ifdef WITH_CONTROL
	સંરચના mosquitto__security_options *opts;
	સંરચના mosquitto__callback *cb_found, *cb_new;
	size_t topic_len;

	જો(topic == ખાલી || cb_func == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}
	topic_len = strlen(topic);
	જો(topic_len == 0 || topic_len > 65535){
		પાછા MOSQ_ERR_INVAL;
	}
	જો(strncmp(topic, "$CONTROL/", strlen("$CONTROL/")) || strlen(topic) < strlen("$CONTROL/A/v1")){
		પાછા MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	જો(cb_found){
		પાછા MOSQ_ERR_ALREADY_EXISTS;
	}

	cb_new = mosquitto_calloc(1, કદ(સંરચના mosquitto__callback));
	જો(cb_new == ખાલી){
		પાછા MOSQ_ERR_NOMEM;
	}
	cb_new->data.topic = mosquitto_strdup(topic);
	જો(cb_new->data.topic == ખાલી){
		mosquitto_FREE(cb_new);
		પાછા MOSQ_ERR_NOMEM;
	}
	cb_new->identifier = pid;
	cb_new->cb = cb_func;
	cb_new->userdata = userdata;
	HASH_ADD_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->data.topic, strlen(cb_new->data.topic), cb_new);

	જો(pid->plugin_name){
		સંરચના control_endpoint *ep;
		ep = mosquitto_malloc(કદ(સંરચના control_endpoint) + topic_len + 2);
		જો(ep){
			ep->next = ખાલી;
			ep->prev = ખાલી;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(ખાલી, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	પાછા MOSQ_ERR_SUCCESS;
#નહીંતર
	UNUSED(pid);
	UNUSED(cb_func);
	UNUSED(topic);
	UNUSED(userdata);
	પાછા MOSQ_ERR_NOT_SUPPORTED;
#endif
}


પૂર્ણાંક control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, અચલ અક્ષર *topic)
{
#ifdef WITH_CONTROL
	સંરચના mosquitto__security_options *opts;

	સંરચના mosquitto__callback *cb_found;
	size_t topic_len;
	સંરચના control_endpoint *ep;

	જો(topic == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}
	topic_len = strlen(topic);
	જો(topic_len == 0 || topic_len > 65535){
		પાછા MOSQ_ERR_INVAL;
	}
	જો(strncmp(topic, "$CONTROL/", strlen("$CONTROL/"))){
		પાછા MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	જો(cb_found && cb_found->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
		mosquitto_FREE(cb_found->data.topic);
		mosquitto_FREE(cb_found);

		DL_FOREACH(identifier->control_endpoints, ep){
			જો(!strcmp(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				તોડો;
			}
		}
		પાછા MOSQ_ERR_SUCCESS;
	}
	પાછા MOSQ_ERR_NOT_FOUND;
#નહીંતર
	UNUSED(identifier);
	UNUSED(cb_func);
	UNUSED(topic);
	પાછા MOSQ_ERR_NOT_SUPPORTED;
#endif
}


/* Unregister all control callbacks for a single plugin */
શૂન્ય control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	સંરચના mosquitto__security_options *opts;

	સંરચના mosquitto__callback *cb_found;
	સંરચના control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_SAFE(identifier->control_endpoints, ep, ep_tmp){
		HASH_FIND(hh, opts->plugin_callbacks.control, ep->topic, strlen(ep->topic), cb_found);
		જો(cb_found){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
			mosquitto_FREE(cb_found->data.topic);
			mosquitto_FREE(cb_found);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
