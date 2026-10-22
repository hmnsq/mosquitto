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
			вернуть "reload";
		case MOSQ_EVT_ACL_CHECK:
			вернуть "acl-check";
		case MOSQ_EVT_BASIC_AUTH:
			вернуть "basic-auth";
		case MOSQ_EVT_PSK_KEY:
			вернуть "psk-key";
		case MOSQ_EVT_EXT_AUTH_START:
			вернуть "auth-start";
		case MOSQ_EVT_EXT_AUTH_CONTINUE:
			вернуть "auth-continue";
		case MOSQ_EVT_CONTROL:
			вернуть "control";
		case MOSQ_EVT_MESSAGE_IN:
			вернуть "message-in";
		case MOSQ_EVT_MESSAGE_OUT:
			вернуть "message-out";
		case MOSQ_EVT_TICK:
			вернуть "tick";
		case MOSQ_EVT_DISCONNECT:
			вернуть "disconnect";
		case MOSQ_EVT_CONNECT:
			вернуть "connect";
		case MOSQ_EVT_CLIENT_OFFLINE:
			вернуть "connect";
		case MOSQ_EVT_SUBSCRIBE:
			вернуть "subscribe";
		case MOSQ_EVT_UNSUBSCRIBE:
			вернуть "unsubscribe";
		case MOSQ_EVT_PERSIST_RESTORE:
			вернуть "persist-restore";
		case MOSQ_EVT_PERSIST_BASE_MSG_ADD:
			вернуть "persist-base-msg-add";
		case MOSQ_EVT_PERSIST_BASE_MSG_DELETE:
			вернуть "persist-base-msg-delete";
		case MOSQ_EVT_PERSIST_RETAIN_MSG_SET:
			вернуть "persist-retain-msg-set";
		case MOSQ_EVT_PERSIST_RETAIN_MSG_DELETE:
			вернуть "persist-retain-msg-delete";
		case MOSQ_EVT_PERSIST_CLIENT_ADD:
			вернуть "persist-client-add";
		case MOSQ_EVT_PERSIST_CLIENT_DELETE:
			вернуть "persist-client-delete";
		case MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			вернуть "persist-client-update";
		case MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			вернуть "persist-subscription-add";
		case MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			вернуть "persist-subscription-delete";
		case MOSQ_EVT_PERSIST_CLIENT_MSG_ADD:
			вернуть "persist-client-msg-add";
		case MOSQ_EVT_PERSIST_CLIENT_MSG_DELETE:
			вернуть "persist-client-msg-delete";
		case MOSQ_EVT_PERSIST_CLIENT_MSG_UPDATE:
			вернуть "persist-client-msg-update";
		case MOSQ_EVT_PERSIST_WILL_ADD:
			вернуть "persist-will-add";
		case MOSQ_EVT_PERSIST_WILL_DELETE:
			вернуть "persist-will-delete";
	}
	вернуть "";
}


static bool check_callback_exists(struct mosquitto__callback *cb_base, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	struct mosquitto__callback *tail, *tmp;

	DL_FOREACH_SAFE(cb_base, tail, tmp){
		если(tail->identifier == identifier && tail->cb == cb_func){
			вернуть true;
		}
	}
	вернуть false;
}

static struct mosquitto__callback **plugin__get_callback_base(struct mosquitto__security_options *security_options, enum mosquitto_plugin_event event)
{
	switch(event){
		case MOSQ_EVT_RELOAD:
			вернуть &security_options->plugin_callbacks.reload;
		case MOSQ_EVT_ACL_CHECK:
			вернуть &security_options->plugin_callbacks.acl_check;
		case MOSQ_EVT_BASIC_AUTH:
			вернуть &security_options->plugin_callbacks.basic_auth;
		case MOSQ_EVT_PSK_KEY:
			вернуть &security_options->plugin_callbacks.psk_key;
		case MOSQ_EVT_EXT_AUTH_START:
			вернуть &security_options->plugin_callbacks.ext_auth_start;
		case MOSQ_EVT_EXT_AUTH_CONTINUE:
			вернуть &security_options->plugin_callbacks.ext_auth_continue;
		case MOSQ_EVT_CONTROL:
			вернуть NULL;
		case MOSQ_EVT_MESSAGE_IN: /* same as MOSQ_EVT_MESSAGE */
			вернуть &security_options->plugin_callbacks.message_in;
		case MOSQ_EVT_TICK:
			вернуть &security_options->plugin_callbacks.tick;
		case MOSQ_EVT_DISCONNECT:
			вернуть &security_options->plugin_callbacks.disconnect;
		case MOSQ_EVT_CONNECT:
			вернуть &security_options->plugin_callbacks.connect;
		case MOSQ_EVT_CLIENT_OFFLINE:
			вернуть &security_options->plugin_callbacks.client_offline;
		case MOSQ_EVT_SUBSCRIBE:
			вернуть &security_options->plugin_callbacks.subscribe;
		case MOSQ_EVT_UNSUBSCRIBE:
			вернуть &security_options->plugin_callbacks.unsubscribe;
		case MOSQ_EVT_PERSIST_RESTORE:
			вернуть &security_options->plugin_callbacks.persist_restore;
		case MOSQ_EVT_PERSIST_CLIENT_ADD:
			вернуть &security_options->plugin_callbacks.persist_client_add;
		case MOSQ_EVT_PERSIST_CLIENT_DELETE:
			вернуть &security_options->plugin_callbacks.persist_client_delete;
		case MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			вернуть &security_options->plugin_callbacks.persist_client_update;
		case MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			вернуть &security_options->plugin_callbacks.persist_subscription_add;
		case MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			вернуть &security_options->plugin_callbacks.persist_subscription_delete;
		case MOSQ_EVT_PERSIST_CLIENT_MSG_ADD:
			вернуть &security_options->plugin_callbacks.persist_client_msg_add;
		case MOSQ_EVT_PERSIST_CLIENT_MSG_DELETE:
			вернуть &security_options->plugin_callbacks.persist_client_msg_delete;
		case MOSQ_EVT_PERSIST_CLIENT_MSG_UPDATE:
			вернуть &security_options->plugin_callbacks.persist_client_msg_update;
		case MOSQ_EVT_PERSIST_BASE_MSG_ADD:
			вернуть &security_options->plugin_callbacks.persist_base_msg_add;
		case MOSQ_EVT_PERSIST_BASE_MSG_DELETE:
			вернуть &security_options->plugin_callbacks.persist_base_msg_delete;
		case MOSQ_EVT_PERSIST_RETAIN_MSG_SET:
			вернуть &security_options->plugin_callbacks.persist_retain_msg_set;
		case MOSQ_EVT_PERSIST_RETAIN_MSG_DELETE:
			вернуть &security_options->plugin_callbacks.persist_retain_msg_delete;
		case MOSQ_EVT_MESSAGE_OUT:
			вернуть &security_options->plugin_callbacks.message_out;
		case MOSQ_EVT_PERSIST_WILL_ADD:
			вернуть &security_options->plugin_callbacks.persist_will_add;
		case MOSQ_EVT_PERSIST_WILL_DELETE:
			вернуть &security_options->plugin_callbacks.persist_will_delete;
	}
	вернуть NULL;
}


