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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "utlist.h"
#அந்தர்பாவ "lib_load.h"


நிலையான மாறிலி எழுத்து *get_event_name(எண்ணிக்கை mosquitto_plugin_event event)
{
	தேர்வு(event){
		நிலை MOSQ_EVT_RELOAD:
			திரும்பு "reload";
		நிலை MOSQ_EVT_ACL_CHECK:
			திரும்பு "acl-check";
		நிலை MOSQ_EVT_BASIC_AUTH:
			திரும்பு "basic-auth";
		நிலை MOSQ_EVT_PSK_சாவி:
			திரும்பு "psk-key";
		நிலை MOSQ_EVT_EXT_AUTH_START:
			திரும்பு "auth-start";
		நிலை MOSQ_EVT_EXT_AUTH_CONTINUE:
			திரும்பு "auth-continue";
		நிலை MOSQ_EVT_CONTROL:
			திரும்பு "control";
		நிலை MOSQ_EVT_MESSAGE_IN:
			திரும்பு "message-in";
		நிலை MOSQ_EVT_MESSAGE_OUT:
			திரும்பு "message-out";
		நிலை MOSQ_EVT_TICK:
			திரும்பு "tick";
		நிலை MOSQ_EVT_DISCONNECT:
			திரும்பு "disconnect";
		நிலை MOSQ_EVT_CONNECT:
			திரும்பு "connect";
		நிலை MOSQ_EVT_CLIENT_OFFLINE:
			திரும்பு "connect";
		நிலை MOSQ_EVT_SUBSCRIBE:
			திரும்பு "subscribe";
		நிலை MOSQ_EVT_UNSUBSCRIBE:
			திரும்பு "unsubscribe";
		நிலை MOSQ_EVT_PERSIST_RESTORE:
			திரும்பு "persist-restore";
		நிலை MOSQ_EVT_PERSIST_BASE_செய்தி_ADD:
			திரும்பு "persist-base-msg-add";
		நிலை MOSQ_EVT_PERSIST_BASE_செய்தி_DELETE:
			திரும்பு "persist-base-msg-delete";
		நிலை MOSQ_EVT_PERSIST_RETAIN_செய்தி_SET:
			திரும்பு "persist-retain-msg-set";
		நிலை MOSQ_EVT_PERSIST_RETAIN_செய்தி_DELETE:
			திரும்பு "persist-retain-msg-delete";
		நிலை MOSQ_EVT_PERSIST_CLIENT_ADD:
			திரும்பு "persist-client-add";
		நிலை MOSQ_EVT_PERSIST_CLIENT_DELETE:
			திரும்பு "persist-client-delete";
		நிலை MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			திரும்பு "persist-client-update";
		நிலை MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			திரும்பு "persist-subscription-add";
		நிலை MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			திரும்பு "persist-subscription-delete";
		நிலை MOSQ_EVT_PERSIST_CLIENT_செய்தி_ADD:
			திரும்பு "persist-client-msg-add";
		நிலை MOSQ_EVT_PERSIST_CLIENT_செய்தி_DELETE:
			திரும்பு "persist-client-msg-delete";
		நிலை MOSQ_EVT_PERSIST_CLIENT_செய்தி_UPDATE:
			திரும்பு "persist-client-msg-update";
		நிலை MOSQ_EVT_PERSIST_WILL_ADD:
			திரும்பு "persist-will-add";
		நிலை MOSQ_EVT_PERSIST_WILL_DELETE:
			திரும்பு "persist-will-delete";
	}
	திரும்பு "";
}


நிலையான bool check_callback_exists(கட்டமைப்பு mosquitto__callback *cb_base, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	கட்டமைப்பு mosquitto__callback *tail, *tmp;

	DL_FOREACH_SAFE(cb_base, tail, tmp){
		எனில்(tail->identifier == identifier && tail->cb == cb_func){
			திரும்பு true;
		}
	}
	திரும்பு false;
}

