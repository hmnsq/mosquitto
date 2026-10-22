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


ساكن صحيح plugin_v4_basic_auth(صحيح event, فراغ *event_data, فراغ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	بنية mosquitto_evt_basic_auth *ed = event_data;

	UNUSED(event);

	𐤀𐤌(plugin->lib.unpwd_check_v4 == عدم){
		أرجع MOSQ_ERR_PLUGIN_DEFER;
	}

	أرجع plugin->lib.unpwd_check_v4(
			plugin->lib.user_data,
			ed->client,
			ed->username,
			ed->password);
}


ساكن صحيح plugin_v4_acl_check(صحيح event, فراغ *event_data, فراغ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	بنية mosquitto_evt_acl_check *ed = event_data;
	بنية mosquitto_acl_msg msg;
	صحيح rc;

	UNUSED(event);

	𐤀𐤌(plugin->lib.acl_check_v4 == عدم){
		أرجع MOSQ_ERR_PLUGIN_DEFER;
	}

	memset(&msg, 0, حجم(msg));
	msg.topic = ed->topic;
	msg.payloadlen = ed->payloadlen;
	msg.payload = ed->payload;
	msg.qos = ed->qos;
	msg.retain = ed->retain;

	rc = acl__pre_check(plugin, ed->client, ed->access);
	𐤀𐤌(rc == MOSQ_ERR_PLUGIN_DEFER){
		أرجع plugin->lib.acl_check_v4(
				plugin->lib.user_data,
				ed->access,
				ed->client,
				&msg);
	}وإلا{
		أرجع rc;
	}
}


ساكن صحيح plugin_v4_auth_start(صحيح event, فراغ *event_data, فراغ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	بنية mosquitto_evt_extended_auth *ed = event_data;

	UNUSED(event);

	𐤀𐤌(plugin->lib.auth_start_v4 == عدم){
		أرجع MOSQ_ERR_PLUGIN_DEFER;
	}

	أرجع plugin->lib.auth_start_v4(
			plugin->lib.user_data,
			ed->client,
			ed->client->auth_method,
			false,
			ed->data_in, ed->data_in_len,
			&ed->data_out, &ed->data_out_len);
}


ساكن صحيح plugin_v4_auth_continue(صحيح event, فراغ *event_data, فراغ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	بنية mosquitto_evt_extended_auth *ed = event_data;

	UNUSED(event);

	𐤀𐤌(plugin->lib.auth_continue_v4 == عدم){
		أرجع MOSQ_ERR_PLUGIN_DEFER;
	}

	أرجع plugin->lib.auth_continue_v4(
			plugin->lib.user_data,
			ed->client,
			ed->client->auth_method,
			ed->data_in, ed->data_in_len,
			&ed->data_out, &ed->data_out_len);
}


ساكن صحيح plugin_v4_psk_key_get(صحيح event, فراغ *event_data, فراغ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	بنية mosquitto_evt_psk_key *ed = event_data;

	UNUSED(event);

	𐤀𐤌(plugin->lib.psk_key_get_v4 == عدم){
		أرجع MOSQ_ERR_PLUGIN_DEFER;
	}

	أرجع plugin->lib.psk_key_get_v4(
			plugin->lib.user_data,
			ed->client,
			ed->hint,
			ed->identity,
			ed->key,
			ed->max_key_len);
}


ساكن صحيح plugin_v4_reload(صحيح event, فراغ *event_data, فراغ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	صحيح rc;

	UNUSED(event);
	UNUSED(event_data);

	rc = plugin->lib.security_cleanup_v4(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_count,
			true);
	𐤀𐤌(rc){
		أرجع rc;
	}

	rc = plugin->lib.security_init_v4(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_count,
			true);
	أرجع rc;
}


صحيح plugin__load_v4(mosquitto_plugin_id_t *plugin, فراغ *lib)
{
	صحيح rc;

	𐤀𐤌(!(plugin->lib.plugin_init_v4 = (FUNC_auth_plugin_init_v4)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}
	𐤀𐤌(!(plugin->lib.plugin_cleanup_v4 = (FUNC_auth_plugin_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}

	𐤀𐤌(!(plugin->lib.security_init_v4 = (FUNC_auth_plugin_security_init_v4)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}

	𐤀𐤌(!(plugin->lib.security_cleanup_v4 = (FUNC_auth_plugin_security_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}

	𐤀𐤌(!(plugin->lib.acl_check_v4 = (FUNC_auth_plugin_acl_check_v4)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(عدم, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		أرجع MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.unpwd_check_v4 = (FUNC_auth_plugin_unpwd_check_v4)LIB_SYM(lib, "mosquitto_auth_unpwd_check");
	𐤀𐤌(plugin->lib.unpwd_check_v4){
		log__printf(عدم, MOSQ_LOG_INFO,
				" ├── Username/password checking enabled.");
	}وإلا{
		log__printf(عدم, MOSQ_LOG_INFO,
				" ├── Username/password checking not enabled.");
	}

	plugin->lib.psk_key_get_v4 = (FUNC_auth_plugin_psk_key_get_v4)LIB_SYM(lib, "mosquitto_auth_psk_key_get");
	𐤀𐤌(plugin->lib.psk_key_get_v4){
		log__printf(عدم, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking enabled.");
	}وإلا{
		log__printf(عدم, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking not enabled.");
	}

	plugin->lib.auth_start_v4 = (FUNC_auth_plugin_auth_start_v4)LIB_SYM(lib, "mosquitto_auth_start");
	plugin->lib.auth_continue_v4 = (FUNC_auth_plugin_auth_continue_v4)LIB_SYM(lib, "mosquitto_auth_continue");

	𐤀𐤌(plugin->lib.auth_start_v4){
		𐤀𐤌(plugin->lib.auth_continue_v4){
			log__printf(عدم, MOSQ_LOG_INFO,
					" └── Extended authentication enabled.");
		}وإلا{
			log__printf(عدم, MOSQ_LOG_ERR,
					"Error: Plugin has missing mosquitto_auth_continue() function.");
			أرجع MOSQ_ERR_UNKNOWN;
		}
	}وإلا{
		log__printf(عدم, MOSQ_LOG_INFO,
				" └── Extended authentication not enabled.");
	}

	plugin->lib.lib = lib;
	plugin->lib.user_data = عدم;
	plugin->lib.identifier = plugin;

	𐤀𐤌(plugin->lib.plugin_init_v4){
		rc = plugin->lib.plugin_init_v4(&plugin->lib.user_data, plugin->config.options, plugin->config.option_count);
		𐤀𐤌(rc){
			log__printf(عدم, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			أرجع rc;
		}
	}

	mosquitto_callback_register(plugin, MOSQ_EVT_RELOAD, plugin_v4_reload, عدم, plugin);

	𐤀𐤌(plugin->lib.unpwd_check_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v4_basic_auth, عدم, plugin);
	}
	𐤀𐤌(plugin->lib.acl_check_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_ACL_CHECK, plugin_v4_acl_check, عدم, plugin);
	}
	𐤀𐤌(plugin->lib.auth_start_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_EXT_AUTH_START, plugin_v4_auth_start, عدم, plugin);
	}
	𐤀𐤌(plugin->lib.auth_continue_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_EXT_AUTH_CONTINUE, plugin_v4_auth_continue, عدم, plugin);
	}
	𐤀𐤌(plugin->lib.psk_key_get_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_PSK_KEY, plugin_v4_psk_key_get, عدم, plugin);
	}

	أرجع 0;
}
