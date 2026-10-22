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

#অন্তর্ভাৱ "config.h"

#অন্তর্ভাৱ "mosquitto_broker_internal.h"
#অন্তর্ভাৱ "utlist.h"
#অন্তর্ভাৱ "lib_load.h"


স্থির ধ্রুৱ ৱর্ণ *get_event_name(গণনা mosquitto_plugin_event event)
{
	ৱিকল্প(event){
		পক্ষ MOSQ_EVT_RELOAD:
			ঘূৰাই "reload";
		পক্ষ MOSQ_EVT_ACL_CHECK:
			ঘূৰাই "acl-check";
		পক্ষ MOSQ_EVT_BASIC_AUTH:
			ঘূৰাই "basic-auth";
		পক্ষ MOSQ_EVT_PSK_চাবি:
			ঘূৰাই "psk-key";
		পক্ষ MOSQ_EVT_EXT_AUTH_START:
			ঘূৰাই "auth-start";
		পক্ষ MOSQ_EVT_EXT_AUTH_CONTINUE:
			ঘূৰাই "auth-continue";
		পক্ষ MOSQ_EVT_CONTROL:
			ঘূৰাই "control";
		পক্ষ MOSQ_EVT_MESSAGE_IN:
			ঘূৰাই "message-in";
		পক্ষ MOSQ_EVT_MESSAGE_OUT:
			ঘূৰাই "message-out";
		পক্ষ MOSQ_EVT_TICK:
			ঘূৰাই "tick";
		পক্ষ MOSQ_EVT_DISCONNECT:
			ঘূৰাই "disconnect";
		পক্ষ MOSQ_EVT_CONNECT:
			ঘূৰাই "connect";
		পক্ষ MOSQ_EVT_CLIENT_OFFLINE:
			ঘূৰাই "connect";
		পক্ষ MOSQ_EVT_SUBSCRIBE:
			ঘূৰাই "subscribe";
		পক্ষ MOSQ_EVT_UNSUBSCRIBE:
			ঘূৰাই "unsubscribe";
		পক্ষ MOSQ_EVT_PERSIST_RESTORE:
			ঘূৰাই "persist-restore";
		পক্ষ MOSQ_EVT_PERSIST_BASE_বাৰ্তা_ADD:
			ঘূৰাই "persist-base-msg-add";
		পক্ষ MOSQ_EVT_PERSIST_BASE_বাৰ্তা_DELETE:
			ঘূৰাই "persist-base-msg-delete";
		পক্ষ MOSQ_EVT_PERSIST_RETAIN_বাৰ্তা_SET:
			ঘূৰাই "persist-retain-msg-set";
		পক্ষ MOSQ_EVT_PERSIST_RETAIN_বাৰ্তা_DELETE:
			ঘূৰাই "persist-retain-msg-delete";
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_ADD:
			ঘূৰাই "persist-client-add";
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_DELETE:
			ঘূৰাই "persist-client-delete";
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			ঘূৰাই "persist-client-update";
		পক্ষ MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			ঘূৰাই "persist-subscription-add";
		পক্ষ MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			ঘূৰাই "persist-subscription-delete";
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_বাৰ্তা_ADD:
			ঘূৰাই "persist-client-msg-add";
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_বাৰ্তা_DELETE:
			ঘূৰাই "persist-client-msg-delete";
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_বাৰ্তা_UPDATE:
			ঘূৰাই "persist-client-msg-update";
		পক্ষ MOSQ_EVT_PERSIST_WILL_ADD:
			ঘূৰাই "persist-will-add";
		পক্ষ MOSQ_EVT_PERSIST_WILL_DELETE:
			ঘূৰাই "persist-will-delete";
	}
	ঘূৰাই "";
}


স্থির bool check_callback_exists(সংরচনা mosquitto__callback *cb_base, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	সংরচনা mosquitto__callback *tail, *tmp;

	DL_FOREACH_SAFE(cb_base, tail, tmp){
		যদি(tail->identifier == identifier && tail->cb == cb_func){
			ঘূৰাই true;
		}
	}
	ঘূৰাই false;
}

