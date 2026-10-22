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

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "mosquitto/broker.h"
#include "mosquitto_broker_internal.h"
#include "mosquitto/broker_plugin.h"
#include "lib_load.h"
#include "utlist.h"

typedef целое (*FUNC_auth_plugin_version)(void);
typedef целое (*FUNC_plugin_version)(целое, const целое *);


static целое plugin_v3_basic_auth(целое event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	struct mosquitto_evt_basic_auth *ed = event_data;

	UNUSED(event);

	если(plugin->lib.unpwd_check_v3 == NULL){
		вернуть MOSQ_ERR_INVAL;
	}

	вернуть plugin->lib.unpwd_check_v3(
			plugin->lib.user_data,
			ed->client,
			ed->username,
			ed->password);
}


static целое plugin_v3_acl_check(целое event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	struct mosquitto_evt_acl_check *ed = event_data;
	struct mosquitto_acl_msg msg;
	целое rc;

	UNUSED(event);

	если(plugin->lib.acl_check_v3 == NULL){
		вернуть MOSQ_ERR_INVAL;
	}

	memset(&msg, 0, sizeof(msg));
	msg.topic = ed->topic;
	msg.payloadlen = ed->payloadlen;
	msg.payload = ed->payload;
	msg.qos = ed->qos;
	msg.retain = ed->retain;

	rc = acl__pre_check(plugin, ed->client, ed->access);
	если(rc == MOSQ_ERR_PLUGIN_DEFER){
		вернуть plugin->lib.acl_check_v3(
				plugin->lib.user_data,
				ed->access,
				ed->client,
				&msg);
	}иначе{
		вернуть rc;
	}
}


static целое plugin_v3_psk_key_get(целое event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	struct mosquitto_evt_psk_key *ed = event_data;

	UNUSED(event);

	если(plugin->lib.psk_key_get_v3 == NULL){
		вернуть MOSQ_ERR_INVAL;
	}

	вернуть plugin->lib.psk_key_get_v3(
			plugin->lib.user_data,
			ed->client,
			ed->hint,
			ed->identity,
			ed->key,
			ed->max_key_len);
}


static целое plugin_v3_reload(целое event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	целое rc;

	UNUSED(event);
	UNUSED(event_data);

	rc = plugin->lib.security_cleanup_v3(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_count,
			true);
	если(rc){
		вернуть rc;
	}

	rc = plugin->lib.security_init_v3(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_count,
			true);
	вернуть rc;
}


целое plugin__load_v3(mosquitto_plugin_id_t *plugin, void *lib)
{
	целое rc;

	если(!(plugin->lib.plugin_init_v3 = (FUNC_auth_plugin_init_v3)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		вернуть MOSQ_ERR_UNKNOWN;
	}
	если(!(plugin->lib.plugin_cleanup_v3 = (FUNC_auth_plugin_cleanup_v3)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		вернуть MOSQ_ERR_UNKNOWN;
	}

	если(!(plugin->lib.security_init_v3 = (FUNC_auth_plugin_security_init_v3)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		вернуть MOSQ_ERR_UNKNOWN;
	}

	если(!(plugin->lib.security_cleanup_v3 = (FUNC_auth_plugin_security_cleanup_v3)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		вернуть MOSQ_ERR_UNKNOWN;
	}

	если(!(plugin->lib.acl_check_v3 = (FUNC_auth_plugin_acl_check_v3)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		вернуть MOSQ_ERR_UNKNOWN;
	}

	если(!(plugin->lib.unpwd_check_v3 = (FUNC_auth_plugin_unpwd_check_v3)LIB_SYM(lib, "mosquitto_auth_unpwd_check"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_unpwd_check().");
		LIB_ERROR();
		вернуть MOSQ_ERR_UNKNOWN;
	}

	если(!(plugin->lib.psk_key_get_v3 = (FUNC_auth_plugin_psk_key_get_v3)LIB_SYM(lib, "mosquitto_auth_psk_key_get"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_psk_key_get().");
		LIB_ERROR();
		вернуть MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = lib;
	plugin->lib.user_data = NULL;
	plugin->lib.identifier = plugin;
	если(plugin->lib.plugin_init_v3){
		rc = plugin->lib.plugin_init_v3(&plugin->lib.user_data, plugin->config.options, plugin->config.option_count);
		если(rc){
			log__printf(NULL, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			вернуть rc;
		}
	}

	mosquitto_callback_register(plugin, MOSQ_EVT_RELOAD, plugin_v3_reload, NULL, plugin);

	если(plugin->lib.unpwd_check_v3){
		mosquitto_callback_register(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v3_basic_auth, NULL, plugin);
	}
	если(plugin->lib.acl_check_v3){
		mosquitto_callback_register(plugin, MOSQ_EVT_ACL_CHECK, plugin_v3_acl_check, NULL, plugin);
	}
	если(plugin->lib.psk_key_get_v3){
		mosquitto_callback_register(plugin, MOSQ_EVT_PSK_KEY, plugin_v3_psk_key_get, NULL, plugin);
	}

	вернуть 0;
}


