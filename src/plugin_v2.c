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

#समावेश "config.h"

#समावेश <stdio.h>
#समावेश <string.h>

#समावेश "mosquitto/broker.h"
#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/broker_plugin.h"
#समावेश "lib_load.h"
#समावेश "utlist.h"

प्रकार पूर्णांक (*FUNC_auth_plugin_version)(व्योम);
प्रकार पूर्णांक (*FUNC_plugin_version)(पूर्णांक, स्थिर पूर्णांक *);


अटल पूर्णांक plugin_v2_basic_auth(पूर्णांक event, व्योम *event_तथ्य, व्योम *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	काष्ठा mosquitto_evt_basic_auth *ed = event_तथ्य;

	UNUSED(event);

	अगर(plugin->lib.unpwd_check_v2 == शून्य){
		वापस MOSQ_ERR_INVAL;
	}

	वापस plugin->lib.unpwd_check_v2(
			plugin->lib.user_तथ्य,
			ed->username,
			ed->password);
}


अटल पूर्णांक plugin_v2_acl_check(पूर्णांक event, व्योम *event_तथ्य, व्योम *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	काष्ठा mosquitto_evt_acl_check *ed = event_तथ्य;
	पूर्णांक rc;

	UNUSED(event);

	अगर(plugin->lib.acl_check_v2 == शून्य){
		वापस MOSQ_ERR_INVAL;
	}

	rc = acl__pre_check(plugin, ed->client, ed->access);
	अगर(rc == MOSQ_ERR_PLUGIN_DEFER){
		वापस plugin->lib.acl_check_v2(
				plugin->lib.user_तथ्य,
				ed->client->id,
				ed->client->username,
				ed->topic,
				ed->access);
	}अन्यथा{
		वापस rc;
	}
}


अटल पूर्णांक plugin_v2_psk_कुंजी_get(पूर्णांक event, व्योम *event_तथ्य, व्योम *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	काष्ठा mosquitto_evt_psk_कुंजी *ed = event_तथ्य;

	UNUSED(event);

	अगर(plugin->lib.psk_कुंजी_get_v2 == शून्य){
		वापस MOSQ_ERR_INVAL;
	}

	वापस plugin->lib.psk_कुंजी_get_v2(
			plugin->lib.user_तथ्य,
			ed->hint,
			ed->identity,
			ed->कुंजी,
			ed->max_कुंजी_len);
}


अटल पूर्णांक plugin_v2_reload(पूर्णांक event, व्योम *event_तथ्य, व्योम *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	पूर्णांक rc;

	UNUSED(event);
	UNUSED(event_तथ्य);

	rc = plugin->lib.security_cleanup_v2(
			plugin->lib.user_तथ्य,
			(काष्ठा mosquitto_auth_opt *)plugin->config.options,
			plugin->config.option_गिनती,
			true);
	अगर(rc){
		वापस rc;
	}

	rc = plugin->lib.security_init_v2(
			plugin->lib.user_तथ्य,
			(काष्ठा mosquitto_auth_opt *)plugin->config.options,
			plugin->config.option_गिनती,
			true);
	वापस rc;
}


पूर्णांक plugin__load_v2(mosquitto_plugin_id_t *plugin, व्योम *lib)
{
	पूर्णांक rc;

	अगर(!(plugin->lib.plugin_init_v2 = (FUNC_auth_plugin_init_v2)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(शून्य, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		वापस MOSQ_ERR_UNKNOWN;
	}
	अगर(!(plugin->lib.plugin_cleanup_v2 = (FUNC_auth_plugin_cleanup_v2)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(शून्य, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		वापस MOSQ_ERR_UNKNOWN;
	}

	अगर(!(plugin->lib.security_init_v2 = (FUNC_auth_plugin_security_init_v2)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(शून्य, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		वापस MOSQ_ERR_UNKNOWN;
	}

	अगर(!(plugin->lib.security_cleanup_v2 = (FUNC_auth_plugin_security_cleanup_v2)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(शून्य, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		वापस MOSQ_ERR_UNKNOWN;
	}

	अगर(!(plugin->lib.acl_check_v2 = (FUNC_auth_plugin_acl_check_v2)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(शून्य, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		वापस MOSQ_ERR_UNKNOWN;
	}

	अगर(!(plugin->lib.unpwd_check_v2 = (FUNC_auth_plugin_unpwd_check_v2)LIB_SYM(lib, "mosquitto_auth_unpwd_check"))){
		log__printf(शून्य, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_unpwd_check().");
		LIB_ERROR();
		वापस MOSQ_ERR_UNKNOWN;
	}

	अगर(!(plugin->lib.psk_कुंजी_get_v2 = (FUNC_auth_plugin_psk_कुंजी_get_v2)LIB_SYM(lib, "mosquitto_auth_psk_key_get"))){
		log__printf(शून्य, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_psk_key_get().");
		LIB_ERROR();
		वापस MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = lib;
	plugin->lib.user_तथ्य = शून्य;
	plugin->lib.identifier = plugin;

	अगर(plugin->lib.plugin_init_v2){
		rc = plugin->lib.plugin_init_v2(
				&plugin->lib.user_तथ्य,
				(काष्ठा mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_गिनती);
		अगर(rc){
			log__printf(शून्य, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			वापस rc;
		}
	}

	mosquitto_callback_register(plugin, MOSQ_EVT_RELOAD, plugin_v2_reload, शून्य, plugin);

	अगर(plugin->lib.unpwd_check_v2){
		mosquitto_callback_register(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v2_basic_auth, शून्य, plugin);
	}
	अगर(plugin->lib.acl_check_v2){
		mosquitto_callback_register(plugin, MOSQ_EVT_ACL_CHECK, plugin_v2_acl_check, शून्य, plugin);
	}
	अगर(plugin->lib.psk_कुंजी_get_v2){
		mosquitto_callback_register(plugin, MOSQ_EVT_PSK_कुंजी, plugin_v2_psk_कुंजी_get, शून्य, plugin);
	}

	वापस 0;
}
