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


ساکن ثابت حرف *get_event_name(شمار mosquitto_plugin_event event)
{
	انتخاب(event){
		حالت MOSQ_EVT_RELOAD:
			واپس "reload";
		حالت MOSQ_EVT_ACL_CHECK:
			واپس "acl-check";
		حالت MOSQ_EVT_BASIC_AUTH:
			واپس "basic-auth";
		حالت MOSQ_EVT_PSK_کلید:
			واپس "psk-key";
		حالت MOSQ_EVT_EXT_AUTH_آغاز:
			واپس "auth-start";
		حالت MOSQ_EVT_EXT_AUTH_CONTINUE:
			واپس "auth-continue";
		حالت MOSQ_EVT_CONTROL:
			واپس "control";
		حالت MOSQ_EVT_پیغام_اندر:
			واپس "message-in";
		حالت MOSQ_EVT_پیغام_برآمد:
			واپس "message-out";
		حالت MOSQ_EVT_TICK:
			واپس "tick";
		حالت MOSQ_EVT_DISCONNECT:
			واپس "disconnect";
		حالت MOSQ_EVT_CONNECT:
			واپس "connect";
		حالت MOSQ_EVT_CLIENT_OFFLINE:
			واپس "connect";
		حالت MOSQ_EVT_SUBSCRIBE:
			واپس "subscribe";
		حالت MOSQ_EVT_UNSUBSCRIBE:
			واپس "unsubscribe";
		حالت MOSQ_EVT_PERSIST_RESTORE:
			واپس "persist-restore";
		حالت MOSQ_EVT_PERSIST_بنیاد_پیغام_جوڑو:
			واپس "persist-base-msg-add";
		حالت MOSQ_EVT_PERSIST_بنیاد_پیغام_DELETE:
			واپس "persist-base-msg-delete";
		حالت MOSQ_EVT_PERSIST_RETAIN_پیغام_سیٹ:
			واپس "persist-retain-msg-set";
		حالت MOSQ_EVT_PERSIST_RETAIN_پیغام_DELETE:
			واپس "persist-retain-msg-delete";
		حالت MOSQ_EVT_PERSIST_CLIENT_جوڑو:
			واپس "persist-client-add";
		حالت MOSQ_EVT_PERSIST_CLIENT_DELETE:
			واپس "persist-client-delete";
		حالت MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			واپس "persist-client-update";
		حالت MOSQ_EVT_PERSIST_SUBSCRIPTION_جوڑو:
			واپس "persist-subscription-add";
		حالت MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			واپس "persist-subscription-delete";
		حالت MOSQ_EVT_PERSIST_CLIENT_پیغام_جوڑو:
			واپس "persist-client-msg-add";
		حالت MOSQ_EVT_PERSIST_CLIENT_پیغام_DELETE:
			واپس "persist-client-msg-delete";
		حالت MOSQ_EVT_PERSIST_CLIENT_پیغام_UPDATE:
			واپس "persist-client-msg-update";
		حالت MOSQ_EVT_PERSIST_WILL_جوڑو:
			واپس "persist-will-add";
		حالت MOSQ_EVT_PERSIST_WILL_DELETE:
			واپس "persist-will-delete";
	}
	واپس "";
}


ساکن bool check_callback_exists(ساخت mosquitto__callback *cb_بنیاد, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	ساخت mosquitto__callback *دم, *tmp;

	DL_FOREACH_محفوظ(cb_بنیاد, دم, tmp){
		اگر(دم->identifier == identifier && دم->cb == cb_func){
			واپس true;
		}
	}
	واپس false;
}

