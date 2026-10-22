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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utlist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "lib_load.h"


ਸਥਿਰ ਅਚਲ ਅੱਖਰ *get_event_name(ਗਿਣਤੀ mosquitto_plugin_event event)
{
	ਚੋਣ(event){
		ਹਾਲਤ MOSQ_EVT_RELOAD:
			ਵਾਪਸ "reload";
		ਹਾਲਤ MOSQ_EVT_ACL_CHECK:
			ਵਾਪਸ "acl-check";
		ਹਾਲਤ MOSQ_EVT_BASIC_AUTH:
			ਵਾਪਸ "basic-auth";
		ਹਾਲਤ MOSQ_EVT_PSK_ਕੁੰਜੀ:
			ਵਾਪਸ "psk-key";
		ਹਾਲਤ MOSQ_EVT_EXT_AUTH_ਸ਼ੁਰੂ:
			ਵਾਪਸ "auth-start";
		ਹਾਲਤ MOSQ_EVT_EXT_AUTH_CONTINUE:
			ਵਾਪਸ "auth-continue";
		ਹਾਲਤ MOSQ_EVT_CONTROL:
			ਵਾਪਸ "control";
		ਹਾਲਤ MOSQ_EVT_ਸੁਨੇਹਾ_ਵਿੱਚ:
			ਵਾਪਸ "message-in";
		ਹਾਲਤ MOSQ_EVT_ਸੁਨੇਹਾ_ਬਾਹਰਲਾ:
			ਵਾਪਸ "message-out";
		ਹਾਲਤ MOSQ_EVT_TICK:
			ਵਾਪਸ "tick";
		ਹਾਲਤ MOSQ_EVT_DISCONNECT:
			ਵਾਪਸ "disconnect";
		ਹਾਲਤ MOSQ_EVT_CONNECT:
			ਵਾਪਸ "connect";
		ਹਾਲਤ MOSQ_EVT_CLIENT_OFFLINE:
			ਵਾਪਸ "connect";
		ਹਾਲਤ MOSQ_EVT_SUBSCRIBE:
			ਵਾਪਸ "subscribe";
		ਹਾਲਤ MOSQ_EVT_UNSUBSCRIBE:
			ਵਾਪਸ "unsubscribe";
		ਹਾਲਤ MOSQ_EVT_PERSIST_RESTORE:
			ਵਾਪਸ "persist-restore";
		ਹਾਲਤ MOSQ_EVT_PERSIST_ਅਧਾਰ_ਸੁਨੇਹਾ_ਜੋੜੋ:
			ਵਾਪਸ "persist-base-msg-add";
		ਹਾਲਤ MOSQ_EVT_PERSIST_ਅਧਾਰ_ਸੁਨੇਹਾ_DELETE:
			ਵਾਪਸ "persist-base-msg-delete";
		ਹਾਲਤ MOSQ_EVT_PERSIST_RETAIN_ਸੁਨੇਹਾ_ਸੈੱਟ:
			ਵਾਪਸ "persist-retain-msg-set";
		ਹਾਲਤ MOSQ_EVT_PERSIST_RETAIN_ਸੁਨੇਹਾ_DELETE:
			ਵਾਪਸ "persist-retain-msg-delete";
		ਹਾਲਤ MOSQ_EVT_PERSIST_CLIENT_ਜੋੜੋ:
			ਵਾਪਸ "persist-client-add";
		ਹਾਲਤ MOSQ_EVT_PERSIST_CLIENT_DELETE:
			ਵਾਪਸ "persist-client-delete";
		ਹਾਲਤ MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			ਵਾਪਸ "persist-client-update";
		ਹਾਲਤ MOSQ_EVT_PERSIST_SUBSCRIPTION_ਜੋੜੋ:
			ਵਾਪਸ "persist-subscription-add";
		ਹਾਲਤ MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			ਵਾਪਸ "persist-subscription-delete";
		ਹਾਲਤ MOSQ_EVT_PERSIST_CLIENT_ਸੁਨੇਹਾ_ਜੋੜੋ:
			ਵਾਪਸ "persist-client-msg-add";
		ਹਾਲਤ MOSQ_EVT_PERSIST_CLIENT_ਸੁਨੇਹਾ_DELETE:
			ਵਾਪਸ "persist-client-msg-delete";
		ਹਾਲਤ MOSQ_EVT_PERSIST_CLIENT_ਸੁਨੇਹਾ_UPDATE:
			ਵਾਪਸ "persist-client-msg-update";
		ਹਾਲਤ MOSQ_EVT_PERSIST_WILL_ਜੋੜੋ:
			ਵਾਪਸ "persist-will-add";
		ਹਾਲਤ MOSQ_EVT_PERSIST_WILL_DELETE:
			ਵਾਪਸ "persist-will-delete";
	}
	ਵਾਪਸ "";
}


