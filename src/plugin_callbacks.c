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

#अन्तर्भाव "config.h"

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "utlist.h"
#अन्तर्भाव "lib_load.h"


स्थिर ध्रुव वर्ण *get_event_name(गणना mosquitto_plugin_event event)
{
	विकल्प(event){
		पक्ष MOSQ_EVT_RELOAD:
			पटिनिवत्त "reload";
		पक्ष MOSQ_EVT_ACL_CHECK:
			पटिनिवत्त "acl-check";
		पक्ष MOSQ_EVT_BASIC_AUTH:
			पटिनिवत्त "basic-auth";
		पक्ष MOSQ_EVT_PSK_कुञ्चिका:
			पटिनिवत्त "psk-key";
		पक्ष MOSQ_EVT_EXT_AUTH_START:
			पटिनिवत्त "auth-start";
		पक्ष MOSQ_EVT_EXT_AUTH_CONTINUE:
			पटिनिवत्त "auth-continue";
		पक्ष MOSQ_EVT_CONTROL:
			पटिनिवत्त "control";
		पक्ष MOSQ_EVT_MESSAGE_IN:
			पटिनिवत्त "message-in";
		पक्ष MOSQ_EVT_MESSAGE_OUT:
			पटिनिवत्त "message-out";
		पक्ष MOSQ_EVT_TICK:
			पटिनिवत्त "tick";
		पक्ष MOSQ_EVT_DISCONNECT:
			पटिनिवत्त "disconnect";
		पक्ष MOSQ_EVT_CONNECT:
			पटिनिवत्त "connect";
		पक्ष MOSQ_EVT_CLIENT_OFFLINE:
			पटिनिवत्त "connect";
		पक्ष MOSQ_EVT_SUBSCRIBE:
			पटिनिवत्त "subscribe";
		पक्ष MOSQ_EVT_UNSUBSCRIBE:
			पटिनिवत्त "unsubscribe";
		पक्ष MOSQ_EVT_PERSIST_RESTORE:
			पटिनिवत्त "persist-restore";
		पक्ष MOSQ_EVT_PERSIST_BASE_सन्देशः_ADD:
			पटिनिवत्त "persist-base-msg-add";
		पक्ष MOSQ_EVT_PERSIST_BASE_सन्देशः_DELETE:
			पटिनिवत्त "persist-base-msg-delete";
		पक्ष MOSQ_EVT_PERSIST_RETAIN_सन्देशः_SET:
			पटिनिवत्त "persist-retain-msg-set";
		पक्ष MOSQ_EVT_PERSIST_RETAIN_सन्देशः_DELETE:
			पटिनिवत्त "persist-retain-msg-delete";
		पक्ष MOSQ_EVT_PERSIST_CLIENT_ADD:
			पटिनिवत्त "persist-client-add";
		पक्ष MOSQ_EVT_PERSIST_CLIENT_DELETE:
			पटिनिवत्त "persist-client-delete";
		पक्ष MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			पटिनिवत्त "persist-client-update";
		पक्ष MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			पटिनिवत्त "persist-subscription-add";
		पक्ष MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			पटिनिवत्त "persist-subscription-delete";
		पक्ष MOSQ_EVT_PERSIST_CLIENT_सन्देशः_ADD:
			पटिनिवत्त "persist-client-msg-add";
		पक्ष MOSQ_EVT_PERSIST_CLIENT_सन्देशः_DELETE:
			पटिनिवत्त "persist-client-msg-delete";
		पक्ष MOSQ_EVT_PERSIST_CLIENT_सन्देशः_UPDATE:
			पटिनिवत्त "persist-client-msg-update";
		पक्ष MOSQ_EVT_PERSIST_WILL_ADD:
			पटिनिवत्त "persist-will-add";
		पक्ष MOSQ_EVT_PERSIST_WILL_DELETE:
			पटिनिवत्त "persist-will-delete";
	}
	पटिनिवत्त "";
}


स्थिर bool check_callback_exists(संरचना mosquitto__callback *cb_base, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	संरचना mosquitto__callback *tail, *tmp;

	DL_FOREACH_SAFE(cb_base, tail, tmp){
		सचे(tail->identifier == identifier && tail->cb == cb_func){
			पटिनिवत्त true;
		}
	}
	पटिनिवत्त false;
}