ساکن ساخت mosquitto__callback **plugin_get_callback_بنیاد(ساخت mosquitto__security_options *security_options, شمار mosquitto_plugin_event event)
{
	انتخاب(event){
		حالت MOSQ_EVT_RELOAD:
			واپس &security_options->plugin_callbacks.reload;
		حالت MOSQ_EVT_ACL_CHECK:
			واپس &security_options->plugin_callbacks.acl_check;
		حالت MOSQ_EVT_BASIC_AUTH:
			واپس &security_options->plugin_callbacks.basic_auth;
		حالت MOSQ_EVT_PSK_کلید:
			واپس &security_options->plugin_callbacks.psk_کلید;
		حالت MOSQ_EVT_EXT_AUTH_آغاز:
			واپس &security_options->plugin_callbacks.ext_auth_آغاز;
		حالت MOSQ_EVT_EXT_AUTH_CONTINUE:
			واپس &security_options->plugin_callbacks.ext_auth_continue;
		حالت MOSQ_EVT_CONTROL:
			واپس تہی;
		حالت MOSQ_EVT_پیغام_اندر: /* same as MOSQ_EVT_MESSAGE */
			واپس &security_options->plugin_callbacks.پیغام_اندر;
		حالت MOSQ_EVT_TICK:
			واپس &security_options->plugin_callbacks.tick;
		حالت MOSQ_EVT_DISCONNECT:
			واپس &security_options->plugin_callbacks.disconnect;
		حالت MOSQ_EVT_CONNECT:
			واپس &security_options->plugin_callbacks.connect;
		حالت MOSQ_EVT_CLIENT_OFFLINE:
			واپس &security_options->plugin_callbacks.client_offline;
		حالت MOSQ_EVT_SUBSCRIBE:
			واپس &security_options->plugin_callbacks.subscribe;
		حالت MOSQ_EVT_UNSUBSCRIBE:
			واپس &security_options->plugin_callbacks.unsubscribe;
		حالت MOSQ_EVT_PERSIST_RESTORE:
			واپس &security_options->plugin_callbacks.persist_restore;
		حالت MOSQ_EVT_PERSIST_CLIENT_جوڑو:
			واپس &security_options->plugin_callbacks.persist_client_جوڑو;
		حالت MOSQ_EVT_PERSIST_CLIENT_DELETE:
			واپس &security_options->plugin_callbacks.persist_client_delete;
		حالت MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			واپس &security_options->plugin_callbacks.persist_client_update;
		حالت MOSQ_EVT_PERSIST_SUBSCRIPTION_جوڑو:
			واپس &security_options->plugin_callbacks.persist_subscription_جوڑو;
		حالت MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			واپس &security_options->plugin_callbacks.persist_subscription_delete;
		حالت MOSQ_EVT_PERSIST_CLIENT_پیغام_جوڑو:
			واپس &security_options->plugin_callbacks.persist_client_پیغام_جوڑو;
		حالت MOSQ_EVT_PERSIST_CLIENT_پیغام_DELETE:
			واپس &security_options->plugin_callbacks.persist_client_پیغام_delete;
		حالت MOSQ_EVT_PERSIST_CLIENT_پیغام_UPDATE:
			واپس &security_options->plugin_callbacks.persist_client_پیغام_update;
		حالت MOSQ_EVT_PERSIST_بنیاد_پیغام_جوڑو:
			واپس &security_options->plugin_callbacks.persist_بنیاد_پیغام_جوڑو;
		حالت MOSQ_EVT_PERSIST_بنیاد_پیغام_DELETE:
			واپس &security_options->plugin_callbacks.persist_بنیاد_پیغام_delete;
		حالت MOSQ_EVT_PERSIST_RETAIN_پیغام_سیٹ:
			واپس &security_options->plugin_callbacks.persist_retain_پیغام_سیٹ;
		حالت MOSQ_EVT_PERSIST_RETAIN_پیغام_DELETE:
			واپس &security_options->plugin_callbacks.persist_retain_پیغام_delete;
		حالت MOSQ_EVT_پیغام_برآمد:
			واپس &security_options->plugin_callbacks.پیغام_برآمد;
		حالت MOSQ_EVT_PERSIST_WILL_جوڑو:
			واپس &security_options->plugin_callbacks.persist_will_جوڑو;
		حالت MOSQ_EVT_PERSIST_WILL_DELETE:
			واپس &security_options->plugin_callbacks.persist_will_delete;
	}
	واپس تہی;
}


