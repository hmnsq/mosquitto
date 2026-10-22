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


स्थिर अचर वर्ण *get_event_name(गणना mosquitto_plugin_event event)
{
	विकल्प(event){
		पक्ष MOSQ_EVT_RELOAD:
			फर्काऊ "reload";
		पक्ष MOSQ_EVT_ACL_CHECK:
			फर्काऊ "acl-check";
		पक्ष MOSQ_EVT_BASIC_AUTH:
			फर्काऊ "basic-auth";
		पक्ष MOSQ_EVT_PSK_साँचो:
			फर्काऊ "psk-key";
		पक्ष MOSQ_EVT_EXT_AUTH_START:
			फर्काऊ "auth-start";
		पक्ष MOSQ_EVT_EXT_AUTH_CONTINUE:
			फर्काऊ "auth-continue";
		पक्ष MOSQ_EVT_CONTROL:
			फर्काऊ "control";
		पक्ष MOSQ_EVT_MESSAGE_IN:
			फर्काऊ "message-in";
		पक्ष MOSQ_EVT_MESSAGE_OUT:
			फर्काऊ "message-out";
		पक्ष MOSQ_EVT_TICK:
			फर्काऊ "tick";
		पक्ष MOSQ_EVT_DISCONNECT:
			फर्काऊ "disconnect";
		पक्ष MOSQ_EVT_CONNECT:
			फर्काऊ "connect";
		पक्ष MOSQ_EVT_CLIENT_OFFLINE:
			फर्काऊ "connect";
		पक्ष MOSQ_EVT_SUBSCRIBE:
			फर्काऊ "subscribe";
		पक्ष MOSQ_EVT_UNSUBSCRIBE:
			फर्काऊ "unsubscribe";
		पक्ष MOSQ_EVT_PERSIST_RESTORE:
			फर्काऊ "persist-restore";
		पक्ष MOSQ_EVT_PERSIST_BASE_सन्देश_ADD:
			फर्काऊ "persist-base-msg-add";
		पक्ष MOSQ_EVT_PERSIST_BASE_सन्देश_DELETE:
			फर्काऊ "persist-base-msg-delete";
		पक्ष MOSQ_EVT_PERSIST_RETAIN_सन्देश_SET:
			फर्काऊ "persist-retain-msg-set";
		पक्ष MOSQ_EVT_PERSIST_RETAIN_सन्देश_DELETE:
			फर्काऊ "persist-retain-msg-delete";
		पक्ष MOSQ_EVT_PERSIST_CLIENT_ADD:
			फर्काऊ "persist-client-add";
		पक्ष MOSQ_EVT_PERSIST_CLIENT_DELETE:
			फर्काऊ "persist-client-delete";
		पक्ष MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			फर्काऊ "persist-client-update";
		पक्ष MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			फर्काऊ "persist-subscription-add";
		पक्ष MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			फर्काऊ "persist-subscription-delete";
		पक्ष MOSQ_EVT_PERSIST_CLIENT_सन्देश_ADD:
			फर्काऊ "persist-client-msg-add";
		पक्ष MOSQ_EVT_PERSIST_CLIENT_सन्देश_DELETE:
			फर्काऊ "persist-client-msg-delete";
		पक्ष MOSQ_EVT_PERSIST_CLIENT_सन्देश_UPDATE:
			फर्काऊ "persist-client-msg-update";
		पक्ष MOSQ_EVT_PERSIST_WILL_ADD:
			फर्काऊ "persist-will-add";
		पक्ष MOSQ_EVT_PERSIST_WILL_DELETE:
			फर्काऊ "persist-will-delete";
	}
	फर्काऊ "";
}


स्थिर bool check_callback_exists(संरचना mosquitto__callback *cb_base, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	संरचना mosquitto__callback *tail, *tmp;

	DL_FOREACH_SAFE(cb_base, tail, tmp){
		यदि(tail->identifier == identifier && tail->cb == cb_func){
			फर्काऊ true;
		}
	}
	फर्काऊ false;
}