நிலையான கட்டமைப்பு mosquitto__callback **plugin__get_callback_base(கட்டமைப்பு mosquitto__security_options *security_options, எண்ணிக்கை mosquitto_plugin_event event)
{
	தேர்வு(event){
		நிலை MOSQ_EVT_RELOAD:
			திரும்பு &security_options->plugin_callbacks.reload;
		நிலை MOSQ_EVT_ACL_CHECK:
			திரும்பு &security_options->plugin_callbacks.acl_check;
		நிலை MOSQ_EVT_BASIC_AUTH:
			திரும்பு &security_options->plugin_callbacks.basic_auth;
		நிலை MOSQ_EVT_PSK_சாவி:
			திரும்பு &security_options->plugin_callbacks.psk_சாவி;
		நிலை MOSQ_EVT_EXT_AUTH_START:
			திரும்பு &security_options->plugin_callbacks.ext_auth_start;
		நிலை MOSQ_EVT_EXT_AUTH_CONTINUE:
			திரும்பு &security_options->plugin_callbacks.ext_auth_continue;
		நிலை MOSQ_EVT_CONTROL:
			திரும்பு வெற்று;
		நிலை MOSQ_EVT_MESSAGE_IN: /* same as MOSQ_EVT_MESSAGE */
			திரும்பு &security_options->plugin_callbacks.message_in;
		நிலை MOSQ_EVT_TICK:
			திரும்பு &security_options->plugin_callbacks.tick;
		நிலை MOSQ_EVT_DISCONNECT:
			திரும்பு &security_options->plugin_callbacks.disconnect;
		நிலை MOSQ_EVT_CONNECT:
			திரும்பு &security_options->plugin_callbacks.connect;
		நிலை MOSQ_EVT_CLIENT_OFFLINE:
			திரும்பு &security_options->plugin_callbacks.client_offline;
		நிலை MOSQ_EVT_SUBSCRIBE:
			திரும்பு &security_options->plugin_callbacks.subscribe;
		நிலை MOSQ_EVT_UNSUBSCRIBE:
			திரும்பு &security_options->plugin_callbacks.unsubscribe;
		நிலை MOSQ_EVT_PERSIST_RESTORE:
			திரும்பு &security_options->plugin_callbacks.persist_restore;
		நிலை MOSQ_EVT_PERSIST_CLIENT_ADD:
			திரும்பு &security_options->plugin_callbacks.persist_client_add;
		நிலை MOSQ_EVT_PERSIST_CLIENT_DELETE:
			திரும்பு &security_options->plugin_callbacks.persist_client_delete;
		நிலை MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			திரும்பு &security_options->plugin_callbacks.persist_client_update;
		நிலை MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			திரும்பு &security_options->plugin_callbacks.persist_subscription_add;
		நிலை MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			திரும்பு &security_options->plugin_callbacks.persist_subscription_delete;
		நிலை MOSQ_EVT_PERSIST_CLIENT_செய்தி_ADD:
			திரும்பு &security_options->plugin_callbacks.persist_client_செய்தி_add;
		நிலை MOSQ_EVT_PERSIST_CLIENT_செய்தி_DELETE:
			திரும்பு &security_options->plugin_callbacks.persist_client_செய்தி_delete;
		நிலை MOSQ_EVT_PERSIST_CLIENT_செய்தி_UPDATE:
			திரும்பு &security_options->plugin_callbacks.persist_client_செய்தி_update;
		நிலை MOSQ_EVT_PERSIST_BASE_செய்தி_ADD:
			திரும்பு &security_options->plugin_callbacks.persist_base_செய்தி_add;
		நிலை MOSQ_EVT_PERSIST_BASE_செய்தி_DELETE:
			திரும்பு &security_options->plugin_callbacks.persist_base_செய்தி_delete;
		நிலை MOSQ_EVT_PERSIST_RETAIN_செய்தி_SET:
			திரும்பு &security_options->plugin_callbacks.persist_retain_செய்தி_set;
		நிலை MOSQ_EVT_PERSIST_RETAIN_செய்தி_DELETE:
			திரும்பு &security_options->plugin_callbacks.persist_retain_செய்தி_delete;
		நிலை MOSQ_EVT_MESSAGE_OUT:
			திரும்பு &security_options->plugin_callbacks.message_out;
		நிலை MOSQ_EVT_PERSIST_WILL_ADD:
			திரும்பு &security_options->plugin_callbacks.persist_will_add;
		நிலை MOSQ_EVT_PERSIST_WILL_DELETE:
			திரும்பு &security_options->plugin_callbacks.persist_will_delete;
	}
	திரும்பு வெற்று;
}