ساکن صحیح remove_callback(mosquitto_plugin_id_t *plugin, ساخت plugin_own_callback *own)
{
	ساخت mosquitto__security_options *security_options;
	ساخت mosquitto__callback *دم, *tmp;
	ساخت mosquitto__callback **cb_بنیاد = تہی;

	برائے(صحیح i=0; i<plugin->config.security_option_گنتی; i++){
		security_options = plugin->config.security_options[i];

		cb_بنیاد = plugin_get_callback_بنیاد(security_options, own->event);
		اگر(cb_بنیاد == تہی){
			واپس MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_محفوظ(*cb_بنیاد, دم, tmp){
			اگر(دم->identifier == plugin && دم->cb == own->cb_func){
				DL_DELETE(*cb_بنیاد, دم);
				mosquitto_FREE(دم);
				توڑو;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	واپس MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحیح mosquitto_callback_رجسٹر(
		mosquitto_plugin_id_t *identifier,
		صحیح event,
		MOSQ_FUNC_generic_callback cb_func,
		ثابت خالی *event_ڈیٹا,
		خالی *userdata)
{
	ساخت mosquitto__callback **cb_بنیاد = تہی, *cb_new;
	ساخت mosquitto__security_options *security_options;
	ساخت plugin_own_callback *own_callback;

	اگر(cb_func == تہی){
		واپس MOSQ_ERR_INVAL;
	}

	اگر(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
			|| event == MOSQ_EVT_PERSIST_بنیاد_پیغام_جوڑو
			|| event == MOSQ_EVT_PERSIST_بنیاد_پیغام_DELETE
			|| event == MOSQ_EVT_PERSIST_RETAIN_پیغام_سیٹ
			|| event == MOSQ_EVT_PERSIST_RETAIN_پیغام_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_جوڑو
			|| event == MOSQ_EVT_PERSIST_CLIENT_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_UPDATE
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_جوڑو
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_پیغام_جوڑو
			|| event == MOSQ_EVT_PERSIST_CLIENT_پیغام_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_پیغام_UPDATE
			|| event == MOSQ_EVT_PERSIST_WILL_جوڑو
			|| event == MOSQ_EVT_PERSIST_WILL_DELETE
			)){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		واپس MOSQ_ERR_INVAL;
	}

	اگر(event == MOSQ_EVT_CONTROL){
		واپس control_رجسٹر_callback(identifier, cb_func, event_ڈیٹا, userdata);
	}

	own_callback = mosquitto_calloc(1, حجم(ساخت plugin_own_callback));
	اگر(own_callback == تہی){
		واپس MOSQ_ERR_NOMEM;
	}
	own_callback->event = (شمار mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	اگر(identifier->config.security_option_گنتی == 0){
		log__printf(تہی, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((شمار mosquitto_plugin_event)event));
		واپس MOSQ_ERR_INVAL;
	}

	برائے(صحیح i=0; i<identifier->config.security_option_گنتی; i++){
		security_options = identifier->config.security_options[i];

		cb_بنیاد = plugin_get_callback_بنیاد(security_options, (شمار mosquitto_plugin_event)event);
		اگر(cb_بنیاد == تہی){
			واپس MOSQ_ERR_NOT_SUPPORTED;
		}

		اگر(check_callback_exists(*cb_بنیاد, identifier, cb_func)){
			واپس MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, حجم(ساخت mosquitto__callback));
		اگر(cb_new == تہی){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			واپس MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_بنیاد, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	اگر(identifier->plugin_name){
		log__printf(تہی, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((شمار mosquitto_plugin_event)event));
	}ورنہ{
		log__printf(تہی, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((شمار mosquitto_plugin_event)event));
	}

	واپس MOSQ_ERR_SUCCESS;
}


صحیح plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	ساخت plugin_own_callback *own, *own_tmp;

	اگر(plugin == تہی){
		واپس MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_محفوظ(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	واپس MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحیح mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		صحیح event,
		MOSQ_FUNC_generic_callback cb_func,
		ثابت خالی *event_ڈیٹا)
{
	ساخت plugin_own_callback *own, *own_tmp;

	اگر(identifier == تہی || cb_func == تہی){
		واپس MOSQ_ERR_INVAL;
	}

	اگر(event == MOSQ_EVT_CONTROL){
		واپس control__unregister_callback(identifier, cb_func, event_ڈیٹا);
	}

	DL_FOREACH_محفوظ(identifier->own_callbacks, own, own_tmp){
		اگر(own->event == (شمار mosquitto_plugin_event)event && own->cb_func == cb_func){
			واپس remove_callback(identifier, own);
		}
	}

	واپس MOSQ_ERR_NOT_ملا;
}
