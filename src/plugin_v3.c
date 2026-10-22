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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "mosquitto/broker.h"
#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/broker_plugin.h"
#অন্তর্ভাব "lib_load.h"
#অন্তর্ভাব "utlist.h"

প্রকারলেখ পূর্ণসংখ্যা (*FUNC_auth_plugin_version)(শূন্য);
প্রকারলেখ পূর্ণসংখ্যা (*FUNC_plugin_version)(পূর্ণসংখ্যা, ধ্রুবক পূর্ণসংখ্যা *);


স্থির পূর্ণসংখ্যা plugin_v3_basic_auth(পূর্ণসংখ্যা event, শূন্য *event_তথ্য, শূন্য *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	কাঠামো mosquitto_evt_basic_auth *ed = event_তথ্য;

	UNUSED(event);

	যদি(plugin->lib.unpwd_check_v3 == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}

	ফেরত plugin->lib.unpwd_check_v3(
			plugin->lib.user_তথ্য,
			ed->client,
			ed->username,
			ed->password);
}


স্থির পূর্ণসংখ্যা plugin_v3_acl_check(পূর্ণসংখ্যা event, শূন্য *event_তথ্য, শূন্য *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	কাঠামো mosquitto_evt_acl_check *ed = event_তথ্য;
	কাঠামো mosquitto_acl_বার্তা বার্তা;
	পূর্ণসংখ্যা rc;

	UNUSED(event);

	যদি(plugin->lib.acl_check_v3 == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}

	memset(&বার্তা, 0, আকার(বার্তা));
	বার্তা.topic = ed->topic;
	বার্তা.payloadlen = ed->payloadlen;
	বার্তা.payload = ed->payload;
	বার্তা.qos = ed->qos;
	বার্তা.retain = ed->retain;

	rc = acl__pre_check(plugin, ed->client, ed->access);
	যদি(rc == MOSQ_ERR_PLUGIN_DEFER){
		ফেরত plugin->lib.acl_check_v3(
				plugin->lib.user_তথ্য,
				ed->access,
				ed->client,
				&বার্তা);
	}অন্যথা{
		ফেরত rc;
	}
}


স্থির পূর্ণসংখ্যা plugin_v3_psk_চাবি_get(পূর্ণসংখ্যা event, শূন্য *event_তথ্য, শূন্য *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	কাঠামো mosquitto_evt_psk_চাবি *ed = event_তথ্য;

	UNUSED(event);

	যদি(plugin->lib.psk_চাবি_get_v3 == ফাঁকা){
		ফেরত MOSQ_ERR_INVAL;
	}

	ফেরত plugin->lib.psk_চাবি_get_v3(
			plugin->lib.user_তথ্য,
			ed->client,
			ed->hint,
			ed->identity,
			ed->চাবি,
			ed->max_চাবি_len);
}


স্থির পূর্ণসংখ্যা plugin_v3_reload(পূর্ণসংখ্যা event, শূন্য *event_তথ্য, শূন্য *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	পূর্ণসংখ্যা rc;

	UNUSED(event);
	UNUSED(event_তথ্য);

	rc = plugin->lib.security_cleanup_v3(
			plugin->lib.user_তথ্য,
			plugin->config.options,
			plugin->config.option_গুনতি,
			true);
	যদি(rc){
		ফেরত rc;
	}

	rc = plugin->lib.security_init_v3(
			plugin->lib.user_তথ্য,
			plugin->config.options,
			plugin->config.option_গুনতি,
			true);
	ফেরত rc;
}


পূর্ণসংখ্যা plugin__load_v3(mosquitto_plugin_id_t *plugin, শূন্য *lib)
{
	পূর্ণসংখ্যা rc;

	যদি(!(plugin->lib.plugin_init_v3 = (FUNC_auth_plugin_init_v3)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(ফাঁকা, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		ফেরত MOSQ_ERR_UNKNOWN;
	}
	যদি(!(plugin->lib.plugin_cleanup_v3 = (FUNC_auth_plugin_cleanup_v3)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(ফাঁকা, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		ফেরত MOSQ_ERR_UNKNOWN;
	}

	যদি(!(plugin->lib.security_init_v3 = (FUNC_auth_plugin_security_init_v3)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(ফাঁকা, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		ফেরত MOSQ_ERR_UNKNOWN;
	}

	যদি(!(plugin->lib.security_cleanup_v3 = (FUNC_auth_plugin_security_cleanup_v3)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(ফাঁকা, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		ফেরত MOSQ_ERR_UNKNOWN;
	}

	যদি(!(plugin->lib.acl_check_v3 = (FUNC_auth_plugin_acl_check_v3)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(ফাঁকা, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		ফেরত MOSQ_ERR_UNKNOWN;
	}

	যদি(!(plugin->lib.unpwd_check_v3 = (FUNC_auth_plugin_unpwd_check_v3)LIB_SYM(lib, "mosquitto_auth_unpwd_check"))){
		log__printf(ফাঁকা, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_unpwd_check().");
		LIB_ERROR();
		ফেরত MOSQ_ERR_UNKNOWN;
	}

	যদি(!(plugin->lib.psk_চাবি_get_v3 = (FUNC_auth_plugin_psk_চাবি_get_v3)LIB_SYM(lib, "mosquitto_auth_psk_key_get"))){
		log__printf(ফাঁকা, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_psk_key_get().");
		LIB_ERROR();
		ফেরত MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = lib;
	plugin->lib.user_তথ্য = ফাঁকা;
	plugin->lib.identifier = plugin;
	যদি(plugin->lib.plugin_init_v3){
		rc = plugin->lib.plugin_init_v3(&plugin->lib.user_তথ্য, plugin->config.options, plugin->config.option_গুনতি);
		যদি(rc){
			log__printf(ফাঁকা, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			ফেরত rc;
		}
	}

	mosquitto_callback_register(plugin, MOSQ_EVT_RELOAD, plugin_v3_reload, ফাঁকা, plugin);

	যদি(plugin->lib.unpwd_check_v3){
		mosquitto_callback_register(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v3_basic_auth, ফাঁকা, plugin);
	}
	যদি(plugin->lib.acl_check_v3){
		mosquitto_callback_register(plugin, MOSQ_EVT_ACL_CHECK, plugin_v3_acl_check, ফাঁকা, plugin);
	}
	যদি(plugin->lib.psk_চাবি_get_v3){
		mosquitto_callback_register(plugin, MOSQ_EVT_PSK_চাবি, plugin_v3_psk_চাবি_get, ফাঁকা, plugin);
	}

	ফেরত 0;
}