স্থির সংরচনা mosquitto__callback **plugin__get_callback_base(সংরচনা mosquitto__security_options *security_options, গণনা mosquitto_plugin_event event)
{
	ৱিকল্প(event){
		পক্ষ MOSQ_EVT_RELOAD:
			ঘূৰাই &security_options->plugin_callbacks.reload;
		পক্ষ MOSQ_EVT_ACL_CHECK:
			ঘূৰাই &security_options->plugin_callbacks.acl_check;
		পক্ষ MOSQ_EVT_BASIC_AUTH:
			ঘূৰাই &security_options->plugin_callbacks.basic_auth;
		পক্ষ MOSQ_EVT_PSK_চাবি:
			ঘূৰাই &security_options->plugin_callbacks.psk_চাবি;
		পক্ষ MOSQ_EVT_EXT_AUTH_START:
			ঘূৰাই &security_options->plugin_callbacks.ext_auth_start;
		পক্ষ MOSQ_EVT_EXT_AUTH_CONTINUE:
			ঘূৰাই &security_options->plugin_callbacks.ext_auth_continue;
		পক্ষ MOSQ_EVT_CONTROL:
			ঘূৰাই রিক্ত;
		পক্ষ MOSQ_EVT_MESSAGE_IN: /* same as MOSQ_EVT_MESSAGE */
			ঘূৰাই &security_options->plugin_callbacks.message_in;
		পক্ষ MOSQ_EVT_TICK:
			ঘূৰাই &security_options->plugin_callbacks.tick;
		পক্ষ MOSQ_EVT_DISCONNECT:
			ঘূৰাই &security_options->plugin_callbacks.disconnect;
		পক্ষ MOSQ_EVT_CONNECT:
			ঘূৰাই &security_options->plugin_callbacks.connect;
		পক্ষ MOSQ_EVT_CLIENT_OFFLINE:
			ঘূৰাই &security_options->plugin_callbacks.client_offline;
		পক্ষ MOSQ_EVT_SUBSCRIBE:
			ঘূৰাই &security_options->plugin_callbacks.subscribe;
		পক্ষ MOSQ_EVT_UNSUBSCRIBE:
			ঘূৰাই &security_options->plugin_callbacks.unsubscribe;
		পক্ষ MOSQ_EVT_PERSIST_RESTORE:
			ঘূৰাই &security_options->plugin_callbacks.persist_restore;
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_ADD:
			ঘূৰাই &security_options->plugin_callbacks.persist_client_add;
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_DELETE:
			ঘূৰাই &security_options->plugin_callbacks.persist_client_delete;
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			ঘূৰাই &security_options->plugin_callbacks.persist_client_update;
		পক্ষ MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			ঘূৰাই &security_options->plugin_callbacks.persist_subscription_add;
		পক্ষ MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			ঘূৰাই &security_options->plugin_callbacks.persist_subscription_delete;
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_বাৰ্তা_ADD:
			ঘূৰাই &security_options->plugin_callbacks.persist_client_বাৰ্তা_add;
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_বাৰ্তা_DELETE:
			ঘূৰাই &security_options->plugin_callbacks.persist_client_বাৰ্তা_delete;
		পক্ষ MOSQ_EVT_PERSIST_CLIENT_বাৰ্তা_UPDATE:
			ঘূৰাই &security_options->plugin_callbacks.persist_client_বাৰ্তা_update;
		পক্ষ MOSQ_EVT_PERSIST_BASE_বাৰ্তা_ADD:
			ঘূৰাই &security_options->plugin_callbacks.persist_base_বাৰ্তা_add;
		পক্ষ MOSQ_EVT_PERSIST_BASE_বাৰ্তা_DELETE:
			ঘূৰাই &security_options->plugin_callbacks.persist_base_বাৰ্তা_delete;
		পক্ষ MOSQ_EVT_PERSIST_RETAIN_বাৰ্তা_SET:
			ঘূৰাই &security_options->plugin_callbacks.persist_retain_বাৰ্তা_set;
		পক্ষ MOSQ_EVT_PERSIST_RETAIN_বাৰ্তা_DELETE:
			ঘূৰাই &security_options->plugin_callbacks.persist_retain_বাৰ্তা_delete;
		পক্ষ MOSQ_EVT_MESSAGE_OUT:
			ঘূৰাই &security_options->plugin_callbacks.message_out;
		পক্ষ MOSQ_EVT_PERSIST_WILL_ADD:
			ঘূৰাই &security_options->plugin_callbacks.persist_will_add;
		পক্ষ MOSQ_EVT_PERSIST_WILL_DELETE:
			ঘূৰাই &security_options->plugin_callbacks.persist_will_delete;
	}
	ঘূৰাই রিক্ত;
}


