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
			प्रत्यागम "reload";
		पक्ष MOSQ_EVT_ACL_CHECK:
			प्रत्यागम "acl-check";
		पक्ष MOSQ_EVT_BASIC_AUTH:
			प्रत्यागम "basic-auth";
		पक्ष MOSQ_EVT_PSK_कुञ्चिका:
			प्रत्यागम "psk-key";
		पक्ष MOSQ_EVT_EXT_AUTH_START:
			प्रत्यागम "auth-start";
		पक्ष MOSQ_EVT_EXT_AUTH_CONTINUE:
			प्रत्यागम "auth-continue";
		पक्ष MOSQ_EVT_CONTROL:
			प्रत्यागम "control";
		पक्ष MOSQ_EVT_MESSAGE_IN:
			प्रत्यागम "message-in";
		पक्ष MOSQ_EVT_MESSAGE_OUT:
			प्रत्यागम "message-out";
		पक्ष MOSQ_EVT_TICK:
			प्रत्यागम "tick";
		पक्ष MOSQ_EVT_DISCONNECT:
			प्रत्यागम "disconnect";
		पक्ष MOSQ_EVT_CONNECT:
			प्रत्यागम "connect";
		पक्ष MOSQ_EVT_CLIENT_OFFLINE:
			प्रत्यागम "connect";
		पक्ष MOSQ_EVT_SUBSCRIBE:
			प्रत्यागम "subscribe";
		पक्ष MOSQ_EVT_UNSUBSCRIBE:
			प्रत्यागम "unsubscribe";
		पक्ष MOSQ_EVT_PERSIST_RESTORE:
			प्रत्यागम "persist-restore";
		पक्ष MOSQ_EVT_PERSIST_BASE_सन्देशः_ADD:
			प्रत्यागम "persist-base-msg-add";
		पक्ष MOSQ_EVT_PERSIST_BASE_सन्देशः_DELETE:
			प्रत्यागम "persist-base-msg-delete";
		पक्ष MOSQ_EVT_PERSIST_RETAIN_सन्देशः_SET:
			प्रत्यागम "persist-retain-msg-set";
		पक्ष MOSQ_EVT_PERSIST_RETAIN_सन्देशः_DELETE:
			प्रत्यागम "persist-retain-msg-delete";
		पक्ष MOSQ_EVT_PERSIST_CLIENT_ADD:
			प्रत्यागम "persist-client-add";
		पक्ष MOSQ_EVT_PERSIST_CLIENT_DELETE:
			प्रत्यागम "persist-client-delete";
		पक्ष MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			प्रत्यागम "persist-client-update";
		पक्ष MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			प्रत्यागम "persist-subscription-add";
		पक्ष MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			प्रत्यागम "persist-subscription-delete";
		पक्ष MOSQ_EVT_PERSIST_CLIENT_सन्देशः_ADD:
			प्रत्यागम "persist-client-msg-add";
		पक्ष MOSQ_EVT_PERSIST_CLIENT_सन्देशः_DELETE:
			प्रत्यागम "persist-client-msg-delete";
		पक्ष MOSQ_EVT_PERSIST_CLIENT_सन्देशः_UPDATE:
			प्रत्यागम "persist-client-msg-update";
		पक्ष MOSQ_EVT_PERSIST_WILL_ADD:
			प्रत्यागम "persist-will-add";
		पक्ष MOSQ_EVT_PERSIST_WILL_DELETE:
			प्रत्यागम "persist-will-delete";
	}
	प्रत्यागम "";
}


स्थिर bool check_callback_exists(संरचना mosquitto__callback *cb_base, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	संरचना mosquitto__callback *tail, *tmp;

	DL_FOREACH_SAFE(cb_base, tail, tmp){
		जइ(tail->identifier == identifier && tail->cb == cb_func){
			प्रत्यागम true;
		}
	}
	प्रत्यागम false;
}

