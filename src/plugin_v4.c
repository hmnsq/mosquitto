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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto/broker.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/broker_plugin.h"
#ಅನ್ತರ್ಭಾವ "lib_load.h"
#ಅನ್ತರ್ಭಾವ "utlist.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_auth_plugin_version)(ಶೂನ್ಯ);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*FUNC_plugin_version)(ಪೂರ್ಣಾಙ್ಕ, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ *);


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ plugin_v4_basic_auth(ಪೂರ್ಣಾಙ್ಕ event, ಶೂನ್ಯ *event_data, ಶೂನ್ಯ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ಸಂರಚನಾ mosquitto_evt_basic_auth *ed = event_data;

	UNUSED(event);

	ಯದಿ(plugin->lib.unpwd_check_v4 == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PLUGIN_DEFER;
	}

	ಪ್ರತ್ಯಾಗಮ plugin->lib.unpwd_check_v4(
			plugin->lib.user_data,
			ed->client,
			ed->username,
			ed->password);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ plugin_v4_acl_check(ಪೂರ್ಣಾಙ್ಕ event, ಶೂನ್ಯ *event_data, ಶೂನ್ಯ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ಸಂರಚನಾ mosquitto_evt_acl_check *ed = event_data;
	ಸಂರಚನಾ mosquitto_acl_ಸಂದೇಶ ಸಂದೇಶ;
	ಪೂರ್ಣಾಙ್ಕ rc;

	UNUSED(event);

	ಯದಿ(plugin->lib.acl_check_v4 == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PLUGIN_DEFER;
	}

	memset(&ಸಂದೇಶ, 0, ಪರಿಮಾಣ(ಸಂದೇಶ));
	ಸಂದೇಶ.topic = ed->topic;
	ಸಂದೇಶ.payloadlen = ed->payloadlen;
	ಸಂದೇಶ.payload = ed->payload;
	ಸಂದೇಶ.qos = ed->qos;
	ಸಂದೇಶ.retain = ed->retain;

	rc = acl__pre_check(plugin, ed->client, ed->access);
	ಯದಿ(rc == MOSQ_ERR_PLUGIN_DEFER){
		ಪ್ರತ್ಯಾಗಮ plugin->lib.acl_check_v4(
				plugin->lib.user_data,
				ed->access,
				ed->client,
				&ಸಂದೇಶ);
	}ಅನ್ಯಥಾ{
		ಪ್ರತ್ಯಾಗಮ rc;
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ plugin_v4_auth_start(ಪೂರ್ಣಾಙ್ಕ event, ಶೂನ್ಯ *event_data, ಶೂನ್ಯ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ಸಂರಚನಾ mosquitto_evt_extended_auth *ed = event_data;

	UNUSED(event);

	ಯದಿ(plugin->lib.auth_start_v4 == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PLUGIN_DEFER;
	}

	ಪ್ರತ್ಯಾಗಮ plugin->lib.auth_start_v4(
			plugin->lib.user_data,
			ed->client,
			ed->client->auth_method,
			false,
			ed->data_in, ed->data_in_len,
			&ed->data_out, &ed->data_out_len);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ plugin_v4_auth_continue(ಪೂರ್ಣಾಙ್ಕ event, ಶೂನ್ಯ *event_data, ಶೂನ್ಯ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ಸಂರಚನಾ mosquitto_evt_extended_auth *ed = event_data;

	UNUSED(event);

	ಯದಿ(plugin->lib.auth_continue_v4 == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PLUGIN_DEFER;
	}

	ಪ್ರತ್ಯಾಗಮ plugin->lib.auth_continue_v4(
			plugin->lib.user_data,
			ed->client,
			ed->client->auth_method,
			ed->data_in, ed->data_in_len,
			&ed->data_out, &ed->data_out_len);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ plugin_v4_psk_ಕೀಲಿ_get(ಪೂರ್ಣಾಙ್ಕ event, ಶೂನ್ಯ *event_data, ಶೂನ್ಯ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ಸಂರಚನಾ mosquitto_evt_psk_ಕೀಲಿ *ed = event_data;

	UNUSED(event);

	ಯದಿ(plugin->lib.psk_ಕೀಲಿ_get_v4 == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PLUGIN_DEFER;
	}

	ಪ್ರತ್ಯಾಗಮ plugin->lib.psk_ಕೀಲಿ_get_v4(
			plugin->lib.user_data,
			ed->client,
			ed->hint,
			ed->identity,
			ed->ಕೀಲಿ,
			ed->max_ಕೀಲಿ_len);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ plugin_v4_reload(ಪೂರ್ಣಾಙ್ಕ event, ಶೂನ್ಯ *event_data, ಶೂನ್ಯ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ಪೂರ್ಣಾಙ್ಕ rc;

	UNUSED(event);
	UNUSED(event_data);

	rc = plugin->lib.security_cleanup_v4(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_ಎಣಿಕೆ,
			true);
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	rc = plugin->lib.security_init_v4(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_ಎಣಿಕೆ,
			true);
	ಪ್ರತ್ಯಾಗಮ rc;
}


ಪೂರ್ಣಾಙ್ಕ plugin__load_v4(mosquitto_plugin_id_t *plugin, ಶೂನ್ಯ *lib)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ(!(plugin->lib.plugin_init_v4 = (FUNC_auth_plugin_init_v4)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}
	ಯದಿ(!(plugin->lib.plugin_cleanup_v4 = (FUNC_auth_plugin_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}

	ಯದಿ(!(plugin->lib.security_init_v4 = (FUNC_auth_plugin_security_init_v4)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}

	ಯದಿ(!(plugin->lib.security_cleanup_v4 = (FUNC_auth_plugin_security_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}

	ಯದಿ(!(plugin->lib.acl_check_v4 = (FUNC_auth_plugin_acl_check_v4)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.unpwd_check_v4 = (FUNC_auth_plugin_unpwd_check_v4)LIB_SYM(lib, "mosquitto_auth_unpwd_check");
	ಯದಿ(plugin->lib.unpwd_check_v4){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO,
				" ├── Username/password checking enabled.");
	}ಅನ್ಯಥಾ{
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO,
				" ├── Username/password checking not enabled.");
	}

	plugin->lib.psk_ಕೀಲಿ_get_v4 = (FUNC_auth_plugin_psk_ಕೀಲಿ_get_v4)LIB_SYM(lib, "mosquitto_auth_psk_key_get");
	ಯದಿ(plugin->lib.psk_ಕೀಲಿ_get_v4){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking enabled.");
	}ಅನ್ಯಥಾ{
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking not enabled.");
	}

	plugin->lib.auth_start_v4 = (FUNC_auth_plugin_auth_start_v4)LIB_SYM(lib, "mosquitto_auth_start");
	plugin->lib.auth_continue_v4 = (FUNC_auth_plugin_auth_continue_v4)LIB_SYM(lib, "mosquitto_auth_continue");

	ಯದಿ(plugin->lib.auth_start_v4){
		ಯದಿ(plugin->lib.auth_continue_v4){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO,
					" └── Extended authentication enabled.");
		}ಅನ್ಯಥಾ{
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR,
					"Error: Plugin has missing mosquitto_auth_continue() function.");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
		}
	}ಅನ್ಯಥಾ{
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO,
				" └── Extended authentication not enabled.");
	}

	plugin->lib.lib = lib;
	plugin->lib.user_data = ರಿಕ್ತ;
	plugin->lib.identifier = plugin;

	ಯದಿ(plugin->lib.plugin_init_v4){
		rc = plugin->lib.plugin_init_v4(&plugin->lib.user_data, plugin->config.options, plugin->config.option_ಎಣಿಕೆ);
		ಯದಿ(rc){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	mosquitto_callback_register(plugin, MOSQ_EVT_RELOAD, plugin_v4_reload, ರಿಕ್ತ, plugin);

	ಯದಿ(plugin->lib.unpwd_check_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v4_basic_auth, ರಿಕ್ತ, plugin);
	}
	ಯದಿ(plugin->lib.acl_check_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_ACL_CHECK, plugin_v4_acl_check, ರಿಕ್ತ, plugin);
	}
	ಯದಿ(plugin->lib.auth_start_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_EXT_AUTH_START, plugin_v4_auth_start, ರಿಕ್ತ, plugin);
	}
	ಯದಿ(plugin->lib.auth_continue_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_EXT_AUTH_CONTINUE, plugin_v4_auth_continue, ರಿಕ್ತ, plugin);
	}
	ಯದಿ(plugin->lib.psk_ಕೀಲಿ_get_v4){
		mosquitto_callback_register(plugin, MOSQ_EVT_PSK_ಕೀಲಿ, plugin_v4_psk_ಕೀಲಿ_get, ರಿಕ್ತ, plugin);
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}
