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
			반환 "reload";
		case MOSQ_EVT_ACL_CHECK:
			반환 "acl-check";
		case MOSQ_EVT_BASIC_AUTH:
			반환 "basic-auth";
		case MOSQ_EVT_PSK_KEY:
			반환 "psk-key";
		case MOSQ_EVT_EXT_AUTH_START:
			반환 "auth-start";
		case MOSQ_EVT_EXT_AUTH_CONTINUE:
			반환 "auth-continue";
		case MOSQ_EVT_CONTROL:
			반환 "control";
		case MOSQ_EVT_MESSAGE_IN:
			반환 "message-in";
		case MOSQ_EVT_MESSAGE_OUT:
			반환 "message-out";
		case MOSQ_EVT_TICK:
			반환 "tick";
		case MOSQ_EVT_DISCONNECT:
			반환 "disconnect";
		case MOSQ_EVT_CONNECT:
			반환 "connect";
		case MOSQ_EVT_CLIENT_OFFLINE:
			반환 "connect";
		case MOSQ_EVT_SUBSCRIBE:
			반환 "subscribe";
		case MOSQ_EVT_UNSUBSCRIBE:
			반환 "unsubscribe";
		case MOSQ_EVT_PERSIST_RESTORE:
			반환 "persist-restore";
		case MOSQ_EVT_PERSIST_BASE_MSG_ADD:
			반환 "persist-base-msg-add";
		case MOSQ_EVT_PERSIST_BASE_MSG_DELETE:
			반환 "persist-base-msg-delete";
		case MOSQ_EVT_PERSIST_RETAIN_MSG_SET:
			반환 "persist-retain-msg-set";
		case MOSQ_EVT_PERSIST_RETAIN_MSG_DELETE:
			반환 "persist-retain-msg-delete";
		case MOSQ_EVT_PERSIST_CLIENT_ADD:
			반환 "persist-client-add";
		case MOSQ_EVT_PERSIST_CLIENT_DELETE:
			반환 "persist-client-delete";
		case MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			반환 "persist-client-update";
		case MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			반환 "persist-subscription-add";
		case MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			반환 "persist-subscription-delete";
		case MOSQ_EVT_PERSIST_CLIENT_MSG_ADD:
			반환 "persist-client-msg-add";
		case MOSQ_EVT_PERSIST_CLIENT_MSG_DELETE:
			반환 "persist-client-msg-delete";
		case MOSQ_EVT_PERSIST_CLIENT_MSG_UPDATE:
			반환 "persist-client-msg-update";
		case MOSQ_EVT_PERSIST_WILL_ADD:
			반환 "persist-will-add";
		case MOSQ_EVT_PERSIST_WILL_DELETE:
			반환 "persist-will-delete";
	}
	반환 "";
}


static bool check_callback_exists(struct mosquitto__callback *cb_base, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	struct mosquitto__callback *tail, *tmp;

	DL_FOREACH_SAFE(cb_base, tail, tmp){
		만약(tail->identifier == identifier && tail->cb == cb_func){
			반환 true;
		}
	}
	반환 false;
}

static struct mosquitto__callback **plugin__get_callback_base(struct mosquitto__security_options *security_options, enum mosquitto_plugin_event event)
{
	switch(event){
		case MOSQ_EVT_RELOAD:
			반환 &security_options->plugin_callbacks.reload;
		case MOSQ_EVT_ACL_CHECK:
			반환 &security_options->plugin_callbacks.acl_check;
		case MOSQ_EVT_BASIC_AUTH:
			반환 &security_options->plugin_callbacks.basic_auth;
		case MOSQ_EVT_PSK_KEY:
			반환 &security_options->plugin_callbacks.psk_key;
		case MOSQ_EVT_EXT_AUTH_START:
			반환 &security_options->plugin_callbacks.ext_auth_start;
		case MOSQ_EVT_EXT_AUTH_CONTINUE:
			반환 &security_options->plugin_callbacks.ext_auth_continue;
		case MOSQ_EVT_CONTROL:
			반환 NULL;
		case MOSQ_EVT_MESSAGE_IN: /* same as MOSQ_EVT_MESSAGE */
			반환 &security_options->plugin_callbacks.message_in;
		case MOSQ_EVT_TICK:
			반환 &security_options->plugin_callbacks.tick;
		case MOSQ_EVT_DISCONNECT:
			반환 &security_options->plugin_callbacks.disconnect;
		case MOSQ_EVT_CONNECT:
			반환 &security_options->plugin_callbacks.connect;
		case MOSQ_EVT_CLIENT_OFFLINE:
			반환 &security_options->plugin_callbacks.client_offline;
		case MOSQ_EVT_SUBSCRIBE:
			반환 &security_options->plugin_callbacks.subscribe;
		case MOSQ_EVT_UNSUBSCRIBE:
			반환 &security_options->plugin_callbacks.unsubscribe;
		case MOSQ_EVT_PERSIST_RESTORE:
			반환 &security_options->plugin_callbacks.persist_restore;
		case MOSQ_EVT_PERSIST_CLIENT_ADD:
			반환 &security_options->plugin_callbacks.persist_client_add;
		case MOSQ_EVT_PERSIST_CLIENT_DELETE:
			반환 &security_options->plugin_callbacks.persist_client_delete;
		case MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			반환 &security_options->plugin_callbacks.persist_client_update;
		case MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			반환 &security_options->plugin_callbacks.persist_subscription_add;
		case MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			반환 &security_options->plugin_callbacks.persist_subscription_delete;
		case MOSQ_EVT_PERSIST_CLIENT_MSG_ADD:
			반환 &security_options->plugin_callbacks.persist_client_msg_add;
		case MOSQ_EVT_PERSIST_CLIENT_MSG_DELETE:
			반환 &security_options->plugin_callbacks.persist_client_msg_delete;
		case MOSQ_EVT_PERSIST_CLIENT_MSG_UPDATE:
			반환 &security_options->plugin_callbacks.persist_client_msg_update;
		case MOSQ_EVT_PERSIST_BASE_MSG_ADD:
			반환 &security_options->plugin_callbacks.persist_base_msg_add;
		case MOSQ_EVT_PERSIST_BASE_MSG_DELETE:
			반환 &security_options->plugin_callbacks.persist_base_msg_delete;
		case MOSQ_EVT_PERSIST_RETAIN_MSG_SET:
			반환 &security_options->plugin_callbacks.persist_retain_msg_set;
		case MOSQ_EVT_PERSIST_RETAIN_MSG_DELETE:
			반환 &security_options->plugin_callbacks.persist_retain_msg_delete;
		case MOSQ_EVT_MESSAGE_OUT:
			반환 &security_options->plugin_callbacks.message_out;
		case MOSQ_EVT_PERSIST_WILL_ADD:
			반환 &security_options->plugin_callbacks.persist_will_add;
		case MOSQ_EVT_PERSIST_WILL_DELETE:
			반환 &security_options->plugin_callbacks.persist_will_delete;
	}
	반환 NULL;
}


