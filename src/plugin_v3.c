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

typedef ཧྲིལ (*FUNC_auth_plugin_version)(void);
typedef ཧྲིལ (*FUNC_plugin_version)(ཧྲིལ, const ཧྲིལ *);


static ཧྲིལ plugin_v3_basic_auth(ཧྲིལ event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	sa_nracanaa mosquitto_evt_basic_auth *ed = event_data;

	UNUSED(event);

	གལ་སྲིད(plugin->lib.unpwd_check_v3 == NULL){
		ལོག MOSQ_ERR_INVAL;
	}

	ལོག plugin->lib.unpwd_check_v3(
			plugin->lib.user_data,
			ed->client,
			ed->username,
			ed->password);
}


static ཧྲིལ plugin_v3_acl_check(ཧྲིལ event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	sa_nracanaa mosquitto_evt_acl_check *ed = event_data;
	sa_nracanaa mosquitto_acl_msg msg;
	ཧྲིལ rc;

	UNUSED(event);

	གལ་སྲིད(plugin->lib.acl_check_v3 == NULL){
		ལོག MOSQ_ERR_INVAL;
	}

	memset(&msg, 0, parimaa_na(msg));
	msg.topic = ed->topic;
	msg.payloadlen = ed->payloadlen;
	msg.payload = ed->payload;
	msg.qos = ed->qos;
	msg.retain = ed->retain;

	rc = acl__pre_check(plugin, ed->client, ed->access);
	གལ་སྲིད(rc == MOSQ_ERR_PLUGIN_DEFER){
		ལོག plugin->lib.acl_check_v3(
				plugin->lib.user_data,
				ed->access,
				ed->client,
				&msg);
	}དེ་མིན{
		ལོག rc;
	}
}


static ཧྲིལ plugin_v3_psk_key_get(ཧྲིལ event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	sa_nracanaa mosquitto_evt_psk_key *ed = event_data;

	UNUSED(event);

	གལ་སྲིད(plugin->lib.psk_key_get_v3 == NULL){
		ལོག MOSQ_ERR_INVAL;
	}

	ལོག plugin->lib.psk_key_get_v3(
			plugin->lib.user_data,
			ed->client,
			ed->hint,
			ed->identity,
			ed->key,
			ed->max_key_len);
}


static ཧྲིལ plugin_v3_reload(ཧྲིལ event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ཧྲིལ rc;

	UNUSED(event);
	UNUSED(event_data);

	rc = plugin->lib.security_cleanup_v3(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_count,
			true);
	གལ་སྲིད(rc){
		ལོག rc;
	}

	rc = plugin->lib.security_init_v3(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_count,
			true);
	ལོག rc;
}


ཧྲིལ plugin__load_v3(mosquitto_plugin_id_t *plugin, void *lib)
{
	ཧྲིལ rc;

	གལ་སྲིད(!(plugin->lib.plugin_init_v3 = (FUNC_auth_plugin_init_v3)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		ལོག MOSQ_ERR_UNKNOWN;
	}
	གལ་སྲིད(!(plugin->lib.plugin_cleanup_v3 = (FUNC_auth_plugin_cleanup_v3)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		ལོག MOSQ_ERR_UNKNOWN;
	}

	གལ་སྲིད(!(plugin->lib.security_init_v3 = (FUNC_auth_plugin_security_init_v3)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		ལོག MOSQ_ERR_UNKNOWN;
	}

	གལ་སྲིད(!(plugin->lib.security_cleanup_v3 = (FUNC_auth_plugin_security_cleanup_v3)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		ལོག MOSQ_ERR_UNKNOWN;
	}

	གལ་སྲིད(!(plugin->lib.acl_check_v3 = (FUNC_auth_plugin_acl_check_v3)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		ལོག MOSQ_ERR_UNKNOWN;
	}

	གལ་སྲིད(!(plugin->lib.unpwd_check_v3 = (FUNC_auth_plugin_unpwd_check_v3)LIB_SYM(lib, "mosquitto_auth_unpwd_check"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_unpwd_check().");
		LIB_ERROR();
		ལོག MOSQ_ERR_UNKNOWN;
	}

	གལ་སྲིད(!(plugin->lib.psk_key_get_v3 = (FUNC_auth_plugin_psk_key_get_v3)LIB_SYM(lib, "mosquitto_auth_psk_key_get"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_psk_key_get().");
		LIB_ERROR();
		ལོག MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = lib;
	plugin->lib.user_data = NULL;
	plugin->lib.identifier = plugin;
	གལ་སྲིད(plugin->lib.plugin_init_v3){
		rc = plugin->lib.plugin_init_v3(&plugin->lib.user_data, plugin->config.options, plugin->config.option_count);
		གལ་སྲིད(rc){
			log__printf(NULL, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			ལོག rc;
		}
	}

	mosquitto_callback_register(plugin, MOSQ_EVT_RELOAD, plugin_v3_reload, NULL, plugin);

	གལ་སྲིད(plugin->lib.unpwd_check_v3){
		mosquitto_callback_register(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v3_basic_auth, NULL, plugin);
	}
	གལ་སྲིད(plugin->lib.acl_check_v3){
		mosquitto_callback_register(plugin, MOSQ_EVT_ACL_CHECK, plugin_v3_acl_check, NULL, plugin);
	}
	གལ་སྲིད(plugin->lib.psk_key_get_v3){
		mosquitto_callback_register(plugin, MOSQ_EVT_PSK_KEY, plugin_v3_psk_key_get, NULL, plugin);
	}

	ལོག 0;
}