स्थिर संरचना mosquitto__callback **plugin__get_callback_base(संरचना mosquitto__security_options *security_options, गणना mosquitto_plugin_event event)
{
	विकल्प(event){
		पक्ष MOSQ_EVT_RELOAD:
			प्रत्यागम &security_options->plugin_callbacks.reload;
		पक्ष MOSQ_EVT_ACL_CHECK:
			प्रत्यागम &security_options->plugin_callbacks.acl_check;
		पक्ष MOSQ_EVT_BASIC_AUTH:
			प्रत्यागम &security_options->plugin_callbacks.basic_auth;
		पक्ष MOSQ_EVT_PSK_कुञ्चिका:
			प्रत्यागम &security_options->plugin_callbacks.psk_कुञ्चिका;
		पक्ष MOSQ_EVT_EXT_AUTH_START:
			प्रत्यागम &security_options->plugin_callbacks.ext_auth_start;
		पक्ष MOSQ_EVT_EXT_AUTH_CONTINUE:
			प्रत्यागम &security_options->plugin_callbacks.ext_auth_continue;
		पक्ष MOSQ_EVT_CONTROL:
			प्रत्यागम रिक्त;
		पक्ष MOSQ_EVT_MESSAGE_IN: /* same as MOSQ_EVT_MESSAGE */
			प्रत्यागम &security_options->plugin_callbacks.message_in;
		पक्ष MOSQ_EVT_TICK:
			प्रत्यागम &security_options->plugin_callbacks.tick;
		पक्ष MOSQ_EVT_DISCONNECT:
			प्रत्यागम &security_options->plugin_callbacks.disconnect;
		पक्ष MOSQ_EVT_CONNECT:
			प्रत्यागम &security_options->plugin_callbacks.connect;
		पक्ष MOSQ_EVT_CLIENT_OFFLINE:
			प्रत्यागम &security_options->plugin_callbacks.client_offline;
		पक्ष MOSQ_EVT_SUBSCRIBE:
			प्रत्यागम &security_options->plugin_callbacks.subscribe;
		पक्ष MOSQ_EVT_UNSUBSCRIBE:
			प्रत्यागम &security_options->plugin_callbacks.unsubscribe;
		पक्ष MOSQ_EVT_PERSIST_RESTORE:
			प्रत्यागम &security_options->plugin_callbacks.persist_restore;
		पक्ष MOSQ_EVT_PERSIST_CLIENT_ADD:
			प्रत्यागम &security_options->plugin_callbacks.persist_client_add;
		पक्ष MOSQ_EVT_PERSIST_CLIENT_DELETE:
			प्रत्यागम &security_options->plugin_callbacks.persist_client_delete;
		पक्ष MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			प्रत्यागम &security_options->plugin_callbacks.persist_client_update;
		पक्ष MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			प्रत्यागम &security_options->plugin_callbacks.persist_subscription_add;
		पक्ष MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			प्रत्यागम &security_options->plugin_callbacks.persist_subscription_delete;
		पक्ष MOSQ_EVT_PERSIST_CLIENT_सन्देशः_ADD:
			प्रत्यागम &security_options->plugin_callbacks.persist_client_सन्देशः_add;
		पक्ष MOSQ_EVT_PERSIST_CLIENT_सन्देशः_DELETE:
			प्रत्यागम &security_options->plugin_callbacks.persist_client_सन्देशः_delete;
		पक्ष MOSQ_EVT_PERSIST_CLIENT_सन्देशः_UPDATE:
			प्रत्यागम &security_options->plugin_callbacks.persist_client_सन्देशः_update;
		पक्ष MOSQ_EVT_PERSIST_BASE_सन्देशः_ADD:
			प्रत्यागम &security_options->plugin_callbacks.persist_base_सन्देशः_add;
		पक्ष MOSQ_EVT_PERSIST_BASE_सन्देशः_DELETE:
			प्रत्यागम &security_options->plugin_callbacks.persist_base_सन्देशः_delete;
		पक्ष MOSQ_EVT_PERSIST_RETAIN_सन्देशः_SET:
			प्रत्यागम &security_options->plugin_callbacks.persist_retain_सन्देशः_set;
		पक्ष MOSQ_EVT_PERSIST_RETAIN_सन्देशः_DELETE:
			प्रत्यागम &security_options->plugin_callbacks.persist_retain_सन्देशः_delete;
		पक्ष MOSQ_EVT_MESSAGE_OUT:
			प्रत्यागम &security_options->plugin_callbacks.message_out;
		पक्ष MOSQ_EVT_PERSIST_WILL_ADD:
			प्रत्यागम &security_options->plugin_callbacks.persist_will_add;
		पक्ष MOSQ_EVT_PERSIST_WILL_DELETE:
			प्रत्यागम &security_options->plugin_callbacks.persist_will_delete;
	}
	प्रत्यागम रिक्त;
}