स्थिर संरचना mosquitto__callback **plugin__get_callback_base(संरचना mosquitto__security_options *security_options, गणना mosquitto_plugin_event event)
{
	विकल्प(event){
		पक्ष MOSQ_EVT_RELOAD:
			फर्काऊ &security_options->plugin_callbacks.reload;
		पक्ष MOSQ_EVT_ACL_CHECK:
			फर्काऊ &security_options->plugin_callbacks.acl_check;
		पक्ष MOSQ_EVT_BASIC_AUTH:
			फर्काऊ &security_options->plugin_callbacks.basic_auth;
		पक्ष MOSQ_EVT_PSK_साँचो:
			फर्काऊ &security_options->plugin_callbacks.psk_साँचो;
		पक्ष MOSQ_EVT_EXT_AUTH_START:
			फर्काऊ &security_options->plugin_callbacks.ext_auth_start;
		पक्ष MOSQ_EVT_EXT_AUTH_CONTINUE:
			फर्काऊ &security_options->plugin_callbacks.ext_auth_continue;
		पक्ष MOSQ_EVT_CONTROL:
			फर्काऊ रिक्त;
		पक्ष MOSQ_EVT_MESSAGE_IN: /* same as MOSQ_EVT_MESSAGE */
			फर्काऊ &security_options->plugin_callbacks.message_in;
		पक्ष MOSQ_EVT_TICK:
			फर्काऊ &security_options->plugin_callbacks.tick;
		पक्ष MOSQ_EVT_DISCONNECT:
			फर्काऊ &security_options->plugin_callbacks.disconnect;
		पक्ष MOSQ_EVT_CONNECT:
			फर्काऊ &security_options->plugin_callbacks.connect;
		पक्ष MOSQ_EVT_CLIENT_OFFLINE:
			फर्काऊ &security_options->plugin_callbacks.client_offline;
		पक्ष MOSQ_EVT_SUBSCRIBE:
			फर्काऊ &security_options->plugin_callbacks.subscribe;
		पक्ष MOSQ_EVT_UNSUBSCRIBE:
			फर्काऊ &security_options->plugin_callbacks.unsubscribe;
		पक्ष MOSQ_EVT_PERSIST_RESTORE:
			फर्काऊ &security_options->plugin_callbacks.persist_restore;
		पक्ष MOSQ_EVT_PERSIST_CLIENT_ADD:
			फर्काऊ &security_options->plugin_callbacks.persist_client_add;
		पक्ष MOSQ_EVT_PERSIST_CLIENT_DELETE:
			फर्काऊ &security_options->plugin_callbacks.persist_client_delete;
		पक्ष MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			फर्काऊ &security_options->plugin_callbacks.persist_client_update;
		पक्ष MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD:
			फर्काऊ &security_options->plugin_callbacks.persist_subscription_add;
		पक्ष MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			फर्काऊ &security_options->plugin_callbacks.persist_subscription_delete;
		पक्ष MOSQ_EVT_PERSIST_CLIENT_सन्देश_ADD:
			फर्काऊ &security_options->plugin_callbacks.persist_client_सन्देश_add;
		पक्ष MOSQ_EVT_PERSIST_CLIENT_सन्देश_DELETE:
			फर्काऊ &security_options->plugin_callbacks.persist_client_सन्देश_delete;
		पक्ष MOSQ_EVT_PERSIST_CLIENT_सन्देश_UPDATE:
			फर्काऊ &security_options->plugin_callbacks.persist_client_सन्देश_update;
		पक्ष MOSQ_EVT_PERSIST_BASE_सन्देश_ADD:
			फर्काऊ &security_options->plugin_callbacks.persist_base_सन्देश_add;
		पक्ष MOSQ_EVT_PERSIST_BASE_सन्देश_DELETE:
			फर्काऊ &security_options->plugin_callbacks.persist_base_सन्देश_delete;
		पक्ष MOSQ_EVT_PERSIST_RETAIN_सन्देश_SET:
			फर्काऊ &security_options->plugin_callbacks.persist_retain_सन्देश_set;
		पक्ष MOSQ_EVT_PERSIST_RETAIN_सन्देश_DELETE:
			फर्काऊ &security_options->plugin_callbacks.persist_retain_सन्देश_delete;
		पक्ष MOSQ_EVT_MESSAGE_OUT:
			फर्काऊ &security_options->plugin_callbacks.message_out;
		पक्ष MOSQ_EVT_PERSIST_WILL_ADD:
			फर्काऊ &security_options->plugin_callbacks.persist_will_add;
		पक्ष MOSQ_EVT_PERSIST_WILL_DELETE:
			फर्काऊ &security_options->plugin_callbacks.persist_will_delete;
	}
	फर्काऊ रिक्त;
}


