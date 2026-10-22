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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "utlist.h"
#അന്തര്ഭാവ "lib_load.h"


സ്ഥിതം സ്ഥിരം അക്ഷരം *get_event_name(എണ്ണൽ mosquitto_plugin_event event)
{
	തിരഞ്ഞെടുപ്പ്(event){
		അവസ്ഥ MOSQ_EVT_RELOAD:
			തിരികെ "reload";
		അവസ്ഥ MOSQ_EVT_ACL_CHECK:
			തിരികെ "acl-check";
		അവസ്ഥ MOSQ_EVT_BASIC_AUTH:
			തിരികെ "basic-auth";
		അവസ്ഥ MOSQ_EVT_PSK_താക്കോൽ:
			തിരികെ "psk-key";
		അവസ്ഥ MOSQ_EVT_EXT_AUTH_START:
			തിരികെ "auth-start";
		അവസ്ഥ MOSQ_EVT_EXT_AUTH_CONTINUE:
			തിരികെ "auth-continue";
		അവസ്ഥ MOSQ_EVT_CONTROL:
			തിരികെ "control";
		അവസ്ഥ MOSQ_EVT_MESSAGE_IN:
			തിരികെ "message-in";
		അവസ്ഥ MOSQ_EVT_MESSAGE_OUT:
			തിരികെ "message-out";
		അവസ്ഥ MOSQ_EVT_TICK:
			തിരികെ "tick";
		അവസ്ഥ MOSQ_EVT_DISCONNECT:
			തിരികെ "disconnect";
		അവസ്ഥ MOSQ_EVT_CONNECT:
			തിരികെ "connect";
		അവസ്ഥ MOSQ_EVT_CLIENT_OFFLINE:
			തിരികെ "connect";
		അവസ്ഥ MOSQ_EVT_SUBSCRIBE:
			തിരികെ "subscribe";
		അവസ്ഥ MOSQ_EVT_UNSUBSCRIBE:
			തിരികെ "unsubscribe";
		അവസ്ഥ MOSQ_EVT_PERSIST_RESTORE:
			തിരികെ "persist-restore";
		അവസ്ഥ MOSQ_EVT_PERSIST_BASE_സന്ദേശം_ADD:
			തിരികെ "persist-base-msg-add";
		അവസ്ഥ MOSQ_EVT_PERSIST_BASE_സന്ദേശം_DELETE:
			തിരികെ "persist-base-msg-delete";
		അവസ്ഥ MOSQ_EVT_PERSIST_RETAIN_സന്ദേശം_SET:
			തിരികെ "persist-retain-msg-set";
		അവസ്ഥ MOSQ_EVT_PERSIST_RETAIN_സന്ദേശം_DELETE:
			തിരികെ "persist-retain-msg-delete";
		അവസ്ഥ MOSQ_EVT_PERSIST_CLIENT_ADD:
			തിരികെ "persist-client-add";
		അവസ്ഥ MOSQ_EVT_PERSIST_CLIENT_DELETE:
			തിരികെ "persist-client-delete";
		അവസ്ഥ MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			തിരികെ "persist-client-update";
		അവസ്ഥ MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			തിരികെ "persist-subscription-add";
		അവസ്ഥ MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			തിരികെ "persist-subscription-delete";
		അവസ്ഥ MOSQ_EVT_PERSIST_CLIENT_സന്ദേശം_ADD:
			തിരികെ "persist-client-msg-add";
		അവസ്ഥ MOSQ_EVT_PERSIST_CLIENT_സന്ദേശം_DELETE:
			തിരികെ "persist-client-msg-delete";
		അവസ്ഥ MOSQ_EVT_PERSIST_CLIENT_സന്ദേശം_UPDATE:
			തിരികെ "persist-client-msg-update";
		അവസ്ഥ MOSQ_EVT_PERSIST_WILL_ADD:
			തിരികെ "persist-will-add";
		അവസ്ഥ MOSQ_EVT_PERSIST_WILL_DELETE:
			തിരികെ "persist-will-delete";
	}
	തിരികെ "";
}


സ്ഥിതം bool check_callback_exists(ഘടന mosquitto__callback *cb_base, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	ഘടന mosquitto__callback *tail, *tmp;

	DL_FOREACH_SAFE(cb_base, tail, tmp){
		എങ്കിൽ(tail->identifier == identifier && tail->cb == cb_func){
			തിരികെ true;
		}
	}
	തിരികെ false;
}