static 정수 remove_callback(mosquitto_plugin_id_t *plugin, struct plugin_own_callback *own)
{
	struct mosquitto__security_options *security_options;
	struct mosquitto__callback *tail, *tmp;
	struct mosquitto__callback **cb_base = NULL;

	위해(정수 i=0; i<plugin->config.security_option_count; i++){
		security_options = plugin->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, own->event);
		만약(cb_base == NULL){
			반환 MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_SAFE(*cb_base, tail, tmp){
			만약(tail->identifier == plugin && tail->cb == own->cb_func){
				DL_DELETE(*cb_base, tail);
				mosquitto_FREE(tail);
				break;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	반환 MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT 정수 mosquitto_callback_register(
		mosquitto_plugin_id_t *identifier,
		정수 event,
		MOSQ_FUNC_generic_callback cb_func,
		const void *event_data,
		void *userdata)
{
	struct mosquitto__callback **cb_base = NULL, *cb_new;
	struct mosquitto__security_options *security_options;
	struct plugin_own_callback *own_callback;

	만약(cb_func == NULL){
		반환 MOSQ_ERR_INVAL;
	}

	만약(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
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
		반환 MOSQ_ERR_INVAL;
	}

	만약(event == MOSQ_EVT_CONTROL){
		반환 control__register_callback(identifier, cb_func, event_data, userdata);
	}

	own_callback = mosquitto_calloc(1, sizeof(struct plugin_own_callback));
	만약(own_callback == NULL){
		반환 MOSQ_ERR_NOMEM;
	}
	own_callback->event = (enum mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	만약(identifier->config.security_option_count == 0){
		log__printf(NULL, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((enum mosquitto_plugin_event)event));
		반환 MOSQ_ERR_INVAL;
	}

	위해(정수 i=0; i<identifier->config.security_option_count; i++){
		security_options = identifier->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, (enum mosquitto_plugin_event)event);
		만약(cb_base == NULL){
			반환 MOSQ_ERR_NOT_SUPPORTED;
		}

		만약(check_callback_exists(*cb_base, identifier, cb_func)){
			반환 MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, sizeof(struct mosquitto__callback));
		만약(cb_new == NULL){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			반환 MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_base, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	만약(identifier->plugin_name){
		log__printf(NULL, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((enum mosquitto_plugin_event)event));
	}아니면{
		log__printf(NULL, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((enum mosquitto_plugin_event)event));
	}

	반환 MOSQ_ERR_SUCCESS;
}


정수 plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	struct plugin_own_callback *own, *own_tmp;

	만약(plugin == NULL){
		반환 MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_SAFE(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	반환 MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT 정수 mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		정수 event,
		MOSQ_FUNC_generic_callback cb_func,
		const void *event_data)
{
	struct plugin_own_callback *own, *own_tmp;

	만약(identifier == NULL || cb_func == NULL){
		반환 MOSQ_ERR_INVAL;
	}

	만약(event == MOSQ_EVT_CONTROL){
		반환 control__unregister_callback(identifier, cb_func, event_data);
	}

	DL_FOREACH_SAFE(identifier->own_callbacks, own, own_tmp){
		만약(own->event == (enum mosquitto_plugin_event)event && own->cb_func == cb_func){
			반환 remove_callback(identifier, own);
		}
	}

	반환 MOSQ_ERR_NOT_FOUND;
}
