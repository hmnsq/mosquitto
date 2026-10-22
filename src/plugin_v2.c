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

/* This loads v2 plugins in a v5 wrapper to make the core code cleaner */

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto/broker.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/broker_plugin.h"
#ضمن "lib_load.h"
#ضمن "utlist.h"

تعريفنوع صحيح (*FUNC_auth_plugin_version)(فراغ);
تعريفنوع صحيح (*FUNC_plugin_version)(صحيح, ثابت صحيح *);


ساكن صحيح plugin_v2_basic_auth(صحيح event, فراغ *event_data, فراغ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	بنية mosquitto_evt_basic_auth *ed = event_data;

	UNUSED(event);

	𐡄𐡍(plugin->lib.unpwd_check_v2 == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	أرجع plugin->lib.unpwd_check_v2(
			plugin->lib.user_data,
			ed->username,
			ed->password);
}


ساكن صحيح plugin_v2_acl_check(صحيح event, فراغ *event_data, فراغ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	بنية mosquitto_evt_acl_check *ed = event_data;
	صحيح rc;

	UNUSED(event);

	𐡄𐡍(plugin->lib.acl_check_v2 == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	rc = acl__pre_check(plugin, ed->client, ed->access);
	𐡄𐡍(rc == MOSQ_ERR_PLUGIN_DEFER){
		أرجع plugin->lib.acl_check_v2(
				plugin->lib.user_data,
				ed->client->id,
				ed->client->username,
				ed->topic,
				ed->access);
	}وإلا{
		أرجع rc;
	}
}


ساكن صحيح plugin_v2_psk_key_get(صحيح event, فراغ *event_data, فراغ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	بنية mosquitto_evt_psk_key *ed = event_data;

	UNUSED(event);

	𐡄𐡍(plugin->lib.psk_key_get_v2 == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	أرجع plugin->lib.psk_key_get_v2(
			plugin->lib.user_data,
			ed->hint,
			ed->identity,
			ed->key,
			ed->max_key_len);
}


ساكن صحيح plugin_v2_reload(صحيح event, فراغ *event_data, فراغ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	صحيح rc;

	UNUSED(event);
	UNUSED(event_data);

	rc = plugin->lib.security_cleanup_v2(
			plugin->lib.user_data,
			(بنية mosquitto_auth_opt *)plugin->config.options,
			plugin->config.option_count,
			true);
	𐡄𐡍(rc){
		أرجع rc;
	}

	rc = plugin->lib.security_init_v2(
			plugin->lib.user_data,
			(بنية mosquitto_auth_opt *)plugin->config.options,
			plugin->config.option_count,
			true);
	أرجع rc;
}


صحيح plugin__load_v2(mosquitto_plugin_id_t *plugin, فراغ *lib)
{
	صحيح rc;

	𐡄𐡍(!(plugin->lib.plugin_init_v2 = (FUNC_auth_plugin_init_v2)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}
	𐡄𐡍(!(plugin->lib.plugin_cleanup_v2 = (FUNC_auth_plugin_cleanup_v2)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}

	𐡄𐡍(!(plugin->lib.security_init_v2 = (FUNC_auth_plugin_security_init_v2)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}

	𐡄𐡍(!(plugin->lib.security_cleanup_v2 = (FUNC_auth_plugin_security_cleanup_v2)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}

	𐡄𐡍(!(plugin->lib.acl_check_v2 = (FUNC_auth_plugin_acl_check_v2)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}

	𐡄𐡍(!(plugin->lib.unpwd_check_v2 = (FUNC_auth_plugin_unpwd_check_v2)LIB_SYM(lib, "mosquitto_auth_unpwd_check"))){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_unpwd_check().");
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}

	𐡄𐡍(!(plugin->lib.psk_key_get_v2 = (FUNC_auth_plugin_psk_key_get_v2)LIB_SYM(lib, "mosquitto_auth_psk_key_get"))){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_psk_key_get().");
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = lib;
	plugin->lib.user_data = عدم;
	plugin->lib.identifier = plugin;

	𐡄𐡍(plugin->lib.plugin_init_v2){
		rc = plugin->lib.plugin_init_v2(
				&plugin->lib.user_data,
				(بنية mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_count);
		𐡄𐡍(rc){
			log__printf(عدم, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			أرجع rc;
		}
	}

	mosquitto_callback_register(plugin, MOSQ_EVT_RELOAD, plugin_v2_reload, عدم, plugin);

	𐡄𐡍(plugin->lib.unpwd_check_v2){
		mosquitto_callback_register(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v2_basic_auth, عدم, plugin);
	}
	𐡄𐡍(plugin->lib.acl_check_v2){
		mosquitto_callback_register(plugin, MOSQ_EVT_ACL_CHECK, plugin_v2_acl_check, عدم, plugin);
	}
	𐡄𐡍(plugin->lib.psk_key_get_v2){
		mosquitto_callback_register(plugin, MOSQ_EVT_PSK_KEY, plugin_v2_psk_key_get, عدم, plugin);
	}

	أرجع 0;
}
