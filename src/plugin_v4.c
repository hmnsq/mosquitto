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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "mosquitto/broker.h"
#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/broker_plugin.h"
#അന്തര്ഭാവ "lib_load.h"
#അന്തര്ഭാവ "utlist.h"

തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_auth_plugin_version)(ശൂന്യം);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*FUNC_plugin_version)(പൂർണ്ണസംഖ്യ, സ്ഥിരം പൂർണ്ണസംഖ്യ *);


സ്ഥിതം പൂർണ്ണസംഖ്യ plugin_v4_basic_auth(പൂർണ്ണസംഖ്യ event, ശൂന്യം *event_data, ശൂന്യം *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ഘടന mosquitto_evt_basic_auth *ed = event_data;

	UNUSED(event);

	എങ്കിൽ(plugin->lib.unpwd_check_v4 == ശൂന്യ){
		തിരികെ MOSQ_ERR_PLUGIN_DEFER;
	}

	തിരികെ plugin->lib.unpwd_check_v4(
			plugin->lib.user_data,
			ed->client,
			ed->username,
			ed->password);
}


സ്ഥിതം പൂർണ്ണസംഖ്യ plugin_v4_acl_check(പൂർണ്ണസംഖ്യ event, ശൂന്യം *event_data, ശൂന്യം *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ഘടന mosquitto_evt_acl_check *ed = event_data;
	ഘടന mosquitto_acl_സന്ദേശം സന്ദേശം;
	പൂർണ്ണസംഖ്യ rc;

	UNUSED(event);

	എങ്കിൽ(plugin->lib.acl_check_v4 == ശൂന്യ){
		തിരികെ MOSQ_ERR_PLUGIN_DEFER;
	}

	memset(&സന്ദേശം, 0, വലുപ്പം(സന്ദേശം));
	സന്ദേശം.topic = ed->topic;
	സന്ദേശം.payloadlen = ed->payloadlen;
	സന്ദേശം.payload = ed->payload;
	സന്ദേശം.qos = ed->qos;
	സന്ദേശം.retain = ed->retain;

	rc = acl__pre_check(plugin, ed->client, ed->access);
	എങ്കിൽ(rc == MOSQ_ERR_PLUGIN_DEFER){
		തിരികെ plugin->lib.acl_check_v4(
				plugin->lib.user_data,
				ed->access,
				ed->client,
				&സന്ദേശം);
	}അല്ലെങ്കിൽ{
		തിരികെ rc;
	}
}


സ്ഥിതം പൂർണ്ണസംഖ്യ plugin_v4_auth_start(പൂർണ്ണസംഖ്യ event, ശൂന്യം *event_data, ശൂന്യം *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ഘടന mosquitto_evt_extended_auth *ed = event_data;

	UNUSED(event);

	എങ്കിൽ(plugin->lib.auth_start_v4 == ശൂന്യ){
		തിരികെ MOSQ_ERR_PLUGIN_DEFER;
	}

	തിരികെ plugin->lib.auth_start_v4(
			plugin->lib.user_data,
			ed->client,
			ed->client->auth_method,
			false,
			ed->data_in, ed->data_in_len,
			&ed->data_out, &ed->data_out_len);
}


സ്ഥിതം പൂർണ്ണസംഖ്യ plugin_v4_auth_continue(പൂർണ്ണസംഖ്യ event, ശൂന്യം *event_data, ശൂന്യം *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ഘടന mosquitto_evt_extended_auth *ed = event_data;

	UNUSED(event);

	എങ്കിൽ(plugin->lib.auth_continue_v4 == ശൂന്യ){
		തിരികെ MOSQ_ERR_PLUGIN_DEFER;
	}

	തിരികെ plugin->lib.auth_continue_v4(
			plugin->lib.user_data,
			ed->client,
			ed->client->auth_method,
			ed->data_in, ed->data_in_len,
			&ed->data_out, &ed->data_out_len);
}


സ്ഥിതം പൂർണ്ണസംഖ്യ plugin_v4_psk_താക്കോൽ_get(പൂർണ്ണസംഖ്യ event, ശൂന്യം *event_data, ശൂന്യം *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ഘടന mosquitto_evt_psk_താക്കോൽ *ed = event_data;

	UNUSED(event);

	എങ്കിൽ(plugin->lib.psk_താക്കോൽ_get_v4 == ശൂന്യ){
		തിരികെ MOSQ_ERR_PLUGIN_DEFER;
	}

	തിരികെ plugin->lib.psk_താക്കോൽ_get_v4(
			plugin->lib.user_data,
			ed->client,
			ed->hint,
			ed->identity,
			ed->താക്കോൽ,
			ed->max_താക്കോൽ_len);
}


