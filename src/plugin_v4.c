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

#অন্তর্ভাৱ "config.h"

#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <string.h>

#অন্তর্ভাৱ "mosquitto/broker.h"
#অন্তর্ভাৱ "mosquitto_broker_internal.h"
#অন্তর্ভাৱ "mosquitto/broker_plugin.h"
#অন্তর্ভাৱ "lib_load.h"
#অন্তর্ভাৱ "utlist.h"

প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_auth_plugin_version)(শূন্য);
প্রকারনির্দেশ পূর্ণাঙ্ক (*FUNC_plugin_version)(পূর্ণাঙ্ক, ধ্রুৱ পূর্ণাঙ্ক *);


স্থির পূর্ণাঙ্ক plugin_v4_basic_auth(পূর্ণাঙ্ক event, শূন্য *event_data, শূন্য *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	সংরচনা mosquitto_evt_basic_auth *ed = event_data;

	UNUSED(event);

	যদি(plugin->lib.unpwd_check_v4 == রিক্ত){
		ঘূৰাই MOSQ_ERR_PLUGIN_DEFER;
	}

	ঘূৰাই plugin->lib.unpwd_check_v4(
			plugin->lib.user_data,
			ed->client,
			ed->username,
			ed->password);
}


স্থির পূর্ণাঙ্ক plugin_v4_acl_check(পূর্ণাঙ্ক event, শূন্য *event_data, শূন্য *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	সংরচনা mosquitto_evt_acl_check *ed = event_data;
	সংরচনা mosquitto_acl_বাৰ্তা বাৰ্তা;
	পূর্ণাঙ্ক rc;

	UNUSED(event);

	যদি(plugin->lib.acl_check_v4 == রিক্ত){
		ঘূৰাই MOSQ_ERR_PLUGIN_DEFER;
	}

	memset(&বাৰ্তা, 0, পরিমাণ(বাৰ্তা));
	বাৰ্তা.topic = ed->topic;
	বাৰ্তা.payloadlen = ed->payloadlen;
	বাৰ্তা.payload = ed->payload;
	বাৰ্তা.qos = ed->qos;
	বাৰ্তা.retain = ed->retain;

	rc = acl__pre_check(plugin, ed->client, ed->access);
	যদি(rc == MOSQ_ERR_PLUGIN_DEFER){
		ঘূৰাই plugin->lib.acl_check_v4(
				plugin->lib.user_data,
				ed->access,
				ed->client,
				&বাৰ্তা);
	}অন্যথা{
		ঘূৰাই rc;
	}
}


স্থির পূর্ণাঙ্ক plugin_v4_auth_start(পূর্ণাঙ্ক event, শূন্য *event_data, শূন্য *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	সংরচনা mosquitto_evt_extended_auth *ed = event_data;

	UNUSED(event);

	যদি(plugin->lib.auth_start_v4 == রিক্ত){
		ঘূৰাই MOSQ_ERR_PLUGIN_DEFER;
	}

	ঘূৰাই plugin->lib.auth_start_v4(
			plugin->lib.user_data,
			ed->client,
			ed->client->auth_method,
			false,
			ed->data_in, ed->data_in_len,
			&ed->data_out, &ed->data_out_len);
}


স্থির পূর্ণাঙ্ক plugin_v4_auth_continue(পূর্ণাঙ্ক event, শূন্য *event_data, শূন্য *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	সংরচনা mosquitto_evt_extended_auth *ed = event_data;

	UNUSED(event);

	যদি(plugin->lib.auth_continue_v4 == রিক্ত){
		ঘূৰাই MOSQ_ERR_PLUGIN_DEFER;
	}

	ঘূৰাই plugin->lib.auth_continue_v4(
			plugin->lib.user_data,
			ed->client,
			ed->client->auth_method,
			ed->data_in, ed->data_in_len,
			&ed->data_out, &ed->data_out_len);
}


স্থির পূর্ণাঙ্ক plugin_v4_psk_চাবি_get(পূর্ণাঙ্ক event, শূন্য *event_data, শূন্য *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	সংরচনা mosquitto_evt_psk_চাবি *ed = event_data;

	UNUSED(event);

	যদি(plugin->lib.psk_চাবি_get_v4 == রিক্ত){
		ঘূৰাই MOSQ_ERR_PLUGIN_DEFER;
	}

	ঘূৰাই plugin->lib.psk_চাবি_get_v4(
			plugin->lib.user_data,
			ed->client,
			ed->hint,
			ed->identity,
			ed->চাবি,
			ed->max_চাবি_len);
}


