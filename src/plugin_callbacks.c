/*
Copyright (c) 2016-2021 Roger Light <roger@atchoo.org>

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

#include "mosquitto_broker_internal.h"
#include "utlist.h"
#include "lib_load.h"


static const char *get_event_name(enum mosquitto_plugin_event event)
{
	switch(event){
		case MOSQ_EVT_RELOAD:
			врати "reload";
		case MOSQ_EVT_ACL_CHECK:
			врати "acl-check";
		case MOSQ_EVT_BASIC_AUTH:
			врати "basic-auth";
		case MOSQ_EVT_PSK_KEY:
			врати "psk-key";
		case MOSQ_EVT_EXT_AUTH_START:
			врати "auth-start";
		case MOSQ_EVT_EXT_AUTH_CONTINUE:
			врати "auth-continue";
		case MOSQ_EVT_CONTROL:
			врати "control";
		case MOSQ_EVT_MESSAGE_IN:
			врати "message-in";
		case MOSQ_EVT_MESSAGE_OUT:
			врати "message-out";
		case MOSQ_EVT_TICK:
			врати "tick";
		case MOSQ_EVT_DISCONNECT:
			врати "disconnect";
		case MOSQ_EVT_CONNECT:
			врати "connect";
		case MOSQ_EVT_CLIENT_OFFLINE:
			врати "connect";
		case MOSQ_EVT_SUBSCRIBE:
			врати "subscribe";
		case MOSQ_EVT_UNSUBSCRIBE:
			врати "unsubscribe";
		case MOSQ_EVT_PERSIST_RESTORE:
			врати "persist-restore";
		case MOSQ_EVT_PERSIST_BASE_MSG_ADD:
			врати "persist-base-msg-add";
		case MOSQ_EVT_PERSIST_BASE_MSG_DELETE:
			врати "persist-base-msg-delete";
		case MOSQ_EVT_PERSIST_RETAIN_MSG_SET:
			врати "persist-retain-msg-set";
		case MOSQ_EVT_PERSIST_RETAIN_MSG_DELETE:
			врати "persist-retain-msg-delete";
		case MOSQ_EVT_PERSIST_CLIENT_ADD:
			врати "persist-client-add";
		case MOSQ_EVT_PERSIST_CLIENT_DELETE:
			врати "persist-client-delete";
		case MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			врати "persist-client-update";
		case MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			врати "persist-subscription-add";
		case MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			врати "persist-subscription-delete";
		case MOSQ_EVT_PERSIST_CLIENT_MSG_ADD:
			врати "persist-client-msg-add";
		case MOSQ_EVT_PERSIST_CLIENT_MSG_DELETE:
			врати "persist-client-msg-delete";
		case MOSQ_EVT_PERSIST_CLIENT_MSG_UPDATE:
			врати "persist-client-msg-update";
		case MOSQ_EVT_PERSIST_WILL_ADD:
			врати "persist-will-add";
		case MOSQ_EVT_PERSIST_WILL_DELETE:
			врати "persist-will-delete";
	}
	врати "";
}


static bool check_callback_exists(struct mosquitto__callback *cb_base, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	struct mosquitto__callback *tail, *tmp;

	DL_FOREACH_SAFE(cb_base, tail, tmp){
		ако(tail->identifier == identifier && tail->cb == cb_func){
			врати true;
		}
	}
	врати false;
}

static struct mosquitto__callback **plugin__get_callback_base(struct mosquitto__security_options *security_options, enum mosquitto_plugin_event event)
{
	switch(event){
		case MOSQ_EVT_RELOAD:
			врати &security_options->plugin_callbacks.reload;
		case MOSQ_EVT_ACL_CHECK:
			врати &security_options->plugin_callbacks.acl_check;
		case MOSQ_EVT_BASIC_AUTH:
			врати &security_options->plugin_callbacks.basic_auth;
		case MOSQ_EVT_PSK_KEY:
			врати &security_options->plugin_callbacks.psk_key;
		case MOSQ_EVT_EXT_AUTH_START:
			врати &security_options->plugin_callbacks.ext_auth_start;
		case MOSQ_EVT_EXT_AUTH_CONTINUE:
			врати &security_options->plugin_callbacks.ext_auth_continue;
		case MOSQ_EVT_CONTROL:
			врати NULL;
		case MOSQ_EVT_MESSAGE_IN: /* same as MOSQ_EVT_MESSAGE */
			врати &security_options->plugin_callbacks.message_in;
		case MOSQ_EVT_TICK:
			врати &security_options->plugin_callbacks.tick;
		case MOSQ_EVT_DISCONNECT:
			врати &security_options->plugin_callbacks.disconnect;
		case MOSQ_EVT_CONNECT:
			врати &security_options->plugin_callbacks.connect;
		case MOSQ_EVT_CLIENT_OFFLINE:
			врати &security_options->plugin_callbacks.client_offline;
		case MOSQ_EVT_SUBSCRIBE:
			врати &security_options->plugin_callbacks.subscribe;
		case MOSQ_EVT_UNSUBSCRIBE:
			врати &security_options->plugin_callbacks.unsubscribe;
		case MOSQ_EVT_PERSIST_RESTORE:
			врати &security_options->plugin_callbacks.persist_restore;
		case MOSQ_EVT_PERSIST_CLIENT_ADD:
			врати &security_options->plugin_callbacks.persist_client_add;
		case MOSQ_EVT_PERSIST_CLIENT_DELETE:
			врати &security_options->plugin_callbacks.persist_client_delete;
		case MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			врати &security_options->plugin_callbacks.persist_client_update;
		case MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			врати &security_options->plugin_callbacks.persist_subscription_add;
		case MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			врати &security_options->plugin_callbacks.persist_subscription_delete;
		case MOSQ_EVT_PERSIST_CLIENT_MSG_ADD:
			врати &security_options->plugin_callbacks.persist_client_msg_add;
		case MOSQ_EVT_PERSIST_CLIENT_MSG_DELETE:
			врати &security_options->plugin_callbacks.persist_client_msg_delete;
		case MOSQ_EVT_PERSIST_CLIENT_MSG_UPDATE:
			врати &security_options->plugin_callbacks.persist_client_msg_update;
		case MOSQ_EVT_PERSIST_BASE_MSG_ADD:
			врати &security_options->plugin_callbacks.persist_base_msg_add;
		case MOSQ_EVT_PERSIST_BASE_MSG_DELETE:
			врати &security_options->plugin_callbacks.persist_base_msg_delete;
		case MOSQ_EVT_PERSIST_RETAIN_MSG_SET:
			врати &security_options->plugin_callbacks.persist_retain_msg_set;
		case MOSQ_EVT_PERSIST_RETAIN_MSG_DELETE:
			врати &security_options->plugin_callbacks.persist_retain_msg_delete;
		case MOSQ_EVT_MESSAGE_OUT:
			врати &security_options->plugin_callbacks.message_out;
		case MOSQ_EVT_PERSIST_WILL_ADD:
			врати &security_options->plugin_callbacks.persist_will_add;
		case MOSQ_EVT_PERSIST_WILL_DELETE:
			врати &security_options->plugin_callbacks.persist_will_delete;
	}
	врати NULL;
}


