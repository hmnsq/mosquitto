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

#समावेश "config.h"

#समावेश "mosquitto_broker_internal.h"
#समावेश "utlist.h"
#समावेश "lib_load.h"


अटल स्थिर अक्षर *get_event_name(क्रमागत mosquitto_plugin_event event)
{
	चयन(event){
		हाल MOSQ_EVT_RELOAD:
			वापस "reload";
		हाल MOSQ_EVT_ACL_CHECK:
			वापस "acl-check";
		हाल MOSQ_EVT_BASIC_AUTH:
			वापस "basic-auth";
		हाल MOSQ_EVT_PSK_कुंजी:
			वापस "psk-key";
		हाल MOSQ_EVT_EXT_AUTH_START:
			वापस "auth-start";
		हाल MOSQ_EVT_EXT_AUTH_CONTINUE:
			वापस "auth-continue";
		हाल MOSQ_EVT_CONTROL:
			वापस "control";
		हाल MOSQ_EVT_MESSAGE_IN:
			वापस "message-in";
		हाल MOSQ_EVT_MESSAGE_OUT:
			वापस "message-out";
		हाल MOSQ_EVT_TICK:
			वापस "tick";
		हाल MOSQ_EVT_DISCONNECT:
			वापस "disconnect";
		हाल MOSQ_EVT_CONNECT:
			वापस "connect";
		हाल MOSQ_EVT_CLIENT_OFFLINE:
			वापस "connect";
		हाल MOSQ_EVT_SUBSCRIBE:
			वापस "subscribe";
		हाल MOSQ_EVT_UNSUBSCRIBE:
			वापस "unsubscribe";
		हाल MOSQ_EVT_PERSIST_RESTORE:
			वापस "persist-restore";
		हाल MOSQ_EVT_PERSIST_BASE_संदेश_ADD:
			वापस "persist-base-msg-add";
		हाल MOSQ_EVT_PERSIST_BASE_संदेश_DELETE:
			वापस "persist-base-msg-delete";
		हाल MOSQ_EVT_PERSIST_RETAIN_संदेश_SET:
			वापस "persist-retain-msg-set";
		हाल MOSQ_EVT_PERSIST_RETAIN_संदेश_DELETE:
			वापस "persist-retain-msg-delete";
		हाल MOSQ_EVT_PERSIST_CLIENT_ADD:
			वापस "persist-client-add";
		हाल MOSQ_EVT_PERSIST_CLIENT_DELETE:
			वापस "persist-client-delete";
		हाल MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			वापस "persist-client-update";
		हाल MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			वापस "persist-subscription-add";
		हाल MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			वापस "persist-subscription-delete";
		हाल MOSQ_EVT_PERSIST_CLIENT_संदेश_ADD:
			वापस "persist-client-msg-add";
		हाल MOSQ_EVT_PERSIST_CLIENT_संदेश_DELETE:
			वापस "persist-client-msg-delete";
		हाल MOSQ_EVT_PERSIST_CLIENT_संदेश_UPDATE:
			वापस "persist-client-msg-update";
		हाल MOSQ_EVT_PERSIST_WILL_ADD:
			वापस "persist-will-add";
		हाल MOSQ_EVT_PERSIST_WILL_DELETE:
			वापस "persist-will-delete";
	}
	वापस "";
}


अटल bool check_callback_exists(काष्ठा mosquitto__callback *cb_base, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	काष्ठा mosquitto__callback *पुच्छ, *tmp;

	DL_FOREACH_SAFE(cb_base, पुच्छ, tmp){
		अगर(पुच्छ->identifier == identifier && पुच्छ->cb == cb_func){
			वापस true;
		}
	}
	वापस false;
}

