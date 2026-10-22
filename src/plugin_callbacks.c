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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "utlist.h"
#અન્તર્ભાવ "lib_load.h"


સ્થિર અચલ અક્ષર *get_event_name(ગણના mosquitto_plugin_event event)
{
	પસંદગી(event){
		સ્થિતિ MOSQ_EVT_RELOAD:
			પાછા "reload";
		સ્થિતિ MOSQ_EVT_ACL_CHECK:
			પાછા "acl-check";
		સ્થિતિ MOSQ_EVT_BASIC_AUTH:
			પાછા "basic-auth";
		સ્થિતિ MOSQ_EVT_PSK_ચાવી:
			પાછા "psk-key";
		સ્થિતિ MOSQ_EVT_EXT_AUTH_START:
			પાછા "auth-start";
		સ્થિતિ MOSQ_EVT_EXT_AUTH_CONTINUE:
			પાછા "auth-continue";
		સ્થિતિ MOSQ_EVT_CONTROL:
			પાછા "control";
		સ્થિતિ MOSQ_EVT_MESSAGE_IN:
			પાછા "message-in";
		સ્થિતિ MOSQ_EVT_MESSAGE_OUT:
			પાછા "message-out";
		સ્થિતિ MOSQ_EVT_TICK:
			પાછા "tick";
		સ્થિતિ MOSQ_EVT_DISCONNECT:
			પાછા "disconnect";
		સ્થિતિ MOSQ_EVT_CONNECT:
			પાછા "connect";
		સ્થિતિ MOSQ_EVT_CLIENT_OFFLINE:
			પાછા "connect";
		સ્થિતિ MOSQ_EVT_SUBSCRIBE:
			પાછા "subscribe";
		સ્થિતિ MOSQ_EVT_UNSUBSCRIBE:
			પાછા "unsubscribe";
		સ્થિતિ MOSQ_EVT_PERSIST_RESTORE:
			પાછા "persist-restore";
		સ્થિતિ MOSQ_EVT_PERSIST_BASE_સંદેશ_ADD:
			પાછા "persist-base-msg-add";
		સ્થિતિ MOSQ_EVT_PERSIST_BASE_સંદેશ_DELETE:
			પાછા "persist-base-msg-delete";
		સ્થિતિ MOSQ_EVT_PERSIST_RETAIN_સંદેશ_SET:
			પાછા "persist-retain-msg-set";
		સ્થિતિ MOSQ_EVT_PERSIST_RETAIN_સંદેશ_DELETE:
			પાછા "persist-retain-msg-delete";
		સ્થિતિ MOSQ_EVT_PERSIST_CLIENT_ADD:
			પાછા "persist-client-add";
		સ્થિતિ MOSQ_EVT_PERSIST_CLIENT_DELETE:
			પાછા "persist-client-delete";
		સ્થિતિ MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			પાછા "persist-client-update";
		સ્થિતિ MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			પાછા "persist-subscription-add";
		સ્થિતિ MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			પાછા "persist-subscription-delete";
		સ્થિતિ MOSQ_EVT_PERSIST_CLIENT_સંદેશ_ADD:
			પાછા "persist-client-msg-add";
		સ્થિતિ MOSQ_EVT_PERSIST_CLIENT_સંદેશ_DELETE:
			પાછા "persist-client-msg-delete";
		સ્થિતિ MOSQ_EVT_PERSIST_CLIENT_સંદેશ_UPDATE:
			પાછા "persist-client-msg-update";
		સ્થિતિ MOSQ_EVT_PERSIST_WILL_ADD:
			પાછા "persist-will-add";
		સ્થિતિ MOSQ_EVT_PERSIST_WILL_DELETE:
			પાછા "persist-will-delete";
	}
	પાછા "";
}


સ્થિર bool check_callback_exists(સંરચના mosquitto__callback *cb_base, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	સંરચના mosquitto__callback *tail, *tmp;

	DL_FOREACH_SAFE(cb_base, tail, tmp){
		જો(tail->identifier == identifier && tail->cb == cb_func){
			પાછા true;
		}
	}
	પાછા false;
}