स्थिर संरचना mosquitto__callback **plugin__get_callback_base(संरचना mosquitto__security_options *security_options, गणना mosquitto_plugin_event event)
{
	विकल्प(event){
		पक्ष MOSQ_EVT_RELOAD:
			पटिनिवत्त &security_options->plugin_callbacks.reload;
		पक्ष MOSQ_EVT_ACL_CHECK:
			पटिनिवत्त &security_options->plugin_callbacks.acl_check;
		पक्ष MOSQ_EVT_BASIC_AUTH:
			पटिनिवत्त &security_options->plugin_callbacks.basic_auth;
		पक्ष MOSQ_EVT_PSK_कुञ्चिका:
			पटिनिवत्त &security_options->plugin_callbacks.psk_कुञ्चिका;
		पक्ष MOSQ_EVT_EXT_AUTH_START:
			पटिनिवत्त &security_options->plugin_callbacks.ext_auth_start;
		पक्ष MOSQ_EVT_EXT_AUTH_CONTINUE:
			पटिनिवत्त &security_options->plugin_callbacks.ext_auth_continue;
		पक्ष MOSQ_EVT_CONTROL:
			पटिनिवत्त रिक्त;
		पक्ष MOSQ_EVT_MESSAGE_IN: /* same as MOSQ_EVT_MESSAGE */
			पटिनिवत्त &security_options->plugin_callbacks.message_in;
		पक्ष MOSQ_EVT_TICK:
			पटिनिवत्त &security_options->plugin_callbacks.tick;
		पक्ष MOSQ_EVT_DISCONNECT:
			पटिनिवत्त &security_options->plugin_callbacks.disconnect;
		पक्ष MOSQ_EVT_CONNECT:
			पटिनिवत्त &security_options->plugin_callbacks.connect;
		पक्ष MOSQ_EVT_CLIENT_OFFLINE:
			पटिनिवत्त &security_options->plugin_callbacks.client_offline;
		पक्ष MOSQ_EVT_SUBSCRIBE:
			पटिनिवत्त &security_options->plugin_callbacks.subscribe;
		पक्ष MOSQ_EVT_UNSUBSCRIBE:
			पटिनिवत्त &security_options->plugin_callbacks.unsubscribe;
		पक्ष MOSQ_EVT_PERSIST_RESTORE:
			पटिनिवत्त &security_options->plugin_callbacks.persist_restore;
		पक्ष MOSQ_EVT_PERSIST_CLIENT_ADD:
			पटिनिवत्त &security_options->plugin_callbacks.persist_client_add;
		पक्ष MOSQ_EVT_PERSIST_CLIENT_DELETE:
			पटिनिवत्त &security_options->plugin_callbacks.persist_client_delete;
		पक्ष MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			पटिनिवत्त &security_options->plugin_callbacks.persist_client_update;
		पक्ष MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			पटिनिवत्त &security_options->plugin_callbacks.persist_subscription_add;
		पक्ष MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			पटिनिवत्त &security_options->plugin_callbacks.persist_subscription_delete;
		पक्ष MOSQ_EVT_PERSIST_CLIENT_सन्देशः_ADD:
			पटिनिवत्त &security_options->plugin_callbacks.persist_client_सन्देशः_add;
		पक्ष MOSQ_EVT_PERSIST_CLIENT_सन्देशः_DELETE:
			पटिनिवत्त &security_options->plugin_callbacks.persist_client_सन्देशः_delete;
		पक्ष MOSQ_EVT_PERSIST_CLIENT_सन्देशः_UPDATE:
			पटिनिवत्त &security_options->plugin_callbacks.persist_client_सन्देशः_update;
		पक्ष MOSQ_EVT_PERSIST_BASE_सन्देशः_ADD:
			पटिनिवत्त &security_options->plugin_callbacks.persist_base_सन्देशः_add;
		पक्ष MOSQ_EVT_PERSIST_BASE_सन्देशः_DELETE:
			पटिनिवत्त &security_options->plugin_callbacks.persist_base_सन्देशः_delete;
		पक्ष MOSQ_EVT_PERSIST_RETAIN_सन्देशः_SET:
			पटिनिवत्त &security_options->plugin_callbacks.persist_retain_सन्देशः_set;
		पक्ष MOSQ_EVT_PERSIST_RETAIN_सन्देशः_DELETE:
			पटिनिवत्त &security_options->plugin_callbacks.persist_retain_सन्देशः_delete;
		पक्ष MOSQ_EVT_MESSAGE_OUT:
			पटिनिवत्त &security_options->plugin_callbacks.message_out;
		पक्ष MOSQ_EVT_PERSIST_WILL_ADD:
			पटिनिवत्त &security_options->plugin_callbacks.persist_will_add;
		पक्ष MOSQ_EVT_PERSIST_WILL_DELETE:
			पटिनिवत्त &security_options->plugin_callbacks.persist_will_delete;
	}
	पटिनिवत्त रिक्त;
}