static целое remove_callback(mosquitto_plugin_id_t *plugin, struct plugin_own_callback *own)
{
	struct mosquitto__security_options *security_options;
	struct mosquitto__callback *tail, *tmp;
	struct mosquitto__callback **cb_base = NULL;

	для(целое i=0; i<plugin->config.security_option_count; i++){
		security_options = plugin->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, own->event);
		если(cb_base == NULL){
			вернуть MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_SAFE(*cb_base, tail, tmp){
			если(tail->identifier == plugin && tail->cb == own->cb_func){
				DL_DELETE(*cb_base, tail);
				mosquitto_FREE(tail);
				break;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	вернуть MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT целое mosquitto_callback_register(
		mosquitto_plugin_id_t *identifier,
		целое event,
		MOSQ_FUNC_generic_callback cb_func,
		const void *event_data,
		void *userdata)
{
	struct mosquitto__callback **cb_base = NULL, *cb_new;
	struct mosquitto__security_options *security_options;
	struct plugin_own_callback *own_callback;

	если(cb_func == NULL){
		вернуть MOSQ_ERR_INVAL;
	}

	если(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
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
		вернуть MOSQ_ERR_INVAL;
	}

	если(event == MOSQ_EVT_CONTROL){
		вернуть control__register_callback(identifier, cb_func, event_data, userdata);
	}

	own_callback = mosquitto_calloc(1, sizeof(struct plugin_own_callback));
	если(own_callback == NULL){
		вернуть MOSQ_ERR_NOMEM;
	}
	own_callback->event = (enum mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	если(identifier->config.security_option_count == 0){
		log__printf(NULL, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((enum mosquitto_plugin_event)event));
		вернуть MOSQ_ERR_INVAL;
	}

	для(целое i=0; i<identifier->config.security_option_count; i++){
		security_options = identifier->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, (enum mosquitto_plugin_event)event);
		если(cb_base == NULL){
			вернуть MOSQ_ERR_NOT_SUPPORTED;
		}

		если(check_callback_exists(*cb_base, identifier, cb_func)){
			вернуть MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, sizeof(struct mosquitto__callback));
		если(cb_new == NULL){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			вернуть MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_base, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	если(identifier->plugin_name){
		log__printf(NULL, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((enum mosquitto_plugin_event)event));
	}иначе{
		log__printf(NULL, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((enum mosquitto_plugin_event)event));
	}

	вернуть MOSQ_ERR_SUCCESS;
}


целое plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	struct plugin_own_callback *own, *own_tmp;

	если(plugin == NULL){
		вернуть MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_SAFE(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	вернуть MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT целое mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		целое event,
		MOSQ_FUNC_generic_callback cb_func,
		const void *event_data)
{
	struct plugin_own_callback *own, *own_tmp;

	если(identifier == NULL || cb_func == NULL){
		вернуть MOSQ_ERR_INVAL;
	}

	если(event == MOSQ_EVT_CONTROL){
		вернуть control__unregister_callback(identifier, cb_func, event_data);
	}

	DL_FOREACH_SAFE(identifier->own_callbacks, own, own_tmp){
		если(own->event == (enum mosquitto_plugin_event)event && own->cb_func == cb_func){
			вернуть remove_callback(identifier, own);
		}
	}

	вернуть MOSQ_ERR_NOT_FOUND;
}
