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


static const var_na *get_event_name(ga_nanaa mosquitto_plugin_event event)
{
	switch(event){
		pak_sa MOSQ_EVT_RELOAD:
			ສົ່ງຄືນ "reload";
		pak_sa MOSQ_EVT_ACL_CHECK:
			ສົ່ງຄືນ "acl-check";
		pak_sa MOSQ_EVT_BASIC_AUTH:
			ສົ່ງຄືນ "basic-auth";
		pak_sa MOSQ_EVT_PSK_KEY:
			ສົ່ງຄືນ "psk-key";
		pak_sa MOSQ_EVT_EXT_AUTH_START:
			ສົ່ງຄືນ "auth-start";
		pak_sa MOSQ_EVT_EXT_AUTH_CONTINUE:
			ສົ່ງຄືນ "auth-continue";
		pak_sa MOSQ_EVT_CONTROL:
			ສົ່ງຄືນ "control";
		pak_sa MOSQ_EVT_MESSAGE_IN:
			ສົ່ງຄືນ "message-in";
		pak_sa MOSQ_EVT_MESSAGE_OUT:
			ສົ່ງຄືນ "message-out";
		pak_sa MOSQ_EVT_TICK:
			ສົ່ງຄືນ "tick";
		pak_sa MOSQ_EVT_DISCONNECT:
			ສົ່ງຄືນ "disconnect";
		pak_sa MOSQ_EVT_CONNECT:
			ສົ່ງຄືນ "connect";
		pak_sa MOSQ_EVT_CLIENT_OFFLINE:
			ສົ່ງຄືນ "connect";
		pak_sa MOSQ_EVT_SUBSCRIBE:
			ສົ່ງຄືນ "subscribe";
		pak_sa MOSQ_EVT_UNSUBSCRIBE:
			ສົ່ງຄືນ "unsubscribe";
		pak_sa MOSQ_EVT_PERSIST_RESTORE:
			ສົ່ງຄືນ "persist-restore";
		pak_sa MOSQ_EVT_PERSIST_BASE_MSG_ADD:
			ສົ່ງຄືນ "persist-base-msg-add";
		pak_sa MOSQ_EVT_PERSIST_BASE_MSG_DELETE:
			ສົ່ງຄືນ "persist-base-msg-delete";
		pak_sa MOSQ_EVT_PERSIST_RETAIN_MSG_SET:
			ສົ່ງຄືນ "persist-retain-msg-set";
		pak_sa MOSQ_EVT_PERSIST_RETAIN_MSG_DELETE:
			ສົ່ງຄືນ "persist-retain-msg-delete";
		pak_sa MOSQ_EVT_PERSIST_CLIENT_ADD:
			ສົ່ງຄືນ "persist-client-add";
		pak_sa MOSQ_EVT_PERSIST_CLIENT_DELETE:
			ສົ່ງຄືນ "persist-client-delete";
		pak_sa MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			ສົ່ງຄືນ "persist-client-update";
		pak_sa MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			ສົ່ງຄືນ "persist-subscription-add";
		pak_sa MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			ສົ່ງຄືນ "persist-subscription-delete";
		pak_sa MOSQ_EVT_PERSIST_CLIENT_MSG_ADD:
			ສົ່ງຄືນ "persist-client-msg-add";
		pak_sa MOSQ_EVT_PERSIST_CLIENT_MSG_DELETE:
			ສົ່ງຄືນ "persist-client-msg-delete";
		pak_sa MOSQ_EVT_PERSIST_CLIENT_MSG_UPDATE:
			ສົ່ງຄືນ "persist-client-msg-update";
		pak_sa MOSQ_EVT_PERSIST_WILL_ADD:
			ສົ່ງຄືນ "persist-will-add";
		pak_sa MOSQ_EVT_PERSIST_WILL_DELETE:
			ສົ່ງຄືນ "persist-will-delete";
	}
	ສົ່ງຄືນ "";
}


static bool check_callback_exists(sa_nracanaa mosquitto__callback *cb_base, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	sa_nracanaa mosquitto__callback *tail, *tmp;

	DL_FOREACH_SAFE(cb_base, tail, tmp){
		ຖ້າ(tail->identifier == identifier && tail->cb == cb_func){
			ສົ່ງຄືນ true;
		}
	}
	ສົ່ງຄືນ false;
}

