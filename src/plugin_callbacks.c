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
			أرجع "reload";
		حالة MOSQ_EVT_ACL_CHECK:
			أرجع "acl-check";
		حالة MOSQ_EVT_BASIC_AUTH:
			أرجع "basic-auth";
		حالة MOSQ_EVT_PSK_ڪنجي:
			أرجع "psk-key";
		حالة MOSQ_EVT_EXT_AUTH_شروعات:
			أرجع "auth-start";
		حالة MOSQ_EVT_EXT_AUTH_CONTINUE:
			أرجع "auth-continue";
		حالة MOSQ_EVT_CONTROL:
			أرجع "control";
		حالة MOSQ_EVT_پيغام_اندريون:
			أرجع "message-in";
		حالة MOSQ_EVT_پيغام_ٻاهريون:
			أرجع "message-out";
		حالة MOSQ_EVT_TICK:
			أرجع "tick";
		حالة MOSQ_EVT_DISCONNECT:
			أرجع "disconnect";
		حالة MOSQ_EVT_CONNECT:
			أرجع "connect";
		حالة MOSQ_EVT_CLIENT_OFFLINE:
			أرجع "connect";
		حالة MOSQ_EVT_SUBSCRIBE:
			أرجع "subscribe";
		حالة MOSQ_EVT_UNSUBSCRIBE:
			أرجع "unsubscribe";
		حالة MOSQ_EVT_PERSIST_RESTORE:
			أرجع "persist-restore";
		حالة MOSQ_EVT_PERSIST_بنياد_نياپو_جوڙيو:
			أرجع "persist-base-msg-add";
		حالة MOSQ_EVT_PERSIST_بنياد_نياپو_DELETE:
			أرجع "persist-base-msg-delete";
		حالة MOSQ_EVT_PERSIST_RETAIN_نياپو_سيٽ:
			أرجع "persist-retain-msg-set";
		حالة MOSQ_EVT_PERSIST_RETAIN_نياپو_DELETE:
			أرجع "persist-retain-msg-delete";
		حالة MOSQ_EVT_PERSIST_CLIENT_جوڙيو:
			أرجع "persist-client-add";
		حالة MOSQ_EVT_PERSIST_CLIENT_DELETE:
			أرجع "persist-client-delete";
		حالة MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			أرجع "persist-client-update";
		حالة MOSQ_EVT_PERSIST_SUBSCRIPTION_جوڙيو:
			أرجع "persist-subscription-add";
		حالة MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			أرجع "persist-subscription-delete";
		حالة MOSQ_EVT_PERSIST_CLIENT_نياپو_جوڙيو:
			أرجع "persist-client-msg-add";
		حالة MOSQ_EVT_PERSIST_CLIENT_نياپو_DELETE:
			أرجع "persist-client-msg-delete";
		حالة MOSQ_EVT_PERSIST_CLIENT_نياپو_UPDATE:
			أرجع "persist-client-msg-update";
		حالة MOSQ_EVT_PERSIST_WILL_جوڙيو:
			أرجع "persist-will-add";
		حالة MOSQ_EVT_PERSIST_WILL_DELETE:
			أرجع "persist-will-delete";
	}
	أرجع "";
}


ساكن bool check_callback_exists(بنية mosquitto__callback *cb_بنياد, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	بنية mosquitto__callback *پڇ, *tmp;

	DL_FOREACH_محفوظ(cb_بنياد, پڇ, tmp){
		إذا(پڇ->identifier == identifier && پڇ->cb == cb_func){
			أرجع true;
		}
	}
	أرجع false;
}

