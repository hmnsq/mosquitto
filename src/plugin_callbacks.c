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

#అన్తర్భావ "config.h"

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "utlist.h"
#అన్తర్భావ "lib_load.h"


స్థిర స్థిరరాశి అక్షరం *get_event_name(గణన mosquitto_plugin_event event)
{
	ఎంపిక(event){
		స్థితి MOSQ_EVT_RELOAD:
			తిరిగి "reload";
		స్థితి MOSQ_EVT_ACL_CHECK:
			తిరిగి "acl-check";
		స్థితి MOSQ_EVT_BASIC_AUTH:
			తిరిగి "basic-auth";
		స్థితి MOSQ_EVT_PSK_కీలకం:
			తిరిగి "psk-key";
		స్థితి MOSQ_EVT_EXT_AUTH_ప్రారంభం:
			తిరిగి "auth-start";
		స్థితి MOSQ_EVT_EXT_AUTH_CONTINUE:
			తిరిగి "auth-continue";
		స్థితి MOSQ_EVT_CONTROL:
			తిరిగి "control";
		స్థితి MOSQ_EVT_సందేశం_లోపల:
			తిరిగి "message-in";
		స్థితి MOSQ_EVT_సందేశం_బహిర్గతం:
			తిరిగి "message-out";
		స్థితి MOSQ_EVT_TICK:
			తిరిగి "tick";
		స్థితి MOSQ_EVT_DISCONNECT:
			తిరిగి "disconnect";
		స్థితి MOSQ_EVT_CONNECT:
			తిరిగి "connect";
		స్థితి MOSQ_EVT_CLIENT_OFFLINE:
			తిరిగి "connect";
		స్థితి MOSQ_EVT_SUBSCRIBE:
			తిరిగి "subscribe";
		స్థితి MOSQ_EVT_UNSUBSCRIBE:
			తిరిగి "unsubscribe";
		స్థితి MOSQ_EVT_PERSIST_RESTORE:
			తిరిగి "persist-restore";
		స్థితి MOSQ_EVT_PERSIST_పునాది_సందేశం_కూడు:
			తిరిగి "persist-base-msg-add";
		స్థితి MOSQ_EVT_PERSIST_పునాది_సందేశం_DELETE:
			తిరిగి "persist-base-msg-delete";
		స్థితి MOSQ_EVT_PERSIST_RETAIN_సందేశం_సమితి:
			తిరిగి "persist-retain-msg-set";
		స్థితి MOSQ_EVT_PERSIST_RETAIN_సందేశం_DELETE:
			తిరిగి "persist-retain-msg-delete";
		స్థితి MOSQ_EVT_PERSIST_CLIENT_కూడు:
			తిరిగి "persist-client-add";
		స్థితి MOSQ_EVT_PERSIST_CLIENT_DELETE:
			తిరిగి "persist-client-delete";
		స్థితి MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			తిరిగి "persist-client-update";
		స్థితి MOSQ_EVT_PERSIST_SUBSCRIPTION_కూడు:
			తిరిగి "persist-subscription-add";
		స్థితి MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			తిరిగి "persist-subscription-delete";
		స్థితి MOSQ_EVT_PERSIST_CLIENT_సందేశం_కూడు:
			తిరిగి "persist-client-msg-add";
		స్థితి MOSQ_EVT_PERSIST_CLIENT_సందేశం_DELETE:
			తిరిగి "persist-client-msg-delete";
		స్థితి MOSQ_EVT_PERSIST_CLIENT_సందేశం_UPDATE:
			తిరిగి "persist-client-msg-update";
		స్థితి MOSQ_EVT_PERSIST_WILL_కూడు:
			తిరిగి "persist-will-add";
		స్థితి MOSQ_EVT_PERSIST_WILL_DELETE:
			తిరిగి "persist-will-delete";
	}
	తిరిగి "";
}


