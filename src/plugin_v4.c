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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/broker_plugin.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "lib_load.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utlist.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_auth_plugin_ਸੰਸਕਰਣ)(ਖਾਲੀ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*FUNC_plugin_ਸੰਸਕਰਣ)(ਪੂਰਨ, ਅਚਲ ਪੂਰਨ *);


ਸਥਿਰ ਪੂਰਨ plugin_v4_basic_auth(ਪੂਰਨ event, ਖਾਲੀ *event_ਅੰਕੜੇ, ਖਾਲੀ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ਬਣਤਰ mosquitto_evt_basic_auth *ed = event_ਅੰਕੜੇ;

	ਅਣਵਰਤਿਆ(event);

	ਜੇ(plugin->lib.unpwd_check_v4 == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_PLUGIN_DEFER;
	}

	ਵਾਪਸ plugin->lib.unpwd_check_v4(
			plugin->lib.user_ਅੰਕੜੇ,
			ed->client,
			ed->username,
			ed->password);
}


ਸਥਿਰ ਪੂਰਨ plugin_v4_acl_check(ਪੂਰਨ event, ਖਾਲੀ *event_ਅੰਕੜੇ, ਖਾਲੀ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ਬਣਤਰ mosquitto_evt_acl_check *ed = event_ਅੰਕੜੇ;
	ਬਣਤਰ mosquitto_acl_ਸੁਨੇਹਾ ਸੁਨੇਹਾ;
	ਪੂਰਨ rc;

	ਅਣਵਰਤਿਆ(event);

	ਜੇ(plugin->lib.acl_check_v4 == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_PLUGIN_DEFER;
	}

	memset(&ਸੁਨੇਹਾ, 0, ਆਕਾਰ(ਸੁਨੇਹਾ));
	ਸੁਨੇਹਾ.topic = ed->topic;
	ਸੁਨੇਹਾ.payloadlen = ed->payloadlen;
	ਸੁਨੇਹਾ.payload = ed->payload;
	ਸੁਨੇਹਾ.qos = ed->qos;
	ਸੁਨੇਹਾ.retain = ed->retain;

	rc = acl__pre_check(plugin, ed->client, ed->access);
	ਜੇ(rc == MOSQ_ERR_PLUGIN_DEFER){
		ਵਾਪਸ plugin->lib.acl_check_v4(
				plugin->lib.user_ਅੰਕੜੇ,
				ed->access,
				ed->client,
				&ਸੁਨੇਹਾ);
	}ਨਹੀਂਤਾਂ{
		ਵਾਪਸ rc;
	}
}


ਸਥਿਰ ਪੂਰਨ plugin_v4_auth_ਸ਼ੁਰੂ(ਪੂਰਨ event, ਖਾਲੀ *event_ਅੰਕੜੇ, ਖਾਲੀ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ਬਣਤਰ mosquitto_evt_extended_auth *ed = event_ਅੰਕੜੇ;

	ਅਣਵਰਤਿਆ(event);

	ਜੇ(plugin->lib.auth_ਸ਼ੁਰੂ_v4 == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_PLUGIN_DEFER;
	}

	ਵਾਪਸ plugin->lib.auth_ਸ਼ੁਰੂ_v4(
			plugin->lib.user_ਅੰਕੜੇ,
			ed->client,
			ed->client->auth_ਵਿਧੀ,
			false,
			ed->ਅੰਕੜੇ_ਵਿੱਚ, ed->ਅੰਕੜੇ_ਵਿੱਚ_len,
			&ed->ਅੰਕੜੇ_ਬਾਹਰਲਾ, &ed->ਅੰਕੜੇ_ਬਾਹਰਲਾ_len);
}


ਸਥਿਰ ਪੂਰਨ plugin_v4_auth_continue(ਪੂਰਨ event, ਖਾਲੀ *event_ਅੰਕੜੇ, ਖਾਲੀ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ਬਣਤਰ mosquitto_evt_extended_auth *ed = event_ਅੰਕੜੇ;

	ਅਣਵਰਤਿਆ(event);

	ਜੇ(plugin->lib.auth_continue_v4 == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_PLUGIN_DEFER;
	}

	ਵਾਪਸ plugin->lib.auth_continue_v4(
			plugin->lib.user_ਅੰਕੜੇ,
			ed->client,
			ed->client->auth_ਵਿਧੀ,
			ed->ਅੰਕੜੇ_ਵਿੱਚ, ed->ਅੰਕੜੇ_ਵਿੱਚ_len,
			&ed->ਅੰਕੜੇ_ਬਾਹਰਲਾ, &ed->ਅੰਕੜੇ_ਬਾਹਰਲਾ_len);
}


ਸਥਿਰ ਪੂਰਨ plugin_v4_psk_ਕੁੰਜੀ_get(ਪੂਰਨ event, ਖਾਲੀ *event_ਅੰਕੜੇ, ਖਾਲੀ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ਬਣਤਰ mosquitto_evt_psk_ਕੁੰਜੀ *ed = event_ਅੰਕੜੇ;

	ਅਣਵਰਤਿਆ(event);

	ਜੇ(plugin->lib.psk_ਕੁੰਜੀ_get_v4 == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_PLUGIN_DEFER;
	}

	ਵਾਪਸ plugin->lib.psk_ਕੁੰਜੀ_get_v4(
			plugin->lib.user_ਅੰਕੜੇ,
			ed->client,
			ed->hint,
			ed->ਪਛਾਣ_ਤੱਤ,
			ed->ਕੁੰਜੀ,
			ed->max_ਕੁੰਜੀ_len);
}