ساكن بنية mosquitto__callback **plugin_get_callback_بنياد(بنية mosquitto__security_options *security_options, تعداد mosquitto_plugin_event event)
{
	بدل(event){
		حالة MOSQ_EVT_RELOAD:
			أرجع &security_options->plugin_callbacks.reload;
		حالة MOSQ_EVT_ACL_CHECK:
			أرجع &security_options->plugin_callbacks.acl_check;
		حالة MOSQ_EVT_BASIC_AUTH:
			أرجع &security_options->plugin_callbacks.basic_auth;
		حالة MOSQ_EVT_PSK_ڪنجي:
			أرجع &security_options->plugin_callbacks.psk_ڪنجي;
		حالة MOSQ_EVT_EXT_AUTH_شروعات:
			أرجع &security_options->plugin_callbacks.ext_auth_شروعات;
		حالة MOSQ_EVT_EXT_AUTH_CONTINUE:
			أرجع &security_options->plugin_callbacks.ext_auth_continue;
		حالة MOSQ_EVT_CONTROL:
			أرجع عدم;
		حالة MOSQ_EVT_پيغام_اندريون: /* same as MOSQ_EVT_MESSAGE */
			أرجع &security_options->plugin_callbacks.پيغام_اندريون;
		حالة MOSQ_EVT_TICK:
			أرجع &security_options->plugin_callbacks.tick;
		حالة MOSQ_EVT_DISCONNECT:
			أرجع &security_options->plugin_callbacks.disconnect;
		حالة MOSQ_EVT_CONNECT:
			أرجع &security_options->plugin_callbacks.connect;
		حالة MOSQ_EVT_CLIENT_OFFLINE:
			أرجع &security_options->plugin_callbacks.client_offline;
		حالة MOSQ_EVT_SUBSCRIBE:
			أرجع &security_options->plugin_callbacks.subscribe;
		حالة MOSQ_EVT_UNSUBSCRIBE:
			أرجع &security_options->plugin_callbacks.unsubscribe;
		حالة MOSQ_EVT_PERSIST_RESTORE:
			أرجع &security_options->plugin_callbacks.persist_restore;
		حالة MOSQ_EVT_PERSIST_CLIENT_جوڙيو:
			أرجع &security_options->plugin_callbacks.persist_client_جوڙيو;
		حالة MOSQ_EVT_PERSIST_CLIENT_DELETE:
			أرجع &security_options->plugin_callbacks.persist_client_delete;
		حالة MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			أرجع &security_options->plugin_callbacks.persist_client_update;
		حالة MOSQ_EVT_PERSIST_SUBSCRIPTION_جوڙيو:
			أرجع &security_options->plugin_callbacks.persist_subscription_جوڙيو;
		حالة MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			أرجع &security_options->plugin_callbacks.persist_subscription_delete;
		حالة MOSQ_EVT_PERSIST_CLIENT_نياپو_جوڙيو:
			أرجع &security_options->plugin_callbacks.persist_client_نياپو_جوڙيو;
		حالة MOSQ_EVT_PERSIST_CLIENT_نياپو_DELETE:
			أرجع &security_options->plugin_callbacks.persist_client_نياپو_delete;
		حالة MOSQ_EVT_PERSIST_CLIENT_نياپو_UPDATE:
			أرجع &security_options->plugin_callbacks.persist_client_نياپو_update;
		حالة MOSQ_EVT_PERSIST_بنياد_نياپو_جوڙيو:
			أرجع &security_options->plugin_callbacks.persist_بنياد_نياپو_جوڙيو;
		حالة MOSQ_EVT_PERSIST_بنياد_نياپو_DELETE:
			أرجع &security_options->plugin_callbacks.persist_بنياد_نياپو_delete;
		حالة MOSQ_EVT_PERSIST_RETAIN_نياپو_سيٽ:
			أرجع &security_options->plugin_callbacks.persist_retain_نياپو_سيٽ;
		حالة MOSQ_EVT_PERSIST_RETAIN_نياپو_DELETE:
			أرجع &security_options->plugin_callbacks.persist_retain_نياپو_delete;
		حالة MOSQ_EVT_پيغام_ٻاهريون:
			أرجع &security_options->plugin_callbacks.پيغام_ٻاهريون;
		حالة MOSQ_EVT_PERSIST_WILL_جوڙيو:
			أرجع &security_options->plugin_callbacks.persist_will_جوڙيو;
		حالة MOSQ_EVT_PERSIST_WILL_DELETE:
			أرجع &security_options->plugin_callbacks.persist_will_delete;
	}
	أرجع عدم;
}


