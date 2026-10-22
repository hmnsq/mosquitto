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


स्थिर स्थिरांक अक्षर *get_event_name(गणना mosquitto_plugin_event event)
{
	निवड(event){
		स्थिती MOSQ_EVT_RELOAD:
			परत "reload";
		स्थिती MOSQ_EVT_ACL_CHECK:
			परत "acl-check";
		स्थिती MOSQ_EVT_BASIC_AUTH:
			परत "basic-auth";
		स्थिती MOSQ_EVT_PSK_किल्ली:
			परत "psk-key";
		स्थिती MOSQ_EVT_EXT_AUTH_START:
			परत "auth-start";
		स्थिती MOSQ_EVT_EXT_AUTH_CONTINUE:
			परत "auth-continue";
		स्थिती MOSQ_EVT_CONTROL:
			परत "control";
		स्थिती MOSQ_EVT_MESSAGE_IN:
			परत "message-in";
		स्थिती MOSQ_EVT_MESSAGE_OUT:
			परत "message-out";
		स्थिती MOSQ_EVT_TICK:
			परत "tick";
		स्थिती MOSQ_EVT_DISCONNECT:
			परत "disconnect";
		स्थिती MOSQ_EVT_CONNECT:
			परत "connect";
		स्थिती MOSQ_EVT_CLIENT_OFFLINE:
			परत "connect";
		स्थिती MOSQ_EVT_SUBSCRIBE:
			परत "subscribe";
		स्थिती MOSQ_EVT_UNSUBSCRIBE:
			परत "unsubscribe";
		स्थिती MOSQ_EVT_PERSIST_RESTORE:
			परत "persist-restore";
		स्थिती MOSQ_EVT_PERSIST_BASE_संदेश_ADD:
			परत "persist-base-msg-add";
		स्थिती MOSQ_EVT_PERSIST_BASE_संदेश_DELETE:
			परत "persist-base-msg-delete";
		स्थिती MOSQ_EVT_PERSIST_RETAIN_संदेश_SET:
			परत "persist-retain-msg-set";
		स्थिती MOSQ_EVT_PERSIST_RETAIN_संदेश_DELETE:
			परत "persist-retain-msg-delete";
		स्थिती MOSQ_EVT_PERSIST_CLIENT_ADD:
			परत "persist-client-add";
		स्थिती MOSQ_EVT_PERSIST_CLIENT_DELETE:
			परत "persist-client-delete";
		स्थिती MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			परत "persist-client-update";
		स्थिती MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			परत "persist-subscription-add";
		स्थिती MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			परत "persist-subscription-delete";
		स्थिती MOSQ_EVT_PERSIST_CLIENT_संदेश_ADD:
			परत "persist-client-msg-add";
		स्थिती MOSQ_EVT_PERSIST_CLIENT_संदेश_DELETE:
			परत "persist-client-msg-delete";
		स्थिती MOSQ_EVT_PERSIST_CLIENT_संदेश_UPDATE:
			परत "persist-client-msg-update";
		स्थिती MOSQ_EVT_PERSIST_WILL_ADD:
			परत "persist-will-add";
		स्थिती MOSQ_EVT_PERSIST_WILL_DELETE:
			परत "persist-will-delete";
	}
	परत "";
}


स्थिर bool check_callback_exists(संरचना mosquitto__callback *cb_base, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	संरचना mosquitto__callback *tail, *tmp;

	DL_FOREACH_SAFE(cb_base, tail, tmp){
		जर(tail->identifier == identifier && tail->cb == cb_func){
			परत true;
		}
	}
	परत false;
}

