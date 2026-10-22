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

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "mosquitto/broker.h"
#include "mosquitto_broker_internal.h"
#include "mosquitto/broker_plugin.h"
#include "lib_load.h"
#include "utlist.h"

typedef цяло (*FUNC_auth_plugin_version)(void);
typedef цяло (*FUNC_plugin_version)(цяло, const цяло *);


static цяло plugin_v2_basic_auth(цяло event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	struct mosquitto_evt_basic_auth *ed = event_data;

	UNUSED(event);

	ако(plugin->lib.unpwd_check_v2 == NULL){
		върни MOSQ_ERR_INVAL;
	}

	върни plugin->lib.unpwd_check_v2(
			plugin->lib.user_data,
			ed->username,
			ed->password);
}


static цяло plugin_v2_acl_check(цяло event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	struct mosquitto_evt_acl_check *ed = event_data;
	цяло rc;

	UNUSED(event);

	ако(plugin->lib.acl_check_v2 == NULL){
		върни MOSQ_ERR_INVAL;
	}

	rc = acl__pre_check(plugin, ed->client, ed->access);
	ако(rc == MOSQ_ERR_PLUGIN_DEFER){
		върни plugin->lib.acl_check_v2(
				plugin->lib.user_data,
				ed->client->id,
				ed->client->username,
				ed->topic,
				ed->access);
	}иначе{
		върни rc;
	}
}


static цяло plugin_v2_psk_key_get(цяло event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	struct mosquitto_evt_psk_key *ed = event_data;

	UNUSED(event);

	ако(plugin->lib.psk_key_get_v2 == NULL){
		върни MOSQ_ERR_INVAL;
	}

	върни plugin->lib.psk_key_get_v2(
			plugin->lib.user_data,
			ed->hint,
			ed->identity,
			ed->key,
			ed->max_key_len);
}


static цяло plugin_v2_reload(цяло event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	цяло rc;

	UNUSED(event);
	UNUSED(event_data);

	rc = plugin->lib.security_cleanup_v2(
			plugin->lib.user_data,
			(struct mosquitto_auth_opt *)plugin->config.options,
			plugin->config.option_count,
			true);
	ако(rc){
		върни rc;
	}

	rc = plugin->lib.security_init_v2(
			plugin->lib.user_data,
			(struct mosquitto_auth_opt *)plugin->config.options,
			plugin->config.option_count,
			true);
	върни rc;
}


цяло plugin__load_v2(mosquitto_plugin_id_t *plugin, void *lib)
{
	цяло rc;

	ако(!(plugin->lib.plugin_init_v2 = (FUNC_auth_plugin_init_v2)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		върни MOSQ_ERR_UNKNOWN;
	}
	ако(!(plugin->lib.plugin_cleanup_v2 = (FUNC_auth_plugin_cleanup_v2)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		върни MOSQ_ERR_UNKNOWN;
	}

	ако(!(plugin->lib.security_init_v2 = (FUNC_auth_plugin_security_init_v2)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		върни MOSQ_ERR_UNKNOWN;
	}

	ако(!(plugin->lib.security_cleanup_v2 = (FUNC_auth_plugin_security_cleanup_v2)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		върни MOSQ_ERR_UNKNOWN;
	}

	ако(!(plugin->lib.acl_check_v2 = (FUNC_auth_plugin_acl_check_v2)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		върни MOSQ_ERR_UNKNOWN;
	}

	ако(!(plugin->lib.unpwd_check_v2 = (FUNC_auth_plugin_unpwd_check_v2)LIB_SYM(lib, "mosquitto_auth_unpwd_check"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_unpwd_check().");
		LIB_ERROR();
		върни MOSQ_ERR_UNKNOWN;
	}

	ако(!(plugin->lib.psk_key_get_v2 = (FUNC_auth_plugin_psk_key_get_v2)LIB_SYM(lib, "mosquitto_auth_psk_key_get"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_psk_key_get().");
		LIB_ERROR();
		върни MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = lib;
	plugin->lib.user_data = NULL;
	plugin->lib.identifier = plugin;

	ако(plugin->lib.plugin_init_v2){
		rc = plugin->lib.plugin_init_v2(
				&plugin->lib.user_data,
				(struct mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_count);
		ако(rc){
			log__printf(NULL, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			върни rc;
		}
	}

	mosquitto_callback_register(plugin, MOSQ_EVT_RELOAD, plugin_v2_reload, NULL, plugin);

	ако(plugin->lib.unpwd_check_v2){
		mosquitto_callback_register(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v2_basic_auth, NULL, plugin);
	}
	ако(plugin->lib.acl_check_v2){
		mosquitto_callback_register(plugin, MOSQ_EVT_ACL_CHECK, plugin_v2_acl_check, NULL, plugin);
	}
	ако(plugin->lib.psk_key_get_v2){
		mosquitto_callback_register(plugin, MOSQ_EVT_PSK_KEY, plugin_v2_psk_key_get, NULL, plugin);
	}

	върни 0;
}
