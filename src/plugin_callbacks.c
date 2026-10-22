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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "utlist.h"
#অন্তর্ভাব "lib_load.h"


স্থির ধ্রুব বর্ণ *get_event_name(গণনা mosquitto_plugin_event event)
{
	বিকল্প(event){
		পক্ষ MOSQ_EVT_RELOAD:
			প্রত্যাগম "reload";
		পক্ষ MOSQ_EVT_ACL_CHECK:
			প্রত্যাগম "acl-check";
		পক্ষ MOSQ_EVT_BASIC_AUTH:
			প্রত্যাগম "basic-auth";
		পক্ষ MOSQ_EVT_PSK_চাবি:
			প্রত্যাগম "psk-key";
		পক্ষ MOSQ_EVT_EXT_AUTH_START:
			প্রত্যাগম "auth-start";
		পক্ষ MOSQ_EVT_EXT_AUTH_CONTINUE:
			প্রত্যাগম "auth-continue";
		পক্ষ MOSQ_EVT_CONTROL:
			প্রত্যাগম "control";
		পক্ষ MOSQ_EVT_MESSAGE_IN:
			প্রত্যাগম "message-in";
		পক্ষ MOSQ_EVT_MESSAGE_OUT:
			প্রত্যাগম "message-out";
		পক্ষ MOSQ_EVT_TICK:
			প্রত্যাগম "tick";
		পক্ষ MOSQ_EVT_DISCONNECT:
			প্রত্যাগম "disconnect";
		পক্ষ MOSQ_EVT_CONNECT:
			প্রত্যাগম "connect";
		পক্ষ MOSQ_EVT_CLIENT_OFFLINE:
			প্রত্যাগম "connect";
		পক্ষ MOSQ_EVT_SUBSCRIBE:
			প্রত্যাগম "subscribe";
		পক্ষ MOSQ_EVT_UNSUBSCRIBE:
			প্রত্যাগম "unsubscribe";
		পক্ষ MOSQ_EVT_PERSIST_RESTORE:
			প্রত্যাগম "persist-restore";
		পক্ষ MOSQ_EVT_PERSIST_BASE_বার্তা_ADD:
			প্রত্যাগম "persist-base-msg-add";
		পক্ষ MOSQ_EVT_PERSIST_BASE_বার্তা_DELETE:
			প্রত্যাগম "persist-base-msg-delete";
		পক্ষ MOSQ_EVT_PERSIST_RETAIN_বার্তা_SET:
			প্রত্যাগম "persist-retain-msg-set";
		পক্ষ MOSQ_EVT_PERSIST_RETAIN_বার্তা_DELETE:
			প্রত্যাগম "persist-retain-msg-delete";
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_ADD:
			প্রত্যাগম "persist-client-add";
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_DELETE:
			প্রত্যাগম "persist-client-delete";
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			প্রত্যাগম "persist-client-update";
		পক্ষ MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			প্রত্যাগম "persist-subscription-add";
		পক্ষ MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			প্রত্যাগম "persist-subscription-delete";
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_বার্তা_ADD:
			প্রত্যাগম "persist-client-msg-add";
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_বার্তা_DELETE:
			প্রত্যাগম "persist-client-msg-delete";
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_বার্তা_UPDATE:
			প্রত্যাগম "persist-client-msg-update";
		পক্ষ MOSQ_EVT_PERSIST_WILL_ADD:
			প্রত্যাগম "persist-will-add";
		পক্ষ MOSQ_EVT_PERSIST_WILL_DELETE:
			প্রত্যাগম "persist-will-delete";
	}
	প্রত্যাগম "";
}


স্থির bool check_callback_exists(সংরচনা mosquitto__callback *cb_base, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	সংরচনা mosquitto__callback *tail, *tmp;

	DL_FOREACH_SAFE(cb_base, tail, tmp){
		যদি(tail->identifier == identifier && tail->cb == cb_func){
			প্রত্যাগম true;
		}
	}
	প্রত্যাগম false;
}