ਸਥਿਰ bool check_callback_exists(ਬਣਤਰ mosquitto__callback *cb_ਅਧਾਰ, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	ਬਣਤਰ mosquitto__callback *ਪੂਛ, *tmp;

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(cb_ਅਧਾਰ, ਪੂਛ, tmp){
		ਜੇ(ਪੂਛ->identifier == identifier && ਪੂਛ->cb == cb_func){
			ਵਾਪਸ true;
		}
	}
	ਵਾਪਸ false;
}

ਸਥਿਰ ਬਣਤਰ mosquitto__callback **plugin_get_callback_ਅਧਾਰ(ਬਣਤਰ mosquitto__security_options *security_options, ਗਿਣਤੀ mosquitto_plugin_event event)
{
	ਚੋਣ(event){
		ਹਾਲਤ MOSQ_EVT_RELOAD:
			ਵਾਪਸ &security_options->plugin_callbacks.reload;
		ਹਾਲਤ MOSQ_EVT_ACL_CHECK:
			ਵਾਪਸ &security_options->plugin_callbacks.acl_check;
		ਹਾਲਤ MOSQ_EVT_BASIC_AUTH:
			ਵਾਪਸ &security_options->plugin_callbacks.basic_auth;
		ਹਾਲਤ MOSQ_EVT_PSK_ਕੁੰਜੀ:
			ਵਾਪਸ &security_options->plugin_callbacks.psk_ਕੁੰਜੀ;
		ਹਾਲਤ MOSQ_EVT_EXT_AUTH_ਸ਼ੁਰੂ:
			ਵਾਪਸ &security_options->plugin_callbacks.ext_auth_ਸ਼ੁਰੂ;
		ਹਾਲਤ MOSQ_EVT_EXT_AUTH_CONTINUE:
			ਵਾਪਸ &security_options->plugin_callbacks.ext_auth_continue;
		ਹਾਲਤ MOSQ_EVT_CONTROL:
			ਵਾਪਸ ਨਿਰਾ;
		ਹਾਲਤ MOSQ_EVT_ਸੁਨੇਹਾ_ਵਿੱਚ: /* same as MOSQ_EVT_MESSAGE */
			ਵਾਪਸ &security_options->plugin_callbacks.ਸੁਨੇਹਾ_ਵਿੱਚ;
		ਹਾਲਤ MOSQ_EVT_TICK:
			ਵਾਪਸ &security_options->plugin_callbacks.tick;
		ਹਾਲਤ MOSQ_EVT_DISCONNECT:
			ਵਾਪਸ &security_options->plugin_callbacks.disconnect;
		ਹਾਲਤ MOSQ_EVT_CONNECT:
			ਵਾਪਸ &security_options->plugin_callbacks.connect;
		ਹਾਲਤ MOSQ_EVT_CLIENT_OFFLINE:
			ਵਾਪਸ &security_options->plugin_callbacks.client_offline;
		ਹਾਲਤ MOSQ_EVT_SUBSCRIBE:
			ਵਾਪਸ &security_options->plugin_callbacks.subscribe;
		ਹਾਲਤ MOSQ_EVT_UNSUBSCRIBE:
			ਵਾਪਸ &security_options->plugin_callbacks.unsubscribe;
		ਹਾਲਤ MOSQ_EVT_PERSIST_RESTORE:
			ਵਾਪਸ &security_options->plugin_callbacks.persist_restore;
		ਹਾਲਤ MOSQ_EVT_PERSIST_CLIENT_ਜੋੜੋ:
			ਵਾਪਸ &security_options->plugin_callbacks.persist_client_ਜੋੜੋ;
		ਹਾਲਤ MOSQ_EVT_PERSIST_CLIENT_DELETE:
			ਵਾਪਸ &security_options->plugin_callbacks.persist_client_delete;
		ਹਾਲਤ MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			ਵਾਪਸ &security_options->plugin_callbacks.persist_client_update;
		ਹਾਲਤ MOSQ_EVT_PERSIST_SUBSCRIPTION_ਜੋੜੋ:
			ਵਾਪਸ &security_options->plugin_callbacks.persist_subscription_ਜੋੜੋ;
		ਹਾਲਤ MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			ਵਾਪਸ &security_options->plugin_callbacks.persist_subscription_delete;
		ਹਾਲਤ MOSQ_EVT_PERSIST_CLIENT_ਸੁਨੇਹਾ_ਜੋੜੋ:
			ਵਾਪਸ &security_options->plugin_callbacks.persist_client_ਸੁਨੇਹਾ_ਜੋੜੋ;
		ਹਾਲਤ MOSQ_EVT_PERSIST_CLIENT_ਸੁਨੇਹਾ_DELETE:
			ਵਾਪਸ &security_options->plugin_callbacks.persist_client_ਸੁਨੇਹਾ_delete;
		ਹਾਲਤ MOSQ_EVT_PERSIST_CLIENT_ਸੁਨੇਹਾ_UPDATE:
			ਵਾਪਸ &security_options->plugin_callbacks.persist_client_ਸੁਨੇਹਾ_update;
		ਹਾਲਤ MOSQ_EVT_PERSIST_ਅਧਾਰ_ਸੁਨੇਹਾ_ਜੋੜੋ:
			ਵਾਪਸ &security_options->plugin_callbacks.persist_ਅਧਾਰ_ਸੁਨੇਹਾ_ਜੋੜੋ;
		ਹਾਲਤ MOSQ_EVT_PERSIST_ਅਧਾਰ_ਸੁਨੇਹਾ_DELETE:
			ਵਾਪਸ &security_options->plugin_callbacks.persist_ਅਧਾਰ_ਸੁਨੇਹਾ_delete;
		ਹਾਲਤ MOSQ_EVT_PERSIST_RETAIN_ਸੁਨੇਹਾ_ਸੈੱਟ:
			ਵਾਪਸ &security_options->plugin_callbacks.persist_retain_ਸੁਨੇਹਾ_ਸੈੱਟ;
		ਹਾਲਤ MOSQ_EVT_PERSIST_RETAIN_ਸੁਨੇਹਾ_DELETE:
			ਵਾਪਸ &security_options->plugin_callbacks.persist_retain_ਸੁਨੇਹਾ_delete;
		ਹਾਲਤ MOSQ_EVT_ਸੁਨੇਹਾ_ਬਾਹਰਲਾ:
			ਵਾਪਸ &security_options->plugin_callbacks.ਸੁਨੇਹਾ_ਬਾਹਰਲਾ;
		ਹਾਲਤ MOSQ_EVT_PERSIST_WILL_ਜੋੜੋ:
			ਵਾਪਸ &security_options->plugin_callbacks.persist_will_ਜੋੜੋ;
		ਹਾਲਤ MOSQ_EVT_PERSIST_WILL_DELETE:
			ਵਾਪਸ &security_options->plugin_callbacks.persist_will_delete;
	}
	ਵਾਪਸ ਨਿਰਾ;
}


