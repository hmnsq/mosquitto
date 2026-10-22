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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "utlist.h"
#ಅನ್ತರ್ಭಾವ "lib_load.h"


ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *get_event_name(ಎಣಿಕೆ mosquitto_plugin_event event)
{
	ಆಯ್ಕೆ(event){
		ಸ್ಥಿತಿ MOSQ_EVT_RELOAD:
			ಹಿಂತಿರುಗಿ "reload";
		ಸ್ಥಿತಿ MOSQ_EVT_ACL_CHECK:
			ಹಿಂತಿರುಗಿ "acl-check";
		ಸ್ಥಿತಿ MOSQ_EVT_BASIC_AUTH:
			ಹಿಂತಿರುಗಿ "basic-auth";
		ಸ್ಥಿತಿ MOSQ_EVT_PSK_ಕೀಲಿ:
			ಹಿಂತಿರುಗಿ "psk-key";
		ಸ್ಥಿತಿ MOSQ_EVT_EXT_AUTH_ಆರಂಭ:
			ಹಿಂತಿರುಗಿ "auth-start";
		ಸ್ಥಿತಿ MOSQ_EVT_EXT_AUTH_CONTINUE:
			ಹಿಂತಿರುಗಿ "auth-continue";
		ಸ್ಥಿತಿ MOSQ_EVT_CONTROL:
			ಹಿಂತಿರುಗಿ "control";
		ಸ್ಥಿತಿ MOSQ_EVT_ಸಂದೇಶ_ಒಳ:
			ಹಿಂತಿರುಗಿ "message-in";
		ಸ್ಥಿತಿ MOSQ_EVT_ಸಂದೇಶ_ಹೊರಹೋಗು:
			ಹಿಂತಿರುಗಿ "message-out";
		ಸ್ಥಿತಿ MOSQ_EVT_TICK:
			ಹಿಂತಿರುಗಿ "tick";
		ಸ್ಥಿತಿ MOSQ_EVT_DISCONNECT:
			ಹಿಂತಿರುಗಿ "disconnect";
		ಸ್ಥಿತಿ MOSQ_EVT_CONNECT:
			ಹಿಂತಿರುಗಿ "connect";
		ಸ್ಥಿತಿ MOSQ_EVT_CLIENT_OFFLINE:
			ಹಿಂತಿರುಗಿ "connect";
		ಸ್ಥಿತಿ MOSQ_EVT_SUBSCRIBE:
			ಹಿಂತಿರುಗಿ "subscribe";
		ಸ್ಥಿತಿ MOSQ_EVT_UNSUBSCRIBE:
			ಹಿಂತಿರುಗಿ "unsubscribe";
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_RESTORE:
			ಹಿಂತಿರುಗಿ "persist-restore";
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_ತಳ_ಸಂದೇಶ_ಕೂಡಿಸು:
			ಹಿಂತಿರುಗಿ "persist-base-msg-add";
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_ತಳ_ಸಂದೇಶ_DELETE:
			ಹಿಂತಿರುಗಿ "persist-base-msg-delete";
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_RETAIN_ಸಂದೇಶ_ಗಣ:
			ಹಿಂತಿರುಗಿ "persist-retain-msg-set";
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_RETAIN_ಸಂದೇಶ_DELETE:
			ಹಿಂತಿರುಗಿ "persist-retain-msg-delete";
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_CLIENT_ಕೂಡಿಸು:
			ಹಿಂತಿರುಗಿ "persist-client-add";
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_CLIENT_DELETE:
			ಹಿಂತಿರುಗಿ "persist-client-delete";
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			ಹಿಂತಿರುಗಿ "persist-client-update";
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_SUBSCRIPTION_ಕೂಡಿಸು:
			ಹಿಂತಿರುಗಿ "persist-subscription-add";
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			ಹಿಂತಿರುಗಿ "persist-subscription-delete";
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_ಕೂಡಿಸು:
			ಹಿಂತಿರುಗಿ "persist-client-msg-add";
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_DELETE:
			ಹಿಂತಿರುಗಿ "persist-client-msg-delete";
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_UPDATE:
			ಹಿಂತಿರುಗಿ "persist-client-msg-update";
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_WILL_ಕೂಡಿಸು:
			ಹಿಂತಿರುಗಿ "persist-will-add";
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_WILL_DELETE:
			ಹಿಂತಿರುಗಿ "persist-will-delete";
	}
	ಹಿಂತಿರುಗಿ "";
}