স্থির পূর্ণাঙ্ক remove_callback(mosquitto_plugin_id_t *plugin, সংরচনা plugin_own_callback *own)
{
	সংরচনা mosquitto__security_options *security_options;
	সংরচনা mosquitto__callback *tail, *tmp;
	সংরচনা mosquitto__callback **cb_base = রিক্ত;

	বাবে(পূর্ণাঙ্ক i=0; i<plugin->config.security_option_গুনতি; i++){
		security_options = plugin->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, own->event);
		যদি(cb_base == রিক্ত){
			ঘূৰাই MOSQ_ERR_NOT_SUPPORTED;
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

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_callback_register(
		mosquitto_plugin_id_t *identifier,
		পূর্ণাঙ্ক event,
		MOSQ_FUNC_generic_callback cb_func,
		ধ্রুৱ শূন্য *event_data,
		শূন্য *userdata)
{
	সংরচনা mosquitto__callback **cb_base = রিক্ত, *cb_new;
	সংরচনা mosquitto__security_options *security_options;
	সংরচনা plugin_own_callback *own_callback;

	যদি(cb_func == রিক্ত){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	যদি(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
			|| event == MOSQ_EVT_PERSIST_BASE_বাৰ্তা_ADD
			|| event == MOSQ_EVT_PERSIST_BASE_বাৰ্তা_DELETE
			|| event == MOSQ_EVT_PERSIST_RETAIN_বাৰ্তা_SET
			|| event == MOSQ_EVT_PERSIST_RETAIN_বাৰ্তা_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_UPDATE
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_বাৰ্তা_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_বাৰ্তা_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_বাৰ্তা_UPDATE
			|| event == MOSQ_EVT_PERSIST_WILL_ADD
			|| event == MOSQ_EVT_PERSIST_WILL_DELETE
			)){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	যদি(event == MOSQ_EVT_CONTROL){
		ঘূৰাই control__register_callback(identifier, cb_func, event_data, userdata);
	}

	own_callback = mosquitto_calloc(1, পরিমাণ(সংরচনা plugin_own_callback));
	যদি(own_callback == রিক্ত){
		ঘূৰাই MOSQ_ERR_NOMEM;
	}
	own_callback->event = (গণনা mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	যদি(identifier->config.security_option_গুনতি == 0){
		log__printf(রিক্ত, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((গণনা mosquitto_plugin_event)event));
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	বাবে(পূর্ণাঙ্ক i=0; i<identifier->config.security_option_গুনতি; i++){
		security_options = identifier->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, (গণনা mosquitto_plugin_event)event);
		যদি(cb_base == রিক্ত){
			ঘূৰাই MOSQ_ERR_NOT_SUPPORTED;
		}

		যদি(check_callback_exists(*cb_base, identifier, cb_func)){
			ঘূৰাই MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, পরিমাণ(সংরচনা mosquitto__callback));
		যদি(cb_new == রিক্ত){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			ঘূৰাই MOSQ_ERR_NOMEM;
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

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	সংরচনা plugin_own_callback *own, *own_tmp;

	যদি(plugin == রিক্ত){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_SAFE(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	ঘূৰাই MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		পূর্ণাঙ্ক event,
		MOSQ_FUNC_generic_callback cb_func,
		ধ্রুৱ শূন্য *event_data)
{
	সংরচনা plugin_own_callback *own, *own_tmp;

	যদি(identifier == রিক্ত || cb_func == রিক্ত){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	যদি(event == MOSQ_EVT_CONTROL){
		ঘূৰাই control__unregister_callback(identifier, cb_func, event_data);
	}

	DL_FOREACH_SAFE(identifier->own_callbacks, own, own_tmp){
		যদি(own->event == (গণনা mosquitto_plugin_event)event && own->cb_func == cb_func){
			ঘূৰাই remove_callback(identifier, own);
		}
	}

	ঘূৰাই MOSQ_ERR_NOT_FOUND;
}