ਸਥਿਰ ਪੂਰਨ remove_callback(mosquitto_plugin_id_t *plugin, ਬਣਤਰ plugin_own_callback *own)
{
	ਬਣਤਰ mosquitto__security_options *security_options;
	ਬਣਤਰ mosquitto__callback *ਪੂਛ, *tmp;
	ਬਣਤਰ mosquitto__callback **cb_ਅਧਾਰ = ਨਿਰਾ;

	ਲਈ(ਪੂਰਨ i=0; i<plugin->config.security_option_ਗਿਣਤੀ; i++){
		security_options = plugin->config.security_options[i];

		cb_ਅਧਾਰ = plugin_get_callback_ਅਧਾਰ(security_options, own->event);
		ਜੇ(cb_ਅਧਾਰ == ਨਿਰਾ){
			ਵਾਪਸ MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_ਸੁਰੱਖਿਅਤ(*cb_ਅਧਾਰ, ਪੂਛ, tmp){
			ਜੇ(ਪੂਛ->identifier == plugin && ਪੂਛ->cb == own->cb_func){
				DL_DELETE(*cb_ਅਧਾਰ, ਪੂਛ);
				mosquitto_FREE(ਪੂਛ);
				ਰੋਕੋ;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ਪੂਰਨ mosquitto_callback_ਰਜਿਸਟਰ(
		mosquitto_plugin_id_t *identifier,
		ਪੂਰਨ event,
		MOSQ_FUNC_generic_callback cb_func,
		ਅਚਲ ਖਾਲੀ *event_ਅੰਕੜੇ,
		ਖਾਲੀ *userdata)
{
	ਬਣਤਰ mosquitto__callback **cb_ਅਧਾਰ = ਨਿਰਾ, *cb_new;
	ਬਣਤਰ mosquitto__security_options *security_options;
	ਬਣਤਰ plugin_own_callback *own_callback;

	ਜੇ(cb_func == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਜੇ(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
			|| event == MOSQ_EVT_PERSIST_ਅਧਾਰ_ਸੁਨੇਹਾ_ਜੋੜੋ
			|| event == MOSQ_EVT_PERSIST_ਅਧਾਰ_ਸੁਨੇਹਾ_DELETE
			|| event == MOSQ_EVT_PERSIST_RETAIN_ਸੁਨੇਹਾ_ਸੈੱਟ
			|| event == MOSQ_EVT_PERSIST_RETAIN_ਸੁਨੇਹਾ_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ਜੋੜੋ
			|| event == MOSQ_EVT_PERSIST_CLIENT_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_UPDATE
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_ਜੋੜੋ
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ਸੁਨੇਹਾ_ਜੋੜੋ
			|| event == MOSQ_EVT_PERSIST_CLIENT_ਸੁਨੇਹਾ_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ਸੁਨੇਹਾ_UPDATE
			|| event == MOSQ_EVT_PERSIST_WILL_ਜੋੜੋ
			|| event == MOSQ_EVT_PERSIST_WILL_DELETE
			)){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਜੇ(event == MOSQ_EVT_CONTROL){
		ਵਾਪਸ control_ਰਜਿਸਟਰ_callback(identifier, cb_func, event_ਅੰਕੜੇ, userdata);
	}

	own_callback = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ plugin_own_callback));
	ਜੇ(own_callback == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}
	own_callback->event = (ਗਿਣਤੀ mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	ਜੇ(identifier->config.security_option_ਗਿਣਤੀ == 0){
		log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((ਗਿਣਤੀ mosquitto_plugin_event)event));
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਲਈ(ਪੂਰਨ i=0; i<identifier->config.security_option_ਗਿਣਤੀ; i++){
		security_options = identifier->config.security_options[i];

		cb_ਅਧਾਰ = plugin_get_callback_ਅਧਾਰ(security_options, (ਗਿਣਤੀ mosquitto_plugin_event)event);
		ਜੇ(cb_ਅਧਾਰ == ਨਿਰਾ){
			ਵਾਪਸ MOSQ_ERR_NOT_SUPPORTED;
		}

		ਜੇ(check_callback_exists(*cb_ਅਧਾਰ, identifier, cb_func)){
			ਵਾਪਸ MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto__callback));
		ਜੇ(cb_new == ਨਿਰਾ){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_ਅਧਾਰ, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	ਜੇ(identifier->plugin_name){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((ਗਿਣਤੀ mosquitto_plugin_event)event));
	}ਨਹੀਂਤਾਂ{
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((ਗਿਣਤੀ mosquitto_plugin_event)event));
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	ਬਣਤਰ plugin_own_callback *own, *own_tmp;

	ਜੇ(plugin == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ਪੂਰਨ mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		ਪੂਰਨ event,
		MOSQ_FUNC_generic_callback cb_func,
		ਅਚਲ ਖਾਲੀ *event_ਅੰਕੜੇ)
{
	ਬਣਤਰ plugin_own_callback *own, *own_tmp;

	ਜੇ(identifier == ਨਿਰਾ || cb_func == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਜੇ(event == MOSQ_EVT_CONTROL){
		ਵਾਪਸ control__unregister_callback(identifier, cb_func, event_ਅੰਕੜੇ);
	}

	DL_FOREACH_ਸੁਰੱਖਿਅਤ(identifier->own_callbacks, own, own_tmp){
		ਜੇ(own->event == (ਗਿਣਤੀ mosquitto_plugin_event)event && own->cb_func == cb_func){
			ਵਾਪਸ remove_callback(identifier, own);
		}
	}

	ਵਾਪਸ MOSQ_ERR_NOT_ਮਿਲਿਆ;
}
