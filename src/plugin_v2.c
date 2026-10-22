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

typedef ཧྲིལ (*FUNC_auth_plugin_version)(void);
typedef ཧྲིལ (*FUNC_plugin_version)(ཧྲིལ, const ཧྲིལ *);


static ཧྲིལ plugin_v2_basic_auth(ཧྲིལ event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	sa_nracanaa mosquitto_evt_basic_auth *ed = event_data;

	UNUSED(event);

	གལ་ཏེ(plugin->lib.unpwd_check_v2 == NULL){
		ཕྱིར MOSQ_ERR_INVAL;
	}

	ཕྱིར plugin->lib.unpwd_check_v2(
			plugin->lib.user_data,
			ed->username,
			ed->password);
}


static ཧྲིལ plugin_v2_acl_check(ཧྲིལ event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	sa_nracanaa mosquitto_evt_acl_check *ed = event_data;
	ཧྲིལ rc;

	UNUSED(event);

	གལ་ཏེ(plugin->lib.acl_check_v2 == NULL){
		ཕྱིར MOSQ_ERR_INVAL;
	}

	rc = acl__pre_check(plugin, ed->client, ed->access);
	གལ་ཏེ(rc == MOSQ_ERR_PLUGIN_DEFER){
		ཕྱིར plugin->lib.acl_check_v2(
				plugin->lib.user_data,
				ed->client->id,
				ed->client->username,
				ed->topic,
				ed->access);
	}དེ་མིན{
		ཕྱིར rc;
	}
}


static ཧྲིལ plugin_v2_psk_key_get(ཧྲིལ event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	sa_nracanaa mosquitto_evt_psk_key *ed = event_data;

	UNUSED(event);

	གལ་ཏེ(plugin->lib.psk_key_get_v2 == NULL){
		ཕྱིར MOSQ_ERR_INVAL;
	}

	ཕྱིར plugin->lib.psk_key_get_v2(
			plugin->lib.user_data,
			ed->hint,
			ed->identity,
			ed->key,
			ed->max_key_len);
}


static ཧྲིལ plugin_v2_reload(ཧྲིལ event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ཧྲིལ rc;

	UNUSED(event);
	UNUSED(event_data);

	rc = plugin->lib.security_cleanup_v2(
			plugin->lib.user_data,
			(sa_nracanaa mosquitto_auth_opt *)plugin->config.options,
			plugin->config.option_count,
			true);
	གལ་ཏེ(rc){
		ཕྱིར rc;
	}

	rc = plugin->lib.security_init_v2(
			plugin->lib.user_data,
			(sa_nracanaa mosquitto_auth_opt *)plugin->config.options,
			plugin->config.option_count,
			true);
	ཕྱིར rc;
}


ཧྲིལ plugin__load_v2(mosquitto_plugin_id_t *plugin, void *lib)
{
	ཧྲིལ rc;

	གལ་ཏེ(!(plugin->lib.plugin_init_v2 = (FUNC_auth_plugin_init_v2)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		ཕྱིར MOSQ_ERR_UNKNOWN;
	}
	གལ་ཏེ(!(plugin->lib.plugin_cleanup_v2 = (FUNC_auth_plugin_cleanup_v2)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		ཕྱིར MOSQ_ERR_UNKNOWN;
	}

	གལ་ཏེ(!(plugin->lib.security_init_v2 = (FUNC_auth_plugin_security_init_v2)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		ཕྱིར MOSQ_ERR_UNKNOWN;
	}

	གལ་ཏེ(!(plugin->lib.security_cleanup_v2 = (FUNC_auth_plugin_security_cleanup_v2)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		ཕྱིར MOSQ_ERR_UNKNOWN;
	}

	གལ་ཏེ(!(plugin->lib.acl_check_v2 = (FUNC_auth_plugin_acl_check_v2)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		ཕྱིར MOSQ_ERR_UNKNOWN;
	}

	གལ་ཏེ(!(plugin->lib.unpwd_check_v2 = (FUNC_auth_plugin_unpwd_check_v2)LIB_SYM(lib, "mosquitto_auth_unpwd_check"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_unpwd_check().");
		LIB_ERROR();
		ཕྱིར MOSQ_ERR_UNKNOWN;
	}

	གལ་ཏེ(!(plugin->lib.psk_key_get_v2 = (FUNC_auth_plugin_psk_key_get_v2)LIB_SYM(lib, "mosquitto_auth_psk_key_get"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_psk_key_get().");
		LIB_ERROR();
		ཕྱིར MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = lib;
	plugin->lib.user_data = NULL;
	plugin->lib.identifier = plugin;

	གལ་ཏེ(plugin->lib.plugin_init_v2){
		rc = plugin->lib.plugin_init_v2(
				&plugin->lib.user_data,
				(sa_nracanaa mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_count);
		གལ་ཏེ(rc){
			log__printf(NULL, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			ཕྱིར rc;
		}
	}

	mosquitto_callback_register(plugin, MOSQ_EVT_RELOAD, plugin_v2_reload, NULL, plugin);

	གལ་ཏེ(plugin->lib.unpwd_check_v2){
		mosquitto_callback_register(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v2_basic_auth, NULL, plugin);
	}
	གལ་ཏེ(plugin->lib.acl_check_v2){
		mosquitto_callback_register(plugin, MOSQ_EVT_ACL_CHECK, plugin_v2_acl_check, NULL, plugin);
	}
	གལ་ཏེ(plugin->lib.psk_key_get_v2){
		mosquitto_callback_register(plugin, MOSQ_EVT_PSK_KEY, plugin_v2_psk_key_get, NULL, plugin);
	}

	ཕྱིར 0;
}
