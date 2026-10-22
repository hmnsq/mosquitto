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

#ضمن "config.h"

#ضمن "mosquitto_broker_internal.h"
#ضمن "utlist.h"
#ضمن "lib_load.h"


ساكن ثابت حرف *get_event_name(تعداد mosquitto_plugin_event event)
{
	بدل(event){
		حالة MOSQ_EVT_RELOAD:
			إرجاع "reload";
		حالة MOSQ_EVT_ACL_CHECK:
			إرجاع "acl-check";
		حالة MOSQ_EVT_BASIC_AUTH:
			إرجاع "basic-auth";
		حالة MOSQ_EVT_PSK_مفتاح:
			إرجاع "psk-key";
		حالة MOSQ_EVT_EXT_AUTH_START:
			إرجاع "auth-start";
		حالة MOSQ_EVT_EXT_AUTH_CONTINUE:
			إرجاع "auth-continue";
		حالة MOSQ_EVT_CONTROL:
			إرجاع "control";
		حالة MOSQ_EVT_MESSAGE_IN:
			إرجاع "message-in";
		حالة MOSQ_EVT_MESSAGE_OUT:
			إرجاع "message-out";
		حالة MOSQ_EVT_TICK:
			إرجاع "tick";
		حالة MOSQ_EVT_DISCONNECT:
			إرجاع "disconnect";
		حالة MOSQ_EVT_CONNECT:
			إرجاع "connect";
		حالة MOSQ_EVT_CLIENT_OFFLINE:
			إرجاع "connect";
		حالة MOSQ_EVT_SUBSCRIBE:
			إرجاع "subscribe";
		حالة MOSQ_EVT_UNSUBSCRIBE:
			إرجاع "unsubscribe";
		حالة MOSQ_EVT_PERSIST_RESTORE:
			إرجاع "persist-restore";
		حالة MOSQ_EVT_PERSIST_BASE_رسالة_ADD:
			إرجاع "persist-base-msg-add";
		حالة MOSQ_EVT_PERSIST_BASE_رسالة_DELETE:
			إرجاع "persist-base-msg-delete";
		حالة MOSQ_EVT_PERSIST_RETAIN_رسالة_SET:
			إرجاع "persist-retain-msg-set";
		حالة MOSQ_EVT_PERSIST_RETAIN_رسالة_DELETE:
			إرجاع "persist-retain-msg-delete";
		حالة MOSQ_EVT_PERSIST_CLIENT_ADD:
			إرجاع "persist-client-add";
		حالة MOSQ_EVT_PERSIST_CLIENT_DELETE:
			إرجاع "persist-client-delete";
		حالة MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			إرجاع "persist-client-update";
		حالة MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			إرجاع "persist-subscription-add";
		حالة MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			إرجاع "persist-subscription-delete";
		حالة MOSQ_EVT_PERSIST_CLIENT_رسالة_ADD:
			إرجاع "persist-client-msg-add";
		حالة MOSQ_EVT_PERSIST_CLIENT_رسالة_DELETE:
			إرجاع "persist-client-msg-delete";
		حالة MOSQ_EVT_PERSIST_CLIENT_رسالة_UPDATE:
			إرجاع "persist-client-msg-update";
		حالة MOSQ_EVT_PERSIST_WILL_ADD:
			إرجاع "persist-will-add";
		حالة MOSQ_EVT_PERSIST_WILL_DELETE:
			إرجاع "persist-will-delete";
	}
	إرجاع "";
}


ساكن bool check_callback_exists(بنية mosquitto__callback *cb_base, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	بنية mosquitto__callback *tail, *tmp;

	DL_FOREACH_SAFE(cb_base, tail, tmp){
		إذا(tail->identifier == identifier && tail->cb == cb_func){
			إرجاع true;
		}
	}
	إرجاع false;
}