static sa_nracanaa mosquitto__callback **plugin__get_callback_base(sa_nracanaa mosquitto__security_options *security_options, ga_nanaa mosquitto_plugin_event event)
{
	switch(event){
		pak_sa MOSQ_EVT_RELOAD:
			ສົ່ງຄືນ &security_options->plugin_callbacks.reload;
		pak_sa MOSQ_EVT_ACL_CHECK:
			ສົ່ງຄືນ &security_options->plugin_callbacks.acl_check;
		pak_sa MOSQ_EVT_BASIC_AUTH:
			ສົ່ງຄືນ &security_options->plugin_callbacks.basic_auth;
		pak_sa MOSQ_EVT_PSK_KEY:
			ສົ່ງຄືນ &security_options->plugin_callbacks.psk_key;
		pak_sa MOSQ_EVT_EXT_AUTH_START:
			ສົ່ງຄືນ &security_options->plugin_callbacks.ext_auth_start;
		pak_sa MOSQ_EVT_EXT_AUTH_CONTINUE:
			ສົ່ງຄືນ &security_options->plugin_callbacks.ext_auth_continue;
		pak_sa MOSQ_EVT_CONTROL:
			ສົ່ງຄືນ NULL;
		pak_sa MOSQ_EVT_MESSAGE_IN: /* same as MOSQ_EVT_MESSAGE */
			ສົ່ງຄືນ &security_options->plugin_callbacks.message_in;
		pak_sa MOSQ_EVT_TICK:
			ສົ່ງຄືນ &security_options->plugin_callbacks.tick;
		pak_sa MOSQ_EVT_DISCONNECT:
			ສົ່ງຄືນ &security_options->plugin_callbacks.disconnect;
		pak_sa MOSQ_EVT_CONNECT:
			ສົ່ງຄືນ &security_options->plugin_callbacks.connect;
		pak_sa MOSQ_EVT_CLIENT_OFFLINE:
			ສົ່ງຄືນ &security_options->plugin_callbacks.client_offline;
		pak_sa MOSQ_EVT_SUBSCRIBE:
			ສົ່ງຄືນ &security_options->plugin_callbacks.subscribe;
		pak_sa MOSQ_EVT_UNSUBSCRIBE:
			ສົ່ງຄືນ &security_options->plugin_callbacks.unsubscribe;
		pak_sa MOSQ_EVT_PERSIST_RESTORE:
			ສົ່ງຄືນ &security_options->plugin_callbacks.persist_restore;
		pak_sa MOSQ_EVT_PERSIST_CLIENT_ADD:
			ສົ່ງຄືນ &security_options->plugin_callbacks.persist_client_add;
		pak_sa MOSQ_EVT_PERSIST_CLIENT_DELETE:
			ສົ່ງຄືນ &security_options->plugin_callbacks.persist_client_delete;
		pak_sa MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			ສົ່ງຄືນ &security_options->plugin_callbacks.persist_client_update;
		pak_sa MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			ສົ່ງຄືນ &security_options->plugin_callbacks.persist_subscription_add;
		pak_sa MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			ສົ່ງຄືນ &security_options->plugin_callbacks.persist_subscription_delete;
		pak_sa MOSQ_EVT_PERSIST_CLIENT_MSG_ADD:
			ສົ່ງຄືນ &security_options->plugin_callbacks.persist_client_msg_add;
		pak_sa MOSQ_EVT_PERSIST_CLIENT_MSG_DELETE:
			ສົ່ງຄືນ &security_options->plugin_callbacks.persist_client_msg_delete;
		pak_sa MOSQ_EVT_PERSIST_CLIENT_MSG_UPDATE:
			ສົ່ງຄືນ &security_options->plugin_callbacks.persist_client_msg_update;
		pak_sa MOSQ_EVT_PERSIST_BASE_MSG_ADD:
			ສົ່ງຄືນ &security_options->plugin_callbacks.persist_base_msg_add;
		pak_sa MOSQ_EVT_PERSIST_BASE_MSG_DELETE:
			ສົ່ງຄືນ &security_options->plugin_callbacks.persist_base_msg_delete;
		pak_sa MOSQ_EVT_PERSIST_RETAIN_MSG_SET:
			ສົ່ງຄືນ &security_options->plugin_callbacks.persist_retain_msg_set;
		pak_sa MOSQ_EVT_PERSIST_RETAIN_MSG_DELETE:
			ສົ່ງຄືນ &security_options->plugin_callbacks.persist_retain_msg_delete;
		pak_sa MOSQ_EVT_MESSAGE_OUT:
			ສົ່ງຄືນ &security_options->plugin_callbacks.message_out;
		pak_sa MOSQ_EVT_PERSIST_WILL_ADD:
			ສົ່ງຄືນ &security_options->plugin_callbacks.persist_will_add;
		pak_sa MOSQ_EVT_PERSIST_WILL_DELETE:
			ສົ່ງຄືນ &security_options->plugin_callbacks.persist_will_delete;
	}
	ສົ່ງຄືນ NULL;
}


