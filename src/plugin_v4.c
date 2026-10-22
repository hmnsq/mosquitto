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

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "mosquitto/broker.h"
#include "mosquitto_broker_internal.h"
#include "mosquitto/broker_plugin.h"
#include "lib_load.h"
#include "utlist.h"

typedef puur_naa_nka (*FUNC_auth_plugin_version)(void);
typedef puur_naa_nka (*FUNC_plugin_version)(puur_naa_nka, const puur_naa_nka *);


static puur_naa_nka plugin_v4_basic_auth(puur_naa_nka event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	sa_nracanaa mosquitto_evt_basic_auth *ed = event_data;

	UNUSED(event);

	ᮚᮦᮔ᮪(plugin->lib.unpwd_check_v4 == NULL){
		ᮝᮜᮤ MOSQ_ERR_PLUGIN_DEFER;
	}

	ᮝᮜᮤ plugin->lib.unpwd_check_v4(
			plugin->lib.user_data,
			ed->client,
			ed->username,
			ed->password);
}


static puur_naa_nka plugin_v4_acl_check(puur_naa_nka event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	sa_nracanaa mosquitto_evt_acl_check *ed = event_data;
	sa_nracanaa mosquitto_acl_msg msg;
	puur_naa_nka rc;

	UNUSED(event);

	ᮚᮦᮔ᮪(plugin->lib.acl_check_v4 == NULL){
		ᮝᮜᮤ MOSQ_ERR_PLUGIN_DEFER;
	}

	memset(&msg, 0, parimaa_na(msg));
	msg.topic = ed->topic;
	msg.payloadlen = ed->payloadlen;
	msg.payload = ed->payload;
	msg.qos = ed->qos;
	msg.retain = ed->retain;

	rc = acl__pre_check(plugin, ed->client, ed->access);
	ᮚᮦᮔ᮪(rc == MOSQ_ERR_PLUGIN_DEFER){
		ᮝᮜᮤ plugin->lib.acl_check_v4(
				plugin->lib.user_data,
				ed->access,
				ed->client,
				&msg);
	}ᮜᮤᮚ{
		ᮝᮜᮤ rc;
	}
}


static puur_naa_nka plugin_v4_auth_start(puur_naa_nka event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	sa_nracanaa mosquitto_evt_extended_auth *ed = event_data;

	UNUSED(event);

	ᮚᮦᮔ᮪(plugin->lib.auth_start_v4 == NULL){
		ᮝᮜᮤ MOSQ_ERR_PLUGIN_DEFER;
	}

	ᮝᮜᮤ plugin->lib.auth_start_v4(
			plugin->lib.user_data,
			ed->client,
			ed->client->auth_method,
			false,
			ed->data_in, ed->data_in_len,
			&ed->data_out, &ed->data_out_len);
}


static puur_naa_nka plugin_v4_auth_continue(puur_naa_nka event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	sa_nracanaa mosquitto_evt_extended_auth *ed = event_data;

	UNUSED(event);

	ᮚᮦᮔ᮪(plugin->lib.auth_continue_v4 == NULL){
		ᮝᮜᮤ MOSQ_ERR_PLUGIN_DEFER;
	}

	ᮝᮜᮤ plugin->lib.auth_continue_v4(
			plugin->lib.user_data,
			ed->client,
			ed->client->auth_method,
			ed->data_in, ed->data_in_len,
			&ed->data_out, &ed->data_out_len);
}


static puur_naa_nka plugin_v4_psk_key_get(puur_naa_nka event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	sa_nracanaa mosquitto_evt_psk_key *ed = event_data;

	UNUSED(event);

	ᮚᮦᮔ᮪(plugin->lib.psk_key_get_v4 == NULL){
		ᮝᮜᮤ MOSQ_ERR_PLUGIN_DEFER;
	}

	ᮝᮜᮤ plugin->lib.psk_key_get_v4(
			plugin->lib.user_data,
			ed->client,
			ed->hint,
			ed->identity,
			ed->key,
			ed->max_key_len);
}


static puur_naa_nka plugin_v4_reload(puur_naa_nka event, void *event_data, void *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	puur_naa_nka rc;

	UNUSED(event);
	UNUSED(event_data);

	rc = plugin->lib.security_cleanup_v4(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_count,
			true);
	ᮚᮦᮔ᮪(rc){
		ᮝᮜᮤ rc;
	}

	rc = plugin->lib.security_init_v4(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_count,
			true);
	ᮝᮜᮤ rc;
}


