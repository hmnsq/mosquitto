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

#شامل "config.h"

#شامل "mosquitto_broker_internal.h"
#شامل "utlist.h"
#شامل "lib_load.h"


ایستا ثابت نویسه *get_event_name(شمارش mosquitto_plugin_event event)
{
	انتخاب(event){
		حالت MOSQ_EVT_RELOAD:
			بازگشت "reload";
		حالت MOSQ_EVT_ACL_CHECK:
			بازگشت "acl-check";
		حالت MOSQ_EVT_BASIC_AUTH:
			بازگشت "basic-auth";
		حالت MOSQ_EVT_PSK_کلید:
			بازگشت "psk-key";
		حالت MOSQ_EVT_EXT_AUTH_START:
			بازگشت "auth-start";
		حالت MOSQ_EVT_EXT_AUTH_CONTINUE:
			بازگشت "auth-continue";
		حالت MOSQ_EVT_CONTROL:
			بازگشت "control";
		حالت MOSQ_EVT_MESSAGE_IN:
			بازگشت "message-in";
		حالت MOSQ_EVT_MESSAGE_OUT:
			بازگشت "message-out";
		حالت MOSQ_EVT_TICK:
			بازگشت "tick";
		حالت MOSQ_EVT_DISCONNECT:
			بازگشت "disconnect";
		حالت MOSQ_EVT_CONNECT:
			بازگشت "connect";
		حالت MOSQ_EVT_CLIENT_OFFLINE:
			بازگشت "connect";
		حالت MOSQ_EVT_SUBSCRIBE:
			بازگشت "subscribe";
		حالت MOSQ_EVT_UNSUBSCRIBE:
			بازگشت "unsubscribe";
		حالت MOSQ_EVT_PERSIST_RESTORE:
			بازگشت "persist-restore";
		حالت MOSQ_EVT_PERSIST_BASE_پیام_ADD:
			بازگشت "persist-base-msg-add";
		حالت MOSQ_EVT_PERSIST_BASE_پیام_DELETE:
			بازگشت "persist-base-msg-delete";
		حالت MOSQ_EVT_PERSIST_RETAIN_پیام_SET:
			بازگشت "persist-retain-msg-set";
		حالت MOSQ_EVT_PERSIST_RETAIN_پیام_DELETE:
			بازگشت "persist-retain-msg-delete";
		حالت MOSQ_EVT_PERSIST_CLIENT_ADD:
			بازگشت "persist-client-add";
		حالت MOSQ_EVT_PERSIST_CLIENT_DELETE:
			بازگشت "persist-client-delete";
		حالت MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			بازگشت "persist-client-update";
		حالت MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			بازگشت "persist-subscription-add";
		حالت MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			بازگشت "persist-subscription-delete";
		حالت MOSQ_EVT_PERSIST_CLIENT_پیام_ADD:
			بازگشت "persist-client-msg-add";
		حالت MOSQ_EVT_PERSIST_CLIENT_پیام_DELETE:
			بازگشت "persist-client-msg-delete";
		حالت MOSQ_EVT_PERSIST_CLIENT_پیام_UPDATE:
			بازگشت "persist-client-msg-update";
		حالت MOSQ_EVT_PERSIST_WILL_ADD:
			بازگشت "persist-will-add";
		حالت MOSQ_EVT_PERSIST_WILL_DELETE:
			بازگشت "persist-will-delete";
	}
	بازگشت "";
}


ایستا bool check_callback_exists(ساختار mosquitto__callback *cb_base, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	ساختار mosquitto__callback *tail, *tmp;

	DL_FOREACH_SAFE(cb_base, tail, tmp){
		اگر(tail->identifier == identifier && tail->cb == cb_func){
			بازگشت true;
		}
	}
	بازگشت false;
}