স্থির সংরচনা mosquitto__callback **plugin__get_callback_base(সংরচনা mosquitto__security_options *security_options, গণনা mosquitto_plugin_event event)
{
	বিকল্প(event){
		পক্ষ MOSQ_EVT_RELOAD:
			প্রত্যাগম &security_options->plugin_callbacks.reload;
		পক্ষ MOSQ_EVT_ACL_CHECK:
			প্রত্যাগম &security_options->plugin_callbacks.acl_check;
		পক্ষ MOSQ_EVT_BASIC_AUTH:
			প্রত্যাগম &security_options->plugin_callbacks.basic_auth;
		পক্ষ MOSQ_EVT_PSK_চাবি:
			প্রত্যাগম &security_options->plugin_callbacks.psk_চাবি;
		পক্ষ MOSQ_EVT_EXT_AUTH_START:
			প্রত্যাগম &security_options->plugin_callbacks.ext_auth_start;
		পক্ষ MOSQ_EVT_EXT_AUTH_CONTINUE:
			প্রত্যাগম &security_options->plugin_callbacks.ext_auth_continue;
		পক্ষ MOSQ_EVT_CONTROL:
			প্রত্যাগম রিক্ত;
		পক্ষ MOSQ_EVT_MESSAGE_IN: /* same as MOSQ_EVT_MESSAGE */
			প্রত্যাগম &security_options->plugin_callbacks.message_in;
		পক্ষ MOSQ_EVT_TICK:
			প্রত্যাগম &security_options->plugin_callbacks.tick;
		পক্ষ MOSQ_EVT_DISCONNECT:
			প্রত্যাগম &security_options->plugin_callbacks.disconnect;
		পক্ষ MOSQ_EVT_CONNECT:
			প্রত্যাগম &security_options->plugin_callbacks.connect;
		পক্ষ MOSQ_EVT_CLIENT_OFFLINE:
			প্রত্যাগম &security_options->plugin_callbacks.client_offline;
		পক্ষ MOSQ_EVT_SUBSCRIBE:
			প্রত্যাগম &security_options->plugin_callbacks.subscribe;
		পক্ষ MOSQ_EVT_UNSUBSCRIBE:
			প্রত্যাগম &security_options->plugin_callbacks.unsubscribe;
		পক্ষ MOSQ_EVT_PERSIST_RESTORE:
			প্রত্যাগম &security_options->plugin_callbacks.persist_restore;
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_ADD:
			প্রত্যাগম &security_options->plugin_callbacks.persist_client_add;
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_DELETE:
			প্রত্যাগম &security_options->plugin_callbacks.persist_client_delete;
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			প্রত্যাগম &security_options->plugin_callbacks.persist_client_update;
		পক্ষ MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			প্রত্যাগম &security_options->plugin_callbacks.persist_subscription_add;
		পক্ষ MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			প্রত্যাগম &security_options->plugin_callbacks.persist_subscription_delete;
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_বার্তা_ADD:
			প্রত্যাগম &security_options->plugin_callbacks.persist_client_বার্তা_add;
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_বার্তা_DELETE:
			প্রত্যাগম &security_options->plugin_callbacks.persist_client_বার্তা_delete;
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_বার্তা_UPDATE:
			প্রত্যাগম &security_options->plugin_callbacks.persist_client_বার্তা_update;
		পক্ষ MOSQ_EVT_PERSIST_BASE_বার্তা_ADD:
			প্রত্যাগম &security_options->plugin_callbacks.persist_base_বার্তা_add;
		পক্ষ MOSQ_EVT_PERSIST_BASE_বার্তা_DELETE:
			প্রত্যাগম &security_options->plugin_callbacks.persist_base_বার্তা_delete;
		পক্ষ MOSQ_EVT_PERSIST_RETAIN_বার্তা_SET:
			প্রত্যাগম &security_options->plugin_callbacks.persist_retain_বার্তা_set;
		পক্ষ MOSQ_EVT_PERSIST_RETAIN_বার্তা_DELETE:
			প্রত্যাগম &security_options->plugin_callbacks.persist_retain_বার্তা_delete;
		পক্ষ MOSQ_EVT_MESSAGE_OUT:
			প্রত্যাগম &security_options->plugin_callbacks.message_out;
		পক্ষ MOSQ_EVT_PERSIST_WILL_ADD:
			প্রত্যাগম &security_options->plugin_callbacks.persist_will_add;
		পক্ষ MOSQ_EVT_PERSIST_WILL_DELETE:
			প্রত্যাগম &security_options->plugin_callbacks.persist_will_delete;
	}
	প্রত্যাগম রিক্ত;
}


