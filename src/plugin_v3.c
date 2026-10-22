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

/* This loads v3 plugins in a v5 wrapper to make the core code cleaner */

#شامل "config.h"

#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto/broker.h"
#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/broker_plugin.h"
#شامل "lib_load.h"
#شامل "utlist.h"

تعریفنوع صحیح (*FUNC_auth_plugin_version)(تهی);
تعریفنوع صحیح (*FUNC_plugin_version)(صحیح, ثابت صحیح *);


ایستا صحیح plugin_v3_basic_auth(صحیح event, تهی *event_data, تهی *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ساختار mosquitto_evt_basic_auth *ed = event_data;

	UNUSED(event);

	اگر(plugin->lib.unpwd_check_v3 == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}

	بازگشت plugin->lib.unpwd_check_v3(
			plugin->lib.user_data,
			ed->client,
			ed->username,
			ed->password);
}


ایستا صحیح plugin_v3_acl_check(صحیح event, تهی *event_data, تهی *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ساختار mosquitto_evt_acl_check *ed = event_data;
	ساختار mosquitto_acl_پیام پیام;
	صحیح rc;

	UNUSED(event);

	اگر(plugin->lib.acl_check_v3 == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}

	memset(&پیام, 0, اندازه(پیام));
	پیام.topic = ed->topic;
	پیام.payloadlen = ed->payloadlen;
	پیام.payload = ed->payload;
	پیام.qos = ed->qos;
	پیام.retain = ed->retain;

	rc = acl__pre_check(plugin, ed->client, ed->access);
	اگر(rc == MOSQ_ERR_PLUGIN_DEFER){
		بازگشت plugin->lib.acl_check_v3(
				plugin->lib.user_data,
				ed->access,
				ed->client,
				&پیام);
	}وگرنه{
		بازگشت rc;
	}
}


ایستا صحیح plugin_v3_psk_کلید_get(صحیح event, تهی *event_data, تهی *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ساختار mosquitto_evt_psk_کلید *ed = event_data;

	UNUSED(event);

	اگر(plugin->lib.psk_کلید_get_v3 == پوچ){
		بازگشت MOSQ_ERR_INVAL;
	}

	بازگشت plugin->lib.psk_کلید_get_v3(
			plugin->lib.user_data,
			ed->client,
			ed->hint,
			ed->identity,
			ed->کلید,
			ed->max_کلید_len);
}


ایستا صحیح plugin_v3_reload(صحیح event, تهی *event_data, تهی *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	صحیح rc;

	UNUSED(event);
	UNUSED(event_data);

	rc = plugin->lib.security_cleanup_v3(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_شمار,
			true);
	اگر(rc){
		بازگشت rc;
	}

	rc = plugin->lib.security_init_v3(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_شمار,
			true);
	بازگشت rc;
}


صحیح plugin__load_v3(mosquitto_plugin_id_t *plugin, تهی *lib)
{
	صحیح rc;

	اگر(!(plugin->lib.plugin_init_v3 = (FUNC_auth_plugin_init_v3)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(پوچ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		بازگشت MOSQ_ERR_UNKNOWN;
	}
	اگر(!(plugin->lib.plugin_cleanup_v3 = (FUNC_auth_plugin_cleanup_v3)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(پوچ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		بازگشت MOSQ_ERR_UNKNOWN;
	}

	اگر(!(plugin->lib.security_init_v3 = (FUNC_auth_plugin_security_init_v3)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(پوچ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		بازگشت MOSQ_ERR_UNKNOWN;
	}

	اگر(!(plugin->lib.security_cleanup_v3 = (FUNC_auth_plugin_security_cleanup_v3)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(پوچ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		بازگشت MOSQ_ERR_UNKNOWN;
	}

	اگر(!(plugin->lib.acl_check_v3 = (FUNC_auth_plugin_acl_check_v3)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(پوچ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		بازگشت MOSQ_ERR_UNKNOWN;
	}

	اگر(!(plugin->lib.unpwd_check_v3 = (FUNC_auth_plugin_unpwd_check_v3)LIB_SYM(lib, "mosquitto_auth_unpwd_check"))){
		log__printf(پوچ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_unpwd_check().");
		LIB_ERROR();
		بازگشت MOSQ_ERR_UNKNOWN;
	}

	اگر(!(plugin->lib.psk_کلید_get_v3 = (FUNC_auth_plugin_psk_کلید_get_v3)LIB_SYM(lib, "mosquitto_auth_psk_key_get"))){
		log__printf(پوچ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_psk_key_get().");
		LIB_ERROR();
		بازگشت MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = lib;
	plugin->lib.user_data = پوچ;
	plugin->lib.identifier = plugin;
	اگر(plugin->lib.plugin_init_v3){
		rc = plugin->lib.plugin_init_v3(&plugin->lib.user_data, plugin->config.options, plugin->config.option_شمار);
		اگر(rc){
			log__printf(پوچ, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			بازگشت rc;
		}
	}

	mosquitto_callback_register(plugin, MOSQ_EVT_RELOAD, plugin_v3_reload, پوچ, plugin);

	اگر(plugin->lib.unpwd_check_v3){
		mosquitto_callback_register(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v3_basic_auth, پوچ, plugin);
	}
	اگر(plugin->lib.acl_check_v3){
		mosquitto_callback_register(plugin, MOSQ_EVT_ACL_CHECK, plugin_v3_acl_check, پوچ, plugin);
	}
	اگر(plugin->lib.psk_کلید_get_v3){
		mosquitto_callback_register(plugin, MOSQ_EVT_PSK_کلید, plugin_v3_psk_کلید_get, پوچ, plugin);
	}

	بازگشت 0;
}