സ്ഥിതം ഘടന mosquitto__callback **plugin__get_callback_base(ഘടന mosquitto__security_options *security_options, എണ്ണൽ mosquitto_plugin_event event)
{
	തിരഞ്ഞെടുപ്പ്(event){
		അവസ്ഥ MOSQ_EVT_RELOAD:
			തിരികെ &security_options->plugin_callbacks.reload;
		അവസ്ഥ MOSQ_EVT_ACL_CHECK:
			തിരികെ &security_options->plugin_callbacks.acl_check;
		അവസ്ഥ MOSQ_EVT_BASIC_AUTH:
			തിരികെ &security_options->plugin_callbacks.basic_auth;
		അവസ്ഥ MOSQ_EVT_PSK_താക്കോൽ:
			തിരികെ &security_options->plugin_callbacks.psk_താക്കോൽ;
		അവസ്ഥ MOSQ_EVT_EXT_AUTH_START:
			തിരികെ &security_options->plugin_callbacks.ext_auth_start;
		അവസ്ഥ MOSQ_EVT_EXT_AUTH_CONTINUE:
			തിരികെ &security_options->plugin_callbacks.ext_auth_continue;
		അവസ്ഥ MOSQ_EVT_CONTROL:
			തിരികെ ശൂന്യ;
		അവസ്ഥ MOSQ_EVT_MESSAGE_IN: /* same as MOSQ_EVT_MESSAGE */
			തിരികെ &security_options->plugin_callbacks.message_in;
		അവസ്ഥ MOSQ_EVT_TICK:
			തിരികെ &security_options->plugin_callbacks.tick;
		അവസ്ഥ MOSQ_EVT_DISCONNECT:
			തിരികെ &security_options->plugin_callbacks.disconnect;
		അവസ്ഥ MOSQ_EVT_CONNECT:
			തിരികെ &security_options->plugin_callbacks.connect;
		അവസ്ഥ MOSQ_EVT_CLIENT_OFFLINE:
			തിരികെ &security_options->plugin_callbacks.client_offline;
		അവസ്ഥ MOSQ_EVT_SUBSCRIBE:
			തിരികെ &security_options->plugin_callbacks.subscribe;
		അവസ്ഥ MOSQ_EVT_UNSUBSCRIBE:
			തിരികെ &security_options->plugin_callbacks.unsubscribe;
		അവസ്ഥ MOSQ_EVT_PERSIST_RESTORE:
			തിരികെ &security_options->plugin_callbacks.persist_restore;
		അവസ്ഥ MOSQ_EVT_PERSIST_CLIENT_ADD:
			തിരികെ &security_options->plugin_callbacks.persist_client_add;
		അവസ്ഥ MOSQ_EVT_PERSIST_CLIENT_DELETE:
			തിരികെ &security_options->plugin_callbacks.persist_client_delete;
		അവസ്ഥ MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			തിരികെ &security_options->plugin_callbacks.persist_client_update;
		അവസ്ഥ MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			തിരികെ &security_options->plugin_callbacks.persist_subscription_add;
		അവസ്ഥ MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			തിരികെ &security_options->plugin_callbacks.persist_subscription_delete;
		അവസ്ഥ MOSQ_EVT_PERSIST_CLIENT_സന്ദേശം_ADD:
			തിരികെ &security_options->plugin_callbacks.persist_client_സന്ദേശം_add;
		അവസ്ഥ MOSQ_EVT_PERSIST_CLIENT_സന്ദേശം_DELETE:
			തിരികെ &security_options->plugin_callbacks.persist_client_സന്ദേശം_delete;
		അവസ്ഥ MOSQ_EVT_PERSIST_CLIENT_സന്ദേശം_UPDATE:
			തിരികെ &security_options->plugin_callbacks.persist_client_സന്ദേശം_update;
		അവസ്ഥ MOSQ_EVT_PERSIST_BASE_സന്ദേശം_ADD:
			തിരികെ &security_options->plugin_callbacks.persist_base_സന്ദേശം_add;
		അവസ്ഥ MOSQ_EVT_PERSIST_BASE_സന്ദേശം_DELETE:
			തിരികെ &security_options->plugin_callbacks.persist_base_സന്ദേശം_delete;
		അവസ്ഥ MOSQ_EVT_PERSIST_RETAIN_സന്ദേശം_SET:
			തിരികെ &security_options->plugin_callbacks.persist_retain_സന്ദേശം_set;
		അവസ്ഥ MOSQ_EVT_PERSIST_RETAIN_സന്ദേശം_DELETE:
			തിരികെ &security_options->plugin_callbacks.persist_retain_സന്ദേശം_delete;
		അവസ്ഥ MOSQ_EVT_MESSAGE_OUT:
			തിരികെ &security_options->plugin_callbacks.message_out;
		അവസ്ഥ MOSQ_EVT_PERSIST_WILL_ADD:
			തിരികെ &security_options->plugin_callbacks.persist_will_add;
		അവസ്ഥ MOSQ_EVT_PERSIST_WILL_DELETE:
			തിരികെ &security_options->plugin_callbacks.persist_will_delete;
	}
	തിരികെ ശൂന്യ;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ remove_callback(mosquitto_plugin_id_t *plugin, ഘടന plugin_own_callback *own)
{
	ഘടന mosquitto__security_options *security_options;
	ഘടന mosquitto__callback *tail, *tmp;
	ഘടന mosquitto__callback **cb_base = ശൂന്യ;

	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<plugin->config.security_option_എണ്ണം; i++){
		security_options = plugin->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, own->event);
		എങ്കിൽ(cb_base == ശൂന്യ){
			തിരികെ MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_SAFE(*cb_base, tail, tmp){
			എങ്കിൽ(tail->identifier == plugin && tail->cb == own->cb_func){
				DL_DELETE(*cb_base, tail);
				mosquitto_FREE(tail);
				നിർത്തുക;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	തിരികെ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_callback_register(
		mosquitto_plugin_id_t *identifier,
		പൂർണ്ണസംഖ്യ event,
		MOSQ_FUNC_generic_callback cb_func,
		സ്ഥിരം ശൂന്യം *event_data,
		ശൂന്യം *userdata)
{
	ഘടന mosquitto__callback **cb_base = ശൂന്യ, *cb_new;
	ഘടന mosquitto__security_options *security_options;
	ഘടന plugin_own_callback *own_callback;

	എങ്കിൽ(cb_func == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}

	എങ്കിൽ(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
			|| event == MOSQ_EVT_PERSIST_BASE_സന്ദേശം_ADD
			|| event == MOSQ_EVT_PERSIST_BASE_സന്ദേശം_DELETE
			|| event == MOSQ_EVT_PERSIST_RETAIN_സന്ദേശം_SET
			|| event == MOSQ_EVT_PERSIST_RETAIN_സന്ദേശം_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_UPDATE
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_സന്ദേശം_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_സന്ദേശം_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_സന്ദേശം_UPDATE
			|| event == MOSQ_EVT_PERSIST_WILL_ADD
			|| event == MOSQ_EVT_PERSIST_WILL_DELETE
			)){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		തിരികെ MOSQ_ERR_INVAL;
	}

	എങ്കിൽ(event == MOSQ_EVT_CONTROL){
		തിരികെ control__register_callback(identifier, cb_func, event_data, userdata);
	}

	own_callback = mosquitto_calloc(1, വലുപ്പം(ഘടന plugin_own_callback));
	എങ്കിൽ(own_callback == ശൂന്യ){
		തിരികെ MOSQ_ERR_NOMEM;
	}
	own_callback->event = (എണ്ണൽ mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	എങ്കിൽ(identifier->config.security_option_എണ്ണം == 0){
		log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((എണ്ണൽ mosquitto_plugin_event)event));
		തിരികെ MOSQ_ERR_INVAL;
	}

	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<identifier->config.security_option_എണ്ണം; i++){
		security_options = identifier->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, (എണ്ണൽ mosquitto_plugin_event)event);
		എങ്കിൽ(cb_base == ശൂന്യ){
			തിരികെ MOSQ_ERR_NOT_SUPPORTED;
		}

		എങ്കിൽ(check_callback_exists(*cb_base, identifier, cb_func)){
			തിരികെ MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto__callback));
		എങ്കിൽ(cb_new == ശൂന്യ){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			തിരികെ MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_base, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	എങ്കിൽ(identifier->plugin_name){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((എണ്ണൽ mosquitto_plugin_event)event));
	}അല്ലെങ്കിൽ{
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((എണ്ണൽ mosquitto_plugin_event)event));
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	ഘടന plugin_own_callback *own, *own_tmp;

	എങ്കിൽ(plugin == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_SAFE(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		പൂർണ്ണസംഖ്യ event,
		MOSQ_FUNC_generic_callback cb_func,
		സ്ഥിരം ശൂന്യം *event_data)
{
	ഘടന plugin_own_callback *own, *own_tmp;

	എങ്കിൽ(identifier == ശൂന്യ || cb_func == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}

	എങ്കിൽ(event == MOSQ_EVT_CONTROL){
		തിരികെ control__unregister_callback(identifier, cb_func, event_data);
	}

	DL_FOREACH_SAFE(identifier->own_callbacks, own, own_tmp){
		എങ്കിൽ(own->event == (എണ്ണൽ mosquitto_plugin_event)event && own->cb_func == cb_func){
			തിരികെ remove_callback(identifier, own);
		}
	}

	തിരികെ MOSQ_ERR_NOT_FOUND;
}