अटल काष्ठा mosquitto__callback **plugin__get_callback_base(काष्ठा mosquitto__security_options *security_options, क्रमागत mosquitto_plugin_event event)
{
	चयन(event){
		हाल MOSQ_EVT_RELOAD:
			वापस &security_options->plugin_callbacks.reload;
		हाल MOSQ_EVT_ACL_CHECK:
			वापस &security_options->plugin_callbacks.acl_check;
		हाल MOSQ_EVT_BASIC_AUTH:
			वापस &security_options->plugin_callbacks.basic_auth;
		हाल MOSQ_EVT_PSK_कुंजी:
			वापस &security_options->plugin_callbacks.psk_कुंजी;
		हाल MOSQ_EVT_EXT_AUTH_START:
			वापस &security_options->plugin_callbacks.ext_auth_start;
		हाल MOSQ_EVT_EXT_AUTH_CONTINUE:
			वापस &security_options->plugin_callbacks.ext_auth_continue;
		हाल MOSQ_EVT_CONTROL:
			वापस शून्य;
		हाल MOSQ_EVT_MESSAGE_IN: /* same as MOSQ_EVT_MESSAGE */
			वापस &security_options->plugin_callbacks.message_in;
		हाल MOSQ_EVT_TICK:
			वापस &security_options->plugin_callbacks.tick;
		हाल MOSQ_EVT_DISCONNECT:
			वापस &security_options->plugin_callbacks.disconnect;
		हाल MOSQ_EVT_CONNECT:
			वापस &security_options->plugin_callbacks.connect;
		हाल MOSQ_EVT_CLIENT_OFFLINE:
			वापस &security_options->plugin_callbacks.client_offline;
		हाल MOSQ_EVT_SUBSCRIBE:
			वापस &security_options->plugin_callbacks.subscribe;
		हाल MOSQ_EVT_UNSUBSCRIBE:
			वापस &security_options->plugin_callbacks.unsubscribe;
		हाल MOSQ_EVT_PERSIST_RESTORE:
			वापस &security_options->plugin_callbacks.persist_restore;
		हाल MOSQ_EVT_PERSIST_CLIENT_ADD:
			वापस &security_options->plugin_callbacks.persist_client_add;
		हाल MOSQ_EVT_PERSIST_CLIENT_DELETE:
			वापस &security_options->plugin_callbacks.persist_client_delete;
		हाल MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			वापस &security_options->plugin_callbacks.persist_client_update;
		हाल MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			वापस &security_options->plugin_callbacks.persist_subscription_add;
		हाल MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			वापस &security_options->plugin_callbacks.persist_subscription_delete;
		हाल MOSQ_EVT_PERSIST_CLIENT_संदेश_ADD:
			वापस &security_options->plugin_callbacks.persist_client_संदेश_add;
		हाल MOSQ_EVT_PERSIST_CLIENT_संदेश_DELETE:
			वापस &security_options->plugin_callbacks.persist_client_संदेश_delete;
		हाल MOSQ_EVT_PERSIST_CLIENT_संदेश_UPDATE:
			वापस &security_options->plugin_callbacks.persist_client_संदेश_update;
		हाल MOSQ_EVT_PERSIST_BASE_संदेश_ADD:
			वापस &security_options->plugin_callbacks.persist_base_संदेश_add;
		हाल MOSQ_EVT_PERSIST_BASE_संदेश_DELETE:
			वापस &security_options->plugin_callbacks.persist_base_संदेश_delete;
		हाल MOSQ_EVT_PERSIST_RETAIN_संदेश_SET:
			वापस &security_options->plugin_callbacks.persist_retain_संदेश_set;
		हाल MOSQ_EVT_PERSIST_RETAIN_संदेश_DELETE:
			वापस &security_options->plugin_callbacks.persist_retain_संदेश_delete;
		हाल MOSQ_EVT_MESSAGE_OUT:
			वापस &security_options->plugin_callbacks.message_out;
		हाल MOSQ_EVT_PERSIST_WILL_ADD:
			वापस &security_options->plugin_callbacks.persist_will_add;
		हाल MOSQ_EVT_PERSIST_WILL_DELETE:
			वापस &security_options->plugin_callbacks.persist_will_delete;
	}
	वापस शून्य;
}


