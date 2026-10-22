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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto/broker.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/broker_plugin.h"
#ଅନ୍ତର୍ଭାଵ "lib_load.h"
#ଅନ୍ତର୍ଭାଵ "utlist.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_version)(ଶୂନ୍ୟ);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_plugin_version)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା *);


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin_v2_basic_auth(ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ଶୂନ୍ୟ *event_data, ଶୂନ୍ୟ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ସଂରଚନା mosquitto_evt_basic_auth *ed = event_data;

	UNUSED(event);

	ଯଦି(plugin->lib.unpwd_check_v2 == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	ଫେରନ୍ତୁ plugin->lib.unpwd_check_v2(
			plugin->lib.user_data,
			ed->username,
			ed->password);
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin_v2_acl_check(ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ଶୂନ୍ୟ *event_data, ଶୂନ୍ୟ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ସଂରଚନା mosquitto_evt_acl_check *ed = event_data;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	UNUSED(event);

	ଯଦି(plugin->lib.acl_check_v2 == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	rc = acl__pre_check(plugin, ed->client, ed->access);
	ଯଦି(rc == MOSQ_ERR_PLUGIN_DEFER){
		ଫେରନ୍ତୁ plugin->lib.acl_check_v2(
				plugin->lib.user_data,
				ed->client->id,
				ed->client->username,
				ed->topic,
				ed->access);
	}ଅନ୍ୟଥା{
		ଫେରନ୍ତୁ rc;
	}
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin_v2_psk_ଚାବି_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ଶୂନ୍ୟ *event_data, ଶୂନ୍ୟ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ସଂରଚନା mosquitto_evt_psk_ଚାବି *ed = event_data;

	UNUSED(event);

	ଯଦି(plugin->lib.psk_ଚାବି_get_v2 == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	ଫେରନ୍ତୁ plugin->lib.psk_ଚାବି_get_v2(
			plugin->lib.user_data,
			ed->hint,
			ed->identity,
			ed->ଚାବି,
			ed->max_ଚାବି_len);
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin_v2_reload(ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ଶୂନ୍ୟ *event_data, ଶୂନ୍ୟ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	UNUSED(event);
	UNUSED(event_data);

	rc = plugin->lib.security_cleanup_v2(
			plugin->lib.user_data,
			(ସଂରଚନା mosquitto_auth_opt *)plugin->config.options,
			plugin->config.option_ଗଣନା,
			true);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	rc = plugin->lib.security_init_v2(
			plugin->lib.user_data,
			(ସଂରଚନା mosquitto_auth_opt *)plugin->config.options,
			plugin->config.option_ଗଣନା,
			true);
	ଫେରନ୍ତୁ rc;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__load_v2(mosquitto_plugin_id_t *plugin, ଶୂନ୍ୟ *lib)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(!(plugin->lib.plugin_init_v2 = (FUNC_auth_plugin_init_v2)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(ଖାଲି, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}
	ଯଦି(!(plugin->lib.plugin_cleanup_v2 = (FUNC_auth_plugin_cleanup_v2)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(ଖାଲି, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}

	ଯଦି(!(plugin->lib.security_init_v2 = (FUNC_auth_plugin_security_init_v2)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(ଖାଲି, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}

	ଯଦି(!(plugin->lib.security_cleanup_v2 = (FUNC_auth_plugin_security_cleanup_v2)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(ଖାଲି, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}

	ଯଦି(!(plugin->lib.acl_check_v2 = (FUNC_auth_plugin_acl_check_v2)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(ଖାଲି, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}

	ଯଦି(!(plugin->lib.unpwd_check_v2 = (FUNC_auth_plugin_unpwd_check_v2)LIB_SYM(lib, "mosquitto_auth_unpwd_check"))){
		log__printf(ଖାଲି, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_unpwd_check().");
		LIB_ERROR();
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}

	ଯଦି(!(plugin->lib.psk_ଚାବି_get_v2 = (FUNC_auth_plugin_psk_ଚାବି_get_v2)LIB_SYM(lib, "mosquitto_auth_psk_key_get"))){
		log__printf(ଖାଲି, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_psk_key_get().");
		LIB_ERROR();
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = lib;
	plugin->lib.user_data = ଖାଲି;
	plugin->lib.identifier = plugin;

	ଯଦି(plugin->lib.plugin_init_v2){
		rc = plugin->lib.plugin_init_v2(
				&plugin->lib.user_data,
				(ସଂରଚନା mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_ଗଣନା);
		ଯଦି(rc){
			log__printf(ଖାଲି, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			ଫେରନ୍ତୁ rc;
		}
	}

	mosquitto_callback_register(plugin, MOSQ_EVT_RELOAD, plugin_v2_reload, ଖାଲି, plugin);

	ଯଦି(plugin->lib.unpwd_check_v2){
		mosquitto_callback_register(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v2_basic_auth, ଖାଲି, plugin);
	}
	ଯଦି(plugin->lib.acl_check_v2){
		mosquitto_callback_register(plugin, MOSQ_EVT_ACL_CHECK, plugin_v2_acl_check, ଖାଲି, plugin);
	}
	ଯଦି(plugin->lib.psk_ଚାବି_get_v2){
		mosquitto_callback_register(plugin, MOSQ_EVT_PSK_ଚାବି, plugin_v2_psk_ଚାବି_get, ଖାଲି, plugin);
	}

	ଫେରନ୍ତୁ 0;
}