ساكن بنية mosquitto__callback **plugin__get_callback_base(بنية mosquitto__security_options *security_options, تعداد mosquitto_plugin_event event)
{
	بدل(event){
		حالة MOSQ_EVT_RELOAD:
			إرجاع &security_options->plugin_callbacks.reload;
		حالة MOSQ_EVT_ACL_CHECK:
			إرجاع &security_options->plugin_callbacks.acl_check;
		حالة MOSQ_EVT_BASIC_AUTH:
			إرجاع &security_options->plugin_callbacks.basic_auth;
		حالة MOSQ_EVT_PSK_مفتاح:
			إرجاع &security_options->plugin_callbacks.psk_مفتاح;
		حالة MOSQ_EVT_EXT_AUTH_START:
			إرجاع &security_options->plugin_callbacks.ext_auth_start;
		حالة MOSQ_EVT_EXT_AUTH_CONTINUE:
			إرجاع &security_options->plugin_callbacks.ext_auth_continue;
		حالة MOSQ_EVT_CONTROL:
			إرجاع عدم;
		حالة MOSQ_EVT_MESSAGE_IN: /* same as MOSQ_EVT_MESSAGE */
			إرجاع &security_options->plugin_callbacks.message_in;
		حالة MOSQ_EVT_TICK:
			إرجاع &security_options->plugin_callbacks.tick;
		حالة MOSQ_EVT_DISCONNECT:
			إرجاع &security_options->plugin_callbacks.disconnect;
		حالة MOSQ_EVT_CONNECT:
			إرجاع &security_options->plugin_callbacks.connect;
		حالة MOSQ_EVT_CLIENT_OFFLINE:
			إرجاع &security_options->plugin_callbacks.client_offline;
		حالة MOSQ_EVT_SUBSCRIBE:
			إرجاع &security_options->plugin_callbacks.subscribe;
		حالة MOSQ_EVT_UNSUBSCRIBE:
			إرجاع &security_options->plugin_callbacks.unsubscribe;
		حالة MOSQ_EVT_PERSIST_RESTORE:
			إرجاع &security_options->plugin_callbacks.persist_restore;
		حالة MOSQ_EVT_PERSIST_CLIENT_ADD:
			إرجاع &security_options->plugin_callbacks.persist_client_add;
		حالة MOSQ_EVT_PERSIST_CLIENT_DELETE:
			إرجاع &security_options->plugin_callbacks.persist_client_delete;
		حالة MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			إرجاع &security_options->plugin_callbacks.persist_client_update;
		حالة MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			إرجاع &security_options->plugin_callbacks.persist_subscription_add;
		حالة MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			إرجاع &security_options->plugin_callbacks.persist_subscription_delete;
		حالة MOSQ_EVT_PERSIST_CLIENT_رسالة_ADD:
			إرجاع &security_options->plugin_callbacks.persist_client_رسالة_add;
		حالة MOSQ_EVT_PERSIST_CLIENT_رسالة_DELETE:
			إرجاع &security_options->plugin_callbacks.persist_client_رسالة_delete;
		حالة MOSQ_EVT_PERSIST_CLIENT_رسالة_UPDATE:
			إرجاع &security_options->plugin_callbacks.persist_client_رسالة_update;
		حالة MOSQ_EVT_PERSIST_BASE_رسالة_ADD:
			إرجاع &security_options->plugin_callbacks.persist_base_رسالة_add;
		حالة MOSQ_EVT_PERSIST_BASE_رسالة_DELETE:
			إرجاع &security_options->plugin_callbacks.persist_base_رسالة_delete;
		حالة MOSQ_EVT_PERSIST_RETAIN_رسالة_SET:
			إرجاع &security_options->plugin_callbacks.persist_retain_رسالة_set;
		حالة MOSQ_EVT_PERSIST_RETAIN_رسالة_DELETE:
			إرجاع &security_options->plugin_callbacks.persist_retain_رسالة_delete;
		حالة MOSQ_EVT_MESSAGE_OUT:
			إرجاع &security_options->plugin_callbacks.message_out;
		حالة MOSQ_EVT_PERSIST_WILL_ADD:
			إرجاع &security_options->plugin_callbacks.persist_will_add;
		حالة MOSQ_EVT_PERSIST_WILL_DELETE:
			إرجاع &security_options->plugin_callbacks.persist_will_delete;
	}
	إرجاع عدم;
}