ساكن صحيح remove_callback(mosquitto_plugin_id_t *plugin, بنية plugin_own_callback *own)
{
	بنية mosquitto__security_options *security_options;
	بنية mosquitto__callback *پڇ, *tmp;
	بنية mosquitto__callback **cb_بنياد = عدم;

	لكل(صحيح i=0; i<plugin->config.security_option_ڳڻپ; i++){
		security_options = plugin->config.security_options[i];

		cb_بنياد = plugin_get_callback_بنياد(security_options, own->event);
		إذا(cb_بنياد == عدم){
			أرجع MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_محفوظ(*cb_بنياد, پڇ, tmp){
			إذا(پڇ->identifier == plugin && پڇ->cb == own->cb_func){
				DL_DELETE(*cb_بنياد, پڇ);
				mosquitto_FREE(پڇ);
				اكسر;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_callback_رجسٽر(
		mosquitto_plugin_id_t *identifier,
		صحيح event,
		MOSQ_FUNC_generic_callback cb_func,
		ثابت فراغ *event_معلومات,
		فراغ *userdata)
{
	بنية mosquitto__callback **cb_بنياد = عدم, *cb_new;
	بنية mosquitto__security_options *security_options;
	بنية plugin_own_callback *own_callback;

	إذا(cb_func == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
			|| event == MOSQ_EVT_PERSIST_بنياد_نياپو_جوڙيو
			|| event == MOSQ_EVT_PERSIST_بنياد_نياپو_DELETE
			|| event == MOSQ_EVT_PERSIST_RETAIN_نياپو_سيٽ
			|| event == MOSQ_EVT_PERSIST_RETAIN_نياپو_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_جوڙيو
			|| event == MOSQ_EVT_PERSIST_CLIENT_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_UPDATE
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_جوڙيو
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_نياپو_جوڙيو
			|| event == MOSQ_EVT_PERSIST_CLIENT_نياپو_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_نياپو_UPDATE
			|| event == MOSQ_EVT_PERSIST_WILL_جوڙيو
			|| event == MOSQ_EVT_PERSIST_WILL_DELETE
			)){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(event == MOSQ_EVT_CONTROL){
		أرجع control_رجسٽر_callback(identifier, cb_func, event_معلومات, userdata);
	}

	own_callback = mosquitto_calloc(1, حجم(بنية plugin_own_callback));
	إذا(own_callback == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}
	own_callback->event = (تعداد mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	إذا(identifier->config.security_option_ڳڻپ == 0){
		log__printf(عدم, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((تعداد mosquitto_plugin_event)event));
		أرجع MOSQ_ERR_INVAL;
	}

	لكل(صحيح i=0; i<identifier->config.security_option_ڳڻپ; i++){
		security_options = identifier->config.security_options[i];

		cb_بنياد = plugin_get_callback_بنياد(security_options, (تعداد mosquitto_plugin_event)event);
		إذا(cb_بنياد == عدم){
			أرجع MOSQ_ERR_NOT_SUPPORTED;
		}

		إذا(check_callback_exists(*cb_بنياد, identifier, cb_func)){
			أرجع MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, حجم(بنية mosquitto__callback));
		إذا(cb_new == عدم){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			أرجع MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_بنياد, cb_new);
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

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	بنية plugin_own_callback *own, *own_tmp;

	إذا(plugin == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_محفوظ(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		صحيح event,
		MOSQ_FUNC_generic_callback cb_func,
		ثابت فراغ *event_معلومات)
{
	بنية plugin_own_callback *own, *own_tmp;

	إذا(identifier == عدم || cb_func == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(event == MOSQ_EVT_CONTROL){
		أرجع control__unregister_callback(identifier, cb_func, event_معلومات);
	}

	DL_FOREACH_محفوظ(identifier->own_callbacks, own, own_tmp){
		إذا(own->event == (تعداد mosquitto_plugin_event)event && own->cb_func == cb_func){
			أرجع remove_callback(identifier, own);
		}
	}

	أرجع MOSQ_ERR_NOT_مليو;
}