ایستا ساختار mosquitto__callback **plugin__get_callback_base(ساختار mosquitto__security_options *security_options, شمارش mosquitto_plugin_event event)
{
	انتخاب(event){
		حالت MOSQ_EVT_RELOAD:
			بازگشت &security_options->plugin_callbacks.reload;
		حالت MOSQ_EVT_ACL_CHECK:
			بازگشت &security_options->plugin_callbacks.acl_check;
		حالت MOSQ_EVT_BASIC_AUTH:
			بازگشت &security_options->plugin_callbacks.basic_auth;
		حالت MOSQ_EVT_PSK_کلید:
			بازگشت &security_options->plugin_callbacks.psk_کلید;
		حالت MOSQ_EVT_EXT_AUTH_START:
			بازگشت &security_options->plugin_callbacks.ext_auth_start;
		حالت MOSQ_EVT_EXT_AUTH_CONTINUE:
			بازگشت &security_options->plugin_callbacks.ext_auth_continue;
		حالت MOSQ_EVT_CONTROL:
			بازگشت پوچ;
		حالت MOSQ_EVT_MESSAGE_IN: /* same as MOSQ_EVT_MESSAGE */
			بازگشت &security_options->plugin_callbacks.message_in;
		حالت MOSQ_EVT_TICK:
			بازگشت &security_options->plugin_callbacks.tick;
		حالت MOSQ_EVT_DISCONNECT:
			بازگشت &security_options->plugin_callbacks.disconnect;
		حالت MOSQ_EVT_CONNECT:
			بازگشت &security_options->plugin_callbacks.connect;
		حالت MOSQ_EVT_CLIENT_OFFLINE:
			بازگشت &security_options->plugin_callbacks.client_offline;
		حالت MOSQ_EVT_SUBSCRIBE:
			بازگشت &security_options->plugin_callbacks.subscribe;
		حالت MOSQ_EVT_UNSUBSCRIBE:
			بازگشت &security_options->plugin_callbacks.unsubscribe;
		حالت MOSQ_EVT_PERSIST_RESTORE:
			بازگشت &security_options->plugin_callbacks.persist_restore;
		حالت MOSQ_EVT_PERSIST_CLIENT_ADD:
			بازگشت &security_options->plugin_callbacks.persist_client_add;
		حالت MOSQ_EVT_PERSIST_CLIENT_DELETE:
			بازگشت &security_options->plugin_callbacks.persist_client_delete;
		حالت MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			بازگشت &security_options->plugin_callbacks.persist_client_update;
		حالت MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			بازگشت &security_options->plugin_callbacks.persist_subscription_add;
		حالت MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			بازگشت &security_options->plugin_callbacks.persist_subscription_delete;
		حالت MOSQ_EVT_PERSIST_CLIENT_پیام_ADD:
			بازگشت &security_options->plugin_callbacks.persist_client_پیام_add;
		حالت MOSQ_EVT_PERSIST_CLIENT_پیام_DELETE:
			بازگشت &security_options->plugin_callbacks.persist_client_پیام_delete;
		حالت MOSQ_EVT_PERSIST_CLIENT_پیام_UPDATE:
			بازگشت &security_options->plugin_callbacks.persist_client_پیام_update;
		حالت MOSQ_EVT_PERSIST_BASE_پیام_ADD:
			بازگشت &security_options->plugin_callbacks.persist_base_پیام_add;
		حالت MOSQ_EVT_PERSIST_BASE_پیام_DELETE:
			بازگشت &security_options->plugin_callbacks.persist_base_پیام_delete;
		حالت MOSQ_EVT_PERSIST_RETAIN_پیام_SET:
			بازگشت &security_options->plugin_callbacks.persist_retain_پیام_set;
		حالت MOSQ_EVT_PERSIST_RETAIN_پیام_DELETE:
			بازگشت &security_options->plugin_callbacks.persist_retain_پیام_delete;
		حالت MOSQ_EVT_MESSAGE_OUT:
			بازگشت &security_options->plugin_callbacks.message_out;
		حالت MOSQ_EVT_PERSIST_WILL_ADD:
			بازگشت &security_options->plugin_callbacks.persist_will_add;
		حالت MOSQ_EVT_PERSIST_WILL_DELETE:
			بازگشت &security_options->plugin_callbacks.persist_will_delete;
	}
	بازگشت پوچ;
}


