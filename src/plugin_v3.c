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

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto/broker.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/broker_plugin.h"
#ضمن "lib_load.h"
#ضمن "utlist.h"

تعريفنوع صحيح (*FUNC_auth_plugin_نسخه)(فراغ);
تعريفنوع صحيح (*FUNC_plugin_نسخه)(صحيح, ثابت صحيح *);


ساكن صحيح plugin_v3_basic_auth(صحيح event, فراغ *event_داده, فراغ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	بنية mosquitto_evt_basic_auth *ed = event_داده;

	استفادهنشده(event);

	إذا(plugin->lib.unpwd_check_v3 == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	أرجع plugin->lib.unpwd_check_v3(
			plugin->lib.user_داده,
			ed->client,
			ed->username,
			ed->password);
}


ساكن صحيح plugin_v3_acl_check(صحيح event, فراغ *event_داده, فراغ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	بنية mosquitto_evt_acl_check *ed = event_داده;
	بنية mosquitto_acl_پیام پیام;
	صحيح rc;

	استفادهنشده(event);

	إذا(plugin->lib.acl_check_v3 == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	memset(&پیام, 0, حجم(پیام));
	پیام.topic = ed->topic;
	پیام.payloadlen = ed->payloadlen;
	پیام.payload = ed->payload;
	پیام.qos = ed->qos;
	پیام.retain = ed->retain;

	rc = acl__pre_check(plugin, ed->client, ed->access);
	إذا(rc == MOSQ_ERR_PLUGIN_DEFER){
		أرجع plugin->lib.acl_check_v3(
				plugin->lib.user_داده,
				ed->access,
				ed->client,
				&پیام);
	}وإلا{
		أرجع rc;
	}
}


ساكن صحيح plugin_v3_psk_کلید_get(صحيح event, فراغ *event_داده, فراغ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	بنية mosquitto_evt_psk_کلید *ed = event_داده;

	استفادهنشده(event);

	إذا(plugin->lib.psk_کلید_get_v3 == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	أرجع plugin->lib.psk_کلید_get_v3(
			plugin->lib.user_داده,
			ed->client,
			ed->hint,
			ed->عنصر_خنثی,
			ed->کلید,
			ed->max_کلید_len);
}


ساكن صحيح plugin_v3_reload(صحيح event, فراغ *event_داده, فراغ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	صحيح rc;

	استفادهنشده(event);
	استفادهنشده(event_داده);

	rc = plugin->lib.security_cleanup_v3(
			plugin->lib.user_داده,
			plugin->config.options,
			plugin->config.option_شمار,
			true);
	إذا(rc){
		أرجع rc;
	}

	rc = plugin->lib.security_init_v3(
			plugin->lib.user_داده,
			plugin->config.options,
			plugin->config.option_شمار,
			true);
	أرجع rc;
}


صحيح plugin_بارگیری_v3(mosquitto_plugin_id_t *plugin, فراغ *lib)
{
	صحيح rc;

	إذا(!(plugin->lib.plugin_init_v3 = (FUNC_auth_plugin_init_v3)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}
	إذا(!(plugin->lib.plugin_cleanup_v3 = (FUNC_auth_plugin_cleanup_v3)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}

	إذا(!(plugin->lib.security_init_v3 = (FUNC_auth_plugin_security_init_v3)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}

	إذا(!(plugin->lib.security_cleanup_v3 = (FUNC_auth_plugin_security_cleanup_v3)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}

	إذا(!(plugin->lib.acl_check_v3 = (FUNC_auth_plugin_acl_check_v3)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}

	إذا(!(plugin->lib.unpwd_check_v3 = (FUNC_auth_plugin_unpwd_check_v3)LIB_SYM(lib, "mosquitto_auth_unpwd_check"))){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_unpwd_check().");
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}

	إذا(!(plugin->lib.psk_کلید_get_v3 = (FUNC_auth_plugin_psk_کلید_get_v3)LIB_SYM(lib, "mosquitto_auth_psk_key_get"))){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_psk_key_get().");
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = lib;
	plugin->lib.user_داده = عدم;
	plugin->lib.identifier = plugin;
	إذا(plugin->lib.plugin_init_v3){
		rc = plugin->lib.plugin_init_v3(&plugin->lib.user_داده, plugin->config.options, plugin->config.option_شمار);
		إذا(rc){
			log__printf(عدم, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			أرجع rc;
		}
	}

	mosquitto_callback_ثبات(plugin, MOSQ_EVT_RELOAD, plugin_v3_reload, عدم, plugin);

	إذا(plugin->lib.unpwd_check_v3){
		mosquitto_callback_ثبات(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v3_basic_auth, عدم, plugin);
	}
	إذا(plugin->lib.acl_check_v3){
		mosquitto_callback_ثبات(plugin, MOSQ_EVT_ACL_CHECK, plugin_v3_acl_check, عدم, plugin);
	}
	إذا(plugin->lib.psk_کلید_get_v3){
		mosquitto_callback_ثبات(plugin, MOSQ_EVT_PSK_کلید, plugin_v3_psk_کلید_get, عدم, plugin);
	}

	أرجع 0;
}


