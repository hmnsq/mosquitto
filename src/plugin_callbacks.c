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


স্থির ধ্রুবক অক্ষর *get_event_name(গণনা mosquitto_plugin_event event)
{
	নির্বাচন(event){
		ক্ষেত্র MOSQ_EVT_RELOAD:
			ফেরত "reload";
		ক্ষেত্র MOSQ_EVT_ACL_CHECK:
			ফেরত "acl-check";
		ক্ষেত্র MOSQ_EVT_BASIC_AUTH:
			ফেরত "basic-auth";
		ক্ষেত্র MOSQ_EVT_PSK_চাবি:
			ফেরত "psk-key";
		ক্ষেত্র MOSQ_EVT_EXT_AUTH_START:
			ফেরত "auth-start";
		ক্ষেত্র MOSQ_EVT_EXT_AUTH_CONTINUE:
			ফেরত "auth-continue";
		ক্ষেত্র MOSQ_EVT_CONTROL:
			ফেরত "control";
		ক্ষেত্র MOSQ_EVT_MESSAGE_IN:
			ফেরত "message-in";
		ক্ষেত্র MOSQ_EVT_MESSAGE_OUT:
			ফেরত "message-out";
		ক্ষেত্র MOSQ_EVT_TICK:
			ফেরত "tick";
		ক্ষেত্র MOSQ_EVT_DISCONNECT:
			ফেরত "disconnect";
		ক্ষেত্র MOSQ_EVT_CONNECT:
			ফেরত "connect";
		ক্ষেত্র MOSQ_EVT_CLIENT_OFFLINE:
			ফেরত "connect";
		ক্ষেত্র MOSQ_EVT_SUBSCRIBE:
			ফেরত "subscribe";
		ক্ষেত্র MOSQ_EVT_UNSUBSCRIBE:
			ফেরত "unsubscribe";
		ক্ষেত্র MOSQ_EVT_PERSIST_RESTORE:
			ফেরত "persist-restore";
		ক্ষেত্র MOSQ_EVT_PERSIST_BASE_বার্তা_ADD:
			ফেরত "persist-base-msg-add";
		ক্ষেত্র MOSQ_EVT_PERSIST_BASE_বার্তা_DELETE:
			ফেরত "persist-base-msg-delete";
		ক্ষেত্র MOSQ_EVT_PERSIST_RETAIN_বার্তা_SET:
			ফেরত "persist-retain-msg-set";
		ক্ষেত্র MOSQ_EVT_PERSIST_RETAIN_বার্তা_DELETE:
			ফেরত "persist-retain-msg-delete";
		ক্ষেত্র MOSQ_EVT_PERSIST_CLIENT_ADD:
			ফেরত "persist-client-add";
		ক্ষেত্র MOSQ_EVT_PERSIST_CLIENT_DELETE:
			ফেরত "persist-client-delete";
		ক্ষেত্র MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			ফেরত "persist-client-update";
		ক্ষেত্র MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			ফেরত "persist-subscription-add";
		ক্ষেত্র MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			ফেরত "persist-subscription-delete";
		ক্ষেত্র MOSQ_EVT_PERSIST_CLIENT_বার্তা_ADD:
			ফেরত "persist-client-msg-add";
		ক্ষেত্র MOSQ_EVT_PERSIST_CLIENT_বার্তা_DELETE:
			ফেরত "persist-client-msg-delete";
		ক্ষেত্র MOSQ_EVT_PERSIST_CLIENT_বার্তা_UPDATE:
			ফেরত "persist-client-msg-update";
		ক্ষেত্র MOSQ_EVT_PERSIST_WILL_ADD:
			ফেরত "persist-will-add";
		ক্ষেত্র MOSQ_EVT_PERSIST_WILL_DELETE:
			ফেরত "persist-will-delete";
	}
	ফেরত "";
}


স্থির bool check_callback_exists(কাঠামো mosquitto__callback *cb_base, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	কাঠামো mosquitto__callback *লেজ, *tmp;

	DL_FOREACH_SAFE(cb_base, লেজ, tmp){
		যদি(লেজ->identifier == identifier && লেজ->cb == cb_func){
			ফেরত true;
		}
	}
	ফেরত false;
}