स्थिर पूर्णाङ्क remove_callback(mosquitto_plugin_id_t *plugin, संरचना plugin_own_callback *own)
{
	संरचना mosquitto__security_options *security_options;
	संरचना mosquitto__callback *tail, *tmp;
	संरचना mosquitto__callback **cb_base = रिक्त;

	अत्थाय(पूर्णाङ्क i=0; i<plugin->config.security_option_गणनम्; i++){
		security_options = plugin->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, own->event);
		सचे(cb_base == रिक्त){
			पटिनिवत्त MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_SAFE(*cb_base, tail, tmp){
			सचे(tail->identifier == plugin && tail->cb == own->cb_func){
				DL_DELETE(*cb_base, tail);
				mosquitto_FREE(tail);
				भङ्ग;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_callback_register(
		mosquitto_plugin_id_t *identifier,
		पूर्णाङ्क event,
		MOSQ_FUNC_generic_callback cb_func,
		ध्रुव शून्य *event_data,
		शून्य *userdata)
{
	संरचना mosquitto__callback **cb_base = रिक्त, *cb_new;
	संरचना mosquitto__security_options *security_options;
	संरचना plugin_own_callback *own_callback;

	सचे(cb_func == रिक्त){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	सचे(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
			|| event == MOSQ_EVT_PERSIST_BASE_सन्देशः_ADD
			|| event == MOSQ_EVT_PERSIST_BASE_सन्देशः_DELETE
			|| event == MOSQ_EVT_PERSIST_RETAIN_सन्देशः_SET
			|| event == MOSQ_EVT_PERSIST_RETAIN_सन्देशः_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_UPDATE
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_सन्देशः_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_सन्देशः_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_सन्देशः_UPDATE
			|| event == MOSQ_EVT_PERSIST_WILL_ADD
			|| event == MOSQ_EVT_PERSIST_WILL_DELETE
			)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	सचे(event == MOSQ_EVT_CONTROL){
		पटिनिवत्त control__register_callback(identifier, cb_func, event_data, userdata);
	}

	own_callback = mosquitto_calloc(1, परिमाण(संरचना plugin_own_callback));
	सचे(own_callback == रिक्त){
		पटिनिवत्त MOSQ_ERR_NOMEM;
	}
	own_callback->event = (गणना mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	सचे(identifier->config.security_option_गणनम् == 0){
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((गणना mosquitto_plugin_event)event));
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	अत्थाय(पूर्णाङ्क i=0; i<identifier->config.security_option_गणनम्; i++){
		security_options = identifier->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, (गणना mosquitto_plugin_event)event);
		सचे(cb_base == रिक्त){
			पटिनिवत्त MOSQ_ERR_NOT_SUPPORTED;
		}

		सचे(check_callback_exists(*cb_base, identifier, cb_func)){
			पटिनिवत्त MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, परिमाण(संरचना mosquitto__callback));
		सचे(cb_new == रिक्त){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			पटिनिवत्त MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_base, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	सचे(identifier->plugin_name){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((गणना mosquitto_plugin_event)event));
	}अञ्ञथा{
		log__printf(रिक्त, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((गणना mosquitto_plugin_event)event));
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	संरचना plugin_own_callback *own, *own_tmp;

	सचे(plugin == रिक्त){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_SAFE(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		पूर्णाङ्क event,
		MOSQ_FUNC_generic_callback cb_func,
		ध्रुव शून्य *event_data)
{
	संरचना plugin_own_callback *own, *own_tmp;

	सचे(identifier == रिक्त || cb_func == रिक्त){
		पटिनिवत्त MOSQ_ERR_INVAL;
	}

	सचे(event == MOSQ_EVT_CONTROL){
		पटिनिवत्त control__unregister_callback(identifier, cb_func, event_data);
	}

	DL_FOREACH_SAFE(identifier->own_callbacks, own, own_tmp){
		सचे(own->event == (गणना mosquitto_plugin_event)event && own->cb_func == cb_func){
			पटिनिवत्त remove_callback(identifier, own);
		}
	}

	पटिनिवत्त MOSQ_ERR_NOT_FOUND;
}