ایستا صحیح remove_callback(mosquitto_plugin_id_t *plugin, ساختار plugin_own_callback *own)
{
	ساختار mosquitto__security_options *security_options;
	ساختار mosquitto__callback *tail, *tmp;
	ساختار mosquitto__callback **cb_base = پوچ;

	برای(صحیح i=0; i<plugin->config.security_option_شمار; i++){
		security_options = plugin->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, own->event);
		اگر(cb_base == پوچ){
			بازگشت MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_SAFE(*cb_base, tail, tmp){
			اگر(tail->identifier == plugin && tail->cb == own->cb_func){
				DL_DELETE(*cb_base, tail);
				mosquitto_FREE(tail);
				بشکن;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	بازگشت MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحیح mosquitto_callback_register(
		mosquitto_plugin_id_t *identifier,
		صحیح event,
		MOSQ_FUNC_generic_callback cb_func,
		ثابت تهی *event_data,
		تهی *userdata)
{
	ساختار mosquitto__callback **cb_base = پوچ, *cb_new;
	ساختار mosquitto__security_options *security_options;
	ساختار plugin_own_callback *own_callback;

	اگر(cb_func == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}

	اگر(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
			|| event == MOSQ_EVT_PERSIST_BASE_پیام_ADD
			|| event == MOSQ_EVT_PERSIST_BASE_پیام_DELETE
			|| event == MOSQ_EVT_PERSIST_RETAIN_پیام_SET
			|| event == MOSQ_EVT_PERSIST_RETAIN_پیام_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_UPDATE
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_پیام_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_پیام_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_پیام_UPDATE
			|| event == MOSQ_EVT_PERSIST_WILL_ADD
			|| event == MOSQ_EVT_PERSIST_WILL_DELETE
			)){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		بازگشت MOSQ_ERR_INVAL;
	}

	اگر(event == MOSQ_EVT_CONTROL){
		بازگشت control__register_callback(identifier, cb_func, event_data, userdata);
	}

	own_callback = mosquitto_calloc(1, اندازه(ساختار plugin_own_callback));
	اگر(own_callback == پوچ){
		بازگشت MOSQ_ERR_NOMEM;
	}
	own_callback->event = (شمارش mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	اگر(identifier->config.security_option_شمار == 0){
		log__printf(پوچ, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((شمارش mosquitto_plugin_event)event));
		بازگشت MOSQ_ERR_INVAL;
	}

	برای(صحیح i=0; i<identifier->config.security_option_شمار; i++){
		security_options = identifier->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, (شمارش mosquitto_plugin_event)event);
		اگر(cb_base == پوچ){
			بازگشت MOSQ_ERR_NOT_SUPPORTED;
		}

		اگر(check_callback_exists(*cb_base, identifier, cb_func)){
			بازگشت MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, اندازه(ساختار mosquitto__callback));
		اگر(cb_new == پوچ){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			بازگشت MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_base, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	اگر(identifier->plugin_name){
		log__printf(پوچ, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((شمارش mosquitto_plugin_event)event));
	}وگرنه{
		log__printf(پوچ, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((شمارش mosquitto_plugin_event)event));
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	ساختار plugin_own_callback *own, *own_tmp;

	اگر(plugin == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_SAFE(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحیح mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		صحیح event,
		MOSQ_FUNC_generic_callback cb_func,
		ثابت تهی *event_data)
{
	ساختار plugin_own_callback *own, *own_tmp;

	اگر(identifier == پوچ || cb_func == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}

	اگر(event == MOSQ_EVT_CONTROL){
		بازگشت control__unregister_callback(identifier, cb_func, event_data);
	}

	DL_FOREACH_SAFE(identifier->own_callbacks, own, own_tmp){
		اگر(own->event == (شمارش mosquitto_plugin_event)event && own->cb_func == cb_func){
			بازگشت remove_callback(identifier, own);
		}
	}

	بازگشت MOSQ_ERR_NOT_FOUND;
}