స్థిర bool check_callback_exists(నిర్మాణం mosquitto__callback *cb_పునాది, mosquitto_plugin_id_t *identifier, MOSQ_FUNC_generic_callback cb_func)
{
	నిర్మాణం mosquitto__callback *తోక, *tmp;

	DL_FOREACH_సురక్షిత(cb_పునాది, తోక, tmp){
		ఒకవేళ(తోక->identifier == identifier && తోక->cb == cb_func){
			తిరిగి true;
		}
	}
	తిరిగి false;
}

స్థిర నిర్మాణం mosquitto__callback **plugin_get_callback_పునాది(నిర్మాణం mosquitto__security_options *security_options, గణన mosquitto_plugin_event event)
{
	ఎంపిక(event){
		స్థితి MOSQ_EVT_RELOAD:
			తిరిగి &security_options->plugin_callbacks.reload;
		స్థితి MOSQ_EVT_ACL_CHECK:
			తిరిగి &security_options->plugin_callbacks.acl_check;
		స్థితి MOSQ_EVT_BASIC_AUTH:
			తిరిగి &security_options->plugin_callbacks.basic_auth;
		స్థితి MOSQ_EVT_PSK_కీలకం:
			తిరిగి &security_options->plugin_callbacks.psk_కీలకం;
		స్థితి MOSQ_EVT_EXT_AUTH_ప్రారంభం:
			తిరిగి &security_options->plugin_callbacks.ext_auth_ప్రారంభం;
		స్థితి MOSQ_EVT_EXT_AUTH_CONTINUE:
			తిరిగి &security_options->plugin_callbacks.ext_auth_continue;
		స్థితి MOSQ_EVT_CONTROL:
			తిరిగి ఖాళీ;
		స్థితి MOSQ_EVT_సందేశం_లోపల: /* same as MOSQ_EVT_MESSAGE */
			తిరిగి &security_options->plugin_callbacks.సందేశం_లోపల;
		స్థితి MOSQ_EVT_TICK:
			తిరిగి &security_options->plugin_callbacks.tick;
		స్థితి MOSQ_EVT_DISCONNECT:
			తిరిగి &security_options->plugin_callbacks.disconnect;
		స్థితి MOSQ_EVT_CONNECT:
			తిరిగి &security_options->plugin_callbacks.connect;
		స్థితి MOSQ_EVT_CLIENT_OFFLINE:
			తిరిగి &security_options->plugin_callbacks.client_offline;
		స్థితి MOSQ_EVT_SUBSCRIBE:
			తిరిగి &security_options->plugin_callbacks.subscribe;
		స్థితి MOSQ_EVT_UNSUBSCRIBE:
			తిరిగి &security_options->plugin_callbacks.unsubscribe;
		స్థితి MOSQ_EVT_PERSIST_RESTORE:
			తిరిగి &security_options->plugin_callbacks.persist_restore;
		స్థితి MOSQ_EVT_PERSIST_CLIENT_కూడు:
			తిరిగి &security_options->plugin_callbacks.persist_client_కూడు;
		స్థితి MOSQ_EVT_PERSIST_CLIENT_DELETE:
			తిరిగి &security_options->plugin_callbacks.persist_client_delete;
		స్థితి MOSQ_EVT_PERSIST_CLIENT_UPDATE:
			తిరిగి &security_options->plugin_callbacks.persist_client_update;
		స్థితి MOSQ_EVT_PERSIST_SUBSCRIPTION_కూడు:
			తిరిగి &security_options->plugin_callbacks.persist_subscription_కూడు;
		స్థితి MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE:
			తిరిగి &security_options->plugin_callbacks.persist_subscription_delete;
		స్థితి MOSQ_EVT_PERSIST_CLIENT_సందేశం_కూడు:
			తిరిగి &security_options->plugin_callbacks.persist_client_సందేశం_కూడు;
		స్థితి MOSQ_EVT_PERSIST_CLIENT_సందేశం_DELETE:
			తిరిగి &security_options->plugin_callbacks.persist_client_సందేశం_delete;
		స్థితి MOSQ_EVT_PERSIST_CLIENT_సందేశం_UPDATE:
			తిరిగి &security_options->plugin_callbacks.persist_client_సందేశం_update;
		స్థితి MOSQ_EVT_PERSIST_పునాది_సందేశం_కూడు:
			తిరిగి &security_options->plugin_callbacks.persist_పునాది_సందేశం_కూడు;
		స్థితి MOSQ_EVT_PERSIST_పునాది_సందేశం_DELETE:
			తిరిగి &security_options->plugin_callbacks.persist_పునాది_సందేశం_delete;
		స్థితి MOSQ_EVT_PERSIST_RETAIN_సందేశం_సమితి:
			తిరిగి &security_options->plugin_callbacks.persist_retain_సందేశం_సమితి;
		స్థితి MOSQ_EVT_PERSIST_RETAIN_సందేశం_DELETE:
			తిరిగి &security_options->plugin_callbacks.persist_retain_సందేశం_delete;
		స్థితి MOSQ_EVT_సందేశం_బహిర్గతం:
			తిరిగి &security_options->plugin_callbacks.సందేశం_బహిర్గతం;
		స్థితి MOSQ_EVT_PERSIST_WILL_కూడు:
			తిరిగి &security_options->plugin_callbacks.persist_will_కూడు;
		స్థితి MOSQ_EVT_PERSIST_WILL_DELETE:
			తిరిగి &security_options->plugin_callbacks.persist_will_delete;
	}
	తిరిగి ఖాళీ;
}