अटल पूर्णांक remove_callback(mosquitto_plugin_id_t *plugin, काष्ठा plugin_own_callback *own)
{
	काष्ठा mosquitto__security_options *security_options;
	काष्ठा mosquitto__callback *पुच्छ, *tmp;
	काष्ठा mosquitto__callback **cb_base = शून्य;

	क्रम(पूर्णांक i=0; i<plugin->config.security_option_गिनती; i++){
		security_options = plugin->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, own->event);
		अगर(cb_base == शून्य){
			वापस MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_SAFE(*cb_base, पुच्छ, tmp){
			अगर(पुच्छ->identifier == plugin && पुच्छ->cb == own->cb_func){
				DL_DELETE(*cb_base, पुच्छ);
				mosquitto_FREE(पुच्छ);
				अवरोध;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	वापस MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णांक mosquitto_callback_register(
		mosquitto_plugin_id_t *identifier,
		पूर्णांक event,
		MOSQ_FUNC_generic_callback cb_func,
		स्थिर व्योम *event_तथ्य,
		व्योम *userdata)
{
	काष्ठा mosquitto__callback **cb_base = शून्य, *cb_new;
	काष्ठा mosquitto__security_options *security_options;
	काष्ठा plugin_own_callback *own_callback;

	अगर(cb_func == शून्य){
		वापस MOSQ_ERR_INVAL;
	}

	अगर(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
			|| event == MOSQ_EVT_PERSIST_BASE_संदेश_ADD
			|| event == MOSQ_EVT_PERSIST_BASE_संदेश_DELETE
			|| event == MOSQ_EVT_PERSIST_RETAIN_संदेश_SET
			|| event == MOSQ_EVT_PERSIST_RETAIN_संदेश_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_UPDATE
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_संदेश_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_संदेश_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_संदेश_UPDATE
			|| event == MOSQ_EVT_PERSIST_WILL_ADD
			|| event == MOSQ_EVT_PERSIST_WILL_DELETE
			)){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		वापस MOSQ_ERR_INVAL;
	}

	अगर(event == MOSQ_EVT_CONTROL){
		वापस control__register_callback(identifier, cb_func, event_तथ्य, userdata);
	}

	own_callback = mosquitto_calloc(1, माप(काष्ठा plugin_own_callback));
	अगर(own_callback == शून्य){
		वापस MOSQ_ERR_NOMEM;
	}
	own_callback->event = (क्रमागत mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	अगर(identifier->config.security_option_गिनती == 0){
		log__printf(शून्य, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((क्रमागत mosquitto_plugin_event)event));
		वापस MOSQ_ERR_INVAL;
	}

	क्रम(पूर्णांक i=0; i<identifier->config.security_option_गिनती; i++){
		security_options = identifier->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, (क्रमागत mosquitto_plugin_event)event);
		अगर(cb_base == शून्य){
			वापस MOSQ_ERR_NOT_SUPPORTED;
		}

		अगर(check_callback_exists(*cb_base, identifier, cb_func)){
			वापस MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, माप(काष्ठा mosquitto__callback));
		अगर(cb_new == शून्य){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			वापस MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_base, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	अगर(identifier->plugin_name){
		log__printf(शून्य, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((क्रमागत mosquitto_plugin_event)event));
	}अन्यथा{
		log__printf(शून्य, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((क्रमागत mosquitto_plugin_event)event));
	}

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	काष्ठा plugin_own_callback *own, *own_tmp;

	अगर(plugin == शून्य){
		वापस MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_SAFE(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	वापस MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णांक mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		पूर्णांक event,
		MOSQ_FUNC_generic_callback cb_func,
		स्थिर व्योम *event_तथ्य)
{
	काष्ठा plugin_own_callback *own, *own_tmp;

	अगर(identifier == शून्य || cb_func == शून्य){
		वापस MOSQ_ERR_INVAL;
	}

	अगर(event == MOSQ_EVT_CONTROL){
		वापस control__unregister_callback(identifier, cb_func, event_तथ्य);
	}

	DL_FOREACH_SAFE(identifier->own_callbacks, own, own_tmp){
		अगर(own->event == (क्रमागत mosquitto_plugin_event)event && own->cb_func == cb_func){
			वापस remove_callback(identifier, own);
		}
	}

	वापस MOSQ_ERR_NOT_FOUND;
}