static цео remove_callback(mosquitto_plugin_id_t *plugin, struct plugin_own_callback *own)
{
	struct mosquitto__security_options *security_options;
	struct mosquitto__callback *tail, *tmp;
	struct mosquitto__callback **cb_base = NULL;

	за(цео i=0; i<plugin->config.security_option_count; i++){
		security_options = plugin->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, own->event);
		ако(cb_base == NULL){
			врати MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_SAFE(*cb_base, tail, tmp){
			ако(tail->identifier == plugin && tail->cb == own->cb_func){
				DL_DELETE(*cb_base, tail);
				mosquitto_FREE(tail);
				break;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	врати MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT цео mosquitto_callback_register(
		mosquitto_plugin_id_t *identifier,
		цео event,
		MOSQ_FUNC_generic_callback cb_func,
		const void *event_data,
		void *userdata)
{
	struct mosquitto__callback **cb_base = NULL, *cb_new;
	struct mosquitto__security_options *security_options;
	struct plugin_own_callback *own_callback;

	ако(cb_func == NULL){
		врати MOSQ_ERR_INVAL;
	}

	ако(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
			|| event == MOSQ_EVT_PERSIST_BASE_MSG_ADD
			|| event == MOSQ_EVT_PERSIST_BASE_MSG_DELETE
			|| event == MOSQ_EVT_PERSIST_RETAIN_MSG_SET
			|| event == MOSQ_EVT_PERSIST_RETAIN_MSG_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_UPDATE
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_MSG_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_MSG_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_MSG_UPDATE
			|| event == MOSQ_EVT_PERSIST_WILL_ADD
			|| event == MOSQ_EVT_PERSIST_WILL_DELETE
			)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		врати MOSQ_ERR_INVAL;
	}

	ако(event == MOSQ_EVT_CONTROL){
		врати control__register_callback(identifier, cb_func, event_data, userdata);
	}

	own_callback = mosquitto_calloc(1, sizeof(struct plugin_own_callback));
	ако(own_callback == NULL){
		врати MOSQ_ERR_NOMEM;
	}
	own_callback->event = (enum mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	ако(identifier->config.security_option_count == 0){
		log__printf(NULL, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((enum mosquitto_plugin_event)event));
		врати MOSQ_ERR_INVAL;
	}

	за(цео i=0; i<identifier->config.security_option_count; i++){
		security_options = identifier->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, (enum mosquitto_plugin_event)event);
		ако(cb_base == NULL){
			врати MOSQ_ERR_NOT_SUPPORTED;
		}

		ако(check_callback_exists(*cb_base, identifier, cb_func)){
			врати MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, sizeof(struct mosquitto__callback));
		ако(cb_new == NULL){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			врати MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_base, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	ако(identifier->plugin_name){
		log__printf(NULL, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((enum mosquitto_plugin_event)event));
	}инако{
		log__printf(NULL, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((enum mosquitto_plugin_event)event));
	}

	врати MOSQ_ERR_SUCCESS;
}


цео plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	struct plugin_own_callback *own, *own_tmp;

	ако(plugin == NULL){
		врати MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_SAFE(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	врати MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT цео mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		цео event,
		MOSQ_FUNC_generic_callback cb_func,
		const void *event_data)
{
	struct plugin_own_callback *own, *own_tmp;

	ако(identifier == NULL || cb_func == NULL){
		врати MOSQ_ERR_INVAL;
	}

	ако(event == MOSQ_EVT_CONTROL){
		врати control__unregister_callback(identifier, cb_func, event_data);
	}

	DL_FOREACH_SAFE(identifier->own_callbacks, own, own_tmp){
		ако(own->event == (enum mosquitto_plugin_event)event && own->cb_func == cb_func){
			врати remove_callback(identifier, own);
		}
	}

	врати MOSQ_ERR_NOT_FOUND;
}