স্থির পূর্ণাঙ্ক plugin_v4_reload(পূর্ণাঙ্ক event, শূন্য *event_data, শূন্য *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	পূর্ণাঙ্ক rc;

	UNUSED(event);
	UNUSED(event_data);

	rc = plugin->lib.security_cleanup_v4(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_গুনতি,
			true);
	যদি(rc){
		ঘূৰাই rc;
	}

	rc = plugin->lib.security_init_v4(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_গুনতি,
			true);
	ঘূৰাই rc;
}


পূর্ণাঙ্ক plugin__load_v4(mosquitto_plugin_id_t *plugin, শূন্য *lib)
{
	পূর্ণাঙ্ক rc;

	যদি(!(plugin->lib.plugin_init_v4 = (FUNC_auth_plugin_init_v4)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(রিক্ত, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		ঘূৰাই MOSQ_ERR_UNKNOWN;
	}
	যদি(!(plugin->lib.plugin_cleanup_v4 = (FUNC_auth_plugin_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(রিক্ত, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		ঘূৰাই MOSQ_ERR_UNKNOWN;
	}

	যদি(!(plugin->lib.security_init_v4 = (FUNC_auth_plugin_security_init_v4)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(রিক্ত, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		ঘূৰাই MOSQ_ERR_UNKNOWN;
	}

	যদি(!(plugin->lib.security_cleanup_v4 = (FUNC_auth_plugin_security_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(রিক্ত, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		ঘূৰাই MOSQ_ERR_UNKNOWN;
	}

	যদি(!(plugin->lib.acl_check_v4 = (FUNC_auth_plugin_acl_check_v4)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(রিক্ত, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		ঘূৰাই MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.unpwd_check_v4 = (FUNC_auth_plugin_unpwd_check_v4)LIB_SYM(lib, "mosquitto_auth_unpwd_check");
	যদি(plugin->lib.unpwd_check_v4){
		log__printf(রিক্ত, MOSQ_LOG_INFO,
				" ├── Username/password checking enabled.");
	}অন্যথা{
		log__printf(রিক্ত, MOSQ_LOG_INFO,
				" ├── Username/password checking not enabled.");
	}

	plugin->lib.psk_চাবি_get_v4 = (FUNC_auth_plugin_psk_চাবি_get_v4)LIB_SYM(lib, "mosquitto_auth_psk_key_get");
	যদি(plugin->lib.psk_চাবি_get_v4){
		log__printf(রিক্ত, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking enabled.");
	}অন্যথা{
		log__printf(রিক্ত, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking not enabled.");
	}

	plugin->lib.auth_start_v4 = (FUNC_auth_plugin_auth_start_v4)LIB_SYM(lib, "mosquitto_auth_start");
	plugin->lib.auth_continue_v4 = (FUNC_auth_plugin_auth_continue_v4)LIB_SYM(lib, "mosquitto_auth_continue");

	যদি(plugin->lib.auth_start_v4){
		যদি(plugin->lib.auth_continue_v4){
			log__printf(রিক্ত, MOSQ_LOG_INFO,
					" └── Extended authentication enabled.");
		}অন্যথা{
			log__printf(রিক্ত, MOSQ_LOG_ERR,
					"Error: Plugin has missing mosquitto_auth_continue() function.");
			ঘূৰাই MOSQ_ERR_UNKNOWN;
		}
	}অন্যথা{
		log__printf(রিক্ত, MOSQ_LOG_INFO,
				" └── Extended authentication not enabled.");
	}

	plugin->lib.lib = lib;
	plugin->lib.user_data = রিক্ত;
	plugin->lib.identifier = plugin;

	যদি(plugin->lib.plugin_init_v4){
		rc = plugin->lib.plugin_init_v4(&plugin->lib.user_data, plugin->config.options, plugin->config.option_গুনতি);
		যদি(rc){
			log__printf(রিক্ত, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			ঘূৰাই rc;
		}
	}

	mosquitto_callback_register(plugin, MOSQ_EVT_RELOAD, plugin_v4_reload, রিক্ত, plugin);

	যদি(plugin->lib.unpwd_check_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v4_basic_auth, রিক্ত, plugin);
	}
	যদি(plugin->lib.acl_check_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_ACL_CHECK, plugin_v4_acl_check, রিক্ত, plugin);
	}
	যদি(plugin->lib.auth_start_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_EXT_AUTH_START, plugin_v4_auth_start, রিক্ত, plugin);
	}
	যদি(plugin->lib.auth_continue_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_EXT_AUTH_CONTINUE, plugin_v4_auth_continue, রিক্ত, plugin);
	}
	যদি(plugin->lib.psk_চাবি_get_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_PSK_চাবি, plugin_v4_psk_চাবি_get, রিক্ত, plugin);
	}

	ঘূৰাই 0;
}