స్థిర పూర్ణాంకం remove_callback(mosquitto_plugin_id_t *plugin, నిర్మాణం plugin_own_callback *own)
{
	నిర్మాణం mosquitto__security_options *security_options;
	నిర్మాణం mosquitto__callback *తోక, *tmp;
	నిర్మాణం mosquitto__callback **cb_పునాది = ఖాళీ;

	కొరకు(పూర్ణాంకం i=0; i<plugin->config.security_option_లెక్క; i++){
		security_options = plugin->config.security_options[i];

		cb_పునాది = plugin_get_callback_పునాది(security_options, own->event);
		ఒకవేళ(cb_పునాది == ఖాళీ){
			తిరిగి MOSQ_ERR_NOT_SUPPORTED;
		}

		DL_FOREACH_సురక్షిత(*cb_పునాది, తోక, tmp){
			ఒకవేళ(తోక->identifier == plugin && తోక->cb == own->cb_func){
				DL_DELETE(*cb_పునాది, తోక);
				mosquitto_FREE(తోక);
				ఆపు;
			}
		}
	}

	DL_DELETE(plugin->own_callbacks, own);
	mosquitto_FREE(own);

	తిరిగి MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT పూర్ణాంకం mosquitto_callback_రిజిస్టరు(
		mosquitto_plugin_id_t *identifier,
		పూర్ణాంకం event,
		MOSQ_FUNC_generic_callback cb_func,
		స్థిరరాశి శూన్య *event_దత్తాంశం,
		శూన్య *userdata)
{
	నిర్మాణం mosquitto__callback **cb_పునాది = ఖాళీ, *cb_new;
	నిర్మాణం mosquitto__security_options *security_options;
	నిర్మాణం plugin_own_callback *own_callback;

	ఒకవేళ(cb_func == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}

	ఒకవేళ(db.config->persistence && (event == MOSQ_EVT_PERSIST_RESTORE
			|| event == MOSQ_EVT_PERSIST_పునాది_సందేశం_కూడు
			|| event == MOSQ_EVT_PERSIST_పునాది_సందేశం_DELETE
			|| event == MOSQ_EVT_PERSIST_RETAIN_సందేశం_సమితి
			|| event == MOSQ_EVT_PERSIST_RETAIN_సందేశం_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_కూడు
			|| event == MOSQ_EVT_PERSIST_CLIENT_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_UPDATE
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_కూడు
			|| event == MOSQ_EVT_PERSIST_SUBSCRIPTION_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_సందేశం_కూడు
			|| event == MOSQ_EVT_PERSIST_CLIENT_సందేశం_DELETE
			|| event == MOSQ_EVT_PERSIST_CLIENT_సందేశం_UPDATE
			|| event == MOSQ_EVT_PERSIST_WILL_కూడు
			|| event == MOSQ_EVT_PERSIST_WILL_DELETE
			)){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: `persistence true` cannot be used with a persistence plugin.");
		తిరిగి MOSQ_ERR_INVAL;
	}

	ఒకవేళ(event == MOSQ_EVT_CONTROL){
		తిరిగి control_రిజిస్టరు_callback(identifier, cb_func, event_దత్తాంశం, userdata);
	}

	own_callback = mosquitto_calloc(1, పరిమాణం(నిర్మాణం plugin_own_callback));
	ఒకవేళ(own_callback == ఖాళీ){
		తిరిగి MOSQ_ERR_NOMEM;
	}
	own_callback->event = (గణన mosquitto_plugin_event)event;
	own_callback->cb_func = cb_func;
	DL_APPEND(identifier->own_callbacks, own_callback);

	ఒకవేళ(identifier->config.security_option_లెక్క == 0){
		log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Plugin could not register callback '%s'",
				get_event_name((గణన mosquitto_plugin_event)event));
		తిరిగి MOSQ_ERR_INVAL;
	}

	కొరకు(పూర్ణాంకం i=0; i<identifier->config.security_option_లెక్క; i++){
		security_options = identifier->config.security_options[i];

		cb_పునాది = plugin_get_callback_పునాది(security_options, (గణన mosquitto_plugin_event)event);
		ఒకవేళ(cb_పునాది == ఖాళీ){
			తిరిగి MOSQ_ERR_NOT_SUPPORTED;
		}

		ఒకవేళ(check_callback_exists(*cb_పునాది, identifier, cb_func)){
			తిరిగి MOSQ_ERR_ALREADY_EXISTS;
		}

		cb_new = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto__callback));
		ఒకవేళ(cb_new == ఖాళీ){
			DL_DELETE(identifier->own_callbacks, own_callback);
			mosquitto_FREE(own_callback);
			తిరిగి MOSQ_ERR_NOMEM;
		}

		DL_APPEND(*cb_పునాది, cb_new);
		cb_new->identifier = identifier;
		cb_new->cb = cb_func;
		cb_new->userdata = userdata;
	}

	ఒకవేళ(identifier->plugin_name){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Plugin %s has registered to receive '%s' events.",
				identifier->plugin_name, get_event_name((గణన mosquitto_plugin_event)event));
	}లేకపోతే{
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Plugin has registered to receive '%s' events.",
				get_event_name((గణన mosquitto_plugin_event)event));
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం plugin__callback_unregister_all(mosquitto_plugin_id_t *plugin)
{
	నిర్మాణం plugin_own_callback *own, *own_tmp;

	ఒకవేళ(plugin == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}

	control__unregister_all_callbacks(plugin);

	DL_FOREACH_సురక్షిత(plugin->own_callbacks, own, own_tmp){
		remove_callback(plugin, own);
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT పూర్ణాంకం mosquitto_callback_unregister(
		mosquitto_plugin_id_t *identifier,
		పూర్ణాంకం event,
		MOSQ_FUNC_generic_callback cb_func,
		స్థిరరాశి శూన్య *event_దత్తాంశం)
{
	నిర్మాణం plugin_own_callback *own, *own_tmp;

	ఒకవేళ(identifier == ఖాళీ || cb_func == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}

	ఒకవేళ(event == MOSQ_EVT_CONTROL){
		తిరిగి control__unregister_callback(identifier, cb_func, event_దత్తాంశం);
	}

	DL_FOREACH_సురక్షిత(identifier->own_callbacks, own, own_tmp){
		ఒకవేళ(own->event == (గణన mosquitto_plugin_event)event && own->cb_func == cb_func){
			తిరిగి remove_callback(identifier, own);
		}
	}

	తిరిగి MOSQ_ERR_NOT_దొరికింది;
}