স্থির কাঠামো mosquitto__callback **plugin__get_callback_base(কাঠামো mosquitto__security_options *security_options, গণনা mosquitto_plugin_event event)
{
	নির্বাচন(event){
		ক্ষেত্র MOSQ_EVT_RELOAD:
			ফেরত &security_options->plugin_callbacks.reload;
		ক্ষেত্র MOSQ_EVT_ACL_CHECK:
			ফেরত &security_options->plugin_callbacks.acl_check;
		ক্ষেত্র MOSQ_EVT_BASIC_AUTH:
			ফেরত &security_options->plugin_callbacks.basic_auth;
		ক্ষেত্র MOSQ_EVT_PSK_চাবি:
			ফেরত &security_options->plugin_callbacks.psk_চাবি;
		ক্ষেত্র MOSQ_EVT_EXT_AUTH_START:
			ফেরত &security_options->plugin_callbacks.ext_auth_start;
		ক্ষেত্র MOSQ_EVT_EXT_AUTH_CONTINUE:
			ফেরত &security_options->plugin_callbacks.ext_auth_continue;
		ক্ষেত্র MOSQ_EVT_CONTROL:
			ফেরত ফাঁকা;
		ক্ষেত্র MOSQ_EVT_MESSAGE_IN: /* same as MOSQ_EVT_MESSAGE */
			ফেরত &security_options->plugin_callbacks.message_in;
		ক্ষেত্র MOSQ_EVT_TICK:
			ফেরত &security_options->plugin_callbacks.tick;
		ক্ষেত্র MOSQ_EVT_DISCONNECT:
			ফেরত &security_options->plugin_callbacks.disconnect;
		ক্ষেত্র MOSQ_EVT_CONNECT:
			ফেরত &security_options->plugin_callbacks.connect;
		ক্ষেত্র MOSQ_EVT_CLIENT_OFFLINE:
			ফেরত &security_options->plugin_callbacks.client_offline;
		ক্ষেত্র MOSQ_EVT_SUBSCRIBE:
			ফেরত &security_options->plugin_callbacks.subscribe;
		ক্ষেত্র MOSQ_EVT_UNSUBSCRIBE:
			ফেরত &security_options->plugin_callbacks.unsubscribe;
		ক্ষেত্র MOSQ_EVT_PERSIST_RESTORE:
			ফেরত &security_options->plugin_callbacks.persist_restore;
		ক্ষেত্র MOSQ_EVT_PERSIST_CLIENT_ADD:
			ফেরত &security_options->plugin_callbacks.persist_client_add;
		ক্ষেত্র MOSQ_EVT_PERSIST_CLIENT_DELETE:
			ফেরত &security_options->plugin_callbacks.persist_client_delete;
		ক্ষেত্র MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			ফেরত &security_options->plugin_callbacks.persist_client_update;
		ক্ষেত্র MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			ফেরত &security_options->plugin_callbacks.persist_subscription_add;
		ক্ষেত্র MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			ফেরত &security_options->plugin_callbacks.persist_subscription_delete;
		ক্ষেত্র MOSQ_EVT_PERSIST_CLIENT_বার্তা_ADD:
			ফেরত &security_options->plugin_callbacks.persist_client_বার্তা_add;
		ক্ষেত্র MOSQ_EVT_PERSIST_CLIENT_বার্তা_DELETE:
			ফেরত &security_options->plugin_callbacks.persist_client_বার্তা_delete;
		ক্ষেত্র MOSQ_EVT_PERSIST_CLIENT_বার্তা_UPDATE:
			ফেরত &security_options->plugin_callbacks.persist_client_বার্তা_update;
		ক্ষেত্র MOSQ_EVT_PERSIST_BASE_বার্তা_ADD:
			ফেরত &security_options->plugin_callbacks.persist_base_বার্তা_add;
		ক্ষেত্র MOSQ_EVT_PERSIST_BASE_বার্তা_DELETE:
			ফেরত &security_options->plugin_callbacks.persist_base_বার্তা_delete;
		ক্ষেত্র MOSQ_EVT_PERSIST_RETAIN_বার্তা_SET:
			ফেরত &security_options->plugin_callbacks.persist_retain_বার্তা_set;
		ক্ষেত্র MOSQ_EVT_PERSIST_RETAIN_বার্তা_DELETE:
			ফেরত &security_options->plugin_callbacks.persist_retain_বার্তা_delete;
		ক্ষেত্র MOSQ_EVT_MESSAGE_OUT:
			ফেরত &security_options->plugin_callbacks.message_out;
		ক্ষেত্র MOSQ_EVT_PERSIST_WILL_ADD:
			ফেরত &security_options->plugin_callbacks.persist_will_add;
		ক্ষেত্র MOSQ_EVT_PERSIST_WILL_DELETE:
			ফেরত &security_options->plugin_callbacks.persist_will_delete;
	}
	ফেরত ফাঁকা;
}