സ്ഥിതം പൂർണ്ണസംഖ്യ plugin_v4_reload(പൂർണ്ണസംഖ്യ event, ശൂന്യം *event_data, ശൂന്യം *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	പൂർണ്ണസംഖ്യ rc;

	UNUSED(event);
	UNUSED(event_data);

	rc = plugin->lib.security_cleanup_v4(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_എണ്ണം,
			true);
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	rc = plugin->lib.security_init_v4(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_എണ്ണം,
			true);
	തിരികെ rc;
}


പൂർണ്ണസംഖ്യ plugin__load_v4(mosquitto_plugin_id_t *plugin, ശൂന്യം *lib)
{
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ(!(plugin->lib.plugin_init_v4 = (FUNC_auth_plugin_init_v4)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		തിരികെ MOSQ_ERR_UNKNOWN;
	}
	എങ്കിൽ(!(plugin->lib.plugin_cleanup_v4 = (FUNC_auth_plugin_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		തിരികെ MOSQ_ERR_UNKNOWN;
	}

	എങ്കിൽ(!(plugin->lib.security_init_v4 = (FUNC_auth_plugin_security_init_v4)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		തിരികെ MOSQ_ERR_UNKNOWN;
	}

	എങ്കിൽ(!(plugin->lib.security_cleanup_v4 = (FUNC_auth_plugin_security_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		തിരികെ MOSQ_ERR_UNKNOWN;
	}

	എങ്കിൽ(!(plugin->lib.acl_check_v4 = (FUNC_auth_plugin_acl_check_v4)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		തിരികെ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.unpwd_check_v4 = (FUNC_auth_plugin_unpwd_check_v4)LIB_SYM(lib, "mosquitto_auth_unpwd_check");
	എങ്കിൽ(plugin->lib.unpwd_check_v4){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO,
				" ├── Username/password checking enabled.");
	}അല്ലെങ്കിൽ{
		log__printf(ശൂന്യ, MOSQ_LOG_INFO,
				" ├── Username/password checking not enabled.");
	}

	plugin->lib.psk_താക്കോൽ_get_v4 = (FUNC_auth_plugin_psk_താക്കോൽ_get_v4)LIB_SYM(lib, "mosquitto_auth_psk_key_get");
	എങ്കിൽ(plugin->lib.psk_താക്കോൽ_get_v4){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking enabled.");
	}അല്ലെങ്കിൽ{
		log__printf(ശൂന്യ, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking not enabled.");
	}

	plugin->lib.auth_start_v4 = (FUNC_auth_plugin_auth_start_v4)LIB_SYM(lib, "mosquitto_auth_start");
	plugin->lib.auth_continue_v4 = (FUNC_auth_plugin_auth_continue_v4)LIB_SYM(lib, "mosquitto_auth_continue");

	എങ്കിൽ(plugin->lib.auth_start_v4){
		എങ്കിൽ(plugin->lib.auth_continue_v4){
			log__printf(ശൂന്യ, MOSQ_LOG_INFO,
					" └── Extended authentication enabled.");
		}അല്ലെങ്കിൽ{
			log__printf(ശൂന്യ, MOSQ_LOG_ERR,
					"Error: Plugin has missing mosquitto_auth_continue() function.");
			തിരികെ MOSQ_ERR_UNKNOWN;
		}
	}അല്ലെങ്കിൽ{
		log__printf(ശൂന്യ, MOSQ_LOG_INFO,
				" └── Extended authentication not enabled.");
	}

	plugin->lib.lib = lib;
	plugin->lib.user_data = ശൂന്യ;
	plugin->lib.identifier = plugin;

	എങ്കിൽ(plugin->lib.plugin_init_v4){
		rc = plugin->lib.plugin_init_v4(&plugin->lib.user_data, plugin->config.options, plugin->config.option_എണ്ണം);
		എങ്കിൽ(rc){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			തിരികെ rc;
		}
	}

	mosquitto_callback_register(plugin, MOSQ_EVT_RELOAD, plugin_v4_reload, ശൂന്യ, plugin);

	എങ്കിൽ(plugin->lib.unpwd_check_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v4_basic_auth, ശൂന്യ, plugin);
	}
	എങ്കിൽ(plugin->lib.acl_check_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_ACL_CHECK, plugin_v4_acl_check, ശൂന്യ, plugin);
	}
	എങ്കിൽ(plugin->lib.auth_start_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_EXT_AUTH_START, plugin_v4_auth_start, ശൂന്യ, plugin);
	}
	എങ്കിൽ(plugin->lib.auth_continue_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_EXT_AUTH_CONTINUE, plugin_v4_auth_continue, ശൂന്യ, plugin);
	}
	എങ്കിൽ(plugin->lib.psk_താക്കോൽ_get_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_PSK_താക്കോൽ, plugin_v4_psk_താക്കോൽ_get, ശൂന്യ, plugin);
	}

	തിരികെ 0;
}
