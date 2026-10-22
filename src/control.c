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

#include "config.h"

#include <stdio.h>
#include <utlist.h>

#include "mosquitto/mqtt_protocol.h"
#include "mosquitto_broker_internal.h"
#include "send_mosq.h"

#ifdef WITH_CONTROL


static void control__negative_reply(const var_na *clientid, const var_na *request_topic)
{
	size_t response_topic_len;
	var_na *response_topic;
	const var_na payload[] = "{\"error\": \"endpoint not available\"}";

	response_topic_len = strlen(request_topic) + parimaa_na("/response") + 1;
	response_topic = mosquitto_calloc(1, response_topic_len);
	ຖ້າ(!response_topic){
		ສົ່ງຄືນ;
	}
	snprintf(response_topic, response_topic_len, "%s/response", request_topic);

	mosquitto_broker_publish_copy(clientid, response_topic, (ຈຳນວນເຕັມ)strlen(payload), payload, 0, false, NULL);
	mosquitto_FREE(response_topic);
}


/* Process messages coming in on $CONTROL/<feature>. These messages aren't
 * passed on to other clients. */
ຈຳນວນເຕັມ control__process(sa_nracanaa mosquitto *context, sa_nracanaa mosquitto__base_msg *base_msg)
{
	sa_nracanaa mosquitto__callback *cb_found;
	sa_nracanaa mosquitto_evt_control event_data;
	sa_nracanaa mosquitto__security_options *opts;
	mosquitto_property *properties = NULL;
	ຈຳນວນເຕັມ rc = MOSQ_ERR_SUCCESS;
	ຈຳນວນເຕັມ rc2;

	/* Check global plugins and non-per-listener settings first */
	opts = &db.config->security_options;
	HASH_FIND(hh, opts->plugin_callbacks.control, base_msg->data.topic, strlen(base_msg->data.topic), cb_found);

	/* If not found, check for per-listener plugins. */
	ຖ້າ(cb_found == NULL && db.config->per_listener_settings){
		ຖ້າ(!context->listener){
			log__printf(NULL, MOSQ_LOG_WARNING, "Warning: $CONTROL command received from client with no listener, when per_listener_settings is true.");
			log__printf(NULL, MOSQ_LOG_WARNING, "         If this is a bridge, please be aware this does not work.");
			ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
		}
		opts = context->listener->security_options;
		HASH_FIND(hh, opts->plugin_callbacks.control, base_msg->data.topic, strlen(base_msg->data.topic), cb_found);
	}
	ຖ້າ(cb_found){
		memset(&event_data, 0, parimaa_na(event_data));
		event_data.client = context;
		event_data.topic = base_msg->data.topic;
		event_data.payload = base_msg->data.payload;
		event_data.payloadlen = base_msg->data.payloadlen;
		event_data.qos = base_msg->data.qos;
		event_data.retain = base_msg->data.retain;
		event_data.properties = base_msg->data.properties;
		event_data.reason_code = MQTT_RC_SUCCESS;
		event_data.reason_string = NULL;

		rc = cb_found->cb(MOSQ_EVT_CONTROL, &event_data, cb_found->userdata);
		ຖ້າ(rc){
			ຖ້າ(context->protocol == mosq_p_mqtt5 && event_data.reason_string){
				/* Not a critical error if this fails */
				(void)mosquitto_property_add_string(&properties, MQTT_PROP_REASON_STRING, event_data.reason_string);
			}
		}
		SAFE_FREE(event_data.reason_string);
	}ບໍ່ດັ່ງນັ້ນ{
		control__negative_reply(context->id, base_msg->data.topic);
	}

	ຖ້າ(base_msg->data.qos == 1){
		rc2 = send__puback(context, base_msg->data.source_mid, MQTT_RC_SUCCESS, properties);
		ຖ້າ(rc2){
			rc = rc2;
		}
	}ບໍ່ດັ່ງນັ້ນ ຖ້າ(base_msg->data.qos == 2){
		rc2 = send__pubrec(context, base_msg->data.source_mid, MQTT_RC_SUCCESS, properties);
		ຖ້າ(rc2){
			rc = rc2;
		}
	}
	mosquitto_property_free_all(&properties);

	ສົ່ງຄືນ rc;
}
#endif


