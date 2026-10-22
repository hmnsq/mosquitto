/*
Copyright (c) 2011-2021 Roger Light <roger@atchoo.org>

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

/* This loads v4 plugins in a v5 wrapper to make the core code cleaner */

#شامل "config.h"

#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto/broker.h"
#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/broker_plugin.h"
#شامل "lib_load.h"
#شامل "utlist.h"

قسمتعریف صحیح (*FUNC_auth_plugin_نسخہ)(خالی);
قسمتعریف صحیح (*FUNC_plugin_نسخہ)(صحیح, ثابت صحیح *);


ساکن صحیح plugin_v4_basic_auth(صحیح event, خالی *event_ڈیٹا, خالی *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ساخت mosquitto_evt_basic_auth *ed = event_ڈیٹا;

	غیر_مستعمل(event);

	اگر(plugin->lib.unpwd_check_v4 == تہی){
		واپس MOSQ_ERR_PLUGIN_DEFER;
	}

	واپس plugin->lib.unpwd_check_v4(
			plugin->lib.user_ڈیٹا,
			ed->client,
			ed->username,
			ed->password);
}


ساکن صحیح plugin_v4_acl_check(صحیح event, خالی *event_ڈیٹا, خالی *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ساخت mosquitto_evt_acl_check *ed = event_ڈیٹا;
	ساخت mosquitto_acl_پیغام پیغام;
	صحیح rc;

	غیر_مستعمل(event);

	اگر(plugin->lib.acl_check_v4 == تہی){
		واپس MOSQ_ERR_PLUGIN_DEFER;
	}

	memset(&پیغام, 0, حجم(پیغام));
	پیغام.topic = ed->topic;
	پیغام.payloadlen = ed->payloadlen;
	پیغام.payload = ed->payload;
	پیغام.qos = ed->qos;
	پیغام.retain = ed->retain;

	rc = acl__pre_check(plugin, ed->client, ed->access);
	اگر(rc == MOSQ_ERR_PLUGIN_DEFER){
		واپس plugin->lib.acl_check_v4(
				plugin->lib.user_ڈیٹا,
				ed->access,
				ed->client,
				&پیغام);
	}ورنہ{
		واپس rc;
	}
}


ساکن صحیح plugin_v4_auth_آغاز(صحیح event, خالی *event_ڈیٹا, خالی *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ساخت mosquitto_evt_extended_auth *ed = event_ڈیٹا;

	غیر_مستعمل(event);

	اگر(plugin->lib.auth_آغاز_v4 == تہی){
		واپس MOSQ_ERR_PLUGIN_DEFER;
	}

	واپس plugin->lib.auth_آغاز_v4(
			plugin->lib.user_ڈیٹا,
			ed->client,
			ed->client->auth_طریقہ,
			false,
			ed->ڈیٹا_اندر, ed->ڈیٹا_اندر_len,
			&ed->ڈیٹا_برآمد, &ed->ڈیٹا_برآمد_len);
}


ساکن صحیح plugin_v4_auth_continue(صحیح event, خالی *event_ڈیٹا, خالی *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ساخت mosquitto_evt_extended_auth *ed = event_ڈیٹا;

	غیر_مستعمل(event);

	اگر(plugin->lib.auth_continue_v4 == تہی){
		واپس MOSQ_ERR_PLUGIN_DEFER;
	}

	واپس plugin->lib.auth_continue_v4(
			plugin->lib.user_ڈیٹا,
			ed->client,
			ed->client->auth_طریقہ,
			ed->ڈیٹا_اندر, ed->ڈیٹا_اندر_len,
			&ed->ڈیٹا_برآمد, &ed->ڈیٹا_برآمد_len);
}


ساکن صحیح plugin_v4_psk_کلید_get(صحیح event, خالی *event_ڈیٹا, خالی *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ساخت mosquitto_evt_psk_کلید *ed = event_ڈیٹا;

	غیر_مستعمل(event);

	اگر(plugin->lib.psk_کلید_get_v4 == تہی){
		واپس MOSQ_ERR_PLUGIN_DEFER;
	}

	واپس plugin->lib.psk_کلید_get_v4(
			plugin->lib.user_ڈیٹا,
			ed->client,
			ed->hint,
			ed->شناختی_عنصر,
			ed->کلید,
			ed->max_کلید_len);
}


ساکن صحیح plugin_v4_reload(صحیح event, خالی *event_ڈیٹا, خالی *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	صحیح rc;

	غیر_مستعمل(event);
	غیر_مستعمل(event_ڈیٹا);

	rc = plugin->lib.security_cleanup_v4(
			plugin->lib.user_ڈیٹا,
			plugin->config.options,
			plugin->config.option_گنتی,
			true);
	اگر(rc){
		واپس rc;
	}

	rc = plugin->lib.security_init_v4(
			plugin->lib.user_ڈیٹا,
			plugin->config.options,
			plugin->config.option_گنتی,
			true);
	واپس rc;
}


