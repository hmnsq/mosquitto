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


ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *get_event_name(ಗಣನಾ mosquitto_plugin_event event)
{
	ವಿಕಲ್ಪ(event){
		ಪಕ್ಷ MOSQ_EVT_RELOAD:
			ಪ್ರತ್ಯಾಗಮ "reload";
		ಪಕ್ಷ MOSQ_EVT_ACL_CHECK:
			ಪ್ರತ್ಯಾಗಮ "acl-check";
		ಪಕ್ಷ MOSQ_EVT_BASIC_AUTH:
			ಪ್ರತ್ಯಾಗಮ "basic-auth";
		ಪಕ್ಷ MOSQ_EVT_PSK_ಕೀಲಿ:
			ಪ್ರತ್ಯಾಗಮ "psk-key";
		ಪಕ್ಷ MOSQ_EVT_EXT_AUTH_START:
			ಪ್ರತ್ಯಾಗಮ "auth-start";
		ಪಕ್ಷ MOSQ_EVT_EXT_AUTH_CONTINUE:
			ಪ್ರತ್ಯಾಗಮ "auth-continue";
		ಪಕ್ಷ MOSQ_EVT_CONTROL:
			ಪ್ರತ್ಯಾಗಮ "control";
		ಪಕ್ಷ MOSQ_EVT_MESSAGE_IN:
			ಪ್ರತ್ಯಾಗಮ "message-in";
		ಪಕ್ಷ MOSQ_EVT_MESSAGE_OUT:
			ಪ್ರತ್ಯಾಗಮ "message-out";
		ಪಕ್ಷ MOSQ_EVT_TICK:
			ಪ್ರತ್ಯಾಗಮ "tick";
		ಪಕ್ಷ MOSQ_EVT_DISCONNECT:
			ಪ್ರತ್ಯಾಗಮ "disconnect";
		ಪಕ್ಷ MOSQ_EVT_CONNECT:
			ಪ್ರತ್ಯಾಗಮ "connect";
		ಪಕ್ಷ MOSQ_EVT_CLIENT_OFFLINE:
			ಪ್ರತ್ಯಾಗಮ "connect";
		ಪಕ್ಷ MOSQ_EVT_SUBSCRIBE:
			ಪ್ರತ್ಯಾಗಮ "subscribe";
		ಪಕ್ಷ MOSQ_EVT_UNSUBSCRIBE:
			ಪ್ರತ್ಯಾಗಮ "unsubscribe";
		ಪಕ್ಷ MOSQ_EVT_PERSIST_RESTORE:
			ಪ್ರತ್ಯಾಗಮ "persist-restore";
		ಪಕ್ಷ MOSQ_EVT_PERSIST_BASE_ಸಂದೇಶ_ADD:
			ಪ್ರತ್ಯಾಗಮ "persist-base-msg-add";
		ಪಕ್ಷ MOSQ_EVT_PERSIST_BASE_ಸಂದೇಶ_DELETE:
			ಪ್ರತ್ಯಾಗಮ "persist-base-msg-delete";
		ಪಕ್ಷ MOSQ_EVT_PERSIST_RETAIN_ಸಂದೇಶ_SET:
			ಪ್ರತ್ಯಾಗಮ "persist-retain-msg-set";
		ಪಕ್ಷ MOSQ_EVT_PERSIST_RETAIN_ಸಂದೇಶ_DELETE:
			ಪ್ರತ್ಯಾಗಮ "persist-retain-msg-delete";
		ಪಕ್ಷ MOSQ_EVT_PERSIST_CLIENT_ADD:
			ಪ್ರತ್ಯಾಗಮ "persist-client-add";
		ಪಕ್ಷ MOSQ_EVT_PERSIST_CLIENT_DELETE:
			ಪ್ರತ್ಯಾಗಮ "persist-client-delete";
		ಪಕ್ಷ MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			ಪ್ರತ್ಯಾಗಮ "persist-client-update";
		ಪಕ್ಷ MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			ಪ್ರತ್ಯಾಗಮ "persist-subscription-add";
		ಪಕ್ಷ MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			ಪ್ರತ್ಯಾಗಮ "persist-subscription-delete";
		ಪಕ್ಷ MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_ADD:
			ಪ್ರತ್ಯಾಗಮ "persist-client-msg-add";
		ಪಕ್ಷ MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_DELETE:
			ಪ್ರತ್ಯಾಗಮ "persist-client-msg-delete";
		ಪಕ್ಷ MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_UPDATE:
			ಪ್ರತ್ಯಾಗಮ "persist-client-msg-update";
		ಪಕ್ಷ MOSQ_EVT_PERSIST_WILL_ADD:
			ಪ್ರತ್ಯಾಗಮ "persist-will-add";
		ಪಕ್ಷ MOSQ_EVT_PERSIST_WILL_DELETE:
			ಪ್ರತ್ಯಾಗಮ "persist-will-delete";
	}
	ಪ್ರತ್ಯಾಗಮ "";
}