स्थिर पूर्णाङ्क remove_callback(mosquitto_plugin_id_t *plugin, संरचना plugin_own_callback *own)
{
	संरचना mosquitto__security_options *security_options;
	संरचना mosquitto__callback *tail, *tmp;
	संरचना mosquitto__callback **cb_base = रिक्त;

	प्रति(पूर्णाङ्क i=0; i<plugin->config.security_option_गणनम्; i++){
		security_options = plugin->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, own->event);
		जइ(cb_base == रिक्त){
			प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_SAFE(*cb_base, tail, tmp){
			जइ(tail->identifier == plugin && tail->cb == own->cb_func){
				DL_DELETE(*cb_base, tail);
				mosquitto_FREE(tail);
				भङ्ग;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	प्रत्यागम MOSQ_ERR_SUCCESS;
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

	जइ(cb_func == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	जइ(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
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
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	जइ(event == MOSQ_EVT_CONTROL){
		प्रत्यागम control__register_callback(identifier, cb_func, event_data, userdata);
	}

	own_callback = mosquitto_calloc(1, परिमाण(संरचना plugin_own_callback));
	जइ(own_callback == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}
	own_callback->event = (गणना mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	जइ(identifier->config.security_option_गणनम् == 0){
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((गणना mosquitto_plugin_event)event));
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	प्रति(पूर्णाङ्क i=0; i<identifier->config.security_option_गणनम्; i++){
		security_options = identifier->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, (गणना mosquitto_plugin_event)event);
		जइ(cb_base == रिक्त){
			प्रत्यागम MOSQ_ERR_NOT_SUPPORTED;
		}

		जइ(check_callback_exists(*cb_base, identifier, cb_func)){
			प्रत्यागम MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, परिमाण(संरचना mosquitto__callback));
		जइ(cb_new == रिक्त){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_base, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	जइ(identifier->plugin_name){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((गणना mosquitto_plugin_event)event));
	}अण्णहा{
		log__printf(रिक्त, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((गणना mosquitto_plugin_event)event));
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	संरचना plugin_own_callback *own, *own_tmp;

	जइ(plugin == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_SAFE(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		पूर्णाङ्क event,
		MOSQ_FUNC_generic_callback cb_func,
		ध्रुव शून्य *event_data)
{
	संरचना plugin_own_callback *own, *own_tmp;

	जइ(identifier == रिक्त || cb_func == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	जइ(event == MOSQ_EVT_CONTROL){
		प्रत्यागम control__unregister_callback(identifier, cb_func, event_data);
	}

	DL_FOREACH_SAFE(identifier->own_callbacks, own, own_tmp){
		जइ(own->event == (गणना mosquitto_plugin_event)event && own->cb_func == cb_func){
			प्रत्यागम remove_callback(identifier, own);
		}
	}

	प्रत्यागम MOSQ_ERR_NOT_FOUND;
}