स्थिर पूर्णाङ्क remove_callback(mosquitto_plugin_id_t *plugin, संरचना plugin_own_callback *own)
{
	संरचना mosquitto__security_options *security_options;
	संरचना mosquitto__callback *tail, *tmp;
	संरचना mosquitto__callback **cb_base = रिक्त;

	लागि(पूर्णाङ्क i=0; i<plugin->config.security_option_गन्ती; i++){
		security_options = plugin->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, own->event);
		यदि(cb_base == रिक्त){
			फर्काऊ MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_SAFE(*cb_base, tail, tmp){
			यदि(tail->identifier == plugin && tail->cb == own->cb_func){
				DL_DELETE(*cb_base, tail);
				mosquitto_FREE(tail);
				रोक;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	फर्काऊ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_callback_register(
		mosquitto_plugin_id_t *identifier,
		पूर्णाङ्क event,
		MOSQ_FUNC_generic_callback cb_func,
		अचर शून्य *event_data,
		शून्य *userdata)
{
	संरचना mosquitto__callback **cb_base = रिक्त, *cb_new;
	संरचना mosquitto__security_options *security_options;
	संरचना plugin_own_callback *own_callback;

	यदि(cb_func == रिक्त){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	यदि(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
			|| event == MOSQ_EVT_PERSIST_BASE_सन्देश_ADD
			|| event == MOSQ_EVT_PERSIST_BASE_सन्देश_DELETE
			|| event == MOSQ_EVT_PERSIST_RETAIN_सन्देश_SET
			|| event == MOSQ_EVT_PERSIST_RETAIN_सन्देश_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_UPDATE
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_ADD
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_सन्देश_ADD
			|| event == MOSQ_EVT_PERSIST_CLIENT_सन्देश_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_सन्देश_UPDATE
			|| event == MOSQ_EVT_PERSIST_WILL_ADD
			|| event == MOSQ_EVT_PERSIST_WILL_DELETE
			)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		फर्काऊ MOSQ_ERR_INVAL;
	}

	यदि(event == MOSQ_EVT_CONTROL){
		फर्काऊ control__register_callback(identifier, cb_func, event_data, userdata);
	}

	own_callback = mosquitto_calloc(1, परिमाण(संरचना plugin_own_callback));
	यदि(own_callback == रिक्त){
		फर्काऊ MOSQ_ERR_NOMEM;
	}
	own_callback->event = (गणना mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	यदि(identifier->config.security_option_गन्ती == 0){
		log__printf(रिक्त, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((गणना mosquitto_plugin_event)event));
		फर्काऊ MOSQ_ERR_INVAL;
	}

	लागि(पूर्णाङ्क i=0; i<identifier->config.security_option_गन्ती; i++){
		security_options = identifier->config.security_options[i];

		cb_base = plugin__get_callback_base(security_options, (गणना mosquitto_plugin_event)event);
		यदि(cb_base == रिक्त){
			फर्काऊ MOSQ_ERR_NOT_SUPPORTED;
		}

		यदि(check_callback_exists(*cb_base, identifier, cb_func)){
			फर्काऊ MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, परिमाण(संरचना mosquitto__callback));
		यदि(cb_new == रिक्त){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			फर्काऊ MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_base, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	यदि(identifier->plugin_name){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((गणना mosquitto_plugin_event)event));
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((गणना mosquitto_plugin_event)event));
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	संरचना plugin_own_callback *own, *own_tmp;

	यदि(plugin == रिक्त){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_SAFE(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	फर्काऊ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		पूर्णाङ्क event,
		MOSQ_FUNC_generic_callback cb_func,
		अचर शून्य *event_data)
{
	संरचना plugin_own_callback *own, *own_tmp;

	यदि(identifier == रिक्त || cb_func == रिक्त){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	यदि(event == MOSQ_EVT_CONTROL){
		फर्काऊ control__unregister_callback(identifier, cb_func, event_data);
	}

	DL_FOREACH_SAFE(identifier->own_callbacks, own, own_tmp){
		यदि(own->event == (गणना mosquitto_plugin_event)event && own->cb_func == cb_func){
			फर्काऊ remove_callback(identifier, own);
		}
	}

	फर्काऊ MOSQ_ERR_NOT_FOUND;
}