static ຈຳນວນເຕັມ remove_callback(mosquitto_plugin_id_t *plugin, sa_nracanaa plugin_own_callback *own)
{
	sa_nracanaa mosquitto__security_options *security_options;
	sa_nracanaa mosquitto__callback *tail, *tmp;
	sa_nracanaa mosquitto__callback **cb_base = NULL;

	ສຳລັບ(ຈຳນວນເຕັມ i=0; i<plugin->config.security_option_count; i++){
		security_options = plugin->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, own->event);
		ຖ້າ(cb_base == NULL){
			ສົ່ງຄືນ MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_SAFE(*cb_base, tail, tmp){
			ຖ້າ(tail->identifier == plugin && tail->cb == own->cb_func){
				DL_DELETE(*cb_base, tail);
				mosquitto_FREE(tail);
				bha_nga;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_callback_register(
		mosquitto_plugin_id_t *identifier,
		ຈຳນວນເຕັມ event,
		MOSQ_FUNC_generic_callback cb_func,
		const void *event_data,
		void *userdata)
{
	sa_nracanaa mosquitto__callback **cb_base = NULL, *cb_new;
	sa_nracanaa mosquitto__security_options *security_options;
	sa_nracanaa plugin_own_callback *own_callback;

	ຖ້າ(cb_func == NULL){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	ຖ້າ(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
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
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	ຖ້າ(event == MOSQ_EVT_CONTROL){
		ສົ່ງຄືນ control__register_callback(identifier, cb_func, event_data, userdata);
	}

	own_callback = mosquitto_calloc(1, parimaa_na(sa_nracanaa plugin_own_callback));
	ຖ້າ(own_callback == NULL){
		ສົ່ງຄືນ MOSQ_ERR_NOMEM;
	}
	own_callback->event = (ga_nanaa mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	ຖ້າ(identifier->config.security_option_count == 0){
		log__printf(NULL, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((ga_nanaa mosquitto_plugin_event)event));
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	ສຳລັບ(ຈຳນວນເຕັມ i=0; i<identifier->config.security_option_count; i++){
		security_options = identifier->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, (ga_nanaa mosquitto_plugin_event)event);
		ຖ້າ(cb_base == NULL){
			ສົ່ງຄືນ MOSQ_ERR_NOT_SUPPORTED;
		}

		ຖ້າ(check_callback_exists(*cb_base, identifier, cb_func)){
			ສົ່ງຄືນ MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__callback));
		ຖ້າ(cb_new == NULL){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			ສົ່ງຄືນ MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_base, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	ຖ້າ(identifier->plugin_name){
		log__printf(NULL, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((ga_nanaa mosquitto_plugin_event)event));
	}ບໍ່ດັ່ງນັ້ນ{
		log__printf(NULL, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((ga_nanaa mosquitto_plugin_event)event));
	}

	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


ຈຳນວນເຕັມ plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	sa_nracanaa plugin_own_callback *own, *own_tmp;

	ຖ້າ(plugin == NULL){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_SAFE(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ຈຳນວນເຕັມ mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		ຈຳນວນເຕັມ event,
		MOSQ_FUNC_generic_callback cb_func,
		const void *event_data)
{
	sa_nracanaa plugin_own_callback *own, *own_tmp;

	ຖ້າ(identifier == NULL || cb_func == NULL){
		ສົ່ງຄືນ MOSQ_ERR_INVAL;
	}

	ຖ້າ(event == MOSQ_EVT_CONTROL){
		ສົ່ງຄືນ control__unregister_callback(identifier, cb_func, event_data);
	}

	DL_FOREACH_SAFE(identifier->own_callbacks, own, own_tmp){
		ຖ້າ(own->event == (ga_nanaa mosquitto_plugin_event)event && own->cb_func == cb_func){
			ສົ່ງຄືນ remove_callback(identifier, own);
		}
	}

	ສົ່ງຄືນ MOSQ_ERR_NOT_FOUND;
}