ಸ್ಥಿರ bool check_callback_exists(ಸಂರಚನಾ mosquitto__callback *cb_base, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	ಸಂರಚನಾ mosquitto__callback *tail, *tmp;

	DL_FOREACH_SAFE(cb_base, tail, tmp){
		ಯದಿ(tail->identifier == identifier && tail->cb == cb_func){
			ಪ್ರತ್ಯಾಗಮ true;
		}
	}
	ಪ್ರತ್ಯಾಗಮ false;
}

ಸ್ಥಿರ ಸಂರಚನಾ mosquitto__callback **plugin__get_callback_base(ಸಂರಚನಾ mosquitto__security_options *security_options, ಗಣನಾ mosquitto_plugin_event event)
{
	ವಿಕಲ್ಪ(event){
		ಪಕ್ಷ MOSQ_EVT_RELOAD:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.reload;
		ಪಕ್ಷ MOSQ_EVT_ACL_CHECK:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.acl_check;
		ಪಕ್ಷ MOSQ_EVT_BASIC_AUTH:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.basic_auth;
		ಪಕ್ಷ MOSQ_EVT_PSK_ಕೀಲಿ:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.psk_ಕೀಲಿ;
		ಪಕ್ಷ MOSQ_EVT_EXT_AUTH_START:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.ext_auth_start;
		ಪಕ್ಷ MOSQ_EVT_EXT_AUTH_CONTINUE:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.ext_auth_continue;
		ಪಕ್ಷ MOSQ_EVT_CONTROL:
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		ಪಕ್ಷ MOSQ_EVT_MESSAGE_IN: /* same as MOSQ_EVT_MESSAGE */
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.message_in;
		ಪಕ್ಷ MOSQ_EVT_TICK:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.tick;
		ಪಕ್ಷ MOSQ_EVT_DISCONNECT:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.disconnect;
		ಪಕ್ಷ MOSQ_EVT_CONNECT:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.connect;
		ಪಕ್ಷ MOSQ_EVT_CLIENT_OFFLINE:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.client_offline;
		ಪಕ್ಷ MOSQ_EVT_SUBSCRIBE:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.subscribe;
		ಪಕ್ಷ MOSQ_EVT_UNSUBSCRIBE:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.unsubscribe;
		ಪಕ್ಷ MOSQ_EVT_PERSIST_RESTORE:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.persist_restore;
		ಪಕ್ಷ MOSQ_EVT_PERSIST_CLIENT_ADD:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.persist_client_add;
		ಪಕ್ಷ MOSQ_EVT_PERSIST_CLIENT_DELETE:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.persist_client_delete;
		ಪಕ್ಷ MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.persist_client_update;
		ಪಕ್ಷ MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.persist_subscription_add;
		ಪಕ್ಷ MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.persist_subscription_delete;
		ಪಕ್ಷ MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_ADD:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.persist_client_ಸಂದೇಶ_add;
		ಪಕ್ಷ MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_DELETE:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.persist_client_ಸಂದೇಶ_delete;
		ಪಕ್ಷ MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_UPDATE:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.persist_client_ಸಂದೇಶ_update;
		ಪಕ್ಷ MOSQ_EVT_PERSIST_BASE_ಸಂದೇಶ_ADD:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.persist_base_ಸಂದೇಶ_add;
		ಪಕ್ಷ MOSQ_EVT_PERSIST_BASE_ಸಂದೇಶ_DELETE:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.persist_base_ಸಂದೇಶ_delete;
		ಪಕ್ಷ MOSQ_EVT_PERSIST_RETAIN_ಸಂದೇಶ_SET:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.persist_retain_ಸಂದೇಶ_set;
		ಪಕ್ಷ MOSQ_EVT_PERSIST_RETAIN_ಸಂದೇಶ_DELETE:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.persist_retain_ಸಂದೇಶ_delete;
		ಪಕ್ಷ MOSQ_EVT_MESSAGE_OUT:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.message_out;
		ಪಕ್ಷ MOSQ_EVT_PERSIST_WILL_ADD:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.persist_will_add;
		ಪಕ್ಷ MOSQ_EVT_PERSIST_WILL_DELETE:
			ಪ್ರತ್ಯಾಗಮ &security_options->plugin_callbacks.persist_will_delete;
	}
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ remove_callback(mosquitto_plugin_id_t *plugin, ಸಂರಚನಾ plugin_own_callback *own)
{
	ಸಂರಚನಾ mosquitto__security_options *security_options;
	ಸಂರಚನಾ mosquitto__callback *tail, *tmp;
	ಸಂರಚನಾ mosquitto__callback **cb_base = ರಿಕ್ತ;

	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<plugin->config.security_option_ಎಣಿಕೆ; i++){
		security_options = plugin->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, own->event);
		ಯದಿ(cb_base == ರಿಕ್ತ){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_SAFE(*cb_base, tail, tmp){
			ಯದಿ(tail->identifier == plugin && tail->cb == own->cb_func){
				DL_DELETE(*cb_base, tail);
				mosquitto_FREE(tail);
				ಭಙ್ಗ;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_callback_register(
		mosquitto_plugin_id_t *identifier,
		ಪೂರ್ಣಾಙ್ಕ event,
		MOSQ_FUNC_generic_callback cb_func,
		ಧ್ರುವ ಶೂನ್ಯ *event_data,
		ಶೂನ್ಯ *userdata)
{
	ಸಂರಚನಾ mosquitto__callback **cb_base = ರಿಕ್ತ, *cb_new;
	ಸಂರಚನಾ mosquitto__security_options *security_options;
	ಸಂರಚನಾ plugin_own_callback *own_callback;

	ಯದಿ(cb_func == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	ಯದಿ(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
			|| event == MOSQ_EVT_PERSIST_BASE_ಸಂದೇಶ_ADD
			|| event == MOSQ_EVT_PERSIST_BASE_ಸಂದೇಶ_DELETE
			|| event == MOSQ_EVT_PERSIST_RETAIN_ಸಂದೇಶ_SET
			|| event == MOSQ_EVT_PERSIST_RETAIN_ಸಂದೇಶ_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_UPDATE
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ಸಂದೇಶ_UPDATE
			|| event == MOSQ_EVT_PERSIST_WILL_ADD
			|| event == MOSQ_EVT_PERSIST_WILL_DELETE
			)){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	ಯದಿ(event == MOSQ_EVT_CONTROL){
		ಪ್ರತ್ಯಾಗಮ control__register_callback(identifier, cb_func, event_data, userdata);
	}

	own_callback = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ plugin_own_callback));
	ಯದಿ(own_callback == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}
	own_callback->event = (ಗಣನಾ mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	ಯದಿ(identifier->config.security_option_ಎಣಿಕೆ == 0){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((ಗಣನಾ mosquitto_plugin_event)event));
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<identifier->config.security_option_ಎಣಿಕೆ; i++){
		security_options = identifier->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, (ಗಣನಾ mosquitto_plugin_event)event);
		ಯದಿ(cb_base == ರಿಕ್ತ){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_SUPPORTED;
		}

		ಯದಿ(check_callback_exists(*cb_base, identifier, cb_func)){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__callback));
		ಯದಿ(cb_new == ರಿಕ್ತ){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_base, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	ಯದಿ(identifier->plugin_name){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((ಗಣನಾ mosquitto_plugin_event)event));
	}ಅನ್ಯಥಾ{
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((ಗಣನಾ mosquitto_plugin_event)event));
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	ಸಂರಚನಾ plugin_own_callback *own, *own_tmp;

	ಯದಿ(plugin == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_SAFE(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		ಪೂರ್ಣಾಙ್ಕ event,
		MOSQ_FUNC_generic_callback cb_func,
		ಧ್ರುವ ಶೂನ್ಯ *event_data)
{
	ಸಂರಚನಾ plugin_own_callback *own, *own_tmp;

	ಯದಿ(identifier == ರಿಕ್ತ || cb_func == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	ಯದಿ(event == MOSQ_EVT_CONTROL){
		ಪ್ರತ್ಯಾಗಮ control__unregister_callback(identifier, cb_func, event_data);
	}

	DL_FOREACH_SAFE(identifier->own_callbacks, own, own_tmp){
		ಯದಿ(own->event == (ಗಣನಾ mosquitto_plugin_event)event && own->cb_func == cb_func){
			ಪ್ರತ್ಯಾಗಮ remove_callback(identifier, own);
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOT_FOUND;
}