স্থির পূর্ণাঙ্ক remove_callback(mosquitto_plugin_id_t *plugin, সংরচনা plugin_own_callback *own)
{
	সংরচনা mosquitto__security_options *security_options;
	সংরচনা mosquitto__callback *tail, *tmp;
	সংরচনা mosquitto__callback **cb_base = রিক্ত;

	প্রতি(পূর্ণাঙ্ক i=0; i<plugin->config.security_option_গুনতি; i++){
		security_options = plugin->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, own->event);
		যদি(cb_base == রিক্ত){
			প্রত্যাগম MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_SAFE(*cb_base, tail, tmp){
			যদি(tail->identifier == plugin && tail->cb == own->cb_func){
				DL_DELETE(*cb_base, tail);
				mosquitto_FREE(tail);
				ভঙ্গ;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_callback_register(
		mosquitto_plugin_id_t *identifier,
		পূর্ণাঙ্ক event,
		MOSQ_FUNC_generic_callback cb_func,
		ধ্রুব শূন্য *event_data,
		শূন্য *userdata)
{
	সংরচনা mosquitto__callback **cb_base = রিক্ত, *cb_new;
	সংরচনা mosquitto__security_options *security_options;
	সংরচনা plugin_own_callback *own_callback;

	যদি(cb_func == রিক্ত){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	যদি(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
			|| event == MOSQ_EVT_PERSIST_BASE_বার্তা_ADD
			|| event == MOSQ_EVT_PERSIST_BASE_বার্তা_DELETE
			|| event == MOSQ_EVT_PERSIST_RETAIN_বার্তা_SET
			|| event == MOSQ_EVT_PERSIST_RETAIN_বার্তা_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_UPDATE
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_বার্তা_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_বার্তা_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_বার্তা_UPDATE
			|| event == MOSQ_EVT_PERSIST_WILL_ADD
			|| event == MOSQ_EVT_PERSIST_WILL_DELETE
			)){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	যদি(event == MOSQ_EVT_CONTROL){
		প্রত্যাগম control__register_callback(identifier, cb_func, event_data, userdata);
	}

	own_callback = mosquitto_calloc(1, পরিমাণ(সংরচনা plugin_own_callback));
	যদি(own_callback == রিক্ত){
		প্রত্যাগম MOSQ_ERR_NOMEM;
	}
	own_callback->event = (গণনা mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	যদি(identifier->config.security_option_গুনতি == 0){
		log__printf(রিক্ত, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((গণনা mosquitto_plugin_event)event));
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	প্রতি(পূর্ণাঙ্ক i=0; i<identifier->config.security_option_গুনতি; i++){
		security_options = identifier->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, (গণনা mosquitto_plugin_event)event);
		যদি(cb_base == রিক্ত){
			প্রত্যাগম MOSQ_ERR_NOT_SUPPORTED;
		}

		যদি(check_callback_exists(*cb_base, identifier, cb_func)){
			প্রত্যাগম MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, পরিমাণ(সংরচনা mosquitto__callback));
		যদি(cb_new == রিক্ত){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			প্রত্যাগম MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_base, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	যদি(identifier->plugin_name){
		log__printf(রিক্ত, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((গণনা mosquitto_plugin_event)event));
	}অন্যথা{
		log__printf(রিক্ত, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((গণনা mosquitto_plugin_event)event));
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	সংরচনা plugin_own_callback *own, *own_tmp;

	যদি(plugin == রিক্ত){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_SAFE(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		পূর্ণাঙ্ক event,
		MOSQ_FUNC_generic_callback cb_func,
		ধ্রুব শূন্য *event_data)
{
	সংরচনা plugin_own_callback *own, *own_tmp;

	যদি(identifier == রিক্ত || cb_func == রিক্ত){
		প্রত্যাগম MOSQ_ERR_INVAL;
	}

	যদি(event == MOSQ_EVT_CONTROL){
		প্রত্যাগম control__unregister_callback(identifier, cb_func, event_data);
	}

	DL_FOREACH_SAFE(identifier->own_callbacks, own, own_tmp){
		যদি(own->event == (গণনা mosquitto_plugin_event)event && own->cb_func == cb_func){
			প্রত্যাগম remove_callback(identifier, own);
		}
	}

	প্রত্যাগম MOSQ_ERR_NOT_FOUND;
}