நிலையான முழு remove_callback(mosquitto_plugin_id_t *plugin, கட்டமைப்பு plugin_own_callback *own)
{
	கட்டமைப்பு mosquitto__security_options *security_options;
	கட்டமைப்பு mosquitto__callback *tail, *tmp;
	கட்டமைப்பு mosquitto__callback **cb_base = வெற்று;

	க்கு(முழு i=0; i<plugin->config.security_option_மொத்தம்; i++){
		security_options = plugin->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, own->event);
		எனில்(cb_base == வெற்று){
			திரும்பு MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_SAFE(*cb_base, tail, tmp){
			எனில்(tail->identifier == plugin && tail->cb == own->cb_func){
				DL_DELETE(*cb_base, tail);
				mosquitto_FREE(tail);
				நிறுத்து;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	திரும்பு MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT முழு mosquitto_callback_register(
		mosquitto_plugin_id_t *identifier,
		முழு event,
		MOSQ_FUNC_generic_callback cb_func,
		மாறிலி வெறுமை *event_data,
		வெறுமை *userdata)
{
	கட்டமைப்பு mosquitto__callback **cb_base = வெற்று, *cb_new;
	கட்டமைப்பு mosquitto__security_options *security_options;
	கட்டமைப்பு plugin_own_callback *own_callback;

	எனில்(cb_func == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}

	எனில்(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
			|| event == MOSQ_EVT_PERSIST_BASE_செய்தி_ADD
			|| event == MOSQ_EVT_PERSIST_BASE_செய்தி_DELETE
			|| event == MOSQ_EVT_PERSIST_RETAIN_செய்தி_SET
			|| event == MOSQ_EVT_PERSIST_RETAIN_செய்தி_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_UPDATE
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_செய்தி_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_செய்தி_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_செய்தி_UPDATE
			|| event == MOSQ_EVT_PERSIST_WILL_ADD
			|| event == MOSQ_EVT_PERSIST_WILL_DELETE
			)){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		திரும்பு MOSQ_ERR_INVAL;
	}

	எனில்(event == MOSQ_EVT_CONTROL){
		திரும்பு control__register_callback(identifier, cb_func, event_data, userdata);
	}

	own_callback = mosquitto_calloc(1, அளவு(கட்டமைப்பு plugin_own_callback));
	எனில்(own_callback == வெற்று){
		திரும்பு MOSQ_ERR_NOMEM;
	}
	own_callback->event = (எண்ணிக்கை mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	எனில்(identifier->config.security_option_மொத்தம் == 0){
		log__printf(வெற்று, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((எண்ணிக்கை mosquitto_plugin_event)event));
		திரும்பு MOSQ_ERR_INVAL;
	}

	க்கு(முழு i=0; i<identifier->config.security_option_மொத்தம்; i++){
		security_options = identifier->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, (எண்ணிக்கை mosquitto_plugin_event)event);
		எனில்(cb_base == வெற்று){
			திரும்பு MOSQ_ERR_NOT_SUPPORTED;
		}

		எனில்(check_callback_exists(*cb_base, identifier, cb_func)){
			திரும்பு MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto__callback));
		எனில்(cb_new == வெற்று){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			திரும்பு MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_base, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	எனில்(identifier->plugin_name){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((எண்ணிக்கை mosquitto_plugin_event)event));
	}இல்லையெனில்{
		log__printf(வெற்று, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((எண்ணிக்கை mosquitto_plugin_event)event));
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	கட்டமைப்பு plugin_own_callback *own, *own_tmp;

	எனில்(plugin == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_SAFE(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT முழு mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		முழு event,
		MOSQ_FUNC_generic_callback cb_func,
		மாறிலி வெறுமை *event_data)
{
	கட்டமைப்பு plugin_own_callback *own, *own_tmp;

	எனில்(identifier == வெற்று || cb_func == வெற்று){
		திரும்பு MOSQ_ERR_INVAL;
	}

	எனில்(event == MOSQ_EVT_CONTROL){
		திரும்பு control__unregister_callback(identifier, cb_func, event_data);
	}

	DL_FOREACH_SAFE(identifier->own_callbacks, own, own_tmp){
		எனில்(own->event == (எண்ணிக்கை mosquitto_plugin_event)event && own->cb_func == cb_func){
			திரும்பு remove_callback(identifier, own);
		}
	}

	திரும்பு MOSQ_ERR_NOT_FOUND;
}