স্থির পূর্ণসংখ্যা remove_callback(mosquitto_plugin_id_t *plugin, কাঠামো plugin_own_callback *own)
{
	কাঠামো mosquitto__security_options *security_options;
	কাঠামো mosquitto__callback *লেজ, *tmp;
	কাঠামো mosquitto__callback **cb_base = ফাঁকা;

	জন্য(পূর্ণসংখ্যা i=0; i<plugin->config.security_option_গুনতি; i++){
		security_options = plugin->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, own->event);
		যদি(cb_base == ফাঁকা){
			ফেরত MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_SAFE(*cb_base, লেজ, tmp){
			যদি(লেজ->identifier == plugin && লেজ->cb == own->cb_func){
				DL_DELETE(*cb_base, লেজ);
				mosquitto_FREE(লেজ);
				বিরতি;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	ফেরত MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_callback_register(
		mosquitto_plugin_id_t *identifier,
		পূর্ণসংখ্যা event,
		MOSQ_FUNC_generic_callback cb_func,
		ধ্রুবক শূন্য *event_তথ্য,
		শূন্য *userdata)
{
	কাঠামো mosquitto__callback **cb_base = ফাঁকা, *cb_new;
	কাঠামো mosquitto__security_options *security_options;
	কাঠামো plugin_own_callback *own_callback;

	যদি(cb_func == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
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
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		ফেরত MOSQ_ERR_INVAL;
	}

	যদি(event == MOSQ_EVT_CONTROL){
		ফেরত control__register_callback(identifier, cb_func, event_তথ্য, userdata);
	}

	own_callback = mosquitto_calloc(1, আকার(কাঠামো plugin_own_callback));
	যদি(own_callback == ফাঁকা){
		ফেরত MOSQ_ERR_NOMEM;
	}
	own_callback->event = (গণনা mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	যদি(identifier->config.security_option_গুনতি == 0){
		log__printf(ফাঁকা, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((গণনা mosquitto_plugin_event)event));
		ফেরত MOSQ_ERR_INVAL;
	}

	জন্য(পূর্ণসংখ্যা i=0; i<identifier->config.security_option_গুনতি; i++){
		security_options = identifier->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, (গণনা mosquitto_plugin_event)event);
		যদি(cb_base == ফাঁকা){
			ফেরত MOSQ_ERR_NOT_SUPPORTED;
		}

		যদি(check_callback_exists(*cb_base, identifier, cb_func)){
			ফেরত MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, আকার(কাঠামো mosquitto__callback));
		যদি(cb_new == ফাঁকা){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			ফেরত MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_base, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	যদি(identifier->plugin_name){
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((গণনা mosquitto_plugin_event)event));
	}অন্যথা{
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((গণনা mosquitto_plugin_event)event));
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	কাঠামো plugin_own_callback *own, *own_tmp;

	যদি(plugin == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_SAFE(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT পূর্ণসংখ্যা mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		পূর্ণসংখ্যা event,
		MOSQ_FUNC_generic_callback cb_func,
		ধ্রুবক শূন্য *event_তথ্য)
{
	কাঠামো plugin_own_callback *own, *own_tmp;

	যদি(identifier == ফাঁকা || cb_func == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}

	যদি(event == MOSQ_EVT_CONTROL){
		ফেরত control__unregister_callback(identifier, cb_func, event_তথ্য);
	}

	DL_FOREACH_SAFE(identifier->own_callbacks, own, own_tmp){
		যদি(own->event == (গণনা mosquitto_plugin_event)event && own->cb_func == cb_func){
			ফেরত remove_callback(identifier, own);
		}
	}

	ফেরত MOSQ_ERR_NOT_FOUND;
}