સ્થિર સંરચના mosquitto__callback **plugin__get_callback_base(સંરચના mosquitto__security_options *security_options, ગણના mosquitto_plugin_event event)
{
	પસંદગી(event){
		સ્થિતિ MOSQ_EVT_RELOAD:
			પાછા &security_options->plugin_callbacks.reload;
		સ્થિતિ MOSQ_EVT_ACL_CHECK:
			પાછા &security_options->plugin_callbacks.acl_check;
		સ્થિતિ MOSQ_EVT_BASIC_AUTH:
			પાછા &security_options->plugin_callbacks.basic_auth;
		સ્થિતિ MOSQ_EVT_PSK_ચાવી:
			પાછા &security_options->plugin_callbacks.psk_ચાવી;
		સ્થિતિ MOSQ_EVT_EXT_AUTH_START:
			પાછા &security_options->plugin_callbacks.ext_auth_start;
		સ્થિતિ MOSQ_EVT_EXT_AUTH_CONTINUE:
			પાછા &security_options->plugin_callbacks.ext_auth_continue;
		સ્થિતિ MOSQ_EVT_CONTROL:
			પાછા ખાલી;
		સ્થિતિ MOSQ_EVT_MESSAGE_IN: /* same as MOSQ_EVT_MESSAGE */
			પાછા &security_options->plugin_callbacks.message_in;
		સ્થિતિ MOSQ_EVT_TICK:
			પાછા &security_options->plugin_callbacks.tick;
		સ્થિતિ MOSQ_EVT_DISCONNECT:
			પાછા &security_options->plugin_callbacks.disconnect;
		સ્થિતિ MOSQ_EVT_CONNECT:
			પાછા &security_options->plugin_callbacks.connect;
		સ્થિતિ MOSQ_EVT_CLIENT_OFFLINE:
			પાછા &security_options->plugin_callbacks.client_offline;
		સ્થિતિ MOSQ_EVT_SUBSCRIBE:
			પાછા &security_options->plugin_callbacks.subscribe;
		સ્થિતિ MOSQ_EVT_UNSUBSCRIBE:
			પાછા &security_options->plugin_callbacks.unsubscribe;
		સ્થિતિ MOSQ_EVT_PERSIST_RESTORE:
			પાછા &security_options->plugin_callbacks.persist_restore;
		સ્થિતિ MOSQ_EVT_PERSIST_CLIENT_ADD:
			પાછા &security_options->plugin_callbacks.persist_client_add;
		સ્થિતિ MOSQ_EVT_PERSIST_CLIENT_DELETE:
			પાછા &security_options->plugin_callbacks.persist_client_delete;
		સ્થિતિ MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			પાછા &security_options->plugin_callbacks.persist_client_update;
		સ્થિતિ MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			પાછા &security_options->plugin_callbacks.persist_subscription_add;
		સ્થિતિ MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			પાછા &security_options->plugin_callbacks.persist_subscription_delete;
		સ્થિતિ MOSQ_EVT_PERSIST_CLIENT_સંદેશ_ADD:
			પાછા &security_options->plugin_callbacks.persist_client_સંદેશ_add;
		સ્થિતિ MOSQ_EVT_PERSIST_CLIENT_સંદેશ_DELETE:
			પાછા &security_options->plugin_callbacks.persist_client_સંદેશ_delete;
		સ્થિતિ MOSQ_EVT_PERSIST_CLIENT_સંદેશ_UPDATE:
			પાછા &security_options->plugin_callbacks.persist_client_સંદેશ_update;
		સ્થિતિ MOSQ_EVT_PERSIST_BASE_સંદેશ_ADD:
			પાછા &security_options->plugin_callbacks.persist_base_સંદેશ_add;
		સ્થિતિ MOSQ_EVT_PERSIST_BASE_સંદેશ_DELETE:
			પાછા &security_options->plugin_callbacks.persist_base_સંદેશ_delete;
		સ્થિતિ MOSQ_EVT_PERSIST_RETAIN_સંદેશ_SET:
			પાછા &security_options->plugin_callbacks.persist_retain_સંદેશ_set;
		સ્થિતિ MOSQ_EVT_PERSIST_RETAIN_સંદેશ_DELETE:
			પાછા &security_options->plugin_callbacks.persist_retain_સંદેશ_delete;
		સ્થિતિ MOSQ_EVT_MESSAGE_OUT:
			પાછા &security_options->plugin_callbacks.message_out;
		સ્થિતિ MOSQ_EVT_PERSIST_WILL_ADD:
			પાછા &security_options->plugin_callbacks.persist_will_add;
		સ્થિતિ MOSQ_EVT_PERSIST_WILL_DELETE:
			પાછા &security_options->plugin_callbacks.persist_will_delete;
	}
	પાછા ખાલી;
}