puur_naa_nka plugin__load_v4(mosquitto_plugin_id_t *plugin, void *lib)
{
	puur_naa_nka rc;

	ᮚᮦᮔ᮪(!(plugin->lib.plugin_init_v4 = (FUNC_auth_plugin_init_v4)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		ᮝᮜᮤ MOSQ_ERR_UNKNOWN;
	}
	ᮚᮦᮔ᮪(!(plugin->lib.plugin_cleanup_v4 = (FUNC_auth_plugin_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		ᮝᮜᮤ MOSQ_ERR_UNKNOWN;
	}

	ᮚᮦᮔ᮪(!(plugin->lib.security_init_v4 = (FUNC_auth_plugin_security_init_v4)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		ᮝᮜᮤ MOSQ_ERR_UNKNOWN;
	}

	ᮚᮦᮔ᮪(!(plugin->lib.security_cleanup_v4 = (FUNC_auth_plugin_security_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		ᮝᮜᮤ MOSQ_ERR_UNKNOWN;
	}

	ᮚᮦᮔ᮪(!(plugin->lib.acl_check_v4 = (FUNC_auth_plugin_acl_check_v4)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(NULL, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		ᮝᮜᮤ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.unpwd_check_v4 = (FUNC_auth_plugin_unpwd_check_v4)LIB_SYM(lib, "mosquitto_auth_unpwd_check");
	ᮚᮦᮔ᮪(plugin->lib.unpwd_check_v4){
		log__printf(NULL, MOSQ_LOG_INFO,
				" ├── Username/password checking enabled.");
	}ᮜᮤᮚ{
		log__printf(NULL, MOSQ_LOG_INFO,
				" ├── Username/password checking not enabled.");
	}

	plugin->lib.psk_key_get_v4 = (FUNC_auth_plugin_psk_key_get_v4)LIB_SYM(lib, "mosquitto_auth_psk_key_get");
	ᮚᮦᮔ᮪(plugin->lib.psk_key_get_v4){
		log__printf(NULL, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking enabled.");
	}ᮜᮤᮚ{
		log__printf(NULL, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking not enabled.");
	}

	plugin->lib.auth_start_v4 = (FUNC_auth_plugin_auth_start_v4)LIB_SYM(lib, "mosquitto_auth_start");
	plugin->lib.auth_continue_v4 = (FUNC_auth_plugin_auth_continue_v4)LIB_SYM(lib, "mosquitto_auth_continue");

	ᮚᮦᮔ᮪(plugin->lib.auth_start_v4){
		ᮚᮦᮔ᮪(plugin->lib.auth_continue_v4){
			log__printf(NULL, MOSQ_LOG_INFO,
					" └── Extended authentication enabled.");
		}ᮜᮤᮚ{
			log__printf(NULL, MOSQ_LOG_ERR,
					"Error: Plugin has missing mosquitto_auth_continue() function.");
			ᮝᮜᮤ MOSQ_ERR_UNKNOWN;
		}
	}ᮜᮤᮚ{
		log__printf(NULL, MOSQ_LOG_INFO,
				" └── Extended authentication not enabled.");
	}

	plugin->lib.lib = lib;
	plugin->lib.user_data = NULL;
	plugin->lib.identifier = plugin;

	ᮚᮦᮔ᮪(plugin->lib.plugin_init_v4){
		rc = plugin->lib.plugin_init_v4(&plugin->lib.user_data, plugin->config.options, plugin->config.option_count);
		ᮚᮦᮔ᮪(rc){
			log__printf(NULL, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			ᮝᮜᮤ rc;
		}
	}

	mosquitto_callback_register(plugin, MOSQ_EVT_RELOAD, plugin_v4_reload, NULL, plugin);

	ᮚᮦᮔ᮪(plugin->lib.unpwd_check_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v4_basic_auth, NULL, plugin);
	}
	ᮚᮦᮔ᮪(plugin->lib.acl_check_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_ACL_CHECK, plugin_v4_acl_check, NULL, plugin);
	}
	ᮚᮦᮔ᮪(plugin->lib.auth_start_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_EXT_AUTH_START, plugin_v4_auth_start, NULL, plugin);
	}
	ᮚᮦᮔ᮪(plugin->lib.auth_continue_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_EXT_AUTH_CONTINUE, plugin_v4_auth_continue, NULL, plugin);
	}
	ᮚᮦᮔ᮪(plugin->lib.psk_key_get_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_PSK_KEY, plugin_v4_psk_key_get, NULL, plugin);
	}

	ᮝᮜᮤ 0;
}
