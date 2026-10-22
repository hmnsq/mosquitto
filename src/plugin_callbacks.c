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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "utlist.h"
#ଅନ୍ତର୍ଭାଵ "lib_load.h"


ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *get_event_name(ଗଣନା mosquitto_plugin_event event)
{
	ବାଛ(event){
		ସ୍ଥିତି MOSQ_EVT_RELOAD:
			ଫେରନ୍ତୁ "reload";
		ସ୍ଥିତି MOSQ_EVT_ACL_CHECK:
			ଫେରନ୍ତୁ "acl-check";
		ସ୍ଥିତି MOSQ_EVT_BASIC_AUTH:
			ଫେରନ୍ତୁ "basic-auth";
		ସ୍ଥିତି MOSQ_EVT_PSK_ଚାବି:
			ଫେରନ୍ତୁ "psk-key";
		ସ୍ଥିତି MOSQ_EVT_EXT_AUTH_START:
			ଫେରନ୍ତୁ "auth-start";
		ସ୍ଥିତି MOSQ_EVT_EXT_AUTH_CONTINUE:
			ଫେରନ୍ତୁ "auth-continue";
		ସ୍ଥିତି MOSQ_EVT_CONTROL:
			ଫେରନ୍ତୁ "control";
		ସ୍ଥିତି MOSQ_EVT_MESSAGE_IN:
			ଫେରନ୍ତୁ "message-in";
		ସ୍ଥିତି MOSQ_EVT_MESSAGE_OUT:
			ଫେରନ୍ତୁ "message-out";
		ସ୍ଥିତି MOSQ_EVT_TICK:
			ଫେରନ୍ତୁ "tick";
		ସ୍ଥିତି MOSQ_EVT_DISCONNECT:
			ଫେରନ୍ତୁ "disconnect";
		ସ୍ଥିତି MOSQ_EVT_CONNECT:
			ଫେରନ୍ତୁ "connect";
		ସ୍ଥିତି MOSQ_EVT_CLIENT_OFFLINE:
			ଫେରନ୍ତୁ "connect";
		ସ୍ଥିତି MOSQ_EVT_SUBSCRIBE:
			ଫେରନ୍ତୁ "subscribe";
		ସ୍ଥିତି MOSQ_EVT_UNSUBSCRIBE:
			ଫେରନ୍ତୁ "unsubscribe";
		ସ୍ଥିତି MOSQ_EVT_PERSIST_RESTORE:
			ଫେରନ୍ତୁ "persist-restore";
		ସ୍ଥିତି MOSQ_EVT_PERSIST_BASE_ସନ୍ଦେଶ_ADD:
			ଫେରନ୍ତୁ "persist-base-msg-add";
		ସ୍ଥିତି MOSQ_EVT_PERSIST_BASE_ସନ୍ଦେଶ_DELETE:
			ଫେରନ୍ତୁ "persist-base-msg-delete";
		ସ୍ଥିତି MOSQ_EVT_PERSIST_RETAIN_ସନ୍ଦେଶ_SET:
			ଫେରନ୍ତୁ "persist-retain-msg-set";
		ସ୍ଥିତି MOSQ_EVT_PERSIST_RETAIN_ସନ୍ଦେଶ_DELETE:
			ଫେରନ୍ତୁ "persist-retain-msg-delete";
		ସ୍ଥିତି MOSQ_EVT_PERSIST_CLIENT_ADD:
			ଫେରନ୍ତୁ "persist-client-add";
		ସ୍ଥିତି MOSQ_EVT_PERSIST_CLIENT_DELETE:
			ଫେରନ୍ତୁ "persist-client-delete";
		ସ୍ଥିତି MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			ଫେରନ୍ତୁ "persist-client-update";
		ସ୍ଥିତି MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			ଫେରନ୍ତୁ "persist-subscription-add";
		ସ୍ଥିତି MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			ଫେରନ୍ତୁ "persist-subscription-delete";
		ସ୍ଥିତି MOSQ_EVT_PERSIST_CLIENT_ସନ୍ଦେଶ_ADD:
			ଫେରନ୍ତୁ "persist-client-msg-add";
		ସ୍ଥିତି MOSQ_EVT_PERSIST_CLIENT_ସନ୍ଦେଶ_DELETE:
			ଫେରନ୍ତୁ "persist-client-msg-delete";
		ସ୍ଥିତି MOSQ_EVT_PERSIST_CLIENT_ସନ୍ଦେଶ_UPDATE:
			ଫେରନ୍ତୁ "persist-client-msg-update";
		ସ୍ଥିତି MOSQ_EVT_PERSIST_WILL_ADD:
			ଫେରନ୍ତୁ "persist-will-add";
		ସ୍ଥିତି MOSQ_EVT_PERSIST_WILL_DELETE:
			ଫେରନ୍ତୁ "persist-will-delete";
	}
	ଫେରନ୍ତୁ "";
}