ਸਥਿਰ ਪੂਰਨ plugin_v4_reload(ਪੂਰਨ event, ਖਾਲੀ *event_ਅੰਕੜੇ, ਖਾਲੀ *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	ਪੂਰਨ rc;

	ਅਣਵਰਤਿਆ(event);
	ਅਣਵਰਤਿਆ(event_ਅੰਕੜੇ);

	rc = plugin->lib.security_cleanup_v4(
			plugin->lib.user_ਅੰਕੜੇ,
			plugin->config.options,
			plugin->config.option_ਗਿਣਤੀ,
			true);
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	rc = plugin->lib.security_init_v4(
			plugin->lib.user_ਅੰਕੜੇ,
			plugin->config.options,
			plugin->config.option_ਗਿਣਤੀ,
			true);
	ਵਾਪਸ rc;
}


ਪੂਰਨ plugin_ਲੱਦੋ_v4(mosquitto_plugin_id_t *plugin, ਖਾਲੀ *lib)
{
	ਪੂਰਨ rc;

	ਜੇ(!(plugin->lib.plugin_init_v4 = (FUNC_auth_plugin_init_v4)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}
	ਜੇ(!(plugin->lib.plugin_cleanup_v4 = (FUNC_auth_plugin_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}

	ਜੇ(!(plugin->lib.security_init_v4 = (FUNC_auth_plugin_security_init_v4)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}

	ਜੇ(!(plugin->lib.security_cleanup_v4 = (FUNC_auth_plugin_security_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}

	ਜੇ(!(plugin->lib.acl_check_v4 = (FUNC_auth_plugin_acl_check_v4)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.unpwd_check_v4 = (FUNC_auth_plugin_unpwd_check_v4)LIB_SYM(lib, "mosquitto_auth_unpwd_check");
	ਜੇ(plugin->lib.unpwd_check_v4){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO,
				" ├── Username/password checking enabled.");
	}ਨਹੀਂਤਾਂ{
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO,
				" ├── Username/password checking not enabled.");
	}

	plugin->lib.psk_ਕੁੰਜੀ_get_v4 = (FUNC_auth_plugin_psk_ਕੁੰਜੀ_get_v4)LIB_SYM(lib, "mosquitto_auth_psk_key_get");
	ਜੇ(plugin->lib.psk_ਕੁੰਜੀ_get_v4){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking enabled.");
	}ਨਹੀਂਤਾਂ{
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking not enabled.");
	}

	plugin->lib.auth_ਸ਼ੁਰੂ_v4 = (FUNC_auth_plugin_auth_ਸ਼ੁਰੂ_v4)LIB_SYM(lib, "mosquitto_auth_start");
	plugin->lib.auth_continue_v4 = (FUNC_auth_plugin_auth_continue_v4)LIB_SYM(lib, "mosquitto_auth_continue");

	ਜੇ(plugin->lib.auth_ਸ਼ੁਰੂ_v4){
		ਜੇ(plugin->lib.auth_continue_v4){
			log__printf(ਨਿਰਾ, MOSQ_LOG_INFO,
					" └── Extended authentication enabled.");
		}ਨਹੀਂਤਾਂ{
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR,
					"Error: Plugin has missing mosquitto_auth_continue() function.");
			ਵਾਪਸ MOSQ_ERR_UNKNOWN;
		}
	}ਨਹੀਂਤਾਂ{
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO,
				" └── Extended authentication not enabled.");
	}

	plugin->lib.lib = lib;
	plugin->lib.user_ਅੰਕੜੇ = ਨਿਰਾ;
	plugin->lib.identifier = plugin;

	ਜੇ(plugin->lib.plugin_init_v4){
		rc = plugin->lib.plugin_init_v4(&plugin->lib.user_ਅੰਕੜੇ, plugin->config.options, plugin->config.option_ਗਿਣਤੀ);
		ਜੇ(rc){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			ਵਾਪਸ rc;
		}
	}

	mosquitto_callback_ਰਜਿਸਟਰ(plugin, MOSQ_EVT_RELOAD, plugin_v4_reload, ਨਿਰਾ, plugin);

	ਜੇ(plugin->lib.unpwd_check_v4){
		mosquitto_callback_ਰਜਿਸਟਰ(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v4_basic_auth, ਨਿਰਾ, plugin);
	}
	ਜੇ(plugin->lib.acl_check_v4){
		mosquitto_callback_ਰਜਿਸਟਰ(plugin, MOSQ_EVT_ACL_CHECK, plugin_v4_acl_check, ਨਿਰਾ, plugin);
	}
	ਜੇ(plugin->lib.auth_ਸ਼ੁਰੂ_v4){
		mosquitto_callback_ਰਜਿਸਟਰ(plugin, MOSQ_EVT_EXT_AUTH_ਸ਼ੁਰੂ, plugin_v4_auth_ਸ਼ੁਰੂ, ਨਿਰਾ, plugin);
	}
	ਜੇ(plugin->lib.auth_continue_v4){
		mosquitto_callback_ਰਜਿਸਟਰ(plugin, MOSQ_EVT_EXT_AUTH_CONTINUE, plugin_v4_auth_continue, ਨਿਰਾ, plugin);
	}
	ਜੇ(plugin->lib.psk_ਕੁੰਜੀ_get_v4){
		mosquitto_callback_ਰਜਿਸਟਰ(plugin, MOSQ_EVT_PSK_ਕੁੰਜੀ, plugin_v4_psk_ਕੁੰਜੀ_get, ਨਿਰਾ, plugin);
	}

	ਵਾਪਸ 0;
}