ಸ್ಥಿರ bool check_callback_exists(ರಚನೆ mosquitto__callback *cb_ತಳ, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	ರಚನೆ mosquitto__callback *ಬಾಲ, *tmp;

	DL_FOREACH_ಸುರಕ್ಷಿತ(cb_ತಳ, ಬಾಲ, tmp){
		ಆದರೆ(ಬಾಲ->identifier == identifier && ಬಾಲ->cb == cb_func){
			ಹಿಂತಿರುಗಿ true;
		}
	}
	ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ ರಚನೆ mosquitto__callback **plugin_get_callback_ತಳ(ರಚನೆ mosquitto__security_options *security_options, ಎಣಿಕೆ mosquitto_plugin_event event)
{
	ಆಯ್ಕೆ(event){
		ಸ್ಥಿತಿ MOSQ_EVT_RELOAD:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.reload;
		ಸ್ಥಿತಿ MOSQ_EVT_ACL_CHECK:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.acl_check;
		ಸ್ಥಿತಿ MOSQ_EVT_BASIC_AUTH:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.basic_auth;
		ಸ್ಥಿತಿ MOSQ_EVT_PSK_ಕೀಲಿ:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.psk_ಕೀಲಿ;
		ಸ್ಥಿತಿ MOSQ_EVT_EXT_AUTH_ಆರಂಭ:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.ext_auth_ಆರಂಭ;
		ಸ್ಥಿತಿ MOSQ_EVT_EXT_AUTH_CONTINUE:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.ext_auth_continue;
		ಸ್ಥಿತಿ MOSQ_EVT_CONTROL:
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		ಸ್ಥಿತಿ MOSQ_EVT_ಸಂದೇಶ_ಒಳ: /* same as MOSQ_EVT_MESSAGE */
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.ಸಂದೇಶ_ಒಳ;
		ಸ್ಥಿತಿ MOSQ_EVT_TICK:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.tick;
		ಸ್ಥಿತಿ MOSQ_EVT_DISCONNECT:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.disconnect;
		ಸ್ಥಿತಿ MOSQ_EVT_CONNECT:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.connect;
		ಸ್ಥಿತಿ MOSQ_EVT_CLIENT_OFFLINE:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.client_offline;
		ಸ್ಥಿತಿ MOSQ_EVT_SUBSCRIBE:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.subscribe;
		ಸ್ಥಿತಿ MOSQ_EVT_UNSUBSCRIBE:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.unsubscribe;
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_RESTORE:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.persist_restore;
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_CLIENT_ಕೂಡಿಸು:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.persist_client_ಕೂಡಿಸು;
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_CLIENT_DELETE:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.persist_client_delete;
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.persist_client_update;
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_SUBSCRIPTION_ಕೂಡಿಸು:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.persist_subscription_ಕೂಡಿಸು;
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.persist_subscription_delete;
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_ಕೂಡಿಸು:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.persist_client_ಸಂದೇಶ_ಕೂಡಿಸು;
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_DELETE:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.persist_client_ಸಂದೇಶ_delete;
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_UPDATE:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.persist_client_ಸಂದೇಶ_update;
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_ತಳ_ಸಂದೇಶ_ಕೂಡಿಸು:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.persist_ತಳ_ಸಂದೇಶ_ಕೂಡಿಸು;
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_ತಳ_ಸಂದೇಶ_DELETE:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.persist_ತಳ_ಸಂದೇಶ_delete;
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_RETAIN_ಸಂದೇಶ_ಗಣ:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.persist_retain_ಸಂದೇಶ_ಗಣ;
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_RETAIN_ಸಂದೇಶ_DELETE:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.persist_retain_ಸಂದೇಶ_delete;
		ಸ್ಥಿತಿ MOSQ_EVT_ಸಂದೇಶ_ಹೊರಹೋಗು:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.ಸಂದೇಶ_ಹೊರಹೋಗು;
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_WILL_ಕೂಡಿಸು:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.persist_will_ಕೂಡಿಸು;
		ಸ್ಥಿತಿ MOSQ_EVT_PERSIST_WILL_DELETE:
			ಹಿಂತಿರುಗಿ &security_options->plugin_callbacks.persist_will_delete;
	}
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ remove_callback(mosquitto_plugin_id_t *plugin, ರಚನೆ plugin_own_callback *own)
{
	ರಚನೆ mosquitto__security_options *security_options;
	ರಚನೆ mosquitto__callback *ಬಾಲ, *tmp;
	ರಚನೆ mosquitto__callback **cb_ತಳ = ಖಾಲಿ;

	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<plugin->config.security_option_ಎಣಿಕೆ; i++){
		security_options = plugin->config.security_options[i];

		cb_ತಳ = plugin_get_callback_ತಳ(security_options, own->event);
		ಆದರೆ(cb_ತಳ == ಖಾಲಿ){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_ಸುರಕ್ಷಿತ(*cb_ತಳ, ಬಾಲ, tmp){
			ಆದರೆ(ಬಾಲ->identifier == plugin && ಬಾಲ->cb == own->cb_func){
				DL_DELETE(*cb_ತಳ, ಬಾಲ);
				mosquitto_FREE(ಬಾಲ);
				ನಿಲ್ಲಿಸು;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_callback_ರಿಜಿಸ್ಟರ್(
		mosquitto_plugin_id_t *identifier,
		ಪೂರ್ಣಾಂಕ event,
		MOSQ_FUNC_generic_callback cb_func,
		ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *event_ದತ್ತಾಂಶ,
		ಶೂನ್ಯ *userdata)
{
	ರಚನೆ mosquitto__callback **cb_ತಳ = ಖಾಲಿ, *cb_new;
	ರಚನೆ mosquitto__security_options *security_options;
	ರಚನೆ plugin_own_callback *own_callback;

	ಆದರೆ(cb_func == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	ಆದರೆ(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
			|| event == MOSQ_EVT_PERSIST_ತಳ_ಸಂದೇಶ_ಕೂಡಿಸು
			|| event == MOSQ_EVT_PERSIST_ತಳ_ಸಂದೇಶ_DELETE
			|| event == MOSQ_EVT_PERSIST_RETAIN_ಸಂದೇಶ_ಗಣ
			|| event == MOSQ_EVT_PERSIST_RETAIN_ಸಂದೇಶ_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ಕೂಡಿಸು
			|| event == MOSQ_EVT_PERSIST_CLIENT_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_UPDATE
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_ಕೂಡಿಸು
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_ಕೂಡಿಸು
			|| event == MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_UPDATE
			|| event == MOSQ_EVT_PERSIST_WILL_ಕೂಡಿಸು
			|| event == MOSQ_EVT_PERSIST_WILL_DELETE
			)){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	ಆದರೆ(event == MOSQ_EVT_CONTROL){
		ಹಿಂತಿರುಗಿ control_ರಿಜಿಸ್ಟರ್_callback(identifier, cb_func, event_ದತ್ತಾಂಶ, userdata);
	}

	own_callback = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ plugin_own_callback));
	ಆದರೆ(own_callback == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	own_callback->event = (ಎಣಿಕೆ mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	ಆದರೆ(identifier->config.security_option_ಎಣಿಕೆ == 0){
		log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((ಎಣಿಕೆ mosquitto_plugin_event)event));
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<identifier->config.security_option_ಎಣಿಕೆ; i++){
		security_options = identifier->config.security_options[i];

		cb_ತಳ = plugin_get_callback_ತಳ(security_options, (ಎಣಿಕೆ mosquitto_plugin_event)event);
		ಆದರೆ(cb_ತಳ == ಖಾಲಿ){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_SUPPORTED;
		}

		ಆದರೆ(check_callback_exists(*cb_ತಳ, identifier, cb_func)){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto__callback));
		ಆದರೆ(cb_new == ಖಾಲಿ){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_ತಳ, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	ಆದರೆ(identifier->plugin_name){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((ಎಣಿಕೆ mosquitto_plugin_event)event));
	}ಇಲ್ಲದಿದ್ದರೆ{
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((ಎಣಿಕೆ mosquitto_plugin_event)event));
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	ರಚನೆ plugin_own_callback *own, *own_tmp;

	ಆದರೆ(plugin == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_ಸುರಕ್ಷಿತ(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		ಪೂರ್ಣಾಂಕ event,
		MOSQ_FUNC_generic_callback cb_func,
		ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *event_ದತ್ತಾಂಶ)
{
	ರಚನೆ plugin_own_callback *own, *own_tmp;

	ಆದರೆ(identifier == ಖಾಲಿ || cb_func == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	ಆದರೆ(event == MOSQ_EVT_CONTROL){
		ಹಿಂತಿರುಗಿ control__unregister_callback(identifier, cb_func, event_ದತ್ತಾಂಶ);
	}

	DL_FOREACH_ಸುರಕ್ಷಿತ(identifier->own_callbacks, own, own_tmp){
		ಆದರೆ(own->event == (ಎಣಿಕೆ mosquitto_plugin_event)event && own->cb_func == cb_func){
			ಹಿಂತಿರುಗಿ remove_callback(identifier, own);
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_NOT_ಸಿಕ್ಕಿತು;
}