صحیح plugin_بھرو_v4(mosquitto_plugin_id_t *plugin, خالی *lib)
{
	صحیح rc;

	اگر(!(plugin->lib.plugin_init_v4 = (FUNC_auth_plugin_init_v4)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(تہی, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		واپس MOSQ_ERR_UNKNOWN;
	}
	اگر(!(plugin->lib.plugin_cleanup_v4 = (FUNC_auth_plugin_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(تہی, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		واپس MOSQ_ERR_UNKNOWN;
	}

	اگر(!(plugin->lib.security_init_v4 = (FUNC_auth_plugin_security_init_v4)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(تہی, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		واپس MOSQ_ERR_UNKNOWN;
	}

	اگر(!(plugin->lib.security_cleanup_v4 = (FUNC_auth_plugin_security_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(تہی, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		واپس MOSQ_ERR_UNKNOWN;
	}

	اگر(!(plugin->lib.acl_check_v4 = (FUNC_auth_plugin_acl_check_v4)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(تہی, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		واپس MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.unpwd_check_v4 = (FUNC_auth_plugin_unpwd_check_v4)LIB_SYM(lib, "mosquitto_auth_unpwd_check");
	اگر(plugin->lib.unpwd_check_v4){
		log__printf(تہی, MOSQ_LOG_INFO,
				" ├── Username/password checking enabled.");
	}ورنہ{
		log__printf(تہی, MOSQ_LOG_INFO,
				" ├── Username/password checking not enabled.");
	}

	plugin->lib.psk_کلید_get_v4 = (FUNC_auth_plugin_psk_کلید_get_v4)LIB_SYM(lib, "mosquitto_auth_psk_key_get");
	اگر(plugin->lib.psk_کلید_get_v4){
		log__printf(تہی, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking enabled.");
	}ورنہ{
		log__printf(تہی, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking not enabled.");
	}

	plugin->lib.auth_آغاز_v4 = (FUNC_auth_plugin_auth_آغاز_v4)LIB_SYM(lib, "mosquitto_auth_start");
	plugin->lib.auth_continue_v4 = (FUNC_auth_plugin_auth_continue_v4)LIB_SYM(lib, "mosquitto_auth_continue");

	اگر(plugin->lib.auth_آغاز_v4){
		اگر(plugin->lib.auth_continue_v4){
			log__printf(تہی, MOSQ_LOG_INFO,
					" └── Extended authentication enabled.");
		}ورنہ{
			log__printf(تہی, MOSQ_LOG_ERR,
					"Error: Plugin has missing mosquitto_auth_continue() function.");
			واپس MOSQ_ERR_UNKNOWN;
		}
	}ورنہ{
		log__printf(تہی, MOSQ_LOG_INFO,
				" └── Extended authentication not enabled.");
	}

	plugin->lib.lib = lib;
	plugin->lib.user_ڈیٹا = تہی;
	plugin->lib.identifier = plugin;

	اگر(plugin->lib.plugin_init_v4){
		rc = plugin->lib.plugin_init_v4(&plugin->lib.user_ڈیٹا, plugin->config.options, plugin->config.option_گنتی);
		اگر(rc){
			log__printf(تہی, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			واپس rc;
		}
	}

	mosquitto_callback_رجسٹر(plugin, MOSQ_EVT_RELOAD, plugin_v4_reload, تہی, plugin);

	اگر(plugin->lib.unpwd_check_v4){
		mosquitto_callback_رجسٹر(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v4_basic_auth, تہی, plugin);
	}
	اگر(plugin->lib.acl_check_v4){
		mosquitto_callback_رجسٹر(plugin, MOSQ_EVT_ACL_CHECK, plugin_v4_acl_check, تہی, plugin);
	}
	اگر(plugin->lib.auth_آغاز_v4){
		mosquitto_callback_رجسٹر(plugin, MOSQ_EVT_EXT_AUTH_آغاز, plugin_v4_auth_آغاز, تہی, plugin);
	}
	اگر(plugin->lib.auth_continue_v4){
		mosquitto_callback_رجسٹر(plugin, MOSQ_EVT_EXT_AUTH_CONTINUE, plugin_v4_auth_continue, تہی, plugin);
	}
	اگر(plugin->lib.psk_کلید_get_v4){
		mosquitto_callback_رجسٹر(plugin, MOSQ_EVT_PSK_کلید, plugin_v4_psk_کلید_get, تہی, plugin);
	}

	واپس 0;
}