ସ୍ଥିର bool check_callback_exists(ସଂରଚନା mosquitto__callback *cb_base, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	ସଂରଚନା mosquitto__callback *tail, *tmp;

	DL_FOREACH_SAFE(cb_base, tail, tmp){
		ଯଦି(tail->identifier == identifier && tail->cb == cb_func){
			ଫେରନ୍ତୁ true;
		}
	}
	ଫେରନ୍ତୁ false;
}

ସ୍ଥିର ସଂରଚନା mosquitto__callback **plugin__get_callback_base(ସଂରଚନା mosquitto__security_options *security_options, ଗଣନା mosquitto_plugin_event event)
{
	ବାଛ(event){
		ସ୍ଥିତି MOSQ_EVT_RELOAD:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.reload;
		ସ୍ଥିତି MOSQ_EVT_ACL_CHECK:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.acl_check;
		ସ୍ଥିତି MOSQ_EVT_BASIC_AUTH:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.basic_auth;
		ସ୍ଥିତି MOSQ_EVT_PSK_ଚାବି:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.psk_ଚାବି;
		ସ୍ଥିତି MOSQ_EVT_EXT_AUTH_START:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.ext_auth_start;
		ସ୍ଥିତି MOSQ_EVT_EXT_AUTH_CONTINUE:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.ext_auth_continue;
		ସ୍ଥିତି MOSQ_EVT_CONTROL:
			ଫେରନ୍ତୁ ଖାଲି;
		ସ୍ଥିତି MOSQ_EVT_MESSAGE_IN: /* same as MOSQ_EVT_MESSAGE */
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.message_in;
		ସ୍ଥିତି MOSQ_EVT_TICK:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.tick;
		ସ୍ଥିତି MOSQ_EVT_DISCONNECT:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.disconnect;
		ସ୍ଥିତି MOSQ_EVT_CONNECT:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.connect;
		ସ୍ଥିତି MOSQ_EVT_CLIENT_OFFLINE:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.client_offline;
		ସ୍ଥିତି MOSQ_EVT_SUBSCRIBE:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.subscribe;
		ସ୍ଥିତି MOSQ_EVT_UNSUBSCRIBE:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.unsubscribe;
		ସ୍ଥିତି MOSQ_EVT_PERSIST_RESTORE:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.persist_restore;
		ସ୍ଥିତି MOSQ_EVT_PERSIST_CLIENT_ADD:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.persist_client_add;
		ସ୍ଥିତି MOSQ_EVT_PERSIST_CLIENT_DELETE:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.persist_client_delete;
		ସ୍ଥିତି MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.persist_client_update;
		ସ୍ଥିତି MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.persist_subscription_add;
		ସ୍ଥିତି MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.persist_subscription_delete;
		ସ୍ଥିତି MOSQ_EVT_PERSIST_CLIENT_ସନ୍ଦେଶ_ADD:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.persist_client_ସନ୍ଦେଶ_add;
		ସ୍ଥିତି MOSQ_EVT_PERSIST_CLIENT_ସନ୍ଦେଶ_DELETE:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.persist_client_ସନ୍ଦେଶ_delete;
		ସ୍ଥିତି MOSQ_EVT_PERSIST_CLIENT_ସନ୍ଦେଶ_UPDATE:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.persist_client_ସନ୍ଦେଶ_update;
		ସ୍ଥିତି MOSQ_EVT_PERSIST_BASE_ସନ୍ଦେଶ_ADD:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.persist_base_ସନ୍ଦେଶ_add;
		ସ୍ଥିତି MOSQ_EVT_PERSIST_BASE_ସନ୍ଦେଶ_DELETE:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.persist_base_ସନ୍ଦେଶ_delete;
		ସ୍ଥିତି MOSQ_EVT_PERSIST_RETAIN_ସନ୍ଦେଶ_SET:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.persist_retain_ସନ୍ଦେଶ_set;
		ସ୍ଥିତି MOSQ_EVT_PERSIST_RETAIN_ସନ୍ଦେଶ_DELETE:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.persist_retain_ସନ୍ଦେଶ_delete;
		ସ୍ଥିତି MOSQ_EVT_MESSAGE_OUT:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.message_out;
		ସ୍ଥିତି MOSQ_EVT_PERSIST_WILL_ADD:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.persist_will_add;
		ସ୍ଥିତି MOSQ_EVT_PERSIST_WILL_DELETE:
			ଫେରନ୍ତୁ &security_options->plugin_callbacks.persist_will_delete;
	}
	ଫେରନ୍ତୁ ଖାଲି;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା remove_callback(mosquitto_plugin_id_t *plugin, ସଂରଚନା plugin_own_callback *own)
{
	ସଂରଚନା mosquitto__security_options *security_options;
	ସଂରଚନା mosquitto__callback *tail, *tmp;
	ସଂରଚନା mosquitto__callback **cb_base = ଖାଲି;

	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<plugin->config.security_option_ଗଣନା; i++){
		security_options = plugin->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, own->event);
		ଯଦି(cb_base == ଖାଲି){
			ଫେରନ୍ତୁ MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_SAFE(*cb_base, tail, tmp){
			ଯଦି(tail->identifier == plugin && tail->cb == own->cb_func){
				DL_DELETE(*cb_base, tail);
				mosquitto_FREE(tail);
				ଭାଙ୍ଗ;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_callback_register(
		mosquitto_plugin_id_t *identifier,
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା event,
		MOSQ_FUNC_generic_callback cb_func,
		ଅଚଳ ଶୂନ୍ୟ *event_data,
		ଶୂନ୍ୟ *userdata)
{
	ସଂରଚନା mosquitto__callback **cb_base = ଖାଲି, *cb_new;
	ସଂରଚନା mosquitto__security_options *security_options;
	ସଂରଚନା plugin_own_callback *own_callback;

	ଯଦି(cb_func == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	ଯଦି(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
			|| event == MOSQ_EVT_PERSIST_BASE_ସନ୍ଦେଶ_ADD
			|| event == MOSQ_EVT_PERSIST_BASE_ସନ୍ଦେଶ_DELETE
			|| event == MOSQ_EVT_PERSIST_RETAIN_ସନ୍ଦେଶ_SET
			|| event == MOSQ_EVT_PERSIST_RETAIN_ସନ୍ଦେଶ_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_UPDATE
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ସନ୍ଦେଶ_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_ସନ୍ଦେଶ_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ସନ୍ଦେଶ_UPDATE
			|| event == MOSQ_EVT_PERSIST_WILL_ADD
			|| event == MOSQ_EVT_PERSIST_WILL_DELETE
			)){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	ଯଦି(event == MOSQ_EVT_CONTROL){
		ଫେରନ୍ତୁ control__register_callback(identifier, cb_func, event_data, userdata);
	}

	own_callback = mosquitto_calloc(1, ଆକାର(ସଂରଚନା plugin_own_callback));
	ଯଦି(own_callback == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}
	own_callback->event = (ଗଣନା mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	ଯଦି(identifier->config.security_option_ଗଣନା == 0){
		log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((ଗଣନା mosquitto_plugin_event)event));
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<identifier->config.security_option_ଗଣନା; i++){
		security_options = identifier->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, (ଗଣନା mosquitto_plugin_event)event);
		ଯଦି(cb_base == ଖାଲି){
			ଫେରନ୍ତୁ MOSQ_ERR_NOT_SUPPORTED;
		}

		ଯଦି(check_callback_exists(*cb_base, identifier, cb_func)){
			ଫେରନ୍ତୁ MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto__callback));
		ଯଦି(cb_new == ଖାଲି){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_base, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	ଯଦି(identifier->plugin_name){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((ଗଣନା mosquitto_plugin_event)event));
	}ଅନ୍ୟଥା{
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((ଗଣନା mosquitto_plugin_event)event));
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	ସଂରଚନା plugin_own_callback *own, *own_tmp;

	ଯଦି(plugin == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_SAFE(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା event,
		MOSQ_FUNC_generic_callback cb_func,
		ଅଚଳ ଶୂନ୍ୟ *event_data)
{
	ସଂରଚନା plugin_own_callback *own, *own_tmp;

	ଯଦି(identifier == ଖାଲି || cb_func == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	ଯଦି(event == MOSQ_EVT_CONTROL){
		ଫେରନ୍ତୁ control__unregister_callback(identifier, cb_func, event_data);
	}

	DL_FOREACH_SAFE(identifier->own_callbacks, own, own_tmp){
		ଯଦି(own->event == (ଗଣନା mosquitto_plugin_event)event && own->cb_func == cb_func){
			ଫେରନ୍ତୁ remove_callback(identifier, own);
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_NOT_FOUND;
}