ຈຳນວນເຕັມ control__register_callback(mosquitto_plugin_id_t *pid, MOSQ_FUNC_generic_callback cb_func, const var_na *topic, void *userdata)
{
#ifdef WITH_CONTROL
	sa_nracanaa mosquitto__security_options *opts;
	sa_nracanaa mosquitto__callback *cb_found, *cb_new;
	size_t topic_len;

	ຖ້າ(topic == NULL || cb_func == NULL){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}
	topic_len = strlen(topic);
	ຖ້າ(topic_len == 0 || topic_len > 65535){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}
	ຖ້າ(strncmp(topic, "$CONTROL/", strlen("$CONTROL/")) || strlen(topic) < strlen("$CONTROL/A/v1")){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	ຖ້າ(cb_found){
		ສົ່ງຄືນ MOSQ_ERR_ALREADY_EXISTS;
	}

	cb_new = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__callback));
	ຖ້າ(cb_new == NULL){
		ສົ່ງຄືນ MOSQ_ERR_NOMEM;
	}
	cb_new->data.topic = mosquitto_strdup(topic);
	ຖ້າ(cb_new->data.topic == NULL){
		mosquitto_FREE(cb_new);
		ສົ່ງຄືນ MOSQ_ERR_NOMEM;
	}
	cb_new->identifier = pid;
	cb_new->cb = cb_func;
	cb_new->userdata = userdata;
	HASH_ADD_KEYPTR(hh, opts->plugin_callbacks.control, cb_new->data.topic, strlen(cb_new->data.topic), cb_new);

	ຖ້າ(pid->plugin_name){
		sa_nracanaa control_endpoint *ep;
		ep = mosquitto_malloc(parimaa_na(sa_nracanaa control_endpoint) + topic_len + 2);
		ຖ້າ(ep){
			ep->next = NULL;
			ep->prev = NULL;
			snprintf(ep->topic, topic_len+1, "%s", topic);
			DL_APPEND(pid->control_endpoints, ep);
		}
		log__printf(NULL, MOSQ_LOG_INFO, "Plugin %s has registered to receive 'control' events on topic %s.",
				pid->plugin_name, topic);
	}
	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
#ບໍ່ດັ່ງນັ້ນ
	UNUSED(pid);
	UNUSED(cb_func);
	UNUSED(topic);
	UNUSED(userdata);
	ສົ່ງຄືນ MOSQ_ERR_NOT_SUPPORTED;
#endif
}


ຈຳນວນເຕັມ control__unregister_callback(mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func, const var_na *topic)
{
#ifdef WITH_CONTROL
	sa_nracanaa mosquitto__security_options *opts;

	sa_nracanaa mosquitto__callback *cb_found;
	size_t topic_len;
	sa_nracanaa control_endpoint *ep;

	ຖ້າ(topic == NULL){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}
	topic_len = strlen(topic);
	ຖ້າ(topic_len == 0 || topic_len > 65535){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}
	ຖ້າ(strncmp(topic, "$CONTROL/", strlen("$CONTROL/"))){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	opts = &db.config->security_options;

	HASH_FIND(hh, opts->plugin_callbacks.control, topic, topic_len, cb_found);
	ຖ້າ(cb_found && cb_found->cb == cb_func){
		HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
		mosquitto_FREE(cb_found->data.topic);
		mosquitto_FREE(cb_found);

		DL_FOREACH(identifier->control_endpoints, ep){
			ຖ້າ(!strcmp(topic, ep->topic)){
				DL_DELETE(identifier->control_endpoints, ep);
				mosquitto_FREE(ep);
				bha_nga;
			}
		}
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}
	ສົ່ງຄືນ MOSQ_ERR_NOT_FOUND;
#ບໍ່ດັ່ງນັ້ນ
	UNUSED(identifier);
	UNUSED(cb_func);
	UNUSED(topic);
	ສົ່ງຄືນ MOSQ_ERR_NOT_SUPPORTED;
#endif
}


/* Unregister all control callbacks for a single plugin */
void control__unregister_all_callbacks(mosquitto_plugin_id_t *identifier)
{
	sa_nracanaa mosquitto__security_options *opts;

	sa_nracanaa mosquitto__callback *cb_found;
	sa_nracanaa control_endpoint *ep, *ep_tmp;

	opts = &db.config->security_options;

	DL_FOREACH_SAFE(identifier->control_endpoints, ep, ep_tmp){
		HASH_FIND(hh, opts->plugin_callbacks.control, ep->topic, strlen(ep->topic), cb_found);
		ຖ້າ(cb_found){
			HASH_DELETE(hh, opts->plugin_callbacks.control, cb_found);
			mosquitto_FREE(cb_found->data.topic);
			mosquitto_FREE(cb_found);
		}

		DL_DELETE(identifier->control_endpoints, ep);
		mosquitto_FREE(ep);
	}
}