स्थिर संरचना mosquitto__callback **plugin__get_callback_base(संरचना mosquitto__security_options *security_options, गणना mosquitto_plugin_event event)
{
	निवड(event){
		स्थिती MOSQ_EVT_RELOAD:
			परत &security_options->plugin_callbacks.reload;
		स्थिती MOSQ_EVT_ACL_CHECK:
			परत &security_options->plugin_callbacks.acl_check;
		स्थिती MOSQ_EVT_BASIC_AUTH:
			परत &security_options->plugin_callbacks.basic_auth;
		स्थिती MOSQ_EVT_PSK_किल्ली:
			परत &security_options->plugin_callbacks.psk_किल्ली;
		स्थिती MOSQ_EVT_EXT_AUTH_START:
			परत &security_options->plugin_callbacks.ext_auth_start;
		स्थिती MOSQ_EVT_EXT_AUTH_CONTINUE:
			परत &security_options->plugin_callbacks.ext_auth_continue;
		स्थिती MOSQ_EVT_CONTROL:
			परत रिक्त;
		स्थिती MOSQ_EVT_MESSAGE_IN: /* same as MOSQ_EVT_MESSAGE */
			परत &security_options->plugin_callbacks.message_in;
		स्थिती MOSQ_EVT_TICK:
			परत &security_options->plugin_callbacks.tick;
		स्थिती MOSQ_EVT_DISCONNECT:
			परत &security_options->plugin_callbacks.disconnect;
		स्थिती MOSQ_EVT_CONNECT:
			परत &security_options->plugin_callbacks.connect;
		स्थिती MOSQ_EVT_CLIENT_OFFLINE:
			परत &security_options->plugin_callbacks.client_offline;
		स्थिती MOSQ_EVT_SUBSCRIBE:
			परत &security_options->plugin_callbacks.subscribe;
		स्थिती MOSQ_EVT_UNSUBSCRIBE:
			परत &security_options->plugin_callbacks.unsubscribe;
		स्थिती MOSQ_EVT_PERSIST_RESTORE:
			परत &security_options->plugin_callbacks.persist_restore;
		स्थिती MOSQ_EVT_PERSIST_CLIENT_ADD:
			परत &security_options->plugin_callbacks.persist_client_add;
		स्थिती MOSQ_EVT_PERSIST_CLIENT_DELETE:
			परत &security_options->plugin_callbacks.persist_client_delete;
		स्थिती MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			परत &security_options->plugin_callbacks.persist_client_update;
		स्थिती MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			परत &security_options->plugin_callbacks.persist_subscription_add;
		स्थिती MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			परत &security_options->plugin_callbacks.persist_subscription_delete;
		स्थिती MOSQ_EVT_PERSIST_CLIENT_संदेश_ADD:
			परत &security_options->plugin_callbacks.persist_client_संदेश_add;
		स्थिती MOSQ_EVT_PERSIST_CLIENT_संदेश_DELETE:
			परत &security_options->plugin_callbacks.persist_client_संदेश_delete;
		स्थिती MOSQ_EVT_PERSIST_CLIENT_संदेश_UPDATE:
			परत &security_options->plugin_callbacks.persist_client_संदेश_update;
		स्थिती MOSQ_EVT_PERSIST_BASE_संदेश_ADD:
			परत &security_options->plugin_callbacks.persist_base_संदेश_add;
		स्थिती MOSQ_EVT_PERSIST_BASE_संदेश_DELETE:
			परत &security_options->plugin_callbacks.persist_base_संदेश_delete;
		स्थिती MOSQ_EVT_PERSIST_RETAIN_संदेश_SET:
			परत &security_options->plugin_callbacks.persist_retain_संदेश_set;
		स्थिती MOSQ_EVT_PERSIST_RETAIN_संदेश_DELETE:
			परत &security_options->plugin_callbacks.persist_retain_संदेश_delete;
		स्थिती MOSQ_EVT_MESSAGE_OUT:
			परत &security_options->plugin_callbacks.message_out;
		स्थिती MOSQ_EVT_PERSIST_WILL_ADD:
			परत &security_options->plugin_callbacks.persist_will_add;
		स्थिती MOSQ_EVT_PERSIST_WILL_DELETE:
			परत &security_options->plugin_callbacks.persist_will_delete;
	}
	परत रिक्त;
}


स्थिर पूर्णांक remove_callback(mosquitto_plugin_id_t *plugin, संरचना plugin_own_callback *own)
{
	संरचना mosquitto__security_options *security_options;
	संरचना mosquitto__callback *tail, *tmp;
	संरचना mosquitto__callback **cb_base = रिक्त;

	साठी(पूर्णांक i=0; i<plugin->config.security_option_मोजणी; i++){
		security_options = plugin->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, own->event);
		जर(cb_base == रिक्त){
			परत MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_SAFE(*cb_base, tail, tmp){
			जर(tail->identifier == plugin && tail->cb == own->cb_func){
				DL_DELETE(*cb_base, tail);
				mosquitto_FREE(tail);
				थांबा;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	परत MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णांक mosquitto_callback_register(
		mosquitto_plugin_id_t *identifier,
		पूर्णांक event,
		MOSQ_FUNC_generic_callback cb_func,
		स्थिरांक शून्य *event_data,
		शून्य *userdata)
{
	संरचना mosquitto__callback **cb_base = रिक्त, *cb_new;
	संरचना mosquitto__security_options *security_options;
	संरचना plugin_own_callback *own_callback;

	जर(cb_func == रिक्त){
		परत MOSQ_ERR_INVAL;
	}

	जर(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
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
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		परत MOSQ_ERR_INVAL;
	}

	जर(event == MOSQ_EVT_CONTROL){
		परत control__register_callback(identifier, cb_func, event_data, userdata);
	}

	own_callback = mosquitto_calloc(1, आकार(संरचना plugin_own_callback));
	जर(own_callback == रिक्त){
		परत MOSQ_ERR_NOMEM;
	}
	own_callback->event = (गणना mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	जर(identifier->config.security_option_मोजणी == 0){
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((गणना mosquitto_plugin_event)event));
		परत MOSQ_ERR_INVAL;
	}

	साठी(पूर्णांक i=0; i<identifier->config.security_option_मोजणी; i++){
		security_options = identifier->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, (गणना mosquitto_plugin_event)event);
		जर(cb_base == रिक्त){
			परत MOSQ_ERR_NOT_SUPPORTED;
		}

		जर(check_callback_exists(*cb_base, identifier, cb_func)){
			परत MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, आकार(संरचना mosquitto__callback));
		जर(cb_new == रिक्त){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			परत MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_base, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	जर(identifier->plugin_name){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((गणना mosquitto_plugin_event)event));
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((गणना mosquitto_plugin_event)event));
	}

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	संरचना plugin_own_callback *own, *own_tmp;

	जर(plugin == रिक्त){
		परत MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_SAFE(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	परत MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णांक mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		पूर्णांक event,
		MOSQ_FUNC_generic_callback cb_func,
		स्थिरांक शून्य *event_data)
{
	संरचना plugin_own_callback *own, *own_tmp;

	जर(identifier == रिक्त || cb_func == रिक्त){
		परत MOSQ_ERR_INVAL;
	}

	जर(event == MOSQ_EVT_CONTROL){
		परत control__unregister_callback(identifier, cb_func, event_data);
	}

	DL_FOREACH_SAFE(identifier->own_callbacks, own, own_tmp){
		जर(own->event == (गणना mosquitto_plugin_event)event && own->cb_func == cb_func){
			परत remove_callback(identifier, own);
		}
	}

	परत MOSQ_ERR_NOT_FOUND;
}