સ્થિર પૂર્ણાંક remove_callback(mosquitto_plugin_id_t *plugin, સંરચના plugin_own_callback *own)
{
	સંરચના mosquitto__security_options *security_options;
	સંરચના mosquitto__callback *tail, *tmp;
	સંરચના mosquitto__callback **cb_base = ખાલી;

	માટે(પૂર્ણાંક i=0; i<plugin->config.security_option_ગણતરી; i++){
		security_options = plugin->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, own->event);
		જો(cb_base == ખાલી){
			પાછા MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_SAFE(*cb_base, tail, tmp){
			જો(tail->identifier == plugin && tail->cb == own->cb_func){
				DL_DELETE(*cb_base, tail);
				mosquitto_FREE(tail);
				તોડો;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	પાછા MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT પૂર્ણાંક mosquitto_callback_register(
		mosquitto_plugin_id_t *identifier,
		પૂર્ણાંક event,
		MOSQ_FUNC_generic_callback cb_func,
		અચલ શૂન્ય *event_data,
		શૂન્ય *userdata)
{
	સંરચના mosquitto__callback **cb_base = ખાલી, *cb_new;
	સંરચના mosquitto__security_options *security_options;
	સંરચના plugin_own_callback *own_callback;

	જો(cb_func == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}

	જો(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
			|| event == MOSQ_EVT_PERSIST_BASE_સંદેશ_ADD
			|| event == MOSQ_EVT_PERSIST_BASE_સંદેશ_DELETE
			|| event == MOSQ_EVT_PERSIST_RETAIN_સંદેશ_SET
			|| event == MOSQ_EVT_PERSIST_RETAIN_સંદેશ_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_UPDATE
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_સંદેશ_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_સંદેશ_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_સંદેશ_UPDATE
			|| event == MOSQ_EVT_PERSIST_WILL_ADD
			|| event == MOSQ_EVT_PERSIST_WILL_DELETE
			)){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		પાછા MOSQ_ERR_INVAL;
	}

	જો(event == MOSQ_EVT_CONTROL){
		પાછા control__register_callback(identifier, cb_func, event_data, userdata);
	}

	own_callback = mosquitto_calloc(1, કદ(સંરચના plugin_own_callback));
	જો(own_callback == ખાલી){
		પાછા MOSQ_ERR_NOMEM;
	}
	own_callback->event = (ગણના mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	જો(identifier->config.security_option_ગણતરી == 0){
		log__printf(ખાલી, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((ગણના mosquitto_plugin_event)event));
		પાછા MOSQ_ERR_INVAL;
	}

	માટે(પૂર્ણાંક i=0; i<identifier->config.security_option_ગણતરી; i++){
		security_options = identifier->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, (ગણના mosquitto_plugin_event)event);
		જો(cb_base == ખાલી){
			પાછા MOSQ_ERR_NOT_SUPPORTED;
		}

		જો(check_callback_exists(*cb_base, identifier, cb_func)){
			પાછા MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, કદ(સંરચના mosquitto__callback));
		જો(cb_new == ખાલી){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			પાછા MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_base, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	જો(identifier->plugin_name){
		log__printf(ખાલી, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((ગણના mosquitto_plugin_event)event));
	}નહીંતર{
		log__printf(ખાલી, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((ગણના mosquitto_plugin_event)event));
	}

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	સંરચના plugin_own_callback *own, *own_tmp;

	જો(plugin == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_SAFE(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	પાછા MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT પૂર્ણાંક mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		પૂર્ણાંક event,
		MOSQ_FUNC_generic_callback cb_func,
		અચલ શૂન્ય *event_data)
{
	સંરચના plugin_own_callback *own, *own_tmp;

	જો(identifier == ખાલી || cb_func == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}

	જો(event == MOSQ_EVT_CONTROL){
		પાછા control__unregister_callback(identifier, cb_func, event_data);
	}

	DL_FOREACH_SAFE(identifier->own_callbacks, own, own_tmp){
		જો(own->event == (ગણના mosquitto_plugin_event)event && own->cb_func == cb_func){
			પાછા remove_callback(identifier, own);
		}
	}

	પાછા MOSQ_ERR_NOT_FOUND;
}