ساكن صحيح remove_callback(mosquitto_plugin_id_t *plugin, بنية plugin_own_callback *own)
{
	بنية mosquitto__security_options *security_options;
	بنية mosquitto__callback *tail, *tmp;
	بنية mosquitto__callback **cb_base = عدم;

	لكل(صحيح i=0; i<plugin->config.security_option_تعداد; i++){
		security_options = plugin->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, own->event);
		إذا(cb_base == عدم){
			إرجاع MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_SAFE(*cb_base, tail, tmp){
			إذا(tail->identifier == plugin && tail->cb == own->cb_func){
				DL_DELETE(*cb_base, tail);
				mosquitto_FREE(tail);
				اكسر;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	إرجاع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_callback_register(
		mosquitto_plugin_id_t *identifier,
		صحيح event,
		MOSQ_FUNC_generic_callback cb_func,
		ثابت فراغ *event_data,
		فراغ *userdata)
{
	بنية mosquitto__callback **cb_base = عدم, *cb_new;
	بنية mosquitto__security_options *security_options;
	بنية plugin_own_callback *own_callback;

	إذا(cb_func == عدم){
		إرجاع MOSQ_ERR_INVAL;
	}

	إذا(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
			|| event == MOSQ_EVT_PERSIST_BASE_رسالة_ADD
			|| event == MOSQ_EVT_PERSIST_BASE_رسالة_DELETE
			|| event == MOSQ_EVT_PERSIST_RETAIN_رسالة_SET
			|| event == MOSQ_EVT_PERSIST_RETAIN_رسالة_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_UPDATE
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_رسالة_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_رسالة_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_رسالة_UPDATE
			|| event == MOSQ_EVT_PERSIST_WILL_ADD
			|| event == MOSQ_EVT_PERSIST_WILL_DELETE
			)){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		إرجاع MOSQ_ERR_INVAL;
	}

	إذا(event == MOSQ_EVT_CONTROL){
		إرجاع control__register_callback(identifier, cb_func, event_data, userdata);
	}

	own_callback = mosquitto_calloc(1, حجم(بنية plugin_own_callback));
	إذا(own_callback == عدم){
		إرجاع MOSQ_ERR_NOMEM;
	}
	own_callback->event = (تعداد mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	إذا(identifier->config.security_option_تعداد == 0){
		log__printf(عدم, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((تعداد mosquitto_plugin_event)event));
		إرجاع MOSQ_ERR_INVAL;
	}

	لكل(صحيح i=0; i<identifier->config.security_option_تعداد; i++){
		security_options = identifier->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, (تعداد mosquitto_plugin_event)event);
		إذا(cb_base == عدم){
			إرجاع MOSQ_ERR_NOT_SUPPORTED;
		}

		إذا(check_callback_exists(*cb_base, identifier, cb_func)){
			إرجاع MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, حجم(بنية mosquitto__callback));
		إذا(cb_new == عدم){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			إرجاع MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_base, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	إذا(identifier->plugin_name){
		log__printf(عدم, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((تعداد mosquitto_plugin_event)event));
	}وإلا{
		log__printf(عدم, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((تعداد mosquitto_plugin_event)event));
	}

	إرجاع MOSQ_ERR_SUCCESS;
}


صحيح plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	بنية plugin_own_callback *own, *own_tmp;

	إذا(plugin == عدم){
		إرجاع MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_SAFE(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	إرجاع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		صحيح event,
		MOSQ_FUNC_generic_callback cb_func,
		ثابت فراغ *event_data)
{
	بنية plugin_own_callback *own, *own_tmp;

	إذا(identifier == عدم || cb_func == عدم){
		إرجاع MOSQ_ERR_INVAL;
	}

	إذا(event == MOSQ_EVT_CONTROL){
		إرجاع control__unregister_callback(identifier, cb_func, event_data);
	}

	DL_FOREACH_SAFE(identifier->own_callbacks, own, own_tmp){
		إذا(own->event == (تعداد mosquitto_plugin_event)event && own->cb_func == cb_func){
			إرجاع remove_callback(identifier, own);
		}
	}

	إرجاع MOSQ_ERR_NOT_FOUND;
}
